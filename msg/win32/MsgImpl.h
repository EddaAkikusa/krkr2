//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Definition of Messages and Message Related Utilities
//---------------------------------------------------------------------------
// 2006/7/2
//     TVPCannnotLinkPluginWhilePluginLinking added.
//---------------------------------------------------------------------------
#ifndef MsgImplH
#define MsgImplH

#include "tjsMessage.h"

#ifndef TVP_MSG_DECL
	#define TVP_MSG_DECL(name, msg) extern tTJSMessageHolder name;
#endif

//---------------------------------------------------------------------------
// Message Strings ( these should be localized )
//---------------------------------------------------------------------------
// Japanese localized messages
TVP_MSG_DECL(TVPScriptExceptionRaised,
	"�X�N���v�g�ŗ�O���������܂���");

TVP_MSG_DECL(TVPHardwareExceptionRaised,
	"�n�[�h�E�F�A��O���������܂���");

TVP_MSG_DECL(TVPMainCDPName,
	"�X�N���v�g�G�f�B�^ (���C��)");

TVP_MSG_DECL(TVPExceptionCDPName,
	"�X�N���v�g�G�f�B�^ (��O�ʒm)");

TVP_MSG_DECL(TVPCannnotLocateUIDLLForFolderSelection,
	"�t�H���_/�A�[�J�C�u�̑I����ʂ�\�����悤�Ƃ��܂����� "
		"krdevui.dll ��������Ȃ��̂ŕ\���ł��܂���.\n"
		"���s����t�H���_/�A�[�J�C�u�̓R�}���h���C���̈����Ƃ��Ďw�肵�Ă�������");

TVP_MSG_DECL(TVPInvalidUIDLL,
	"krdevui.dll ���ُ킩�A�o�[�W��������v���܂���");

TVP_MSG_DECL(TVPInvalidBPP,
	"�����ȐF�[�x�ł�");

TVP_MSG_DECL(TVPCannotLoadPlugin,
	"�w�肳�ꂽ�v���O�C�� %1 ��ǂݍ��߂܂���");

TVP_MSG_DECL(TVPNotValidPlugin,
	"%1 �͗L���ȃv���O�C���ł͂���܂���");

TVP_MSG_DECL(TVPPluginUninitFailed,
	"�v���O�C���̉���Ɏ��s���܂���");

TVP_MSG_DECL(TVPCannnotLinkPluginWhilePluginLinking,
	"�v���O�C���̐ڑ����ɑ��̃v���O�C����ڑ����邱�Ƃ͂ł��܂܂���");

TVP_MSG_DECL(TVPNotSusiePlugin,
	"�ُ�� Susie �v���O�C���ł�");

TVP_MSG_DECL(TVPSusiePluginError,
	"Susie �v���O�C���ŃG���[���������܂���/�G���[�R�[�h %1");

TVP_MSG_DECL(TVPCannotReleasePlugin,
	"�w�肳�ꂽ�v���O�C���͎g�p���̂��߉���ł��܂���");

TVP_MSG_DECL(TVPNotLoadedPlugin,
	"%1 �͓ǂݍ��܂�Ă��܂���");

TVP_MSG_DECL(TVPCannotAllocateBitmapBits,
	"�r�b�g�}�b�v�p���������m�ۂł��܂���/%1(size=%2)");

TVP_MSG_DECL(TVPScanLineRangeOver,
	"�X�L�������C�� %1 �͔͈�(0�`%2)�𒴂��Ă��܂�");

TVP_MSG_DECL(TVPPluginError,
	"�v���O�C���ŃG���[���������܂���/%1");

TVP_MSG_DECL(TVPInvalidCDDADrive,
	"�w�肳�ꂽ�h���C�u�ł� CD-DA ���Đ��ł��܂���");

TVP_MSG_DECL(TVPCDDADriveNotFound,
	"CD-DA ���Đ��ł���h���C�u��������܂���");

TVP_MSG_DECL(TVPMCIError,
	"MCI �ŃG���[���������܂��� : %1");

TVP_MSG_DECL(TVPInvalidSMF,
	"�L���� SMF �t�@�C���ł͂���܂��� : %1");

