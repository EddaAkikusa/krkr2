/**
 * �L�����N�^�p���C��
 * LEVEL �ɉ��������W�␳�@�\������
 */
class CharacterGraphicLayer extends EnvGraphicLayer {

	yoffset = null; // yoffset���
	
	/**
	 * �R���X�g���N�^
	 * @param owner �I�u�W�F�N�g���������Ă�e
	 * @param isfore �\��ʂ�����ʂ�
	 */
	constructor(owner,isfore) {
		yoffset = owner.yoffset;
		::EnvGraphicLayer.constructor(owner, isfore);
	}

	function assign(src) {
		//dm("�A�T�C������:" + yoffset);
		::EnvGraphicLayer.assign(src);
		this.yoffset = src.yoffset;
	}

	_level = null;
	function getLevel() {
		return _level;
	}

	function setLevel(v) {
		_level = v;
		recalcPosition();
	}
	
	function recalcPosition() {
		//dm("�L�����\��:" + name + ":" + yoffset + ":"  +level + ":" + left + ":" + top);
		// ���x���ʍ��W�␳�����B�Ƃ肠�����K���ŉ�ʒ����Ɍ������ďk���������Ă���
		local zoom = 100;
		local levelYoffset = 0;
		if (level in owner.env.levels) {
			local levelInfo = owner.env.levels[level];
			if ("zoom"  in levelInfo) {
				zoom = levelInfo.zoom;
			}
			if ("yoffset" in levelInfo) {
				levelYoffset = levelInfo.yoffset;
			}
		}
		setRealPosition(owner.env.xmax + (left * zoom / 100) - width / 2, 
						owner.env.ymax + ((yoffset - top) * zoom / 100) - height + levelYoffset);
	}
};

/**
 * �L�����N�^���C��
 * �|�[�Y�̎w�葼
 */
class EnvCharacter extends EnvLevelLayer {

	/// ��������
	initName = null;

    /// ���������
	poses = null;

    // �摜�t�@�C�����ڎw��
	image = null;
    // �ʃI�t�Z�b�g�␳
	_yoffset = null;
	function getYoffset() {
		if (_yoffset != null) {
			return _yoffset;
		}
		if ("yoffset" in init) {
			return init.yoffset;
		}
		return env.yoffset;
	}
	function setYoffset(v) {
		_yoffset = v;
	}
    
    /// �|�[�Y
	_pose = null;
	function getPose() {
		if (_pose != null) {
			return _pose;
		}
		if ("defaultPose" in init) {
			return init.defaultPose;
		}
		return "�ʏ�";
	}
	function setPose(v) {
		_pose = v;
    };

    /// ����
	dress = null;
	/// �\��
	face = null;

	// �\��ƃ|�[�Y�̑Ή��\
	facePoseMap = {};

	charCommands = null;
	
    /**
     * �R���X�g���N�^
     * @param env ���I�u�W�F�N�g
     * @param name �L�����N�^��
     * @param init �L�����N�^�������f�[�^
     */
    function constructor(env, name, initName, init) {
		::EnvLevelLayer.constructor(emv, name);
        
        //dm("�L�����N�^�o�^:" + name); 

        this.initName = initName;
		this.init     = init;
        if (init != null) {
			if ("poses" in init) {
				poses = init.poses;
			}
			// �\��|�[�Y�����@�\
			if ("facePose" in init) {
				// �\���|�[�Y�ɑ΂���}�b�v���쐬����
				foreach(name, value in poses) {
					foreach (face, fvalue in values.faces) {
						facePoseMap[face] = name;
					}
				}
			}
		} else {
			init = {};
		}

		charCommands = {
			image   = setImage,
			dress   = setDress,
			face    = setFace,
			front   = setFront,
			back    = setBack,
			level   = setLevel,
			pos     = setPosition,
			facewin = function(param, elm) { setDisp(FACE); redraw = true; },
			faceon = null,
			faceoff = null,
		};
	}
    
	// �\��`�揈�����s��
	redrawFace = null;

    /**
     * ���ډ摜�w��
     */
    function setImage(imageName, elm) {
		if (imageName != image || isClear()) {
			image   = imageName;
			if ("yoffset" in elm) {
				setYoffset(elm.yoffset);
			}
			setDisp(SHOW);
			redraw  = true;
			if ("poseTrans" in init) {
				setAutoTrans(init.poseTrans);
			} else if ("charTrans" in init) {
				setAutoTrans(init.charTrans);
			} else if ("poseTrans" in env.envinfo) {
				setAutoTrans(env.envinfo.poseTrans);
			} else if ("charTrans" in env.envinfo) {
				setAutoTrnas(env.envinfo.charTrans);
			}
		}
    }
    
