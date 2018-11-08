#include "Torch.h"



Torch::Torch()
{
}


Torch::~Torch()
{
}

void Torch::Render(int CameraX, int CameraY)
{
	animations[0]->Render(x - CameraX, y - CameraY);
}

void Torch::GetBoundingBox(float & l, float & t, float & r, float & b)
{
	l = x;
	t = y;
	r = x + 18;
	b = y + 32;
}
