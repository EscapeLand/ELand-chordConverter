#pragma once
#include <windows.h>
#include <vector>
#include <algorithm>

#define CLR_DEFAULT             0xFF000000L
#define PBS_SMOOTH              0x01
#define PBM_SETRANGE            (WM_USER+1)
#define PBM_SETPOS              (WM_USER+2)
#define PBM_SETSTEP             (WM_USER+4)
#define PBM_STEPIT              (WM_USER+5)
#define PBM_GETPOS              (WM_USER+8)
#define PBM_SETBARCOLOR         (WM_USER+9)

#define MAXSIZE 1024
#define ID_MENU 9001

#define assert(expression) ((void)0)
extern HINSTANCE hi;
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

static void* getForm(HWND hWnd);
static std::vector<void*> formSet;

enum permission {
	readWrite,readOnly,writeOnly
};

template <typename Container,typename Value,enum permission prop,bool permissive>
class Property {
private:
	Container * me = NULL;
	void (Container::*Set)(Value value) = NULL;
	Value (Container::*Get)() = NULL;
	bool flag = true;
public:
	Property(){}
	void setContainer(Container* x)
	{
		me = x;
	}
	void setter(void (Container::*pSet)(Value value))
	{
		if (permissive) {
			if (flag) {
				Set = pSet;
				flag = false;
				return;
			}
		}
		if ((prop == writeOnly) || (prop == readWrite)) Set = pSet;
		
	}
	void getter(Value (Container::*pGet)())
	{
		if ((prop == readWrite) || (prop == readOnly)) Get = pGet;
	}
	Value operator =(const Value& value)
	{
		assert(me);
		assert(Set);
		(me->*Set)(value);
		if (Set && prop == readOnly) Set = NULL;
		return value;
	}
	operator Value() {
		assert(me);
		assert(Get);
		return (me->*Get)();
	}
};

class window {
private:
	LPCSTR Name = NULL;
	HWND hwnd = NULL;
	
	void setName(LPCSTR newName) {
		if (hWnd) SetWindowTextA(hWnd, newName);
		Name = newName;
	}
	LPCSTR getName() {
		return Name;
	}
	void setHwnd(HWND hwnd) {
		this->hwnd = hwnd;
	}
	HWND getHwnd() {
		return hwnd;
	}
public:
	int x;
	int y;
	int w;
	int h;
	char type;
	unsigned int id;
	window() {
		name.setContainer(this);
		hWnd.setContainer(this);
		name.setter(&window::setName);
		name.getter(&window::getName);
		hWnd.getter(&window::getHwnd);
		hWnd.setter(&window::setHwnd);
	}
	Property<window, LPCSTR, readWrite,false> name;
	Property<window, HWND, readOnly,true> hWnd;
	void hide() {
		ShowWindow(hWnd, 0);
	}
	void show() {
		ShowWindow(hWnd, 1);
	}
	void resize(int w, int h) {
		if (w) this->w = w;
		if (h) this->h = h;
		MoveWindow(this->hWnd, x, y, this->w, this->h, true);
	}
	void move(int x, int y) {
		if (x) this->x = x;
		if (y) this->y = y;
		MoveWindow(this->hWnd, this->x, this->y, this->w, this->h, true);
	}
};

class form :public window{
private:
	//窝已经尽量私有了QAQ
	LPCSTR className;
	LPCSTR title;
	
	
	std::vector<LPCSTR> menuList;
	std::vector<void*> menuEventList;

	HMENU RBmenu = NULL;
	LPCSTR icon = IDI_APPLICATION;
	LPCSTR smallIcon = IDI_APPLICATION;

	bool createOver = false;
	HMENU CONTEXTMENU() { return this->RBmenu; }
	bool isCreated() { return this->createOver; };
public:
	//构造
	form() {}
	form(char* className, char* title) {

		this->title = title;
		this->className = className;
		RButtonMenu.setContainer(this);
		MessageCreated.setContainer(this);
		RButtonMenu.getter(&form::CONTEXTMENU);
		MessageCreated.getter(&form::isCreated);
	}
	//属性
	Property<form, HMENU, readOnly,false> RButtonMenu;
	Property<form, bool, readOnly,false> MessageCreated;
	int brush = 0;
	
