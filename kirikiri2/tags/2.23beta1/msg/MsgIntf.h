//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Definition of Messages and Message Related Utilities
//---------------------------------------------------------------------------
#ifndef MsgIntfH
#define MsgIntfH

#include "tjs.h"
#include "tjsMessage.h"

#ifndef TVP_MSG_DECL
	#define TVP_MSG_DECL(name, msg) extern tTJSMessageHolder name;
	#define TVP_MSG_DECL_CONST(name, msg) extern tTJSMessageHolder name;
#endif

#include "MsgImpl.h"


//---------------------------------------------------------------------------
// Message Strings ( these should be localized )
//---------------------------------------------------------------------------
// Japanese localized messages
TVP_MSG_DECL_CONST(TVPAboutString,
	
"�g���g��[���肫��] 2 ���s�R�A version %1 ( TJS version %2 )\n"
"Compiled on " __DATE__ " " __TIME__ "\n"
"Copyright (C) 1997-2004 W.Dee All rights reserved.\n"
"�g���g�����s�R�A�̎g�p/�z�z/���ς́A\n"
"SDK �t���� license.txt �ɏ�����Ă��郉�C�Z���X�ɏ]���čs�����Ƃ��ł��܂�.\n"
"------------------------------------------------------------------------------\n"
"Thanks for many libraries, contributers and supporters not listible here.\n"
"This software is based in part on the work of Independent JPEG Group.\n"
"Regex++ Copyright (c) 1998-2003 Dr John Maddock\n"
"ERINA-Library Copyright (C) 2001 Leshade Entis, Entis-soft.\n"
"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n"
"Using \"A C-program for MT19937\"\n"
"\n"
"   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,\n"
"   All rights reserved.\n"
"\n"
"   Redistribution and use in source and binary forms, with or without\n"
"   modification, are permitted provided that the following conditions\n"
"   are met:\n"
"\n"
"     1. Redistributions of source code must retain the above copyright\n"
"        notice, this list of conditions and the following disclaimer.\n"
"\n"
"     2. Redistributions in binary form must reproduce the above copyright\n"
"        notice, this list of conditions and the following disclaimer in the\n"
"        documentation and/or other materials provided with the distribution.\n"
"\n"
"     3. The names of its contributors may not be used to endorse or promote\n"
"        products derived from this software without specific prior written\n"
"        permission.\n"
"\n"
"   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n"
"   \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n"
"   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n"
"   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR\n"
"   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\n"
"   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,\n"
"   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR\n"
"   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF\n"
"   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING\n"
"   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n"
"   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"
"------------------------------------------------------------------------------\n"
"�����\n"
  // important log (environment information, Debug.notice etc.) comes here
);

TVP_MSG_DECL_CONST(TVPVersionInformation,
	
"�g���g��[���肫��] 2 ���s�R�A version %1 [" __DATE__ " " __TIME__ "] ( TJS version %2 ) "
"Copyright (C) 1997-2004 W.Dee All rights reserved.");

TVP_MSG_DECL_CONST(TVPVersionInformation2,
	"�o�[�W�������̏ڍׂ� Ctrl + F12 �ŉ{���ł��܂�");

TVP_MSG_DECL_CONST(TVPDownloadPageURL,
	"http://kikyou.info/tvp/");

TVP_MSG_DECL(TVPInternalError,
	"�����G���[���������܂���: at %1 line %2");

TVP_MSG_DECL(TVPInvalidParam,
	"�s���ȃp�����[�^�ł�");

TVP_MSG_DECL(TVPCommandLineParamIgnoredAndDefaultUsed,
	"�R�}���h���C���p�����[�^ %1 �Ɏw�肳�ꂽ�l %2 �͖����̂��߃f�t�H���g�̐ݒ��p���܂�");

TVP_MSG_DECL(TVPInvalidCommandLineParam,
	"�R�}���h���C���p�����[�^ %1 �Ɏw�肳�ꂽ�l %2 �͖����ł�");

