Author: �n粍�(go@wamsoft.jp)
Date: 2009/4/22

���T�v

squirrel �ŋ^���X���b�h�������������郉�C�u�����ł��B

���g����

�ڍׂ� manual.nut ���Q�Ƃ��Ă�������

���g�ݍ���

������g�ݍ��ނ��߂ɂ́A�����̃V�X�e���ɂ��킹��
���b�p�[�������ꕔ�Ǝ���������K�v������܂��B

���񓯊��t�@�C�����[�h�̎���

�t�@�C����񓯊��ɓǂݍ��ޏ������s�����߂Ɉȉ��̃��\�b�h���������Ă��������B

�� squirrel �W���@�\�� sqstd_loadfile ���͑S���g���Ă܂���B
�@ �T���v���� sqfunc �� sqplusfunc ���̏����������ł� 
   sqstdmath �� sqstdstring �����o�^���Ă��܂��B

   ���[������������̂ŁAdofile() �̗��p�ɂ͂����ӂ��������B

-----------------------------------------------------------------------------
/**
 * �t�@�C����񓯊��ɊJ��
 * @param filename �X�N���v�g�t�@�C����
 * @return �t�@�C���n���h��
 */
extern void *sqobjOpenFile(const SQChar *filename);

/**
 * �t�@�C�����J���ꂽ���ǂ����̃`�F�b�N
 * @param handler �t�@�C���n���h��
 * @param dataPtr �f�[�^�i�[��A�h���X(�o��)
 * @param dataSize �f�[�^�T�C�Y(�o��)
 * @return ���[�h�������Ă����� true
 */
extern bool sqobjCheckFile(void *handler, const char **dataAddr, int *dataSize);

/**
 * �t�@�C�������
 * @param handler �t�@�C���n���h��
 */
extern void sqobjCloseFile(void *handler);
----------------------------------------------------------------------------

���O���[�o��VM�֌W���\�b�h�̎���

SQObject �� SQThread �́A����̃O���[�o�� Squirrel VM �Ɉˑ����܂��B
������擾���邽�߂̈ȉ��̃��\�b�h���������܂�

----------------------------------------------------------
namespace sqobject{
  extern HSQUIRRELVM�@init();        /// < VM������
  extern void done();                /// < VM�j��
  extern HSQUIRRELVM�@getGlobalVM(); /// < �O���[�o��VM�擾
}
----------------------------------------------------------

���I�u�W�F�N�g�Q�Ə����̎���

ObjectInfo �̃����o obj ���� Object �� Thread ��
�l�C�e�B�u C++ �C���X�^���X���擾����ȉ��̃��\�b�h���������܂��B

-----------------------------------------------
namespace sqobject{
  Thread *ObjectInfo::getThread();
  Object *ObjectInfo::getObject();
}
-----------------------------------------------

���I�u�W�F�N�g�o�^�����̎���

�I�u�W�F�N�g���N���X�Ƃ��ēo�^���邽�߂̈ȉ��̃��\�b�h���������܂��B
�v���O�����͂���ƁAThread::registGlobal() ���Ăяo�����Ƃ�
���\�b�h��o�^�ł��܂�

-----------------------------------------------
namespace sqobject{
  void Object::registerClass();
  void Thread::registerClass();
}
-----------------------------------------------

���Ǝ��I�u�W�F�N�g�̎����Ɠo�^

sqobject::Object ��P��p������`�ŃI�u�W�F�N�g���쐬���Ă��������B
�������邱�ƂŁAObject �N���X�Ɋg������Ă���A�v���p�e�B��f���Q�[�g��
�@�\���g�����Ƃ��ł���ق��A�^���X���b�h�� wait �ΏۂƂ��ăI�u�W�F�N�g
���������Ƃ��ł��܂��B

���I�u�W�F�N�g�̌p�������͓Ǝ��Ɏ�������K�v������܂�

�����s�����̎���

��������

1. sqobject::init() ���Ăяo��
2. print�֐��o�^���K�v�ȏ���
3. �N���X�o�^

  Object::registerClass();
  Thread::registerClass();
  Thread::registerGlobal();

  ���K�v�ɉ����ăN���X��o�^

�����s��������

�^���X���b�h���ғ�������ɂ̓��C�����[�v���ňȉ��̏������Ăяo���Ă��������B

-----------------------------------------------
/**
 * @param diff �o�ߎ���
 * @return ���쒆�̃X���b�h�̐�
 */	
int Thread::main(int diff);
-----------------------------------------------

��{�\���͎��̂悤�ɂ���̂��Ó��ł��B

while(true) {
  �C�x���g����
�@Thread::main(���ԍ���)
�@��ʍX�V����
};

���Ԃ̊T�O�̓V�X�e�����ŔC�ӂɑI���ł��܂��B
��ʓI�ɂ̓t���[�������Ams �w����g���܂��B����Ŏw�肵���l��
wait() ���߂ɓn�����l�p�����[�^�̈Ӗ��ɂȂ�܂��B

���I�������̎���

1. Thread::done() �ŃX���b�h�̏��������j��
2. roottable �̏���S�N���A
3. sqobject::done() �Ăяo���� VM �����

�������T���v���R�[�h

 sqfunc.cpp     �V���v���Ȍp��/�����o�֐������̎�����
 sqplusfunc.cpp	SQPlus���g���ꍇ�̎�����

��continuous handler �@�\

�X���b�h�@�\�Ƃ͕ʂɁA�P���ɃG���W������������ squirrel 
�X�N���v�g�����I�ɌĂяo���@�\�ł��B

�X���b�h����      : ���[�U�ɂ�鐧�䏈��
continuous handler: ���䂪�I��������Ƃ̎����v�Z����

�Ƃ������g��������z�肵�Ă��܂��Bcontinuous handler��
�Ăяo���́A���ׂẴX���b�h��������U suspend ������ɂȂ�܂��B

�����̌Ăяo���́A��ɒʏ�� sq_call �ɂ����̂Ȃ̂ŁA
  �X�N���v�g�� suspend() ���ĕ��A���邱�Ƃ͂ł��܂���B

�Esqobject::registerContinuous() ���Ăяo�����Ƃŋ@�\�o�^����܂�

�Esqobject::Thread::main(diff) ���Ăяo���O��
  sqobject::beforeContinuous(diff) ���A�Ăяo�������
  sqobject::afterContinuous() ���Ăяo���Ă�������

�E�S�����I�����ɂ́@sqoject::doneContinuous() ���Ăяo���܂�


���X�N���v�g������̓o�^

addContinuous(func), removeContinuous(func) �Ŋ֐���o�^/�����ł��܂��B


���X�N���v�g�̌Ăяo��

C++������X�N���v�g���N������ꍇ�́AThread::fork() ���g�����A
squirrel �� API ���g���āA�O���[�o���֐� fork() ���Ăяo���悤�ɂ��Ă��������B

sqplus(��)�ł̗�
---------------------------------------------------------------
SqPlus::SquirrelFunction<int>("fork")(NULL, _SC("file.nut"));
---------------------------------------------------------------

���X���b�h���s���̒���

�ʂ̃X�N���v�g�́A�҂���ԂɂȂ�܂Ŏ��s�𒆒f���Ȃ����߁A
�e�Ղ� busy loop �ɂȂ�܂��B����I�� wait() �܂��� suspend() 
������ăV�X�e���ɏ�����߂��悤�ɂ���K�v������܂��B
�����Ƃ��ẮA��ʍX�V���K�v�ȃ^�C�~���O�� suspend() 
���s���Ηǂ����ƂɂȂ�܂��B

�����C�Z���X

squirrel ���l zlib���C�Z���X�ɏ]���ė��p���Ă��������B

/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */
