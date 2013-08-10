#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(void)
{
}


ParticleSystem::~ParticleSystem(void)
{
	for(int i = 0; i < this->particlePolicies.size(); i++)
	{
		delete particlePolicies.at(i);
	}
}

void ParticleSystem::update(ID3D11DeviceContext* immediateContext, ID3D11Device* device)
{
	//update all policies
	for(int i = 0; i < particlePolicies.size(); i++)
	{
		particlePolicies.at(i)->update(immediateContext, device);
	}
}

void ParticleSystem::render(ID3D11DeviceContext* immediateContext, D3DXMATRIX wvp)
{
	for(int i = 0; i < particlePolicies.size(); i++)
	{
		particlePolicies.at(i)->render(immediateContext, wvp);
	}
	
}

void ParticleSystem::addRain(ID3D11DeviceContext* immediateContext, ID3D11Device* device, string shaderFileName, D3DXVECTOR3 startPosition, D3DXVECTOR3 direction, int lenghtX, int lenghtZ, int spacing, int intensity, int timeToLive, float velocity)
{
	particlePolicies.push_back(new Rain(immediateContext, device, shaderFileName, startPosition, direction, lenghtX, lenghtZ, spacing, intensity, timeToLive, velocity));
}

void ParticleSystem::addFire(ID3D11DeviceContext* immediateContext, ID3D11Device* device, string shaderFileName, string textureFileName, D3DXVECTOR3 position, D3DXVECTOR3 coneDir, int intensity, int timeToLive, float velocity)
{
	particlePolicies.push_back(new Fire(immediateContext, device, shaderFileName, textureFileName, position, coneDir, intensity, timeToLive, velocity));
}

void ParticleSystem::addCloud(ID3D11DeviceContext* immediateContext, ID3D11Device* device, std::string shaderFileName ,D3DXVECTOR3 color, D3DXVECTOR3 position , int intensity, int timeToLive, float velocity, float lengthX, float lengthY, float lengthZ)
{
	particlePolicies.push_back(new Cloud(immediateContext, device, shaderFileName, color, position, intensity, timeToLive, velocity, lengthX, lengthY, lengthZ));
}

bool ParticleSystem::removePolicy() // kompletera med hur detta ska ske, hur removar man den policy man vill åt?(id? string namn?)
{
	return true;
}