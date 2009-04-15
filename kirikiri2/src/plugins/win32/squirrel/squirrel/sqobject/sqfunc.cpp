/**
 * ��ʓI�� sqobject ����
 * 
 * Object, Thread �̓o�^�����̎�����ł��B
 * Thread �͌p���s�ŁAObject �͌p���Ώۂ����ׂă��X�g�Ǘ����đΉ����Ă��܂��B
 */
#include "sqobjectinfo.h"
#include "sqobject.h"
#include "sqthread.h"
#include <map>

SQRESULT ERROR_CREATE(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("can't create native instance"));
}

SQRESULT ERROR_BADINSTANCE(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("bad instance"));
}

SQRESULT ERROR_BADMETHOD(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("bad method"));
}

// ���N���X�_�~�[�p
struct BaseClass {};
inline const SQChar *GetTypeName(const BaseClass *t) { return NULL; }

// �N���X���o�^�p
#define DECLARE_CLASSNAME(TYPE,NAME) \
inline const SQChar * GetTypeName(const TYPE *t) { return _SC(#NAME); }
#define DECLARE_CLASS(TYPE) DECLARE_CLASSNAME(TYPE,TYPE)

// �Y���I�u�W�F�N�g�̃^�O�̃��X�g
typedef std::vector<SQUserPointer> SQTypeTagList;
// �I�u�W�F�N�g�ʂ̃^�O�̃}�b�v
typedef std::map<sqobject::tstring, SQTypeTagList> SQTypeTagListMap;
// �I�u�W�F�N�g�̌p���֌W�̋L�^�p
typedef std::map<sqobject::tstring, sqobject::tstring> SQTypeMap;

SQTypeTagListMap typeTagListMap;
SQTypeMap typeMap;

/**
 * �p���֌W��o�^����
 * @param typeName �^��
 * @param parentName �e�̌^��
 */
void
registerInherit(const SQChar *typeName, const SQChar *parentName)
{
	typeMap[typeName] = parentName;
}

/**
 * �e�̌^����Ԃ�
 * @param typeName �^��
 * @return �e�̌^��
 */
const SQChar *
getParentName(const SQChar *typeName)
{
	SQTypeMap::const_iterator p = typeMap.find(typeName);
	if (p != typeMap.end()) {
		return p->second.c_str();
	}
	return NULL;
}

/**
 * �I�u�W�F�N�g�̃^�O��o�^����B�e�I�u�W�F�N�g�ɂ��ċA�I�ɓo�^����B
 * @param typeName �^��
 * @parma tag �^�O
 */
void
registerTypeTag(const SQChar *typeName, SQUserPointer tag)
{
	// �^�O��o�^
	SQTypeTagListMap::iterator l = typeTagListMap.find(typeName);
	if (l != typeTagListMap.end()) {
		l->second.push_back(tag);
	} else {
		typeTagListMap[typeName] = SQTypeTagList();
		typeTagListMap[typeName].push_back(tag);
	}

	// �e�N���X�ɂ��o�^
	const SQChar *pname = getParentName(typeName);
	if (pname) {
		registerTypeTag(pname, tag);
	}
}

/**
 * �Y���I�u�W�F�N�g�̃l�C�e�B�u�C���X�^���X���擾�B�o�^����Ă�^�O���X�g���g��
 * @param v squirrelVM
 * @param idx �X�^�b�N�C���f�b�N�X
 * @param typeName �^��
 * @return �w�肳�ꂽ�^�̃l�C�e�B�u�C���X�^���X�B�݂���Ȃ��ꍇ�� NULL
 */
SQUserPointer
getInstance(HSQUIRRELVM v, int idx, const SQChar *typeName)
{
	SQTypeTagListMap::iterator l = typeTagListMap.find(sqobject::tstring(typeName));
	if (l != typeTagListMap.end()) {
		SQTypeTagList &tags = l->second;
		SQTypeTagList::const_iterator i = tags.begin();
		while (i != tags.end()) {
			SQUserPointer tag = *i;
			SQUserPointer up;
			if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, tag))) {
				return up;
			}
			i++;
		}
	}
	return NULL;
}

/**
 * �N���X�o�^�p�e���v���[�g
 * @param T �o�^�Ώی^
 * @param P �e�̌^
 */
