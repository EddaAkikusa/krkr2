#include "ncbind/ncbind.hpp"
#include <vector>
using namespace std;

//----------------------------------------------
// ���C���C���[�W���샆�[�e�B���e�B

// �o�b�t�@�Q�Ɨp�̌^
typedef unsigned char       *WrtRefT;

/**
 * ���C���̃T�C�Y�ƃo�b�t�@���擾����
 */
static bool
GetLayerSize(iTJSDispatch2 *lay, long &w, long &h, long &pitch)
{
	// ���C���C���X�^���X�ȊO�ł̓G���[
	if (!lay || TJS_FAILED(lay->IsInstanceOf(0, 0, 0, TJS_W("Layer"), lay))) return false;

	// ���C���C���[�W�͍݂邩�H
	tTJSVariant val;
	if (TJS_FAILED(lay->PropGet(0, TJS_W("hasImage"), 0, &val, lay)) || (val.AsInteger() == 0)) return false;

	// ���C���T�C�Y���擾
	val.Clear();
	if (TJS_FAILED(lay->PropGet(0, TJS_W("imageWidth"), 0, &val, lay))) return false;
	w = (long)val.AsInteger();

	val.Clear();
	if (TJS_FAILED(lay->PropGet(0, TJS_W("imageHeight"), 0, &val, lay))) return false;
	h = (long)val.AsInteger();

	// �s�b�`�擾
	val.Clear();
	if (TJS_FAILED(lay->PropGet(0, TJS_W("mainImageBufferPitch"), 0, &val, lay))) return false;
	pitch = (long)val.AsInteger();

	// ����Ȓl���ǂ���
	return (w > 0 && h > 0 && pitch != 0);
}

// �������ݗp
static bool
GetLayerBufferAndSize(iTJSDispatch2 *lay, long &w, long &h, WrtRefT &ptr, long &pitch)
{
	if (!GetLayerSize(lay, w, h, pitch)) return false;

	// �o�b�t�@�擾
	tTJSVariant val;
	if (TJS_FAILED(lay->PropGet(0, TJS_W("mainImageBufferForWrite"), 0, &val, lay))) return false;
	ptr = reinterpret_cast<WrtRefT>(val.AsInteger());
	return  (ptr != 0);
}

/**
 * Layer.copyRightBlueToLeftAlpha
 * ���C���E������ Blue CHANNEL ���������� Alpha �̈�ɕ�������
 */
static tjs_error TJS_INTF_METHOD
copyRightBlueToLeftAlpha(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *lay)
{
	// �������ݐ�
	WrtRefT dbuf = 0;
	long dw, dh, dpitch;
	if (!GetLayerBufferAndSize(lay, dw, dh, dbuf, dpitch)) {
		TVPThrowExceptionMessage(TJS_W("dest must be Layer."));
	}

	// ����
	dw /= 2;
	// �R�s�[

	WrtRefT sbuf = dbuf + dw*4;
	dbuf += 3;
	for (int i=0;i<dh;i++) {
		WrtRefT p = sbuf;   // B�̈�
		WrtRefT q = dbuf;   // A�̈�
		for (int j=0;j<dw;j++) {
			*q = *p;
			p += 4;
			q += 4;
		}
		sbuf += dpitch;
		dbuf += dpitch;
	}
	return TJS_S_OK;
}

NCB_ATTACH_FUNCTION(copyRightBlueToLeftAlpha, Layer, copyRightBlueToLeftAlpha);
