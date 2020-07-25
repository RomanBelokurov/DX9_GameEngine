#ifndef __LGS_CAMERA__H__
#define __LGS_CAMERA__H__
struct FrustumPlane
{
     D3DXVECTOR3 normal;
     float distance;
	 inline void Normalize()
     {
		  float tmp =1/sqrt((normal.x*normal.x)+normal.y*normal.y) + (normal.z*normal.z);
		  normal.x = normal.x * tmp;
		  normal.y = normal.y * tmp;
          normal.z = normal.z * tmp;
          distance = distance * tmp;
     }
     inline float DistanceToPoint(D3DXVECTOR3 *pnt)
     {
          return D3DXVec3Dot(&normal, pnt) + distance;
     }
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class DXCamera
{
public:
	DXCamera();
	LPDIRECT3DDEVICE9 		g_pd3dDevice;

	const D3DXMATRIX& view() const;
	const D3DXMATRIX& proj() const;
	const D3DXMATRIX& viewProj() const;

	const D3DXVECTOR3& right() const;
	const D3DXVECTOR3& up() const;
	const D3DXVECTOR3& look() const;

	D3DXVECTOR3& pos();

	void lookAt(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up);
	void setLens(LPDIRECT3DDEVICE9 pDev,float fov, float aspect, float nearZ, float farZ);
	void setSpeed(float s);

	// Box coordinates should be relative to world space.
	bool isVisible(D3DXVECTOR3 min,D3DXVECTOR3 max);
	void update(D3DXVECTOR3 dir,float yAngle,float pitch,float dt, float roll);
	void getWorldPickingRay(HWND hWnd,D3DXVECTOR3& originW, D3DXVECTOR3& dirW);

protected:
	void buildView();
	void buildWorldFrustumPlanes();

public:
	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mViewProj;
	D3DXMATRIX mRoll;

	// Relative to world space.
	D3DXVECTOR3 mPosW;
	D3DXVECTOR3 mRightW;
	D3DXVECTOR3 mUpW;
	D3DXVECTOR3 mLookW;

	float mSpeed;
	FrustumPlane frustumPlanes[6];
};

DXCamera VP1Camera;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void DXCamera::getWorldPickingRay(HWND hWnd,D3DXVECTOR3& originW, D3DXVECTOR3& dirW)
{
	POINT s;
	GetCursorPos(&s);
	ScreenToClient(hWnd, &s);
	float w = 1024.0f;
	float h = 768.0f;
	float x = (2.0f*s.x/w - 1.0f) / mProj(0,0);
	float y = (-2.0f*s.y/h + 1.0f) / mProj(1,1);
	D3DXVECTOR3 origin(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 dir(x, y, 1.0f);
	D3DXMATRIX invView;
	D3DXMatrixInverse(&invView, 0, &mView);
	D3DXVec3TransformCoord(&originW, &origin, &invView);
	D3DXVec3TransformNormal(&dirW, &dir, &invView);
	D3DXVec3Normalize(&dirW, &dirW);
}


bool DXCamera::isVisible(D3DXVECTOR3 min,D3DXVECTOR3 max)
{
	int p;
	for ( p = 0; p < 6; p++ )
	{
		if (frustumPlanes[p].normal.x*(min.x) + frustumPlanes[p].normal.y*(min.y) +  frustumPlanes[p].normal.z*(min.z) + frustumPlanes[p].distance>0) continue;
		if (frustumPlanes[p].normal.x*(max.x) + frustumPlanes[p].normal.y*(min.y) +  frustumPlanes[p].normal.z*(min.z) + frustumPlanes[p].distance>0) continue;
		if (frustumPlanes[p].normal.x*(min.x) + frustumPlanes[p].normal.y*(max.y) +  frustumPlanes[p].normal.z*(min.z) + frustumPlanes[p].distance>0) continue;
		if (frustumPlanes[p].normal.x*(max.x) + frustumPlanes[p].normal.y*(max.y) +  frustumPlanes[p].normal.z*(min.z) + frustumPlanes[p].distance>0) continue;
		if (frustumPlanes[p].normal.x*(min.x) + frustumPlanes[p].normal.y*(min.y) +  frustumPlanes[p].normal.z*(max.z) + frustumPlanes[p].distance>0) continue;
		if (frustumPlanes[p].normal.x*(max.x) + frustumPlanes[p].normal.y*(min.y) +  frustumPlanes[p].normal.z*(max.z) + frustumPlanes[p].distance>0) continue;
		if (frustumPlanes[p].normal.x*(min.x) + frustumPlanes[p].normal.y*(max.y) +  frustumPlanes[p].normal.z*(max.z) + frustumPlanes[p].distance>0) continue;
		if (frustumPlanes[p].normal.x*(max.x) + frustumPlanes[p].normal.y*(max.y) +  frustumPlanes[p].normal.z*(max.z) + frustumPlanes[p].distance>0) continue;
		return false;
	}
	return true;
}

void DXCamera::buildWorldFrustumPlanes()
{
		D3DXMATRIX Matrix, _invViewMatrix, MatrixView, MatrixProjection;
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &MatrixProjection);
        g_pd3dDevice->GetTransform(D3DTS_VIEW, &MatrixView);
        D3DXMatrixInverse(&_invViewMatrix, 0, &MatrixView);
        D3DXMATRIXA16 matComb;
        D3DXMatrixMultiply(&matComb, &MatrixView, &MatrixProjection);

        frustumPlanes[0].normal.x = matComb._14 + matComb._11;
        frustumPlanes[0].normal.y = matComb._24 + matComb._21;
        frustumPlanes[0].normal.z = matComb._34 + matComb._31;
        frustumPlanes[0].distance = matComb._44 + matComb._41;
		frustumPlanes[1].normal.x = matComb._14 - matComb._11;
        frustumPlanes[1].normal.y = matComb._24 - matComb._21;
        frustumPlanes[1].normal.z = matComb._34 - matComb._31;
        frustumPlanes[1].distance = matComb._44 - matComb._41;
        frustumPlanes[2].normal.x = matComb._14 - matComb._12;
        frustumPlanes[2].normal.y = matComb._24 - matComb._22;
        frustumPlanes[2].normal.z = matComb._34 - matComb._32;
        frustumPlanes[2].distance = matComb._44 - matComb._42;
        frustumPlanes[3].normal.x = matComb._14 + matComb._12;
        frustumPlanes[3].normal.y = matComb._24 + matComb._22;
        frustumPlanes[3].normal.z = matComb._34 + matComb._32;
        frustumPlanes[3].distance = matComb._44 + matComb._42;
        frustumPlanes[4].normal.x = matComb._13;
        frustumPlanes[4].normal.y = matComb._23;
		frustumPlanes[4].normal.z = matComb._33;
        frustumPlanes[4].distance = matComb._43;
        frustumPlanes[5].normal.x = matComb._14 - matComb._13;
        frustumPlanes[5].normal.y = matComb._24 - matComb._23;
        frustumPlanes[5].normal.z = matComb._34 - matComb._33;
        frustumPlanes[5].distance = matComb._44 - matComb._43;
}


DXCamera::DXCamera()
{
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mViewProj);

