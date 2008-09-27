#include "IrrlichtBase.h"

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
IrrlichtBase::IrrlichtBase()
{
	device = NULL;
	driver = NULL;
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
	if (driver) {
		dimension2d<s32> size = driver->getScreenSize();
		message_log("�f�o�C�X������̃X�N���[���T�C�Y:%d, %d", size.Width, size.Height);
		size = driver->getCurrentRenderTargetSize();
		message_log("�f�o�C�X�������RenderTarget��:%d, %d", size.Width, size.Height);
	}
}

/**
 * �E�C���h�E�̉���
 */
void
IrrlichtBase::detach()
{
	if (device) {
		device->closeDevice();
		device->drop();
		device = NULL;
		driver = NULL;
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
		
		// �`��J�n
		driver->beginScene(true, true, irr::video::SColor(255,0,0,0));
		
		/// �V�[���}�l�[�W���̕`��
		device->getSceneManager()->drawAll();
		
		// �ŗL����
		update(tick);
		
		// GUI�̕`��
		device->getGUIEnvironment()->drawAll();
		
		// �`�抮��
		driver->endScene();
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
