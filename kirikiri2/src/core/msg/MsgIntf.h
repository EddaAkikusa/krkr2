//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

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

#ifdef __GNUC__
#  include "gtk/MsgImpl.h"
#else
#  include "win32/MsgImpl.h"
#endif

#include "svn_revision.h"

//---------------------------------------------------------------------------
// Message Strings ( these should be localized )
//---------------------------------------------------------------------------
// Japanese localized messages
TVP_MSG_DECL_CONST(TVPAboutString,
	
"��Τ��Τ[���꤭��] 2 �¹ԥ��� version %1 ( TJS version %2 )\n"
"Compiled on " __DATE__ " " __TIME__ "\n"
TVP_SVN_REVISION "\n"
"Copyright (C) 1997-2005 W.Dee All rights reserved.\n"
"��Τ��Τ�¹ԥ����λ���/����/���Ѥϡ�\n"
"SDK ��°�� license.txt �˽񤫤�Ƥ���饤���󥹤˽��äƹԤ����Ȥ��Ǥ��ޤ�.\n"
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
"�Ķ�����\n"
  // important log (environment information, Debug.notice etc.) comes here
);

TVP_MSG_DECL_CONST(TVPVersionInformation,
	
"��Τ��Τ[���꤭��] 2 �¹ԥ���/%1 "
"(SVN revision:" TVP_SVN_REVISION_NUMBER "; Compiled on " __DATE__ " " __TIME__ ") TJS2/%2 "
"Copyright (C) 1997-2005 W.Dee All rights reserved.");

TVP_MSG_DECL_CONST(TVPVersionInformation2,
	"�С���������ξܺ٤� Ctrl + F12 �Ǳ����Ǥ��ޤ�");

TVP_MSG_DECL_CONST(TVPDownloadPageURL,
	"http://kikyou.info/tvp/");

TVP_MSG_DECL(TVPInternalError,
	"�������顼��ȯ�����ޤ���: at %1 line %2");

TVP_MSG_DECL(TVPInvalidParam,
	"�����ʥѥ�᡼���Ǥ�");

TVP_MSG_DECL(TVPWarnDebugOptionEnabled,
	"-debug ���ץ���󤬻��ꤵ��Ƥ��뤿�ᡢ���� ��Τ��Τ�ϥǥХå��⡼�ɤ�ư��Ƥ��ޤ����ǥХå��⡼�ɤǤϽ�ʬ�ʼ¹�®�٤��Фʤ���礬����Τ���դ��Ƥ�������");

TVP_MSG_DECL(TVPCommandLineParamIgnoredAndDefaultUsed,
	"���ޥ�ɥ饤��ѥ�᡼�� %1 �˻��ꤵ�줿�� %2 ��̵���Τ���ǥե���Ȥ�������Ѥ��ޤ�");

TVP_MSG_DECL(TVPInvalidCommandLineParam,
	"���ޥ�ɥ饤��ѥ�᡼�� %1 �˻��ꤵ�줿�� %2 ��̵���Ǥ�");

TVP_MSG_DECL(TVPNotImplemented,
	"̤�����ε�ǽ��ƤӽФ����Ȥ��ޤ���");

TVP_MSG_DECL(TVPCannotOpenStorage,
	"���ȥ졼�� %1 �򳫤����Ȥ��Ǥ��ޤ���");

TVP_MSG_DECL(TVPCannotFindStorage,
	"���ȥ졼�� %1 �����Ĥ���ޤ���");

TVP_MSG_DECL(TVPCannotOpenStorageForWrite,
	"���ȥ졼�� %1 ��񤭹����Ѥ˳������Ȥ��Ǥ��ޤ��󡣥ե����뤬�񤭹��߶ػߤˤʤäƤ��ʤ��������뤤�ϥե�����˽񤭹��߸��¤����뤫�ɤ��������뤤�Ϥ��⤽�⤽�줬�񤭹��߲�ǽ�ʥ�ǥ�����ե�����ʤΤ����ǧ���Ƥ�������");

TVP_MSG_DECL(TVPStorageInArchiveNotFound,
	"���ȥ졼�� %1 ������������ %2 ����˸��Ĥ���ޤ���");

