#define V3 D3DXVECTOR3

class AutoCamera
{	
	V3 CameraTarget,Position,Intrp;
	vector<V3> Camera_AnimationPOS;
	int InterpolationCounter;	
	float TowerYaw,TowerPitch;
public:
	int CameraType;
	float angle;
	bool AlternativeCamActive;
	AutoCamera()
	{
		CameraType=0;
		AlternativeCamActive=false;
	}
	void CameraChange()
	{
		if(CameraType<2)CameraType++;else CameraType=0;
		Position=V3(p1.pBodyVehicle->GetPosition().x,p1.pBodyVehicle->GetPosition().y+1.0f,p1.pBodyVehicle->GetPosition().z);
	}
	void Update();
	void Redraw();
	void InterpolateCAM();

};
AutoCamera ACam;

void AutoCamera::InterpolateCAM()
{		
	if(InterpolationCounter<10)
	{		
		Camera_AnimationPOS[0]+=Intrp;
		InterpolationCounter++;
	}else 
	{
		Camera_AnimationPOS.erase(Camera_AnimationPOS.begin());
		Intrp.x=float((Camera_AnimationPOS[1].x-Camera_AnimationPOS[0].x)/10);
		Intrp.y=float((Camera_AnimationPOS[1].y-Camera_AnimationPOS[0].y)/10);
		Intrp.z=float((Camera_AnimationPOS[1].z-Camera_AnimationPOS[0].z)/10);		
		InterpolationCounter=0;
	}
}

void AutoCamera::Update()
{	
	switch(CameraType)
	{
		case 0:
		{
			CameraTarget=V3((p1.pBodyWheel[0]->GetPosition().x+p1.pBodyWheel[1]->GetPosition().x)/2,
							(p1.pBodyWheel[0]->GetPosition().y+p1.pBodyWheel[1]->GetPosition().y)/2+0.6f,
							(p1.pBodyWheel[0]->GetPosition().z+p1.pBodyWheel[1]->GetPosition().z)/2);

			Position=V3(p1.pBodyVehicle->GetPosition().x,p1.pBodyVehicle->GetPosition().y+0.55f,p1.pBodyVehicle->GetPosition().z);
			angle=p1.pBodyWheel[0]->GetPosition().y-p1.pBodyWheel[1]->GetPosition().y;
			break;
		}
				
		case 1:
		{
			CameraTarget=V3((p1.pBodyWheel[0]->GetPosition().x+p1.pBodyWheel[1]->GetPosition().x)/2,
							(p1.pBodyWheel[0]->GetPosition().y+p1.pBodyWheel[1]->GetPosition().y)/2,
							(p1.pBodyWheel[0]->GetPosition().z+p1.pBodyWheel[1]->GetPosition().z)/2);

			Position=V3(p1.pBodyVehicle->GetPosition().x,p1.pBodyVehicle->GetPosition().y,p1.pBodyVehicle->GetPosition().z);
			Position=V3(Position.x+(Position.x-CameraTarget.x)*3.0f,Position.y+2.0f,Position.z+(Position.z-CameraTarget.z)*3.0f);
			if(Camera_AnimationPOS.size()<5)Camera_AnimationPOS.push_back(Position);
			InterpolateCAM();
			break;
		}
		
		case 2:
		{
			CameraTarget=V3(p1.pBodyVehicle->GetPosition().x,p1.pBodyVehicle->GetPosition().y,p1.pBodyVehicle->GetPosition().z);
			angle=0.05f;
			break;
		}
	}
}

void AutoCamera::Redraw()
{
	D3DXVECTOR3 lValue = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	if(!AlternativeCamActive)
	if(CameraType==1)
	{
		if(Camera_AnimationPOS.size()>0)VP1Camera.lookAt(Camera_AnimationPOS[0],CameraTarget, lValue);
		VP1Camera.update(D3DXVECTOR3(0.0f,0.0f,0.0f),DEG_TO_RAD(0),DEG_TO_RAD(0),0.05f,0);
	}
	else
	{
		VP1Camera.lookAt(Position,CameraTarget, lValue);
		VP1Camera.update(D3DXVECTOR3(0.0f,0.0f,0.0f),DEG_TO_RAD(0),DEG_TO_RAD(0),0.05f,angle*4);
	}
}