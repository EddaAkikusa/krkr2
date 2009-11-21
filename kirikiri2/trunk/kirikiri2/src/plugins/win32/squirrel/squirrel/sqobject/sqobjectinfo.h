/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */

#ifndef __SQOBJECTINFO_H__
#define __SQOBJECTINFO_H__

#include <stdio.h>
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
	void getStack(HSQUIRRELVM v, SQInteger idx);

    // �X�^�b�N�����Q�ƂƂ��Ď擾
	void getStackWeak(HSQUIRRELVM v, SQInteger idx);
  
	// �R���X�g���N�^
	ObjectInfo();

	// �R���X�g���N�^
	ObjectInfo(HSQUIRRELVM v, SQInteger idx);

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

	/// ������o�^(���s������NULL)
	void pushClone(HSQUIRRELVM v) const;
	
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
	// �z��E���������p
	// ---------------------------------------------------

	/// �z��Ƃ��ď�����
	void initArray(SQInteger size=0);
	
	/// @return �z��Ȃ� true
	bool isArray() const { return sq_isarray(obj); }

	/// �z��Ƃ��ď�����
	void initTable();

	/// @return �z��Ȃ� true
	bool isTable() const { return sq_istable(obj); }
	
	/// �z��ɒl��ǉ�
	void append(HSQUIRRELVM v, SQInteger idx);

	/// �z��ɔz���ǉ�
	void appendArray(ObjectInfo &array);
	
	/// �z��ɒl��ǉ�
	template<typename T>
	void append(T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, value);
		sq_arrayappend(gv, -2);
		sq_pop(gv,1);
	}

	/// �z��ɒl��}��
	template<typename T>
	void insert(SQInteger index, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, value);
		sq_arrayinsert(gv, -2, index);
		sq_pop(gv,1);
	}

	/// �z��̒l���폜
	void remove(SQInteger index) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		sq_arrayremove(gv, -1, index);
		sq_pop(gv,1);
	}
	
	/// �z��/�����ɒl���i�[
	template<typename K, typename T>
	void set(K key, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, key);
		pushValue(gv, value);
		sq_set(gv, -3);
		sq_pop(gv,1);
	}

	/// �����ɒl��V�K�i�[
	template<typename K, typename T>
	void create(K key, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, key);
		pushValue(gv, value);
		sq_createslot(gv, -3);
		sq_pop(gv,1);
	}

	/// ��������l���폜
	template<typename K>
	void deleteslot(K key) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, key);
		sq_deleteslot(gv, -2, SQFalse);
		sq_pop(gv,1);
	}
	
	/// �z��/�����̒l���擾
	template<typename K>
	ObjectInfo get(K key) {
		HSQUIRRELVM gv = getGlobalVM();
		pushData(gv, key);
		ObjectInfo ret(gv, -1);
		sq_pop(gv,1);
		return ret;
	}

	/// �z��/�����̒l���擾�B�݂���Ȃ����null
	template<typename K, typename T>
	SQRESULT get(K key, T *value) {
		HSQUIRRELVM gv = getGlobalVM();
		pushData(gv, key);
		SQRESULT ret = getValue(gv, value);
		sq_pop(gv,1);
		return ret;
	}
	
	/// �z��̒l��push�B������Ȃ����null
	template<typename T>
	void pushData(HSQUIRRELVM v, T key) const {
		push(v);
		pushValue(v, key);
		if (SQ_FAILED(sq_get(v, -2))) {
			sq_pushnull(v);
		}
		sq_remove(v,-2);
	}
	
	/// �z��E�����̒��g���N���A
	void clearData() {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		sq_clear(gv,-1);
		sq_pop(gv,1);
	}

	/// @return �z��̒���
	SQInteger len() const;

	/**
	 * �z��̓��e��S��PUSH
	 * @param v squirrelVM
	 * @return push ������
	 */
	SQInteger pushArray(HSQUIRRELVM v) const;

	// ---------------------------------------------------
	// �֐������p
	// ---------------------------------------------------

	/// @return �z��Ȃ� true
	bool isClosure() const { return sq_isclosure(obj) || sq_isnativeclosure(obj); }

	// �Ăяo������
	SQRESULT call(ObjectInfo *self=NULL);

	template<typename T1> SQRESULT call(T1 p1, ObjectInfo *self=NULL) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		if (self) {
			self->push(gv);
		} else {
			sq_pushroottable(gv); // root
		}
		pushValue(gv, p1);
		SQRESULT ret = sq_call(gv, 2, SQFalse, SQTrue);
		sq_pop(gv, 1);
		return ret;
	}

	template<typename T1, typename T2> SQRESULT call(T1 p1, T2 p2, ObjectInfo *self=NULL) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		if (self) {
			self->push(gv);
		} else {
			sq_pushroottable(gv); // root
		}
		pushValue(gv, p1);
		pushValue(gv, p2);
		SQRESULT ret = sq_call(gv, 3, SQFalse, SQTrue);
		sq_pop(gv, 1);
		return ret;
	}
	
	template<typename R> SQRESULT callResult(R* r, ObjectInfo *self=NULL) {
		SQRESULT ret;
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		if (self) {
			self->push(gv);
		} else {
			sq_pushroottable(gv); // root
		}
		if (SQ_SUCCEEDED(ret = sq_call(gv, 1, SQTrue, SQTrue))) {
			ret = getValue(gv, r);
			sq_pop(gv, 1);
		}
		sq_pop(gv, 1);
		return ret;
	}
	template<typename R, typename T1> SQRESULT callResult(R* r, T1 p1, ObjectInfo *self=NULL) {
		SQRESULT ret;
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		if (self) {
			self->push(gv);
		} else {
			sq_pushroottable(gv); // root
		}
		pushValue(gv, p1);
		if (SQ_SUCCEEDED(ret = sq_call(gv, 2, SQTrue, SQTrue))) {
			ret = getValue(gv, r);
			sq_pop(gv, 1);
		}
		sq_pop(gv, 1);
		return ret;
	}
	template<typename R, typename T1, typename T2> SQRESULT callResult(R* r, T1 p1, T2 p2, ObjectInfo *self=NULL) {
		SQRESULT ret;
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		if (self) {
			self->push(gv);
		} else {
			sq_pushroottable(gv); // root
		}
		pushValue(gv, p1);
		pushValue(gv, p2);
		if (SQ_SUCCEEDED(ret = sq_call(gv, 3, SQTrue, SQTrue))) {
			ret = getValue(gv, r);
			sq_pop(gv, 1);
		}
		sq_pop(gv, 1);
		return ret;
	}

	/**
	 * ���ȃI�u�W�F�N�g���\�b�h�Ăяo���i��������)
	 * @param methodName ���\�b�h��
	 */
	SQRESULT callMethod(const SQChar *methodName) {
		if (!isNull()) {
			ObjectInfo method = get(methodName);
			if (method.isClosure()) {
				return method.call(this);
			}
		}
		return SQ_ERROR;
	}

	/**
	 * ���ȃI�u�W�F�N�g���\�b�h�Ăяo���i����1��)
	 * @param methodName ���\�b�h��
	 * @param p1 ����
	 */
	template<typename T1> SQRESULT callMethod(const SQChar *methodName, T1 p1) {
		if (!isNull()) {
			ObjectInfo method = get(methodName);
			if (method.isClosure()) {
				return method.call(p1, this);
			}
		}
		return SQ_ERROR;
	}
	
	/**
	 * ���ȃI�u�W�F�N�g���\�b�h�Ăяo���i����2��)
	 * @param methodName ���\�b�h��
	 * @param p1 ����
	 * @param p2 ����2
	 */
	template<typename T1, typename T2> SQRESULT callMethod(const SQChar *methodName, T1 p1, T2 p2) {
		if (!isNull()) {
			ObjectInfo method = get(methodName);
			if (method.isClosure()) {
				return method.call(p1, p2, this);
			}
		}
		return SQ_ERROR;
	}
	
	/**
	 * �Ԓl�L�莩�ȃI�u�W�F�N�g���\�b�h�Ăяo���i��������)
	 * @param r �A��l�|�C���^
	 * @param methodName ���\�b�h��
	 */
	template<typename R> SQRESULT callMethodResult(R* r, const SQChar *methodName) {
		if (!isNull()) {
			ObjectInfo method = get(methodName);
			if (method.isClosure()) {
				return method.call(r, this);
			}
		}
		return SQ_ERROR;
	}

	/**
	 * �Ԓl���莩�ȃI�u�W�F�N�g���\�b�h�Ăяo���i����1��)
	 * @param r �A��l�|�C���^
	 * @param methodName ���\�b�h��
	 * @param p1 ����
	 */
	template<typename R, typename T1> SQRESULT callMethodResult(R* r, const SQChar *methodName, T1 p1) {
		if (!isNull()) {
			ObjectInfo method = get(methodName);
			if (method.isClosure()) {
				return method.call(r, p1, this);
			}
		}
		return SQ_ERROR;
	}
	
	/**
	 * �Ԓl�L�莩�ȃI�u�W�F�N�g���\�b�h�Ăяo���i����2��)
	 * @param r �A��l�|�C���^
	 * @param methodName ���\�b�h��
	 * @param p1 ����
	 * @param p2 ����2
	 */
	template<typename R, typename T1, typename T2> SQRESULT callMethodResult(R* r, const SQChar *methodName, T1 p1, T2 p2) {
		if (!isNull()) {
			ObjectInfo method = get(methodName);
			if (method.isClosure()) {
				return method.call(r, p1, p2, this);
			}
		}
		return SQ_ERROR;
	}

	// ---------------------------------------------------
	// �N���X�����p
	// ---------------------------------------------------

	// �N���X�I�u�W�F�N�g��
	bool isClass() const;

