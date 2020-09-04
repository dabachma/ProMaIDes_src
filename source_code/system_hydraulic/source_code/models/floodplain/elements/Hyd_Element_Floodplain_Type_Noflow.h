#pragma once
/**\class Hyd_Element_Floodplain_Type_Noflow
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_ELEMENT_FLOODPLAIN_TYPE_NOFLOW_H
#define HYD_ELEMENT_FLOODPLAIN_TYPE_NOFLOW_H

//hyd system
//base class
#include "_Hyd_Element_Floodplain_Type.h"

///Class for the element type of the floodplain-model, which is in a noflow polygon \ingroup hyd
/**
The element is determined via interception with a noflow-polygon. There is no flow and no waterlevel in this 
element.
\see Hyd_Floodplain_Polygon
*/
class Hyd_Element_Floodplain_Type_Noflow : public _Hyd_Element_Floodplain_Type
{
///Class where the element type is decided and which work with the element type
friend class Hyd_Element_Floodplain;

public:
	///Default constructor
	Hyd_Element_Floodplain_Type_Noflow(void);
	///Default destructor
	~Hyd_Element_Floodplain_Type_Noflow(void);

	//methods
	///Initialize the element (here it is not needed)
	void init_element(_hyd_neighbouring_elems neigh_elem, double *width_x, double *width_y, double *area, Hyd_Param_Material *mat_table, const bool clone);

private:
	///Output the setted members
	void output_setted_members(ostringstream *cout);

	///Ouput the result members per internal timestep
	void output_result_values(ostringstream *cout);
	///Ouput the maximum calculated values
	void output_maximum_calculated_values(ostringstream *cout);
};

#endif