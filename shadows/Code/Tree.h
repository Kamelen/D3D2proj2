#pragma once
#include "stdafx.h"
#include "billboard.h"
class Tree
{
public:
	Tree(void);
	Tree(Vertex* mesh,int nrOfVerts);
	~Tree(void);

private:
	D3DXVECTOR3 pos; // kanske behövs

	D3DXMATRIX worldMatObj; // world matrix for the normal tree
	D3DXMATRIX worldMatbillboard; // world matrix for the billboard version of the tree
};