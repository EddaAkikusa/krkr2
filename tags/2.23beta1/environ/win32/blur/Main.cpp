//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
#include <math.h>
//---------------------------------------------------------------------------



/*
	I_out = ��(I_in*A_in) / ��A_in
	A_out = ��(A_in) / ��1
*/

//---------------------------------------------------------------------------
// �ڂ������s���N���X
//---------------------------------------------------------------------------
class tBlur
{
	tjs_uint8 * DestBuffer;
	tjs_uint8 * SrcBuffer;
	tjs_int Pitch;
	tjs_int Width;
	tjs_int Height;
	tjs_int BlurWidth;
	tjs_int BlurHeight;
	bool HasAlpha;

	tjs_uint32 *  TmpBuf; // ��Ɨ̈�
	tjs_uint32 ** TmpOrgLines; // ��Ɨ̈�̊e���C���̐擪�ւ̃|�C���^
	tjs_uint32 ** TmpLines; // ��Ɨ̈� (TmpOrgLines����ёւ�����)
	tjs_uint32 ** TmpLinePtr: // SrcLines + BlurHeight/2

public:
	tBlur(tjs_uint8 * destbuffer,
			tjs_uint8 *srcbuffer,
			tjs_int pitch,
			tjs_int width,
			tjs_int height,
			tjs_int blurwidth,
			tjs_int blurheight,
			bool hasalpha) :
				DestBuffer(destbuffer),
				SrcBuffer(srcbuffer),
				Pitch(pitch),
				Width(width),
				Height(height),
				BlurWidth(blurwidth),
				BlurHeight(blurheight),
				HasAlpha(hasalpha)
	{
		// SrcLines
		TmpBuf = NULL;
		TmpOrgLines = NULL;
		TmpLinePtr = NULL;
		try
		{
			int i;
			int pitch = (Width + 1) & ~ 1; // �����ɃA���C��
			TmpBuf = new tjs_uint32 [BlurHeight * pitch];

			TmpOrgLines = new tjs_uint32 * [BlurHeight];
			for(i = 0; i < BlurHeight; i++) TmpOrgLines[i] = TmpBuf + pitch*i;
			TmpLines = new tjs_uint32 * [BlurHeight];

			TmpLinePtr = TmpLines + BlurHeight / 2;
		}
		catch(...)
		{
			Cleanup();
			throw;
		}
	}

	~tBlur()
	{
		Cleanup();
	}

private:
	void Cleanup()
	{
		if(TmpBuf) delete [] TmpBuf, TmpBuf = NULL;
		if(TmpOrgLines) delete [] TmpOrgLines, TmpOrgLines = NULL;
		if(TmpLines) delete [] TmpLines, TmpLines = NULL;
	}

private:

public:
	void DoBlur();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// �ڂ������s�� TJS �֐�
//---------------------------------------------------------------------------
class tBlurImageFunction : public tTJSDispatch
{
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		if(membername) return TJS_E_MEMBERNOTFOUND;

		// ���� : ���C��, ��, ����

		if(numparams < 3) return TJS_E_BADPARAMCOUNT;

		iTJSDispatch2 * layerobj = param[0]->AsObjectNoAddRef();

		tjs_uint8 * bufferptr;
		tjs_int bufferpitch;
		tjs_int width;
		tjs_int height;
		bool hasalpha;
		tTVPDrawFace face;

		// �摜�̃T�C�Y�A�s�N�Z���o�b�t�@�ւ̃|�C���^�A�s�b�`�𓾂�

		tTJSVariant val;
		static tjs_uint32 imageWidth_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("imageWidth"), &imageWidth_hint, &val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageWidth failed."));
		width = val;

		static tjs_uint32 imageHeight_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("imageHeight"), &imageHeight_hint, &val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageHeight failed."));
		height = val;

		static tjs_uint32 mainImageBufferForWrite_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("mainImageBufferForWrite"), &mainImageBufferForWrite_hint,
			&val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.mainImageBufferForWrite failed."));
		bufferptr = (tjs_uint8 *)(tjs_int)val;

		static tjs_uint32 mainImageBufferPitch_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("mainImageBufferPitch"), &mainImageBufferPitch_hint,
			&val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.mainImageBufferPitch failed."));
		bufferpitch = val;

		static tjs_uint32 face_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("face"), &face_hint,
			&val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.face failed."));
		face = (tTVPDrawFace)(tjs_int)val;
		if(face == dfBoth)
			hasalpha = true;
		else if(face == dfMain)
			hasalpha = false;
		else
			TVPThrowExceptionMessage(TJS_W("not drawable face"));

