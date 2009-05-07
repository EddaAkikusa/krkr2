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
	static void init();

	/**
	 * �N���X�̓o�^
	 * @param v squirrelVM
	 * @param className �o�^�N���X��
	 * @param tjsClassInfo �N���X��`���(TJS�̃N���X���̃��X�g)
	 */
	static void registerClass(HSQUIRRELVM v, const tjs_char *className, tTJSVariant *tjsClassInfo);

	/**
	 * �g���g���I�u�W�F�N�g�̎擾
	 * @param v squirrelVM
	 * @param idx �C���f�b�N�X
	 * @return �g���g���f�B�X�p�b�`��
	 */
	static iTJSDispatch2 *getDispatch(HSQUIRRELVM v, int idx);

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
	
protected:
	// �R���X�g���N�^
	TJSObject(HSQUIRRELVM v);

	// �f�X�g���N�^
	~TJSObject();
	
	/**
	 * �I�u�W�F�N�g�̃����[�T
	 */
	static SQRESULT release(SQUserPointer up, SQInteger size);

	/**
	 * TJS�I�u�W�F�N�g�̃R���X�g���N�^
	 * ����1 �I�u�W�F�N�g
	 * ����2 �N���X��
	 * ����3 ����
	 */
	static SQRESULT tjsConstructor(HSQUIRRELVM v);

private:
	// �����ΏۃI�u�W�F�N�g
	iTJSDispatch2 *dispatch;

};

#endif
