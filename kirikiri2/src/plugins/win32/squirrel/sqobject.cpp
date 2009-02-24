/**
 * squirrel �p���I�u�W�F�N�g�{�^���X���b�h
 */
#include "sqobject.h"

#include <ctype.h>
#include <sqstdio.h>
#include <string>
#include <new>

// ���O�o�͗p
#define PRINT(v,msg) {\
	SQPRINTFUNCTION print = sq_getprintfunc(v);\
	if (print) {\
		print(v,msg);\
	}\
}

/**
 * ���[�v���s�J�n
 * �X���b�h���������ꂽ�̂Ń��C�����[�v���J�n���鏈�����Ăяo��
 * ���[�U���Œ�`����K�v������܂�
 */
extern void sqobject_start();

namespace sqobject {

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

SQRESULT ERROR_CREATE(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("can't create native instance"));
}

SQRESULT ERROR_NOMEMBER(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("no such member"));
}

SQRESULT ERROR_BADINSTANCE(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("bad instance"));
}

void getSetterName(tstring &store, const SQChar *name)
{
	store = _SC("set");
	store += toupper(*name);
	store += (name + 1);
}

void getGetterName(tstring &store, const SQChar *name)
{
	store = _SC("get");
	store += toupper(*name);
	store += (name + 1);
}

static const SQUserPointer OBJTYPETAG = (SQUserPointer)"OBJTYPETAG";
static const SQUserPointer THREADTYPETAG = (SQUserPointer)"THREADTYPETAG";

// ---------------------------------------------------------
// ObjectInfo
// ---------------------------------------------------------

/**
 * squirrel �I�u�W�F�N�g�ێ��p�N���X
 */
class ObjectInfo {

protected:
	HSQUIRRELVM v; // �I�u�W�F�N�g�̑����Ă���VM
	HSQOBJECT obj; // �I�u�W�F�N�g�Q�Ə��

public:
	// ���e����
	void clear() {
		sq_release(v,&obj);
		v = NULL;
		sq_resetobject(&obj);
	}

	// �X�^�b�N����擾
	void getStack(HSQUIRRELVM v, int idx) {
		clear();
		this->v = v;
		sq_getstackobj(v, idx, &obj);
		sq_addref(v, &obj);
	}

	// �R���X�g���N�^
	ObjectInfo() : v(NULL) {
		sq_resetobject(&obj);
	}

	// �R���X�g���N�^
	ObjectInfo(HSQUIRRELVM v, int idx) : v(v) {
		sq_resetobject(&obj);
		sq_getstackobj(v, idx, &obj);
		sq_addref(v, &obj);
	}

	// �R�s�[�R���X�g���N�^
	ObjectInfo(const ObjectInfo &orig) {
		v   = orig.v;
		obj = orig.obj;
		sq_addref(v, &obj);
	}

	// ���
	ObjectInfo & operator=(const ObjectInfo &orig) {
		clear();
		v = orig.v;
		obj = orig.obj;
		sq_addref(v, &obj);
		return *this;
	}

	// �f�X�g���N�^
	virtual ~ObjectInfo() {
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
	Thread *getThread() {
		if (sq_isinstance(obj)) {
			sq_pushobject(v, obj);
			SQUserPointer up = NULL;
			if (SQ_SUCCEEDED(sq_getinstanceup(v, -1, &up, THREADTYPETAG))) {
				sq_pop(v,1);
				return (Thread*)up;
			}
			sq_pop(v, 1);
		}
		return NULL;
	}

	// �C���X�^���X���[�U�|�C���^���擾
	Object *getObject() {
		if (sq_isinstance(obj)) {
			sq_pushobject(v, obj);
			Object *ret = Object::getObject(v, -1);
			sq_pop(v, 1);
			return ret;
		}
		return NULL;
	}
	
	// ---------------------------------------------------
	// wait�����p���\�b�h
	// ---------------------------------------------------

	bool isSameString(const SQChar *str) const {
		if (str && sq_isstring(obj)) {
			const SQChar *mystr;
			sq_pushobject(v, obj);
			sq_getstring(v, -1, &mystr);
			sq_pop(v, 1);
			return mystr && scstrcmp(str, mystr) == 0;
		}
		return false;
	}
};

// ---------------------------------------------------------
// Object
// ---------------------------------------------------------

/**
 * �I�u�W�F�N�g�҂��̓o�^
 * @param thread �X���b�h
 */
void
Object::addWait(Thread *thread) {
	_waitThreadList.push_back(thread);
}

/**
 * �I�u�W�F�N�g�҂��̉���
 * @param thread �X���b�h
 */
void
Object::removeWait(Thread *thread) {
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
Object::Object() : delegate(NULL) {
}

/**
 * �f�X�g���N�^
 */
Object::~Object() {
	_notifyAll();
	if (delegate) {
		delete delegate;
	}
}

// ------------------------------------------------------------------

/**
 * @return �I�u�W�F�N�g���I�u�W�F�N�g
 */
Object *
Object::getObject(HSQUIRRELVM v, int idx) {
	SQUserPointer up;
	std::vector<SQUserPointer>::const_iterator i = tags.begin();
	while (i != tags.end()) {
		if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, *i))) {
			return (Object*)up;
		}
		i++;
	}
	return NULL;
}

