#include "Camera.h"


Camera::Camera(D3DXVECTOR3 pos, D3DXVECTOR3 look, D3DXVECTOR3 right, D3DXVECTOR3 up)
{
	this->position = pos;
	this->look = look;
	this->right = right;
	this->up = up;
}


Camera::~Camera(void)
{
}


//////////////////////////////////////////////////////////////
///////////////////////GetSet Methods/////////////////////////
//////////////////////////////////////////////////////////////
//vectors
D3DXVECTOR3 Camera::getPosition() const
{
	return this->position;
}

D3DXVECTOR3 Camera::getRight() const
{
	return this->right;
}

D3DXVECTOR3 Camera::getUp() const
{
	return this->up;
}

D3DXVECTOR3 Camera::getLook() const
{
	return this->look;
}

//floats
float Camera::getNearZ() const
{
	return this->nearZ;
}
	
float Camera::getFarZ() const
{
	return this->farZ;
}
	
float Camera::getAspect() const
{
	return this->aspect;
}

float Camera::getFovY() const
{
	return this->fovY;
}
	
float Camera::getNearWindowWidth() const
{
	return this->aspect * this->nearWindowHeight;
}

float Camera::getNearWindowHeight() const
{
	return this->nearWindowHeight;
}

float Camera::getFarWindowWidth() const
{
	return this->aspect * this->farWindowHeight;
}

float Camera::getFarWindowHeight() const
{
	return this->farWindowHeight;
}

float Camera::getFovX() const
{
	float halfWidth = 0.5f * this->getNearWindowWidth();
	return 2.0f*atan(halfWidth / this->nearZ);
}

D3DXMATRIX Camera::getView() const
{
	return this->view;
}	
D3DXMATRIX Camera::getProj() const
{
	return this->proj;
}
//////////////////////////////////////////////////////////////
///////////////////////Camera Methods/////////////////////////
//////////////////////////////////////////////////////////////
	
void Camera::setLens(float fovY, float aspect, float zn, float zf)
{
	this->fovY = fovY;
	this->aspect = aspect;
	this->nearZ = zn;
	this->farZ = zf;
	this->nearWindowHeight = 2.0f * this->nearZ * tanf(0.5 * this->fovY);
	this->farWindowHeight = 2.0f * this->farZ * tanf(0.5 * this->fovY);

	D3DXMatrixPerspectiveFovLH(&this->proj,this->fovY,this->aspect,this->nearZ,this->farZ);
}

void Camera::walk(float d)
{
	this->position += (d*this->look);
}

void Camera::strafe(float d)
{
	this->position += (d*this->right);
}

void Camera::pitch(float angle)
{
	D3DXMATRIX rotation;
	D3DXMatrixRotationAxis(&rotation,&right,angle);

	D3DXVec3TransformNormal(&up,&up,&rotation);
	D3DXVec3TransformNormal(&look,&look,&rotation);
	
}
void Camera::lookAtO()
{
	D3DXMatrixLookAtLH(&this->view, &this->position, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0));
}

void Camera::rotateY(float angle)
{
	D3DXMATRIX rotation;
	D3DXMatrixRotationY(&rotation,angle);

	D3DXVec3TransformNormal(&right,&right,&rotation);
	D3DXVec3TransformNormal(&up,&up,&rotation);
	D3DXVec3TransformNormal(&look,&look,&rotation);	
}

void Camera::updateViewMatrix()
{
	D3DXVECTOR3 R = this->right;
	D3DXVECTOR3 U = this->up;
	D3DXVECTOR3 L = this->look;
	D3DXVECTOR3 P = this->position;
	
	D3DXVec3Normalize(&L,&L);
	
	D3DXVec3Cross(&U,&L,&R);
	D3DXVec3Normalize(&U,&U);

	D3DXVec3Cross(&R,&U,&L);

	float x = -D3DXVec3Dot(&P,&R);
	float y = -D3DXVec3Dot(&P,&U);
	float z = -D3DXVec3Dot(&P,&L);

	this->right = R;
	this->up = U;
	this->look = L;

	view(0,0) = this->right.x;
	view(1,0) = this->right.y;
	view(2,0) = this->right.z;
	view(3,0) = x;

	view(0,1) = this->up.x;
	view(1,1) = this->up.y;
	view(2,1) = this->up.z;
	view(3,1) = y;

	view(0,2) = this->look.x;
	view(1,2) = this->look.y;
	view(2,2) = this->look.z;
	view(3,2) = z;

	view(0,3) = 0.0f;
	view(1,3) = 0.0f;
	view(2,3) = 0.0f;
	view(3,3) = 1.0f;
}





//debug

void Camera::setPos(D3DXVECTOR3 position)
{
	this->position = position;
}




