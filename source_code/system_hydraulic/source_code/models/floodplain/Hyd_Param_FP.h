#pragma once
/**\class Hyd_Param_FP
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_PARAM_FP_H
#define HYD_PARAM_FP_H

//system_sys_ classes
#include "_Sys_Common_System.h"
#include "Common_Const.h"

///Structure where the geometrical information of the floodplain are stored \ingroup hyd
struct _hyd_floodplain_geo_info{
	///Element width in x-direction
	double width_x;
	///Element width in y-direction
	double width_y;
	///Angle of the x-axis to the normal x-direction (--> x-normal; positive angle=> turning of the coodinate system (global) in clockwise direction (local) )
	double angle;
	///Number of elements in x
	int number_x;
	///Number of elements in y
	int number_y;
	///Global x-coordinate of the origin
	double origin_global_x;
	///Global y-coordinate of the origin
	double origin_global_y;
};

///Structure where the geometrical information of the floodplain are stored \ingroup hyd
struct _hyd_floodplain_scheme_info {
	///Selected scheme for the floodplain simulation
	model::schemeTypes::schemeTypes scheme_type;
	///Device selected to calculate the simulation on
	int selected_device;
	///Courant number used when calculating a maximum timestep to use
	double courant_number;
	///Number used to calculate the parallelism of the reduction kernel
	int reduction_wavefronts;
	///Flag for using friction in the selected scheme
	bool friction_status;
	///Workgroup size of the GPU in the x-direction
	int workgroup_size_x;
	///Workgroup size of the GPU in the y-direction
	int workgroup_size_y;
};
///Container class, where the members of the floodplain model is stored (Hyd_Model_Floodplain) \ingroup hyd
/**
	This is a data container for the floodplain model members. Besides data storing it handles:
		- the input per file parser,
		- the output of the members,
		- checking the members for application

	\see Hyd_Model_Floodplain
*/
class Hyd_Param_FP
{
	///File parser for the members of Hyd_Param_FP
	friend class Hyd_Parse_FP;
	///The members, which are stored in Hyd_Param_FP are used in Hyd_Model_Floodplain
	friend class Hyd_Model_Floodplain;

public:
	///Default constructor
	Hyd_Param_FP(void);
	///Copy constructor
	Hyd_Param_FP(const Hyd_Param_FP& par);
	///Default destructor
	~Hyd_Param_FP(void);

	//members				

	//methods
	///Set the floodplain model parameters per parser from file
	void parameter_per_parser(const string file, const int index, const string path);
	///Get the floodplain number
	int get_floodplain_number(void);
	///Get the floodplain name
	string get_floodplain_name(void);
	///Check the mebers
	void check_members(void);
	///Get number of elements in x-direction
	int get_no_elems_x(void);
	///Get number of elements in y-direction
	int get_no_elems_y(void);
	///Get the geometrical information of the floodplain
	_hyd_floodplain_scheme_info get_scheme_info(void);
	///Get the geometrical information of the floodplain
	_hyd_floodplain_geo_info get_geometrical_info(void);
	///Get a pointer to the absolute tolerance for the solver
	double* get_absolute_solver_tolerance(void);
	///Get the relative tolerance for the solver
	double get_relative_solver_tolerance(void);
	//Get the boundary, where the element is wet
	double get_wet_boundary(void);
	///Output the members of the floodplain model
	void output_members(void);
	///Get the filename for the floodplain geometrie file
	string get_filename_geometrie2file(const string type);
	///Get the filename for the floodplain geometrie file without suffix
	string get_filename_geometrie(const string type);
	///Get the filename for the floodplain result file 
	string get_filename_result2file(const string type, const double timepoint);
	///Get the filename for the floodplain result file without suffix
	string get_filename_result(const string type);
	///Get the filename for the floodplain result file 
	string get_filename_result_max2file(const string type);
	///Get the filename for the maximum floodplain observation points to file 
	string get_filename_obs_point2file(const string type);
	///Calculate the approximate workspace requirement
    long long int calculate_approx_workspace(void);

	///Get pointer to the element width in x-direction
	double *get_ptr_width_x(void);
	///Get pointer to the element width in y-direction
	double *get_ptr_width_y(void);
	///Get pointer to the element area
	double *get_ptr_elem_area(void);

	///Copy operator
	Hyd_Param_FP& operator= (const Hyd_Param_FP& par);

	///Convert the scheme type enumerator from text to enum
	model::schemeTypes::schemeTypes convert_txt2schemetype(string txt);
	///Convert the scheme type enumerator from enum to txt 
	string convert_schemetype2txt(model::schemeTypes::schemeTypes type);

private:
	///Name of the Floodplain model
	string FPName;	
	///Index of the Flodplain model
	int FPNumber;
	///Number of elements in x
	int FPNofX;
	///Number of elements in y
	int FPNofY;

	///Element width in x-direction
	double width_x;
	///Element width in y-direction
	double width_y;
	///Area of element
	double area;
	///Angle of the x-axis to the normal x-direction (--> x-normal; positive angle=> anti-clockwise)
	double angle;

	///Global x-coordinate of the origin
	double FPLowLeftX;
	///Global y-coordinate of the origin
	double FPLowLeftY;		
	///Name of the element file
	string floodplain_file;

	///Number of instationary boundary curves used
	int number_instat_boundary;
	///Filename where the instationary boundary curves are given
	string inst_boundary_file;

	///Value which marks a element, where no information is available
	double noinfo_value;	

	///Selected scheme for the floodplain simulation
	model::schemeTypes::schemeTypes scheme_type;
	///Device selected to calculate the simulation on
	int selected_device;
	///Courant number used when calculating a maximum timestep to use
	double courant_number;
	///Number used to calculate the parallelism of the reduction kernel
	int reduction_wavefronts;
	///Flag for using friction in the selected scheme
	bool friction_status;
	///Workgroup size of the GPU in the x-direction
	int workgroup_size_x;
	///Workgroup size of the GPU in the y-direction
	int workgroup_size_y;

	///Number of Polygons which enclose noflow elements
	int number_noflow_polys;
	///Filename where the Polygons enclosing noflow elements are stated
	string noflow_file;

	///Number of polylines which set a boundary for Poleni evaluation to the corresponding elments
	int number_dike_polylines;
	///Filename where the dike polylines are stated
	string dikeline_file;

	///Name of the result file
	string tecplot_outfile_name;

	///Absolute tolerance of the solver for each model
	double abs_tolerance;
	///Default absolute tolerance of the solver for each model
	const double default_abs_tol;
	///Relative tolerance of the solver for each model
	double rel_tolerance;
	///Default relative tolerance of the solver for each model
	const double default_rel_tol;
	
	///Boundaryvalue to mark cells as wet or dry; no influence to numerical solution, only to Tecplot result files
	double FPWet;
	///Default boundaryvalue to mark cells as wet or dry; no influence to numerical solution, only to Tecplot result files
	const double default_wet_bound;


	//methods
	///Complete the filenames with the path
	void complete_filenames_with_path(string global_path);

	///Calculate the element area
	void calculate_area(void);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

};
#endif
