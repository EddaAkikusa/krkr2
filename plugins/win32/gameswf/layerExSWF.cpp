#include "layerExSWF.hpp"

/**
 * �R���X�g���N�^
 */
layerExSWF::layerExSWF(DispatchT obj) : layerExCairo(obj)
{
}

/**
 * �f�X�g���N�^
 */
layerExSWF::~layerExSWF()
{
}

/**
 * @param swf ���[�r�[
 * @param advance �o�ߎ���(ms)
 */
void
layerExSWF::drawSWF(SWFMovie *swf)
{
	swf->draw(cairo, reseted, _width, _height);
}
