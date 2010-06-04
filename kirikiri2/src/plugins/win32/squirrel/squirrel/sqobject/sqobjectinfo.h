/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */

#ifndef __SQOBJECTINFO_H__
#define __SQOBJECTINFO_H__

#include <stdio.h>
#include <squirrel.h>
#include <string>

#ifdef USESQPLUS
#include <sqplus.h>
#else
#ifdef USESQRAT
#include <sqrat.h>
#else
#include "sqfunc.h"
#endif
#endif

namespace sqobject {

class Object;

/// ������
HSQUIRRELVM init();
/// �I��
extern void done();
/// ���ێ��p�O���[�o��VM�̎擾
extern HSQUIRRELVM getGlobalVM();

// ---------------------------------------------------------
// ObjectInfo
// ---------------------------------------------------------

/**
 * squirrel �I�u�W�F�N�g�ێ��p�N���X
 * ��Q�Ƃ��ێ��\
 */
class ObjectInfo {

public:
	
	// roottable �̎擾
	static ObjectInfo getRoot();
	static ObjectInfo createArray(SQInteger size=0);
	static ObjectInfo createTable();

	/// ��r
	bool operator ==(const ObjectInfo& o);
	bool operator !=(const ObjectInfo& o);
	bool operator <(const ObjectInfo& o);
	bool operator <=(const ObjectInfo& o);
	bool operator >(const ObjectInfo& o);
	bool operator >=(const ObjectInfo& o);

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

	// �C�ӌ^�̃R���X�g���N�^
	template<typename T>
	ObjectInfo(T value) {
		setValue(value);
	}

	// �C�ӌ^�̑��
	template<typename T>
	ObjectInfo & operator=(T value) {
		setValue(value);
		return *this;
	}

	// �C�ӌ^�ւ̃L���X�g
	// �擾�ł��Ȃ������ꍇ�̓N���A�l�ɂȂ�
	template<typename T>
	operator T() const
	{
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		T value;
		if (SQ_FAILED(getValue(gv, &value))) {
			clearValue(&value);
		}
		sq_pop(gv, 1);
		return value;
	}
	
	// �l�̐ݒ�
	template<typename T>
	void setValue(T value) {
		HSQUIRRELVM gv = getGlobalVM();
		pushValue(gv, value);
		getStack(gv, -1);
		sq_pop(gv, 1);
	}
	
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
	SQRESULT append(HSQUIRRELVM v, SQInteger idx);

	/// �z��ɔz���ǉ�
	SQRESULT appendArray(ObjectInfo &array);
	
	/// �z��ɒl��ǉ�
	template<typename T>
	SQRESULT append(T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, value);
		SQRESULT ret = sq_arrayappend(gv, -2);
		sq_pop(gv,1);
		return ret;
	}

