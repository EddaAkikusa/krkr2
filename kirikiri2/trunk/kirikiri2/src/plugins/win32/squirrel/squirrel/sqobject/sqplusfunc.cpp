/**
 * SQPLUS �ł� sqobject ����
 *
 * sqplus ���g���� Object, Thread �o�^�����̎�����ł��B
 * sqplus �̋@�\�������Čp�����������Ă��܂��B
 */
#include "sqobjectinfo.h"
#include "sqobject.h"
#include "sqthread.h"
#include "sqplusfunc.h"

#include <sqstdstring.h>
#include <sqstdmath.h>
#include <sqstdaux.h>

using namespace SqPlus;
using namespace sqobject;
DECLARE_INSTANCE_TYPE_NAME_RELEASE(Object, SQOBJECT);
DECLARE_INSTANCE_TYPE_NAME_RELEASE(Thread, SQTHREAD);

namespace sqobject {

// global vm
HSQUIRRELVM vm;

/// vm ������
HSQUIRRELVM init() {
	vm = sq_open(1024);
	SquirrelVM::InitNoRef(vm);
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
	SquirrelVM::ClearRootTable();
	SquirrelVM::Shutdown();
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
	Thread *ret = SqPlus::GetInstance<Thread,false>(gv, -1);
	sq_pop(gv,1);
	return ret;
}

// Object �̃C���X�^���X���[�U�|�C���^���擾
Object *
ObjectInfo::getObject()
{
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	Object *ret = SqPlus::GetInstance<Object,false>(gv, -1);
	sq_pop(gv, 1);
	return ret;
}

/**
 * Object �N���X�̓o�^
 */
void
Object::registerClass()
{
	SQCLASS(Object);
	SQVCONSTRUCTOR(Object);
	SQDESTRUCTOR(Object);
	SQFUNC(Object,notify);
	SQFUNC(Object,notifyAll);
	SQVFUNC(Object,hasSetProp);
	SQVFUNC(Object,setDelegate);
	SQVFUNC(Object,getDelegate);
	SQVFUNC(Object,_get);
	SQVFUNC(Object,_set);
	// _get / _set �œo�^�������̂͌ォ��Q�Ƃł��Ȃ��̂ŁA
	// �p����Ŏg�����߂ɕʖ��œo�^���Ă���
	cls.funcVarArgs(&Object::_get,_SC("get"));
	cls.funcVarArgs(&Object::_set,_SC("set"));
};

/**
 * Thread �N���X�̓o�^
 */
void
Thread::registerClass()
{
	SQCLASSEX(Thread, Object);
	SQVCONSTRUCTOR(Thread);
	SQDESTRUCTOR(Thread);
	SQVFUNC(Thread,exec);
	SQVFUNC(Thread,exit);
	SQFUNC(Thread,stop);
	SQFUNC(Thread,run);
	SQFUNC(Thread,getCurrentTick);
	SQFUNC(Thread,getStatus);
	SQVFUNC(Thread,getExitCode);
	SQVFUNC(Thread,wait);
	SQFUNC(Thread,cancelWait);
};

}