			/*
				note:
				PropGet ���� "hint" (�q���g) �����ɂ� tjs_uint32 �^�̕ϐ��ւ̃|�C���^��n���B
				TJS2 �́A�����o������������Ƃ��ɁA�����r���ŎZ�o���ꂽ���l�� hint �Ɋi�[���ĕԂ��B
				�Q��ڈȍ~�́A�Ă� hint �ɓn���ꂽ�l��p���Č������s�����߁A�����������ɂȂ�B
				���ۂ̓q���g�̓����o���̃n�b�V���ŁA���������o���Ȃ�Γ������l�ɂȂ邪�A
				�����q���g�̒l������Ƃ����ē��������o���ł���Ƃ͌���Ȃ��B
				�q���g�̒l���s���ȏꍇ�͐������l�Ɏ����I�ɏC������邪�A�����l�Ƃ��Ă� 0 ��
				�w�肷�邱�Ƃ����������B
			*/


		BlurParam bp;
		bp.laybuf = bufferptr;
		bp.laybufpitch = bufferpitch;
		bp.laywidth = width;
		bp.layheight = height;
		bp.blurwidth = *param[1];
		bp.blurheight = *param[2];
		bp.hasalpha = hasalpha;

		// �ڂ���
		BlurImage(bp);

		
		// �̈���A�b�v�f�[�g
		{
			tTJSVariant val[4];
			tTJSVariant *pval[4] = { val, val +1, val +2, val +3 };

			val[0] = (tjs_int64)0;
			val[1] = (tjs_int64)0;
			val[2] = (tjs_int64)width;
			val[3] = (tjs_int64)height;
			static tjs_uint32 update_hint = 0;
			layerobj->FuncCall(0, L"update", &update_hint, NULL, 4, pval, layerobj);
		}


		// �߂�
		return TJS_S_OK;
	}
} * BlurImageFunction;
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" HRESULT _stdcall _export V2Link(iTVPFunctionExporter *exporter)
{
	// �X�^�u�̏�����(�K���L�q����)
	TVPInitImportStub(exporter);

	// TestFunction �̍쐬�Ɠo�^
	tTJSVariant val;

	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	// 1 �܂��I�u�W�F�N�g���쐬
	BlurImageFunction = new tBlurImageFunction();

	// 2 BlurImageFunction �� tTJSVariant �^�ɕϊ�
	val = tTJSVariant(BlurImageFunction);

	// 3 ���ł� val �� BlurImageFunction ��ێ����Ă���̂ŁA
	// BlurImageFunction �� Release ����
	BlurImageFunction->Release();


	// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
	global->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		TJS_W("blurImage"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
		NULL, // �q���g ( �{���̓����o���̃n�b�V���l�����ANULL �ł��悢 )
		&val, // �o�^����l
		global // �R���e�L�X�g ( global �ł悢 )
		);


	// - global �� Release ����
	global->Release();

	// val ���N���A����B
	// ����͕K���s���B�������Ȃ��� val ���ێ����Ă���I�u�W�F�N�g
	// �� Release ���ꂸ�A���Ɏg�� TVPPluginGlobalRefCount �����m�ɂȂ�Ȃ��B
	val.Clear();


	// ���̎��_�ł� TVPPluginGlobalRefCount �̒l��
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	TVPAddLog(TVPPluginGlobalRefCount);
	// �Ƃ��čT���Ă����BTVPPluginGlobalRefCount �͂��̃v���O�C������
	// �Ǘ�����Ă��� tTJSDispatch �h���I�u�W�F�N�g�̎Q�ƃJ�E���^�̑��v�ŁA
	// ������ɂ͂���Ɠ������A����������Ȃ��Ȃ��ĂȂ��ƂȂ�Ȃ��B
	// �����Ȃ��ĂȂ���΁A�ǂ����ʂ̂Ƃ���Ŋ֐��Ȃǂ��Q�Ƃ���Ă��āA
	// �v���O�C���͉���ł��Ȃ��ƌ������ƂɂȂ�B

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
	TVPAddLog(TVPPluginGlobalRefCount);
	if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;
		// E_FAIL ���A��ƁAPlugins.unlink ���\�b�h�͋U��Ԃ�

	// TJS �̃O���[�o���I�u�W�F�N�g�ɓo�^���� blurImage �֐����폜����

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
			TJS_W("blurImage"), // �����o��
			NULL, // �q���g
			global // �R���e�L�X�g
			);
			// �o�^�����֐�����������ꍇ�� ������J��Ԃ�
	}

	// - global �� Release ����
	if(global) global->Release();

	// �X�^�u�̎g�p�I��(�K���L�q����)
	TVPUninitImportStub();

	return S_OK;
}
//---------------------------------------------------------------------------



