#pragma once
/**\class Hyd_Element_Floodplain_Type_Dam
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012 
*/
#ifndef HYD_ELEMENT_FLOODPLAIN_TYPE_DAM_H
#define HYD_ELEMENT_FLOODPLAIN_TYPE_DAM_H

//hyd system
//base class
#include "_Hyd_Element_Floodplain_Type.h"

///This class represents the an elements of a floodplain model, with just the result values \ingroup hyd
/**
This element type is exclusively used in a DAM-Project, where the hydraulic results are defined by the user.
No hydrodynamic numerical calculation is performed.

*/
class Hyd_Element_Floodplain_Type_Dam : public _Hyd_Element_Floodplain_Type
{
public:
	///Default constructor
	Hyd_Element_Floodplain_Type_Dam(void);
	///Default destructor
	~Hyd_Element_Floodplain_Type_Dam(void);


	//method
	///Initialize the element (call it backward: not from the origin; start at the diagonal corner of the origin)
	void init_element(_hyd_neighbouring_elems neigh_elem, double *width_x, double *width_y, double *area, Hyd_Param_Material *mat_table, const bool clone);
	
	///Set the maximum results values directly
	void set_max_results_directly(const double waterlevel, const double max_v, const double duration, const double dh_dt);

	///Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
	void set_maximum_value2querystring(ostringstream *query_string);

	///Get the overall was_wet_flag
	bool get_was_wet_flag(void);

private:

	//method
	///Output the setted members
	void output_setted_members(ostringstream *cout);
	///Ouput the maximum calculated values
	void output_maximum_calculated_values(ostringstream *cout);
	///Ouput the result members per internal timestep
	void output_result_values(ostringstream *cout);


	//members
	///Maximum local waterlevel [maximum result variables]
    float max_h_value;
	///Maximum of the variation of the global waterlevel over time [maximum result variables]
    float max_ds_dt;
	///Maximum total flow velocity [maximum result variables]
    float max_v_total;
	///Maximum value of the product of the total flow velocity times waterlevel (h*v)
    float max_hv;
	///Duration how long the element was wet [result variables]
	bool wet_duration;
};

#endif
