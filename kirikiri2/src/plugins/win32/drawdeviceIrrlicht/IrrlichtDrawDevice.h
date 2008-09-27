#ifndef IRRLICHTDRAWDEVICE_H
#define IRRLICHTDRAWDEVICE_H

#include "IrrlichtBase.h"
#include "BasicDrawDevice.h"

/**
 * Irrlicht �x�[�X�� DrawDevice
 */
class IrrlichtDrawDevice : public tTVPDrawDevice,
						   public IrrlichtBase
{
	typedef tTVPDrawDevice inherited;
	
public:
	// �R���X�g���N�^
	IrrlichtDrawDevice();
	// �f�X�g���N�^
	virtual ~IrrlichtDrawDevice();

private:
	// �f�o�C�X�������
	virtual void detach();

	// �e�N�X�`���̊��蓖��
	void allocInfo(iTVPLayerManager * manager);
	// �e�N�X�`���̉��
	void freeInfo(iTVPLayerManager * manager);

	// Irrlicht �ɃC�x���g�𑗂�
	bool postEvent(irr::SEvent &ev);
	
public:
	//---- LayerManager �̊Ǘ��֘A
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD RemoveLayerManager(iTVPLayerManager * manager);

	//---- �`��ʒu�E�T�C�Y�֘A
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd, bool is_main);

//---- �ĕ`��֘A
	virtual void TJS_INTF_METHOD Show();
	
	//---- ���[�U�[�C���^�[�t�F�[�X�֘A
	// window �� drawdevice
	virtual void TJS_INTF_METHOD OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnKeyDown(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyUp(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyPress(tjs_char key);
	virtual void TJS_INTF_METHOD OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y);
	
	//---- LayerManager ����̉摜�󂯓n���֘A
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager);

	// ------------------------------------------------------------
	// �g���g������̌Ăяo������p
	// ------------------------------------------------------------
protected:
	/**
	 * �N���X�ŗL�X�V����
	 * �V�[���}�l�[�W���̏�����AGUI�̏����O�ɌĂ΂��
	 */
	void update(tjs_uint64 tick);

public:
	/**
	 * @return �f�o�C�X���
	 */
	tjs_int64 GetDevice() {
		return reinterpret_cast<tjs_int64>((tTVPDrawDevice*)this);
	}
};

#endif