    /**
     * �|�[�Y�̐ݒ�
     */
    function setPose(poseName, elm) {

		if (poseName in poses) {
			local info = poses[poseName];
			if (poseName != pose || isClear()) {
				pose = poseName;
				setYoffset(info.yoffset);
				setDisp(SHOW);
				redraw = true;

				if ("trans" in info) {
					setAutoTrans(info.trans);
				} else if ("poseTrans" in init) {
					setAutoTrans(init.poseTrans);
				} else if ("charTrans" in init) {
					setAutoTrans(init.charTrans);
				} else if ("poseTrans" in env.envinfo) {
					setAutoTrans(env.envinfo.poseTrans);
				} else if ("charTrans" in env.envinfo) {
					setAutoTrnas(env.envinfo.charTrans);
				}
				
                // �\��\���擾
				if (isShowFace() &&
					(getval(elm,"faceon") || getval(env, "showFaceMode")) &&
					!getval(elm,"faceoff")) {
					redrawFace = true;
				}
            }
			// ��������������
			if (dress != null && (!("dresses" in info) || !(dress in info.dresses))) {
				dress = null;
			}
			// �\���������
			if (face != null && (!("faces" in info) || !(face in info.faces))) {
				face = null;
			}
        }

        // �C���[�W��j��
		image = null;
		// Y���W�̓|�[�Y�ݒ肵���Ƃ��ɏ����l�ɖ߂�
		if ("defaultYpos" in env) {
			setTop(env.defaultYpos);
		}
    }

    /**
     * �����̐ݒ�
     */
    function setDress(dressName, elm) {
		if (dressName != dress || isClear()) {
			dress = dressName;
			setDisp(SHOW);
			redraw = true;

			if ("dressTrans" in init) {
				setAutoTrans(init.dressTrans);
			} else if ("charTrans" in init) {
				setAutoTrans(init.charTrans);
			} else if ("dressTrans" in env.envinfo) {
				setAutoTrans(env.envinfo.dressTrans);
			} else if ("charTrans" in env.envinfo) {
				setAutoTrnas(env.envinfo.charTrans);
			}

			// �\��\���擾
			if (isShowFace() && (getval(elm,"faceon") || getval(env,"showFaceMode")) && !getval(elm,"faceoff")) {
				redrawFace = true;
			}
		}
		image = null;
    }
    /**
     * �\��̐ݒ�
      */
    function _setFace(faceName, elm) {
		if (faceName != face || isClear()) {
			face = faceName;
			setDisp(SHOW);
			redraw = true;

			if ("faceTrans" in init) {
				setAutoTrans(init.faceTrans);
			} else if ("charTrans" in init) {
				setAutoTrans(init.charTrans);
			} else if ("faceTrans" in env.envinfo) {
				setAutoTrans(env.envinfo.faceTrans);
			} else if ("charTrans" in env.envinfo) {
				setAutoTrnas(env.envinfo.charTrans);
			}

			// �\��\���擾
			if (isShowFace() && (getval(elm,"faceon") || getval(env,"showFaceMode")) && !getval(elm,"faceoff")) {
				redrawFace = true;
			}
		}
		image = null;
    }

	function setFace(cmd,elm) {
		if (facePoseMap != null) {
			if (cmd in facePoseMap) {
				local p = facePoseMap.cmd;
				setPose(p,elm);
				_setFace(cmd,elm);
			}
		} else {
			_setFace(cmd,elm);
		}
	}
	
    /**
     * �ꏊ�\���p�g�����W�V�����ݒ�
     */
    function setPositionTrans(info) {
		if ("trans" in info) {
			setAutoTrans(info.trans);
		} else if ("positionTrans" in env.envinfo) {
			setAutoTrans(env.envinf.positionTrans);
		}
    }

	// �\���ʒu�w��
	posName = null;
	
