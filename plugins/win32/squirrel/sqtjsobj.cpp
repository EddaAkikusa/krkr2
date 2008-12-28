#include <windows.h>
#include "tp_stub.h"
#include "sqobject.h"

// ���O�o�͗p
extern void printFunc(HSQUIRRELVM v, const SQChar* format, ...);

extern void sq_pushvariant(HSQUIRRELVM v, tTJSVariant &variant);
extern SQRESULT sq_getvariant(HSQUIRRELVM v, int idx, tTJSVariant *result);

// �^���
static const SQUserPointer TJSOBJTYPETAG = (SQUserPointer)"TJSOBJTYPETAG";

/**
 * �����f�[�^����N���X�o�^�p�̃X�N���v�g�𐶐����邽�߂̃I�u�W�F�N�g
 */
class ScriptCreater : public tTJSDispatch /** EnumMembers �p */
{
protected:
	// �X�N���v�g�L�^�p
	ttstr script;

	// ���\�b�h�̃R�[�h�𐶐�
	void registMethod(const tjs_char *methodName) {
		script += "function ";
		script += methodName;
		script += "(){local args=[];for(local i=0;i<vargc;i++){args.add(vargc[i]);};return tjsInvoker(\"";
		script += methodName;
		script += "\", args);}";
	}

	// �Z�b�^�[�̃R�[�h�𐶐�
	void registSetter(const tjs_char *propertyName) {
		tstring name;
		getSetterName(name, propertyName);
		script += "function ";
		script += name.c_str();
		script += "(arg){tjsSetter(\"";
		script += propertyName;
		script += "\", arg);}";
	}

	// �Q�b�^�[�̃R�[�h�𐶐�
	void registGetter(const tjs_char *propertyName) {
		tstring name;
		getSetterName(name, propertyName);
		script += "function ";
		script += name.c_str();
		script += "(){return tjsGetter(\"";
		script += propertyName;
		script += "\");}";
	}

public:

	// �R���X�g���N�^
	ScriptCreater(const tjs_char *className) {
		script += "constructor(...){local args=[];for(local i=0;i<vargc;i++){args.add(vargv[i]);};return tjsConstructor(\"";
		script += className;
		script += "\", args);}";
	};

	// �f�[�^�̎擾
	const tjs_char *getData() {
		return script.c_str();
	}

	// �f�[�^�̒����̎擾
	int getLength() {
		return script.GetLen();
	}

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
				const tjs_char *name = param[0]->GetString();
				int type = (int)*param[2];
				if (type == 0) {
					registMethod(name);
				} else {
					if ((type & 0x01)) {
						registGetter(name);
					}
					if ((type & 0x02)) {
						registSetter(name);
					}
				}
			}
		}
		if (result) {
			*result = true;
		}
		return TJS_S_OK;
	}
};

/**
 * �g���g���I�u�W�F�N�g��ێ�����N���X
 */
class TJSObject : public MyObject {

protected:
	// �����ΏۃI�u�W�F�N�g
	iTJSDispatch2 *dispatch;

protected:
	// �R���X�g���N�^
	TJSObject() : dispatch(NULL) {
	}

	// �f�X�g���N�^
	~TJSObject() {
		if (dispatch) {
			dispatch->Release();
		}
	}
	
	/**
	 * @return �X���b�h���I�u�W�F�N�g
	 */
	static TJSObject *getTJSObject(HSQUIRRELVM v, int idx) {
		SQUserPointer up;
#ifdef USEUD
		SQUserPointer typetag;
		if (SQ_SUCCEEDED(sq_getuserdata(v, idx, &up, &typetag)) &&
			(typetag == TJSOBJTYPETAG)) {
			return (TJSObject*)up;
		}
#else
		if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, TJSOBJTYPETAG))) {
			return (TJSObject*)up;
		}
