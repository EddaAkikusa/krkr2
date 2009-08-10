#include <stdio.h>
#include "ncbind/ncbind.hpp"

#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdblob.h>
#include <sqstdaux.h>
#include <sqthread.h>
#include <sqcont.h>
#include "sqtjsobj.h"

// squirrel ��ł� TJS2�̃O���[�o����Ԃ̎Q�Ɩ�
#define KIRIKIRI_GLOBAL L"krkr"
// TJS2 ��ł� squirrel �̃O���[�o����Ԃ̎Q�Ɩ�
#define SQUIRREL_GLOBAL L"sqglobal"

// �R�s�[���C�g�\�L
static const char *copyright =
"\n------ Squirrel Copyright START ------\n"
"Copyright (c) 2003-2009 Alberto Demichelis\n"
"------ Squirrel Copyright END ------";

static HSQUIRRELVM vm = NULL;

#include <tchar.h>

/**
 * ���O�o�͗p for squirrel
 */
static void printFunc(HSQUIRRELVM v, const SQChar* format, ...)
{
	va_list args;
	va_start(args, format);
	TCHAR msg[1024];
	_vsntprintf_s(msg, 1024, _TRUNCATE, format, args);
	TCHAR *p = msg;
	int c;
	int n = 0;
	while ((c = *(p+n))) {
		if (c == '\n') {
			TVPAddLog(ttstr(p,n));
			p += n;
			n = 0;
			p++;
		} else {
			n++;
		}
	}
	if (n > 0) {
		TVPAddLog(ttstr(p));
	}
	va_end(args);
}

//---------------------------------------------------------------------------
// squirrel -> TJS2 �u���b�W�p
//---------------------------------------------------------------------------

extern void sq_pushvariant(HSQUIRRELVM v, tTJSVariant &variant);
extern SQRESULT sq_getvariant(HSQUIRRELVM v, int idx, tTJSVariant *result);
extern void SQEXCEPTION(HSQUIRRELVM v);

/**
 * Squirrel �� �O���[�o����Ԃɓo�^�������s��
 */
static void registerglobal(HSQUIRRELVM v, const SQChar *name, tTJSVariant &variant)
{
	sq_pushroottable(v);
	sq_pushstring(v, name, -1);
	sq_pushvariant(v, variant);
	sq_createslot(v, -3); 
	sq_pop(v, 1);
}

/**
 * Squirrel �� �O���[�o����Ԃ���폜�������s��
 */
static void unregisterglobal(HSQUIRRELVM v, const SQChar *name)
{
	sq_pushroottable(v);
	sq_pushstring(v, name, -1);
	sq_deleteslot(v, -2, SQFalse); 
	sq_pop(v, 1);
}

//---------------------------------------------------------------------------

#include "../json/Writer.hpp"

// squirrel �\���ꗗ
static const char *reservedKeys[] = 
{
	"break",
	"case",
	"catch",
	"class",
	"clone",
	"continue",
	"const",
	"default",
	"delegate",
	"delete",
	"else",
	"enum",
	"extends",
	"for",
	"function",
	"if",
	"in",
	"local",
	"null",
	"resume",
	"return",
	"switch",
	"this",
	"throw",
	"try",
	"typeof" ,
	"while",
	"parent",
	"yield",
	"constructor",
	"vargc",
	"vargv" ,
	"instanceof",
	"true",
	"false",
	"static",
	NULL
};

#include <set>
using namespace std;

// �\���ꗗ
set<ttstr> reserved;

// �\���̓o�^
static void initReserved()
{
	const char **p = reservedKeys;
	while (*p != NULL) {
		reserved.insert(ttstr(*p));
		p++;
	}
}

// �\��ꂩ�ǂ���
static bool isReserved(const tjs_char *key)
{
	return reserved.find(ttstr(key)) != reserved.end();
}

static bool isal(int c) {
	return (c >= 'a' && c <= 'z' ||
			c >= 'A' && c <= 'Z' ||
			c == '_');
}


static bool isaln(int c) {
	return (c >= '0' && c <= '9' ||
			c >= 'a' && c <= 'z' ||
			c >= 'A' && c <= 'Z' ||
			c == '_');
}

static bool
isSimpleString(const tjs_char *str)
{
	const tjs_char *p = str;
	if (!isal(*p)) {
		return false;
	}
	while (*p != '\0') {
		if (!isaln(*p)) {
			return false;
		}
		p++;
	}
	return true;
}

