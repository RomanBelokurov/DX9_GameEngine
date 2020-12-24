// PlayerClass




struct WAP
{
	V3 Position;
	D3DXMATRIX World;
	void UpdateMatrix()
	{
        D3DXMATRIX mm1,mm2,mm3,mrot,mResc,mTrans;
		D3DXMatrixTranslation(&mTrans,Position.x,Position.y,Position.z);

		D3DXVECTOR3 _x(1.0f, 0.0f, 0.0f);
		D3DXVECTOR3 _y(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 _z(0.0f, 0.0f, 1.0f);
		D3DXMatrixRotationAxis(&mm1, &_x, DEG_TO_RAD(0.0f));
		D3DXMatrixRotationAxis(&mm2, &_y, DEG_TO_RAD(0.0f));
		D3DXMatrixRotationAxis(&mm3, &_z, DEG_TO_RAD(0.0f));
		mrot=mm3*mm1*mm2;
		D3DXMatrixScaling(&mResc,1.0f,1.0f,1.0f);
		World=mrot*mResc*mTrans;
	}
	void Draw()
	{
		DrawBB(dx9Driver.VPORT1,Position-V3(0.01f,0.01f,0.01f),Position+V3(0.01f,0.01f,0.01f),D3DCOLOR_XRGB(255,0,0));
    }
};

struct LI_MODEL
{
	LPDIRECT3DTEXTURE9*		Textures;
	LPD3DXMESH				Mesh;
	DWORD					MatNum;

	V3 Scal, Pos, Rot;
	D3DXMATRIX Matr,mRotation;

