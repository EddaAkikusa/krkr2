/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */
#ifndef __SQOBJECT_H__
#define __SQOBJECT_H__

// �^��
#ifndef SQOBJECTNAME
#define SQOBJECTNAME _SC("Object")
#endif

#include "sqobjectinfo.h"

namespace sqobject {

const SQChar *getString(HSQUIRRELVM v, int idx);

/**
 * �I�u�W�F�N�g�p
 */
class Object {

protected:
	// ���ȎQ��
	ObjectInfo self;
	// ���̃I�u�W�F�N�g��҂��Ă�X���b�h�̈ꗗ
	ObjectInfo _waitThreadList;
	// delegate
	ObjectInfo delegate;

public:
	/**
	 * �I�u�W�F�N�g�҂��̓o�^
	 * @param thread �X���b�h
	 */
	void _addWait(ObjectInfo &thread);
	
	/**
	 * �I�u�W�F�N�g�҂��̉���
	 * @param thread �X���b�h
	 */
	void _removeWait(ObjectInfo &thread);
	
	/**
	 * �R���X�g���N�^
	 */
	Object();
	
	/**
	 * �R���X�g���N�^
	 */
	Object(HSQUIRRELVM v, int delegateIdx=2);

	/**
	 * �f�X�g���N�^
	 */
	virtual ~Object();

public:
	
	// ------------------------------------------------------------------

	/**
	 * ���̃I�u�W�F�N�g��҂��Ă���P�X���b�h�̑҂�������
	 */
	void notify();
	
	/**
	 * ���̃I�u�W�F�N�g��҂��Ă���S�X���b�h�̑҂�������
	 */
	void notifyAll();
	
	/**
	 * �v���p�e�B����l���擾
	 * @param name �v���p�e�B��
	 * @return �v���p�e�B�l
	 */
	SQRESULT _get(HSQUIRRELVM v);

	/**
	 * �v���p�e�B�ɒl��ݒ�
	 * @param name �v���p�e�B��
	 * @param value �v���p�e�B�l
	 */
	SQRESULT _set(HSQUIRRELVM v);

	/**
	 * set�v���p�e�B�̑��݊m�F
	 * @param name �v���p�e�B��
	 * @return set�v���p�e�B�����݂����� true
	 */
	SQRESULT hasSetProp(HSQUIRRELVM v);
	
	/**
	 * �Ϗ��̐ݒ�
	 */
	SQRESULT setDelegate(HSQUIRRELVM v);

	/**
	 * �Ϗ��̎擾
	 */
	SQRESULT getDelegate(HSQUIRRELVM v);

public:
	/**
	 * squirrel �N���X�o�^
	 */
	static void registerClass();
};

};// namespace

// ���O�o�͗p
#define SQPRINT(v,msg) {\
	SQPRINTFUNCTION print = sq_getprintfunc(v);\
	if (print) {\
		print(v,msg);\
	}\
}

#endif
