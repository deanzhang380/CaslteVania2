#pragma once
#include "GameObject.h"

class Torch:public CGameObject
{
public:
	Torch();
	~Torch();
	virtual void Render(int CameraX, int CameraY);
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
};

