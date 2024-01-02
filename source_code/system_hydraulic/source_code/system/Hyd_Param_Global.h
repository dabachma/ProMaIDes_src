#pragma once
/**\class Hyd_Param_Global
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_PARAM_GLOBAL_H
#define HYD_PARAM_GLOBAL_H

//sys classes
#include "_Sys_Common_System.h"
#include "Common_Const.h"
#include "Tables.h"

//enums
///Enumerator for defining the type of preconditioner settings for the cvode-solver \ingroup hyd
enum _hyd_prec_type {
	///Left preconditioning of the matrix to solve in the cvode-solver
	ePREC_LEFT = 1,
	///Right preconditioning of the matrix to solve in the cvode-solver
	ePREC_RIGHT = 2
};
///Enumerator for defining the type of Gram-Schmidt orthogonalization scheme for the cvode-solver \ingroup hyd
enum _hyd_gs_scheme_type {
	///Modified Gram-Schmidt orthogonalization scheme of the matrix to solve in the cvode-solver
	eMODIFIED_GS = 1,
	///Classical Gram-Schmidt orthogonalization scheme of the matrix to solve in the cvode-solver
	eCLASSICAL_GS = 2 
};




///Structure where the geometrical information of the floodplain are stored \ingroup hyd
struct _hyd_output_flags {

	///Output flag if a tecplot 1d output is required
	bool tecplot_1d_required;
	///Output flag if a tecplot 2d output is required
	bool tecplot_2d_required;

	///Output flag if a bluekenue 2d output is required
	bool bluekenue_2d_required;

	///Output flag if a paraview / csv 1d output is required
	bool paraview_1d_required;
	///Output flag if a paraview / csv 2d output is required
	bool paraview_2d_required;

	///Output flag if database instationary output is required
	bool database_instat_required;

	///String to outputfolder
	string output_folder;

};

///Container class for the global parameters of the hydraulic system (Hyd_System_Hydraulic) \ingroup hyd
/**
	This is a data container for the hydraulic system members. Besides data storing it handles:
		- the input per file parser,
		- the input per database,
		- the output of the members,
		- checking the members for application
		- the managment of the static database tables
 
	\see Hyd_System_Hydraulic
*/
class Hyd_Param_Global
{
	///File parser for the members of Hyd_Param_Global
	friend class Hyd_Parse_Glob;
	///Part of the members, which are stored in Hyd_Param_Global are used in _Hyd_Model, like the integration settings or  preconditioner settings for the cvode-solver
	friend class _Hyd_Model;
	///Main part of the members, which are stored in Hyd_Param_Global are used in Hyd_Hydraulic_System
	friend class Hyd_Hydraulic_System;
	///Class for displaying the mebers in a widget
	friend class HydGui_System_Member_Wid;

public:
	///Default constructor
	Hyd_Param_Global(void);
	///Copy constructor
	Hyd_Param_Global(Hyd_Param_Global &par);
	///Default destructor
	~Hyd_Param_Global(void);

	//members
	///Pointer to the table for the global parameters in a database
	static Tables *global_param_table;

	//method

	///Set global parameter per parser from a given file
	void globals_per_parser(string globalfile);

	///Create the database table for the global data of the hydraulic system
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the global parameters: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database, const bool not_close=false);
	///Delete all data in the database table for the global parameters 
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the global parameters
	static void close_table(void);

	///Transfer the global parameter of a file into a database table
	void transfer_globals_file2database_table(QSqlDatabase *ptr_database, const string globalfile);

	///Transfer default global parameter to database table
	void transfer_default2database_table(QSqlDatabase *ptr_database);

	///Set the global parameters per database
	void globals_per_database(QSqlDatabase *ptr_database, const bool output=true);

	///Output the global parameter
	void output_members(void);

	///Check the mebers
	void check_members(void);

	///Filename conversion method
	static string get_print_filename(const int MaxLength, const char *FName);

	///Get the start time
	double get_startime(void);
	///Get the stepsize
	double get_stepsize(void);
	///Get number of timesteps
	double get_number_timesteps(void);

	///Get the total number of models (river, floodplian and coast-model)
	int get_total_number_models(void);
	///Get the total number of setted couplings (dikebreak, diversion channel, structure coupling)
	int get_total_number_setted_couplings(void);
	///Get the number of floodplain models in the system
	int get_number_floodplain_model(void);
	///Get the number of river models in the system
	int get_number_river_model(void);
	///Get the flag if the coast model is applied in the system
	bool get_coast_model_applied(void);

	///Get the output flags which output is required
	_hyd_output_flags get_output_flags(void);

	///Set OpenCL available flag (Current system has an OpenCL runtime)
	void set_opencl_available(bool);

	///Get OpenCL available flag (Current system has an OpenCL runtime)
	bool get_opencl_available(void);

	///Set the number of river-models
	void set_number_river_models(const int number);
	///Set the number of floodplain-models
	void set_number_floodplain_models(const int number);
	///Set if the coast-model is applied
	void set_coast_model_applied(const bool applied);
	///Set the number of diversion-channels
	void set_number_diversion_channels(const int number);
	///Set the number of the coupling between river- and floodplain-models via a dikebreak/wallbreak
	void set_number_rv2fp_dikebreaks(const int number);
	///Set the number of the coupling between coast- and floodplain-models via a dikebreak/wallbreak
	void set_number_fp2co_dikebreaks(const int number);
	///Set the number of the coupling between river- and floodplain-models via a hydraulic structure
	void set_number_hydraulic_structure_coupling(const int number);

	///Get the global path
	string get_global_path(void);

	///Total reset of the global parameters
	void total_reset(void);

	///Copy operator
	Hyd_Param_Global& operator= (const Hyd_Param_Global& par);

