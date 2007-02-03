#include "layerExSWF.hpp"

layerExSWF::layerExSWF(DispatchT obj) : layerExBase(obj)
{
	width = 0;
	height = 0;
	buffer = 0;
	pitch = 0;
	surface = NULL;
	cairo = NULL;
}

layerExSWF::~layerExSWF()
{
	cairo_destroy(cairo);
	cairo_surface_destroy(surface);
}


/**
 * ���Z�b�g����
 */
void
layerExSWF::reset() {

	// ��{����
	layerExBase::reset();
	
	// ���C���̏��ύX��������������
	if (width != _width ||
		height != _height ||
		buffer != _buffer ||
		pitch != _pitch) {

		width  = _width;
		height = _height;
		buffer = _buffer;
		pitch  = _pitch;
		
		// cairo �p�R���e�L�X�g�̍Đ���
		cairo_destroy(cairo);
		cairo_surface_destroy(surface);
		surface = cairo_image_surface_create_for_data((BYTE*)_buffer, CAIRO_FORMAT_ARGB32, width, height, pitch);
		cairo = cairo_create(surface);
		if (cairo_status(cairo) != CAIRO_STATUS_SUCCESS) {
			TVPThrowExceptionMessage(L"can't create cairo context");
			cairo_destroy(cairo);
			cairo = NULL;
		}
	}
}

void
layerExSWF::drawTest(double x, double y, double w, double h)
{
	if (cairo) {
		cairo_set_source_rgb (cairo, 0.25, 0.25, 0.25); /* dark gray */
		cairo_rectangle (cairo, x, y, w, h);
		cairo_fill (cairo);
	}
}

////////////////////////////////////////////////////////////////////////////////
/// ncBind �p�}�N��

#include "ncbind/ncbind.hpp"

NCB_GET_INSTANCE_HOOK(layerExSWF)
{
	// �C���X�^���X�Q�b�^
	NCB_INSTANCE_GETTER(objthis) { // objthis �� iTJSDispatch2* �^�̈����Ƃ���
		
		ClassT* obj = GetNativeInstance(objthis);	// �l�C�e�B�u�C���X�^���X�|�C���^�擾
		if (!obj) {
			obj = new ClassT(objthis);				// �Ȃ��ꍇ�͐�������
			SetNativeInstance(objthis, obj);		// objthis �� obj ���l�C�e�B�u�C���X�^���X�Ƃ��ēo�^����
		}
		if (obj) obj->reset();						// ���\�b�h���ĂԑO�ɕK���Ă΂��
		return (_obj = obj);						//< �f�X�g���N�^�Ŏg�p�������ꍇ�̓v���C�x�[�g�ϐ��ɕۑ�
	}

	// �f�X�g���N�^�i���ۂ̃��\�b�h���Ă΂ꂽ��ɌĂ΂��j
	~NCB_GET_INSTANCE_HOOK_CLASS () {
		if (_obj) _obj->redraw();					// ���\�b�h���Ă񂾌�ɕK���Ă΂��
	}

private:
	ClassT *_obj;
}; // ���̂� class ��`�Ȃ̂� ; ��Y��Ȃ��ł�

// �t�b�N���A�^�b�`
NCB_ATTACH_CLASS_WITH_HOOK(layerExSWF, Layer) {
	NCB_METHOD(drawTest);
}
