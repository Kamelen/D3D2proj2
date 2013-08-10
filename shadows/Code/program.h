#pragma once
#include "stdafx.h"
#include "WinHandler.h"
#include "D3D11Handler.h"
#include "D3DObject.h"
#include "Terrain.h"
#include "Camera.h"
#include <iostream>
#include "Input.h"
#include "ParticleSystem.h"
#include "OBJReader.h"
#include "ShadowMap.h"

using namespace std;

//include direct3d
class program : public D3D11Handler
{
public:
	program(void);
	~program(void);

	bool initiate(HINSTANCE hInstance, int nCmdShow);
	void run(float deltaTime);

private:
	vector<D3DObject> objects;
	Terrain* map;
	ParticleSystem *pSys;
	OBJReader *objReader;


	Buffer* vBuffer;
	Vertex* mesh;

	ShadowMap* shadowMap;

	Camera* cam;
	Camera* lightCam;
	Input* input;

	int nrOfVerts;
	int nrOfObjects;
	int update(float deltaTime);
	void render(float deltaTime);
	void checkKeyBoard(float deltaTime);
	void mouseOnMove(WPARAM btnState, int x, int y);

	void buildShadowMap(D3DXMATRIX &lightViewProj);


	float switchTime;
	int apply;
};

