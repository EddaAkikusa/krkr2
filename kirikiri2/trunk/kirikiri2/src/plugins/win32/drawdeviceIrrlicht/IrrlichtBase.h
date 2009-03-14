#ifndef IRRLICHTBASE_H
#define IRRLICHTBASE_H

#include <windows.h>
#include "tp_stub.h"
#include <irrlicht.h>

/**
 * Irrlicht �����̃x�[�X
 */
class IrrlichtBase : public irr::IEventReceiver
{

protected:
	/// TJS�I�u�W�F�N�g
	iTJSDispatch2 *objthis;
	/// �f�o�C�X
	irr::IrrlichtDevice *device;

	void showDriverInfo();

	// �C�x���g�Ăяo��
	void sendEvent(const tjs_char *eventName);

	// �f�o�C�X���蓖�čς�
	bool attached;
	// �f�o�C�X�̊��蓖��
	void attach(HWND hwnd, int width=0, int height=0);
	// �f�o�C�X�̊��蓖�Č㏈��
	virtual void onAttach() {}
	
	// �f�o�C�X�̔j��
	void detach();
	// �f�o�C�X�̔j���O����
	virtual void onDetach() {};
	
public:
	IrrlichtBase(iTJSDispatch2 *objthis); //!< �R���X�g���N�^
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
	 * Irrlicht�`�揈��
	 * @param destRect �`���̈�B�ȗ�����ƕ\����S��
	 * @param srcRect �`�挳�̈�B�ȗ�����ƑS��
	 * @param destDC �`���DC�B�w�肷��Ɩ{���̕\����̑���ɂ���DC�ɕ`��
	 * @return �`�悳�ꂽ�� true
	 */
	bool show(irr::core::rect<irr::s32> *destRect=NULL, irr::core::rect<irr::s32> *srcRect=NULL, HDC destDC=0);
	
	// ------------------------------------------------------------
	// Irrlicht �C�x���g�����p
	// ------------------------------------------------------------
public:
	// Irrlicht �ɃC�x���g�𑗂�
	bool postEvent(irr::SEvent &ev);

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

	/**
	 * @return ���K�[�̎擾
	 */
	irr::ILogger *getLogger() {
		return device ? device->getLogger() : NULL;
	}

	/**
	 * @return �t�@�C���V�X�e���̎擾
	 */
	irr::io::IFileSystem *getFileSystem() {
		return device ? device->getFileSystem() : NULL;
	}
};

/**
 * Irrlicht �����̃x�[�X�F�����X�V��
 */
class IrrlichtBaseUpdate : public IrrlichtBase, public tTVPContinuousEventCallbackIntf
{
protected:
	/**
	 * Irrlicht �Ăяo�������J�n
	 */
	void start();

	/**
	 * Irrlicht �Ăяo���������f
	 */
	void stop();
	
public:
	IrrlichtBaseUpdate(iTJSDispatch2 *objthis); //!< �R���X�g���N�^
	virtual ~IrrlichtBaseUpdate(); //!< �f�X�g���N�^
};

#endif