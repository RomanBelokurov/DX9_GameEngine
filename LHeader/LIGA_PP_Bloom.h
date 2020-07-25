#define SBLOOMVERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_TEX1)
#define REDUCED_WIDTH  256
#define REDUCED_HEIGHT 256
 


struct SBloomVertex
{
	D3DXVECTOR4 pos;
	FLOAT		u;
	FLOAT		v;
};



class CBloomEffect
{
protected:
	ID3DXEffect*			m_Effect;
	LPDIRECT3DTEXTURE9		m_texCopyOrgRT;
	LPDIRECT3DTEXTURE9		m_texReduced;
	LPDIRECT3DTEXTURE9		m_texBloomed;
	LPDIRECT3DVERTEXBUFFER9 m_VB;
	LPDIRECT3DVERTEXBUFFER9 m_VBreduced;

public:
	CBloomEffect();
	bool Create( LPDIRECT3DDEVICE9 Device );
	void PostProcess( LPDIRECT3DDEVICE9 Device );
	void SetBloomFactor( float factor );
	void Destroy();
};

CBloomEffect BLOOMEFFECT;
BOOL DoBloom=true;

CBloomEffect::CBloomEffect()
{
	m_texCopyOrgRT	= NULL;
	m_texReduced	= NULL;
	m_texBloomed	= NULL;
	m_VB			= NULL;
	m_VBreduced		= NULL;
	m_Effect		= NULL;
}

bool CBloomEffect::Create( LPDIRECT3DDEVICE9 Device )
{
	ID3DXBuffer*		ErrorBuffer = NULL;
	LPDIRECT3DSURFACE9	BackBuffer = NULL;
	D3DSURFACE_DESC		BackBufferDesc;
	void*				VertexData;

	if(FAILED(D3DXCreateEffectFromFile(Device,"System/Effects/bloom.fx",NULL,NULL,0,NULL,&m_Effect,NULL)))return false;


	Device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &BackBuffer );
	BackBuffer->GetDesc( &BackBufferDesc );
	ReleaseCOM(BackBuffer)

	if( FAILED( Device->CreateTexture( BackBufferDesc.Width, BackBufferDesc.Height, 1, D3DUSAGE_RENDERTARGET, BackBufferDesc.Format,D3DPOOL_DEFAULT, &m_texCopyOrgRT, NULL ) ) )return false;
	if( FAILED( Device->CreateTexture( REDUCED_WIDTH, REDUCED_HEIGHT, 1, D3DUSAGE_RENDERTARGET, BackBufferDesc.Format,D3DPOOL_DEFAULT, &m_texReduced, NULL ) ) )return false;
	if( FAILED( Device->CreateTexture( REDUCED_WIDTH, REDUCED_HEIGHT, 1, D3DUSAGE_RENDERTARGET, BackBufferDesc.Format,D3DPOOL_DEFAULT, &m_texBloomed, NULL ) ) )return false;
	{
		SBloomVertex Vertices[6] = 
		{ 
		{ D3DXVECTOR4(   -0.5f,						-0.5f,						0.0f, 1.0f), 0.0f, 0.0f },
		{ D3DXVECTOR4(   BackBufferDesc.Width-0.5f,	-0.5f,						0.0f, 1.0f), 1.0f, 0.0f },
		{ D3DXVECTOR4(   -0.5f,						BackBufferDesc.Height-0.5f, 0.0f, 1.0f), 0.0f, 1.0f },
		{ D3DXVECTOR4(   -0.5f,						BackBufferDesc.Height-0.5f, 0.0f, 1.0f), 0.0f, 1.0f },
		{ D3DXVECTOR4(   BackBufferDesc.Width-0.5f,	-0.5f,						0.0f, 1.0f), 1.0f, 0.0f },
		{ D3DXVECTOR4(   BackBufferDesc.Width-0.5f, BackBufferDesc.Height-0.5f, 0.0f, 1.0f), 1.0f, 1.0f },
		};
	
		if( FAILED( Device->CreateVertexBuffer( sizeof(SBloomVertex)*6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,D3DPOOL_DEFAULT, &m_VB, NULL ) ) )exit(0);
		if( FAILED( m_VB->Lock( 0, 0, &VertexData, D3DLOCK_DISCARD ) ) )exit(0);
		memcpy( VertexData, Vertices, sizeof(SBloomVertex)*6 );
		m_VB->Unlock();
	}

	{
		SBloomVertex Vertices[6] = 
		{ 
		{ D3DXVECTOR4(   -0.5f,					-0.5f,					0.0f, 1.0f), 0.0f, 0.0f },
		{ D3DXVECTOR4(   REDUCED_WIDTH-0.5f,	-0.5f,					0.0f, 1.0f), 1.0f, 0.0f },
		{ D3DXVECTOR4(   -0.5f,					REDUCED_HEIGHT-0.5f,	0.0f, 1.0f), 0.0f, 1.0f },
		{ D3DXVECTOR4(   -0.5f,					REDUCED_HEIGHT-0.5f,	0.0f, 1.0f), 0.0f, 1.0f },
		{ D3DXVECTOR4(   REDUCED_WIDTH-0.5f,	-0.5f,					0.0f, 1.0f), 1.0f, 0.0f },
		{ D3DXVECTOR4(   REDUCED_WIDTH-0.5f,	REDUCED_HEIGHT-0.5f,	0.0f, 1.0f), 1.0f, 1.0f },
		};
	
		if( FAILED( Device->CreateVertexBuffer( sizeof(SBloomVertex)*6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,D3DPOOL_DEFAULT, &m_VBreduced, NULL ) ) )exit(0);
		if( FAILED( m_VBreduced->Lock( 0, 0, &VertexData, D3DLOCK_DISCARD ) ) )exit(0);

		memcpy( VertexData, Vertices, sizeof(SBloomVertex)*6 );
		m_VBreduced->Unlock();
	}
	return true;
}

