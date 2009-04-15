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

protected:
	HSQOBJECT obj; // �I�u�W�F�N�g�Q�Ə��

public:
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

protected:
	static void pushValue(HSQUIRRELVM v, int value) { sq_pushinteger(v,value); }
	static void pushValue(HSQUIRRELVM v, const SQChar *value) { sq_pushstring(v,value,-1); }

public:
	/// �z��Ƃ��ď�����
	void initArray(int size=0);

	/// @return �z��Ȃ� true
	bool isArray() const { return sq_isarray(obj); }

	/// �z��ɒl��ǉ�
	void append(HSQUIRRELVM v, int idx);
	
	/// �z��ɒl��ǉ�
	template<typename T>
	void append(T value);

	/// �z��ɒl��}��
	template<typename T>
	void insert(int index, T value);

	/// �z��ɒl���i�[
	template<typename T>
	void set(int index, T value);

	/// @return �z��̒���
	int len() const;

	/**
	 * �z��̓��e��S��PUSH
	 * @param v squirrelVM
	 * @return push ������
	 */
	int pushArray(HSQUIRRELVM v) const;
};

};

#endif
