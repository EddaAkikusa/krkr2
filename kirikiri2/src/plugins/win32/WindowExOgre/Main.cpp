#include <windows.h>
#include <list>
#include <map>
using namespace std;
#include "tp_stub.h"
#include <stdio.h>

#include "Ogre.h"
#include "OgreConfigFile.h"
using namespace Ogre;

class OgreInfo : public tTVPContinuousEventCallbackIntf
{

public:
	Ogre::Root *root;

	/**
	 * �R���X�g���N�^
	 */
	OgreInfo() {
		root = new Ogre::Root();

		// �R���t�B�O������
		ConfigFile cf;
		cf.load("resources.cfg");
		// Go through all sections & settings in the file
		ConfigFile::SectionIterator seci = cf.getSectionIterator();
		
		String secName, typeName, archName;
		while (seci.hasMoreElements()) {
			secName = seci.peekNextKey();
			ConfigFile::SettingsMultiMap *settings = seci.getNext();
			ConfigFile::SettingsMultiMap::iterator i;
			for (i = settings->begin(); i != settings->end(); ++i) {
				typeName = i->first;
					archName = i->second;
				ResourceGroupManager::getSingleton().addResourceLocation(
					archName, typeName, secName);
			}
		}
	}

	/**
	 * �f�X�g���N�^
	 */
	~OgreInfo() {
		TVPRemoveContinuousEventHook(this);
		delete root;
	}

public:

	bool config() {
		if (root->showConfigDialog()) {
			root->initialise(false);
			return true;
		}
		return false;
	}

	void start() {
		TVPRemoveContinuousEventHook(this);
		TVPAddContinuousEventHook(this);
	}
	
	/**
	 * Continuous �R�[���o�b�N
	 * �g���g�����ɂȂƂ��ɏ�ɌĂ΂��
	 * �h�蒼������
	 */
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick) {
		root->renderOneFrame();
	}
};

OgreInfo *ogreInfo = NULL;

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

#include "WindowExBase.h"

/*
 * �E�C���h�E�ɒ������� OGRE ����ێ����邽�߂̃l�C�e�B�u�C���X�^���X
 */
class NI_OgreBase : public tTJSNativeInstance
{
	
protected:
	/// �E�C���h�E�I�u�W�F�N�g
	iTJSDispatch2 * _windowobj;

	/// OGRE �֌W
	string windowName;
	RenderWindow* _renderWindow;
	SceneManager* _sceneManager;
	
private:

	/**
	 * �E�C���h�E�̉���
	 */
	void detach();

	/*
	 * �E�C���h�E�̐ݒ�
	 */
	void attach();

	/**
	 * ���b�Z�[�W�̃��V�[�o
	 */
	static bool __stdcall messageReceiver(void *userdata, tTVPWindowMessage *Message);

	
public:
	/**
	 * �R���X�g���N�^
	 */
	NI_OgreBase(iTJSDispatch2 *windowobj);

	/**
	 * �f�X�g���N�^
	 */
	~NI_OgreBase();

public:

	/**
	 * ����������
	 */
	void init() {

		/* nothing to do */

		// XXX �e�X�g�p�ɃI�u�W�F�N�g��z�u���Ă݂�
		_sceneManager->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
		Entity *ent = _sceneManager->createEntity("head", "ogrehead.mesh");
		_sceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

        // Green nimbus around Ogre
        ParticleSystem* pSys1 = _sceneManager->createParticleSystem("Nimbus", 
            "Examples/GreenyNimbus");
        _sceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(pSys1);

        // Create a rainstorm 
        ParticleSystem* pSys4 = _sceneManager->createParticleSystem("rain", 
            "Examples/Rain");
        SceneNode* rNode = _sceneManager->getRootSceneNode()->createChildSceneNode();
        rNode->translate(0,1000,0);
        rNode->attachObject(pSys4);
        // Fast-forward the rain so it looks more natural
        pSys4->fastForward(5);

        // Aureola around Ogre perpendicular to the ground
        ParticleSystem* pSys5 = _sceneManager->createParticleSystem("Aureola", 
            "Examples/Aureola");
        _sceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(pSys5);

		// Set nonvisible timeout
		ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);
	}

};

