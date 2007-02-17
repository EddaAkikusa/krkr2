#ifndef _layExCairo_hpp_
#define _layExCairo_hpp_

#include <windows.h>
#include "layerExBase.hpp"
#include "cairo.h"

/*
 * Cairo �`��p���C��
 */
struct layerExCairo : public layerExBase
{
private:
	GeometryT width;
	GeometryT height;
	BufferT buffer;
	PitchT pitch;
	cairo_surface_t *surface;
	cairo_t * cairo;

public:
	layerExCairo(DispatchT obj);
	~layerExCairo();
	
public:
	/**
	 * ���C����񃊃Z�b�g����
	 */
	virtual void reset();

	/**
	 * �`��e�X�g
	 */
	void drawTest(double x, double y, double w, double h);
};

#endif
