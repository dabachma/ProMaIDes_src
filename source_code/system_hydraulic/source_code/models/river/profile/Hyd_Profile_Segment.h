#pragma once
/**\class Hyd_Profile_Segment
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_PROFILE_SEGMENT_H
#define HYD_PROFILE_SEGMENT_H

//system_sys_ classes
#include "Common_Const.h"
//base class
#include "Geo_Segment.h"

///Class for a geometrical segment of a river profile \ingroup hyd
/**
The segments members are the wetted diameter and a material value (n-value).

The segments are used for the calculation of the profile specific values, like the flow area, the 
convenyance-factor etc. dependend to the waterlevel. This values are stored in the profile tables (Hyd_Tables).

\see _Hyd_River_Profile_Type
*/
class Hyd_Profile_Segment : public Geo_Segment
{
public:
	///Default constructor
	Hyd_Profile_Segment(void);
	///Copy constructor
	Hyd_Profile_Segment(const Hyd_Profile_Segment& object);
	///Default destructor
	~Hyd_Profile_Segment(void);

	//method
	///Set the profile segment specific parameters for a segment used in a river profile
	void set_profilespecific_members(const double n_value, const int activity);
	///Re-calculate the profile segment specific members
	void recalc_profilespecific_members(void);

	///Get the effective wetted diameter of this segment
	double get_wetted_diameter(void);
	///Get the product of effective wetted diameter with the  material-value
	double get_luXn_value(void);
	///Get the effective wetted diameter of this segment
	double get_lu(void);
	///Get the material-value
	double get_mat_value(void);

	///Get the minimal y_coordinate
	double get_minimal_y_coordinate(void);

	///Output the segment members to display/console
	void output_members(ostringstream *cout);

	///Switch the points of the segment (point1->point2)
	void switch_points(void);

	///Copy operator
	Hyd_Profile_Segment& operator=(const Hyd_Profile_Segment& object);

private:
	//members
	///Material-value (n-value)
	double n_value;
	///Wetted diameter [m]
	double lu; 
	///Active wetted diameter factor (1 is active; 0 is inactive, e.g. waterlevel segment)
	int activity_lu;

	double luXn_value;

	//methods
	///Calculate the specific values, e.g. wetted diameter of the segment
	void calculate_specific_values(void);
};
#endif