protected:
	// ---------------------------------------------------
	// �X�^�b�N�����p
	// ---------------------------------------------------

	// �l�� push
	static void pushValue(HSQUIRRELVM v) {}
	static void pushValue(HSQUIRRELVM v, SQInteger value) { sq_pushinteger(v,value); }
	static void pushValue(HSQUIRRELVM v, SQFloat value) { sq_pushfloat(v,value); }
	static void pushValue(HSQUIRRELVM v, const SQChar *value) { sq_pushstring(v,value,-1); }
	static void pushValue(HSQUIRRELVM v, SQUserPointer value) { sq_pushuserpointer(v,value); }
	static void pushValue(HSQUIRRELVM v, ObjectInfo &obj) { obj.push(v); }
	
	// �l�̎擾
	static SQRESULT getValue(HSQUIRRELVM v, void *value) { return SQ_OK; }
	static SQRESULT getValue(HSQUIRRELVM v, SQInteger *value) { return sq_getinteger(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, SQFloat *value) { return sq_getfloat(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, const SQChar **value) { return sq_getstring(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, SQUserPointer *value) { return sq_getuserpointer(v, -1, value); }
	
private:
	HSQOBJECT obj; // �I�u�W�F�N�g�Q�Ə��

	// -------------------------------------------------------------
	// �X���b�h�n����p
	// -------------------------------------------------------------

public:
	// Object
	void addWait(ObjectInfo &thread);
	void removeWait(ObjectInfo &thread);

	// Thread
	bool notifyTrigger(const SQChar *name);
	bool notifyObject(ObjectInfo &target);
};

};

#endif
