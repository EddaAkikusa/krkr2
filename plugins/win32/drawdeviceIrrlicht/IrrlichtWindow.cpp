#include "IrrlichtWindow.h"

#define CLASSNAME "IrrlichtWindow"
#define KRKRDISPWINDOWCLASS "TScrollBox"

extern void message_log(const char* format, ...);
extern void error_log(const char *format, ...);

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

/**
 * �E�C���h�E�v���V�[�W��
 */
LRESULT CALLBACK
IrrlichtWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// �Q�l: irrlicht/source/Irrlicht/CIrrDeviceWin32.cpp
	// �����ɏ��������낢�돑���K�v����

	IrrlichtWindow *self = (IrrlichtWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch (message) {
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			if (self) {
				self->show();
			}
			EndPaint(hWnd, &ps);
		}
		return 0;
	case WM_ERASEBKGND:
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void registerWindowClass()
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof wcex);
	wcex.cbSize		= sizeof(WNDCLASSEX);
	wcex.style		= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)IrrlichtWindow::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= GetModuleHandle(NULL);
	wcex.hIcon		    = NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= CLASSNAME;
	wcex.hIconSm		= 0;
	RegisterClassEx(&wcex);
}

void unregisterWindowClass()
{
	UnregisterClass(CLASSNAME, GetModuleHandle(NULL));
}



// �C�x���g����
bool __stdcall
IrrlichtWindow::messageHandler(void *userdata, tTVPWindowMessage *Message)
{
	IrrlichtWindow *self = (IrrlichtWindow*)userdata;
	switch (Message->Msg) {
	case TVP_WM_DETACH:
		self->destroyWindow();
		break;
	case TVP_WM_ATTACH:
		self->createWindow((HWND)Message->LParam);
		break;
	default:
		break;
	}
	return false;
}

// ���[�U���b�Z�[�W���V�[�o�̓o�^/����
void
IrrlichtWindow::setReceiver(tTVPWindowMessageReceiver receiver, bool enable)
{
	tTJSVariant mode     = enable ? (tTVInteger)(tjs_int)wrmRegister : (tTVInteger)(tjs_int)wrmUnregister;
	tTJSVariant proc     = (tTVInteger)(tjs_int)receiver;
	tTJSVariant userdata = (tTVInteger)(tjs_int)this;
	tTJSVariant *p[] = {&mode, &proc, &userdata};
	if (window->FuncCall(0, L"registerMessageReceiver", NULL, NULL, 4, p, window) != TJS_S_OK) {
		if (enable) {
			TVPThrowExceptionMessage(L"can't regist user message receiver");
		}
	}
}

/**
 * �E�C���h�E�𐶐�
 * @param parent �e�E�C���h�E
 */
void
IrrlichtWindow::createWindow(HWND krkr)
{
	if (krkr && (parent = FindWindowEx(krkr, NULL, KRKRDISPWINDOWCLASS, NULL))) {
		hwnd = CreateWindow(CLASSNAME, "",
							WS_CHILD|WS_CLIPCHILDREN,
							left, top, width, height,
							parent, NULL, GetModuleHandle(NULL), NULL);
		if (hwnd) {
			_setPos();
			attach(hwnd);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
		}
	}
}

/**
 * �E�C���h�E��j��
 */
void
IrrlichtWindow::destroyWindow()
{
	if (hwnd) {
		detach();
		DestroyWindow(hwnd);
		hwnd = 0;
	}
	parent = 0;
}

/**
 * �R���X�g���N�^
 */
IrrlichtWindow::IrrlichtWindow(iTJSDispatch2 *win, int left, int top, int width, int height)
	: IrrlichtBaseUpdate(), window(NULL), parent(0), hwnd(0), visible(false)
{
	if (win->IsInstanceOf(0, NULL, NULL, L"Window", win) != TJS_S_TRUE) {
		TVPThrowExceptionMessage(L"must set window object");
	}

	window = win;
	window->AddRef();
	setReceiver(messageHandler, true);
	
	tTJSVariant krkrHwnd; // �e�̃n���h��
	if (window->PropGet(0, TJS_W("HWND"), NULL, &krkrHwnd, window) == TJS_S_OK) {
		HWND parent = FindWindowEx((HWND)(tjs_int)krkrHwnd, NULL, KRKRDISPWINDOWCLASS, NULL);
		if (parent) {
			RECT rect;
			GetClientRect(parent, &rect);
			this->left   = 0;
			this->top    = 0;
			this->width  = rect.right - rect.left;
			this->height = rect.bottom - rect.top;
		}
	}
	this->left   = left;
	this->top    = top;
	if (width != 0) {
		this->width  = width;
	}
	if (height != 0) {
		this->height = height;
	}
	createWindow((HWND)(tjs_int)krkrHwnd);
}

/**
 * �f�X�g���N�^
 */
IrrlichtWindow::~IrrlichtWindow()
{
	destroyWindow();
	if (window) {
		setReceiver(messageHandler, false);
		window->Release();
		window = NULL;
	}
}

// -----------------------------------------------------------------------
// Continuous
// -----------------------------------------------------------------------

/**
 * Continuous �R�[���o�b�N
 * �g���g�����ɂȂƂ��ɏ�ɌĂ΂��
 * ���ꂪ������̃��C�����[�v�ɂȂ�
 */
void TJS_INTF_METHOD
IrrlichtWindow::OnContinuousCallback(tjs_uint64 tick)
{
	InvalidateRect(hwnd, NULL, false);
}

// -----------------------------------------------------------------------
// �\���w��
// -----------------------------------------------------------------------

void
IrrlichtWindow::_setPos()
{
	if (hwnd) {
		MoveWindow(hwnd, left, top, width, height, FALSE);
		ShowWindow(hwnd, visible);
	}
}
	
void
IrrlichtWindow::setVisible(bool v)
{
	visible = v;
	_setPos();
}

bool
IrrlichtWindow::getVisible()
{
	return visible;
}

void
IrrlichtWindow::setLeft(int l)
{
	left = l;
	_setPos();
}

int
IrrlichtWindow::getLeft()
{
	return left;
}

void
IrrlichtWindow::setTop(int t)
{
	top = t;
	_setPos();
}

int
IrrlichtWindow::getTop()
{
	return top;
}

void
IrrlichtWindow::setWidth(int w)
{
	width = w;
	_setPos();
}

int
IrrlichtWindow::getWidth()
{
	return width;
}

void
IrrlichtWindow::setHeight(int h)
{
	height = h;
	_setPos();
}

int
IrrlichtWindow::getHeight()
{
	return height;
}
	
/**
 * ���ꏊ�w��
 */	
void
IrrlichtWindow::setPos(int l, int t)
{
	left = l;
	top  = t;
	_setPos();
}

/**
 * ���T�C�Y�w��
 */	
void
IrrlichtWindow::setSize(int w, int h)
{
	width = w;
	height = h;
	_setPos();
}