bool __stdcall
NI_OgreBase::messageReceiver(void *userdata, tTVPWindowMessage *Message)
{
	// ���̊֐��ɂ�(����������Ȃ��Ɋւ�炸)���ɑ�ʂ̃��b�Z�[�W���ʉ߂��邽��
	// �֌W�Ȃ����b�Z�[�W�ɂ��Ă̓p�t�H�[�}���X�����܂�ቺ�����Ȃ��悤�Ȏ������]�܂��
	
	switch(Message->Msg) {
	case TVP_WM_DETACH:
		// �E�B���h�E�n���h������蒼����邩�A���邢��
		// �E�B���h�E���j������钼�O�ɂ��̃��b�Z�[�W������B
		// �g���g���� Window �N���X�̃E�B���h�E�̎q�E�B���h�E�Ƃ���
		// �E�B���h�E��\��t�����ꍇ�͂��̃^�C�~���O�ŃE�B���h�E��
		// ���O���Ȃ���΂Ȃ�Ȃ��B
		// WPARAM �� 1 �Ȃ�΃E�B���h�E���j�������Ƃ��A0 �Ȃ��
		// �E�B���h�E����蒼�����Ƃ��ł���B
		// TVP_WM_DETACH �� TVP_WM_ATTACH �̊Ԃɂ�
		// ���b�Z�[�W���������邱�Ƃ����邪�������ׂ��B
		if(Message->WParam == 1)
			TVPAddLog(TJS_W("TVP_WM_DETACH: closing"));
		else if(Message->WParam == 0)
			TVPAddLog(TJS_W("TVP_WM_DETACH: rebuilding"));
		((NI_OgreBase*)userdata)->detach();
		break;
		
	case TVP_WM_ATTACH:
		// �E�B���h�E����蒼���ꂽ��ɂ��̃��b�Z�[�W������B
		// LPARAM �ɂ� �V�����E�B���h�E�n���h���������Ă���B
		// �q�E�B���h�E���܂����t����ꍇ�͂��̃^�C�~���O�ł܂����t����
		// ���Ƃ��ł���B
		TVPAddLog(TJS_W("TVP_WM_ATTACH: new window handle ") +
			TJSInt32ToHex(Message->LParam, 8));
		((NI_OgreBase*)userdata)->attach();
		break;

	default:
		break;
	}
	return false;
}

/**
 * �E�C���h�E�̉���
 */
void
NI_OgreBase::detach()
{
	if (_renderWindow) {
		_sceneManager->destroyAllCameras();
		_renderWindow->removeAllViewports();
		_renderWindow->removeAllListeners();
		ogreInfo->root->getRenderSystem()->destroyRenderWindow(windowName);
		_renderWindow = NULL;
	}
}

/**
 * �E�C���h�E�̍Đݒ�
 */
