// Particle system comes here
#include 	<vector>
using namespace std;
 
float getRandomMinMax( float fMin, float fMax )
{
    float fRandNum = (float)rand () / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}


struct Particle
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Velocity;
	float		Alpha;
	int 		DestroyTime;
	int			currentTime;
	void ResetParticle(V3 StartPosition,V3 StartRange,V3 StartVelocityRange,int LifeTime,float AlphaStart)
	{
		Position=StartPosition+StartRange;
		Velocity=StartVelocityRange;
		Alpha=AlphaStart;
		currentTime=0;
		DestroyTime=LifeTime;
	}
};


struct PartSettings
{
	D3DXVECTOR2 StartRangeX,StartRangeY,StartRangeZ;
	D3DXVECTOR2 VelocityRangeX,VelocityRangeY,VelocityRangeZ;
	float AlphaMin;
	D3DXVECTOR2 LifetimeRange;
	D3DXVECTOR2 Tex1WH,Tex2WH;
	D3DXVECTOR3 SpriteColor;
};


class LiPartSys
{
	vector<Particle> 	PART;
	LPDIRECT3DTEXTURE9	TextureStart;
	LPDIRECT3DTEXTURE9	TextureEnd;

	LPD3DXSPRITE        pSprite;
	V3 					SpriteCenter1,SpriteCenter2;
//	D3DXMATRIX			World;
public:
	DWORD 				PartsNum;
	PartSettings		pSet;
	V3 					ExternalForce;
	V3 					Position;
	float 				Scale;
	D3DXVECTOR3			Color;
	float 				SystemSpeed;

	bool Gradation;
	float GradLimit;
	float AlphaStart;
public:
	//void Rescale(V3 Scale){D3DXMatrixScaling(&World,Scale.x,Scale.y,Scale.z);}
	LiPartSys()
	{
		TextureStart=NULL;
		TextureEnd=NULL;
		pSet.StartRangeX.x=-0.5f; pSet.StartRangeX.y=0.5f;
		pSet.StartRangeY.x=-0.5f; pSet.StartRangeY.y=0.5f;
		pSet.StartRangeZ.x=-0.5f; pSet.StartRangeZ.y=0.5f;

		pSet.VelocityRangeX.x=-0.5f;	pSet.VelocityRangeX.y=0.5f;
		pSet.VelocityRangeY.x=-0.5f;	pSet.VelocityRangeY.y=0.5f;
		pSet.VelocityRangeZ.x=-0.5f;	pSet.VelocityRangeZ.y=0.5f;
		pSet.LifetimeRange= D3DXVECTOR2(20,50);
		SystemSpeed=0.5f;
		Position= D3DXVECTOR3(0,0,0);
		ExternalForce=V3(0,0,0);
		Gradation = true;
		GradLimit = 0.5f;
		AlphaStart = 1.0f;
	}
	void CreateSystem(int partDensity=50);
	void Load_1st_Texture(const char* Filename,int width,int height)
	{
		SpriteCenter1=V3(width/2,height/2,width/2);
		pSet.Tex1WH.x=width;pSet.Tex1WH.y=height;
		D3DXCreateTextureFromFileEx(dx9Driver.VPORT1,Filename,width,height,3,0,D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,D3DX_FILTER_LINEAR,D3DX_FILTER_LINEAR,NULL,NULL,NULL,&TextureStart);
	}

	void Load_2nd_Texture(const char* Filename,int width,int height)
	{
		SpriteCenter2=V3(width/2,height/2,width/2);
		pSet.Tex2WH.x=width;pSet.Tex2WH.y=height;
		D3DXCreateTextureFromFileEx(dx9Driver.VPORT1,Filename,width,height,3,0,D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,D3DX_FILTER_LINEAR,D3DX_FILTER_LINEAR,NULL,NULL,NULL,&TextureEnd);
	}

	void UpdateSystem();
	void RenderSystem();
	void ResetSystem();

	void CreateParticles(int num)
	{
		PART.clear();
		Particle tmp;
		for(int i=0;i<num;i++)
		{
			tmp.ResetParticle(
				Position,
				V3(
					getRandomMinMax(pSet.StartRangeX.x,pSet.StartRangeX.y),
					getRandomMinMax(pSet.StartRangeY.x,pSet.StartRangeY.y),
					getRandomMinMax(pSet.StartRangeZ.x,pSet.StartRangeZ.y)),
				V3(
					getRandomMinMax(pSet.VelocityRangeX.x,pSet.VelocityRangeX.y),
					getRandomMinMax(pSet.VelocityRangeY.x,pSet.VelocityRangeY.y),
					getRandomMinMax(pSet.VelocityRangeZ.x,pSet.VelocityRangeZ.y)),
				getRandomMinMax(pSet.LifetimeRange.x,pSet.LifetimeRange.y),AlphaStart);
				
			PART.push_back(tmp);
		}
	}
};

void LiPartSys::CreateSystem(int partDensity)
{
	D3DXCreateSprite(dx9Driver.VPORT1, &pSprite );
	PartsNum=partDensity;
	CreateParticles(partDensity);
}

