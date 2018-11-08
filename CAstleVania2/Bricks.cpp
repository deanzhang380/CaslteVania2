#include "Bricks.h"

void CBrick::Render(int CameraX, int CameraY)
{
	animations[0]->Render(x-CameraX, y-CameraY);
	//RenderBoundingBox();
}

void CBrick::GetBoundingBox(float &l, float &t, float &r, float &b)
{
	l = x;
	t = y;
	r = x + BRICK_BBOX_WIDTH;
	b = y + BRICK_BBOX_HEIGHT;
}