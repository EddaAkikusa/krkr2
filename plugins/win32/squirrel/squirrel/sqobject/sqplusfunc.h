#ifndef SQPLUSFUNC_H
#define SQPLUSFUNC_H

#include "sqobjectinfo.h"
using namespace SqPlus;

/**
 * �e�폈���p�e���v���[�g
 */
template <typename T>
struct SQTemplate {
  static SQRESULT release(SQUserPointer up, SQInteger size) {
	T* self = (T*)up;
	if (self){ 
	  self->destructor();
	  delete self;
	}
	return SQ_OK;
  }
  static SQRESULT _constructor(HSQUIRRELVM v, T *data) {
	if (data) {
	  data->initSelf(v);
	  SqPlus::PostConstruct<T>(v, data, release);
	  return SQ_OK;
	} else {
	  return SQ_ERROR;
	}
  }
  static SQRESULT constructor(HSQUIRRELVM v) {
	T *data = new T();
	if (data) {
	  data->initSelf(v);
	  SqPlus::PostConstruct<T>(v, data, release);
	  return SQ_OK;
	} else {
	  return SQ_ERROR;
	}
  }
  static SQRESULT vconstructor(HSQUIRRELVM v) {
	T *data = new T(v);
	if (data) {
	  SqPlus::PostConstruct<T>(v, data, release);
	  return SQ_OK;
	} else {
	  return SQ_ERROR;
	}
  }
  static SQRESULT noconstructor(HSQUIRRELVM v) {
	return sq_throwerror(v, "can't create instance");
  }
  static SQRESULT destructor(HSQUIRRELVM v) {
	  return SQ_OK;
  }
};

/**
 * �Y������^�̃I�u�W�F�N�g���X�^�b�N�Ɋi�[����B
 * initSelf�̌Ăяo���������čs��
 * @param name �^��
 * @param obj �I�u�W�F�N�g
 * @return SQRESULT
 */
template <typename T>
SQRESULT pushsqobj(HSQUIRRELVM v, const SQChar *className, T *obj) {
	if (obj && CreateNativeClassInstance(v, className, obj, SQTemplate<T>::release)) {
		obj->initSelf(v,-1);
		return 1;
	} else {
		return sq_throwerror(v, "can't create instance");
	}
}

/**
 * �Y������^�̃I�u�W�F�N�g���X�^�b�N�Ɋi�[����B�������s�����ꍇ�̓I�u�W�F�N�g��delete����
 * initSelf�̌Ăяo���������čs��
 * @param name �^��
 * @param obj �I�u�W�F�N�g
 * @return SQRESULT
 */
template <typename T>
SQRESULT pushnewsqobj(HSQUIRRELVM v, const SQChar *className, T *obj) {
	if (obj && CreateNativeClassInstance(v, className, obj, SQTemplate<T>::release)) {
		obj->initSelf(v,-1);
		return 1;
	} else {
		delete obj;
		return sq_throwerror(v, "can't create instance");
	}
}

// ------------------------------------------------------------------
// �N���X�o�^�p�}�N��
// ------------------------------------------------------------------

// ----------------------------------------------
// �N���X��`�p�}�N��
// �N���X�̖��O�Q�Ƃ̂��߂��炩���� DECLARE_INSTANCE_TYPE_* �Ō^�����`���Ă����K�v������܂�

// �N���X��`(�ʏ�p)
#define SQCLASS(Class) SQClassDefNoConstructor<Class> cls(TypeInfo<Class>().typeName)
// �N���X��` sqobject::Object���p���i�^��:Object)
#define SQCLASSOBJ(Class) SQClassDefNoConstructor<Class,Object> cls(TypeInfo<Class>().typeName, SQOBJECTNAME)
// �N���X��` �C�ӌp��
#define SQCLASSEX(Class,Parent) SQClassDefNoConstructor<Class,Parent> cls(TypeInfo<Class>().typeName, TypeInfo<Parent>().typeName)

// ----------------------------------------------
// �R���X�g���N�^��`�p�}�N��

// sqobject::Object �n�R���X�g���N�^�E�����Ȃ�
#define SQCONSTRUCTOR(Class) cls.staticFuncVarArgs(SQTemplate<Class>::constructor, _SC("constructor"))
#define SQCONSTRUCTOR_(Class, TypeCheck) cls.staticFuncVarArgs(SQTemplate<Class>::constructor, _SC("constructor"), TypeCheck)

// sqobject::Object �n�R���X�g���N�^�EHSQUIRREL������
#define SQVCONSTRUCTOR(Class) cls.staticFuncVarArgs(SQTemplate<Class>::vconstructor, _SC("constructor"))
#define SQVCONSTRUCTOR_(Class,TypeCheck) cls.staticFuncVarArgs(SQTemplate<Class>::vconstructor, _SC("constructor"), TypeCheck)

// �N���X�� static SQRESULT constructor(HSQUIRRELVM) �����ꍇ�p
#define SQSCONSTRUCTOR(Class) cls.staticFuncVarArgs(Class::constructor, _SC("constructor"))
#define SQSCONSTRUCTOR_(Class,TypeCheck) cls.staticFuncVarArgs(Class::constructor, _SC("constructor"), TypeCheck)

// �R���X�g���N�^����
#define SQNOCONSTRUCTOR(Class) cls.staticFuncVarArgs(SQTemplate<Class>::noconstructor, _SC("constructor"))

// ���O�R���X�g���N�^
#define SQMYCONSTRUCTOR(constructor) cls.staticFuncVarArgs(constructor, _SC("constructor"))
#define SQMYCONSTRUCTOR_(constructor, TypeCheck) cls.staticFuncVarArgs(constructor, _SC("constructor"), TypeCheck)

// �f�X�g���N�^(��)
#define SQDESTRUCTOR(Class) cls.staticFuncVarArgs(SQTemplate<Class>::destructor, _SC("destructor"))

// ----------------------------------------------
// �t�@���N�V�����o�^�p�}�N��

#define SQFUNC(Class, Name) cls.func(&Class::Name,_SC(#Name))
#define SQFUNC_(Class, Name, TypeCheck) cls.func(&Class::Name,_SC(#Name), TypeCheck)
#define SQVFUNC(Class, Name) cls.funcVarArgs(&Class::Name,_SC(#Name))
#define SQVFUNC_(Class, Name, TypeCheck) cls.funcVarArgs(&Class::Name,_SC(#Name), TypeCheck)
#define SQSFUNC(Class, Name) cls.staticFunc(&Class::Name,_SC(#Name))
#define SQSFUNC_(Class, Name, TypeCheck) cls.staticFunc(&Class::Name,_SC(#Name), TypeCheck)
#define SQSVFUNC(Class, Name) cls.staticFuncVarArgs(&Class::Name,_SC(#Name))
#define SQSVFUNC_(Class, Name, TypeCheck) cls.staticFuncVarArgs(&Class::Name,_SC(#Name), TypeCheck)

#define SQPUSH(v, obj) pushsqobj(v, GetTypeName(*obj), obj)
#define SQPUSHNEW(v, obj) pushnewsqobj(v, GetTypeName(*obj), obj)
#define SQSETINSTANCE(v, Class, param) SQTemplate<Class>::_constructor(v, new Class param)

#endif
