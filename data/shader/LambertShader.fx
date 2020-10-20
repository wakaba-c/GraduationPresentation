//=================================================================================
//型定義
//=================================================================================
struct VS_OUT
{
    float4 Position	: POSITION;
    float4 Color	: COLOR0;
    float2 Texture : TEXCOORD0;
};

//=================================================================================
//グローバル
//=================================================================================
float4x4 WVP; //ワールド行列
float4x4 WIT;  //ワールド行列の逆行列の転置行列
float4 LightDir;  //ｒライトの方向ベクトル
float4 LightIntensity; //入射光の強度

float4 Diffuse = {1.0f, 1.0f, 1.0f, 1.0f}; //拡散反射率
float4 Ambient = {1.0f, 1.0f, 1.0f, 1.0f};	// 環境光

texture texDecal; //メッシュのテクスチャ
texture texToon;  // トゥーンシェーダー用

sampler Sampler = sampler_state //サンプラー
{
    Texture = <texDecal>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
    AddressU = Clamp;
    AddressV = Clamp;
};

sampler ToonShader = sampler_state
{
    Texture = <texToon>;
};

//=================================================================================
//バーテックス・シェーダー
//=================================================================================
VS_OUT VS(float4 Position : POSITION, float3 Normal : NORMAL, float2 Texture : TEXCOORD)
{
	VS_OUT Out = (VS_OUT)0;   
	
	Out.Position=mul(Position,WVP);
	
	float3 WNormal=normalize(mul(Normal,(float3x3)WIT));

    // float4 col = {1.0f, 1.0f, 1.0f, 1.0f};

	// col.xyz = Ambient.xyz * LightIntensity.xyz * Diffuse.xyz * dot(WNormal,-LightDir.xyz);
    // col.w = Diffuse.w;

    // float4 BaseCol = {1.0f, 1.0f, 1.0f, 0.0f};
    // // Out.Color = col * BaseCol;
    Out.Color = LightIntensity * Diffuse * max(Ambient, dot(WNormal,-LightDir.xyz));
    Out.Texture = Texture;
	return Out;
}
//=================================================================================
//ピクセル・シェーダー
//=================================================================================
float4 PS(VS_OUT In) : COLOR
{   
    return In.Color * 2 * tex2D(Sampler, In.Texture);
}

//=================================================================================
// テクニック
//=================================================================================
technique tecLambert
{
    pass Lambert
    {
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}