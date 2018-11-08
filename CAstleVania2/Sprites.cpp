#include "Sprites.h"
#include "Game.h"
#include "debug.h"

#include <fstream>
#include <string>

CSprite::CSprite(int id, int left, int top, int right, int bottom, LPDIRECT3DTEXTURE9 tex)
{
	this->id = id;
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
	this->texture = tex;
}

CSprites * CSprites::__instance = NULL;

//Get the information of the Sprites from txt file.
void CSprites::AddFromFile(LPDIRECT3DTEXTURE9 tex, LPCWSTR filePath)
{
	ifstream file(filePath);
	string line;
	if (!file)
	{
		DebugOut(L"[ERROR] GetInfoFromFile failed: %s\n", filePath);
	}
	while (getline(file, line))	//Passing the first line (information line.)
	{
		string name;						//name of the frame.
		int id, left, top, right, bottom;	//Id, posittion of the frame
		file >> name >> id >> left >> top >> right >> bottom;
		this->Add(id, left, top, right, bottom, tex);
	}

	file.close();
}

CSprites *CSprites::GetInstance()
{
	if (__instance == NULL) __instance = new CSprites();
	return __instance;
}

void CSprite::Draw(float x, float y, int alpha)
{
	CGame * game = CGame::GetInstance();
	game->Draw(x, y, texture, left, top, right, bottom, alpha);
}

void CSprites::Add(int id, int left, int top, int right, int bottom, LPDIRECT3DTEXTURE9 tex)
{
	LPSPRITE s = new CSprite(id, left, top, right, bottom, tex);
	sprites[id] = s;
}

LPSPRITE CSprites::Get(int id)
{
	return sprites[id];
}



void CAnimation::Add(int spriteId, DWORD time)
{
	int t = time;
	if (time == 0) t = this->defaultTime;

	LPSPRITE sprite = CSprites::GetInstance()->Get(spriteId);
	LPANIMATION_FRAME frame = new CAnimationFrame(sprite, t);
	frames.push_back(frame);
}

void CAnimation::Render(float x, float y, int alpha)
{
	DWORD now = GetTickCount();
	if (currentFrame == -1)
	{
		currentFrame = 0;
		lastFrameTime = now;
	}
	else
	{
		DWORD t = frames[currentFrame]->GetTime();
		if (now - lastFrameTime > t)
		{
			currentFrame++;
			lastFrameTime = now;
			if (currentFrame == frames.size()) currentFrame = 0;
		}

	}

	frames[currentFrame]->GetSprite()->Draw(x, y, alpha);
}

CAnimations * CAnimations::__instance = NULL;

CAnimations * CAnimations::GetInstance()
{
	if (__instance == NULL) __instance = new CAnimations();
	return __instance;
}

void CAnimations::Add(int id, LPANIMATION ani)
{
	animations[id] = ani;
}

void CAnimations::AddFromFile(LPCWSTR filePath)
{
	LPANIMATION ani = new CAnimation(0); // initialize to prevent error.
	ifstream file(filePath);
	string line;
	if (!file)
	{
		DebugOut(L"[ERROR] GetInfoFromFile failed: %s\n", filePath);
	}
	int oldID = 0;
	bool flag = false;

	while (getline(file, line))
	{
		string name;
		int sID, aniID, time;

		file >> name >> sID >> aniID >> time;

		if (aniID != oldID)
		{
			if (oldID > 0)
				this->Add(oldID, ani);
			ani = new CAnimation(time);
			ani->Add(sID);
			oldID = aniID;
		}
		else
		{
			ani->Add(sID);
			oldID = aniID;
		}
	}

	file.close();
}

LPANIMATION CAnimations::Get(int id)
{
	return animations[id];
}