/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */

#ifndef __SQOBJECTINFO_H__
#define __SQOBJECTINFO_H__

#include <squirrel.h>

namespace sqobject {

/// ������
HSQUIRRELVM init();
/// �I��
extern void done();
/// ���ێ��p�O���[�o��VM�̎擾
extern HSQUIRRELVM getGlobalVM();

class Object;
class Thread;
	
// ---------------------------------------------------------
// ObjectInfo
// ---------------------------------------------------------

/**
 * squirrel �I�u�W�F�N�g�ێ��p�N���X
 * ��Q�Ƃ��ێ��\
 */
class ObjectInfo {

public:
	/// ��r
	bool operator ==(const ObjectInfo& o);

	// ���e����
	void clear();

	// �X�^�b�N����擾
	void getStack(HSQUIRRELVM v, int idx);

    // �X�^�b�N�����Q�ƂƂ��Ď擾
	void getStackWeak(HSQUIRRELVM v, int idx);
  
	// �R���X�g���N�^
	ObjectInfo();

	// �R���X�g���N�^
	ObjectInfo(HSQUIRRELVM v, int idx);

	// �R�s�[�R���X�g���N�^
	ObjectInfo(const ObjectInfo &orig);

	// ���
	ObjectInfo & operator=(const ObjectInfo &orig);

	// �f�X�g���N�^
	virtual ~ObjectInfo();

	// null ���H
	bool isNull() const;
	
	// �����X���b�h���H
	bool isSameThread(const HSQUIRRELVM v) const;

	// �X���b�h���擾
	operator HSQUIRRELVM() const;
	
	// �I�u�W�F�N�g��PUSH
	void push(HSQUIRRELVM v) const;

	// ---------------------------------------------------
	// delegate �����p
	// ---------------------------------------------------

	// delegate �Ƃ��ċ@�\���邩�ǂ���
	bool isDelegate() const;

	// bindenv �����邩�ǂ���
	bool isBindDelegate() const;

	// ---------------------------------------------------
	// �f�[�^�擾
	// ---------------------------------------------------

	const SQChar *getString();
	
	// ---------------------------------------------------
	// �I�u�W�F�N�g�擾
	// ---------------------------------------------------

	// Thread �̃C���X�^���X���[�U�|�C���^���擾
	Thread *getThread();
	
	// Object �̃C���X�^���X���[�U�|�C���^���擾
	Object *getObject();
	
	// ---------------------------------------------------
	// wait�����p���\�b�h
	// ---------------------------------------------------

	bool isSameString(const SQChar *str) const;

	// ---------------------------------------------------
	// �z�񏈗��p
	// ---------------------------------------------------

	/// �z��Ƃ��ď�����
	void initArray(int size=0);

	/// @return �z��Ȃ� true
	bool isArray() const { return sq_isarray(obj); }

	/// �z��ɒl��ǉ�
	void append(HSQUIRRELVM v, int idx);
	
	/// �z��ɒl��ǉ�
	template<typename T>
	void append(T value) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		pushValue(gv, value);
		sq_arrayappend(gv, -2);
		sq_pop(gv,1);
	}

	/// �z��ɒl��}��
	template<typename T>
	void insert(int index, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		pushValue(gv, value);
		sq_arrayinsert(gv, -2, index);
		sq_pop(gv,1);
	}

	/// �z��ɒl���i�[
	template<typename T>
	void set(int index, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_pushinteger(gv, index);
		pushValue(gv, value);
		sq_set(gv, -3);
		sq_pop(gv,1);
	}

	/// @return �z��̒���
	int len() const;

	/**
	 * �z��̓��e��S��PUSH
	 * @param v squirrelVM
	 * @return push ������
	 */
	int pushArray(HSQUIRRELVM v) const;

	// ---------------------------------------------------
	// �֐������p
	// ---------------------------------------------------

	/// @return �z��Ȃ� true
	bool isClosure() const { return sq_isclosure(obj) || sq_isnativeclosure(obj); }

	// �Ăяo������
	SQRESULT call();

	template<typename T1> SQRESULT call(T1 p1) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_pushroottable(gv); // root
		pushValue(gv, p1);
		SQRESULT ret = sq_call(gv, 2, SQFalse, SQTrue);
		sq_pop(gv, 1);
		return ret;
	}

	template<typename T1, typename T2> SQRESULT call(T1 p1, T2 p2) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_pushroottable(gv); // root
		pushValue(gv, p1);
		pushValue(gv, p2);
		SQRESULT ret = sq_call(gv, 3, SQFalse, SQTrue);
		sq_pop(gv, 1);
		return ret;
	}
	
	template<typename R> SQRESULT call(R*) {
		SQRESULT ret;
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_pushroottable(gv); // root
		if (SQ_SUCCEEDED(ret = sq_call(gv, 1, SQTrue, SQTrue))) {
			getValue(gv, r);
			sq_pop(gv, 1);
		}
		sq_pop(gv, 1);
		return ret;
	}
	template<typename R, typename T1> SQRESULT call(R* r, T1 p1) {
		SQRESULT ret;
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_pushroottable(gv); // root
		pushValue(gv, p1);
		if (SQ_SUCCEEDED(ret = sq_call(gv, 2, SQTrue, SQTrue))) {
			getValue(gv, r);
			sq_pop(gv, 1);
		}
		sq_pop(gv, 1);
		return ret;
	}
	template<typename R, typename T1, typename T2> SQRESULT call(R* r, T1 p1, T2 p2) {
		SQRESULT ret;
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_pushroottable(gv); // root
		pushValue(gv, p1);
		pushValue(gv, p2);
		if (SQ_SUCCEEDED(ret = sq_call(gv, 3, SQTrue, SQTrue))) {
			getValue(gv, r);
			sq_pop(gv, 1);
		}
		sq_pop(gv, 1);
		return ret;
	}
	
protected:
	// ---------------------------------------------------
	// �X�^�b�N�����p
	// ---------------------------------------------------

	// �l�� push
	static void pushValue(HSQUIRRELVM v) {}
	static void pushValue(HSQUIRRELVM v, int value) { sq_pushinteger(v,value); }
	static void pushValue(HSQUIRRELVM v, const SQChar *value) { sq_pushstring(v,value,-1); }
	
	// �l�̎擾
	static void getValue(HSQUIRRELVM v, void *value) {}
	static void getValue(HSQUIRRELVM v, int *value) { sq_getinteger(v, -1, value); }
	static void getValue(HSQUIRRELVM v, const SQChar **value) { sq_getstring(v, -1, value); }
	
private:
	HSQOBJECT obj; // �I�u�W�F�N�g�Q�Ə��
};

};

#endif
