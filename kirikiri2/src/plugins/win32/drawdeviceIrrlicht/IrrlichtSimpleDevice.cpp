#include "ncbind/ncbind.hpp"
#include "IrrlichtSimpleDevice.h"

extern void message_log(const char* format, ...);
extern void error_log(const char *format, ...);

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

// �C�x���g����
bool __stdcall
IrrlichtSimpleDevice::messageHandler(void *userdata, tTVPWindowMessage *Message)
{
	IrrlichtSimpleDevice *self = (IrrlichtSimpleDevice*)userdata;
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
IrrlichtSimpleDevice::setReceiver(tTVPWindowMessageReceiver receiver, bool enable)
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
IrrlichtSimpleDevice::createWindow(HWND krkr)
{
	attach(krkr, width, height, -1);
}

/**
 * �E�C���h�E��j��
 */
void
IrrlichtSimpleDevice::destroyWindow()
{
	detach();
}

/**
 * �R���X�g���N�^
 */
IrrlichtSimpleDevice::IrrlichtSimpleDevice(iTJSDispatch2 *window, int width, int height)
	: IrrlichtBase(), window(window), width(width), height(height)
{
	if (window == NULL || window->IsInstanceOf(0, NULL, NULL, L"Window", window) != TJS_S_TRUE) {
		TVPThrowExceptionMessage(L"must set window object");
	}
	window->AddRef();
	setReceiver(messageHandler, true);
	
	tTJSVariant krkrHwnd; // �e�̃n���h��
	if (window->PropGet(0, TJS_W("HWND"), NULL, &krkrHwnd, window) == TJS_S_OK) {
		HWND hwnd = (HWND)(tjs_int)krkrHwnd;
		if (hwnd) {
			createWindow(hwnd);
		}
	}
}

/**
 * �f�X�g���N�^
 */
IrrlichtSimpleDevice::~IrrlichtSimpleDevice()
{
	destroyWindow();
	if (window) {
		setReceiver(messageHandler, false);
		window->Release();
		window = NULL;
	}
}

// -----------------------------------------------------------------------
// �ŗL�@�\
// -----------------------------------------------------------------------

void
IrrlichtSimpleDevice::_setSize()
{
	if (device) {
		IVideoDriver *driver = device->getVideoDriver();
		if (driver) {
			driver->OnResize(dimension2d<s32>(width, height));
		}
	}
}

/**
 */
void
IrrlichtSimpleDevice::updateToLayer(iTJSDispatch2 *layer)
{
	if (device && layer) {
		// ���Ԃ�i�߂� XXX tick ���O������^�����Ȃ����H
		device->getTimer()->tick();
		
		IVideoDriver *driver = device->getVideoDriver();
		// �`��J�n
		if (driver && driver->beginScene(true, true, irr::video::SColor(255,0,0,0))) {

			/// �V�[���}�l�[�W���̕`��
			ISceneManager *smgr = device->getSceneManager();
			if (smgr) {
				smgr->drawAll();
			}
			
			// GUI�̕`��
			IGUIEnvironment *gui = device->getGUIEnvironment();
			if (gui) {
				gui->drawAll();
			}

			// ���C�����擾
			ncbPropAccessor obj(layer);
			tjs_int dwidth  = obj.GetValue(L"imageWidth", ncbTypedefs::Tag<tjs_int>());
			tjs_int dheight = obj.GetValue(L"imageHeight", ncbTypedefs::Tag<tjs_int>());
			tjs_int dPitch  = obj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
			unsigned char *dbuffer = (unsigned char *)obj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int>());

			// �`���̃r�b�g�}�b�v�����
			// ���ڏ����ł���Ƃ����񂾂��ǁc�c
			BITMAPINFO biBMP;
			ZeroMemory(&biBMP, sizeof biBMP);
			biBMP.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			biBMP.bmiHeader.biBitCount = 32;
			biBMP.bmiHeader.biPlanes = 1;
			biBMP.bmiHeader.biWidth  = dwidth;
			biBMP.bmiHeader.biHeight = -dheight;
			void *buffer;
			HBITMAP hbmp = CreateDIBSection(NULL, &biBMP, DIB_RGB_COLORS, &buffer, NULL, 0);
			if (hbmp) {
				// �`��p��DC�����
				HDC destDC = CreateCompatibleDC(NULL);
				if (destDC) {
					HBITMAP old = (HBITMAP)SelectObject(destDC, hbmp);
					// ����DC�ɑ΂��ĕ`�悵�Ă��炤
					irr::core::rect<s32> destRect(0,0,dwidth,dheight);
					driver->endScene(0, NULL, &destRect, destDC);
					SelectObject(destDC, old);
					DeleteDC(destDC);
				}
				// �r�b�g�}�b�v����R�s�[
				for (tjs_int y = 0; y < dheight; y++) {
					unsigned char *src = (unsigned char *)(buffer) + dwidth * y * 4;
					CopyMemory(dbuffer, src, dwidth*4);
					dbuffer += dPitch;
				}
				// ���C�����X�V
				layer->FuncCall(0, L"update", NULL, NULL, 0, NULL, layer);

				DeleteObject(hbmp);
			}
		}
	}
}
