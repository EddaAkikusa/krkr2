#ifndef __SQTJS_OBJ_H_
#define __SQTJS_OBJ_H_

#include <sqobject.h>

/**
 * �g���g���I�u�W�F�N�g��ێ�����N���X
 */
class TJSObject : public sqobject::Object {

public:
	/**
	 * �������p
	 */
	static void init(HSQUIRRELVM vm);

	/**
	 * �p���p
	 */
	static void done();

	// ---------------------------------------------------------------
	
	/**
	 * �X�^�b�N����̋g���g���I�u�W�F�N�g�̎擾
	 * @param v squirrelVM
	 * @param idx �C���f�b�N�X
	 * @return �g���g���f�B�X�p�b�`��
	 */
	static iTJSDispatch2 *getDispatch(HSQUIRRELVM v, int idx);

	/**
	 * �X�^�b�N�ւ̋g���g���I�u�W�F�N�g�̓o�^
	 * @return �o�^���������� true
	 */
	static bool pushDispatch(HSQUIRRELVM v, iTJSDispatch2 *dispatch);

	// ---------------------------------------------------------------
	
	/**
	 * �g���g���N���X���� squirrel �N���X�𐶐�
	 * @param v squirrelVM
	 */
	static SQRESULT createClass(HSQUIRRELVM v);

	// ---------------------------------------------------------------
	
	/**
	 * TJS�I�u�W�F�N�g�p�̃��\�b�h
	 * ����1 �I�u�W�F�N�g
	 * ����2�`�z��
	 * ���R�ϐ�1 �����o��
	 */
	static SQRESULT tjsInvoker(HSQUIRRELVM v);

	/**
	 * TJS�I�u�W�F�N�g�p�̃v���p�e�B�Q�b�^�[
	 * ����1 �I�u�W�F�N�g
	 * ���R�ϐ�1 �v���p�e�B��
	 */
	static SQRESULT tjsGetter(HSQUIRRELVM v);

	/**
	 * TJS�I�u�W�F�N�g�p�̃v���p�e�B�Z�b�^�[
	 * ����1 �I�u�W�F�N�g
	 * ����2 �ݒ�l
	 * ���R�ϐ�1 �v���p�e�B��
	 */
	static SQRESULT tjsSetter(HSQUIRRELVM v);

	/**
	 * TJS�I�u�W�F�N�g�p�̃��\�b�h
	 * ����1 �I�u�W�F�N�g
	 * ����2�`�z��
	 * ���R�ϐ�1 �N���X��
	 * ���R�ϐ�2 �����o��
	 */
	static SQRESULT tjsStaticInvoker(HSQUIRRELVM v);

	/**
	 * TJS�I�u�W�F�N�g�p�̃v���p�e�B�Q�b�^�[
	 * ����1 �I�u�W�F�N�g
	 * ���R�ϐ�1 �N���X��
	 * ���R�ϐ�2 �v���p�e�B��
	 */
	static SQRESULT tjsStaticGetter(HSQUIRRELVM v);

	/**
	 * TJS�I�u�W�F�N�g�p�̃v���p�e�B�Z�b�^�[
	 * ����1 �I�u�W�F�N�g
	 * ����2 �ݒ�l
	 * ���R�ϐ�1 �N���X��
	 * ���R�ϐ�2 �v���p�e�B��
	 */
	static SQRESULT tjsStaticSetter(HSQUIRRELVM v);
	
protected:
	// �R���X�g���N�^
	TJSObject(HSQUIRRELVM v);

	// �R���X�g���N�^
	// �C���X�^���X��Ԃ��ꍇ�p
	TJSObject(HSQUIRRELVM v, iTJSDispatch2 *dispatch);
	
	// �f�X�g���N�^
	~TJSObject();
	
	/**
	 * �I�u�W�F�N�g�̃����[�T
	 */
	static SQRESULT release(SQUserPointer up, SQInteger size);

	/**
	 * TJS�I�u�W�F�N�g�̃R���X�g���N�^
	 * TJS�I�u�W�F�N�g�̃R���X�g���N�^
	 * ����1 �I�u�W�F�N�g
	 * ����2�` ����
	 * ���R�ϐ�1 �N���X��
	 */
	static SQRESULT tjsConstructor(HSQUIRRELVM v);

private:
	// �����ΏۃI�u�W�F�N�g
	iTJSDispatch2 *dispatch;

	// �o�^���ꂽ�N���X�̖��O�}�b�v
	static sqobject::ObjectInfo classMap;
};

#endif