static void
quoteString(const tjs_char *str, IWriter *writer)
{
	if (str) {
		writer->write((tjs_char)'"');
		const tjs_char *p = str;
		int ch;
		while ((ch = *p++)) {
			if (ch == '"') {
				writer->write(L"\\\"");
			} else if (ch == '\\') {
				writer->write(L"\\\\");
			} else {
				writer->write((tjs_char)ch);
			}
		}
		writer->write((tjs_char)'"');
	} else {
		writer->write(L"\"\"");
	}
}

static void getVariantString(tTJSVariant &var, IWriter *writer);

/**
 * �����̓��e�\���p�̌Ăяo�����W�b�N
 */
class DictMemberDispCaller : public tTJSDispatch /** EnumMembers �p */
{
protected:
	IWriter *writer;
	bool first;
public:
	DictMemberDispCaller(IWriter *writer) : writer(writer) { first = true; };
	virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
												tjs_uint32 flag,			// calling flag
												const tjs_char * membername,// member name ( NULL for a default member )
												tjs_uint32 *hint,			// hint for the member name (in/out)
												tTJSVariant *result,		// result
												tjs_int numparams,			// number of parameters
												tTJSVariant **param,		// parameters
												iTJSDispatch2 *objthis		// object as "this"
												) {
		if (numparams > 1) {
			if ((int)param[1] != TJS_HIDDENMEMBER) {
				if (first) {
					first = false;
				} else {
					writer->write((tjs_char)',');
//					writer->newline();
				}
				const tjs_char *name = param[0]->GetString();
				if (!isReserved(name) && isSimpleString(name)) {
					writer->write(name);
					writer->write((tjs_char)'=');
				} else {
					writer->write((tjs_char)'[');
					quoteString(name, writer);
					writer->write(L"]=");
				}
				getVariantString(*param[2], writer);
			}
		}
		if (result) {
			*result = true;
		}
		return TJS_S_OK;
	}
};

static void getDictString(iTJSDispatch2 *dict, IWriter *writer)
{
	writer->write((tjs_char)'{');
	//writer->addIndent();
	DictMemberDispCaller caller(writer);
	tTJSVariantClosure closure(&caller);
	dict->EnumMembers(TJS_IGNOREPROP, &closure, dict);
	//writer->delIndent();
	writer->write((tjs_char)'}');
}

// Array �N���X�����o
static iTJSDispatch2 *ArrayCountProp   = NULL;   // Array.count

// �z��̐����擾
tjs_int getArrayCount(iTJSDispatch2 *array) {
	tTJSVariant result;
	if (TJS_SUCCEEDED(ArrayCountProp->PropGet(0, NULL, NULL, &result, array))) {
		return (tjs_int)result.AsInteger();
	}
	return 0;
}

// �z�񂩂當������擾
void getArrayString(iTJSDispatch2 *array, int idx, ttstr &store)
{
	tTJSVariant result;
	if (array->PropGetByNum(TJS_IGNOREPROP, idx, &result, array) == TJS_S_OK) {
		store = result.GetString();
	}
}

static void getArrayString(iTJSDispatch2 *array, IWriter *writer)
{
	writer->write((tjs_char)'[');
	//writer->addIndent();
	tjs_int count = getArrayCount(array);
	for (tjs_int i=0; i<count; i++) {
		if (i != 0) {
			writer->write((tjs_char)',');
			writer->newline();
		}
		tTJSVariant result;
		if (array->PropGetByNum(TJS_IGNOREPROP, i, &result, array) == TJS_S_OK) {
			getVariantString(result, writer);
		}
	}
	//writer->delIndent();
	writer->write((tjs_char)']');
}

static void
getVariantString(tTJSVariant &var, IWriter *writer)
{
	switch(var.Type()) {

	case tvtVoid:
		writer->write(L"null");
		break;
		
	case tvtObject:
		{
			iTJSDispatch2 *obj = var.AsObjectNoAddRef();
			if (obj == NULL) {
				writer->write(L"null");
			} else if (obj->IsInstanceOf(TJS_IGNOREPROP,NULL,NULL,L"Array",obj) == TJS_S_TRUE) {
				getArrayString(obj, writer);
			} else {
				getDictString(obj, writer);
			}
		}
		break;
		
	case tvtString:
		quoteString(var.GetString(), writer);
		break;

	case tvtInteger:
		writer->write((tTVInteger)var);
		break;

	case tvtReal:
		writer->write((tTVReal)var);
		break;

	default:
		writer->write(L"null");
		break;
	};
}

