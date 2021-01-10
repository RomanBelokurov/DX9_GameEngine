#define SMAP_SIZE		256
#define SHADOW_EPSILON	0.001f
#define REDUCE_ALIASING


float4x4 MatViewerWorld;
float4x4 MatViewerWVP;
float4x4 MatLightWVP;
float4   MtrlColor;
texture  TexScene;
texture  TexSceneDet;
texture  Blend;
texture  Layer4;
texture  TexShadow;
float3   LightDirWorld;
float4   LightAmbientColor;
float4   LPosition;
float 	 TerrainDetail=500.0f;

sampler2D SamplerScene = sampler_state
{
    Texture = <TexScene>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};
sampler2D SamplerSceneDet = sampler_state
{
    Texture = <TexSceneDet>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};
sampler2D BlendMap = sampler_state
{
    Texture = <Blend>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};
sampler2D Layer4map = sampler_state
{
    Texture = <Layer4>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler2D SamplerShadow = sampler_state
{
    Texture		= <TexShadow>;
    MinFilter	= Linear;
    MagFilter	= Linear;
    MipFilter	= Linear;
    AddressU	= Clamp;
    AddressV	= Clamp;
};

struct SceneVertexInput
{
	float4 Pos			: POSITION;
	float3 Normal		: NORMAL;
	float2 Tex			: TEXCOORD0;
};

struct SceneVertexOutput
{
	float4 Pos 			: POSITION;
	float2 Tex 			: TEXCOORD0;
	float4 PosLightSpace	: TEXCOORD1;
	float3 NormalWorld	: TEXCOORD2;
	float3 LightDir 		: TEXCOORD3;
};


//////////////////////////////////////////////////////////

struct ShadowVertexInput
{
	float4 Pos	: POSITION;
    	float3 Normal	: NORMAL;
};

struct ShadowVertexOutput
{
	float4 Pos	: POSITION;
    	float2 Depth	: TEXCOORD0;
};

ShadowVertexOutput VS_Shadow( ShadowVertexInput IN )
{
	ShadowVertexOutput OUT;
    	OUT.Pos	= mul( IN.Pos, MatLightWVP );
    	OUT.Depth.xy = OUT.Pos.zw;
	return OUT;
}

float4 PS_Shadow( ShadowVertexOutput IN ) : COLOR
{
    	return (IN.Depth.x/IN.Depth.y);
}

//////////////////////////////////////////////////////////
// Scene-Shader //////////////////////////////////////////
//////////////////////////////////////////////////////////
SceneVertexOutput VS_Scene( SceneVertexInput IN )
{
	SceneVertexOutput OUT;
	
    	OUT.Pos		= mul( IN.Pos, MatViewerWVP );
    	OUT.NormalWorld	= mul( IN.Normal, (float3x3)MatViewerWorld );
    	OUT.Tex 		= IN.Tex;
    	OUT.PosLightSpace	= mul( IN.Pos, MatLightWVP );
	OUT.LightDir 	= LPosition - OUT.PosLightSpace;

    	return OUT;
}
float4 PS_Scene( SceneVertexOutput IN ) : COLOR
{
	float4 Diffuse;
	float2 ShadowTexCoord;
	IN.LightDir = normalize( IN.LightDir );
	
	ShadowTexCoord   = 0.5*IN.PosLightSpace.xy / IN.PosLightSpace.w + float2( 0.5, 0.5 );
	ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;
#ifdef REDUCE_ALIASING
	float2 Texelpos	= SMAP_SIZE*ShadowTexCoord;
	float2 Lerps	= frac( Texelpos );

	float Sample[4];
	Sample[0] = (tex2D( SamplerShadow, ShadowTexCoord ) + SHADOW_EPSILON < IN.PosLightSpace.z/IN.PosLightSpace.w)? 0.0f: 1.0f;  
	Sample[1] = (tex2D( SamplerShadow, ShadowTexCoord + float2(1.0/SMAP_SIZE, 0) ) + SHADOW_EPSILON < IN.PosLightSpace.z/IN.PosLightSpace.w)? 0.0f: 1.0f;  
	Sample[2] = (tex2D( SamplerShadow, ShadowTexCoord + float2(0, 1.0/SMAP_SIZE) ) + SHADOW_EPSILON < IN.PosLightSpace.z/IN.PosLightSpace.w)? 0.0f: 1.0f;  
	Sample[3] = (tex2D( SamplerShadow, ShadowTexCoord + float2(1.0/SMAP_SIZE, 1.0/SMAP_SIZE) ) + SHADOW_EPSILON < IN.PosLightSpace.z/IN.PosLightSpace.w)? 0.0f: 1.0f;  

	float LightAmount = lerp( lerp( Sample[0], Sample[1], Lerps.x ),lerp( Sample[2], Sample[3], Lerps.x ), Lerps.y );
#else
	float DepthShadowMap = tex2D( SamplerShadow, ShadowTexCoord ) + SHADOW_EPSILON;
	float LightAmount = DepthShadowMap < IN.PosLightSpace.z/IN.PosLightSpace.w ? 0.0f : 1.0f;
#endif
	Diffuse = ( saturate( dot( IN.LightDir, normalize( IN.NormalWorld ) ) ) * LightAmount * ( 1 - LightAmbientColor ) + LightAmbientColor ) * MtrlColor;
	return tex2D( SamplerScene, IN.Tex ) * Diffuse*1.1;
}

float4 PS_SceneT( SceneVertexOutput IN ) : COLOR
{
	float2 ShadowTexCoord;
	ShadowTexCoord   = 0.5*IN.PosLightSpace.xy / IN.PosLightSpace.w + float2( 0.5, 0.5 );
	ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;
	
#ifdef REDUCE_ALIASING
	float2 Texelpos	= SMAP_SIZE*ShadowTexCoord;
	float2 Lerps	= frac( Texelpos );

	float Sample[4];
	Sample[0] = (tex2D( SamplerShadow, ShadowTexCoord ) < IN.PosLightSpace.z/IN.PosLightSpace.w)? 0.0f: 1.0f;
	Sample[1] = (tex2D( SamplerShadow, ShadowTexCoord + float2(1.0/SMAP_SIZE, 0) ) < IN.PosLightSpace.z/IN.PosLightSpace.w)? 0.0f: 1.0f;
	Sample[2] = (tex2D( SamplerShadow, ShadowTexCoord + float2(0, 1.0/SMAP_SIZE) )  < IN.PosLightSpace.z/IN.PosLightSpace.w)? 0.0f: 1.0f;
	Sample[3] = (tex2D( SamplerShadow, ShadowTexCoord + float2(1.0/SMAP_SIZE, 1.0/SMAP_SIZE) ) < IN.PosLightSpace.z/IN.PosLightSpace.w)? 0.0f: 1.0f;

	float LightAmount = lerp( lerp( Sample[0], Sample[1], Lerps.x ),lerp( Sample[2], Sample[3], Lerps.x ), Lerps.y );
#else
	float DepthShadowMap = tex2D( SamplerShadow, ShadowTexCoord ) + SHADOW_EPSILON;
	float LightAmount = DepthShadowMap < IN.PosLightSpace.z/IN.PosLightSpace.w ? 0.0f : 1.0f; 
#endif
	float4 Diffuse = ((  LightAmount * ( 1 - LightAmbientColor ) + LightAmbientColor ) * MtrlColor)*1.1;
        float3 c0 = tex2D(SamplerScene, IN.Tex*TerrainDetail).rgb;
	float3 c1 = tex2D(SamplerSceneDet, IN.Tex*TerrainDetail).rgb;
	float3 c2 = tex2D(Layer4map, IN.Tex*TerrainDetail).rgb;
	float3 B = tex2D(BlendMap, IN.Tex).rgb;
	float totalInverse = 1.0f / (B.r + B.g + B.b);
	c0 *= B.r * totalInverse;
	c1 *= B.g * totalInverse;
	c2 *= B.b * totalInverse;
	float3 final = (c0 + c1 + c2);
	return float4(final.xyz * Diffuse.xyz,Diffuse.w);
}



/////////////////////////////////////////////////////////////
/// Technique ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////
technique ShadowMapping
{
    pass p0
    {
      VertexShader = compile vs_1_1 VS_Shadow();
      PixelShader = compile ps_2_0  PS_Shadow();
    }

    pass p1
    {
      VertexShader = compile vs_1_1 VS_Scene();
      PixelShader = compile ps_2_0 PS_Scene();
    }
    pass p2
    {
	VertexShader = compile vs_1_1 VS_Scene();
     	PixelShader = compile ps_2_0 PS_SceneT();
    }
}


