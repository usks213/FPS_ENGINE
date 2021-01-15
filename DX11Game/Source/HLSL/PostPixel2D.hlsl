// 2D用ピクセルシェーダ

// グローバル
cbuffer global : register(b1) {
    matrix g_mTexture;
    float4 g_color;
    int SampleCount;
    float4 Offset[16];
};

// パラメータ
struct VS_OUTPUT {
	float4	Position	: SV_Position;
	float2	TexCoord	: TEXCOORD0;
	float4	Diffuse		: COLOR0;
};

Texture2D    g_texture : register(t0);	// テクスチャ
SamplerState g_sampler : register(s0);	// サンプラ

Texture2D<float> g_depthTexture : register(t1);			// 深度ステンシルテクスチャ
SamplerComparisonState g_depthSampler : register(s1);	// 深度ステンシルサンプラ

float4 main(VS_OUTPUT input) : SV_Target0
{
	float4 Color = input.Diffuse;
	if (g_mTexture._44 > 0.0f) {
		Color *= g_texture.Sample(g_sampler, input.TexCoord);
        float w = Color.w;
		
		//--- ポストエフェクト
		
		//// ガウスブラー
  //      for (int i = 0; i < SampleCount; ++i)
  //      {
  //          Color += Offset[i].z * g_texture.Sample(g_sampler, input.TexCoord + Offset[i].xy);
  //      }
  //      Color.w = w;
		
		// 色収差
        float size = 0.013f;
        float2 uvBase = input.TexCoord - 0.5f;
		// R値
        float2 uvR = uvBase * (1.0f - size * 2.0f) + 0.5f;
        Color.r = g_texture.Sample(g_sampler, uvR).r;
		// G値
        float2 uvG = uvBase * (1.0f - size) + 0.5f;
        Color.g = g_texture.Sample(g_sampler, uvG).g;
		
		
		// フォグ？
        //float depth = g_depthTexture.SampleCmpLevelZero(g_depthSampler, input.TexCoord, 0.99f);
        //depth = 1.0f - depth;
        //Color = lerp(float4(0.5, 0.5, 0.5, 0.5), Color, depth);
		
        //for (int i = 1; i < 5; i++)
        //{
        //    Color += g_texture.Sample(g_sampler, input.TexCoord + float2(0.002f, 0.002f) * i) * 0.2f;
        //}
		
	}
	
	return Color * g_color;
}
