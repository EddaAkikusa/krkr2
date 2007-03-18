//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//!@file �`��f�o�C�X�Ǘ�
//---------------------------------------------------------------------------
#ifndef DRAWDEVICE_H
#define DRAWDEVICE_H

#include "LayerIntf.h"
#include "ComplexRect.h"

//---------------------------------------------------------------------------
//! @brief		�`��f�o�C�X�C���^�[�t�F�[�X
//---------------------------------------------------------------------------
class iTVPDrawDevice
{
public:
//---- �I�u�W�F�N�g�������Ԑ���
	virtual void TJS_INTF_METHOD Destruct() = 0;

//---- �`��ʒu�E�T�C�Y�֘A
	//
	// null ���w�肳���ꍇ������B
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd) = 0;
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect) = 0;

//---- HID�C���^�[�t�F�[�X�֘A
	virtual void TJS_INTF_METHOD OnClick(tjs_int x, tjs_int y) = 0;
	virtual void TJS_INTF_METHOD OnDoubleClick(tjs_int x, tjs_int y) = 0;
	virtual void TJS_INTF_METHOD OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;
	virtual void TJS_INTF_METHOD OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;
	virtual void TJS_INTF_METHOD OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags) = 0;
	virtual void TJS_INTF_METHOD OnReleaseCapture() = 0;
	virtual void TJS_INTF_METHOD OnMouseOutOfWindow() = 0;
	virtual void TJS_INTF_METHOD OnKeyDown(tjs_uint key, tjs_uint32 shift) = 0;
	virtual void TJS_INTF_METHOD OnKeyUp(tjs_uint key, tjs_uint32 shift) = 0;
	virtual void TJS_INTF_METHOD OnKeyPress(tjs_char key) = 0;
	virtual void TJS_INTF_METHOD OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y) = 0;

//---- LayerManager �̊Ǘ��֘A
	virtual void TJS_INTF_METHOD AddLayerManager(tTVPLayerManager * manager) = 0;

//---- LayerManager ����̉摜�󂯓n���֘A
	virtual void TJS_INTF_METHOD StartBitmapCompletion(tTVPLayerManager * manager) = 0;
	virtual tTVPLayerType TJS_INTF_METHOD GetDesiredLayerType() = 0;
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(const tTVPRect &destrect,
		void * bits, BITMAPINFO * bitmapinfo, const tTVPRect &cliprect,
		tTVPLayerType type, tjs_int opacity) = 0;
	virtual void TJS_INTF_METHOD EndBitmapCompletion() = 0;

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		�`��f�o�C�X�C���^�[�t�F�[�X�̊�{�I�Ȏ���
//---------------------------------------------------------------------------
class tTVPDrawDevice : public iTVPDrawDevice
{
protected:
	size_t PrimaryLayerManagerIndex; //!< HID����n�����C���}�l�[�W��
	std::vector<tTVPLayerManager *> Managers; //!< ���C���}�l�[�W���̔z��
	tTVPRect DestRect; //!< �`���ʒu

protected:
	tTVPDrawDevice(); //!< �R���X�g���N�^
protected:
	virtual ~tTVPDrawDevice(); //!< �f�X�g���N�^

public:
	//! @brief		�w��ʒu�ɂ��郌�C���}�l�[�W���𓾂�
	//! @param		index		�C���f�b�N�X(0�`)
	//! @return		�w��ʒu�ɂ��郌�C���}�l�[�W��(AddRef����Ȃ��̂Œ���)�B
	//!				�w��ʒu�Ƀ��C���}�l�[�W�����Ȃ����NULL���Ԃ�
	tTVPLayerManager * GetLayerManagerAt(size_t index)
	{
		if(Managers.size() <= index) return NULL;
		return Managers[index];
	}

	//! @brief		Device->LayerManager�����̍��W�̕ϊ����s��
	//! @param		x		X�ʒu
	//! @param		y		Y�ʒu
	//! @return		�ϊ��ɐ�������ΐ^�B�����Ȃ���΋U�BPrimaryLayerManagerIndex�ɊY������
	//!				���C���}�l�[�W�����Ȃ���΋U���Ԃ�
	//! @note		x, y �� DestRect�� (0,0) �����_�Ƃ�����W�Ƃ��ēn�����ƌ��Ȃ�
	bool TransformToPrimaryLayerManager(tjs_int &x, tjs_int &y);

//---- �I�u�W�F�N�g�������Ԑ���
	virtual void TJS_INTF_METHOD Destruct();

//---- �`��ʒu�E�T�C�Y�֘A
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect);

//---- HID�C���^�[�t�F�[�X�֘A
	virtual void TJS_INTF_METHOD OnClick(tjs_int x, tjs_int y);
	virtual void TJS_INTF_METHOD OnDoubleClick(tjs_int x, tjs_int y);
	virtual void TJS_INTF_METHOD OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnReleaseCapture();
	virtual void TJS_INTF_METHOD OnMouseOutOfWindow();
	virtual void TJS_INTF_METHOD OnKeyDown(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyUp(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyPress(tjs_char key);
	virtual void TJS_INTF_METHOD OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y);

//---- LayerManager �̊Ǘ��֘A
	virtual void TJS_INTF_METHOD AddLayerManager(tTVPLayerManager * manager);


// �ق��̃��\�b�h�ɂ��Ă͎������Ȃ�
};
//---------------------------------------------------------------------------
#endif
