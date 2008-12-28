/**
 * squirrel �p���I�u�W�F�N�g�{�^���X���b�h
 */
#include <ctype.h>
#include <squirrel.h>
#include <sqstdio.h>
#include <string>
#include <new>
#include <vector>

// ���O�o�͗p
extern void printFunc(HSQUIRRELVM v, const SQChar* format, ...);

#include <tchar.h>
#if _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

//#define USEUD

// ���[�v���s�J�n
extern void sqobject_start();
// ���[�v���s�I��
extern void sqobject_stop();

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

/**
 * �p�����[�^���̎擾
 * @param v VM
 * @return �p�����[�^��
 */
static int getParamCount(HSQUIRRELVM v)
{
	return sq_gettop(v) - 1;
}

/**
 * ���l�擾�p
 * @param v VM
 * @param idx �C���f�b�N�X
 * @param defValue �f�t�H���g�l
 * @return �����l
 */
static int getInt(HSQUIRRELVM v, int idx, int defValue) {
	if (sq_gettop(v) > idx) {
		switch (sq_gettype(v, idx)) {
		case OT_INTEGER:
		case OT_FLOAT:
			{
				int ret;
				sq_getinteger(v, idx, &ret);
				return ret;
			}
		}
	}
	return defValue;
};

/**
 * ������擾�p
 * @param v VM
 * @param idx �C���f�b�N�X
 * @return ������
 */
static const SQChar *getString(HSQUIRRELVM v, int idx) {
	const SQChar *x = NULL;
	sq_getstring(v, idx, &x);
	return x;
};

static const SQUserPointer OBJTYPETAG = (SQUserPointer)"OBJTYPETAG";
static const SQUserPointer THREADTYPETAG = (SQUserPointer)"THREADTYPETAG";

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
	void clear() {
		if (v) {
			sq_release(v,&obj);
			sq_resetobject(&obj);
			v = NULL;
		}
	}

	// �X�^�b�N����擾
	void getStack(HSQUIRRELVM v, int idx) {
		clear();
		this->v = v;
		sq_getstackobj(v, idx, &obj);
		sq_addref(v, &obj);
	}
	
	// �R���X�g���N�^
	SQObjectInfo() : v(NULL) {
		sq_resetobject(&obj);
	}

	// �R���X�g���N�^
	SQObjectInfo(HSQUIRRELVM v, int idx) : v(v) {
		sq_resetobject(&obj);
		sq_getstackobj(v, idx, &obj);
		sq_addref(v, &obj);
	}

	// �R�s�[�R���X�g���N�^
	SQObjectInfo(const SQObjectInfo &orig) {
		v   = orig.v;
		obj = orig.obj;
		if (v) {
			sq_addref(v, &obj);
		}
	}

	// ���
	SQObjectInfo & operator=(const SQObjectInfo &orig) {
		clear();
		v = orig.v;
		obj = orig.obj;
		if (v) {
			sq_addref(v, &obj);
		}
		return *this;
	}
	
	// �f�X�g���N�^
	virtual ~SQObjectInfo() {
		clear();
	}

	// �X���b�h���H
	bool isThread() const {
		return sq_isthread(obj);
	}

	// �����X���b�h���H
	bool isSameThread(const HSQUIRRELVM v) const {
		return sq_isthread(obj) && obj._unVal.pThread == v;
	}

	// �X���b�h���擾
	operator HSQUIRRELVM() const {
		HSQUIRRELVM vm = sq_isthread(obj) ? obj._unVal.pThread : NULL;
		return vm;
	}
	
	// �C���X�^���X���[�U�|�C���^���擾
	SQUserPointer getInstanceUserPointer(const SQUserPointer tag) {
		SQUserPointer up = NULL;
		if (v && sq_isinstance(obj)) {
			sq_pushobject(v, obj);
			if (!SQ_SUCCEEDED(sq_getinstanceup(v, -1, &up, tag))) {
				up = NULL;
			}
			sq_pop(v, 1);
		}
		return up;
	}

	// �I�u�W�F�N�g��PUSH
	void push(HSQUIRRELVM v) const {
		sq_pushobject(v, obj);
	}

	// ---------------------------------------------------
	// delegate �����p
	// ---------------------------------------------------

	// delegate �Ƃ��ċ@�\���邩�ǂ���
	bool isDelegate() {
		return v && (sq_isinstance(obj) || sq_istable(obj));
	}

	// bindenv �����邩�ǂ���
	bool isBindDelegate() {
		return v && (sq_isinstance(obj));
	}

	// ---------------------------------------------------
	// �I�u�W�F�N�g�擾
	// ---------------------------------------------------

	// �C���X�^���X���[�U�|�C���^���擾
	MyThread *getMyThread() {
		SQUserPointer up = NULL;
		if (v && sq_isinstance(obj)) {
			sq_pushobject(v, obj);
			if (!SQ_SUCCEEDED(sq_getinstanceup(v, -1, &up, THREADTYPETAG))) {
				up = NULL;
			}
			sq_pop(v, 1);
		}
		return (MyThread*)up;
	}

	// �C���X�^���X���[�U�|�C���^���擾
	MyObject *getMyObject() {
		SQUserPointer up = NULL;
		if (v && sq_isinstance(obj)) {
			sq_pushobject(v, obj);
			if (!SQ_SUCCEEDED(sq_getinstanceup(v, -1, &up, OBJTYPETAG)) ||
				!SQ_SUCCEEDED(sq_getinstanceup(v, -1, &up, THREADTYPETAG))) {
				up = NULL;
			}
			sq_pop(v, 1);
		}
		return (MyObject*)up;
	}
	
	// ---------------------------------------------------
	// wait�����p���\�b�h
	// ---------------------------------------------------

	// MyObject���H
	bool isMyObject() {
		bool ret = false;
		if (sq_isinstance(obj)) {
			SQUserPointer typetag;
			sq_pushobject(v, obj);
			if (SQ_SUCCEEDED(sq_gettypetag(v, -1, &typetag))) {
				ret = (typetag == OBJTYPETAG ||
					   typetag == THREADTYPETAG);
			}
			sq_pop(v, 1);
		}
		return ret;
	}

	bool isString() const {
		return sq_isstring(obj);
	}

	bool isSameString(const SQChar *str) const {
		if (str && sq_isstring(obj)) {
			const SQChar *mystr;
			sq_pushobject(v, obj);
			sq_getstring(v, -1, &mystr);
			sq_pop(v, 1);
			return mystr && _tcscmp(str, mystr) == 0;
		}
		return false;
	}

	bool isNumeric() const {
		return sq_isnumeric(obj) != 0;
	}
};

