/**
 * Squirrel ���� �g���g���u���b�W����
 * �g���g���̃I�u�W�F�N�g�� UserData �Ƃ��ĊǗ�����
 */

#include <windows.h>
#include "tp_stub.h"
#include <squirrel.h>

// �i�[�E�擾�p
void sq_pushvariant(HSQUIRRELVM v, tTJSVariant &variant);
SQRESULT sq_getvariant(HSQUIRRELVM v, int idx, tTJSVariant *result);

/**
 * �G���[����
 */
void
SQEXCEPTION(HSQUIRRELVM v)
{
	sq_getlasterror(v);
	const SQChar *str;
	sq_getstring(v, -1, &str);
	ttstr error = str;
	sq_pop(v, 1);
	TVPThrowExceptionMessage(error.c_str());
}

/**
 * HSQOBJECT �p iTJSDispatch2 ���b�p�[
 */
class iTJSDispatch2Wrapper : public tTJSDispatch
{
protected:
	/// �����ێ��p
	HSQUIRRELVM v;
	HSQOBJECT obj;

public:
	/**
	 * �R���X�g���N�^
	 * @param obj IDispatch
	 */
	iTJSDispatch2Wrapper(HSQUIRRELVM v, HSQOBJECT obj) : v(v), obj(obj) {
		sq_addref(v, &obj);
	}
	
	/**
	 * �f�X�g���N�^
	 */
	~iTJSDispatch2Wrapper() {
		if(v) {
			sq_release(v, &obj);
		}
	}

public:
	// �I�u�W�F�N�g����
	tjs_error TJS_INTF_METHOD CreateNew(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		iTJSDispatch2 **result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis) {
		if (membername) {
			return TJS_E_MEMBERNOTFOUND;
		}
		if (obj._type != OT_CLASS) {
			return TJS_E_NOTIMPL;
		}
		int ret = S_FALSE;
		sq_pushobject(v, obj);
		sq_pushroottable(v);			// this ��������
		for (int i=0;i<numparams;i++) {	// �p�����[�^�Q
			sq_pushvariant(v, *param[i]);
		}
		if (SQ_SUCCEEDED(sq_call(v, numparams + 1, result ? SQTrue:SQFalse, SQTrue))) {
			if (result) {
				tTJSVariant var;
				sq_getvariant(v, -1, &var);
				sq_pop(v, 1);
				*result = var.AsObject();
			}
			sq_pop(v, 1);
		} else {
			sq_pop(v, 1);
			SQEXCEPTION(v);
		}
		return TJS_S_OK;
	}