TVP_MSG_DECL(TVPNotImplemented,
	"�������̋@�\���Ăяo�����Ƃ��܂���");

TVP_MSG_DECL(TVPCannotOpenStorage,
	"�X�g���[�W %1 ���J�����Ƃ��ł��܂���");

TVP_MSG_DECL(TVPCannotFindStorage,
	"�X�g���[�W %1 ��������܂���");

TVP_MSG_DECL(TVPCannotOpenStorageForWrite,
	"�X�g���[�W %1 ���������ݗp�ɊJ�����Ƃ��ł��܂���");

TVP_MSG_DECL(TVPStorageInArchiveNotFound,
	"�X�g���[�W %1 ���A�[�J�C�u %2 �̒��Ɍ�����܂���");

TVP_MSG_DECL(TVPInvalidPathName,
	"�p�X�� %1 �͖����Ȍ`���ł�");

TVP_MSG_DECL(TVPUnsupportedMediaName,
	"\"%1\" �͑Ή����Ă��Ȃ����f�B�A�^�C�v�ł�");

TVP_MSG_DECL(TVPCannotUnbindXP3EXE,
	"%1 �Ɍ������ꂽ�A�[�J�C�u�𔭌��ł��܂���ł���");

TVP_MSG_DECL(TVPCannotFindXP3Mark,
	"%1 �� XP3 �A�[�J�C�u�ł͂Ȃ����A�Ή��ł��Ȃ��`���ł�");

TVP_MSG_DECL(TVPMissingPathDelimiterAtLast,
	"�p�X���̍Ō�ɂ� '>' �܂��� '/' ���w�肵�Ă������� (�g���g���Q 2.19 beta 14 ���A�[�J�C�u�̋�؂�L���� '#' ���� '>' �ɕς��܂���)");

TVP_MSG_DECL(TVPFilenameContainsSharpWarn,
	"(����) '#' ���t�@�C���� \"%1\" �Ɋ܂܂�Ă��܂��B�A�[�J�C�u�̋�؂蕶���͋g���g���Q 2.19 beta 14 ���'#' ���� '>' �ɕς��܂����B"
	"�����A�[�J�C�u�̋�؂蕶���̂���� '#' ���g�p�����ꍇ�́A���萔�ł��� '>' �ɕς��Ă�������");

TVP_MSG_DECL(TVPCannotGetLocalName,
	"�X�g���[�W�� %1 �����[�J���t�@�C�����ɕϊ��ł��܂���");

TVP_MSG_DECL(TVPReadError,
	"�ǂݍ��݃G���[�ł�");

TVP_MSG_DECL(TVPWriteError,
	"�������݃G���[�ł�");

TVP_MSG_DECL(TVPSeekError,
	"�V�[�N�Ɏ��s���܂���");

TVP_MSG_DECL(TVPTruncateError,
	"�t�@�C���̒�����؂�l�߂�̂Ɏ��s���܂���");

TVP_MSG_DECL(TVPInsufficientMemory,
	"�������m�ۂɎ��s���܂���");

TVP_MSG_DECL(TVPUncompressionFailed,
	"�t�@�C���̓W�J�Ɏ��s���܂���");

TVP_MSG_DECL(TVPCompressionFailed,
	"�t�@�C���̈��k�Ɏ��s���܂���");

TVP_MSG_DECL(TVPCannotWriteToArchive,
	"�A�[�J�C�u�Ƀf�[�^���������ނ��Ƃ͂ł��܂���");

TVP_MSG_DECL(TVPUnsupportedCipherMode,
	"%1 �͖��Ή��̈Í����`�����A�f�[�^�����Ă��܂�");

TVP_MSG_DECL(TVPUnsupportedModeString,
	"�F���ł��Ȃ����[�h������̎w��ł�(%1)");

