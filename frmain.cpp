#include"GUI.h"

form main("form", "E-Land Chord Converter");

int pix;
char f[MAX_PATH];

button scan(&main, 560, 200, 112, 56, "Go!");
Label info(&main, 0, 464, 560, 24, "");
extern int go(std::string f);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
	main.create();
	main.Event_Window_Resize = [](form* me) {
		pix = me->w / 12;
	};

	scan.Event_On_Click = [](button* me) {
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = main.hWnd;
		ofn.lpstrDefExt = 0;
		ofn.lpstrFile = f;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = "Í¼Æ¬ÎÄ¼þ\0*.bmp;*.jpg;*.JPG;*.jpeg;*.png;*.gif\0\0";
		ofn.nFilterIndex = 0;
		ofn.lpstrInitialDir = 0;
		ofn.lpstrTitle = "Ñ¡ÔñÀÖÆ×£º";
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		if (GetOpenFileName(&ofn))
		{
			info.name = f;
			if (go(std::string(f)) == 0) {
				info.name = "success";
			}
		}
	};
	button home(&main, 8, 0, 112, 56, "Home");
	button history(&main, 8, 64, 112, 56, "History");
	button setting(&main, 8, 128, 112, 56, "Settings");
	button exit(&main, 8, 400, 112, 56, "Exit");

	home.Event_On_Click = [](button* me) {
		scan.show();
	};

	scan.create();
	home.create();
	history.create();
	setting.create();
	exit.create();
	info.create();

	main();
}