/**
 * �I�u�W�F�N�g�̃����[�T
 */
SQRESULT
Object::release(SQUserPointer up, SQInteger size) {
	Object *self = (Object*)up;
	delete self;
	return SQ_OK;
}

/**
 * �I�u�W�F�N�g�̃R���X�g���N�^
 */
SQRESULT
Object::constructor(HSQUIRRELVM v) {
	SQRESULT result = SQ_OK;
	Object *self = new Object();
	if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {;
		sq_setreleasehook(v, 1, release);
	} else {
		delete self;
	}
	if (SQ_SUCCEEDED(result)) {
		if (getParamCount(v) > 0) {
			self->delegate = new ObjectInfo(v, 2);
		}
	}
	return result;
}

bool
Object::isClosure(SQObjectType type) {
	return type == OT_CLOSURE || type == OT_NATIVECLOSURE;
}
	
/**
 * �v���p�e�B����l���擾
 * @param name �v���p�e�B��
 * @return �v���p�e�B�l
 */
SQRESULT
Object::_get(HSQUIRRELVM v) {
	SQRESULT result = SQ_OK;
	const SQChar *name = getString(v, 2);
	if (name && *name) {
		
			// delegate�̎Q��
		Object *self = getObject(v, 1);
		if (self && self->delegate &&  self->delegate->isDelegate()) {
			self->delegate->push(v);
			sq_pushstring(v, name, -1);
			if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
				// ���\�b�h�̏ꍇ�͑�������
				if (isClosure(sq_gettype(v,-1)) && self->delegate->isBindDelegate()) {
					self->delegate->push(v);
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
SQRESULT
Object::_set(HSQUIRRELVM v) {
	SQRESULT result = SQ_OK;
	const SQChar *name = getString(v, 2);
	if (name && *name) {
		
		// delegate�̎Q��
		Object *self = getObject(v, 1);
		if (self && self->delegate && self->delegate->isDelegate()) {
			self->delegate->push(v);
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
SQRESULT
Object::hasSetProp(HSQUIRRELVM v) {
	SQRESULT result = SQ_OK;
	SQBool ret = SQFalse;
	if (getParamCount(v) >= 2) {
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
Object::setDelegate(HSQUIRRELVM v) {
	Object *self = getObject(v, 1);
	if (self) {
		if (self->delegate) {
			delete self->delegate;
			self->delegate = NULL;
		}
		if (getParamCount(v) > 0) {
			self->delegate = new ObjectInfo(v,2);
		}
		return SQ_OK;
	} else {
		return ERROR_BADINSTANCE(v);
	}
}

/**
 * �Ϗ��̐ݒ�
 */
SQRESULT
Object::getDelegate(HSQUIRRELVM v) {
	Object *self = getObject(v, 1);
	if (self) {
		if (self->delegate) {
			self->delegate->push(v);
		} else {
			sq_pushnull(v);
		}
		return 1;
	} else {
		return ERROR_BADINSTANCE(v);
	}
}

/**
 * �P��X���b�h�ւ̃I�u�W�F�N�g�҂��̏I���ʒm�p
 */
SQRESULT
Object::notify(HSQUIRRELVM v) {
	Object *self = getObject(v, 1);
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
SQRESULT
Object::notifyAll(HSQUIRRELVM v) {
	Object *self = getObject(v, 1);
	if (self) {
		self->_notifyAll();
		return SQ_OK;
	} else {
		return ERROR_BADINSTANCE(v);
	}
}

/**
 * �N���X�̓o�^
 * @param v squirrel VM
 */
void
Object::registClass(HSQUIRRELVM v) {

	Object::pushTag(OBJTYPETAG);

	sq_pushroottable(v); // root
	sq_pushstring(v, SQOBJECTNAME, -1);
	sq_newclass(v, false);
	sq_settypetag(v, -1, OBJTYPETAG);
	
	REGISTMETHOD(constructor);
	REGISTMETHOD(_set);
	REGISTMETHOD(_get);
	REGISTMETHOD(hasSetProp);
	REGISTMETHOD(setDelegate);
	REGISTMETHOD(getDelegate);
	REGISTMETHOD(notify);
	REGISTMETHOD(notifyAll);
	sq_createslot(v, -3);
	sq_pop(v,1); // root
};

std::vector<SQUserPointer> Object::tags;

class Thread : public Object {

protected:
	long _currentTick; //< ���̃X���b�h�̎��s����

	// �X���b�h�f�[�^
	ObjectInfo _thread;

	// �҂��Ώ�
	std::vector<ObjectInfo> _waitList;
	// �҂�����
	SQInteger _waitTimeout;

	// �҂����^�C���A�E�g������
	ObjectInfo _waitResult;

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
	bool notifyObject(Object *target) {
		bool find = false;
		std::vector<ObjectInfo>::iterator i = _waitList.begin();
		while (i != _waitList.end()) {
			Object *obj = i->getObject();
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
		std::vector<ObjectInfo>::iterator i = _waitList.begin();
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
	Thread() : _currentTick(0), _waitTimeout(-1), _status(THREAD_STOP) {
	}
	
	// �f�X�g���N�^
	~Thread() {
		_clear();
	}

	/**
	 * �I�u�W�F�N�g�ɑ΂���҂������N���A����
	 * @param status �L�����Z���̏ꍇ�� true
	 */
	void _clearWait() {
		std::vector<ObjectInfo>::iterator i = _waitList.begin();
		while (i != _waitList.end()) {
			Object *obj = i->getObject();
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
	void _wait(HSQUIRRELVM v) {
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
				// �҂����X�g�ɓo�^
				_waitList.push_back(ObjectInfo(v,i));
				break;
			case OT_INSTANCE:
				// �I�u�W�F�N�g�ɑ҂��o�^���Ă���҂����X�g�ɓo�^
				{
					ObjectInfo o(v,i);
					Object *obj = o.getObject();
					if (obj) {
						obj->addWait(this);
					}
					_waitList.push_back(o);
				}
				break;
			}
		}
	}

	/**
	 * wait�̃L�����Z��
	 */
	void _cancelWait() {
		_clearWait();
		_waitResult.clear();
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
	SQRESULT _exec(HSQUIRRELVM v) {
		_clear();

		HSQUIRRELVM testVM = sq_newthread(v, 1024);
		_thread.getStack(v, -1);
		sq_pop(v, 1);
		
		// �X���b�h�擪�ɃX�N���v�g�����[�h
		if (sq_gettype(v, 2) == OT_STRING) {
			SQRESULT result;
			if (SQ_FAILED(result = sqstd_loadfile(_thread, getString(v, 2), SQTrue))) {
				_clear();
				return result;
			}
		} else {
			sq_move(_thread, v, 2);
		}
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
			SQRESULT result;
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
					PRINT(_thread, str);
				} else {
					PRINT(_thread, _SC("failed to run by unknown reason"));
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
	static Thread *getThread(HSQUIRRELVM v, int idx) {
		SQUserPointer up;
		if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, THREADTYPETAG))) {
			return (Thread*)up;
		}
		return NULL;
	}

	/**
	 * �I�u�W�F�N�g�̃����[�T
	 */
	static SQRESULT release(SQUserPointer up, SQInteger size) {
		Thread *self = (Thread*)up;
		delete self;
		return SQ_OK;
	}
	
	/**
	 * �I�u�W�F�N�g�̃R���X�g���N�^
	 */
	static SQRESULT constructor(HSQUIRRELVM v) {
		SQRESULT result = SQ_OK;
		Thread *self = new Thread();
		if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {
			sq_setreleasehook(v, 1, release);
		} else {
			delete self;
		}
		if (SQ_SUCCEEDED(result)) {
			if (getParamCount(v) > 1) {
				self->delegate = new ObjectInfo(v, 3);
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
	
	static SQRESULT getCurrentTick(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
		if (self) {
			sq_pushinteger(v, self->_currentTick);
			return 1;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	static SQRESULT getStatus(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
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
	static SQRESULT exec(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
		if (self) {
			return self->_exec(v);
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	/**
	 * �����𒆒f����
	 */
	static SQRESULT exit(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
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
	static SQRESULT stop(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
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
	static SQRESULT run(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
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
	static SQRESULT wait(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
		if (self) {
			self->_wait(v);
			return SQ_OK;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	/**
	 * �����҂�
	 */
	static SQRESULT cancelWait(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
		if (self) {
			self->_cancelWait();
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

		Object::pushTag(THREADTYPETAG);

		sq_pushroottable(v); // root
		sq_pushstring(v, SQTHREADNAME, -1);
		sq_pushstring(v, SQOBJECTNAME, -1);
		sq_get(v,-3);
		sq_newclass(v, true); // �p������
		sq_settypetag(v, -1, THREADTYPETAG);

		REGISTENUM(STOP, THREAD_STOP);
		REGISTENUM(RUN, THREAD_RUN);
		REGISTENUM(WAIT, THREAD_WAIT);
		REGISTENUM(END, THREAD_END);
		REGISTMETHOD(constructor);
		REGISTMETHOD(getCurrentTick);
		REGISTMETHOD(getStatus);
		REGISTMETHOD(exec);
		REGISTMETHOD(exit);
		REGISTMETHOD(stop);
		REGISTMETHOD(run);
		REGISTMETHOD(wait);
		REGISTMETHOD(cancelWait);
		sq_createslot(v, -3);
		sq_pop(v, 1); // root
	};
	
	// -------------------------------------------------------------
	//
	// �O���[�o���X���b�h����p���@�\
	//
	// -------------------------------------------------------------
	
protected:
	static std::vector<ObjectInfo> threadList; //< �X���b�h�ꗗ
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
			first = true;
		}
		threadList.push_back(ObjectInfo(v,1));
	}

	// -------------------------------------------------------------
	// �O���[�o�����\�b�h�p
	// -------------------------------------------------------------

	/**
	 * ���ݎ����̎擾
	 */
	static SQRESULT global_getCurrentTick(HSQUIRRELVM v) {
		sq_pushinteger(v, currentTick);
		return 1;
	}

	static SQRESULT ERROR_NOTHREAD(HSQUIRRELVM v) {
		return sq_throwerror(v, _SC("no thread"));
	}
	
	/*
	 * @return ���݂̃X���b�h��Ԃ�
	 */
	static SQRESULT global_getCurrentThread(HSQUIRRELVM v) {
		std::vector<ObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			Thread *th = i->getThread();
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
	static SQRESULT global_getThreadList(HSQUIRRELVM v) {
		sq_newarray(v, 0);
		std::vector<ObjectInfo>::const_iterator i = threadList.begin();
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
	static SQRESULT global_fork(HSQUIRRELVM v) {
		//dm("�X���b�h�𐶐�!");
		SQRESULT result;
		sq_pushroottable(v); // root
		sq_pushstring(v, SQTHREADNAME, -1);
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
	 * @return ���ݎ��s���̃X���b�h���I�u�W�F�N�g(Thread*)
	 */
	static Thread *getCurrentThread(HSQUIRRELVM v) {
		std::vector<ObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			Thread *th = i->getThread();
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
	static SQRESULT global_exec(HSQUIRRELVM v) {
		Thread *th = getCurrentThread(v);
		if (th) {
			SQRESULT result;
			if (SQ_FAILED(result = th->_exec(v))) {
				return result;
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
	static SQRESULT global_exit(HSQUIRRELVM v) {
		Thread *th = getCurrentThread(v);
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
	static SQRESULT global_wait(HSQUIRRELVM v) {
		Thread *th = getCurrentThread(v);
		if (th) {
			th->_wait(v);
			return sq_suspendvm(v);
		} else {
			return ERROR_NOTHREAD(v);
		}
	}

	/**
	 * �S�X���b�h�ւ̃g���K�ʒm
	 * @param name �����҂��g���K��
	 */
	static SQRESULT global_trigger(HSQUIRRELVM v) {
		const SQChar *name = getString(v, 2);
		std::vector<ObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			Thread *th = i->getThread();
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
	 * @return 
	 */
	static int main(long tick) {
		//dm("tick:" + tick + "\n");
		if (first) {
			prevTick = tick;
			first = false;
		}
		currentTick = tick;
		long diff = tick - prevTick;
		std::vector<ObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			Thread *th = i->getThread();
			if (!th || th->_main(diff)) {
				i = threadList.erase(i);
			} else {
				i++;
			}
		}
		prevTick = tick;
		return threadList.size();
	};


	static void done() {
		threadList.clear();
	}
};

std::vector<ObjectInfo> Thread::threadList; //< �X���b�h�ꗗ
bool Thread::first;        //< ����Ăяo������
long Thread::prevTick;     //< �O��̌Ăяo������
long Thread::currentTick;  //< ����̌Ăяo������

/**
 * ���̃I�u�W�F�N�g��҂��Ă���P�X���b�h�̑҂�������
 */
void
Object::_notify() {
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
Object::_notifyAll()
{
	std::vector <Thread *>::iterator i = _waitThreadList.begin();
	while (i != _waitThreadList.end()) {
		(*i)->notifyObject(this);
		i = _waitThreadList.erase(i);
	}
}

};

/**
 * �I�u�W�F�N�g/�X���b�h�����̏�����
 * @param v squirrel VM
 */
void
sqobject_init(HSQUIRRELVM v)
{
	sqobject::Object::registClass(v);
	sqobject::Thread::registClass(v);
	sqobject::Thread::registGlobal(v);
}

/**
 * �I�u�W�F�N�g/�X���b�h�������C������
 * @param tick tick�l
 */
int
sqobject_main(int tick)
{
	return sqobject::Thread::main(tick);
}

/**
 * �I�u�W�F�N�g/�X���b�h�����I������
 */
void
sqobject_done()
{
	sqobject::Thread::done();
}
