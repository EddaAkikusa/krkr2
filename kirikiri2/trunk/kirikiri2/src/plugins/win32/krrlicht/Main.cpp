#include <windows.h>
#include <stdio.h>
#include "tp_stub.h"

#include <irrlicht.h>
#include <iostream>
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
#pragma comment(lib, "Irrlicht.lib")

/**
 * ���O�o�͗p
 */
static void log(const tjs_char *format, ...)
{
	va_list args;
	va_start(args, format);
	tjs_char msg[1024];
	_vsnwprintf(msg, 1024, format, args);
	TVPAddLog(msg);
	va_end(args);
}

//---------------------------------------------------------------------------

static void
addMember(iTJSDispatch2 *dispatch, const tjs_char *name, iTJSDispatch2 *member)
{
	tTJSVariant var = tTJSVariant(member);
	member->Release();
	dispatch->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		name, // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
		NULL, // �q���g ( �{���̓����o���̃n�b�V���l�����ANULL �ł��悢 )
		&var, // �o�^����l
		dispatch // �R���e�L�X�g
		);
}

static iTJSDispatch2*
getMember(iTJSDispatch2 *dispatch, const tjs_char *name)
{
	tTJSVariant val;
	if (TJS_FAILED(dispatch->PropGet(TJS_IGNOREPROP,
									 name,
									 NULL,
									 &val,
									 dispatch))) {
		ttstr msg = TJS_W("can't get member:");
		msg += name;
		TVPThrowExceptionMessage(msg.c_str());
	}
	return val.AsObject();
}

static bool
isValidMember(iTJSDispatch2 *dispatch, const tjs_char *name)
{
	return dispatch->IsValid(TJS_IGNOREPROP,
							 name,
							 NULL,
							 dispatch) == TJS_S_TRUE;
}

static void
delMember(iTJSDispatch2 *dispatch, const tjs_char *name)
{
	dispatch->DeleteMember(
		0, // �t���O ( 0 �ł悢 )
		name, // �����o��
		NULL, // �q���g
		dispatch // �R���e�L�X�g
		);
}

//---------------------------------------------------------------------------

/**
 * Irrlicht ���N���X
 * �EIrrlicht �̃f�o�C�X����ێ����܂�
 * �EIrrlicht �̃f�o�C�X�ɑ΂���`�����ێ����܂�
 */
class NI_Irrlicht : public tTJSNativeInstance, tTVPContinuousEventCallbackIntf
{

protected:
	// ���C����������擾���邽�߂̃v���p�e�B
	// �����ł����������邽�߃L���b�V�����Ă���
	static iTJSDispatch2 * _leftProp;
	static iTJSDispatch2 * _topProp;
	static iTJSDispatch2 * _widthProp;
	static iTJSDispatch2 * _heightProp;
	static iTJSDispatch2 * _pitchProp;
	static iTJSDispatch2 * _bufferProp;
	static iTJSDispatch2 * _updateProp;

public:
	// �X�^�e�B�b�N�n�̏���������
	static void init(iTJSDispatch2 *layerobj);
	static void unInit();
	
private:
	/// �f�o�C�X
	IrrlichtDevice *device;
	/// �h���C�o
	video::IVideoDriver* driver;
	/// �V�[���}�l�[�W��
	ISceneManager* smgr;

	/// ���C���`���e�N�X�`��
	ITexture *texture;

	/// ���C��
	iTJSDispatch2 *layer;
	
public:

	/**
	 * �R���X�g���N�^
	 */
	NI_Irrlicht() {
	}

