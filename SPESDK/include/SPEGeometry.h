
#ifndef SPEGEOMETRY_H
#define SPEGEOMETRY_H

struct SPEPlane
{
	SPEVector	n;
	float		d;

	SPEPlane(){}
	SPEPlane(float x, float y, float z, float dis)
	{
		n=SPEVector(x, y, z);
		d=dis;
	}
	SPEPlane(const SPEVector& normal, float dis)
	{
		n=normal;
		d=dis;
	}
	SPEPlane(const SPEVector& normal, const SPEVector& point)
	{
		n=normal.Normal ();
		d=n*point;
	}
	SPEPlane(const SPEVector& v0, const SPEVector& v1, const SPEVector& v2)
	{
		FromTriangle(v0,v1,v2);
	}
	
	void FromTriangle(const SPEVector& v0, const SPEVector& v1, const SPEVector& v2)
	{
		SPEVector e0=(v1-v0);
		SPEVector e1=(v2-v0);
		n=e0^e1;
		n.Normalize();
		d=v0*n;
	}
};

struct SPEAabb
{
	SPEVector Min, Max;
};


#endif