void
NI_OgreBase::attach()
{
	NI_WindowExBase *base;
	if ((base = NI_WindowExBase::getNative(_windowobj))) {
		
		// �p�����[�^�擾
		base->reset(_windowobj);
		
		// �E�C���h�E����
		{
			// �n���h���𕶎���
			char hwndName[100];
			snprintf(hwndName, sizeof hwndName, "%d", base->_hwnd);
			
			// �������p�����[�^
			NameValuePairList params;
			params["parentWindowHandle"] = hwndName;
			params["left"] = "0";
			params["top"] = "0";

			windowName = "window";
			windowName += hwndName;
			
			// �E�C���h�E����
			_renderWindow = ogreInfo->root->createRenderWindow(windowName,
															   base->_innerWidth,
															   base->_innerHeight,
															   false,
															   &params);
			ogreInfo->start();
		}

		// XXX ���������擾�p�����[�^�擾
		base->reset(_windowobj);
	}

	Camera *camera = _sceneManager->createCamera("Player");
	camera->setPosition(Vector3(0,0,500));
	camera->lookAt(Vector3(0,0,-300));
	camera->setNearClipDistance(5);
	
	Viewport* vp = _renderWindow->addViewport(camera);
	vp->setBackgroundColour(ColourValue(0,0,0));

	// ���\�[�X������
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

/**
 * �R���X�g���N�^
 */
NI_OgreBase::NI_OgreBase(iTJSDispatch2 *windowobj)
{
	_windowobj = windowobj; // no addRef
	_renderWindow = NULL;
	
	// registerMessageReceiver ���Ă�(���V�[�o�o�^)
	tTJSVariant mode, proc, userdata;
	tTJSVariant *p[3] = {&mode, &proc, &userdata};
	mode = (tTVInteger)(tjs_int)wrmRegister;
	proc = (tTVInteger)reinterpret_cast<tjs_int>(messageReceiver);
	userdata = (tTVInteger)(tjs_int)this;
	_windowobj->FuncCall(0, TJS_W("registerMessageReceiver"), NULL, NULL, 3, p, _windowobj);
	
	// �V�[���}�l�[�W��
	_sceneManager = ogreInfo->root->createSceneManager(ST_GENERIC, "ExampleSMInstance");

	// �E�C���h�E��������
	attach();
}

/**
 * �f�X�g���N�^
 */
NI_OgreBase::~NI_OgreBase()
{
	// registerMessageReceiver ���Ă�(�o�^����)
	tTJSVariant mode, proc, userdata;
	tTJSVariant *p[3] = {&mode, &proc, &userdata};
	mode = (tTVInteger)(tjs_int)wrmUnregister;
	proc = (tTVInteger)reinterpret_cast<tjs_int>(messageReceiver);
	userdata = (tTVInteger)(tjs_int)0;
	_windowobj->FuncCall(0, TJS_W("registerMessageReceiver"), NULL, NULL, 3, p, _windowobj);
	detach();
}

// �N���XID
static tjs_int32 ClassID_OgreBase = -1;

void
addMember(iTJSDispatch2 *dispatch, const tjs_char *memberName, iTJSDispatch2 *member)
{
	tTJSVariant var = tTJSVariant(member);
	member->Release();
	dispatch->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		memberName, // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
		NULL, // �q���g ( �{���̓����o���̃n�b�V���l�����ANULL �ł��悢 )
		&var, // �o�^����l
		dispatch // �R���e�L�X�g
		);
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

/**
 * �E�C���h�E�I�u�W�F�N�g���� OGRE �p�l�C�e�B�u�C���X�^���X���擾����B
 * �l�C�e�B�u�C���X�^���X�������ĂȂ��ꍇ�͎����I�Ɋ��蓖�Ă�
 * @param windowobj �E�C���h�E�I�u�W�F�N�g
 * @return OGRE �p�l�C�e�B�u�C���X�^���X�B�擾���s������ NULL
 */
static NI_OgreBase *
getOgreBaseNative(iTJSDispatch2 *windowobj)
{
	if (!windowobj) return NULL;
	// ���擾
	NI_OgreBase *_this;
	if (TJS_FAILED(windowobj->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
													ClassID_OgreBase, (iTJSNativeInstance**)&_this))) {
		// �E�C���h�E�g��������
		if (NI_WindowExBase::getNative(windowobj) == NULL) {
			return NULL;
		}
		// OGRE �g�����o�^
		_this = new NI_OgreBase(windowobj);
		if (TJS_FAILED(windowobj->NativeInstanceSupport(TJS_NIS_REGISTER,
														ClassID_OgreBase, (iTJSNativeInstance **)&_this))) {
			delete _this;
			return NULL;
		}
	}
	return _this;
}