//---------------------------------------------------------------------------
// sqyurrel �X���b�h�����p�C���^�[�t�F�[�X
//---------------------------------------------------------------------------

// Continuous Handelr �p
class SQThreadContinuous : public tTVPContinuousEventCallbackIntf {
public:
	SQThreadContinuous() {
	}

	~SQThreadContinuous() {
		TVPRemoveContinuousEventHook(this);
	}

	tjs_uint64 prevTick;
	
	void start() {
		TVPAddContinuousEventHook(this);
		prevTick = TVPGetTickCount();
	}

	void stop() {
		TVPRemoveContinuousEventHook(this);
	}
	
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick) {
		static ttstr begin(TJS_W("onSquirrelBegin"));
		static ttstr end(TJS_W("onSquirrelEnd"));
		iTJSDispatch2 *global= TVPGetScriptDispatch();

		tjs_uint64 diff = tick - prevTick;
		tTJSVariant params[2];
		params[0] = (tjs_int64)diff;
		params[1] = (tjs_int64)tick;
		TVPPostEvent(global, global, begin, 0, TVP_EPT_IMMEDIATE, 2, params);
		sqobject::Thread::main((int)diff);
		sqobject::mainContinuous();
		TVPPostEvent(global, global, end, 0, TVP_EPT_IMMEDIATE, 2, params);
		prevTick = tick;
	}
};

SQThreadContinuous sqthreadcont;

//---------------------------------------------------------------------------

/**
 * Scripts �N���X�ւ� Squirrel ���s���\�b�h�̒ǉ�
 */
class ScriptsSquirrel {

public:
	ScriptsSquirrel(){};

	/**
	 * squirrel �X�N���v�g�̓ǂݍ���
	 * @param script �X�N���v�g
	 * @return �ǂݍ��܂ꂽ�X�N���v�g
	 */
	static tjs_error TJS_INTF_METHOD load(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis) {
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		if (SQ_SUCCEEDED(sq_compilebuffer(vm, param[0]->GetString(), param[0]->AsString()->GetLength(), L"TEXT", SQTrue))) {
			sq_getvariant(vm, -1, result);
			sq_pop(vm, 1);
		} else {
			SQEXCEPTION(vm);
		}
		return TJS_S_OK;
	}


	/**
	 * squirrel �X�N���v�g�̎��s
	 * @param script �X�N���v�g
	 * @param ... ����
	 * @return ���s����
	 */
	static tjs_error TJS_INTF_METHOD exec(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis) {
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		if (SQ_SUCCEEDED(sq_compilebuffer(vm, param[0]->GetString(), param[0]->AsString()->GetLength(), L"TEXT", SQTrue))) {
			sq_pushroottable(vm); // this
			for (int i=1;i<numparams;i++) {	// ����
				sq_pushvariant(vm, *param[i]);
			}
			if (SQ_SUCCEEDED(sq_call(vm, numparams, result ? SQTrue:SQFalse, SQTrue))) {
				if (result) {
					sq_getvariant(vm, -1, result);
					sq_pop(vm, 1);
				}
				sq_pop(vm, 1);
			} else {
				sq_pop(vm, 1);
				SQEXCEPTION(vm);
			}
		} else {
			SQEXCEPTION(vm);
		}
		return TJS_S_OK;
	}

	/**
	 * squirrel �X�N���v�g�̃t�@�C������̓ǂݍ���
	 * @param filename �t�@�C����
	 * @return �ǂݍ��܂ꂽ�X�N���v�g
	 */
	static tjs_error TJS_INTF_METHOD loadStorage(tTJSVariant *result,
												 tjs_int numparams,
												 tTJSVariant **param,
												 iTJSDispatch2 *objthis) {
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		if (SQ_SUCCEEDED(sqstd_loadfile(vm, param[0]->GetString(), SQTrue))) {
			sq_getvariant(vm, -1, result);
			sq_pop(vm, 1);
		} else {
			SQEXCEPTION(vm);
		} 
		return TJS_S_OK;
	}
	
