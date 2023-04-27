#pragma once
#ifndef COMMON_CONST_H
#define COMMON_CONST_H
//include of lib
#include <string>//stdlib for string-functions
#include <iostream>//stdlib for readin/read out
#include <iomanip>//stdlib for formated input/output
#include <sstream>//stdlib for handling stringstreams
#include <fstream>//stdlib for file handling
#include <cmath>//standard math-functions
#include <limits>//Defines the template class numeric_limits and two enumerations concerning floating-point representations and rounding
#include <ctime>//time functions
#include <cstdio>//remove files
#include <omp.h>//parallel programming
#include <memory.h>//Simple wrappers for the C libraries allocation functions.

//qt classes
#include <QtSql>//from the Qt open sorce version;
#include <QTableWidget>

//std namespace 
using namespace std;



//do not output the warning von vc about the enum use with a scope (e.g. _sys_table_type::fpl)
//#pragma warning(disable : 4482)


//defines
//Output Format Macros
#define FORMAT_FIXED_REAL setiosflags( ios_base::right ) << resetiosflags( ios::scientific ) << setiosflags( ios_base::fixed )
//Scientific output
#define FORMAT_SCIENTIFIC_REAL setiosflags( ios_base::right ) << resetiosflags( ios::fixed ) << setiosflags( ios_base::scientific )
//Set the whitespaces for output
#define W(A) setw(A)
//Set the rest with A
#define FI(A) setfill(A)
//Set the precision of the output
#define P(A) setprecision(A)
//For the old string used in the hyd_system
typedef char word[512];




//module definition and explaination for the doxygen generator

//If tables-name are implemented: ATTENTION just use maximum 30 signs !!!

///The module SYStem provides general classes for the application like the output, exception-handling, GUI-classes or geometry-classes
/**
* @defgroup sys SYS module
* This module make blabla
*/

///The module FloodProtectionLine (FPL) provides classes for the calculation of failure probabilities or fragility curves for different section, e.g. dike, flood-walls, of the floodprotection line
/**
* @defgroup fpl FPL module
* This module makes blabla
*/

///The module HYDraulic provides classes for the hydraulic claculation for 1d-river and 2d-floodplains
/**
* @defgroup hyd HYD module
* This module makes blabla
*/

///The module DAMage provides classes for the damage claculation in connection with the results of the hydraulic module
/**
* @defgroup dam DAM module
* This module makes blabla
*/

///The module RISK combines the results of the FPL-module (failure probability) and the DAM-module (damages) to the risk
/**
* @defgroup risk RISK module
* This module makes blabla
*/

///The module MultiAttributeDesicionMethod (MADM) includes mathematical method for the evaluation of a decision matrix
/**
* @defgroup madm MADM module
* This module makes blabla
*/

///The module COST includes cost calculation methods of diverse flood protection alternatives
/**
* @defgroup cost COST module
* This module makes blabla
*/

///The module ALT includes methods for the implementation of diverse floodprotection alternatives into the system
/**
* @defgroup alt ALT module
* This module makes blabla
*/



//..introduce further modules


///Data structure for describing the state of the area of investigation \ingroup sys
struct _sys_system_id{
	///Identifier for the area state; in each area state different measure can be implemented and tested
	int area_state;
	///Identifier for the measure number of an area state
	int measure_nr;
};

///Enumerator for the modules used in the system \ingroup sys
enum _sys_system_modules{
	///Modul system_sys
	SYS_SYS,
	///Modul system_sys geometry
	GEOSYS_SYS,
	///Modul system_hyd
	HYD_SYS,
	///Modul system_fpl
	FPL_SYS,
	///Modul system_madm
	MADM_SYS,
	///Modul system_dam
	DAM_SYS,
	///Modul exception
	EXC_SYS,
	///Modul risk
	RISK_SYS,
	///Modul cost
	COST_SYS,
	///Modul alternatives
	ALT_SYS,
	///Modul hydrology
	HYDROL_SYS,
	//..introduce further modules

};
///General mathematical constants for all moduls \ingroup sys
namespace constant{

	///flag if 2dGPU solver is used TODO: later user setting!
	const bool gpu2d_applied = false;

	//general
	///Acceleration of gravity
	const double Cgg=9.8066;
	///Viscosity of Water (10°)
	const double Cvisco_water=1.31e-6;
	///Number Pi
	const double Cpi=3.1415926;
	///Constant for the Poleni formula
	const double Cfacweir=pow(2.0*Cgg,0.5)*2.0/3.0;

	///Density of water [kg/m³]
	const double dens_water = 1000.0;
	///Density of air [kg/m³]
	const double dens_air = 1.204;
	///C-value water [J/kg*K]
	const double c_water = 4195.0;
	///C-value air [J/kg*K]
	const double c_air = 1015.0;
	///Stefan-Boltzmann Konstante
	const double boltzman = 5.67037e-8;
	///Air Pressure (mbar)
	const double pressure_air = 1013;
	///Psychometric constant
	const double psychometric = 0.67;
	///Kelvin comnstant
	const double kelvin_const = 273.15;



	///Maximum number of threads
	const int max_threads=7;

	///Seconds per day
	const int day_second=86400;
	///Seconds per hour
	const int hour_second=3600;
	///Seconds per minute
	const int minute_second=60;

	///Bytes per KByte
	const double kbyte_size=1000.0;
	///Bytes per MByte
	const double mbyte_size=1000.0*1000.0;
	///Bytes per GByte
	const double gbyte_size=1000.0*1000.0*1000.0;

	///Maximum of elements
    const int max_elems=30000000;
	///Maximum row number to select
	const int max_rows=250000;

	//general epsilon boundaries
	///Epsilon boundary for the area values 
	const double area_epsilon=1.0e-3;
	///Epsilon boundary to check a value which is numerically greater than zero
	const double zero_epsilon=1.0e-8;
	///Epsilon boundary to check equality in meter (e.g. coordinates, waterlevels)
	const double meter_epsilon=2.0e-4;
	///Epsilon boundary to check equality of percentage value (e.g. relative areas)
	const double percentage_epsilon=1.0e-4;
	///Epsilon boundary to check equality of degree value (e.g. angle)
	const double angle_epsilon=1.0e-5;
	///Epsilon boundary to check equality of time value
	const double sec_epsilon=1.0e-5;
	///Epsilon boundary to check a equality of gradients
	const double diff_grad_epsilon=1.0e-5;

	//hydraulic
	///Boundary for the gradient of the hydraulic flow calculation
	const double flow_epsilon=1.0e-9;
	///Boundary of wet and dry elements/profiles for the hydraulic flow calculation 
	const double dry_hyd_epsilon=1.0e-4;
	///Double of the boundary of wet and dry elements/profiles for the hydraulic flow calculation 
	const double double_dry_hyd_epsilon=2.0e-4;
	///Sill height for the FP2FP coupling of models
	const double sill_heightcoupling_fp= 5.0e-8;
	///Factor for the smoothing of the coupling discharge with the coupling discharge calculated one internal timestep before (0.0 no smoothing; 1.0 no new calculated discharge is taken)
	const double smoothing_coupling_factor=0.0;
	///Standard value for the Poleni factor for the broad weir
	const double poleni_const=0.577;
	///Standard value for the side weir reduction for the Poleni formula used by the coupling via a weir in direction river to floodplain
	const double side_weir_reduction=0.95;
	///Standard value for the 1d stabilization discharge
	const double stabilization_q=50.0;
	///The value for the minimal bridge height; it is 1.0 m
	const double min_height_bridge=1.0;
	///The value for the minimal bridge body size; it is 1.0 m
	const double min_seize_bridge=0.15;
	///Number of the Krylow-Subspace for the cvode solver
	const int krylow_subspace=5;

	///Maximum height differences for connected river elements
	const double max_height_diff = 1.0;

	///Default value for a maximum breach width [m]
	const double maximum_breach_width=200.0;

	///Number of stored gradients
	const int no_stored_grad=1;

	//risk
	///Number of elements stored in a block of _risk_mc_scenario (\see _risk_mc_scenario_block)
	const int risk_sc_block_number=50;

	//fpl
	///Maximum value for the boundary of a random variable (\see Random_Variables)
	const double max_boundary=1e20;
	///Minum value for the boundary of a random variable (\see Random_Variables)
	const double min_boundary=-1e20;


	


}

///General functions for all moduls
namespace functions{
	

	///Signum function
	double signum_func(const double value);
	///Convert seconds into a string day:hour:minute:second
	string convert_seconds2string(const double seconds);
	///Convert seconds into a string year/month/day hour:minute:second
	string convert_seconds2datestring(const double seconds);
    ///Convert seconds into a structure tm from ctime year/month/day hour:minute:second
    void convert_seconds2datestruct(const double seconds, tm *t_struct);

	///Convert time to time string
	string convert_time2time_str(const double seconds);
	///Convert time to time string without '
	string convert_time2time_str_without(const double seconds);

	///Function to encrypt/decrypt the password with XOR
	string crypter(string value,string key);
	///Convert a byte value (int) into a string with units
    string convert_byte2string(const long long int mem_bytes);
	///Check a double-value for infinite
	bool check_infinite(const double value);
	///Delete a comment "#", leading and ending tabs/white spaces of a given string
	void clean_string(string *txt);
	///Clean all white spaces 
	string clean_white_space(string *txt);
	///Convert all char's in a string to the lower case
	string convert_string2lower_case(string *txt);
	///Convert all char's in a string to the upper case
	string convert_string2lupper_case(string *txt);
	///Convert all char's in a string to the lower case
	string convert_string2lower_case(string txt);
	///Convert all char's in a string to the upper case
	string convert_string2lupper_case(string txt);
	///Count the number of columns in a line; delimiters are tabs and whitepsaces
	int count_number_columns(string txt);
	///Get the first column, erase it from the given string and return it
	string get_column(string *line);
	///Convert a string to a boolean
	bool convert_string2boolean(const string my_string);
	///Convert a boolean to a string
	string convert_boolean2string(const bool flag);
	///Get the file path without the file name
	string get_file_path(const string path_file);
	///Get the file name without the file path
	string get_file_name(const string path_file);

	///Remove the suffix of a file name (e.g. .dat)
	string remove_suffix_file_name(const string file_name);

	///Convert the system module enumerator (_sys_system_modules) to a string 
	string convert_system_module2txt(_sys_system_modules type);
	///Convert a string  to the system module enumerator (_sys_system_modules) 
	_sys_system_modules convert_txt2system_module2(const string txt);
	///Decide if a line of file consists of a !$BEGIN (return true) or !$END-keyword (return false). No keyword found=> empty string
	bool decide_begin_end_keyword(QString *type);

    ///Set flags for table widgets
    void set_table_widget_flags(QTableWidget * widget);

	///Make output path complete
	string make_complete_output_path(const string path, const string folder, const string name);

	///Add seperator for csv-output
	void add_seperator_csv(const string sep, ofstream *output, const int number);

	///For binary output of vtk-files (paraview)
	template <typename T>
	void SwapEnd(T& var)
	{
		char* varArray = reinterpret_cast<char*>(&var);
		for (long i = 0; i < static_cast<long>(sizeof(var) / 2); i++)
			std::swap(varArray[sizeof(var) - 1 - i], varArray[i]);
	}


}
///General text labels specially for the module SYS \ingroup sys
namespace sys_label{

	//polygon interception
	///String for a polygon interception: polygon is completly inside the other (_polygon_intercept)
	const string complete_in("completly inside");
	///String for a polygon interception: polygon is completly outside the other (_polygon_intercept)
	const string complete_out("completly outside");
	///String for a polygon interception: the polygons are intercepting each other (_polygon_intercept)
	const string partly_intercept("partly intercept");
	///String for a polygon interception: polygon is inside the other with boundary contact (_polygon_intercept)
	const string in_with_boundary("inside with boundary");
	///String for a polygon interception: polygon is outside the other with boundary contact (_polygon_intercept)
	const string out_with_boundary("outside with boundary");

	///String for the variable type for creating a database table column: integer
	const string tab_col_type_int("INTEGER");
	///String for the variable type for creating a database table column: double
	const string tab_col_type_double("DOUBLE");
	///String for the variable type for creating a database table column: string
	const string tab_col_type_string("TEXT");
	///String for the variable type for creating a database table column: boolean
	const string tab_col_type_bool("BOOL");
	///String for the variable type for creating a database table column: geo-data type polygon
	const string tab_col_type_polygon("POLYGON");
	///String for the variable type for creating a database table column: geo-data type linestring
	const string tab_col_type_polyline("LINESTRING");
	///String for the variable type for creating a database table column: geo-data type point
	const string tab_col_type_point("POINT");

	///String for the module SYS
	const string str_sys("SYS");
	///String for the module SYS geometry
	const string str_geosys("GEO");
	///String for the module HYD
	const string str_hyd("HYD");
	///String for the module FPL
	const string str_fpl("FPL");
	///String for the module MADM
	const string str_madm("MADM");
	///String for the module DAM
	const string str_dam("DAM");
	///String for the module RISK
	const string str_risk("RISK");
	///String for the module COST
	const string str_cost("COST");
	///String for the module ALT
	const string str_alt("ALT");
	///String for the module GIS
	const string str_gis("GIS");
	///String for the exceptions
	const string str_excep("EXCEP");
	///String for the database name of the thread for loading sql-tables
	const string str_sys_data_tables("SYS_DATA_TABLES");
	///String for the database name of the thread for loading data to the data tree
	const string str_sys_data_tree("SYS_DATA_TREE");

	///Key word for the database name for file-input \see Data_Base
	const string key_database_name("!DATABASENAME");
	///Key word for the hostname name for file-input \see Data_Base
	const string key_hostname("!HOSTNAME");
	///Key word for the user name for file-input \see Data_Base
	const string key_user_name("!USERNAME");
	///Key word for the password for file-input \see Data_Base
	const string key_password("!PASSWORD");
	///Key word for the password to save for file-input \see Data_Base
	const string key_password_save("!PASSWORD_SAVE");
	///Key word for the password for file-input \see Data_Base
	const string key_driver_name("!DRIVER");

	///Key word for the general parameter (project name) of a project for file-input \see Sys_Project
	const string key_project_name("!NAME");
	///Key word for the general parameter (project author) of a project for file-input \see Sys_Project
	const string key_project_author("!AUTHOR");
	///Key word for the general parameter (project generation time) of a project for file-input \see Sys_Project
	const string key_project_time("!TIME");
	///Key word for the general parameter (project description) of a project for file-input \see Sys_Project
	const string key_project_description("!DESCRIPTON");
	///Key word for the general parameter (project current path) of a project for file-input \see Sys_Project
	const string key_project_current_path("!PATH");
	///Key word for the general parameter (project type) of a project for file-input \see Sys_Project
	const string key_project_type("!TYPE");
	///Key word for the general parameter (flag for saving the logfile) of a project for file-input \see Sys_Project
	const string key_project_logfile_save("!SAVE_LOG");
	///Key word for the general parameter (flag for SYS detailed output) of a project for file-input \see Sys_Project
	const string key_project_sys_out_detailed("!SYS_OUT_DETAILED");
	///Key word for the general parameter (flag for FPL detailed output) of a project for file-input \see Sys_Project
	const string key_project_fpl_out_detailed("!FPL_OUT_DETAILED");
	///Key word for the general parameter (flag for HYD detailed output) of a project for file-input \see Sys_Project
	const string key_project_hyd_out_detailed("!HYD_OUT_DETAILED");
	///Key word for the general parameter (flag for DAM detailed output) of a project for file-input \see Sys_Project
	const string key_project_dam_out_detailed("!DAM_OUT_DETAILED");
	///Key word for the general parameter (flag for RISK detailed output) of a project for file-input \see Sys_Project
	const string key_project_risk_out_detailed("!RISK_OUT_DETAILED");
	///Key word for the general parameter (flag for MADM detailed output) of a project for file-input \see Sys_Project
	const string key_project_madm_out_detailed("!MADM_OUT_DETAILED");
	///Key word for the general parameter (flag for COST detailed output) of a project for file-input \see Sys_Project
	const string key_project_cost_out_detailed("!COST_OUT_DETAILED");
	///Key word for the general parameter (flag for ALT detailed output) of a project for file-input \see Sys_Project
	const string key_project_alt_out_detailed("!ALT_OUT_DETAILED");


	///Key word for the general parameter (flag for the current risk state) of a project for file-input \see Sys_Project
	const string key_project_risk_state("!RISK_STATE");
	///Key word for the general parameter (flag for the reliability of the hydraulic results for a risk calculation) of a project for file-input \see Sys_Project, Risk_System
	const string key_project_reli_hyd_risk("!RELIABILITY_HYD_RISK");
	///Key word for the general parameter (flag for the reliability of the damage results for a risk calculation) of a project for file-input \see Sys_Project, Risk_System
	const string key_project_reli_dam_risk("!RELIABILITY_DAM_RISK");
	///Key word for the general parameter (flag for the reliability of the fpl results for a risk calculation) of a project for file-input \see Sys_Project, Risk_System
	const string key_project_reli_fpl_risk("!RELIABILITY_FPL_RISK");

	///Key word for the general parameter (flag if the nobreak risk approach is further applied) of a project for file-input \see Sys_Project, Risk_System
	const string key_project_nobreak_risk_applied("!NOBREAK_RISK_APPLIED");
	///Key word for the general parameter (flag if the catchment risk approach is further applied) of a project for file-input \see Sys_Project, Risk_System
	const string key_project_catchment_risk_applied("!CATCHMENT_RISK_APPLIED");

	///Key word for the general parameter (integer for the current area state) of a project for file-input \see Sys_Project
	const string key_project_area_state("!AREA_STATE");
	///Key word for the general parameter (integer for the current measure state) of a project for file-input \see Sys_Project
	const string key_project_measure_state("!MEASURE_STATE");
	///Key word for the general parameter (flag for a file output of the hydraulic calcultions) of a project for file-input \see Sys_Project
	const string key_project_hyd_file_out("!FILE_OUTPUT");
	///Key word for the general parameter (integer for the threads) of a project for file-input \see Sys_Project
	const string key_project_hyd_thread("!THREADS");

	///Folder name of the logfile archive
	const string folder_logfile_archiv("/archiv_logfile");
	///Folder name of the outputfile archive
	const string folder_outputfile("/output");
	///Folder name of the data
	const string folder_datafile("/data");

	///String for the project types: all moduls \see Sys_Project
	const string proj_typ_all("ALL");
	///String for the project types: damage module + hydraulic modul \see Sys_Project
	const string proj_typ_dam_hyd("DAM_HYD");
	///String for the project types: hydraulic modul just with file input \see Sys_Project
	const string proj_typ_hyd_file("HYD_FILE");
	///String for the project types: fpl modul just with file input \see Sys_Project
	const string proj_typ_fpl_file("FPL_FILE");

	///String for the project types: hydrol modul \see Hydrol_Project
	const string proj_typ_hydrol("HYDROL");

	///String for the project types: temperature model in the hyd modul \see Hyd_Project
	const string proj_typ_hyd_temp("HYD_TEMP");

	///Key word for starting a database table block for file input \see Tables
	const string table_begin("!BEGIN_TABLE");
	///Key word for ending a database table block for file input \see Tables
	const string table_end("!END_TABLE");



}
///General text labels for all moduls \ingroup sys
namespace label{
	//general
	///String for a string, which is not set
	const string not_set("not_set");

	///String for a string, which is not defined
	const string not_defined("not_defined");
	///General string label for an unknown type
	const string unknown_type("unknown_type");

	//point names
	///String for general point name for an identification of the point: Mid point of an raster element (Geo_Raster, Geo_Raster_Polygon)
	const string elem_mid("mid");
	///String for general point name for an identification of the point: Corner point of an raster element (Geo_Raster, Geo_Raster_Polygon)
	const string elem_corner("cor");
	///String for general point name for an identification of the point: Raster point of an raster element (Geo_Raster, Geo_Raster_Polygon)
	const string raster_point("ra");
	///String for general point name for an identification of the point: Interception point (_geo_interception_point, Geo_Interception_Point_List)
	const string interception_point("interception_point");


	//units

	///String for units: meter
	const string m(" [m] ");
	///String for units: meter per meter
	const string m_per_m(" [m/m] ");
	///String for units: kilometer
	const string km(" [km] ");
	///String for units: meter per second
	const string m_per_sec(" [m/s] ");
	///String for units: meter per minute
	const string m_per_min(" [m/min] ");
	///String for units: degree
	const string degree(" [degree] ");
	///String for units: cubic meter per second
	const string qm_per_sec(" [m^3/s] ");
	///String for units: cubic meter per second and meter
	const string qm_per_secm(" [m^3/(m*s)] ");
	///String for units: square meter per second
	const string sqm_per_sec(" [m^2/s] ");
	///String for units: cubic meter meter per second and square meter
	const string qm_per_secsquarem(" [m^3/(s*m^2)] ");
	///String for units: density
	const string kg_per_qm(" [kg/m^3] ");
	///String for units: hour
	const string hour(" [h] ");
	///String for units: day
	const string day(" [d] ");
	///String for units: second
	const string sec (" [s] ");
	///String for units: second per meter
	const string sec_per_m (" [s/m] ");
	///String for units: minute
	const string minute(" [min] ");
	///String for units: second per hour
	const string sec_per_hour (" [s/h] ");
	///String for units: one per second
	const string per_sec(" [1/s] ");
	///String for units: no unit
	const string no_unit(" [-] ");
	///String for units: square meter
	const string squaremeter(" [m^2] ");
	///String for units: square kilometer
	const string squarekm(" [km^2] ");
	///String for units: cubic meter
	const string cubicmeter(" [m^3] ");
	///String for units: unit for the Strickler-value
	const string kst_unit (" [m^(1/3)/s] ");
	///String for units: unit for the Manning-value
	const string n_unit (" [s/m^(1/3)] ");
	///String for units: time unit output
	const string time_unit_output(" [dd:hh:mm:ss] " );
	///String for units: stress unit for a wall break
	const string stress_unit (" [m^0.5*s] ");
	///String for units: pressure unit
	const string kn_per_sqm(" [kN/m^2] ");
	///String for units: pressure unit
	const string n_per_sqm(" [N/m^2] ");
	///String for units: wind-direct /speed
	const string wind_dir_speed(" [degree]/[m/s] ");
	///String for units: byte
	const string byte(" [B] ");
	///String for units: kilo byte
	const string kbyte(" [KB] ");
	///String for units: mega byte
	const string mbyte(" [MB] ");
	///String for units: giga byte
	const string gbyte(" [GB] ");
	///String for units: euro
	const string euro(" [monetary] ");
	///String for units: score
	const string score(" [score] ");
	///String for units: euro per square meter
	const string euro_per_sqm(" [monetary/m^2] ");
	///String for units: person per square meter
	const string person_per_sqm(" [Pe/m^2] ");
	///String for units: person r
	const string person(" [Pe] ");
	///String for units: percentage
	const string percentage(" [%] ");
	///String for units: annus
	const string annus(" [a] ");
	///String for units: 1 per annus
	const string per_annus(" [1/a] ");
	///String for units: Monetary per annus
	const string monetary_per_annus(" [monetary/a] ");
	///String for units: score per annus
	const string score_per_annus(" [score/a] ");
	///String for units: person per annus
	const string person_per_annus(" [Pe/a] ");
	///String for units: person x time per annus
	const string person_sec_per_annus(" [Pe*s/a] ");
	///String for units: qm/s per annus
	const string qm_per_sec_annus(" [m^3/(s*a)] ");
	///String for units: 1/m*s
	const string one_per_sec_m(" [1/(s*m)] ");
	///String for units: variable
	const string unit_variable(" [variable] ");

	///String for units: K
	const string kelvin(" [K] ");
	///String for units: Watt per square meter
	const string watt_per_square_m(" [W/m^2] ");


	//keywords
	///Keyword for the database table column of different object tables: areastate as identifier [-]
	const string areastate_id("AREASTATE_ID");
	///Keyword for the database table column of different object tables: measure as identifier [-]
	const string measure_id("MEASURE_ID");
	///Keyword for the database table column of different object tables: flag if the object is applied or not for the calculation [-]
	const string applied_flag("APPLIED_FLAG");
	///Keyword for the database table column of different object tables: description text [-]
	const string description("DESCRIPTION");
	///Keyword for the database table column of different object tables: link text [-]
	const string link("LINK");
	///Keyword for the database table column of database: global identifier of the data records [-]
	const string glob_id("GLOB_ID");


}

///General text labels specially for the module FPL \ingroup fpl
namespace fpl_label{

	///Keyword for the file input of the fpl section: begin of the section informations (Fpl_Section)
	const string begin_sectioninfo("!$BEGIN_SECTION");
	///Keyword for the file input of the fpl section: end of the section informations (Fpl_Section)
	const string end_sectioninfo("!$END_SECTION");
	///Keyword for the file input of the fpl section: section name (Fpl_Section)
	const string key_sec_name("!name");
	///Keyword for the file input of the fpl section: section type (Fpl_Section)
	const string key_sec_type("!type");
	///Keyword for the file input of the fpl section: similarity length (Fpl_Section)
	const string key_simil_length("!simil_length");
	///Keyword for the file input of the fpl section: similarity factor (Fpl_Section)
	const string key_simil_factor("!simil_factor");
	///Keyword for the file input of the fpl section: direction of the waterside of the section (0°:=north) (Fpl_Section)
	const string key_sec_direction("!direction");
	///Keyword for the file input of the fpl section: x-coordiante of the waterside crest point [m] (Fpl_Section)
	const string key_sec_crestwater_x("!crestwater_x");
	///Keyword for the file input of the fpl section: y-coordiante of the waterside crest point [m] (Fpl_Section)
	const string key_sec_crestwater_y("!crestwater_y");
	///Keyword for the file input of the fpl section: material zone of the waterside crest point [-] (Fpl_Section)
	const string key_sec_crestwater_zone("!crestwater_zone");
	

	///Keyword for the file input of the fpl section: begin of the hydraulic connection parameters (Fpl_Section)
	const string begin_hyd_connection("!$BEGIN_HYD_CONNECTION");
	///Keyword for the file input of the fpl section: begin of the hydraulic connection parameters  (Fpl_Section)
	const string end_hyd_connection("!$END_HYD_CONNECTION");
	///Keyword for the file input of the fpl section: river connection id (Fpl_Section)
	const string key_rv_conn_id("!rv_conn_id");
	///Keyword for the file input of the fpl section: boolean indicating the side of the river in flow direction (true:=left; flase:= right) (Fpl_Section)
	const string key_leftdirection("!leftdirection");
	///Keyword for the file input of the fpl section: riverstation, where the section starts [m] (Fpl_Section)
	const string key_station_start("!station_start");
	///Keyword for the file input of the fpl section: riverstation, where the section ends [m] (Fpl_Section)
	const string key_station_end("!station_end");
	///Keyword for the file input of the fpl section: maximum breach width [m] (Fpl_Section)
	const string key_max_breach_width("!max_breach_width");
	///Keyword for the file input of the fpl section: starting waterlevel for a breach [m] (Fpl_Section)
	const string key_start_breach_height("!start_breach_height");
	///Keyword for the file input of the fpl section: resistance factor for a continuous breaching, e.g. dikes [m/s] (Fpl_Section)
	const string key_critical_velocity("!critical_velocity");
	///Keyword for the file input of the fpl section: resistance factor for a discontinuous breaching, e.g. wall [s] (Fpl_Section)
	const string key_wall_resistance("!wall_resistance");
	///Keyword for the file input of the fpl section: abrupt opening width for a discontinuous breaching, e.g. wall [m] (Fpl_Section)
	const string key_abrupt_breach_open("!abrupt_breach_open");

	
	///Keyword for the file input of the fpl section: begin of the general data, geometry and random variables (Fpl_Section, _Fpl_Section_Type)
	const string begin_sec_general("!$BEGIN_GENERAL");
	///Keyword for the file input of the fpl section: end of the general data, geometry and random variables (Fpl_Section, _Fpl_Section_Type)
	const string end_sec_general("!$END_GENERAL");

	///Keyword for the file input of the random variable: begin of the random variable data (_Fpl_Section_Type, Random_Variables)
	const string begin_random("!$BEGIN_RANDOM");
	///Keyword for the file input of the random variable: end of the random variable data (_Fpl_Section_Type, Random_Variables)
	const string end_random("!$END_RANDOM");
	///Keyword for the file input of the random variable: name of the random variable (Random_Variables)
	const string key_name_var("!name_var");
	///Keyword for the file input of the random variable: Id of the zone (Random_Variables)
	const string key_zone_id("!zone_id");
	///Keyword for the file input of the random variable: distribution type (Random_Variables)
	const string key_dist_type("!dist_type");
	///Keyword for the file input of the random variable: default name (Random_Variables)
	const string key_default_name("!default_name");
	///Keyword for the file input of the random variable: maximum boundary (Random_Variables)
	const string key_max_boundary("!max_boundary");
	///Keyword for the file input of the random variable: minimum boundary (Random_Variables)
	const string key_min_boundary("!min_boundary");

	///Keyword for the file input of the random variable: Flag if the zone is a grass cover (Random_Variables)
	const string key_zone_grass("!grass_cover");

