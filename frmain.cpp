#include "GUI.h"
#include  <io.h>

std::string picPath = "";
HINSTANCE hi;
extern int threshold(void);
form frmain((char*)"ELand", (char*)"二值化");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
	hi = hInstance;
	frmain.create();
	button btngo(&frmain,240,56,112,72,(char*)"二值化");
	Textbox txtpath(&frmain, 8, 136,560,28, (char*)"C:\\Users\\Administrator\\Desktop\\");
	txtpath.Multiline = false;
	btngo.create();
	txtpath.create();
	btngo.Event_On_Click = [](button* me) {
		char s[MAXSIZE];
		((Textbox*)frmain.tab[1])->value(s);
		if (_access(s, 0) != -1)
		{
			picPath = s;
			threshold();
		}
	};
	frmain.Event_Load_Complete = [](form* me) {
		me->resize(592,240);
	};
	frmain.run();
};