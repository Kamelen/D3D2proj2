#include "Fire.h"

Fire::Fire(ID3D11DeviceContext* deviceContext, ID3D11Device* device, std::string shaderFileName , std::string textureFileName, D3DXVECTOR3 position, D3DXVECTOR3 coneDir, int intensity, int timeToLive, float velocity)
{
	this->nrOfVertsPerParticle = 4;
	this->intensity = intensity;

	this->vBuffer = NULL;
	this->iBuffer = NULL;

	this->emitter = new BaseParticle(position , coneDir, timeToLive , velocity);
	
	if(FAILED(D3DX11CreateShaderResourceViewFromFile(device, textureFileName.c_str() , NULL , NULL, &this->texture, NULL))){}

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	shaderFileName = "../Shaders/" + shaderFileName;

	this->shader = new Shader();
	if(FAILED(this->shader->Init(device, deviceContext, (char*)shaderFileName.c_str(), inputDesc, 3))){}
}

Fire::~Fire()
{
	if(this->shader)
	{
		delete this->shader;
		this->shader = NULL;
	}
	

	if(this->vBuffer)
	{
		delete this->vBuffer;
		this->vBuffer = NULL;
	}

	if(this->emitter)
	{
		delete this->emitter;
		this->emitter = NULL;
	}
	

	if(this->iBuffer)
	{
		delete this->iBuffer;
		this->iBuffer = NULL;
	}

	if(this->texture)
	{
		this->texture->Release();
		this->texture = NULL;
	}
	

	
}

void Fire::update(ID3D11DeviceContext *immediateContext, ID3D11Device *device)
{
	for(int i = 0; i < intensity; i++)
	{
		emitt();
	}
	

	for(int i = 0; i < this->particles.size(); i++)
	{
		if(!this->particles.at(i).update())
		{
			this->particles.erase(this->particles.begin() + i);
		}
	}

	createVertices();
	createIndices();
	initiate(immediateContext, device);
}

void Fire::render(ID3D11DeviceContext *deviceContext, D3DXMATRIX wvp)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->vBuffer->Apply();
	this->iBuffer->Apply();
	this->shader->SetMatrix("gWVP", wvp);

	shader->SetResource("Texture" , this->texture);
	this->shader->Apply(0);
	deviceContext->DrawIndexed((UINT)this->indices.size(),0,0);
}

void Fire::emitt()
{
	BaseParticle tempParticle = *this->emitter;

	static float srand((float)time(NULL));
	float posX = (float)rand()/(float)(RAND_MAX/4.0f);
	float posY = (float)rand()/(float)(RAND_MAX/4.0f);
	float posZ = (float)rand()/(float)(RAND_MAX/4.0f);
	float randLife = (float)rand()/(float)(RAND_MAX/10.0f);



	D3DXVECTOR3 origPos = tempParticle.getPosition();
	int timeToLive = tempParticle.getTimeToLive();

	origPos.x += posX;
	origPos.y += posY;
	origPos.z += posZ;

	timeToLive += (int)randLife;

	tempParticle.setDirection(D3DXVECTOR3(0,1,0));
	tempParticle.setPosition(origPos);
	tempParticle.setTimeToLive(timeToLive);


	this->particles.push_back(tempParticle);
}

void Fire::createVertices()
{
	this->vertices.clear();

	for(int i = 0; i < this->particles.size(); i++)
	{
		Vertex vert1, vert2, vert3, vert4;
		D3DXVECTOR3 originalPosition = this->particles.at(i).getPosition();

		vert1.pos = originalPosition;
		vert1.uv = D3DXVECTOR2(0,0);
		vert1.normal = D3DXVECTOR3(0,0,-1);

		vert2.pos = originalPosition;
		vert2.pos.x += 0.5;
		vert2.uv = D3DXVECTOR2(1,0);
		vert2.normal = D3DXVECTOR3(0,0,-1);

		vert3.pos = originalPosition;
		vert3.pos.y += 0.5;
		vert3.uv = D3DXVECTOR2(0,1);
		vert3.normal = D3DXVECTOR3(0,0,-1);

		vert4.pos = originalPosition;
		vert4.pos.x += 0.5;
		vert4.pos.y += 0.5;
		vert4.uv = D3DXVECTOR2(1,1);
		vert4.normal = D3DXVECTOR3(0,0,-1);


		this->vertices.push_back(vert1);
		this->vertices.push_back(vert2);
		this->vertices.push_back(vert3);
		this->vertices.push_back(vert4);
	}
}

void Fire::createIndices()
{
	this->indices.clear();

	for(int i = 0; i < this->vertices.size(); i += 4)
	{
		this->indices.push_back(i);
		this->indices.push_back(i+1);
		this->indices.push_back(i+2);

		this->indices.push_back(i+1);
		this->indices.push_back(i+3);
		this->indices.push_back(i+2);
	}
}

void Fire::initiate(ID3D11DeviceContext *immediateContext, ID3D11Device *device)
{
	if(this->vBuffer)
	{
		delete this->vBuffer;
		this->vBuffer = NULL;
	}

	if(this->iBuffer)
	{
		delete this->iBuffer;
		this->iBuffer = NULL;
	}

	BUFFER_INIT_DESC vBufferDesc;
	vBufferDesc.ElementSize = sizeof(Vertex);
	vBufferDesc.InitData = &this->vertices[0];
	vBufferDesc.NumElements = (UINT32)this->vertices.size();
	vBufferDesc.Type = VERTEX_BUFFER;
	vBufferDesc.Usage = BUFFER_DEFAULT;

	this->vBuffer = new Buffer;

	if(FAILED(this->vBuffer->Init(device , immediateContext, vBufferDesc)))
	{
		return;
	}

	BUFFER_INIT_DESC iBufferDesc;
	iBufferDesc.ElementSize = sizeof(int);
	iBufferDesc.InitData = &this->indices[0];
	iBufferDesc.NumElements = (UINT32)this->indices.size();
	iBufferDesc.Type = INDEX_BUFFER;
	iBufferDesc.Usage = BUFFER_DEFAULT;

	this->iBuffer = new Buffer;
	if(FAILED(this->iBuffer->Init(device , immediateContext, iBufferDesc)))
	{
		return;
	}

}