private:
	//members

	///Number of the set of global parameters
	int param_set_number;
	///Starttime
	double GlobTStart;
	///Length of the global timestep
	double GlobTStep;	
	///Number of global timesteps
	int GlobTNof;
	///Number of internal timesteps (between the global timstep)
	int GlobNofITS;

	///Number of floodplain models
	int GlobNofFP;
	///Number of river models
	int GlobNofRV;
	///Flag if a coast model is appleid
	bool coastmodel_applied;

	///Flag if temperature model applied
	bool tempmodel_applied;

	///Flag if OpenCL is available
	bool opencl_available;

	///Number of diversion channels (have to be smaller than number of river models)
	int number_div_channel;
	///Number of structure couplings
	int number_struc_coupling;
	///Number of dikebreak couplings river to floodplain model
	int number_dikebreak_rv2fp_coupling;
	///Number of dikebreak couplings coast to floodplain model
	int number_dikebreak_fp2co_coupling;

	///Filename of the material file
	string material_file;
	///Filename of the observation point file
	string obs_point_file;

	///Name of the path of the global file
	string global_path;
	
	///Maximum number of timesteps for the solver to reach next internal time
    int GlobMaxNumSteps;
	///Default value for the maximum number of timesteps for the solver to reach next internal time
    const int default_max_steps;
	///Maximum stepsize for the solver
	double GlobMaxStepSize;	
	///Initial stepsize for the solver
	double GlobInitStepSize;
	///Default value for the initial stepsize for the solver
	const double default_init_stepsize;

	///Preconditioning type for the preconditioner (enum _hyd_prec_type) 
	_hyd_prec_type GlobPreType;				
	///Specifies the type of Gram-Schmidt orthogonalization for the preconditioner (enum _hyd_gs_scheme_type)
	_hyd_gs_scheme_type GlobGramSchmidt;

	///Syncronisation settings: maximum of waterlevel change per syncronisation timestep in a floodplain element (boundary condition, coupling)
	double max_h_change_fp;
	///Default value for the syncronisation settings: maximum of waterlevel change per syncronisation timestep in a floodplain element (boundary condition, coupling)
	const double default_max_h_change_fp;
	///Syncronisation settings: maximum of waterlevel change per syncronisation timestep in a river element (boundary condition, coupling)
	double max_h_change_rv;
	///Default value for the syncronisation settings: maximum of waterlevel change per syncronisation timestep in a river element (boundary condition, coupling)
	const double default_max_h_change_rv;
	///Syncronisation settings: maximum change of the flow velocity energy head explicitly taken in a river model per syncronisation timestep
	double max_v_change_rv;
	///Default value for syncronisation settings: maximum change of the flow velocity energy head explicitly taken in a river model per syncronisation timestep
	const double default_max_v_change_rv;

	///Syncronisation settings: minimum of internal timestep
	double min_internal_step;
	///Default value for syncronisation settings: minimum of internal timestep	
	const double default_min_internal_step;


	///Total number of models
	int total_number_models;
	///Total number of setted couplings
	int total_setted_couplings;

	///Output flags, which output is required
	_hyd_output_flags output_flags;



	//methods

	///Calculate the total number of models and the total number of setted coupling
	void calculate_total_numbers(void);
	///Convert the Preconditioner type enumerator from text to enum
	_hyd_prec_type convert_txt2precontype(string txt);
	///Convert the Preconditioner type enumerator from enum to txt 
	string convert_precontype2txt(_hyd_prec_type type);

	///Convert the Gramschmidt type enumerator from text to enum
	_hyd_gs_scheme_type convert_txt2gramschmidttype(string txt);
	///Convert the Gramschmidt type enumerator from enum to txt 
	string convert_gramschmidttype2txt(_hyd_gs_scheme_type type);

	///Set the global path
	void set_global_path(string filename);
	///Input the global parameters to a database table
	void input_globals2database_table(QSqlDatabase *ptr_database, const bool output=true);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
