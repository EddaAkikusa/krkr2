#ifndef IRRLICHTDRAWDEVICE_H
#define IRRLICHTDRAWDEVICE_H

#include <windows.h>
#include "tp_stub.h"
#include "BasicDrawDevice.h"

#include <irrlicht.h>
#include <iostream>
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;

#include <string>
using namespace std;

/**
 * Irrlicht �x�[�X�� DrawDevice
 */
class tTVPIrrlichtDrawDevice : public tTVPDrawDevice, public tTVPContinuousEventCallbackIntf
{
	typedef tTVPDrawDevice inherited;

	/// �f�o�C�X
	IrrlichtDevice *device;
	/// �h���C�o
	video::IVideoDriver* driver;
	/// �V�[���}�l�[�W��
	ISceneManager* smgr;

	/// ���C���`���e�N�X�`��
	ITexture *texture;
	
public:
	tTVPIrrlichtDrawDevice(); //!< �R���X�g���N�^
private:
	virtual ~tTVPIrrlichtDrawDevice(); //!< �f�X�g���N�^

	IrrlichtDevice *create(HWND hwnd, video::E_DRIVER_TYPE type);
	void attach(HWND hwnd);
	void detach();

	int width;
	int height;

	unsigned char *destBuffer;
	int destWidth;
	int destHeight;
	int destPitch;

public:

	//---- �`��ʒu�E�T�C�Y�֘A
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd);

	//---- LayerManager ����̉摜�󂯓n���֘A
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager);

	//---------------------------------------------------------------------------

	// �e�X�g�p����
	void init();

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
};

#endif