	// �I�u�W�F�N�g�@�\�Ăяo��
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis
		) {
		if (membername == NULL) {
			if (!(obj._type == OT_CLOSURE ||
				  obj._type == OT_NATIVECLOSURE ||
				  obj._type == OT_GENERATOR)) {
				return TJS_E_NOTIMPL;
			}
			sq_pushobject(v, obj);
			sq_pushroottable(v);
			for (int i=0;i<numparams;i++) {	// �p�����[�^�Q
				sq_pushvariant(v, *param[i]);
			}
			if (SQ_SUCCEEDED(sq_call(v, numparams + 1, result ? SQTrue:SQFalse, SQTrue))) {
				if (result) {
					sq_getvariant(v, -1, result);
					sq_pop(v, 1);
				}
				sq_pop(v, 1);
			} else {
				sq_pop(v, 1);
				SQEXCEPTION(v);
			}
			return TJS_S_OK;
		} else {
			sq_pushobject(v, obj);
			sq_pushstring(v, membername,-1);
			if (SQ_SUCCEEDED(sq_get(v,-2))) {
				sq_pushobject(v, obj); // this
				for (int i=0;i<numparams;i++) {	// �p�����[�^�Q
					sq_pushvariant(v, *param[i]);
				}
				// �A��l������ꍇ
				if (SQ_SUCCEEDED(sq_call(v, numparams + 1, result ? SQTrue:SQFalse, SQTrue))) {
					if (result) {
						sq_getvariant(v, -1, result);
						sq_pop(v, 1);
					}
					sq_pop(v, 2);
				} else {
					sq_pop(v, 2);
					SQEXCEPTION(v);
				}
			} else {
				sq_pop(v, 1);
				return TJS_E_MEMBERNOTFOUND;
			}
			return TJS_S_OK;
		}
	}

	// �v���p�e�B�擾
	tjs_error TJS_INTF_METHOD PropGet(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result,
		iTJSDispatch2 *objthis) {
		// �v���p�e�B�͂Ȃ�
		if (!membername) {
			return TJS_E_NOTIMPL;
		}
		sq_pushobject(v, obj);
		sq_pushstring(v, membername,-1);
		if (SQ_SUCCEEDED(sq_get(v,-2))) {
			if (result) {
				sq_getvariant(v, -1, result);
			}
			sq_pop(v,2);
		} else {
			sq_pop(v,1);
			return TJS_E_MEMBERNOTFOUND;
		}
		return TJS_S_OK;
	}

	// �v���p�e�B�ݒ�
	tjs_error TJS_INTF_METHOD PropSet(
		tjs_uint32 flag,
		const tjs_char *membername,
		tjs_uint32 *hint,
		const tTJSVariant *param,
		iTJSDispatch2 *objthis) {
		// �v���p�e�B�͂Ȃ�
		if (!membername) {
			return TJS_E_NOTIMPL;
		}
		sq_pushobject(v, obj);
		sq_pushstring(v, membername,-1);
		sq_pushvariant(v, (tTJSVariant&)*param);
		if ((flag & TJS_MEMBERENSURE)) {
			if (SQ_SUCCEEDED(sq_newslot(v,-3, SQFalse))) {
				sq_pop(v,1);
			} else {
				sq_pop(v,1);
				SQEXCEPTION(v);
			}
		} else {
			if (SQ_SUCCEEDED(sq_set(v,-3))) {
				sq_pop(v,1);
			} else {
				sq_pop(v,1);
				SQEXCEPTION(v);
			}
		}
		return TJS_S_OK;
	}
};


//----------------------------------------------------------------------------
// iTJSDispatch2 �� UserData �Ƃ��ĕێ����邽�߂̋@�\
//----------------------------------------------------------------------------

/**
 * iTJSDispatch2 �p�I�u�W�F�N�g�J������
 */
static SQInteger
tjsDispatchRelease(SQUserPointer up, SQInteger size)
{
	iTJSDispatch2 *dispatch	= *((iTJSDispatch2**)up);
	dispatch->Release();
	return 1;
}

// ������擾�p
static const SQChar *GetString(HSQUIRRELVM v, int idx)
{
	const SQChar *x = NULL;
	sq_getstring(v,idx,&x);
	return x;
}

static const SQUserPointer TJSTYPETAG = (SQUserPointer)"TJSTYPETAG";

// iTJSDispatch2* �擾�p
static iTJSDispatch2* GetDispatch(HSQUIRRELVM v, int idx)
{
	SQUserPointer otag;
	SQUserPointer up;
	if (SQ_SUCCEEDED(sq_getuserdata(v,idx,&up,&otag)) && otag == TJSTYPETAG) {
		return *((iTJSDispatch2**)up);
	}
	return NULL;
}

static SQInteger ERROR_NOMEMBER(HSQUIRRELVM v)
{
	return sq_throwerror(v, _SC("no such member"));
}

/**
 * iTJSDispatch2 �p�v���p�e�B�̎擾
 * @param v squirrel VM
 */
static SQInteger
get(HSQUIRRELVM v)
{
	iTJSDispatch2 *dispatch = GetDispatch(v, 1);
	if (dispatch) {
		tTJSVariant result;
		if (SUCCEEDED(dispatch->PropGet(0, GetString(v, 2), NULL, &result, dispatch))) {
			sq_pushvariant(v, result);
			return 1;
		}
	}
	return ERROR_NOMEMBER(v);
}