	///Keyword for the file input of the distribution type: begin of the distribution type data (_Fpl_Section_Type, _Distribution_Class)
	const string begin_dist_type("!$BEGIN_DIST_TYPE");
	///Keyword for the file input of the distribution type: end of the distribution type data (_Fpl_Section_Type, _Distribution_Class)
	const string end_dist_type("!$END_DIST_TYPE");
	///Keyword for the file input of the distribution type mean: mean value (_Fpl_Section_Type, Distribution_Class_Mean, Distribution_Class_Mean_Mean)
	const string key_mean_value("!mean_value");
	///Keyword for the file input of the distribution type mean: mean type (_Fpl_Section_Type, Distribution_Class_Mean, Distribution_Class_Mean_Mean)
	const string key_mean_type("!mean_type");
	///Keyword for the file input of the distribution type mean: second moment (_Fpl_Section_Type, Distribution_Class_Mean, Distribution_Class_Mean_Mean)
	const string key_second_moment("!second_moment");

	///Keyword for the file input of the distribution type mean2mean: mean type of the appended distribution (_Fpl_Section_Type, Distribution_Class_Mean_Mean)
	const string key_mean_type_2("!mean_type_2");
	///Keyword for the file input of the distribution type mean2mean: variation coefficient of the appended distribution (_Fpl_Section_Type, Distribution_Class_Mean_Mean)
	const string key_var_coeff_2("!var_coeff_2");
	///Keyword for the file input of the distribution type mean2mean: constant of the appended distribution (_Fpl_Section_Type, Distribution_Class_Mean_Mean)
	const string key_constant_2("!constant_2");

	///Keyword for the file input of the distribution type triangle: mid value (_Fpl_Section_Type, Distribution_Class_Triangle)
	const string key_mid_value("!mid");
	///Keyword for the file input of the distribution type triangle: maximum type (_Fpl_Section_Type, Distribution_Class_Triangle)
	const string key_max_value("!max");
	///Keyword for the file input of the distribution type triangle: minimum moment (_Fpl_Section_Type, Distribution_Class_Triangle)
	const string key_min_value("!min");

	///Keyword for the file input of the distribution type discrete: number of discrete values (_Fpl_Section_Type, Distribution_Class_Discret)
	const string key_no_discret("!no_discret");
	///Keyword for the file input of the distribution type: constant (_Fpl_Section_Type, _Distribution_Class)
	const string key_constant("!constant");

	///Keyword for the file input of the geometry (point): begin of the geometric point data (_Fpl_Section_Type, Fpl_Section_Points)
	const string begin_point("!$BEGIN_POINT");
	///Keyword for the file input of the geometry (point): end of the geometric point data (_Fpl_Section_Type, Fpl_Section_Points)
	const string end_point("!$END_POINT");

	///Keyword for the file input of the geometry: the waterside cubature (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_waterside_cub("!$BEGIN_WATERSIDE_CUB");
	///Keyword for the file input of the geometry: the waterside cubature (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_waterside_cub("!$END_WATERSIDE_CUB");

	///Keyword for the file input of the geometry: the foreland (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_foreland("!$BEGIN_FORELAND");
	///Keyword for the file input of the geometry: the foreland (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_foreland("!$END_FORELAND");

	///Keyword for the file input of the geometry: the landside cubature (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_landside_cub("!$BEGIN_LANDSIDE_CUB");
	///Keyword for the file input of the geometry: the landside cubature (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_landside_cub("!$END_LANDSIDE_CUB");

	///Keyword for the file input of the geometry: the hinterland (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_hinterland("!$BEGIN_HINTERLAND");
	///Keyword for the file input of the geometry: the hinterland (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_hinterland("!$END_HINTERLAND");

	///Keyword for the file input of the geometry: the material zones (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_material_zone("!$BEGIN_MATZONE");
	///Keyword for the file input of the geometry: the material zones (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_material_zone("!$END_MATZONE");

	///Keyword for the file input of the material variable zones (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_material_variable_zone("!$BEGIN_MATZONE_VARIABLES");
	///Keyword for the file input of the material variable zones (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_material_variable_zone("!$END_MATZONE_VARIABLES");

	///Keyword for the file input of the material variable zones of the waterside cubature (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_material_variable_zone_waterside("!$BEGIN_MATZONE_WATERSIDE");
	///Keyword for the file input of the material variable zones of the waterside cubature (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_material_variable_zone_waterside("!$END_MATZONE_WATERSIDE");

	///Keyword for the file input of the number of material zones(_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string key_no_zone("!no_zone");

	///Keyword for the file input of the geometry: one material zone (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_material_zone_info("!$BEGIN_ZONE_INFO");
	///Keyword for the file input of the geometry: one material zone (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_material_zone_info("!$END_ZONE_INFO");

	///Keyword for the file input of the name of a material zones(Fpl_Dike_Geo_Materialzone)
	const string key_name_zone("!zone_name");
	///Keyword for the file input of the name of a material zones type(Fpl_Dike_Geo_Materialzone)
	const string key_type_zone("!zone_type");

	///Keyword for the file input of the mechanism overflow: begin of the mechanism overflow data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_mech_overflow("!$BEGIN_MECH_OVERFLOW");
	///Keyword for the file input of the mechanism overflow: end of the mechanism overflow data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_mech_overflow("!$END_MECH_OVERFLOW");
	///Keyword for the file input of the mechanism bursting after DIN19712: begin of the mechanism bursting data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_mech_burst_din("!$BEGIN_MECH_BURST_DIN");
	///Keyword for the file input of the mechanism overflow: end of the mechanism bursting data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_mech_burst_din("!$END_MECH_BURST_DIN");
	///Keyword for the file input of the mechanism piping after Lane: begin of the mechanism piping data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_mech_pip_lane("!$BEGIN_MECH_PIP_LANE");
	///Keyword for the file input of the mechanism piping after Lane: end of the mechanism piping data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_mech_pip_lane("!$END_MECH_PIP_LANE");
	///Keyword for the file input of the mechanism piping after Sellmeijer: begin of the mechanism piping data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_mech_pip_sell("!$BEGIN_MECH_PIP_SELL");
	///Keyword for the file input of the mechanism piping after Lane: end of the mechanism piping data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_mech_pip_sell("!$END_MECH_PIP_SELL");
	///Keyword for the file input of the mechanism piping after Schmertmann: begin of the mechanism piping data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_mech_pip_schmert("!$BEGIN_MECH_PIP_SCHMERT");
	///Keyword for the file input of the mechanism piping after Schmertmann: end of the mechanism piping data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_mech_pip_schmert("!$END_MECH_PIP_SCHMERT");
	///Keyword for the file input of the mechanism wind-wave generation: begin of the mechanism wind-wave data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_mech_wind_wave("!$BEGIN_MECH_WIND");
	///Keyword for the file input of the mechanism wind-wave generation: end of the mechanism wind-wave data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_mech_wind_wave("!$END_MECH_WIND");
	///Keyword for the file input of the mechanism wind-wave generation: begin of the geometrical wind fetches (Mech_Wave_Generation, Geo_Wind_Fetch)
	const string begin_wind_fetches("!$BEGIN_FETCHES");
	///Keyword for the file input of the mechanism wind-wave generation: end of the geometrical wind fetches (Mech_Wave_Generation, Geo_Wind_Fetch)
	const string end_wind_fetches("!$END_FETCHES");
	///Keyword for the file input of the mechanism wind-wave generation: begin of the bathymetry (Mech_Wave_Generation, Geo_Wind_Fetch)
	const string begin_bathymetry("!$BEGIN_BATHY");
	///Keyword for the file input of the mechanism wind-wave generation: end of the bathymetry (Mech_Wave_Generation, Geo_Wind_Fetch)
	const string end_bathmetry("!$END_BATHY");

	///Keyword for the file input of the mechanism landside erosion: begin of the mechanism landside erosion data (_Fpl_Section_Type)
	const string begin_mech_landside_ero("!$BEGIN_MECH_LANDSIDE_ERO");
	///Keyword for the file input of the mechanism landside erosion: end of the mechanism landside erosion data (_Fpl_Section_Type)
	const string end_mech_landside_ero("!$END_MECH_LANDSIDE_ERO");

	///Keyword for the file input of the mechanism waterside dune/dike erosion: begin of the mechanism waterside erosion of dune/dike-type data (_Fpl_Section_Type, Fpl_Sec_Type_Dune, Fpl_Sec_Type_Dike)
	const string begin_mech_waterside_ero("!$BEGIN_MECH_WATERSIDE_ERO");
	///Keyword for the file input of the mechanism waterside dune/dike erosion: end of the mechanism waterside erosion of dune/dike-type data (_Fpl_Section_Type, Fpl_Sec_Type_Dune, Fpl_Sec_Type_Dike)
	const string end_mech_waterside_ero("!$END_MECH_WATERSIDE_ERO");

	///Keyword for the file input of the start mechanism waterside dike erosion: begin of the start mechanism waterside erosion of dune/dike-type data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_mech_start_waterside_ero("!$BEGIN_MECH_START_WATERSIDE_ERO");
	///Keyword for the file input of the start mechanism waterside dune/dike erosion: end of the start mechanism waterside erosion of dune/dike-type data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_mech_start_waterside_ero("!$END_MECH_START_WATERSIDE_ERO");


	///Keyword for the file input of the mechanism waterside wave impact: begin of the mechanism waterside wave impact of dike-type data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_mech_wave_impact("!$BEGIN_MECH_WAVE_IMPACT");
	///Keyword for the file input of the mechanism waterside wave impact: end of the mechanism waterside wave impact of dike-type data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_mech_wave_impact("!$END_MECH_WAVE_IMPACT");


	///Keyword for the file input of the mechanism wind-wave generation: number of fetches (Mech_Wave_Generation, Geo_Wind_Fetch)
	const string key_no_fetch("!no_fetch");
	///Keyword for the file input of the mechanism wind-wave generation: number of bathymetrie zones (Mech_Wave_Generation, Geo_Wind_Fetch)
	const string key_no_bathy("!no_bathy");
	///Keyword for the file input of geometrical data: number of points (Cubature_Waterside)
	const string key_no_points("!no_points");
	///Keyword for the file input of material data: number of material zones (Cubature_Waterside)
	const string key_no_material("!no_material");


	///Keyword for the file input of the mechanism wave-runup: begin of the mechanism wave-runup data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_mech_wave_runup("!$BEGIN_MECH_RUNUP");
	///Keyword for the file input of the mechanism wave-runup: end of the mechanism wave-runup data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_mech_wave_runup("!$END_MECH_RUNUP");

	///Keyword for the file input of the mechanism slope stability landside: begin of the mechanism slope stability data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string begin_mech_slope("!$BEGIN_MECH_SLOPE");
	///Keyword for the file input of the mechanism slope stability landside: end of the mechanism slope stability data (_Fpl_Section_Type, Fpl_Sec_Type_Dike)
	const string end_mech_slope("!$END_MECH_SLOPE");

	///Keyword for the file input of the mechanism slope stability: begin of the control parameters for the landside stability (Fpl_Mech_Slope_Stability_Dike)
	const string begin_slope_control_land("!$BEGIN_CONTROL_LAND");
	///Keyword for the file input of the mechanism slope stability: end of the control parameters for the landside stability (Fpl_Mech_Slope_Stability_Dike)
	const string end_slope_control_land("!$END_CONTROL_LAND");

	///Keyword for the file input of the mechanism slope stability: begin of the control parameters for the waterside stability (Fpl_Mech_Slope_Stability_Dike)
	const string begin_slope_control_water("!$BEGIN_CONTROL_WATER");
	///Keyword for the file input of the mechanism slope stability: end of the control parameters for the waterside stability (Fpl_Mech_Slope_Stability_Dike)
	const string end_slope_control_water("!$END_CONTROL_WATER");

	//slope stability control parameters
	///Keyword for the file input of the slope stability mechanism: x-coordinate of the origin [m] (Fpl_Mech_Slope_Stability_Dike)
	const string key_x_origin("!x_origin");
	///Keyword for the file input of the slope stability mechanism: y-coordinate of the origin [m] (Fpl_Mech_Slope_Stability_Dike)
	const string key_y_origin("!y_origin");
	///Keyword for the file input of the slope stability mechanism: delta x for the search raster [m] (Fpl_Mech_Slope_Stability_Dike)
	const string key_delta_x("!delta_x");
	///Keyword for the file input of the slope stability mechanism: delta y for the search raster [m] (Fpl_Mech_Slope_Stability_Dike)
	const string key_delta_y("!delta_y");
	///Keyword for the file input of the slope stability mechanism: number of search points in x-direction [-] (Fpl_Mech_Slope_Stability_Dike)
	const string key_no_x("!no_x");
	///Keyword for the file input of the slope stability mechanism: number of search points in y-direction [-] (Fpl_Mech_Slope_Stability_Dike)
	const string key_no_y("!no_y");
	///Keyword for the file input of the slope stability mechanism: delta h for the storing slice information depending to the waterlevel in tables [m] (Fpl_Mech_Slope_Stability_Dike)
	const string key_delta_h("!delta_h");
	///Keyword for the file input of the slope stability mechanism: delta radius for generation of the slip circle [m] (Fpl_Mech_Slope_Stability_Dike)
	const string key_delta_r("!delta_r");
	///Keyword for the file input of the slope stability mechanism: width of the slices [m] (Fpl_Mech_Slope_Stability_Dike)
	const string key_slice_width("!slice_width");


	//section types names
	///String for the fpl section type: Highground (_Fpl_Section_Type)
	const string sec_highground("highground");
	///String for the fpl section type: Dike structure (_Fpl_Section_Type)
	const string sec_dike("dike");
	///String for the fpl section type: Dune structure (_Fpl_Section_Type)
	const string sec_dune("dune");
	///String for the fpl section type: Gate structure (_Fpl_Section_Type)
	const string sec_gate("gate");
	///String for the fpl section type: Wall structure (_Fpl_Section_Type)
	const string sec_wall("wall");
	///String for the fpl section type: Mobile wall structure (_Fpl_Section_Type)
	const string sec_mobile_wall("mobile_wall");
	///String for the fpl section type: The results of the section, e.g. fragility curve, is set by the user (_Fpl_Section_Type)
	const string sec_set_by_hand("set_by_hand");
	///String for the fpl section type: All section type are included (_Fpl_Section_Type)
	const string sec_all("all_sec_type");
	///String for the fpl section type: Unknown section type (_Fpl_Section_Type)
	const string sec_unknown_type("unknown_type");

	//mechanism names
	///String for the mechanisms: Overflow (_Fpl_Section_Type)
	const string mech_overflow("overflow");
	///String for the mechanisms: Wind and wave generation (_Fpl_Section_Type)
	const string mech_wind_wave("wind_wave");
	///String for the mechanisms: Runup of waves (_Fpl_Section_Type)
	const string mech_wave_runup("wave_runup");
	///String for the mechanisms: Bursting after DIN19712 (_Fpl_Section_Type)
	const string mech_bursting_din("bursting_din");
	///String for the mechanisms: Piping after Sellmeijer (_Fpl_Section_Type)
	const string mech_piping_sell("piping_sell");
	///String for the mechanisms: Piping after Lane (_Fpl_Section_Type)
	const string mech_piping_lane("piping_lane");
	///String for the mechanisms: Piping after Schmertmann (_Fpl_Section_Type)
	const string mech_piping_schmert("piping_schmert");
	///String for the mechanisms: Slope stability  (_Fpl_Section_Type)
	const string mech_slope("slope");
	///String for the mechanisms: Slope stability landside  (_Fpl_Section_Type)
	const string mech_slope_landside("slope_landside");
	///String for the mechanisms: Slope stability waterside  (_Fpl_Section_Type)
	const string mech_slope_waterside("slope_waterside");
	///String for the mechanisms: Landside slope erosion  (_Fpl_Section_Type)
	const string mech_erosion_landside("erosion_landside");

	///String for the mechanisms: Waterside slope erosion of dune-type after van Gent (_Fpl_Section_Type)
	const string mech_erosion_waterside_gent("erosion_waterside_gent");

	///String for the mechanisms: Waterside slope erosion of dike-type (_Fpl_Section_Type)
	const string mech_erosion_waterside("erosion_waterside");
	///String for the mechanisms: Start waterside slope erosion of dike-type (_Fpl_Section_Type)
	const string mech_erosion_waterside_start("start_erosion_waterside");
	///String for the mechanisms: Waterside wave impact of dike-type (_Fpl_Section_Type)
	const string mech_wave_impact("wave_impact");

	///String for the mechanisms: all mechanisms (_Fpl_Section_Type)
	const string mech_all("all_mechanism");

	///String for the mechanisms: material zone (_Fpl_Section_Type)
	const string material_zone("mat_zone");
	///String for the mechanisms: material zone waterside (_Fpl_Section_Type)
	const string material_zone_w("mat_zone_waterside");

	//type of material zones
	///Material zone of the impermeable layer (Fpl_Dike_Var_Materialzone)
	const string zone_impermeable_layer("imperm_layer");
	///Material zone of the permeable layer (Fpl_Dike_Var_Materialzone)
	const string zone_permeable_layer("perm_layer");


	//type of the seepage calculation for the maximum waterlevel
	///String for the seepage calculation type by maximum waterlevel: endpoint is the base point land
	const string seepage_max_base_land("base_land");
	///String for the seepage calculation type by maximum waterlevel: endpoint is 1/3 of the waterlevel
	const string seepage_max_one_third("one_third_land");
	///String for the seepage calculation type by maximum waterlevel: calculation via the Kozeny formula
	const string seepage_max_kozeny("kozeny");

	//type of the seepage calculation for the minimum waterlevel
	///String for the seepage calculation type by minimum waterlevel: endpoint is the base point water
	const string seepage_min_base_land("base_water");
	///String for the seepage calculation type by minimum waterlevel: endpoint is 1/3 of the mid waterlevel
	const string seepage_min_one_third("one_third_water");


	//random variable names used in the mechanism
	///String for random-variable name for an identification of the variable: waterlevel for an Mc-simulation or a deterministic one (Mc_Sim)
	const string waterlevel("waterlevel");
	///String for random-variable name for an identification of the variable: waterlevel for a frc-calculation (Frc_Sim)
	const string waterlevel_frc("waterlevel_frc");
	///String for random-variable name for an identification of the variable: structure height 
	const string height("structure_height");
	///String for random-variable name for an identification of the variable: water density 
	const string water_density("water_density");

	///String for random-variable default name for an identification of the variable: standard values 
	const string standard("standard");

	//wave runup (dike)
	///String for random-variable name for an identification of the variable: gamma-factor for the roughness (Mech_Wave_Runup)
	const string gamma_roughness("gamma_roughness");
	///String for random-variable name for an identification of the variable: gamma-factor for the wave direction (Mech_Wave_Runup)
	const string gamma_wavedirection("gamma_wavedirection");
	///String for random-variable name for an identification of the variable: gamma-factor for berms (Mech_Wave_Runup)
	const string gamma_berms("gamma_berm");
	///String for random-variable name for an identification of the variable: height of the wave run-up (Mech_Wave_Runup)
	const string height_runup("h_runup");
	///String for random-variable name for an identification of the variable: discharge of the wave run-up (Mech_Wave_Runup)
	const string q_waveoverflow("q_waveoverflow");
	///String for random-variable name for an identification of the variable: average gradient of the waterside slope (Mech_Wave_Runup)
	const string mid_gradient_waterside("mid_gradient_waterside");
	///String for an identification of a roughness coefficient for a grass-material as dike cover (Mech_Wave_Runup)
	//const string material_roughness_grass("grass");

	//overflow (dike)
	///String for random-variable name for an identification of the variable: discharge of the overflow (Fpl_Mech_Overflow_Dike)
	const string q_overflow("q_overflow");
	///String for random-variable name for an identification of the variable: poleni factor for the overflow (Fpl_Mech_Overflow_Dike)
	const string mue_factor("mue_factor");
	///String for random-variable name for an identification of the variable: width of the dike crest (Fpl_Mech_Overflow_Dike)
	const string factor_crestwidth("factor_crestwidth");
	///String for random-variable name for an identification of the variable: factor for the flow contraction (Fpl_Mech_Overflow_Dike)
	const string factor_flowcontraction("factor_flowcontraction");
	///String for random-variable name for an identification of the variable: factor for the waterside slope (Fpl_Mech_Overflow_Dike)
	const string factor_watersideslope("factor_watersideslope");
	///String for random-variable name for an identification of the variable: factor for the lanside slope (Fpl_Mech_Overflow_Dike)
	const string factor_landsideslope("factor_landsideslope");
	
	//wave generation (all)
	///String for random-variable name for an identification of the variable: wave height (Mech_Wave_Generation)
	const string wave_height("wave_height");
	///String for random-variable name for an identification of the variable: wave length (Mech_Wave_Generation)
	const string wave_length("wave_length");
	///String for random-variable name for an identification of the variable: wave period (Mech_Wave_Generation)
	const string wave_period("wave_period");
	///String for random-variable name for an identification of the variable: wave setup (Mech_Wave_Generation)
	const string wave_setup("wave_setup");
	///String for random-variable name for an identification of the variable: average water depth (Mech_Wave_Generation)
	const string mean_depth("mid_depth");
	///String for random-variable name for an identification of the variable: wind direction and corresponding speed (Mech_Wave_Generation)
	const string wind_speed_direction("wind_speed_direction");
	///String for random-variable name for an identification of the variable: wave direction (Mech_Wave_Generation)
	const string wave_direction("wave_direction");
	///String for random-variable name for an identification of the variable: bathymetrie (Mech_Wave_Generation)
	const string bathymetrie("bathymetrie");

	///String for random-variable name for an identification of the variable: m-factor for the gradient of the correlation line between wave height and wave period (wp=m*(wh)^e+b) in case of user defined parameters (Mech_Wave_Generation)
	const string m_corr_factor("m_corr_factor");
	///String for random-variable name for an identification of the variable: b-factor for the y-interception of the correlation line between wave height and wave period (wp=m*(wh)^e+b) in case of user defined parameters (Mech_Wave_Generation)
	const string b_corr_factor("b_corr_add");
	///String for random-variable name for an identification of the variable: e-factor for the exponent of the correlation line between wave height and wave period (wp=m*(wh)^e+b) in case of user defined parameters (wp=m*(wh)^e+b) in case of user defined parameters (Mech_Wave_Generation)
	const string e_corr_factor("e_corr_exp");

	///String for random-variable name for an identification of the variable: Goda factor for a probabilistic modelling of the wave reduction in shallow waters in case of user defined parameters (Mech_Wave_Generation)
	const string goda_fac("goda_height");


	//wave impact artificial revetments
	///String for random-variable name for an identification of the variable: stability factor f of the artificial revetment (Fpl_Cub_Waterside_Materialzone)
	const string stability_f("stability_f");
	///String for random-variable name for an identification of the variable: exponent for the breaker factor for artificial revetment (Fpl_Cub_Waterside_Materialzone)
	const string exponent_x("exponent_x");
	///String for random-variable name for an identification of the variable: artificial/grass cover thickness (Fpl_Cub_Waterside_Materialzone)
	const string cover_thickness("cover_d");
	///String for random-variable name for an identification of the variable: bulk density of the artificial cover (Fpl_Cub_Waterside_Materialzone)
	const string bulk_density("density");
	///String for random-variable name for an identification of the variable: porosity of the artificial cover (Fpl_Cub_Dike_Waterside)
	const string porosity_cover("porosity_cover");
	///String for random-variable name for an identification of the variable: friction angle of the artificial cover (Fpl_Cub_Dike_Waterside)
	const string friction_angle_cover("friction_angle_cover");
	///String for random-variable name for an identification of the variable: total stability of the artificial cover (Fpl_Cub_Dike_Waterside)
	const string tot_stab_cover("total_stability_cover");
	///String for random-variable name for an identification of the variable: factor of the under layer quality under the artificial cover/root zone (Fpl_Cub_Dike_Waterside)
	const string under_layer_quality("under_layer_quality");
	///String for random-variable name for an identification of the variable: the under layer thickness under the artificial cover (Fpl_Cub_Dike_Waterside)
	const string under_layer_thickness("under_layer_d");

	//wave impact and erosion for grass cover
	///String for random-variable name for an identification of the variable: factor of the grass cover quality (Fpl_Cub_Waterside_Materialzone)
	const string quality_grass("grass_quality");
	///String for random-variable name for an identification of the variable: crack thickness (Fpl_Cub_Waterside_Materialzone)
	const string crack_thickness("crack_d");
	///String for random-variable name for an identification of the variable: undrained cohesion (Fpl_Cub_Waterside_Materialzone)
	const string cohesion_undrained("und_cohesion");	
	///String for random-variable name for an identification of the variable: root cohesion (Fpl_Cub_Waterside_Materialzone)
	const string cohesion_root("root_cohesion");
	///String for random-variable name for an identification of the variable: root zone thickness (Fpl_Cub_Waterside_Materialzone)
	const string root_zone_thickness("root_zone_d");
	
	


	//bursting DIN19712 (dike)
	///String for random-variable name for an identification of the variable: variable leakage length [m] (Fpl_Mech_Bursting_Din_Dike)
	const string var_leak_length("variable_leak_length");
	///String for random-variable name for an identification of the variable: hyraulic head loss [m/m] (Fpl_Mech_Bursting_Din_Dike)
	const string hyd_head_loss("hydraulic_head_loss");
	///String for random-variable name for an identification of the variable: size impermeable layer [m] (Fpl_Mech_Bursting_Din_Dike)
	const string size_imperm_lay("size_imperm_layer");
	///String for random-variable name for an identification of the variable: density impermeable layer [kg/m³] (Fpl_Mech_Bursting_Din_Dike)
	const string dens_imperm_lay("density_imperm_layer");
	///String for random-variable name for an identification of the variable: resistance of the bursting mechanism after DIN19712 [kN/m²] (Fpl_Mech_Bursting_Din_Dike)
	const string res_burst_din("res_bursting_din");
	///String for random-variable name for an identification of the variable: stress of the bursting mechanism after DIN19712 [kN/m²] (Fpl_Mech_Bursting_Din_Dike)
	const string stress_burst_din("stress_bursting_din");


	//piping Lane (dike)
	///String for random-variable name for an identification of the variable: Lane factor [-] (Fpl_Mech_Piping_Lane_Dike)
	const string lane_factor("lane_factor");
	///String for random-variable name for an identification of the variable: resistance of the piping mechanism after Lane [-] (Fpl_Mech_Piping_Lane_Dike)
	const string res_piping_lane("res_piping_lane");


	//piping Sellmeijer (dike)
	///String for random-variable name for an identification of the variable: size permeable layer [m] (Fpl_Mech_Piping_Sell_Dike)
	const string size_perm_lay("size_perm_layer");
	///String for random-variable name for an identification of the variable: density permeable layer [kg/m³] (Fpl_Mech_Piping_Sell_Dike)
	const string dens_perm_lay("density_perm_layer");
	///String for random-variable name for an identification of the variable: dragforce factor [-] (Fpl_Mech_Piping_Sell_Dike)
	const string dragforce_fac("dragforce_factor");
	///String for random-variable name for an identification of the variable: rolling resistance angle [°] (Fpl_Mech_Piping_Sell_Dike)
	const string roll_res_angle("rolling_resistance_angle");
	///String for random-variable name for an identification of the variable: kf-value permeable layer [m/s] (Fpl_Mech_Piping_Sell_Dike)
	const string kf_perm_lay("kf_perm_layer");
	///String for random-variable name for an identification of the variable: Grain distribution size 70% (d_70) [m] (Fpl_Mech_Piping_Sell_Dike)
	const string d_70("d_70");
	///String for random-variable name for an identification of the variable: resistance of the piping mechanism after Sellmeijer (Fpl_Mech_Piping_Sell_Dike)
	const string res_piping_sell("res_piping_sell");

	//piping after Schmertmann (dike)
	///String for random-variable name for an identification of the variable: resistance of the piping mechanism after Schmertmann (Fpl_Mech_Piping_Schmertmann_Dike)
	const string res_piping_schmert("res_piping_schmert");
	///String for random-variable name for an identification of the variable: Grain distribution size 60% (d_60) [m] (Fpl_Mech_Piping_Schmertmann_Dike)
	const string d_60("d_60");
	///String for random-variable name for an identification of the variable: Grain distribution size 10% (d_10) [m] (Fpl_Mech_Piping_Schmertmann_Dike)
	const string d_10("d_10");



	//slope stability landside
	///String for random-variable name for an identification of the variable: macro stability of the landside slope calculation after Krey (Fpl_Mech_Slope_Stability_Dike)
	const string stab_slope_land_macro("stab_slope_land_macro");
	///String for random-variable name for an identification of the variable: micro stability of the landside slope calculation after Krey (Fpl_Mech_Slope_Stability_Dike)
	const string stab_slope_land_micro("stab_slope_land_micro");

	//slope stability waterside
	///String for random-variable name for an identification of the variable: macro stability of the waterside slope calculation after Krey (Fpl_Mech_Slope_Stability_Dike)
	const string stab_slope_water_macro("stab_slope_water_macro");
	///String for random-variable name for an identification of the variable: micro stability of the waterside slope calculation after Krey (Fpl_Mech_Slope_Stability_Dike)
	const string stab_slope_water_micro("stab_slope_water_micro");