	/**
	 * squirrel �X�N���v�g�̃t�@�C������̎��s
	 * @param filename �t�@�C����
	 * @param ... ����
	 * @return ���s����
	 */
	static tjs_error TJS_INTF_METHOD execStorage(tTJSVariant *result,
												 tjs_int numparams,
												 tTJSVariant **param,
												 iTJSDispatch2 *objthis) {
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		if (SQ_SUCCEEDED(sqstd_loadfile(vm, param[0]->GetString(), SQTrue))) {
			sq_pushroottable(vm); // this
			for (int i=1;i<numparams;i++) {	// ����
				sq_pushvariant(vm, *param[i]);
			}
			if (SQ_SUCCEEDED(sq_call(vm, numparams, result ? SQTrue:SQFalse, SQTrue))) {
				if (result) {
					sq_getvariant(vm, -1, result);
					sq_pop(vm, 1);
				}
				sq_pop(vm, 1);
			} else {
				sq_pop(vm, 1);
				SQEXCEPTION(vm);
			}
		} else {
			SQEXCEPTION(vm);
		}
		return TJS_S_OK;
	}

	/**
	 * Squirrel �X�N���v�g�̃X���b�h���s�B
	 * @param text �X�N���v�g���i�[���ꂽ������
	 * @param ... ����
	 * @return Thread�I�u�W�F�N�g
	 */
	static tjs_error TJS_INTF_METHOD fork(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis) {
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		sq_pushroottable(vm); // root
		sq_pushstring(vm, SQTHREADNAME, -1);
		if (SQ_SUCCEEDED(sq_get(vm,-2))) { // class
			sq_pushroottable(vm); // ����:self(root)
			sq_pushnull(vm); // delegate
			if (SQ_SUCCEEDED(sq_compilebuffer(vm, param[0]->GetString(), param[0]->AsString()->GetLength(), L"TEXT", SQTrue))) {
				for (int i=1;i<numparams;i++) {	// ����
					sq_pushvariant(vm, *param[i]);
				}
				if (SQ_SUCCEEDED(sq_call(vm, numparams+2, SQTrue, SQTrue))) { // �R���X�g���N�^�Ăяo��
					sq_getvariant(vm, -1, result);
					sq_pop(vm, 1); // thread
				}
			} else {
				sq_pop(vm, 2); // delegate, self
			}
			sq_pop(vm, 1); // class
		}
		sq_pop(vm,1); // root
		return TJS_S_OK;
	}

	/**
	 * Squirrel �X�N���v�g�̃t�@�C������̃X���b�h���s�B
	 * @param filename �X�N���v�g���i�[���ꂽ�t�@�C��
	 * @param ... ����
	 * @return Thread�I�u�W�F�N�g
	 */
	static tjs_error TJS_INTF_METHOD forkStorage(tTJSVariant *result,
												 tjs_int numparams,
												 tTJSVariant **param,
												 iTJSDispatch2 *objthis) {
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		sq_pushroottable(vm); // root
		sq_pushstring(vm, SQTHREADNAME, -1);
		if (SQ_SUCCEEDED(sq_get(vm,-2))) { // class
			sq_pushroottable(vm); // ���� self(root)
			sq_pushnull(vm);      // ���� delegate
			sq_pushstring(vm, param[0]->GetString(), -1); // ���� func
			for (int i=1;i<numparams;i++) {	// ����
				sq_pushvariant(vm, *param[i]);
			}
			if (SQ_SUCCEEDED(sq_call(vm, numparams+2, SQTrue, SQTrue))) { // �R���X�g���N�^�Ăяo��
				sq_getvariant(vm, -1, result);
				sq_pop(vm, 1); // thread
			}
			sq_pop(vm, 1); // class
		}
		sq_pop(vm,1); // root
		return TJS_S_OK;
	}
	
