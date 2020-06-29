#pragma once
/**\class Hyd_River_Profile_Type_Weir
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_RIVER_PROFILE_TYPE_WEIR_H
#define HYD_RIVER_PROFILE_TYPE_WEIR_H

//hyd system classes
//base class
#include "Hyd_River_Profile_Type_Standard.h"

///Class of a river profile type representing a weir profile \ingroup hyd
/**

\see _Hyd_River_Profile_Type
*/
class Hyd_River_Profile_Type_Weir : public Hyd_River_Profile_Type_Standard
{
public:
	///Default constructor
	Hyd_River_Profile_Type_Weir(void);
	///Default destructor
	~Hyd_River_Profile_Type_Weir(void);

	//members
	///Pointer to an array of profile table for each weir-segment in which the discharge for channel is stored dependent to the waterlevel 
	Hyd_Tables *weir_discharge_table;

	//methods
	///Calculate and allocate the tables (Hyd_Tables): area, waterlevel, convenyance for left bank, right bank (mid channel is weir channel)
	void calc_alloc_tables(const double delta_x, Hyd_Param_Material *material_table);
	//Set the actuel flow specific values with a given waterlevel from tables
	void set_actuel_profilevalues_by_waterlevel(const double global_waterlevel);
	///Output the tables
	void output_tables(void);
	///Calculate the actual discharge through this profile by a given upstream and downstream profile
	double calculate_actual_discharge(_Hyd_River_Profile *upstream_profile,  _Hyd_River_Profile *downstream_profile, const double distance_upstream);
	
	///Set the actuel flow specific values with a given area from tables; not needed for a weir type; values are set by calculating the discharge directly
	void set_actuel_profilevalues_by_table(const double area);

	///Reset the maximum values and the result
	void reset_result_max_values(void);

	///Clone river profile type
	void clone_profile_type(_Hyd_River_Profile_Type *prof_type);


private:
	//members
	///Local downstream waterlevel; given by the upstream profile
	double downstream_waterlevel_local;
	///Global downstream waterlevel; given by the upstream profile
	double downstream_waterlevel_global;
	///Local upstream waterlevel; given by the upstream profile
	double upstream_waterlevel_local;
	///Global upstream waterlevel; given by the upstream profile
	double upstream_waterlevel_global;

	//methods
	///Check the segments of the main channel (= weir channel)
	void check_weir_segments(void);
	///Calculate the weir tables
	void calculate_weir_tables(void);
	///Calculate the overflow discharge for each segment for generating the table 
	double calculate_segment_overflow_discharge2table(Hyd_Profile_Segment *segment);
	///Set actual conveyance with a given global waterlevel
	void set_actual_conveyance_by_table(const double waterlevel);
	///Set the error(s)
	Error set_error(const int err_type);
};
#endif
