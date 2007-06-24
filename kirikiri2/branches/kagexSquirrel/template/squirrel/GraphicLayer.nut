/**
 * �\����ʐ؂�ւ������̑ΏۂɂȂ郌�C��
 * �E�A�N�V�����N������
 * �E�t�F�[�h�������@�\
 */
class GraphicLayer extends BasicLayer {

	// �����Ώۃv���C���[
	player = null;
	
	/**
	 * �R���X�g���N�^
	 * @parma isfore �\��ʂɑ����Ă���
	 */
	constructor(player, isfore) {
		::BasicLayer.constructor();
		this.player = player;
	}

	// �A�N�V��������
	function assign(src) {
		::BasicLayer.assign(src);
		hide = src.hide;
		setVisvalue(src.getVisValue());
		player.copyAction(src, this, onActionCompleted);
	}

    // --------------------------------------------
    // �A�N�V�����n�g��
    // --------------------------------------------

	hide = null; // �A�N�V�����I�����ɏ���

	function onActionCompleted() {
		if (hide) {
			setVisible(false);
		}
		player.onActionCompleted(this);
    }

	function getActionResult() {
		return player.getActionResult(this);
	}
	
	function beginAction(elm, hide, nowait) {
		if (hide != null) {
			this.hide = hide;
		}
		return player.beginAction(this, elm, onActionCompleted, nowait);
    }

    function stopAction() {
        player.stopAction(this);
    }

    function reset() {
		hide = false;
		stopAction();
		::BasicLayer.reset();
    }

	_visvalue = 0;
	function getVisible() {
		return _visvalue > 0;
	}
	function setVisible(v) {
		_visvalue = v ? 100 : 0;
		setOpacity(__opacity);
	}

	function getVisvalue() {
		return _visvalue;
	}
	function setVisvalue(v) {
		_visvalue = v;
		setOpacity(__opacity);
	}
	
	__opacity = 255;
	function getOpacity() {
		return __opacity;
	}
	function setOpacity(v) {
		__opacity = v;
		local o = _opacity * _visvalue / 100;
		::BasicLayer.setOpacity(o);
		::BasicLayer.setVisible(o > 0);
	}
};
