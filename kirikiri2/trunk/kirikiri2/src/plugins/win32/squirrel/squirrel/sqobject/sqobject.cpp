/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */
#include "sqthread.h"

#include <string.h>
#include <ctype.h>
#include <string>
#include <new>

namespace sqobject {

/**
 * �N���[�W�����ǂ���
 */
static bool
isClosure(SQObjectType type)
{
	return type == OT_CLOSURE || type == OT_NATIVECLOSURE;
}


/**
 * ������擾�p
 * @param v VM
 * @param idx �C���f�b�N�X
 * @return ������
 */
const SQChar *getString(HSQUIRRELVM v, int idx) {
	const SQChar *x = NULL;
	sq_getstring(v, idx, &x);
	return x;
};

static SQRESULT ERROR_NOMEMBER(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("no such member"));
}

// setter���O����
void getSetterName(tstring &store, const SQChar *name)
{
	store = _SC("set");
	store += toupper(*name);
	store += (name + 1);
}

// getter���O����
void getGetterName(tstring &store, const SQChar *name)
{
	store = _SC("get");
	store += toupper(*name);
	store += (name + 1);
}

// ---------------------------------------------------------
// Object
// ---------------------------------------------------------

/**
 * �I�u�W�F�N�g�҂��̓o�^
 * @param thread �X���b�h
 */
void
Object::addWait(Thread *thread)
{
	_waitThreadList.push_back(thread);
}

/**
 * �I�u�W�F�N�g�҂��̉���
 * @param thread �X���b�h
 */
void
Object::removeWait(Thread *thread)
{
	std::vector<Thread*>::iterator i = _waitThreadList.begin();
	while (i != _waitThreadList.end()) {
		if (*i == thread) {
			i = _waitThreadList.erase(i);
		} else {
			i++;	
		}
	}
}

/**
 * �R���X�g���N�^
 */
Object::Object()
{
}

/**
 * �R���X�g���N�^
 */
Object::Object(HSQUIRRELVM v, int delegateIdx)
{
	if (sq_gettop(v) >= delegateIdx) {
		delegate.getStack(v, delegateIdx);
	}
}

/**
 * �f�X�g���N�^
 */
Object::~Object()
{
	notifyAll();
}

/**
 * ���̃I�u�W�F�N�g��҂��Ă���P�X���b�h�̑҂�������
 */
void
Object::notify()
{
	std::vector <Thread *>::iterator i = _waitThreadList.begin();
	while (i != _waitThreadList.end()) {
		if ((*i)->notifyObject(this)) {
			i = _waitThreadList.erase(i);
			return;
		} else {
			i++;
		}
	}
}
	
/**
 * ���̃I�u�W�F�N�g��҂��Ă���S�X���b�h�̑҂�������
 */
void
Object::notifyAll()
{
	std::vector <Thread *>::iterator i = _waitThreadList.begin();
	while (i != _waitThreadList.end()) {
		(*i)->notifyObject(this);
		i = _waitThreadList.erase(i);
	}
}

/**
 * �v���p�e�B����l���擾
 * @param name �v���p�e�B��
 * @return �v���p�e�B�l
 */
