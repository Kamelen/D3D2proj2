#pragma once
#include "stdafx.h"
#include "Buffer.h"
#include "billboard.h"
#include "Tree.h"

using namespace std;
class Forest
{
public:
	Forest(void);
	Forest(int var);

	~Forest(void);

private:
	Tree** trees;
	int nrOfTrees;

	int nrOfVertsPerTree;
	Buffer* vBufferTree;
	Buffer* vBufferTreeBillboard;

};