void CBloomEffect::Destroy()
{
	ReleaseCOM( m_Effect )
	ReleaseCOM( m_VB )
	ReleaseCOM( m_VBreduced )
	ReleaseCOM( m_texCopyOrgRT )
	ReleaseCOM( m_texReduced )
	ReleaseCOM( m_texBloomed )
}


void CBloomEffect::PostProcess( LPDIRECT3DDEVICE9 Device )
{
	UINT				passes			= 0;
	LPDIRECT3DSURFACE9	OrgRenderTarget	= NULL;
	LPDIRECT3DSURFACE9	Surface			= NULL;

	Device->GetRenderTarget( 0, &OrgRenderTarget );

	m_texCopyOrgRT->GetSurfaceLevel( 0, &Surface );
	Device->StretchRect( OrgRenderTarget, NULL, Surface, NULL, D3DTEXF_NONE );
	Surface->Release();

	Device->SetRenderState( D3DRS_ZENABLE, false );
	Device->SetRenderState( D3DRS_ZWRITEENABLE, false );
	Device->SetFVF( SBLOOMVERTEX_FVF );

	m_texReduced->GetSurfaceLevel( 0, &Surface );
	Device->SetRenderTarget( 0, Surface );
	Surface->Release();

	m_Effect->SetTechnique( "Blooming" );
	m_Effect->Begin( &passes, 0 );



	Device->BeginScene();
	{
		m_Effect->SetTexture( "TexInput0", m_texCopyOrgRT );
		m_Effect->BeginPass( 0 );
		Device->SetStreamSource( 0, m_VBreduced, 0, sizeof(SBloomVertex) );
        Device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
 		m_Effect->EndPass();
	}
	Device->EndScene();


	m_texBloomed->GetSurfaceLevel( 0, &Surface );
	Device->SetRenderTarget( 0, Surface );
	Surface->Release();


	Device->BeginScene();
	{
		m_Effect->SetTexture( "TexInput0", m_texReduced );
		m_Effect->BeginPass( 1 );
		Device->SetStreamSource( 0, m_VBreduced, 0, sizeof(SBloomVertex) );
        Device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
 		m_Effect->EndPass();
	}
	Device->EndScene();

	
	Device->SetRenderTarget( 0, OrgRenderTarget );
	OrgRenderTarget->Release();


	Device->BeginScene();
	{
		m_Effect->SetTexture( "TexInput0", m_texBloomed );
		m_Effect->SetTexture( "TexInput1", m_texCopyOrgRT );
		m_Effect->BeginPass( 2 );
		Device->SetStreamSource( 0, m_VB, 0, sizeof(SBloomVertex) );
        Device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
 		m_Effect->EndPass();
	}
	Device->EndScene();


	m_Effect->End();
	Device->SetRenderState( D3DRS_ZENABLE, true );
	Device->SetRenderState( D3DRS_ZWRITEENABLE, true );
}



void CBloomEffect::SetBloomFactor( float factor )
{
	if( m_Effect )m_Effect->SetFloat( "BloomFactor", 5.0f );
}