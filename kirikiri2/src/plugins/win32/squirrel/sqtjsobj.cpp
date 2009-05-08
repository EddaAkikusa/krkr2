#include <windows.h>
#include "tp_stub.h"
#include "sqtjsobj.h"

// sqwrapper.cpp ���
extern void sq_pushvariant(HSQUIRRELVM v, tTJSVariant &variant);
extern SQRESULT sq_getvariant(HSQUIRRELVM v, int idx, tTJSVariant *result);
extern SQRESULT ERROR_KRKR(HSQUIRRELVM v, tjs_error error);

// sqfunc.cpp ���
extern SQRESULT ERROR_CREATE(HSQUIRRELVM v);
extern SQRESULT ERROR_BADINSTANCE(HSQUIRRELVM v);
void registerInherit(const SQChar *typeName, const SQChar *parentName);
void registerTypeTag(const SQChar *typeName, SQUserPointer tag);
SQUserPointer getInstance(HSQUIRRELVM v, int idx, const SQChar *typeName);

// Main.cpp ���
tjs_int getArrayCount(iTJSDispatch2 *array);
void getArrayString(iTJSDispatch2 *array, int idx, ttstr &store);

// �^���
static const SQChar *typeName = _SC("TJSObject"); ///< �^��
static const SQUserPointer TJSOBJTYPETAG = (SQUserPointer)_SC("TJSOBJTYPETAG"); ///< squirrel�^�^�O
static const SQChar *tjsClassAttrName = _SC("tjsClass");

/**
 * �����o�o�^�����p
 */
class MemberRegister : public tTJSDispatch /** EnumMembers �p */
{

public:
	// �R���X�g���N�^
	MemberRegister(HSQUIRRELVM v, tTJSVariant &tjsClassObj) : v(v), tjsClassObj(tjsClassObj) {
	};

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
				tTVInteger flag = param[1]->AsInteger();
				bool staticMember = (flag & TJS_STATICMEMBER) != 0;
				iTJSDispatch2 *member = param[2]->AsObjectNoAddRef();
				if (member) {
					if (isFunction(member)) {
						registFunction(name, staticMember);
					} else if (isProperty(member)) {
						registProperty(name, staticMember);
					}
				} else {
					registNull(name);
				}
			}
		}
		if (result) {
			*result = true;
		}
		return TJS_S_OK;
	}

protected:

	// null �o�^
	void registNull(const tjs_char *functionName) {
		sq_pushstring(v, functionName, -1);
		sq_pushnull(v);
		sq_createslot(v, -3);
	}

	// �t�@���N�V�����o�^
	void registFunction(const tjs_char *functionName, bool staticMember) {
		sq_pushstring(v, functionName, -1);
		sq_pushstring(v, functionName, -1);
		if (staticMember) {
			sq_pushvariant(v, tjsClassObj);
			sq_newclosure(v, TJSObject::tjsStaticInvoker, 2);
		} else {
			sq_newclosure(v, TJSObject::tjsInvoker, 1);
		}
		sq_createslot(v, -3);
	}

	// �v���p�e�B�o�^
	void registProperty(const tjs_char *propertyName, bool staticMember) {

		ttstr name = L"set";
		name += toupper(*propertyName);
		name += (propertyName + 1);
		sq_pushstring(v, name.c_str(), -1);
		sq_pushstring(v, propertyName, -1);
		if (staticMember) {
			sq_pushvariant(v, tjsClassObj);
			sq_newclosure(v, TJSObject::tjsStaticSetter, 2);
		} else {
			sq_newclosure(v, TJSObject::tjsSetter, 1);
		}
		sq_createslot(v, -3);

		name = L"get";
		name += toupper(*propertyName);
		name += (propertyName + 1);
		sq_pushstring(v, name.c_str(), -1);
		sq_pushstring(v, propertyName, -1);
		if (staticMember) {
			sq_pushvariant(v, tjsClassObj);
			sq_newclosure(v, TJSObject::tjsStaticGetter, 2);
		} else {
			sq_newclosure(v, TJSObject::tjsGetter, 1);
		}
		sq_createslot(v, -3);
	}

	// �t�@���N�V�������ǂ���
	static bool isFunction(iTJSDispatch2 *member) {
		return TJS_SUCCEEDED(member->IsInstanceOf(0, NULL, NULL, L"Function", member));
	}

	// �v���p�e�B���ǂ���
	static bool isProperty(iTJSDispatch2 *member) {
		return TJS_SUCCEEDED(member->IsInstanceOf(0, NULL, NULL, L"Property", member));
	}
private:
	HSQUIRRELVM v;
	tTJSVariant &tjsClassObj;
};

// -----------------------------------------------------------------------

sqobject::ObjectInfo TJSObject::classMap;