	/**
	 * squirrel �O���[�o�����\�b�h�̌Ăяo��
	 * @param name ���\�b�h��
	 * @param ... ����
	 * @return ���s����
	 */
	static tjs_error TJS_INTF_METHOD call(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis) {
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		sq_pushroottable(vm);
		sq_pushstring(vm, param[0]->GetString(), -1);
		if (SQ_SUCCEEDED(sq_get(vm, -2))) { // ���[�g�e�[�u������֐����擾
			sq_pushroottable(vm); // this
			for (int i=1;i<numparams;i++) {	// ����
				sq_pushvariant(vm, *param[i]);
			}
			if (SQ_SUCCEEDED(sq_call(vm, numparams, result ? SQTrue:SQFalse, SQTrue))) {
				if (result) {
					sq_getvariant(vm, -1, result);
					sq_pop(vm, 1); // result
				}
				sq_pop(vm, 2); // func, root
			} else {
				sq_pop(vm, 2); // func, root
				SQEXCEPTION(vm);
			}
		} else {
			sq_pop(vm, 1); // root
			SQEXCEPTION(vm);
		}
		return S_OK;
	}

	
	/**
	 * squirrel �X�N���v�g�̃R���p�C������
	 * @param text �X�N���v�g���i�[���ꂽ������
	 * @store store �o�C�i���N���[�W���i�[��t�@�C��
	 * @return �G���[������܂��� void
	 */
	static tjs_error TJS_INTF_METHOD compile(tTJSVariant *result,
											 tjs_int numparams,
											 tTJSVariant **param,
											 iTJSDispatch2 *objthis) {
		if (numparams < 2) return TJS_E_BADPARAMCOUNT;
		if (SQ_SUCCEEDED(sq_compilebuffer(vm, param[0]->GetString(), param[0]->AsString()->GetLength(), L"TEXT", SQTrue))) {
			if (SQ_SUCCEEDED(sqstd_writeclosuretofile(vm, param[1]->GetString()))) {
				sq_pop(vm, 1);
			} else {
				sq_pop(vm, 1);
				SQEXCEPTION(vm);
			} 
		} else {
			SQEXCEPTION(vm);
		}
		return TJS_S_OK;
	}

	/**
	 * Squirrel �R�[�h�̃R���p�C������
	 * @param �R���p�C�����t�@�C��
	 * @store �o�C�i���N���[�W���i�[��t�@�C��
	 * @return �G���[������܂��� void
	 */
	static tjs_error TJS_INTF_METHOD compileStorage(tTJSVariant *result,
													tjs_int numparams,
													tTJSVariant **param,
													iTJSDispatch2 *objthis) {
		if (numparams < 2) return TJS_E_BADPARAMCOUNT;
		if (SQ_SUCCEEDED(sqstd_loadfile(vm, param[0]->GetString(), SQTrue))) {
			if (SQ_SUCCEEDED(sqstd_writeclosuretofile(vm, param[1]->GetString()))) {
				sq_pop(vm, 1);
			} else {
				sq_pop(vm, 1);
				SQEXCEPTION(vm);
			} 
		} else {
			SQEXCEPTION(vm);
		} 
		return TJS_S_OK;
	}
	
	/**
	 * squirrel �`���ł̃I�u�W�F�N�g�̕ۑ�
	 * @param filename �t�@�C����
	 * @param obj �I�u�W�F�N�g
	 * @param utf true �Ȃ� UTF-8 �ŏo��
	 * @param newline ���s�R�[�h 0:CRLF 1:LF
	 * @return ���s����
	 */
	static tjs_error TJS_INTF_METHOD save(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis) {
		if (numparams < 2) return TJS_E_BADPARAMCOUNT;
		IFileWriter writer(param[0]->GetString(),
						   numparams > 2 ? (int)*param[2] != 0: false,
						   numparams > 3 ? (int)*param[3] : 0
						   );
		writer.write(L"return ");
		getVariantString(*param[1], &writer);
		return TJS_S_OK;
	}

	/**
	 * squirrel �`���ŕ�����
	 * @param obj �I�u�W�F�N�g
	 * @param newline ���s�R�[�h 0:CRLF 1:LF
	 * @return ���s����
	 */
	static tjs_error TJS_INTF_METHOD toString(tTJSVariant *result,
											  tjs_int numparams,
											  tTJSVariant **param,
											  iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		if (result) {
			IStringWriter writer(numparams > 1 ? (int)*param[1] : 0);
			getVariantString(*param[0], &writer);
			*result = writer.buf;
		}
		return TJS_S_OK;
	}

	/**
	 * squirrel �̖��O��Ԃɓo�^
	 * @param name �I�u�W�F�N�g��
	 * @param obj �I�u�W�F�N�g
	 */
	static tjs_error TJS_INTF_METHOD registerSQ(tTJSVariant *result,
											  tjs_int numparams,
											  tTJSVariant **param,
											  iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		if (numparams > 1) {
			registerglobal(vm, param[0]->GetString(), *param[1]);
		} else {
			tTJSVariant var;
			TVPExecuteExpression(param[0]->GetString(), &var);
			registerglobal(vm, param[0]->GetString(), var);
		}
		return TJS_S_OK;
	}

