#pragma once
#include "stdafx.h"
#include "Buffer.h"

using namespace std;

class Billboard
{
public:
	Billboard(void);
	Billboard(Vertex* mesh, int nrOfVerts, D3DXVECTOR3 pos, ID3D11Device *device, ID3D11DeviceContext * deviceContext);
	~Billboard(void);
	
	Buffer* getVertexBuffer() const;
	int getNrOfVerts() const;
	D3DXMATRIX getUpdatedWorldMat(D3DXVECTOR3 cameraPos);
private:
	Buffer* vBuffer;
	int nrOfVerts;
	D3DXVECTOR3 position;

	D3DXMATRIX worldMat;

};