/**
 * iTJSDispatch2 �p�v���p�e�B�̐ݒ�
 * @param v squirrel VM
 */
static SQInteger
set(HSQUIRRELVM v)
{
	iTJSDispatch2 *dispatch	= GetDispatch(v, 1);
	if (dispatch) {
		tTJSVariant result;
		sq_getvariant(v, 3, &result);
		dispatch->PropSet(TJS_MEMBERENSURE, GetString(v, 2), NULL, &result, dispatch);
		return SQ_OK;
	}
	return ERROR_NOMEMBER(v);
}

static SQInteger ERROR_CREATE(HSQUIRRELVM v)
{
	return sq_throwerror(v, _SC("invalid create"));
}

/**
 * iTJSDispatch2 �p�R���X�g���N�^�̌Ăяo��
 * @param v squirrel VM
 */
static SQInteger
callConstructor(HSQUIRRELVM v)
{
	// param1 �I�u�W�F�N�g
	// param2 �I���W�i���I�u�W�F�N�g
	// param3 �` �{���̈�����

	iTJSDispatch2 *dispatch	= GetDispatch(v, 1);
	if (dispatch) {
		
		// this ���擾
		iTJSDispatch2 *thisobj = GetDispatch(v, 2);

		int argc = sq_gettop(v) - 2;
		
		// �����ϊ�
		tTJSVariant **args = new tTJSVariant*[argc];
		for (int i=0;i<argc;i++) {
			args[i] = new tTJSVariant();
			sq_getvariant(v, i+3, args[i]);
		}

		int ret = 0;
		iTJSDispatch2 *instance = NULL;
		if (SUCCEEDED(dispatch->CreateNew(0, NULL, NULL, &instance, argc, args, thisobj))) {
			tTJSVariant var(instance, instance);
			sq_pushvariant(v, var);
			instance->Release();
			ret = 1;
		} else {
			ret = ERROR_CREATE(v);
		}
			
		// �����j��
		for (int i=0;i<argc;i++) {
			delete args[i];
		}
		delete[] args;

		return ret;
	}
	return ERROR_CREATE(v);
}

static SQInteger ERROR_CALL(HSQUIRRELVM v)
{
	return sq_throwerror(v, _SC("invalid call"));
}

/**
 * iTJSDispatch2 �p���\�b�h�̌Ăяo��
 * @param v squirrel VM
 */
static SQInteger
callMethod(HSQUIRRELVM v)
{
	// param1 �I�u�W�F�N�g
	// param2 �I���W�i���I�u�W�F�N�g
	// param3 �` �{���̈�����

	iTJSDispatch2 *dispatch	= GetDispatch(v, 1);
	if (dispatch) {

		// this ���擾
		iTJSDispatch2 *thisobj = GetDispatch(v, 2);
		
		int argc = sq_gettop(v) - 2;
		
		// �����ϊ�
		tTJSVariant **args = new tTJSVariant*[argc];
		for (int i=0;i<argc;i++) {
			args[i] = new tTJSVariant();
			sq_getvariant(v, i+3, args[i]);
		}

		// ���\�b�h�Ăяo��
		int ret = 0;
		tTJSVariant result;
		if (SUCCEEDED(dispatch->FuncCall(0, NULL, NULL, &result, argc, args, thisobj))) {
			if (result.Type() != tvtVoid) {
				sq_pushvariant(v, result);
				ret = 1;
			} else {
				ret = 0;
			}
		} else {
			ret = ERROR_CALL(v);
		}
			
		// �����j��
		for (int i=0;i<argc;i++) {
			delete args[i];
		}
		delete[] args;

		return ret;
	}
	return ERROR_CALL(v);
}

/**
 * tTJSVariant �� squirrel �̋�Ԃɓ�������
 * @param v squirrel VM
 * @param variant tTJSVariant
 */
