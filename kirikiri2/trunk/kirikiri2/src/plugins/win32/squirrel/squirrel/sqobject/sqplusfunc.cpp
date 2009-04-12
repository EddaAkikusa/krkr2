/**
 * SQPLUS �ł� sqobject ����
 *
 * sqplus ���g���� Object, Thread �o�^�����̎�����ł��B
 * sqplus �̋@�\�������Čp�����������Ă��܂��B
 */
#include "sqobjectinfo.h"
#include "sqobject.h"
#include "sqthread.h"
#include <sqplus.h>

using namespace SqPlus;
using namespace sqobject;
DECLARE_INSTANCE_TYPE(Object);
DECLARE_INSTANCE_TYPE(Thread);

namespace sqobject {

// global vm
HSQUIRRELVM vm;

/// vm ������
HSQUIRRELVM init() {
	vm = sq_open(1024);
	SquirrelVM::InitNoRef(vm);
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
	if (sq_isinstance(obj)) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		Thread *ret = SqPlus::GetInstance<Thread,false>(gv, -1);
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
		Object *ret = SqPlus::GetInstance<Object,false>(gv, -1);
		sq_pop(gv, 1);
		return ret;
	}
	return NULL;
}

/**
 * �ėp�R���X�g���N�^
 * @param v squirrel VM
 */
template <typename T>
struct GeneralConstructor {
	static SQRESULT release(SQUserPointer up, SQInteger size) {
		if (up) { delete (T*)up; }
		return SQ_OK;
	}
	static SQRESULT constructor(HSQUIRRELVM v) {
		return SqPlus::PostConstruct<T>(v, new T(v), release);
	}
};

// ------------------------------------------------------------------
// �N���X�o�^�p�}�N��
// ------------------------------------------------------------------

#define SQCONSTRUCT(Class) cls.staticFuncVarArgs(GeneralConstructor<Class>::constructor, _SC("constructor"))
#define SQFUNC(Class, Name) cls.func(&Class::Name,_SC(#Name))
#define SQVFUNC(Class, Name) cls.funcVarArgs(&Class::Name,_SC(#Name))

/**
 * Object �N���X�̓o�^
 */
void
Object::registerClass()
{
	SqPlus::SQClassDef<Object> cls(SQOBJECTNAME);
	SQCONSTRUCT(Object);
	SQFUNC(Object,notify);
	SQFUNC(Object,notifyAll);
	SQVFUNC(Object,hasSetProp);
	SQVFUNC(Object,setDelegate);
	SQVFUNC(Object,getDelegate);
	SQVFUNC(Object,_get);
	SQVFUNC(Object,_set);
};

/**
 * Thread �N���X�̓o�^
 */
void
Thread::registerClass()
{
	SqPlus::SQClassDef<Thread,Object> cls(SQTHREADNAME);
	SQCONSTRUCT(Thread);
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
