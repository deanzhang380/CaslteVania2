#pragma once
#include"GameObject.h"
#include "Ropes.h"
#include"defines.h"
#include "debug.h"
class Simon :public CGameObject
{
	int time_jump = 0;
	int time = 0;
	int untouchable;
	DWORD untouchable_start;
public:
	float XCamera, YCamera;
	Ropes * ropes= new Ropes();
	bool is_walking = false;
	bool is_die = false;
	bool is_sit = false;
	bool is_jumping = false;
	bool is_attack = false;
	Simon() :CGameObject()
	{
		untouchable = 0;
	}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL);
	virtual void Render(int CameraX,int CameraY);
	void SetState(int state);

	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }

	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
};
