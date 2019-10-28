#include "DxLib.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1)
		return -1;

	DrawBox(0, 0, 100, 100, 0xFFFFFF, TRUE);

	WaitKey();

	DxLib_End();
	return 0;
}