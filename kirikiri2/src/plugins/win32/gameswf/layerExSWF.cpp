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

void
layerExSWF::drawSWF(SWFMovie *swf)
{
	swf->draw(cairo);
}
