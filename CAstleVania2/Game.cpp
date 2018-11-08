#include "Game.h"


CGame * CGame::__instance = NULL;

/*
Initialize DirectX, create a Direct3D device for rendering within the window, initial Sprite library for
rendering 2D images
- hInst: Application instance handle
- hWnd: Application window handle
*/
void CGame::Init(HWND hWnd)
{
	LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);

	this->hWnd = hWnd;

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;

	RECT r;
	GetClientRect(hWnd, &r);	// retrieve Window width & height 

	d3dpp.BackBufferHeight = r.bottom + 1;
	d3dpp.BackBufferWidth = r.right + 1;

	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddv);

	if (d3ddv == NULL)
	{
		OutputDebugString(L"[ERROR] CreateDevice failed\n");
		return;
	}

	d3ddv->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

	// Initialize sprite helper from Direct3DX helper library
	D3DXCreateSprite(d3ddv, &spriteHandler);

	//Background
	//GAMEWORLDWIDTH = TILEWIDTH * MAPWIDTH;
	//GAMEWORLDHEIGHT = TILEHEIGHT * MAPHEIGHT;
	////load the tile images
	//tiles = LoadSurface(L"textures\\map1_1.bmp");
	//if (!tiles) return;
	////create the scroll buffer surface in memory, slightly bigger
	////than the screen
	//const int SCROLLBUFFERWIDTH = SCREEN_WIDTH + TILEWIDTH * 2;
	//const int SCROLLBUFFERHEIGHT = SCREEN_HEIGHT + TILEHEIGHT * 2;
	//HRESULT result = d3ddv->CreateOffscreenPlainSurface(
	//	SCROLLBUFFERWIDTH, SCROLLBUFFERHEIGHT,
	//	D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT,
	//	&scrollbuffer,
	//	NULL);
	//if (result != S_OK) return;

	//create pointer to the back buffer
	//d3ddv->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &scrollbuffer);

	//load the tile images

	OutputDebugString(L"[INFO] InitGame done;\n");
}

/*
Utility function to wrap LPD3DXSPRITE::Draw
*/
void CGame::Draw(float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom, int alpha)
{
	D3DXVECTOR3 p(x, y, 0);
	RECT r;
	r.left = left;
	r.top = top;
	r.right = right;
	r.bottom = bottom;
	spriteHandler->Draw(texture, &r, NULL, &p, D3DCOLOR_ARGB(alpha, 255, 255, 255));
}

int CGame::IsKeyDown(int KeyCode)
{
	return (keyStates[KeyCode] & 0x80) > 0;
}

void CGame::InitKeyboard(LPKEYEVENTHANDLER handler)
{
	HRESULT
		hr = DirectInput8Create
		(
		(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			DIRECTINPUT_VERSION,
			IID_IDirectInput8, (VOID**)&di, NULL
		);

	if (hr != DI_OK)
	{
		DebugOut(L"[ERROR] DirectInput8Create failed!\n");
		return;
	}

	hr = di->CreateDevice(GUID_SysKeyboard, &didv, NULL);

	// TO-DO: put in exception handling
	if (hr != DI_OK)
	{
		DebugOut(L"[ERROR] CreateDevice failed!\n");
		return;
	}

	// Set the data format to "keyboard format" - a predefined data format 
	//
	// A data format specifies which controls on a device we
	// are interested in, and how they should be reported.
	//
	// This tells DirectInput that we will be passing an array
	// of 256 bytes to IDirectInputDevice::GetDeviceState.

	hr = didv->SetDataFormat(&c_dfDIKeyboard);

	hr = didv->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);


	// IMPORTANT STEP TO USE BUFFERED DEVICE DATA!
	//
	// DirectInput uses unbuffered I/O (buffer size = 0) by default.
	// If you want to read buffered data, you need to set a nonzero
	// buffer size.
	//
	// Set the buffer size to DINPUT_BUFFERSIZE (defined above) elements.
	//
	// The buffer size is a DWORD property associated with the device.
	DIPROPDWORD dipdw;

	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = KEYBOARD_BUFFER_SIZE; // Arbitary buffer size

	hr = didv->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

	hr = didv->Acquire();
	if (hr != DI_OK)
	{
		DebugOut(L"[ERROR] DINPUT8::Acquire failed!\n");
		return;
	}

	this->keyHandler = handler;

	DebugOut(L"[INFO] Keyboard has been initialized successfully\n");
}