	//landside slope resistance
	///String for random-variable name for an identification of the variable: resistance of the landside slope erosion (Fpl_Mech_Landside_Erosion)
	const string res_landside_discharge("res_landside_discharge");
	///String for random-variable name for an identification of the variable: stress of the landside slope erosion (Fpl_Mech_Landside_Erosion)
	const string stress_landside_discharge("stress_landside_discharge");
	///String for random-variable name for an identification of the variable: critical discharge of the landside slope erosion (Fpl_Mech_Landside_Erosion)
	const string crit_landside_discharge("crit_landside_discharge");

	///String for random-variable name for an identification of the variable: resistance of the waterside wave impact (Fpl_Mech_Wave_Impact_Waterside_Dike)
	const string res_wave_impact("res_wave_impact");
	///String for random-variable name for an identification of the variable: stress of the waterside wave impact (Fpl_Mech_Wave_Impact_Waterside_Dike)
	const string stress_wave_impact("stress_wave_impact");

	///String for random-variable name for an identification of the variable: resistance of the start of waterside slope erosion for a dike type (Fpl_Mech_Waterside_Erosion_Start)
	const string res_waterside_erosion_start("res_start_waterside_erosion");
	///String for random-variable name for an identification of the variable: stress of the start of waterside slope erosion for a dike type (Fpl_Mech_Waterside_Erosion_Start)
	const string stress_waterside_erosion_start("stress_start_waterside_erosion");


	///String for random-variable name for an identification of the variable: duration of wave attack (Fpl_Mech_Waterside_Erosion)
	const string duration_wave_attack("duration_wave_attack");

	//waterside erosion dune
	///String for random-variable name for an identification of the variable: resistance of the waterside slope erosion for a dune/dike type (Fpl_Mech_Erosion_Dune_van_Gent, Fpl_Mech_Waterside_Erosion)
	const string res_waterside_erosion("res_waterside_erosion");
	///String for random-variable name for an identification of the variable: stress of the waterside slope erosion for a dune/dike type (Fpl_Mech_Erosion_Dune_van_Gent, Fpl_Mech_Waterside_Erosion)
	const string stress_waterside_erosion("stress_waterside_erosion");
	///String for random-variable name for an identification of the variable: critical crest width for the waterside slope erosion for a dune type (Fpl_Mech_Erosion_Dune_van_Gent)
	const string crit_crest_width("crit_crest_width");
	///String for random-variable name for an identification of the variable: mean particle diameter for the waterside slope erosion for a dune type (Fpl_Mech_Erosion_Dune_van_Gent)
	const string dia_mean("mean_diameter");
	///String for random-variable name for an identification of the variable: roundness factor of the particle for the waterside slope erosion for a dune type (Fpl_Mech_Erosion_Dune_van_Gent)
	const string roundness_fac("roundness");
	///String for random-variable name for an identification of the variable: shape factor of the particle for the waterside slope erosion for a dune type (Fpl_Mech_Erosion_Dune_van_Gent)
	const string shape_fac("shape");
	///String for random-variable name for an identification of the variable: settling velocity of the particle for the waterside slope erosion for a dune type (Fpl_Mech_Erosion_Dune_van_Gent)
	const string settling_v("settling_v");


	//general slope
	///String for random-variable name for an identification of the variable of the slope stability after Krey: porosity [-] (Fpl_Mech_Slope_Stability_Dike, Fpl_Dike_Material_Slope)
	const string porosity("porosity");
	///String for random-variable name for an identification of the variable of the slope stability after Krey: grain density [kg/m³] (Fpl_Mech_Slope_Stability_Dike, Fpl_Dike_Material_Slope, Fpl_Mech_Erosion_Dune_van_Gent)
	const string grain_density("grain_density");
	///String for random-variable name for an identification of the variable of the slope stability after Krey: water content [-] (Fpl_Mech_Slope_Stability_Dike, Fpl_Dike_Material_Slope)
	const string water_content("water_content");

	///String for random-variable name for an identification of the variable of the slope stability after Krey: angle of friction [°] (Fpl_Mech_Slope_Stability_Dike, Fpl_Dike_Material_Slope)
	const string friction_angle("friction_angle");
	///String for random-variable name for an identification of the variable of the slope stability after Krey: cohesion [N/m²] (Fpl_Mech_Slope_Stability_Dike, Fpl_Dike_Material_Slope)
	const string cohesion("cohesion");


	//point names
	///String for fpl-point name for an identification of the point: points of the waterside cubature 
	const string cub_waterside("cub_waterside");
	///String for fpl-point name for an identification of the point: points of the landside cubature 
	const string cub_landside("cub_landside");
	///String for fpl-point name for an identification of the point: points of the hinterland of a structure 
	const string hinterland("hinterland");
	///String for fpl-point name for an identification of the point: points of the foreland of a structure 
	const string foreland("foreland");

	///String for fpl-point name for an identification of the point: base-point of the waterside 
	const string base_water("base_water");
	///String for fpl-point name for an identification of the point: crest-point of the waterside
	const string crest_water("crest_water");
	///String for fpl-point name for an identification of the point: crest-point of the landside
	const string crest_land("crest_land");
	///String for fpl-point name for an identification of the point: base-point of the landside
	const string base_land("base_land");
	///String for fpl-point name for an identification of the point: fetch points (polar)
	const string fetches("fetches");



	//region names
	///String for the region, where a geometrical object belongs to: general (_Fpl_Geometrie)
	const string region_general("general");
	///String for the region, where a geometrical object belongs to: suroundings of the section (e.g. fetches) (_Fpl_Geometrie)
	const string region_surround("surround");
	///String for the region, where a geometrical object belongs to: cubature of the section (e.g. waterside) (_Fpl_Geometrie)
	const string region_cubature("cubature");
	///String for the region, where a geometrical object belongs to: underground of the section (e.g. fetches) (_Fpl_Geometrie)
	const string region_underground("underground");
	///String for the region, where a geometrical object belongs to: body of the section (e.g. fetches)
	const string region_body("body");

	//probabilistic simulation types
	///String for the probabilistic simulation type, which is applied: Fragility curve (FRC) calculation (Mc_Sim)
	const string frc_simulation("FRC");
	///String for the probabilistic simulation type, which is applied: Monte-Carlo (MC) simulation (Mc_Sim)
	const string mc_simulation("MC");
	///String for the probabilistic simulation type, which is applied: Deterministic (DETERM) simulation (Mc_Sim)
	const string determ_simulation("DETERM");

	//types of random generators
	///String for the type of random generator, which is applied: Ranmar generator (Fpl_Random_Generator_Ranmar)
	const string generator_ranmar("RANMAR");
	///String for the type of random generator, which is applied: Ziggurat generator (Fpl_Random_Generator_Zikurat)
	const string generator_zikurat("ZIGGURAT");
	///String for the type of random generator, which is applied: Mother of all generator (Fpl_Random_Generator_MofAll)
	const string generator_MofAll("MOTHER_OF_ALL");
	///String for the type of random generator, which is applied: Sfmt generator (Fpl_Random_Generator_Sfmt)
	const string generator_sfmt("SFMT");
	

	//distribution types
	///String for the distribution types: mean distribution type  (Distribution_Class_Mean)
	const string dist_mean("mean");
	///String for the distribution types: triangle distribution type  (Distribution_Class_Triangle)
	const string dist_triangle("triangle");
	///String for the distribution types: discrete distribution type  (Distribution_Class_Discret)
	const string dist_discret("discret");
	///String for the distribution types: mean distribution type to mean distribution type  (Distribution_Class_Mean_Mean)
	const string dist_mean2mean("mean2mean");
	///String for the distribution types: discrete distribution type to mean distribution type  (Distribution_Class_Discret_Mean)
	const string dist_discret2mean("discret2mean");

	//Distribution type of the mean type distribution
	///String for the distribution types of the mean type distribution: determininistic (Distribution_Class_Mean)
	const string dist_mean_determ("deterministic");
	///String for the distribution types of the mean type distribution: normal (Distribution_Class_Mean)
	const string dist_mean_normal("normal");
	///String for the distribution types of the mean type distribution: log-normal (Distribution_Class_Mean)
	const string dist_mean_log_normal("log_normal");
	///String for the distribution types of the mean type distribution: uniform (Distribution_Class_Mean)
	const string dist_mean_uniform("uniform");
	///String for the distribution types of the mean type distribution: exponentiel (Distribution_Class_Mean)
	const string dist_mean_exponentiel("exponentiel");
	///String for the distribution types of the mean type distribution: gumbel (Distribution_Class_Mean)
	const string dist_mean_gumbel("gumbel");
	///String for the distribution types of the mean type distribution: weibull (Distribution_Class_Mean)
	const string dist_mean_weibull("weibull");

	///String for a user defined random variable (Random_Variables)
	const string given("user_defined");

	///Default material names for the default values of the random variables: material water (Random_Variables)
	const string def_water("mat_water");
	///Default material names for the default values of the random variables: Clay (Random_Variables)
	const string def_clay("mat_clay");
	///Default material names for the default values of the random variables: Silt (Random_Variables)
	const string def_silt("mat_silt");
	///Default material names for the default values of the random variables: Fine sand (Random_Variables)
	const string def_fine_sand("mat_fine_sand");
	///Default material names for the default values of the random variables: Medium sand (Random_Variables)
	const string def_medium_sand("mat_medium_sand");
	///Default material names for the default values of the random variables: Coarse sand (Random_Variables)
	const string def_coarse_sand("mat_coarse_sand");
	///Default material names for the default values of the random variables: Fine gravel (Random_Variables)
	const string def_fine_gravel("mat_fine_gravel");
	///Default material names for the default values of the random variables: Medium gravel (Random_Variables)
	const string def_medium_gravel("mat_medium_gravel");
	///Default material names for the default values of the random variables: Coarse gravel (Random_Variables)
	const string def_coarse_gravel("mat_coarse_gravel");
	///Default material names for the default values of the random variables: Boulders (Random_Variables)
	const string def_boulders("mat_boulders");

	///Default material names for the default values of the random variables: weak erosion resistance of the landside slope (Random_Variables)
	const string def_weak_ero_res("weak_erosion_resistance");
	///Default material names for the default values of the random variables: medium erosion resistance of the landside slope (Random_Variables)
	const string def_medium_ero_res("medium_erosion_resistance");
	///Default material names for the default values of the random variables: strong erosion resistance of the landside slope (Random_Variables)
	const string def_strong_ero_res("strong_erosion_resistance");
	///Default material names for the default values of the random variables: very strong erosion resistance of the landside slope (Random_Variables)
	const string def_very_strong_ero_res("very_strong_erosion_resistance");


	///Keyword for the database table column of database: global identifier of the data records [-]
	const string glob_id("GLOB_ID");

	///Keyword for the database table name of the section data (Fpl_Section)
	const string tab_fpl_section("FPL_SECTION");
	///Keyword for the database table column of the section data: Identifier of the section [-] (Fpl_Section)
	const string section_id("SECTION_ID");
	///Keyword for the database table column of the section data: Name of the section [-] (Fpl_Section)
	const string section_name("NAME");
	///Keyword for the database table column of the section data: Type of the section [-] (Fpl_Section)
	const string section_type("SECTION_TYPE");
	///Keyword for the database table column of the section data: Length of the section [m] (Fpl_Section)
	const string section_length("LENGTH");
	///Keyword for the database table column of the section data: Similarity length of the section [m] (Fpl_Section)
	const string section_similarity_length("SIMIL_LENGTH");
	///Keyword for the database table column of the section data: Similarity factor of the section [-] (Fpl_Section)
	const string section_similarity_factor("SIMIL_FACTOR");
	///Keyword for the database table column of the section data: Structure height of the section [m] (Fpl_Section)
	const string structure_height("HEIGHT");
	///Keyword for the database table column of the section data: Direction of the section [°] (Fpl_Section)
	const string section_direction("DIRECTION");
	///Keyword for the database table column of the section data: Connected river id of the section [-] (Fpl_Section)
	const string section_conn_rv_id("CONN_RV_ID");
	///Keyword for the database table column of the section data: River bank of the connected river of the section [-] (Fpl_Section)
	const string section_rv_bank_flag("RV_BANK");
	///Keyword for the database table column of the section data: Riverstation start of the connected river of the section [m] (Fpl_Section)
	const string section_station_start("STATION_START");
	///Keyword for the database table column of the section data: Riverstation end of the connected river of the section [m] (Fpl_Section)
	const string section_station_end("STATION_END");
	///Keyword for the database table column of the section data: Maximum breach width [m] (Fpl_Section)
	const string section_max_breach("MAXIMUM_BREACH_WIDTH");
	///Keyword for the database table column of the section data: Starting breach waterlevel related to to the base-point [m] (Fpl_Section)
	const string section_start_breach_h("START_BREACH_HEIGHT");
	///Keyword for the database table column of the section data: Flag if the section fails abruptly [m] (Fpl_Section)
	const string section_abrupt_breach("ABRUPT_BREACH");
	///Keyword for the database table column of the section data: Breach resistance parameter [m/s (continuous) or m^0.5/s (abrupt)] (Fpl_Section, _Hyd_River_Profile)
	const string section_resistance_breach("BREACH_RESISTANCE");
	///Keyword for the database table column of the section data: Abrupt breach opening width [m], just required for an abrupt opening like wall breaching (Fpl_Section, _Hyd_River_Profile)
	const string section_abrupt_open_width("ABRUPT_OPEN_WIDTH");

	///Keyword for the database table column of the section data: geometric line of the section [-] (Fpl_Section)
	const string section_line("GEO_LINE");

	///Keyword for the database table name of the fpl-section gemetric point data (_Fpl_Geometrie)
	const string tab_fpl_geo_point("FPL_GEO_POINT");
	///Keyword for the database table column of the fpl-section gemetric point data: Name of the point [-] (_Fpl_Geometrie)
	const string point_name("POINT_NAME");
	///Keyword for the database table column of the fpl-section gemetric point data: x-coordinate (cartesian) or length (polar) [-] (_Fpl_Geometrie)
	const string point_x_coor("X_LENGTH_COOR");
	///Keyword for the database table column of the fpl-section gemetric point data: y-coordinate (cartesian) or angle (polar) [-] (_Fpl_Geometrie)
	const string point_y_coor("Y_ANGLE_COOR");
	///Keyword for the database table column of the fpl-section gemetric point data: region of the fpl-section where the points belongs to [-] (_Fpl_Geometrie)
	const string point_region("REGION");


	///Keyword for the database table name of the mechanisms applied for a section type (_Fpl_Section_Type)
	const string tab_fpl_mechanism("FPL_MECHANISM");
	///Keyword for the database table column of the applied mechanisms: Name of the mechnisms [-] (_Fpl_Section_Type)
	const string mecha_name("MECHANISM_NAME");
	///Keyword for the database table column of the applied mechanisms: Name of the section type, where the mechánism is applied [-] (_Fpl_Section_Type)
	const string mecha_sec_type("SECTION_TYPE");
	///Keyword for the database table column of the applied mechanisms: Flag if the mechanism is applied [-] (_Fpl_Section_Type)
	const string mecha_applied("APPLIED");
	///Keyword for the database table column of the applied mechanisms: Flag if the mechanism is applied in the fault-tree calculation [-] (_Fpl_Section_Type)
	const string mecha_applied_tree("APPLIED_TREE");
	

	///Keyword for the database table name of the control parameter of probability calculation (Mc_Sim, Frc_Sim)
	const string tab_fpl_control("FPL_CONTROL_PARAM");
	///Keyword for the database table column of the control parameter of probability calculation: Name of the control parameter [-] (Mc_Sim, Frc_Sim)
	const string control_name("NAME");
	///Keyword for the database table column of the control parameter of probability calculation: Value of the control parameter [divers] (Mc_Sim, Frc_Sim)
	const string control_value("VALUE");

	///Keyword for the database table name of the reliability results (Frc_Sim, Mc_Sim)
	const string tab_fpl_reliability("FPL_RELIABILITY");
	///Keyword for the database table column of the reliability results: Flag for a MC-simulation (true) or FRC-simulation (false) [-] (Frc_Sim, Mc_Sim)
	const string reli_mc_frc_flag("MC_FRC");
	///Keyword for the database table column of the reliability results: Waterlevel [m] (Frc_Sim, Mc_Sim)
	const string reli_waterlevel("WATERLEVEL");
	///Keyword for the database table column of the reliability results: Probability [-] (Frc_Sim, Mc_Sim)
	const string reli_probability("PROBABILITY");
	///Keyword for the database table column of the reliability results: Lower boundary [-] (Frc_Sim, Mc_Sim)
	const string reli_low_bound("LOWER_BOUND");
	///Keyword for the database table column of the reliability results: Lower boundary [-] (Frc_Sim, Mc_Sim)
	const string reli_up_bound("UPPER_BOUND");
	///Keyword for the database table column of the reliability results: Computation time [sec] (Frc_Sim, Mc_Sim)
	const string reli_comp_time("COMPUTATION_TIME");
	///Keyword for the database table column of the reliability results: Number of MC-simulations [-] (Frc_Sim, Mc_Sim)
	const string reli_number_mc("NUMBER_MC_SIM");

	///Keyword for the database table name of the dividing intervals of the frc-curve (Fpl_Frc_Curve)
	const string tab_fpl_frc_interval("FPL_FRC_INTERVAL");
	///Keyword for the database table column of the frc-curve intervals: Identifier of the interval [-] (Fpl_Frc_Curve)
	const string id_frc_interval("ID_INTERVAL");
	///Keyword for the database table column of the frc-curve intervals: most likely break height [m] (Fpl_Frc_Curve)
	const string break_height("BREAK_HEIGHT");

	///Keyword for the database table column of the frc-curve intervals: probability of the lower boundary of the interval [-] (Fpl_Frc_Curve)
	const string interval_low_prob("LOWER_PROBABILITY");
	///Keyword for the database table column of the frc-curve intervals: probability of the upper boundary of the interval [-] (Fpl_Frc_Curve)
	const string interval_up_prob("UPPER_PROBABILITY");
	///Keyword for the database table column of the frc-curve intervals: waterlevel of the lower boundary of the interval [m] (Fpl_Frc_Curve)
	const string interval_low_h("LOWER_WATERLEVEL");
	///Keyword for the database table column of the frc-curve intervals: waterlevel of the upper boundary of the interval [m] (Fpl_Frc_Curve)
	const string interval_up_h("UPPER_WATERLEVEL");



	///Keyword for the database table name of the results of the mechanism of a highground section (Fpl_Sec_Type_Highground)
	const string tab_fpl_highground_result("FPL_HIGHGROUND_RESULT");
	///Keyword for the database table column of the results of the mechanism: Simulation type [-] (_Fpl_Section_Type, _fpl_simulation_type)
	const string simulation_type("SIMULATION_TYPE");

	///Keyword for the database table name of the results of the mechanism of a dike section (Fpl_Sec_Type_Dike)
	const string tab_fpl_dike_result("FPL_DIKE_RESULT");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike/dune section: single occurence probability of the overflow mechanism  [-] (Fpl_Sec_Type_Dike, Fpl_Sec_Type_Dune)
	const string single_prob_overflow("SINGLE_PROB_OVERFLOW");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike/dune section: fault-tree occurence probability of the overflow mechanism  [-] (Fpl_Sec_Type_Dike, Fpl_Sec_Type_Dune)
	const string tree_prob_overflow("TREE_PROB_OVERFLOW");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike/dune section: single occurence probability of the wave runup mechanism  [-] (Fpl_Sec_Type_Dike, Fpl_Sec_Type_Dune)
	const string single_prob_runup("SINGLE_PROB_RUNUP");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike/dune section: fault-tree occurence probability of the wave runup mechanism  [-] (Fpl_Sec_Type_Dike, Fpl_Sec_Type_Dune)
	const string tree_prob_runup("TREE_PROB_RUNUP");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: single occurence probability of the bursting mechanism  [-] (Fpl_Sec_Type_Dike)
	const string single_prob_bursting("SINGLE_PROB_BURSTING");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: single occurence probability of the piping after Sellmeijer mechanism  [-] (Fpl_Sec_Type_Dike)
	const string single_prob_pip_sell("SINGLE_PROB_PIP_SELL");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: fault-tree occurence probability of the piping after Sellmeijer mechanism  [-] (Fpl_Sec_Type_Dike)
	const string tree_prob_pip_sell("TREE_PROB_PIP_SELL");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: single occurence probability of the piping after Lane mechanism  [-] (Fpl_Sec_Type_Dike)
	const string single_prob_pip_lane("SINGLE_PROB_PIP_LANE");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: fault-tree occurence probability of the piping after Lane mechanism  [-] (Fpl_Sec_Type_Dike)
	const string tree_prob_pip_lane("TREE_PROB_PIP_LANE");

	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: single occurence probability of the piping after the Schmertmann mechanism  [-] (Fpl_Sec_Type_Dike)
	const string single_prob_pip_schmert("SINGLE_PROB_PIP_SCHMERT");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: fault-tree occurence probability of the piping after the Schmertmann mechanism  [-] (Fpl_Sec_Type_Dike)
	const string tree_prob_pip_schmert("TREE_PROB_PIP_SCHMERT");

	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: single occurence probability of the slope landside macro stability mechanism  [-] (Fpl_Sec_Type_Dike)
	const string single_prob_slope_land_macro("SINGLE_PROB_SLOPE_LAND_MACRO");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: fault-tree occurence probability of the slope landside macro stability mechanism  [-] (Fpl_Sec_Type_Dike)
	const string tree_prob_slope_land_macro("TREE_PROB_SLOPE_LAND_MACRO");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: single occurence probability of the slope landside micro stability mechanism  [-] (Fpl_Sec_Type_Dike)
	const string single_prob_slope_land_micro("SINGLE_PROB_SLOPE_LAND_MICRO");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: fault-tree occurence probability of the slope landside micro stability mechanism  [-] (Fpl_Sec_Type_Dike)
	const string tree_prob_slope_land_micro("TREE_PROB_SLOPE_LAND_MICRO");

	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: single occurence probability of the slope waterside macro stability mechanism  [-] (Fpl_Sec_Type_Dike)
	const string single_prob_slope_water_macro("SINGLE_PROB_SLOPE_WATER_MACRO");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: fault-tree occurence probability of the slope waterside macro stability mechanism  [-] (Fpl_Sec_Type_Dike)
	const string tree_prob_slope_water_macro("TREE_PROB_SLOPE_WATER_MACRO");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: single occurence probability of the slope waterside micro stability mechanism  [-] (Fpl_Sec_Type_Dike)
	const string single_prob_slope_water_micro("SINGLE_PROB_SLOPE_WATER_MICRO");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: fault-tree occurence probability of the slope waterside micro stability mechanism  [-] (Fpl_Sec_Type_Dike)
	const string tree_prob_slope_water_micro("TREE_PROB_SLOPE_WATER_MICRO");

	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: single occurence probability of the seepage mechanism  [-] (Fpl_Sec_Type_Dike)
	const string single_prob_seepage("SINGLE_PROB_SEEPAGE");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: fault-tree occurence probability of the seepage mechanism  [-] (Fpl_Sec_Type_Dike)
	const string tree_prob_seepage("TREE_PROB_SEEPAGE");

	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: single occurence probability of the wave impact mechanism  [-] (Fpl_Sec_Type_Dike)
	const string single_prob_wave_impact("SINGLE_PROB_WAVE_IMP");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: fault-tree occurence probability of the wave impact mechanism  [-] (Fpl_Sec_Type_Dike)
	const string tree_prob_wave_impact("TREE_PROB_WAVE_IMP");

	///Keyword for the database table name of the results of the mechanism of a dune section (Fpl_Sec_Type_Dune)
	const string tab_fpl_dune_result("FPL_DUNE_RESULT");

	///Keyword for the database table column of the probabilistic results of the mechanisms of a dike section: single occurence probability of the starting of the waterside erosion mechanism  [-] (Fpl_Sec_Type_Dike)
	const string single_prob_water_ero_start("SINGLE_PROB_START_WATER_ERO");

	///Keyword for the database table column of the probabilistic results of the mechanisms of a dune/dike section: single occurence probability of the waterside erosion mechanism  [-] (Fpl_Sec_Type_Dune, Fpl_Sec_Type_Dike)
	const string single_prob_water_ero("SINGLE_PROB_WATER_ERO");
	///Keyword for the database table column of the probabilistic results of the mechanisms of a dune/dike section: fault-tree occurence probability of the wate side erosion mechanism  [-] (Fpl_Sec_Type_Dune, Fpl_Sec_Type_Dike)
	const string tree_prob_water_ero("TREE_PROB_WATER_ERO");

	///Keyword for the database table name of the random variables (Random_Variables)
	const string tab_random_variables("FPL_RANDOM_VARIABLES");
	///Keyword for the database table column of the random variable data/the fpl-section gemetric point data (general): Identifier of the zone where the random variable is applied [-] (Random_Variables, _Fpl_Geometrie)
	const string zone_id("ZONE_ID");
	///Keyword for the database table column of the random variable data (general): Minimum boundary of the variables [-] (Random_Variables)
	const string min_boundary("MIN_BOUNDARY");
	///Keyword for the database table column of the random variable data (general): Maximum boundary of the variables [-] (Random_Variables)
	const string max_boundary("MAX_BOUNDARY");
	///Keyword for the database table column of the random variable data (general): Type of the distribution, e.g. mean-, triangle, discrete- or combinated types [-] (Random_Variables)
	const string distribution_type("DISTRIBUTION_TYPE");
	///Keyword for the database table column of the random variable data (general): Unit of the distribution, e.g. m, kg etc [-] (Random_Variables)
	const string distribution_unit("UNIT");

	///Keyword for the database table column of the distribution type data (general): Identifier of the random vaiable [-] (Random_Variables, _Distribution_Class)
	const string variable_id("VARIABLE_ID");
	///Keyword for the database table column of the distribution type data (general): Name of the random vaiable [-] (Random_Variables, _Distribution_Class)
	const string variable_name("VARIABLE_NAME");
	///Keyword for the database table column of the distribution type data (general): Constant of the distribution [-] (Random_Variables, _Distribution_Class)
	const string constant("CONSTANT");
	///Keyword for the database table column of the distribution type data (general): Name of a default distribution [-] (Random_Variables, _Distribution_Class)
	const string default_name("DEFAULT_NAME");

	///Keyword for the database table name of the mean type distribution variables, user defined values (Distribution_Class_Mean)
	const string tab_rand_mean_user("FPL_RAND_MEAN_USER");
	///Keyword for the database table name of the mean type distribution variables, default values (Distribution_Class_Mean)
	const string tab_rand_mean_default("FPL_RAND_MEAN_DEFAULT");

	///Keyword for the database table column of the mean type distribution data: Mean value [different] (Random_Variables, Distribution_Class_Mean)
	const string mean_mean("MEAN");
	///Keyword for the database table column of the mean type distribution data: Second moment, standard derivation (not dynamic) or variation coefficient [-] (Random_Variables, Distribution_Class_Mean)
	const string mean_second_moment("SECOND_MOMENT");
	///Keyword for the database table column of the mean type distribution data: Distribution type of the mean type distributions (Random_Variables, Distribution_Class_Mean)
	const string mean_type("MEAN_TYPE");


	///Keyword for the database table name of the triangle type distribution variables, user defined values (Distribution_Class_Triangle)
	const string tab_rand_triangle_user("FPL_RAND_TRIANGLE_USER");
	///Keyword for the database table name of the triangle type distribution variables, default values (Distribution_Class_Triangle)
	const string tab_rand_triangle_default("FPL_RAND_TRIANGLE_DEFAULT");

	///Keyword for the database table column of the triangle type distribution data: Mid value [different] (Random_Variables, Distribution_Class_Triangle)
	const string triangle_mid("MID");
	///Keyword for the database table column of the triangle type distribution data: Minimum value [different] (Random_Variables, Distribution_Class_Triangle)
	const string triangle_min("MIN");
	///Keyword for the database table column of the triangle type distribution data: Maximum value [different] (Random_Variables, Distribution_Class_Triangle)
	const string triangle_max("MAX");


	///Keyword for the database table name of the discrete type distribution variables, user defined values (Distribution_Class_Discret)
	const string tab_rand_discret_user("FPL_RAND_DISCRET_USER");
	///Keyword for the database table name of the discrete type distribution variables, default values (Distribution_Class_Discret)
	const string tab_rand_discret_default("FPL_RAND_DISCRET_DEFAULT");

	///Keyword for the database table column of the discrete type distribution data: Value identifier [-] (Random_Variables, Distribution_Class_Discret)
	const string discret_value_id("VALUE_ID");
	///Keyword for the database table column of the discrete type distribution data: Discrete value [different] (Random_Variables, Distribution_Class_Discret)
	const string discret_value("VALUE");
	///Keyword for the database table column of the discrete type distribution data: Value probability [%] (Random_Variables, Distribution_Class_Discret)
	const string discret_probability("PROBABILITY");

	///Keyword for the database table name of the mean2mean type distribution variables, user defined values (Distribution_Class_Mean_Mean)
	const string tab_rand_mean2mean_user("FPL_RAND_MEAN2MEAN_USER");
	///Keyword for the database table name of the mean2mean type distribution variables, default values (Distribution_Class_Mean_Mean)
	const string tab_rand_mean2mean_default("FPL_RAND_MEAN2MEAN_DEFAULT");