template <typename T, typename P>
class SQTemplate {

private:
	HSQUIRRELVM v;
	
public:
	/**
	 * �N���X���`����
	 * @param v squirrelVM
	 * @param typetag �^���ʃ^�O
	 */
	SQTemplate(HSQUIRRELVM v, SQUserPointer typetag) : v(v) {

		// �^���� DECLARE_CLASSNAME �̓o�^�������ĎQ��
		T* typeDummy = NULL;
		P* parentDummy = NULL;
		const SQChar *typeName = GetTypeName(typeDummy);
		const SQChar *parentName = GetTypeName(parentDummy);
		
		sq_pushstring(v, typeName, -1);
		if (parentName) {
			// �e�N���X���w�肳��Ă�ꍇ�͌p������
			::registerInherit(typeName, parentName);
			sq_pushstring(v, parentName, -1);
			sq_get(v,-3);
			sq_newclass(v, true);
		} else {
			// �p���Ȃ��ŃN���X����
			sq_newclass(v, false);
		}
		// �^�O��o�^
		sq_settypetag(v, -1, typetag);
		::registerTypeTag(typeName, typetag);
		
		// �R���X�g���N�^��o�^
		Register(constructor, _SC("constructor"));
	}
	
	/*
	 * �l�C�e�B�u�I�u�W�F�N�g�̃����[�T�B
	 * delete���邾���B
	 */
	static SQRESULT release(SQUserPointer up, SQInteger size) {
		if (up) { delete (T*)up; }
		return SQ_OK;
	}
	
	/**
	 * �R���X�g���N�^
	 * �l�C�e�B�u�I�u�W�F�N�g�̃R���X�g���N�^�Ɉ����Ƃ��� HSQUIRRELVM ��n���B
	 */
	static SQRESULT constructor(HSQUIRRELVM v) {
		T *self = new T(v);
		if (self) {
			SQRESULT result;
			if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {;
				sq_setreleasehook(v, 1, release);
			} else {
				delete self;
			}
			return result;
		} else {
			return ERROR_CREATE(v);
		}
	}

	// -------------------------------------------------
	// �X�^�e�B�b�N�֐��̓o�^
	// -------------------------------------------------

	// SQFUNCTION �o�^
	void Register(SQFUNCTION func, const SQChar *name) {
		sq_pushstring(v, name, -1);
		sq_newclosure(v, func, 0);
		sq_createslot(v, -3);
	}

	// -------------------------------------------------
	// �����o�֐��̓o�^�p
	// -------------------------------------------------

	// �C���X�^���X�擾
	static T *getInstance(HSQUIRRELVM v, int idx=1) {
		const T *dummy = NULL;
		return static_cast<T*>(::getInstance(v,idx,GetTypeName(dummy)));
	}
	
	// �֐��|�C���^�擾
	template <typename Func>
	static void getFunc(HSQUIRRELVM v, Func **func) {
		SQUserPointer x = NULL;
		sq_getuserdata(v,sq_gettop(v),&x,NULL);
		*func = (Func*)x;
	}

	// -------------------------------------------------

	// �A��l int �ň��������̊֐�
	typedef void (T::*VoidFunc)();

	// IntFunc �Ăяo��
	static SQRESULT VoidFuncCaller(HSQUIRRELVM v) {
		T *instance = getInstance(v);
		if (instance) {
			VoidFunc *func;
			getFunc(v, &func);
			if (func) {
				(instance->*(*func))();
				return 0;
			}
			return ERROR_BADMETHOD(v);
		}
		return ERROR_BADINSTANCE(v);
	}

	// VoidFunc �o�^
	void Register(VoidFunc func, const SQChar *name) {
		sq_pushstring(v, name, -1);
		SQUserPointer up = sq_newuserdata(v,sizeof(func));
		memcpy(up,&func,sizeof(func));
		sq_newclosure(v,VoidFuncCaller,1);
		sq_createslot(v, -3);
	}

	// -------------------------------------------------
	
	// �A��l int �ň��������̊֐�
	typedef int (T::*IntFunc)();

	// IntFunc �Ăяo��
	static SQRESULT IntFuncCaller(HSQUIRRELVM v) {
		T *instance = getInstance(v);
		if (instance) {
			IntFunc *func;
			getFunc(v, &func);
			if (func) {
				sq_pushinteger(v, (instance->*(*func))());
				return 1;
			}
			return ERROR_BADMETHOD(v);
		}
		return ERROR_BADINSTANCE(v);
	}

	// IntFunc �o�^
	void Register(IntFunc func, const SQChar *name) {
		sq_pushstring(v, name, -1);
		SQUserPointer up = sq_newuserdata(v,sizeof(func));
		memcpy(up,&func,sizeof(func));
		sq_newclosure(v,IntFuncCaller,1);
		sq_createslot(v, -3);
	}

	// -------------------------------------------------