	/**
	 * squirrel �̖��O��Ԃ���폜
	 * @param name �I�u�W�F�N�g��
	 * @param obj �I�u�W�F�N�g
	 */
	static tjs_error TJS_INTF_METHOD unregisterSQ(tTJSVariant *result,
												tjs_int numparams,
												tTJSVariant **param,
												iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		unregisterglobal(vm, param[0]->GetString());
		return TJS_S_OK;
	}

};

NCB_ATTACH_CLASS(ScriptsSquirrel, Scripts) {

	RawCallback("loadSQ",        &ScriptsSquirrel::load,        TJS_STATICMEMBER);
	RawCallback("loadStorageSQ", &ScriptsSquirrel::loadStorage, TJS_STATICMEMBER);
	RawCallback("execSQ",        &ScriptsSquirrel::exec,        TJS_STATICMEMBER);
	RawCallback("execStorageSQ", &ScriptsSquirrel::execStorage, TJS_STATICMEMBER);
	RawCallback("forkSQ",        &ScriptsSquirrel::fork,        TJS_STATICMEMBER);
	RawCallback("forkStorageSQ", &ScriptsSquirrel::forkStorage, TJS_STATICMEMBER);
	RawCallback("callSQ",        &ScriptsSquirrel::call,        TJS_STATICMEMBER);
	RawCallback("saveSQ",        &ScriptsSquirrel::save,        TJS_STATICMEMBER);
	RawCallback("toSQString",    &ScriptsSquirrel::toString,    TJS_STATICMEMBER);
	RawCallback("registerSQ",       &ScriptsSquirrel::registerSQ,      TJS_STATICMEMBER);
	RawCallback("unregisterSQ",     &ScriptsSquirrel::unregisterSQ,    TJS_STATICMEMBER);
	RawCallback("compileSQ",        &ScriptsSquirrel::compile,        TJS_STATICMEMBER);
	RawCallback("compileStorageSQ", &ScriptsSquirrel::compileStorage, TJS_STATICMEMBER);
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
		sq_resetobject(&obj);          // �n���h����������
		sq_pushroottable(vm);
		sq_pushstring(vm, name, -1);
		if (SQ_SUCCEEDED(sq_get(vm, -2))) { // ���[�g�e�[�u������֐����擾
			sq_getstackobj(vm, -1, &obj); // �ʒu-1����I�u�W�F�N�g�n���h���𓾂�
			sq_addref(vm, &obj);          // �I�u�W�F�N�g�ւ̎Q�Ƃ�ǉ�
			sq_pop(vm, 2);
		} else {
			sq_pop(vm, 1);
			SQEXCEPTION(vm);
		}
	}

	/**
	 * �f�X�g���N�^
	 */
	~SQContinuous() {
		stop();
		sq_release(vm, &obj);  
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
		sq_pushobject(vm, obj);
		sq_pushroottable(vm);
		sq_pushinteger(vm, (SQInteger)tick); // �؂�̂Č��
		if (SQ_SUCCEEDED(sq_call(vm, 2, SQFalse, SQTrue))) {
			sq_pop(vm, 1);
		} else {
			stop();
			sq_pop(vm, 1);
			SQEXCEPTION(vm);
		}
	}
};

NCB_REGISTER_CLASS(SQContinuous) {
	NCB_CONSTRUCTOR((const tjs_char *));
	NCB_METHOD(start);
	NCB_METHOD(stop);
};


/**
 * ���� squirrel �̃O���[�o���t�@���N�V�������Ăяo�����Ƃ��ł��郉�b�p�[
 * �ϊ�����������ɏ����ł���̂Ō������ǂ�
 */
class SQFunction {

protected:
	// �����I�u�W�F�N�g�Q�ƕێ��p
	HSQOBJECT obj;

public:
	/**
	 * �R���X�g���N�^
	 * @param name ����
	 */
	SQFunction(const tjs_char *name) {
		sq_resetobject(&obj);          // �n���h����������
		sq_pushroottable(vm);
		sq_pushstring(vm, name, -1);
		if (SQ_SUCCEEDED(sq_get(vm, -2))) { // ���[�g�e�[�u������֐����擾
			sq_getstackobj(vm, -1, &obj); // �ʒu-1����I�u�W�F�N�g�n���h���𓾂�
			sq_addref(vm, &obj);          // �I�u�W�F�N�g�ւ̎Q�Ƃ�ǉ�
			sq_pop(vm, 2);
		} else {
			sq_pop(vm, 1);
			SQEXCEPTION(vm);
		}
	}

