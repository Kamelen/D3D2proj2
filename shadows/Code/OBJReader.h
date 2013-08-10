#pragma once
#include "stdafx.h"
#include <fstream>
#include <iostream>

using namespace std;

class OBJReader
{

public:
	OBJReader(void);
	~OBJReader(void);

	Vertex* getOBJfromFile(string fileName, int &nrOfVerts);

private:
	bool ReadFileCounts(string fileName, int& vertexCount, int& textureCount, int& normalCount, int& faceCount);
	Vertex* LoadDataStructures(string fileName, int vertexCount, int textureCount, int normalCount, int faceCount);
};

