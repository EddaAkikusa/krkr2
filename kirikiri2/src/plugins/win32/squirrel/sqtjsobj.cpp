#include <windows.h>
#include "tp_stub.h"
#include "sqobject.h"

// ���O�o�͗p
extern void printFunc(HSQUIRRELVM v, const SQChar* format, ...);

extern void sq_pushvariant(HSQUIRRELVM v, tTJSVariant &variant);
extern SQRESULT sq_getvariant(HSQUIRRELVM v, int idx, tTJSVariant *result);
extern SQRESULT ERROR_KRKR(HSQUIRRELVM v, tjs_error error);

// �^���
static const SQUserPointer TJSOBJTYPETAG = (SQUserPointer)"TJSOBJTYPETAG";

tjs_int getArrayCount(iTJSDispatch2 *array);
void getArrayString(iTJSDispatch2 *array, int idx, ttstr &store);

/**
 * �����f�[�^����N���X�o�^�p�̃X�N���v�g�𐶐����邽�߂̃I�u�W�F�N�g
 */
class ScriptCreater : public tTJSDispatch /** EnumMembers �p */
{
protected:
	// �X�N���v�g�L�^�p
	ttstr script;

	// �t�@���N�V�����̃R�[�h�𐶐�
	void registFunction(const tjs_char *functionName) {
		script += "function ";
		script += functionName;
		script += "(...){local args=[];for(local i=0;i<vargc;i++){args.append(vargv[i]);};return tjsInvoker(\"";
		script += functionName;
		script += "\", args);}";
	}

	// �v���p�e�B�̃R�[�h�𐶐�
	void registProperty(const tjs_char *propertyName) {
		tstring name;
		getSetterName(name, propertyName);
		script += "function ";
		script += name.c_str();
		script += "(arg){tjsSetter(\"";
		script += propertyName;
		script += "\", arg);}";

		getGetterName(name, propertyName);
		script += "function ";
		script += name.c_str();
		script += "(){return tjsGetter(\"";
		script += propertyName;
		script += "\");}";
	}

	// �t�@���N�V�������ǂ���
	static bool isFunction(iTJSDispatch2 *member) {
		return TJS_SUCCEEDED(member->IsInstanceOf(0, NULL, NULL, L"Function", member));
	}

	// �v���p�e�B���ǂ���
	static bool isProperty(iTJSDispatch2 *member) {
		return TJS_SUCCEEDED(member->IsInstanceOf(0, NULL, NULL, L"Property", member));
	}
	
public:

