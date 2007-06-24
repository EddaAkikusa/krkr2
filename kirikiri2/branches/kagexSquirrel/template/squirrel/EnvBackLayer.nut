/**
 * �w�i�p���C��
 * EnvLevelLayer �̒�`���㏑�����ď����Ă��܂�
 */
class EnvBackLayer extends EnvLayer {

	// �\�����x��
	absolute = null;
	
	// �`��Ώۃ��C��
	targetLayer = null;
	
    /**
     * �R���X�g���N�^
     */
	constructor(env, name, absolute) {
		::EnvLayer.constructor(env, name);
		this.absolute = absolute;
    }

	/**
	 * ���C���̃R�s�[����
	 */
	function onCopyLayer(toback, backlay) {
		if (backlay) {
			if (toback) {
				if (targetLayer != null) {
					if (player.isForeLayer(targetLayer) && targetLayer.visible) {
						targetLayer = player.getNewLayer(this, false, targetLayer);
						targetLayer.absolute = absolute;
					} else {
						targetLayer = player.trashLayer(targetLayer);
					}
				}
			} else {
				if (targetLayer != null) {
					if (!player.isForeLayer(targetLayer) && targetLayer.visible) {
						player.toForeLayer(targetLayer);
						targetLayer.absolute = absolute;
					} else {
						targetLayer = player.trashLayer(targetLayer);
					}
				}
			}
		}
	}
	
    /**
     * ���C�����̎擾
     * @param isfore fore / back      
     * �����Ƀ��x���ɉ����� front/back �̏��������s����
     */
	function getLayer(isfore) {
		if (targetLayer == null || player.isForeLayer(targetLayer) != isfore) {
			targetLayer = player.getNewLayer(this, isfore, targetLayer);
		}
		targetLayer.absolute = absolute;
		return targetLayer;
    }

}
