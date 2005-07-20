#include <windows.h>
#include <DispEx.h>
#include "tp_stub.h"
#include <stdio.h>

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

#if 0
/**
 * OLE -> �g���g�� �C�x���g�f�B�X�p�b�`��
 * sender (IUnknown) ���� DIID �̃C�x���g���󗝂��A
 * receiver (tTJSDispatch2) �ɑ��M����B
 */ 
class SuperSink : public IDispatch
{
protected:
	REFIID diid;
	//CComPtr<IUnkown> sender;
	DWORD cookie;
	
	VNBSink(IUnkown *sender, GUID diid) {

		// advise ����
	}

	//----------------------------------------------------------------------------
	// IUnknown ����
	
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
											 void __RPC_FAR *__RPC_FAR *ppvObject) {
		if (riid == IID_IUnknown ||
			riid == IID_IDispatch||
			riid == diid) {
			if (ppvObject == NULL)
				return E_POINTER;
			*ppvObject = this;
			AddRef();
			return S_OK;
		} else {
			*ppvObject = 0;
			return E_NOINTERFACE;
		}
	}

	ULONG STDMETHODCALLTYPE AddRef() {
		return 1;
	}

	ULONG STDMETHODCALLTYPE Release() {
		return 1;
	}
	
	// -------------------------------------
	// IDispatch �̎���
public:
	STDMETHOD (GetTypeInfoCount) (UINT* pctinfo)
	{
		return	E_NOTIMPL;
	}

	STDMETHOD (GetTypeInfo) (UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
	{
		return	E_NOTIMPL;
	}

	STDMETHOD (GetIDsOfNames) (REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid)
	{
		return	E_NOTIMPL;
	}

	STDMETHOD (Invoke) (DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr)
	{
		// XXX sender �ւ̃C�x���g���M����
		return S_OK;
	}
};
#endif

#include "IDispatchWrapper.hpp"

//---------------------------------------------------------------------------

/*
 * WIN32OLE �l�C�e�B�u�C���X�^���X
 */
class NI_WIN32OLE : public tTJSNativeInstance // �l�C�e�B�u�C���X�^���X
{
public:
	IDispatch *pDispatch;

	NI_WIN32OLE() {
		// �R���X�g���N�^
		pDispatch = NULL;
	}

	tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj)
	{
		// TJS2 �I�u�W�F�N�g���쐬�����Ƃ��ɌĂ΂��
		if (numparams >= 1) {
			if (param[0]->Type() == tvtString) {
				const tjs_char *name = param[0]->GetString();
				HRESULT hr;
				CLSID   clsid;
				OLECHAR *oleName = SysAllocString(name);
				if (FAILED(hr = CLSIDFromProgID(oleName, &clsid))) {
					hr = CLSIDFromString(oleName, &clsid);
				}
				SysFreeString(oleName);
				if (SUCCEEDED(hr)) {
					// COM �ڑ�����IDispatch ���擾����
					/* get IDispatch interface */
					hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IDispatch, (void**)&pDispatch);
					if (SUCCEEDED(hr)) {
						// missing ���\�b�h�̓o�^
						tTJSVariant name(TJS_W("missing"));
						tjs_obj->ClassInstanceInfo(TJS_CII_SET_MISSING, 0, &name);
						return TJS_S_OK;
					} else {
						log(TJS_W("CoCreateInstance failed %s"), name);
					}
				} else {
					log(TJS_W("bad CLSID %s"), name);
				}
			} 
			return TJS_E_INVALIDPARAM;
		} else {
			return TJS_E_BADPARAMCOUNT;
		}
	}

	void TJS_INTF_METHOD Invalidate()
	{
		// �I�u�W�F�N�g�������������Ƃ��ɌĂ΂��
		if (pDispatch) {
			pDispatch->Release();
		}
	}

	/**
	 * ���\�b�h���s
	 */
	tjs_error invoke(DWORD wFlags,
					 const tjs_char *membername,
					 tTJSVariant *result,
					 tjs_int numparams,
					 tTJSVariant **param) {
		if (pDispatch) {
			return iTJSDispatch2Wrapper::Invoke(pDispatch,
												wFlags,
												membername,
												result,
												numparams,
												param);
		}
		return TJS_E_FAIL;
	}
	
	/**
	 * ���\�b�h���s
	 */
	tjs_error invoke(DWORD wFlags,
					 tTJSVariant *result,
					 tjs_int numparams,
					 tTJSVariant **param) {
		//log(TJS_W("native invoke %d"), numparams);
		if (pDispatch) {
			// �p�����[�^�̂P�ڂ����\�b�h��
			if (numparams > 0) {
				if (param[0]->Type() == tvtString) {
					return iTJSDispatch2Wrapper::Invoke(pDispatch,
														wFlags,
														param[0]->GetString(),
														result,
														numparams - 1,
														param ? param + 1 : NULL);
				} else {
					return TJS_E_INVALIDPARAM;
				}
			} else {
				return TJS_E_BADPARAMCOUNT;
			}
		}
		return TJS_E_FAIL;
	}

	/**
	 * �萔�̎擾
	 */
