#pragma once
/**\class Hyd_Param_Material
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_PARAM_MATERIAL_H
#define HYD_PARAM_MATERIAL_H

//system_sys_ classes
#include "_Sys_Common_System.h"

//enum
///Enumerator for the material types (Manning, Poleni) \ingroup hyd
enum _hyd_mat_coefficient_types{
	///Manning coefficient [s/m^(1/3)]
	manning_coefficient,
	///Poleni coefficient [-]
	poleni_coefficient,
};

///Data container for one material parameter: the value, the type (_hyd_mat_coefficient_types) and a identifying number are stored \ingroup hyd
struct _hyd_mat_params{
	///Value of the material parameter 
	double value;
	///Identifying number
	int type_number;
	///Type of the material parameter (_hyd_mat_coefficient_types)
	_hyd_mat_coefficient_types type; 
};
///Container class for the material parameters of the hydraulic system (Hyd_System_Hydraulic) \ingroup hyd
/**
	This is a data container for the material members. This parameters are global. They are relevant for the elements
	of a floodplain model (Hyd_Floodplain_Element) as well as the profiles of river model (_Hyd_River_Profile). 
	It can be a Manning-coefficient or a Poleni-factor (weir-flow).
	Besides data storing it handles:
		- the input per file parser,
		- the input per database,
		- the output of the members,
		- transfering the material type id to the material value, connected with a check of the material value

	\see Hyd_Floodplain_Element, _Hyd_River_Profile
*/
class Hyd_Param_Material
{
public:
	///Default constructor
	Hyd_Param_Material(void);
	///Copy constructor
	Hyd_Param_Material(Hyd_Param_Material &par);
	///Default destructor
	~Hyd_Param_Material(void);

	//members


	//methods

	///Set the filename, where the material parameters are stored
	void set_filename(const string filename);
	///Get the material parameter corresponding to material type and the identifying number
	double get_mat_value(const int mat_number, _hyd_mat_coefficient_types type);
	///Read the material parameters from file
	void read_values_per_file(void);

	///Create the database table for the material data of the hydraulic system
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the material parameters: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the material parameters
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the material parameters
	static void close_table(void);

	///Transfer the material parameters of a file into a database table
	void transfer_materialparams_file2database_table(QSqlDatabase *ptr_database, const string mat_file);
	///Transfer just one dummy material parameter to database 
	void transfer_dummy_params2database_table(QSqlDatabase *ptr_database);

	///Set the material parameters per database
	void matparams_per_database(QSqlDatabase *ptr_database, const bool with_output=true);

	///Output the members
	void output_members(void);

	///Total reset of the material parameters
	void total_reset(void);

	///Copy operator
	Hyd_Param_Material& operator= (const Hyd_Param_Material& par);
	

private:
	//members

	///Pointer to the table for the material parameters in a database
	static Tables *mat_param_table;

	///Number of the global material parameters
	int no_values;
	///Filename where the material parameters are stored
	string filename;
	///Pointer to the array with the material parameters
	_hyd_mat_params *material_params;

	///Default value for the manning parameter
	const double default_manning;
	///Default value for the poleni parameter
	const double default_poleni;

	//methods

	///Allocate the material parameters for the number of values:
	void alloc_mat_array(void);
	///Convert a string to an enumerator mat_coefficient_types
	_hyd_mat_coefficient_types convert_txt2coefficienttype(const string txt);
	///Convert an enumerator mat_coefficient_types to a string
    string convert_coefficienttype2txt(_hyd_mat_coefficient_types type);

	///Transfer the material parameters to a database table
	void material_params2database(QSqlDatabase *ptr_database);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
