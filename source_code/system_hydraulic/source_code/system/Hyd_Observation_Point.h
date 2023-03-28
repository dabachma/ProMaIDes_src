#pragma once
/**\class Hyd_Observation_Point
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef HYD_OBSERVATION_POINT_H
#define HYD_OBSERVATION_POINT_H


#include "Geo_Point.h"
#include "Hyd_Element_Floodplain.h"
#include "_Hyd_River_Profile.h"
#include "HydTemp_Model.h"

///Data container for information per time step, which are stored in one observation point \ingroup hyd
struct _hyd_observation_time_point{
	///Time point 
	double time;
	///Waterlevel h [m]
	double waterlevel;
	///Waterlevel plus geodetic height s-value [m]
	double s_value;
	///Flow velocity [m/s]
	double velocity;
	///Flow velocity in x-direction [m/s]
	double x_velocity;
	///Flow velocity in y-direction [m/s]
	double y_velocity;

	///Waterlevel change per time [m/min] ds/dt in case of a floodplain element; Froude number in case of a river pofile
	double ds2dt_fr;

	///Waterlevel change per time [m/min] due to coupling of a floodplain element
	double ds2dt_coupling;

	///Current discharge by river profile
	double discharge;
};


///Class of the observation points in the hydraulic models (e.g. river, floodplain etc.)  \ingroup hyd
/**
The results like waterlevel or flow velocity are stored in the points at each internal
timestep. The results are output to a file in tecplot-format.

*/

class Hyd_Observation_Point: public Geo_Point
{
public:
	///Default constructor
	Hyd_Observation_Point(void);
	///Default destructor
	~Hyd_Observation_Point(void);

	//method
	///Set the number of required time point
	void set_number_time_point(const int no_output, const int no_internal);
	///Synchronise observation time
	void synchron_obs_point(const double time);

	///Synchronise observation time for temperature modelling
	void synchron_temp_obs_point(const double time);

	///Set the geometrical point information
	void set_geo_point_info(Geo_Point *point);

	///Get the index of the element/profile
	int get_index_elem_prof(void);

	///Initialize the observation points for river models
	bool init_obs_point_river(Hyd_Model_River *model, const int index);
	///Initialize the observation points for river models
	bool init_obs_point_floodplain(Hyd_Model_Floodplain *model, const int index);

	///Initialize the observation points for temperature models
	bool init_temp_obs_point_river(HydTemp_Model *model, const int index);

	///Get the flag in which model (river/floodplain) the observation point is located
	bool get_model_flag(void);

	///Ouput the observation point to file as tecplot output
	void output_obs_point2file(ofstream *output, const int counter_used);
	///Ouput the observation point to file as csv output
	void output_obs_point2csvfile(ofstream *output, const int counter_used);

	///Ouput the temperature observation point to file as csv output
	void output_temp_obs_point2csvfile(ofstream *output, const int counter_used);

	///Output setted members
	void output_setted_members(ostringstream *cout, const int index);

	///Clear the observation points
	void clear_obs_point(void);

	///Clone the observation points; the pointer are not cloned
	void clone_obs_points(Hyd_Observation_Point *src, Hyd_Model_River *river,  Hyd_Model_Floodplain *floodplain);


private:

	//members
	///Number of time points
	int number_time_point;

	///Counter of already used time points
	int counter_time_points;

	///Time point
	_hyd_observation_time_point *time_point;


	///Appendant element
	Hyd_Element_Floodplain *element;
	///Appendant river profile
	_Hyd_River_Profile *profile;
	///Appendant temperature profile
	HydTemp_Profile *temp_profile;

	///Flag, if the observation point is in a floodplain or in a river model
	bool floodplain_flag;
	///Index of the model (river model or floodplain model)
	int index_model;
	///Index of the element/profile
	int index;



	//method
	///Allocate the time points
	void allocate_time_point(void);
	//Delete time points
	void delete_time_point(void);

	///Set error(s)
	Error set_error(const int err_type);

};
#endif