protected:

	/**
	 * �萔�̎擾
	 * @param pTypeInfo TYPEINFO
	 * @param target �i�[��
	 */
	void getConstant(ITypeInfo *pTypeInfo, iTJSDispatch2 *target) {
		// �����
		TYPEATTR  *pTypeAttr = NULL;
		if (SUCCEEDED(pTypeInfo->GetTypeAttr(&pTypeAttr))) {
			for (int i=0; i<pTypeAttr->cVars; i++) {
				VARDESC *pVarDesc = NULL;
				if (SUCCEEDED(pTypeInfo->GetVarDesc(i, &pVarDesc))) {
					if (pVarDesc->varkind == VAR_CONST &&
						!(pVarDesc->wVarFlags & (VARFLAG_FHIDDEN | VARFLAG_FRESTRICTED | VARFLAG_FNONBROWSABLE))) {
						BSTR bstr = NULL;
						unsigned int len;
						if (SUCCEEDED(pTypeInfo->GetNames(pVarDesc->memid, &bstr, 1, &len)) && len >= 0 && bstr) {
							log(TJS_W("const:%s"), bstr);
							tTJSVariant result;
							IDispatchWrapper::storeVariant(result, *(pVarDesc->lpvarValue));
							target->PropSet(TJS_MEMBERENSURE|TJS_IGNOREPROP,
											bstr,
											NULL,
											&result,
											target
											);
							SysFreeString(bstr);
						}
					}
					pTypeInfo->ReleaseVarDesc(pVarDesc);
				}
			}
			pTypeInfo->ReleaseTypeAttr(pTypeAttr);
		}
	}

	/**
	 * �萔�̎擾
	 * @param pTypeLib TYPELIB
	 * @param target �i�[��
	 */
	void getConstant(ITypeLib *pTypeLib, iTJSDispatch2 *target) {
		unsigned int count = pTypeLib->GetTypeInfoCount();
		for (unsigned int i=0; i<count; i++) {
			ITypeInfo *pTypeInfo = NULL;
			if (SUCCEEDED(pTypeLib->GetTypeInfo(i, &pTypeInfo))) {
				getConstant(pTypeInfo, target);
				pTypeInfo->Release();
			}
		}
	}

