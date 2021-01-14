// �s�N�Z���V�F�[�_

// �O���[�o��
cbuffer global : register(b1) {
	float4	g_vEye;			// ���_���W
	// ����
	float4	g_vLightDir;	// ��������
	float4	g_vLa;			// ����
	float4	g_vLd;			// �g�U���ˌ�
	float4	g_vLs;			// ���ʔ��ˌ�
        // �}�e���A��
    float4 g_vKa; // �A���r�G���g�F(+�e�N�X�`���L��)
    float4 g_vKd; // �f�B�t���[�Y�F
    float4 g_vKs; // �X�y�L�����F(+�X�y�L�������x)
    float4 g_vKe; // �G�~�b�V�u�F
    
    bool g_bBump;
};

// �p�����[�^
struct VS_OUTPUT {
	float4	Position	: SV_Position;
	float4	lightPosition	: WORLD_POSITION;
	float3	Pos4PS		: TEXCOORD0;
    float3 Normal : TEXCOORD1;
	float2	TexCoord	: TEXCOORD2;
    float3 Target : TEXCOORD3;
    float3 BinNormal : TEXCOORD4;
	float4	Diffuse		: COLOR0;
    float  Fog     : COLOR1;
    
   // uint    instID      : TEXCOORD5;
};

Texture2D    g_texture : register(t0);	// �e�N�X�`��
SamplerState g_sampler : register(s0);	// �T���v��

Texture2D<float>    g_shadowTexture : register(t1);	// �V���h�E�e�N�X�`��
SamplerComparisonState g_shadowSampler : register(s1);	// �V���h�E�T���v��

Texture2D g_bumpMaptexture : register(t2); // �e�N�X�`��

// �����ɂ͐��K�����ꂽ���˃x�N�g������
float2 envMapEquirect(float3 dir)
{
    float PI = 3.14159265359f;
    float phi = acos(-dir.y);
    float theta = atan2(-1.0f * dir.x, dir.z) + PI;
    return float2(theta / (PI * 2.0f), phi / PI);
}