	///Keyword for the database table column of the mean2mean type distribution data: Mean value of the first (leading) distribution [different] (Random_Variables, Distribution_Class_Mean_Mean)
	const string mean2mean_mean_1("MEAN_1");
	///Keyword for the database table column of the mean2mean type distribution data: Second moment, standard derivation (not dynamic) or variation coefficient of the first (leading) distribution [-] (Random_Variables, Distribution_Class_Mean_Mean)
	const string mean2mean_second_moment_1("SECOND_MOMENT_1");
	///Keyword for the database table column of the mean2mean type distribution data: Distribution type of the first (leading) distribution (Random_Variables, Distribution_Class_Mean_Mean)
	const string mean2mean_type_1("MEAN_TYPE_1");
	///Keyword for the database table column of the mean2mean type distribution data: Constant of the first (leading) distribution (Random_Variables, Distribution_Class_Mean_Mean)
	const string mean2mean_constant_1("CONSTANT_1");
	
	///Keyword for the database table column of the mean2mean type distribution data: Second moment, standard derivation (not dynamic) or variation coefficient of the second (subordinated) distribution [-] (Random_Variables, Distribution_Class_Mean_Mean)
	const string mean2mean_second_moment_2("VARIATIONCOEFFICIENT_2");
	///Keyword for the database table column of the mean2mean type distribution data: Distribution type of the second (subordinated) distribution (Random_Variables, Distribution_Class_Mean_Mean)
	const string mean2mean_type_2("MEAN_TYPE_2");
	///Keyword for the database table column of the mean2mean type distribution data: Constant of the second (subordinated) distribution (Random_Variables, Distribution_Class_Mean_Mean)
	const string mean2mean_constant_2("CONSTANT_2");


	///Keyword for the database table name of the mean2mean type distribution variables, user defined values (Distribution_Class_Discret_Mean)
	const string tab_rand_discret2mean_user("FPL_RAND_DISCRET2MEAN_USER");
	///Keyword for the database table name of the mean2mean type distribution variables, default values (Distribution_Class_Discret_Mean)
	const string tab_rand_discret2mean_default("FPL_RAND_DISCRET2MEAN_DEFAULT");


	///Keyword for the database table name of the control parameters of the slope mechanism of a dike section (Fpl_Mech_Slope_Stability_Dike)
	const string tab_fpl_control_slope("FPL_SLOPE_CONTROL");
	///Keyword for the database table column of the slope control parameters: Flag if it is a landside slope (true) or a waterside one (false) (Fpl_Mech_Slope_Stability_Dike)
	const string landside_flag("LANDSIDE");
	///Keyword for the database table column of the slope control parameters: x-origin of the search raster of the slope mid-points [m] (Fpl_Mech_Slope_Stability_Dike)
	const string x_origin("X_ORIGIN");
	///Keyword for the database table column of the slope control parameters: y-origin of the search raster of the slope mid-points [m] (Fpl_Mech_Slope_Stability_Dike)
	const string y_origin("Y_ORIGIN");
	///Keyword for the database table column of the slope control parameters: number of the slope mid-points in x-direction [-] (Fpl_Mech_Slope_Stability_Dike)
	const string no_x("NO_X");
	///Keyword for the database table column of the slope control parameters: number of the slope mid-points in y-direction [-] (Fpl_Mech_Slope_Stability_Dike)
	const string no_y("NO_Y");
	///Keyword for the database table column of the slope control parameters: delta x of the search raster of the slope mid-points [m] (Fpl_Mech_Slope_Stability_Dike)
	const string delta_x("DELTA_X");
	///Keyword for the database table column of the slope control parameters: delta y of the search raster of the slope mid-points [m] (Fpl_Mech_Slope_Stability_Dike)
	const string delta_y("DELTA_Y");
	///Keyword for the database table column of the slope control parameters: delta r of the radius of the slopes [m] (Fpl_Mech_Slope_Stability_Dike)
	const string delta_r("DELTA_R");
	///Keyword for the database table column of the slope control parameters: delta h for storing information of different waterlevels per slice [m] (Fpl_Mech_Slope_Stability_Dike)
	const string delta_h("DELTA_H");
	///Keyword for the database table column of the slope control parameters: width of the slice [m] (Fpl_Mech_Slope_Stability_Dike)
	const string slice_width("SLICE_WIDTH");

	
	///Keyword for the database table name of the material zones of a fpl-section (Fpl_Dike_Geo_Materialzone)
	const string tab_fpl_material_zones("FPL_MATERIAL_ZONE");
	///Keyword for the database table column of the material zones of a fpl-section: zone name [-] (Fpl_Dike_Geo_Materialzone)
	const string zone_name("ZONE_NAME");
	///Keyword for the database table column of the material zones of a fpl-section: identifier for the variable material zone [-] (Fpl_Dike_Geo_Materialzone)
	const string zone_var_id("ZONE_VAR_ID");
	///Keyword for the database table column of the material zones of a fpl-section: type of the zone [-] (Fpl_Dike_Geo_Materialzone)
	const string zone_type("ZONE_TYPE");


	///Keyword for the database table name of the material zone points of a fpl-section (Fpl_Dike_Geo_Materialzone)
	const string tab_fpl_material_zone_point("FPL_MATERIAL_ZONE_POINTS");
	///Keyword for the database table column of the material zone points of a fpl-section: point identifier [-] (Fpl_Dike_Geo_Materialzone)
	const string point_id("POINT_ID");
	///Keyword for the database table column of the material zone points of a fpl-section: x-coordinate [-] (Fpl_Dike_Geo_Materialzone)
	const string x_coor("X_COOR");
	///Keyword for the database table column of the material zone points of a fpl-section: y-coordinate [-] (Fpl_Dike_Geo_Materialzone)
	const string y_coor("Y_COOR");

}
///General text labels specially for the module HYD \ingroup hyd
namespace hyd_label{




	//model types (hydraulic models)
	///String for the hydraulic model type: river model (_Hyd_Model_Type)
	const string river_model("river_model");
	///String for the hydraulic model type: floodplain model (_Hyd_Model_Type)
	const string floodplain_model("floodplain_model");
	///String for the hydraulic model type: coast model (_Hyd_Model_Type)
	const string coast_model("coast_model");

	//profile types (river-model)
	///String for the profile type of the river-profiles (river): discharge is calculated via Manning-formula (_profile_types)
	const string river_type("river");
	///String for the profile type of the river-profiles (weir): discharge is calculated via Poleni-formula (_profile_types)
	const string weir_type("weir");
	///String for the profile type of the river-profiles (bridge): discharge is calculated via Manning- or in cases where the bridge gets relvant with the Gate-flow formula (_profile_types)
	const string bridge_type("bridge");

	//connection types (river model)
	///String for the connection types of the river-profiles: river profiles inbetween are standard (_connection_types)
	const string standard_conn("standard");
	///String for the connection types of the river-profiles: first profile upstream of a river is an inflow-type (_connection_types)
	const string inflow_conn("inflow");
	///String for the connection types of the river-profiles: last profile downstream of a river is an outflow-type (_connection_types)
	const string outflow_conn("outflow");

	//channel types
	///String for the channel types of the river-profiles
	const string channel_left("left bank");
	///String for the channel types of the river-profiles
	const string channel_main("main channel");
	///String for the channel types of the river-profiles
	const string channel_right("right bank");
	
	///Keyword for the file input of the river profile: riverstation [m] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string ProfLDist ("ProfLDist");
	///Keyword for the file input of the river profile: profile type, e.g. river, weir, bridge [-] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string ProfType ("ProfType");
	///Keyword for the file input of the river profile: connection type, e.g. inflow, standard, outflow [-] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string ConnectionType("ConnectionType");
	///Keyword for the file input of the river profile: initial condition [m] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string InitCondition("InitCondition");
	///Keyword for the file input of the river profile: flag for a boundary point condition [-] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string BoundaryPointCondition("BoundaryPointCondition");
	///Keyword for the file input of the river profile: flag for a stationary boundary point condition [-] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string BoundaryPointStationary("BoundaryPointStationary");
	///Keyword for the file input of the river profile: value of the boundary point condition; a discharge for stationary [m³/s], a curve number for a instationary condtion [-] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string BoundaryPointValue("BoundaryPointValue"); 
	///Keyword for the file input of the river profile: flag for a boundary lateral condition [-] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string BoundaryLateralCondition("BoundaryLateralCondition");
	///Keyword for the file input of the river profile: flag for a stationary boundary lateral condition [-] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string BoundaryLateralStationary("BoundaryLateralStationary");
	///Keyword for the file input of the river profile: value of the boundary lateral condition; a discharge for stationary [m³/s/m], a curve number for a instationary condtion [-] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string BoundaryLateralValue("BoundaryLateralValue");
	///Keyword for the file input of the river profile: flag for a boundary waterlevel condition [-] (Hyd_River_Profile_Connection_Outflow)
	const string BoundaryWaterlevelCondition("BoundaryWaterlevelCondition");
	///Keyword for the file input of the river profile: flag for a stationary boundary waterlevel condition [-] (Hyd_River_Profile_Connection_Outflow)
	const string BoundaryWaterlevelStationary("BoundaryWaterlevelStationary");
	///Keyword for the file input of the river profile: value of the boundary lateral condition; a waterlevl for stationary [m], a curve number for a instationary condtion [-] (Hyd_River_Profile_Connection_Outflow)
	const string BoundaryWaterlevelValue("BoundaryWaterlevelValue");

	///Keyword for the file input of the river profile: falg if an overflow over the left bank is required (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string OverflowCouplingLeft("OverflowCouplingLeft");
	///Keyword for the file input of the river profile: Poleni factor for the overflow over the left bank (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string PoleniFacLeft("PoleniFacLeft");
	///Keyword for the file input of the river profile: falg if an overflow over the right bank is required (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string OverflowCouplingRight("OverflowCouplingRight");
	///Keyword for the file input of the river profile: Poleni factor for the overflow over the right bank (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string PoleniFacRight("PoleniFacRight");

	///Keyword for the file input of the river profile: Discretisation of the tables for the profile [m] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string DeltaXtable("DeltaXtable");

	//dikebreak
	///Keyword for the file input of the river profile: Base point of the floodprtection line at the right bank (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string BasePointRight("BasePointRight");
	///Keyword for the file input of the river profile: Base point of the floodprtection line at the left bank (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string BasePointLeft("BasePointLeft");
	///Keyword for the file input of the river profile: Critical flow velocity representing the dike material at the left bank [m/s] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string CritVelocityLeft("CriticalVelocityLeft");
	///Keyword for the file input of the river profile: Critical flow velocity representing the dike material at the right bank [m/s] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string CritVelocityRight("CriticalVelocityRight");
	///Keyword for the file input of the river profile: Resistance factor which represents the strength of the wall/mobile wall at the left bank [s] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string WallResistanceLeft("WallResistanceLeft");
	///Keyword for the file input of the river profile: Resistance factor which represents the strength of the wall/mobile wall at the right bank [s] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string WallResistanceRight("WallResistanceRight");
	///Keyword for the file input of the river profile: Abrupt breach opening at the left bank [m] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string AbruptBreachOpenLeft("AbruptBreachOpenLeft");
	///Keyword for the file input of the river profile: Abrupt breach opening at the right bank [m] (_Hyd_River_Profile, _Hyd_River_Profile_Type)
	const string AbruptBreachOpenRight("AbruptBreachOpenRight");

	//for bridge profiles
	///Keyword for the file input of the river profile: Vertical size of the bridge body [m] (_Hyd_River_Profile, Hyd_River_Profile_Type_Bridge)
	const string BridgeBodySize("BridgeBodySize");
	///Keyword for the file input of the river profile: Local height of the bridge to the river base [m] (_Hyd_River_Profile, Hyd_River_Profile_Type_Bridge)
	const string LocalBridgeHeight("LocalBridgeHeight");

	//profile table types
	///String for the river profile table type: waterlevel dependent to area (Hyd_Tables)
	const string Waterlevel_table("x= area; y=Waterlevel");
	///String for the river profile table type: width dependent to area (Hyd_Tables)
	const string Width_table("x= area; y=Width");
	///String for the river profile table type: Conveyance of the left bank dependent to area (Hyd_Tables)
	const string Conveyance_table_left("x= area; y=Conveyance of left bank");
	///String for the river profile table type: Conveyance of the main channel dependent to area (Hyd_Tables)
	const string Conveyance_table_mid("x= area; y=Conveyance of main channel");
	///String for the river profile table type: Conveyance of right bank dependent to area (Hyd_Tables)
	const string Conveyance_table_right("x= area; y=Conveyance of right bank");
	///String for the river profile table type: Area dependent to waterlevel (Hyd_Tables)
	const string Area_table("x=waterlevel; y=area");
	///String for the weir profile table type: Discharge per segment dependent to waterlevel (Hyd_Tables)
	const string Weir_discharge_table("x=waterlevel; y=discharge per segment");
	///String for the weir profile table type: Conveyance of the left bank of a weir profile dependent to waterlevel (Hyd_Tables)
	const string Conveyance_table_left_weir("x=waterlevel; y=Conveyance of left bank of weir profile");
	///String for the weir profile table type: Conveyance of the right bank of a weir profile dependent to waterlevel (Hyd_Tables)
	const string Conveyance_table_right_weir("x=waterlevel; y=Conveyance of right bank of weir profile");
	///String for the weir profile table type: Waterlevel over lowest segment point to waterlevel (Hyd_Tables)
	const string Weir_height_table("x=waterlevel; y=waterheight over lowest segment point");

	//boundary curve types
	///Point boundary condition: discharge is in m³/s
	const string Point_Boundary("point");
	///Length boundary condition: discharge is in (m³/s)/m
	const string Length_Boundary("length");
	///Area boundary condition: discharge is in (m³/s)/m²
	const string Area_Boundary("area");
	///Waterlevel boundary condition: waterlevel is given as a local waterlevel related to the river base
	const string Waterlevel_Boundary("waterlevel");
	//boundary curves types for temperature model
	///Boundary condition: temperature in K
	const string Temp_Boundary("temp");
	///Boundary condition: Percentage between 0 and 1 [-]
	const string Perc_Boundary("perc");
	///Boundary condition: solar radiation in W/m²
	const string Solar_rad_Boundary("rad");
	///Boundary condition: speed in m/s
	const string Speed_Boundary("speed");



	//material coefficient types
	///Poleni coefficient [-]
	const string POL("POL");
	///Manning coefficcient n [s/m^(1/3)] 
	const string MAN("MAN");

	//point names
	///String for hyd-point name for an identification of the point: profile x-coordinate, z-coordinate (geodetic height) of the point (_Hyd_River_Profile_Type)
	const string profile_point_x_z("pr_p_x_z");
	///String for hyd-point name for an identification of the point: profile x-coordinate, y-coordinate  of the point (_Hyd_River_Profile_Type)
	const string profile_point_x_y("pr_p_x_y");
	///String for hyd-point name for an identification of the point: mid-point of the profile (x-coordinate, y-coordinate) (_Hyd_River_Profile_Type)
	const string profile_midpoint_x_y("pr_mid_x_y");
	///String for hyd-point name for an identification of the point: min-point of the profile (x-coordinate, y-coordinate) (_Hyd_River_Profile_Type)
	const string profile_minpoint_x_y("pr_min_x_y");
	///String for hyd-point name for an identification of the point: coupling point between floodplain- and coast-model (Hyd_Coupling_Point_FP2CO)
	const string coupling_point_FP2CO("cp_p_FP2CO");
	///String for hyd-point name for an identification of the point: coupling point between floodplain- and floodplain-model (Hyd_Coupling_Point_FP2FP)
	const string coupling_point_FP2FP("cp_p_FP2FP");
	///String for hyd-point name for an identification of the point: coupling point between river- and floodplain-model (Hyd_Coupling_Point_RV2FP)
	const string coupling_point_RV2FP("cp_p_RV2FP");
	///String for hyd-point name for an identification of the point: coupling point between river- and floodplain-model via an hydraulic structure (Hyd_Coupling_RV2FP_Structure)
	const string coupling_point_RV2FP_structure("cp_p_RV2FP_str");
	///String for hyd-point name for an identification of the point: section point of a river-model (Hyd_Model_River)
	const string river_section_point("r_sec_p");
	///String for hyd-point name for an identification of the point: dikeline points (Hyd_Floodplain_Dikeline_Point)
	const string dikeline_point("di_p");

	//preconditioner types
	///Left preconditioning of the matrix (see for documentation of solver cvode: https://computation.llnl.gov/casc/sundials/documentation/cv_guide.pdf)
	const string precon_left_type("prec_left");
	///Right preconditioning of the matrix (see for documentation of solver cvode: https://computation.llnl.gov/casc/sundials/documentation/cv_guide.pdf)
	const string precon_right_type("prec_right");

	//Graham-Schmidt scheme type
	///Classical Graham-Schmidt scheme of the matrix orthonormalization (see for documentation of solver cvode: https://computation.llnl.gov/casc/sundials/documentation/cv_guide.pdf)
	const string gs_classical_type("classical_gs");
	///Modified Graham-Schmidt scheme of the matrix orthonormalization (see for documentation of solver cvode: https://computation.llnl.gov/casc/sundials/documentation/cv_guide.pdf)
	const string gs_modified_type("modified_gs");

	///Floodplain element type standard/normal
	const string standard_elem("normal");
	///Floodplain element type coast
	const string coast_elem("coast");
	///Floodplain element type noflow
	const string noflow_elem("no_flow");
	///Floodplain element type noinfo
	const string noinfo_elem("no_info");
	///Floodplain element type intercepted by a river
	const string river_elem("river");
	///Floodplain element type inside another floodplain model
	const string other_fp_elem("other_fp");
	///Floodplain element type intercepted by a dikeline
	const string dikeline_elem("dikeline");

	///Floodplain raster segment type flow in x-direction
	const string rast_seg_flow_x("flow_x");
	///Floodplain raster segment type flow in y-direction
	const string rast_seg_flow_y("flow_y");
	///Floodplain raster segment type border
	const string rast_seg_border("border");

	///Hyd_Floodplain_Polysegment type coastline
	const string coast_line("coastline");
	///Hyd_Floodplain_Polysegment type riverline
	const string river_line("riverline");
	///Hyd_Floodplain_Polysegment type dikeline
	const string dike_line("dikeline");
	///Hyd_Floodplain_Polysegment type riverline
	const string river_bankline("riverbankline");
	///Hyd_Floodplain_Polysegment type part of a floodplain boundary
	const string fp_boundaryline("fp_boundaryline");

	///_1d_outflow_types types weir
	const string weir_coupling("weir");
	///_1d_outflow_types types gate
	const string gate_coupling("gate");

	///String for the hydraulic boundary base szenario (Hyd_Boundary_Szenario_Management)
	const string sz_hyd_bound_base("base szenario");


	///Keyword for the file input of the hydraulic raster: begin of the hydraulic raster informations [-] (Hyd_Gis2Promraster_Converter)
	const string begin_rasterinfo("!$BEGIN_RASTERINFO");
	///Keyword for the file input of the hydraulic raster: end of the hydraulic raster informations [-] (Hyd_Gis2Promraster_Converter)
	const string end_rasterinfo("!$END_RASTERINFO");

	///Keyword for the file input of the hydraulic raster: begin of the hydraulic raster elements characteristics [-] (Hyd_Gis2Promraster_Converter)
	const string begin_charac("!$BEGIN_CHARAC");
	///Keyword for the file input of the hydraulic raster: end of the hydraulic raster elements characteristics [-] (Hyd_Gis2Promraster_Converter)
	const string end_charac("!$END_CHARAC");

	///Keyword for the file input of the raster: number of elements in x direction; the keyword is adapted from the ArcGis output of raster2ascii (Hyd_Gis2Promraster_Converter)
	const string raster_no_x("!ncols");
	///Keyword for the file input of the raster: number of elements in y direction; the keyword is adapted from the ArcGis output of raster2ascii (Hyd_Gis2Promraster_Converter)
	const string raster_no_y("!nrows");
	///Keyword for the file input of the raster: x coordinate of raster origin [m]; the keyword is adapted from the ArcGis output of raster2ascii (Hyd_Gis2Promraster_Converter)
	const string raster_x_origin("!xllcorner");
	///Keyword for the file input of the raster: y coordinate of raster origin [m]; the keyword is adapted from the ArcGis output of raster2ascii (Hyd_Gis2Promraster_Converter)
	const string raster_y_origin("!yllcorner");
	///Keyword for the file input of the raster: cellsize in x-direction [m]; the keyword is adapted from the ArcGis output of raster2ascii (DHyd_Gis2Promraster_Converter)
	const string raster_width_x("!width_x");
	///Keyword for the file input of the raster: cellsize in y-direction [m]; the keyword is adapted from the ArcGis output of raster2ascii (DHyd_Gis2Promraster_Converter)
	const string raster_width_y("!width_y");
	///Keyword for the file input of the raster: cellsize in y-direction [m]; the keyword is adapted from the ArcGis output of raster2ascii (DHyd_Gis2Promraster_Converter)
	const string raster_angle("!angle");
	///Keyword for the file input of the raster: value if no information are available [-]; the keyword is adapted from the ArcGis output of raster2ascii (Hyd_Gis2Promraster_Converter)
	const string raster_noinfo("!NODATA_value");
	///Keyword for the file input of the raster: name of the output file [-] (Hyd_Gis2Promraster_Converter)
	const string raster_output_file("!output_file");
	
	///Keyword for the file input of the river profiles: number of profiles [-] (just used for replacing profiles)
	const string no_of_profile("!NOFPROF");


	///Keyword for the database table name of the global parameters of the hydraulic system (Hyd_Param_Global)
	const string tab_sys_param("HYD_SYSTEM_PARAM");
	///Keyword for the database table column of the global parameters: number of the set of global parameters [-] (Hyd_Param_Global)
	const string nofset("SET_ID");
	///Keyword for the database table column of the global parameters, time settings: start time [s] (Hyd_Param_Global)
	const string tstart("START_TIME");
    ///Keyword for the database table column of the global parameters, time settings: length of output time steps [s] (Hyd_Param_Global)
	const string tstep("TIME_STEP_LENGTH");
	///Keyword for the database table column of the global parameters, time settings: number of output timesteps [-] (Hyd_Param_Global)
	const string tnof("NUMBER_TIMESTEPS");
	///Keyword for the database table column of the global parameters, time settings: number of internal time steps [-] (Hyd_Param_Global)
	const string nofits("NUMBER_INTERNAL_TIMESTEPS");
	///Keyword for the database table column of the global parameters, integration settings: maximum number of solver steps [-] (Hyd_Param_Global)
	const string maxnumsteps("MAX_SOLVER_STEPS");
	///Keyword for the database table column of the global parameters, integration settings: maximum solver step size [s] (Hyd_Param_Global)
	const string maxstepsize("MAX_SOLVER_STEPSIZE");
	///Keyword for the database table column of the global parameters, integration settings: initial solver step size [s] (Hyd_Param_Global)
	const string inistepsize("INIT_SOLVER_STEPSIZE");
	///Keyword for the database table column of the global parameters, preconditioner settings: precondition type [-] (Hyd_Param_Global)
	const string pretype("PRECONDIT_TYPE");
	///Keyword for the database table column of the global parameters, preconditioner settings: orthogonalization scheme [-] (Hyd_Param_Global)
	const string gramschmidt("GRAMSCHMIDT_SCHEMA");
	///Keyword for the database table column of the global parameters: flag if the coast model (Hyd_Coast_Model) is applied [-] (Hyd_Param_Global)
	const string coastmodel("COASTMODEL_APPLIED");

	///Keyword for the database table column of the global parameters: maximum of waterlevel change per syncronisation timestep [m] per floodplain element (Hyd_Param_Global)
	const string syn_maxchange_h_fp("SYN_MAXCHANGE_H_FP");
	///Keyword for the database table column of the global parameters: maximum of waterlevel change per syncronisation timestep [m] per river element (Hyd_Param_Global)
	const string syn_maxchange_h_rv("SYN_MAXCHANGE_H_rv");
	///Keyword for the database table column of the global parameters: maximum change of explicitly taken energy head of the flow velocity in the river model per syncronisation timestep [m] (Hyd_Param_Global)
	const string syn_maxchange_v_rv("SYN_MAXCHANGE_V_RV");
	///Keyword for the database table column of the global parameters: minimum internal time step [s] (Hyd_Param_Global)
	const string syn_min_int_tstep("SYN_MIN_INT_TSTEP");

	///Keyword for the database table column of the global parameters: output for tecplot 1d  (Hyd_Param_Global)
	const string output_tecplot_1d("output_tecplot_1d");
	///Keyword for the database table column of the global parameters: output for tecplot 2d  (Hyd_Param_Global)
	const string output_tecplot_2d("output_tecplot_2d");
	///Keyword for the database table column of the global parameters: output for bluekenue 2d  (Hyd_Param_Global)
	const string output_bluekenue_2d("output_bluekenue_2d");
	///Keyword for the database table column of the global parameters: output for paraview 1d  (Hyd_Param_Global)
	const string output_paraview_1d("output_paraview_1d");
	///Keyword for the database table column of the global parameters: output for paraview 2d  (Hyd_Param_Global)
	const string output_paraview_2d("output_paraview_2d");
	///Keyword for the database table column of the global parameters: instationary output for database  (Hyd_Param_Global)
	const string output_instat_db("insta_output_db");
	///Keyword for the database table column of the global parameters: output folder (Hyd_Param_Global)
	const string output_folder("output_folder");

	///Keyword for the database table name of the material parameters (Hyd_Param_Material)
	const string tab_mat_param("HYD_MATERIAL_PARAM");
	///Keyword for the database table column of the material parameters: number as identifier [-] (Hyd_Param_Material)
	const string matparam_id("MATERIAL_ID");
	///Keyword for the database table column of the material parameters: value (Hyd_Param_Material) 
	const string matparam_val("VALUE");
	///Keyword for the database table column of the material parameters: type (manning, poleni parameter) [-] (Hyd_Param_Material)
	const string matparam_typ("TYPE");

	///Keyword for the database table name of the general parameters of a river model (Hyd_Model_River)
	const string tab_rv_gen("HYD_RIVER_GENERAL");
	///Keyword for the database table column of the general model river parameters: pre-name and local path for the 1d output [-] (Hyd_Model_River)
	const string d1output("OUTPUT_1D");
	///Keyword for the database table column of the general model river parameters: flag for the interface: false:= no interface is taken between right/left bank and main channel; true:=interface is taken from the main channel [-] (Hyd_Model_River)
	const string prof_interface("PROFILE_INTERFACE");
	///Keyword for the database table column of the general model river parameters: flag for the the gradient calculation: false:= velocity head is taken explicitly into account, flase:= it is not taken into account [-] (Hyd_Model_River)
	const string v_explicit("V_EXPLICIT");


	///Keyword for the database table name of the general temperature parameters of a river model (HydTemp_Model)
	const string tab_temprv_gen("HYD_TEMP_RIVER_GENERAL");
	///Keyword for the database table column of the general model river temperature parameters: flag if the temperature model is applied for the river (HydTemp_Model)
	const string temp_model_applied("TEMP_APPLIED");
	///Keyword for the database table column of the general model river temperature parameters: groundwater temperature [K] (HydTemp_Model)
	const string gw_temp("GW_TEMP");
	///Keyword for the database table column of the general model river temperature parameters: brunt coefficient [-] (HydTemp_Model)
	const string brunt_coef("BRUNT_COEF");
	///Keyword for the database table column of the general model river temperature parameters: view2sky coefficient [-] (HydTemp_Model)
	const string view2sky("VIEW2SKY");
	///Keyword for the database table column of the general model river temperature parameters: bed conductivity [-] (HydTemp_Model)
	const string cond_bed("K_BED");
	///Keyword for the database table column of the general model river temperature parameters: bed temperature [K] (HydTemp_Model)
	const string bed_temp("BED_TEMP");
	///Keyword for the database table column of the general model river temperature parameters: bed warming [-] (HydTemp_Model)
	const string bed_warm_coef("BED_WARM_COEF");
	///Keyword for the database table column of the general model river temperature parameters: diffusive solar radiation [-] (HydTemp_Model)
	const string diff_solar_rad("DIFF_SOLAR_RAD");

	///Keyword for the database table name of the temperature profile data of a temperature model (HydTemp_Profile)
	const string tab_tempprof("HYD_TEMP_PROFILE");

	///Keyword for the database table name of the boundary conditions of the temperature model (HydTemp_Profile)
	const string tab_tempprof_bound("HYD_TEMP_PROF_BOUND");

	///Keyword for the the temperature boundary condition names: air temperature [K] (HydTemp_Profile)
	const string air_temp("AIR_TEMP");
	///Keyword for the the temperature boundary condition names: water temperature [K] (HydTemp_Profile)
	const string water_temp("WATER_TEMP");
	///Keyword for the the temperature boundary condition names: solar radiation [W/m^2] (HydTemp_Profile)
	const string solar_rad("SOLAR_RAD");
	///Keyword for the the temperature boundary condition names: humidity [-] (HydTemp_Profile)
	const string humidity("HUMID");
	///Keyword for the the temperature boundary condition names: wind speed [m/s] (HydTemp_Profile)
	const string wind("WINDSPEED");
	///Keyword for the the temperature boundary condition names: cloudness [-] (HydTemp_Profile)
	const string cloud("CLOUDNESS");
	///Keyword for the the temperature boundary condition names: shadow [-] (HydTemp_Profile)
	const string shadow("SHADOWS");
	///Keyword for the the temperature boundary condition names: inlet temperature [K] (HydTemp_Profile)
	const string inlet_temp("INLET_TEMP");


