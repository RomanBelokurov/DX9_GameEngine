class DXText
{
	LPD3DXFONT g_pd3dxFont;
public:
	DXText(){g_pd3dxFont=NULL;}
	~DXText(){ReleaseCOM(g_pd3dxFont);}
	void Setup2DFont(LPDIRECT3DDEVICE9 g_pd3dDevice, const char* font,int fsize,int style);
	void Draw2DText(string textstring,int x,int y,D3DXCOLOR color);
};
 
void DXText::Setup2DFont(LPDIRECT3DDEVICE9 g_pd3dDevice, const char* font,int fsize,int style)
{
	if(FAILED(D3DXCreateFont(g_pd3dDevice,fsize,0,style,1,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,DEFAULT_QUALITY,0,font,&g_pd3dxFont)))MessageBox(0,_T("text!"),_T("T"),NULL);
}

void DXText::Draw2DText(string textstring,int x,int y,D3DXCOLOR color)
{
	RECT destRect1;
	SetRect( &destRect1, x, y, 0, 0 );

	if((g_pd3dxFont->DrawTextA(NULL,textstring.c_str(),-1,&destRect1,DT_NOCLIP,color))<=0)exit(0);
}


class DXSprite
{
	LPDIRECT3DTEXTURE9		Texture;
	LPD3DXSPRITE            pSprite;
	int SP_height,SP_width;
public:
	void LoadSprite(LPDIRECT3DDEVICE9 g_pd3dDevice, const char* TextureFName,int width,int height,D3DCOLOR IgnoreColor);
	~DXSprite(){if(Texture)ReleaseCOM(Texture);if(pSprite)ReleaseCOM(pSprite);}
	void DrawSprite(D3DXVECTOR3 position,D3DCOLOR IgnoreColor);
};

void DXSprite::LoadSprite(LPDIRECT3DDEVICE9 g_pd3dDevice, const char* TextureFName,int width,int height,D3DCOLOR IgnoreColor)
{
	D3DXIMAGE_INFO d3dxImageInfo;
	SP_height=height;SP_width=width;
	D3DXCreateTextureFromFileEx( g_pd3dDevice, TextureFName,width,height,1,D3DPOOL_DEFAULT,D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,&d3dxImageInfo,NULL,&Texture );
	D3DXCreateSprite( g_pd3dDevice, &pSprite ); 
}

void DXSprite::DrawSprite(D3DXVECTOR3 position,D3DCOLOR IgnoreColor)
{
	pSprite->Begin(D3DXSPRITE_ALPHABLEND);//D3DXSPRITE_SORT_DEPTH_BACKTOFRONT
	if(FAILED(pSprite->Draw( Texture,NULL,NULL,&position,IgnoreColor )))MessageBox(0,"Sprite error1","Error!",NULL);
	pSprite->End();
}