TVP_MSG_DECL(TVPUnknownGraphicFormat,
	"%1 �͖��m�̉摜�`���ł�");

TVP_MSG_DECL(TVPCannotSuggestGraphicExtension,
	"%1 �ɂ��ēK�؂Ȋg���q���������t�@�C�����������܂���ł���");

TVP_MSG_DECL(TVPMaskSizeMismatch,
	"�}�X�N�摜�̃T�C�Y�����C���摜�̃T�C�Y�ƈႢ�܂�");

TVP_MSG_DECL(TVPProvinceSizeMismatch,
	"�̈�摜 %1 �̓��C���摜�ƃT�C�Y���Ⴂ�܂�");

TVP_MSG_DECL(TVPImageLoadError,
	"�摜�ǂݍ��ݒ��ɃG���[���������܂���/%1");

TVP_MSG_DECL(TVPJPEGLoadError,
	"JPEG �ǂݍ��ݒ��ɃG���[���������܂���/�G���[�R�[�h %1");

TVP_MSG_DECL(TVPPNGLoadError,
	"PNG �ǂݍ��ݒ��ɃG���[���������܂���/%1");

TVP_MSG_DECL(TVPERILoadError,
	"ERI �ǂݍ��ݒ��ɃG���[���������܂���/%1");

TVP_MSG_DECL(TVPTLGLoadError,
	"TLG �ǂݍ��ݒ��ɃG���[���������܂���/%1");

TVP_MSG_DECL(TVPInvalidImageSaveType,
	"�����ȕۑ��摜�`���ł�(%1)");

TVP_MSG_DECL(TVPInvalidOperationFor8BPP,
	"8bpp �摜�ɑ΂��Ă͍s���Ȃ�������s�����Ƃ��܂���");

TVP_MSG_DECL(TVPSpecifyWindow,
	"Window �N���X�̃I�u�W�F�N�g���w�肵�Ă�������");

TVP_MSG_DECL(TVPSpecifyLayer,
	"Layer �N���X�̃I�u�W�F�N�g���w�肵�Ă�������");

TVP_MSG_DECL(TVPCannotCreateEmptyLayerImage,
	"�摜�T�C�Y�̉������邢�͏c���� 0 �ȉ��̐��ɐݒ肷�邱�Ƃ͂ł��܂���");

TVP_MSG_DECL(TVPCannotSetPrimaryInvisible,
	"�v���C�}�����C���͕s���ɂł��܂���");

TVP_MSG_DECL(TVPCannotMovePrimary,
	"�v���C�}�����C���͈ړ��ł��܂���");

TVP_MSG_DECL(TVPCannotSetParentSelf,
	"�������g��e�Ƃ��邱�Ƃ͂ł��܂���");

TVP_MSG_DECL(TVPCannotMoveNextToSelfOrNotSiblings,
	"�������g�̑O���e�̈قȂ郌�C���̑O��Ɉړ����邱�Ƃ͂ł��܂���");

TVP_MSG_DECL(TVPCannotMovePrimaryOrSiblingless,
	"�v���C�}�����C����Z��̖������C���͑O��Ɉړ����邱�Ƃ͂ł��܂���");

TVP_MSG_DECL(TVPInvalidImagePosition,
	"���C���̈�ɉ摜�̖����̈悪�������܂���");

TVP_MSG_DECL(TVPCannotSetModeToDisabledOrModal,
	"���łɃ��[�_���ȃ��C���̐e���C���A���邢�͕s��/�����ȃ��C�������[�_���ɂ��邱�Ƃ͂ł��܂���");

TVP_MSG_DECL(TVPNotDrawableLayerType,
	"���� type �̃��C���ł͕`���摜�ǂݍ��݂�摜�T�C�Y/�ʒu�̕ύX/�擾�͂ł��܂���");

TVP_MSG_DECL(TVPSourceLayerHasNoImage,
	"�]�������C���͉摜�������Ă��܂���");

