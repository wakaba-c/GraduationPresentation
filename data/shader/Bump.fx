//簡単のため環境光および、メッシュのマテリアルは考慮していません。

///////////////////////////////////////////////////////////////////////////////////
//型定義
///////////////////////////////////////////////////////////////////////////////////
struct VS_OUT
{
    float4 Pos	: POSITION;
    float2 Tex	: TEXCOORD0;	
    float3 Light	: TEXCOORD1;	
    float3 Eye	: TEXCOORD2;	
};
///////////////////////////////////////////////////////////////////////////////////
//グローバル
///////////////////////////////////////////////////////////////////////////////////
float4x4 mWVP;	
float4 vLightDir;
float4 vColor;
float3 vEyePos;	
texture Decal;
texture NormalMap;

sampler DecalSampler = sampler_state
{
    Texture = <Decal>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
    AddressU = Clamp;
    AddressV = Clamp;
};
sampler NormalSampler = sampler_state
{
    Texture = <NormalMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
    AddressU = Clamp;
    AddressV = Clamp;
};
///////////////////////////////////////////////////////////////////////////////////
//バーテックス・シェーダー
///////////////////////////////////////////////////////////////////////////////////
VS_OUT VS(float4 Pos : POSITION,float3 Normal : NORMAL,float3 Tangent :
						 TANGENT0,float2 Texcoord : TEXCOORD0)
{
	VS_OUT Out = (VS_OUT)0;       

	Out.Pos = mul(Pos, mWVP);
	Out.Tex = Texcoord;	
	float3 Binormal = cross(Normal,Tangent);

	float3 Eye = vEyePos - Pos.xyz;
	Out.Eye.x = dot(Eye,Tangent);
	Out.Eye.y = dot(Eye,Binormal);
	Out.Eye.z = dot(Eye,Normal);
	normalize(Out.Eye);

	float3 Light = vLightDir.xyz;
	Out.Light.x = dot(Light,Tangent);
	Out.Light.y = dot(Light,Binormal);
	Out.Light.z = dot(Light,Normal);
	normalize(Out.Light);
	
	return Out;
}
///////////////////////////////////////////////////////////////////////////////////
//ピクセル・シェーダー
///////////////////////////////////////////////////////////////////////////////////
float4 PS(VS_OUT In) : COLOR
{   
	float3 Normal = 2.0*tex2D( NormalSampler, In.Tex ).xyz-1.0;
	float3 Reflect = reflect(-normalize(In.Eye), Normal);	
	
	float4 	DiffuseTerm = tex2D( DecalSampler, In.Tex )* max(0, dot(Normal, In.Light));
	float4 SpecularTerm = 0.7 * pow(max(0,dot(Reflect, In.Light)), 2);
	
	float4 FinalColor=DiffuseTerm+ SpecularTerm;
	 
    return FinalColor;    
}
///////////////////////////////////////////////////////////////////////////////////
// テクニック
///////////////////////////////////////////////////////////////////////////////////
technique tecBumpMap
{
    pass P0
    {
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}