TVP_MSG_DECL(TVPInvalidPathName,
	"�ѥ�̾ %1 ��̵���ʷ����Ǥ������������������ɤ������ǧ���Ƥ�������");

TVP_MSG_DECL(TVPUnsupportedMediaName,
	"\"%1\" ���б����Ƥ��ʤ���ǥ��������פǤ�");

TVP_MSG_DECL(TVPCannotUnbindXP3EXE,
	"%1 �ϼ¹Բ�ǽ�ե�����˸����ޤ���������˷�礵�줿���������֤�ȯ���Ǥ��ޤ���Ǥ���");

TVP_MSG_DECL(TVPCannotFindXP3Mark,
	"%1 �� XP3 ���������֤ǤϤʤ������б��Ǥ��ʤ������Ǥ������������֥ե��������ꤹ�٤����̤��̾�Υե��������ꤷ����硢���뤤���б��Ǥ��ʤ����������֥ե��������ꤷ�����ʤɤˤ��Υ��顼��ȯ�����ޤ��Τǡ���ǧ���Ƥ�������");

TVP_MSG_DECL(TVPMissingPathDelimiterAtLast,
	"�ѥ�̾�κǸ�ˤ� '>' �ޤ��� '/' ����ꤷ�Ƥ������� (��Τ��Τ�� 2.19 beta 14 ��ꥢ�������֤ζ��ڤ국�椬 '#' ���� '>' ���Ѥ��ޤ���)");

TVP_MSG_DECL(TVPFilenameContainsSharpWarn,
	"(���) '#' ���ե�����̾ \"%1\" �˴ޤޤ�Ƥ��ޤ������������֤ζ��ڤ�ʸ���ϵ�Τ��Τ�� 2.19 beta 14 ���'#' ���� '>' ���Ѥ��ޤ�����"
	"�⤷���������֤ζ��ڤ�ʸ���ΤĤ��� '#' ����Ѥ������ϡ�������Ǥ��� '>' ���Ѥ��Ƥ�������");

TVP_MSG_DECL(TVPCannotGetLocalName,
	"���ȥ졼��̾ %1 �������ե�����̾���Ѵ��Ǥ��ޤ��󡣥��������֥ե�������Υե�����䡢������ե�����Ǥʤ��ե�����ϥ�����ե�����̾���Ѵ��Ǥ��ޤ���");

TVP_MSG_DECL(TVPReadError,
	"�ɤ߹��ߥ��顼�Ǥ����ե����뤬��»���Ƥ����ǽ���䡢�ǥХ���������ɤ߹��ߤ˼��Ԥ�����ǽ��������ޤ�");

TVP_MSG_DECL(TVPWriteError,
	"�񤭹��ߥ��顼�Ǥ�");

TVP_MSG_DECL(TVPSeekError,
	"�������˼��Ԥ��ޤ������ե����뤬��»���Ƥ����ǽ���䡢�ǥХ���������ɤ߹��ߤ˼��Ԥ�����ǽ��������ޤ�");

TVP_MSG_DECL(TVPTruncateError,
	"�ե������Ĺ�����ڤ�ͤ��Τ˼��Ԥ��ޤ���");

TVP_MSG_DECL(TVPInsufficientMemory,
	"������ݤ˼��Ԥ��ޤ�����");

TVP_MSG_DECL(TVPUncompressionFailed,
	"�ե������Ÿ���˼��Ԥ��ޤ�����̤�б��ΰ��̷��������ꤵ�줿�������뤤�ϥե����뤬��»���Ƥ����ǽ��������ޤ�");

TVP_MSG_DECL(TVPCompressionFailed,
	"�ե�����ΰ��̤˼��Ԥ��ޤ���");

TVP_MSG_DECL(TVPCannotWriteToArchive,
	"���������֤˥ǡ�����񤭹��ळ�ȤϤǤ��ޤ���");

TVP_MSG_DECL(TVPUnsupportedCipherMode,
	"%1 ��̤�б��ΰŹ沽���������ǡ�������»���Ƥ��ޤ�");