float4 main(VS_OUTPUT input) : SV_Target0
{
    float3 Diff = input.Diffuse.rgb * g_vKd.rgb;
    float Alpha = input.Diffuse.a * g_vKd.a;
    float4 vTd = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (g_vKa.a > 0.0f)
    {
		// �e�N�X�`���L
        vTd = g_texture.Sample(g_sampler, input.TexCoord);
        Diff *= vTd.rgb;
        Alpha *= vTd.a;
    }
	//clip(Alpha - 0.0001f);
    if (Alpha <= 0.0f)
        discard;

    if (g_vLightDir.x != 0.0f || g_vLightDir.y != 0.0f || g_vLightDir.z != 0.0f)
    {
		
		//===== �f�t�H���g�V�F�[�f�B���O =====
		// �����L��
        float3 L = normalize(-g_vLightDir.xyz); // �����ւ̃x�N�g��
        float3 V = normalize(g_vEye.xyz - input.Pos4PS); // ���_�ւ̃x�N�g��
        float3 N = normalize(input.Normal); // �@���x�N�g��
        float3 H = normalize(L + V); // �n�[�t�x�N�g��
		
		
		//===== �V���h�E�}�b�v =====
		
   //     // �V���h�E�}�b�v�̐[�x�l�Ɣ�r����.
   //     float3 shadowCoord = input.lightPosition.xyz / input.lightPosition.w;

   //     // �ő�[�x�X�΂����߂�.
   //     float maxDepthSlope = max(abs(ddx(shadowCoord.z)), abs(ddy(shadowCoord.z)));
   //     // �V���h�E�}�b�v�J���[
   //     float3 sc = float3(0.25f, 0.25f, 0.25f);
        
   //     // �V���h�E�}�b�v����
   //     if ((saturate(shadowCoord.x) == shadowCoord.x) &&
			//(saturate(shadowCoord.y) == shadowCoord.y) &&
			//(maxDepthSlope > 0))
   //     {
   //         float shadowThreshold = 1.0f; // �V���h�E�ɂ��邩�ǂ�����臒l�ł�.
   //         float bias = 0.01f; // �Œ�o�C�A�X�ł�.
   //         float slopeScaledBias = 0.01f; // �[�x�X��.
   //         float depthBiasClamp = 0.1f; // �o�C�A�X�N�����v�l.

   //         float shadowBias = bias + slopeScaledBias * maxDepthSlope;
   //         shadowBias = min(shadowBias, depthBiasClamp);

   //         float3 shadowColor = float3(0.25f, 0.25f, 0.25f);
   //         shadowThreshold = g_shadowTexture.SampleCmpLevelZero(g_shadowSampler, shadowCoord.xy, shadowCoord.z - shadowBias);
   //         shadowColor = lerp(shadowColor, float3(1.0f, 1.0f, 1.0f), shadowThreshold);
   //         sc = shadowColor;

   //     }
        
			// �ǉ�
        float3 lightPos = input.lightPosition.xyz / input.lightPosition.w;
        float2 shadowTexCoords = lightPos.xy;
       // shadowTexCoords = (lightPos.xy + float2(1, -1)) * float2(0.5, -0.5);
			//shadowTexCoords.x = 0.5f + (input.lightPosition.x / input.lightPosition.w * 0.5f);
			//shadowTexCoords.y = 0.5f - (input.lightPosition.y / input.lightPosition.w * 0.5f);
        float pixelDepth = input.lightPosition.z / input.lightPosition.w;

        float lighting = 1;
        float3 sc = float3(1.0f, 1.0f, 1.0f);

//        if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) &&
//			(saturate(shadowTexCoords.y) == shadowTexCoords.y) &&
//			(pixelDepth > 0))
//        {
//            float margin = acos(saturate(dot(N, L)));
//#ifdef LINEAR
//				float epsilon = 0.0005 / margin;
//#else
//            float epsilon = 0.005 / margin;
//#endif
//            epsilon = clamp(epsilon, 0, 0.1);

//            lighting = float(g_shadowTexture.SampleCmpLevelZero(
//				g_shadowSampler,
//				shadowTexCoords,
//				pixelDepth - epsilon));
//            sc = float3(0.5f, 0.5f, 0.5f);
//            sc = lerp(sc, float3(1.0f, 1.0f, 1.0f), lighting);

//        }
        
        
        // ===== �o���v�}�b�v =====
        if (g_bBump)
        {
			// �@���A�ڐ��A�]�@��
            //float3 n = normalize(input.Normal); // �@���x�N�g��
            //float3 t = normalize(cross(n, float3(0.0f, 1.0f, 0.0f))); // �ڐ�
            //float3 b = normalize((n, t)); // �]�@��
            float3 n = normalize(input.Normal);
            float3 t = normalize(input.Target);
            float3 b = normalize(input.BinNormal);
			
			//// ����
   //         float3 v;
   //         v.x = dot(t, V);
   //         v.y = dot(b, V);
   //         v.z = dot(n, V);
		
			//// ���C�g
   //         float3 l;
   //         l.x = dot(t, L);
   //         l.y = dot(b, L);
   //         l.z = dot(n, L);

			//// �n�[�t
   //         float3 h = normalize(l + v); // �n�[�t�x�N�g��

			// �m�[�}���}�b�v
            float3 nm = (g_bumpMaptexture.Sample(g_sampler, input.TexCoord) * 2.0f - 1.0f).rgb;
			
            nm = (nm.x * t) + (nm.y * b) + (nm.z * n);
            nm = normalize(nm);
            N = nm;
			
    //        Diff = g_vLa.rgb * g_vKa.rgb + g_vLd.rgb *
				//Diff * saturate(dot(l, nm)); // �g�U�F + ���F
    //        float3 Spec = g_vLs.rgb * g_vKs.rgb *
				//pow(saturate(dot(nm, h)), g_vKs.a); // ���ʔ��ːF
    //        Diff += Spec;
    //        Diff += g_vKe.rgb; // �G�~�b�V����
            
    //        return float4(Diff, Alpha);
          //  discard;
            
        }
    //    else
       // Diff *= sc;
        
        //===== �p�m���}���}�b�v =====
        float3 r = 2.0f * N * dot(N, V) - V;
        //Diff = g_texture.Sample(g_sampler, -envMapEquirect(r));
        
        // Half Lambert
        float PI = 3.14159265359f;
        float val = max(dot(N, L), 0.0f) * 0.5f + 0.5f;
        val = val * val; // * (3.0f / (4.0f * PI));
        
        // �n�[�t�����o�[�g
        Diff = g_vLd.rgb * Diff * val * sc * 2; // �g�U�F
        //Diff = g_vLd.rgb * Diff * val * sc * N * 2; // �g�U�F
       // Diff = g_vLd.rgb * Diff * val * sc + N; // �g�U�F
        
        // �����o�[�g
        //Diff = g_vLd.rgb * Diff * saturate(dot(L, N)) * sc; // �g�U�F
        //Diff = g_vLd.rgb * Diff * N; // �g�U�F
        
        
        Diff += g_vLa.rgb * g_vKa.rgb * vTd.rgb * sc; // ����
        float3 Spec = g_vLs.rgb * g_vKs.rgb *
			pow(saturate(dot(N, H)), g_vKs.a) * sc * vTd.rgb; // ���ʔ��ːF
        Diff += Spec;
        Diff += g_vKe.rgb * vTd.rgb; // �G�~�b�V����
        
        //�@�t�H�O�F�ƃI�u�W�F�N�g�F�Ɛ��`����
        //float3 FogColor = float3(1, 1, 1);
        //float3 FogColor = float3(0.557f, 0.631f, 0.6f);
        float3 FogColor = float3(0.0f, 0.51f, 0.51f);
        Diff = lerp(FogColor, Diff, input.Fog);
    }


    return float4(Diff, Alpha);
}
