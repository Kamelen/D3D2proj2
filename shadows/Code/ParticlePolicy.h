#ifndef PARTICLEPOLICY_H
#define PARTICLEPOLICY_H

#include "stdafx.h"
#include "Buffer.h"
#include "Shader.h"
#include "BaseParticle.h"

class ParticlePolicy
{
	protected:
		Buffer *vBuffer;
		
		int nrOfVertsPerParticle;
		int intensity;
		
		BaseParticle *emitter;
		std::vector<BaseParticle> particles;

		virtual void emitt() = 0;
		virtual void createVertices() = 0;
		virtual void initiate(ID3D11DeviceContext* immediateContext, ID3D11Device* device) = 0;
		

	public:
		virtual void update(ID3D11DeviceContext *immediateContext, ID3D11Device* device) = 0;
		virtual void render(ID3D11DeviceContext *immediateContext, Shader* shader) = 0;

		virtual ~ParticlePolicy(){};

};

#endif