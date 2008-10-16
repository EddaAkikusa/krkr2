#ifndef IRRLICHTWINDOW_H
#define IRRLICHTWINDOW_H

#include "IrrlichtBase.h"

extern void registerWindowClass();
extern void unregisterWindowClass();

/**
 * Irrlicht �x�[�X�� DrawDevice
 */
class IrrlichtWindow : public IrrlichtBase
{
protected:
	HWND parent; //< �e�̃n���h��
	HWND hwnd; //< ���݂̃n���h��
	iTJSDispatch2 *window; //< �I�u�W�F�N�g���̎Q��
	
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
	 */
	IrrlichtWindow(int driverType, iTJSDispatch2 *win, int left, int top, int width, int height);
		
	/**
	 * �f�X�g���N�^
	 */
	virtual ~IrrlichtWindow();

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
	
	// -----------------------------------------------------------------------
	// �ŗL���\�b�h
	// -----------------------------------------------------------------------
	
protected:
	bool visible;
	int left;
	int top;
	int width;
	int height;

	void _setPos();
	
public:
	void setVisible(bool v);
	bool getVisible();

	void setLeft(int l);
	int getLeft();

	void setTop(int t);
	int getTop();
	
	void setWidth(int w);
	int getWidth();

	void setHeight(int h);
	int getHeight();
	
	/**
	 * ���ꏊ�w��
	 */	
	void setPos(int l, int t);

	/**
	 * ���T�C�Y�w��
	 */	
	void setSize(int w, int h);
};

#endif
