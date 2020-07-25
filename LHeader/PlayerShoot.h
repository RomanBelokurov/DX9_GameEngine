#define FRONT_WEAPON 0
#define TOWER_WEAPON 1

#define BULET12MM 0
#define TRANCHEN 1
#define LASER 3



class PlayerShoot
{
public:
	V3 FS1,TOS1;
	V3 Contact1;
	V3 Vector1,VisVector;

	LiPartSys Explosion;
	LiPartSys SimpleBulet;
	LiPartSys ShootTrace;
	LiPartSys FireHole;

	int IterNum;
	int ExploTime;
	int ReloadTimer;
	bool NowShoot;
	V3 Weapon1Pos;
public:
	
	void InitParticles()
	{		
		Explosion=LoadParticleFromFile("System\\Particles\\test1.lps");
		Explosion.Scale=0.5f;
		
		ShootTrace=LoadParticleFromFile("System\\Particles\\test2.lps");
		ShootTrace.Scale=0.07f;
		
		SimpleBulet=LoadParticleFromFile("System\\Particles\\Bullet.lps");
		SimpleBulet.Scale=0.03f;
		
		FireHole=LoadParticleFromFile("System\\Particles\\FireHole.lps");
		FireHole.Scale=0.05f;
	}
	
	void Shoot(DWORD Type,DWORD WeaponType)
	{
		D3DXMATRIX matWorld,mat;
		p1.Bodies[0]->GetTransformMesh (&matWorld);
		if(ReloadTimer<=0)
		switch(Type)
		{			
			case(FRONT_WEAPON):
			{
				mat=p1.WAPS[4].World*matWorld;
				FS1=V3(mat._41,mat._42,mat._43);
				mat=p1.WAPS[6].World*matWorld;
				TOS1=V3(mat._41,mat._42,mat._43);
				Vector1=(TOS1-FS1)*0.05f;
				
				SPECastRayInfo Info;
				pWorld->CastRay(SPEVector(FS1.x,FS1.y,FS1.z),SPEVector(Vector1.x,Vector1.y,Vector1.z),Info);
				Contact1=V3(Info.Position.x,Info.Position.y,Info.Position.z);
				VisVector=(Contact1-FS1)/(int)Info.Distance;
				IterNum=(int)Info.Distance;
				ExploTime=20;
				ReloadTimer=100;
				
				Explosion.Position=Contact1/0.5f;
				Explosion.ResetSystem();
				if(WeaponType>0)p1.pBodyVehicle->SetVelocity(-SPEVector(Vector1.x,Vector1.y,Vector1.z));
				
				Weapon1Pos=p1.Front.Pos;
				p1.Front.Pos.x=getRandomMinMax(p1.Front.Pos.x-0.05f,p1.Front.Pos.x+0.05f);
				p1.Front.MatrixUpdate();
				p1.Front.Pos=Weapon1Pos;
				NowShoot=true;
				break;
			}
			
			case(TOWER_WEAPON):
			{				
				mat=p1.WAPS[2].World*matWorld;
				FS1=V3(mat._41,mat._42,mat._43);
				mat=p1.WAPS[0].World*matWorld;
				V3 BackAimPos=V3(mat._41,mat._42,mat._43);				
				FS1=SphereTrans(BackAimPos,FS1,V3(0,p1.Tower.Rot.y,0));
				mat=p1.WAPS[8].World*matWorld;
				TOS1=V3(mat._41,mat._42,mat._43);
				TOS1=SphereTrans(BackAimPos,TOS1,V3(0,p1.Tower.Rot.y,0));
				Vector1=(TOS1-FS1)*0.05f;

				SPECastRayInfo Info;
				pWorld->CastRay(SPEVector(FS1.x,FS1.y,FS1.z),SPEVector(Vector1.x,Vector1.y,Vector1.z),Info);
				Contact1=V3(Info.Position.x,Info.Position.y,Info.Position.z);
				VisVector=(Contact1-FS1)/(int)Info.Distance;
				IterNum=(int)Info.Distance;
				ExploTime=20;
				ReloadTimer=100;
				
				Explosion.Position=Contact1/0.5f;
				Explosion.ResetSystem();
				if(WeaponType>0)p1.pBodyVehicle->SetVelocity(-SPEVector(Vector1.x,Vector1.y,Vector1.z));
				
				Weapon1Pos=p1.Tower.Pos;
				p1.Tower.Pos.x=getRandomMinMax(p1.Tower.Pos.x-0.05f,p1.Tower.Pos.x+0.05f);
				p1.Tower.MatrixUpdate();
				p1.Tower.Pos=Weapon1Pos;
				NowShoot=true;
				break;
			}
		}	
	}
	

	void Draw(int WeaponSort)
	{
		ReloadTimer--;		
		switch(WeaponSort)
		{
			case(LASER):
			{
				DrawLine(VPORT1,FS1,TOS1,D3DCOLOR_XRGB(255,255,255));
				break;
			}
			case(BULET12MM):
			{
				if(NowShoot)
				{
					FireHole.Color=V3(0.1f,0.1f,0.1f);
					FireHole.Scale=0.03f;
					FireHole.Position=Contact1/0.03f;
					FireHole.UpdateSystem();
					FireHole.RenderSystem();

					SimpleBulet.Color=V3(0.0f,0.0f,0.0f);
					SimpleBulet.Scale=0.005f;
					SimpleBulet.Position=FS1/0.005f;
					SimpleBulet.UpdateSystem();
					SimpleBulet.RenderSystem();
					ReloadTimer=0;
					NowShoot=false;
				}
				break;
			}
			case(TRANCHEN):
			{
				if(IterNum>0)
				{
					FS1+=VisVector;
					ShootTrace.Color=V3(1,1,1);
					ShootTrace.Scale=0.07f;
					ShootTrace.Position=FS1/0.07f;
					ShootTrace.UpdateSystem();
					ShootTrace.RenderSystem();
					IterNum--;
				}else
				if(ExploTime>0)
				{
					Explosion.Color=V3(1.0f,0.8f,0.5f);				
					Explosion.UpdateSystem();
					Explosion.RenderSystem();
					ExploTime--;
				}
				break;
			}
		}
		
	}	
};

DWORD PLAYER_FRONT_WEAPON;
PlayerShoot ShootVisuals;