#pragma once
/**\class Fpl_Slope_External_Force
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef FPL_SLOPE_EXTERNAL_FORCE_H
#define FPL_SLOPE_EXTERNAL_FORCE_H

//system sys
#include "Geo_Polysegment.h"

//system fpl


///Geometrical class for external forces for a slope calculation \ingroup fpl
/**


*/
class Fpl_Slope_External_Force : public Geo_Polysegment
{
public:
	///Default constructor
	Fpl_Slope_External_Force(void);
	///Default destructor
	~Fpl_Slope_External_Force(void);

	//method
	///Set the orthogonal distance to the mid point of the slip cricle (lever arm) for each segment
	void set_lever_arm(Geo_Point *mid_point, const bool landside);
	///Calculate external water forces to the slip circle
	double calc_external_water_forces(const double water_level, const double water_density);

private:

	//member
	///Orthogonal distance to the mid point of the slip cricle (lever arm) for each segment
	double *lever_arm;

	//method
	///Allocate the orthogonal distance to the mid point of the slip cricle (lever arm) for each segment
	void allocate_lever_arm(void);
	///Delete the orthogonal distance to the mid point of the slip cricle (lever arm) for each segment
	void delete_lever_arm(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);


};
#endif
