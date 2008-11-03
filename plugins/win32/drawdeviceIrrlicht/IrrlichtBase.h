#ifndef IRRLICHTBASE_H
#define IRRLICHTBASE_H

#include <windows.h>
#include "tp_stub.h"
#include <irrlicht.h>

/**
 * Irrlicht �����̃x�[�X
 */
class IrrlichtBase :
	public irr::IEventReceiver,
	public tTVPContinuousEventCallbackIntf
{

protected:
	/// �f�o�C�X
	irr::IrrlichtDevice *device;

	void showDriverInfo();

	/**
	 * Irrlicht �Ăяo�������J�n
	 */
	void start();

	/**
	 * Irrlicht �Ăяo���������f
	 */
	void stop();
	
	// �f�o�C�X���蓖��
	virtual void attach(HWND hwnd);
	
	// �f�o�C�X�̔j��
	virtual void detach();
	
public:
	IrrlichtBase(); //!< �R���X�g���N�^
	virtual ~IrrlichtBase(); //!< �f�X�g���N�^

	// ------------------------------------------------------------
	// �X�V����
	// ------------------------------------------------------------
protected:
	/**
	 * �N���X�ŗL�X�V����
	 * �V�[���}�l�[�W���̏�����AGUI�̏����O�ɌĂ΂��
	 */
	virtual void update(irr::video::IVideoDriver *driver) {};

protected:
	/**
	 * �ĕ`�揈��
	 * @param destRect �`���̈�
	 */
	void show(irr::core::rect<irr::s32> *destRect=NULL);
	
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
	irr::video::IVideoDriver *getVideoDriver() {
		return device ? device->getVideoDriver() : NULL;
	}
	
	/**
	 * @return �V�[���}�l�[�W�����̎擾
	 */
	irr::scene::ISceneManager *getSceneManager() {
		return device ? device->getSceneManager() : NULL;
	}

	/**
	 * @return GUI�����̎擾
	 */
	irr::gui::IGUIEnvironment *getGUIEnvironment() {
		return device ? device->getGUIEnvironment() : NULL;
	}
};

#endif