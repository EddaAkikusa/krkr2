/**
 * �g���g���N���X�̎擾
 * @param className �g���g���N���X���w��(������)
 * @param ... �p�����Ă���e�N���X���
 * @return squirrel�N���X
 *
 * �g���g���̃N���X�� squirrel�N���X�Ƃ��Ď擾���܂��B
 * ���̃N���X�͌p���\�ł��B
 *
 * ���g���g�����Őe�N���X�����Q�Ɛ����ł��Ȃ����߁A
 * �e�N���X���p�����Ă���N���X�̖��O�����ׂĎ蓮�ŗ񋓂���K�v������܂��B
 * �܂����̋@�\�ō쐬�����g���g���N���X�̃C���X�^���X���g���g��������
 * �Ԃ����ꍇ�́Asquirrel �̃N���X�ł̃C���X�^���X�Ƃ��ă��b�s���O����܂��B
 */
function createTJSClass(className, ...);

/**
 * �g���g���N���X�� squirrel �ɂ�������N���X�\��
 * ���̃C���X�^���X�ɑ΂���g���g��������̃����o�Q�Ƃ́A���̋g���g���C���X�^���X�̂��ꂪ�Ă΂�܂����A
 * ���݂��ĂȂ������o�̏ꍇ�́Amissing �@�\�ɂ�� squirrel ���I�u�W�F�N�g�̓��������o���Q�Ƃ���܂��B
 */
class TJSObject extends Object {

	/**
	 * �R���X�g���N�^
	 */
	constructor();

	/**
	 * �g���g���I�u�W�F�N�g�̗L�����̊m�F
	 * @return valid �Ȃ� true
	 */
	function tjsIsValid();
	
	/**
	 * �g���g���I�u�W�F�N�g�̋����I�[�o���C�h����
	 * �g���g���C���X�^���X�̃����o�������I�ɒu�������܂��B
	 * �C�x���g������ squirrel ���ōs�������ꍇ�A���ɂ����N���X���Œ�`�������ꍇ�́Amissing ������
	 * �����I�� squirrel �����o���Q�Ƃ���܂����A���ɂ����N���X���Œ�`�����ɂ����āA������㏑��������
	 * �ꍇ�Ɏg���܂��B
	 * @param name �����o��
	 * @param value �o�^����l
	 */
	function tjsOverride(name, value);
};

/**
 * TJS�p��NULL�l�Btjs�̃��\�b�h�� null ��n���K�v������ꍇ�ȂǂɎg���܂��B
 * squirrel �� null �� tjs �ł� void �����ł��B
 */
tjsNull;
