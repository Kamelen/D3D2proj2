#pragma once
#include "stdafx.h"
#include <string>
#include "D3DObject.h"
using namespace std;
class Terrain : public D3DObject
{
	private:
		//funktioner
		void filter(int rows, int cols);
		float sampleHeight(int i, int j);
		bool inBounds(int i, int j);
		void CalculateNormals();

		//variablar
		ID3D11ShaderResourceView** textures;

		float **heightMap;
		int terrainWidth, terrainHeight;
		int vertexCount;
		int spacing;
		
	
	public:
		Terrain(const string& filename, int rows, int cols, float heightScale, float heightOffset, ID3D11Device *device, ID3D11DeviceContext *deviceContext, std::string* textureName, std::string blendMap);
		~Terrain(void);

		void LoadHeightMap(const string& filename, int rows, int cols, float heightScale, float heightOffset);
		void createMesh();
		void createIndices();
		
		D3DXVECTOR3 getTerrainPos(int col,int row);
		float getY(int x, int z);
		ID3D11ShaderResourceView* getTerTexture(int i) const;


};

