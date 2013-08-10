#ifndef INPUT_H
#define INPUT_H

#include "stdafx.h"

class Input
{
	private:
		D3DXVECTOR3 prevMousePos;

	public:
		Input();
		~Input(){};

		float mousePitch(WPARAM btnState, int x, int y);
		float mouseRotateY(WPARAM btnState, int x, int y);
		bool checkKeyDown(char input) const;
};

#endif