public:
	/**
	 * �萔�̎擾
	 * @param target �i�[��
	 */
	void getConstant(iTJSDispatch2 *target) {
		if (target) {
			ITypeInfo *pTypeInfo = NULL;
			if (SUCCEEDED(pDispatch->GetTypeInfo(0, LOCALE_SYSTEM_DEFAULT, &pTypeInfo))) {
				unsigned int index = 0;
				ITypeLib *pTypeLib = NULL;
				if (SUCCEEDED(pTypeInfo->GetContainingTypeLib(&pTypeLib, &index))) {
					getConstant(pTypeLib, target);
					pTypeLib->Release();
				}
				pTypeInfo->Release();
			}
		}
	}

};
//---------------------------------------------------------------------------
/*
	����� NI_WIN32OLE �̃I�u�W�F�N�g���쐬���ĕԂ������̊֐��ł��B
	��q�� TJSCreateNativeClassForPlugin �̈����Ƃ��ēn���܂��B
*/
static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_WIN32OLE()
{
	return new NI_WIN32OLE();
}
//---------------------------------------------------------------------------
/*
	TJS2 �̃l�C�e�B�u�N���X�͈�ӂ� ID �ŋ�ʂ���Ă���K�v������܂��B
	����͌�q�� TJS_BEGIN_NATIVE_MEMBERS �}�N���Ŏ����I�Ɏ擾����܂����A
	���� ID ���i�[����ϐ����ƁA���̕ϐ��������Ő錾���܂��B
	�����l�ɂ͖����� ID ��\�� -1 ���w�肵�Ă��������B
*/
#define TJS_NATIVE_CLASSID_NAME ClassID_WIN32OLE
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;
//---------------------------------------------------------------------------
/*
	TJS2 �p�́u�N���X�v���쐬���ĕԂ��֐��ł��B
*/
static iTJSDispatch2 * Create_NC_WIN32OLE()
{
	// �N���X�I�u�W�F�N�g
	tTJSNativeClassForPlugin * classobj = TJSCreateNativeClassForPlugin(TJS_W("WIN32OLE"), Create_NI_WIN32OLE);

	// �l�C�e�B�u�����o�[
	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/WIN32OLE)

		TJS_DECL_EMPTY_FINALIZE_METHOD

		TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(
			/*var.name*/_this,
			/*var.type*/NI_WIN32OLE,
			/*TJS class name*/WIN32OLE)
		{
			// NI_WIN32OLE::Construct �ɂ����e���L�q�ł���̂�
			// �����ł͉������Ȃ�
			return TJS_S_OK;
		}
		TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/WIN32OLE)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/invoke) // invoke ���\�b�h
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			return _this->invoke(DISPATCH_PROPERTYGET|DISPATCH_METHOD, result, numparams, param);
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/invoke)
			
		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/set) // set ���\�b�h
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			return _this->invoke(DISPATCH_PROPERTYPUT, result, numparams, param);
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/set)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/get) // get ���\�b�h
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			return _this->invoke(DISPATCH_PROPERTYGET, result, numparams, param);
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/get)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/missing) // missing ���\�b�h
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			if (numparams < 3) {return TJS_E_BADPARAMCOUNT;};
			bool ret = false;
			const tjs_char *membername = param[1]->GetString();
			if ((int)*param[0]) {
				// put
				ret = TJS_SUCCEEDED(_this->invoke(DISPATCH_PROPERTYPUT, membername, NULL, 1, &param[2]));
			} else {
				// get
				tTJSVariant result;
				if ((ret = TJS_SUCCEEDED(_this->invoke(DISPATCH_PROPERTYGET|DISPATCH_METHOD, membername, &result, 0, NULL)))) {
					iTJSDispatch2 *value = param[2]->AsObject();
					if (value) {
						value->PropSet(0, NULL, NULL, &result, NULL);
						value->Release();
					}
				}
			}
			if (result) {
				*result = ret;
			}
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/missing)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getConstant) // getConstant ���\�b�h
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_WIN32OLE);
			if (numparams > 0) {
				iTJSDispatch2 *store = param[0]->AsObject();
				if (store) {
					_this->getConstant(store);
					store->Release();
				}
			} else {
				_this->getConstant(objthis);
			}
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/getConstant)
			
	TJS_END_NATIVE_MEMBERS

	/*
		���̊֐��� classobj ��Ԃ��܂��B
	*/
	return classobj;
}
//---------------------------------------------------------------------------
/*
	TJS_NATIVE_CLASSID_NAME �͈ꉞ undef ���Ă������ق����悢�ł��傤
*/
#undef TJS_NATIVE_CLASSID_NAME
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
	void* lpReserved)
{
	return 1;
}

extern void registArchive();
extern void unregistArchive();

//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
static BOOL gOLEInitialized = false;

