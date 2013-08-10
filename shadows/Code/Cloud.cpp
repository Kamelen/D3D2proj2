#include "Cloud.h"

Cloud::Cloud(ID3D11DeviceContext* deviceContext, ID3D11Device* device, std::string shaderFileName , D3DXVECTOR3 color, D3DXVECTOR3 position , int intensity, int timeToLive, float velocity, float lengthX, float lengthY, float lengthZ)
{
	this->nrOfVertsPerParticle = 1;
	this->intensity = intensity;
	
	this->vBuffer = NULL;
	this->color = color;
	this->lengthX = lengthX;
	this->lengthY = lengthY;
	this->lengthZ = lengthZ;

	this->emitter = new BaseParticle(position, D3DXVECTOR3(0,1,0), timeToLive, velocity);

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	shaderFileName = "../Shaders/" + shaderFileName;

	this->shader = new Shader();
	if(FAILED(this->shader->Init(device, deviceContext, (char*)shaderFileName.c_str(), inputDesc, 3))){}
}

Cloud::~Cloud()
{
	if(this->vBuffer)
	{
		delete this->vBuffer;
		this->vBuffer = NULL;
	}
	if(this->shader)
	{
		delete shader;
		this->shader = NULL;
	}
	
	if(this->emitter)
	{
		delete this->emitter;
		this->emitter = NULL;
	}
	
}

void Cloud::update(ID3D11DeviceContext *immediateContext, ID3D11Device *device)
{
	for(int i = 0; i < this->intensity; i++)
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
	initiate(immediateContext, device);
}

void Cloud::render(ID3D11DeviceContext *immediateContext, D3DXMATRIX wvp)
{
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	this->vBuffer->Apply();
	this->shader->SetMatrix("gWVP", wvp);

	this->shader->Apply(0);
	immediateContext->Draw(this->vertices.size(),0);

}

void Cloud::emitt()
{
	BaseParticle Particle = *this->emitter;
	static float srand(time(NULL));
	float halfLengthX = this->lengthX * 0.5;
	float halfLengthY = this->lengthY * 0.5;
	float halfLengthZ = this->lengthZ * 0.5;

	D3DXVECTOR3 origPos = Particle.getPosition();

	float posX = origPos.x;
	float posY = origPos.y;
	float posZ = origPos.z;

	posX += ((float(rand()) / float(RAND_MAX)) * (halfLengthX - (-halfLengthX))) + (-halfLengthX);
	posY += ((float(rand()) / float(RAND_MAX)) * (halfLengthY - (-halfLengthY))) + (-halfLengthY);
	posZ += ((float(rand()) / float(RAND_MAX)) * (halfLengthZ - (-halfLengthZ))) + (-halfLengthZ);

	float dirX = (float)rand() / (float)RAND_MAX -0.5f;
	float dirY = (float)rand() / (float)RAND_MAX -0.5f;
	float dirZ = (float)rand() / (float)RAND_MAX -0.5f;

	
	Particle.setDirection(D3DXVECTOR3(dirX,dirY,dirZ));
	Particle.setPosition(D3DXVECTOR3(posX,posY,posZ));

	this->particles.push_back(Particle);
	
}
void Cloud::createVertices()
{
	this->vertices.clear();

	for(int i = 0; i < this->particles.size(); i++)
	{
		VertexColor vert1;

		vert1.normal = D3DXVECTOR3(0,0,-1);
		vert1.pos = this->particles.at(i).getPosition();
		vert1.color = this->color;

		vertices.push_back(vert1);
	}
}

void Cloud::initiate(ID3D11DeviceContext *immediateContext, ID3D11Device *device)
{
	if(this->vBuffer)
	{
		delete this->vBuffer;
		this->vBuffer = NULL;
	}

	BUFFER_INIT_DESC vBufferDesc;
	vBufferDesc.ElementSize = sizeof(VertexColor);
	vBufferDesc.InitData = &this->vertices[0];
	vBufferDesc.NumElements = (UINT32)this->vertices.size();
	vBufferDesc.Type = VERTEX_BUFFER;
	vBufferDesc.Usage = BUFFER_DEFAULT;

	this->vBuffer = new Buffer;

	if(FAILED(this->vBuffer->Init(device , immediateContext, vBufferDesc)))
	{
		return;
	}
}