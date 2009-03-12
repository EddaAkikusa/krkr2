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

// �f�o�C�X���蓖��
void
IrrlichtSimpleDevice::attach(HWND hwnd, int width, int height)
{
	if (useRender) {
		IrrlichtBase::attach(hwnd, width, height);
		if (device) {
			IVideoDriver *driver = device->getVideoDriver();
			// �`��J�n
			if (driver) {
				target = driver->createRenderTargetTexture(driver->getScreenSize());
				if (target) {
					driver->setRenderTarget(target);
				} else {
					error_log("failed to create rendertarget");
				}
			}
		}
	} else {
		IrrlichtBase::attach(hwnd, width, height);
	}
}

// �f�o�C�X�̔j��
void
IrrlichtSimpleDevice::detach()
{
	if (target) {
		target->drop();
		target = NULL;
	}
	IrrlichtBase::detach();
}

/**
 * �E�C���h�E�𐶐�
 * @param parent �e�E�C���h�E
 */
void
IrrlichtSimpleDevice::createWindow(HWND krkr)
{
	attach(krkr, width, height);
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
 * @param widow �E�C���h�E
 * @param width ����
 * @param height �c��
 * @param useRender �����_�[�^�[�Q�b�g���g��(�����m���ɗL��)
 */
IrrlichtSimpleDevice::IrrlichtSimpleDevice(iTJSDispatch2 *window, int width, int height, bool useRender)
	: IrrlichtBase(), window(window), width(width), height(height), useRender(useRender), dwidth(-1), dheight(-1), hbmp(0), oldbmp(0), destDC(0), bmpbuffer(NULL)
{
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
	clearDC();
	destroyWindow();
	if (window) {
		setReceiver(messageHandler, false);
		window->Release();
		window = NULL;
	}
}


//! returns the size of a texture which would be the optimize size for rendering it
static s32 getTextureSizeFromImageSize(s32 size)
{
	s32 ts = 0x01;
	while(ts < size)
		ts <<= 1;
	return ts;
}

tjs_error
IrrlichtSimpleDevice::Factory(IrrlichtSimpleDevice **obj, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
	if (numparams < 4) {
		return TJS_E_BADPARAMCOUNT;
	}
	iTJSDispatch2 *window = param[0]->AsObjectNoAddRef();
	if (window == NULL || window->IsInstanceOf(0, NULL, NULL, L"Window", window) != TJS_S_TRUE) {
		TVPThrowExceptionMessage(L"must set Window object");
	}
	int width  = (tjs_int)*param[1];
	int height = (tjs_int)*param[2];
	bool useRender = numparams >= 4 ? (tjs_int)*param[3]!=0 : false;

	if (useRender) {
		if (getTextureSizeFromImageSize(width) != width ||
			getTextureSizeFromImageSize(height) != height) {
			TVPThrowExceptionMessage(L"width/height must be power of 2 when render mode");
		}
	}

	*obj = new IrrlichtSimpleDevice(window, width, height, useRender);
	return TJS_S_OK;
}

// -----------------------------------------------------------------------
// �ŗL�@�\
// -----------------------------------------------------------------------

void
IrrlichtSimpleDevice::_setSize()
{
	if (useRender) {
		TVPThrowExceptionMessage(L"can't change width/height when render mode");
	}
	if (device) {
		IVideoDriver *driver = device->getVideoDriver();
		if (driver) {
			driver->OnResize(dimension2d<s32>(width, height));
		}
	}
}

/**
 * DC��j��
 */
void
IrrlichtSimpleDevice::clearDC()
{
	if (destDC) {
		SelectObject(destDC, oldbmp);
		DeleteDC(destDC);
		DeleteObject(hbmp);
		oldbmp = 0;
		hbmp = 0;
		destDC = 0;
	}
}

/**
 * DC���X�V
 */
void
IrrlichtSimpleDevice::updateDC(tjs_int dwidth, tjs_int dheight)
{
	if (this->dwidth != dwidth || this->dheight != dheight) {
		// ��x�j��
		clearDC();
		// �`����DIB�����
		BITMAPINFO biBMP;
		ZeroMemory(&biBMP, sizeof biBMP);
		biBMP.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		biBMP.bmiHeader.biBitCount = 32;
		biBMP.bmiHeader.biPlanes = 1;
		biBMP.bmiHeader.biWidth  = dwidth;
		biBMP.bmiHeader.biHeight = -dheight;
		hbmp = CreateDIBSection(NULL, &biBMP, DIB_RGB_COLORS, &bmpbuffer, NULL, 0);
		if (hbmp) {
			// �`��p��DC�����
			destDC = CreateCompatibleDC(NULL);
			if (destDC) {
				oldbmp = (HBITMAP)SelectObject(destDC, hbmp);
			} else {
				DeleteObject(hbmp);
				hbmp = 0;
			}
		}
		this->dwidth = dwidth;
		this->dheight = dheight;
	}
}

/**
 * ���C���ɑ΂��čX�V�`��
 * �o�b�N�o�b�t�@����R�s�[���܂��B
 * @param layer ���C��
 * @param srcRect �\�[�X�̈�
 */
void
IrrlichtSimpleDevice::_updateToLayer(iTJSDispatch2 *layer, irr::core::rect<s32> *srcRect)
{
	if (device) {
		// ���Ԃ�i�߂� XXX tick ���O������^�����Ȃ����H
		device->getTimer()->tick();
		
		IVideoDriver *driver = device->getVideoDriver();
		// �`��J�n
		if (driver) {

			// ���C�����擾
			ncbPropAccessor obj(layer);
			tjs_int dwidth  = obj.GetValue(L"imageWidth", ncbTypedefs::Tag<tjs_int>());
			tjs_int dheight = obj.GetValue(L"imageHeight", ncbTypedefs::Tag<tjs_int>());
			tjs_int dPitch  = obj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
			unsigned char *dbuffer = (unsigned char *)obj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int>());

			// �`���DC�̍X�V
			updateDC(dwidth, dheight);

			if (destDC) {
				if (driver->beginScene(true, true, irr::video::SColor(0,0,0,0))) {
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

					// ����DC�ɑ΂��ĕ`�揈�����s��
					irr::core::rect<s32> destRect(0,0,dwidth,dheight);
					driver->endScene(0, srcRect, &destRect, destDC);
					
					// �r�b�g�}�b�v����R�s�[
					for (tjs_int y = 0; y < dheight; y++) {
						unsigned char *src = (unsigned char *)(bmpbuffer) + dwidth * y * 4;
						CopyMemory(dbuffer, src, dwidth*4);
						dbuffer += dPitch;
					}
					// ���C�����X�V
					layer->FuncCall(0, L"update", NULL, NULL, 0, NULL, layer);
				} else {
					error_log("failed to beginScene");
				}
			}
		}
	}
}

tjs_error
IrrlichtSimpleDevice::updateToLayer(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
	IrrlichtSimpleDevice *self = ncbInstanceAdaptor<IrrlichtSimpleDevice>::GetNativeInstance(objthis);
	if (!self) {
		return TJS_E_NATIVECLASSCRASH;
	}
	if (numparams < 1) {
		return TJS_E_BADPARAMCOUNT;
	}
	// ���C���I�u�W�F�N�g���ǂ���
	iTJSDispatch2 *layer = param[0]->AsObjectNoAddRef();
	if (layer == NULL || layer->IsInstanceOf(0, NULL, NULL, L"Layer", layer) != TJS_S_TRUE) {
		TVPThrowExceptionMessage(L"must set Layer object");
	}
	// �\�[�X�̈攻��
	irr::core::rect<s32> *srcRect = NULL, _srcRect;
	if (numparams >= 5) {
		_srcRect.setLeft((tjs_int)*param[1]);
		_srcRect.setTop((tjs_int)*param[2]);
		_srcRect.setWidth((tjs_int)*param[3]);
		_srcRect.setHeight((tjs_int)*param[4]);
		srcRect = &_srcRect;
	}
	self->_updateToLayer(layer, srcRect);
	return TJS_S_OK;
}