// �������p
void
TJSObject::init(HSQUIRRELVM vm)
{
	classMap.initTable();
	registerInherit(typeName, SQOBJECTNAME);
	registerTypeTag(typeName, TJSOBJTYPETAG);
	
	sq_pushroottable(vm);
	sq_pushstring(vm, _SC("createClass"), -1);
	sq_newclosure(vm, createClass, 0);
	sq_createslot(vm, -3);
	sq_pop(vm, 1);
}

// �������p
void
TJSObject::done()
{
	classMap.clear();
}

/**
 * �N���X�̓o�^
 * @param HSQUIRRELVM v
 */
SQRESULT
TJSObject::createClass(HSQUIRRELVM v)
{
	int top = sq_gettop(v);
	if (top < 2) {
		return sq_throwerror(v, _SC("invalid param"));
	}
	
	// �N���X�𐶐�
	sq_pushstring(v, SQOBJECTNAME, -1);
	sq_get(v,-3);
	sq_newclass(v, true); // �p������
	sq_settypetag(v, -1, TJSOBJTYPETAG);

	top = sq_gettop(v);

	// �����o�o�^
	const tjs_char *tjsClassName = NULL;
	tTJSVariant tjsClassObj;
	for (int i=top;i>1;i--) {
		if (SQ_SUCCEEDED(sq_getstring(v, i, &tjsClassName))) {
			TVPExecuteExpression(tjsClassName, &tjsClassObj);
			iTJSDispatch2 *dispatch = tjsClassObj.AsObjectNoAddRef();
			if (dispatch && TJS_SUCCEEDED(dispatch->IsInstanceOf(0, NULL, NULL, L"Class", dispatch))) {
				MemberRegister r(v, tjsClassObj);
				tTJSVariantClosure closure(&r);
				dispatch->EnumMembers(TJS_IGNOREPROP, &closure, dispatch);
			}
		}
	}

	top = sq_gettop(v);

	if (tjsClassName) {
		// �R���X�g���N�^�o�^
		sq_pushstring(v, _SC("constructor"), -1);
		sq_pushstring(v, tjsClassName, -1);
		sq_newclosure(v, tjsConstructor, 1);
		sq_createslot(v, -3);
		// �N���X������ tjs�N���X��o�^
		sq_pushnull(v);
		sq_newtable(v);
		if (SQ_SUCCEEDED(sq_setattributes(v,-3))) {
			sq_pop(v,1);
			sq_pushnull(v);
			if (SQ_SUCCEEDED(sq_getattributes(v, -2))) {
				top = sq_gettop(v);
				sq_pushstring(v, tjsClassAttrName, -1);
				sq_pushvariant(v, tjsClassObj);
				top = sq_gettop(v);
				if (SQ_SUCCEEDED(sq_createslot(v, -3))) {
					sq_pop(v,1);
				} else {
					sq_pop(v,2);
				}
			} else {
				// XXX
				sq_pop(v,1);
			}
		} else {
			// XXX
			sq_pop(v,2);
		}
		top = sq_gettop(v);
		// �쐬�����N���X���� tjs�N���X���Ƃ��킹�ċL�^
		classMap.create(tjsClassName, sqobject::ObjectInfo(v,-1));
	}

	top = sq_gettop(v);

	return 1;
}

/**
 * squirrel ����g���g���I�u�W�F�N�g���擾
 */
iTJSDispatch2 *
TJSObject::getDispatch(HSQUIRRELVM v, int idx)
{
	if (sq_gettype(v, idx) == OT_CLASS) {
		if (idx < 0) {
			idx = sq_gettop(v) + 1 + idx;
		}
		iTJSDispatch2 *dispatch = NULL;
		// �N���X��������I�u�W�F�N�g���������o��
		sq_pushnull(v);
		if (SQ_SUCCEEDED(sq_getattributes(v, idx))) {
			sq_pushstring(v, tjsClassAttrName, -1);
			if (SQ_SUCCEEDED(sq_get(v,-2))) {
				tTJSVariant tjsClassObj;
				if (SQ_SUCCEEDED(sq_getvariant(v,-1, &tjsClassObj))) {
					dispatch = tjsClassObj.AsObjectNoAddRef();
				}
				sq_pop(v,1);
			}
			sq_pop(v,1);
		} else {
			// XXX
			sq_pop(v,1);
		}
		return dispatch;
	} else if (sq_gettype(v, idx) == OT_INSTANCE) {
		TJSObject *obj = (TJSObject*)::getInstance(v, idx, typeName);
		return obj ? obj->dispatch : NULL;
	}
	return NULL;
}

/**
 * �g���g���I�u�W�F�N�g�� squirrel �ɓo�^�B
 * �o�^�ς݃N���X�̏ꍇ�͂��̃N���X�̃C���X�^���X�Ƃ��Ă��ݏグ��
 * @return �o�^����
 */
