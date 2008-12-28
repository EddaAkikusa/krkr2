#ifndef __SQOBJECT_H__
#define __SQOBJECT_H__

#include <squirrel.h>
#include <vector>

// �p�����[�^�Q�Ɨp
extern int getInt(HSQUIRRELVM v, int idx, int defValue);
extern const SQChar *getString(HSQUIRRELVM v, int idx);

// �G���[�����p
extern SQInteger ERROR_CREATE(HSQUIRRELVM v);
extern SQInteger ERROR_NOMEMBER(HSQUIRRELVM v);
extern SQInteger ERROR_BADINSTANCE(HSQUIRRELVM v);

// �^��`
extern const SQUserPointer OBJTYPETAG;
extern const SQUserPointer THREADTYPETAG;

#include <tchar.h>
#if _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

extern void getSetterName(tstring &store, const SQChar *name);
extern void getGetterName(tstring &store, const SQChar *name);

class MyObject;
class MyThread;

/**
 * squirrel �I�u�W�F�N�g�ێ��p�N���X
 */
class SQObjectInfo {

protected:
	HSQUIRRELVM v; // �I�u�W�F�N�g�̑����Ă���VM
	HSQOBJECT obj; // �I�u�W�F�N�g�Q�Ə��

public:
	// ���e����
	void clear();

	// �X�^�b�N����擾
	void getStack(HSQUIRRELVM v, int idx);

	// �R���X�g���N�^
	SQObjectInfo();

	// �R���X�g���N�^
	SQObjectInfo(HSQUIRRELVM v, int idx);

	// �R�s�[�R���X�g���N�^
	SQObjectInfo(const SQObjectInfo &orig);

	// ���
	SQObjectInfo & operator=(const SQObjectInfo &orig);
	
	// �f�X�g���N�^
	virtual ~SQObjectInfo();

	// �X���b�h���H
	bool isThread() const;

	// �����X���b�h���H
	bool isSameThread(const HSQUIRRELVM v) const;

	// �X���b�h���擾
	operator HSQUIRRELVM() const;
	
	// �C���X�^���X���[�U�|�C���^���擾
	SQUserPointer getInstanceUserPointer(const SQUserPointer tag);

	// �I�u�W�F�N�g��PUSH
	void push(HSQUIRRELVM v) const;

	// ---------------------------------------------------
	// delegate �����p
	// ---------------------------------------------------

	// delegate �Ƃ��ċ@�\���邩�ǂ���
	bool isDelegate();

	// bindenv �����邩�ǂ���
	bool isBindDelegate();

	// ---------------------------------------------------
	// �I�u�W�F�N�g�擾
	// ---------------------------------------------------

	// �C���X�^���X���[�U�|�C���^���擾
	MyThread *getMyThread();

	// �C���X�^���X���[�U�|�C���^���擾
	MyObject *getMyObject();
	
	// ---------------------------------------------------
	// wait�����p���\�b�h
	// ---------------------------------------------------

	bool isString() const;

	bool isSameString(const SQChar *str) const;

	bool isNumeric() const;
};

/**
 * �I�u�W�F�N�g�p
 */
class MyObject {

	// ------------------------------
	// �p�����
	// ------------------------------
protected:
	static std::vector<SQUserPointer>tags;
public:
	static void pushTag(const SQUserPointer tag) {
		tags.push_back(tag);
	}

protected:
	// ���̃I�u�W�F�N�g��҂��Ă�X���b�h�̈ꗗ
	std::vector<MyThread*> _waitThreadList;
	// delegate
	SQObjectInfo delegate;

public:
	/**
	 * �I�u�W�F�N�g�҂��̓o�^
	 * @param thread �X���b�h
	 */
	void addWait(MyThread *thread);

	/**
	 * �I�u�W�F�N�g�҂��̉���
	 * @param thread �X���b�h
	 */
	void removeWait(MyThread *thread);

protected:

	/**
	 * �R���X�g���N�^
	 */
	MyObject();

	/**
	 * �f�X�g���N�^
	 */
	virtual ~MyObject();

	/**
	 * ���̃I�u�W�F�N�g��҂��Ă���P�X���b�h�̑҂�������
	 */
	void _notify();
	
	/**
	 * ���̃I�u�W�F�N�g��҂��Ă���S�X���b�h�̑҂�������
	 */
	void _notifyAll();

	// ------------------------------------------------------------------

public:
	/**
	 * @return �I�u�W�F�N�g���I�u�W�F�N�g
	 */
	static MyObject *getObject(HSQUIRRELVM v, int idx);

protected:
	/**
	 * �I�u�W�F�N�g�̃����[�T
	 */
	static SQInteger release(SQUserPointer up, SQInteger size);

	/**
	 * �I�u�W�F�N�g�̃R���X�g���N�^
	 */
	static SQInteger constructor(HSQUIRRELVM v);

	static bool isClosure(SQObjectType type);
	
	/**
	 * �v���p�e�B����l���擾
	 * @param name �v���p�e�B��
	 * @return �v���p�e�B�l
	 */
	static SQInteger _get(HSQUIRRELVM v);

	/**
	 * �v���p�e�B�ɒl��ݒ�
	 * @param name �v���p�e�B��
	 * @param value �v���p�e�B�l
	 */
	static SQInteger _set(HSQUIRRELVM v);

	/**
	 * set�v���p�e�B�̑��݊m�F
	 * @param name �v���p�e�B��
	 * @return set�v���p�e�B�����݂����� true
	 */
	static SQInteger hasSetProp(HSQUIRRELVM v);
	
	/**
	 * �Ϗ��̐ݒ�
	 */
	static SQInteger setDelegate(HSQUIRRELVM v);
	
	/**
	 * �P��X���b�h�ւ̃I�u�W�F�N�g�҂��̏I���ʒm�p
	 */
	static SQInteger notify(HSQUIRRELVM v);

	/**
	 * �S�X���b�h�ւ̃I�u�W�F�N�g�҂��̏I���ʒm
	 */
	static SQInteger notifyAll(HSQUIRRELVM v);

public:
	/**
	 * �N���X�̓o�^
	 * @param v squirrel VM
	 */
	static void registClass(HSQUIRRELVM v);
};

// �^��
#define OBJECTNAME _SC("Object")
#define THREADNAME _SC("Thread")

// ���\�b�h�o�^
#define REGISTMETHOD(name) \
	sq_pushstring(v, _SC(#name), -1);\
	sq_newclosure(v, name, 0);\
	sq_createslot(v, -3);

// ���\�b�h�o�^�i���O���j
#define REGISTMETHODNAME(name, method) \
	sq_pushstring(v, _SC(#name), -1);\
	sq_newclosure(v, method, 0);\
	sq_createslot(v, -3);

// ���\�b�h�o�^
#define REGISTENUM(name) \
	sq_pushstring(v, _SC(#name), -1);\
	sq_pushinteger(v, name);\
	sq_createslot(v, -3);

#endif
