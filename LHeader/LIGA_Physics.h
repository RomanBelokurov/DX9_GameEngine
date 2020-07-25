#pragma comment(lib,"spe.lib")
#include "SPE.h" 
#define DXFVF D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1

LPSPEWORLD				 pWorld;

struct DXVERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	D3DXVECTOR2 t;
};


void InitShape(ISPEShape* pShape, ID3DXMesh* pMesh)
{
    BYTE *pVB;
    void *pIB;
    pMesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, (void**)&pVB);
    pMesh->LockIndexBuffer (D3DLOCK_NOSYSLOCK, (void**)&pIB);
    if(pMesh->GetOptions() & D3DXMESH_32BIT)  // 32BIT index
    {
        pShape->Initialize (pVB, pMesh->GetNumBytesPerVertex (),
            (int*)pIB, pMesh->GetNumFaces ());
    }
    else  // 16BIT index
    {
        pShape->Initialize (pVB, pMesh->GetNumBytesPerVertex (),
            (WORD*)pIB, pMesh->GetNumFaces ());
    }
    pMesh->UnlockVertexBuffer ();
    pMesh->UnlockIndexBuffer ();
}


void LoadMesh(IDirect3DDevice9* pd3dDevice, LPD3DXMESH &pMesh, TCHAR* xFileName, D3DXMATRIX& mat)
{
	LPD3DXMESH	tempMesh=0;

	DWORD num;
	D3DXLoadMeshFromX(xFileName, D3DXMESH_MANAGED, pd3dDevice, NULL, NULL, NULL, &num, &tempMesh);

	tempMesh->CloneMeshFVF( tempMesh->GetOptions() | D3DXMESH_32BIT, DXFVF, pd3dDevice, &pMesh );
	ReleaseCOM( tempMesh );

	D3DXComputeNormals(pMesh, 0);	
	DXVERTEX *dxvb;
	pMesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, (void**)&dxvb);
	for(int i=0; i<(int)pMesh->GetNumVertices (); i++)
	{
		D3DXVec3TransformCoord(&dxvb[i].p,&dxvb[i].p,&mat);
	}
	pMesh->UnlockVertexBuffer ();

	DWORD *pData;
	pMesh->LockAttributeBuffer (D3DLOCK_NOSYSLOCK, &pData);
	for(int i=0; i<(int)pMesh->GetNumFaces(); i++)
	{
		pData[i]=0;
	}
	pMesh->UnlockAttributeBuffer ();

}


LPD3DXMESH LoadMeshActor(IDirect3DDevice9* pd3dDevice, LPD3DXMESH pMesh, D3DXMATRIX& mat)
{
	pMesh->CloneMeshFVF( pMesh->GetOptions() | D3DXMESH_32BIT, DXFVF, pd3dDevice, &pMesh );

	D3DXComputeNormals(pMesh, 0);	
	DXVERTEX *dxvb;
	pMesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, (void**)&dxvb);
	for(int i=0; i<(int)pMesh->GetNumVertices (); i++)
	{
		D3DXVec3TransformCoord(&dxvb[i].p,&dxvb[i].p,&mat);
	}
	pMesh->UnlockVertexBuffer ();

	DWORD *pData;
	pMesh->LockAttributeBuffer (D3DLOCK_NOSYSLOCK, &pData);
	for(int i=0; i<(int)pMesh->GetNumFaces(); i++)
	{
		pData[i]=0;
	}
	pMesh->UnlockAttributeBuffer ();
	return pMesh;
}




void Init_Physics()
{
	pWorld = CreateSPEWorld();  // create a instance of physics world
    pWorld -> SetGravity (SPEVector(0, -9.8f, 0)); // set the gravity
}



