// 頂点シェーダ

// インスタンシングデータ
#define MAX_INSTANCE (512)
struct InstancingMatrix
{
    matrix mWorld;
    matrix mTexture;
};


// グローバル
cbuffer global : register(b0) {
	matrix g_mVP;
	matrix g_mLightVP;
};

// インスタンシンググローバル
cbuffer InstancingGlobal : register(b2)
{
    InstancingMatrix g_mInstancing[MAX_INSTANCE];
};

// パラメータ
struct VS_INPUT {
	float3	Position	: POSITION;
	float3	Normal		: NORMAL;
	float2	TexCoord	: TEXCOORD0;
	float4	Diffuse		: COLOR0;
    uint instID : SV_InstanceID;
};

struct VS_OUTPUT {
	float4	Position	: SV_Position;
	float4	lightPosition	: WORLD_POSITION;
	float3	Pos4PS		: TEXCOORD0;
    float3 Normal : TEXCOORD1;
	float2	TexCoord	: TEXCOORD2;
    float3  Target      : TEXCOORD3;
    float3  BinNormal   : TEXCOORD4;
	float4	Diffuse		: COLOR0;
    uint    instID      : TEXCOORD5;
};

VS_OUTPUT main(VS_INPUT input)
{
    uint n = input.instID % MAX_INSTANCE;
    
	VS_OUTPUT output;
	float4 P = float4(input.Position, 1.0f);
	float4 wPos = mul(P, g_mInstancing[n].mWorld);
    output.Position = mul(wPos, g_mVP);
	output.lightPosition = mul(wPos, g_mLightVP);
    output.Pos4PS = wPos.xyz;
    
    output.Normal = normalize(input.Normal);
    float3 v = float3(0.5f, 0.5f, 0.5f);
    v = normalize(v);
    //if (input.Normal.y > 0.75f)
    //    v.x = 1.0f;
    //else
    //    v.y = 1.0f;
    output.Target = normalize(cross(input.Normal, v)); // 接線
    output.BinNormal = normalize(cross(input.Normal, output.Target)); // 従法線
    
    output.Normal = mul(float4(output.Normal, 0.0f), g_mInstancing[n].mWorld).xyz;
    output.Target = mul(float4(output.Target, 0.0f), g_mInstancing[n].mWorld).xyz;
    output.BinNormal = mul(float4(output.BinNormal, 0.0f), g_mInstancing[n].mWorld).xyz;
    
    
    output.TexCoord = mul(float4(input.TexCoord, 0.0f, 1.0f), g_mInstancing[n].mTexture).xy;
	output.Diffuse = input.Diffuse;
    
    output.instID = n;
    
	return output;
}


// nointerpolation ←セマンティクスの補正を無くす