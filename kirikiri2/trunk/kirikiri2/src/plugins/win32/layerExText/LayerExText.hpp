#ifndef _layExText_hpp_
#define _layExText_hpp_

#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include <vector>
using namespace std;

#include "layerExBase.hpp"

/**
 * �t�H���g���
 */
class FontInfo {
	friend class LayerExText;

protected:
	FontFamily *fontFamily; //< �t�H���g�t�@�~���[
	REAL emSize; //< �t�H���g�T�C�Y 
	INT style; //< �t�H���g�X�^�C��

	/**
	 * �t�H���g���̃N���A
	 */
	void clearFont();

public:

	FontInfo();
	/**
	 * �R���X�g���N�^
	 * @param familyName �t�H���g�t�@�~���[
	 * @param emSize �t�H���g�̃T�C�Y
	 * @param style �t�H���g�X�^�C��
	 */
	FontInfo(const tjs_char *familyName, REAL emSize, INT style);
	FontInfo(const FontInfo &orig);

	/**
	 * �f�X�g���N�^
	 */
	virtual ~FontInfo();

	void setFamilyName(const tjs_char *familyName);
	void setEmSize(REAL emSize) { this->emSize = emSize; }
	REAL getEmSize() {  return emSize; }
	void setStyle(INT style) { this->style = style; }
	INT getStyle() { return style; }

	/**
	 * �v���C�x�[�g�t�H���g�̒ǉ�
	 * @param fontFileName �t�H���g�t�@�C����
	 */
	static void addPrivateFont(const tjs_char *fontFileName);

	/**
	 * �v���C�x�[�g�t�H���g�ꗗ�����O�ɏo��
	 */
	static void showPrivateFontList();
};

/**
 * �`��O�Ϗ��
 */
class Appearance {
	friend class LayerExText;
public:
	// �`����
	struct DrawInfo{
		int type;   // 0:�u���V 1:�y��
		void *info; // ���I�u�W�F�N�g
		REAL ox; //< �\���I�t�Z�b�g
		REAL oy; //< �\���I�t�Z�b�g
		DrawInfo() : ox(0), oy(0), type(0), info(NULL) {}
		DrawInfo(REAL ox, REAL oy, Pen *pen) : ox(ox), oy(oy), type(0), info(pen) {}
		DrawInfo(REAL ox, REAL oy, Brush *brush) : ox(ox), oy(oy), type(1), info(brush) {}
		DrawInfo(const DrawInfo &orig) {
			ox = orig.ox;
			oy = orig.oy;
			type = orig.type;
			if (orig.info) {
				switch (type) {
				case 0:
					info = (void*)((Pen*)orig.info)->Clone();
					break;
				case 1:
					info = (void*)((Brush*)orig.info)->Clone();
					break;
				}
			} else {
				info = NULL;
			}
		}
		virtual ~DrawInfo() {
			if (info) {
				switch (type) {
				case 0:
					delete (Pen*)info;
					break;
				case 1:
					delete (Brush*)info;
					break;
				}
			}
		}	
	};
	vector<DrawInfo>drawInfos;

public:
	Appearance();
	virtual ~Appearance();

	/**
	 * ���̃N���A
	 */
	void clear();
	
	/**
	 * �Œ�F�u���V�̒ǉ�
	 * @param argb �F�w��
	 * @param ox �\���I�t�Z�b�gX
	 * @param oy �\���I�t�Z�b�gY
	 */
	void addSolidBrush(ARGB argb, REAL ox=0, REAL oy=0);

	/**
	 * �O���f�[�V�����u���V�̒ǉ�
	 * @param x1
	 * @param y1
	 * @param argb1 �F�w�肻�̂P
	 * @param x1
	 * @param y1
	 * @param argb1 �F�w�肻�̂Q
	 * @param ox �\���I�t�Z�b�gX
	 * @param oy �\���I�t�Z�b�gY
	 */
	void addLinearGradientBrush(REAL x1, REAL y1, ARGB argb1, REAL x2, REAL y2, ARGB argb2, REAL ox=0, REAL oy=0);
	
	/**
	 * �Œ�F�y���̒ǉ�
	 * @param argb �F�w��
	 * @param width �y����
	 * @param ox �\���I�t�Z�b�gX
	 * @param oy �\���I�t�Z�b�gY
	 */
	void addColorPen(ARGB argb, REAL width=1.0, REAL ox=0, REAL oy=0);
};

/*
 * �A�E�g���C���x�[�X�̃e�L�X�g�`�惁�\�b�h�̒ǉ�
 */
class LayerExText : public layerExBase
{
protected:
	// ���ێ��p
	GeometryT width, height;
	BufferT   buffer;
	PitchT    pitch;
	
	/// ���C�����Q�Ƃ���r�b�g�}�b�v
	Bitmap *bitmap;
	/// ���C���ɑ΂��ĕ`�悷��R���e�L�X�g
	Graphics *graphics;

public:	
	LayerExText(DispatchT obj);
	~LayerExText();
	virtual void reset();

public:
	/**
	 * �o�^�ς݃u���V/�y���ɂ�镶����̕`��
	 * @param font �t�H���g
	 * @param app �A�s�A�����X
	 * @param x �`��ʒuX
	 * @param y �`��ʒuY
	 * @param text �`��e�L�X�g
	 * @param noupdate ��ʍX�V�������s��Ȃ�
	 */
	void drawString(FontInfo *font, Appearance *app, REAL x, REAL y, const tjs_char *text, bool noupdate=false);
};

#endif
