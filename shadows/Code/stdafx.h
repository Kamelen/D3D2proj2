#ifndef _STDAFX__H
#define _STDAFX__H

#include <windows.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <D3DX10math.h>
#include <d3dx11effect.h>
#include <d3dCompiler.h>

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <ctime>

#define SAFE_RELEASE(x) if( x ) { (x)->Release(); (x) = NULL; }
#define SAFE_DELETE(x) if( x ) { delete(x); (x) = NULL; }
#define SAFE_DELETE_ARRAY(x) if( x ) { delete[](x); (x) = NULL; }
#define PI (3.14159265358979323846f)

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment (lib,"dxerr.lib")

#ifdef _DEBUG
#pragma comment(lib, "d3dx11d.lib")
#pragma comment(lib, "Effects11D.lib")
#pragma comment(lib, "d3dx10d.lib")
#else
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "Effects11.lib")
#pragma comment(lib, "d3dx10.lib")
#endif

/////
struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 uv;

	Vertex(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR2 uv)
	{
		this->pos = pos;
		this->normal = normal;
		this->uv = uv;
	}
	Vertex()
	{
	}

};

struct POINTLIGHTINSTANCE
{
	D3DXVECTOR3 lightPos;
	D3DXVECTOR3 lightColor;
	float lightRadius;

	POINTLIGHTINSTANCE()
	{
		this->lightPos = D3DXVECTOR3(0,0,0);
		this->lightColor = D3DXVECTOR3(1,1,1);
		this->lightRadius = 1;
	}

	POINTLIGHTINSTANCE(D3DXVECTOR3 lightPos, D3DXVECTOR3 lightColor, float lightRadius)
	{
		this->lightPos = lightPos;
		this->lightColor = lightColor;
		this->lightRadius = lightRadius;
	}


};

struct FaceType
{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;

		FaceType()
		{
		}

};



struct VertexColor
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 color;

	VertexColor(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 color)
	{
		this->pos = pos;
		this->normal = normal;
		this->color = color;
	}
	VertexColor()
	{
	}

};


//////////////////////////////////////////////////////////////////////////
// to find memory leaks
//////////////////////////////////////////////////////////////////////////
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define myMalloc(s)       _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define myCalloc(c, s)    _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define myRealloc(p, s)   _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define myExpand(p, s)    _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define myFree(p)         _free_dbg(p, _NORMAL_BLOCK)
#define myMemSize(p)      _msize_dbg(p, _NORMAL_BLOCK)
#define myNew new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define myDelete delete  // Set to dump leaks at the program exit.
#define myInitMemoryCheck() \
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#define myDumpMemoryLeaks() \
	_CrtDumpMemoryLeaks()
#else
#define myMalloc malloc
#define myCalloc calloc
#define myRealloc realloc
#define myExpand _expand
#define myFree free
#define myMemSize _msize
#define myNew new
#define myDelete delete
#define myInitMemoryCheck()
#define myDumpMemoryLeaks()
#endif 
//////////////////////////////////////////////////////////////////////////



#endif
