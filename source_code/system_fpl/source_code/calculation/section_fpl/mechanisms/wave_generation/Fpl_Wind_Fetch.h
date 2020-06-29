#pragma once
/**\class Fpl_Wind_Fetch
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_WIND_FETCH_H
#define FPL_WIND_FETCH_H

//system sys class
#include "_Geo_Geometrie.h"
//fpl class
#include "_Fpl_Section_Type.h"

//struct
///Data strcuture for the parameters of one wind fetch of a fpl-section \ingroup fpl
struct _fpl_wind_fetch{
	///Fetch length
	double length;
	///Angel of the fetch to the north direction
	double angle_to_north;
	///Zone number of the fetch
	int zone_number;
	///Flag if the fetch is dry
	bool dry_flag;
	///Pointer to the random varaiable of the bathymetrie
	Fpl_Random_Variables *ptr_random_bathy;
};

///Geometric class for a fecth of a fpl-section \ingroup fpl
/**
The fetch is an important input parameter of the wave generation to due an wind event. The fetches
are geometric values, length, angle etc. They are connected to an bathymetrie zones (random varaibles), which 
represent the waterlevel in this zone.
*/
class Fpl_Wind_Fetch: public _Geo_Geometrie
{
public:
	///Default constructor
	Fpl_Wind_Fetch(void);
	///Default destructor
	~Fpl_Wind_Fetch(void);

	//methods
	///Allocate and set the fetches from database table
	int new_set_global_fetches(QSqlDatabase *ptr_database, const _sys_system_id id, const double section_direction, const int section_id);
	///Allocate and set the fetches from file
	void new_set_global_fetches(QFile *ifile, int *line_counter,  const double section_direction);
	///Allocate and set the random varaiables of the bathymetrie from database table
	void new_set_bathymetry(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Allocate and set the random varaiables of the bathymetrie from file
	void new_set_bathymetry(QFile *ifile, int *line_counter);

	///Connect bathymetrie and fetches
	void connect_fetches2bathy(void);

	///Transfer the wind-wave fetch data to database, e.g. geometry, random variables etc.
	void transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);

	///Output members to display/console
	void output_members(void);

	///Get the number of the fetches
	int get_fetch_number(void);
	///Get the angle related to the north direction
	double get_angle(const int zone_index);
	///Get the fetch length
	double get_length(const int zone_index);
	///Get index of the corresponding bathymetrie zone
	int get_zone(const int zone_index);
	///Get if the fetch is dry
	bool get_dry_flag(const int zone_index);
	///Get random generated variables of the bathymetrie of each fetch zone
	double calculate_batymetrie(const bool random_calculation, const int zone_index);

	///Reset the random generated flags of the random variables
	void reset_random_flag(void);

	///Check the statistic of the random variables
	void check_statistic(void);
	///Reset the statistic important values
	void reset_statistics(void);
	///Output the statistics of the random variables to display/console
	void output_statistic(void);

	///Convert a global angle (0° is north) to a local one; local 0° is the reference angle
	static double global_angle_to_local(const double global_angle, const double reference_angle);


private:
	//members
	///Number of the fetch
	int fetch_number;
	///Pointer to a data structure gathering the fetch informations
	_fpl_wind_fetch *fetches;
	///Number of the bathymetrie zone
	int bathy_zones_number;
	///Pointer to the random varaibles of the bathymetrie
	Fpl_Random_Variables *bathymetrie;

	//method
	///Check the global fetches
	void check_global_fetches(const double section_direction);
	///Allocate the fetches
	void allocate_fetches(void);
	///Delete the fetches
	void delete_fetches(void);
	///Allocate the bathymetry
	void allocate_bathymetry(void);
	///Delete the bathymetry
	void delete_bathymetry(void);

	///Find the key-values for the file input of the fetches and bathymetry
	void find_key_values_file(const string myline);
	
	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif