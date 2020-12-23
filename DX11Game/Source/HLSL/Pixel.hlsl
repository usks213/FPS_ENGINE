// ピクセルシェーダ

// グローバル
cbuffer global : register(b1) {
	float4	g_vEye;			// 視点座標
	// 光源
	float4	g_vLightDir;	// 光源方向
	float4	g_vLa;			// 環境光
	float4	g_vLd;			// 拡散反射光
	float4	g_vLs;			// 鏡面反射光
	// マテリアル
	float4	g_vKa;			// アンビエント色(+テクスチャ有無)
	float4	g_vKd;			// ディフューズ色
	float4	g_vKs;			// スペキュラ色(+スペキュラ強度)
	float4	g_vKe;			// エミッシブ色
    
    bool g_bBump;
};

// パラメータ
struct VS_OUTPUT {
	float4	Position	: SV_Position;
	float4	lightPosition	: WORLD_POSITION;
	float3	Pos4PS		: TEXCOORD0;
	float3	Normal		: TEXCOORD1;
	float2	TexCoord	: TEXCOORD2;
    float3 Target : TEXCOORD3;
    float3 BinNormal : TEXCOORD4;
	float4	Diffuse		: COLOR0;
};

Texture2D    g_texture : register(t0);	// テクスチャ
SamplerState g_sampler : register(s0);	// サンプラ

Texture2D<float>    g_shadowTexture : register(t1);	// シャドウテクスチャ
SamplerComparisonState g_shadowSampler : register(s1);	// シャドウサンプラ

Texture2D g_bumpMaptexture : register(t2); // テクスチャ

// 引数には正規化された反射ベクトルを代入
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
		// テクスチャ有
        vTd = g_texture.Sample(g_sampler, input.TexCoord);
        Diff *= vTd.rgb;
        Alpha *= vTd.a;
    }
	//clip(Alpha - 0.0001f);
    if (Alpha <= 0.0f)
        discard;

    if (g_vLightDir.x != 0.0f || g_vLightDir.y != 0.0f || g_vLightDir.z != 0.0f)
    {
		
		//===== デフォルトシェーディング =====
		// 光源有効
        float3 L = normalize(-g_vLightDir.xyz); // 光源へのベクトル
        float3 V = normalize(g_vEye.xyz - input.Pos4PS); // 視点へのベクトル
        float3 N = normalize(input.Normal); // 法線ベクトル
        float3 H = normalize(L + V); // ハーフベクトル
		
		
		//===== シャドウマップ =====
		
			// 追加
        float3 lightPos = input.lightPosition.xyz / input.lightPosition.w;
        float2 shadowTexCoords;
        shadowTexCoords = (lightPos + float2(1, -1)) * float2(0.5, -0.5);
			//shadowTexCoords.x = 0.5f + (input.lightPosition.x / input.lightPosition.w * 0.5f);
			//shadowTexCoords.y = 0.5f - (input.lightPosition.y / input.lightPosition.w * 0.5f);
        float pixelDepth = input.lightPosition.z / input.lightPosition.w;

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
            float epsilon = 0.005 / margin;
#endif
            epsilon = clamp(epsilon, 0, 0.1);

            lighting = float(g_shadowTexture.SampleCmpLevelZero(
				g_shadowSampler,
				shadowTexCoords,
				pixelDepth - epsilon));
            sc = float3(0.5f, 0.5f, 0.5f);
            sc = lerp(sc, float3(1.0f, 1.0f, 1.0f), lighting);

        }
        
        
        // ===== バンプマップ =====
        if (g_bBump)
        {
			// 法線、接線、従法線
            //float3 n = normalize(input.Normal); // 法線ベクトル
            //float3 t = normalize(cross(n, float3(0.0f, 1.0f, 0.0f))); // 接線
            //float3 b = normalize((n, t)); // 従法線
            float3 n = normalize(input.Normal);
            float3 t = normalize(input.Target);
            float3 b = normalize(input.BinNormal);
			
			//// 視線
   //         float3 v;
   //         v.x = dot(t, V);
   //         v.y = dot(b, V);
   //         v.z = dot(n, V);
		
			//// ライト
   //         float3 l;
   //         l.x = dot(t, L);
   //         l.y = dot(b, L);
   //         l.z = dot(n, L);

			//// ハーフ
   //         float3 h = normalize(l + v); // ハーフベクトル

			// ノーマルマップ
            float3 nm = (g_bumpMaptexture.Sample(g_sampler, input.TexCoord) * 2.0f - 1.0f).rgb;
			
            nm = (nm.x * t) + (nm.y * b) + (nm.z * n);
            nm = normalize(nm);
            N = nm;
			
    //        Diff = g_vLa.rgb * g_vKa.rgb + g_vLd.rgb *
				//Diff * saturate(dot(l, nm)); // 拡散色 + 環境色
    //        float3 Spec = g_vLs.rgb * g_vKs.rgb *
				//pow(saturate(dot(nm, h)), g_vKs.a); // 鏡面反射色
    //        Diff += Spec;
    //        Diff += g_vKe.rgb; // エミッション
            
    //        return float4(Diff, Alpha);
          //  discard;
            
        }
    //    else
       // Diff *= sc;
        
        //===== パノラマ環境マップ =====
        float3 r = 2.0f * N * dot(N, V) - V;
        //Diff = g_texture.Sample(g_sampler, -envMapEquirect(r));
        
        // Half Lambert
        float PI = 3.14159265359f;
        float val = max(dot(N, L), 0.0f) * 0.5f + 0.5f;
        val = val * val; // * (3.0f / (4.0f * PI));
        
        // ハーフランバート
        Diff = g_vLd.rgb * Diff * val * sc; // 拡散色
        
        // ランバート
       // Diff = g_vLd.rgb * Diff * saturate(dot(L, N)) * sc; // 拡散色
        
        
        Diff += g_vLa.rgb * g_vKa.rgb * vTd.rgb * sc; // 環境光
        float3 Spec = g_vLs.rgb * g_vKs.rgb *
			pow(saturate(dot(N, H)), g_vKs.a) * sc * vTd.rgb; // 鏡面反射色
        Diff += Spec;
        Diff += g_vKe.rgb * vTd.rgb; // エミッション
        
    }


    return float4(Diff, Alpha);
}
