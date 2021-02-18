#pragma once
/**\class Hyd_River_Profile_Type_Standard
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_RIVER_PROFILE_TYPE_STANDARD_H
#define HYD_RIVER_PROFILE_TYPE_STANDARD_H


//system_hyd classes
#include "_Hyd_River_Profile_Type.h"

//forward declaration
class _Hyd_River_Profile;

///Class of a river profile type representing a standard river profile \ingroup hyd
/**

\see _Hyd_River_Profile_Type
*/
class Hyd_River_Profile_Type_Standard : public _Hyd_River_Profile_Type
{
public:
	///Default constructor
	Hyd_River_Profile_Type_Standard(void);
	///Default destructor
	virtual ~Hyd_River_Profile_Type_Standard(void);

	//members
	///Table in which the convenyance for left bank is stored dependent from the area
	Hyd_Tables c_left_table;
	///Table in which the convenyance for mid channel is stored dependent from the area
	Hyd_Tables c_mid_table;
	///Table in which the convenyance for right bank is stored dependent from the area
	Hyd_Tables c_right_table;

	//method
	///Calculate and allocate the tables (Hyd_Tables): area, waterlevel, convenyance for left bank, right bank and mid channel
	virtual void calc_alloc_tables(const double delta_x, Hyd_Param_Material *material_table);
	///Generate a plot of the profile (HydGui_Profile_Plot)
	void generate_plot(QWidget *parent=NULL);

	///Output the Hyd_Profile_Segment of the profile
	void output_area_segments(void);
	///Output the Hyd_Tables, which are generated
	void output_tables(void);

	///Get the convenyance factor for the mid channel
	double get_c_mid_channel(void);
	///Get the convenyance factor total
	virtual double get_c_value(void);

	///Set the actuel flow specific values with a given area from tables
	virtual void set_actuel_profilevalues_by_table(const double area);
	///Get the waterlevel by a given area from table 
	virtual double get_waterlevel_from_table(const double area);

	///Get the width by a given area from table 
	virtual double get_width_from_table(const double area);

	///Set the actuel flow specific values with a given waterlevel from tables
	virtual void set_actuel_profilevalues_by_waterlevel(const double global_waterlevel);

	///Calculate the actual discharge through this profile by a given upstream and downstream profile
	virtual double calculate_actual_discharge(_Hyd_River_Profile *upstream_profile,  _Hyd_River_Profile *downstream_profile, const double distance_upstream);

	///Get a starting value in cause of: a hole before, this profile is dry, the waterlevel before is greater than this global z_min value
	double get_starting_value(void);

	///Clone profile type
	virtual void clone_profile_type(_Hyd_River_Profile_Type *prof_type);

protected:
	//members
	///Number of points for the mid channel
	int no_mid_points;
	///Number of segments for the mid channel
	int no_mid_segments;
	///Number of points for the left bank
	int no_left_bank_points;
	///Number of segments for the left bank
	int no_left_bank_segments;
	///Number of points for the right bank
	int no_right_bank_points;
	///Number of segments for the right bank
	int no_right_bank_segments;
	///Total number of segments in this profile
	int total_segment_number;

	///Actual convenyance value for the right bank
	double c_right_value;
	///Actual convenyance value for the mid channel
	double c_mid_value;
	///Actual convenyance value for the left bank
	double c_left_value;


	///Pointer to a Polyline of Segments for the mid channel
	Hyd_Profile_Segment *mid_profile;
	///Pointer to a Polyline of Segments for the left bank
	Hyd_Profile_Segment *left_bank_profile;
	///Pointer to a Polyline of Segments for the right bank
	Hyd_Profile_Segment *right_bank_profile;

	///Flag if a left bank is existent
	bool left_bank_flag;
	///Flag if a right bank is existent
	bool right_bank_flag;

	//methods
		
	///Set the actual conveyance with a given area from tables
	virtual void set_actual_conveyance_by_table(const double area);

private:
	//method
	///Calculate the gradient
	virtual void calculate_gradient(const double distance, _Hyd_River_Profile *upstream, _Hyd_River_Profile *downstream_profile);
	///Check if there is a right bank and a left bank in the profile
	void count_area_points(void);

	///Generate for each area (left bank, mid ,right bank) a poly line of Hyd_Profile_Segment
	void generate_area_segments(Hyd_Param_Material *material_table);
	///Calculate the number of Hyd_Profile_Segment for each area (left, right bank, mid channel)
	void calculate_number_segments(void);
	///Allocate the Hyd_Profile_Segment for each area (left, right bank, mid channel)
	void allocate_area_segments(void);
	///Delete the Hyd_Profile_Segment for each area (left, right bank, mid channel)
	void delete_area_segments(void);
	///Set the Hyd_Profile_Segment (left, right bank, mid channel) with the given profile points (Hyd_River_Profile_Points)
	void set_points2areasegments(Hyd_Param_Material *material_table);

	///Generate the relevant Hyd_Profile_Segment by intersecting the profile with the waterlevel
	void generate_intersected_area_segments(const int no_real_segments, Hyd_Profile_Segment *real_segments, int *no_intersect_segments, Hyd_Profile_Segment *(*intersect_segments));

	///Calculate the table values (h, a, conveyance) and store them in Hyd_Tables class
	void calculate_table_values(const double delta_x);
	///Calculate the area with the generated Hyd_Profile_Segment for the table generation
	double calculate_area2table(const int no_intersect_segments, Hyd_Profile_Segment *intersect_segments);
	///Calculate the width with the generated Hyd_Profile_Segment for the table generation
	double calculate_width2table(const int no_intersect_segments, Hyd_Profile_Segment *intersect_segments);
	///Calculate the hydraulic_radius with the generated Hyd_Profile_Segment for the table generation
	double calculate_hydraulic_radius2table(const int no_intersect_segments, Hyd_Profile_Segment *intersect_segments);
	///Calculate the conveyance with the generated Hyd_Profile_Segment for the table generation
	double calculate_conveyance2table(const int no_intersect_segments, Hyd_Profile_Segment *intersect_segments, const double area);

	///Set the error
	Error set_error(const int err_type);
	
	
};
#endif