bool
TJSObject::pushDispatch(HSQUIRRELVM v, iTJSDispatch2 *dispatch)
{
	// �C���X�^���X����N���X�����擾
	tTJSVariant val;
	if (TJS_SUCCEEDED(dispatch->ClassInstanceInfo(TJS_CII_GET, 0, &val))) {
		const tjs_char *className = val.GetString();
		// ���v����N���X�����o�^�ς݂��H
		sqobject::ObjectInfo info = classMap.get(className);
		if (info.isClass()) {
			// �N���X�� push
			info.push(v);
			if (SQ_SUCCEEDED(sq_createinstance(v, -1))) {
				// �C���X�^���X�𐶐�
				TJSObject *obj = new TJSObject(v, dispatch);
				if (SQ_SUCCEEDED(sq_setinstanceup(v, -1, obj))) {
					sq_setreleasehook(v, -1, release);
					sq_remove(v, -2);
					return true;
				} else {
					sq_pop(v,1);
					obj->dispatch = NULL;
					delete obj;
				}
			}
			sq_pop(v,1);
		}
	}
	return false;
}

// -----------------------------------------------------------------------

// �R���X�g���N�^
TJSObject::TJSObject(HSQUIRRELVM v) : dispatch(NULL)
{
	initSelf(v);
}

// �R���X�g���N�^�B�l��Ԃ��ꍇ�p
TJSObject::TJSObject(HSQUIRRELVM v, iTJSDispatch2 *dispatch) : dispatch(dispatch)
{
	initSelf(v, -1);
}

// �f�X�g���N�^
TJSObject::~TJSObject()
{
	if (dispatch) {
		dispatch->Release();
	}
}

/**
 * �I�u�W�F�N�g�̃����[�T
 */
SQRESULT
TJSObject::release(SQUserPointer up, SQInteger size)
{
	TJSObject *self = (TJSObject*)up;
	delete self;
	return SQ_OK;
}

/**
 * TJS�I�u�W�F�N�g�̃R���X�g���N�^
 * ����1 �I�u�W�F�N�g
 * ����2�` ����
 * ���R�ϐ�1 �N���X��
 */
SQRESULT
TJSObject::tjsConstructor(HSQUIRRELVM v)
{
	SQRESULT result = SQ_OK;
	TJSObject *self = new TJSObject(v);
	if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {
		sq_setreleasehook(v, 1, release);
	} else {
		delete self;
	}
	if (SQ_SUCCEEDED(result)) {
		
		// �N���X�𐶐�����
		tTJSVariant tjsClassObj;
		TVPExecuteExpression(sqobject::getString(v,-1), &tjsClassObj);
		iTJSDispatch2 *dispatch = tjsClassObj.AsObjectNoAddRef();
		if (dispatch) {
			// �����ϊ�
			int argc = sq_gettop(v) - 2;
			tTJSVariant **args = NULL;
			if (argc > 0) {
				args = new tTJSVariant*[argc];
				for (int i=0;i<argc;i++) {
					args[i] = new tTJSVariant();
					sq_getvariant(v, 2+i, args[i]);
				}
			}
			tjs_error error;
			if (TJS_SUCCEEDED(error = dispatch->CreateNew(0, NULL, NULL, &self->dispatch, argc, args, dispatch))) {
				result = SQ_OK;
			} else {
				result = ERROR_KRKR(v, error);
			}
			// �����j��
			if (args) {
				for (int i=0;i<argc;i++) {
					delete args[i];
				}
				delete[] args;
			}
		} else {
			result = ERROR_CREATE(v);
		}
	}
	return result;
}

/**
 * TJS�I�u�W�F�N�g�p�̃��\�b�h
 * ����1 �I�u�W�F�N�g
 * ����2�` ����
 * ���R�ϐ�1 �����o��
 */
SQRESULT
TJSObject::tjsInvoker(HSQUIRRELVM v)
{
	int top = sq_gettop(v);
	iTJSDispatch2 *dispatch = getDispatch(v, 1);
	if (dispatch) {

		// �����ϊ�
		int argc = sq_gettop(v) - 2;
		tTJSVariant **args = NULL;
		if (argc > 0) {
			args = new tTJSVariant*[argc];
			for (int i=0;i<argc;i++) {
				args[i] = new tTJSVariant();
				sq_getvariant(v, 2+i, args[i]);
			}
		}
		
		// ���\�b�h�Ăяo��
		int ret = 0;
		tTJSVariant result;
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->FuncCall(0, sqobject::getString(v,-1), NULL, &result, argc, args, dispatch))) {
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
		if (args) {
			for (int i=0;i<argc;i++) {
				delete args[i];
			}
			delete[] args;
		}
		return result;
	}
	return ERROR_BADINSTANCE(v);
}

