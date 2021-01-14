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
    
    bool g_bLight;
    bool g_bBump;
    bool g_bAnbient;
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
    float   Fog			: COLOR1;
};

Texture2D    g_texture : register(t0);	// テクスチャ
SamplerState g_sampler : register(s0);	// サンプラ

Texture2D<float>    g_shadowTexture : register(t1);	// シャドウテクスチャ
SamplerComparisonState g_shadowSampler : register(s1);	// シャドウサンプラ

Texture2D g_bumpMaptexture : register(t2);      // 法線マップ
Texture2D g_anbientMaptexture : register(t3);   // 環境マップ

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

    if (g_bLight && 
        (g_vLightDir.x != 0.0f || g_vLightDir.y != 0.0f || g_vLightDir.z != 0.0f))
    {
		
		//===== デフォルトシェーディング =====
		// 光源有効
        float3 L = normalize(-g_vLightDir.xyz); // 光源へのベクトル
        float3 V = normalize(g_vEye.xyz - input.Pos4PS); // 視点へのベクトル
        float3 N = normalize(input.Normal); // 法線ベクトル
        float3 H = normalize(L + V); // ハーフベクトル
		
		
		//===== シャドウマップ =====
		
   //     // シャドウマップの深度値と比較する.
   //     float3 shadowCoord = input.lightPosition.xyz / input.lightPosition.w;

   //     // 最大深度傾斜を求める.
   //     float maxDepthSlope = max(abs(ddx(shadowCoord.z)), abs(ddy(shadowCoord.z)));
   //     // シャドウマップカラー
   //     float3 sc = float3(0.25f, 0.25f, 0.25f);
        
   //     // シャドウマップ判定
   //     if ((saturate(shadowCoord.x) == shadowCoord.x) &&
			//(saturate(shadowCoord.y) == shadowCoord.y) &&
			//(maxDepthSlope > 0))
   //     {
   //         float shadowThreshold = 1.0f; // シャドウにするかどうかの閾値です.
   //         float bias = 0.01f; // 固定バイアスです.
   //         float slopeScaledBias = 0.01f; // 深度傾斜.
   //         float depthBiasClamp = 0.1f; // バイアスクランプ値.

   //         float shadowBias = bias + slopeScaledBias * maxDepthSlope;
   //         shadowBias = min(shadowBias, depthBiasClamp);

   //         float3 shadowColor = float3(0.25f, 0.25f, 0.25f);
   //         shadowThreshold = g_shadowTexture.SampleCmpLevelZero(g_shadowSampler, shadowCoord.xy, shadowCoord.z - shadowBias);
   //         shadowColor = lerp(shadowColor, float3(1.0f, 1.0f, 1.0f), shadowThreshold);
   //         sc = shadowColor;

   //     }
        
			// 追加
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
        
        //===== パノラマ環境マップ =====
        if (g_bAnbient)
        {
            float3 r = 2.0f * N * dot(N, V) - V;
            Diff = g_anbientMaptexture.Sample(g_sampler, -envMapEquirect(r));
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
        }
        
        
        //===== ライティング =====
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
        
        //　フォグ色とオブジェクト色と線形合成
        float3 FogColor = float3(0.557f, 0.631f, 0.6f);
       // float3 FogColor = float3(0.0f, 0.51f, 0.51f);
        Diff = lerp(FogColor, Diff, input.Fog);
        Alpha = lerp(1, Alpha, input.Fog);
    }


    return float4(Diff, Alpha);
}
