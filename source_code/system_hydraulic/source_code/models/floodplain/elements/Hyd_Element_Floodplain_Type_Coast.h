#pragma once
/**\class Hyd_Element_Floodplain_Type_Coast
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_ELEMENT_FLOODPLAIN_TYPE_COAST_H
#define HYD_ELEMENT_FLOODPLAIN_TYPE_COAST_H

//hyd system
//base class
#include "_Hyd_Element_Floodplain_Type.h"

///Class for the element type of the floodplain-model, which is in a coast-model \ingroup hyd
/**
The element is determined via interception with the coast-model. The waterlevel of this
element type is set by the coast-model directly.
\see Hyd_Coast_Model
*/
class Hyd_Element_Floodplain_Type_Coast : public _Hyd_Element_Floodplain_Type
{
///Class where the element type is decided and which work with the element type
friend class Hyd_Element_Floodplain;

public:
	///Default constructor
	Hyd_Element_Floodplain_Type_Coast(void);
	///Default destructor
	~Hyd_Element_Floodplain_Type_Coast(void);

	//method
	///Initialize the element (here it is not needed)
	void init_element(_hyd_neighbouring_elems neigh_elem, double *width_x, double *width_y, double *area, Hyd_Param_Material *mat_table, const bool clone);

	//Make the syncronistation of the elements to a given time point
	void calculate_boundary_value(const double time_point);
	///Calculate the maximum values and the wet duration
	void calc_max_values(const double time_point, const double wet_boundary);

	///Set the pointer to the sea waterlevel
	void set_ptr_sea_waterlevel(double *ptr);



	///Get the global waterlevel (s_value)
	double get_s_value(void);
	///Get the local waterlevel (h_value)
	double get_h_value(void);
	///Get the if the element is wet
	double get_wet_flag(void);

	///Get the maximum local waterlevel (h_value)
	_hyd_max_values get_max_h_value(void);
	///Get the overall was_wet_flag
	bool get_was_wet_flag(void);
	///Get the duration of overflooding
	double get_wet_duration(void);
	///Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
	void set_maximum_value2querystring(ostringstream *query_string);
	///Set the instationary result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
	void set_instat_value2querystring(ostringstream *query_string);

	///Reset the hydrological balance value and the maximum values
	void reset_hydrobalance_maxvalues(void);

private:
	///members
	///Pointer to the sea-waterlevel
	double *ptr_sea_waterlevel;
	///Gloabl waterlevel:= geodetic height (z) + waterlevel (h); it is also the result of the solver [result variables]
	double s_value;
	///Local waterlevel:= waterlevel (h) [result variables]
	double h_value;
	///Flag if the element is wet [result variables]
	bool wet_flag;
	///Maximum local waterlevel [maximum result variables
	_hyd_max_values max_h_value;
	///Point in time when the element gets wet [result variables]
    double time_wet_start;
	///Duration ohow long the elemnt was wet [result variables]
    double wet_duration;
	///Point in time when the water arrives first at this element
    double first_arrival_time;

	//methods
	///Calculate the maximum values to a given time point
	void calculate_maximum_values(const double time_point);
	///Calculate if the element gets wet or dry to a given timepoint and the duration of being wet
	void calculate_wet_dry_duration(const double time_point, const double wet_boundary);

	///Output the setted members
	void output_setted_members(ostringstream *cout);
	///Ouput the result members per internal timestep
	void output_result_values(ostringstream *cout);
	///Ouput the maximum calculated values
	void output_maximum_calculated_values(ostringstream *cout);

};

#endif
