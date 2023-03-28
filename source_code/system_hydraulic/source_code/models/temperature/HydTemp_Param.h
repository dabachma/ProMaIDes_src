#pragma once
/**\class HydTemp_Param
	\author Daniel Bachmann et al.
	\author produced by the AG-FRM University of applied sciences Magdeburg-Stendal
	\version 0.0.1                                                              
	\date 2023 
*/
#ifndef HYDTEMP_PARAM_H
#define HYDTEMP_PARAM_H

//system_sys_ classes
#include "_Sys_Common_System.h"
#include "Common_Const.h"

///Container class, where the members of the temperature model is stored (HydTemp_Model) \ingroup hyd
/**
	This is a data container for the temperature model members. Besides data storing it handles:
		- the input per file parser,
		- the output of the members,
		- checking the members for application

	\see HydTemp_Model
*/
class HydTemp_Param
{
	///File parser for the members of HydTemp_Param
	friend class HydTemp_Parse;
	///The members, which are stored in HydTemp_Param are used in Hyd_Model_River
	friend class HydTemp_Model;
	friend class HydTemp_Profile;

public:
	///Default constructor
	HydTemp_Param(void);
	///Copy constructor
	HydTemp_Param(const HydTemp_Param& par);
	///Default destructor
	~HydTemp_Param(void);

	//members
		///Pointer to the parameters of the corresponding river model
	Hyd_Param_RV *Param_RV;
	
	//methods
	///Set the temperature model parameters of the river model per parser from file 
	void parameter_per_parser(const string file, const int index, const string path);
//
	///Output the members of the temperature model
	void output_members(void);
	///Check the members
	void check_members(void);

	///Get the filename for the rivermodel results file as 2d for file output
	string get_filename_result2file_2d(const string type);
	///Get the filename for the rivermodel results file as 1d for file output
	string get_filename_result2file_1d(const string type);
	///Get the filename for the rivermodel maximum result to file as 1d file output
	string get_filename_result2file_1d_maxvalues(const string type);
	///Get the filename for the rivermodel maximum result to file as 2d file output
	string get_filename_result2file_2d_maxvalues(const string type);
	///Get the filename for the rivermodel observation points to file output
	string get_filename_result2file_1d_obs_point(const string type);



	///Set the temperature data profile file directly
	void set_temp_profile_file(const string file);

	///Copy operator
	HydTemp_Param& operator= (const HydTemp_Param& par);
//
private:
	//members
			
	///Filename, where the profiles information are given 
	string temp_profile_File;	

	///Groundwater temperature [K]
	double gw_temp;
	///Brunt coefficient [-]
	double brunt_coef;
	///View-to-sky coefficient [0-1] [-]
	double view2sky_coef;
	///Coefficient of heat conductivity multiplied by the distance (3-52) [W/(m^2 * K)]
	double con_bed_coef;
	///Bed temperature [K]
	double bed_temp;
	///Coefficient bed warming [-]
	double bed_warming_coeff;
	///Coefficient diffusive solar radiation [-]
	double diffusiv_sol_rad_coeff;




//
//	//methods
	///Complete the filenames with the path
	void complete_filenames_with_path(string global_path);

	///Set the warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

};
#endif