	mPosW   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mRightW = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	mUpW    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	mLookW  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	mSpeed  = 50.0f;
}

const D3DXMATRIX& DXCamera::view() const
{
	return mView;
}

const D3DXMATRIX& DXCamera::proj() const
{
	return mProj;
}

const D3DXMATRIX& DXCamera::viewProj() const
{
	return mViewProj;
}

const D3DXVECTOR3& DXCamera::right() const
{
	return mRightW;
}

const D3DXVECTOR3& DXCamera::up() const
{
	return mUpW;
}

const D3DXVECTOR3& DXCamera::look() const
{
	return mLookW;
}

D3DXVECTOR3& DXCamera::pos()
{
	return mPosW;
}

void DXCamera::lookAt(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up)
{
	D3DXVECTOR3 L = target - pos;
	D3DXVec3Normalize(&L, &L);

	D3DXVECTOR3 R;
	D3DXVec3Cross(&R, &up, &L);
	D3DXVec3Normalize(&R, &R);

	D3DXVECTOR3 U;
	D3DXVec3Cross(&U, &L, &R);
	D3DXVec3Normalize(&U, &U);

	mPosW   = pos;
	mRightW = R;
	mUpW    = U;
	mLookW  = L;

	buildView();
	buildWorldFrustumPlanes();

	mViewProj = mView * mProj;
}

