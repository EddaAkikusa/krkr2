/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */
#include "sqobjectinfo.h"
#include "sqobject.h"
#include "sqthread.h"
#include <string.h>

namespace sqobject {

// ��r
bool ObjectInfo::operator == (const ObjectInfo &o)
{
	bool cmp = false;
	HSQUIRRELVM v = getGlobalVM();
	push(v);
	o.push(v);
	cmp = sq_cmp(v) == 0;
	sq_pop(v,2);
	return cmp;
}
	
// ���e����
void
ObjectInfo::clear()
{
	HSQUIRRELVM gv = getGlobalVM();
	sq_release(gv,&obj);
	sq_resetobject(&obj);
}

// �X�^�b�N����擾
void
ObjectInfo::getStack(HSQUIRRELVM v, int idx)
{
	clear();
	HSQUIRRELVM gv = getGlobalVM();
	sq_move(gv, v, idx);
	sq_getstackobj(gv, -1, &obj);
	sq_addref(gv, &obj);
	sq_pop(gv, 1);
}

// �X�^�b�N�����Q�ƂƂ��Ď擾
void
ObjectInfo::getStackWeak(HSQUIRRELVM v, int idx)
{
	clear();
	HSQUIRRELVM gv = getGlobalVM();
	sq_weakref(v, idx);
	sq_move(gv, v, -1);
	sq_pop(v, 1);
	sq_getstackobj(gv, -1, &obj);
	sq_addref(gv, &obj);
	sq_pop(gv, 1);
}


// �R���X�g���N�^
ObjectInfo::ObjectInfo() {
	sq_resetobject(&obj);
}

// �R���X�g���N�^
ObjectInfo::ObjectInfo(HSQUIRRELVM v, int idx)
{
	sq_resetobject(&obj);
	HSQUIRRELVM gv = getGlobalVM();
	sq_move(gv, v, idx);
	sq_getstackobj(gv, -1, &obj);
	sq_addref(gv, &obj);
	sq_pop(gv, 1);
}

// �R�s�[�R���X�g���N�^
ObjectInfo::ObjectInfo(const ObjectInfo &orig)
{
	HSQUIRRELVM gv = getGlobalVM();
	sq_resetobject(&obj);
	obj = orig.obj;
	sq_addref(gv, &obj);
}

// ���
ObjectInfo& ObjectInfo::operator =(const ObjectInfo &orig)
{
	HSQUIRRELVM gv = getGlobalVM();
	clear();
	obj = orig.obj;
	sq_addref(gv, &obj);
	return *this;
}

// �f�X�g���N�^
ObjectInfo::~ObjectInfo()
{
	clear();
}

// null���H
bool
ObjectInfo::isNull() const
{
	if (sq_isweakref(obj)) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_getweakrefval(gv, -1);
		bool ret = sq_gettype(gv, -1) == OT_NULL;
		sq_pop(gv, 2);
		return ret;
	}
	return sq_isnull(obj);
}

// �����X���b�h���H
bool
ObjectInfo::isSameThread(const HSQUIRRELVM v) const
{
	return sq_isthread(obj) && obj._unVal.pThread == v;
}

// �X���b�h���擾
ObjectInfo::operator HSQUIRRELVM() const
{
	HSQUIRRELVM vm = sq_isthread(obj) ? obj._unVal.pThread : NULL;
	return vm;
}
	
// �I�u�W�F�N�g��PUSH
void
ObjectInfo::push(HSQUIRRELVM v) const
{
    if (sq_isweakref(obj)) {
		sq_pushobject(v, obj);
		sq_getweakrefval(v, -1);
		sq_remove(v, -2);
	} else {
		sq_pushobject(v, obj);
	}
}

/// ������o�^(���s������null��o�^
void
ObjectInfo::pushClone(HSQUIRRELVM v) const
{
	push(v);
	if (!SQ_SUCCEEDED(sq_clone(v, -1))) {
		sq_pushnull(v);
	}
	sq_remove(v, -2);
}

// ---------------------------------------------------
// delegate �����p
// ---------------------------------------------------