void CGame::ProcessKeyboard()
{
	HRESULT hr;

	// Collect all key states first
	hr = didv->GetDeviceState(sizeof(keyStates), keyStates);
	if (FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			HRESULT h = didv->Acquire();
			if (h == DI_OK)
			{
				DebugOut(L"[INFO] Keyboard re-acquired!\n");
			}
			else return;
		}
		else
		{
			//DebugOut(L"[ERROR] DINPUT::GetDeviceState failed. Error: %d\n", hr);
			return;
		}
	}

	keyHandler->KeyState((BYTE *)&keyStates);



	// Collect all buffered events
	DWORD dwElements = KEYBOARD_BUFFER_SIZE;
	hr = didv->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), keyEvents, &dwElements, 0);
	if (FAILED(hr))
	{
		//DebugOut(L"[ERROR] DINPUT::GetDeviceData failed. Error: %d\n", hr);
		return;
	}

	// Scan through all buffered events, check if the key is pressed or released
	for (DWORD i = 0; i < dwElements; i++)
	{
		int KeyCode = keyEvents[i].dwOfs;
		int KeyState = keyEvents[i].dwData;
		if ((KeyState & 0x80) > 0)
			keyHandler->OnKeyDown(KeyCode);
		else
			keyHandler->OnKeyUp(KeyCode);
	}
}

CGame::~CGame()
{
	if (spriteHandler != NULL) spriteHandler->Release();
	if (backBuffer != NULL) backBuffer->Release();
	if (d3ddv != NULL) d3ddv->Release();
	if (d3d != NULL) d3d->Release();
}

/*
SweptAABB
*/
void CGame::SweptAABB(
	float ml, float mt, float mr, float mb,
	float dx, float dy,
	float sl, float st, float sr, float sb,
	float &t, float &nx, float &ny)
{

	float dx_entry, dx_exit, tx_entry, tx_exit;
	float dy_entry, dy_exit, ty_entry, ty_exit;

	float t_entry;
	float t_exit;

	t = -1.0f;			// no collision
	nx = ny = 0;

	//
	// Broad-phase test 
	//

	float bl = dx > 0 ? ml : ml + dx;
	float bt = dy > 0 ? mt : mt + dy;
	float br = dx > 0 ? mr + dx : mr;
	float bb = dy > 0 ? mb + dy : mb;

	if (br < sl || bl > sr || bb < st || bt > sb) return;


	if (dx == 0 && dy == 0) return;		// moving object is not moving > obvious no collision

	if (dx > 0)
	{
		dx_entry = sl - mr;
		dx_exit = sr - ml;
	}
	else if (dx < 0)
	{
		dx_entry = sr - ml;
		dx_exit = sl - mr;
	}


	if (dy > 0)
	{
		dy_entry = st - mb;
		dy_exit = sb - mt;
	}
	else if (dy < 0)
	{
		dy_entry = sb - mt;
		dy_exit = st - mb;
	}

	if (dx == 0)
	{
		tx_entry = -99999999999;
		tx_exit = 99999999999;
	}
	else
	{
		tx_entry = dx_entry / dx;
		tx_exit = dx_exit / dx;
	}

	if (dy == 0)
	{
		ty_entry = -99999999999;
		ty_exit = 99999999999;
	}
	else
	{
		ty_entry = dy_entry / dy;
		ty_exit = dy_exit / dy;
	}


	if ((tx_entry < 0.0f && ty_entry < 0.0f) || tx_entry > 1.0f || ty_entry > 1.0f) return;

	t_entry = max(tx_entry, ty_entry);
	t_exit = min(tx_exit, ty_exit);

	if (t_entry > t_exit) return;

	t = t_entry;

	if (tx_entry > ty_entry)
	{
		ny = 0.0f;
		dx > 0 ? nx = -1.0f : nx = 1.0f;
	}
	else
	{
		nx = 0.0f;
		dy > 0 ? ny = -1.0f : ny = 1.0f;
	}

}

CGame *CGame::GetInstance()
{
	if (__instance == NULL) __instance = new CGame();
	return __instance;
}


