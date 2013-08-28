#pragma once
#include "ParticlePolicy.h"
using namespace std;
class Rain : public ParticlePolicy
{
public:
	Rain(void);
	Rain(ID3D11DeviceContext* immediateContext, ID3D11Device* device, D3DXVECTOR3 startPosition, D3DXVECTOR3 direction, int halfLenghtX, int halfLenghtZ, int spacing, int intensity, int timeToLive, float velocity);

	~Rain(void);

	void emitt();
	void createVertices();
	void initiate(ID3D11DeviceContext* immediateContext, ID3D11Device* device);

	void update(ID3D11DeviceContext *immediateContext, ID3D11Device* device);
	void render(ID3D11DeviceContext *immediateContext, Shader* shader);

private:
	D3DXVECTOR3 position;
	int halfLengthX;
	int halfLengthZ;
	int spacing;
	vector<VertexColor> mesh;

};