	/**
	 * �f�X�g���N�^
	 */
	~SQFunction() {
		sq_release(vm, &obj);  
	}

	/**
	 * �t�@���N�V�����Ƃ��ČĂяo��
	 */
	static tjs_error TJS_INTF_METHOD call(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  SQFunction *objthis) {
		sq_pushobject(vm, objthis->obj); // func
		sq_pushroottable(vm); // this
		for (int i=0;i<numparams;i++) { // ����
			sq_pushvariant(vm, *param[i]);
		}
		if (SQ_SUCCEEDED(sq_call(vm, numparams + 1, result ? SQTrue:SQFalse, SQTrue))) {
			if (result) {
				sq_getvariant(vm, -1, result);
				sq_pop(vm, 1); // result
			}
			sq_pop(vm, 1); // func
		} else {
			sq_pop(vm, 1); // func
			SQEXCEPTION(vm);
		}
		return TJS_S_OK;
	}
};

NCB_REGISTER_CLASS(SQFunction) {
	NCB_CONSTRUCTOR((const tjs_char *));
	RawCallback("call", &SQFunction::call, 0);
};

//---------------------------------------------------------------------------

/**
 * �X�^�b�N���̕\��
 */
static SQInteger
printCallStack(HSQUIRRELVM v)
{
	sqstd_printcallstack(v);
	return 0;
}

/**
 * �o�^�����O
 */
static void PreRegistCallback()
{
	// Copyright �\��
	TVPAddImportantLog(ttstr(copyright));
	// �\���o�^
	initReserved();
	// squirrel �o�^
	vm = sqobject::init();
	
	// �o�͗p
	sq_setprintfunc(vm, printFunc);

	sq_pushroottable(vm);

	// ���̑��̊�{���C�u�����̓o�^
	sqstd_register_iolib(vm);
	sqstd_register_bloblib(vm);
	// printCallStack �̓o�^
	sq_pushstring(vm, _SC("printCallStack"), -1);
	sq_newclosure(vm, printCallStack, 0);
	sq_createslot(vm, -3);

	sq_pop(vm, 1);
	
	sqobject::registerContinuous();
	
	// �I�u�W�F�N�g�@�\������
	sqobject::Object::registerClass();
	sqobject::Thread::registerClass();
	sqobject::Thread::registerGlobal();
	TJSObject::init(vm);
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
			tTJSVariant result;
			sq_pushroottable(vm);
			sq_getvariant(vm, -1, &result);
			sq_pop(vm, 1);
			global->PropSet(TJS_MEMBERENSURE, SQUIRREL_GLOBAL, NULL, &result, global);
		}
		// Squirrel �� �O���[�o���ɋg���g���� �O���[�o����o�^����
		tTJSVariant var(global, global);
		registerglobal(vm, KIRIKIRI_GLOBAL, var);
		global->Release();
	}

	// Array.count ���擾
	{
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("Array"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		tTJSVariant val;
		if (TJS_FAILED(dispatch->PropGet(TJS_IGNOREPROP,
										 TJS_W("count"),
										 NULL,
										 &val,
										 dispatch))) {
			TVPThrowExceptionMessage(L"can't get Array.count");
		}
		ArrayCountProp = val.AsObject();
	}

	// �X���b�h�@�\������
	sqobject::Thread::init();
	sqthreadcont.start();
}

/**
 * �J�������O
 */
static void PreUnregistCallback()
{
	sqthreadcont.stop();
	sqobject::doneContinuous();
	sqobject::Thread::done();
	TJSObject::done();

	if (ArrayCountProp) {
		ArrayCountProp->Release();
		ArrayCountProp = NULL;
	}
	
	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global)	{
		// Squirrel �� �O���[�o������g���g���̃O���[�o�����폜
		unregisterglobal(vm, KIRIKIRI_GLOBAL);
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
	sqobject::done();
	TVPAddLog(L"squirrel�I��");
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_REGIST_CALLBACK(PostRegistCallback);
NCB_PRE_UNREGIST_CALLBACK(PreUnregistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
