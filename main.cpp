
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

#define GAME_BACKIMAGE_1	"BACKIMAGE\\oruga1.jpg"		//�^�C�g����ʔw�i
#define GAME_BACKIMAGE_2	"BACKIMAGE\\BackImage2.jpg"	//�@�v���C��ʔw�i
#define GAME_BACKIMAGE_3    "BACKIMAGE\\BackImage3.jpg"  //�G���h��ʔw�i
#define GAME_BACKIMAGE_4    "CHARA\\stop.png"  //�G���h��ʃL����
#define GAME_GOAL			"BACKIMAGE\\Goal.png" //�S�[��

#define GAME_CHARA          "CHARA\\player.png"  //�v���C���[�\��



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
	int MoveSpeed;
};


typedef STRUCT_GAZOU GAZOU;

GAZOU	BackGround; //�w�i�̉摜
GAZOU   BackGround2; //�w�i�Q�̉摜
GAZOU   Goal;		//�S�[���̉摜
GAZOU   Player;      //�v���C���[�̉摜


WNDPROC WndProc;
BOOL IsWM_CREATE = FALSE;

int StartTimeFps;
int CountFps;
float CalcFps;
int SampleNumFps = GAME_FPS_SPEED;

char AllKeyState[256];

int GameSceneNow = (int)GAME_SCENE_TITLE; //�ŏ��̃Q�[����ʂ��^�C�g���ɐݒ�@

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

VOID PLAYER_MOVE(GAZOU *);

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
	if (MY_GAZOU_LOAD(&BackGround2, 0, 0, GAME_BACKIMAGE_2) == FALSE) { MessageBox(NULL, GAME_BACKIMAGE_2, "NotFound", MB_OK); return -1; }
	if (MY_GAZOU_LOAD(&Goal, 0, 0, GAME_GOAL) == FALSE) { MessageBox(NULL, GAME_GOAL, "NotFound", MB_OK); return -1; }
	if (MY_GAZOU_LOAD(&Player, 0, 0, GAME_CHARA) == FALSE) { MessageBox(NULL, GAME_CHARA, "NotFound", MB_OK); return -1; }


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


	GetGraphSize(BackGround.Handle, &BackGround.X, &BackGround.Y);  //�摜�T�C�Y���擾

	DrawExtendGraph(100, 150, 100 + BackGround.X * 2, 150 + BackGround.Y*2, BackGround.Handle, TRUE); //�摜�\���Ɗg��
	

	char StrGameTitle[1][128] = { "Don't Stop" };
	char StrFontTitle[128] = { "MS �S�V�b�N" };
	
	MY_DRAW_STRING_CENTER_CENTER(&StrGameTitle[0], 1, StrFontTitle, 64); //��ʂ̒��S�ɕ`��


	DrawString(0, 0, "�v���C���(�X�y�[�X�L�[�������Ă�������)", GetColor(255, 255, 255));

	return;
}

VOID MY_GAME_PLAY(VOID)
{
	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_END;
	}

	GetGraphSize(BackGround2.Handle, &BackGround2.X, &BackGround2.Y);  //�w�i�摜�T�C�Y���擾

	DrawExtendGraph(0,0, 100 + BackGround2.X * 3, 150 + BackGround2.Y * 2.5, BackGround2.Handle, TRUE); //�w�i�摜�\���Ɗg��

	//SetTransColor(0, 1, 252);  //���߂������F�w��

	GetGraphSize(Player.Handle, &Player.X, &Player.Y); //�v���C���[�̉摜�T�C�Y���擾

	DrawExtendGraph(0, 0, 0 + Player.X, 0 + Player.Y, Player.Handle, TRUE); //�v���C���[�\��

	GetGraphSize(Goal.Handle, &Goal.X, &Goal.Y); //�S�[���摜�T�C�Y���擾

	DrawExtendGraph(600, 400,600+Goal.X*1/2,400+Goal.Y*1/2, Goal.Handle, TRUE); //�S�[���\��

	PLAYER_MOVE(&Player);

	DrawString(0, 0, "�v���C���(�o�b�N�X�y�[�X�L�[�������Ă�������)", GetColor(255, 255, 255));
}

VOID MY_GAME_END(VOID)
{
	if (AllKeyState[KEY_INPUT_RETURN] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}

	DrawString(0, 0, "�^�C�g�����(�G���^�[�L�[�������Ă�������)", GetColor(255, 255, 255));

	return;
}

//�摜�̕\���ݒ�

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
	g->MoveSpeed = 4;

	return TRUE;
}

//�����𒆉��ɕ`��

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


//�t�H���g�ݒ�

VOID MY_DRAW_SET_DEFAULT_FONT(BOOL anti)
{
	ChangeFont("MS �S�V�b�N", DX_CHARSET_DEFAULT);
	SetFontSize(16);

	if (anti)
	{
		ChangeFontType(DX_FONTTYPE_ANTIALIASING);
	}
	return;
}

VOID PLAYER_MOVE(GAZOU *Player)
{
	if (AllKeyState[KEY_INPUT_UP] != 0)
	{
		if (Player.Y - Player.MoveSpeed > 0)
		{
			Player.Y -= Player.MoveSpeed;
		}
	}

	else if (AllKeyState[KEY_INPUT_DOWN] != 0)
	{
		if (Player.Y + Player.Height + Player.MoveSpeed < GAME_HEIGHT)
		{
			Player.Y += Player.MoveSpeed;
		}
	}

	if (AllKeyState[KEY_INPUT_LEFT] != 0)
	{
		if (Player.X - Player.MoveSpeed > 0)
		{
			Player.X -= Player.MoveSpeed;
		}
	}

	else if (AllKeyState[KEY_INPUT_RIGHT] != 0)
	{
		if (Player.X + Player.Width + Player.MoveSpeed < GAME_WIDTH)
		{
			Player.X += Player.MoveSpeed;
		}
	}
}



LRESULT CALLBACK MY_WNDPROC(HWND hwnd, UINT mgs, WPARAM wp, LPARAM lp)
{
	switch (mgs)
	{

	case WM_CREATE:

		IsWM_CREATE = TRUE;

		return 0;

	case WM_CLOSE:

		MessageBox(hwnd, TEXT("�Q�[�����I�����܂��B"), TEXT("�I�����b�Z�[�W"), MB_OK);
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