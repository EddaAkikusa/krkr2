#ifndef IRRLICHTGUIENVIRONMENT_H
#define IRRLICHTGUIENVIRONMENT_H

#include <irrlicht.h>

/**
 * GUI������p�N���X
 */
class IrrlichtGUIEnvironment {

protected:
	irr::gui::IGUIEnvironment *gui;
	
public:
	IrrlichtGUIEnvironment();
	IrrlichtGUIEnvironment(irr::gui::IGUIEnvironment *gui);
	IrrlichtGUIEnvironment(const IrrlichtGUIEnvironment &orig);
	~IrrlichtGUIEnvironment();

};

#endif

