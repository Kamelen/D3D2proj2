#include "D3DObject.h"

D3DObject::D3DObject(Vertex* mesh, int nrOfVertices, D3DXMATRIX worldMat)
{
	this->mesh.resize(nrOfVertices);

	for(int i = 0; i < nrOfVertices; i++)
	{
		this->mesh.at(i) = mesh[i];
	}

	this->nrOfVertices = nrOfVertices;
	this->texture = NULL;
	this->vB = NULL;
	this->iB = NULL;

	this->worldMat = worldMat;
}

D3DObject::D3DObject(void)
{
	this->nrOfVertices = 0;
	this->texture = NULL;
}

D3DObject::~D3DObject()
{
	if(this->vB != NULL)
	{
		delete this->vB;
	}
	if(this->iB != NULL)
	{
		delete this->iB;
	}

	if(this->texture != NULL)
	{
		this->texture->Release();
		this->texture = NULL;
	}
}

bool D3DObject::initBuffer(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
{
	BUFFER_INIT_DESC vertexBufferDesc;
 	vertexBufferDesc.ElementSize = sizeof(Vertex);
	vertexBufferDesc.InitData = &this->mesh[0];
	vertexBufferDesc.NumElements = this->mesh.size();
	vertexBufferDesc.Type = VERTEX_BUFFER;
	vertexBufferDesc.Usage = BUFFER_DEFAULT;

	this->vB = new Buffer();
	if(FAILED(this->vB->Init(device, deviceContext, vertexBufferDesc)))
	{
		return false;
	}

	if(indices.size() > 0)
	{
		BUFFER_INIT_DESC indexBufferDesc;
 		indexBufferDesc.ElementSize = sizeof(int);
		indexBufferDesc.InitData = &indices[0];
		indexBufferDesc.NumElements = indices.size();
		indexBufferDesc.Type = INDEX_BUFFER;
		indexBufferDesc.Usage = BUFFER_DEFAULT;

		this->iB = new Buffer();
		if(FAILED(this->iB->Init(device, deviceContext, indexBufferDesc)))
		{
			return false;
		}
	}

	return true;
}

int D3DObject::getNrOfVertices() const
{
	return this->mesh.size();
}

Buffer* D3DObject::getVertexBuffer() const
{
	return this->vB;
}

Buffer* D3DObject::getIndexBuffer() const
{
	return this->iB;
}

ID3D11ShaderResourceView* D3DObject::getTexture() const
{
	return this->texture;
}

D3DXMATRIX D3DObject::getWorldMatrix()
{
	return this->worldMat;
}