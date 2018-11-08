#include "Simon.h"

void Simon::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	vy += SIMON_GRAVITY * dt;

	if (time < 0) {
		time += MAX_FRAME_RATE;
		ropes = new Ropes(this->x + 23, this->y + 9);
	}//Thoi gian song khi danh
	if (time >= 0) {
		time = 0;
		is_attack = false;
		ropes->~Ropes();
	}
	if (time_jump < 0) {
		time_jump += 10;
	}//Thoi gian tu nhay den tiep dat
	if (time_jump >= 0) {
		time_jump = 0;
		is_jumping = false;
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	//if (state != SIMON_STATE_DIE)
	CalcPotentialCollisions(coObjects, coEvents);
	if (GetTickCount() - untouchable_start > SIMON_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else {
		float min_tx, min_ty, nx = 0, ny;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);
		x += min_tx * dx + nx * 0.1f;
		y += min_ty * dy + ny * 0.1f;

		if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		//for (UINT i = 0; i < coEventsResult.size(); i++) {
		//	LPCOLLISIONEVENT e = coEventsResult[i];

		//}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void Simon::Render(int CameraX,int CameraY)
{
	int ani;

	if (time < 0 && is_sit == false && is_die == false && is_walking == false) {
		if (nx > 0) {
			ani = SIMON_STATE_ATTACK;
		}
		if (nx < 0) {
			ani = SIMON_STATE_ATTACK_LEFT;
		}
	}
	else {
		ani = SIMON_STATE_IDLE;
		if (vx == 0)
		{
			if (nx > 0) ani = SIMON_STATE_IDLE;
			else ani = SIMON_IDLE_LEFT;

			is_walking = false;
		}
		else if (vx > 0) {
			ani = SIMON_STATE_WALKING_RIGHT;
		}
		else ani = SIMON_STATE_WALKING_LEFT;

		if (vy < 0)
		{
			if (nx > 0)
			{
				ani = SIMON_STATE_JUMP;
			}
			if (nx < 0)
			{
				ani = SIMON_STATE_JUMP_LEFT;
			}
		}

	}
	if (is_sit == true) {
		if (nx > 0) {
			ani = SIMON_STATE_SIT_RIGHT;
		}
		if (nx < 0) {
			ani = SIMON_STATE_SIT_LEFT;
		}
	}
	if (is_die == true) {
		if (nx > 0) {
			ani = SIMON_STATE_DIE;
		}
		if (nx < 0) {
			ani = SIMON_STATE_DIE_LEFT;
		}
	}
	int alpha = 255;
	if (untouchable) alpha = 128;
	
	animations[ani]->Render(x-CameraX, y-CameraY, alpha);
}

void Simon::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case SIMON_STATE_WALKING_RIGHT:
		if (is_attack == false && is_sit == false) {
			vx = SIMON_WALKING_SPEED;
			nx = 1;
			is_walking = true;
		}

		break;
	case SIMON_STATE_WALKING_LEFT:
		if (is_attack == false && is_sit == false) {
			vx = -SIMON_WALKING_SPEED;
			nx = -1;
			is_walking = true;
		}

		break;
	case SIMON_STATE_JUMP:
		if (is_jumping == false) {
			time_jump = -500; //thoi gian bay den tiep dat
			vy = -SIMON_JUMP_SPEED_Y * 0.7f;//Gia toc cua nhay (vy cang nho nhay cang cao)
			is_jumping = true; //co hieu dau hieu dang nhay
		}

		break;
	case SIMON_STATE_IDLE:
		vx = 0;
		break;
	case SIMON_STATE_DIE:
		is_die = true;
		break;
	case SIMON_STATE_ATTACK:
		if (is_attack == false && is_walking == false) {
			time = -(MAX_FRAME_RATE * 30); //set thoi gian song cua dong tac danh
			is_attack = true; //co hieu trang thai tan cong	
		}
		break;
	case SIMON_STATE_ATTACK_LEFT:
		if (is_attack == false && is_walking == false) {
			time = -(MAX_FRAME_RATE * 30);
			is_attack = true;
		}
		break;
	case SIMON_STATE_SIT_RIGHT:
		is_sit = true;
		vx = 0; //khi ngoi xuong van toc cua simon tra ve 0
		break;
	}
}

void Simon::GetBoundingBox(float & left, float & top, float & right, float & bottom)
{
	left = x+15;
	top = y;

	/*if (level == SIMON_LEVEL_BIG)
	{
	right = x + SIMON_BIG_BBOX_WIDTH;
	bottom = y + SIMON_BIG_BBOX_HEIGHT;
	}
	else
	{
	right = x + SIMON_SMALL_BBOX_WIDTH;
	bottom = y + SIMON_SMALL_BBOX_HEIGHT;
	}*/

	if (state == SIMON_STATE_SIT_RIGHT)
	{
		right = x + SIMON_SITDOWN_BBOX_WIDTH;
		bottom = y + SIMON_SITDOWN_BBOX_HEIGHT;
	}

	//right = x + SIMON_STAND_BBOX_WIDTH;
	//bottom = y + SIMON_STAND_BBOX_HEIGHT;
	right = x + SIMON_STAND_BBOX_HEIGHT;
	bottom = y + SIMON_STAND_BBOX_WIDTH;
}


