/**
 * sqrat �ł� sqobject ����
 *
 * sqrat ���g���� Object, Thread �o�^�����̎�����ł��B
 * sqrat �̋@�\�������Čp�����������Ă��܂��B
 */
#include "sqobjectinfo.h"
#include "sqobject.h"
#include "sqthread.h"
#include "sqratfunc.h"

#include <sqstdstring.h>
#include <sqstdmath.h>
#include <sqstdaux.h>

using namespace sqobject;

namespace sqobject {

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

// ---------------------------------------------------
// �I�u�W�F�N�g�擾
// ---------------------------------------------------

// Thread �̃C���X�^���X���[�U�|�C���^���擾
Thread *
ObjectInfo::getThread()
{
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	Thread *ret = Sqrat::Var<Thread*>(gv, -1).value;
	sq_pop(gv,1);
	return ret;
}

// Object �̃C���X�^���X���[�U�|�C���^���擾
Object *
ObjectInfo::getObject()
{
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	Object *ret = Sqrat::Var<Object*>(gv, -1).value;
	sq_pop(gv, 1);
	return ret;
}

// �f�X�g���N�^�o�^�p
static SQRESULT destructor(HSQUIRRELVM v) {
	return SQ_OK;
}

/**
 * Object �N���X�̓o�^
 */
void
Object::registerClass()
{
	Sqrat::Class<Object, sqobject::VMConstructor<Object>> cls(vm);
	cls.SquirrelFunc(_SC("destructor"), ::destructor);
	SQFUNC(Object,notify);
	SQFUNC(Object,notifyAll);
	SQVFUNC(Object,hasSetProp);
	SQVFUNC(Object,setDelegate);
	SQVFUNC(Object,getDelegate);
	// sqrat �� set/get ���㏑������ sqobject �@�\�Ɛ������Ƃ�
	sqobject::OverrideSetGet<Object>::Func(vm);
	Sqrat::RootTable(vm).Bind(SQOBJECTNAME, cls);
};

/**
 * Thread �N���X�̓o�^
 */
void
Thread::registerClass()
{
	Sqrat::DerivedClass<Thread, Object, sqobject::VMConstructor<Thread>> cls(vm);
	SQVFUNC(Thread,exec);
	SQVFUNC(Thread,exit);
	SQFUNC(Thread,stop);
	SQFUNC(Thread,run);
	SQFUNC(Thread,getCurrentTick);
	SQFUNC(Thread,getStatus);
	SQVFUNC(Thread,getExitCode);
	SQVFUNC(Thread,wait);
	SQFUNC(Thread,cancelWait);
	sqobject::OverrideSetGet<Thread>::Func(vm);
	Sqrat::RootTable(vm).Bind(SQTHREADNAME, cls);
};

}
