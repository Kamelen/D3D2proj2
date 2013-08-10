#pragma once
#include "stdafx.h"
using namespace std;
class Camera
{
public:
	Camera(D3DXVECTOR3 pos, D3DXVECTOR3 look, D3DXVECTOR3 right, D3DXVECTOR3 up);
	~Camera(void);

	//getset metoder

	D3DXVECTOR3 getPosition() const;
	D3DXVECTOR3 getRight() const;
	D3DXVECTOR3 getUp() const;
	D3DXVECTOR3 getLook() const;

	float getNearZ() const;
	float getFarZ() const;
	float getAspect() const;
	float getFovY() const;
	float getFovX() const;
	
	float getNearWindowWidth() const;
	float getNearWindowHeight() const;
	float getFarWindowWidth() const;
	float getFarWindowHeight() const;
	
	void setLens(float fovY, float aspect, float zn, float zf);
	void lookAt(D3DXVECTOR3 pos,D3DXVECTOR3 target, D3DXVECTOR3 worldUp);
	void lookAtO();

	D3DXMATRIX getView() const;
	D3DXMATRIX getProj() const;

	void strafe(float d);
	void walk(float d);

	void pitch(float angle);
	void rotateY(float angle);

	//debug
	void setPos(D3DXVECTOR3 position);

	void updateViewMatrix();


private:
	D3DXMATRIX view;
	D3DXMATRIX proj;
	
	D3DXVECTOR3 position;
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;
	D3DXVECTOR3 look;
	
	float nearZ;
	float farZ;
	float aspect;
	float fovY;
	float nearWindowHeight;
	float farWindowHeight;
};

