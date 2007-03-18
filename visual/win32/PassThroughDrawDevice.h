//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//!@file "PassThrough" �`��f�o�C�X�Ǘ�
//---------------------------------------------------------------------------
#ifndef PASSTHROUGHDRAWDEVICE_H
#define PASSTHROUGHDRAWDEVICE_H

#include "DrawDevice.h"

//---------------------------------------------------------------------------
//! @brief		�uPass Through�v�f�o�C�X(�����Ƃ���{�I�ȕ`����s���݂̂̃f�o�C�X)
//---------------------------------------------------------------------------
class tTVPPassThroughDrawDevice : public tTVPDrawDevice
{
	typedef tTVPDrawDevice inherited;

	HWND TargetWindow; // �^�[�Q�b�g�E�B���h�E
	HDC TargetDC; // �^�[�Q�b�g�E�B���h�E�̕`��pDC

public:
	tTVPPassThroughDrawDevice(); //!< �R���X�g���N�^
private:
	~tTVPPassThroughDrawDevice(); //!< �f�X�g���N�^
public:

//---- �`��ʒu�E�T�C�Y�֘A
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd);

//---- LayerManager �̊Ǘ��֘A
	virtual void TJS_INTF_METHOD AddLayerManager(tTVPLayerManager * manager);

//---- LayerManager ����̉摜�󂯓n���֘A
	virtual void TJS_INTF_METHOD StartBitmapCompletion(tTVPLayerManager * manager);
	virtual tTVPLayerType TJS_INTF_METHOD GetDesiredLayerType();
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(const tTVPRect &destrect,
		void * bits, BITMAPINFO * bitmapinfo, const tTVPRect &cliprect,
		tTVPLayerType type, tjs_int opacity);
	virtual void TJS_INTF_METHOD EndBitmapCompletion();

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tTJSNI_PassThroughDrawDevice
//---------------------------------------------------------------------------
class tTJSNI_PassThroughDrawDevice :
	public tTJSNativeInstance
{
	typedef tTJSNativeInstance inherited;

	tTVPPassThroughDrawDevice * Device;

public:
	tTJSNI_PassThroughDrawDevice();
	~tTJSNI_PassThroughDrawDevice();
	tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param,
			iTJSDispatch2 *tjs_obj);
	void TJS_INTF_METHOD Invalidate();

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNC_PassThroughDrawDevice
//---------------------------------------------------------------------------
class tTJSNC_PassThroughDrawDevice : public tTJSNativeClass
{
public:
	tTJSNC_PassThroughDrawDevice();

	static tjs_uint32 ClassID;

private:
	iTJSNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------


#endif
