Title: Squirrel Plugin
Author: �킽�Ȃׂ���

������͂ȂɁH

Squirrel (http://squirrel-lang.org/) �̋g���g���o�C���h�ł��B

Squirrel �g�ݍ��ݗp�I�u�W�F�N�g�w������ł��B
���@�I�ɂ� C ���ꕗ�ŁATJS2 �ƍ\�����T�O���悭���Ă��܂��B

Squirrel �́A�����X���b�h�i�R���[�`���j���T�|�[�g���Ă���A
������C�ӂ̃^�C�~���O�Œ��f�ł��邽�߁A�Q�[���p�̃��W�b�N
��g�ނ̂ɔ��ɓK���Ă��܂��B

���g�p���@

����{

�ESquirrel �̃O���[�o����Ԃ͋g���g���S�̂ɑ΂��ĂP�������݂��܂��B
�@
�@Squirrel �p�̃X�N���v�g�̎��s�͂��̃O���[�o����ԏ�ł����Ȃ��A
�@��`���ꂽ�t�@���N�V������N���X�����̃O���[�o����Ԃɓo�^����Ă����܂��B

�ETJS2 �̃O���[�o����Ԃ� Squirrel ������ "::krkr" �ŎQ�Ƃł��܂��B

  �v���~�e�B�u�l�̎Q�ƂƁA�I�u�W�F�N�g�ɑ΂���
�@get/set/call ����������Ă��܂��B
  ��call �̓N���X����̃C���X�^���X�̐������\�ł��B

�ESquirrel �̃O���[�o����Ԃ� TJS2 ������ "sqglobal" �ŎQ�Ƃł��܂��B

�@�v���~�e�B�u�l�̎Q�ƂƁA
  �I�u�W�F�N�g�ɑ΂��� PropGet/PropSet/FuncCall/CreateNew
  ����������Ă��܂��B�Ȃ� incontextof �͈Ӗ��������܂���B

�ESquirrel �W�����C�u�����̂����ȉ��̂��̂����p�\�ł�

  - I/O
  - blob
  - math
  - string

�@I/O �֌W�� OS���ڂł͂Ȃ��ATJS �̃X�g���[�W��Ԃ��Q�Ƃ���܂��B
�@�܂��A�t�@�C������ TJS �̃X�g���[�W���ɂȂ�܂��B
�@stdin/stdout/stderr �͗��p�ł��܂���

��Scripts �g��

Squirrel �X�N���v�g�̎��s�@�\�� Scripts �Ɋg������܂��B
������̃��\�b�h���Ԃ�l��Ԃ����Ƃ��ł��܂��B�v���~�e�B�u�Ȓl��
���̂܂ܕϊ�����āA�܂��A�N���X�� TJS2 �� iTJSDispatch ��
���b�s���O���ꂽ��ԂŕԂ�܂��B

------------------------------------------------------------------------------
Scripts.execSQ("local a=10; return a;");	// Squirrel �X�N���v�g�̎��s
Scripts.execStorageSQ("filename.nut");		// Squirrel �X�N���v�g�t�@�C���̎��s
------------------------------------------------------------------------------

��SQContinous �g��

Squirrel �̃t�@���N�V�����𒼐ڌĂяo�� Continuous Handler ��ێ�����N���X�ł��B
TJS2 ���b�s���O�ɂ��]���ȕ��ׂȂ��ɌĂяo���������s�����Ƃ��ł��܂��B

�^���R�[�h�ɂ��}�j���A��

------------------------------------------------------------------------------
class SQContinuous {

	/**
	 * �R���X�g���N�^
     * @param func squirrel �̃O���[�o���Ȋ֐� func(tick){} 
     * tick �� SQInteger �ɃL���X�g����Ă��܂�
	 */
	function SQContinuous(func);

	/**
	 * �Ăяo�����J�n����
     */
 	function start();
    
    /**
	 * �Ăяo�����~����
	 */
    function stop();
};
------------------------------------------------------------------------------

�����C�Z���X

Squirrel �� ������ zlib/libpng�X�^�C�����C�Z���X�ł��B

Copyright (c) 2003-2007 Alberto Demichelis

This software is provided 'as-is', without any 
express or implied warranty. In no event will the 
authors be held liable for any damages arising from 
the use of this software.

Permission is granted to anyone to use this software 
for any purpose, including commercial applications, 
and to alter it and redistribute it freely, subject 
to the following restrictions:

		1. The origin of this software must not be 
		misrepresented; you must not claim that 
		you wrote the original software. If you 
		use this software in a product, an 
		acknowledgment in the product 
		documentation would be appreciated but is 
		not required.

		2. Altered source versions must be plainly 
		marked as such, and must not be 
		misrepresented as being the original 
		software.

		3. This notice may not be removed or 
		altered from any source distribution.
-----------------------------------------------------
END OF COPYRIGHT

���̃v���O�C�����̂̃��C�Z���X�͋g���g���{�̂ɏ������Ă��������B
