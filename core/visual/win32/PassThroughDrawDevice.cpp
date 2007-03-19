//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//!@file "PassThrough" �`��f�o�C�X�Ǘ�
//---------------------------------------------------------------------------

#include "tjsCommHead.h"
#include "DrawDevice.h"
#include "PassThroughDrawDevice.h"
#include "LayerIntf.h"
#include "MsgIntf.h"


//---------------------------------------------------------------------------
tTVPPassThroughDrawDevice::tTVPPassThroughDrawDevice()
{
	TargetWindow = NULL;
	TargetDC = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPPassThroughDrawDevice::~tTVPPassThroughDrawDevice()
{
	if(TargetDC && TargetWindow) ReleaseDC(TargetWindow, TargetDC);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::SetTargetWindow(HWND wnd)
{
	if(wnd)
	{
		// �`��p DC ���擾����
		TargetDC = GetDC(wnd);
	}
	else
	{
		// �`��p DC ���J������
		ReleaseDC(TargetWindow, TargetDC);
	}

	TargetWindow = wnd;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::AddLayerManager(iTVPLayerManager * manager)
{
	if(inherited::Managers.size() > 0)
	{
		// "Pass Through" �f�o�C�X�ł͂Q�ȏ��Layer Manager��o�^�ł��Ȃ�
		TVPThrowExceptionMessage(TJS_W("\"passthrough\" device does not support layer manager more than 1"));
			// TODO: i18n
	}
	inherited::AddLayerManager(manager);

	manager->SetDesiredLayerType(ltOpaque); // ltOpaque �ȏo�͂��󂯎�肽��
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::StartBitmapCompletion(iTVPLayerManager * manager)
{
	// ���ɂ�邱�ƂȂ�
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::NotifyBitmapCompleted(iTVPLayerManager * manager,
	const tTVPRect &destrect, const void * bits, const BITMAPINFO * bitmapinfo,
	const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)
{
	// bits, bitmapinfo �ŕ\�����r�b�g�}�b�v�� cliprect �̗̈���Adestrect �ɕ`��
	// ����Bdestrect �� ���ƍ����͖������Ă悢 (cliprect �̕��ƍ����Ɠ���Ƃ݂Ȃ���)
	// opacity �� type �͖������邵���Ȃ��̂Ŗ�������
	StretchDIBits(
		TargetDC,
		destrect.left + DestRect.left,
		destrect.top  + DestRect.top,
		destrect.get_width(),
		destrect.get_height(),
		cliprect.left,
		bitmapinfo->bmiHeader.biHeight - cliprect.get_height() - cliprect.top,
		cliprect.get_width(),
		cliprect.get_height(),
		bits,
		bitmapinfo,
		DIB_RGB_COLORS,
		SRCCOPY);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::EndBitmapCompletion(iTVPLayerManager * manager)
{
	// ���ɂ�邱�ƂȂ�
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
// tTJSNC_PassThroughDrawDevice : PassThroughDrawDevice TJS native class
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_PassThroughDrawDevice::ClassID = (tjs_uint32)-1;
tTJSNC_PassThroughDrawDevice::tTJSNC_PassThroughDrawDevice() :
	tTJSNativeClass(TJS_W("PassThroughDrawDevice"))
{
	// register native methods/properties

	TJS_BEGIN_NATIVE_MEMBERS(PassThroughDrawDevice)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
// constructor/methods
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var.name*/_this, /*var.type*/tTJSNI_PassThroughDrawDevice,
	/*TJS class name*/PassThroughDrawDevice)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/PassThroughDrawDevice)
//----------------------------------------------------------------------

//---------------------------------------------------------------------------
//----------------------------------------------------------------------
// properties
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(interface)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_PassThroughDrawDevice);
		*result = reinterpret_cast<tjs_int64>(_this->GetDevice());
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(interface)

//----------------------------------------------------------------------
	TJS_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------
iTJSNativeInstance *tTJSNC_PassThroughDrawDevice::CreateNativeInstance()
{
	return new tTJSNI_PassThroughDrawDevice();
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tTJSNI_PassThroughDrawDevice::tTJSNI_PassThroughDrawDevice()
{
	Device = new tTVPPassThroughDrawDevice();
}
//---------------------------------------------------------------------------
tTJSNI_PassThroughDrawDevice::~tTJSNI_PassThroughDrawDevice()
{
	if(Device) Device->Destruct(), Device = NULL;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSNI_PassThroughDrawDevice::Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj)
{
	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSNI_PassThroughDrawDevice::Invalidate()
{
	if(Device) Device->Destruct(), Device = NULL;
}
//---------------------------------------------------------------------------

