/**
 * �g���g���N���X�̎擾
 * @param className �g���g���N���X���w��(������)
 * @param ... �p�����Ă���e�N���X���
 * @return �R���X�g���N�^���\�b�h
 *
 * �g���g���̃N���X�� Javascript �N���X�Ƃ��Ď擾���܂��B
 *
 * ���g���g�����Őe�N���X�����Q�Ɛ����ł��Ȃ����߁A
 * �e�N���X���p�����Ă���N���X�̖��O�����ׂĎ蓮�ŗ񋓂���K�v������܂��B
 * �܂����̋@�\�ō쐬�����g���g���N���X�̃C���X�^���X���g���g��������
 * �Ԃ����ꍇ�͂��̃N���X�̃C���X�^���X�Ƃ��ă��b�s���O����܂��B
 */
function createTJSClass(className, ...);

/**
 * �g���g���N���X�� javascript �ɂ�������N���X�\��
 * ���̃C���X�^���X�ɑ΂���g���g��������̃����o�Q�Ƃ́A���̋g���g���C���X�^���X�̂��ꂪ�Ă΂�܂����A
 * ���݂��ĂȂ������o�̏ꍇ�́Amissing �@�\�ɂ�� javascript ���I�u�W�F�N�g�̓��������o���Q�Ƃ���܂��B
 * �g���g������Ă΂����̂����� squirrel�C���X�^���X�̂���ɂɍ����ւ���ꍇ��
 * tjsOverride() �ŋ����㏑���������邱�Ƃ��ł��܂��B�C�x���g�̓o�^�Ɏg���܂��B
 */
function TJSObject();

/**
 * �g���g���I�u�W�F�N�g�̗L�����̊m�F
 * ���C���Ȃǋg���g�����ŋ��� invalidate �����\��������I�u�W�F�N�g�̏󋵊m�F�Ɏg���܂��B
 * @return valid �Ȃ� true
 */
TJSObject.tjsIsValid = function();

/**
 * �g���g���I�u�W�F�N�g�̋����I�[�o���C�h����
 * �g���g���C���X�^���X�̃����o�������I�ɏ㏑�����܂��B
 * �C�x���g�Ȃǂ� javascript ���ł��������ꍇ�Ɏw�肵�܂�
 * �l���ȗ������ꍇ�͎��ȃI�u�W�F�N�g���Q�Ƃ��܂��B���̂Ƃ��A
 * �N���[�W�����w�肳��Ă����ꍇ�́A�����I�� bindenv(this) ���ꂽ���̂��o�^����܂�
 * @param name �����o��
 * @param value �o�^����l(�ȗ���)
 */
TJSObject.tjsOverride = function(name, value=null);
