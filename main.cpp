
#include "DxLib.h"


#define GAME_WIDTH 800    
#define GAME_HEIGHT 600
#define GAME_COLOR 32



#define GAME_WINDOW_NAME "GAME_TITLE"
#define GAME_WINDOW_MODECHANGE	TRUE

#define GAME_FPS_SPEED                       60

#define SET_WINDOW_ST_MODE_DEFAULT			0
#define SET_WINDOW_ST_MODE_TITLE_NONE		1
#define SET_WINDOW_ST_MODE_TITLE_FLAME_NONE 2
#define SET_WINDOW_ST_MODE_FLAME_NONE		3

#define GAME_BACKIMAGE_1	"BACKIMAGE\\oruga1.jpg"
#define GAME_BACKIMAGE_2	
#define GAME_BACKIMAGE_3 

enum GAME_SCENE {
	GAME_SCENE_TITLE,
	GAME_SCENE_PLAY,
	GAME_SCENE_END
};

struct STRUCT_GAZOU {
	int Handle;
	char FilePath[128];
	int X;
	int Y;
	int Width;
	int Height;
	int C_Width;
	int C_Height;
};


typedef STRUCT_GAZOU GAZOU;

GAZOU	BackGround; //背景の画像
GAZOU   Player;      //プレイヤーの画像


WNDPROC WndProc;
BOOL IsWM_CREATE = FALSE;

int StartTimeFps;
int CountFps;
float CalcFps;
int SampleNumFps = GAME_FPS_SPEED;

char AllKeyState[256];

int GameSceneNow = (int)GAME_SCENE_TITLE; //最初のゲーム画面をタイトルに設定　

LRESULT CALLBACK MY_WNDPROC(HWND, UINT, WPARAM, LPARAM);

VOID MY_FPS_UPDATE(VOID);
VOID MY_FPS_DRAW(VOID);
VOID MY_FPS_WAIT(VOID);

VOID MY_ALL_KEYDOWN_UPDATE(VOID);

VOID MY_GAME_TITLE(VOID);
VOID MY_GAME_PLAY(VOID);
VOID MY_GAME_END(VOID);

VOID MY_DRAW_STRING_CENTER_CENTER(char[][128], int, char *, int);
VOID MY_DRAW_SET_DEFAULT_FONT(BOOL);



BOOL MY_GAZOU_LOAD(GAZOU *, int, int, const char*);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	ChangeWindowMode(TRUE);

	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);

	SetWindowStyleMode(SET_WINDOW_ST_MODE_DEFAULT);

	SetMainWindowText(TEXT(GAME_WINDOW_NAME));


	if (DxLib_Init() == -1) {
		return -1;
	}

	SetDrawScreen(DX_SCREEN_BACK);

	if (MY_GAZOU_LOAD(&BackGround, 0, 0, GAME_BACKIMAGE_1) == FALSE) { MessageBox(NULL, GAME_BACKIMAGE_1, "NotFound", MB_OK); return -1; }

	while (TRUE)
	{
		if (ProcessMessage() != 0)
		{
			break;
		}

		if (ClearDrawScreen() != 0) {
			break;
		}

		MY_ALL_KEYDOWN_UPDATE();


		switch (GameSceneNow)
		{
		case(int)GAME_SCENE_TITLE:

			MY_GAME_TITLE();

			break;

		case (int)GAME_SCENE_PLAY:

			MY_GAME_PLAY();

			break;

		case (int)GAME_SCENE_END:

			MY_GAME_END();

			break;

		default:

			break;
		}

		

		MY_FPS_UPDATE();

		MY_FPS_DRAW();

		ScreenFlip();

		MY_FPS_WAIT();

	}

	DeleteGraph(BackGround.Handle);

	DxLib_End();

	return 0;
}



VOID MY_FPS_UPDATE(VOID)
{
	if (CountFps == 0)
	{
		StartTimeFps = GetNowCount();
	}

	if (CountFps == SampleNumFps)
	{
		int now = GetNowCount();
		CalcFps = 1000.f / ((now - StartTimeFps) / (float)SampleNumFps);
		CountFps = 0;
		StartTimeFps = now;
	}
	CountFps++;
	return;
}

VOID MY_FPS_DRAW(VOID)
{
	DrawFormatString(0, GAME_HEIGHT - 20, GetColor(255, 255, 255), "FPS:%.1f", CalcFps);
	return;
}