void LiPartSys::ResetSystem()
{
	for(int i=0; i<PART.size(); i++)
				PART[i].ResetParticle(
				Position,
				V3(
					getRandomMinMax(pSet.StartRangeX.x,pSet.StartRangeX.y),
					getRandomMinMax(pSet.StartRangeY.x,pSet.StartRangeY.y),
					getRandomMinMax(pSet.StartRangeZ.x,pSet.StartRangeZ.y)),
				V3(
					getRandomMinMax(pSet.VelocityRangeX.x,pSet.VelocityRangeX.y),
					getRandomMinMax(pSet.VelocityRangeY.x,pSet.VelocityRangeY.y),
					getRandomMinMax(pSet.VelocityRangeZ.x,pSet.VelocityRangeZ.y)),
				getRandomMinMax(pSet.LifetimeRange.x,pSet.LifetimeRange.y),AlphaStart);
}

void LiPartSys::UpdateSystem()
{
	for(int i=0; i<PART.size(); i++)
	{
		if(PART[i].currentTime>PART[i].DestroyTime)PART[i].ResetParticle(
				Position,
				V3(
					getRandomMinMax(pSet.StartRangeX.x,pSet.StartRangeX.y),
					getRandomMinMax(pSet.StartRangeY.x,pSet.StartRangeY.y),
					getRandomMinMax(pSet.StartRangeZ.x,pSet.StartRangeZ.y)),
				V3(
					getRandomMinMax(pSet.VelocityRangeX.x,pSet.VelocityRangeX.y),
					getRandomMinMax(pSet.VelocityRangeY.x,pSet.VelocityRangeY.y),
					getRandomMinMax(pSet.VelocityRangeZ.x,pSet.VelocityRangeZ.y)),
				getRandomMinMax(pSet.LifetimeRange.x,pSet.LifetimeRange.y),AlphaStart);
				
		PART[i].currentTime++;
		PART[i].Velocity+=ExternalForce;
		PART[i].Position += (PART[i].Velocity*SystemSpeed);
		if(PART[i].Alpha-(1.0f/PART[i].DestroyTime)>0)
			PART[i].Alpha = PART[i].Alpha-(1.0f/PART[i].DestroyTime);
		else PART[i].Alpha = 0;
	}
}

void LiPartSys::RenderSystem()
{
	D3DXMATRIX pView,World;
	dx9Driver.VPORT1->GetTransform(D3DTS_VIEW,&pView);
	D3DXMatrixScaling(&World,Scale,Scale,Scale);
	pSprite->SetWorldViewLH(&World,&pView);

	dx9Driver.VPORT1->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_BILLBOARD );
	for(int i=0; i<PART.size(); i++)
	{
		if(!Gradation)			
			pSprite->Draw( TextureStart, NULL, &SpriteCenter1, &(PART[i].Position), D3DCOLOR_COLORVALUE(Color.x,Color.y,Color.z,PART[i].Alpha));
		else
		{
			if(PART[i].Alpha > GradLimit)pSprite->Draw( TextureStart, NULL, &SpriteCenter1, &(PART[i].Position), D3DCOLOR_COLORVALUE(Color.x,Color.y,Color.z,PART[i].Alpha));
			else pSprite->Draw( TextureEnd, NULL, &SpriteCenter1, &(PART[i].Position), D3DCOLOR_COLORVALUE(Color.x,Color.y,Color.z,PART[i].Alpha));
		}
	}
	pSprite->End();
	dx9Driver.VPORT1->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
}



LiPartSys LoadParticleFromFile(const char* Filename)
{
	LiPartSys PSys;
	unsigned long fsize;
	float float_data1,float_data2,float_data3;
	int int_data;
	byte b;
	FILE *FIN,*FOUT;
	FIN=fopen(Filename,"rb");

	fread(&fsize,sizeof(int),1,FIN);

	FOUT=fopen("~tex1.tmp","wb");
	for(int i=0;i<fsize;i++)
	{
		putc(getc(FIN),FOUT);
	}
	if(FOUT!=NULL)fclose(FOUT);
	PSys.Load_1st_Texture("~tex1.tmp",16,16);
	remove("~tex1.tmp");
	fread(&fsize,sizeof(int),1,FIN);
	FOUT=fopen("~tex2.tmp","wb");
	for(int i=0;i<fsize;i++)
	{
		putc(getc(FIN),FOUT);
	}
	if(FOUT!=NULL)fclose(FOUT);
	PSys.Load_2nd_Texture("~tex2.tmp",16,16);
	remove("~tex2.tmp");

//params
	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	PSys.Position=V3(float_data1,float_data2,float_data3);

	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	PSys.ExternalForce=V3(float_data1,float_data2,float_data3);
	fread(&PSys.PartsNum,sizeof(int),1,FIN);
	fread(&PSys.SystemSpeed,sizeof(float),1,FIN);
	fread(&int_data,sizeof(int),1,FIN);
	if(int_data>0)PSys.Gradation=true; else PSys.Gradation=false;
	fread(&PSys.GradLimit,sizeof(float),1,FIN);
	fread(&PSys.pSet,sizeof(PartSettings),1,FIN);
	if(FIN!=NULL)fclose(FIN);
	
	PSys.CreateSystem(PSys.PartsNum);
	return PSys;
}

//LiPartSys Shooting;