TVP_MSG_DECL(TVPMalformedMIDIMessage,
	"�w�肳�ꂽ���b�Z�[�W�� MIDI ���b�Z�[�W�Ƃ��ėL���Ȍ`���ł͂���܂���");

TVP_MSG_DECL(TVPCannotInitDirectSound,
	"DirectSound ���������ł��܂��� : %1");

TVP_MSG_DECL(TVPCannotCreateDSSecondaryBuffer,
	"DirectSound �Z�J���_���o�b�t�@���쐬�ł��܂��� : %1/%2");

TVP_MSG_DECL(TVPInvalidLoopInformation,
	"���[�v��� %1 �ُ͈�ł�");

TVP_MSG_DECL(TVPNotChildMenuItem,
	"�w�肳�ꂽ���j���[���ڂ͂��̃��j���[���ڂ̎q�ł͂���܂���");

TVP_MSG_DECL(TVPCannotInitDirectDraw,
	"DirectDraw ���������ł��܂��� : %1");

TVP_MSG_DECL(TVPCannotFindDisplayMode,
	"�K�������ʃ��[�h��������܂��� : %1");

TVP_MSG_DECL(TVPCannotSwitchToFullScreen,
	"�t���X�N���[���ɐ؂�ւ����܂��� : %1");

TVP_MSG_DECL(TVPInvalidPropertyInFullScreen,
	"�t���X�N���[�����ł͑���ł��Ȃ��v���p�e�B��ݒ肵�悤�Ƃ��܂���");

TVP_MSG_DECL(TVPInvalidMethodInFullScreen,
	"�t���X�N���[�����ł͑���ł��Ȃ����\�b�h���Ăяo�����Ƃ��܂���");

TVP_MSG_DECL(TVPCannotLoadCursor,
	"�}�E�X�J�[�\�� %1 �̓ǂݍ��݂Ɏ��s���܂���");

TVP_MSG_DECL(TVPCannotLoadKrMovieDLL,
	"�r�f�I/Shockwave Flash ���Đ����邽�߂ɂ� krmovie.dll / krflash.dll "
		"���K�v�ł��� �ǂݍ��ނ��Ƃ��ł��܂���");

TVP_MSG_DECL(TVPInvalidKrMovieDLL,
	"krmovie.dll/krflash.dll ���ُ킩 �Ή��ł��Ȃ��o�[�W�����ł�");

TVP_MSG_DECL(TVPErrorInKrMovieDLL,
	"krmovie.dll/krflash.dll ���ŃG���[���������܂���/%1");

TVP_MSG_DECL(TVPWindowAlreadyMissing,
	"�E�B���h�E�͂��łɑ��݂��Ă��܂���");

TVP_MSG_DECL(TVPPrerenderedFontMappingFailed,
	"�����_�����O�ς݃t�H���g�̃}�b�s���O�Ɏ��s���܂��� : %1");

TVP_MSG_DECL_CONST(TVPConfigFailOriginalFileCannotBeRewritten,
	"%1 �ɏ������݂ł��܂���B�\�t�g�E�F�A�����s���̂܂܂ɂȂ��Ă��Ȃ����A���邢��"
		"�������݌��������邩�ǂ������m�F���Ă�������");

TVP_MSG_DECL(TVPConfigFailTempExeNotErased,
	"%1 �̏I�����m�F�ł��Ȃ����߁A������폜�ł��܂���ł���(���̃t�@�C���͍폜���Č��\�ł�)");

TVP_MSG_DECL_CONST(TVPExecutionFail,
	"%1 �����s�ł��܂���");

TVP_MSG_DECL(TVPPluginUnboundFunctionError,
	"�v���O�C������֐� %1 ��v������܂������A���̊֐��͖{�̓��ɑ��݂��܂���B�v���O�C����"
			"�{�̂̃o�[�W�������������Ή����Ă��邩�m�F���Ă�������");

TVP_MSG_DECL(TVPExceptionHadBeenOccured,
	" = (��O����)");

TVP_MSG_DECL(TVPConsoleResult,
	"�R���\�[�� : ");


//---------------------------------------------------------------------------



#endif