	///Keyword for the database table name of the profile temperature result data of a temperature model (HydTemp_Model, HydTemp_Profile)
	const string tab_tempprof_erg_max("HYD_TEMP_PROFILE_MAX_RESULTS");

	///Keyword for the database table column of the temperature result data of a river model: local maximal temperature in the profile [K] (HydTemp_Model, HydTemp_Profile)
	const string proferg_T_max("TEMP_MAX");
	///Keyword for the database table column of the temperature result data of a river model: local minimal temperature in the profile [K] (HydTemp_Model, HydTemp_Profile)
	const string proferg_T_min("TEMP_MIN");
	///Keyword for the database table column of the temperature result data of a river model: local average temperature in the profile [K] (HydTemp_Model, HydTemp_Profile)
	const string proferg_T_av("TEMP_AV");
	///Keyword for the database table column of the temperature result data of a river model: time point, when the local maximal temperature in the profile occur [s] (HydTemp_Model, HydTemp_Profile)
	const string proferg_t_T_max("TIME_TEMP_MAX");
	///Keyword for the database table column of the temperature result data of a river model: time point, when the local maximal temperature in the profile occur [s] (HydTemp_Model, HydTemp_Profile)
	const string proferg_t_T_min("TIME_TEMP_MIN");


	///Keyword for the database table name of the profile temperature instationary result data of a temperature model (HydTemp_Model, HydTemp_Profile)
	const string tab_tempprof_instat_erg_max("HYD_TEMP_PROFILE_INSTAT_RESULTS");
	///Keyword for the database table column of the temperature result data of a river model: local water temperature in the profile [K] (HydTemp_Model, HydTemp_Profile)
	const string proferg_T_water("TEMP_WATER");


	///Keyword for the database table name of the general parameters of a floodplain model (Hyd_Model_Floodplain)
	const string tab_fp_gen("HYD_FLOODPLAIN_GENERAL");
	///Keyword for the database table column of the general model floodplain parameters: number of elements in x-direction [-] (Hyd_Model_Floodplain)
	const string nx("NX");
	///Keyword for the database table column of the general model floodplain parameters: number of elements in y-direction [-] (Hyd_Model_Floodplain)
	const string ny("NY");
	///Keyword for the database table column of the general model floodplain parameters: width of the element in x-direction [m] (Hyd_Model_Floodplain)
	const string elemwidth_x("ELEMWIDTH_X");
	///Keyword for the database table column of the general model floodplain parameters: width of the element in y-direction [m] (Hyd_Model_Floodplain)
	const string elemwidth_y("ELEMWIDTH_Y");
	///Keyword for the database table column of the general model floodplain parameters: angle of the coordinate system [°] (Hyd_Model_Floodplain)
	const string angle("ANGLE");
	///Keyword for the database table column of the general model floodplain parameters: low left x coordinate [m] (Hyd_Model_Floodplain)
	const string lowleftx("LOW_LEFT_X");
	///Keyword for the database table column of the general model floodplain parameters: low left y coordinate [m] (Hyd_Model_Floodplain)
	const string lowlefty("LOW_LEFT_Y");
	///Keyword for the database table column of the general model floodplain parameters: value for a elemnet with no informations; it is compared with the element height [m] (Hyd_Model_Floodplain)
	const string noinfovalue("NOINFO_VALUE");
	///Keyword for the database table column of the general model floodplain parameters: value for a elemnet with no informations; boundary-value if a element/profile is wet or dry [m] (Hyd_Model_Floodplain)
	const string wet("WET_HEIGHT");

	///Keyword for the database table column of the general model (river/floodplain) parameters: absolute solver tolerance [-] (Hyd_Model_Floodplain, Hyd_Model_River)
	const string atol("ABSOULTE_TOLERANCE");
	///Keyword for the database table column of the general model (river/floodplain) parameters: relative solver tolerance [-] (Hyd_Model_Floodplain, Hyd_Model_River)
	const string rtol("RELATIV_TOLERANCE");
	///Keyword for the database table column of the general model (river/floodplain) parameters: pre-name and local path for the 2d output [-] (Hyd_Model_Floodplain, Hyd_Model_River)
	const string d2output("OUTPUT_2D");
	///Keyword for the database table column of the general model (river/floodplain) parameters: number as identifier of the model [-] (Hyd_Model_Floodplain, Hyd_Model_River)
	const string genmod_id("MODEL_ID");
	///Keyword for the database table column of the general model (river/floodplain) parameters: type for specifying the type of model [-] (Hyd_Model_Floodplain, Hyd_Model_River)
	const string genmod_type("MODEL_TYPE");
	///Keyword for the database table column of the general model (river/floodplain) parameters: model name [-] (Hyd_Model_Floodplain, Hyd_Model_River)
	const string genmod_name("NAME");

	///Keyword for the database table name of the element data of a floodplain model (Hyd_Element_Floodplain)
	const string tab_fpelem("HYD_FLOODPLAIN_ELEMENT");
	///Keyword for the database table column of the element data of a floodplain model: number of the floodplain [-] (Hyd_Element_Floodplain)
	const string elemdata_fpno("FLOODPLAIN_ID");
	///Keyword for the database table column of the element data of a floodplain model: global identifier of the data records[-] (Hyd_Element_Floodplain) 
	const string elemdata_glob_id("ELEM_GLOB_ID");
	///Keyword for the database table column of the element data of a floodplain model: local identifier of the floodplain element [-] (Hyd_Element_Floodplain)
	const string elemdata_id("ELEM_ID");
	///Keyword for the database table column of the element data of a floodplain model: geodetic height of the floodplain element [m] (Hyd_Element_Floodplain)
	const string elemdata_z("GEODETIC_HEIGHT");
	///Keyword for the database table column of the element data of a floodplain model: material type id of the floodplain element [-] (Hyd_Element_Floodplain)
	const string elemdata_matid("MATERIAL_ID");
	///Keyword for the database table column of the element data of a floodplain model: initial-condition of the floodplain element [m] (Hyd_Element_Floodplain)
	const string elemdata_init("INIT_CONDITION");
	///Keyword for the database table column of the element data of a floodplain model: x-coordinate of the element midpoint [m] (Hyd_Element_Floodplain)
	const string elemdata_mid_x("MID_X");
	///Keyword for the database table column of the element data of a floodplain model: y-coordinate of the element midpoint [m] (Hyd_Element_Floodplain)
	const string elemdata_mid_y("MID_Y");
	///Keyword for the database table column of the element data of a floodplain model: element as polygon [-] (Hyd_Element_Floodplain)
	const string elemdata_polygon("GEO_POLYGON");

	///Keyword for the database table name of the element boundary data of a floodplain model (Hyd_Element_Floodplain)
	const string tab_fpelem_bound("HYD_FLOODPLAIN_ELEM_BOUND_COND");

	///Keyword for the database table name of the element result data of a floodplain model (Hyd_Element_Floodplain)
	const string tab_fpelem_erg_max("HYD_FLOODPLAIN_ELEM_MAX_RESULT");
	///Keyword for the database table name of the element instationary result data of a floodplain model (Hyd_Element_Floodplain)
	const string tab_fpelem_erg_instat("HYD_FLOODPLAIN_ELEM_INSTAT_RESULT");
	///Keyword for the database view name of the element instationary result data of a floodplain model (Hyd_Element_Floodplain)
	const string view_fpelem_erg_instat("HYD_FLOODPLAIN_ELEM_INSTAT_RES_VIEW");
	///Keyword for the database view name of the connection of elements to boundary conditions (Hyd_Element_Floodplain)
	const string view_fpelem2bound("HYD_FLOODPLAIN_ELEM_BOUND_VIEW");
	///Keyword for the database view name of the connection of elements to boundary conditions (Hyd_Element_Floodplain)
	const string view_rvprofile2bound("HYD_RIVER_PROFILE_BOUND_VIEW");
	///Keyword for the database view name of the connection of elements to boundary conditions (Hyd_Element_Floodplain)
	const string view_rvprofile2tempbound("HYD_TEMP_PROFILE_BOUND_VIEW");
	///Keyword for the database table column of the element result data of a floodplain model: local maximal waterlevel in an element [m] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_h_max("WATERLEVEL");
	///Keyword for the database table column of the element result data of a floodplain model: global maximal waterlevel in an element [m] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_s_max("GLOB_WATERLEVEL");
	///Keyword for the database table column of the element result data of a floodplain model: maximal change in waterlevel per time  [m/min] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_dsdt_max("DSDT");
	///Keyword for the database table column of the element result data of a floodplain model: maximal flow velocity in x-direction [m/s] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_vx_max("X_VELOCITY");
	///Keyword for the database table column of the element result data of a floodplain model: maximal flow velocity in y-direction [m/s] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_vy_max("Y_VELOCITY");
	///Keyword for the database table column of the element result data of a floodplain model: maximal total flow velocity [m/s] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_vtot_max("VELOCITY_TOTAL");
	///Keyword for the database table column of the element result data of a floodplain model: maximal product of total flow velocity times waterlevel [m²/s] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_hv_max("HV");
	///Keyword for the database table column of the element result data of a floodplain model: first arrival time of water to the element [m] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_t_first("FIRST_ARRIVAL_TIME");
	///Keyword for the database table column of the element result data of a floodplain model: duration, during the element is wet [s] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_dur_wet("DURATION_WET");
	///Keyword for the database table column of the element result data of a floodplain model: watervolume in the element at the end of the calculation [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_end_vol("END_VOLUME");
	///Keyword for the database table column of the element result data of a floodplain model: total inflow boundary volume to the element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_bound_in("IN_BOUNDARY");
	///Keyword for the database table column of the element result data of a floodplain model: total outflow boundary volume out of an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_bound_out("OUT_BOUNDARY");
	///Keyword for the database table column of the element result data of a floodplain model: total inflow volume due to a structure coupling of a river model to an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_struc_in("IN_STRUCTURE");
	///Keyword for the database table column of the element result data of a floodplain model: total outflow volume due to a structure coupling of a river model out of an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_struc_out("OUT_STRUCTURE");
	///Keyword for the database table column of the element result data of a floodplain model: total inflow volume due to a direct coupling with a river model to an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_dir_in("IN_DIRECT");
	///Keyword for the database table column of the element result data of a floodplain model: total outflow volume due to a direct coupling with a river model out of an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_dir_out("OUT_DIRECT");
	///Keyword for the database table column of the element result data of a floodplain model: total inflow volume due to an overflow coupling of a river model to an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_rv_ov_in("IN_RV_OVERFLOW");
	///Keyword for the database table column of the element result data of a floodplain model: total outflow volume due to an overflow coupling of a river model out of an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_rv_ov_out("OUT_RV_OVERFLOW");
	///Keyword for the database table column of the element result data of a floodplain model: total inflow volume due to a dikebreak coupling of a river model to an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_rv_db_in("IN_RV_BREAK");
	///Keyword for the database table column of the element result data of a floodplain model: total outflow volume due to a dikebreak coupling of a river model out of an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_rv_db_out("OUT_RV_BREAK");
	///Keyword for the database table column of the element result data of a floodplain model: total inflow volume due to an overflow coupling of a coast model to an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_co_ov_in("IN_CO_OVERFLOW");
	///Keyword for the database table column of the element result data of a floodplain model: total outflow volume due to an overflow coupling of a coast model out of an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_co_ov_out("OUT_CO_OVERFLOW");
	///Keyword for the database table column of the element result data of a floodplain model: total inflow volume due to a dikebreak coupling of a coast model to an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_co_db_in("IN_CO_BREAK");
	///Keyword for the database table column of the element result data of a floodplain model: total outflow volume due to a dikebreak coupling of a coast model out of an element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_co_db_out("OUT_CO_BREAK");
	///Keyword for the database table column of the element result data of a floodplain model: total inflow volume due to a floodplain coupling to another floodplain element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_fp_in("IN_OTHER_FP");
	///Keyword for the database table column of the element result data of a floodplain model: total outflow volume due to a floodplain coupling to another floodplain element [m³] (Hyd_Element_Floodplain, _Hyd_Element_Floodplain_Type)
	const string elemerg_cv_fp_out("OUT_OTHER_FP");

	///Keyword for the database table column of the instationary result data (Hyd_Element_Floodplain, _Hyd_River_Profile)
	const string data_time("DATE_TIME");


	///Keyword for the database table name of the profile data of a river model (_Hyd_River_Profile)
	const string tab_rvprof("HYD_RIVER_PROFILE");
	///Keyword for the database table column of the profile data of a river model: number of the river [-] (_Hyd_River_Profile)
	const string profdata_rvno("RIVER_ID");
	///Keyword for the database table column of the profile data of a river model: name [-] (_Hyd_River_Profile)
	const string profdata_name("NAME");
	///Keyword for the database table column of the profile data of a river model: distance to the upstream profile [m] (_Hyd_River_Profile)
	const string profdata_ldist("RIVERSTATION");
	///Keyword for the database table column of the profile data of a river model: delta x for the profile table discretization [m] (_Hyd_River_Profile)
	const string profdata_deltax("DELTA_X");
	///Keyword for the database table column of the profile data of a river model: connection type of the river profile (_connection_types) [-] (_Hyd_River_Profile)
	const string profdata_contyp("CONNECTION_TYPE");
	///Keyword for the database table column of the profile data of a river model: profile type of the river profile (_profile_types) [-] (_Hyd_River_Profile)
	const string profdata_proftyp("PROFILE_TYPE");
	///Keyword for the database table column of the profile data of a river model: initial condition [m] (_Hyd_River_Profile)
	const string profdata_init("INIT_CONDITION");
	///Keyword for the database table column of the profile data of a river model: river internal identifier [-] (_Hyd_River_Profile)
	const string profdata_prof_id("PROFILE_ID");
	///Keyword for the database table column of the profile data of a river model: global identifier of the data records [-] (_Hyd_River_Profile)
	const string profdata_glob_id("PROFILE_GLOB_ID");
	///Keyword for the database table column of the profile data of a river model: flag for an overfall over the right bank [-] (_Hyd_River_Profile)
	const string profdata_over_r("RIGHT_OVERFLOW");
	///Keyword for the database table column of the profile data of a river model: flag for an overfall over the left bank [-] (_Hyd_River_Profile)
	const string profdata_over_l("LEFT_OVERFLOW");
	///Keyword for the database table column of the profile data of a river model: poleni factor for an overfall over the right bank [-] (_Hyd_River_Profile)
	const string profdata_poleni_r("RIGHT_POLENI");
	///Keyword for the database table column of the profile data of a river model: poleni factor for an overfall over the left bank [-] (_Hyd_River_Profile)
	const string profdata_poleni_l("LEFT_POLENI");
	///Keyword for the database table column of the profile data of a river model: polyline of the profile points [-] (_Hyd_River_Profile)
	const string profdata_polyline("GEO_POLYLINE");
	///Keyword for the database table column of the river profile break data : number of the left-bank base point, to which height the break will developed [-] (_Hyd_River_Profile)
	const string profdata_base_left("LEFT_BASE");
	///Keyword for the database table column of the river profile break data : number of the right-bank base point, to which height the break will developed [-] (_Hyd_River_Profile)
	const string profdata_base_right("RIGHT_BASE_POINT");

	///Keyword for the database table name of the profile boundary data of a river model (_Hyd_River_Profile)
	const string tab_rvprof_bound("HYD_RIVER_PROF_BOUND_CONDITION");

	/////Keyword for the database table column of the boundary data of the models: identifier for the szenario [-] (Hyd_Model_Floodplain, Hyd_Model_River, Hyd_Instatonary_Boundary)
	//const string bounddata_sz_id("BOUNDDATA_SZ_ID");
	///Keyword for the database table column of the boundary data of the models: flag fo a statinary boundary condition (true:= stationary, flase:=instationary) [-] (Hyd_Model_Floodplain, Hyd_Model_River, HydTemp_Profile)
	const string bounddata_stat("STATIONARY");
	///Keyword for the database table column of the boundary data of the models: stationary => discharge or waterlevel value; instationary => curve number [-] (Hyd_Model_Floodplain, Hyd_Model_River, Hyd_Instatonary_Boundary, HydTemp_Profile)
	const string bounddata_value("VALUE");
	///Keyword for the database table column of the boundary data of the models: enumarator for the boundary type (_bound_type) [-] (Hyd_Model_Floodplain, Hyd_Model_River, Hyd_Instatonary_Boundary, Hyd_Instatonary_Boundary, HydTemp_Profile)
	const string bounddata_type("BOUNDARY_TYPE");

	///Keyword for the database table column of the boundary data of the models: enumarator for the boundary type (_bound_type) [-] (HydTemp_Profile)
	const string bounddata_name("BOUNDARY_NAME");

	///Keyword for the database table column of the river profile break data : the river bank in flow direction, for which the break data are valid (true:=left river bank; false:= right river bank) [-] (_Hyd_River_Profile)
	const string profbreak_left("LEFT_BANK_BREAK");

	///Keyword for the database table name of the profile wallbreak data of a river model (_Hyd_River_Profile)
	const string tab_rvprof_wallbreak("HYD_RIVER_PROFILE_WALLBREAK");
	///Keyword for the database table column of the river profile wallbreak data : resistance parameter of the wall [m^0.5/s] (_Hyd_River_Profile)
	const string profwall_res("WALL_RESISTANCE");
	///Keyword for the database table column of the river profile break data : length of an abrupt failure of a wallbreak [m] (_Hyd_River_Profile)
	const string profwall_abrupt("ABRUPT_BREAK_LENGTH");


	///Keyword for the database table name of the profile dikebreak data of a river model (_Hyd_River_Profile)
	const string tab_rvprof_dikebreak("HYD_RIVER_PROFILE_DIKEBREAK");
	///Keyword for the database table column of the river profile dikebreak data : resistance parameter of the dike [m/s] (_Hyd_River_Profile)
	const string profdike_crit_v("CRITICAL_VELOCITY");

	///Keyword for the database table name of the profile bridge data of a river model (_Hyd_River_Profile)
	const string tab_rvprof_bridge("HYD_RIVER_PROFILE_BRIDGE");
	///Keyword for the database table column of the river profile bridge data: size of the bridge body [m] (_Hyd_River_Profile)
	const string profbridge_size("BRIDGE_SIZE");
	///Keyword for the database table column of the river profile bridge data: local bridge height to the river base [m] (_Hyd_River_Profile)
	const string profbridge_height("BRIDGE_HEIGHT");

	///Keyword for the database table name of the profile point data of a river model; it is handled in _Hyd_River_Profile_Type
	const string tab_rvprof_points("HYD_RIVER_PROFILE_POINTS");
	///Keyword for the database table column of the river profile point data: global identifier [-] (_Hyd_River_Profile_Type)
	const string profpoint_glob_id("POINT_GLOB_ID");
	///Keyword for the database table column of the river profile point data: local identifier [-] (_Hyd_River_Profile_Type)
	const string profpoint_id("POINT_ID");
	///Keyword for the database table column of the river profile point data: global x-coordinate [m] (_Hyd_River_Profile_Type)
	const string profpoint_x_co("POINT_X");
	///Keyword for the database table column of the river profile point data: global y-coordinate [m] (_Hyd_River_Profile_Type)
	const string profpoint_y_co("POINT_Y");
	///Keyword for the database table column of the river profile point data: global z-coordinate [m] (_Hyd_River_Profile_Type)
	const string profpoint_z_co("POINT_Z");
	///Keyword for the database table column of the river profile point data: identifier for the material value [-] (_Hyd_River_Profile_Type)
	const string profpoint_mat_id("MATERIAL_ID");
	///Keyword for the database table column of the river profile point data: local distance between the points orthogonal to the flow direction [m] (_Hyd_River_Profile_Type)
	const string profpoint_dist("DISTANCE");
	///Keyword for the database table column of the river profile point data: identifier for the channel: left bank (1), mid channel (2), right bank (3) [-] (_Hyd_River_Profile_Type)
	const string profpoint_ch_id("BANK_CHANNEL_ID");
	///Keyword for the database table column of the river profile point data: geometry data of the point [-] (_Hyd_River_Profile_Type)
	const string profpoint_point("GEO_POINT");

	///Keyword for the database table name of the profile result data of a river model (Hyd_Model_River)
	const string tab_rvprof_erg_max("HYD_RIVER_PROFILE_MAX_RESULTS");
	///Keyword for the database table name of the profile instationary result data of a river model (Hyd_Model_River)
	const string tab_rvprof_erg_instat("HYD_RIVER_PROFILE_INSTAT_RESULTS");
	///Keyword for the database table column of the profile result data of a river model: local maximal waterlevel in the profile [m] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_h_max("H_WATERLEVEL");
	///Keyword for the database table column of the profile result data of a river model: global maximal waterlevel in the profile [m] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_s_max("S_WATERLEVEL");
	///Keyword for the database table column of the profile result data of a river model: the local maximal waterlevel in the profile related to the left base point [m] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_hmax_lb("LEFT_BANK_WATERLEVEL");
	///Keyword for the database table column of the profile result data of a river model: the local maximal waterlevel in the profile related to the right base point [m] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_hmax_rb("RIGHT_BANK_WATERLEVEL");
	///Keyword for the database table column of the profile result data of a river model: time point, when the local maximal waterlevel in the profile occur [s] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_t_hmax("TIME_WATERLEVEL");
	///Keyword for the database table column of the profile result data of a river model: the local maximal waterlevel in the profile related to the left base point for concerning a break [m] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_hmax_lb_break("BREAK_LEFT_BANK_WATERLEVEL");
	///Keyword for the database table column of the profile result data of a river model: the local maximal waterlevel in the profile related to the right base point for concerning a break [m] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_hmax_rb_break("BREAK_RIGHT_BANK_WATERLEVEL");
	///Keyword for the database table column of the profile result data of a river model: maximal flow velocity in the profile [m/s] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_v_max("VELOCITY");
	///Keyword for the database table column of the profile result data of a river model: maximal width in the profile [m] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_width_max("WIDTH");
	///Keyword for the database table column of the profile result data of a river model: duration, during the profile is wet [s] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_dur_wet("WET_DURATION");
	///Keyword for the database table column of the profile result data of a river model: duration, during the profile is dry [s] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_dur_dry("DRY_DURATION");
	///Keyword for the database table column of the profile result data of a river model: total inflow volume due to a coupling to another river model [m³] (Hyd_Model_River, _Hyd_River_Profile)
	const string proferg_cv_rv_in("IN_RIVER");
	///Keyword for the database table column of the profile result data of a river model: total outflow volume due to a coupling to another river model [m³] (Hyd_Model_River, _Hyd_River_Profile)
	const string proferg_cv_rv_out("OUT_RIVER");
	///Keyword for the database table column of the profile result data of a river model: total inflow volume due to a coupling to a floodplain model via a hydraulic structure [m³] (Hyd_Model_River, _Hyd_River_Profile, Hyd_Model_Floodplain)
	const string proferg_cv_struc_in("IN_STRUCTURE");
	///Keyword for the database table column of the profile result data of a river model: total outflow volume due to a coupling to a floodplain model via a hydraulic structure [m³] (Hyd_Model_River, _Hyd_River_Profile, Hyd_Model_Floodplain)
	const string proferg_cv_struc_out("OUT_STRUCTURE");
	///Keyword for the database table column of the profile result data of a river model: total inflow volume due to a coupling to a floodplain model via an overflow over the right bank [m³] (Hyd_Model_River, _Hyd_River_Profile, Hyd_Model_Floodplain)
	const string proferg_cv_r_ov_in("IN_RI_OVERFLOW");
	///Keyword for the database table column of the profile result data of a river model: total outflow volume due to a coupling to a floodplain model via an overflow over the right bank [m³] (Hyd_Model_River, _Hyd_River_Profile, Hyd_Model_Floodplain)
	const string proferg_cv_r_ov_out("OUT_RI_OVERFLOW");
	///Keyword for the database table column of the profile result data of a river model: total inflow volume due to a coupling to a floodplain model via an overflow over the left bank [m³] (Hyd_Model_River, _Hyd_River_Profile, Hyd_Model_Floodplain)
	const string proferg_cv_l_ov_in("IN_LE_OVERFLOW");
	///Keyword for the database table column of the profile result data of a river model: total outflow volume due to a coupling to a floodplain model via an overflow over the left bank [m³] (Hyd_Model_River, _Hyd_River_Profile, Hyd_Model_Floodplain)
	const string proferg_cv_l_ov_out("OUT_LE_OVERFLOW");
	///Keyword for the database table column of the profile result data of a river model: total inflow volume due to a coupling to a floodplain model via a flow through a breach of the right bank [m³] (Hyd_Model_River, _Hyd_River_Profile, Hyd_Model_Floodplain)
	const string proferg_cv_r_db_in("IN_RI_BREAK");
	///Keyword for the database table column of the profile result data of a river model: total outflow volume due to a coupling to a floodplain model via a flow through a breach of the right bank [m³] (Hyd_Model_River, _Hyd_River_Profile, Hyd_Model_Floodplain)
	const string proferg_cv_r_db_out("OUT_RI_BREAK");
	///Keyword for the database table column of the profile result data of a river model: total inflow volume due to a coupling to a floodplain model via a flow through a breach of the left bank [m³] (Hyd_Model_River, _Hyd_River_Profile, Hyd_Model_Floodplain)
	const string proferg_cv_l_db_in("IN_LE_BREAK");
	///Keyword for the database table column of the profile result data of a river model: total outflow volume due to a coupling to a floodplain model via a flow through a breach of the left bank [m³] (Hyd_Model_River, _Hyd_River_Profile, Hyd_Model_Floodplain)
	const string proferg_cv_l_db_out("OUT_LE_BREAK");
	///Keyword for the database table column of the profile result data of a river model: maximal discharge through the river profile [m³/s] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_q_max("DISCHARGE");
	///Keyword for the database table column of the profile result data of a river model: river section as polygon [-] (Hyd_Model_River, _Hyd_River_Profile_Type)
	const string proferg_polygon("GEO_POLYGON");

	///Keyword for the database table name of the maximum waterlevel for a coupling between hydraulic and fpl-sections (Hyd_Coupling_RV2FP_Merged)
	const string tab_coup_max("HYD_COUPLING_MAX_H2FPL");
	///Keyword for the database table column of the maximum waterlevel for a coupling between hydraulic and fpl-sections: the identifier of the couplingpoint where the maximum waterlevel occurs  [-] (Hyd_Coupling_RV2FP_Merged)
	const string coupling_point_max_h("COUP_POINT_MAX_H");
	///Keyword for the database table column of the maximum waterlevel for a coupling between hydraulic and fpl-sections: maximum waterlevel for the coupling to the fragility curve  [m] (Hyd_Coupling_RV2FP_Merged)
	const string maxh("MAX_H");
	///Keyword for the database table column of the maximum waterlevel for a coupling between hydraulic and fpl-sections: timepoint, when the maximum waterlevel for the coupling to the fragility curve occurs [m] (Hyd_Coupling_RV2FP_Merged)
	const string time_maxh("TIME_MAX_H");
	///Keyword for the database table column of the maximum waterlevel for a coupling between hydraulic and fpl-sections: the identifier of the couplingpoint where the maximum waterlevel for a break occurs  [-] (Hyd_Coupling_RV2FP_Merged)
	const string coupling_point_max_h2break("COUP_POINT_MAX_H2BREAK");
	///Keyword for the database table column of the maximum waterlevel for a coupling between hydraulic and fpl-sections: maximum waterlevel for the coupling between river and floodplain-model via a break  [m] (Hyd_Coupling_RV2FP_Merged)
	const string maxh2break("MAX_H2BREAK");
	///Keyword for the database table column of the maximum waterlevel for a coupling between hydraulic and fpl-sections: timepoint, when themaximum waterlevel for the coupling between river and floodplain-model via a break occurs  [m] (Hyd_Coupling_RV2FP_Merged)
	const string time_maxh2break("TIME_MAX_H2BREAK");

	///Keyword for the database table name of the instationary boundary curves (Hyd_Instationary_Boundary)
	const string tab_instat_bound("HYD_INSTAT_BOUND_CURVE");
	///Keyword for the database table column of the instationary boundary curves: the identifier for the boundary curve  [-] (Hyd_Instationary_Boundary)
	const string instatbound_curve_id("CURVE_ID");
	///Keyword for the database table column of the instationary boundary curves: global identifier of the data records [-] (Hyd_Instationary_Boundary)
	const string instatbound_glob_id("CURVE_GLOB_ID");

	///Keyword for the database table name of the points of the instationary boundary curves (Hyd_Instationary_Boundary)
	const string tab_instatbound_point("HYD_INSTAT_BOUND_CURVE_POINT");
	///Keyword for the database table column of the points of the instationary boundary curves: the point in time of an instationary boundary point [h] (Hyd_Instationary_Boundary)
	const string instatbound_time("TIME");
	///Keyword for the database table column of the points of the instationary boundary curves: the identifier of the points of boundary curve [-] (Hyd_Instationary_Boundary)
	const string instatbound_point_id("POINT_ID");
	///Keyword for the database table column of the points of the instationary boundary curves: global identifier of the data records [-] (Hyd_Instationary_Boundary)
	const string instatbound_point_glob_id("POINT_GLOB_ID");

