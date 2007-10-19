#pragma comment(lib, "gdiplus.lib")
#include "LayerExText.hpp"

#include <stdio.h>
/**
 * ���O�o�͗p
 */
static void log(const tjs_char *format, ...)
{
	va_list args;
	va_start(args, format);
	tjs_char msg[1024];
	_vsnwprintf(msg, 1024, format, args);
	TVPAddLog(msg);
	va_end(args);
}

// GDI+ ��{���
static GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR gdiplusToken;

/// �v���C�x�[�g�t�H���g���
static PrivateFontCollection *privateFontCollection = NULL;
static vector<void*> fontDatas;

// GDI+ ������
void initGdiPlus()
{
	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

// GDI+ �I��
void deInitGdiPlus()
{
	// �t�H���g�f�[�^�̉��
	delete privateFontCollection;
	vector<void*>::const_iterator i = fontDatas.begin();
	while (i != fontDatas.end()) {
		delete[] *i;
		i++;
	}
	fontDatas.clear();
	GdiplusShutdown(gdiplusToken);
}

// --------------------------------------------------------
// �t�H���g���
// --------------------------------------------------------

/**
 * �v���C�x�[�g�t�H���g�̒ǉ�
 * @param fontFileName �t�H���g�t�@�C����
 */
void
GdiPlus::addPrivateFont(const tjs_char *fontFileName)
{
	if (!privateFontCollection) {
		privateFontCollection = new PrivateFontCollection();
	}
	ttstr filename = TVPGetPlacedPath(fontFileName);
	if (filename.length()) {
		if (!wcschr(filename.c_str(), '>')) {
			// ���t�@�C��������
			TVPGetLocalName(filename);
			privateFontCollection->AddFontFile(filename.c_str());
			return;
		} else {
			// �������Ƀ��[�h���ēW�J
			IStream *in = TVPCreateIStream(filename, TJS_BS_READ);
			if (in) {
				STATSTG stat;
				in->Stat(&stat, STATFLAG_NONAME);
				// �T�C�Y���ӂꖳ������
				ULONG size = stat.cbSize.QuadPart;
				char *data = new char[size];
				if (in->Read(data, size, &size) == S_OK) {
					privateFontCollection->AddMemoryFont(data, size);
					fontDatas.push_back(data);					
				} else {
					delete[] data;
				}
				in->Release();
				return;
			}
		}
	}
	TVPThrowExceptionMessage((ttstr(TJS_W("cannot open : ")) + fontFileName).c_str());
}

/**
 * �v���C�x�[�g�t�H���g�ꗗ�����O�ɏo��
 */
void
GdiPlus::showPrivateFontList()
{
	if (!privateFontCollection)	return;
	int count = privateFontCollection->GetFamilyCount();

	// fontCollection.
	FontFamily *families = new FontFamily[count];
	if (families) {
		TVPAddLog("--- private font families ---");
		privateFontCollection->GetFamilies(count, families, &count);
		for (int i=0;i<count;i++) {
			WCHAR familyName[LF_FACESIZE];
			if (families[i].GetFamilyName(familyName) == Ok) {
				TVPAddLog(familyName);
			}
		}
	}
}

/**
 * �R���X�g���N�^
 */
FontInfo::FontInfo() : fontFamily(NULL), emSize(12), style(0) {}

/**
 * �R���X�g���N�^
 * @param familyName �t�H���g�t�@�~���[
 * @param emSize �t�H���g�̃T�C�Y
 * @param style �t�H���g�X�^�C��
 */
FontInfo::FontInfo(const tjs_char *familyName, REAL emSize, INT style) : fontFamily(NULL)
{
	setFamilyName(familyName);
	setEmSize(emSize);
	setStyle(style);
}

/**
 * �R�s�[�R���X�g���N�^
 */
FontInfo::FontInfo(const FontInfo &orig)
{
	fontFamily = orig.fontFamily ? orig.fontFamily->Clone() : NULL;
	emSize = orig.emSize;
	style = orig.style;
}

/**
 * �f�X�g���N�^
 */
FontInfo::~FontInfo()
{
	clear();
}

/**
 * �t�H���g���̃N���A
 */
void
FontInfo::clear()
{
	delete fontFamily;
	fontFamily = NULL;
	familyName = "";
}

/**
 * �t�H���g�̎w��
 */
void
FontInfo::setFamilyName(const tjs_char *familyName)
{
	if (familyName) {
		clear();
		if (privateFontCollection) {
			fontFamily = new FontFamily(familyName, privateFontCollection);
			if (fontFamily->IsAvailable()) {
				this->familyName = familyName;
				return;
			} else {
				clear();
			}
		}
		fontFamily = new FontFamily(familyName);
		if (fontFamily->IsAvailable()) {
			this->familyName = familyName;
			return;
		} else {
			clear();
		}
	}
}

// --------------------------------------------------------
// �A�s�A�����X���
// --------------------------------------------------------

Appearance::Appearance() {}

Appearance::~Appearance()
{
	clear();
}

/**
 * ���̃N���A
 */
void
Appearance::clear()
{
	drawInfos.clear();
}


/**
 * �Œ�F�u���V�̒ǉ�
 * @param argb �F�w��
 * @param ox �\���I�t�Z�b�gX
 * @param oy �\���I�t�Z�b�gY
 */
void
Appearance::addSolidBrush(ARGB argb, REAL ox, REAL oy)
{
	drawInfos.push_back(DrawInfo(ox, oy, new SolidBrush(Color(argb))));
}


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
void 
Appearance::addLinearGradientBrush(REAL x1, REAL y1, ARGB argb1, REAL x2, REAL y2, ARGB argb2, REAL ox, REAL oy)
{
	drawInfos.push_back(DrawInfo(ox, oy, new LinearGradientBrush(PointF(x1,y1), PointF(x2,y2), Color(argb1), Color(argb2))));
}

/**
 * �Œ�F�y���̒ǉ�
 * @param ox �\���I�t�Z�b�gX
 * @param oy �\���I�t�Z�b�gY
 * @param argb �F�w��
 * @param width �y����
 */
void 
Appearance::addColorPen(ARGB argb, REAL width, REAL ox, REAL oy)
{
	Pen *pen = new Pen(Color(argb), width);
	// �Ƃ肠�����܂邭�ڑ�
	pen->SetLineCap(LineCapRound, LineCapRound, DashCapRound);
	pen->SetLineJoin(LineJoinRound);
	drawInfos.push_back(DrawInfo(ox, oy, pen));
}

// --------------------------------------------------------
// �t�H���g�`��n
// --------------------------------------------------------

/**
 * �R���X�g���N�^
 */
LayerExText::LayerExText(DispatchT obj)
	: layerExBase(obj), width(-1), height(-1), pitch(0), buffer(NULL), bitmap(NULL), graphics(NULL)
{
}

/**
 * �f�X�g���N�^
 */
LayerExText::~LayerExText()
{
	delete graphics;
	delete bitmap;
}

void
LayerExText::reset()
{
	layerExBase::reset();
	// �ύX����Ă���ꍇ�͂���Ȃ���
	if (!(graphics &&
		  width  == _width &&
		  height == _height &&
		  pitch  == _pitch &&
		  buffer == _buffer)) {
		delete graphics;
		delete bitmap;
		width  = _width;
		height = _height;
		pitch  = _pitch;
		buffer = _buffer;
		bitmap = new Bitmap(width, height, pitch, PixelFormat32bppARGB, (unsigned char*)buffer);
		graphics = new Graphics(bitmap);
		graphics->SetSmoothingMode(SmoothingModeAntiAlias);
		graphics->SetCompositingMode(CompositingModeSourceOver);
		graphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
	}
}

/**
 * �p�X�ł̕�����̕`��i�u���V�{�y���j
 * @param text �`��e�L�X�g
 * @param x ���_X
 * @param y ���_Y
 */
void
LayerExText::drawString(FontInfo *font, Appearance *app, REAL x, REAL y, const tjs_char *text, bool noupdate)
{
	// ������̃p�X������
	GraphicsPath path;
	path.AddString(text, -1, font->fontFamily, font->style, font->emSize, PointF(x, y), NULL);

	// �̈�L�^�p
	Rect unionRect;
	Rect rect;

	// �`������g���Ď��X�`��
	vector<Appearance::DrawInfo>::const_iterator i = app->drawInfos.begin();
	while (i != app->drawInfos.end()) {
		if (i->info) {
			Matrix matrix(1,0,0,1,i->ox,i->oy);
			graphics->SetTransform(&matrix);
			switch (i->type) {
			case 0:
				{
					Pen *pen = (Pen*)i->info;
					graphics->DrawPath(pen, &path);
					if (!noupdate) {
						path.GetBounds(&rect, &matrix, pen);
						unionRect.Union(unionRect, unionRect, rect);
					}
				}
				break;
			case 1:
				graphics->FillPath((Brush*)i->info, &path);
				if (!noupdate) {
					path.GetBounds(&rect, &matrix, NULL);
					unionRect.Union(unionRect, unionRect, rect);
				}
				break;
			}
		}
		i++;
	}

	if (!noupdate) {
		// �X�V����
		tTJSVariant  vars [4] = { unionRect.X, unionRect.Y, unionRect.Width, unionRect.Height };
		tTJSVariant *varsp[4] = { vars, vars+1, vars+2, vars+3 };
		_pUpdate(4, varsp);
	}
}