	std::vector<void*> tab;
	//方法
	
	LPCSTR CLASS() { return this->className; }
	void setIcon(LPCSTR smallIconName,LPCSTR iconName = NULL) {
		if(iconName) this->icon = iconName;
		smallIcon = smallIconName;
	}
	void pushmenu(void(*Event_Menu_Click)(),char* menu) {
		if(!this->RBmenu) RBmenu = CreatePopupMenu();
		if (AppendMenuA(this->RBmenu, MF_STRING, ID_MENU + this->menuList.size(), menu)) {
			LPCSTR A = MAKEINTRESOURCE(ID_MENU + this->menuList.size());
			this->menuList.push_back(MAKEINTRESOURCE(ID_MENU + this->menuList.size()));
			this->menuEventList.push_back(Event_Menu_Click);
		}
	}
	void pushMenu(void(*Event_Menu_Click)(),LPCSTR ID) {
		this->menuList.push_back(ID);
		this->menuEventList.push_back(Event_Menu_Click);
	}
	void Event_Menu_Click(WORD ID) {
		size_t A = menuList.size();
		for (UINT i = 0; i < menuList.size(); i++) {
			if ((WORD)(ULONG_PTR)menuList[i] == ID) {
				((void(*)())menuEventList[i])();
			}
		}
	}
	
	void minimum() {
		ShowWindow(hWnd, SW_SHOWMINNOACTIVE);
	}
	void resize(int nw,int nh) {
		if (MoveWindow(hWnd, x, y, nw, nh, true)) {
			w = nw;
			h = nh;
		}
	}
	void move(int nx, int ny) {
		if (MoveWindow(hWnd, nx, ny, w, h, false)) {
			x = nx;
			y = ny;
		}
	}
	int create() {
		WNDCLASSEXA wndclass;
		wndclass.cbSize = sizeof(wndclass);
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = WinProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hbrBackground = (HBRUSH)GetStockObject(brush);
		wndclass.hCursor = LoadCursorA(NULL, IDC_ARROW);
		//注意：此处下两句，MSDN中记载，如果是标准图标，第一个参数应该是NULL而不是hInstance. 此处尚未做处理，但程序运行通过，暂且放下
		wndclass.hIcon = LoadIconA(hi, this->icon);
		wndclass.hIconSm = LoadIconA(hi,this->smallIcon);
		wndclass.hInstance = hi;
		wndclass.lpszClassName = className;
		wndclass.lpszMenuName = NULL;
		if (find_if(formSet.begin(), formSet.end(), [wndclass](const void* x) -> bool {return ((form*)x)->CLASS() == wndclass.lpszClassName; }) == formSet.end()) {
			if (!RegisterClassExA(&wndclass)) {
				char s[10];
				_itoa_s(GetLastError(), s, 10);
				MessageBox(NULL, TEXT("注册类名失败！"), s, MB_OK);
				return -1;
			}
		}
		id = (UINT)formSet.size();
		formSet.push_back((void*)this);
		this->hWnd = CreateWindowA(className, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hi, NULL);
		if (!hWnd)
		{
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建窗口失败！"), s, MB_OK);
			UnregisterClassA(className, hi);
			formSet.pop_back();
			return -1;
		}
		
		if (this->Event_On_Create) this->Event_On_Create(this);
		return id;
	}
	unsigned long long run() {				//在此处主程序挂起
		MSG msg;
		ShowWindow(hWnd, SW_SHOW); 
		UpdateWindow(hWnd);
		ZeroMemory(&msg, sizeof(msg));
		if (this->Event_Load_Complete) this->Event_Load_Complete(this);
		this->createOver = true;						//消息队列建立
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		UnregisterClassA(className, hi);
		return msg.wParam;
	}
	void* getControl(HWND controlHwnd);
	//事件
	void(*Event_On_Create)(form*) = NULL;
	void(*Event_On_Unload)(form*) = NULL;
	void(*Event_Load_Complete)(form*) = NULL;
	void(*Event_Window_Resize)(form*) = NULL;
};

