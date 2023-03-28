#pragma once
/**\class Hyd_Parse_Glob
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _HYD_PARSE_GLOB_H
#define _HYD_PARSE_GLOB_H

//sys classes
#include "Common_Const.h"

//hyd_system class
#include "_Hyd_Parse_IO.h"
#include "Hyd_Param_Global.h"

///Class for parsing the general input parameter in a file for the hydraulic system \ingroup hyd
/**
The general parameter are stored in Hyd_Param_Global.

\see Hyd_Hydraulic_System, Hyd_Param_Global
*/
class Hyd_Parse_Glob: public _Hyd_Parse_IO
{
public:
	///Default constructor
	Hyd_Parse_Glob(void);
	///Default destructor
	~Hyd_Parse_Glob(void);

	//members

	//methods
	///First short check, if the input file is valid
	void check_valid_file(void);

	///Parse for the global keywords in the global parameter file and read in the parameters
	void parse_global_params(void);
	///Get the globals parameter, which are read in per file parser
	Hyd_Param_Global get_global_params(void);


private:
	//members
	///Container for all Global parameters; it is filled per fileparser
	Hyd_Param_Global Globals;

	///Foundflag for the number of rivers setting
	bool found_no_river;
	///Foundflag for the number of floodplain setting
	bool found_no_floodplain;
	///Foundflag for the number of internal timesteps
	bool found_no_int_timesteps;
	///Foundflag for the number of timesteps
	bool found_no_timesteps;
	///Foundflag for the timestep
	bool found_timestep;
	///Foundflag for the start time
	bool found_starttime;
	///Foundflag for the material file name
	bool found_material_file;


	//methods
	 ///Parse time settings
	void parse_time_setting(_hyd_keyword_file Key, word Command);
	///Parse number of floodplains 
	void parse_floodplain_number(_hyd_keyword_file Key, word Command);
	///Parse number of rivers 
	void parse_river_number(_hyd_keyword_file Key, word Command);
	///Parse number of diversion channels 
	void parse_diversion_channel_number(_hyd_keyword_file Key, word Command);
	///Parse number of structure coupling (1d2d) 
	void parse_structure_coupling_number(_hyd_keyword_file Key, word Command);
	///Parse number of dikebreak coupling river to floodplain models  
	void parse_dikebreak_rv2fp_coupling_number(_hyd_keyword_file Key, word Command);
	///Parse number of dikebreak coupling coast to floodplain models  
	void parse_dikebreak_fp2co_coupling_number(_hyd_keyword_file Key, word Command);
	///Appling of a coastmodel
	void parse_coastmodel(_hyd_keyword_file Key, word Command);
	///Appling of a temperature model
	void parse_tempmodel(_hyd_keyword_file Key, word Command);
	 ///Parse integration setting
	void parse_integration_setting(_hyd_keyword_file Key, word Command);
	///Parse output setting
	void parse_output_setting(_hyd_keyword_file Key, word Command);
	///Parse preconditioner settings
	void parse_precon_setting(_hyd_keyword_file Key, word Command);
	///Parse syncronisation settings
	void parse_sync_setting(_hyd_keyword_file Key, word Command);
	///Parse file name, where the manning values are set
	void parse_material_file(_hyd_keyword_file Key, word Command);
	///Parse file name, where the observation points are set
	void parse_obs_point_file(_hyd_keyword_file Key, word Command);
	///Check if the requiered paramaters were found
	void check_parameters_found(void);

	///Set warnings
	Warning set_warning(const int warn_type);
	///Set errors
	Error set_error(const int err_type);
};
#endif