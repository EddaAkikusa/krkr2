#ifndef __WindowExBase__
#define __WindowExBase__

#include <windows.h>
#include "tp_stub.h"

/**
 * �E�C���h�E�g�� ��{���ێ��p�l�C�e�B�u�C���X�^���X�B
 */
class NI_WindowExBase : public tTJSNativeInstance
{
protected:
	// �E�C���h�E��������擾���邽�߂̃v���p�e�B
	static iTJSDispatch2 * _HWNDProp;
	static iTJSDispatch2 * _innerWidthProp;
	static iTJSDispatch2 * _innerHeightProp;
	static iTJSDispatch2 * _primaryLayerProp;
	
	// ���C����������擾���邽�߂̃v���p�e�B
	static iTJSDispatch2 * _leftProp;
	static iTJSDispatch2 * _topProp;
	static iTJSDispatch2 * _widthProp;
	static iTJSDispatch2 * _heightProp;
	static iTJSDispatch2 * _pitchProp;
	static iTJSDispatch2 * _bufferProp;
	static iTJSDispatch2 * _updateProp;

	// ���Z�b�g�����p work �ϐ�
	iTJSDispatch2 *_windowobj;
public:
	// ���Z�b�g�������s
	void _reset();
	
public:
	// ���ێ��p
	HWND _hwnd;
	int _innerWidth;
	int _innerHeight;

	// ���C������r�ێ��p
	tjs_int _width;
	tjs_int _height;
	tjs_int _pitch;
	unsigned char *_buffer;

public:
	// �N���X�h�c�ێ��p
	static int classId;

	/**
	 * �������������s
	 * @param windowobj �E�C���h�E�N���X�I�u�W�F�N�g
	 * @parma layerobj  ���C���N���X�I�u�W�F�N�g
	 */
	static void init(iTJSDispatch2 *windowobj, iTJSDispatch2 *layerobj);

	/**
	 * ����������s
	 */
	static void unInit();
	
	/**
	 * �l�C�e�B�u�I�u�W�F�N�g�̎擾
	 * @param objthis this
	 * @parma crete true �Ȃ瑶�݂��Ȃ��ꍇ�쐬
	 * @return �l�C�e�B�u�I�u�W�F�N�g
	 */
	static NI_WindowExBase *getNative(iTJSDispatch2 *objthis, bool create=true);

	/**
	 * �O���t�B�b�N������������
	 * ���C���̃r�b�g�}�b�v��񂪕ύX����Ă���\��������̂Ŗ���`�F�b�N����B
	 * �ύX����Ă���ꍇ�͕`��p�̃R���e�L�X�g��g�݂Ȃ���
	 * @param windowobj ���C���I�u�W�F�N�g
	 * @return ���������s���ꂽ�ꍇ�� true ��Ԃ�
	 */
	void reset(iTJSDispatch2 *windowobj);
	
public:
	/**
	 * �R���X�g���N�^
	 */
	NI_WindowExBase();
};

#endif
