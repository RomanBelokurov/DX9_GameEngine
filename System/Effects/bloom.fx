#define REDUCED_WIDTH 256
#define REDUCED_HEIGHT 256

Texture 		TexInput0;
Texture 		TexInput1;
float			BloomFactor =  0.3f;


static const float BlurWeights[13] = 
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};
 


float2 HorizontalPixel[13] =
{
    { -6, 0 },
    { -5, 0 },
    { -4, 0 },
    { -3, 0 },
    { -2, 0 },
    { -1, 0 },
    {  0, 0 },
    {  1, 0 },
    {  2, 0 },
    {  3, 0 },
    {  4, 0 },
    {  5, 0 },
    {  6, 0 },
};
float2 VerticalPixel[13] =
{
    { 0, -6 },
    { 0, -5 },
    { 0, -4 },
    { 0, -3 },
    { 0, -2 },
    { 0, -1 },
    { 0,  0 },
    { 0,  1 },
    { 0,  2 },
    { 0,  3 },
    { 0,  4 },
    { 0,  5 },
    { 0,  6 },
};

//TextureSampler
sampler Sampler0 = sampler_state 
{
    texture = <TexInput0>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
sampler Sampler1 = sampler_state 
{
    texture = <TexInput1>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

void VS_Passthru( float4 Pos : POSITION,
				  float2 tex : TEXCOORD0,
				  out float4 oPos : POSITION,
				  out float2 texcoord : TEXCOORD0 )
{
	oPos		= Pos;
	texcoord	= tex;
}

void PS_Textured(  float2 texcoord : TEXCOORD0,
                   out float4 Color : COLOR )
{
	Color = tex2D( Sampler0, texcoord); 	    
}

void PS_Bloom(  float2 texcoord : TEXCOORD0,
           out float4 Color : COLOR )
{
	float2 uvoffset;
	Color = float4( 0.0f, 0.0f, 0.0f, 1.0f );
	for( int i=0; i < 13; i++ )
	{	
		uvoffset = VerticalPixel[i].xy/float2( 1.0f, REDUCED_HEIGHT );	
		Color	+= tex2D( Sampler0, texcoord + uvoffset ) * BlurWeights[i];
		uvoffset = HorizontalPixel[i].xy/float2( REDUCED_WIDTH, 1.0f );	
		Color	+= tex2D( Sampler0, texcoord + uvoffset ) * BlurWeights[i];
	}
	
 	Color=Color*BloomFactor;
}

void PS_Add(  float2 texcoord : TEXCOORD0,
           out float4 Color : COLOR )
{
	Color = tex2D( Sampler0, texcoord )*0.5f + tex2D( Sampler1, texcoord );
}

technique Blooming
{
    pass Downsize
    {		
		VertexShader	= compile vs_2_0 VS_Passthru();
		PixelShader		= compile ps_2_0 PS_Textured();
    }
    pass Bloom
    {
		VertexShader	= compile vs_2_0 VS_Passthru();
		PixelShader		= compile ps_2_0 PS_Bloom();
    }
    pass AddTextures
    {
		VertexShader	= compile vs_2_0 VS_Passthru();
		PixelShader		= compile ps_2_0 PS_Add();
    }
    
}