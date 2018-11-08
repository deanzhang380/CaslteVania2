#include "Ropes.h"



Ropes::Ropes()
{
	x = 0;
	y = 0;
}


Ropes::Ropes(int wx, int wy)
{
	this->x = wx+22;
	this->y = wy+9;
}


Ropes::~Ropes()
{
	
}

void Ropes::GetBoundingBox(float & l, float & t, float & r, float & b)
{
	l = x;
	t = y;
	r = x + 25;
	b = y + 12;
}

void Ropes::Render(int CameraX, int CameraY)
{

}

void Ropes::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);
}

void Ropes::SetState(int state)
{
	CGameObject::SetState(state);
	
}

void Ropes::LoadResource()
{

}

void Ropes::SetPosition(int wx, int wy)
{
	x = wx;
	y = wy;
}
