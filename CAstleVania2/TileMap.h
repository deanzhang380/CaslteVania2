#pragma once
#include<d3dx9.h>
#include<d3d9.h>
#include"defines.h"

class TileMap
{
	int SCREENW = SCREEN_WIDTH;
	int SCREENH = SCREEN_HEIGHT;

	//settings for the scroller
	const int TILEWIDTH = 32;
	const int TILEHEIGHT = 32;
	const int MAPWIDTH = 24;
	const int MAPHEIGHT = 6;

	//scrolling window size
	const int WINDOWWIDTH = (SCREENW / TILEWIDTH) * TILEWIDTH;
	const int WINDOWHEIGHT = (SCREENH / TILEHEIGHT) * TILEHEIGHT;
	LPCWSTR filename;
	int ViewPortX, ViewPortY;
	int SpeedX, SpeedY;
	long start;
	LPDIRECT3DSURFACE9 scrollbuffer = NULL;
	LPDIRECT3DSURFACE9 tiles = NULL;
	LPDIRECT3DDEVICE9 d3ddv = NULL;				// Direct3D device object

	LPDIRECT3DSURFACE9 backBuffer = NULL;
	

	int MAPDATA[24 * 6] = {
		1, 2, 2, 2, 2, 2, 3, 3, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 5, 5, 6,
		7, 8, 7, 7, 7, 7, 2, 2, 7, 8, 7, 7, 7, 7, 2, 2, 2, 2, 9, 9, 10, 11, 12, 13,
		8, 14, 8, 8, 8, 8, 8, 8, 8, 14, 8, 8, 8, 8, 8, 8, 8, 8, 14, 14, 15, 16, 12, 6,
		17, 18, 14, 17, 17, 14, 17, 14, 17, 18, 14, 17, 17, 14, 17, 14, 14, 14, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 24, 24, 26, 24, 26, 24, 25, 26, 24, 24, 26, 24, 26, 26, 27, 25, 28, 29, 30, 31, 32,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

public:
	TileMap(LPDIRECT3DDEVICE9 dv, LPDIRECT3DSURFACE9 bb, LPCWSTR filename);
	TileMap(LPCWSTR filename);
	~TileMap();
	void DrawTile(LPDIRECT3DSURFACE9 source, // source surface image
		int tilenum, // tile #
		int width, // tile width
		int height, // tile height
		int columns, // columns of tiles
		LPDIRECT3DSURFACE9 dest, // destination surface
		int destx, // destination x
		int desty // destination y
		);
	LPDIRECT3DSURFACE9 LoadSurface(LPCWSTR filename);
	void DrawTiles();

	void DrawScrollWindow(bool scaled = false);
	void ViewPort() {}
	void UpdateScrollPosition();
	void SetViewPortY(float y) { ViewPortY += y; }
	void SetViewportX(float x) { ViewPortX += x; }
	float GetViewportX() { return ViewPortX; }
	float GetViewPortY() { return ViewPortY; }
};