// delegate �Ƃ��ċ@�\���邩�ǂ���
bool
ObjectInfo::isDelegate() const
{
	return (sq_isinstance(obj) || sq_istable(obj));
}

// bindenv �����邩�ǂ���
bool
ObjectInfo::isBindDelegate() const
{
	return (sq_isinstance(obj));
}

// ---------------------------------------------------
// �f�[�^�擾
// ---------------------------------------------------

const SQChar *
ObjectInfo::getString()
{
	if (sq_isstring(obj)) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		const SQChar *mystr;
		sq_getstring(gv, -1, &mystr);
		sq_pop(gv, 1);
		return mystr;
	}
	return NULL;
}

// ---------------------------------------------------
// wait�����p���\�b�h
// ---------------------------------------------------

bool
ObjectInfo::isSameString(const SQChar *str) const
{
	if (str && sq_isstring(obj)) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		const SQChar *mystr;
		sq_getstring(gv, -1, &mystr);
		sq_pop(gv, 1);
		return mystr && scstrcmp(str, mystr) == 0;
	}
	return false;
}

// ---------------------------------------------------
// �z�񏈗��p���\�b�h
// ---------------------------------------------------


/// �z��Ƃ��ď�����
void
ObjectInfo::initArray(int size)
{
	clear();
	HSQUIRRELVM gv = getGlobalVM();
	sq_newarray(gv, size);
	sq_getstackobj(gv, -1, &obj);
	sq_addref(gv, &obj);
	sq_pop(gv, 1);
}

/// �z��Ƃ��ď�����
void
ObjectInfo::initTable()
{
	clear();
	HSQUIRRELVM gv = getGlobalVM();
	sq_newtable(gv);
	sq_getstackobj(gv, -1, &obj);
	sq_addref(gv, &obj);
	sq_pop(gv, 1);
}

/// �z��ɒl��ǉ�
void ObjectInfo::append(HSQUIRRELVM v, int idx)
{
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	sq_move(gv, v, idx);
	sq_arrayappend(gv, -2);
	sq_pop(gv,1);
}

/// �z��ɔz���ǉ�
void ObjectInfo::appendArray(ObjectInfo &array)
{
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	int max = array.len();
	for (int i=0;i<max;i++) {
		array.pushData(gv, i);
		sq_arrayappend(gv, -2);
	}
	sq_pop(gv,1);
}

/// �z��̒���
int
ObjectInfo::len() const
{
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	int ret = sq_getsize(gv,-1);
	sq_pop(gv,1);
	return ret;
}

/**
 * �z��̓��e��S��PUSH
 * @param v squirrelVM
 * @return push ������
 */
int
ObjectInfo::pushArray(HSQUIRRELVM v) const
{
	if (!isArray()) {
		return 0;
	}
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	int len = sq_getsize(gv,-1);
	for (int i=0;i<len;i++) {
		sq_pushinteger(gv, i);
		if (SQ_SUCCEEDED(sq_get(gv, -2))) {
			sq_move(v, gv, -1);
			sq_pop(gv, 1);
		}
	}
	sq_pop(gv,1);
	return len;
}

// ---------------------------------------------------
// �֐������p���\�b�h
// ---------------------------------------------------

SQRESULT ObjectInfo::call()
{
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	sq_pushroottable(gv); // root
	SQRESULT ret = sq_call(gv, 1, SQFalse, SQTrue);
	sq_pop(gv, 1);
	return ret;
}

// -------------------------------------------------------------
// �X���b�h�n����p
// -------------------------------------------------------------

void
ObjectInfo::addWait(ObjectInfo &thread)
{
	Object *object = getObject();
	if (object) {
		object->_addWait(thread);
	}
}

void
ObjectInfo::removeWait(sqobject::ObjectInfo &thread)
{
	Object *object = getObject();
	if (object) {
		object->_removeWait(thread);
	}
}

bool
ObjectInfo::notifyObject(ObjectInfo &target)
{
	Thread *thread = getThread();
	return thread ? thread->_notifyObject(target) : false;
}

bool
ObjectInfo::notifyTrigger(const SQChar *name)
{
	Thread *thread = getThread();
	return thread ? thread->_notifyTrigger(name) : false;
}

};