class control:public window {				//继承类
private:
	
public:
	//属性
	form* parent = NULL;
	std::string tag;
	//方法
	control(){}
	//虚的
	virtual int create() = 0;
	//实的
	
};

class button :public control {
public:
	button() {}
	button(form* parent, int x, int y, int w, int h, char* Name) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->type = 'b';
		this->parent = parent;
		this->name = Name;				//x
		id = (UINT)parent->tab.size();
		parent->tab.push_back(this);
	}
	//int create();
	void(*Event_On_Click)(button*) = NULL;
	int create() {
		this->hWnd = CreateWindowA(
			"BUTTON",   // predefined class  
			this->name,       // button text  
			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles  
			this->x,         // starting x position  
			this->y,         // starting y position  
			this->w,        // button width  
			this->h,        // button height  
			this->parent->hWnd,       // parent window  
			NULL,       // No menu  
			hi,
			NULL);      // pointer not needed  
		if (!hWnd) {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建按钮失败！"), s, MB_OK);
			return -1;
		}
		return id;
	}
};



class Textbox :public control {
public:
	bool Multiline = true;
	Textbox() {}
	Textbox(form* parent, int x, int y, int w, int h, char* Name) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->type = 't';
		this->parent = parent;
		this->name = Name;				//x
		id = (UINT)parent->tab.size();
		parent->tab.push_back(this);
	}
	//int create();
	void(*Event_Text_Change)(Textbox*) = NULL;
	LPCSTR value(char* r) {
		GetWindowText(hWnd,r,MAXSIZE);
		return r;
	}
	int create() {
		hWnd = CreateWindowExA(
			NULL,
			"Edit",
			this->name,
			(this->Multiline ? ES_MULTILINE | WS_CHILD : WS_CHILD) | WS_VISIBLE | WS_BORDER | WS_GROUP | WS_TABSTOP | ES_WANTRETURN,
			this->x, this->y, this->w, this->h,
			this->parent->hWnd,
			NULL,
			hi,
			NULL);
		if (!hWnd) {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建文本框失败！"), s, MB_OK);
			return -1;
		}
		return id;
	}
};



class Label :public control {
public:
	
	Label() {}
	Label(form* parent, int x, int y, int w, int h, char* Name) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->type = 'l';
		this->parent = parent;
		this->name = Name;				//x
		id = (UINT)parent->tab.size();
		parent->tab.push_back(this);
	}
	//int create();
	void(*Event_On_Click)(Label*) = NULL;
	int create() {
		hWnd = CreateWindowA(
			"STATIC",
			this->name,
			WS_CHILD | WS_VISIBLE | SS_NOTIFY,
			this->x, this->y, this->w, this->h,
			this->parent->hWnd,
			NULL,
			hi,
			NULL);
		if (!hWnd) {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建标签框失败！"), s, MB_OK);
			return -1;
		}
		return id;
	}
};



class Picture :public control {
	//.bmp only
public:
	LPCSTR path;
	Picture() {}
	Picture(form* parent, int x, int y, int w, int h, char* Name,char* picPath) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->type = 'p';
		this->parent = parent;
		this->name = Name;				//x
		this->path = picPath;
		id = (UINT)parent->tab.size();
		parent->tab.push_back(this);
	}
	//int create();
	void(*Event_On_Click)(Picture*) = NULL;
	int create() {
		hWnd = CreateWindowA(
			"STATIC",
			this->name,
			WS_CHILD | SS_BITMAP | WS_VISIBLE,
			this->x, this->y, this->w, this->h,
			this->parent->hWnd,
			NULL,
			hi,
			NULL);
		if (hWnd) {
			HBITMAP hbmp = (HBITMAP)LoadImage(NULL, this->path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (!hbmp) {
				char s[10];
				_itoa_s(GetLastError(), s, 10);
				MessageBox(NULL, TEXT("读取位图失败！"), s, MB_OK);
				return -1;
			}
			HBITMAP holdmap = (HBITMAP)SendMessage(hWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbmp);
			if (holdmap) {
				DeleteObject(holdmap);
			}
			DeleteObject(hbmp);
		}
		else {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建图片失败！"), s, MB_OK);
			return -1;
		}
		return id;
	}
};



