//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Definition of Messages and Message Related Utilities
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
	"������ץȤ��㳰��ȯ�����ޤ���");

TVP_MSG_DECL(TVPHardwareExceptionRaised,
	"�ϡ��ɥ������㳰��ȯ�����ޤ���");

TVP_MSG_DECL(TVPMainCDPName,
	"������ץȥ��ǥ��� (�ᥤ��)");

TVP_MSG_DECL(TVPExceptionCDPName,
	"������ץȥ��ǥ��� (�㳰����)");

TVP_MSG_DECL(TVPCannnotLocateUIDLLForFolderSelection,
	"�ե����/���������֤�������̤�ɽ�����褦�Ȥ��ޤ����� "
		"krdevui.dll �����Ĥ���ʤ��Τ�ɽ���Ǥ��ޤ���.\n"
		"�¹Ԥ���ե����/���������֤ϥ��ޥ�ɥ饤��ΰ����Ȥ��ƻ��ꤷ�Ƥ�������");

TVP_MSG_DECL(TVPInvalidUIDLL,
	"krdevui.dll ���۾狼���С�����󤬰��פ��ޤ���");

TVP_MSG_DECL(TVPInvalidBPP,
	"̵���ʿ����٤Ǥ�");

TVP_MSG_DECL(TVPCannotLoadPlugin,
	"���ꤵ�줿�ץ饰���� %1 ���ɤ߹���ޤ���");

TVP_MSG_DECL(TVPNotValidPlugin,
	"%1 ��ͭ���ʥץ饰����ǤϤ���ޤ���");

TVP_MSG_DECL(TVPPluginUninitFailed,
	"�ץ饰����β����˼��Ԥ��ޤ���");

TVP_MSG_DECL(TVPCannnotLinkPluginWhilePluginLinking,
	"�ץ饰�������³���¾�Υץ饰�������³���뤳�ȤϤǤ��ޤޤ���");

TVP_MSG_DECL(TVPNotSusiePlugin,
	"�۾�� Susie �ץ饰����Ǥ�");

TVP_MSG_DECL(TVPSusiePluginError,
	"Susie �ץ饰����ǥ��顼��ȯ�����ޤ���/���顼������ %1");

TVP_MSG_DECL(TVPCannotReleasePlugin,
	"���ꤵ�줿�ץ饰����ϻ�����Τ�������Ǥ��ޤ���");

TVP_MSG_DECL(TVPNotLoadedPlugin,
	"%1 ���ɤ߹��ޤ�Ƥ��ޤ���");

TVP_MSG_DECL(TVPCannotAllocateBitmapBits,
	"�ӥåȥޥå��ѥ������ݤǤ��ޤ���/%1(size=%2)");

TVP_MSG_DECL(TVPScanLineRangeOver,
	"�������饤�� %1 ���ϰ�(0��%2)��Ķ���Ƥ��ޤ�");

TVP_MSG_DECL(TVPPluginError,
	"�ץ饰����ǥ��顼��ȯ�����ޤ���/%1");

TVP_MSG_DECL(TVPInvalidCDDADrive,
	"���ꤵ�줿�ɥ饤�֤Ǥ� CD-DA ������Ǥ��ޤ���");

TVP_MSG_DECL(TVPCDDADriveNotFound,
	"CD-DA ������Ǥ���ɥ饤�֤����Ĥ���ޤ���");

TVP_MSG_DECL(TVPMCIError,
	"MCI �ǥ��顼��ȯ�����ޤ��� : %1");

TVP_MSG_DECL(TVPInvalidSMF,
	"ͭ���� SMF �ե�����ǤϤ���ޤ��� : %1");

TVP_MSG_DECL(TVPMalformedMIDIMessage,
	"���ꤵ�줿��å������� MIDI ��å������Ȥ���ͭ���ʷ����ǤϤ���ޤ���");

TVP_MSG_DECL(TVPCannotInitDirectSound,
	"DirectSound �������Ǥ��ޤ��� : %1");