TVP_MSG_DECL(TVPUnsupportedLayerType,
	"%1 �͂��� type �̃��C���ł͎g�p�ł��܂���");

TVP_MSG_DECL(TVPNotDrawableFaceType,
	"%1 �ł͂��� face �ɕ`��ł��܂���");

TVP_MSG_DECL(TVPSrcRectOutOfBitmap,
	"�]�������r�b�g�}�b�v�O�̗̈���܂�ł��܂�");

TVP_MSG_DECL(TVPCannotChangeFocusInProcessingFocus,
	"�t�H�[�J�X�ύX�������̓t�H�[�J�X��V���ɕύX���邱�Ƃ͂ł��܂���");

TVP_MSG_DECL(TVPWindowHasNoLayer,
	"�E�B���h�E�Ƀ��C��������܂���");

TVP_MSG_DECL(TVPWindowHasAlreadyPrimaryLayer,
	"�E�B���h�E�ɂ͂��łɃv���C�}�����C��������܂�");

TVP_MSG_DECL(TVPSpecifiedEventNeedsParameter,
	"�C�x���g %1 �ɂ̓p�����[�^���K�v�ł�");

TVP_MSG_DECL(TVPSpecifiedEventNeedsParameter2,
	"�C�x���g %1 �ɂ̓p�����[�^ %2 ���K�v�ł�");

TVP_MSG_DECL(TVPSpecifiedEventNameIsUnknown,
	"�C�x���g�� %1 �͖��m�̃C�x���g���ł�");

TVP_MSG_DECL(TVPOutOfRectangle,
	"��`�O���w�肳��܂���");

TVP_MSG_DECL(TVPInvalidMethodInUpdating,
	"��ʍX�V���͂��̋@�\�����s�ł��܂���");

TVP_MSG_DECL(TVPCannotCreateInstance,
	"���̃N���X�̓C���X�^���X���쐬�ł��܂���");

TVP_MSG_DECL(TVPUnknownWaveFormat,
	"%1 �͑Ή��ł��Ȃ� Wave �`���ł�");

TVP_MSG_DECL(TVPSpecifyMenuItem,
	"MenuItem �N���X�̃I�u�W�F�N�g���w�肵�Ă�������");

TVP_MSG_DECL(TVPCurrentTransitionMustBeStopping,
	"���݂̃g�����W�V�������~�����Ă���V�����g�����W�V�������J�n���Ă�������");

TVP_MSG_DECL(TVPTransHandlerError,
	"�g�����W�V�����n���h���ŃG���[���������܂��� : %1");

TVP_MSG_DECL(TVPTransAlreadyRegistered,
	"�g�����W�V���� %1 �͊��ɓo�^����Ă��܂�");

TVP_MSG_DECL(TVPCannotFindTransHander,
	"�g�����W�V�����n���h�� %1 ��������܂���");

TVP_MSG_DECL(TVPSpecifyTransitionSource,
	"�g�����W�V���������w�肵�Ă�������");

TVP_MSG_DECL(TVPCannotLoadRuleGraphic,
	"���[���摜 %1 ��ǂݍ��ނ��Ƃ��ł��܂���");

TVP_MSG_DECL(TVPSpecifyOption,
	"�I�v�V���� %1 ���w�肵�Ă�������");

TVP_MSG_DECL(TVPTransitionLayerSizeMismatch,
	"�g�����W�V������(%1)�ƃg�����W�V������(%2)�̃��C���̃T�C�Y����v���܂���");

TVP_MSG_DECL(TVPTransitionMutualSource,
	"�g�����W�V�������̃g�����W�V���������������g�ł�");

TVP_MSG_DECL(TVPKAGNoLine,
	"�ǂݍ������Ƃ����V�i���I�t�@�C�� %1 �͋�ł�");

