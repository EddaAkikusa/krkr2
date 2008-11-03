#ifndef IRRLICHTDRAWDEVICE_H
#define IRRLICHTDRAWDEVICE_H

#include "IrrlichtBase.h"
#include "BasicDrawDevice.h"

/**
 * Irrlicht �x�[�X�� DrawDevice
 */
class IrrlichtDrawDevice :
	public IrrlichtBase,
	public tTVPDrawDevice
{
	typedef tTVPDrawDevice inherited;

protected:
	tjs_int screenWidth;
	tjs_int screenHeight;
	tjs_int width;
	tjs_int height;
	bool zoomMode;
	irr::core::rect<irr::s32> destRect;

public:
	// �R���X�g���N�^
	IrrlichtDrawDevice(int width, int height);
	// �f�X�g���N�^
	virtual ~IrrlichtDrawDevice();

	// -----------------------------------------------------------------------
	// continuous handler
	// -----------------------------------------------------------------------
public:
	/**
	 * Continuous �R�[���o�b�N
	 */
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick);
	
protected:
	// zoomMode �X�V
	void updateZoomMode();

	// �e�N�X�`���̊��蓖��
	void allocInfo(iTVPLayerManager * manager);
	// �e�N�X�`���̉��
	void freeInfo(iTVPLayerManager * manager);

	// �f�o�C�X�������
	virtual void detach();
	
	// Irrlicht �ɃC�x���g�𑗂�
	bool postEvent(irr::SEvent &ev);

	/**
	 * Device��Irrlicht�����̍��W�̕ϊ����s��
	 * @param		x		X�ʒu
	 * @param		y		Y�ʒu
	 * @note		x, y �� DestRect�� (0,0) �����_�Ƃ�����W�Ƃ��ēn�����ƌ��Ȃ�
	 */
	void TransformTo(tjs_int &x, tjs_int &y);

	/** Irrlicht��Device�����̍��W�̕ϊ����s��
	 * @param		x		X�ʒu
	 * @param		y		Y�ʒu
	 * @note		x, y �� ���C���� (0,0) �����_�Ƃ�����W�Ƃ��ēn�����ƌ��Ȃ�
	 */
	void TransformFrom(tjs_int &x, tjs_int &y);

	// ------------------------------------------------------------
	// �X�V����
	// ------------------------------------------------------------
protected:
	/**
	 * �N���X�ŗL�X�V����
	 * �V�[���}�l�[�W���̏�����AGUI�̏����O�ɌĂ΂��
	 */
	void update(irr::video::IVideoDriver *driver);
	
public:
	//---- LayerManager �̊Ǘ��֘A
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD RemoveLayerManager(iTVPLayerManager * manager);

	//---- �`��ʒu�E�T�C�Y�֘A
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd, bool is_main);
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect);
	virtual void TJS_INTF_METHOD GetSrcSize(tjs_int &w, tjs_int &h);
	virtual void TJS_INTF_METHOD NotifyLayerResize(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyLayerImageChange(iTVPLayerManager * manager) {}

	//---- ���[�U�[�C���^�[�t�F�[�X�֘A
	// window �� drawdevice
	virtual void TJS_INTF_METHOD OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnKeyDown(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyUp(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyPress(tjs_char key);
	virtual void TJS_INTF_METHOD OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y);

	//---- �ĕ`��֘A
	virtual void TJS_INTF_METHOD Show();
	
	//---- LayerManager ����̉摜�󂯓n���֘A
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager);

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
	
public:
	/**
	 * @return �f�o�C�X���
	 */
	tjs_int64 getDevice() {
		return reinterpret_cast<tjs_int64>((tTVPDrawDevice*)this);
	}

	bool getZoomMode() {
		return zoomMode;
	}
	
	void setZoomMode(bool zoomMode) {
		this->zoomMode = zoomMode;
		updateZoomMode();
	}

	tjs_int getWidth() {
		return width;
	}
	
	void setWidth(tjs_int width) {
		this->width = width;
		Window->NotifySrcResize();
	}

	tjs_int getHeight() {
		return height;
	}
	
	void setHeight(tjs_int height) {
		this->height = height;
		Window->NotifySrcResize();
	}

	void setSize(tjs_int width, tjs_int height) {
		this->width = width;
		this->height = height;
		Window->NotifySrcResize();
	}
};

#endif
