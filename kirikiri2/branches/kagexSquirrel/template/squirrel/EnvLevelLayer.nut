/**
 * �O�i���C���̃��x������p
 */
class EnvLevelLayer extends EnvImage {

    /// �\�����x��
	level = null;
	/// �\����΃��x��
	absolute = null;
	
	// �`��Ώۃ��C��
	targetLayer = null;
	
	/// �ʒu�ύX
    front = null;
	back = null;

    /**
     * �R���X�g���N�^
     */
	function constructor(env, name) {
		::EnvImage.constructor(env, name);
		level = null;
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
					} else {
						targetLayer = player.trashLayer(targetLayer);
					}
				}
			} else {
				if (targetLayer != null) {
					if (!player.isForeLayer(targetLayer) && targetLayer.visible) {
						player.toForeLayer(targetLayer);
					} else {
						targetLayer = player.trashLayer(targetLayer);
					}
				}
			}
		}
	}
	
    /**
     * ���C�����̎擾
     * @param base fore / back      
     * �����Ƀ��x���ɉ����� front/back �̏��������s����
     */
	function getLayer(isfore) {
		if (targetLayer == null || player.isForeLayer(targetLayer) != isfore) {
			targetLayer = player.getNewLayer(this, isfore, targetLayer);
		}
		if (level == null) {
			level = env.defaultLevel;
		}
		player.toLevel(targetLayer, level);
		if (front) {
			player.toFront(targetLayer);
		} else if (back) {
			player.toBack(targetLayer);
		}
		front = back = null;
		absolute = targetLayer.absolute;
		return targetLayer;
    }

    /**
     * ���x�����őO��
     */
    function setFront() {
        front = true;
		updateFlag = true;
    }

    /**
     * ���x�����Ō��
     */
    function setBack() {
        back = true;
		updateFlag = true;
    }

    /**
     * ���x���̎w��
	 */
	function setLevel(cmd, elm) {
		level = cmd;
		absolute = null;
	} 
}
