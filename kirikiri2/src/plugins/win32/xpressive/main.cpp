#include <windows.h>
#include "tp_stub.h"


class tTJSNativeClassForRegExp : public tTJSDispatch
{
public:
	tTJSNativeClassForRegExp(tjs_char const *className) : tTJSDispatch(), classname(className) {
		self = this;
		classobj = TJSCreateNativeClassForPlugin(className, _CreateNativeInstance);
	}

	static void Link(iTJSDispatch2 *global) {
		Initialize();
		if (self) {
			self->Setup(global);
		}
	}
	static void Unlink(iTJSDispatch2 *global) {
		if (self) {
			self->Terminate(global);
			self->Release();
		}
		self = 0;
	}

private:
	static tTJSNativeClassForRegExp * self;
	static iTJSNativeInstance * TJS_INTF_METHOD _CreateNativeInstance() {
		return self ? self->CreateNativeInstance() : NULL;
	}
	static void Initialize();
	void Setup(iTJSDispatch2 *global) {
		tTJSVariant val(classobj);
		global->PropSet(
			TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
			classname, // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
			NULL, // �q���g ( �{���̓����o���̃n�b�V���l�����ANULL �ł��悢 )
			&val, // �o�^����l
			global // �R���e�L�X�g ( global �ł悢 )
			);
		classobj->Release();
	}
	void Terminate(iTJSDispatch2 *global) {
		global->DeleteMember(
			0, // �t���O ( 0 �ł悢 )
			classname, // �����o��
			NULL, // �q���g
			global // �R���e�L�X�g
			);
	}

protected:
	tjs_char const *classname;
	tTJSNativeClassForPlugin * classobj;
	virtual iTJSNativeInstance *CreateNativeInstance() = 0;
};
tTJSNativeClassForRegExp* tTJSNativeClassForRegExp::self = 0;


// hacks

#define TJS_cdecl
#define TJS_strchr			wcschr

#define TJS_NATIVE_CLASSID_NAME tTJSNC_RegExp::ClassID
#define tTJSNativeClass tTJSNativeClassForRegExp

#define TJS_eTJSError TVPThrowExceptionMessage
inline void TVPThrowExceptionMessage(const tjs_nchar * msg) {
	ttstr msg_(msg);
	TVPThrowExceptionMessage(msg_.c_str());
}

#pragma warning (disable: 4996)
#pragma warning (disable: 4800)

#define TJS_USE_XPRESSIVE
#include "tjsRegExp.cpp"

void tTJSNativeClassForRegExp::Initialize()
{
	TJS::TJSCreateRegExpClass();
}


#define EXPORT(hr) extern "C" __declspec(dllexport) hr __stdcall

#ifdef _MSC_VER
#pragma comment(linker, "/EXPORT:V2Link=_V2Link@4")
#pragma comment(linker, "/EXPORT:V2Unlink=_V2Unlink@0")
#endif

//--------------------------------------
int WINAPI
DllEntryPoint(HINSTANCE /*hinst*/, unsigned long /*reason*/, void* /*lpReserved*/)
{
	return 1;
}

//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;

EXPORT(HRESULT) V2Link(iTVPFunctionExporter *exporter)
{
	// �X�^�u�̏�����(�K���L�q����)
	TVPInitImportStub(exporter);

	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global) {

		// �o�^����
		tTJSNativeClassForRegExp::Link(global);

		// - global �� Release ����
		global->Release();
	}

	// ���̎��_�ł� TVPPluginGlobalRefCount �̒l��
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	// �Ƃ��čT���Ă����BTVPPluginGlobalRefCount �͂��̃v���O�C������
	// �Ǘ�����Ă��� tTJSDispatch �h���I�u�W�F�N�g�̎Q�ƃJ�E���^�̑��v�ŁA
	// ������ɂ͂���Ɠ������A����������Ȃ��Ȃ��ĂȂ��ƂȂ�Ȃ��B
	// �����Ȃ��ĂȂ���΁A�ǂ����ʂ̂Ƃ���Ŋ֐��Ȃǂ��Q�Ƃ���Ă��āA
	// �v���O�C���͉���ł��Ȃ��ƌ������ƂɂȂ�B

	return S_OK;
}
//---------------------------------------------------------------------------
EXPORT(HRESULT) V2Unlink()
{
	// �g���g��������A�v���O�C����������悤�Ƃ���Ƃ��ɌĂ΂��֐�

	// �������炩�̏����Ńv���O�C��������ł��Ȃ��ꍇ��
	// ���̎��_�� E_FAIL ��Ԃ��悤�ɂ���B
	// �����ł́ATVPPluginGlobalRefCount �� GlobalRefCountAtInit ����
	// �傫���Ȃ��Ă���Ύ��s�Ƃ������Ƃɂ���B
	if (TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;


	/*
		�������A�N���X�̏ꍇ�A�����Ɂu�I�u�W�F�N�g���g�p���ł���v�Ƃ������Ƃ�
		�m�邷�ׂ�����܂���B��{�I�ɂ́APlugins.unlink �ɂ��v���O�C���̉����
		�댯�ł���ƍl���Ă������� (�������� Plugins.link �Ń����N������A�Ō��
		�Ńv���O�C������������A�v���O�����I���Ɠ����Ɏ����I�ɉ��������̂��g)�B
	*/

	// - �܂��ATJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if(global) {

		// ��n��
		tTJSNativeClassForRegExp::Unlink(global);

		// - global �� Release ����
		global->Release();
	}

	// �X�^�u�̎g�p�I��(�K���L�q����)
	TVPUninitImportStub();

	return S_OK;
}