class ProgressBar :public control {
public:
	int step = 10;
	int range = 100;
	ProgressBar() {}
	ProgressBar(form* parent, int x, int y, int w, int h, char* Name) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->type = 'P';
		this->parent = parent;
		this->name = Name;				//x
		id = (UINT)parent->tab.size();
		parent->tab.push_back(this);
	}
	//int create();
	void stepIn() {
		SendMessage(hWnd, PBM_STEPIT, 0, 0);
	}
	int setStep(int newStep = 0) {
		if (newStep) { 
			SendMessage(hWnd, PBM_SETSTEP, (WPARAM)newStep, 0);
			step = newStep;
		}
		return step;
	}
	int setRange(int newRange = 0) {
		if (newRange) {
			SendMessage(hWnd, PBM_SETRANGE, 0, MAKELPARAM(0, newRange));
			range = newRange;
		}
		return range;
	}
	void setPos(int pos) {
		SendMessage(hWnd, PBM_SETPOS, pos, 0);
	}
	void empty() {
		setPos(0);
	}
	void full() {
		setPos((int)SendMessage(hWnd, PBM_GETPOS, 0, 0));
	}
	void setColor(ULONG color = CLR_DEFAULT) {
		//back to default if void
		SendMessage(hWnd, PBM_SETBARCOLOR,0,color);
	}
	int create() {
		hWnd = CreateWindowExA(
			NULL,
			"msctls_progress32",
			this->name,
			WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
			this->x, this->y, this->w, this->h,
			this->parent->hWnd,
			NULL,
			hi,
			NULL);
		if (!hWnd) {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建进度条失败！"), s, MB_OK);
			return -1;
		}
		return id;
	}
};

class radio :public control {
private:
	bool getCheck() {
		return (int)SendMessage(hWnd, BM_GETCHECK,0,0);
	}
	void setCheck(bool value) {
		SendMessage(hWnd,BM_SETCHECK,(int)value,0);
	}
public:
	//属性
	bool head = false;
	Property<radio, bool, readWrite,false> Value;
	//方法
	radio() {}
	radio(form* parent, int x, int y, int w, int h, char* Name, bool head) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->head = head;
		this->type = 'r';
		this->parent = parent;
		this->name = Name;				//x
		id = (UINT)parent->tab.size();
		parent->tab.push_back(this);
		Value.setContainer(this);
		Value.setter(&radio::setCheck);
		Value.getter(&radio::getCheck);
	}
	int create() {
		hWnd = CreateWindowA(
			"BUTTON",   // predefined class  
			this->name,       // button text  
			this->head? WS_GROUP | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD : BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD,  // styles  
			this->x,         // starting x position  
			this->y,         // starting y position  
			this->w,        // button width  
			this->h,        // button height  
			this->parent->hWnd,       // parent window  
			NULL,       // No menu  
			hi,
			NULL);      // pointer not needed  
		if (!hWnd) {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建单选框失败！"), s, MB_OK);
			return -1;
		}
		return id;
	}
	//事件
	void(*Event_On_Check)(radio*) = NULL;
};