TVP_MSG_DECL(TVPKAGCannotOmmitFirstLabelName,
	"�V�i���I�t�@�C���̍ŏ��̃��x�����͏ȗ��ł��܂���");

TVP_MSG_DECL(TVPKAGLabelNotFound,
	"�V�i���I�t�@�C�� %1 ���Ƀ��x�� %2 ��������܂���");

TVP_MSG_DECL(TVPKAGInlineScriptNotEnd,
	"[endscript] �܂��� @endscript ��������܂���");

TVP_MSG_DECL(TVPKAGSyntaxError,
	"�^�O�̕��@�G���[�ł�");

TVP_MSG_DECL(TVPKAGMacroEntityNotAvailable,
	"�}�N���G���e�B�e�B�̓}�N���O�ł͎g�p�ł��܂���");

TVP_MSG_DECL(TVPKAGCallStackUnderflow,
	"return �^�O�� call �^�O�ƑΉ����Ă��܂��� ( return �^�O������ )");

TVP_MSG_DECL(TVPKAGReturnLostSync,
	"�V�i���I�t�@�C���ɕύX������������ return �̖߂��ʒu�����ł��܂���");

TVP_MSG_DECL(TVPKAGSpecifyKAGParser,
	"KAGParser �N���X�̃I�u�W�F�N�g���w�肵�Ă�������");

TVP_MSG_DECL(TVPKAGMalformedSaveData,
	"�x�f�[�^���ُ�ł�");

TVP_MSG_DECL(TVPLabelOrScriptInMacro,
	"���x���� iscript �̓}�N�����ɋL�q�ł��܂���");

TVP_MSG_DECL(TVPUnknownMacroName,
	"�}�N�� \"%1\" �͓o�^����Ă��܂���");

TVP_MSG_DECL(TVPWindowHasAlreadyVideoOverlayObject,
	"�E�B���h�E�͂��łɃr�f�I�I�u�W�F�N�g�������Ă��܂�");

//---------------------------------------------------------------------------
// Utility Functions
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(ttstr, TVPFormatMessage, (const tjs_char *msg, const ttstr & p1));
TJS_EXP_FUNC_DEF(ttstr, TVPFormatMessage, (const tjs_char *msg, const ttstr & p1,
	const ttstr & p2));
TJS_EXP_FUNC_DEF(void, TVPThrowExceptionMessage, (const tjs_char *msg));
TJS_EXP_FUNC_DEF(void, TVPThrowExceptionMessage, (const tjs_char *msg,
	const ttstr &p1, tjs_int num));
TJS_EXP_FUNC_DEF(void, TVPThrowExceptionMessage, (const tjs_char *msg, const ttstr &p1));
TJS_EXP_FUNC_DEF(void, TVPThrowExceptionMessage, (const tjs_char *msg,
	const ttstr & p1, const ttstr & p2));

TJS_EXP_FUNC_DEF(ttstr, TVPGetAboutString, ());
TJS_EXP_FUNC_DEF(ttstr, TVPGetVersionInformation, ());
TJS_EXP_FUNC_DEF(ttstr, TVPGetVersionString, ());

#define TVPThrowInternalError \
	TVPThrowExceptionMessage(TVPInternalError, TJS_W("" __FILE__),  __LINE__)
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// version retrieving
//---------------------------------------------------------------------------
extern tjs_int TVPVersionMajor;
extern tjs_int TVPVersionMinor;
extern tjs_int TVPVersionRelease;
extern tjs_int TVPVersionBuild;
//---------------------------------------------------------------------------
extern void TVPGetVersion();
/*
	implement in each platforms;
	fill these four version field.
*/
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(void, TVPGetSystemVersion, (tjs_int &major, tjs_int &minor,
	tjs_int &release, tjs_int &build));
TJS_EXP_FUNC_DEF(void, TVPGetTJSVersion, (tjs_int &major, tjs_int &minor,
	tjs_int &release));
//---------------------------------------------------------------------------


#endif
