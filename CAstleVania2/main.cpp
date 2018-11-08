
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "debug.h"
#include "Game.h"
#include "GameObject.h"
#include "Textures.h"
#include"Bricks.h"
#include "Simon.h"
#include "Ropes.h"
#include "defines.h"
#include "TileMap.h"
#include "Torch.h"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(255, 255, 200)

#define WINDOW_CLASS_NAME L"SampleWindow"
#define MAIN_WINDOW_TITLE L"Castlevania"

#define PATH_MARIO_SPRITE L"textures\\mario.txt"
#define PATH_MARIO_ANI	L"textures\\mario_ani.txt"
#define PATH_BACKGROUND L"textures\\Map1_1.bmp"
#define PATH_SIMON_SPRITE L"textures\\simon.txt"
#define PATH_SIMON_SPRITE_LEFT L"textures\\simon2.txt"
#define PATH_SIMON_ANI L"textures\\simon_ani.txt"

#define MAX_FRAME_RATE 120

#define ID_TEX_MARIO 0
#define ID_TEX_ENEMY 10
#define ID_TEX_MISC 20
#define ID_TEX_SIMON			100
#define ID_TEX_BIG_CANDLES		5000
#define ID_TEX_SMALL_CANDLES	5100

	CGame *game;

	//CMario *mario;
	Simon *simon;
	Ropes *ropes;
	DWORD dt;
	vector<LPGAMEOBJECT> objects;
	TileMap *tilemap;
	
	class CSampleKeyHander : public CKeyEventHandler
	{
		virtual void KeyState(BYTE *states);
		virtual void OnKeyDown(int KeyCode);
		virtual void OnKeyUp(int KeyCode);
	};

	CSampleKeyHander * keyHandler;

	void CSampleKeyHander::OnKeyDown(int KeyCode)
	{
		DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
		switch (KeyCode)
		{
		case DIK_SPACE:
			simon->SetState(SIMON_STATE_JUMP);
			break;
		
		case DIK_X:
			simon->SetState(SIMON_STATE_ATTACK);
			
			break;
		}
	}

	void CSampleKeyHander::OnKeyUp(int KeyCode)
	{
		DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
		switch (KeyCode)
		{
		case DIK_DOWN:
			simon->is_sit = false;
		}
	}

	void CSampleKeyHander::KeyState(BYTE *states)
	{
		if (game->IsKeyDown(DIK_RIGHT))
		{
			simon->SetState(SIMON_STATE_WALKING_RIGHT);
			
		}
		else if (game->IsKeyDown(DIK_LEFT))
		{
			simon->SetState(SIMON_STATE_WALKING_LEFT);
			
		}
		else if (game->IsKeyDown(DIK_DOWN)) {
			simon->SetState(SIMON_STATE_SIT_RIGHT);
			tilemap->SetViewportX(1);
		}
		else
			simon->SetState(SIMON_STATE_IDLE);

	}

	LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		return 0;
	}

	/*
	Load all game resources
	In this example: load textures, sprites, animations and mario object

	TO-DO: Improve this function by loading texture,sprite,animation,object from file
	*/
	void LoadResources()
	{
		CTextures * textures = CTextures::GetInstance();
		textures->Add(ID_TEX_SIMON, L"textures\\simon-belmont.png", D3DCOLOR_XRGB(255, 255, 255));
		textures->Add(ID_TEX_SIMON_ATTACK, L"textures\\simon_attack.png", D3DCOLOR_XRGB(255, 255, 255));
		textures->Add(ID_TORCH, L"textures\\BGObject.png", D3DCOLOR_XRGB(34,177,76));
		CSprites * sprites = CSprites::GetInstance();
		CAnimations * animations = CAnimations::GetInstance();

		//LPDIRECT3DTEXTURE9 texMario = textures->Get(ID_TEX_MARIO);
		LPDIRECT3DTEXTURE9 texSimon = textures->Get(ID_TEX_SIMON);
		LPDIRECT3DTEXTURE9 texSimon_attack = textures->Get(ID_TEX_SIMON_ATTACK);
		LPDIRECT3DTEXTURE9 textorch = textures->Get(ID_TORCH);
		//walk right
		sprites->Add(10001, 149, 41, 149 + 46, 41 + 31, texSimon);//face right
		sprites->Add(10002, 187, 41, 187+46, 41+31, texSimon);
		sprites->Add(10003, 229, 41, 229+ 46, 41+31, texSimon);

		//walk left
		sprites->Add(10011, 109, 41, 109+46, 41+31, texSimon);//face left
		sprites->Add(10012, 69, 41, 69+46,41+31, texSimon);
		sprites->Add(10013, 29, 41, 29+46, 41+31, texSimon);

		//Sit
		sprites->Add(10004, 314, 217, 314 + 46, 217 + 31, texSimon_attack);//jump right
		sprites->Add(10005, 391, 219, 391 + 46, 219 + 31, texSimon_attack);//jump left

		//Die
		sprites->Add(10006, 235, -11, 235 + 34, 24, texSimon); //die right
		sprites->Add(10007, 35, -11, 35 + 34, 24, texSimon); //die left

		 //jump
		sprites->Add(10021, 314, 217, 314 + 46, 217 + 31, texSimon_attack);//jump right
		sprites->Add(10022, 391, 219, 391 + 46, 219+31, texSimon_attack);//jump left

		 //attack
		sprites->Add(10041, 312, 23, 312 + 46, 23+31, texSimon_attack);//attack right
		sprites->Add(10042, 313, 95, 313 + 46, 95+31, texSimon_attack);
		sprites->Add(10043, 314, 168, 314 + 56, 168+31, texSimon_attack);

		sprites->Add(10044, 443,22, 443+46, 22+31, texSimon_attack);//attack left
		sprites->Add(10045, 442, 94, 442+46,94+31, texSimon_attack);
		sprites->Add(10046, 441, 167, 441+46, 167+31, texSimon_attack);

		//Torch
		sprites->Add(40001, 47, 25, 47+18, 24+32, textorch);
		sprites->Add(40002, 74, 25, 74+18, 24+32, textorch);
		//Ropes 1
		/*sprites->Add(40001, 41, 35, 41+24, 35+28, texSimon_attack);
		sprites->Add(40002, 44, 86, 44+24, 86 + 28, texSimon_attack);
		sprites->Add(40003, 43,129,43 + 24,129 + 28, texSimon_attack);*/
		LPANIMATION ani;
		ani = new CAnimation(200);//face right
		ani->Add(10001);
		animations->Add(0, ani);

		ani = new CAnimation(200);//face left
		ani->Add(10011);
		animations->Add(1, ani);

		ani = new CAnimation(200);//walk right
		ani->Add(10001);
		ani->Add(10002);
		ani->Add(10003);
		ani->Add(10002);
		animations->Add(2, ani);

		ani = new CAnimation(200);//walk left
		ani->Add(10011);
		ani->Add(10012);
		ani->Add(10013);
		ani->Add(10012);
		animations->Add(3, ani);

		ani = new CAnimation(200);//sit right
		ani->Add(10004);
		animations->Add(8, ani);

		ani = new CAnimation(200);//sit left
		ani->Add(10005);
		animations->Add(9, ani);

		ani = new CAnimation(200);//die right
		ani->Add(10006);
		animations->Add(10, ani);

		ani = new CAnimation(200);//die left
		ani->Add(10007);
		animations->Add(11, ani);

		ani = new CAnimation(500);//jump left
		ani->Add(10022);
		animations->Add(5, ani);

		ani = new CAnimation(500);//jump right
		ani->Add(10021);
		animations->Add(4, ani);

		ani = new CAnimation(150); //attack right
		ani->Add(10041);
		ani->Add(10042);
		ani->Add(10043);
		animations->Add(6, ani);

		ani = new CAnimation(150); //attack left
		ani->Add(10044);
		ani->Add(10045);
		ani->Add(10046);
		animations->Add(7, ani);

		ani = new CAnimation(300);//zombie walk
		ani->Add(20001);
		ani->Add(20002);
		animations->Add(701, ani);

		ani = new CAnimation(150);
		ani->Add(40001);
		ani->Add(40002);
		ani->Add(40003);
		animations->Add(301, ani);

		LPDIRECT3DTEXTURE9 texMisc = textures->Get(ID_TEX_MISC);
		sprites->Add(20001, 408, 225, 424, 241, texMisc);

	
		ani = new CAnimation(100);		// brick
		ani->Add(20001);
		animations->Add(601, ani);

		ani = new CAnimation(300);		// Goomba walk
		ani->Add(30001);
		ani->Add(30002);
		animations->Add(701, ani);

		ani = new CAnimation(1000);		// Goomba dead
		ani->Add(30003);
		animations->Add(702, ani);

		ani = new CAnimation(200);
		ani->Add(40001);
		ani->Add(40002);
		animations->Add(602, ani);

	
		simon = new Simon();
		simon->AddAnimation(0);
		simon->AddAnimation(1);
		simon->AddAnimation(2);
		simon->AddAnimation(3);
		simon->AddAnimation(4);
		simon->AddAnimation(5);
		simon->AddAnimation(6);
		simon->AddAnimation(7);
		simon->AddAnimation(8);
		simon->AddAnimation(9);
		simon->AddAnimation(10);
		simon->AddAnimation(11);
		simon->SetPosition(50.0f, 0);
		objects.push_back(simon);

		
		for (int i = 0; i < 30; i++)
		{
			CBrick *brick = new CBrick();
			brick->AddAnimation(601);
			brick->SetPosition(0 + i * 16.0f, 105);
			objects.push_back(brick);
		}

		Torch *torch = new Torch();
		torch->AddAnimation(602);
		torch->SetPosition(342, 85);
		objects.push_back(torch);

	}

	
	void Update(DWORD dt)
	{
		
		vector<LPGAMEOBJECT> coObjects;
		for (int i = 1; i < objects.size(); i++)
		{
			coObjects.push_back(objects[i]);
		}

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->Update(dt, &coObjects);
		}
		
	
	}

	/*
	Render a frame
	*/
	void Render()
	{
		LPDIRECT3DDEVICE9 d3ddv = game->GetDirect3DDevice();
		LPDIRECT3DSURFACE9 bb = game->GetBackBuffer();
		LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();
		tilemap->UpdateScrollPosition();
		if (d3ddv->BeginScene())
		{
			d3ddv->ColorFill(bb, NULL, BACKGROUND_COLOR);
			d3ddv->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 255, 3660, 100), 0.1f, 0);
			tilemap->DrawScrollWindow();
			

			spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);
			for (int i = 0; i < objects.size(); i++)
				objects[i]->Render(tilemap->GetViewportX(),tilemap->GetViewPortY());
			

			spriteHandler->End();

			d3ddv->EndScene();
			d3ddv->Present(NULL, NULL, NULL, NULL);
		}


	}

	HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
	{
		WNDCLASSEX wc;
		wc.cbSize = sizeof(WNDCLASSEX);

		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.hInstance = hInstance;

		wc.lpfnWndProc = (WNDPROC)WinProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = WINDOW_CLASS_NAME;
		wc.hIconSm = NULL;

		RegisterClassEx(&wc);

		HWND hWnd =
			CreateWindow(
				WINDOW_CLASS_NAME,
				MAIN_WINDOW_TITLE,
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				ScreenWidth,
				ScreenHeight,
				NULL,
				NULL,
				hInstance,
				NULL);

		if (!hWnd)
		{
			OutputDebugString(L"[ERROR] CreateWindow failed");
			DWORD ErrCode = GetLastError();
			return FALSE;
		}

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);

		return hWnd;
	}

	int Run()
	{
		MSG msg;
		int done = 0;
		DWORD frameStart = GetTickCount();
		DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

		while (!done)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT) done = 1;

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			DWORD now = GetTickCount();

			dt = now - frameStart;

			if (dt >= tickPerFrame)
			{
				frameStart = now;

				game->ProcessKeyboard();

				Update(dt);
				Render();
			}
			else
				Sleep(tickPerFrame - dt);
		}

		return 1;
	}

	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

		game = CGame::GetInstance();
		game->Init(hWnd);

		keyHandler = new CSampleKeyHander();
		game->InitKeyboard(keyHandler);

		tilemap = new TileMap(game->GetDirect3DDevice(), game->GetBackBuffer(), L"textures\\map1_1.bmp");
		LoadResources();

		SetWindowPos(hWnd, 0, 0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

		Run();

		return 0;
	}
