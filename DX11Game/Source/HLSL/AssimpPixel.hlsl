// Assimp用ピクセルシェーダ (AssimpPixel.hlsl)

// グローバル
cbuffer global : register(b0) {
	matrix	g_mtxWVP;			// ワールド×ビュー×射影行列
	matrix	g_mtxLitWVP;		// ライトのワールド×ビュー×射影行列
	matrix	g_mtxWorld;			// ワールド行列
	matrix	g_mtxTexture;		// テクスチャ行列
	float4	g_vCameraPos;		// 視点座標(ワールド空間)
	float4	g_vLightDir;		// 光源方向(ワールド空間)
	float4	g_vLightAmbient;	// 環境光
	float4	g_vLightDiffuse;	// 拡散光
	float4	g_vLightSpecular;	// 鏡面反射光
};

// マテリアル
cbuffer global2 : register(b1) {
	float4	g_Ambient;			// 環境色
	float4	g_Diffuse;			// 拡散色+アルファ
	float4	g_Specular;			// 鏡面反射色+強度
	float4	g_Emissive;			// 発光色
	float4	g_Flags;			// 拡散色テクスチャ有無, 発光色テクスチャ有無, 透過テクスチャ有無
};

Texture2D    g_texture			: register(t0);	// テクスチャ
Texture2D    g_texEmissive		: register(t1);	// 発光テクスチャ
Texture2D    g_texTransparent	: register(t2);	// 透過テクスチャ
Texture2D    g_texSpecular		: register(t3);	// 鏡面反射テクスチャ
SamplerState g_sampler			: register(s0);	// サンプラ


Texture2D<float>    g_shadowTexture : register(t4);	// シャドウテクスチャ
SamplerComparisonState g_shadowSampler : register(s1);	// シャドウサンプラ


// パラメータ
struct VS_OUTPUT {
	float4	Pos			: SV_Position;
	float4	LitPos		: WORLD_POSITION;
	float2	Tex			: TEXCOORD0;
	float3	Normal		: TEXCOORD1;
	float3	PosForPS	: TEXCOORD2;
};


// 引数には正規化された反射ベクトルを代入
float2 envMapEquirect(float3 dir)
{
    float PI = 3.14159265359f;
    float phi = acos(-dir.y);
    float theta = atan2(-1.0f * dir.x, dir.z) + PI;
    return float2(theta / (PI * 2.0f), phi / PI);
}

//
// ピクセルシェーダ
//
float4 main(VS_OUTPUT input) : SV_Target0
{
	float3 Diff = g_Diffuse.rgb;
	float Alpha = g_Diffuse.a;
	if (g_Flags.x != 0.0f) {		// テクスチャ有無
		float4 TexDiff = g_texture.Sample(g_sampler, input.Tex);
		Diff *= TexDiff.rgb;
		Alpha *= TexDiff.a;
	}
	if (g_Flags.z != 0.0f) {		// テクスチャ有無
		float4 TexTran = g_texTransparent.Sample(g_sampler, input.Tex);
		Alpha *= (TexTran.r * TexTran.g * TexTran.b * TexTran.a);
	}
	if (Alpha <= 0.0f) discard;		// 完全透明なら描画しない
	float3 Spec = g_Specular.rgb;
	if (g_Flags.w != 0.0f) {		// テクスチャ有無
		float4 TexSpec = g_texSpecular.Sample(g_sampler, input.Tex);
		Spec *= TexSpec.rgb;
	}
    
	float3 L = normalize(-g_vLightDir.xyz);					// 光源へのベクトル
	float3 N = normalize(input.Normal);						// 法線ベクトル
    
    // 追加
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

		float3 V = normalize(g_vCameraPos.xyz - input.PosForPS);// 視点へのベクトル
		float3 H = normalize(L + V);							// ハーフベクトル
        
        // 環境マップ
        float3 r = 2.0f * N * dot(N, V) - V;
      //  Diff = g_texture.Sample(g_sampler, -envMapEquirect(r));
        Diff *= sc;
        
		// Half Lambert
		float PI = 3.14159265359f;
        float val = max(dot(N, L), 0.0f) * 0.5f + 0.5f;
        val = val * val; // * (3.0f / (4.0f * PI));
		
		// ハーフランバート
        Diff = g_vLightAmbient.rgb * g_Ambient.rgb +
			g_vLightDiffuse.rgb * Diff * val; // 拡散色 + 環境色
		
        // ライティング
   //     Diff = g_vLightAmbient.rgb * g_Ambient.rgb +
			//g_vLightDiffuse.rgb * Diff * saturate(dot(L, N)); // 拡散色 + 環境色
		
		
        Spec = Spec * g_vLightSpecular.rgb *
			pow(saturate(dot(N, H)), g_Specular.a); // 鏡面反射色
        Diff += Spec;
    }

	float3 Emis = g_Emissive.rgb;
	if (g_Flags.y != 0.0f) {		// テクスチャ有無
		float4 TexEmis = g_texEmissive.Sample(g_sampler, input.Tex);
		Emis *= (TexEmis.rgb * TexEmis.a);
	}
	Diff += Emis;

	return float4(Diff, Alpha);
}
