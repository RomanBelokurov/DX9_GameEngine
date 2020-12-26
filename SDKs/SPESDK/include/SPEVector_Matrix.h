
#ifndef SPEVECTOR_MATRIX_H
#define SPEVECTOR_MATRIX_H
////////////////////////////////////////////////////////////////////////////
//	SPEVector & SPEMatrix class for SPE
//		Copyright(c) spehome.com, All Rights Reserved.
//
//
///////////////////////////////////////////////////////////////////////////
#include "memory.h"
#include "SPELib.h"

struct SPEVector3;
struct SPEVector;
struct SPEMatrix;
SPE_API inline SPEVector operator*(float, const SPEVector &v);

struct SPE_API SPEVector2
{
	float x, y;
	SPEVector2(){}
	SPEVector2(float _x, float _y):x(_x), y(_y){}
	SPEVector2 operator+(const SPEVector2 &v) { return SPEVector2(x+v.x, y+v.y); }
	SPEVector2 operator-(const SPEVector2 &v) { return SPEVector2(x-v.x, y-v.y); }
	SPEVector2 operator*(float s){ return SPEVector2(x*s,y*s);}
	SPEVector2& Read(void *pfloat)
	{
		memcpy(this, pfloat, sizeof(float)*2);
		return *this;
	}
};
struct SPE_API SPEVector3
{
	float x, y, z;
	SPEVector3(){}	
	SPEVector3(float _x, float _y, float _z):x(_x), y(_y), z(_z){}
	
	SPEVector3 operator+(const SPEVector3 &v) const { return SPEVector3(x+v.x, y+v.y, z+v.z); }	
	SPEVector3& operator+=(const SPEVector3 &v)
	{
		x+=v.x;
		y+=v.y;
		z+=v.z;
		return *this;
	}
	SPEVector3 operator-(const SPEVector3 &v) const { return SPEVector3(x-v.x,y-v.y,z-v.z);}
	SPEVector3 operator-(){ return SPEVector3(-x,-y,-z);	}
	SPEVector3& operator-=(const SPEVector3 &v)
	{
		x-=v.x;
		y-=v.y;
		z-=v.z;
		return *this;
	}
	float operator*(const SPEVector3 &v) const {return (x*v.x+y*v.y+z*v.z);}  //   dot
	SPEVector3 operator*(float s){ return SPEVector3(x*s,y*s,z*s);}
	SPEVector3 operator/(float s){ 	float invs=1.0f/s; return SPEVector3(x*invs,y*invs,z*invs);}
	SPEVector3& operator*=(float s)
	{
		x*=s;
		y*=s;
		z*=s;
		return *this;
	}
	inline SPEVector3 operator*(const SPEMatrix &m) const;
	SPEVector3& operator/=(float s)
	{
		s=1/s;
		x*=s;
		y*=s;
		z*=s;
		return *this;
	}
	SPEVector3 operator^(const SPEVector3 &v) const 
	{
		return SPEVector3(y*v.z - z*v.y,
			z*v.x - x*v.z ,
			x*v.y - y*v.x );
	}

	SPEVector3& Normalize(float unit=1.0f);

	SPEVector3& Read(void *pfloat)
	{
		memcpy(this, pfloat, sizeof(float)*3);
		return *this;
	}
};

// SPEVector is the class used in SPE core, NOT SPEVector3
struct SPE_API SPEVector
{
	float x,y,z;
	union{float t; float w; unsigned int c;};

	SPEVector(){}
	SPEVector(float _x, float _y, float _z, float _t=0.0f):x(_x), y(_y), z(_z), t(_t){}
	~SPEVector(){}

	SPEVector operator+(const SPEVector &v) const { return SPEVector(x+v.x, y+v.y, z+v.z); }	
	SPEVector& operator+=(const SPEVector &v)
	{
		x+=v.x;
		y+=v.y;
		z+=v.z;
		return *this;
	}
	SPEVector operator-(const SPEVector &v) const { return SPEVector(x-v.x,y-v.y,z-v.z);}
	SPEVector operator-(){ return SPEVector(-x,-y,-z);	}
	SPEVector& operator-=(const SPEVector &v)
	{
		x-=v.x;
		y-=v.y;
		z-=v.z;
		return *this;
	}
	
	float operator*(const SPEVector &v) const {return (x*v.x+y*v.y+z*v.z);}  //   dot
	SPEVector operator*(float s){ return SPEVector(x*s,y*s,z*s);}
	SPEVector operator/(float s){ 	float invs=1.0f/s; return SPEVector(x*invs,y*invs,z*invs);}
	SPEVector& operator*=(float s)
	{
		x*=s;
		y*=s;
		z*=s;
		return *this;
	}