//void CGame::UpdateScrollPosition()
//{
//	//update horizontal scrolling position and speed
//	//ScrollX += SpeedX;
//	if (ScrollX < 0)
//	{
//		ScrollX = 0;
//		SpeedX = 0;
//	}
//	else if (ScrollX > GAMEWORLDWIDTH - WINDOWWIDTH)
//	{
//		ScrollX = GAMEWORLDWIDTH - WINDOWWIDTH;
//		SpeedX = 0;
//	}
//	//update vertical scrolling position and speed
//	//ScrollY += SpeedY;
//	if (ScrollY < 0)
//	{
//		ScrollY = 0;
//		SpeedY = 0;
//	}
//	else if (ScrollY > GAMEWORLDHEIGHT - WINDOWHEIGHT)
//	{
//		ScrollY = GAMEWORLDHEIGHT - WINDOWHEIGHT;
//		SpeedY = 0;
//	}
//}
//
//void CGame::DrawTile(
//	LPDIRECT3DSURFACE9 source, // source surface image
//	int tilenum, // tile #
//	int width, // tile width
//	int height, // tile height
//	int columns, // columns of tiles
//	LPDIRECT3DSURFACE9 dest, // destination surface
//	int destx, // destination x
//	int desty) // destination y
//{
//	//create a RECT to describe the source image
//	RECT r1;
//	r1.left = (tilenum % columns) * width;
//	r1.top = (tilenum / columns) * height;
//	r1.right = r1.left + width;
//	r1.bottom = r1.top + height;
//	//set destination rect
//	RECT r2 = { destx,desty,destx + width,desty + height };
//	//draw the tile
//	d3ddv->StretchRect(source, &r1, dest, &r2, D3DTEXF_NONE);
//}
//
//void CGame::DrawTiles()
//{
//	int tilex, tiley;
//	int columns, rows;
//	int x, y;
//	int tilenum;
//	//calculate starting tile position
//	tilex = ScrollX / TILEWIDTH;
//	tiley = ScrollY / TILEHEIGHT;
//	//calculate the number of columns and rows
//	columns = WINDOWWIDTH / TILEWIDTH;
//	rows = WINDOWHEIGHT / TILEHEIGHT;
//	//draw tiles onto the scroll buffer surface
//	for (y = 0; y <= rows; y++)
//	{
//		for (x = 0; x <= columns; x++)
//		{
//			//retrieve the tile number from this position
//			tilenum = MAPDATA[((tiley + y) * MAPWIDTH + (tilex + x))];
//			//draw the tile onto the scroll buffer
//			DrawTile(tiles, tilenum, TILEWIDTH, TILEHEIGHT, 16, backBuffer,
//				x*TILEWIDTH, y*TILEHEIGHT);
//		}
//	}
//}
//
////TO-DO: Update this func to viewport.
//void CGame::DrawScrollWindow(/*bool scaled = false*/)
//{
//	bool scaled = false;
//	//calculate the partial sub-tile lines to draw using modulus
//	int partialx = ScrollX % TILEWIDTH;
//	int partialy = ScrollY % TILEHEIGHT;
//	//set dimensions of the source image as a rectangle
//	RECT r1 = { partialx, partialy, partialx + WINDOWWIDTH - 1, partialy + WINDOWHEIGHT - 1 };
//	//set the destination rectangle
//	RECT r2;
//	if (scaled) {
//		//use this line for scaled display
//		RECT r = { 0, 0, WINDOWWIDTH - 1, WINDOWHEIGHT - 1 };
//		r2 = r;
//	}
//	else {
//		//use this line for non-scaled display
//		RECT r = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 };
//		r2 = r;
//	}
//	//draw the "partial tile" scroll window onto the back buffer
//	d3ddv->StretchRect(scrollbuffer, &r1, backBuffer, &r2,
//		D3DTEXF_NONE);
//}
//
LPDIRECT3DSURFACE9 CGame::LoadSurface(LPCWSTR filename)
{
	LPDIRECT3DSURFACE9 image = NULL;
	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename, &info); //LPCWSTR
	if (result != D3D_OK) return NULL;
	//create surface
	result = d3ddv->CreateOffscreenPlainSurface(
		info.Width, //width of the surface
		info.Height, //height of the surface
		D3DFMT_X8R8G8B8, //surface format
		D3DPOOL_DEFAULT, //memory pool to use
		&image, //pointer to the surface
		NULL); //reserved (always NULL)
	if (result != D3D_OK) return NULL;
	//load surface from file into newly created surface
	result = D3DXLoadSurfaceFromFile(
		image, //destination surface
		NULL, //destination palette
		NULL, //destination rectangle
		filename,//.c_str(), //source filename
		NULL, //source rectangle
		D3DX_DEFAULT, //controls how image is filtered
		D3DCOLOR_XRGB(0, 0, 0), //for transparency (0 for none)
		NULL); //source image info (usually NULL)
			   //make sure file was loaded okay
	if (result != D3D_OK) return NULL;
	return image;
}