/**
 * TJS�I�u�W�F�N�g�p�̃v���p�e�B�Q�b�^�[
 * ����1 �I�u�W�F�N�g
 * ���R�ϐ�1 �v���p�e�B��
 */
SQRESULT
TJSObject::tjsGetter(HSQUIRRELVM v)
{
	iTJSDispatch2 *dispatch = getDispatch(v, 1);
	if (dispatch) {
		tTJSVariant result;
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->PropGet(0, sqobject::getString(v, -1), NULL, &result, dispatch))) {
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
 * ����2 �ݒ�l
 * ���R�ϐ�1 �v���p�e�B��
 */
SQRESULT
TJSObject::tjsSetter(HSQUIRRELVM v)
{
	iTJSDispatch2 *dispatch = getDispatch(v, 1);
	if (dispatch) {
		tTJSVariant result;
		sq_getvariant(v, 2, &result);
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->PropSet(TJS_MEMBERENSURE, sqobject::getString(v, -1), NULL, &result, dispatch))) {
			return SQ_OK;
		} else {
			return ERROR_KRKR(v, error);
		}
	}
	return ERROR_BADINSTANCE(v);
}

/**
 * TJS�I�u�W�F�N�g�p�̃��\�b�h
 * ����1 �I�u�W�F�N�g
 * ����2�` ����
 * ���R�ϐ�1 �N���X�I�u�W�F�N�g
 * ���R�ϐ�2 �����o��
 */
SQRESULT
TJSObject::tjsStaticInvoker(HSQUIRRELVM v)
{
	tTJSVariant tjsClassObj;
	if (SQ_SUCCEEDED(sq_getvariant(v, -2, &tjsClassObj))) {
		iTJSDispatch2 *dispatch = tjsClassObj.AsObjectNoAddRef();
		if (dispatch) {
			// �����ϊ�
			int argc = sq_gettop(v) - 3;
			tTJSVariant **args = NULL;
			if (argc > 0) {
				args = new tTJSVariant*[argc];
				for (int i=0;i<argc;i++) {
					args[i] = new tTJSVariant();
					sq_getvariant(v, 2+i, args[i]);
				}
			}
			// ���\�b�h�Ăяo��
			int ret = 0;
			tTJSVariant result;
			tjs_error error;
			if (TJS_SUCCEEDED(error = dispatch->FuncCall(0, sqobject::getString(v,-1), NULL, &result, argc, args, dispatch))) {
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
			if (args) {
				for (int i=0;i<argc;i++) {
					delete args[i];
				}
				delete[] args;
			}
			return result;
		}
	}
	return ERROR_BADINSTANCE(v);
}

/**
 * TJS�I�u�W�F�N�g�p�̃v���p�e�B�Q�b�^�[
 * ����1 �I�u�W�F�N�g
 * ���R�ϐ�1 �N���X�I�u�W�F�N�g
 * ���R�ϐ�2 �v���p�e�B��
 */
SQRESULT
TJSObject::tjsStaticGetter(HSQUIRRELVM v)
{
	tTJSVariant tjsClassObj;
	if (SQ_SUCCEEDED(sq_getvariant(v, -2, &tjsClassObj))) {
		iTJSDispatch2 *dispatch = tjsClassObj.AsObjectNoAddRef();
		if (dispatch) {
			tTJSVariant result;
			tjs_error error;
			if (TJS_SUCCEEDED(error = dispatch->PropGet(0, sqobject::getString(v, -1), NULL, &result, dispatch))) {
				sq_pushvariant(v, result);
				return 1;
			} else {
				return ERROR_KRKR(v, error);
			}
		}
	}
	return ERROR_BADINSTANCE(v);
}
	
/**
 * TJS�I�u�W�F�N�g�p�̃v���p�e�B�Z�b�^�[
 * ����1 �I�u�W�F�N�g
 * ����2 �ݒ�l
 * ���R�ϐ�1 �N���X�I�u�W�F�N�g
 * ���R�ϐ�2 �v���p�e�B��
 */
SQRESULT
TJSObject::tjsStaticSetter(HSQUIRRELVM v)
{
	tTJSVariant tjsClassObj;
	if (SQ_SUCCEEDED(sq_getvariant(v, -2, &tjsClassObj))) {
		iTJSDispatch2 *dispatch = tjsClassObj.AsObjectNoAddRef();
		if (dispatch) {
			tTJSVariant result;
			sq_getvariant(v, 2, &result);
			tjs_error error;
			if (TJS_SUCCEEDED(error = dispatch->PropSet(TJS_MEMBERENSURE, sqobject::getString(v, -1), NULL, &result, dispatch))) {
				return SQ_OK;
			} else {
				return ERROR_KRKR(v, error);
			}
		}
	}
	return ERROR_BADINSTANCE(v);
}