TVP_MSG_DECL(TVPUnsupportedModeString,
	"ǧ���Ǥ��ʤ��⡼��ʸ����λ���Ǥ�(%1)");

TVP_MSG_DECL(TVPUnknownGraphicFormat,
	"%1 ��̤�Τβ��������Ǥ�");

TVP_MSG_DECL(TVPCannotSuggestGraphicExtension,
	"%1 �ˤĤ���Ŭ�ڤʳ�ĥ�Ҥ���ä��ե�����򸫤Ĥ����ޤ���Ǥ���");

TVP_MSG_DECL(TVPMaskSizeMismatch,
	"�ޥ��������Υ��������ᥤ������Υ������Ȱ㤤�ޤ�");

TVP_MSG_DECL(TVPProvinceSizeMismatch,
	"�ΰ���� %1 �ϥᥤ������ȥ��������㤤�ޤ�");

TVP_MSG_DECL(TVPImageLoadError,
	"�����ɤ߹�����˥��顼��ȯ�����ޤ���/%1");

TVP_MSG_DECL(TVPJPEGLoadError,
	"JPEG �ɤ߹�����˥��顼��ȯ�����ޤ���/���顼������ %1");

TVP_MSG_DECL(TVPPNGLoadError,
	"PNG �ɤ߹�����˥��顼��ȯ�����ޤ���/%1");

TVP_MSG_DECL(TVPERILoadError,
	"ERI �ɤ߹�����˥��顼��ȯ�����ޤ���/%1");

TVP_MSG_DECL(TVPTLGLoadError,
	"TLG �ɤ߹�����˥��顼��ȯ�����ޤ���/%1");

TVP_MSG_DECL(TVPInvalidImageSaveType,
	"̵������¸���������Ǥ�(%1)");

TVP_MSG_DECL(TVPInvalidOperationFor8BPP,
	"8bpp �������Ф��ƤϹԤ��ʤ�����Ԥ����Ȥ��ޤ���");

TVP_MSG_DECL(TVPSpecifyWindow,
	"Window ���饹�Υ��֥������Ȥ���ꤷ�Ƥ�������");

TVP_MSG_DECL(TVPSpecifyLayer,
	"Layer ���饹�Υ��֥������Ȥ���ꤷ�Ƥ�������");

TVP_MSG_DECL(TVPCannotCreateEmptyLayerImage,
	"�����������β������뤤�Ͻ����� 0 �ʲ��ο������ꤹ�뤳�ȤϤǤ��ޤ���");

TVP_MSG_DECL(TVPCannotSetPrimaryInvisible,
	"�ץ饤�ޥ�쥤����ԲĻ�ˤǤ��ޤ���");

TVP_MSG_DECL(TVPCannotMovePrimary,
	"�ץ饤�ޥ�쥤��ϰ�ư�Ǥ��ޤ���");

TVP_MSG_DECL(TVPCannotSetParentSelf,
	"��ʬ���Ȥ�ƤȤ��뤳�ȤϤǤ��ޤ���");

TVP_MSG_DECL(TVPCannotMoveNextToSelfOrNotSiblings,
	"��ʬ���Ȥ������Ƥΰۤʤ�쥤�������˰�ư���뤳�ȤϤǤ��ޤ���");

TVP_MSG_DECL(TVPCannotMovePrimaryOrSiblingless,
	"�ץ饤�ޥ�쥤��䷻���̵���쥤�������˰�ư���뤳�ȤϤǤ��ޤ���");

TVP_MSG_DECL(TVPInvalidImagePosition,
	"�쥤���ΰ�˲�����̵���ΰ褬ȯ�����ޤ���");

TVP_MSG_DECL(TVPCannotSetModeToDisabledOrModal,
	"���Ǥ˥⡼����ʥ쥤��οƥ쥤�䡢���뤤���ԲĻ�/̵���ʥ쥤���⡼����ˤ��뤳�ȤϤǤ��ޤ���");

TVP_MSG_DECL(TVPNotDrawableLayerType,
	"���� type �Υ쥤��Ǥ����������ɤ߹��ߤ����������/���֤��ѹ�/�����ϤǤ��ޤ���");

