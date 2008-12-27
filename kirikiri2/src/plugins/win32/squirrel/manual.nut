//
// squirrel �^���X���b�h�����x�����C�u����
//

/**
 * ���I�u�W�F�N�g
 * �X���b�h�̓I�u�W�F�N�g���u�҂v���Ƃ��ł��܂��B
 */
class Object {

	/**
	 * �R���X�g���N�^
	 */
	constructor();

	/**
	 * ���̃I�u�W�F�N�g��҂��Ă���X���b�h1�ɏI����ʒm����
	 */
	function notify();

	/**
	 * ���̃I�u�W�F�N�g��҂��Ă���S�X���b�h�ɏI����ʒm����
	 * �����̃��\�b�h�̓I�u�W�F�N�g�p�����ɂ����s����܂��B
	 */
	function notifyAll();

	/**
	 * @param name �v���p�e�B��
	 * @return �w�肳�ꂽ���O�̃v���p�e�B�� setter ������� true
	 */
	function hasSetProp(name);

	/**
	 * �v���p�e�B�@�\�ɂ���
	 * Object ���p�������I�u�W�F�N�g�́A�v���p�e�B�@�\�������܂��B
	 * �Y�����郁���o�����݂��Ȃ������ꍇ�A�����I�� getter/setter �t�@���N�V������T���āA
	 * ���ꂪ����΂�����Ăяo���܂��B
	 * val = obj.name; �� val = obj.getName();
	 * obj.name = val; �� obj.setName(val)
	 */ 
};

/**
 * �X���b�h����p�I�u�W�F�N�g
 * �^���X���b�h�𐧌䂷�邽�߂̃I�u�W�F�N�g�ł��B
 * �X���b�h�����s���̏ꍇ�A���[�U�̎Q�Ƃ��Ȃ��Ȃ��Ă��V�X�e�����Q�Ƃ��ێ����܂��B
 */
class Thread extends Object {

	// �X���b�h�̃X�e�[�g
	THREAD_STOP = 0; // ��~
	THREAD_RUN = 1;  // ���s��
	THREAD_WAIT = 2; // �����҂�
	THREAD_END = 3;  // �I��
	
	/**
	 * �R���X�g���N�^
	 * @param func �X���b�h�𐶐�����s����t�@���N�V�����܂��̓t�@�C����
	 */
	constructor(func=null);

	/**
	 * @return ���̃X���b�h�̎��s����
	 */
	function getCurrentTick();

	/**
	 * @return ���̃X���b�h�̎��s�X�e�[�^�X THREAD_STOP/THREAD_RUN/THREAD_WAIT/THREAD_END
	 */
	function getStatus();

	/**
	 * �X���b�h�̎��s�J�n
	 * @param func �Ăяo���O���[�o���֐��܂��̓X�N���v�g�t�@�C����
	 */
	function exec(func);

	/**
	 * �X���b�h�̏I��
	 */
	function exit();

	/**
	 * �X���b�h�̈ꎞ��~
	 */
	function stop();

	/**
	 * �ꎞ��~�����X���b�h�̍ĊJ
	 */
	function run();

	/**
	 * �X���b�h�̎��s�҂��B�����ꂩ�̏����ŉ�������܂��B�������w�肵�Ȃ������ꍇ�ł��A
	 * �������񏈗������f���āA�V�X�e�����̃C�x���g/�X�V����������ɕ��A���܂��B
	 * @param param �҂������w�� ������:�g���K�҂� �I�u�W�F�N�g:�I�u�W�F�N�g�҂� ���l:���ԑ҂�(ms)���ŏ��̎w�肪�L��
	 * @return wait�����̌���
	 */
	function wait(param, ...);
};

/**
 * @return �ғ����X���b�h�̈ꗗ(�z��)��Ԃ��܂��B
 */
function getThreadList();

/**
 * @return ���ݎ��s���̃X���b�h��Ԃ��܂��B
 */
function getCurrentThread();

/**
 * �V�����X���b�h�𐶐����Ď��s���܂��BThread(func) �Ɠ����ł��B
 * @param func �Ăяo�����\�b�h�A�܂��̓t�@�C����
 * @return �V�K�X���b�h����I�u�W�F�N�g(Thread)
 */
function fork(func);

/**
 * ���ݎ��s���̃X���b�h��ʂ̎��s�ɐ؂�ւ��܂�
 * @param func �Ăяo���O���[�o���֐��܂��̓X�N���v�g�t�@�C����
 */
function exec(func);

/**
 * ���ݎ��s���̃X���b�h���I�����܂�
 */
function exit();

/**
 * ���ݎ��s���̃X���b�h�̎��s�҂��B�����ꂩ�̏����ŉ�������܂��B�������w�肵�Ȃ������ꍇ�ł��A
 * �������񏈗������f���āA�V�X�e�����̃C�x���g/�X�V����������ɕ��A���܂��B
 * @param param �҂������w�� ������:�g���K�҂� �I�u�W�F�N�g:�I�u�W�F�N�g�҂� ���l:���ԑ҂�(ms)���ŏ��̎w�肪�L��
 * @return wait�����̌���
 */
function wait(param, ...);

/**
 * �g���K���M
 * �S�X���b�h�ɑ΂��ăg���K�𑗐M���܂��B
 * �Y������g���K��҂��Ă����X���b�h�̑҂�����������܂��B
 * @param name �g���K��
 */
function trigger(name);
