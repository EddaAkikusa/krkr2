#include "ncbind/ncbind.hpp"
#include "sqplus.h"

// squirrel ��ł� TJS2�̃O���[�o����Ԃ̎Q�Ɩ�
#define KIRIKIRI_GLOBAL L"krkr"
// TJS2 ��ł� squirrel �̃O���[�o����Ԃ̎Q�Ɩ�
#define SQUIRREL_GLOBAL L"sqglobal"

// �R�s�[���C�g�\�L
static const char *copyright =
"\n------ Squirrel Copyright START ------\n"
"Copyright (c) 2003-2007 Alberto Demichelis\n"
"------ Squirrel Copyright END ------";

/**
 * ���O�o�͗p for squirrel
 */
static void PrintFunc(HSQUIRRELVM v, const SQChar* format, ...)
{
	va_list args;
	va_start(args, format);
	tjs_char msg[1024];
	_vsnwprintf(msg, sizeof msg, format, args);
	TVPAddLog(msg);
	va_end(args);
}

//---------------------------------------------------------------------------
// squirrel -> TJS2 �u���b�W�p
//---------------------------------------------------------------------------

extern void store(tTJSVariant &result, SquirrelObject &variant);
extern void store(tTJSVariant &result, HSQUIRRELVM v, int idx=-1);
extern void registglobal(HSQUIRRELVM v, const SQChar *name, iTJSDispatch2 *dispatch);
extern void unregistglobal(HSQUIRRELVM v, const SQChar *name);

//---------------------------------------------------------------------------

/**
 * Scripts �N���X�ւ� Squirrel ���s���\�b�h�̒ǉ�
 */
class ScriptsSquirrel {

public:
	ScriptsSquirrel(){};

	/**
	 * squirrel �X�N���v�g�̎��s
	 * @param script �X�N���v�g
	 * @return ���s����
	 */
	static tjs_error TJS_INTF_METHOD exec(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis) {
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		try {
			SquirrelObject script = SquirrelVM::CompileBuffer(param[0]->GetString());
			SquirrelObject ret    = SquirrelVM::RunScript(script);
			if (result) {
				store(*result, ret);
			}
		} catch(SquirrelError e) {
			TVPThrowExceptionMessage(e.desc);
		}
		return S_OK;
	}

	/**
	 * squirrel �X�N���v�g�̃t�@�C������̎��s
	 * @param filename �t�@�C����
	 * @return ���s����
	 */
	static tjs_error TJS_INTF_METHOD execStorage(tTJSVariant *result,
												 tjs_int numparams,
												 tTJSVariant **param,
												 iTJSDispatch2 *objthis) {

		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		iTJSTextReadStream * stream = TVPCreateTextStreamForRead(param[0]->GetString(), TJS_W(""));
		try {
			ttstr data;
			stream->Read(data, 0);
			SquirrelObject script = SquirrelVM::CompileBuffer(data.c_str());
			SquirrelObject ret    = SquirrelVM::RunScript(script);
			if (result) {
				store(*result, ret);
			}
		} catch(SquirrelError e) {
			stream->Destruct();
			TVPThrowExceptionMessage(e.desc);
		} catch(...) {
			stream->Destruct();
			throw;
		}
		stream->Destruct();
		return S_OK;
	}
};

NCB_ATTACH_CLASS(ScriptsSquirrel, Scripts) {
	RawCallback("execSQ",        &ScriptsSquirrel::exec,        TJS_STATICMEMBER);
	RawCallback("execStorageSQ", &ScriptsSquirrel::execStorage, TJS_STATICMEMBER);
};

/**
 * Squirrel �p Continuous �n���h���N���X
 * ���� squirrel �̃��\�b�h���Ăяo�� Continuous �n���h���𐶐�����
 */