	void UpdateRotationMatrix()
	{
		D3DXMATRIX mm1,mm2,mm3;
		D3DXVECTOR3 _x(1.0f, 0.0f, 0.0f);
		D3DXVECTOR3 _y(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 _z(0.0f, 0.0f, 1.0f);

		D3DXMatrixRotationAxis(&mm1, &_x, DEG_TO_RAD(Rot.x));
		D3DXMatrixRotationAxis(&mm2, &_y, DEG_TO_RAD(Rot.y));
		D3DXMatrixRotationAxis(&mm3, &_z, DEG_TO_RAD(Rot.z));
		mRotation=mm3*mm1*mm2;	
	}

	void LoadMeshPart(const char* FileName)
	{
		ReleaseCOM(Mesh);
		Mesh=NULL;
		D3DXLoadMeshFromX (FileName, D3DXMESH_MANAGED, dx9Driver.VPORT1, NULL, NULL,NULL, &MatNum, &Mesh);
		Textures = new LPDIRECT3DTEXTURE9[MatNum];
		for( DWORD i=0; i<MatNum; i++ )Textures[i]=NULL;
	}

	void LoadTexturePart(const char* FileName,int ID)
	{
		if(Textures[ID]!=NULL)ReleaseCOM(Textures[ID]);
		D3DXCreateTextureFromFileEx(dx9Driver.VPORT1,FileName,512,512,7,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_FILTER_LINEAR,NULL,NULL,NULL,&Textures[ID]);
	}

	void LoadObject(const char* FileName)
	{
		FILE *FIN,*FOUT;
		FIN=fopen(FileName,"rb");
		int fsize=0;

		fread(&fsize,sizeof(int),1,FIN);
		FOUT=fopen("mesh.tmp","wb");
		for(int i=0;i<fsize;i++)
		{
			putc(getc(FIN),FOUT);
		}
		if(FOUT!=NULL)fclose(FOUT);
		LoadMeshPart("mesh.tmp");
		remove("mesh.tmp");

		for(int i=0;i<MatNum;i++)
		{
			fread(&fsize,sizeof(int),1,FIN);
			FOUT=fopen("tex.tmp","wb");
			for(int i=0;i<fsize;i++)
			{
				putc(getc(FIN),FOUT);
			}
			if(FOUT!=NULL)fclose(FOUT);
			LoadTexturePart("tex.tmp",i);
			remove("tex.tmp");
		}
		if(FIN!=NULL)fclose(FIN); 
	}

	void MatrixUpdate()
	{
		D3DXMATRIX mm1,mm2,mm3,mrot,mResc,mTrans;
		D3DXMatrixTranslation(&mTrans,Pos.x,Pos.y,Pos.z);
		D3DXVECTOR3 _x(1.0f, 0.0f, 0.0f);
		D3DXVECTOR3 _y(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 _z(0.0f, 0.0f, 1.0f);
		D3DXMatrixRotationAxis(&mm1, &_x, DEG_TO_RAD(Rot.x));
		D3DXMatrixRotationAxis(&mm2, &_y, DEG_TO_RAD(Rot.y));
		D3DXMatrixRotationAxis(&mm3, &_z, DEG_TO_RAD(Rot.z));
		mrot=mm3*mm1*mm2;
		D3DXMatrixScaling(&mResc,Scal.x,Scal.y,Scal.z);
		Matr=mrot*mResc*mTrans;
	}

	void Draw(D3DXMATRIX M)
	{		
		if(Mesh!=NULL)
		{
			dx9Driver.VPORT1->SetTransform (D3DTS_WORLD, &M);
			if(dx9Driver.RConf.RenderShadows)
			{
				SKYDOME.Update();
				SKYDOME.m_Effect->Begin( NULL, 0 );
				SKYDOME.m_Effect->BeginPass( 1 );
				for( DWORD i=0; i<MatNum; i++ )
				{
					if(Textures[i]!=NULL)SKYDOME.m_Effect->SetTexture( "TexScene", Textures[i]);
					SKYDOME.m_Effect->CommitChanges();
					Mesh->DrawSubset( i );
				}
				SKYDOME.m_Effect->EndPass();
				SKYDOME.m_Effect->End();
			}else
			{
				SKYDOME.Update();
				SKYDOME.Lightmap_Effect->Begin( NULL, 0 );
				SKYDOME.Lightmap_Effect->BeginPass( 0 );
				for( DWORD i=0; i<MatNum; i++ )
				{
					if(Textures[i]!=NULL)SKYDOME.Lightmap_Effect->SetTexture( "TexScene", Textures[i]);
					SKYDOME.Lightmap_Effect->CommitChanges();
					Mesh->DrawSubset( i );
				}
				SKYDOME.Lightmap_Effect->EndPass();
				SKYDOME.Lightmap_Effect->End();
			}
			D3DXMATRIX Reset;
			D3DXMatrixIdentity(&Reset);
			dx9Driver.VPORT1->SetTransform (D3DTS_WORLD, &Reset);
		}		
	}
};



struct LI_PHY
{
	LPD3DXMESH phBody;
	V3 BodyScal, BodyPos, BodyRot;
	D3DXMATRIX PhysMatr;
	void Rescale()
	{
		D3DXMatrixIdentity(&PhysMatr);
		D3DXMatrixScaling(&PhysMatr,BodyScal.x,BodyScal.y,BodyScal.z);
	}
	
	void Draw(D3DXMATRIX M)
	{
		dx9Driver.VPORT1->SetTransform (D3DTS_WORLD, &M);
		if(phBody!=NULL)
		{
			dx9Driver.VPORT1->SetTexture( 0, NULL);
			phBody->DrawSubset( 0 );
		}
		D3DXMATRIX Reset;
		D3DXMatrixIdentity(&Reset);
		dx9Driver.VPORT1->SetTransform (D3DTS_WORLD, &Reset);
		dx9Driver.VPORT1->SetTexture( 0, NULL);
	}

	void MatrixUpdate()
	{
		D3DXMATRIX mm1,mm2,mm3,mrot,mResc,mTrans;
		D3DXMatrixTranslation(&mTrans,BodyPos.x,BodyPos.y,BodyPos.z);
		D3DXVECTOR3 _x(1.0f, 0.0f, 0.0f);
		D3DXVECTOR3 _y(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 _z(0.0f, 0.0f, 1.0f);
		D3DXMatrixRotationAxis(&mm1, &_x, DEG_TO_RAD(BodyRot.x));
		D3DXMatrixRotationAxis(&mm2, &_y, DEG_TO_RAD(BodyRot.y));
		D3DXMatrixRotationAxis(&mm3, &_z, DEG_TO_RAD(BodyRot.z));
		mrot=mm3*mm1*mm2;
		D3DXMatrixScaling(&mResc,BodyScal.x,BodyScal.y,BodyScal.z);
		PhysMatr=mrot*mResc*mTrans;
	}
};



class Actor
{
	LI_PHY Body,Wheels[4];
public:
	WAP WAPS[10];
	LI_MODEL Cusov, Wheel, Tower, Front;
	LPSPERIGIDBODY pBodyVehicle, pBodyWheel[4];
	SPEArray<LPSPERIGIDBODY>		Bodies;
	SPEArray<LPSPEJOINT>			Joints;
	SPEVector WheelPin;
	SPEVector RotationPos1,RotationPos2, RotationDir;
	SPEVector VehicleBodyAnchor[4];  // initialization body anchor of vehicle
	float VehicleDirection;
	LiPartSys Wheel1Foam,Wheel2Foam;
	V3 BackAimPos,FrontAimPos;

	void CreatePhysics();
public:	
	void LoadActor(const char* FileName);
	void LoadWheels(const char* FileName){Wheel.LoadObject(FileName);}
	void LoadTower(const char* FileName){Tower.LoadObject(FileName);}
	void LoadFront(const char* FileName){Front.LoadObject(FileName);}
	

	void TurnTower(float Value,float Angle)
	{
		Tower.Rot.y+=Value;
//		Tower.Rot.z+=Angle;
		Tower.MatrixUpdate();
		Tower.UpdateRotationMatrix();

		D3DXMATRIX matWorld,mat;
		Bodies[0]->GetTransformMesh (&matWorld);
		mat=WAPS[0].World*matWorld;
		BackAimPos=V3(mat._41,mat._42,mat._43);
		mat=WAPS[1].World*matWorld;
		FrontAimPos=V3(mat._41,mat._42,mat._43);

		FrontAimPos=SphereTrans(BackAimPos,FrontAimPos,V3(0.0f,Tower.Rot.y,0.0f));
		FrontAimPos.y-=Tower.Rot.z*0.01f;
		
		D3DXVECTOR3 lValue = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		float angle=pBodyWheel[0]->GetPosition().y-pBodyWheel[1]->GetPosition().y;
		VP1Camera.lookAt(BackAimPos,FrontAimPos, lValue);
		VP1Camera.update(D3DXVECTOR3(0.0f,0.0f,0.0f),DEG_TO_RAD(0),DEG_TO_RAD(0),0.05f,angle*4);		
	}

	void SetPosition(V3 Position)
	{
		pBodyVehicle->SetPosition(pBodyVehicle->GetPosition()+SPEVector(Position.x,Position.y,Position.z));
		for(int i=0;i<4;i++)pBodyWheel[i]->SetPosition(pBodyWheel[i]->GetPosition()+SPEVector(Position.x,Position.y,Position.z));
	}
	
	int GetSpeed()
	{
		SPEVector vel=pBodyVehicle->GetVelocity();
		vel.y=0.0f;
		return vel.Length()*10;
	}
	
	void RenderActor()
	{
		D3DXMATRIX matWorld;
		for(int i=0; i<Bodies.size; i++)
		{
			if(Bodies[i]->UserData==0) continue;
			Bodies[i]->GetTransformMesh (&matWorld);
			if(i==0)
			{
				Cusov.Draw(Cusov.Matr*matWorld);
				Tower.Draw(Tower.Matr*matWorld);
				Front.Draw(Front.Matr*matWorld);
				if(dx9Driver.RConf.DrawParticles)
				{
					Wheel1Foam.UpdateSystem();
					Wheel2Foam.UpdateSystem();
					Wheel2Foam.AlphaStart = Wheel1Foam.AlphaStart = (float)GetSpeed() /200;
					Wheel1Foam.Position=V3(pBodyWheel[2]->GetPosition().x,pBodyWheel[2]->GetPosition().y-0.1f,pBodyWheel[2]->GetPosition().z)/0.07f;
					Wheel2Foam.Position=V3(pBodyWheel[3]->GetPosition().x,pBodyWheel[3]->GetPosition().y-0.1f,pBodyWheel[3]->GetPosition().z)/0.07f;
					Wheel1Foam.RenderSystem();				
					Wheel2Foam.RenderSystem();				
				}
			}
			else Wheel.Draw(Wheels[i-1].PhysMatr*matWorld);
		}
		D3DXMATRIX Reset;
		D3DXMatrixIdentity(&Reset);
		dx9Driver.VPORT1->SetTransform (D3DTS_WORLD, &Reset);
	}

	void Gas();
	void Back();
	void Brake(){for(int i=2; i<4; i++)pBodyWheel[i]->SetAngularVelocity(SPEVector(0, 0, 0));}
	void Turn(DWORD dir); 
};

void Actor::CreatePhysics()
{
	LPSPESHAPE pShape=pWorld->CreateShape();
	//body creation:
	
	Body.phBody = LoadMeshActor(dx9Driver.VPORT1, Body.phBody , Body.PhysMatr);
	InitShape(pShape,Body.phBody);
	pBodyVehicle=pWorld->AddRigidBody (pShape);
	pBodyVehicle->UserData=Body.phBody;
	pBodyVehicle->SetMass(3000);
	Bodies.push(pBodyVehicle);
	
	//Wheels
	WheelPin=SPEVector(0.0f,0.0f,1.0f);	
	SPEVector WheelPosition[]={
		SPEVector(Wheels[0].BodyPos.x,0,Wheels[0].BodyPos.z),
		SPEVector(Wheels[1].BodyPos.x,0,Wheels[1].BodyPos.z), 
		SPEVector(Wheels[2].BodyPos.x,0,Wheels[2].BodyPos.z), 
		SPEVector(Wheels[3].BodyPos.x,0,Wheels[3].BodyPos.z)};
		
		D3DXMATRIX mat;
		D3DXMatrixScaling(&mat, Wheels[0].BodyScal.x, Wheels[0].BodyScal.y, Wheels[0].BodyScal.z);
		Wheels[0].phBody = LoadMeshActor(dx9Driver.VPORT1, Wheels[0].phBody , mat);
		Wheels[0].Rescale();
		Wheels[3]=Wheels[2]=Wheels[1]=Wheels[0]; 
	pShape->InitializeAsSphere(Wheels[0].BodyScal.x*0.5f);

	SPEJointDesc jd;
	LPSPEJOINT pJoint;
	for(int i=0; i<4; i++)
	{
		pBodyWheel[i]=pWorld->AddRigidBody(pShape);
		pBodyWheel[i]->SetPosition (WheelPosition[i]);		
		pBodyWheel[i]->UserData=Wheels[i].phBody;
		Bodies.push(pBodyWheel[i]);

		for(int j=0; j<2; j++)
		{
			jd.Default();
			jd.Body[0]=pBodyVehicle;
			jd.Body[1]=pBodyWheel[i];
			SPEVector offset = j==0 ? WheelPin*0.5f : WheelPin*-0.5f;
			SPEVector WorldAnchor=WheelPosition[i]+offset;
			if(i==0)
			{
				VehicleBodyAnchor[j]=pBodyVehicle->GetState()->PointWorldToBody(WorldAnchor); //
				RotationPos1=pBodyVehicle->GetState()->PointWorldToBody(WheelPosition[i]);
				RotationDir=SPEVector(0.0f,1.0f,0.0f);
			}else
			if(i==1)
			{
				VehicleBodyAnchor[j+2]=pBodyVehicle->GetState()->PointWorldToBody(WorldAnchor); //
				RotationPos2=pBodyVehicle->GetState()->PointWorldToBody(WheelPosition[i]);
				RotationDir=SPEVector(0.0f,1.0f,0.0f);
			}
			jd.WorldAnchor=WorldAnchor;
			jd.DetectCollision=false; // disable collision detection between pBodyVehicle and pBodyWheel[i]
			pJoint=pWorld->AddJoint(jd);
			Joints.push(pJoint);
		}
	}
	pBodyWheel[0]->SetFriction(1.0f);	pBodyWheel[0]->SetMass(1000);
	pBodyWheel[1]->SetFriction(1.0f);	pBodyWheel[1]->SetMass(1000);
	pBodyWheel[2]->SetFriction(1.0f);	pBodyWheel[2]->SetMass(1000);
	pBodyWheel[3]->SetFriction(1.0f);	pBodyWheel[3]->SetMass(1000);
	VehicleDirection=0.0f;
	
	for(int i=0; i<2; i++)
	{
		VehicleBodyAnchor[i].y-=Body.BodyPos.y;
		VehicleBodyAnchor[i+2].y-=Body.BodyPos.y;
	}
}

void Actor::LoadActor(const char* FileName)
{
	unsigned long fsize;
	float float_data1,float_data2,float_data3;
	byte b;


	FILE *FIN,*FOUT;
	FIN=fopen(FileName,"rb");

	fread(&fsize,sizeof(int),1,FIN);
	if(fsize>0)
	{
		FOUT=fopen("tmp.x","wb");
		for(int i=0; i<fsize; i++) putc(getc(FIN),FOUT);
		if(FOUT!=NULL)fclose(FOUT);
		D3DXLoadMeshFromX ("tmp.x", D3DXMESH_MANAGED, dx9Driver.VPORT1, NULL, NULL,NULL,NULL, &Body.phBody);
		remove("tmp.x");
	}

	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Body.BodyPos=V3(float_data1,float_data2,float_data3);
	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Body.BodyRot=V3(float_data1,float_data2,float_data3);
	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Body.BodyScal=V3(float_data1,float_data2,float_data3);

//wheel	
	fread(&fsize,sizeof(int),1,FIN);
	if(fsize>0)
	{
		FOUT=fopen("tmp.x","wb");
		for(int i=0; i<fsize; i++) putc(getc(FIN),FOUT);
		if(FOUT!=NULL)fclose(FOUT);
		D3DXLoadMeshFromX ("tmp.x", D3DXMESH_MANAGED, dx9Driver.VPORT1, NULL, NULL,NULL,NULL, &Wheels[0].phBody);
		remove("tmp.x");
	}
	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Wheels[0].BodyRot=V3(float_data1,float_data2,float_data3);

	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Wheels[0].BodyScal=V3(float_data1,float_data2,float_data3);

	Wheels[3]=Wheels[2]=Wheels[1]=Wheels[0];

	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Wheels[0].BodyPos=V3(float_data1,float_data2,float_data3);

	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Wheels[1].BodyPos=V3(float_data1,float_data2,float_data3);

	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Wheels[2].BodyPos=V3(float_data1,float_data2,float_data3);

	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Wheels[3].BodyPos=V3(float_data1,float_data2,float_data3);

	fread(&fsize,sizeof(int),1,FIN);
	if(fsize>0)
	{
		FOUT=fopen("tmp.md","wb");
		for(int i=0; i<fsize; i++) putc(getc(FIN),FOUT);
		if(FOUT!=NULL)fclose(FOUT);
		Cusov.LoadObject("tmp.md");
		remove("tmp.md");
	}

	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Cusov.Pos=V3(float_data1,float_data2,float_data3);
	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Cusov.Rot=V3(float_data1,float_data2,float_data3);
	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Cusov.Scal=V3(float_data1,float_data2,float_data3);

	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Tower.Pos=V3(float_data1,float_data2,float_data3);
	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Tower.Rot=V3(float_data1,float_data2,float_data3);
	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Tower.Scal=V3(float_data1,float_data2,float_data3);

	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Front.Pos=V3(float_data1,float_data2,float_data3);
	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Front.Rot=V3(float_data1,float_data2,float_data3);
	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	Front.Scal=V3(float_data1,float_data2,float_data3);

	for(int i=0;i<6;i++)
	{
		fread(&float_data1,sizeof(float),1,FIN);
		fread(&float_data2,sizeof(float),1,FIN);
		fread(&float_data3,sizeof(float),1,FIN);
		WAPS[i].Position=V3(float_data1,float_data2,float_data3);
		WAPS[i].UpdateMatrix();
	}
	WAPS[0].Position=V3(Tower.Pos.x,WAPS[0].Position.y,Tower.Pos.z);
	WAPS[0].UpdateMatrix();

	WAPS[6].Position=WAPS[4].Position+V3(50.0f,0.0f,0.0f);WAPS[6].UpdateMatrix();
	WAPS[7].Position=WAPS[5].Position+V3(50.0f,0.0f,0.0f);WAPS[7].UpdateMatrix();
	
	WAPS[8].Position=WAPS[2].Position+V3(-50.0f,0.0f,0.0f);WAPS[8].UpdateMatrix();
	WAPS[9].Position=WAPS[3].Position+V3(-50.0f,0.0f,0.0f);WAPS[9].UpdateMatrix();
	
	Cusov.MatrixUpdate();
	Tower.MatrixUpdate();
	Front.MatrixUpdate();
	Body.MatrixUpdate();
	Wheel1Foam=LoadParticleFromFile("System\\Particles\\test.lps");
	Wheel1Foam.Scale=0.07f;
	Wheel2Foam=LoadParticleFromFile("System\\Particles\\test.lps");
	Wheel2Foam.Scale=0.07f;
	Wheel1Foam.Color=V3(0.7f,0.7f,0.5f);
	Wheel2Foam.Color=V3(0.7f,0.7f,0.5f);
	CreatePhysics();
}


void Actor::Turn(DWORD dir)
{
	if(dir==1)
	{
		VehicleDirection-=3;
	}
	else if(dir==2)
	{
		VehicleDirection+=3;
	}else 
	{
		float m=VehicleDirection;
		if(abs(m)>2) m=m/abs(m)*2;
		VehicleDirection-=m;
	}
	
	VehicleDirection=max(VehicleDirection, -30);
	VehicleDirection=min(VehicleDirection, 30);
	for(int i=0; i<2; i++)
	{
		Joints[i]->SetBodyAnchor(0, VehicleBodyAnchor[i].Rotate(RotationPos1, RotationDir, VehicleDirection));
		Joints[i+2]->SetBodyAnchor(0, VehicleBodyAnchor[i+2].Rotate(RotationPos2, RotationDir, VehicleDirection));
	}

}

void Actor::Gas()
{
	pBodyVehicle->SetSleeping(false); // the wheel may not powerful enough at the start, so wake up the main body	
	pBodyWheel[0]->SetAngularVelocity(SPEVector(0, 0, 0));	
	pBodyWheel[1]->SetAngularVelocity(SPEVector(0, 0, 0));	
	pBodyWheel[0]->ApplyBodyTorque(WheelPin*-600);
	pBodyWheel[1]->ApplyBodyTorque(WheelPin*-600);
}

void Actor::Back()
{	
	pBodyVehicle->SetSleeping(false);	
	pBodyWheel[0]->SetAngularVelocity(SPEVector(0, 0, 0));	pBodyWheel[0]->ApplyBodyTorque(WheelPin*300);
	pBodyWheel[1]->SetAngularVelocity(SPEVector(0, 0, 0));	pBodyWheel[1]->ApplyBodyTorque(WheelPin*300);	
}
 

Actor p1;