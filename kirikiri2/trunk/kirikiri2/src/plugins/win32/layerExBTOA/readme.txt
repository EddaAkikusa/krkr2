Title: layerExBTOA
Author: �킽�Ȃׂ���

������͂ȂɁH

������p�Ƀ��C���̉E�������獶�����ɃR�s�[���郁�\�b�h��ǉ����܂��B

���g����

Layer�Ɉȉ��̃��\�b�h���g������܂��B������ĂԂ����ł��B

/**
 * ���C���E������ Blue CHANNEL ���������� Alpha �̈�ɕ�������
 */
function copyRightBlueToLeftAlpha();

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