    /**
     * �\���ʒu�̐ݒ�
     */
    function setPosition(cmd, elm) {

		local istrans = false;
		
		posName = getTo(cmd);
		if (!("positions" in env) || !(posName in env.positions)) {
			return;
		}
		local info = env.positions[posName];
		local istrans = false;
		
		if ("xpos" in info || "ypos" in info) {
			if (getDisp() == CLEAR) {
				setDisp(SHOW);
				istrans = true;
			}
			local posFrom = getFrom(cmd);
			if (posFrom != null && (posFrom in env.positions[posFrom])) {
				local fromInfo = env.positions[posFrom];
				if ("xpos" in info) {
					setPropFromTo("left", env.xmax, "xpos" in fromInfo ? fromInfo.xpos : null, info.xpos, elm);
				}
				if ("ypos" in info) {
					setPropFromTo("top", env.ymax, "ypos" in fromInfo ? fromInfo.ypos : null, info.ypos, elm);
				}
			} else {
				if ("xpos" in info) {
					setPropFromTo("left", env.xmax, null, info.xpos, elm);
				}
				if ("ypos" in info) {
					setPropFromTo("top", env.ymax, null, info.ypos, elm);
				}
			}
			// �����������f�t�H���g
			if (!("left" in props)) {
				dm("�f�t�H���g��X�ʒu���w��");
				if ("defaultXpos" in env) {
					setLeft(env.defaultXpos);
				}
			}
			if (!("top" in props)) {
				if ("defaultYpos" in env) {
					setTop(env.defaultYpos);
				}
			}
		}

		if ("disp" in info) {
			setDisp(info.disp);
			istrans = true;
		}

		if ("level" in info) {
			setLevel(info.level);
			redraw = true;
			istrans = true;
		}

		if (istrans) {
			setPositionTrans(info);
		}
	}

    /**
     * �|�[�Y�w���D�揈������
     */
    function doPoseCommand(cmd, param, elm) {

        // �|�[�Y�R�}���h
        if (cmd == "pose") {
            setPose(param, elm);
            return true;
        }

        // ��|�[�Y�}�b�v�����݂���ꍇ
		if (facePoseMap != null) {
			if (cmd in facePoseMap) {
				local p = facePoseMap[cmd];
                setPose(p,elm);
                _setFace(cmd,elm);
                return true;
            }
        }
        
        // �|�[�Y�w��
        if (poses != null) {
			if (cmd in poses[cmd]) {
				setPose(cmd,elm);
				return true;
			}
        }
        
        return false;
    }
    
    /**
     * �R�}���h�̎��s
     * @param cmd �R�}���h
     * @param param �p�����[�^
     * @param elm ���̃R�}���h���܂ޑS�p�����[�^
     * @return ���s���s��ꂽ�ꍇ true
     */
    function doCommand(cmd, param, elm) {

		if (::EnvImage.doCommand(cmd, param, elm)) {
			return true;
        }

		if (cmd in charCommands) {
			local func = charCommands[cmd];
			if (func != null) {
				func(param, elm);
			}
            return true;
        }
        
        local info;
        local find = false;

		if (poses != null && pose != null && image == null) {
			
			if (pose in poses) {
				local poseInfo = poses[pose];
				if ("dresses" in poseInfo && cmd in poseInfo.dresses) {
					//dm("������ݒ�");
					find = true;
					setDress(cmd,elm);
				} else if ("faces" in poseInfo && cmd in poseInfo.faces) {
					//dm("�\���ݒ�");
					find = true;
					setFace(cmd,elm);
                }
            } else {
				dm("�|�[�Y��񂪂���܂���:" + pose + ":" + cmd);
            }
        }

		local posName = getTo(cmd);
		if (!find && "positions" in env && posName in env.positions) {
			setPosition(cmd, elm);
			find = true;
        }            

        return find;
    };