void DXCamera::setLens(LPDIRECT3DDEVICE9 pDev,float fov, float aspect, float nearZ, float farZ)
{
	g_pd3dDevice=pDev;
	D3DXMatrixPerspectiveFovLH(&mProj, DEG_TO_RAD(fov), aspect, nearZ, farZ);
	buildWorldFrustumPlanes();
	mViewProj = mView * mProj;
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &mProj);
}

void DXCamera::setSpeed(float s)
{
	mSpeed = s;
}

void DXCamera::update(D3DXVECTOR3 dir,float yAngle,float pitch,float dt, float roll)
{
	D3DXVec3Normalize(&dir, &dir);
	D3DXVECTOR3 newPos = mPosW + dir*mSpeed*dt;

	mPosW = newPos;

	// Rotate DXCamera's look and up vectors around the DXCamera's right vector.
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &mRightW, pitch);
	D3DXVec3TransformCoord(&mLookW, &mLookW, &R);
	D3DXVec3TransformCoord(&mUpW, &mUpW, &R);

	// Rotate DXCamera axes about the world's y-axis.
	D3DXMatrixRotationY(&R, yAngle);
	D3DXVec3TransformCoord(&mRightW, &mRightW, &R);
	D3DXVec3TransformCoord(&mUpW, &mUpW, &R);
	D3DXVec3TransformCoord(&mLookW, &mLookW, &R);

	
	D3DXMatrixRotationAxis(&mRoll, &mLookW, roll);
	D3DXVec3TransformCoord(&mRightW, &mRightW, &mRoll);
	D3DXVec3TransformCoord(&mUpW, &mUpW, &mRoll);

	// Rebuild the view matrix to reflect changes.
	buildView();
	buildWorldFrustumPlanes();

	mViewProj = mView * mProj;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &mView);
}

void DXCamera::buildView()
{
	// Keep DXCamera's axes orthogonal to each other and of unit length.
	D3DXVec3Normalize(&mLookW, &mLookW);

	D3DXVec3Cross(&mUpW, &mLookW, &mRightW);
	D3DXVec3Normalize(&mUpW, &mUpW);

	D3DXVec3Cross(&mRightW, &mUpW, &mLookW);
	D3DXVec3Normalize(&mRightW, &mRightW);

	// Fill in the view matrix entries.

	float x = -D3DXVec3Dot(&mPosW, &mRightW);
	float y = -D3DXVec3Dot(&mPosW, &mUpW);
	float z = -D3DXVec3Dot(&mPosW, &mLookW);

	mView(0,0) = mRightW.x;
	mView(1,0) = mRightW.y;
	mView(2,0) = mRightW.z;
	mView(3,0) = x;

	mView(0,1) = mUpW.x;
	mView(1,1) = mUpW.y;
	mView(2,1) = mUpW.z;
	mView(3,1) = y;

	mView(0,2) = mLookW.x;
	mView(1,2) = mLookW.y;
	mView(2,2) = mLookW.z;
	mView(3,2) = z;

	mView(0,3) = 0.0f;
	mView(1,3) = 0.0f;
	mView(2,3) = 0.0f;
	mView(3,3) = 1.0f;
}
#endif  __LGS_CAMERA__H__