	///Keyword for the database table name of the floodplain polysegments (Hyd_Floodplain_Polysegment) like dikelines or coastline
	const string tab_fp_polyseg("HYD_FLOODPLAIN_POLYSEGMENT");
	///Keyword for the database table column of the floodplain polysegments: global identifier of the data records [-] (Hyd_Floodplain_Polysegment)
	const string polyseg_glob_id("SEGMENT_GLOB_ID");
	///Keyword for the database table column of the floodplain polysegments: the global identifier of polysegment point [-] (Hyd_Floodplain_Polysegment)
	const string polyseg_point_glob_id("POINT_GLOB_ID");
	///Keyword for the database table column of the floodplain polysegments: the local identifier of the points of the polysegment [-] (Hyd_Floodplain_Polysegment)
	const string polyseg_point_id("POINT_ID");
	///Keyword for the database table column of the floodplain polysegments: x-coordinate of the segment point [m] (Hyd_Floodplain_Polysegment)
	const string polyseg_point_x("POINT_X");
	///Keyword for the database table column of the floodplain polysegments: y-coordinate of the segment point [m] (Hyd_Floodplain_Polysegment)
	const string polyseg_point_y("POINT_Y");
	///Keyword for the database table column of the floodplain polysegments: absolute h-coordinate of the segment point [m] (Hyd_Floodplain_Polysegment)
	const string polyseg_point_h("ABS_HEIGHT");
	///Keyword for the database table column of the floodplain polysegments: absolute base-coordinate of the segment point [m] (Hyd_Floodplain_Polysegment)
	const string polyseg_point_base("BASE_HEIGHT");
	///Keyword for the database table column of the floodplain polysegments: poleni value of the segment point [-] (Hyd_Floodplain_Polysegment)
	const string polyseg_point_pol("POLENI");
	///Keyword for the database table column of the floodplain polysegments: overflow flag of the segment point [-] (Hyd_Floodplain_Polysegment)
	const string polyseg_point_overflow("OVERFLOW");
	///Keyword for the database table column of the floodplain polysegments: break flag of the segment point [-] (Hyd_Floodplain_Polysegment)
	const string polyseg_point_break("BREAK");
	///Keyword for the database table column of the floodplain polysegments: abrupt break flag of the segment point [-] (Hyd_Floodplain_Polysegment)
	const string polyseg_point_abrupt_break("ABRUPT_BREAK");
	///Keyword for the database table column of the floodplain polysegments (break parameter): resistance of the segment point [m/s (continuous) or m^0.5/s (abrupt)] (Hyd_Floodplain_Polysegment)
	const string polyseg_point_resistance("RESISTANCE");
	///Keyword for the database table column of the floodplain polysegments (break parameter, just neede incase of abrupt breaching): abrupt opening of the segment point [m] (Hyd_Floodplain_Polysegment)
	const string polyseg_point_open_width("ABRUPT_OPEN_WIDTH");

	///Keyword for the database table name of the observation points (Hyd_Observation_Point_Management)
	const string tab_obs_point("HYD_OBS_POINT");
	///Keyword for the database table column of the observation points: the local identifier of the observation points[-] (Hyd_Observation_Point_Management)
	const string obs_point_id("POINT_ID");
	///Keyword for the database table column of the observation points: x-coordinate of the observation point [m] (Hyd_Observation_Point_Management)
	const string obs_point_x("POINT_X");
	///Keyword for the database table column of the observation points: y-coordinate of the observation point [m] (Hyd_Observation_Point_Management)
	const string obs_point_y("POINT_Y");
	///Keyword for the database table column of the observation points: name of the observation point [-] (Hyd_Observation_Point_Management)
	const string obs_point_name("NAME");
	///Keyword for the database table column of the observation points: geometrical data type of the segment point [m] (Hyd_Observation_Point_Management)
	const string obs_point("GEO_POINT");


	///Keyword for the database table column of the floodplain polysegments: geometrical data type of the segment point [m] (Hyd_Floodplain_Polysegment)
	const string polyseg_point("GEO_POINT");
	///Keyword for the database table column of the floodplain polysegments: flag if the polysegment is closed [-] (Hyd_Floodplain_Polysegment)
	const string polyseg_closed("CLOSED");
	///Keyword for the database table column of the floodplain polysegments: geo data type for displaing the polysegment [-] (Hyd_Floodplain_Polysegment)
	const string polyseg_out("GEO_POLYLINE");
	///Keyword for the database table column of the floodplain polysegments: flag if the polysegment is a coast-line [-] (Hyd_Floodplain_Polysegment)
	const string polyseg_coast("COAST");
	///Keyword for the database table column of the floodplain polysegments: name of the polysegment [-] (Hyd_Floodplain_Polysegment)
	const string polyseg_name("NAME");


	
	///Keyword for the database table name of the floodplain polysegment points (Hyd_Floodplain_Polysegment) like dikelines or coastline
	const string tab_fp_polyseg_point("HYD_FLOODPLAIN_POLYSEG_POINT");

	///Keyword for the database table name of the floodplain polygons (Hyd_Floodplain_Polygon) like noflow-polygons
	const string tab_fp_polygon("HYD_FLOODPLAIN_POLYGON");
	///Keyword for the database table column of the floodplain polygons: global identifier of the data records [-] (Hyd_Floodplain_Polygon)
	const string polygon_glob_id("POLYGON_GLOB_ID");
	///Keyword for the database table column of the floodplain polygons: the global identifier of the points of the polygon [-] (Hyd_Floodplain_Polygon)
	const string polygon_point_glob_id("POINT_GLOB_ID");
	///Keyword for the database table column of the floodplain polygons: the identifier of the points of the polygon [-] (Hyd_Floodplain_Polygon)
	const string polygon_point_id("POINT_ID");
	///Keyword for the database table column of the floodplain polygons: x-coordinate of the polygon point [m] (Hyd_Floodplain_Polygon)
	const string polygon_point_x("POINT_X");
	///Keyword for the database table column of the floodplain polygons: y-coordinate of the polygon point [m] (Hyd_Floodplain_Polygon)
	const string polygon_point_y("POINT_Y");
	///Keyword for the database table column of the floodplain polygons: geo data type for displaing the polygon [-] (Hyd_Floodplain_Polygon)
	const string polygon_out("GEO_POLYGON");

	///Keyword for the database table name of the floodplain polygon points (Hyd_Floodplain_Polygon) like noflow-polygons
	const string tab_fp_polygon_point("HYD_FLOODPLAIN_POLYGON_POINT");

	///Keyword for the database table column for an hydraulic coupling structure: type of structure (gate/weir) [-] (Hyd_Coupling_RV2FP_Structure, Hyd_Coupling_RV2RV_Diversion, _Hyd_Coupling_Structure)
	const string struct_hyd_type("TYPE");
	///Keyword for the database table column for an hydraulic coupling structure: sill width [m] (Hyd_Coupling_RV2FP_Structure, Hyd_Coupling_RV2RV_Diversion, _Hyd_Coupling_Structure)
	const string struct_hyd_width("WIDTH");
	///Keyword for the database table column for an hydraulic coupling structure: flag if the structure is controlled or uncontrolled [m] (Hyd_Coupling_RV2FP_Structure, Hyd_Coupling_RV2RV_Diversion, _Hyd_Coupling_Structure)
	const string struct_hyd_control("CONTROLABLE");
	///Keyword for the database table column for an hydraulic coupling structure: controlled paramter; it is depending to the type; WEIR type: the local sill height [m]; GATE type: the local opening height [m] (Hyd_Coupling_RV2FP_Structure, Hyd_Coupling_RV2RV_Diversion, _Hyd_Coupling_Structure)
	const string struct_hyd_control_param("CONTROLED_PARAM");
	///Keyword for the database table column for an hydraulic coupling structure: uncontrolled paramter; it is depending to the type; WEIR type: the Poleni Factor [-]; GATE type: the local sill height [m] (Hyd_Coupling_RV2FP_Structure, Hyd_Coupling_RV2RV_Diversion, _Hyd_Coupling_Structure)
	const string struct_hyd_uncontrol_param("UNCONTROLED_PARAM");
	///Keyword for the database table column for an hydraulic coupling structure: geometrical point of the structure location at the river line (Hyd_Coupling_RV2FP_Structure, Hyd_Coupling_RV2RV_Diversion, _Hyd_Coupling_Structure)
	const string struct_hyd_point("GEO_POINT");

	///Keyword for the database table name of the diversion channel data (Hyd_Coupling_RV2RV_Diversion)
	const string tab_coup_dv_channel("HYD_COUPLING_DIVERSION_CHANNEL");
	///Keyword for the database table column of the diversion channel data: global identifier of the data records [-] (Hyd_Coupling_RV2RV_Diversion)
	const string coup_dv_glob_id("CHANNEL_GLOB_ID");
	///Keyword for the database table column of the diversion channel data: identifier of the river model representing the channel [-] (Hyd_Coupling_RV2RV_Diversion)
	const string coup_dv_rv_id("DIVERSION_CHANNEL_RIVER_ID");
	///Keyword for the database table column of the diversion channel data: identifier of the river model, to which the inflow of the channel is connected [-] (Hyd_Coupling_RV2RV_Diversion)
	const string coup_dvin_rv_id("IN_RIVER_ID");
	///Keyword for the database table column of the diversion channel data: identifier of the river model, to which the outflow of the channel is connected  [-] (Hyd_Coupling_RV2RV_Diversion)
	const string coup_dvout_rv_id("OUT_RIVER_ID");

	///Keyword for the database table name of the hydraulic coupling structure data (Hyd_Coupling_RV2FP_Structure)
	const string tab_coup_structure("HYD_COUPLING_STRUCTURE");
	///Keyword for the database table column of the hydraulic coupling structure data: global identifier of the data records [-] (Hyd_Coupling_RV2FP_Structure)
	const string coup_struct_glob_id("STRUCTURE_GLOB_ID");
	///Keyword for the database table column of the hydraulic coupling structure data: identifier of the river model [-] (Hyd_Coupling_RV2FP_Structure)
	const string coup_struct_rv_id("RIVER_ID");
	///Keyword for the database table column of the hydraulic coupling structure data: identifier for the river profile, to which the distance of the structure's midpoint is related[-] (Hyd_Coupling_RV2FP_Structure)
	const string coup_struct_prof_id("RIVER_PROFILE_ID");
	///Keyword for the database table column of the hydraulic coupling structure data: distance related to the given river profile, where the the structure's midpoint is located [m] (Hyd_Coupling_RV2FP_Structure)
	const string coup_struct_rel_dist("RELATIVE_DISTANCE");
	///Keyword for the database table column of the hydraulic coupling structure data: the river bank in flow direction, where the structure is located (true:=left river bank; false:= right river bank) [-] (Hyd_Coupling_RV2FP_Structure)
	const string coup_struct_left_bank("LEFT_BANK");

	///Keyword for the database table name of the dike-/wallbreak data for a coupling (Hyd_Coupling_RV2FP_Dikebreak)
	const string tab_coup_break("HYD_COUPLING_BREAK");
	///Keyword for the database table column of the dike-/wallbreak data: global identifier of the data records [-] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_glob_id("BREAK_GLOB_ID");
	///Keyword for the database table column of the dike-/wallbreak data: identifier of the river model [-] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_rv_id("RIVER_ID");
	///Keyword for the database table column of the dike-/wallbreak data: identifier for the station (river profile/point of dikeline), to which the distance of the break-start point is related[-] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_prof_id("STATION_ID");
	///Keyword for the database table column of the dike-/wallbreak data: distance related to the given river profile, where the break starts [m] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_rel_dist("RELATIVE_DISTANCE");
	///Keyword for the database table column of the dike-/wallbreak data: the river bank in flow direction, where the break is located (true:=left river bank; false:= right river bank) [-] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_left_bank("LEFT_BANK");
	///Keyword for the database table column of the dike-/wallbreak data: the relatvie start waterlevel (related to the corresponding base point), when the break starts [m] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_start_h("START_HEIGHT");
	///Keyword for the database table column of the dike-/wallbreak data: maximum possible break width [m] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_max_w("MAX_BREAK_WIDTH");
	///Keyword for the database table column of the dike-/wallbreak data: geometrical point of breach starting point at the river line (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_point("GEO_POINT");

	///Keyword for the database table name of the dike-/wallbreak result data for a coupling (Hyd_Coupling_RV2FP_Dikebreak)
	const string tab_coup_break_result("HYD_COUPLING_BREAK_RESULT");
	///Keyword for the database table column of the dike-/wallbreak result data: Flag if the breach is user defined [-] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_user_defined("USER_DEFINED");
	///Keyword for the database table column of the dike-/wallbreak result data: Start time [s] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_start_time("START_TIME");
	///Keyword for the database table column of the dike-/wallbreak result data: Total breach width [m] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_width_total("WIDTH_TOTAL");
	///Keyword for the database table column of the dike-/wallbreak result data: Upstream breach width from the starting point [m] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_width_upstream("WIDTH_UPSTREAM");
	///Keyword for the database table column of the dike-/wallbreak result data: Downstream breach width from the starting point [m] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_width_downstream("WIDTH_DOWNSTREAM");
	///Keyword for the database table column of the dike-/wallbreak result data: Upstream breach has stopped [-] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_upstream_stop("UPSTREAM_STOP");
	///Keyword for the database table column of the dike-/wallbreak result data: Downstream breach has stopped [-] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_downstream_stop("DOWNSTREAM_STOP");
	///Keyword for the database table column of the dike-/wallbreak result data: Maximum breach velocity (absolute) [m/s] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_max_v("MAX_BREACH_V");
	///Keyword for the database table column of the dike-/wallbreak result data: Time of maximum breach velocity  [s] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_max_v_time("TIME_MAX_BREACH_V");
	///Keyword for the database table column of the dike-/wallbreak result data: Total volume through the breach  [m³] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_total_volume("TOTAL_VOLUME");
	///Keyword for the database table column of the dike-/wallbreak result data: Volume through the breach to the floodplain  [m³] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_volume_fp("VOL2FP");
	///Keyword for the database table column of the dike-/wallbreak result data: Volume through the breach to river  [m³] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_volume_rv("VOL_OUT_FP");
	///Keyword for the database table column of the dike-/wallbreak result data: x-coordinate of the starting point  [m] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_x_start("START_X");
	///Keyword for the database table column of the dike-/wallbreak result data: y-coordinate of the starting point  [m] (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_y_start("START_Y");
	///Keyword for the database table column of the dike-/wallbreak data: geometrical polyline of the breach (Hyd_Coupling_RV2FP_Dikebreak)
	const string coup_break_polyline("GEO_POLYLINE");

	///Keyword for the database table name of the boundary szenarios (Hyd_Boundary_Szenario_Management)
	const string tab_sz_bound("HYD_BOUND_SCENARIO");
	///Keyword for the database table column of the boundary szenario data: global identifier of the boundary szenario [-] (Hyd_Boundary_Szenario_Management, Hyd_Boundary_Szenario)
	const string sz_bound_id("BOUNDARY_SCENARIO_ID");
	///Keyword for the database table column of the boundary szenario data: the annuality of the boundary szenario [a] (Hyd_Boundary_Szenario_Management, Hyd_Boundary_Szenario)
	const string sz_bound_anu("ANNUALITY");
	///Keyword for the database table column of the boundary szenario data: occurrence probability of the form-event [-] (Hyd_Boundary_Szenario_Management, Hyd_Boundary_Szenario)
	const string sz_bound_prob_form("PROBA_FORM");
	///Keyword for the database table column of the boundary szenario data: occurrence probability of the form-event [-] (Hyd_Boundary_Szenario_Management, Hyd_Boundary_Szenario)
	const string sz_bound_prob_max("PROBA_MAX_VALUE");
	///Keyword for the database table column of the boundary szenario data: name of the szenario [-] (Hyd_Boundary_Szenario_Management, Hyd_Boundary_Szenario)
	const string sz_bound_name("NAME");


	///Output for Tecplot
	const string tecplot("tecplot");
	///Output for BlueKenue
	const string bluekenue("bluekenue");
	///Output for ParaView / csv
	const string paraview("paraview");

	///File type .dat (tecplot)
	const string dat(".dat");
	///File type .r2s (bluekenue)
	const string r2s(".r2s");
	///File type .csv (paraview)
	const string csv(".csv");
	///File type .vtk (paraview)
	const string vtk(".vtk");

}
///General text labels specially for the module DAM \ingroup dam
namespace dam_label{

	///String for the impact type: waterlevel [m] (_dam_impact_type, Dam_Impact_Values)
	const string impact_h("IMPACT_H");
	///String for the impact type: flow velocity [m/s] (_dam_impact_type, Dam_Impact_Values)
	const string impact_v("IMPACT_V");
	///String for the impact type: wet duration [s] (_dam_impact_type, Dam_Impact_Values)
	const string impact_d("IMPACT_D");
	///String for the impact type: product of waterlevel times flow velocity [s] (_dam_impact_type, Dam_Impact_Values)
	const string impact_vh("IMPACT_VH");
	///String for the impact type: wet or dry [-] (_dam_impact_type, Dam_Impact_Values)
	const string impact_wet_dry("WET_DRY");

	///Keyword for the file input of the damage raster: begin of the damage raster informations [-] (Dam_Raster)
	const string begin_rasterinfo("!$BEGIN_RASTERINFO");
	///Keyword for the file input of the damage raster: end of the damage raster informations [-] (Dam_Raster)
	const string end_rasterinfo("!$END_RASTERINFO");

	///Keyword for the file input of the damage raster: number of elements in x direction; the keyword is adapted from the ArcGis output of raster2ascii (Dam_Raster)
	const string raster_no_x("!ncols");
	///Keyword for the file input of the damage raster: number of elements in y direction; the keyword is adapted from the ArcGis output of raster2ascii (Dam_Raster)
	const string raster_no_y("!nrows");
	///Keyword for the file input of the damage raster: x coordinate of raster origin [m]; the keyword is adapted from the ArcGis output of raster2ascii (Dam_Raster)
	const string raster_x_origin("!xllcorner");
	///Keyword for the file input of the damage raster: y coordinate of raster origin [m]; the keyword is adapted from the ArcGis output of raster2ascii (Dam_Raster)
	const string raster_y_origin("!yllcorner");
	///Keyword for the file input of the damage raster: cellsize [m]; the keyword is adapted from the ArcGis output of raster2ascii (Dam_Raster)
	const string raster_cellsize("!cellsize");
	///Keyword for the file input of the damage raster: value if no information are available [-]; the keyword is adapted from the ArcGis output of raster2ascii (Dam_Raster)
	const string raster_noinfo("!NODATA_value");
	///Keyword for the file input of the damage raster: name of the raster [-] (Dam_Raster)
	const string raster_name("!name");
	///Keyword for the file input of the damage raster: type of the raster [-]; the keyword is adapted from the ArcGis output of raster2ascii [-] (Dam_Raster)
	const string raster_type("!type");

	///Keyword for the file input of the damage raster (just simple counting damages): name of the simple counting categroy [-]; (Dam_Sc_Raster)
	const string sc_category_name("!category");

	///Keyword for the file input of the damage polygons: number of polygons in file[-]; (Dam_Damage_System)
	const string number_polygons("!no_polygons");

	///Keyword of the raster-types: economical raster with the information about the immobile land-use type [-] (Dam_Ecn_Raster)
	const string rast_ecn_immob("ecn_immob");
	///Keyword of the raster-types: economical raster with the information about the mobile land-use type [-] (Dam_Ecn_Raster)
	const string rast_ecn_mob("ecn_mob");
	///Keyword of the raster-types: economical raster with the information about the individual immobile stock-values [€/m²] (Dam_Ecn_Raster)
	const string rast_ecn_immob_stock("ecn_immob_stock");
	///Keyword of the raster-types: economical raster with the information about the individual mobile stock-values [€/m²] (Dam_Ecn_Raster)
	const string rast_ecn_mob_stock("ecn_mob_stock");
	///Keyword of the raster-types: economical raster with all required information for the damage calculation (Dam_Ecn_Raster)
	const string rast_ecn_total("ecn_total");

	///Keyword of the raster-types: ecological raster with the information about the biotope-type [-] (Dam_Eco_Btype_Raster)
	const string rast_eco_btype("eco_btype");
	///Keyword of the raster-types: ecological raster with the information about the soil-cost type [-] (Dam_Eco_Soil_Raster)
	const string rast_eco_soil_cost("eco_soil_cost");
	///Keyword of the raster-types: ecological raster with the information about the soil-erosion type [-] (Dam_Eco_Soil_Raster)
	const string rast_eco_soil_erosion("eco_soil_erosion");
	///Keyword of the raster-types: ecological raster with all required information for the soil-erosion damage calculation (Dam_Eco_Soil_Raster)
	const string rast_eco_soil_total("eco_soil_total");

	///Keyword of the raster-types: people2risk raster with the information about the damage category [-] (Dam_People_Raster)
	const string rast_pop_dam_category("pop_dam_category");
	///Keyword of the raster-types: people2risk/psycho-social raster raster with the information about the population density [P/m²] (Dam_Pys_Raster)
	const string rast_pop_density("pop_density");
	///Keyword of the raster-types: people2risk raster with all required information for the people2risk damage calculation (Dam_People_Raster)
	const string rast_pop_total("pop_total");


	///Keyword of the raster-types: psycho-social raster with the information category [-] (Dam_Pys_Raster)
	const string rast_pys_category("pys_category");
	///Keyword of the raster-types: psycho-social raster with the information about the criteria age class 50 to 59 years  [%] (Dam_Pys_Raster)
	const string rast_pys_crit_age50_49("pys_crit_age50_59");
	///Keyword of the raster-types: psycho-social raster with the information about the criteria age class greater 80 years  [%] (Dam_Pys_Raster)
	const string rast_pys_crit_age80("pys_crit_age80");
	///Keyword of the raster-types: psycho-social raster with the information about the criteria female gender  [%] (Dam_Pys_Raster)
	const string rast_pys_crit_female("pys_crit_female");
	///Keyword of the raster-types: psycho-social raster with the information about the criteria home owners  [%] (Dam_Pys_Raster)
	const string rast_pys_crit_owner("pys_crit_owner");

	///Keyword of the raster-types: psycho-social raster with all required information for the psycho-social damage calculation (Dam_Pys_Raste)
	const string rast_pys_total("pys_total");

	///Keyword of the raster-types: relative waterlevel [m] for the directly given hydraulic results (Dam_Raster)
	const string rast_hyd_waterlevel("hyd_waterlevel");
	///Keyword of the raster-types: geodetic height [m] for the directly given hydraulic results (Dam_Raster)
	const string rast_hyd_geo_height("hyd_height");
	///Keyword of the raster-types: maximum flow velocity [m/s] for the directly given hydraulic results (Dam_Raster)
	const string rast_hyd_max_v("hyd_velocity");
	///Keyword of the raster-types: duration of flood [s] for the directly given hydraulic results (Dam_Raster)
	const string rast_hyd_duration("hyd_duration");
	///Keyword of the raster-types: speed of waterlevel rise [m/s] for the directly given hydraulic results (Dam_Raster)
	const string rast_hyd_dh_dt("hyd_dhdt");

	///Keyword for the file input of the damage raster: begin of the damage raster elements characteristics [-] (Dam_Raster)
	const string begin_charac("!$BEGIN_CHARAC");
	///Keyword for the file input of the damage raster: end of the damage raster elements characteristics [-] (Dam_Raster)
	const string end_charac("!$END_CHARAC");


	///Keyword for the database table name of the economical raster data (Dam_Ecn_Raster)
	const string tab_ecn_raster("DAM_ECN_RASTER");
	///Keyword for the database table name of the ecological raster of the biotope-types data (Dam_Eco_Btype_Raster)
	const string tab_eco_btype_raster("DAM_ECO_BTYPE_RASTER");
	///Keyword for the database table name of the ecological raster of the soil-erosion-types data (Dam_Eco_Soil_Raster)
	const string tab_eco_soil_raster("DAM_ECO_SOIL_RASTER");
	///Keyword for the database table name of the people2risk raster data (Dam_People_Raster)
	const string tab_pop_raster("DAM_POP_RASTER");
	///Keyword for the database table name of the simple-counting raster data (Dam_Sc_Raster)
	const string tab_sc_raster("DAM_SC_RASTER");
	///Keyword for the database table name of the psycho-social raster data (Dam_Pys_Raster)
	const string tab_pys_raster("DAM_PYS_RASTER");

	///Keyword for the database table column of the damage raster data: global identifier of the raster [-] (Dam_Raster and child-classes)
	const string raster_id("RASTER_ID");
	///Keyword for the database table column of the damage raster data: name of the raster [-] (Dam_Raster and child-classes)
	const string raster_name_db("NAME");
	///Keyword for the database table column of the damage raster data: number of elements in x-direction [-] (Dam_Raster and child-classes)
	const string raster_no_x_db("NX");
	///Keyword for the database table column of the damage raster data: number of elements in y-direction [-] (Dam_Raster and child-classes)
	const string raster_no_y_db("NY");
	///Keyword for the database table column of the damage raster data: x-coordinate of the origin [m] (Dam_Raster and child-classes)
	const string raster_x_origin_db("LOW_LEFT_X");
	///Keyword for the database table column of the damage raster data: y-coordinate of the origin [m] (Dam_Raster and child-classes)
	const string raster_y_origin_db("LOW_LEFT_Y");
	///Keyword for the database table column of the damage raster data: size of the raster; each raster element is a quadrat [m] (Dam_Raster and child-classes)
	const string raster_cellsize_db("ELEMENT_SIZE");
	///Keyword for the database table column of the damage raster data: no-info value for the raster elements [-] (Dam_Raster and child-classes)
	const string raster_noinfo_db("NOINFO_VALUE");
	///Keyword for the database table column of the damage raster data: the boundary of the raster as a polygon [-] (Dam_Raster and child-classes)
	const string raster_polygon("GEO_POLYGON");
	///Keyword for the database table column of the damage raster data: the raster is connected [-] (Dam_Raster and child-classes)
	const string raster_connected("CONNECTED");


	///Keyword for the database table name of the economical element raster data (Dam_Ecn_Element)
	const string tab_ecn_element("DAM_ECN_ELEMENTS");

	///Keyword for the database table column of the element data of a raster: global identifier of the data records [-] (_Dam_Element and child-classes) 
	const string glob_id("GLOB_ID");
	///Keyword for the database table column of the element data of a raster: local identifier of the raster elements [-] (_Dam_Element and child-classes)
	const string elem_id("ELEMENT_ID");
	///Keyword for the database table column of the element data of a raster: x-coordinate of the mid-point of the element [-] (_Dam_Element and child-classes)
	const string elem_mid_x("MID_X");
	///Keyword for the database table column of the element data of a raster: y-coordinate of the mid-point of the element [-] (_Dam_Element and child-classes)
	const string elem_mid_y("MID_Y");
	///Keyword for the database table column of the element data of a raster: the element as a polygon [-] (_Dam_Element and child-classes)
	const string elem_poly("GEO_POLYGON");
	///Keyword for the database table column of the element data of a raster: the identifier of the connected floodplain [-] (_Dam_Element and child-classes)
	const string conn_fp_id("FP_ID");
	///Keyword for the database table column of the element data of a raster: the identifier of the connected floodplain element [-] (_Dam_Element and child-classes)
	const string conn_fp_elem_id("ELEM_FP_ID");
	///Keyword for the database table column of the element data of a raster: the area of the element [m²] (_Dam_Element and child-classes)
	const string area_elem("AREA");

	///Keyword for the database table column of the element data of a economical raster: identifier for the immobile damage-function [-] (Dam_Ecn_Damage_Function, Dam_Ecn_Element)
	const string immob_id("IMMOB_ID");
	///Keyword for the database table column of the element data of a economical raster: identifier for the mobile damage-function [-] (Dam_Ecn_Damage_Function, Dam_Ecn_Element)
	const string mob_id("MOB_ID");
	///Keyword for the database table column of the element data of a economical raster: flag if the element is an no-info element concerning immobile damages [-] (Dam_Ecn_Element)
	const string no_immob_id("IMMOB_NOINFO");
	///Keyword for the database table column of the element data of a economical raster: flag if the element is an no-info element concerning mobile damages [-] (Dam_Ecn_Element)
	const string no_mob_id("MOB_NOINFO");
	///Keyword for the database table column of the element data of a economical raster: individual stock-value for the immobile damages [-] (Dam_Ecn_System)
	const string immob_stock("IMMOB_STOCK_VALUE");
	///Keyword for the database table column of the element data of a economical raster: individual stock-value for the mobile damages [-] (Dam_Ecn_System)
	const string mob_stock("MOB_STOCK_VALUE");


	///Keyword for the database table name of the damage results (Dam_Damage_System)
	const string tab_dam_erg("DAM_RESULTS");

	///Keyword for the database table name of the damage systems (Dam_Damage_System)
	const string tab_dam_system("DAM_SYSTEM");
	///Keyword for the database table column of the Damage system: maximum timesteps for instationary calculation [-] (Dam_Damage_System)
	const string max_time_step("MAX_TIME_STEPS");
	///Keyword for the database table column of the Damage system: instationary calculation CI-damage calculation until all elements are active [-] (Dam_Damage_System)
	const string all_active_flag("ALL_ACTIVE_FLAG");