class SQContinuous : public tTVPContinuousEventCallbackIntf {

protected:
	// �����I�u�W�F�N�g�Q�ƕێ��p
	HSQOBJECT obj;

public:
	/**
	 * �R���X�g���N�^
	 * @param name ����
	 */
	SQContinuous(const tjs_char *name) {
		HSQUIRRELVM v = SquirrelVM::GetVMPtr();
		sq_resetobject(&obj);          // �n���h����������
		sq_pushroottable(v);
		sq_pushstring(v, name, -1);
		sq_get(v, -2);                   // ���[�g�e�[�u������֐����擾
		sq_getstackobj(v, -1, &obj); // �ʒu-1����I�u�W�F�N�g�n���h���𓾂�
		sq_addref(v, &obj);              // �I�u�W�F�N�g�ւ̎Q�Ƃ�ǉ�
		sq_pop(v, 2);
	}

	/**
	 * �f�X�g���N�^
	 */
	~SQContinuous() {
		stop();
		HSQUIRRELVM v = SquirrelVM::GetVMPtr();
		sq_release(v, &obj);  
	}

	/**
	 * �Ăяo���J�n
	 */
	void start() {
		stop();
		TVPAddContinuousEventHook(this);
	}

	/**
	 * �Ăяo����~
	 */
	void stop() {
		TVPRemoveContinuousEventHook(this);
	}

	/**
	 * Continuous �R�[���o�b�N
	 * �g���g�����ɂȂƂ��ɏ�ɌĂ΂��
	 */
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick) {
		HSQUIRRELVM v = SquirrelVM::GetVMPtr();
		sq_pushobject(v, obj);
		sq_pushroottable(v);
		sq_pushinteger(v, (SQInteger)tick); // �؂�̂Č��
		if (!SQ_SUCCEEDED(sq_call(v, 2, SQFalse, SQFalse))) {
			stop();
		}
		sq_pop(SquirrelVM::GetVMPtr(), 1);
	}
};

NCB_REGISTER_CLASS(SQContinuous) {
	NCB_CONSTRUCTOR((const tjs_char *));
	NCB_METHOD(start);
	NCB_METHOD(stop);
};

//---------------------------------------------------------------------------

/**
 * �o�^�����O
 */
static void PreRegistCallback()
{
	// Copyright �\��
	TVPAddImportantLog(ttstr(copyright));

	// squirrel ������
	SquirrelVM::Init();
	// squirrel printf �o�^
	sq_setprintfunc(SquirrelVM::GetVMPtr(), PrintFunc);
}

/**
 * �o�^������
 */
static void PostRegistCallback()
{
	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global) {
		// �g���g���̃O���[�o���� Squirrel �̃O���[�o����o�^����
		{
			tTJSVariant result = tTJSVariant();
			HSQUIRRELVM v = SquirrelVM::GetVMPtr();
			sq_pushroottable(v);
			store(result, v, -1);
			sq_pop(v, 1);
			global->PropSet(TJS_MEMBERENSURE, SQUIRREL_GLOBAL, NULL, &result, global);
		}
		// Squirrel �� �O���[�o���ɋg���g���� �O���[�o����o�^����
		registglobal(SquirrelVM::GetVMPtr(), KIRIKIRI_GLOBAL, global);
		global->Release();
	}
}

/**
 * �J�������O
 */
static void PreUnregistCallback()
{
	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global)	{
		// Squirrel �� �O���[�o������g���g���̃O���[�o�����폜
		unregistglobal(SquirrelVM::GetVMPtr(), KIRIKIRI_GLOBAL);
		// squirrel �� global �ւ̓o�^������
		global->DeleteMember(0, SQUIRREL_GLOBAL, NULL, global);
		global->Release();
	}
}

/**
 * �J��������
 */
static void PostUnregistCallback()
{
	// squirrel �I��
	SquirrelVM::Shutdown();
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_REGIST_CALLBACK(PostRegistCallback);
NCB_PRE_UNREGIST_CALLBACK(PreUnregistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
