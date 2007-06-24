/**
 * ���b�Z�[�W���N���X
 */
class MessageWindow extends Object {

	// �x�[�X�̃e�L�X�g���C��
	msg = null;
	// �w�i�̓h��Ԃ�
	back = null;
	
	/**
	 * �R���X�g���N�^
	 */
	constructor(x,y,w,h) {
		msg  = MessageLayer(x,y,w,h);
		msg.setLevel(4096);
		back = FillRect(x,y,w,h,4095,0x80000088); // XXX
		back.setLevel(4095);
	}

	/**
	 * �e�L�X�g�̕\��
	 */
	function write(text) {
		msg.write(text);
	}

	/**
	 * �e�L�X�g�̏���
	 */
	function clear() {
		msg.clear();
	}

	/**
	 * ���O�v���[�g�̕\��
	 */
	function showNamePlate(name) {
	}

	/**
	 * �J�[�\���̕\��
	 */
	function showCursor(cursor) {
		//msg.showCursor(cursor);
	}

	/**
	 * �\�����
	 */
	function show(show) {
		print("���b�Z�[�W���\�����:" + show + "\n");
		msg.show(show);
		back.show(show);
	}
}
