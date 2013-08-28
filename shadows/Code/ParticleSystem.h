#pragma once
#include <vector>
#include "stdafx.h"
#include "Rain.h"
#include "Fire.h"
#include "Cloud.h"


using namespace std;
class ParticleSystem
{
public:
	ParticleSystem(void);
	~ParticleSystem(void);

	void update(ID3D11DeviceContext* immediateContext, ID3D11Device* device);
	void render(ID3D11DeviceContext* immediateContext,  Shader* shader);

	void addRain(ID3D11DeviceContext* immediateContext, ID3D11Device* device, D3DXVECTOR3 startPosition, D3DXVECTOR3 direction, int lenghtX, int lenghtZ, int spacing, int intensity, int timeToLive, float velocity);
	void addFire(ID3D11DeviceContext* immediateContext, ID3D11Device* device, D3DXVECTOR3 position, D3DXVECTOR3 coneDir, int intensity, int timeToLive, float velocity);
	void addCloud(ID3D11DeviceContext* immediateContext, ID3D11Device* device, D3DXVECTOR3 position , int intensity, int timeToLive, float velocity, float lengthX, float lengthY, float lengthZ); //kompletera med hur ett cloud ska defineras

	bool removePolicy(); // kompletera med hur detta ska ske, hur removar man den policy man vill åt?(id? string namn?)

private:
	vector<ParticlePolicy*> particlePolicies;
};

