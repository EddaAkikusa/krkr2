#ifndef IRRLICHTBASE_H
#define IRRLICHTBASE_H

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
	/// �f�o�C�X
	irr::IrrlichtDevice *device;
	/// �h���C�o
	irr::video::IVideoDriver *driver;

	void showDriverInfo();
	
public:
	IrrlichtBase(); //!< �R���X�g���N�^
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
	virtual void update(tjs_uint64 tick) {};

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

	// ------------------------------------------------------------
	// �V�[������
	// ------------------------------------------------------------
public:
	/**
	 * �摜�̃L���v�`��
	 * @param dest �i�[�惌�C��
	 */
	void captureScreenShot(iTJSDispatch2 *layer);

	/**
	 * �V�[���f�[�^�t�@�C���̓ǂݍ���
	 */
	void loadScene(const char *name);

	/**
	 * �J�����V�[���̐ݒ�
	 */
	//void addCameraSceneNode(int no, irr::core::vector3df *a, irr::core::vector3df *b);
};

#endif