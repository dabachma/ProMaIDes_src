#pragma once
/**\class Dam_Hyd_Result_Raster
	\author Kufeld et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef DAM_HYD_RESULT_RASTER_H
#define DAM_HYD_RESULT_RASTER_H

//sys system
#include "_Sys_Common_System.h"
#include "Geo_Raster.h"

//dam system
#include "Dam_Hydimport_Element.h"


///This class provides the geometrical raster as well as the hydraulic specific information for the damage calculation \ingroup dam
/**
This class provides geometrical information as the raster boundary and the coordinates of the raster elements as well as the
characteristics of each element. In this case here are following chracteristics:
	- relative waterlevel h [m]
	- geodetic height z [m] [optional]
	- maximum flow velocity vmax [m/s] [optional]
	- duration of flooding d [s] [optional]
	- speed of waterlevel rise [m/s] [optional]
This characteristics are read in and transfered to database. This class is just used in the
DAM-project type.

*/
class Dam_Hyd_Result_Raster: public Geo_Raster
{

public:
	///Default constructor
	Dam_Hyd_Result_Raster(void);
	///Default destructor
	~Dam_Hyd_Result_Raster(void);

	//methods
	///Read all five hydraulic result file for one floodplain raster, which is to be imported for damage calculation to database
	void read_all_hydraulic_rasters_per_file(string fname_waterlevel, string fname_height, string fname_v, string fname_duration, string fname_dhdt);
	///Transfer the hydraulic result data to the database
	void transfer_input_members2database(QSqlDatabase *ptr_database, const int fp_index, Hyd_Boundary_Szenario *sc_id, const bool base_sc);


	///Set the geometrical member of the raster by a Sql-query
	void set_geometrical_member_database(const int fp_index, QSqlQueryModel *query_result);

	///Output the members of hydraulic result raster to display/console
	void output_member(void);

	///Clear the raster information
	void clear_raster(void);

	///Compare two raster for equality of the raster informations
	bool compare_raster_infos(Dam_Hyd_Result_Raster *buffer);

	///Get the name of the raster
	string get_raster_name(void);

	///Set the number of the raster
	void set_raster_number(const int number);


private:
	//member
	///Pointer to elements (used to store input data)
	Dam_Hydimport_Element *elements;
	///Characteristic for each element
    double *characteristic;
	///Index/Number of the raster
	int number;
	///Name of the raster
	string name;
	///Value for no available information of an element
	double no_info_value; 
	///Flag if raster is set
	bool is_set;
	///Type of the raster (_dam_raster_types)
	_dam_raster_types type;
	
	//methods
	///Allocate the characteristics for each element
	void allocate_characteristics(void);
	///Delete the characteristics for each element
	void delete_characteristics(void);

	///Initialize the raster: the points, segments and characteristics
	void init_raster(void);
	///Read in the raster data from file
	void read_damage_raster_per_file(const string file_raster);
	///Read the raster information per file
	void read_raster_infos(ifstream *ifile, int *line_counter);
	///Read the charactersistics of the elements per file
	void read_raster_elem_characteristic(ifstream *ifile, int *line_counter);
	///Find the key values for the file input
	void find_key_values_file(string myline , int *must_found_counter);
	///Sort raster characteristics from read down-right to up-left
	void sort_raster_characteristics(void);

	///Allocate the result elements
	void allocate_result_elements(void);
	///Delete the result elements
	void delete_result_elements(void);


	///Transfer the midpoint information from the raster element to the raster element information
	void transfer_midpoint2elem_info(void);
	///Set the waterlevel to the elements
	void set_waterlevel2elements(Dam_Hyd_Result_Raster *raster);
	///Set the height to the elements
	void set_height2elements(Dam_Hyd_Result_Raster *raster);
	///Set the velocity to the elements
	void set_velocity2elements(Dam_Hyd_Result_Raster *raster);
	///Set the duration to the elements
	void set_duration2elements(Dam_Hyd_Result_Raster *raster);
	///Set the dhdt-value to the elements
	void set_dhdt2elements(Dam_Hyd_Result_Raster *raster);



	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);


};
#endif