TVP_MSG_DECL(TVPSourceLayerHasNoImage,
	"ž�����쥤��ϲ�������äƤ��ޤ���");

TVP_MSG_DECL(TVPUnsupportedLayerType,
	"%1 �Ϥ��� type �Υ쥤��Ǥϻ��ѤǤ��ޤ���");

TVP_MSG_DECL(TVPNotDrawableFaceType,
	"%1 �ǤϤ��� face ������Ǥ��ޤ���");

TVP_MSG_DECL(TVPCannotConvertLayerTypeUsingGivenDirection,
	"���ꤵ�줿�쥤�䥿�����Ѵ��ϤǤ��ޤ���");

TVP_MSG_DECL(TVPNegativeOpacityNotSupportedOnThisFace,
	"�����Ʃ���٤Ϥ��� face �Ǥϻ���Ǥ��ޤ���");

TVP_MSG_DECL(TVPSrcRectOutOfBitmap,
	"ž�������ӥåȥޥå׳����ΰ��ޤ�Ǥ��ޤ����������ϰϤ˼��ޤ�褦��ž��������ꤷ�Ƥ�������");

TVP_MSG_DECL(TVPBoxBlurAreaMustContainCenterPixel,
	"����֥顼���ϰϤ�ɬ��(0,0)�򤽤���˴ޤ�ɬ�פ�����ޤ���left��right��ξ���Ȥ����ο��͡����뤤��ξ���Ȥ���ο��ͤȤ�������ϤǤ��ޤ���(top��bottom���Ф��Ƥ�Ʊ��)");

TVP_MSG_DECL(TVPBoxBlurAreaMustBeSmallerThan16Million,
	"����֥顼���ϰϤ��礭�����ޤ�������֥顼���ϰϤ�1677���ʲ��Ǥ���ɬ�פ�����ޤ�");

TVP_MSG_DECL(TVPCannotChangeFocusInProcessingFocus,
	"�ե��������ѹ�������ϥե��������򿷤����ѹ����뤳�ȤϤǤ��ޤ���");

TVP_MSG_DECL(TVPWindowHasNoLayer,
	"������ɥ��˥쥤�䤬����ޤ���");

TVP_MSG_DECL(TVPWindowHasAlreadyPrimaryLayer,
	"������ɥ��ˤϤ��Ǥ˥ץ饤�ޥ�쥤�䤬����ޤ�");

TVP_MSG_DECL(TVPSpecifiedEventNeedsParameter,
	"���٥�� %1 �ˤϥѥ�᡼����ɬ�פǤ�");

TVP_MSG_DECL(TVPSpecifiedEventNeedsParameter2,
	"���٥�� %1 �ˤϥѥ�᡼�� %2 ��ɬ�פǤ�");

TVP_MSG_DECL(TVPSpecifiedEventNameIsUnknown,
	"���٥��̾ %1 ��̤�ΤΥ��٥��̾�Ǥ�");

TVP_MSG_DECL(TVPOutOfRectangle,
	"���������ꤵ��ޤ���");

TVP_MSG_DECL(TVPInvalidMethodInUpdating,
	"���̹�����Ϥ��ε�ǽ��¹ԤǤ��ޤ���");

TVP_MSG_DECL(TVPCannotCreateInstance,
	"���Υ��饹�ϥ��󥹥��󥹤�����Ǥ��ޤ���");

TVP_MSG_DECL(TVPUnknownWaveFormat,
	"%1 ���б��Ǥ��ʤ� Wave �����Ǥ�");

TVP_MSG_DECL(TVPSpecifyMenuItem,
	"MenuItem ���饹�Υ��֥������Ȥ���ꤷ�Ƥ�������");

TVP_MSG_DECL(TVPCurrentTransitionMustBeStopping,
	"���ߤΥȥ�󥸥�������ߤ����Ƥ��鿷�����ȥ�󥸥����򳫻Ϥ��Ƥ���������Ʊ���쥤����Ф���ʣ���Υȥ�󥸥�����Ʊ���˼¹Ԥ��褦�Ȥ���Ȥ��Υ��顼��ȯ�����ޤ�");

