#include "WindowExBase.h"

int NI_WindowExBase::classId;

iTJSDispatch2 * NI_WindowExBase::_HWNDProp = NULL;
iTJSDispatch2 * NI_WindowExBase::_innerWidthProp = NULL;
iTJSDispatch2 * NI_WindowExBase::_innerHeightProp = NULL;
iTJSDispatch2 * NI_WindowExBase::_primaryLayerProp = NULL;

iTJSDispatch2 * NI_WindowExBase::_leftProp   = NULL;
iTJSDispatch2 * NI_WindowExBase::_topProp    = NULL;
iTJSDispatch2 * NI_WindowExBase::_widthProp  = NULL;
iTJSDispatch2 * NI_WindowExBase::_heightProp = NULL;
iTJSDispatch2 * NI_WindowExBase::_pitchProp  = NULL;
iTJSDispatch2 * NI_WindowExBase::_bufferProp = NULL;
iTJSDispatch2 * NI_WindowExBase::_updateProp = NULL;

void
NI_WindowExBase::init(iTJSDispatch2 *windowobj, iTJSDispatch2 *layerobj)
{
	// �v���p�e�B�擾
	tTJSVariant var;

	if (TJS_FAILED(windowobj->PropGet(TJS_IGNOREPROP, TJS_W("HWND"), NULL, &var, windowobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Window.HWND failed."));
	} else {
		_HWNDProp = var;
	}

	if (TJS_FAILED(windowobj->PropGet(TJS_IGNOREPROP, TJS_W("innerWidth"), NULL, &var, windowobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Window.innerWidth failed."));
	} else {
		_innerWidthProp = var;
	}

	if (TJS_FAILED(windowobj->PropGet(TJS_IGNOREPROP, TJS_W("innerHeight"), NULL, &var, windowobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Window.innerHeight failed."));
	} else {
		_innerHeightProp = var;
	}

	if (TJS_FAILED(windowobj->PropGet(TJS_IGNOREPROP, TJS_W("primaryLayer"), NULL, &var, windowobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Window.primaryLayer failed."));
	} else {
		_primaryLayerProp = var;
	}
	
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("imageLeft"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageLeft failed."));
	} else {
		_leftProp = var;
	}
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("imageTop"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageTop failed."));
	} else {
		_topProp = var;
	}
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("imageWidth"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageWidth failed."));
	} else {
		_widthProp = var;
	}
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("imageHeight"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageHeight failed."));
	} else {
		_heightProp = var;
	}
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("mainImageBuffer"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.mainImageBuffer failed."));
	} else {
		_bufferProp = var;
	}
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("mainImageBufferPitch"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.mainImageBufferPitch failed."));
	} else {
		_pitchProp = var;
	}
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("update"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.update failed."));
	} else {
		_updateProp = var;
	}
}

void
NI_WindowExBase::unInit()
{
	// ���C���p�v���p�e�B�̉��
	if (_leftProp)   _leftProp->Release();
	if (_topProp)    _topProp->Release();
	if (_widthProp)  _widthProp->Release();
	if (_heightProp) _heightProp->Release();
	if (_bufferProp) _bufferProp->Release();
	if (_pitchProp)  _pitchProp->Release();
	if (_updateProp) _updateProp->Release();

	// �E�C���h�E�p�v���p�e�B�̉��
	if (_primaryLayerProp)   _primaryLayerProp->Release();
	if (_innerWidthProp) _innerWidthProp->Release();
	if (_innerHeightProp) _innerHeightProp->Release();
	if (_HWNDProp) _HWNDProp->Release();
}

/// �v���p�e�B���� int �l���擾����
static tjs_int64 getPropValue(iTJSDispatch2 *dispatch, iTJSDispatch2 *layerobj)
{
	tTJSVariant var;
	if(TJS_FAILED(dispatch->PropGet(0, NULL, NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("can't get int value from property."));
	}
	return var;
}

/**
 * �����L�^��Ԃ̏������p�B��O���������̂� TVPDoTryBlock �p�ɕ���
 */

void
NI_WindowExBase::_reset()
{
	// �v���C�}�����C�����擾
	// ���s�����ꍇ�͗�O�ɂȂ�
	tTJSVariant primaryLayer;
	_primaryLayerProp->PropGet(0, NULL, NULL, &primaryLayer, _windowobj);
	
	if (primaryLayer.Type() == tvtObject) {
		iTJSDispatch2 *layerobj = primaryLayer.AsObjectNoAddRef();
		_width  = (int)getPropValue(_widthProp, layerobj);
		_height = (int)getPropValue(_heightProp, layerobj);
		_buffer = (unsigned char *)getPropValue(_bufferProp, layerobj);
		_pitch  = (int)getPropValue(_pitchProp, layerobj);
	}
}

//---------------------------------------------------------------------------
// ��O�����p

// ��O���������u���b�N
static void TJS_USERENTRY TryBlock1(void * data)
{
	NI_WindowExBase *winBase = (NI_WindowExBase*)data;
	winBase->_reset();
}

// ��O������
static bool TJS_USERENTRY CatchBlock1(void * data, const tTVPExceptionDesc & desc)
{
	NI_WindowExBase *winBase = (NI_WindowExBase*)data;
	winBase->_width  = 0;
	winBase->_height = 0;
	winBase->_pitch  = 0;
	winBase->_buffer = NULL;
	TVPAddLog(TJS_W("exception type:") + desc.type + TJS_W(", message:") + desc.message);
	return false;
}

// �����㏈��
static void TJS_USERENTRY FinallyBlock1(void * data)
{
	// nothing to do
}
//---------------------------------------------------------------------------

/**
 * ������Ԃ�����������
 */
void
NI_WindowExBase::reset(iTJSDispatch2 *windowobj)
{
	_windowobj = windowobj; // no add ref
	_hwnd = (HWND)getPropValue(_HWNDProp, windowobj);
	_innerWidth  = getPropValue(_innerWidthProp, windowobj);
	_innerHeight = getPropValue(_innerHeightProp, windowobj);
	// ��O��������ꍇ������̂Ńu���b�L���O
	TVPDoTryBlock(TryBlock1, CatchBlock1, FinallyBlock1, (void*)this);
}

/**
 * �E�C���h�E�g���̃l�C�e�B�u�C���X�^���X�̎擾
 * @param objthis this
 * @param create true �Ȃ瑶�݂��Ȃ����ɐ���
 */
NI_WindowExBase *
NI_WindowExBase::getNative(iTJSDispatch2 *objthis, bool create)
{
	NI_WindowExBase *_this = NULL;
	if (TJS_FAILED(objthis->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
												  classId, (iTJSNativeInstance**)&_this)) && create) {
		_this = new NI_WindowExBase();
		if (TJS_FAILED(objthis->NativeInstanceSupport(TJS_NIS_REGISTER,
													  classId, (iTJSNativeInstance **)&_this))) {
			delete _this;
			_this = NULL;
		}
	}
	return _this;
}
	
/**
 * �R���X�g���N�^
 */
NI_WindowExBase::NI_WindowExBase()
{
	_width  = 0;
	_height = 0;
	_pitch  = 0;
	_buffer = NULL;
}
