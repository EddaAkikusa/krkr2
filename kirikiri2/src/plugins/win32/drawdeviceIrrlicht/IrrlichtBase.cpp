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

// -----------------------------------
// �e��I�u�W�F�N�g�Q�Ɨp
// -----------------------------------

bool IsArray(tTJSVariant &var)
{
	if (var.Type() == tvtObject) {
		iTJSDispatch2 *obj = var.AsObjectNoAddRef();
		return obj->IsInstanceOf(0, NULL, NULL, L"Array", obj) == TJS_S_TRUE;
	}
	return false;
}

dimension2df getDimension2D(tTJSVariant &var)
{
	ncbPropAccessor info(var);
	if (IsArray(var)) {
		return dimension2df((f32)info.getRealValue(0),
							(f32)info.getRealValue(1));
	} else {
		return dimension2df((f32)info.getRealValue(L"width"),
							(f32)info.getRealValue(L"height"));
	}
}

position2df getPosition2D(tTJSVariant &var)
{
	ncbPropAccessor info(var);
	if (IsArray(var)) {
		return position2df((f32)info.getRealValue(0),
						   (f32)info.getRealValue(1));
	} else {
		return position2df((f32)info.getRealValue(L"x"),
						   (f32)info.getRealValue(L"y"));
	}
}

vector2df getVector2D(tTJSVariant &var)
{
	ncbPropAccessor info(var);
	if (IsArray(var)) {
		return vector2df((f32)info.getRealValue(0),
						 (f32)info.getRealValue(1));
	} else {
		return vector2df((f32)info.getRealValue(L"x"),
						 (f32)info.getRealValue(L"y"));
	}
}

vector3df getVector3D(tTJSVariant &var)
{
	ncbPropAccessor info(var);
	if (IsArray(var)) {
		return vector3df((f32)info.getRealValue(0),
						 (f32)info.getRealValue(1),
						 (f32)info.getRealValue(2));
	} else {
		return vector3df((f32)info.getRealValue(L"x"),
						 (f32)info.getRealValue(L"y"),
						 (f32)info.getRealValue(L"z"));
	}
}

rect<s32> getRect(tTJSVariant &var)
{
	ncbPropAccessor info(var);
	if (IsArray(var)) {
		return rect<s32>(info.getIntValue(0),
						 info.getIntValue(1),
						 info.getIntValue(2),
						 info.getIntValue(3));
	} else {
		int x = info.getIntValue(L"x");
		int y = info.getIntValue(L"y");
		int x2, y2;
		if (info.HasValue(L"width")) {
			x2 = x + info.getIntValue(L"width") - 1;
			y2 = x + info.getIntValue(L"height") - 1;
		} else {
			x2 = info.getIntValue(L"x2");
			y2 = info.getIntValue(L"y2");
		}
		return rect<s32>(x, y, x2, y2);
	}
}

// ----------------------------------- 

/**
 * �R���X�g���N�^
 */
IrrlichtBase::IrrlichtBase(video::E_DRIVER_TYPE driverType) : driverType(driverType), device(NULL)
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

/**
 * �E�C���h�E�̍Đݒ�
 * @param hwnd �n���h��
 */
void
IrrlichtBase::attach(HWND hwnd)
{
	// �f�o�C�X����
	SIrrlichtCreationParameters params;
	params.WindowId     = reinterpret_cast<void*>(hwnd);
	params.DriverType    = driverType;
	params.Bits          = 32;
	params.Stencilbuffer = true;
	params.Vsync = true;
	params.EventReceiver = this;
	params.AntiAlias = true;

	if ((device = irr::createDeviceEx(params))) {
		TVPAddLog(L"Irrlicht�f�o�C�X������");
	} else {
		TVPThrowExceptionMessage(L"Irrlicht �f�o�C�X�̏������Ɏ��s���܂���");
	}
	
	showDriverInfo();
	start();
}

/**
 * �E�C���h�E�̉���
 */
void
IrrlichtBase::detach()
{
	if (device) {
		device->drop();
		device = NULL;
	}
	stop();
}

/**
 * Irrlicht �Ăяo�������J�n
 */
void
IrrlichtBase::start()
{
	stop();
	TVPAddContinuousEventHook(this);
}

/**
 * Irrlicht �Ăяo��������~
 */
void
IrrlichtBase::stop()
{
	TVPRemoveContinuousEventHook(this);
}

/**
 * Continuous �R�[���o�b�N
 * �g���g�����ɂȂƂ��ɏ�ɌĂ΂��
 * ���ꂪ������̃��C�����[�v�ɂȂ�
 */
void TJS_INTF_METHOD
IrrlichtBase::OnContinuousCallback(tjs_uint64 tick)
{
	if (device) {
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

			// �ŗL����
			update(driver, tick);

			// GUI�̕`��
			IGUIEnvironment *gui = device->getGUIEnvironment();
			if (gui) {
				gui->drawAll();
			}

			// �`�抮��
			driver->endScene();
		}
	}
};

/**
 * �C�x���g��
 * HWND ���w�肵�Đ������Ă���֌W�� Irrlicht ���g�̓E�C���h�E����
 * �C�x���g���擾���邱�Ƃ͂Ȃ��B�̂� GUI Environment ����̃C�x���g
 * �����������ɂ��邱�ƂɂȂ�H
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


/**
 * @return �h���C�o���̎擾
 */
IrrlichtDriver
IrrlichtBase::getDriver()
{
	return device ? IrrlichtDriver(device->getVideoDriver()) : IrrlichtDriver();
}
	
/**
 * @return �V�[���}�l�[�W�����̎擾
 */
IrrlichtSceneManager
IrrlichtBase::getSceneManager()
{
	return device ? IrrlichtSceneManager(device->getSceneManager()) : IrrlichtSceneManager();
}


/**
 * @return GUI�����̎擾
 */
IrrlichtGUIEnvironment
IrrlichtBase::getGUIEnvironment()
{
	return device ? IrrlichtGUIEnvironment(device->getGUIEnvironment()) : IrrlichtGUIEnvironment();
}
