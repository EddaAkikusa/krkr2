#ifndef IRRLICHTBASE_H
#define IRRLICHTBASE_H

#include "IrrlichtDriver.h"
#include "IrrlichtSceneManager.h"
#include "IrrlichtGUIEnvironment.h"

#include <windows.h>
#include "tp_stub.h"
#include <irrlicht.h>

/**
 * Irrlicht �����̃x�[�X
 */
class IrrlichtBase :
	public tTVPContinuousEventCallbackIntf,
	public irr::IEventReceiver
{

protected:
	// �h���C�o���
	irr::video::E_DRIVER_TYPE  driverType;
	/// �f�o�C�X
	irr::IrrlichtDevice *device;

	void showDriverInfo();
	
public:
	IrrlichtBase(irr::video::E_DRIVER_TYPE driverType); //!< �R���X�g���N�^
	virtual ~IrrlichtBase(); //!< �f�X�g���N�^

protected:
	// �f�o�C�X���蓖��
	virtual void attach(HWND hwnd);
	
	// �f�o�C�X�̔j��
	virtual void detach();

	// ------------------------------------------------------------
	// �g���g������̌Ăяo������p
	// ------------------------------------------------------------
protected:
	/**
	 * �N���X�ŗL�X�V����
	 * �V�[���}�l�[�W���̏�����AGUI�̏����O�ɌĂ΂��
	 */
	virtual void update(irr::video::IVideoDriver *driver, tjs_uint64 tick) {};

public:
	/**
	 * Irrlicht �Ăяo�������J�n
	 */
	void start();

	/**
	 * Irrlicht �Ăяo���������f
	 */
	void stop();

	/**
	 * Continuous �R�[���o�b�N
	 * �g���g�����ɂȂƂ��ɏ�ɌĂ΂��
	 * �h�蒼������
	 */
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick);

	// ------------------------------------------------------------
	// Irrlicht �C�x���g�����p
	// ------------------------------------------------------------
public:
	/**
	 * �C�x���g��
	 * GUI Environment ����̃C�x���g�������ɑ����Ă���
	 * @param event �C�x���g���
	 * @return ���������� true
	 */
	virtual bool OnEvent(const irr::SEvent &event);

public:
	/**
	 * @return �h���C�o���̎擾
	 */
	IrrlichtDriver getDriver();
	
	/**
	 * @return �V�[���}�l�[�W�����̎擾
	 */
	IrrlichtSceneManager getSceneManager();

	/**
	 * @return GUI�����̎擾
	 */
	IrrlichtGUIEnvironment getGUIEnvironment();
};

#endif