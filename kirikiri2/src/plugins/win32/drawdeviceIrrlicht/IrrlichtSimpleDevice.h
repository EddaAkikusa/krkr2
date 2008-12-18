#ifndef IrrlichtSimpleDevice_H
#define IrrlichtSimpleDevice_H

#include "IrrlichtBase.h"

/**
 * Irrlicht �`��f�o�C�X
 */
class IrrlichtSimpleDevice :	public IrrlichtBase
{

protected:
	iTJSDispatch2 *window; //< �I�u�W�F�N�g���̎Q��(�e�E�C���h�E)
	tjs_int width;  //< Irrlicht ����ʂ̉�ʉ���
	tjs_int height; //< Irrlicht ����ʂ̉�ʏc��

	// �C�x���g����
	static bool __stdcall messageHandler(void *userdata, tTVPWindowMessage *Message);
	
	// ���[�U���b�Z�[�W���V�[�o�̓o�^/����
	void setReceiver(tTVPWindowMessageReceiver receiver, bool enable);
	
	/**
	 * �E�C���h�E�𐶐�
	 * @param krkr �g���g���̃E�C���h�E
	 */
	void createWindow(HWND krkr);

	/**
	 * �E�C���h�E��j��
	 */
	void destroyWindow();
	
public:
	/**
	 * �R���X�g���N�^
	 * @param window �e�ɂȂ鑋
	 * @param width ����
	 * @param height �c��
	 */
	IrrlichtSimpleDevice(iTJSDispatch2 *window, int width, int height);
		
	/**
	 * �f�X�g���N�^
	 */
	virtual ~IrrlichtSimpleDevice();

	// -----------------------------------------------------------------------
	// ���ʃ��\�b�h�Ăяo���p
	// -----------------------------------------------------------------------

public:
	irr::video::IVideoDriver *getVideoDriver() {
		return IrrlichtBase::getVideoDriver();
	}

	irr::scene::ISceneManager *getSceneManager() {
		return IrrlichtBase::getSceneManager();
	}

	irr::gui::IGUIEnvironment *getGUIEnvironment() {
		return IrrlichtBase::getGUIEnvironment();
	}

	irr::ILogger *getLogger() {
		return IrrlichtBase::getLogger();
	}

	irr::io::IFileSystem *getFileSystem() {
		return IrrlichtBase::getFileSystem();
	}

	// -----------------------------------------------------------------------
	// �ŗL���\�b�h
	// -----------------------------------------------------------------------
protected:
	void _setSize();
	
public:
	void setWidth(int w) {
		if (width != w) {
			width = w;
			_setSize();
		}
	}

	int getWidth() {
		return width;
	}

	void setHeight(int h) {
		if (height != h) {
			height = h;
			_setSize();
		}
	}

	int getHeight() {
		return height;
	}
	
	/**
	 * ���T�C�Y�w��
	 */	
	void setSize(int w, int h) {
		if (width != w || height != h) {
			width = w;
			height = h;
			_setSize();
		}
	}

	/**
	 * ���C���ɑ΂��čX�V�`��
	 */
	void updateToLayer(iTJSDispatch2 *layer);
};

#endif
