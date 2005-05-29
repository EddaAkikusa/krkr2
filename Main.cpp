#include <windows.h>
#include <DispEx.h>
#include "tp_stub.h"

//---------------------------------------------------------------------------

// ATL
#if _MSC_VER == 1200
// Microsoft SDK �̂��̂Ƃ��������̂Ŕr��
#define __IHTMLControlElement_INTERFACE_DEFINED__
#endif

#include <atlbase.h>
static CComModule _Module;
#include <atlwin.h>
#include <atlcom.h>
#include <atliface.h>
#define _ATL_DLL
#include <atlhost.h>
#include <ExDispID.h>

/**
 * �V���N�i�C�x���g�f�B�X�p�b�`���j�N���X
 * �����N��SHIFT�{�N���b�N�ł̕ʃE�B���h�E�I�[�v�����������ׂɁA
 * DISPID_NEWWINDOW���t�b�N����B
 */ 

class ATL_NO_VTABLE VNBSink :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatch
{
public:
	CComPtr<IWebBrowser2> m_pBrowser;
	CWindow *showWhenComplete;
	bool disableControl;
	
	VNBSink() {
		m_pBrowser = NULL;
		showWhenComplete = NULL;
		disableControl = true;
	}

	BEGIN_COM_MAP(VNBSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(DIID_DWebBrowserEvents2, IDispatch)
	END_COM_MAP()

// IDispatch
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
		HRESULT hr = S_OK;
		if ( pdispparams ) {
			switch ( dispidMember ) {
			case DISPID_DOCUMENTCOMPLETE:
				if (m_pBrowser) {
					CComPtr<IDispatch> pDisp;
					m_pBrowser->get_Document(&pDisp);
					if (pDisp) {
						CComQIPtr<IHTMLDocument2> pDoc = pDisp;
						if (pDoc) {
							USES_CONVERSION;
							CComBSTR str;
							pDoc->get_title(&str);
							showWhenComplete->SetWindowText(OLE2T(str));
						}
					}
				}
				if (showWhenComplete) {
					showWhenComplete->ShowWindow(TRUE);
				}
				break;
			case DISPID_NEWWINDOW:
			case DISPID_NEWWINDOW2:
			case DISPID_FRAMENEWWINDOW:
				// ���݂̃E�B���h�E�ł��̂܂܃I�[�v������
				//if ( m_pBrowser ) {
				//	VARIANT defArg;
				//	defArg.vt = VT_ERROR;
				//	defArg.scode = DISP_E_PARAMNOTFOUND;
				//	m_pBrowser->Navigate(pdispparams->rgvarg[5].bstrVal, &defArg, &defArg, pdispparams->rgvarg[2].pvarVal, &defArg);
				//}
				if (disableControl) {
					*(pdispparams->rgvarg[0].pboolVal) = VARIANT_TRUE;
				}
				break;
			default:
				break;
			}
		} else {
			hr = DISP_E_PARAMNOTFOUND;
		}
		return hr;
	}
};


/**
 * DHTML�̊O���ďo��(window.external)�Ɗe��UI�������󗝂��邽�߂̃N���X
 * CAxWindow::SetExternalUIHandler �Őݒ肷�邱�Ƃ�z�肵�Ă���B
 * getExternal ���\�b�h�ŁA�O���Ăяo���p�� IDispatch ��Ԃ��Ă���B
 *
 * ���ݎ�������Ă��郁�\�b�h
 * window.external.SetBackColor �w�i�F��ݒ肷�郁�\�b�h
 *
 * ���ꂼ��N���X���̑Ή����郁�\�b�h���Ăяo���Ă���B�p�����Ē��g���L�q���邱�Ƃ�
 * �������ύX�ł���B�p�����[�^��COM�X�^�C���̂���(VARIANT)�ɂȂ�̂Œ���
 */ 
class CHtmlExt : public IDocHostUIHandlerDispatch {

protected:
	IDispatchEx *dispatchEx;

public:
	CHtmlExt() {
		dispatchEx = NULL;
	}

	~CHtmlExt() {
		if (dispatchEx) {
			dispatchEx->Release();
		}
	}
	
