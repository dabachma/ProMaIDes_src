#pragma once
/**\class Hyd_Param_RV
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_PARAM_RV_H
#define HYD_PARAM_RV_H

//system_sys_ classes
#include "_Sys_Common_System.h"
#include "Common_Const.h"

///Container class, where the members of the river model is stored (Hyd_Model_River) \ingroup hyd
/**
	This is a data container for the river model members. Besides data storing it handles:
		- the input per file parser,
		- the output of the members,
		- checking the members for application

	\see Hyd_Model_River
*/
class Hyd_Param_RV
{
	///File parser for the members of Hyd_Param_RV
	friend class Hyd_Parse_RV;
	///The members, which are stored in Hyd_Param_RV are used in Hyd_Model_River
	friend class Hyd_Model_River;
	///This class uses also some river information
	friend class HydTemp_Param;
	friend class HydTemp_Model;

public:
	///Default constructor
	Hyd_Param_RV(void);
	///Copy constructor
	Hyd_Param_RV(const Hyd_Param_RV& par);
	///Default destructor
	~Hyd_Param_RV(void);

	//members
	
	//methods
	///Set the river model parameters per parser from file 
	void parameter_per_parser(const string file, const int index, const string path);
	///Get the river number
	int get_river_number(void);
	///Get the river name
	string get_river_name(void);

	///Get a pointer to the absolute tolerance for the solver
	double* get_absolute_solver_tolerance(void);
	///Get the relative tolerance for the solver
	double get_relative_solver_tolerance(void);

	///Output the members of the river model
	void output_members(void);
	///Check the members
	void check_members(void);

	///Get the filename for the rivermodel as 2d (geometrie) for file output
	string get_filename_geometrie2file_2d(const string type);
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
	///Get the crude version of the filename for 1d output
	string get_crude_filename_result_1d(void);

	///Get the number of profiles
	int get_number_profiles(void);


	///Set the geometry file directly
	void set_geometry_file(const string file);
	///Set the river name directly
	void set_river_name(const string name);

	///Set number of instationary boudary curves
	void set_number_instant_bound(const int no);



	///Copy operator
	Hyd_Param_RV& operator= (const Hyd_Param_RV& par);

private:
	//members
	///Name of the river model
	string RVName;						
	///Number of the river model
	int RVNumber;

	///Absolute tolerance of the solver for each model
	double abs_tolerance;
	///Default absolute tolerance of the solver for each model
	const double default_abs_tol;
	///Relative tolerance of the solver for each model
	double rel_tolerance;
	///Default relative tolerance of the solver for each model
	const double default_rel_tol;





	///User settings for specifc calculation flags
	_hyd_profile_calc_setting user_setting;

	///Number of instationary boundary curves used
	int number_instat_boundary;
	///Filename where the instationary boundary curves are given
	string inst_boundary_file_name;

	///Number of profiles used
	int RVNofProf;						
	///Filename, where the profiles are given 
	string RVGeomtryFile;	

	///Filename for the output of the results for tecplot as 1d
	string tecplot_outfile_name_1d;
	///Filename for the output of the results for tecplot as 2d
	string tecplot_outfile_name_2d;

	///Global path of the files
	string global_path;

	//methods
	///Complete the filenames with the path
	void complete_filenames_with_path(string global_path);

	///Set the warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

};
#endif