#ifndef D3DObject_H
#define D3DObject_H

#include "Buffer.h"
#include "stdafx.h"
using namespace std;

class D3DObject
{
	private:
		Buffer* vB;
		Buffer* iB;
		D3DXMATRIX worldMat;
		

	protected:
		vector<Vertex> mesh;
		vector<int> indices;
		int nrOfVertices;
		ID3D11ShaderResourceView* texture;


	public:
		D3DObject(Vertex* mesh, int nrOfVertices, D3DXMATRIX worldMat);
		D3DObject();
		~D3DObject(); 

		int getNrOfVertices() const;
		Buffer* getVertexBuffer() const;
		Buffer* getIndexBuffer() const;
		ID3D11ShaderResourceView* getTexture() const;
		bool initBuffer(ID3D11Device *device, ID3D11DeviceContext *deviceContext);

		D3DXMATRIX getWorldMatrix();

};

#endif