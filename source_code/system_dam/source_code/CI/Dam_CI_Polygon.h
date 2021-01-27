#pragma once
/**\class _Dam_System
	\author produced by AG FRM HS-M
	\version 0.0.1
	\date 2021
*/

#ifndef DAM_CI_POLYGON
#define DAM_CI_POLYGON



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"
#include "_Geo_Polygon.h"

//dam-libs
#include "_Dam_CI_Element.h"

//Class for the polygons as CI elements points which are used in the CI-damage calculation  \ingroup dam
/**
The CI-elements, points and polygons, are connected by lists (\ref Dam_CI_Element_List) with each other. In- and outcomming list are available.
*/
class Dam_CI_Polygon : public _Dam_CI_Element, public _Geo_Polygon
{
public:
	///Default constructor
	Dam_CI_Polygon(void);
	///Default destructor
	~Dam_CI_Polygon(void);

	//method

private:
	///Set error(s)
};
#endif