	///Keyword for the database table name of the economical damage results (Dam_Ecn_Element)
	const string tab_ecn_erg("DAM_ECN_RESULTS");
	///Keyword for the database table column of the element results of a economical raster: immobile damages [€] (Dam_Ecn_System)
	const string immob_dam("IMMOB");
	///Keyword for the database table column of the element results of a economical raster: immobile damages 5%-quantile [€] (Dam_Ecn_System)
	const string immob_5_quant("IMMOB5_QUANTILE");
	///Keyword for the database table column of the element results of a economical raster: immobile damages 95%-quantile [€] (Dam_Ecn_System)
	const string immob_95_quant("IMMOB95_QUANTILE");
	///Keyword for the database table column of the element results of a economical raster: mobile damages [€] (Dam_Ecn_System)
	const string mob_dam("MOB");
	///Keyword for the database table column of the element results of a economical raster: mobile damages 5%-quantile [€] (Dam_Ecn_System)
	const string mob_5_quant("MOB5_QUANTILE");
	///Keyword for the database table column of the element results of a economical raster: mobile damages 95%-quantile [€] (Dam_Ecn_System)
	const string mob_95_quant("MOB95_QUANTILE");

	///Keyword for the database table column of the element results of a economical raster: total damages [€] (Dam_Ecn_System)
	const string total_dam("TOTAL");

	///Keyword for the database table name of the economical damage functions (Dam_Ecn_Damage_Function)
	const string tab_ecn_func("DAM_ECN_FUNCT");
	///Keyword for the database table column of the functions used in the damage calculation: global identifier of the function [-] (Dam_Ecn_Damage_Function, _Dam_2value_Function, Dam_People_Damage_Function )
	const string function_id("FUNCTION_ID");
	///Keyword for the database table column of the functions used in the damage calculation: keyword for the impact-type of the function [-] (Dam_Ecn_Damage_Function, _Dam_2value_Function, Dam_People_Damage_Function )
	const string impact_type("IMPACT_TYPE");
	///Keyword for the database table column of the functions used in the damage calculation: flag for a stepwise calculations of the function [-] (Dam_Ecn_Damage_Function, _Dam_2value_Function, Dam_People_Damage_Function )
	const string stepwise("STEPWISE");

	///Keyword for the database table column of the economical damage functions: Identifier of the landuse-category [-] (Dam_Ecn_Damage_Function)
	const string landuse_id("LANDUSE_ID");
	///Keyword for the database table column of the economical damage functions: Name of the landuse-category [-] (Dam_Ecn_Damage_Function)
	const string landuse_name("LANDUSE_NAME");
	///Keyword for the database table column of the economical damage functions: Default stockvalue of the landuse-category [€/m²] (Dam_Ecn_Damage_Function)
	const string stock_value("STOCK_VALUE");
	///Keyword for the database table column of the economical damage functions: 5%-quantile of the default stockvalue of the landuse-category [€/m²] (Dam_Ecn_Damage_Function)
	const string stock_5_quant("STOCK5_QUANTILE");
	///Keyword for the database table column of the economical damage functions: 95%-quantile of the default stockvalue of the landuse-category [€/m²] (Dam_Ecn_Damage_Function)
	const string stock_95_quant("STOCK95_QUANTILE");
	
	///Keyword for the database table name of the points of the economical damage functions (Dam_Ecn_Damage_Function)
	const string tab_ecn_func_point("DAM_ECN_FUNCT_POINT");
	///Keyword for the database table column of the function points used in the damage calculation: local identifier of the function points [-] (Dam_Ecn_Damage_Function, _Dam_2value_Function, Dam_People_Damage_Function )
	const string point_id("POINT_ID");
	///Keyword for the database table column of the function points used in the damage calculation: impact value; x-coordinates of the functions points [-] (Dam_Ecn_Damage_Function, _Dam_2value_Function, Dam_People_Damage_Function )
	const string impact_value("IMPACT_VALUE");

	///Keyword for the database table column of the function points used in the economical/ ecological biotope-type damage calculation: relative damages; y-coordinates of the functions points [%] (Dam_Ecn_Damage_Function, Dam_Eco_Btype_Function)
	const string rel_damage("REL_DAMAGE");
	///Keyword for the database table column of the function points used in the economical damage calculation: 5% quantile of the relative damages [%] (Dam_Ecn_Damage_Function)
	const string rel_damage_5_quant("REL_DAM5_QUANTILE");
	///Keyword for the database table column of the function points used in the economical damage calculation: 95% quantile of the relative damages [%] (Dam_Ecn_Damage_Function)
	const string rel_damage_95_quant("REL_DAM95_QUANTILE");

	///Keyword for the database table name of the ecological soil-erosion raster element data (Dam_Eco_Soil_Element)
	const string tab_eco_soil_element("DAM_ECO_SOIL_ELEMENT");
	///Keyword for the database table column of the ecological soil-erosion raster element data: soil-type [-] (Dam_Eco_Soil_Element)
	const string soil_type("SOIL_TYPE");
	///Keyword for the database table column of the ecological soil-erosion raster element data: flag if the element is an no-info element concerning the soil-type [-] (Dam_Eco_Soil_Element)
	const string no_soil_type("NOINFO_SOIL_TYPE");
	///Keyword for the database table column of the ecological soil-erosion raster element data: erosion-type [-] (Dam_Eco_Soil_Element)
	const string erosion_type("ERO_TYPE");
	///Keyword for the database table column of the ecological soil-erosion raster element data: flag if the element is an no-info element concerning the erosion-type [-] (Dam_Eco_Soil_Element)
	const string no_erosion_type("NOINFO_ERO_TYPE");

	///Keyword for the database table name of the ecological soil-erosion raster element data (Dam_Eco_Btype_Element)
	const string tab_eco_btype_element("DAM_ECO_BTYPE_ELEMENT");
	///Keyword for the database table column of the ecological biotope-type raster element data: biotope-type [-] (Dam_Eco_Btype_Element)
	const string biotope_type("B_TYPE");
	///Keyword for the database table column of the ecological biotope-type raster element data: flag if the element is an no-info element concerning the biotope-type [-] (Dam_Eco_Btype_Element)
	const string no_biotope_type("NOINFO_B_TYPE");

	///Keyword for the database table name of the people2risk raster element data (Dam_People_Element)
	const string tab_pop_element("DAM_POP_ELEMENT");
	///Keyword for the database table column of the people2risk raster element data/ psycho-social element raster data: population density [P/m²] (Dam_People_Element, Dam_Pys_Element)
	const string pop_density("POP_DENSITY");
	///Keyword for the database table column of the people2risk raster element data/ psycho-social element raster data: flag if the element is an no-info element concerning the population density [-] (Dam_People_Element, Dam_Pys_Element)
	const string no_pop_density("NOINFO_DENSITY");
	///Keyword for the database table column of the people2risk raster element data: flag if the element is an no-info element concerning poeple2risk type[-] (Dam_People_Element)
	const string pop2risk_type("POP_RISK_TYPE");
	///Keyword for the database table column of the people2risk raster element data: poeple2risk type[-] (Dam_People_Element)
	const string no_pop2risk_type("NOINFO_RISK_TYPE");

	///Keyword for the database table name of the ecological soil-erosion damage results (Dam_Eco_Soil_Element)
	const string tab_eco_soil_erg("DAM_ECO_SOIL_RESULTS");
	///Keyword for the database table column of the ecological soil-erosion damage results: soil-erosion costs [€] (Dam_Eco_Soil_Element)
	const string soil_cost("SOIL_COST");
	///Keyword for the database table column of the ecological soil-erosion damage results: soil-erosion damage zone [-] (Dam_Eco_Soil_Element)
	const string soil_erosion_zone("ERO_ZONE");


	///Keyword for the database table name of the ecological biotope-type damage results (Dam_Eco_Btype_Element)
	const string tab_eco_btype_erg("DAM_ECO_BTYPE_RESULT");
	///Keyword for the database table column of the ecological biotope-type damage results: biotope-type costs [€] (Dam_Eco_Btype_Element)
	const string btype_cost("BTYPE");
	///Keyword for the database table column of the ecological biotope-type damage results: biotope-type damage zone [-] (Dam_Eco_Btype_Element)
	const string btype_damage_zone("BTYPE_DAMAGE_ZONE");

	///Keyword for the database table name of the people2risk damage results (Dam_People_Element)
	const string tab_pop_erg("DAM_POP_RESULT");
	///Keyword for the database table column of the people2risk damage results: affected population [P] (Dam_People_Element)
	const string pop_affected("POP_AFFECTED");
	///Keyword for the database table column of the people2risk damage results: endangered population [P] (Dam_People_Element)
	const string pop_endangered("POP_ENDANGERED");
	///Keyword for the database table column of the people2risk damage results: endangered zone [-] (Dam_People_Element)
	const string zone_endangered("ZONE_ENDANGERED");

	///Keyword for the database table name of the people2risk damage functions (Dam_People_Damage_Function)
	const string tab_pop_func("DAM_POP_FUNCT");
	///Keyword for the database table column of the people2risk damage functions: Identifier of the people2risk type [-] (Dam_People_Damage_Function)
	const string pop_type_id("TYPE_ID");
	///Keyword for the database table column of the people2risk damage functions: Name of the people2risk type [-] (Dam_People_Damage_Function)
	const string pop_type_name("TYPE_NAME");

	///Keyword for the database table name of the points of the people2risk damage functions (Dam_People_Damage_Function)
	const string tab_pop_func_point("DAM_POP_FUNCT_POINT");
	///Keyword for the database table column of the function points used in the people2risk damage calculation: relative damages in zone 1; y-coordinates of the functions points [%] (Dam_People_Damage_Function)
	const string rel_dam_zone_1("DAMAGE_ZONE_1");
	///Keyword for the database table column of the function points used in the people2risk damage calculation: relative damages in zone 2; y-coordinates of the functions points [%] (Dam_People_Damage_Function)
	const string rel_dam_zone_2("DAMAGE_ZONE_2");
	///Keyword for the database table column of the function points used in the people2risk damage calculation: relative damages in zone 3; y-coordinates of the functions points [%] (Dam_People_Damage_Function)
	const string rel_dam_zone_3("DAMAGE_ZONE_3");

	///Keyword for the database table name of the ecological soil-erosion functions (Dam_Eco_Soil_Erosion_Function)
	const string tab_eco_soil_ero_func("DAM_ECO_SOIL_ERO_FUNCT");
	///Keyword for the database table column of the ecological soil-erosion functions: Identifier of the erosion-type [-] (Dam_Eco_Soil_Erosion_Function)
	const string ero_type_id("TYPE_ID");
	///Keyword for the database table column of the ecological soil-erosion functions: Name of the erosion-type [-] (Dam_Eco_Soil_Erosion_Function)
	const string ero_type_name("TYPE_NAME");

	///Keyword for the database table name of the ecological soil-erosion function points (Dam_Eco_Soil_Erosion_Function)
	const string tab_eco_soil_ero_func_point("DAM_ECO_SOIL_ERO_FUNCT_POINT");
	///Keyword for the database table column of the ecological soil-erosion functions points: Erosion value; it is the y-coordinate of the function [-] (Dam_Eco_Soil_Erosion_Function)
	const string erosion_value("EROSION_VALUE");

	///Keyword for the database table name of the ecological soil-cost functions (Dam_Eco_Soil_Cost_Function)
	const string tab_eco_soil_cost_func("DAM_ECO_SOIL_COST_FUNCT");
	///Keyword for the database table column of the ecological soil-cost functions: Identifier of the soil-type [-] (Dam_Eco_Soil_Cost_Function)
	const string soil_type_id("TYPE_ID");
	///Keyword for the database table column of the ecological soil-cost functions: Name of the soil-type [-] (Dam_Eco_Soil_Cost_Function)
	const string soil_type_name("TYPE_NAME");

	///Keyword for the database table name of the ecological soil-cost function points (Dam_Eco_Soil_Cost_Function)
	const string tab_eco_soil_cost_func_point("DAM_ECO_SOIL_COST_FUNCT_POINT");
	///Keyword for the database table column of the ecological soil-cost functions points: Erosion value; it is the y-coordinate of the function [-] (Dam_Eco_Soil_Cost_Function)
	const string cost_value("COST_VALUE");

	///Keyword for the database table name of the ecological biotope-types (Dam_Eco_Btype)
	const string tab_eco_btype("DAM_ECO_BIOTOPE");
	///Keyword for the database table column of the ecological biotope-types: Identifier of the biotope-type [-] (Dam_Eco_Btype)
	const string biotope_type_id("TYPE_ID");
	///Keyword for the database table column of the ecological biotope-types: Name of the biotope-type [-] (Dam_Eco_Btype)
	const string biotope_type_name("TYPE_NAME");
	///Keyword for the database table column of the ecological biotope-types: Value of the biotope-type [€/m²] (Dam_Eco_Btype)
	const string biotope_type_value("TYPE_VALUE");

	///Keyword for the database table name of the ecological biotope-type functions (Dam_Eco_Btype_Function)
	const string tab_eco_btype_func("DAM_ECO_BTYPE_FUNCTION");
	///Keyword for the database table column of the ecological biotope-type functions: Weight of the function [-] (Dam_Eco_Btype_Function)
	const string func_weight("FUNCTION_WEIGHT");

	///Keyword for the database table name of the ecological biotope-type function points (Dam_Eco_Btype_Function)
	const string tab_eco_btype_func_point("DAM_ECO_BTYPE_FUNCT_POINT");

	///Keyword for the database table name of the simple-counting damage subcategories (Dam_Sc_Category)
	const string tab_sc_subcategory("DAM_SC_SUBCATEGORY");

	///Keyword for the database table name of the psyacho-social damage categories (Dam_Pys_Category)
	const string tab_pys_category("DAM_PYS_CATEGORY");

	///Keyword for the database table column of the category for the simple-counting /psycho-social damage calculation: Identifier of the category [-] (Dam_Sc_Point, Dam_Pys_Category)
	const string category_id("CAT_ID");
	///Keyword for the database table column of the category for the simple-counting /psycho-social damage calculation: Name of the category [-] (Dam_Sc_Point, Dam_Pys_Category)
	const string category_name("CAT_NAME");

	///Keyword for the database table column of the sector for CI damage calculation: Identifier of the sector[-] (Dam_CI_Point, Dam_CI_Polygon)
	const string sector_id("SEC_ID");
	///Keyword for the database table column of the sector for CI damage calculation: Number of end user in polygon [dependent to sector] (Dam_CI_Polygon)
	const string end_user("END_USER");
	///Keyword for the database table column of the category for the CI damage calculation: Name of the sector [-] (Dam_CI_Point, Dam_CI_Polygon)
	const string sector_name("SEC_NAME");
	///Keyword for the database table column of the category for the CI damage calculation: Level in the sector [-] (Dam_CI_Point, Dam_CI_Polygon)
	const string sector_level("SEC_LEVEL");
	///Keyword for the database table column of the category for the CI damage calculation: if the CI-Element is end of the cascade [-] (Dam_CI_Point, Dam_CI_Polygon)
	const string final_flag("FINAL_FLAG");
	///Keyword for the database table column of the category for the CI damage calculation: if the CI-Element is a regular (true) or an emergency structure (false) [-] (Dam_CI_Point, Dam_CI_Polygon)
	const string regular_flag("REGULAR_FLAG");
	///Keyword for the database table column of the category for the CI damage calculation: if the CI-Element is an emergency structure the activation time [-] (Dam_CI_Point, Dam_CI_Polygon)
	const string activation_time("ACTIVATION_TIME");

	///Keyword for the database table column of the category for the CI damage calculation: the HUB-value of a CI-element (number outgoing elements) [-] (Dam_CI_Point, Dam_CI_Polygon)
	const string hub_value("HUB_value");
	///Keyword for the database table column of the category for the CI damage calculation: the AUThority-value of a CI-element (number incoming elements) [-] (Dam_CI_Point, Dam_CI_Polygon)
	const string aut_value("AUT_value");
	///Keyword for the database table column of the category for the CI damage calculation: the cascade vulnerbility-value of a CI-element ( [-] (Dam_CI_Point, Dam_CI_Polygon)
	const string cv_value("CV_value");
	///Keyword for the database table column of the category for the CI damage calculation: the cascade potential-value of a CI-element  [-] (Dam_CI_Point, Dam_CI_Polygon)
	const string cp_value("CP_value");

	///Keyword for the database table column of the sector for CI damage calculation: Id of incomming CI-element for the connection [-] (Dam_CI_Element_List, Dam_CI_Point, Dam_CI_Polygon)
	const string in_id("IN_ID");
	///Keyword for the database table column of the sector for CI damage calculation: Flag if the incomming CI-element is point (0) or polygon (1) [-] (Dam_CI_Element_List, Dam_CI_Point, Dam_CI_Polygon)
	const string in_point_flag("IN_POINT");
	///Keyword for the database table column of the sector for CI damage calculation: Id of outgoing CI-element for the connection [-] (Dam_CI_Element_List, Dam_CI_Point, Dam_CI_Polygon)
	const string out_id("OUT_ID");
	///Keyword for the database table column of the sector for CI damage calculation: Flag if the outgoing CI-element is point (0) or polygon (1) [-] (Dam_CI_Element_List, Dam_CI_Point, Dam_CI_Polygon)
	const string out_point_flag("OUT_POINT");
	///Keyword for the database table column of the sector for CI damage calculation: Flag if it is a transsectoral connection (sectoral:=false; transectoral:=tue) [-] (Dam_CI_Element_List, Dam_CI_Point, Dam_CI_Polygon)
	const string transsec_flag("TRANSSECTORAL");

	///Keyword for the database table name of the CI point data (Dam_CI_Point)
	const string tab_ci_point("DAM_CI_POINT");
	///Keyword for the database table name of the CI polygon data (Dam_CI_Polygon)
	const string tab_ci_polygon("DAM_CI_POLYGON");
	///Keyword for the database table name of the CI polygon point data (Dam_CI_Polygon)
	const string tab_ci_polygon_point("DAM_CI_POLYGON_POINT");
	///Keyword for the database table name of the CI connection data (Dam_CI_Element_List)
	const string tab_ci_connection("DAM_CI_CONNECTION");

	///Keyword for the database table name of the simple-counting point data (Dam_Sc_Point)
	const string tab_sc_point("DAM_SC_POINT");
	///Keyword for the database table column of the simple-counting point data: Score-value of the point [-] (Dam_Sc_Point)
	const string sc_score("SC_SCORE");
	///Keyword for the database table column of the simple-counting point data: Boundary value for an impact; the impact is specified by the waterlevel [-] (Dam_Sc_Point)
	const string boundary_value("BOUNDARY_VALUE");
	///Keyword for the database table column of the simple-counting point data: Boundary value for an impact; the impact is specified by the waterlevel [-] (Dam_Sc_Point)
	const string recovery_time("RECOVERY_TIME");
	///Keyword for the database table column of the simple-counting point data: Point name [-] (Dam_Sc_Point, Dam CI_Point)
	const string point_name("POINT_NAME");
	///Keyword for the database table column of the CI polygon data: Polygon name [-] (Dam_CI_Polygon)
	const string polygon_name("POLYGON_NAME");
	///Keyword for the database table column of the CI polygon data: Polygon id [-] (Dam_CI_Polygon)
	const string polygon_id("POLYGON_ID");
	///Keyword for the database table column of the simple-counting point data: Identifier of the subcategory [-] (Dam_Sc_Point)
	const string subcategory_id("SUBCAT_ID");
	///Keyword for the database table column of the simple-counting point data: geometrical point (Dam_Sc_Point)
	const string sc_point("GEO_POINT");

	///Keyword for the database table name of the simple-counting damage results (Dam_Sc_Point)
	const string tab_sc_erg("DAM_SC_POINT_ERG");
	///Keyword for the database table column of the simple-counting damage results: affected score value [-] (Dam_Sc_Point)
	const string affected_score("AFFECT_SCORE");

	///Keyword for the database table name of the CI-point damage results (Dam_CI_Point)
	const string tab_ci_point_erg("DAM_CI_POINT_ERG");
	///Keyword for the database table column of the CI-point damage results: failure type [-] (Dam_CI_Point)
	const string failure_type("FAILURE_TYPE");
	///Keyword for the database table column of the CI-point damage results: failure duration [d] (Dam_CI_Point, Dam_CI_Polygon)
	const string failure_duration("FAILURE_DURATION");
	///Keyword for the database table name of the CI-polygon damage results (Dam_CI_Polygon)
	const string tab_ci_polygon_erg("DAM_CI_POLYGON_ERG");
	///Keyword for the database table column of the CI-point damage results: failure duration multiplied with endusers [enduser x d] (Dam_CI_Point, Dam_CI_Polygon)
	const string enduser_duration("FAILURE_ENDUSER_DURATION");
	
	///Keyword for the database table name of the CI-connection damage results (Dam_CI_Element_List)
	const string tab_ci_connection_erg("DAM_CI_CONNECT_ERG");
	///Keyword for the database table name of the CI-connection damage instationary results (Dam_CI_Element_List)
	const string tab_ci_connection_instat_erg("DAM_CI_CONNECT_INSTAT_ERG");

	///Keyword for the database table name of the CI-point damage instationary results (Dam_CI_Point)
	const string tab_ci_point_instat_erg("DAM_CI_POINT_INSTAT_ERG");
	///Keyword for the database table column of the CI-point damage results: active flag [-] (Dam_CI_Point, Dam_CI_Polygon)
	const string active_flag("ACTIVE");

	///Keyword for the database table name of the CI-polygon damage instationary results (Dam_CI_Polygon)
	const string tab_ci_polygon_instat_erg("DAM_CI_POLYGON_INSTAT_ERG");






	///Keyword for the database table name of the psycho-social element raster data (Dam_Pys_Element)
	const string tab_pys_element("DAM_PYS_ELEM");
	///Keyword for the database table column of the psycho-social element raster data: Criteria-value of the element; age class 50 to 59 [%] (Dam_Pys_Element, Dam_Pys_Function)
	const string crit_age_50_59("C_AGE50_59");
	///Keyword for the database table column of the psycho-social element raster data: Criteria-value of the element; age class greater 80 [%] (Dam_Pys_Element, Dam_Pys_Function)
	const string crit_age_80("C_AGE80");
	///Keyword for the database table column of the psycho-social element raster data: Criteria-value of the element; female gender [%] (Dam_Pys_Element, Dam_Pys_Function)
	const string crit_female("C_FEMALE");
	///Keyword for the database table column of the psycho-social element raster data: Criteria-value of the element; home owner [%] (Dam_Pys_Element, Dam_Pys_Function)
	const string crit_owner("C_HOME_OWNER");
	///Keyword for the database table column of the psycho-social element raster data: Boolean for no-information about the elemnt [-] (Dam_Pys_Element, Dam_Pys_Function)
	const string no_category_id("NOINFO_CATEGORY_ID");

	///Keyword for the database table column of the element results of a psycho-social raster: total score [score] (Dam_Pys_System , Dam_Pys_Element)
	const string total_score("TOTAL_SCORE");

	///Keyword for the database table name of the psycho-social damage results (Dam_Pys_Element)
	const string tab_pys_erg("DAM_PYS_RESULTS");
	///Keyword for the database table column of the psycho-social damage results: Score multiplied with the density factor for the crtiteria for the age class 50 to 59 [-] (Dam_Pys_Element, Dam_Pys_Function, Dam_Pys_Density_Function)
	const string denscrit_age50_59("D_C_AGE50_59");
	///Keyword for the database table column of the psycho-social damage results: Score multiplied with the density factor for the crtiteria for the age class greater 80 [-] (Dam_Pys_Element, Dam_Pys_Function, Dam_Pys_Density_Function)
	const string denscrit_age_80("D_C_AGE80");
	///Keyword for the database table column of the psycho-social damage results: Score multiplied with the density factor for the crtiteria for the female gender [-] (Dam_Pys_Element, Dam_Pys_Function, Dam_Pys_Density_Function)
	const string denscrit_female("D_C_FEMALE");
	///Keyword for the database table column of the psycho-social damage results: Score multiplied with the density factor for the crtiteria for the home owner [-] (Dam_Pys_Element, Dam_Pys_Function, Dam_Pys_Density_Function)
	const string denscrit_owner("D_C_HOME_OWNER");

	///Keyword for the database table column of the element results of a psycho-social raster: total score with density function [score] (Dam_Pys_System , Dam_Pys_Element)
	const string total_dens_score("TOTAL_DENS_SCORE");



	///Keyword for the database table name of the psycho-social score-functions (Dam_Pys_Function)
	const string tab_pys_score_func("DAM_PYS_SCORE_FUNCT");
	///Keyword for the database table column of the psycho-social score-functions: Name of the score-function [-] (Dam_Pys_Function)
	const string score_function_criteria("SCORE_FUNCTION_CRITERIA");


	///Keyword for the database table name of the psycho-social score-function points (Dam_Pys_Function)
	const string tab_pys_score_func_point("DAM_PYS_SCORE_FUNCT_POINT");
	///Keyword for the database table column of the psycho-social score-function points: Score-value, which represents the y-coordinate of the score-function [-] (Dam_Pys_Function)
	const string score_value("SCORE_VALUE");
	///Keyword for the database table column of the psycho-social score-function points: Criteria-value, which represents the x-coordinate of the score-function [-] (Dam_Pys_Function)
	const string criteria_value("CRITERIA_VALUE");


	///Keyword for the database table name of the psycho-social density-function points (Dam_Pys_Density_Function)
	const string tab_pys_dens_func_point("DAM_PYS_DENS_FUNCT_POINT");
	///Keyword for the database table column of the psycho-social density-function points: Density-factor, which represents the y-coordinate of the density-function [-] (Dam_Pys_Density_Function)
	const string density_factor("DENSITY_FACTOR");



	///String for the simple counting categories: public buildings \see Dam_Sc_Point
	const string sc_pub_build("SC_PUBLIC_BUILDINGS");
	///String for the simple counting categories: ecologic perilous sites or buildings \see Dam_Sc_Point
	const string sc_eco_build("SC_ECO_PERI_SITES");
	///String for the simple counting categories: cultural heritage  \see Dam_Sc_Point
	const string sc_cult_build("SC_CULTURAL_HERITAGE");
	///String for the simple counting categories: buildings with highly vulnerable person  \see Dam_Sc_Point
	const string sc_person_build("SC_HIGH_VUL_PERS_SITES");

	///String for the CI sector \see _Dam_CI_Element 
	const string ci_elect("Electricity");
	///String for the CI sector \see _Dam_CI_Element 
	const string ci_info_tec("Information_technology");
	///String for the CI sector \see _Dam_CI_Element 
	const string ci_water_sup("Water_supply");
	///String for the CI sector \see _Dam_CI_Element 
	const string ci_water_treat("Water_treatment");
	///String for the CI sector \see _Dam_CI_Element 
	const string ci_energy("Energy");


	///String for the CI sector \see _Dam_CI_Element 
	const string ci_emerg("Emergency_service");
	///String for the CI sector \see _Dam_CI_Element 
	const string ci_health("Health");
	///String for the CI sector \see _Dam_CI_Element 
	const string ci_logistic_goods("Logistic_goods");
	///String for the CI sector \see _Dam_CI_Element 
	const string ci_logistic_person("Logistic_person");
	///String for the CI sector \see _Dam_CI_Element 
	const string ci_goverment("Govermental_institutions");
	///String for the CI sector \see Dam_CI_Point
	const string ci_haz_mat("Hazardous_Materials");
	///String for the CI sector \see _Dam_CI_Element 
	const string ci_industry("Industry");
	///String for the CI sector \see _Dam_CI_Element 
	const string ci_cultur("Cultural");
	///String for the CI sector \see _Dam_CI_Element 
	const string ci_education("Education");
	///String for the CI sector \see _Dam_CI_Element 
	const string ci_jail("Jail");

	///Keyword for the database table column: String for the CI sector result Population time \see Dam_Damage_System 
	const string ci_elect_pt("Pop_t_Electricity");
	///Keyword for the database table column: String for the CI sector result Population time\see Dam_Damage_System 
	const string ci_info_tec_pt("Pop_t_Information_technology");
	///Keyword for the database table column: String for the CI sector result Population time\see Dam_Damage_System 
	const string ci_water_sup_pt("Pop_t_Water_supply");
	///Keyword for the database table column: String for the CI sector result Population time\see Dam_Damage_System 
	const string ci_water_treat_pt("Pop_t_Water_treatment");
	///Keyword for the database table column: String for the CI sector result Population time \see Dam_Damage_System  
	const string ci_energy_pt("Pop_t_Energy");

	///Keyword for the database table column: String for the CI sector result Population \see Dam_Damage_System 
	const string ci_elect_p("Pop_Electricity");
	///Keyword for the database table column: String for the CI sector result Population \see Dam_Damage_System  
	const string ci_info_tec_p("Pop_Information_technology");
	///Keyword for the database table column: String for the CI sector result Population \see Dam_Damage_System  
	const string ci_water_sup_p("Pop_Water_supply");
	///Keyword for the database table column: String for the CI sector result Population \see Dam_Damage_System 
	const string ci_water_treat_p("Pop_Water_treatment");
	//Keyword for the database table column: /String for the CI sector result Population time \see Dam_Damage_System 
	const string ci_energy_p("Pop_Energy");



	///Keyword for the database table column: String for the CI sector result Population time: health summmarizes id 10 and 11 \see Dam_Damage_System  
	const string ci_health_pt("Pop_t_Health");
	///Keyword for the database table column: String for the CI sector result Population time: social summmarizes id 14, 17, 18 and 19\see Dam_Damage_System 
	const string ci_social_pt("Pop_t_Social");
	///Keyword for the database table column: String for the CI sector result Population time: materialistic summarizes id 12, 13, 15, 16\see Dam_Damage_System 
	const string ci_mat_pt("Pop_t_Materialistic");