class Checkbox :public control {
private:
	bool getCheck() {
		return (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
	}
	void setCheck(bool value) {
		SendMessage(hWnd, BM_SETCHECK, (int)value, 0);
	}
public:
	Property<Checkbox, bool, readWrite,false> Value;
	Checkbox() {}
	Checkbox(form* parent, int x, int y, int w, int h, char* Name) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->type = 'c';
		this->parent = parent;
		this->name = Name;				//x
		id = (UINT)parent->tab.size();
		parent->tab.push_back(this);
		Value.setContainer(this);
		Value.setter(&Checkbox::setCheck);
		Value.getter(&Checkbox::getCheck);
	}
	void(*Event_On_Check)(Checkbox*) = NULL;
	int create() {
		hWnd = CreateWindowA(
			"BUTTON",   // predefined class  
			this->name,       // button text  
			BS_AUTOCHECKBOX | WS_VISIBLE | WS_CHILD,  // styles  
			this->x,         // starting x position  
			this->y,         // starting y position  
			this->w,        // button width  
			this->h,        // button height  
			this->parent->hWnd,       // parent window  
			NULL,       // No menu  
			hi,
			NULL);      // pointer not needed  
		if (!hWnd) {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建单选框失败！"), s, MB_OK);
			return -1;
		}
		return id;
	}
};

static LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	form* t = (form*)getForm(hwnd);
	switch (message)
	{
	case WM_CREATE:
		//创建事件的思路还没有头绪。。暂时丢到CreateWindow后面去
		break;
	case WM_CONTEXTMENU:
	{
		RECT rect;
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		GetClientRect((HWND)wParam, &rect);
		//把屏幕坐标转为客户区坐标  
		ScreenToClient((HWND)wParam, &pt);
		if (PtInRect(&rect, pt))
			if (!TrackPopupMenu(t->RButtonMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON, LOWORD(lParam), HIWORD(lParam), 0, (HWND)wParam, NULL)) {
				if(t->RButtonMenu) MessageBoxA(NULL,TEXT("弹出菜单失败"),NULL,MB_OK);
			}
		else return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}
	case WM_SIZE:
	{
		t->w = LOWORD(lParam);
		t->h = HIWORD(lParam); 
		if(t->MessageCreated) if(t->Event_Window_Resize) t->Event_Window_Resize(t);
		break;
	}
	case WM_MOVE:
	{
		
		t->x = LOWORD(lParam);
		t->y = HIWORD(lParam);
	}
	case WM_COMMAND:
		if (lParam)
		{
			//这是控件点击事件
			void* p = t->getControl((HWND)lParam);
			if (p) switch (((control*)p)->type) {
			case 'b':
				if (((button*)p)->Event_On_Click) ((button*)p)->Event_On_Click((button*)p);
				break;
			case 'l':
				if (((Label*)p)->Event_On_Click) ((Label*)p)->Event_On_Click((Label*)p);
				break;
			case 'p':
				if (((Picture*)p)->Event_On_Click) ((Picture*)p)->Event_On_Click((Picture*)p);
				break;
			case 'r':
				if (((radio*)p)->Event_On_Check) ((radio*)p)->Event_On_Check((radio*)p);
				break;
			case 'c':
				if (((Checkbox*)p)->Event_On_Check) ((Checkbox*)p)->Event_On_Check((Checkbox*)p);
				break;
			}
		}
		else {
			
			t->Event_Menu_Click(LOWORD(wParam));
			
		}
		break;
	case WM_CTLCOLORSTATIC://拦截WM_CTLCOLORSTATIC消息
	{
		SetBkMode((HDC)wParam, TRANSPARENT);//设置背景透明
		if(t) return (INT_PTR)GetStockObject(t->brush);//返回父窗画刷
	}
	case WM_CLOSE:
	{
		if (t->Event_On_Unload) t->Event_On_Unload(t);
		DestroyWindow(hwnd);
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

static void* getForm(HWND hWnd) {
	std::vector<void*>::iterator r = find_if(formSet.begin(), formSet.end(), [hWnd](const void* x) -> bool {return ((form*)x)->hWnd == hWnd; });
	if (r == formSet.end()) return NULL;
	else return *r;
}

inline void* form::getControl(HWND controlHwnd) {
	std::vector<void*>::iterator r = find_if(tab.begin(), tab.end(), [controlHwnd](const void* x) -> bool { return ((control*)x)->hWnd == controlHwnd; });
	if (r == tab.end()) return NULL;
	else return *r;
}