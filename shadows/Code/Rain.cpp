#include "Rain.h"


Rain::Rain(void)
{
}

Rain::Rain(ID3D11DeviceContext* immediateContext, ID3D11Device* device, D3DXVECTOR3 startPosition, D3DXVECTOR3 direction, int halfLengthX, int halfLengthZ, int spacing, int intensity, int timeToLive, float velocity)
{
	this->emitter = new BaseParticle(startPosition,direction,timeToLive,velocity);
	this->nrOfVertsPerParticle = 2;
	this->halfLengthX = halfLengthX;
	this->halfLengthZ = halfLengthZ;
	this->spacing = spacing;
	this->intensity = intensity;

	this->vBuffer = NULL;
}


Rain::~Rain(void)
{
	if(vBuffer)
	{
		delete vBuffer;
	}

	if(emitter)
	{
		delete emitter;
	}
	
}


void Rain::emitt()
{
	static int srand(time(NULL));

	int randX = rand()%(halfLengthX*2) - halfLengthX;
	int randZ = rand()%(halfLengthZ*2) - halfLengthZ;
	//when particle is made set the startPos x and z to one step forward according to spacing and then reset when limit is reached

	BaseParticle tempParticle = *emitter;
	tempParticle.setPosition(D3DXVECTOR3(tempParticle.getPosition().x + randX, tempParticle.getPosition().y - 50, tempParticle.getPosition().z + randZ));

	this->particles.push_back(BaseParticle(tempParticle));
	
}

void Rain::createVertices()
{
	//make first vertice out of the position of each particle
	mesh.clear();

	for(int i = 0; i < this->particles.size(); i++)
	{
		VertexColor vertex1, vertex2;

		vertex1.color =  D3DXVECTOR3(0.5,0.5,0.5);
		vertex1.normal = D3DXVECTOR3(0,0,0);
		vertex1.pos = particles.at(i).getPosition();
		mesh.push_back(vertex1);

		vertex2.color = D3DXVECTOR3(0.5,0.5,0.5);
		vertex2.normal = D3DXVECTOR3(0,0,0);
		vertex2.pos = D3DXVECTOR3(particles.at(i).getPosition().x, particles.at(i).getPosition().y - 2, particles.at(i).getPosition().z);
		mesh.push_back(vertex2);
		
	}
}

void Rain::initiate(ID3D11DeviceContext* immediateContext, ID3D11Device* device)
{
	if(vBuffer)
	{
		delete vBuffer;
		vBuffer = NULL;
	}

	BUFFER_INIT_DESC vertexBufferDesc;
 	vertexBufferDesc.ElementSize = sizeof(VertexColor);
	vertexBufferDesc.InitData = &mesh[0];
	vertexBufferDesc.NumElements = mesh.size();
	vertexBufferDesc.Type = VERTEX_BUFFER;
	vertexBufferDesc.Usage = BUFFER_DEFAULT;

	this->vBuffer = new Buffer();
	this->vBuffer->Init(device, immediateContext, vertexBufferDesc);
}

void Rain::update(ID3D11DeviceContext *immediateContext, ID3D11Device* device)
{

	if(this->particles.size() <= this->intensity)
	{
		this->emitt();
	}

	//check particles

	for(int i = 0; i < this->particles.size(); i++)
	{
		if(!this->particles.at(i).update())
		{
			this->particles.erase(this->particles.begin() + i);
		}
	}


	//create vertices
	createVertices();

	//initiate buffer(s)
	initiate(immediateContext,device);
}

void Rain::render(ID3D11DeviceContext *immediateContext, Shader* shader)
{

	//set topology
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);

	shader->SetMatrix("world", world);
	shader->SetBool("useCubeMap",false);
	shader->SetBool("useShadowMap",false);
	shader->SetBool("useBlendMap",false);

	//apply shader and buffer
	this->vBuffer->Apply();
	shader->Apply(0);


	//draw call
	immediateContext->Draw(this->mesh.size(),0);
	
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
}