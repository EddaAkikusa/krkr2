//---------------------------------------------------------------------------
/*
	���̃\�[�X�ł́Axp3enc.dll ���g���� Releaser �ňÍ������� XP3 �t�@�C����
	������悤�ɂ���g���g���p�v���O�C�����������B

	�v���O�C���̃t�@�C�����́A�g���q�� tpm �Ȃ�΂Ȃ�ł����܂�Ȃ��B
	�g���g���͊g���q�� tpm �̃t�@�C����������� startup.tjs ��ǂݍ�������
	����O�A���Ȃ킿 XP3 �t�@�C�����ŏ��ɊJ�����O�ɓǂݍ��ނ̂ŁA�g���q��
	tpm �ł���K�v������B
	�g���q�� tpm �Ȃ�΃t�@�C������(�v���O�C���Ԃ̈ˑ������Ȃ����)�Ȃ�ł�
	���܂�Ȃ����A�����ł͕֋X��Axp3dec.tpm �Ƃ���B

	xp3enc.dll �ƈقȂ�Axp3dec.tpm �͋g���g���{�̂̃v���O�C���Ƃ��ē��삷��B
	���̂��߁A�g���g���{�̗p�̃v���O�C���̍�@�ɏ]���č쐬����Ȃ���΂Ȃ�
	�Ȃ����A�g���g���{�̂̊e�@�\�𗘗p�ł���B
*/





//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
	// tp_stub.h �ɂ̓C���N���[�h�E�p�X���w�肵�Ă���
	// tp_stub.cpp ����� tp_stub.h �̃o�[�W�����͋g���g���{�̂�
	// �������̕��ł��邱�Ƃ��D�܂���
	// (�������{�̑��̃C���^�[�t�F�[�X���ς��Ȃ���΁A�Â�
	// xp3dec.dll �ł��p�����ė��p�ł���)
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void TJS_INTF_METHOD TVPXP3ArchiveExtractionFilter(
	tTVPXP3ExtractionFilterInfo *info)
{
	// TVPXP3ArchiveExtractionFilter �֐��͖{�̑�����Ăяo�����
	// �R�[���o�b�N�֐��ł���B
	// ����������A����� tTVPXP3ExtractionFilterInfo �\���̂ւ̃|�C���^�ł���B

	// TVPXP3ArchiveExtractionFilter �́A��q��
	// V2Link �֐����� TVPSetXP3ArchiveExtractionFilter �ɂ��
	// �ݒ肳���B

	// �����ł͒P���ɁAxp3enc.dll �̃T���v���ō쐬���ꂽ XP3 �A�[�J�C�u��
	// �������ׂ��A�f�[�^�����ׂăr�b�g���]���邱�Ƃɂ���

	/*
		tTVPXP3ExtractionFilterInfo �̃����o�͈ȉ��̒ʂ�
		* SizeOfSelf        : �������g�̍\���̂̃T�C�Y
		* Offset            : "Buffer" �����o���w�������f�[�^���A
		*                   : �A�[�J�C�u�Ɋi�[����Ă��邻�̃t�@�C���̐擪�����
		*                   : �ǂ̃I�t�Z�b�g�ʒu����̂��̂��A��\��
		* Buffer            : �f�[�^�{��
		* BufferSize        : "Buffer" �����o�̎w�������f�[�^�̃T�C�Y(�o�C�g�P��)
		* FileName          : �A�[�J�C�u���t�@�C���̃t�@�C����
		* ArchiveName       : �A�[�J�C�u�t�@�C�����ꎩ�̂̃t�@�C����
	*/

	// �ꉞ�\���̂̃T�C�Y���`�F�b�N����
	if(info->SizeOfSelf != sizeof(tTVPXP3ExtractionFilterInfo))
	{
		// �\���̂̃T�C�Y���Ⴄ�ꍇ�͂����ŃG���[�ɂ��������悢
		TVPThrowExceptionMessage(TJS_W("Incompatible tTVPXP3ExtractionFilterInfo size"));
			// TVPThrowExceptionMessage �͗�O���b�Z�[�W�𓊂���֐�
			// ���̊֐��͖߂�Ȃ� ( �����ƌĂяo�����������̂ڂ����ʒu��
			// ��O���⑫����邽�� )
	}

	// ����
	tjs_uint i;
	for(i = 0; i < info->BufferSize; i++)
		((unsigned char *)info->Buffer)[i] ^= 0xff;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
	void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall V2Link(iTVPFunctionExporter *exporter)
{
	// V2Link �́A�g���g���{�̂ƃ����N�����Ƃ��ɌĂяo�����
	
	// �X�^�u�̏�����(�K���L�q����)
	TVPInitImportStub(exporter);

	// TVPSetXP3ArchiveExtractionFilter �֐���
	// TVPXP3ArchiveExtractionFilter ���w�肵�A�R�[���o�b�N�֐���ݒ肷��
	TVPSetXP3ArchiveExtractionFilter(TVPXP3ArchiveExtractionFilter);

	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall V2Unlink()
{
	// V2Unlink �́A�g���g���{�̂���؂藣�����Ƃ��ɌĂяo�����

	// �ꉞ TVPSetXP3ArchiveExtractionFilter �� NULL ��n���A
	// �R�[���o�b�N�֐��̓o�^����������
	TVPSetXP3ArchiveExtractionFilter(NULL);

	// �X�^�u�̎g�p�I��(�K���L�q����)
	TVPUninitImportStub();

	return S_OK; 
}
//---------------------------------------------------------------------------



