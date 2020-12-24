#define DEG_TO_RAD(ang) ((ang)*D3DX_PI/180)		//макрос конверта градус-радиан
#define RAD_TO_DEG(ang) ((ang)*180/D3DX_PI)		//макрос конверта радиан-градус


struct AABB
{
	D3DXVECTOR3 center()const
	{
		return (minPt+maxPt)*0.5f;
	}

	D3DXVECTOR3 extent()const
	{
		return (maxPt-minPt)*0.5f;
	}

	void xform(const D3DXMATRIX& M, AABB& out)
	{
		// Convert to center/extent representation.
		D3DXVECTOR3 c = center();
		D3DXVECTOR3 e = extent();

		// Transform center in usual way.
		D3DXVec3TransformCoord(&c, &c, &M);

		// Transform extent.
		D3DXMATRIX absM;
		D3DXMatrixIdentity(&absM);
		absM(0,0) = fabs(M(0,0)); absM(0,1) = fabs(M(0,1)); absM(0,2) = fabs(M(0,2));
		absM(1,0) = fabs(M(1,0)); absM(1,1) = fabs(M(1,1)); absM(1,2) = fabs(M(1,2));
		absM(2,0) = fabs(M(2,0)); absM(2,1) = fabs(M(2,1)); absM(2,2) = fabs(M(2,2));
		D3DXVec3TransformNormal(&e, &e, &absM);

		// Convert back to AABB representation.
		out.minPt = c - e;
		out.maxPt = c + e;
	}
	
	bool PointInBox(D3DXVECTOR3 *pos)
	{
		if(minPt.x <= pos->x && maxPt.x >= pos->x)
		{
			if(minPt.z <= pos->z && maxPt.z >= pos->z)
			{
				if(minPt.y <= pos->y && maxPt.y >= pos->y)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		return false;
	}

	bool PointInColon(D3DXVECTOR3 *pos)
	{
		if(minPt.x <= pos->x && maxPt.x >= pos->x)
		{
			if(minPt.z <= pos->z && maxPt.z >= pos->z)
			{
				return true;
			}else return false;
		}
		return false;
	}

	float GetDiagonal()
	{
		return (maxPt.x-minPt.x)+(maxPt.y-minPt.y)+(maxPt.z-minPt.z);
	}

	D3DXVECTOR3 minPt;
	D3DXVECTOR3 maxPt;
};


D3DXVECTOR3 SphereTrans(D3DXVECTOR3 RotationPoint, D3DXVECTOR3 Point, D3DXVECTOR3 Angle)
{
	D3DXVECTOR3 Vect;
	D3DXMATRIX m1, m2, m3, m;
	Vect = Point - RotationPoint;

	D3DXMatrixRotationX(&m1, D3DXToRadian(Angle.x));
	D3DXMatrixRotationY(&m2, D3DXToRadian(Angle.y));
	D3DXMatrixRotationZ(&m3, D3DXToRadian(Angle.z));
	m = m3 * m1 * m2;
	D3DXVec3TransformCoord(&Vect, &Vect, &m);
	Point = Vect + RotationPoint;
	return Point;
} 