/**
 * �I�u�W�F�N�g�p
 */
class MyObject {

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
	void addWait(MyThread *thread) {
		_waitThreadList.push_back(thread);
	}

	/**
	 * �I�u�W�F�N�g�҂��̉���
	 * @param thread �X���b�h
	 */
	void removeWait(MyThread *thread) {
		std::vector<MyThread*>::iterator i = _waitThreadList.begin();
		while (i != _waitThreadList.end()) {
			if (*i == thread) {
				i = _waitThreadList.erase(i);
			} else {
				i++;	
			}
		}
	}

protected:

	/**
	 * �R���X�g���N�^
	 */
	MyObject() {
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~MyObject() {
		_notifyAll();
	}

	/**
	 * ���̃I�u�W�F�N�g��҂��Ă���P�X���b�h�̑҂�������
	 */
	void _notify();
	
	/**
	 * ���̃I�u�W�F�N�g��҂��Ă���S�X���b�h�̑҂�������
	 */
	void _notifyAll();

	// ------------------------------------------------------------------

	/**
	 * @return �I�u�W�F�N�g���I�u�W�F�N�g
	 */
	static MyObject *getObject(HSQUIRRELVM v, int idx) {
		SQUserPointer up;
#ifdef USEUD
		SQUserPointer typetag;
		if (SQ_SUCCEEDED(sq_getuserdata(v, idx, &up, &typetag)) &&
			(typetag == THREADTYPETAG || typetag == OBJTYPETAG)) {
			return (MyObject*)up;
		}
#else
		if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, THREADTYPETAG)) ||
			SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, OBJTYPETAG))) {
			return (MyObject*)up;
		}
