/**
 * ����p���C��
 */
class StageGraphicLayer extends EnvGraphicLayer {

	/**
	 * �R���X�g���N�^
	 * @param owner �I�u�W�F�N�g���������Ă�e
	 * @param isfore �\��ʂ�����ʂ�
	 */
	function constructor(owner, isfore) {
		::EnvGraphicLayer.constructor(owner, isfore);
	}

	function recalcPosition() {
		setRealPosition(owner.env.xmax + _left - getWidth() / 2,
						owner.env.ymax + _top  - getHeight() / 2);
	}
};


/**
 * �w�i���������郌�C��
 * �O���[�o���Ɉȉ��̃��\�b�h���������ꍇ�̓t�b�N�Ƃ��ČĂяo���܂�
 */
class EnvStageLayer extends EnvBackLayer {

	/// ����
	timeName = null;
    /// ����
	stageName = null;

	function initImage() {
		timeName = null;
		stageName = null;
	}

	stageLayerCommands = null;
	
    /**
	 * �R���X�g���N�^
     * @param env ��
	 * @param name ���̃��C����
	 */
	function constructor(env) {
		::EnvBackLayer.constructor(env, "stage", 100);
		stageLayerCommands = {
			stage = setStage,
			stime = setTime,
		};
    }

	/**
	 * �w�i�摜�̍X�V����
	 */
	function updateStageImage(elm) {

		if (stageName == null) {
			return;
		}

		local info;
		if (stageName in env.stages) {
			info = env.stages[stageName];
		} else {
			throw "�X�e�[�W��񂪂���܂���:" + stageName;
		}
		
		local image = info.image;
		if (timeName == null) {
			timeName = env.defaultTime;
		}
		if (timeName in env.times) {
			image = replace(image, "TIME", env.times[timeName].prefix);
		}

		if (!isExistImageName(image)) {
			// �摜���Ȃ��ꍇ�͕W���摜�̕␳�őΉ�
			image = info.image;
			image = replace(image, "TIME", env.times[env.defaultTime].prefix);
			// �F�␳����
			local timeInfo;
			if ((timeInfo = env.getCurrentTime()) != null) {
				if ("lightColor" in timeInfo) {
					elm.lightcolor = timeInfo.lightColor;
				}
				if ("lightType" in timeInfo) {
					elm.lighttype  = timeInfo.lightType;
				}
				if ("brigtness" in timeInfo) {
					elm.brightness = timeInfo.brightness;
				}
				if ("contrast" in timeInfo) {
					elm.contrast   = timeInfo.contrast;
				}
			}
		}
		
		if (!("xpos" in elm) && !("left" in elm) && "left" in info) {
			props.left <- info.left;
		}
		if (!("ypos" in elm) && !("top" in elm) && "top" in info) {
			props.top <- info.top;
		}
		setImageFile(image, elm);
		if ("trans" in info) {
			setAutoTrans(info.trans);
		} else if ("stageTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.stageTrans);
		} else if ("envTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.envTrans);
		}
	}
	
	/**
     * �����ݒ肷��
     * @param stageName ���䖼
     * @param elm �R�}���h�̂ق��̗v�f
     */
    function setStage(stageName, elm) {

		if (this.stageName != stageName || isClear() || env.event.isShow()) {
			this.stageName = stageName;
			updateStageImage(elm);
        }
		// �C�x���g�G�͏���
		if (env.event.isShow()) {
			env.event.disp = CLEAR;
		}
	}

    /**
     * ���Ԃ�ݒ肷��
     * @param timeName ���Ԗ�
     * @param elm �R�}���h�̂ق��̗v�f
     */
    function setTime(timeName, elm) {

		if (this.timeName != timeName || isClear() || env.event.isShow()) {
			
			this.timeName = timeName;
			updateStageImage(elm);
			
			// ���ԕύX�̓L�����̗����G���ĕ`��̕K�v������
			foreach (name, value in env.characters) {
				value.setRedraw();
            };
        }
        // �C�x���g�G�͏���
		if (env.event.isShow()) {
			env.event.disp = CLEAR;
        }
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

		// ���C�����ʃR�}���h
		if (cmd in stageLayerCommands) {
			local func = stageLayerCommands[cmd];
			if (func != null) {
				func(param, elm);
			}
			return true;
		}

		// ���Ԃƕ���
		if (cmd in env.times) {
			setTime(cmd, elm);
			return true;
		} else if (cmd in env.stages) {
			setStage(cmd, elm);
			return true;
		}

		return false;
	}
	
	// ------------------------------------------------------

	/**
	 * �X�V����
	 * �L�����N�^���X�V����K�v������
	 */
	function update(isfore) {
		foreach (name, value in env.characters) {
			value.update(isfore);
		};
		::EnvBackLayer.update(isfore);
	}
	
	// ------------------------------------------------------

	/**
	 * �\�������p�̎����g�����W�V�����̎w��
	 */
	function setDispAutoTrans() {
		if ("stageTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.stageTrans);
		} else if ("envTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.envTrans);
		}
	}

	/**
	 * �V���C������
	 */
	function createLayer(isfore) {
		return StageGraphicLayer(this, isfore);
	}
}
