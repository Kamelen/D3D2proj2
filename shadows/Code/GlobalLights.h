#pragma once
#include "stdafx.h"
#include <vector>
using namespace std;

class GlobalLights // array of lights all used with one shader
{
public:
	GlobalLights(void);
	~GlobalLights(void);

	PointLight* getPointLights();
	int getnrOfPointLights();

	void addPointLight(PointLight pointLight);

private:
	vector<PointLight> pointLights;
};