SQRESULT
Object::_get(HSQUIRRELVM v)
{
	SQRESULT result = SQ_OK;
	const SQChar *name = getString(v, 2);
	if (name && *name) {
		// delegate�̎Q��
		if (delegate.isDelegate()) {
			delegate.push(v);
			sq_pushstring(v, name, -1);
			if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
				// ���\�b�h�̏ꍇ�͑�������
				if (isClosure(sq_gettype(v,-1)) && delegate.isBindDelegate()) {
					delegate.push(v);
					if (SQ_SUCCEEDED(sq_bindenv(v, -2))) {
						sq_remove(v, -2); // ���̃N���[�W��
					}
				}
				sq_remove(v, -2);
				return 1;
			} else {
				sq_pop(v,1); // delegate
			}
		}
		
		// getter ��T���ăA�N�Z�X
		tstring name2;
		getGetterName(name2, name);
		sq_push(v, 1); // self
		sq_pushstring(v, name2.c_str(), -1);
		if (SQ_SUCCEEDED(result = sq_rawget(v,-2))) {
			sq_push(v, 1); //  self;
			if (SQ_SUCCEEDED(result = sq_call(v,1,SQTrue,SQTrue))) {
				//sqprintf("�Ăяo������:%s\n", name);
				sq_remove(v, -2); // func
				sq_remove(v, -2); // self
				return 1;
			} else {
				sq_pop(v,2); // func, self
			}
		} else {
			sq_pop(v, 1); // self
#if 1
			// �O���[�o���ϐ����Q��
			sq_pushroottable(v);
			sq_pushstring(v, name, -1);
			if (SQ_SUCCEEDED(sq_rawget(v,-2))) {
				sq_remove(v, -2); // root
				return 1;
			} else {
				sq_pop(v,1);
			}
#endif
		}
	}
	return ERROR_NOMEMBER(v);
}

/**
 * �v���p�e�B�ɒl��ݒ�
 * @param name �v���p�e�B��
 * @param value �v���p�e�B�l
 */
SQRESULT
Object::_set(HSQUIRRELVM v)
{
	SQRESULT result = SQ_OK;
	const SQChar *name = getString(v, 2);
	if (name && *name) {
		// delegate�̎Q��
		if (delegate.isDelegate()) {
			delegate.push(v);
			sq_push(v, 2); // name
			sq_push(v, 3); // value
			if (SQ_SUCCEEDED(result = sq_set(v,-3))) {
				sq_pop(v,1); // delegate
				return SQ_OK;
			} else {
				sq_pop(v,1); // delegate
			}
		}
		
		// setter ��T���ăA�N�Z�X
		tstring name2;
		getSetterName(name2, name);
		
		sq_push(v, 1); // self
		sq_pushstring(v, name2.c_str(), -1);
		if (SQ_SUCCEEDED(result = sq_rawget(v,-2))) {
			sq_push(v, 1); // self
			sq_push(v, 3); // value
			if (SQ_SUCCEEDED(result = sq_call(v,2,SQFalse,SQTrue))) {
				//sqprintf("�Ăяo������:%s\n", name);
				sq_pop(v,2); // func, self
				return SQ_OK;
			} else {
				sq_pop(v,2); // func, self
			}
		}
		
	}
	//return result;
	return ERROR_NOMEMBER(v);
}

/**
 * set�v���p�e�B�̑��݊m�F
 * @param name �v���p�e�B��
 * @return set�v���p�e�B�����݂����� true
 */
SQRESULT
Object::hasSetProp(HSQUIRRELVM v)
{
	SQRESULT result = SQ_OK;
	SQBool ret = SQFalse;
	if (sq_gettop(v) > 1) {
		const SQChar *name = getString(v, 2);
		if (name && *name) {
			tstring name2;
			getSetterName(name2, name);
			sq_push(v, 1); // object
			sq_pushstring(v, name2.c_str(), -1);
			if (SQ_SUCCEEDED(result = sq_rawget(v,-2))) {
				sq_pop(v,1);
				ret = SQTrue;
			} else {
				sq_pushstring(v, name, -1);
				if (SQ_SUCCEEDED(result = sq_rawget(v,-2))) {
					sq_pop(v,1);
					ret = SQTrue;
				}
			}
			sq_pop(v,1); // object
		}
	}
	if (SQ_SUCCEEDED(result)) {
		sq_pushbool(v, ret);
		return 1;
	} else {
		return result;
	}
}

/**
 * �Ϗ��̐ݒ�
 */
SQRESULT
Object::setDelegate(HSQUIRRELVM v)
{
	if (sq_gettop(v) > 1) {
		delegate.getStack(v,2);
	} else {
		delegate.clear();
	}
	return SQ_OK;
}

/**
 * �Ϗ��̐ݒ�
 */
SQRESULT
Object::getDelegate(HSQUIRRELVM v)
{
	delegate.push(v);
	return 1;
}

};