TVP_MSG_DECL(TVPCannotCreateDSSecondaryBuffer,
	"DirectSound ���������Хåե�������Ǥ��ޤ��� : %1/%2");

TVP_MSG_DECL(TVPInvalidLoopInformation,
	"�롼�׾��� %1 �ϰ۾�Ǥ�");

TVP_MSG_DECL(TVPNotChildMenuItem,
	"���ꤵ�줿��˥塼���ܤϤ��Υ�˥塼���ܤλҤǤϤ���ޤ���");

TVP_MSG_DECL(TVPCannotInitDirectDraw,
	"DirectDraw �������Ǥ��ޤ��� : %1");

TVP_MSG_DECL(TVPCannotFindDisplayMode,
	"Ŭ�礹����̥⡼�ɤ����Ĥ���ޤ��� : %1");

TVP_MSG_DECL(TVPCannotSwitchToFullScreen,
	"�ե륹���꡼����ڤ��ؤ����ޤ��� : %1");

TVP_MSG_DECL(TVPInvalidPropertyInFullScreen,
	"�ե륹���꡼����Ǥ����Ǥ��ʤ��ץ�ѥƥ������ꤷ�褦�Ȥ��ޤ���");

TVP_MSG_DECL(TVPInvalidMethodInFullScreen,
	"�ե륹���꡼����Ǥ����Ǥ��ʤ��᥽�åɤ�ƤӽФ����Ȥ��ޤ���");

TVP_MSG_DECL(TVPCannotLoadCursor,
	"�ޥ����������� %1 ���ɤ߹��ߤ˼��Ԥ��ޤ���");

TVP_MSG_DECL(TVPCannotLoadKrMovieDLL,
	"�ӥǥ�/Shockwave Flash ��������뤿��ˤ� krmovie.dll / krflash.dll "
		"��ɬ�פǤ��� �ɤ߹��ळ�Ȥ��Ǥ��ޤ���");

TVP_MSG_DECL(TVPInvalidKrMovieDLL,
	"krmovie.dll/krflash.dll ���۾狼 �б��Ǥ��ʤ��С������Ǥ�");

TVP_MSG_DECL(TVPErrorInKrMovieDLL,
	"krmovie.dll/krflash.dll ��ǥ��顼��ȯ�����ޤ���/%1");

TVP_MSG_DECL(TVPWindowAlreadyMissing,
	"������ɥ��Ϥ��Ǥ�¸�ߤ��Ƥ��ޤ���");

TVP_MSG_DECL(TVPPrerenderedFontMappingFailed,
	"������󥰺Ѥߥե���ȤΥޥåԥ󥰤˼��Ԥ��ޤ��� : %1");

TVP_MSG_DECL_CONST(TVPConfigFailOriginalFileCannotBeRewritten,
	"%1 �˽񤭹��ߤǤ��ޤ��󡣥��եȥ��������¹���ΤޤޤˤʤäƤ��ʤ��������뤤��"
		"�񤭹��߸��¤����뤫�ɤ������ǧ���Ƥ�������");

TVP_MSG_DECL(TVPConfigFailTempExeNotErased,
	"%1 �ν�λ���ǧ�Ǥ��ʤ����ᡢ��������Ǥ��ޤ���Ǥ���(���Υե�����Ϻ�����Ʒ빽�Ǥ�)");

TVP_MSG_DECL_CONST(TVPExecutionFail,
	"%1 ��¹ԤǤ��ޤ���");

TVP_MSG_DECL(TVPPluginUnboundFunctionError,
	"�ץ饰���󤫤�ؿ� %1 ���׵ᤵ��ޤ����������δؿ����������¸�ߤ��ޤ��󡣥ץ饰�����"
			"���ΤΥС�������������б����Ƥ��뤫��ǧ���Ƥ�������");

TVP_MSG_DECL(TVPExceptionHadBeenOccured,
	" = (�㳰ȯ��)");

TVP_MSG_DECL(TVPConsoleResult,
	"���󥽡��� : ");


//---------------------------------------------------------------------------



#endif
