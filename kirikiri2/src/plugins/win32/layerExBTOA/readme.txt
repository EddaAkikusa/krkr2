Title: layerExBTOA
Author: �킽�Ȃׂ���

������͂ȂɁH

������p�Ƀ��C���̉E�������獶�����ɃR�s�[���郁�\�b�h��ǉ����܂��B

���g����

Layer�Ɉȉ��̃��\�b�h���g������܂��B������ĂԂ����ł��B

/**
 * ���C���E������ Blue CHANNEL ���������� Alpha CHANNEL�ɕ�������
 */
function copyRightBlueToLeftAlpha();

/**
 * ���C���������� Blue CHANNEL ���㔼���� Alpha CHANNEL�ɕ�������
 */
function copyBottomBlueToTopAlpha();

/**
 * ���C���� Alpha CHANNEL ��0xff�Ŗ��߂�
 */
function fillAlpha();

/**
 * ���C���� Alpha CHANNEL �� Province Image�i�̈�摜�j�ɃR�s�[����
 * @param threshold 臒l�B�ȗ����܂���void�Ȃ炻�̂܂܃R�s�[����
 *                  ���l���w�肷���Alpha��臒l�ȏ�Ȃ�1�C����ȊO��0�ɖ��߂�
 */
function copyAlphaToProvince(threshold);

/**
 * ���C���� Alpha CHANNEL �� src �� Alpha CHANNEL �ŏ�Z����
 * @param dleft   ���[�ʒu
 * @param dtop    ��[�ʒu
 * @param src     ��Z�p�̃������\�[�X�摜
 * @param sleft   �\�[�X�̍��[�ʒu
 * @param stop    �\�[�X�̏�[�ʒu
 * @param swidth  �\�[�X�̉���
 * @param sheight �\�[�X�̏c��
 * @param clear   ���Z�͈͊O�̈�����̒l�iopacity�j�œh��Ԃ�
 *                �ȗ����܂���0-255�̒l�ȊO�̏ꍇ�͓h��Ԃ��Ȃ�
 */
function clipAlphaRect(dleft, dtop, src, sleft, stop, swidth, sheight, clear);



VideoOverlay �N���X���g���ă��C���ɓ���(�E�����Ƀ��摜)
��`�悵�����ƁAonFrameUpdate() �ŌĂяo���Ă��������B
�����͓����摜�f�[�^(imageWidth�̃T�C�Y)�ɑ΂��čs���܂��B

�`��惌�C���� width �� VideoOverlay �N���X�ɂ����
����̃T�C�Y�Ɋg������Ă�̂ŁA���̃^�C�~���O�Ŕ�����
�Ē������Ă��������B

��
class AlphaVideo extends VideoOverlay
{
  function AlphaVideo(window) {
    super.VideoOverlay(window);
    mode = vomLayer;
  }

  function onFrameUpdate(frame) {
    if (layer1) {
      layer1.width = layer1.imageWidth / 2;
      layer1.copyRightBlueToLeftAlpha();
    }
  }
}

�����C�Z���X

���̃v���O�C���̃��C�Z���X�͋g���g���{�̂ɏ������Ă��������B