	///Keyword for the database table column: String for the CI sector result Population: health summmarizes id 10 and 11 \see Dam_Damage_System 
	const string ci_health_p("Pop_Health");
	///Keyword for the database table column: String for the CI sector result Population: social summmarizes id 14, 17 and 18\see Dam_Damage_System  
	const string ci_social_p("Pop_Social");
	///Keyword for the database table column: String for the CI sector result Population: materialistic summarizes id 12, 13, 15, 16\see Dam_Damage_System 
	const string ci_mat_p("Pop_Materialistic");


	///String for the CI element failure type \see _Dam_CI_Element 
	const string direct_failure("direct");
	///String for the CI element failure type \see _Dam_CI_Element 
	const string no_failure("no_failure");
	///String for the CI element failure type \see _Dam_CI_Element 
	const string sectoral_failure("sectoral");
	///String for the CI element failure type \see _Dam_CI_Element 
	const string transsectoral_failure("transsectoral");
	///String for the CI element failure type \see _Dam_CI_Element 
	const string direct_active_failure("direct_activated");
	

}

///General text labels specially for the module RISK \ingroup risk
namespace risk_label{

	///Keyword of the risk-types: risk analysis without any break of a fpl-section [-] (Risk_System)
	const string risk_nobreak("NOBREAK");
	///Keyword of the risk-types: risk analysis with one break in a fpl-section (scenario based) [-] (Risk_System)
	const string risk_scenario("SCENARIO");
	///Keyword of the risk-types: risk analysis with one/multiple break(s) in a  different fpl-section (catchment area based) [-] (Risk_System)
	const string risk_catchment("CATCHMENT");

	///Keyword of the break scenario sources: user defined scenario [-] (_risk_break_sc_source, Risk_Break_Scenario)
	const string src_sc_user("user-defined");
	///Keyword of the break scenario sources: fixed required scenario [-] (_risk_break_sc_source, Risk_Break_Scenario)
	const string src_sc_fixed("fixed");
	///Keyword of the break scenario sources: generated scenario via monte-carlo analysis [-] (_risk_break_sc_source, Risk_Break_Scenario)
	const string src_sc_gen("generated");


	///Keyword for the database table name of the risk configaration (Risk_System)
	const string tab_risk_config("RISK_CONFIGURATION");

	///Keyword for the database table column of the risk configaration: name as identifier [-] (Risk_System) 
	const string name_param("NAME");
	///Keyword for the database table column of the risk configaration: value [-] (Risk_System) 
	const string value_param("VALUE");

	///Keyword for the database table name of the detailed risk results (Risk_System)
	const string tab_risk_detailed("RISK_DETAILED_RESULT");
	///Keyword for the database table name of the detailed predicted risk results (Risk_System)
	const string tab_risk_predict_detailed("RISK_PREDICT_DETAIL_RESULT");
	///Keyword for the database table name of the cumulated risk results (Risk_System)
	const string tab_risk_cumulated("RISK_CUMULAT_RESULT");
	///Keyword for the database table name of the risk results (Risk_System)
	const string tab_risk_result("RISK_RESULT");

	///Keyword for the database table column of the risk results/detailed results: risk type [-] (Risk_System, _risk_type) 
	const string risk_type("RISK_TYPE");
	///Keyword for the database table column of the risk results/detailed results: risk of the maximum outflow discharge of the main river [-] (Risk_System) 
	const string max_outflow("MAXIMUM_OUTFLOW");

	///Keyword for the database table column of the risk results: number of detailed risk results for cumulation [-] (Risk_System) 
	const string number_detailed("NUMBER_DETAILED");
	///Keyword for the database table column of the risk results: number of cumulated risk results for total integration [-] (Risk_System) 
	const string number_cumulated("NUMBER_CUMULATED");

	///Keyword for the database table name of the break scenarios (Risk_Break_Szenario_Management)
	const string tab_sz_break("RISK_BREAK_SCENARIO");
	///Keyword for the database table column of the break scenario data: global identifier of the break szenario [string] (Risk_Break_Szenario_Management, Risk_Break_Szenario)
	const string sz_break_id("BREAK_SCENARIO_ID");
	///Keyword for the database table column of the break scenario data: probability of the break szenario [-] (Risk_Break_Szenario_Management, Risk_Break_Szenario)
	const string sz_break_prob("PROBABILITY");
	///Keyword for the database table column of the break scenario data: minimum boundary of the mid probability of the break szenario (95%-confidence) [-] (Risk_Break_Szenario_Management, Risk_Break_Szenario)
	const string sz_min_bound_break_prob("MIN_BOUND_PROB");
	///Keyword for the database table column of the break scenario data: maximum boundary of the mid probability of the break szenario (95%-confidence) [-] (Risk_Break_Szenario_Management, Risk_Break_Szenario)
	const string sz_max_bound_break_prob("MAX_BOUND_PROB");
	///Keyword for the database table column of the break scenario data: predicted probability of the break szenario update with new hydraulic information [-] (Risk_Break_Szenario_Management, Risk_Break_Szenario)
	const string sz_predict_break_prob("PREDICTED_PROBABILITY");
	///Keyword for the database table column of the break scenario data: probability of the break szenario update with new hydraulic information [-] (Risk_Break_Szenario_Management, Risk_Break_Szenario)
	const string sz_no_predict_break_prob("NO_PREDICTED_PROBABILITY");
	///Keyword for the database table column of the break scenario data: sources of the break scenario [-] (Risk_Break_Szenario_Management, Risk_Break_Szenario)
	const string sz_break_src("SOURCE");


	///Keyword for the database table name of the probabilities of each FPL-section (Risk_Break_Szenario_Management)
	const string tab_risk_prob_sec("RISK_PROB_SEC");

	///Keyword for the database table name of the detailed risk results per damage raster-element of the economical damage raster (Risk_System)
	const string tab_risk_ecn_elem("RISK_ECN_ELEM");
	///Keyword for the database table column of the detailed risk results per damage raster-element: immobile risk with the HYD-boundary probability [monetary/a] (Risk_System)
	const string risk_immob_with_hyd("IMMOB_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element: mobile risk with the HYD-boundary probability [monetary/a] (Risk_System)
	const string risk_mob_with_hyd("MOB_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element: total risk with the HYD-boundary probability [monetary/a] (Risk_System)
	const string risk_ecn_total_with_hyd("TOTAL_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element: risk factor [-] (Risk_System)
	const string risk_ecn_total_fac("RISK_FACTOR");


	///Keyword for the database table name of the detailed risk results per damage raster-element of the ecological biotope-type damage raster (Risk_System)
	const string tab_risk_eco_btype_elem("RISK_ECO_BTYPE_ELEM");
	///Keyword for the database table column of the detailed risk results per damage raster-element: biotope-type risk with the HYD-boundary probability [monetary/a] (Risk_System)
	const string risk_btype_with_hyd("B_TYPE_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element: risk factor [-] (Risk_System)
	const string risk_btype_fac("RISK_FACTOR");

	///Keyword for the database table name of the detailed risk results per damage raster-element of the ecological soil-erosion damage raster (Risk_System)
	const string tab_risk_eco_soil_elem("RISK_ECO_SOIL_ELEM");
	///Keyword for the database table column of the detailed risk results per damage raster-element: soil-erosion risk with the HYD-boundary probability [monetary/a] (Risk_System)
	const string risk_soil_with_hyd("SOIL_ERO_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element: risk factor [-] (Risk_System)
	const string risk_soil_fac("RISK_FACTOR");


	///Keyword for the database table name of the detailed risk results per damage raster-element of the people2risk damage raster (Risk_System)
	const string tab_risk_pop_elem("RISK_POP_ELEM");
	///Keyword for the database table column of the detailed risk results per damage raster-element: affected people risk with the HYD-boundary probability [person/a] (Risk_System)
	const string risk_pop_aff_with_hyd("POP_AFFECTED_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element: risk factor [-] (Risk_System)
	const string risk_pop_aff_fac("R_FAC_AFFECTED");
	///Keyword for the database table column of the detailed risk results per damage raster-element: endangered people risk with the HYD-boundary probability [person/a] (Risk_System)
	const string risk_pop_dan_with_hyd("POP_ENDANGERED_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element: risk factor [-] (Risk_System)
	const string risk_pop_dan_fac("R_FAC_ENDANGERED");

	

	///Keyword for the database table name of the detailed risk results per damage raster-element of the psycho-social damage raster (Risk_System)
	const string tab_risk_pys_elem("RISK_PYS_ELEM");
	///Keyword for the database table column of the detailed risk results per damage raster-element:  psycho-social criteria 50< age<59 with the HYD-boundary probability [score/a] (Risk_System)
	const string risk_crit_age_50_59_with_hyd("C_AGE50_59_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element:  psycho-social criteria 80< age with the HYD-boundary probability [score/a] (Risk_System)
	const string risk_crit_age_80_with_hyd("C_AGE80_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element:  psycho-social criteria female gender with the HYD-boundary probability [score/a] (Risk_System)
	const string risk_crit_female_with_hyd("C_FEMALE_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element:  psycho-social criteria home owner with the HYD-boundary probability [score/a] (Risk_System)
	const string risk_crit_owner_with_hyd("C_HOME_OWNER_HYD_PROB");

	///Keyword for the database table column of the detailed risk results per damage raster-element:  total score of psycho-social criteria with the HYD-boundary probability [score/a] (Risk_System)
	const string risk_pys_total_score("TOTAL_SCORE_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element:  risk factor [-] (Risk_System)
	const string risk_pys_total_fac("R_FAC_SCORE");

	///Keyword for the database table column of the detailed risk results per damage raster-element:  psycho-social criteria 50< age<59 (population density funtion included) with the HYD-boundary probability [score/a] (Risk_System)
	const string risk_dens_crit_age_50_59_with_hyd("D_C_AGE50_59_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element:  psycho-social criteria 80< age (population density funtion included) with the HYD-boundary probability [score/a] (Risk_System)
	const string risk_dens_crit_age_80_with_hyd("D_C_AGE80_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element:  psycho-social criteria female (population density funtion included) gender with the HYD-boundary probability [score/a] (Risk_System)
	const string risk_dens_crit_female_with_hyd("D_C_FEMALE_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element:  psycho-social criteria home owner (population density funtion included) with the HYD-boundary probability [score/a] (Risk_System)
	const string risk_dens_crit_owner_with_hyd("D_C_HOME_OWNER_HYD_PROB");

	///Keyword for the database table column of the detailed risk results per damage raster-element:  total score of psycho-social criteria (population density funtion included) with the HYD-boundary probability [score/a] (Risk_System)
	const string risk_pys_total_dens_score("TOTAL_DENS_SCORE_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per damage raster-element:  risk factor (population density funtion included) [-] (Risk_System)
	const string risk_pys_total_dens_fac("R_FAC_DENS_SCORE");


	
	///Keyword for the database table name of the detailed risk results per damage point of the simple counting damage (Risk_System)
	const string tab_risk_sc_point("RISK_SC_POINT");
	///String for the simple counting categories: public buildings \see Dam_Sc_Point
	const string risk_sc_pub_build_with_hyd("SC_PUBLIC_BUILDINGS_HYD_PROB");
	///String for the simple counting categories: ecologic perilous sites or buildings \see Dam_Sc_Point
	const string risk_sc_eco_build_with_hyd("SC_ECO_PERI_SITES_HYD_PROB");
	///String for the simple counting categories: cultural heritage  \see Dam_Sc_Point
	const string risk_sc_cult_build_with_hyd("SC_CULTURAL_HERITAGE_HYD_PROB");
	///String for the simple counting categories: buildings with highly vulnerable person  \see Dam_Sc_Point
	const string risk_sc_person_build_with_hyd("SC_HIGH_VUL_PERS_SITES_HYD_PROB");
	///Keyword for the database table column of the detailed risk results per dpoint: risk factor [-] \see Risk_System
	const string risk_sc_fac("RISK_FACTOR");

	///Keyword for the database table column of the detailed predicted risk results:  total economical risk [€/a] \see Risk_System
	const string risk_total_ecn_predict("TOTAL_ECN");
	///Keyword for the database table column of the detailed predicted risk results:  percentage concerning the total economical risk [%] \see Risk_System
	const string risk_total_ecn_predict_perc("TOTAL_ECN_PERC");
	///Keyword for the database table column of the detailed predicted risk results:  total ecological risk [€/a] \see Risk_System
	const string risk_total_eco_predict("TOTAL_ECO");
	///Keyword for the database table column of the detailed predicted risk results:  percentage concerning the total ecological risk [%] \see Risk_System
	const string risk_total_eco_predict_perc("TOTAL_ECO_PERC");

	///Keyword for the database table column of the detailed predicted risk results:  percentage concerning the total affected people risk [%] \see Risk_System
	const string risk_pop_affected_perc("POP_AFFECTED_PERC");
	///Keyword for the database table column of the detailed predicted risk results:  percentage concerning the total endangered people risk [%] \see Risk_System
	const string risk_pop_endangered_perc("POP_ENDANGERED_PERC");

	///Keyword for the database table column of the detailed predicted risk results:  percentage concerning the total psycho-social score risk [%] \see Risk_System
	const string risk_total_score_perc("TOTAL_SCORE_PERC");
	///Keyword for the database table column of the detailed predicted risk results:  percentage concerning the total psycho-social score (with density) risk [%] \see Risk_System
	const string risk_total_dens_score_perc("TOTAL_DENS_SCORE_PERC");

}
///General text labels specially for the module MADM \ingroup madm
namespace madm_label{


	///String for defining the name of a weight set: standard \see Madm_System
	const string weight_standard("standard");
	///String for defining the name of a weight set: economic standard \see Madm_System
	const string weight_ecn_standard("ecn_standard");
	///String for defining the name of a weight set: ecologic standard \see Madm_System
	const string weight_eco_standard("eco_standard");
	///String for defining the name of a weight set: population standard \see Madm_System
	const string weight_pop_standard("pop_standard");



	///Keyword for the file input of the madm analysis: begin of the general analysis informations [-] \see Madm_Decision_Matrix
	const string begin_general("!$BEGIN_GENERAL");
	///Keyword for the file input of the madm analysis: end of the general analysis informations [-] \see Madm_Decision_Matrix
	const string end_general("!$END_GENERAL");

	///Keyword for the file input of the madm analysis: name of the analysis \see Madm_Decision_Matrix
	const string analysis_name("!name");
	///Keyword for the file input of the madm analysis: number of applied criteria \see Madm_Decision_Matrix
	const string no_crit("!no_criteria");
	///Keyword for the file input of the madm analysis: number of applied alternatives \see Madm_Decision_Matrix
	const string no_alt("!no_alternative");

	///Keyword for the file input of the madm analysis: begin of the criteria informations [-] \see Madm_Decision_Matrix
	const string begin_criteria("!$BEGIN_CRITERIA");
	///Keyword for the file input of the madm analysis: end of the criteria informations [-] \see Madm_Decision_Matrix
	const string end_criteria("!$END_CRITERIA");

	///Keyword for the file input of the madm analysis: criteria names \see Madm_Decision_Matrix
	const string crit_name("!crit_name");
	///Keyword for the file input of the madm analysis: specifies the optimum (maximal:=true, minimal:=false) \see Madm_Decision_Matrix
	const string crit_max_min("!max_min_opt");
	///Keyword for the file input of the madm analysis: weight of the criteria \see Madm_Decision_Matrix
	const string crit_weight("!weight");

	///Keyword for the file input of the madm analysis: begin of the matrix informations [-] \see Madm_Decision_Matrix
	const string begin_matrix("!$BEGIN_MATRIX");
	///Keyword for the file input of the madm analysis: end of the matrix informations [-] \see Madm_Decision_Matrix
	const string end_matrix("!$END_MATRIX");


	///Keyword for the file input of the madm weight sets: number of weight sets in file \see Madm_Decision_Matrix
	const string no_weight_set("!$no_weight_set");

	///Keyword for the file input of the madm weight sets: begin of the weight set [-] \see Madm_Analysis
	const string begin_weight_set("!$BEGIN_SET");
	///Keyword for the file input of the madm weight sets: end of the weight set [-] \see Madm_Analysis
	const string end_weight_set("!$END_SET");

	///Keyword for the file input of the madm weight sets: name of the set \see Madm_Analysis
	const string weight_set_name("!name");
	///Keyword for the file input of the madm weight sets: weight for the economical risk criteria \see Madm_Analysis
	const string weight_risk_ecn("!crit_risk_ecn");
	///Keyword for the file input of the madm weight sets: weight for the ecological risk criteria \see Madm_Analysis
	const string weight_risk_eco("!crit_risk_eco");
	///Keyword for the file input of the madm weight sets: weight for the psycho-social risk criteria \see Madm_Analysis
	const string weight_risk_pys("!crit_risk_pys");
	///Keyword for the file input of the madm weight sets: weight for the people2risk (affected people) risk criteria \see Madm_Analysis
	const string weight_risk_pop_aff("!crit_risk_pop_aff");
	///Keyword for the file input of the madm weight sets: weight for the people2risk (endangered people) risk criteria \see Madm_Analysis
	const string weight_risk_pop_dan("!crit_risk_pop_dan");

	///Keyword for the file input of the madm weight sets: weight for the simple counting (public buildings) risk criteria \see Madm_Analysis
	const string weight_risk_sc_pub("!crit_risk_sc_pub");
	///Keyword for the file input of the madm weight sets: weight for the simple counting (ecologic perilous sites or buildings) risk criteria \see Madm_Analysis
	const string weight_risk_sc_eco("!crit_risk_sc_eco");
	///Keyword for the file input of the madm weight sets: weight for the simple counting (cultural heritage) risk criteria \see Madm_Analysis
	const string weight_risk_sc_cult("!crit_risk_sc_cult");
	///Keyword for the file input of the madm weight sets: weight for the simple counting (buildings with highly vulnerable person) risk criteria \see Madm_Analysis
	const string weight_risk_sc_person("!crit_risk_sc_person");


	///Keyword for the file input of the madm weight sets: weight for the CI (sector electricity) risk criteria \see Madm_Analysis
	const string weight_risk_ci_elect("!crit_risk_ci_elect");
	///Keyword for the file input of the madm weight sets: weight for the CI (sector information technology) risk criteria \see Madm_Analysis
	const string weight_risk_ci_info_tec("!crit_risk_ci_info_tec");
	///Keyword for the file input of the madm weight sets: weight for the CI (sector water supply) risk criteria \see Madm_Analysis
	const string weight_risk_wat_sup("!crit_risk_ci_water_sup");
	///Keyword for the file input of the madm weight sets: weight for the CI (sector water treatment) risk criteria \see Madm_Analysis
	const string weight_risk_ci_wat_treat("!crit_risk_ci_water treat");
	///Keyword for the file input of the madm weight sets: weight for the CI (sector energy) risk criteria \see Madm_Analysis
	const string weight_risk_ci_energy("!crit_risk_ci_energy");

	///Keyword for the file input of the madm weight sets: weight for the CI (sector health) risk criteria \see Madm_Analysis
	const string weight_risk_ci_health("!crit_risk_ci_health");
	///Keyword for the file input of the madm weight sets: weight for the CI (sector social) risk criteria \see Madm_Analysis
	const string weight_risk_ci_social("!crit_risk_ci_social");
	///Keyword for the file input of the madm weight sets: weight for the CI (sector material) risk criteria \see Madm_Analysis
	const string weight_risk_ci_mat("!crit_risk_ci_mat");




	///Keyword for the file input of the madm weight sets: weight for the maximum outflow discharge risk criteria \see Madm_Analysis
	const string weight_risk_outflow("!crit_risk_outflow");
	///Keyword for the file input of the madm weight sets: weight for the economical cost criteria \see Madm_Analysis
	const string weight_cost_ecn("!crit_cost_ecn");

	///Keyword of the madm calculation approach: Simple Additive Weighting (standard normalisation)  [-] \see Madm_Analysis
	const string analysis_saw_standard("SAW_STANDARD");
	///Keyword of the madm calculation approach: Simple Additive Weighting (range normalisation)  [-] \see Madm_Analysis
	const string analysis_saw_range("SAW_RANGE");
	///Keyword of the madm calculation approach: Technique for Order Preference by Similarity to Ideal Solution  [-] \see Madm_Analysis
	const string analysis_topsis("TOPSIS");
	///Keyword of the madm calculation approach: ELimination Et Choix Traduisant la REalité [-] \see Madm_Analysis
	const string analysis_electre("ELECTRE");
	///Keyword of the madm calculation approach: all scores are taken into account [-] \see Madm_Analysis
	const string analysis_total("TOTAL");

	///Keyword of the madm normalisation scheme: standard [-] \see Madm_Decision_Matrix
	const string norm_standard("standard");
	///Keyword of the madm normalisation scheme: range [-] \see Madm_Decision_Matrix
	const string norm_range("range");
	///Keyword of the madm normalisation scheme: vectoriel [-] \see Madm_Decision_Matrix
	const string norm_vector("vector");
	///Keyword of the madm normalisation scheme: no normalisation applied [-] \see Madm_Decision_Matrix
	const string norm_no("no_norm");


	///Keyword for the database table name of the set of weights \see Madm_System
	const string tab_set("MADM_SET");
	///Keyword for the database table column of the set of weights: Set id [-] \see Madm_System 
	const string set_id("SET_ID");
	///Keyword for the database table column of the set of weights: Name of set [-] \see Madm_System
	const string set_name("NAME");

	///Keyword for the database table column of divers madm tables: Criteria variation of risk economic [monetary/a] \see Madm_System 
	const string crit_risk_ecn("CRIT_RISK_ECN");
	///Keyword for the database table column of divers madm tables: Criteria variation of risk ecologic [monetary/a] \see Madm_System 
	const string crit_risk_eco("CRIT_RISK_ECO");
	///Keyword for the database table column of divers madm tables: Criteria variation of risk psycho-social [score/a] \see Madm_System 
	const string crit_risk_pys("CRIT_RISK_PYS");
	///Keyword for the database table column of divers madm tables: Criteria variation of risk people2risk (affected persons) [person/a] \see Madm_System
	const string crit_risk_pop_aff("CRIT_RISK_POP_AFF");
	///Keyword for the database table column of divers madm tables: Criteria variation of risk people2risk (endangered persons) [person/a] \see Madm_System
	const string crit_risk_pop_dan("CRIT_RISK_POP_DAN");
	///Keyword for the database table column of divers madm tables: Criteria variation of risk outflow [(m³/s)/a] \see Madm_System 
	const string crit_risk_outflow("CRIT_RISK_OUTFLOW");
	///Keyword for the database table column of divers madm tables: Criteria variation of risk for simple counting public buildings [score/a] \see Madm_System
	const string crit_risk_sc_pub("CRIT_RISK_SC_PUB");
	///Keyword for the database table column of divers madm tables: Criteria variation of risk for simple counting ecologic perilous sites or buildings [score/a] \see Madm_System 
	const string crit_risk_sc_eco("CRIT_RISK_SC_ECO");
	///Keyword for the database table column of divers madm tables: Criteria variation of risk for simple counting cultural heritage [score/a] \see Madm_System 
	const string crit_risk_sc_cult("CRIT_RISK_SC_CULT");
	///Keyword for the database table column of divers madm tables: Criteria variation of risk for simple counting buildings with highly vulnerable person [score/a] \see Madm_System 
	const string crit_risk_sc_person("CRIT_RISK_SC_PERSON");

	///Keyword for the database table column of divers madm tables: Criteria cost economic [monetary] \see Madm_System
	const string crit_cost_ecn("CRIT_COST_ECN");

	///Keyword for the database table column of divers madm tables: Population time Sector electricity [(person x sec)/a] \see Madm_System
	const string crit_risk_ci_elect_pt("CRIT_PT_ELECT_CI");
	///Keyword for the database table column of divers madm tables: Population time Sector information technology [(person x sec)/a] \see Madm_System
	const string crit_risk_ci_info_tec_pt("CRIT_PT_INFO_TEC_CI");
	///Keyword for the database table column of divers madm tables: Population time Sector water supply [(person x sec)/a] \see Madm_System
	const string crit_risk_ci_water_sup_pt("CRIT_PT_WATER_SUP_CI");
	///Keyword for the database table column of divers madm tables: Population time Sector water treatment [(person x sec)/a] \see Madm_System
	const string crit_risk_ci_water_treat_pt("CRIT_PT_WATER_TREAT_CI");
	///Keyword for the database table column of divers madm tables: Population time Sector energy [(person x sec)/a] \see Madm_System
	const string crit_risk_ci_energy_pt("CRIT_PT_ENERGY_CI");
	///Keyword for the database table column of divers madm tables: Population time Sector health [(person x sec)/a] \see Madm_System
	const string crit_risk_ci_health_pt("CRIT_PT_HEALTH_CI");
	///Keyword for the database table column of divers madm tables: Population time Sector social [(person x sec)/a] \see Madm_System
	const string crit_risk_ci_social_pt("CRIT_PT_SOCIAL_CI");
	///Keyword for the database table column of divers madm tables: Population time Sector material [(person x sec)/a] \see Madm_System
	const string crit_risk_ci_mat_pt("CRIT_PT_MAT_CI");






	/////Keyword for the database table name of the scores (Madm_System)
	//const string tab_score("MADM_SCORE");
	///Keyword for the database table column of the scores and the matrix table: Type of madm-analysis [-] (Madm_System) 
	const string analysis_type("TYPE_ANALYSIS");

	///Keyword for the database table name of the decision matrix (Madm_System)
	const string tab_matrix("MADM_MATRIX");

	///Keyword for the database table name of the ranking (Madm_System)
	const string tab_ranking("MADM_RANKING");
	///Keyword for the database table column of the ranking: Ranking of the alternatives [-] (Madm_System) 
	const string rank("RANK");
	///Keyword for the database table column of the ranking: Score of the alternatives [-] (Madm_System) 
	const string score("SCORE");

}
///General text labels specially for the module ALT \ingroup alt
namespace alt_label{

	///String for defining the name of a m,easure category: floodprotectionline fpl (Alt_System, _alt_measure_category)
	const string cat_fpl("fpl");
	///String for defining the name of a m,easure category: hydraulic hyd (Alt_System, _alt_measure_category)
	const string cat_hyd("hyd");
	///String for defining the name of a m,easure category: damage dam (Alt_System, _alt_measure_category)
	const string cat_dam("dam");



	///Keyword for the database table name of the measures (alternatives) (Alt_System)
	const string tab_measures("ALT_MEASURE");
	///Keyword for the database table column of the measures: Name of the alternative [-] (Alt_System) 
	const string measure_name("NAME");
	///Keyword for the database table column of the measures: Category of the alternative [-] (Alt_System) 
	const string measure_cat("CATEGORY");
	///Keyword for the database table column of the measures: Sub-category of the alternative [-] (Alt_System) 
	const string measure_sub_cat("SUB_CATEGORY");
	///Keyword for the database table column of the measures: Flag if the measure is replacing [-] (Alt_System) 
	const string measure_replacing("REPLACING");

	///Keyword for the database table name of the replacing measures, where detailed information about the replacing act are stored (Alt_System)
	const string tab_replacing_measures("ALT_REPLACE_MEASURE");
	///Keyword for the database table column of the replacing measures: Name of the table where the applied flag is set to false [-] (Alt_System) 
	const string replacing_tab_name("TABLE_NAME");
	///Keyword for the database table column of the replacing measures: Id of the data set where the applied flag is set to false [-] (Alt_System) 
	const string replacing_id("REPLACING_ID");


	///Keyword for adding a item (measure) to the system (AltGui_Measure_Replacing_File_Dia)
	const string adding_item("-1 ADDING");

}
///General text labels specially for the module COST \ingroup cost
namespace cost_label{


	///Keyword for the database table name of the economical cost (Cost_Ecn_Cost_System)
	const string tab_cost_ecn("COST_ECN_DATA");
	///Keyword for the database table column of the economical cost: area cost [monetary] (Cost_Ecn_Cost_System) 
	const string area_cost("AREA_COST");
	///Keyword for the database table column of the economical cost: building cost [monetary] (Cost_Ecn_Cost_System) 
	const string build_cost("BUILDING_COST");
	///Keyword for the database table column of the economical cost: ecological trade-off [%] (Cost_Ecn_Cost_System) 
	const string eco_trade_off("ECO_TRADE_OFF");
	///Keyword for the database table column of the economical cost: ecological trade-off cost [monetary] (Cost_Ecn_Cost_System) 
	const string eco_trade_off_cost("ECO_TRADE_OFF_COST");
	///Keyword for the database table column of the economical cost: matching coefficient [-] (Cost_Ecn_Cost_System) 
	const string matching_coeff("MATCHING_COEFF");
	///Keyword for the database table column of the economical cost: flag for planning cost calculating after HOAI [-] (Cost_Ecn_Cost_System) 
	const string plan_cost_hoai("PLANNING_COST_HOAI");
	///Keyword for the database table column of the economical cost: planning cost [monetary] (Cost_Ecn_Cost_System) 
	const string plan_cost("PLANNING_COST");
	///Keyword for the database table column of the economical cost: total cost [monetary] (Cost_Ecn_Cost_System) 
	const string total_cost("TOTAL_COST");

}
#endif