VOID MY_FPS_WAIT(VOID)
{
	int resultTime = GetNowCount() - StartTimeFps;
	int waitTime = CountFps * 1000 / GAME_FPS_SPEED - resultTime;

	if (waitTime > 0)
	{
		Sleep(waitTime);
	}
	return;

}

VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{
	char TempKey[256];

	GetHitKeyStateAll(TempKey);

	for (int i = 0; i < 256; i++)
	{
		if (TempKey[i] != 0)
		{
			AllKeyState[i]++;
		}
		else
		{
			AllKeyState[i] = 0;
		}
	}
	return;
}


VOID MY_GAME_TITLE(VOID)
{
	if (AllKeyState[KEY_INPUT_SPACE] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_PLAY;
	}

	//DrawGraph(250, 230, BackGround.Handle, FALSE);
	
	//BackGround.Handle = DrawGraph(250, 230, BackGround.Handle, FALSE);

	GetGraphSize(BackGround.Handle, &BackGround.X, &BackGround.Y);

	DrawExtendGraph(100, 150, 100 + BackGround.X * 2, 150 + BackGround.Y*2, BackGround.Handle, TRUE);
	
	char StrGameTitle[1][128] = { "Don't Stop" };
	char StrFontTitle[128] = { "MS ゴシック" };
	
	MY_DRAW_STRING_CENTER_CENTER(&StrGameTitle[0], 1, StrFontTitle, 64);


	DrawString(0, 0, "プレイ画面(スペースキーを押してください)", GetColor(255, 255, 255));

	return;
}

VOID MY_GAME_PLAY(VOID)
{
	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_END;
	}

	DrawString(0, 0, "プレイ画面(バックスペースキーを押してください)", GetColor(255, 255, 255));
}

VOID MY_GAME_END(VOID)
{
	if (AllKeyState[KEY_INPUT_RETURN] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}

	DrawString(0, 0, "タイトル画面(エンターキーを押してください)", GetColor(255, 255, 255));

	return;
}

//画像の表示設定

BOOL MY_GAZOU_LOAD(GAZOU *g, int x, int y, const char *path)
{
	wsprintf(g->FilePath, path);
	g->Handle = LoadGraph(g->FilePath);

	if (g->Handle == -1) {
		return FALSE;
	}

	GetGraphSize(g->Handle, &g->Width, &g->Height);
	g->X = x;
	g->Y = y;
	g->C_Width = g->Width / 2;
	g->C_Height = g->Height / 2;

	return TRUE;
}

//文字を中央に描画

VOID MY_DRAW_STRING_CENTER_CENTER(char str[][128], int row, char *fontname, int size)
{
	ChangeFont(fontname, DX_CHARSET_DEFAULT);
	SetFontSize(size);
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);

	int StrHeightSum = 0;
	for (int sum = 0; sum < row; sum++)
	{
		StrHeightSum += GetFontSize();
	}

	for (int rCnt = 0; rCnt < row; rCnt++)
	{
		int StrWidth = GetDrawFormatStringWidth(&str[rCnt][0]);
		int StrHeight = GetFontSize();

		DrawFormatString(
			GAME_WIDTH / 2 - StrWidth / 2,
			GAME_HEIGHT / 2 - StrHeightSum + StrHeight * rCnt,
			GetColor(255, 255, 255), &str[rCnt][0]);
	}

	MY_DRAW_SET_DEFAULT_FONT(TRUE);

	return;
}


//フォント設定

VOID MY_DRAW_SET_DEFAULT_FONT(BOOL anti)
{
	ChangeFont("MS ゴシック", DX_CHARSET_DEFAULT);
	SetFontSize(16);

	if (anti)
	{
		ChangeFontType(DX_FONTTYPE_ANTIALIASING);
	}
	return;
}

LRESULT CALLBACK MY_WNDPROC(HWND hwnd, UINT mgs, WPARAM wp, LPARAM lp)
{
	switch (mgs)
	{

	case WM_CREATE:

		IsWM_CREATE = TRUE;

		return 0;

	case WM_CLOSE:

		MessageBox(hwnd, TEXT("ゲームを終了します。"), TEXT("終了メッセージ"), MB_OK);
		break;

	case WM_RBUTTONDOWN:

		SendMessage(hwnd, WM_CLOSE, 0, 0);
		break;

	case WM_LBUTTONDOWN:

		PostMessage(hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, lp);
		break;

	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, mgs, wp, lp);
}