	/**
	 * �R���X�g���N�^
	 */
	tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj) {

		tjs_int width  = 800;
		tjs_int height = 600;
		ttstr title    = "test title";
		if (numparams >= 1 && param[0]->Type() != tvtVoid) {
			width = (tjs_int)*param[0];
		}
		if (numparams >= 2 && param[1]->Type() != tvtVoid) {
			height = (tjs_int)*param[1];
		}
		if (numparams >= 3 && param[2]->Type() != tvtVoid) {
			title = *param[2];
		}
		if (numparams >= 4 && param[3]->Type() == tvtObject) {
			layer = param[3]->AsObject();
		} else {
			layer = NULL;
		}

		device = createDevice(video::EDT_DIRECT3D9, core::dimension2d<s32>(width, height));
		if (device == NULL) {
			device = createDevice(video::EDT_DIRECT3D8, core::dimension2d<s32>(width, height));
			if (device == NULL) {
				device = createDevice(video::EDT_OPENGL, core::dimension2d<s32>(width, height));
			}
		}
		
		if (device == NULL) {
			TVPThrowExceptionMessage(L"Irrlicht �f�o�C�X�̏������Ɏ��s���܂���");
		}

		device->setWindowCaption(title.c_str());
		driver = device->getVideoDriver();
		smgr   = device->getSceneManager();
		texture = driver->addTexture(core::dimension2d<s32>(1024, 1024), "layer", ECF_A8R8G8B8);

#if 0
		// �ȉ��T���v���f�[�^�̃��[�h����
		ttstr dataPath = "sydney.md2";
		dataPath = TVPNormalizeStorageName(dataPath);
		TVPGetLocalName(dataPath);
		int len = dataPath.GetNarrowStrLen() + 1;
		char *str = new char[len];
		dataPath.ToNarrowStr(str, len);
		IAnimatedMesh *mesh = smgr->getMesh(str);
		IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);
		if (node) {
			node->setMaterialFlag(EMF_LIGHTING, false);
			node->setMD2Animation ( scene::EMAT_STAND );
		}
		smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));
#else
		ttstr dataPath = "map-20kdm2.pk3";
		dataPath = TVPNormalizeStorageName(dataPath);
		TVPGetLocalName(dataPath);
		int len = dataPath.GetNarrowStrLen() + 1;
		char *str = new char[len];
		dataPath.ToNarrowStr(str, len);
		device->getFileSystem()->addZipFileArchive(str);

		scene::IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
		scene::ISceneNode* node = 0;
		
		if (mesh)
			node = smgr->addOctTreeSceneNode(mesh->getMesh(0));
		
		if (node)
			node->setPosition(core::vector3df(-1300,-144,-1249));

		smgr->addCameraSceneNodeFPS();
		device->getCursorControl()->setVisible(false);
#endif
		
		// ���[�N�v���b�N�Ƃ��ēo�^
		TVPAddContinuousEventHook(this);
		
		return S_OK;
	}

	void TJS_INTF_METHOD Invalidate() {
		// ���[�N�v���b�N�̉���
		TVPRemoveContinuousEventHook(this);
		device->drop();
		if (layer) {
			layer->Release();
		}
	}


protected:
	/**
	 * ���C����`�悷��
	 * @param layerobj �^�[�Q�b�g�ɂȂ郌�C��
	 */
	void drawLayer(iTJSDispatch2 *layerobj);
	
public:
	/**
	 * Continuous �R�[���o�b�N
	 * �g���g�����ɂȂƂ��ɏ�ɌĂ΂��
	 */
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick) {
		if (driver) {
			device->getTimer()->tick();
			if (device->isWindowActive()) {
				driver->beginScene(true, true, video::SColor(255,100,101,140));
				smgr->drawAll();
				drawLayer(layer);
				driver->endScene();
			}
		}
	}

};

// �v���p�e�B
iTJSDispatch2 * NI_Irrlicht::_leftProp   = NULL;
iTJSDispatch2 * NI_Irrlicht::_topProp    = NULL;
iTJSDispatch2 * NI_Irrlicht::_widthProp  = NULL;
iTJSDispatch2 * NI_Irrlicht::_heightProp = NULL;
iTJSDispatch2 * NI_Irrlicht::_pitchProp  = NULL;
iTJSDispatch2 * NI_Irrlicht::_bufferProp = NULL;
iTJSDispatch2 * NI_Irrlicht::_updateProp = NULL;

/**
 * �v���p�e�B�擾����
 * @param layerobj ���C���N���X�I�u�W�F�N�g
 */
