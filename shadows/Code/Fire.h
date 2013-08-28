#ifndef FIRE_H
#define FIRE_H

#include "ParticlePolicy.h"
#include <ctime>

class Fire : public ParticlePolicy
{
	private:
		Buffer *iBuffer;
		ID3D11ShaderResourceView* texture;
		std::vector<Vertex> vertices;
		std::vector<int> indices;

		void emitt();
		void createVertices();
		void createIndices();
		void initiate(ID3D11DeviceContext *immediateContext, ID3D11Device *device);

	public:
		Fire(ID3D11DeviceContext* immediateContext, ID3D11Device* device,D3DXVECTOR3 position, D3DXVECTOR3 coneDir, int intensity, int timeToLive, float velocity);
		~Fire();
		void update(ID3D11DeviceContext *immediateContext, ID3D11Device *device);
		void render(ID3D11DeviceContext *immediateContext, Shader* shader);
};

#endif