#endif
		return NULL;
	}

	/**
	 * �I�u�W�F�N�g�̃����[�T
	 */
	static SQInteger release(SQUserPointer up, SQInteger size) {
		TJSObject *self = (TJSObject*)up;
#ifdef USEUD
		self->~TJSObject();
#else
		delete self;
#endif
		return SQ_OK;
	}

	/**
	 * TJS�I�u�W�F�N�g�̃R���X�g���N�^
	 * ����1 �I�u�W�F�N�g
	 * ����2 �N���X��
	 * ����3 ����
	 */
	static SQInteger tjsConstructor(HSQUIRRELVM v) {
		SQInteger result = SQ_OK;
#ifdef USEUD
		TJSObject *self = getTJSObject(v, 1);
		if (self) {
			new (self) TJSObject();
			result = sq_setreleasehook(v, 1, release);
		} else {
			result = ERROR_CREATE(v);
		}
#else
		TJSObject *self = new TJSObject();
		if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {
			sq_setreleasehook(v, 1, release);
		} else {
			delete self;
		}
#endif
		if (SQ_SUCCEEDED(result)) {
			// �N���X�𐶐�����
			tTJSVariant classObj;
			TVPExecuteExpression(getString(v,2), &classObj);
			iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
			if (dispatch) {
				// �����ϊ�
				int argc = sq_getsize(v,3);
				tTJSVariant **args = new tTJSVariant*[argc];
				for (int i=0;i<argc;i++) {
					args[i] = new tTJSVariant();
					sq_pushinteger(v, i);
					if (SQ_SUCCEEDED(sq_get(v, 3))) {
						sq_getvariant(v, -1, args[i]);
						sq_pop(v, 1);
					}
				}
				if (SUCCEEDED(dispatch->CreateNew(0, NULL, NULL, &self->dispatch, argc, args, dispatch))) {
					result = SQ_OK;
				} else {
					result = ERROR_CREATE(v);
				}
				// �����j��
				for (int i=0;i<argc;i++) {
					delete args[i];
				}
				delete[] args;
			} else {
				result = ERROR_CREATE(v);
			}
		}
		return result;
	}

	/**
	 * TJS�I�u�W�F�N�g�p�̃��\�b�h
	 * ����1 �I�u�W�F�N�g
	 * ����2 �����o��
	 * ����3 �����̔z��
	 */
	static SQInteger tjsInvoker(HSQUIRRELVM v) {
		TJSObject *self = getTJSObject(v, 1);
		if (self && self->dispatch) {
			iTJSDispatch2 *dispatch = self->dispatch;
			
			// �����ϊ�
			int argc = sq_getsize(v,3);
			tTJSVariant **args = new tTJSVariant*[argc];
			for (int i=0;i<argc;i++) {
				args[i] = new tTJSVariant();
				sq_pushinteger(v, i);
				if (SQ_SUCCEEDED(sq_get(v, 3))) {
					sq_getvariant(v, -1, args[i]);
					sq_pop(v, 1);
				}
			}
			
			// ���\�b�h�Ăяo��
			int ret = 0;
			tTJSVariant result;
			if (SUCCEEDED(dispatch->FuncCall(0, getString(v,2), NULL, &result, argc, args, dispatch))) {
				if (result.Type() != tvtVoid) {
					sq_pushvariant(v, result);
					result = 1;
				} else {
					result = 0;
				}
			} else {
				result = ERROR_CALL(v);
			}
			
			// �����j��
			for (int i=0;i<argc;i++) {
				delete args[i];
			}
			delete[] args;
			return result;
		}
		return ERROR_BADINSTANCE(v);
	}

	/**
	 * TJS�I�u�W�F�N�g�p�̃v���p�e�B�Q�b�^�[
	 * ����1 �I�u�W�F�N�g
	 * ����2 �v���p�e�B��
	 */
	static SQInteger tjsGetter(HSQUIRRELVM v) {
		TJSObject *self = getTJSObject(v, 1);
		if (self && self->dispatch) {
			iTJSDispatch2 *dispatch = self->dispatch;
			tTJSVariant result;
			if (SUCCEEDED(dispatch->PropGet(0, getString(v,2), NULL, &result, dispatch))) {
				sq_pushvariant(v, result);
				return 1;
			}
			return ERROR_NOMEMBER(v);
		}
		return ERROR_BADINSTANCE(v);
	}

	/**
	 * TJS�I�u�W�F�N�g�p�̃v���p�e�B�Z�b�^�[
	 * ����1 �I�u�W�F�N�g
	 * ����2 �v���p�e�B��
	 * ����3 �ݒ�l
	 */
	static SQInteger tjsSetter(HSQUIRRELVM v) {
		TJSObject *self = getTJSObject(v, 1);
		if (self && self->dispatch) {
			iTJSDispatch2 *dispatch = self->dispatch;
			tTJSVariant result;
			sq_getvariant(v, 3, &result);
			if (SUCCEEDED(dispatch->PropSet(TJS_MEMBERENSURE, getString(v, 2), NULL, &result, dispatch))) {
				return SQ_OK;
			}
			return ERROR_NOMEMBER(v);
		}
		return ERROR_BADINSTANCE(v);
	}




	
public:
	static void registClassInit() {
		MyObject::pushTag(TJSOBJTYPETAG);
	}

	/**
	 * �N���X�̓o�^
	 */
	static void registClass(HSQUIRRELVM v, const tjs_char *className, const tjs_char *tjsClassName, iTJSDispatch2 *methods) {

		sq_pushroottable(v); // root
		sq_pushstring(v, className, -1);
		sq_pushstring(v, OBJECTNAME, -1);
		sq_get(v,-3);
		sq_newclass(v, true); // �p������
		sq_settypetag(v, -1, TJSOBJTYPETAG);
#ifdef USEUD
		sq_setclassudsize(v, -1, sizeof TJSObject);
#endif
		// TJS�p�������\�b�h��o�^
		REGISTMETHOD(tjsConstructor);
		REGISTMETHOD(tjsInvoker);
		REGISTMETHOD(tjsSetter);
		REGISTMETHOD(tjsGetter);
		
		// �N���X�����p�X�N���v�g�𐶐�����
		ScriptCreater script(tjsClassName);
		if (methods) {
			tTJSVariantClosure closure(&script);
			methods->EnumMembers(TJS_IGNOREPROP, &closure, methods);
		}

		// �N���X�̃R���e�L�X�g�Ŏ��s
		if (SQ_SUCCEEDED(sq_compilebuffer(v, script.getData(), script.getLength(), className, SQTrue))) {
			sq_push(v, -2); // class
			if (SQ_FAILED(sq_call(v, 1, SQFalse, SQTrue))) {
				sq_getlasterror(v);
				const SQChar *str;
				sq_getstring(v, -1, &str);
				printFunc(v, str);
				sq_pop(v, 1);
			}
			sq_pop(v, 1);
		} else {
			sq_getlasterror(v);
			const SQChar *str;
			sq_getstring(v, -1, &str);
			printFunc(v, str);
			sq_pop(v, 1);
		}
	
		sq_createslot(v, -3);
		sq_pop(v, 1); // root
	}
};

void sqtjsobj_init()
{
	TJSObject::registClassInit();
}

void sqtjsobj_regist(HSQUIRRELVM v, const tjs_char *className, const tjs_char *tjsClassName, iTJSDispatch2 *methods)
{
	TJSObject::registClass(v, className, tjsClassName, methods);
};
