#include "Input.h"

Input::Input()
{
	this->prevMousePos = D3DXVECTOR3(0,0,0);
}

bool Input::checkKeyDown(char input) const
{
	if(GetAsyncKeyState(input) & 0x8000)
	{
		return true;
	}

	return false;
}

float Input::mousePitch(WPARAM btnState, int x, int y)
{
	float dy = 0;
	if((btnState & MK_LBUTTON) == 0 )
	{
		dy = D3DXToRadian(0.25f * static_cast<float>(y - this->prevMousePos.y));
	}

	this->prevMousePos.y = y;

	return dy;
}

float Input::mouseRotateY(WPARAM btnState, int x, int y)
{
	float dx = 0; 
	if((btnState & MK_LBUTTON) == 0)
	{
		dx = D3DXToRadian(0.25f * static_cast<float>(x - this->prevMousePos.x));
	}

	this->prevMousePos.x = x;

	return dx;
}
