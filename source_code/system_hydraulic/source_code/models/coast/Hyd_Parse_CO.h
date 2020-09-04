#pragma once
/**\class Hyd_Parse_CO
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_PARSE_CO_H
#define HYD_PARSE_CO_H

//hyd_system class
#include "_Hyd_Parse_IO.h"

///Data structure where the parameter of Hyd_Coast_Model are gathered \ingroup hyd
struct _hyd_coast_params{
	///Name of the model
	string model_name;
	///Name of the file where the coast line is specified
	string filename_coastline;
	///Name of the file where the instationary boundary curve of a gloabl waterlevel is specified 
	string filename_instat_boundary;
	///Name of the file where the geometry is shown  
	string filename_geometry_tecplot;
};
///Parser class for a file input of the coast model data stored in the hydraulic system file (Hyd_Coast_Model) \ingroup hyd
/**
	\see Hyd_Coast_Model
*/
class Hyd_Parse_CO : public _Hyd_Parse_IO 
{
public:
	///Default constructor
	Hyd_Parse_CO(void);
	///Default destructor
	~Hyd_Parse_CO(void);

	//method
	///Parse for the global coastmodel keywords and read in the parameters for the coastmodel
	void parse_coastmodel_params(void);
	///Get the read in coast parameter (_hyd_coast_params)
	_hyd_coast_params get_coastmodel_params(void);


private:
	//members
	///The read in parameter of Hyd_Coast_Model are gathered here
	_hyd_coast_params params;

	//methods
	///Parse for the name of the coastline file name and the direction of the water to it
	void parse_coastline(_hyd_keyword_file Key, word Command);
	///Parse for the instationary_boundary_file name
	void parse_instat_boundary(_hyd_keyword_file Key, word Command);
	///Parse for the geometry_file name
	void parse_geometry_file_tecplot(_hyd_keyword_file Key, word Command);

	///Check if the requiered paramaters were found
	void check_parameters_found(void);


	///Set the warning
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
