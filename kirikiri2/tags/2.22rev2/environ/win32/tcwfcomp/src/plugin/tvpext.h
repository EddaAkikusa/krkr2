#ifndef TVPEXTH
#define TVPEXTH
#include <windows.h>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#pragma pack(push,4)
struct TTVPVersion
{
	char Version[20];
	char VersionInfo[128];
};
#pragma pack(pop)

//---------------------------------------------------------------------------
#ifndef TJS_ITJSDISPATCH_DEFINED
#define TJS_ITJSDISPATCH_DEFINED

//#pragma option push -b  -a4
#pragma pack(push,4)
enum TTJSVariantType
{
	tvtVoid,  // empty
	tvtObject,
	tvtString,
	tvtInteger,
	tvtReal
};
class ITJSDispatch;
struct TTJSVariant_S
{
	typedef __int64 TTVInteger;
	typedef long double TTVReal;

	union
	{
		ITJSDispatch *Object;
		wchar_t *String;  // SysAllocString ���Ŋm�ۂ���
		TTVInteger Integer;
		TTVReal Real;
	};
	TTJSVariantType vt;
};
#pragma pack(pop)
//#pragma option pop


class ITJSDispatch : public IUnknown
{
public:
// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
			REFIID riid,
			void **ppvObject) = 0;
	virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
	virtual ULONG STDMETHODCALLTYPE Release(void) = 0;

// ITJSDispatch
	virtual HRESULT STDMETHODCALLTYPE Invoke(wchar_t* name,DWORD flag,
		TTJSVariant_S &Result,TTJSVariant_S **ValArray,long NumVal,
		ITJSDispatch *ObjThis) =0;

	virtual HRESULT STDMETHODCALLTYPE Reserved1(WORD,long *,
		TTJSVariant_S *,TTJSVariant_S *)=0;
	virtual HRESULT STDMETHODCALLTYPE Reserved2(WORD,ITJSDispatch *)=0;
};
#else
typedef TTJSVariant TTJSVariant_S;
#endif
//---------------------------------------------------------------------------

class ITVP
{
public:
	// IUnkown �h���N���X�ł͂Ȃ��̂Œ���

// ITVP
	virtual HRESULT STDMETHODCALLTYPE
		GetVersion(TTVPVersion *version)=0;
		/*
			TTVPVersion �\���̂Ƀo�[�W������\���������ݒ肵�܂��B
		*/


	virtual HRESULT STDMETHODCALLTYPE
		GetAppWindow(HWND *appwin)=0;
		/*
			�A�v���P�[�V�����E�B���h�E�� appwin �ɕԂ��܂��B
			�A�v���P�[�V�����E�B���h�E�́A�g���g���̂��ׂẴE�B���h�E�̐e
			�E�B���h�E�ŁA��ʏ�ł͂�����\���ł����A�^�X�N�o�[�ɕ\����
			��Ă���̂͂��̃A�v���P�[�V�����E�B���h�E�ł��B
		*/

	virtual HRESULT STDMETHODCALLTYPE
		GetScriptDispatch(ITJSDispatch **out)=0;
		/*
			�X�N���v�g���� TJS �� global �I�u�W�F�N�g�� out �ɕԂ��܂��B
			global �I�u�W�F�N�g����A�g���g����� system ���͂��߂Ƃ���
			�I�u�W�F�N�g�E�v���p�e�B�ɃA�N�Z�X�ł��܂��B
		*/

	virtual HRESULT STDMETHODCALLTYPE
		GetStream(char *filename,IStream **out);
		/*
			filename �Ŏw�肳�ꂽ�t�@�C���ɑ΂���X�g���[���� out ��
			�Ԃ��܂��B
		*/
};


//---------------------------------------------------------------------------

class IWaveUnpacker
{
public:
// IUnknown �h���N���X�ł͂Ȃ��̂Œ���
	virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
	virtual ULONG STDMETHODCALLTYPE Release(void) = 0;

// IWaveUnpacker
	virtual HRESULT STDMETHODCALLTYPE GetTypeName(char *buf,long buflen)=0;
		/*
			buf �ɁA���� Wave �`����\��������� *buf �ɐݒ肵�Ă��������B
			buflen �́Abuf �Ɋm�ۂ��ꂽ������ŁAnull terminater ���܂ނ̂�
			���ӁB
		*/

	virtual HRESULT STDMETHODCALLTYPE GetWaveFormat(long *samplepersec,
		long *channels,long *bitspersample)=0;
		/*
			�o�͂��� Wave �̌`���� *samplepersec, *channels, *bitspersample ��
			�Ԃ��Ă��������B
		*/

	virtual HRESULT STDMETHODCALLTYPE Render(void *buffer,long bufsize,
		long *numwrite) =0;
		/*
			�f�R�[�h���Ă��������B
			bufsize �ɂ� buffer �̃T�C�Y���o�C�g�P�ʂŎw�肳��܂��B
			numwrite �ɂ́A�o�b�t�@�ɏ����ꂽ�f�[�^�̐����o�C�g�P�ʂŕԂ��܂��B
			�������AWaveUnpacker �́Anumwrite<bufsize �̏ꍇ�́A�c���
			0 �Ŗ��߂Ă��������B
		*/
	
	virtual HRESULT STDMETHODCALLTYPE GetLength(long *length)=0;
		/*
			�f�[�^���� ms �P�ʂ� *length �ɕԂ��Ă��������B
			�Ή��ł��Ȃ��ꍇ�� E_NOTIMPL ��Ԃ��Ă��������B���̏ꍇ��
			WaveSoundBuffer �� totalTime �v���p�e�B�� 0 ��\���悤�ɂȂ�܂��B
		*/

	virtual HRESULT STDMETHODCALLTYPE GetCurrentPosition(long *pos)=0;
		/*
			���݂̃f�R�[�h�ʒu�� *pos �ɕԂ��Ă��������B
			�Ή��ł��Ȃ��ꍇ�� E_NOTIMPL ��Ԃ��Ă��������B���̏ꍇ��
			WaveSoundBuffer �� position �v���p�e�B�͈Ӗ��̂Ȃ����l��
			�����悤�ɂȂ�܂��B
		*/

	virtual HRESULT STDMETHODCALLTYPE SetCurrentPosition(long pos)=0;
		/*
			���݂̃f�R�[�h�ʒu��ݒ肵�Ă��������Bpos �� ms �P�ʂł�
			�ʒu�ł��B
			�Œ�ł� pos=0 �Ƃ��ČĂ΂ꂽ�Ƃ��ɁA�擪�ւ̊����߂���
			�o���悤�ɂ��Ă��������B

			���̂ق��̏ꍇ�A�Ή��ł��Ȃ��ꍇ�� E_NOTIMPL ��Ԃ��Ă��������B
			���̏ꍇ��WaveSoundBuffer �� position �v���p�e�B�ւ̑���͖�������܂��B
		*/

	virtual HRESULT STDMETHODCALLTYPE Invoke(wchar_t* name,DWORD flag,
		TTJSVariant_S &Result,TTJSVariant_S **ValArray,long NumVal,
		ITJSDispatch *ObjThis) =0;
		/*
			WaveSoundBuffer �I�u�W�F�N�g�̂����A��������Ȃ������v���p�e�B/
			���\�b�h������ Invoke ��ʂ��Ēʒm����܂��̂ŁA�K�v�ɉ�����
			�������Ă��������B�v���p�e�B�⃁�\�b�h��ǉ�����̂Ɏg���܂��B
			�������Ȃ��ꍇ�� E_NOTIMPL ��Ԃ��Ă��������B
		*/
};

//---------------------------------------------------------------------------
#endif