	//----------------------------------------------------------------------------
	// IUnknown ����
	
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
											 void __RPC_FAR *__RPC_FAR *ppvObject) {
		if (dispatchEx && (riid == IID_IUnknown || riid == IID_IDispatch || riid == IID_IDispatchEx)) {
			//TVPAddLog(TJS_W("get dispatchEx"));
			if (ppvObject == NULL)
				return E_POINTER;
			dispatchEx->AddRef();
			*ppvObject = dispatchEx;
			return S_OK;
		} else if (riid == IID_IUnknown || riid == IID_IDispatch) {
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

	//----------------------------------------------------------------------------
	// IDispatch ����

	STDMETHOD(GetTypeInfoCount)(UINT* pctinfo) {
		return E_NOTIMPL;
	}

	STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo** pptinfo) {
		return E_NOTIMPL;
	}

	/**
	 * ���\�b�h����ID�̑Ή����Ƃ郁�\�b�h
	 * regszNames �Ƀ��\�b�h���̔z�񂪂���̂ŁA
	 * rgdispid �ɑΉ����� dispid ��Ԃ��Ă��
	 */
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
							 LCID lcid, DISPID* rgdispid) {
		return E_NOTIMPL;
	}

	/**
	 * ���\�b�h���s
	 * dispidMember �Ń��\�b�h���w�肳���B������ pdispparams �� VARIANT �̔z��
	 * �̌`�ł킽�����̂ł�����g��
	 */
	STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid,
					  LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
					  EXCEPINFO* pexcepinfo, UINT* puArgErr) {
		return E_NOTIMPL;
	}

	//----------------------------------------------------------------------------
	// IDocHostUIHandlerDispatch �̎���

	/**
	 * �R���e�L�X�g���j���[����
	 * �������Ȃ����ƂŃ��j���[�������Ă���
	 */
	HRESULT STDMETHODCALLTYPE ShowContextMenu( 
		/* [in] */ DWORD dwID,
		/* [in] */ DWORD x,
		/* [in] */ DWORD y,
		/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
		/* [in] */ IDispatch __RPC_FAR *pdispReserved,
		/* [retval][out] */ HRESULT __RPC_FAR *dwRetVal) {
		*dwRetVal = S_OK;      //This is what the WebBrowser control is looking for.
		//You can show your own context menu here.
		return S_OK;        
	}

	HRESULT STDMETHODCALLTYPE GetHostInfo( 
		/* [out][in] */ DWORD __RPC_FAR *pdwFlags,
		/* [out][in] */ DWORD __RPC_FAR *pdwDoubleClick) {
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE ShowUI( 
		/* [in] */ DWORD dwID,
		/* [in] */ IUnknown __RPC_FAR *pActiveObject,
		/* [in] */ IUnknown __RPC_FAR *pCommandTarget,
		/* [in] */ IUnknown __RPC_FAR *pFrame,
		/* [in] */ IUnknown __RPC_FAR *pDoc,
		/* [retval][out] */ HRESULT __RPC_FAR *dwRetVal) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE HideUI( void) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE UpdateUI( void) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE EnableModeless(
		/* [in] */ VARIANT_BOOL fEnable) {
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE OnDocWindowActivate( 
		/* [in] */ VARIANT_BOOL fActivate) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(
		/* [in] */ VARIANT_BOOL fActivate) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE ResizeBorder( 
		/* [in] */ long left,
		/* [in] */ long top,
		/* [in] */ long right,
		/* [in] */ long bottom,
		/* [in] */ IUnknown __RPC_FAR *pUIWindow,
		/* [in] */ VARIANT_BOOL fFrameWindow) {
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
		/* [in] */ DWORD hWnd,
		/* [in] */ DWORD nMessage,
		/* [in] */ DWORD wParam,
		/* [in] */ DWORD lParam,
		/* [in] */ BSTR bstrGuidCmdGroup,
		/* [in] */ DWORD nCmdID,
		/* [retval][out] */ HRESULT __RPC_FAR *dwRetVal) {
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE GetOptionKeyPath( 
		/* [out] */ BSTR __RPC_FAR *pbstrKey,
		/* [in] */ DWORD dw) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE GetDropTarget( 
		/* [in] */ IUnknown __RPC_FAR *pDropTarget,
		/* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppDropTarget) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE GetExternal( 
		/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) {
		*ppDispatch = this;
		return S_OK;
	}
        
	HRESULT STDMETHODCALLTYPE TranslateUrl( 
		/* [in] */ DWORD dwTranslate,
		/* [in] */ BSTR bstrURLIn,
		/* [out] */ BSTR __RPC_FAR *pbstrURLOut) {
		return E_NOTIMPL;
	}
        
	HRESULT STDMETHODCALLTYPE FilterDataObject( 
		/* [in] */ IUnknown __RPC_FAR *pDO,
		/* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppDORet) {
		return E_NOTIMPL;
	}
};

class CBrowserWindow : public CWindowImpl<CBrowserWindow, CAxWindow>, public CHtmlExt
{
protected:
	bool disableControl; // ���䐧���p�t���O

public:

	LRESULT OnKeydown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){

		// browser is a data member of type IWebBrowser2.
		// Using CComQIPtr in this way queries browser
		// for the IOleInPlaceActiveObject interface which is
		// then stored in the pIOIPAO variable.
		//
		CComQIPtr<IOleInPlaceActiveObject,
		&IID_IOleInPlaceActiveObject> pIOIPAO(browser);
		
		HRESULT hr = S_FALSE;

		// �ꕔ�̋@�\���E���ׂɃA�N�Z�����[�^���Ă΂Ȃ��悤�ɂ���
		switch ( wParam ) {
		case 'A': // �S�I��
		case 'C': // �R�s�[
		case 'X': // �J�b�g
		case 'V': // �y�[�X�g
		case 'Z': // UNDO
		case VK_TAB:    // �^�u(���̍���)
		case VK_INSERT: // �e�킱�҂؏���?
		case VK_DELETE: // �I���폜
			if (pIOIPAO) {
				MSG msg;
				msg.message = uMsg;
				msg.wParam = wParam;
				msg.lParam = lParam;
				hr = pIOIPAO->TranslateAccelerator(&msg);
			}
			break;
		//case 'N':		// �V�K�E�B���h�E
		//case 'P':		// ���
		//case 'O':		// �I�[�v��
		//case 'F':		// ����
		default:
			break;
		}
		return hr;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
		bHandled = true;
		setVisible(false);
		return S_OK;
	}
	
private:
	// �u���E�U�̃R���g���[��
	DWORD dwCookie;
	CComPtr<IUnknown> pSink;
public:
	CComPtr<IWebBrowser2> browser;

public:

	/**
	 * ����������
	 */
	CBrowserWindow(HWND hwnd, bool disableControl = true) : browser(0) {

		HRESULT hr;
		this->disableControl = disableControl;
		
		// �e�T�C�Y���擾
		RECT rect;
		if (hwnd) {
			::GetClientRect(hwnd, &rect);
		}
		
		// HTML �R���g���[���쐬
		if (Create(hwnd, hwnd ? rect : rcDefault, NULL, hwnd ? WS_CHILD|WS_CLIPCHILDREN : WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN)) {
			hr = CreateControl(OLESTR("Shell.Explorer.2"));
			if (SUCCEEDED(hr)) {
				// �������g�� external �����p�ɓo�^����B
				// SetExternalDispatch ���{���̂��̖ړI�̃��\�b�h����
				// IDocHostUIHandlerDispatch ���g���ꍇ�͂���� GetExternal ���D�悳���
				// ���������̂Ŗ��Ӗ��ɂȂ�BSetExternalUIHandler �ł̈ꊇ�w����s���B
				if (disableControl) {
					hr = SetExternalUIHandler(static_cast<CHtmlExt*>(this));
				} else {
					hr = SetExternalDispatch(static_cast<CHtmlExt*>(this));
				}
				if (SUCCEEDED(hr)) {
					
					// �C���^�[�t�F�[�X�擾
					hr = QueryControl(&browser);
					
					// �V���N�I�u�W�F�N�g�̍쐬
					CComObject<VNBSink>* sink;
					CComObject<VNBSink>::CreateInstance(&sink);
					
					// �C�x���gDispatch�̎w��
					hr = sink->QueryInterface(IID_IUnknown, (void**)&pSink);
					if ( SUCCEEDED(hr) ) {
						hr = AtlAdvise(browser, pSink, DIID_DWebBrowserEvents2, &dwCookie);
						if ( SUCCEEDED(hr) ) {
							sink->m_pBrowser = browser;
							sink->showWhenComplete = this;
							sink->disableControl = disableControl;
						}
					}
				}
			}
			// XXX ������IE �ɑ΂���t�b�N�������{������hook = gemEnvDepWinAddDispatchHook(BrowserDispatchHook, this);
		}
	}

	/**
	 * �폜������
	 */
	virtual ~CBrowserWindow() {
		if (browser) {
			if ( pSink ) {
				AtlUnadvise(browser, DIID_DWebBrowserEvents2, dwCookie);
			}
			browser.Release();
			browser.Detach();
		}
		if (m_hWnd) DestroyWindow();
	}
	
	bool isOk() {
		return m_hWnd && browser;
	}
	
	/**
	 * �u���E�U�N��
	 * @param path �N��������URL
	 */
	void open(const tjs_char *path) {

		{
			ttstr msg = "open:";
			msg += path;
			//TVPAddLog(msg);
		}
		
		if (browser) {
			CComVariant vUrl(path);
			CComVariant vempty;
			browser->Navigate2(&vUrl, &vempty, &vempty, &vempty, &vempty);
		}
	}

	virtual void reload() {
		if (browser) browser->Refresh();
	}
	
	void back() {
		if (browser) browser->GoBack();
	}
	
	void forward() {
		if (browser) browser->GoForward();
	}

	void setVisible(bool visible) {
		if (m_hWnd) {
			ShowWindow(visible);
		}
	}
	
	bool getVisible() {
		return m_hWnd && IsWindowVisible();
	}
	
	/**
	 * ���ꏊ�w��
	 */	
	void setPos(int x, int y, int w, int h) {
		RECT rect = {x, y, x+w, y+h};
		SetWindowPos(0, &rect, 0);
	}

	BEGIN_MSG_MAP(CBrowserWindow)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeydown)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
	END_MSG_MAP()
};

/**
 * ���b�Z�[�W�̃t�b�N�����B�������������̂��o�^�ł��Ȃ��Ƃ��܂������Ȃ��c
 * TranslateMessage ��ADispatchMessage �O�ɌĂяo�����
 * IE �ɑ�����͂��̃��b�Z�[�W������肵�ď����ł���ꏊ�ɑ���
 */
extern "C" int BrowserDispatchHook(MSG *msg, void *vwork)
{
	CBrowserWindow *br = (CBrowserWindow*)vwork;
	if (IsChild(br->m_hWnd, msg->hwnd)) {
		if ((msg->message >= WM_KEYFIRST && msg->message <= WM_KEYLAST)) {
			// �L�[���͂�IE�̐e�ɂȂ��Ă��� CBrowserWindow �ɑ���
			SendMessage(br->m_hWnd, msg->message, msg->wParam, msg->lParam);
			// ����� IE �ɂ������Ă���
//		} else if (msg->message == WM_COMMAND) {
//			// WM_COMMAND �͂���ɐe�ɑ���
//			SendMessage(br->parentHwnd, msg->message, msg->wParam, msg->lParam);
//			// ���ɑ���K�v���Ȃ��̂Ŏ̂Ă�
//			return 1;
		}
	}
	return 0;
}

//---------------------------------------------------------------------------

#include "IDispatchWrapper.hpp"

/**
 * �p������ TJS �p�Ɏ���
 * windows.external �� global �Ƀ}�b�s���O���Ă��܂�
 */
class TJSBrowserWindow : public CBrowserWindow {
public:
	/// �R���X�g���N�^
	TJSBrowserWindow(HWND hwnd, bool disableControl = true) : CBrowserWindow(hwnd, disableControl) {
		iTJSDispatch2 * global = TVPGetScriptDispatch();
		dispatchEx = new IDispatchWrapper(global);
		global->Release();
	}
};


//---------------------------------------------------------------------------

/*
	�v���O�C�����Ńl�C�e�B�u�������ꂽ�N���X��񋟂��A�g���g�����Ŏg�p�ł���悤��
	�����ł��B

	�l�C�e�B�u�N���X�� tTJSNativeInstance ���p�������N���X��ɍ쐬���A���̃l�C
	�e�B�u�N���X�Ƃ̃C���^�[�t�F�[�X�� tTJSNativeClassForPlugin ���x�[�X�ɍ쐬��
	�܂��B

	�uTJS2 ���t�@�����X�v�́u�g�ݍ��݂̎�����v�́u��{�I�Ȏg�����v�ɂ����Ɠ���
	�N���X�������ł͍쐬���܂��B�������A�v���O�C���Ŏ�������s����ATJS2 ���t�@
	�����X�ɂ����Ƃ͎኱�����̎d�����قȂ邱�Ƃɒ��ӂ��Ă��������B
*/

//---------------------------------------------------------------------------
// �e�X�g�N���X
//---------------------------------------------------------------------------
/*
	�e�I�u�W�F�N�g (iTJSDispatch2 �C���^�[�t�F�[�X) �ɂ̓l�C�e�B�u�C���X�^���X��
	�Ă΂��AiTJSNativeInstance �^�̃I�u�W�F�N�g��o�^���邱�Ƃ��ł��A�����
	�I�u�W�F�N�g������o�����Ƃ��ł��܂��B
	�܂��A�l�C�e�B�u�C���X�^���X�̎����ł��B�l�C�e�B�u�C���X�^���X����������ɂ�
	tTJSNativeInstance ����N���X�𓱏o���܂��BtTJSNativeInstance ��
	iTJSNativeInstance �̊�{�I�ȓ�����������Ă��܂��B
*/
class NI_Browser : public tTJSNativeInstance // �l�C�e�B�u�C���X�^���X
{

public:
	TJSBrowserWindow *browser;

	NI_Browser()
	{
		// �R���X�g���N�^
		/*
			NI_Browser �̃R���X�g���N�^�ł��BC++ �N���X�Ƃ��Ă̏������� ��q��
			Construct ���������ōς܂��Ă����AConstruct �ł̏������͍ŏ����̕�
			�ɂ��邱�Ƃ��������߂��܂��B���̗�ł́A�f�[�^�����o�� Value �ɏ���
			�l�Ƃ��� 0 ��ݒ肵�Ă��܂��B
		 */
		browser = NULL;
	}

	tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj)
	{
		HWND handle = 0;
		bool disableControl = true;
		if (numparams >= 1) {
			iTJSDispatch2 *win = param[0]->AsObjectNoAddRef();
			if (win) {
				tTJSVariant hwnd;
				if (win->PropGet(0, TJS_W("HWND"), NULL, &hwnd, win) == TJS_S_OK) {
					handle = (HWND)(int)hwnd;
				}
			}
			if (numparams >= 2) {
				disableControl = (int)*param[1] != 0;
			}
		}
		browser = new TJSBrowserWindow(handle, disableControl);
		return S_OK;
	}

	void TJS_INTF_METHOD Invalidate()
	{
		// �I�u�W�F�N�g�������������Ƃ��ɌĂ΂��
		if (browser) {
			delete browser;
		}
	}
};
//---------------------------------------------------------------------------
/*
	����� NI_Browser �̃I�u�W�F�N�g���쐬���ĕԂ������̊֐��ł��B
	��q�� TJSCreateNativeClassForPlugin �̈����Ƃ��ēn���܂��B
*/
static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_Browser()
{
	return new NI_Browser();
}
//---------------------------------------------------------------------------
/*
	TJS2 �̃l�C�e�B�u�N���X�͈�ӂ� ID �ŋ�ʂ���Ă���K�v������܂��B
	����͌�q�� TJS_BEGIN_NATIVE_MEMBERS �}�N���Ŏ����I�Ɏ擾����܂����A
	���� ID ���i�[����ϐ����ƁA���̕ϐ��������Ő錾���܂��B
	�����l�ɂ͖����� ID ��\�� -1 ���w�肵�Ă��������B
*/
#define TJS_NATIVE_CLASSID_NAME ClassID_Browser
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;
//---------------------------------------------------------------------------
/*
	TJS2 �p�́u�N���X�v���쐬���ĕԂ��֐��ł��B
*/
static iTJSDispatch2 * Create_NC_Browser()
{
	/*
		�܂��A�N���X�̃x�[�X�ƂȂ�N���X�I�u�W�F�N�g���쐬���܂��B
		����ɂ� TJSCreateNativeClassForPlugin ��p���܂��B
		TJSCreateNativeClassForPlugin �̑�P�����̓N���X���A��Q������
		�l�C�e�B�u�C���X�^���X��Ԃ��֐����w�肵�܂��B
		�쐬�����I�u�W�F�N�g���ꎞ�I�Ɋi�[���郍�[�J���ϐ��̖��O��
		classobj �ł���K�v������܂��B
	*/
	tTJSNativeClassForPlugin * classobj =
		TJSCreateNativeClassForPlugin(TJS_W("Browser"), Create_NI_Browser);


	/*
		TJS_BEGIN_NATIVE_MEMBERS �}�N���ł��B�����ɂ� TJS2 ���Ŏg�p����N���X��
		���w�肵�܂��B
		���̃}�N���� TJS_END_NATIVE_MEMBERS �}�N���ŋ��܂ꂽ�ꏊ�ɁA�N���X��
		�����o�ƂȂ�ׂ����\�b�h��v���p�e�B�̋L�q�����܂��B
	*/
	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/Browser)

		/*
			��� finalize ���\�b�h��錾���܂��Bfinalize �ɑ������鏈����
			tTJSNativeInstance::Invalidate ���I�[�o�[���C�h���邱�Ƃł������ł�
			�܂��̂ŁA�ʏ�͋�̃��\�b�h�ŏ\���ł��B
		*/
		TJS_DECL_EMPTY_FINALIZE_METHOD

		/*
			(TJS��) �R���X�g���N�^��錾���܂��BTJS �ŃN���X�������Ƃ��A
			�N���X���ŃN���X�Ɠ����̃��\�b�h��錾���Ă��镔���ɑ������܂��B

			TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL �}�N���̂P�Ԗڂ̈����̓l�C�e�B�u
			�C���X�^���X�Ɋ��蓖�Ă�ϐ����ŁA�Q��ʖڂ̈����͂��̕ϐ��̌^���ł��B
			���̗�ł̂��̃u���b�N���ł� NI_Browser * _this �Ƃ����ϐ������p�\�ŁA
			�l�C�e�B�u�C���X�^���X�ɃA�N�Z�X���邱�Ƃ��ł��܂��B
			�}�N���̂R�Ԗڂ̈����́ATJS ���Ŏg�p����N���X�����w�肵�܂��B
			TJS_END_NATIVE_CONSTRUCTOR_DECL �}�N���̈��������l�ł��B
			�������A�R���X�g���N�^�ɑ������鏈���� tTJSNativeInstance::Construct
			���I�[�o�[���C�h���鎖�Ŏ����ł���̂ŁA�����ł͉��������� S_OK ���
			���܂��B
		*/
		TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(
			/*var.name*/_this,
			/*var.type*/NI_Browser,
			/*TJS class name*/Browser)
		{
			// NI_Browser::Construct �ɂ����e���L�q�ł���̂�
			// �����ł͉������Ȃ�
			return TJS_S_OK;
		}
		TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/Browser)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/open) // open ���\�b�h
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_Browser);
			if (numparams < 1) return TJS_E_BADPARAMCOUNT;
			_this->browser->open(param[0]->GetString());
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/open)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setPos) // setPos ���\�b�h
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_Browser);
			if (numparams < 4) return TJS_E_BADPARAMCOUNT;
			_this->browser->setPos(*param[0], *param[1], *param[2], *param[3]);
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/setPos)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/back) // back ���\�b�h
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_Browser);
			_this->browser->back();
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/back)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/forward) // forward ���\�b�h
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_Browser);
			_this->browser->forward();
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/forward)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/reload) // reload ���\�b�h
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/NI_Browser);
			_this->browser->reload();
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/reload)

		TJS_BEGIN_NATIVE_PROP_DECL(visible) // visible �v���p�e�B
		{
			TJS_BEGIN_NATIVE_PROP_GETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_Browser);
				*result = _this->browser->getVisible();
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_GETTER

			TJS_BEGIN_NATIVE_PROP_SETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,	/*var. type*/NI_Browser);
				_this->browser->setVisible((bool)*param);
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_SETTER
		}
		TJS_END_NATIVE_PROP_DECL(visible)

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
		iTJSDispatch2 * tjsclass = Create_NC_Browser();
		
		// 2 tjsclass �� tTJSVariant �^�ɕϊ�
		val = tTJSVariant(tjsclass);
		
		// 3 ���ł� val �� tjsclass ��ێ����Ă���̂ŁAtjsclass ��
		//   Release ����
		tjsclass->Release();
		
		
		// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
		global->PropSet(
			TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
			TJS_W("Browser"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
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

	// �A�[�J�C�u����
	registArchive();
	
	// ATL�֘A������
	_Module.Init(NULL, NULL);
	AtlAxWinInit();

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

	// TJS �̃O���[�o���I�u�W�F�N�g�ɓo�^���� Browser �N���X�Ȃǂ��폜����

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
			TJS_W("Browser"), // �����o��
			NULL, // �q���g
			global // �R���e�L�X�g
			);
	}
	
	// - global �� Release ����
	if(global) global->Release();

	// �X�^�u�̎g�p�I��(�K���L�q����)
	TVPUninitImportStub();

	_Module.Term();

	unregistArchive();
	
	return S_OK;
}
//---------------------------------------------------------------------------

