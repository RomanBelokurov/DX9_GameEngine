
float4x4 MatViewerWorld;
float4x4 MatViewerWVP;
float4x4 MatLightWVP;

float4   	LightAmbientColor;
float4   	MtrlColor;
texture  	TexScene;
float4   	LPosition;
float4   	EyePosition;
float 	        Spec=5.0f;


sampler2D SamplerScene = sampler_state
{
    Texture = <TexScene>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};


//////////////////////////////////////////////////////////
// Scene-Shader //////////////////////////////////////////
//////////////////////////////////////////////////////////
struct A2P
{
	float4 Pos			: POSITION;
	float3 Normal		: NORMAL;
	float2 Tex0			: TEXCOORD0;
};

struct V2P
{
	float4 Pos : POSITION;
	float4 WorldPos : TEXCOORD0;
	float3 ViewNormal : TEXCOORD1;
	float2 TexCoord0 : TEXCOORD2;
	float4 Diffuse : TEXCOORD3;
	float3 LightDir : TEXCOORD4;
	float3 ViewDir : TEXCOORD5;
};

V2P VS( A2P IN )
{
	V2P OUT;
	OUT.Pos		= mul( IN.Pos, MatViewerWVP );
	OUT.WorldPos	= mul( IN.Pos, MatViewerWorld );
	OUT.LightDir	= LPosition - OUT.WorldPos;
	OUT.ViewDir		= EyePosition - OUT.WorldPos;
	OUT.ViewNormal	= normalize( mul( IN.Normal, (float3x3)MatViewerWorld));
	OUT.Diffuse		= MtrlColor;
	OUT.TexCoord0	= IN.Tex0;
	return OUT;
}


float4 PS( in V2P IN ) : COLOR
{
	IN.LightDir = normalize( IN.LightDir );
	IN.ViewDir = normalize( IN.ViewDir );
	float4 I = ( saturate( dot( IN.LightDir, normalize( IN.ViewNormal ) ) ) * ( 1 - LightAmbientColor ) + LightAmbientColor ) * MtrlColor;
	float3 Reflect = normalize(2 * I * IN.ViewNormal - IN.LightDir);
	float4 Specular = pow(0.98f*saturate(dot(Reflect, IN.ViewDir)), 32);
	float4 TexColor = tex2D( SamplerScene, IN.TexCoord0 ).xyzw;
	return  IN.Diffuse * TexColor * I + Specular * 0.2f;
}



/////////////////////////////////////////////////////////////
/// Technique ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////
technique LightMapping
{
    pass p0
    {
		VertexShader = compile vs_1_0 VS();
		PixelShader = compile ps_2_0  PS();
    }
}


