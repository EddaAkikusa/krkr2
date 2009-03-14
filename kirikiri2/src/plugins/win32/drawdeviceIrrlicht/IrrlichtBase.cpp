#include "IrrlichtBase.h"
#include "ncbind/ncbind.hpp"

extern void message_log(const char* format, ...);
extern void error_log(const char *format, ...);

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

/**
 * �R���X�g���N�^
 */
IrrlichtBase::IrrlichtBase(iTJSDispatch2 *objthis) : objthis(objthis), device(NULL), attached(false)
{
}

/**
 * �f�X�g���N�^
 */
IrrlichtBase::~IrrlichtBase()
{
	detach();
}

/**
 * �h���C�o�̏��\��
 */
void
IrrlichtBase::showDriverInfo()
{
	IVideoDriver *driver = device->getVideoDriver();
	if (driver) {
		dimension2d<s32> size = driver->getScreenSize();
		message_log("�f�o�C�X������̃X�N���[���T�C�Y:%d, %d", size.Width, size.Height);
		size = driver->getCurrentRenderTargetSize();
		message_log("�f�o�C�X�������RenderTarget��:%d, %d", size.Width, size.Height);
	}
}

// �C�x���g�N��
void
IrrlichtBase::sendEvent(const tjs_char *eventName)
{
	tTJSVariant method;
	if (TJS_SUCCEEDED(objthis->PropGet(0, eventName, NULL, &method, objthis))) { // �C�x���g���\�b�h���擾
		if (method.Type() == tvtObject) {
			iTJSDispatch2 *m = method.AsObjectNoAddRef();
			if (TJS_SUCCEEDED(m->IsInstanceOf(0, NULL, NULL, L"Function", m))) { // �t�@���N�V�������ǂ���
				tTJSVariant self(objthis, objthis);
				tTJSVariant *params[] = {&self};
				m->FuncCall(0, NULL, NULL, NULL, 1, params, method.AsObjectThisNoAddRef());
			}
		}
	}
}

/**
 * �E�C���h�E�̍Đݒ�
 * @param hwnd �n���h��
 */
void
IrrlichtBase::attach(HWND hwnd, int width, int height)
{
	if (!attached && hwnd) {
		// �f�o�C�X����
		SIrrlichtCreationParameters params;
		params.WindowId     = reinterpret_cast<void*>(hwnd);
		params.DriverType    = EDT_DIRECT3D9;
		params.Stencilbuffer = true;
		params.Vsync = true;
		params.EventReceiver = this;
		params.AntiAlias = true;
		if (width != 0 && height != 0) {
			params.WindowSize = core::dimension2d<s32>(width, height);
		}
		if ((device = irr::createDeviceEx(params))) {
			TVPAddLog(L"Irrlicht�f�o�C�X������");
			// �e�N�X�`���̃��������ɂ����Z�e�X�g���s���悤�ɁB
			// device->getSceneManager()->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);
			showDriverInfo();
			onAttach();
			sendEvent(L"onAttach");
		} else {
			TVPThrowExceptionMessage(L"Irrlicht �f�o�C�X�̏������Ɏ��s���܂���");
		}
		attached = true;
	}
}

/**
 * �E�C���h�E�̉���
 */
void
IrrlichtBase::detach()
{
	if (device) {
		sendEvent(L"onDetach");
		onDetach();
		device->drop();
		device = NULL;
	}
	attached = false;
}

/**
 * Irrlicht�`�揈��
 * @param destRect �`���̈�
 * @param srcRect �`�挳�̈�
 * @param destDC �`���DC
 * @return �`�悳�ꂽ
 */
bool
IrrlichtBase::show(irr::core::rect<irr::s32> *destRect, irr::core::rect<irr::s32> *srcRect, HDC destDC)
{
	if (device) {
		// ���Ԃ�i�߂� XXX tick ���O������^�����Ȃ����H
		device->getTimer()->tick();
		
		IVideoDriver *driver = device->getVideoDriver();
		// �`��J�n
		if (driver && driver->beginScene(true, true, irr::video::SColor(0,0,0,0))) {

			/// �V�[���}�l�[�W���̕`��
			ISceneManager *smgr = device->getSceneManager();
			if (smgr) {
				smgr->drawAll();
			}
			
			// �ŗL����
			update(driver);

			// GUI�̕`��
			IGUIEnvironment *gui = device->getGUIEnvironment();
			if (gui) {
				gui->drawAll();
			}
			
			// �`�抮��
			driver->endScene(0, srcRect, destRect, destDC);
			return true;
		}
	}
	return false;
};

/**
 * Irrlicht �ւ̃C�x���g���M
 */
bool
IrrlichtBase::postEvent(SEvent &ev)
{
	if (device) {
		if (device->getGUIEnvironment()->postEventFromUser(ev) ||
			device->getSceneManager()->postEventFromUser(ev)) {
			return true;
		}
	}
	return false;
}

/**
 * �C�x���g��
 * HWND ���w�肵�Đ������Ă���֌W�� Irrlicht ���g�̓E�C���h�E����
 * �C�x���g���擾���邱�Ƃ͂Ȃ��B�̂� GUI Environment ����̃C�x���g
 * �����������ɂ��邱�ƂɂȂ�B�����̓K���ȃ��\�b�h���Ăяo���悤�ɗv�C�� XXX
 * @param event �C�x���g���
 * @return ���������� true
 */
bool
IrrlichtBase::OnEvent(const irr::SEvent &event)
{
	switch (event.EventType) {
	case EET_GUI_EVENT:
		message_log("GUI�C�x���g:%d", event.GUIEvent.EventType);
		switch(event.GUIEvent.EventType) {
		case EGET_BUTTON_CLICKED:
			message_log("�{�^�������ꂽ");
			break;
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
#if 0
		message_log("�}�E�X�C�x���g:%d x:%d y:%d wheel:%f",
			event.MouseInput.Event,
			event.MouseInput.X,
			event.MouseInput.Y,
			event.MouseInput.Wheel);
#endif
		break;
	case EET_KEY_INPUT_EVENT:
//		message_log("�L�[�C�x���g:%x", event.KeyInput.Key);
		{
			int shift = 0;
			if (event.KeyInput.Shift) {
				shift |= 0x01;
			}
			if (event.KeyInput.Control) {
				shift |= 0x04;
			}
		}
		break;
	case EET_LOG_TEXT_EVENT:
		message_log("���O���x��:%d ���O:%s",
					event.LogEvent.Level,
					event.LogEvent.Text);
		break;
	case EET_USER_EVENT:
		message_log("���[�U�C�x���g");
		break;
	}
	return false;
}

// --------------------------------------------------------------------------------

/**
 * Irrlicht �Ăяo�������J�n
 */
void
IrrlichtBaseUpdate::start()
{
	stop();
	TVPAddContinuousEventHook(this);
}

/**
 * Irrlicht �Ăяo��������~
 */
void
IrrlichtBaseUpdate::stop()
{
	TVPRemoveContinuousEventHook(this);
}

/**
 * �R���X�g���N�^
 */
IrrlichtBaseUpdate::IrrlichtBaseUpdate(iTJSDispatch2 *objthis) : IrrlichtBase(objthis)
{
}


/**
 * �f�X�g���N�^
 */
IrrlichtBaseUpdate::~IrrlichtBaseUpdate()
{
	stop();
}