	//friend inline SPEVector operator *(const SPEVector &v, const SPEMatrix &mat);
	inline SPEVector operator*(const SPEMatrix &m) const;

	SPEVector& operator/=(float s)
	{
		s=1/s;
		x*=s;
		y*=s;
		z*=s;
		return *this;
	}
	
	//	cross
	SPEVector operator^(const SPEVector &v) const 
	{
		return SPEVector(y*v.z - z*v.y,
			z*v.x - x*v.z ,
			x*v.y - y*v.x );
	}

	float& operator[](int i)
	{
		if(i<0 || i>3)
			return (&x)[0];		
		return (&x)[i];
	}

	SPEVector& Read(void *pfloat)
	{
		memcpy(this, pfloat, sizeof(float)*3);
		return *this;
	}

	void Write(void *pfloat)
	{
		memcpy(pfloat, this, sizeof(float)*3);
	}

	SPEVector& Normalize(float unit=1.0f);
	SPEVector Normal() const
	{
		SPEVector n=*this;
		n.Normalize ();
		return n;
	}

	float Length()const;
	float Length2()const{ return (x*x+y*y+z*z); }

	SPEVector Rotate(const SPEVector &RayPos, const SPEVector &RayDir, float Degree);
	SPEVector Rotate(const SPEVector &Dir, float Degree);
	
	SPEVector GetVertical()const ;

	bool operator==(const SPEVector &v) const 
	{
		return x==v.x && y==v.y && z==v.z;
	}
	bool equal(const SPEVector &v, float delta=0.00001f) const 
	{
		return (*this-v).Length2()<delta;
	}

};


SPE_API inline SPEVector operator*(float s, const SPEVector &v)
{
	return SPEVector(s*v.x,s*v.y,s*v.z);
}


// all static function names contain a "m" prefix, means return a new "matrix"
struct SPE_API SPEMatrix
{
	SPEVector v[4];

	SPEMatrix(bool bIdentity=true){ if(bIdentity)Identity(); }
	SPEMatrix& Read(void *pf)
	{
		memcpy((void*)v, pf, sizeof(SPEMatrix));
		return *this;
	}
	SPEMatrix& Identity()
	{
		v[0]=SPEVector(1,0,0,0);
		v[1]=SPEVector(0,1,0,0);
		v[2]=SPEVector(0,0,1,0);
		v[3]=SPEVector(0,0,0,1);
		return *this;
	}
	SPEMatrix& Translate(const SPEVector &pos)
	{
		Identity();
		v[3]=pos;
		v[3].w=1.0f;
		return *this;
	}
	SPEMatrix& Translate(float x, float y, float z)
	{
		SPEVector v(x, y, z);
		return Translate(v);
	}
	static SPEMatrix mTranslate(const SPEVector& pos)
	{
		SPEMatrix mat;
		mat.Translate (pos);
		return mat;
	}
	static SPEMatrix mTranslate(float x, float y, float z)
	{
		SPEVector v(x, y, z);
		return mTranslate(v);
	}
	SPEMatrix& Rotate(const SPEVector axis, float radian);
	SPEMatrix& Rotate(float x, float y, float z, float radian)
	{
		return Rotate(SPEVector(x, y, z), radian);
	}

	static SPEMatrix mRotate(const SPEVector axis, float radian)
	{
		SPEMatrix mat;
		mat.Rotate(axis, radian);
		return mat;
	}
	static SPEMatrix mRotate(float x, float y, float z, float radian)
	{
		SPEMatrix mat;
		mat.Rotate(SPEVector(x, y, z), radian);
		return mat;
	}
	static SPEMatrix mAnglesZYX(float z, float y, float x)
	{
		float d2r = 0.017453f; //     PI/180
		SPEMatrix mat;
		z=z*d2r;
		y=y*d2r;
		x=x*d2r;
		mat=SPEMatrix::mRotate (SPEVector(0,0,1),z);
		mat=SPEMatrix::mRotate (SPEVector(0,1,0),y) * mat;
		mat=SPEMatrix::mRotate (SPEVector(1,0,0),x) * mat;
		return mat;
	}

	static SPEMatrix mZDirect2(const SPEVector &zdir);// generate an orientation matrix to make zaxis direct to zdir

