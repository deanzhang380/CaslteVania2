#pragma once
#include "GameObject.h"
#include"defines.h"

class CBrick : public CGameObject
{

public:

	virtual void Render(int CameraX,int CameraY);
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
};