	/// �z��ɒl��}��
	template<typename T>
	SQRESULT insert(SQInteger index, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, value);
		SQRESULT ret = sq_arrayinsert(gv, -2, index);
		sq_pop(gv,1);
		return ret;
	}

	/// �z��̒l���폜
	SQRESULT remove(SQInteger index) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		SQRESULT ret = sq_arrayremove(gv, -1, index);
		sq_pop(gv,1);
		return ret;
	}
	
	/// �z��/�����ɒl���i�[
	template<typename K, typename T>
	SQRESULT set(K key, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, key);
		pushValue(gv, value);
		SQRESULT ret = sq_set(gv, -3);
		sq_pop(gv,1);
		return ret;
	}

	/// �����ɒl��V�K�i�[
	template<typename K, typename T>
	SQRESULT create(K key, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, key);
		pushValue(gv, value);
		SQRESULT ret = sq_newslot(gv, -3, SQFalse);
		sq_pop(gv,1);
		return ret;
	}

	/// ��������l���폜
	template<typename K>
	SQRESULT deleteslot(K key) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, key);
		SQRESULT ret = sq_deleteslot(gv, -2, SQFalse);
		sq_pop(gv,1);
		return ret;
	}
	
	/// �z��/�����̒l���擾
	template<typename K>
	ObjectInfo get(K key) const {
		HSQUIRRELVM gv = getGlobalVM();
		pushData(gv, key);
		ObjectInfo ret(gv, -1);
		sq_pop(gv,1);
		return ret;
	}

	/**
	 * �z��Q�Ɨp
	 */
	class ObjectInfoReference {

	public:
		// �R���X�g���N�^
		ObjectInfoReference(ObjectInfo *obj, int key) : obj(obj), intKey(key), type(0) {};
		ObjectInfoReference(ObjectInfo *obj, const SQChar *key) : obj(obj), strKey(key), type(1) {};
		ObjectInfoReference(ObjectInfo *obj, std::basic_string<SQChar> &key) : obj(obj), strKey(key.c_str()), type(1) {};
		
		// �C�ӌ^�̑��
		// �ݒ�ł��Ȃ������ꍇ�Ŏ����̏ꍇ�͍쐬���Ă��܂�
		template<typename T>
		const T &operator=(const T &value) {
			if (obj->type() == OT_ARRAY) {
				if (type == 0) {
					// �T�C�Y���͂��ĂȂ��̂� null �ł��߂�
					if (obj->len() <= intKey) {
						HSQUIRRELVM gv = getGlobalVM();
						obj->push(gv);
						while (obj->len() <= intKey) {
							sq_pushnull(gv);
							sq_arrayappend(gv, -2);
						}
						sq_pop(gv,1);
					}
					obj->set(intKey, value);
				}
			} else {
				SQRESULT ret;
				if (type == 0) {
					ret = obj->set(intKey, value);
				} else {
					ret = obj->set(strKey, value);
				}
				if (SQ_FAILED(ret)) {
					if (obj->type() == OT_TABLE || obj->type() == OT_CLASS) {
						if (type == 0) {
							obj->create(intKey, value);
						} else {
							obj->create(strKey, value);
						}
					}
				}
			}
			return value;
		}
		
		// �C�ӌ^�ւ̃L���X�g
		// �ϊ��ł��Ȃ������ꍇ�͏����l(0)�ł̃N���A�ɂȂ�
		template<typename T>
		operator T() const {
			T value;
			SQRESULT ret;
			if (type == 0) {
				ret = obj->get(intKey, &value);
			} else {
				ret = obj->get(strKey, &value);
			}
			if (SQ_FAILED(ret)) {
				clearValue(&value);
			}
			return value;
		}
		
	protected:
		ObjectInfo *obj;
		int intKey;
		const SQChar *strKey;
		int type;
	};
	

	/**
	 * �z��/�����Q�Ɨp���t�@�����X���擾
	 * �����A�N�Z�X�����ꍇ��
	 * �E������s�����玩���ō쐬
	 * �E�擾���s�����珉���l(0)��Ԃ�
	 * �z��A�N�Z�X�����ꍇ��
	 * �E�T�C�Y�������������玩���I�ɂ��̃T�C�Y�܂ő��₷(null�����߂�)
	 * �E�擾���s�����珉���l(0)��Ԃ�
	 * �Ƃ��������������I�ɍs���܂�
	 */
	template<typename K>
	ObjectInfoReference operator[](K key) {
		return ObjectInfoReference(this, key);
	}

	/// �z��/�����̒l���擾�B�݂���Ȃ����null
	template<typename K, typename T>
	SQRESULT get(K key, T *value) const {
		HSQUIRRELVM gv = getGlobalVM();
		pushData(gv, key);
		SQRESULT ret = getValue(gv, value);
		sq_pop(gv,1);
		return ret;
	}
	
	/// �z��/�����̒l��push�B������Ȃ����null
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

	/// @return �I�u�W�F�N�g�̌^
	SQObjectType type() const {
		return obj._type;
	}
	
	/// @return �z��̒���/�����̃T�C�Y/������̒���
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
			ret = getResultValue(gv, r);
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
			ret = getResultValue(gv, r);
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
			ret = getResultValue(gv, r);
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
				return method.callResult(r, this);
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
				return method.callResult(r, p1, this);
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
				return method.callResult(r, p1, p2, this);
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

	// Object�� push����
	// @return ���ł� squirrel �p�ɏ������ς݂ŃC���X�^���X�������Ă��� push �ł����� true
	static bool pushObject(HSQUIRRELVM v, Object *obj);
	
	// �l�� push
	static void pushValue(HSQUIRRELVM v, bool value) { sq_pushbool(v,value ? SQTrue : SQFalse); }
	static void pushValue(HSQUIRRELVM v, SQBool value) { sq_pushbool(v,value); }
	static void pushValue(HSQUIRRELVM v, SQInteger value) { sq_pushinteger(v,value); }
	static void pushValue(HSQUIRRELVM v, SQFloat value) { sq_pushfloat(v,value); }
	static void pushValue(HSQUIRRELVM v, const SQChar *value) { if (value) {sq_pushstring(v,value,-1);} else { sq_pushnull(v);} }
	static void pushValue(HSQUIRRELVM v, SQUserPointer value) { if (value) {sq_pushuserpointer(v,value);} else { sq_pushnull(v);} }
	static void pushValue(HSQUIRRELVM v, ObjectInfo &obj) { obj.push(v); }
	static void pushValue(HSQUIRRELVM v, std::basic_string<SQChar> &value) { sq_pushstring(v,value.c_str(),value.length()); }
	
	// �l�̎擾
	static SQRESULT getValue(HSQUIRRELVM v, bool *value) { SQBool b;SQRESULT ret = sq_getbool(v, -1, &b); *value = b != SQFalse; return ret; }
	static SQRESULT getValue(HSQUIRRELVM v, SQBool *value) { return sq_getbool(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, SQInteger *value) { return sq_getinteger(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, SQFloat *value) { return sq_getfloat(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, const SQChar **value) { return sq_getstring(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, SQUserPointer *value) { return sq_getuserpointer(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, ObjectInfo *value) { value->getStack(v,-1); return SQ_OK; }
	static SQRESULT getValue(HSQUIRRELVM v, std::basic_string<SQChar> *value) {const SQChar *str; SQRESULT ret; if (SQ_SUCCEEDED((ret = sq_getstring(v, -1, &str)))) { *value = str;} return ret;}

#ifdef USESQPLUS
	// �l�̊i�[
	// �i�[���s�����Ƃ��̓I�u�W�F�N�g���폜�����̂� delete �̕K�v�͂Ȃ�
	// ������ squirrel ���Ő������ꂽ�I�u�W�F�N�g�������ꍇ�͕K����������
	template<typename T>
	static void pushValue(HSQUIRRELVM v, T *value) {
		if (value) {
			if (pushObject(v, value) || SQ_SUCCEEDED(pushnewsqobj(v, GetTypeName(*value), value))) {
				return;
			}
		}
		sq_pushnull(v);
	}
	// �l�̎擾
	template<typename T>
	static SQRESULT getValue(HSQUIRRELVM v, T **value) {
		*value = SqPlus::GetInstance<T,false>(v, -1);
		return SQ_OK;
	}
#else
#ifdef USESQRAT
	// �l�̊i�[ XXX����������
	// �l�̎擾
	template<typename T>
	static SQRESULT getValue(HSQUIRRELVM v, T **value) {
		*value = Sqrat::Var<T*>(v, -1).value;
		return SQ_OK;
	}
#else
	// �l�̊i�[ XXX����������
	// �l�̎擾
	template<typename T>
	static SQRESULT getValue(HSQUIRRELVM v, T **value) {
		*value = (T*)::getInstance(v, -1, GetTypeName(*value));
		return SQ_OK;
	}
#endif
#endif
	
	// �l�̋���������
	static void clearValue(bool *value) { *value = false; }
	static void clearValue(SQBool *value) { *value = SQFalse; }
	static void clearValue(SQInteger *value) { *value = 0; }
	static void clearValue(SQFloat *value) { *value = 0.0f; }
	static void clearValue(const SQChar **value) { *value = 0; }
	static void clearValue(SQUserPointer *value) { *value = 0; }
	static void clearValue(ObjectInfo *value) { value->clear(); }
	static void clearValue(std::basic_string<SQChar> *value) { *value = _SC(""); }

	// �l�̋���������
	template<typename T>
	static void clearValue(T **value) {
		*value = 0;
	}
	
	// �l�̎擾�F��{���̃R�s�y�B������͈��S�łȂ��ꍇ������̂Ŕr������K�v����
	static SQRESULT getResultValue(HSQUIRRELVM v, bool *value) { SQBool b;SQRESULT ret = sq_getbool(v, -1, &b); *value = b != SQFalse; return ret; }
	static SQRESULT getResultValue(HSQUIRRELVM v, SQBool *value) { return sq_getbool(v, -1, value); }
	static SQRESULT getResultValue(HSQUIRRELVM v, SQInteger *value) { return sq_getinteger(v, -1, value); }
	static SQRESULT getResultValue(HSQUIRRELVM v, SQFloat *value) { return sq_getfloat(v, -1, value); }
	static SQRESULT getResultValue(HSQUIRRELVM v, SQUserPointer *value) { return sq_getuserpointer(v, -1, value); }
	static SQRESULT getResultValue(HSQUIRRELVM v, ObjectInfo *value) { value->getStack(v,-1); return SQ_OK; }
	static SQRESULT getResultValue(HSQUIRRELVM v, std::basic_string<SQChar> *value) {const SQChar *str; SQRESULT ret; if (SQ_SUCCEEDED((ret = sq_getstring(v, -1, &str)))) { *value = str;} return ret;}

	// �l�̎擾
	template<typename T>
	SQRESULT getResultValue(HSQUIRRELVM v, T **value) {
		return getValue(value);
	}
	
private:
	HSQOBJECT obj; // �I�u�W�F�N�g�Q�Ə��
};

/**
 * ���������p���
 */
class StackInfo {

public:
	/**
	 * �R���X�g���N�^
	 * @param vm VM
	 */
	StackInfo(HSQUIRRELVM vm) : vm(vm) {
		argc = sq_gettop(vm) - 1;
	}
	
	/**
	 * @return �����̐�
	 */
	int len() const {
		return argc;
	}

	/**
	 * @return self�Q��
	 */
	ObjectInfo getSelf() const {
		return ObjectInfo(vm, 1);
	}

	/**
	 * @param n �����ԍ� 0�`
	 * @return �����̌^
	 */
	SQObjectType getType(int n) const {
		if (n < argc) {
			return sq_gettype(vm, n+2);
		}
		return OT_NULL;
	}
	
	/**
	 * @param n �����ԍ� 0�`
	 * @return �����̌^
	 */
	ObjectInfo getArg(int n) const {
		ObjectInfo ret;
		if (n < argc) {
			ret.getStack(vm, n+2);
		}
		return ret;
	}

	/**
	 * �����擾
	 * @param n �����ԍ� 0�`
	 * @return �����I�u�W�F�N�g
	 */
	ObjectInfo operator[](int n) const {
		return getArg(n);
	}

	// ���ʓo�^
	SQRESULT setReturn() const { return 0; }
	template<typename T>
	SQRESULT setReturn(T value) const {
		ObjectInfo::pushValue(vm, value);
		return 1;
	}

protected:
	HSQUIRRELVM vm; //< VM
	SQInteger argc; //< �����̐�
};

};

#endif