	SPEMatrix& Scale(const SPEVector &s)
	{
		v[0]=SPEVector(s.x,0,0,0);
		v[1]=SPEVector(0,s.y,0,0);
		v[2]=SPEVector(0,0,s.z,0);
		v[3]=SPEVector(0,0,0,1);
		return *this;
	}
	SPEMatrix& Scale(float x, float y, float z)
	{
		return Scale(SPEVector(x, y, z));
	}
	SPEMatrix& Scale(float s)
	{
		return Scale(SPEVector(s, s, s));
	}
	static SPEMatrix mScale(float x, float y, float z)
	{
		SPEMatrix mat;
		mat.Scale (x, y, z);
		return mat;
	}
	static SPEMatrix mScale(float s)
	{
		SPEMatrix mat;
		mat.Scale (s, s, s);
		return mat;
	}
	SPEMatrix& Transpose()
	{
		SPEMatrix mat;
		mat.v[0]=SPEVector(v[0].x, v[1].x, v[2].x, v[3].x);
		mat.v[1]=SPEVector(v[0].y, v[1].y, v[2].y, v[3].y);
		mat.v[2]=SPEVector(v[0].z, v[1].z, v[2].z, v[3].z);
		mat.v[3]=SPEVector(v[0].w, v[1].w, v[2].w, v[3].w);
		*this=mat;
		return *this;
	}

	SPEMatrix mTranspose()
	{
		SPEMatrix mat;
		mat.v[0]=SPEVector(v[0].x, v[1].x, v[2].x, v[3].x);
		mat.v[1]=SPEVector(v[0].y, v[1].y, v[2].y, v[3].y);
		mat.v[2]=SPEVector(v[0].z, v[1].z, v[2].z, v[3].z);
		mat.v[3]=SPEVector(v[0].w, v[1].w, v[2].w, v[3].w);
		return mat;
	}

	SPEMatrix& Inverse(); 
	SPEMatrix mInverse()
	{
		SPEMatrix mat=*this;
		mat.Inverse ();
		return mat;
	}
	SPEMatrix eInverse();  // exact inverse matrix

	SPEMatrix operator*(const SPEMatrix &mat);

	SPEMatrix& operator*=(const SPEMatrix &mat)
	{
		*this=*this*mat;
		return *this;
	}

	SPEVector Transform(const SPEVector& p)
	{
		return *this*p;
	}

	SPEVector operator*(const SPEVector& p)
	{
		SPEVector vOut;		
		//vOut=(x*mat.v[0]+y*mat.v[1]+z*mat.v[2]+mat.v[3]);
		vOut.x=p.x*v[0].x+p.y*v[1].x+p.z*v[2].x+v[3].x;
		vOut.y=p.x*v[0].y+p.y*v[1].y+p.z*v[2].y+v[3].y;
		vOut.z=p.x*v[0].z+p.y*v[1].z+p.z*v[2].z+v[3].z;
		float w=p.x*v[0].w+p.y*v[1].w+p.z*v[2].w+v[3].w;
		vOut/=w;
		return vOut;
	}

	SPEMatrix operator^(float m)  // multiply scalar while used as 3x3 Matrix
	{
		SPEMatrix mat;
		int i,j;
		for(i=0; i<3; i++)
		{
			for(j=0; j<3; j++)
			{
				mat.v[i][j]=v[i][j]*m;
			}
		}
		return mat;
	}

	SPEVector GetRotation();
};

SPE_API inline SPEVector SPEVector::operator *(const SPEMatrix &mat) const 
{
	SPEVector vOut;
	vOut.x=x*mat.v[0].x+y*mat.v[1].x+z*mat.v[2].x+mat.v[3].x;
	vOut.y=x*mat.v[0].y+y*mat.v[1].y+z*mat.v[2].y+mat.v[3].y;
	vOut.z=x*mat.v[0].z+y*mat.v[1].z+z*mat.v[2].z+mat.v[3].z;
	float w=x*mat.v[0].w+y*mat.v[1].w+z*mat.v[2].w+mat.v[3].w;
	vOut/=w;
	return vOut;
}

SPE_API inline SPEVector3 SPEVector3::operator *(const SPEMatrix &mat) const 
{
	SPEVector3 vOut;
	vOut.x=x*mat.v[0].x+y*mat.v[1].x+z*mat.v[2].x+mat.v[3].x;
	vOut.y=x*mat.v[0].y+y*mat.v[1].y+z*mat.v[2].y+mat.v[3].y;
	vOut.z=x*mat.v[0].z+y*mat.v[1].z+z*mat.v[2].z+mat.v[3].z;
	float w=x*mat.v[0].w+y*mat.v[1].w+z*mat.v[2].w+mat.v[3].w;
	vOut/=w;
	return vOut;
}


#endif