	// SQFUNCTION �X�^�C���̊֐�
	typedef SQRESULT (T::*VFunc)(HSQUIRRELVM v);

	// VFunc �Ăяo��
	static SQRESULT VFuncCaller(HSQUIRRELVM v) {
		T *instance = getInstance(v);
		if (instance) {
			VFunc *func = NULL;
			getFunc(v, &func);
			if (func) {
				return (instance->*(*func))(v);
			}
			return ERROR_BADMETHOD(v);
		}
		return ERROR_BADINSTANCE(v);
	}

	// VFunc �o�^
	void RegisterV(VFunc func, const SQChar *name) {
		sq_pushstring(v, name, -1);
		SQUserPointer up = sq_newuserdata(v,sizeof(func));
		memcpy(up,&func,sizeof(func));
		sq_newclosure(v,VFuncCaller,1);
		sq_createslot(v, -3);
	}
};

namespace sqobject {

// typetag �S�\�[�X�Ń��j�[�N�ȃA�h���X�ɂ���K�v������
const SQUserPointer OBJECTTYPETAG = (SQUserPointer)"OBJECTTYPETAG";
const SQUserPointer THREADTYPETAG = (SQUserPointer)"THREADTYPETAG";

// �N���X����`

DECLARE_CLASSNAME(Object, SQOBJECTNAME);
DECLARE_CLASSNAME(Thread, SQTHREADNAME);

// global vm
HSQUIRRELVM vm;

/// vm ������
HSQUIRRELVM init() {
	vm = sq_open(1024);
	sq_pushroottable(vm);
	sqstd_register_mathlib(vm);
	sqstd_register_stringlib(vm);
	sqstd_seterrorhandlers(vm);
	sq_pop(vm,1);
	return vm;
}

/// ���ێ��p�O���[�o��VM�̎擾
HSQUIRRELVM getGlobalVM()
{
	return vm;
}

/// vm �I��
void done()
{
	sq_close(vm);
}

// Thread �̃C���X�^���X���[�U�|�C���^���擾
Thread *
ObjectInfo::getThread()
{
	if (sq_isinstance(obj)) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		Thread *ret = NULL;
		ret = (Thread*)::getInstance(gv, -1, GetTypeName(ret));
		sq_pop(gv,1);
		return ret;
	}
	return NULL;
}

// Object �̃C���X�^���X���[�U�|�C���^���擾
Object *
ObjectInfo::getObject()
{
	if (sq_isinstance(obj)) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		Object *ret = NULL;
		ret = (Object*)::getInstance(gv, -1, GetTypeName(ret));
		sq_pop(gv,1);
		return ret;
	}
	return NULL;
}

// ------------------------------------------------------------------
// �N���X�o�^�p�}�N��
// ------------------------------------------------------------------

#define SQCLASS(Class,Parent,typetag) SQTemplate<Class,Parent> cls(v,typetag)
#define SQFUNC(Class, Name)   cls.Register(&Class::Name, _SC(#Name))
#define SQVFUNC(Class, Name)  cls.RegisterV(&Class::Name, _SC(#Name))

/**
 * �N���X�̓o�^
 * @param v squirrel VM
 */
void
Object::registerClass()
{
	HSQUIRRELVM v = getGlobalVM();
	sq_pushroottable(v); // root

	SQCLASS(Object,BaseClass,OBJECTTYPETAG);

	SQFUNC(Object,notify);
	SQFUNC(Object,notifyAll);
	SQVFUNC(Object,hasSetProp);
	SQVFUNC(Object,setDelegate);
	SQVFUNC(Object,getDelegate);
	SQVFUNC(Object,_get);
	SQVFUNC(Object,_set);
	cls.RegisterV(&Object::_get, _SC("get"));
	cls.RegisterV(&Object::_set, _SC("set"))

	sq_createslot(v, -3); // ���������N���X��o�^
	sq_pop(v,1); // root
};

/**
 * �N���X�̓o�^
 * @param v squirrel VM
 */
void
Thread::registerClass()
{
	HSQUIRRELVM v = getGlobalVM();
	sq_pushroottable(v); // root

	SQCLASS(Thread, Object, THREADTYPETAG);

	SQVFUNC(Thread,exec);
	SQVFUNC(Thread,exit);
	SQFUNC(Thread,stop);
	SQFUNC(Thread,run);
	SQFUNC(Thread,getCurrentTick);
	SQFUNC(Thread,getStatus);
	SQVFUNC(Thread,getExitCode);
	SQVFUNC(Thread,wait);
	SQFUNC(Thread,cancelWait);

	sq_createslot(v, -3);
	sq_pop(v, 1); // root
};

}

