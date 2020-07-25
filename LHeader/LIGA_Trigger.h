// Trigger definitions


class Trigger
{
public:
	string TriggerTitle;
	string SceneFileName;	
	DWORD Type;
public:
	AABB TriggerBox;
	Trigger(V3 Pos){TriggerBox.minPt=Pos-V3(2,2,2);TriggerBox.maxPt=Pos+V3(2,2,2);}
	void Draw(){DrawBB(VPORT1,TriggerBox.minPt,TriggerBox.maxPt,D3DCOLOR_XRGB(255,0,0));}
	void Action();			
};

vector<Trigger> TRIGGER_VECTOR;
