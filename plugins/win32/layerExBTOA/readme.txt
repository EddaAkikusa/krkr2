Title: layerExBTOA
Author: �킽�Ȃׂ���

������͂ȂɁH

���C���̃��̈�� Province�摜�������郁�\�b�h���W�߂����̂ł�

���g����

�e���\�b�h�ɂ��Ă� manual.tjs �Q��

������p�Ɏg���ꍇ�́AVideoOverlay �N���X���g���ă��C���ɓ���(�E�����Ƀ��摜)
��`�悵�����ƁAonFrameUpdate() �� copyRightBlueToLeftAlpha() ��
�Ăяo���Ă��������B�����͓����摜�f�[�^(imageWidth�̃T�C�Y)�ɑ΂��čs���܂��B

�`��惌�C���� width �� VideoOverlay �N���X�ɂ���ē���̃T�C�Y��
�g������Ă�̂ŁA���̃^�C�~���O�Ŕ����ɍĒ������Ă��������B

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
