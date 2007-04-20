#ifndef IRRLICHTDRAWDEVICE_H
#define IRRLICHTDRAWDEVICE_H

#include <windows.h>
#include "tp_stub.h"
#include <iostream>
#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;

#include <string>
using namespace std;

#include "BasicDrawDevice.h"


/**
 * ���C���}�l�[�W���p�t�����
 */
class LayerManagerInfo {
	
public:
	// �R���X�g���N�^
	LayerManagerInfo(ITexture *texture);
	virtual ~LayerManagerInfo();
	
private:
	// �R�s�[�����p�ꎞ�ϐ�
	unsigned char *destBuffer;
	int destWidth;
	int destHeight;
	int destPitch;
	
public:
	// ���蓖�ăe�N�X�`��
	ITexture *texture;

	void lock();
	void copy(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
			  const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	void unlock();
};

/**
 * Irrlicht �x�[�X�� DrawDevice
 */
class tTVPIrrlichtDrawDevice : public tTVPDrawDevice, public tTVPContinuousEventCallbackIntf, public IEventReceiver
{
	typedef tTVPDrawDevice inherited;

	/// �f�o�C�X
	IrrlichtDevice *device;

public:
	tTVPIrrlichtDrawDevice(); //!< �R���X�g���N�^
private:
	virtual ~tTVPIrrlichtDrawDevice(); //!< �f�X�g���N�^

	// �e�N�X�`���̊��蓖��
	void allocInfo(iTVPLayerManager * manager);
	// �e�N�X�`���̉��
	void freeInfo(iTVPLayerManager * manager);
	
	IrrlichtDevice *create(HWND hwnd, video::E_DRIVER_TYPE type);
	void attach(HWND hwnd);
	void detach();

	int width;
	int height;

public:
	//---- LayerManager �̊Ǘ��֘A
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD RemoveLayerManager(iTVPLayerManager * manager);

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


	// ------------------------------------------------------------
	// �g���g������̌Ăяo������p
	// ------------------------------------------------------------
	
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
	// �C�x���g�����p
	// ------------------------------------------------------------

	virtual bool OnEvent(SEvent event);
};

#endif