void
NI_Irrlicht::init(iTJSDispatch2 *layerobj)
{
	// �v���p�e�B�擾
	tTJSVariant var;
	
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

/**
 * �v���p�e�B���
 */
void
NI_Irrlicht::unInit()
{
	if (_leftProp)   _leftProp->Release();
	if (_topProp)    _topProp->Release();
	if (_widthProp)  _widthProp->Release();
	if (_heightProp) _heightProp->Release();
	if (_bufferProp) _bufferProp->Release();
	if (_pitchProp)  _pitchProp->Release();
	if (_updateProp) _updateProp->Release();
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
 * ���C���̕`�揈��
 * Irrlicht �̋�Ԃɑ΂��Ă��郌�C���̓��e���z���o���ĕ`�悷��
 */
void
NI_Irrlicht::drawLayer(iTJSDispatch2 *layerobj)
{
	if (layerobj && texture) {

		// �X�V

		// �v���p�e�B�擾
		{
			tTJSVariant var;
			if (TJS_SUCCEEDED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("onPaint"), NULL, &var, layerobj))) {
				var.AsObjectNoAddRef()->FuncCall(0, NULL, NULL, NULL, 0, NULL, layerobj);
			}
		}
		
		// ���C���p�����[�^�擾
		int srcWidth             = (int)getPropValue(_widthProp, layerobj);
		int srcHeight            = (int)getPropValue(_heightProp, layerobj);
		int srcPitch             = (int)getPropValue(_pitchProp, layerobj);
		unsigned char *srcBuffer = (unsigned char *)getPropValue(_bufferProp, layerobj);

		// �e�N�X�`���p�����[�^�擾
		core::dimension2d<s32> size = texture->getSize();
		int destWidth = size.Width;
		int destHeight = size.Height;
		int destPitch  = texture->getPitch();
		unsigned char *destBuffer = (unsigned char *)texture->lock();

		int width  = srcWidth > destWidth ? destWidth : srcWidth;
		int height = srcHeight > destHeight ? destHeight : srcHeight;
		for (int i=0;i<height;i++) {
			memcpy(destBuffer, srcBuffer, width * 4);
			srcBuffer += srcPitch;
			destBuffer += destPitch;
		}
		texture->unlock();

		// �e�N�X�`���`�揈��!!
		driver->draw2DImage(texture, core::position2d<s32>(0,0),
							core::rect<s32>(0,0,800,600), 0, 
							video::SColor(255,255,255,255), true);
	}
}


//---------------------------------------------------------------------------
/*
	����� NI_Irrlicht �̃I�u�W�F�N�g���쐬���ĕԂ������̊֐��ł��B
	��q�� TJSCreateNativeClassForPlugin �̈����Ƃ��ēn���܂��B
*/
static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_Irrlicht()
{
	return new NI_Irrlicht();
}
//---------------------------------------------------------------------------
/*
	TJS2 �̃l�C�e�B�u�N���X�͈�ӂ� ID �ŋ�ʂ���Ă���K�v������܂��B
	����͌�q�� TJS_BEGIN_NATIVE_MEMBERS �}�N���Ŏ����I�Ɏ擾����܂����A
	���� ID ���i�[����ϐ����ƁA���̕ϐ��������Ő錾���܂��B
	�����l�ɂ͖����� ID ��\�� -1 ���w�肵�Ă��������B
*/
#define TJS_NATIVE_CLASSID_NAME ClassID_Irrlicht
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;
//---------------------------------------------------------------------------
/*
	TJS2 �p�́u�N���X�v���쐬���ĕԂ��֐��ł��B
*/
static iTJSDispatch2 * Create_NC_Irrlicht()
{
	// �N���X�I�u�W�F�N�g����
	tTJSNativeClassForPlugin * classobj =
		TJSCreateNativeClassForPlugin(TJS_W("Irrlicht"), Create_NI_Irrlicht);

	// �����o�o�^
	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/Irrlicht)

		TJS_DECL_EMPTY_FINALIZE_METHOD

		TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(
			/*var.name*/_this,
			/*var.type*/NI_Irrlicht,
			/*TJS class name*/Irrlicht)
		{
			// NI_Irrlicht::Construct �ɂ����e���L�q�ł���̂�
			// �����ł͉������Ȃ�
			return TJS_S_OK;
		}
		TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/Irrlicht)

	TJS_END_NATIVE_MEMBERS

	/*
		���̊֐��� classobj ��Ԃ��܂��B
	*/
	return classobj;
}

#undef TJS_NATIVE_CLASSID_NAME

//---------------------------------------------------------------------------

#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
	void* lpReserved)
{
	return 1;
}

//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" HRESULT _stdcall V2Link(iTVPFunctionExporter *exporter)
{
	// �X�^�u�̏�����
	TVPInitImportStub(exporter);

	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global) {
		addMember(global, TJS_W("Irrlicht"), Create_NC_Irrlicht());

		// Layer �N���X�I�u�W�F�N�g���擾
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("Layer"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		if (dispatch) {
			NI_Irrlicht::init(dispatch);
		}

		global->Release();
	}
			
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;

	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall V2Unlink()
{
	if (TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;

	// �v���p�e�B�J��
	NI_Irrlicht::unInit();
	
	// - �܂��ATJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global)	{
		delMember(global, TJS_W("Irrlicht"));
		global->Release();
	}

	// �X�^�u�̎g�p�I��	TVPUninitImportStub();
	return S_OK;
}