	// �R���X�g���N�^
	ScriptCreater(const tjs_char *className) {
		script += "function constructor(...){local args=[];for(local i=0;i<vargc;i++){args.append(vargv[i]);};return tjsConstructor(\"";
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

	// EnumMember�p�J��Ԃ����s��
	// param[0] �����o��
	// param[1] �t���O
	// param[2] �����o�̒l
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
			if (/**(int)param[1] != TJS_HIDDENMEMBER &&*/ param[2]->Type() == tvtObject) {
				const tjs_char *name = param[0]->GetString();
				iTJSDispatch2 *member = param[2]->AsObjectNoAddRef();
				if (isFunction(member)) {
					registFunction(name);
				} else if (isProperty(member)) {
					registProperty(name);
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

public:
	iTJSDispatch2 *getDispatch() {
		return dispatch;
	}

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
	
public:
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

protected:
	/**
	 * �I�u�W�F�N�g�̃����[�T
	 */
	static SQRESULT release(SQUserPointer up, SQInteger size) {
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
	static SQRESULT tjsConstructor(HSQUIRRELVM v) {
		SQRESULT result = SQ_OK;
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
				tjs_error error;
				if (TJS_SUCCEEDED(error = dispatch->CreateNew(0, NULL, NULL, &self->dispatch, argc, args, dispatch))) {
					result = SQ_OK;
				} else {
					result = ERROR_KRKR(v, error);
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
	static SQRESULT tjsInvoker(HSQUIRRELVM v) {
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
			tjs_error error;
			const tjs_char *methodName = getString(v,2);
			if (TJS_SUCCEEDED(error = dispatch->FuncCall(0, getString(v,2), NULL, &result, argc, args, dispatch))) {
				if (result.Type() != tvtVoid) {
					sq_pushvariant(v, result);
					result = 1;
				} else {
					result = 0;
				}
			} else {
				result = ERROR_KRKR(v, error);
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
	static SQRESULT tjsGetter(HSQUIRRELVM v) {
		TJSObject *self = getTJSObject(v, 1);
		if (self && self->dispatch) {
			iTJSDispatch2 *dispatch = self->dispatch;
			tTJSVariant result;
			tjs_error error;
			if (TJS_SUCCEEDED(error = dispatch->PropGet(0, getString(v,2), NULL, &result, dispatch))) {
				sq_pushvariant(v, result);
				return 1;
			} else {
				return ERROR_KRKR(v, error);
			}
		}
		return ERROR_BADINSTANCE(v);
	}

	/**
	 * TJS�I�u�W�F�N�g�p�̃v���p�e�B�Z�b�^�[
	 * ����1 �I�u�W�F�N�g
	 * ����2 �v���p�e�B��
	 * ����3 �ݒ�l
	 */
	static SQRESULT tjsSetter(HSQUIRRELVM v) {
		TJSObject *self = getTJSObject(v, 1);
		if (self && self->dispatch) {
			iTJSDispatch2 *dispatch = self->dispatch;
			tTJSVariant result;
			sq_getvariant(v, 3, &result);
			tjs_error error;
			if (TJS_SUCCEEDED(error = dispatch->PropSet(TJS_MEMBERENSURE, getString(v, 2), NULL, &result, dispatch))) {
				return SQ_OK;
			} else {
				return ERROR_KRKR(v, error);
			}
		}
		return ERROR_BADINSTANCE(v);
	}
	
public:
	static void registClassInit() {
		MyObject::pushTag(TJSOBJTYPETAG);
	}

	/**
	 * �N���X�̓o�^
	 * @param HSQUIRRELVM v
	 * @param className �o�^�N���X��
	 * @param tjsClassInfo �N���X��`���(TJS�̃N���X���̃��X�g)
	 */
	static void registClass(HSQUIRRELVM v, const tjs_char *className, tTJSVariant *tjsClassInfo) {
		
		ttstr tjsClassName;
		iTJSDispatch2 *classArray;
		
		if (tjsClassInfo == NULL) {
			classArray = NULL;
			tjsClassName = className;
		} else if (tjsClassInfo->Type() == tvtString) {
			classArray = NULL;
			tjsClassName = tjsClassInfo->GetString();
		} else {
			classArray = tjsClassInfo->AsObjectNoAddRef();
			getArrayString(classArray, 0, tjsClassName);
		}

		// �N���X�����p�X�N���v�g�𐶐�����
		ScriptCreater script(tjsClassName.c_str());
		tTJSVariantClosure closure(&script);

		// �e�N���X�̃��\�b�h��o�^
		if (classArray) {
			int count = getArrayCount(classArray);
			for (int i=count-1;i>0;i--) {
				ttstr className;
				getArrayString(classArray, i, className);
				{			
					tTJSVariant classObj;
					TVPExecuteExpression(className, &classObj);
					iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
					if (!dispatch || TJS_FAILED(dispatch->IsInstanceOf(0, NULL, NULL, L"Class", dispatch))) {
						TVPThrowExceptionMessage(L"not tjs class");
					}
					dispatch->EnumMembers(TJS_IGNOREPROP, &closure, dispatch);
				}
			}
		}

		// �w��N���X�̃��\�b�h��o�^
		{
			tTJSVariant classObj;
			TVPExecuteExpression(tjsClassName, &classObj);
			iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
			if (!dispatch || TJS_FAILED(dispatch->IsInstanceOf(0, NULL, NULL, L"Class", dispatch))) {
				TVPThrowExceptionMessage(L"not tjs class");
			}
			dispatch->EnumMembers(TJS_IGNOREPROP, &closure, dispatch);
		}
		
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
		
		// �N���X�̃R���e�L�X�g�Ő����X�N���v�g�����s
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

// �������p
void sqtjsobj_init()
{
	TJSObject::registClassInit();
}

// �N���X�o�^
void sqtjsobj_regist(HSQUIRRELVM v, const tjs_char *className, tTJSVariant *tjsClassInfo)
{
	TJSObject::registClass(v, className, tjsClassInfo);
};

// iTJSDispatch2* ���X�^�b�N����擾
iTJSDispatch2 *sqtjsobj_getDispatch(HSQUIRRELVM v, int idx) {
	TJSObject *obj = TJSObject::getTJSObject(v, idx);
	return obj ? obj->getDispatch() : NULL;
}
