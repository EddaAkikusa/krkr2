//
// squirrel �^���X���b�h�����x�����C�u����
//

/**
 * ���I�u�W�F�N�g
 * �X���b�h�̓I�u�W�F�N�g���u�҂v���Ƃ��ł��܂��B
 * �I�u�W�F�N�g�ɑ΂���҂��́A�I�u�W�F�N�g�� notify/notifyAll ���邱�Ƃŉ�������܂��B
 *
 * ���v���p�e�B�@�\
 * Object ���p�������I�u�W�F�N�g�́A�ʏ�� squirrel �̃I�u�W�F�N�g�ɂ͂Ȃ��v���p�e�B�@�\��
 * �g������Ă��܂��B�Y�����郁���o�����݂��Ȃ������ꍇ�A�����I�� getter/setter
 * �t�@���N�V������T���āA���ꂪ����΂�����Ăяo���Ēl����������܂��B
 * val = obj.name; �� val = obj.getName();
 * obj.name = val; �� obj.setName(val)
 *
 * ��delegate�@�\
 * Object ���p�������I�u�W�F�N�g�́Asquirrel �̃e�[�u��/���[�U�f�[�^���T�|�[�g���Ă�悤��
 * delegate �@�\���g�����Ƃ��ł��܂��B�Ϗ���̃I�u�W�F�N�g�́Asquirrel �̕W���@�\���l��
 * �e�[�u���̑��A�ʂ̃I�u�W�F�N�g�C���X�^���X���w��\�ł��B
 * �Ϗ��I�u�W�F�N�g���C���X�^���X�̏ꍇ�́A�N���[�W�����Q�Ƃ���ہA���̃C���X�^���X����
 * �Ƃ��� bindenv ���ꂽ��ԂŎ擾����܂�(TJS�̃f�t�H���g�̋����Ɠ����悤�ɂȂ�܂�)�B
 * �e�[�u���̏ꍇ�͊����Đݒ肵�Ȃ��̂ŁA�N���[�W���͌��̃I�u�W�F�N�g�̊��Ŏ��s����܂�
 */ 
class Object {

	/**
	 * �R���X�g���N�^
	 * @param delegate �������Ϗ�����I�u�W�F�N�g���w�肵�܂��B
	 */
	constructor(delegate=null);

	/**
	 * ���̃I�u�W�F�N�g�ɑ΂���Ϗ���ݒ肵�܂�(�R���X�g���N�^�w��Ɠ��@�\)
	 * @param delegate �Ϗ���I�u�W�F�N�g
	 */
	function setDelegate(delegate=null);

	/**
	 * ���̃I�u�W�F�N�g�ɑ΂���Ϗ����擾���܂��B
	 * @return �Ϗ���I�u�W�F�N�g
	 */
	function getDelegate();
	
	/**
	 * @param name �v���p�e�B��
	 * @return �w�肳�ꂽ���O�̃v���p�e�B�� setter ������� true
	 */
	function hasSetProp(name);
	
	/**
	 * ���̃I�u�W�F�N�g��҂��Ă���X���b�h1�ɏI����ʒm����
	 */
	function notify();

	/**
	 * ���̃I�u�W�F�N�g��҂��Ă���S�X���b�h�ɏI����ʒm����
	 * �����̃��\�b�h�̓I�u�W�F�N�g�p�����ɂ����s����܂��B
	 */
	function notifyAll();
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
	 * @param delegate �@�\�̈Ϗ���̎w��
	 */
	constructor(func=null, delegate=null);

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
	 */
	function wait(param, ...);

	/**
	 * ���݂̑҂������ׂċ����I�ɃL�����Z�����܂�
	 */
	function cancelWait();
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
 * @return wait�����̌����B�҂����������L�����Z�����ꂽ�ꍇ�� null
 */
function wait(param, ...);

/**
 * �g���K���M
 * �S�X���b�h�ɑ΂��ăg���K�𑗐M���܂��B
 * �Y������g���K��҂��Ă����X���b�h�̑҂�����������܂��B
 * @param name �g���K��
 */
function trigger(name);
