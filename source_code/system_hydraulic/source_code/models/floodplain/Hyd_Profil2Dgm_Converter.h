#pragma once
/**\class Hyd_Profil2Dgm_Converter
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2013
*/
#ifndef HYD_PROFIL2DGM_CONVERTER_H
#define HYD_PROFIL2DGM_CONVERTER_H

//sys system
#include "HydGui_Profil2Dgm_Converter_Dia.h"
//hyd system
#include "Hyd_Floodplain_Polysegment.h"
#include "Hyd_Floodplain_Dikeline_Point_List.h"


///Class to convert given profile points to a river DGM-W \ingroup hyd
/**
It interpolatess points of profile with x, y, z, data to a DGM-W surface
along given streamlines.

\see 
*/
class Hyd_Profil2Dgm_Converter
{



public:
	///Default constructor
	Hyd_Profil2Dgm_Converter(void);
	///Default destructor
	~Hyd_Profil2Dgm_Converter(void);

	//method
	///Set the dialog pointer with the information about files, number of profiles and number of streamlines
	void set_dia_ptr(HydGui_Profil2Dgm_Converter_Dia *ptr);

	///Start the interpolation
	void start_interpolation(void);

	

private:
	//member
	///Pointer to the dialog with infomration about files, number of profiles and number of streamlines
	HydGui_Profil2Dgm_Converter_Dia *dia;
	///Filename of the profile
	string file_prof;
	///Number of profiles
	int no_prof;
	///Filename of the left streamline
	string file_stream_l;
	///Filename of the right streamline
	string file_stream_r;
	///Number of additional streamlines
	int no_add_stream;
	///Right streamline as polysegment
	Hyd_Floodplain_Polysegment stream_r;
	///Left streamline as polysegment
	Hyd_Floodplain_Polysegment stream_l;
	///Profiles as polysegment
	Hyd_Floodplain_Polysegment *profiles;

	///Additional streamlines as polysegment
	Hyd_Floodplain_Polysegment *add_streamline;


	///Density along streamlines
	double density_stream;

	///Offset x direction
	double offset_x;
	///Offset y direction
	double offset_y;

	//method
	///Input the data from file
	void input_data_file(void);
	///Set additional stream line points on the segment between left and right streamline
	void set_additional_stream_line_points(void);

	///Intersect streamlines with profiles
	void intersect_stream_line_profile(void);

	///Output points to file
	void output_points_to_file(void);

	///Allocate the polysegments for the profiles
	void allocate_profiles(void);
	///Delete the polysegments for the profiles
	void delete_profiles(void);

	///Allocate additional streamlines
	void allocate_additional_stream(void);
	///Delete additional streamlines
	void delete_additional_streamlines(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif

