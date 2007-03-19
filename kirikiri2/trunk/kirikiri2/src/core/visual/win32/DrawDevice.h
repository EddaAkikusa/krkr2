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
#include "LayerManager.h"
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
	virtual void TJS_INTF_METHOD GetSrcSize(tjs_int &w, tjs_int &h) = 0;

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

//---- �v���C�}�����C���֘A
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer() = 0;
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer() = 0;
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer) = 0;

//---- LayerManager �̊Ǘ��֘A
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager) = 0;
	virtual void TJS_INTF_METHOD RemoveLayerManager(iTVPLayerManager * manager) = 0;

//---- �ĕ`��֘A
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect & rect) = 0;
	virtual void TJS_INTF_METHOD Update() = 0;

//---- LayerManager ����̉摜�󂯓n���֘A
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager) = 0;
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity) = 0;
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager) = 0;

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		�`��f�o�C�X�C���^�[�t�F�[�X�̊�{�I�Ȏ���
//---------------------------------------------------------------------------
class tTVPDrawDevice : public iTVPDrawDevice
{
protected:
	size_t PrimaryLayerManagerIndex; //!< �v���C�}�����C���}�l�[�W��
	std::vector<iTVPLayerManager *> Managers; //!< ���C���}�l�[�W���̔z��
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
	iTVPLayerManager * GetLayerManagerAt(size_t index)
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
	virtual void TJS_INTF_METHOD GetSrcSize(tjs_int &w, tjs_int &h);

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

//---- �v���C�}�����C���֘A
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer();
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer();
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer);

//---- LayerManager �̊Ǘ��֘A
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD RemoveLayerManager(iTVPLayerManager * manager);

//---- �ĕ`��֘A
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect & rect);
	virtual void TJS_INTF_METHOD Update();

// �ق��̃��\�b�h�ɂ��Ă͎������Ȃ�
};
//---------------------------------------------------------------------------
#endif
