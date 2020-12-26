
#ifndef SPEMESH_H
#define SPEMESH_H

struct SPEVertex
{
	SPEVector3 p;
	SPEVector3 n;
	SPEVector2 t;
};
struct SPETriangle
{
	SPEVertex v[3];
	void Init(SPEVertex &v0, SPEVertex &v1, SPEVertex &v2)
	{
		v[0]=v0;
		v[1]=v1;
		v[2]=v2;		
	}
};
struct AttributeItem
{
	int Id;
	int Start;
	int Num;
	AttributeItem()
	{
		Id=-1;
		Start=-1;
		Num=0;
	}
};

struct SPEMesh
{
	SPEArray<SPETriangle>	Triangle;
	SPEArray<int>				Attribute;
	SPEArray<AttributeItem> Table;

	void Clear()
	{
		Triangle.clear ();
		Attribute.clear ();
		Table.clear ();
	}
	SPERESULT Initialize(BYTE *pPosition, int PositionStride, BYTE *pTexture, int TextureStride, int *pIndex, int NumTriangles)
	{
		if(pPosition == 0 || pIndex == 0) return SPE_FAILED;
		Clear();		
		SPETriangle tri;
		for(int i=0; i<NumTriangles; i++)
		{
			for(int j=0; j<3; j++)
			{
				tri.v[j].p.Read (&pPosition[pIndex[i*3+j]*PositionStride]);
				if(pTexture) tri.v[j].t.Read (&pTexture[pIndex[i*3+j]*TextureStride]);
				else tri.v[j].t = SPEVector2(0.0f, 0.0f);
			}
			SPEVector3 e0=tri.v[1].p-tri.v[0].p;
			SPEVector3 e1=tri.v[2].p-tri.v[0].p;
			SPEVector3 n=e0^e1;
			n.Normalize ();
			for(int j=0; j<3; j++)
				tri.v[j].n=n;

			Triangle.push(tri);
		}

		for(int i=0; i<Triangle.size; i++)
		{
			Attribute.push(0);
		}
		
		AttributeItem att;
		att.Id = 0;
		att.Start = 0;
		att.Num = Triangle.size;
		Table.push(att);

		return SPE_OK;
	}
	void* GetData(int AttribId)
	{
		if(Table.size==0) return 0;
		for(int i=0; i<Table.size; i++)
		{
			if(Table[i].Id == AttribId)
				return &Triangle[Table[i].Start];
		}
		return 0;
	}
	int GetNum(int AttribId)
	{
		if(Table.size==0) return -1;
		for(int i=0; i<Table.size; i++)
		{
			if(Table[i].Id == AttribId)
				return Table[i].Num;
		}
		return -1;
	}
	

};

#endif

