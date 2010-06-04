/**
 * ��ʓI�� sqobject ����
 * 
 * Object, Thread �̓o�^�����̎�����ł��B
 * �p�����͒P�����X�g�Ǘ����Ă܂�
 */
#include "sqobjectinfo.h"
#include "sqobject.h"
#include "sqthread.h"

#include <string.h>
#include <sqstdstring.h>
#include <sqstdmath.h>
#include <sqstdaux.h>

SQRESULT ERROR_CREATE(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("can't create native instance"));
}

SQRESULT ERROR_BADINSTANCE(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("bad instance"));
}

SQRESULT ERROR_BADMETHOD(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("bad method"));
}

sqobject::ObjectInfo typeTagListMap;
sqobject::ObjectInfo typeMap;

/**
 * �p���֌W��o�^����
 * @param typeName �^��
 * @param parentName �e�̌^��
 */
void
registerInherit(const SQChar *typeName, const SQChar *parentName)
{
	typeMap.create(typeName, parentName);
}

/**
 * �e�̌^����Ԃ�
 * @param typeName �^��
 * @return �e�̌^��
 */
const SQChar *
getParentName(const SQChar *typeName)
{
	const SQChar *parent = NULL;
	typeMap.get(typeName, &parent);
	return parent;
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
	sqobject::ObjectInfo list = typeTagListMap.get(typeName);
	if (!list.isArray()) {
		list.initArray();
		typeTagListMap.create(typeName, list);
	}
	list.append(tag);

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
getInstance(HSQUIRRELVM v, SQInteger idx, const SQChar *typeName)
{
	sqobject::ObjectInfo list = typeTagListMap.get(typeName);
	if (list.isArray()) {
		SQInteger max = list.len();
		for (SQInteger i=0;i<max;i++) {
			SQUserPointer tag;
			list.get(i, &tag);
			SQUserPointer up;
			if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, tag))) {
				return up;
			}
		}
	}
	return NULL;
}

namespace sqobject {

// typetag �S�\�[�X�Ń��j�[�N�ȃA�h���X�ɂ���K�v������
const SQUserPointer OBJECTTYPETAG = (SQUserPointer)"OBJECTTYPETAG";
const SQUserPointer THREADTYPETAG = (SQUserPointer)"THREADTYPETAG";

// �N���X����`

DECLARE_CLASSNAME(Object, SQOBJECT);
DECLARE_CLASSNAME(Thread, SQTHREAD);

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
	typeMap.initTable();
	typeTagListMap.initTable();
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
	// ���[�g�e�[�u�����N���A
	sq_pushroottable(vm);
	sq_clear(vm,-1);
	sq_pop(vm,1);
	typeTagListMap.clearData();
	typeTagListMap.clear();
	typeMap.clearData();
	typeMap.clear();
	sq_close(vm);
}

// ------------------------------------------------------------------
// �N���X�o�^�p�}�N��
// ------------------------------------------------------------------

#ifndef USE_SQOBJECT_TEMPLATE

static SQRESULT Object_notify(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		instance->notify();
		return SQ_OK;
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Object_notifyAll(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		instance->notifyAll();
		return SQ_OK;
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Object_hasSetProp(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		return instance->hasSetProp(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Object_setDelegate(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		return instance->setDelegate(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Object_getDelegate(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		return instance->getDelegate(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Object_get(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		return instance->_get(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Object_set(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		return instance->_set(v);
	}
	return ERROR_BADINSTANCE(v);
}

#endif

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
	SQVCONSTRUCTOR();
	
#ifdef USE_SQOBJECT_TEMPLATE
	SQFUNC(Object,notify);
	SQFUNC(Object,notifyAll);
	SQVFUNC(Object,hasSetProp);
	SQVFUNC(Object,setDelegate);
	SQVFUNC(Object,getDelegate);
	SQVFUNC(Object,_get);
	SQVFUNC(Object,_set);
	cls.RegisterV(&Object::_get, _SC("get"));
	cls.RegisterV(&Object::_set, _SC("set"));
#else
	SQNFUNC(Object,notify);
	SQNFUNC(Object,notifyAll);
	SQNFUNC(Object,hasSetProp);
	SQNFUNC(Object,setDelegate);
	SQNFUNC(Object,getDelegate);
	cls.Register(Object_get, _SC("_get"));
	cls.Register(Object_set, _SC("_set"));
	SQNFUNC(Object,get);
	SQNFUNC(Object,set);
#endif
	sq_createslot(v, -3); // ���������N���X��o�^
	sq_pop(v,1); // root
};

#ifndef USE_SQOBJECT_TEMPLATE

static SQRESULT Thread_exec(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		return instance->exec(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_exit(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		return instance->exit(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_stop(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		instance->stop();
		return SQ_OK;
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_run(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		instance->run();
		return SQ_OK;
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_getCurrentTick(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		sq_pushinteger(v,instance->getCurrentTick());
		return 1;
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_getStatus(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		sq_pushinteger(v,instance->getStatus());
		return 1;
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_getExitCode(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		return instance->getExitCode(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_wait(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		return instance->wait(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_cancelWait(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		instance->cancelWait();
		return SQ_OK;
	}
	return ERROR_BADINSTANCE(v);
}

#endif

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
	SQVCONSTRUCTOR();

#ifdef USE_SQOBJECT_TEMPLATE
	SQVFUNC(Thread,exec);
	SQVFUNC(Thread,exit);
	SQFUNC(Thread,stop);
	SQFUNC(Thread,run);
	SQFUNC(Thread,getCurrentTick);
	SQFUNC(Thread,getStatus);
	SQVFUNC(Thread,getExitCode);
	SQVFUNC(Thread,wait);
	SQFUNC(Thread,cancelWait);
#else
	SQNFUNC(Thread,exec);
	SQNFUNC(Thread,exit);
	SQNFUNC(Thread,stop);
	SQNFUNC(Thread,run);
	SQNFUNC(Thread,getCurrentTick);
	SQNFUNC(Thread,getStatus);
	SQNFUNC(Thread,getExitCode);
	SQNFUNC(Thread,wait);
	SQNFUNC(Thread,cancelWait);
#endif
	
	sq_createslot(v, -3);
	sq_pop(v, 1); // root
};

}

