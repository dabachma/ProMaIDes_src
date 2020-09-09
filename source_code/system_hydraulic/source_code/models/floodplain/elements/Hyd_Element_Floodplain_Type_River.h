#pragma once
/**\class Hyd_Element_Floodplain_Type_River
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_ELEMENT_FLOODPLAIN_TYPE_RIVER_H
#define HYD_ELEMENT_FLOODPLAIN_TYPE_RIVER_H

//hyd system
//base class
#include "_Hyd_Element_Floodplain_Type.h"


///Class for the element type of the floodplain-model, which is in a river polygon \ingroup hyd
/**
The element is determined via interception with a the polygon of the river model. There is no flow determined for this 
element type. The waterlevel is set to the waterlevel of a neighbouring standard element, if existing. It is done
because of a damage calculation, where the hydraulic results are important input data. 
\see Hyd_Model_River
*/
class Hyd_Element_Floodplain_Type_River : public _Hyd_Element_Floodplain_Type
{
///Class where the element type is decided and which work with the element type
friend class Hyd_Element_Floodplain;

public:
	///Default constructor
	Hyd_Element_Floodplain_Type_River(void);
	///Default destructor
	~Hyd_Element_Floodplain_Type_River(void);

	//methods
	///Initialize the element 
	void init_element(_hyd_neighbouring_elems neigh_elem, double *width_x, double *width_y, double *area, Hyd_Param_Material *mat_table, const bool clone);

	//Set the connected element
	void set_connected_element(_hyd_neighbouring_elems neigh_elem);

	///Calculate the maximum values and the wet duration
	void calc_max_values(const double time_point, const double wet_boundary); 

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

	//member
	///Element wich is connected to this river element
	Hyd_Element_Floodplain *connected_elem;
	///Direction of the connected element
	_hyd_neighbouring_direction connected_elem_dir;
	///Data structure of the flow data of the element (_hyd_flowdata_floodplain_elem) [calculation variables]
	_hyd_flowdata_floodplain_elem flow_data;

	///Width of the element in x-direction [calculation variables]
	double *x_width;
	///Width of the element in x-direction [calculation variables]
	double *y_width;

	///Gloabl waterlevel:= geodetic height (z) + waterlevel (h); it is also the result of the solver [result variables]
	double s_value;
	///Local waterlevel:= waterlevel (h) [result variables]
	double h_value;
	///Maximum local waterlevel [maximum result variables]
	_hyd_max_values max_h_value;
	///Flag if the element is wet [result variables]
	bool wet_flag;
	///Point in time when the element gets wet [result variables]
    double time_wet_start;
	///Duration ohow long the elemnt was wet [result variables]
    double wet_duration;

	//method
	///Set the data buffers of _hyd_boundarydata_floodplain_elem, _hyd_flowdata_floodplain_elem and the global geodetic height from class Hyd_Floodplain_Element 
    void set_data_buffers(const _hyd_boundarydata_floodplain_elem boundary, const _hyd_flowdata_floodplain_elem flow, double *z_value);
	
	///Get a pointer to the flow data
	_hyd_flowdata_floodplain_elem get_flow_data(void);

	///Output the setted members
	void output_setted_members(ostringstream *cout);
	///Ouput the result members per internal timestep
	void output_result_values(ostringstream *cout);
	///Ouput the maximum calculated values
	void output_maximum_calculated_values(ostringstream *cout);

	///Set element flag for flow in x-direction
	void set_x_noflow_flag(const bool flag);
	///Set element flag for flow in y-direction
	void set_y_noflow_flag(const bool flag);
	///Set the absolute element border height z (m) in x-direction and the Poleni-factor
	void set_x_border_z(const double abs_height, const double pol_factor);
	///Set the absolute element border height z (m) in y-direction and the Poleni-factor
	void set_y_border_z(const double abs_height, const double pol_factor);

	///Set the waterlevel from the connected element
	void set_connected_waterlevel(void);

	///Calculate the maximum values to a given time point
	void calculate_maximum_values(const double time_point);

	///Calculate if the element gets wet or dry to a given timepoint and the duration of being wet
	void calculate_wet_dry_duration(const double time_point, const double wet_boundary);

	///Check if the neighbouring element is convenient to connect it to this element
	bool find_convenient_neigh_element(Hyd_Element_Floodplain *neigh_elem, _hyd_neighbouring_direction dir, double *height_diff);
	///Get the element index of the connected element (important for Hyd_Element_Floodplian_Type_River because here the connected element number is returned)
	int get_coupling_relevant_element_number(void);
};

#endif