#endif
		return NULL;
	}

	/**
	 * �I�u�W�F�N�g�̃����[�T
	 */
	static SQInteger release(SQUserPointer up, SQInteger size) {
		MyObject *self = (MyObject*)up;
#ifdef USEUD
		self->~MyObject();
#else
		delete self;
#endif
		return SQ_OK;
	}

	static SQInteger ERROR_CREATE(HSQUIRRELVM v) {
		return sq_throwerror(v, _SC("can't create native instance"));
	}
	
	/**
	 * �I�u�W�F�N�g�̃R���X�g���N�^
	 */
	static SQInteger constructor(HSQUIRRELVM v) {
		SQInteger result = SQ_OK;
#ifdef USEUD
		MyObject *self = getObject(v, 1);
		if (self) {
			new (self) MyObject();
			result = sq_setreleasehook(v, 1, release);
		} else {
			result = ERROR_CREATE(v);
		}
#else
		MyObject *self = new MyObject();
		if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {;
			sq_setreleasehook(v, 1, release);
		}
#endif
		if (SQ_SUCCEEDED(result)) {
			if (getParamCount(v) > 0) {
				self->delegate.getStack(v, 2);
			}
		}
		return result;
	}

	static SQInteger ERROR_NOMEMBER(HSQUIRRELVM v) {
		return sq_throwerror(v, _SC("no such member"));
	}

	static bool isClosure(SQObjectType type) {
		return type == OT_CLOSURE || type == OT_NATIVECLOSURE;
	}
	
	/**
	 * �v���p�e�B����l���擾
	 * @param name �v���p�e�B��
	 * @return �v���p�e�B�l
	 */
	static SQInteger _get(HSQUIRRELVM v) {
		SQInteger result = SQ_OK;
		const SQChar *name = getString(v, 2);
		if (name && *name) {

			// delegate�̎Q��
			MyObject *self = getObject(v, 1);
			if (self && self->delegate.isDelegate()) {
				self->delegate.push(v);
				sq_pushstring(v, name, -1);
				if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
					// ���\�b�h�̏ꍇ�͑�������
					if (isClosure(sq_gettype(v,-1)) && self->delegate.isBindDelegate()) {
						self->delegate.push(v);
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
			tstring name2 = _SC("get");
			name2 += toupper(*name);
			name2 += (name + 1);
			sq_push(v, 1); // self
			sq_pushstring(v, name2.c_str(), -1);
			if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
				sq_push(v, 1); //  self;
				if (SQ_SUCCEEDED(result = sq_call(v,1,SQTrue,SQTrue))) {
					//printf("�Ăяo������:%s\n", name);
					sq_remove(v, -2); // func
					sq_remove(v, -2); // self
					return 1;
				} else {
					sq_pop(v,2); // func, self
				}
			} else {
				sq_pop(v, 1); // self
			}

		}
		//return result;
		return ERROR_NOMEMBER(v);
	}
	
	/**
	 * �v���p�e�B�ɒl��ݒ�
	 * @param name �v���p�e�B��
	 * @param value �v���p�e�B�l
	 */
	static SQInteger _set(HSQUIRRELVM v) {
		SQInteger result = SQ_OK;
		const SQChar *name = getString(v, 2);
		if (name && *name) {

			// delegate�̎Q��
			MyObject *self = getObject(v, 1);
			if (self && self->delegate.isDelegate()) {
				self->delegate.push(v);
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
			tstring name2 = _SC("set");
			name2 += toupper(*name);
			name2 += (name + 1);
			sq_push(v, 1); // self
			sq_pushstring(v, name2.c_str(), -1);
			if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
				sq_push(v, 1); // self
				sq_push(v, 3); // value
				if (SQ_SUCCEEDED(result = sq_call(v,2,SQFalse,SQTrue))) {
					//printf("�Ăяo������:%s\n", name);
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
	static SQInteger hasSetProp(HSQUIRRELVM v) {
		SQInteger result = SQ_OK;
		SQBool ret = SQFalse;
		if (getParamCount(v) >= 2) {
			const SQChar *name = getString(v, 2);
			if (name && *name) {
				tstring name2 = _SC("set");
				name2 += toupper(*name);
				name2 += (name + 1);
				sq_push(v, 1); // object
				sq_pushstring(v, name2.c_str(), -1);
				if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
					sq_pop(v,1);
					ret = SQTrue;
				} else {
					sq_pushstring(v, name, -1);
					if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
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
	
	static SQInteger ERROR_BADINSTANCE(HSQUIRRELVM v) {
		return sq_throwerror(v, _SC("bad instance"));
	}

	/**
	 * �Ϗ��̐ݒ�
	 */
	static SQInteger setDelegate(HSQUIRRELVM v) {
		MyObject *self = getObject(v, 1);
		if (self) {
			if (getParamCount(v) > 0) {
				self->delegate.getStack(v, 2);
			} else {
				self->delegate.clear();
			}
			return SQ_OK;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}
	
	/**
	 * �P��X���b�h�ւ̃I�u�W�F�N�g�҂��̏I���ʒm�p
	 */
	static SQInteger notify(HSQUIRRELVM v) {
		MyObject *self = getObject(v, 1);
		if (self) {
			self->_notify();
			return SQ_OK;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	/**
	 * �S�X���b�h�ւ̃I�u�W�F�N�g�҂��̏I���ʒm
	 */
	static SQInteger notifyAll(HSQUIRRELVM v) {
		MyObject *self = getObject(v, 1);
		if (self) {
			self->_notifyAll();
			return SQ_OK;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

public:
	/**
	 * �N���X�̓o�^
	 * @param v squirrel VM
	 */
	static void registClass(HSQUIRRELVM v) {
		sq_pushroottable(v); // root
		sq_pushstring(v, OBJECTNAME, -1);
		sq_newclass(v, false);
		sq_settypetag(v, -1, OBJTYPETAG);
#ifdef USEUD
		sq_setclassudsize(v, -1, sizeof MyObject);
#endif
		REGISTMETHOD(constructor);
		REGISTMETHOD(_set);
		REGISTMETHOD(_get);
		REGISTMETHOD(hasSetProp);
		REGISTMETHOD(setDelegate);
		REGISTMETHOD(notify);
		REGISTMETHOD(notifyAll);
		sq_createslot(v, -3);
		sq_pop(v,1); // root
	};
};

class MyThread : public MyObject {

protected:
	long _currentTick; //< ���̃X���b�h�̎��s����

	// �X���b�h�f�[�^
	SQObjectInfo _thread;

	// �҂��Ώ�
	std::vector<SQObjectInfo> _waitList;
	// �҂�����
	SQInteger _waitTimeout;

	// �҂����^�C���A�E�g������
	SQObjectInfo _waitResult;

	/**
	 * �X���b�h���
	 */
	enum ThreadStatus {
		THREAD_STOP,  // ��~
		THREAD_RUN,   // ���쒆
		THREAD_WAIT,  // �҂���
		THREAD_END,   // �I��
	} _status;

	/**
	 * @return �����҂�����
	 */
	bool isWait() {
		return _waitList.size() > 0 || _waitTimeout >= 0;
	}

public:

	/**
	 * @return �Y���X���b�h�ƌ��݊Ǘ����̃X���b�h����v���Ă�� true
	 */
	bool isSameThread(HSQUIRRELVM v) {
		return _thread.isSameThread(v);
	}

	/**
	 * �I�u�W�F�N�g�ɑ΂���҂���������������
	 * @param target �҂��Ώ�
	 * @return �Y���I�u�W�F�N�g��҂��Ă��ꍇ�� true
	 */
	bool notifyObject(MyObject *target) {
		bool find = false;
		std::vector<SQObjectInfo>::iterator i = _waitList.begin();
		while (i != _waitList.end()) {
			MyObject *obj = i->getMyObject();
			if (obj && obj == target) {
				find = true;
				_waitResult = *i;
				i = _waitList.erase(i);
			} else {
				i++;
			}
		}
		if (find) {
			_clearWait();
		}
		return find;
	}

	/**
	 * �g���K�ɑ΂���҂���������������
	 * @param name �g���K��
	 * @return �Y���I�u�W�F�N�g��҂��Ă��ꍇ�� true
	 */
	bool notifyTrigger(const SQChar *name) {
		bool find = false;
		std::vector<SQObjectInfo>::iterator i = _waitList.begin();
		while (i != _waitList.end()) {
			if (i->isSameString(name)) {
				find = true;
				_waitResult = *i;
				i = _waitList.erase(i);
			} else {
				i++;
			}
		}
		if (find) {
			_clearWait();
		}
		return find;
	}

protected:
	// �R���X�g���N�^
	MyThread() : _currentTick(0), _waitTimeout(-1), _status(THREAD_STOP) {
	}
	
	// �f�X�g���N�^
	~MyThread() {
		_clear();
	}

	/**
	 * �I�u�W�F�N�g�ɑ΂���҂������N���A����
	 * @param status �L�����Z���̏ꍇ�� true
	 */
	void _clearWait() {
		std::vector<SQObjectInfo>::iterator i = _waitList.begin();
		while (i != _waitList.end()) {
			MyObject *obj = i->getMyObject();
			if (obj) {
				obj->removeWait(this);
			}
			i = _waitList.erase(i);
		}
		_waitTimeout = -1;
	}

	/**
	 * �҂��o�^
	 */
	SQInteger _wait(HSQUIRRELVM v) {
		_clearWait();
		_waitResult.clear();
		int max = sq_gettop(v);
		for (int i=2;i<=max;i++) {
			switch (sq_gettype(v, 2)) {
			case OT_INTEGER:
			case OT_FLOAT:
				// ���l�̏ꍇ�̓^�C���A�E�g�҂�
				{
					int timeout;
					sq_getinteger(v, i, &timeout);
					if (timeout >= 0) {
						if (_waitTimeout < 0  || _waitTimeout > timeout) {
							_waitResult.getStack(v, i);
							_waitTimeout = timeout;
						}
					}
				}
				break;
			case OT_STRING:
			case OT_INSTANCE:
				// ������܂��̓C���X�^���X�̏ꍇ�͑҂����X�g�ɓo�^
				_waitList.push_back(SQObjectInfo(v,i));
				break;
			}
		}
		return sq_suspendvm(v);
	}

	/**
	 * ���j��
	 */
	void _clear() {
		_clearWait();
		_thread.clear();
	}
	
	/**
	 * ���s�J�n
	 * @param func ���s�Ώۃt�@���N�V�����B������̏ꍇ�Y���X�N���v�g��ǂݍ���
	 */
	SQInteger _exec(HSQUIRRELVM v) {
		_clear();

		HSQUIRRELVM testVM = sq_newthread(v, 1024);
		_thread.getStack(v, -1);
		sq_pop(v, 1);
		
		// �X���b�h�擪�ɃX�N���v�g�����[�h
		if (sq_gettype(v, 2) == OT_STRING) {
			SQInteger result;
			if (SQ_FAILED(result = sqstd_loadfile(_thread, getString(v, 2), SQTrue))) {
				_clear();
				return result;
			}
		} else {
			sq_move(_thread, v, 2);
		}
		_currentTick = 0;
		_status = THREAD_RUN;
		entryThread(v); // �X�^�b�N�� 1�ԂɌ��̃I�u�W�F�N�g�������Ă�
		return SQ_OK;
	}

	/**
	 * ���s�I��
	 */
	void _exit() {
		_clear();
		_notifyAll();
		_status = THREAD_STOP;
	}

	/**
	 * ���s��~
	 */
	void _stop() {
		if (_thread.isThread()) {
			if (_status == THREAD_RUN) {
				_status = THREAD_STOP;
			}
		}
	}

	/**
	 * ���s�ĊJ
	 */
	void _run() {
		if (_thread.isThread()) {
			if (_status == THREAD_STOP) {
				_status = THREAD_RUN;
			}
		}
	}

	/**
	 * @return ���s�X�e�[�^�X
	 */
	int getStatus() {
		return !_thread.isThread() ? THREAD_END : isWait() ? THREAD_WAIT : _status;
	}

	/**
	 * �X���b�h�̃��C������
	 * @param diff �o�ߎ���
	 * @return �X���b�h���s�I���Ȃ� true
	 */
	bool _main(long diff) {

		//dm("�X���b�h���s:" + this);
		_currentTick += diff;

		// �^�C���A�E�g����
		if (_waitTimeout >= 0) {
			_waitTimeout -= diff;
			if (_waitTimeout < 0) {
				_clearWait();
			}
		}

		// �X���b�h���s
		if (getStatus() == THREAD_RUN) {
			SQInteger result;
			if (sq_getvmstate(_thread) == SQ_VMSTATE_SUSPENDED) {
				_waitResult.push(_thread);
				_waitResult.clear();
				result = sq_wakeupvm(_thread, SQTrue, SQFalse, SQTrue);
			} else {
				sq_pushroottable(_thread);
				result = sq_call(_thread, 1, SQFalse, SQTrue);
			}
			if (SQ_FAILED(result)) {
				sq_getlasterror(_thread);
				const SQChar *str;
				if (SQ_SUCCEEDED(sq_getstring(_thread, -1, &str))) {
					printFunc(_thread, str);
				} else {
					printFunc(_thread, _SC("failed to run by unknown reason"));
				}
				sq_pop(_thread, 1);
				_exit();
			} else  if (sq_getvmstate(_thread) == SQ_VMSTATE_IDLE) {
				_exit();
			}
		}
		return getStatus() == THREAD_END;
	}

	// ------------------------------------------------------------------

	/**
	 * @return �X���b�h���I�u�W�F�N�g
	 */
	static MyThread *getThread(HSQUIRRELVM v, int idx) {
		SQUserPointer up;
#ifdef USEUD
		SQUserPointer typetag;
		if (SQ_SUCCEEDED(sq_getuserdata(v, idx, &up, &typetag)) &&
			(typetag == THREADTYPETAG))) {
			return (MyObject*)up;
		}
#else
		if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, THREADTYPETAG))) {
			return (MyThread*)up;
		}
#endif
		return NULL;
	}

	/**
	 * �I�u�W�F�N�g�̃����[�T
	 */
	static SQInteger release(SQUserPointer up, SQInteger size) {
		MyThread *self = (MyThread*)up;
#ifdef USEUD
		self->~MyThread();
#else
		delete self;
#endif
		return SQ_OK;
	}
	
	/**
	 * �I�u�W�F�N�g�̃R���X�g���N�^
	 */
	static SQInteger constructor(HSQUIRRELVM v) {
		SQInteger result = SQ_OK;
#ifdef USEUD
		MyThread *self = getThread(v, 1);
		if (self) {
			new (self) MyThread();
			result = sq_setreleasehook(v, 1, release);
		} else {
			result = ERROR_CREATE(v);
		}
#else
		MyThread *self = new MyThread();
		if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {
			sq_setreleasehook(v, 1, release);
		}
#endif
		if (SQ_SUCCEEDED(result)) {
			if (getParamCount(v) > 1) {
				self->delegate.getStack(v, 3);
			}
			if (getParamCount(v) > 0) {
				result = self->_exec(v);
			}
		}
		return result;
	}

	// ------------------------------------------------------
	// �v���p�e�B
	// ------------------------------------------------------
	
	static SQInteger getCurrentTick(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
		if (self) {
			sq_pushinteger(v, self->_currentTick);
			return 1;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	static SQInteger getStatus(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
		if (self) {
			sq_pushinteger(v, self->getStatus());
			return 1;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}
	
	// ------------------------------------------------------
	// ���\�b�h
	// ------------------------------------------------------

	/**
	 * �������J�n����
	 * @param func �X���b�h�Ŏ��s����t�@���N�V����
	 */
	static SQInteger exec(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
		if (self) {
			return self->_exec(v);
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	/**
	 * �����𒆒f����
	 */
	static SQInteger exit(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
		if (self) {
			self->_exit();
			return 0;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}
	
	/**
	 * �������ꎞ��~����
	 */
	static SQInteger stop(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
		if (self) {
			self->_stop();
			return 0;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	/**
	 * �������ĊJ����
	 */
	static SQInteger run(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
		if (self) {
			self->_run();
			return 0;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	/**
	 * �����҂�
	 */
	static SQInteger wait(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
		if (self) {
			return self->_wait(v);
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

public:
	/**
	 * �N���X�̓o�^
	 * @param v squirrel VM
	 */
	static void registClass(HSQUIRRELVM v) {
		sq_pushroottable(v); // root
		sq_pushstring(v, THREADNAME, -1);
		sq_pushstring(v, OBJECTNAME, -1);
		sq_get(v,-3);
		sq_newclass(v, true); // �p������
		sq_settypetag(v, -1, THREADTYPETAG);
#ifdef USEUD
		sq_setclassudsize(v, -1, sizeof MyThread);
#endif
		REGISTENUM(THREAD_STOP);
		REGISTENUM(THREAD_RUN);
		REGISTENUM(THREAD_WAIT);
		REGISTENUM(THREAD_END);
		REGISTMETHOD(constructor);
		REGISTMETHOD(getCurrentTick);
		REGISTMETHOD(getStatus);
		REGISTMETHOD(exec);
		REGISTMETHOD(exit);
		REGISTMETHOD(stop);
		REGISTMETHOD(run);
		REGISTMETHOD(wait);
		sq_createslot(v, -3);
		sq_pop(v, 1); // root
	};
	
	// -------------------------------------------------------------
	//
	// �O���[�o���X���b�h����p���@�\
	//
	// -------------------------------------------------------------
	
protected:
	static std::vector<SQObjectInfo> threadList; //< �X���b�h�ꗗ
	static bool first;        //< ����Ăяo������
	static long prevTick;     //< �O��̌Ăяo������
	static long currentTick;  //< ����̌Ăяo������

	/**
	 * �X���b�h���s�J�n�o�^
	 * @param v Squirrel�X�^�b�N 1�ԂɃX���b�h�I�u�W�F�N�g���͂����Ă�͂�
	 */
	static void entryThread(HSQUIRRELVM v) {
		if (threadList.size() == 0) {
			sqobject_start();
		}
		threadList.push_back(SQObjectInfo(v,1));
		first = true;
	}

	// -------------------------------------------------------------
	// �O���[�o�����\�b�h�p
	// -------------------------------------------------------------

	/**
	 * ���ݎ����̎擾
	 */
	static SQInteger global_getCurrentTick(HSQUIRRELVM v) {
		sq_pushinteger(v, currentTick);
		return 1;
	}

	static SQInteger ERROR_NOTHREAD(HSQUIRRELVM v) {
		return sq_throwerror(v, _SC("no thread"));
	}
	
	/*
	 * @return ���݂̃X���b�h��Ԃ�
	 */
	static SQInteger global_getCurrentThread(HSQUIRRELVM v) {
		std::vector<SQObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			MyThread *th = i->getMyThread();
			if (th && th->isSameThread(v)) {
				i->push(v);
				return 1;
			}
			i++;
		}
		return ERROR_NOTHREAD(v);
	}
	
	/*
	 * @return ���݂̃X���b�h�ꗗ��Ԃ�
	 */
	static SQInteger global_getThreadList(HSQUIRRELVM v) {
		sq_newarray(v, 0);
		std::vector<SQObjectInfo>::const_iterator i = threadList.begin();
		while (i != threadList.end()) {
			i->push(v);
			sq_arrayappend(v, -2);
			i++;
		}
		return 1;
	}

	/*
	 * �X�N���v�g��V�����X���b�h�Ƃ��Ď��s����
	 * �� return Thread(func); ����
	 * @param func �X���b�h�Ŏ��s����t�@���N�V����
	 * @return �V�X���b�h
	 */
	static SQInteger global_fork(HSQUIRRELVM v) {
		//dm("�X���b�h�𐶐�!");
		SQInteger result;
		sq_pushroottable(v); // root
		sq_pushstring(v, THREADNAME, -1);
		if (SQ_SUCCEEDED(result = sq_get(v,-2))) { // class
			sq_pushroottable(v); // ����:self
			sq_push(v, 2);       // ����:func
			if (SQ_SUCCEEDED(result = sq_call(v, 2, SQTrue, SQTrue))) { // �R���X�g���N�^�Ăяo��
				sq_remove(v, -2); // class, root
				return 1;
			} else {
				sq_pop(v, 1); // class
			}
		}
		sq_pop(v,1); // root
		return result;
	}

	/**
	 * @return ���ݎ��s���̃X���b�h���I�u�W�F�N�g(MyThread*)
	 */
	static MyThread *getCurrentThread(HSQUIRRELVM v) {
		std::vector<SQObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			MyThread *th = i->getMyThread();
			if (th && th->isSameThread(v)) {
				return th;
			}
			i++;
		}
		return NULL;
	}
	
	/**
	 * �X�N���v�g��؂�ւ���
	 * @param func �X���b�h�Ŏ��s����t�@���N�V����
	 */
	static SQInteger global_exec(HSQUIRRELVM v) {
		MyThread *th = getCurrentThread(v);
		if (th) {
			SQInteger ret = th->_exec(v);
			if (ret) {
				return ret;
			} else {
				return sq_suspendvm(v);
			}
		} else {
			return ERROR_NOTHREAD(v);
		}
	}

	/**
	 * ���s���X���b�h�̏I��
	 */
	static SQInteger global_exit(HSQUIRRELVM v) {
		MyThread *th = getCurrentThread(v);
		if (th) {
			th->_exit();
			return sq_suspendvm(v);
		} else {
			return ERROR_NOTHREAD(v);
		}
	}

	/**
	 * ���s���X���b�h�̏����҂�
	 * @param target int:���ԑ҂�(ms), string:�g���K�҂�, obj:�I�u�W�F�N�g�҂�
	 * @param timeout �^�C���A�E�g(�ȗ����͖����ɑ҂�)
	 * @return �҂����L�����Z�����ꂽ�� true
	 */
	static SQInteger global_wait(HSQUIRRELVM v) {
		MyThread *th = getCurrentThread(v);
		if (th) {
			return th->_wait(v);
		} else {
			return ERROR_NOTHREAD(v);
		}
	}

	/**
	 * �S�X���b�h�ւ̃g���K�ʒm
	 * @param name �����҂��g���K��
	 */
	static SQInteger global_trigger(HSQUIRRELVM v) {
		const SQChar *name = getString(v, 2);
		std::vector<SQObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			MyThread *th = i->getMyThread();
			if (th) {
				th->notifyTrigger(name);
			}
			i++;
		}
		return SQ_OK;
	}

public:
	/*
	 * �O���[�o�����\�b�h�̓o�^
	 * @param v squirrel VM
	 */
	static void registGlobal(HSQUIRRELVM v) {
		sq_pushroottable(v); // root
		REGISTMETHODNAME(getCurrentTick, global_getCurrentTick);
		REGISTMETHODNAME(getCurrentThread, global_getCurrentThread);
		REGISTMETHODNAME(getThreadList, global_getThreadList);
		REGISTMETHODNAME(fork, global_fork);
		REGISTMETHODNAME(exec, global_exec);
		REGISTMETHODNAME(exit, global_exit);
		REGISTMETHODNAME(wait, global_wait);
		REGISTMETHODNAME(notify, global_trigger);
		sq_pop(v, 1); // root
	}
	
public:

	/*
	 * ���s�������C�����[�v
	 * @param tick tick�l
	 * ���ݑ��݂���X���b�h�𑍂Ȃ߂łP�x�������s����B
	 * �V�X�e���{�̂̃��C�����[�v(�C�x���g�����{�摜����)
	 * ����1�x�����Ăяo�����Ƃŋ@�\����B���ꂼ��̃X���b�h�́A
	 * �������疾���I�� suspend() �܂��� wait�n�̃��\�b�h���Ăяo���ď�����
	 * ���̃X���b�h�ɈϏ�����K�v������B
	 */
	static void main(long tick) {
		//dm("tick:" + tick + "\n");
		if (first) {
			prevTick = tick;
			first = false;
		}
		currentTick = tick;
		long diff = tick - prevTick;
		std::vector<SQObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			MyThread *th = i->getMyThread();
			if (!th || th->_main(diff)) {
				i = threadList.erase(i);
			} else {
				i++;
			}
		}
		if (threadList.size() == 0) {
			sqobject_stop();
		}
		prevTick = tick;
	};

};

std::vector<SQObjectInfo> MyThread::threadList; //< �X���b�h�ꗗ
bool MyThread::first;        //< ����Ăяo������
long MyThread::prevTick;     //< �O��̌Ăяo������
long MyThread::currentTick;  //< ����̌Ăяo������

/**
 * ���̃I�u�W�F�N�g��҂��Ă���P�X���b�h�̑҂�������
 */
void
MyObject::_notify() {
	std::vector <MyThread *>::iterator i = _waitThreadList.begin();
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
MyObject::_notifyAll()
{
	std::vector <MyThread *>::iterator i = _waitThreadList.begin();
	while (i != _waitThreadList.end()) {
		(*i)->notifyObject(this);
		i = _waitThreadList.erase(i);
	}
}

/**
 * �I�u�W�F�N�g/�X���b�h�����̏�����
 */
void
sqobject_init(HSQUIRRELVM v)
{
	MyObject::registClass(v);
	MyThread::registClass(v);
	MyThread::registGlobal(v);
}

/**
 * �I�u�W�F�N�g/�X���b�h�������C������
 * @param tick tick�l
 */
void
sqobject_main(int tick)
{
	MyThread::main(tick);
}