#define FUNC(funcname,pnum) \
class funcname : public tTJSDispatch\
{\
protected:\
public:\
	tjs_error TJS_INTF_METHOD FuncCall(\
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,\
		tTJSVariant *result,\
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {\
		NI_OgreBase *_this;\
		if ((_this = getOgreBaseNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;\
		if (numparams < pnum) return TJS_E_BADPARAMCOUNT;

#define FUNCEND \
		return TJS_S_OK;\
	}\
};

#define PROP(funcname) \
class funcname : public tTJSDispatch\
{\
protected:\
public:

#define GETTER \
	tjs_error TJS_INTF_METHOD PropGet(\
		tjs_uint32 flag,\
		const tjs_char * membername,\
		tjs_uint32 *hint,\
		tTJSVariant *result,\
		iTJSDispatch2 *objthis)	{\
		NI_OgreBase *_this;\
		if ((_this = getOgreBaseNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;\
		

#define SETTER \
        return TJS_S_OK;\
    }\
	tjs_error TJS_INTF_METHOD PropSet(\
		tjs_uint32 flag,\
		const tjs_char *membername,\
		tjs_uint32 *hint,\
		const tTJSVariant *param,\
		iTJSDispatch2 *objthis)	{\
		NI_OgreBase *_this;\
		if ((_this = getOgreBaseNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;
		
#define PROPEND \
		return TJS_S_OK;\
	}\
};

// �v���p�e�B�⃁�\�b�h�̓o�^�͈ȉ��ōs��
// _this ���l�C�e�B�u�I�u�W�F�N�g�ɂȂ��Ă���
FUNC(tInitFunction,0)
	_this->init();
FUNCEND

#if 0
// �v���p�e�B�p
PROP(tXXXProp)
GETTER
	*result = _this->getXXX();
SETTER
	_this->setXXX(*param);
FUNCEND
#endif

//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}

//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;

/**
 * �v���O�C������������
 */
extern "C" HRESULT _stdcall V2Link(iTVPFunctionExporter *exporter)
{
	// �X�^�u�̏�����(�K���L�q����)
	TVPInitImportStub(exporter);

	// OGRE �̊�{���
	ogreInfo = new OgreInfo();
	if (!ogreInfo->config()) {
		delete ogreInfo;
		ogreInfo = NULL;
		TVPThrowExceptionMessage(TJS_W("can't init OGRE."));
	}
	
	// �N���X�I�u�W�F�N�g�`�F�b�N
	if ((NI_WindowExBase::classId = TJSFindNativeClassID(L"WindowExBase")) <= 0) {
		NI_WindowExBase::classId = TJSRegisterNativeClass(L"WindowExBase");
	}
	
	// �N���X�I�u�W�F�N�g�o�^
	ClassID_OgreBase = TJSRegisterNativeClass(TJS_W("OgreBase"));

	{
		// Window �N���X�I�u�W�F�N�g���擾
		tTJSVariant winScripts;
		TVPExecuteExpression(TJS_W("Window"), &winScripts);
		iTJSDispatch2 *window = winScripts.AsObjectNoAddRef();
		
		// Layer �N���X�I�u�W�F�N�g���擾
		tTJSVariant layerScripts;
		TVPExecuteExpression(TJS_W("Layer"), &layerScripts);
		iTJSDispatch2 *layer = layerScripts.AsObjectNoAddRef();
		
		if (window && layer) {
			// �v���p�e�B������
			NI_WindowExBase::init(window, layer);
			// �����o�o�^
			addMember(window, L"initOGRE", new tInitFunction());
		}
	}
	
	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 *global = TVPGetScriptDispatch();
	if (global) {
		global->Release();
	}
			
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	return S_OK;
}

/**
 * �v���O�C���������
 */
extern "C" HRESULT _stdcall V2Unlink()
{
	if (ogreInfo) {

		if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;

		{
			// Window �N���X�I�u�W�F�N�g���擾
			tTJSVariant winScripts;
			TVPExecuteExpression(TJS_W("Window"), &winScripts);
			iTJSDispatch2 *window = winScripts.AsObjectNoAddRef();
			// �����o�폜
			delMember(window, L"initOGRE");
		}
		
		// Window�g����{�v���p�e�B�J��
		NI_WindowExBase::unInit();
		
		iTJSDispatch2 * global = TVPGetScriptDispatch();
		if (global) {
			global->Release();
		}

		delete ogreInfo;
	}
	
	TVPUninitImportStub();

	return S_OK;
}
