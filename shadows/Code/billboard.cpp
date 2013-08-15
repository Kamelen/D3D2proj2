#include "billboard.h"


Billboard::Billboard(void)
{
	vBuffer = NULL;
	this->nrOfVerts = 0;
	D3DXMatrixIdentity(&this->worldMat);

}

Billboard::Billboard(Vertex* mesh, int nrOfVerts, D3DXVECTOR3 pos, ID3D11Device *device, ID3D11DeviceContext * deviceContext)
{
	this->nrOfVerts = nrOfVerts;
	this->position = pos;


	BUFFER_INIT_DESC vertexBufferDesc;											
 	vertexBufferDesc.ElementSize = sizeof(Vertex);								
	vertexBufferDesc.InitData = mesh;											
	vertexBufferDesc.NumElements = this->nrOfVerts;								
	vertexBufferDesc.Type = VERTEX_BUFFER;										
	vertexBufferDesc.Usage = BUFFER_DEFAULT;									
																				
	this->vBuffer = new Buffer();												
	if(FAILED(this->vBuffer->Init(device, deviceContext, vertexBufferDesc)))	
	{																			
		//error															
	}


}

Billboard::~Billboard(void)
{
	SAFE_DELETE(vBuffer);
}


int Billboard::getNrOfVerts() const
{
	return this->nrOfVerts;
}

D3DXMATRIX Billboard::getUpdatedWorldMat(D3DXVECTOR3 cameraPos)
{
	D3DXMatrixIdentity(&worldMat);
	D3DXMATRIX trans,scale;
	float angle;
	float rotation;
	//D3DXMatrixTranslation(&trans,position.x,position.y,position.z);
	D3DXMatrixScaling(&scale,10,10,10);
	angle = atan2(position.x - cameraPos.x, position.z - cameraPos.z) * (180.0 / D3DX_PI);
	rotation = (float)angle * 0.0174532925f;

	D3DXMatrixRotationY(&worldMat, rotation);
	D3DXMatrixTranslation(&trans, position.x, position.y, position.z);
	trans = scale * trans;

	D3DXMatrixMultiply(&worldMat,&worldMat,&trans);
	return this->worldMat;
}

Buffer* Billboard::getVertexBuffer() const
{
	return this->vBuffer;
}