void
sq_pushvariant(HSQUIRRELVM v, tTJSVariant &variant)
{
	switch (variant.Type()) {
	case tvtVoid:
		sq_pushnull(v);
		break;
	case tvtObject:
		{
			iTJSDispatch2 *dispatch = variant.AsObject();
			if (dispatch) {
				// UserData �m��
				SQUserPointer up = sq_newuserdata(v, sizeof *dispatch);
				*((iTJSDispatch2**)up) = dispatch;
				
				// �^�O�o�^
				sq_settypetag(v, -1, TJSTYPETAG);
				
				// �J�����W�b�N��ǉ�
				sq_setreleasehook(v, -1, tjsDispatchRelease);
				
				// ���\�b�h�Q��ǉ�
				sq_newtable(v);
				
				sq_pushstring(v, L"_get", -1);
				sq_newclosure(v, get, 0);
				sq_createslot(v, -3);
				
				sq_pushstring(v, L"_set", -1);
				sq_newclosure(v, set, 0);
				sq_createslot(v, -3);
				
				sq_pushstring(v, L"_call", -1);
				if (dispatch->IsInstanceOf(0, NULL, NULL, L"Class", dispatch) == TJS_S_TRUE) {
					sq_newclosure(v, callConstructor, 0);
				} else {
					sq_newclosure(v, callMethod, 0);
				}
				sq_createslot(v, -3);
				
				sq_setdelegate(v, -2);
			} else {
				sq_pushnull(v);
			}
		}
		break;
	case tvtString:
		sq_pushstring(v, variant.GetString(), -1);
		break;
	case tvtOctet: // XXX
		sq_pushnull(v);
		break;
	case tvtInteger:
		sq_pushinteger(v, (SQInteger)(variant));
		break;
	case tvtReal:
		sq_pushfloat(v, (SQFloat)(double)(variant));
		break;
	}
}

/**
 * tTJSVariant �� squirrel �̋�Ԃ���擾����
 * @param v squirrel VM
 * @param idx �X�^�b�N�̃C���f�b�N�X
 * @param result tTJSVariant ��Ԃ���
 */
SQRESULT
sq_getvariant(HSQUIRRELVM v, int idx, tTJSVariant *result)
{
	if (result) {
		switch (sq_gettype(v, idx)) {
		case OT_NULL: result->Clear(); break;
		case OT_INTEGER: { SQInteger i; sq_getinteger(v, idx, &i);	*result = (tjs_int64)i; } break;
		case OT_FLOAT:   { SQFloat f; sq_getfloat(v, idx, &f); 	    *result = (double)f; } break;
		case OT_BOOL:    { SQBool b; sq_getbool(v, idx, &b);        *result = b != SQFalse; } break;
		case OT_STRING:  { const SQChar *c; sq_getstring(v, idx, &c); *result = c; } break;
		case OT_USERDATA:
			{
				iTJSDispatch2 *dispatch = GetDispatch(v, idx);
				if (dispatch) {
					// ���X�g���g��������n���ꂽ�f�[�^
					*result = tTJSVariant(dispatch, dispatch);
				} else {
					result->Clear();
				}
			}
			break;
		case OT_TABLE:
		case OT_ARRAY:
		case OT_CLOSURE:
		case OT_NATIVECLOSURE:
		case OT_GENERATOR:
		case OT_USERPOINTER:
		case OT_THREAD:
		case OT_CLASS:
		case OT_INSTANCE:
		case OT_WEAKREF:
			// ���b�s���O
			{
				HSQOBJECT x;
				sq_resetobject(&x);
				sq_getstackobj(v,idx,&x);
				iTJSDispatch2 *tjsobj = new iTJSDispatch2Wrapper(v, x);
				if (tjsobj) {
					*result = tTJSVariant(tjsobj, tjsobj);
					tjsobj->Release();
				}
			}
			break;
		default:
			result->Clear();
		}
		return SQ_OK;
	}
	return SQ_ERROR;
}
