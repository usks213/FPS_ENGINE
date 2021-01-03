// Assimp�p�s�N�Z���V�F�[�_ (AssimpPixel.hlsl)

// �O���[�o��
cbuffer global : register(b0) {
	matrix	g_mtxWVP;			// ���[���h�~�r���[�~�ˉe�s��
	matrix	g_mtxLitWVP;		// ���C�g�̃��[���h�~�r���[�~�ˉe�s��
	matrix	g_mtxWorld;			// ���[���h�s��
	matrix	g_mtxTexture;		// �e�N�X�`���s��
	float4	g_vCameraPos;		// ���_���W(���[���h���)
	float4	g_vLightDir;		// ��������(���[���h���)
	float4	g_vLightAmbient;	// ����
	float4	g_vLightDiffuse;	// �g�U��
	float4	g_vLightSpecular;	// ���ʔ��ˌ�
};

// �}�e���A��
cbuffer global2 : register(b1) {
	float4	g_Ambient;			// ���F
	float4	g_Diffuse;			// �g�U�F+�A���t�@
	float4	g_Specular;			// ���ʔ��ːF+���x
	float4	g_Emissive;			// �����F
	float4	g_Flags;			// �g�U�F�e�N�X�`���L��, �����F�e�N�X�`���L��, ���߃e�N�X�`���L��
};

Texture2D    g_texture			: register(t0);	// �e�N�X�`��
Texture2D    g_texEmissive		: register(t1);	// �����e�N�X�`��
Texture2D    g_texTransparent	: register(t2);	// ���߃e�N�X�`��
Texture2D    g_texSpecular		: register(t3);	// ���ʔ��˃e�N�X�`��
SamplerState g_sampler			: register(s0);	// �T���v��


Texture2D<float>    g_shadowTexture : register(t4);	// �V���h�E�e�N�X�`��
SamplerComparisonState g_shadowSampler : register(s1);	// �V���h�E�T���v��


// �p�����[�^
struct VS_OUTPUT {
	float4	Pos			: SV_Position;
	float4	LitPos		: WORLD_POSITION;
	float2	Tex			: TEXCOORD0;
	float3	Normal		: TEXCOORD1;
	float3	PosForPS	: TEXCOORD2;
};


// �����ɂ͐��K�����ꂽ���˃x�N�g������
float2 envMapEquirect(float3 dir)
{
    float PI = 3.14159265359f;
    float phi = acos(-dir.y);
    float theta = atan2(-1.0f * dir.x, dir.z) + PI;
    return float2(theta / (PI * 2.0f), phi / PI);
}

//
// �s�N�Z���V�F�[�_
//
float4 main(VS_OUTPUT input) : SV_Target0
{
	float3 Diff = g_Diffuse.rgb;
	float Alpha = g_Diffuse.a;
	if (g_Flags.x != 0.0f) {		// �e�N�X�`���L��
		float4 TexDiff = g_texture.Sample(g_sampler, input.Tex);
		Diff *= TexDiff.rgb;
		Alpha *= TexDiff.a;
	}
	if (g_Flags.z != 0.0f) {		// �e�N�X�`���L��
		float4 TexTran = g_texTransparent.Sample(g_sampler, input.Tex);
		Alpha *= (TexTran.r * TexTran.g * TexTran.b * TexTran.a);
	}
	if (Alpha <= 0.0f) discard;		// ���S�����Ȃ�`�悵�Ȃ�
	float3 Spec = g_Specular.rgb;
	if (g_Flags.w != 0.0f) {		// �e�N�X�`���L��
		float4 TexSpec = g_texSpecular.Sample(g_sampler, input.Tex);
		Spec *= TexSpec.rgb;
	}
    
	float3 L = normalize(-g_vLightDir.xyz);					// �����ւ̃x�N�g��
	float3 N = normalize(input.Normal);						// �@���x�N�g��
    
    // �ǉ�
    float3 lightPos = input.LitPos.xyz / input.LitPos.w;
    float2 shadowTexCoords;
    shadowTexCoords = (lightPos.xy + float2(1, -1)) * float2(0.5, -0.5);
	//shadowTexCoords.x = 0.5f + (input.LitPos.x / input.LitPos.w * 0.5f);
	//shadowTexCoords.y = 0.5f - (input.LitPos.y / input.LitPos.w * 0.5f);
    float pixelDepth = input.LitPos.z / input.LitPos.w;

    float lighting = 1;
    float3 sc = float3(1.0f, 1.0f, 1.0f);

    if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) &&
		(saturate(shadowTexCoords.y) == shadowTexCoords.y) &&
		(pixelDepth > 0))
    {
        float margin = acos(saturate(dot(N, L)));
#ifdef LINEAR
		float epsilon = 0.0005 / margin;
#else
        float epsilon = 0.0005 / margin;
#endif
        epsilon = clamp(epsilon, 0, 0.1);

        lighting = float(g_shadowTexture.SampleCmpLevelZero(
			g_shadowSampler,
			shadowTexCoords,
			pixelDepth - epsilon));
        sc = float3(0.25f, 0.25f, 0.25f);
        sc = lerp(sc, float3(1.0f, 1.0f, 1.0f), lighting);
        //Diff *= sc;
    }
    
	if (g_vLightDir.x != 0.0f || g_vLightDir.y != 0.0f || g_vLightDir.z != 0.0f) {

		float3 V = normalize(g_vCameraPos.xyz - input.PosForPS);// ���_�ւ̃x�N�g��
		float3 H = normalize(L + V);							// �n�[�t�x�N�g��
        
        // ���}�b�v
        float3 r = 2.0f * N * dot(N, V) - V;
      //  Diff = g_texture.Sample(g_sampler, -envMapEquirect(r));
        Diff *= sc;
        
		// Half Lambert
		float PI = 3.14159265359f;
        float val = max(dot(N, L), 0.0f) * 0.5f + 0.5f;
        val = val * val; // * (3.0f / (4.0f * PI));
		
		// �n�[�t�����o�[�g
        Diff = g_vLightAmbient.rgb * g_Ambient.rgb +
			g_vLightDiffuse.rgb * Diff * val; // �g�U�F + ���F
		
        // ���C�e�B���O
   //     Diff = g_vLightAmbient.rgb * g_Ambient.rgb +
			//g_vLightDiffuse.rgb * Diff * saturate(dot(L, N)); // �g�U�F + ���F
		
		
        Spec = Spec * g_vLightSpecular.rgb *
			pow(saturate(dot(N, H)), g_Specular.a); // ���ʔ��ːF
        Diff += Spec;
    }

	float3 Emis = g_Emissive.rgb;
	if (g_Flags.y != 0.0f) {		// �e�N�X�`���L��
		float4 TexEmis = g_texEmissive.Sample(g_sampler, input.Tex);
		Emis *= (TexEmis.rgb * TexEmis.a);
	}
	Diff += Emis;

	return float4(Diff, Alpha);
}
