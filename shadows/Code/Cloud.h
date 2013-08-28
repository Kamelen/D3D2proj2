#ifndef CLOUD_H
#define CLOUD_H

#include "ParticlePolicy.h"
#include <ctime>

class Cloud : public ParticlePolicy
{
	private:
		std::vector<VertexColor> vertices;

		D3DXVECTOR3 color;
		float lengthX;
		float lengthY;
		float lengthZ;
		void emitt();
		void createVertices();
		void initiate(ID3D11DeviceContext *immediateContext, ID3D11Device *device);

	public:
		Cloud(ID3D11DeviceContext* deviceContext, ID3D11Device* device, D3DXVECTOR3 position , int intensity, int timeToLive, float velocity, float lengthX, float lengthY, float lengthZ);
		~Cloud();

		void update(ID3D11DeviceContext *immediateContext, ID3D11Device *device);
		void render(ID3D11DeviceContext *immediateContext, Shader * shader);
};

#endif