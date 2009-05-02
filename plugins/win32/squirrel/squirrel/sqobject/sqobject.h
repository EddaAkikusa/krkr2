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

	/**
	 * ���ȎQ�Ə������p
	 * �f�t�H���g�R���X�g���N�^�ŏ��������ꍇ�͕K�����̏�������Ԃ���
	 * @param v SQUIRREL vm
	 * @param idx �����̃I�u�W�F�N�g������C���f�b�N�X
	 */
	void initSelf(HSQUIRRELVM v, int idx=1);

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


protected:

	/**
	 * ���ȃI�u�W�F�N�g�C�x���g�Ăяo���i��������)
	 * @param eventName �C�x���g��
	 */
	SQRESULT callEvent(const SQChar *eventName) {
		return self.callMethod(eventName);
	}

	/**
	 * ���ȃI�u�W�F�N�g�C�x���g�Ăяo���i����1��)
	 * @param eventName �C�x���g��
	 * @param p1 ����
	 */
	template<typename T1> SQRESULT callEvent(const SQChar *eventName, T1 p1) {
		return self.callMethod(eventName, p1);
	}
	
	/**
	 * ���ȃI�u�W�F�N�g�C�x���g�Ăяo���i����2��)
	 * @param eventName �C�x���g��
	 * @param p1 ����
	 * @param p2 ����2
	 */
	template<typename T1, typename T2> SQRESULT callEvent(const SQChar *eventName, T1 p1, T2 p2) {
		return self.callMethod(eventName, p1, p2);
	}
	
	/**
	 * �Ԓl�L�莩�ȃI�u�W�F�N�g�C�x���g�Ăяo���i��������)
	 * @param r �A��l�|�C���^
	 * @param eventName �C�x���g��
	 */
	template<typename R> SQRESULT callEventResult(R* r, const SQChar *eventName) {
		return self.callMethodResult(r, eventName);
	}

	/**
	 * �Ԓl���莩�ȃI�u�W�F�N�g�C�x���g�Ăяo���i����1��)
	 * @param r �A��l�|�C���^
	 * @param eventName �C�x���g��
	 * @param p1 ����
	 */
	template<typename R, typename T1> SQRESULT callEventResult(R* r, const SQChar *eventName, T1 p1) {
		return self.callMethodResult(r, eventName, p1);
	}
	
	/**
	 * �Ԓl�L�莩�ȃI�u�W�F�N�g�C�x���g�Ăяo���i����2��)
	 * @param r �A��l�|�C���^
	 * @param eventName �C�x���g��
	 * @param p1 ����
	 * @param p2 ����2
	 */
	template<typename R, typename T1, typename T2> SQRESULT callEventResult(R* r, const SQChar *eventName, T1 p1, T2 p2) {
		return self.callMethodResult(r, eventName, p1, p2);
	}

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