	/**
	 * �����G�摜�̎擾
	 * @param levelName ���݂̕\�����x��
	 * @param pose      �|�[�Y�w��
	 */
	function _getPoseImage(levelName, pose) {
		local poseInfo;
		if (poses != null && pos in poses) {
			local poseInfo = poses[pose];
			local imageFile;
			
			// ��`��̏ꍇ�� msgImage / msgFaceImage ��D��
			local imageName;
			if (env.faceLevelName == levelName && "msgImage" in poseInfo) {
				imageName = poseInfo.msgImage;
			} else if ("image" in poseInfo) {
				imageName = poseInfo.image;
			}
			if (imageName == null) {
				return;
			}

			imageName = replace(imageName, "LEVEL", levelName);
			
			// �����w�肪�����ꍇ�̓f�t�H���g���Q��
			local dressName;
			if (dress != null && "dresses" in poseInfo && dress in poseInfo.dresses) {
				dressName = poseInfo.dresses[dress];
            } 
			if (dressName == null && "defaultDress" in poseInfo) {
				dressName = poseInfo.defaultDress;
			}

            // �\��w�肪�����ꍇ�̓f�t�H���g���Q��
            local faceName;
			if (face != null && "faces" in poseInfo && face in poseInfo.faces) {
				faceName = poseInfo.faces[face];
            }
			if (faceName == null && "defaultFace" in poseInfo) {
				faceName = poseInfo.defaultFace;
			}

            //dm("dress:", dressName, "face:", faceName);
            
			// �獇���^�����G
			// �������ŏ㏑��
			if (dressName != null) {
				imageFile = replace(imageName, "DRESS", dressName);
			} else {
				imageFile = imageName;
			}
			// �\��ŏ㏑��
			if (faceName != null) {
				imageFile = replace(imageFile, "FACE", faceName);
			}
			return imageFile;
		}
	}
	
	/**
	 * �����G�摜�̎擾
	 * @param levelName ���݂̕\�����x��
	 */
	function getPoseImage(levelName) {
		local imageName = _getPoseImage(levelName, pose);
		if ((imageName == null || !isExistImageName(imageName)) && "defaultPose" in init && pose != init.defaultPose) {
			imageName = _getPoseImage(levelName, init.defaultPose);
		}
		return imageName;
	}

    /**
     * ���݂̗����G�̕`��
     * @param layer �`��Ώۃ��C��
     */
	function drawLayer(layer) {
		if (isShowBU()) {

			// ���x���ŕ␳
			local levelName = level;
			if (level in env.levels) {
				if ("name" in env.levels[level]) {
					levelName = env.levels[level].name;
				}
			}

			// �摜������
			local imageName;
			if (image != null) {
				imageName = replace(image, "LEVEL", levelName);
			} else {
				imageName = getPoseImage(levelName);
			}

			// �摜�̃��[�h
			try {
				local elm = { storage=imageName };
				// �F�␳
				local timeInfo;
				if ((timeInfo = env.getCurrentTime()) != null) {
					if ("charLightColor" in timeInfo) {
						elm.lightcolor = timeInfo.charLightColor;
					}
					if ("charLightType" in timeInfo) {
						elm.lighttype  = timeInfo.charLightType;
					}
					if ("charBrigtness" in timeInfo) {
						elm.brightness = timeInfo.charBrightness;
					}
					if ("charContrast" in timeInfo) {
						elm.contrast   = timeInfo.charContrast;
					}
				}
				layer.loadImages(elm);
			} catch (e) {
				player.errorImage(imageName + ":�����G�摜�����[�h�ł��܂���");
				return;
			}
			// �T�C�Y�␳
			//layer.setSizeToImageSize();
			_width  = layer.width;
			_height = layer.height;
		}
	}

	/**
	 * �V���C������
	 */
	function createLayer(isfore) {
		return CharacterGraphicLayer(this, isfore);
	}
	
    /**
     * ���C����ԍX�V����
     * �G���[�V����������ǉ�
     */
	function updateLayer(layer) {
		::EnvImage.updateLayer(layer);
		// �\��`��
		if (redrawFace) {
			// �\��Ɩ��O�̍ĕ`��w�� XXXXX
			redrawFace = false;
        }
    }

	// ------------------------------------------------------
	
	/**
	 * �^�O����
	 * @param elm �R�}���h�p�����[�^
	 */
	function command(elm) {
		local e = clone elm;
		foreach (name, value in e) {
			if (doPoseCommand(name, value, e)) {
				delete e[name];
			}
		}
		::EnvImage.command(e);
	}

	/**
	 * �\�������p�̎����g�����W�V�����̎w��
	 */
	function setDispAutoTrans() {
		if ("dispTrans" in init) {
			setAutoTrans(init.dispTrans);
		} else if ("charDispTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.charDispTrans);
		}
	}

	// �t�F�[�h�w��̃f�t�H���g�l
	function getFadeValue() {
		if ("fadeValue" in init) {
			return init.fadeValue;
		} else {
			if ("charFadeValue" in env.envinfo) {
				return env.envinfo.charFadeValue;
			} else {
				return env.getFadeValue();
			}
		}
    }
};
