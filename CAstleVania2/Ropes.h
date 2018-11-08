#pragma once
#include "GameObject.h"
#include "defines.h"
class Ropes: public CGameObject
{
	int type = 0;
public:
	Ropes();
	Ropes(int x, int y);
	~Ropes();
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
	virtual void Render(int CameraX, int CameraY);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void SetState(int state);
	void LoadResource();
	void SetPosition(int x, int y);

};