extern "C" HRESULT _stdcall _export V2Link(iTVPFunctionExporter *exporter)
{
	// �X�^�u�̏�����(�K���L�q����)
	TVPInitImportStub(exporter);

	tTJSVariant val;

	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	{
		//-----------------------------------------------------------------------
		// 1 �܂��N���X�I�u�W�F�N�g���쐬
		iTJSDispatch2 * tjsclass = Create_NC_WIN32OLE();
		
		// 2 tjsclass �� tTJSVariant �^�ɕϊ�
		val = tTJSVariant(tjsclass);
		
		// 3 ���ł� val �� tjsclass ��ێ����Ă���̂ŁAtjsclass ��
		//   Release ����
		tjsclass->Release();
		
		
		// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
		global->PropSet(
			TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
			TJS_W("WIN32OLE"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
			NULL, // �q���g ( �{���̓����o���̃n�b�V���l�����ANULL �ł��悢 )
			&val, // �o�^����l
			global // �R���e�L�X�g ( global �ł悢 )
			);
		//-----------------------------------------------------------------------
	}

	// - global �� Release ����
	global->Release();

	// �����A�o�^����֐�����������ꍇ�� 1 �` 4 ���J��Ԃ�

	// val ���N���A����B
	// ����͕K���s���B�������Ȃ��� val ���ێ����Ă���I�u�W�F�N�g
	// �� Release ���ꂸ�A���Ɏg�� TVPPluginGlobalRefCount �����m�ɂȂ�Ȃ��B
	val.Clear();

	// ���̎��_�ł� TVPPluginGlobalRefCount �̒l��
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	// �Ƃ��čT���Ă����BTVPPluginGlobalRefCount �͂��̃v���O�C������
	// �Ǘ�����Ă��� tTJSDispatch �h���I�u�W�F�N�g�̎Q�ƃJ�E���^�̑��v�ŁA
	// ������ɂ͂���Ɠ������A����������Ȃ��Ȃ��ĂȂ��ƂȂ�Ȃ��B
	// �����Ȃ��ĂȂ���΁A�ǂ����ʂ̂Ƃ���Ŋ֐��Ȃǂ��Q�Ƃ���Ă��āA
	// �v���O�C���͉���ł��Ȃ��ƌ������ƂɂȂ�B

	if (!gOLEInitialized) {
		if (SUCCEEDED(OleInitialize(NULL))) {
			gOLEInitialized = true;
		} else {
			log(TJS_W("OLE ���������s"));
		}
	}
	
	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall _export V2Unlink()
{
	// �g���g��������A�v���O�C����������悤�Ƃ���Ƃ��ɌĂ΂��֐��B

	// �������炩�̏����Ńv���O�C��������ł��Ȃ��ꍇ��
	// ���̎��_�� E_FAIL ��Ԃ��悤�ɂ���B
	// �����ł́ATVPPluginGlobalRefCount �� GlobalRefCountAtInit ����
	// �傫���Ȃ��Ă���Ύ��s�Ƃ������Ƃɂ���B
	if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;
		// E_FAIL ���A��ƁAPlugins.unlink ���\�b�h�͋U��Ԃ�

	/*
		�������A�N���X�̏ꍇ�A�����Ɂu�I�u�W�F�N�g���g�p���ł���v�Ƃ������Ƃ�
		�m�邷�ׂ�����܂���B��{�I�ɂ́APlugins.unlink �ɂ��v���O�C���̉����
		�댯�ł���ƍl���Ă������� (�������� Plugins.link �Ń����N������A�Ō��
		�Ńv���O�C������������A�v���O�����I���Ɠ����Ɏ����I�ɉ��������̂��g)�B
	*/

	// TJS �̃O���[�o���I�u�W�F�N�g�ɓo�^���� WIN32OLE �N���X�Ȃǂ��폜����

	// - �܂��ATJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	// - global �� DeleteMember ���\�b�h��p���A�I�u�W�F�N�g���폜����
	if(global)
	{
		// TJS ���̂����ɉ������Ă����Ƃ��Ȃǂ�
		// global �� NULL �ɂȂ蓾��̂� global �� NULL �łȂ�
		// ���Ƃ��`�F�b�N����

		global->DeleteMember(
			0, // �t���O ( 0 �ł悢 )
			TJS_W("WIN32OLE"), // �����o��
			NULL, // �q���g
			global // �R���e�L�X�g
			);
	}
	
	// - global �� Release ����
	if(global) global->Release();

	if (gOLEInitialized) {
		OleUninitialize();
		gOLEInitialized = false;
	}
	
	// �X�^�u�̎g�p�I��(�K���L�q����)
	TVPUninitImportStub();

	return S_OK;
}
//---------------------------------------------------------------------------