TVP_MSG_DECL(TVPTransHandlerError,
	"�ȥ�󥸥����ϥ�ɥ�ǥ��顼��ȯ�����ޤ��� : %1");

TVP_MSG_DECL(TVPTransAlreadyRegistered,
	"�ȥ�󥸥���� %1 �ϴ�����Ͽ����Ƥ��ޤ�");

TVP_MSG_DECL(TVPCannotFindTransHander,
	"�ȥ�󥸥����ϥ�ɥ� %1 �����Ĥ���ޤ���");

TVP_MSG_DECL(TVPSpecifyTransitionSource,
	"�ȥ�󥸥���󸵤���ꤷ�Ƥ�������");

TVP_MSG_DECL(TVPCannotLoadRuleGraphic,
	"�롼����� %1 ���ɤ߹��ळ�Ȥ��Ǥ��ޤ���");

TVP_MSG_DECL(TVPSpecifyOption,
	"���ץ���� %1 ����ꤷ�Ƥ�������");

TVP_MSG_DECL(TVPTransitionLayerSizeMismatch,
	"�ȥ�󥸥����(%1)�ȥȥ�󥸥������(%2)�Υ쥤��Υ����������פ��ޤ���");

TVP_MSG_DECL(TVPTransitionMutualSource,
	"�ȥ�󥸥���󸵤Υȥ�󥸥���󸵤���ʬ���ȤǤ�");

TVP_MSG_DECL(TVPHoldDestinationAlphaParameterIsNowDeprecated,
	"�ٹ� : �᥽�å� %1 �� %2 ���ܤ��Ϥ��줿 hda �ѥ�᡼���ϡ���Τ��Τ�� 2.23 beta 2 ���̵�뤵���褦�ˤʤ�ޤ���������� Layer.holdAlpha �ץ�ѥƥ����Ѥ��Ƥ���������");

TVP_MSG_DECL(TVPKAGNoLine,
	"�ɤ߹��⤦�Ȥ������ʥꥪ�ե����� %1 �϶��Ǥ�");

TVP_MSG_DECL(TVPKAGCannotOmmitFirstLabelName,
	"���ʥꥪ�ե�����κǽ�Υ�٥�̾�Ͼ�ά�Ǥ��ޤ���");

TVP_MSG_DECL(TVPKAGLabelNotFound,
	"���ʥꥪ�ե����� %1 ��˥�٥� %2 �����Ĥ���ޤ���");

TVP_MSG_DECL(TVPKAGInlineScriptNotEnd,
	"[endscript] �ޤ��� @endscript �����Ĥ���ޤ���");

TVP_MSG_DECL(TVPKAGSyntaxError,
	"������ʸˡ���顼�Ǥ���'[' �� ']' ���б���\" �� \" ���б������ڡ���������˺�졢;ʬ�ʲ��Ԥʤɤ��ǧ���Ƥ�������");

TVP_MSG_DECL(TVPKAGMacroEntityNotAvailable,
	"�ޥ�����ƥ��ƥ��ϥޥ����Ǥϻ��ѤǤ��ޤ���");

TVP_MSG_DECL(TVPKAGCallStackUnderflow,
	"return ������ call �������б����Ƥ��ޤ��� ( return ������¿�� )");

TVP_MSG_DECL(TVPKAGReturnLostSync,
	"���ʥꥪ�ե�������ѹ������ä����� return ���������֤�����Ǥ��ޤ���");

TVP_MSG_DECL(TVPKAGSpecifyKAGParser,
	"KAGParser ���饹�Υ��֥������Ȥ���ꤷ�Ƥ�������");

TVP_MSG_DECL(TVPKAGMalformedSaveData,
	"�٥ǡ������۾�Ǥ����ǡ�������»���Ƥ����ǽ��������ޤ�");

TVP_MSG_DECL(TVPLabelOrScriptInMacro,
	"��٥�� iscript �ϥޥ�����˵��ҤǤ��ޤ���");

TVP_MSG_DECL(TVPUnknownMacroName,
	"�ޥ��� \"%1\" ����Ͽ����Ƥ��ޤ���");

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
