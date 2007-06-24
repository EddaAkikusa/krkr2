/**
 * �C�x���g�摜���������郌�C��
 */
class EnvEventLayer extends EnvBackLayer {

    /**
	 * �R���X�g���N�^
     * @param env ��
	 * @param name ���̃��C����
	 */
	constructor(env) {
		::EnvBackLayer.constructor(env, "event", 6* 100000 - 100);
    }

	/**
	 * �C�x���g�p�Ƀt�@�C�����̂�������
	 */
    function setImageFile(file, elm) {
		local eventInfo;
		if ("events" in env && file in env.events) {
			eventInfo = env.events[file];
			file = eventInfo.image;
			if (!("xpos" in elm) && !("left" in elm) && "left" in eventInfo) {
				props.left <- eventInfo.left;
			}
			if (!("ypos" in elm) && !("top" in elm) && "top" in eventInfo) {
				props.top <- eventInfo.top;
			}
		}
		::EnvBackLayer.setImageFile(file, elm);
		if (eventInfo != null && "trans" in eventInfo) {
			setAutoTrans(eventTrans);
		} else if ("eventTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.eventTrans);
		} else if ("envTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.envTrans);
		}
        return true;
    }

	// ------------------------------------------------------
	
    /**
	 * �R�}���h�̎��s
	 * @param cmd �R�}���h
	 * @param param �p�����[�^
	 * @param elm ���̃R�}���h���܂ޑS�p�����[�^
     * @return ���s���s��ꂽ�ꍇ true
     */
    function doCommand(cmd, param, elm) {

		// ���ʃR�}���h
		if (::EnvBackLayer.doCommand(cmd, param, elm)) {
			return true;
        }
		
		// �摜�̃��[�h
		return setImageFile(cmd, elm);
	}
	// ------------------------------------------------------

	/**
	 * �\�������p�̎����g�����W�V�����̎w��
	 */
	function setDispAutoTrans() {
		if ("eventTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.eventTrans);
		} else if ("envTrans" in env.envinfo) {
			setAutoTrans(env.eventinfo.envTrans);
		}
	}
}
