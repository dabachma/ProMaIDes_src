#pragma once
/**\class Hyd_Observation_Point_Manager
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef HYD_OBSERVATION_POINT_MANAGER_H
#define HYD_OBSERVATION_POINT_MANAGER_H


#include "Hyd_Observation_Point.h"

///Class for the managing of observation points in the hydraulic models  \ingroup hyd
/**

\see Hyd_Observation_Point
*/
class Hyd_Observation_Point_Manager
{
public:
	///Default constructor
	Hyd_Observation_Point_Manager(void);
	///Default destructor
	~Hyd_Observation_Point_Manager(void);


	//members
	///Pointer to the table for the observation point data in a database
	static Tables *obs_point_table;


	//method
	///Create the database table for the observation point data in a database
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the observation point data: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database, const bool not_close=false);
	///Delete all data in the database table for the observation point data
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the observation point data
	static void close_table(void);

	///Get the number of observation points
	int get_number_obs_points(void);
	///Get a pointer to the geometrical observation point list
	Hyd_Observation_Point* get_ptr_obs_point(void);

	///Read in the observation points per file
	void input_obs_point(const string file);
	///Read in the obervation point per database
	void input_obs_point(QSqlDatabase *ptr_database, const bool output=true);

	///Transfer the data of the observation points to database table
	void transfer_obs_points2database(QSqlDatabase *ptr_database, const string file);

	///Initialize the observation points
	void init_obs_points(const int num_rv, Hyd_Model_River *river, const int num_fp, Hyd_Model_Floodplain *floodplain, const int no_output, const int no_internal);
	///Initialize the observation points of temperature models
	void init_temp_obs_points(const int num_rv, HydTemp_Model *temp_model, const int no_output, const int no_internal);

	///Output setted members
	void output_setted_members(void);
	///Output members of the temperature observation points
	void output_setted_temp_members(void);


	///Syncronize the observation points
	void syncron_obs_points(const double time_point);
	///Syncronize the observation points for temoerature calculation
	void syncron_temp_obs_points(const double time_point);

	///Output the data of the observation points to tecplot file
	void output_obs_points2tecplot_file(const string filename_rv, const string filename_fp);
	///Output the data of the observation points to ParaView / cvs file
	void output_obs_points2paraview_file(const string filename_rv, const string filename_fp);
	///Output the data of the temperature observation points to ParaView / cvs file
	void output_temp_obs_points2paraview_file(const string filename_rv);

	///Clear the observation points
	void clear_obs_points(void);

	///Clone the observation points
	void clone_obs_points(Hyd_Observation_Point_Manager *src, Hyd_Model_River *river,  Hyd_Model_Floodplain *floodplain);

	///Total reset of the manager
	void total_reset(void);

private:


	//members
	///Number of observation points
	int number_obs_point;
	///Observation points in the models
	Hyd_Observation_Point *obs_point;

	///Number of observation points in a river model
	int number_obs_rv;
	///Number of observation points in a floodplain model
	int number_obs_fp;

	///Counter of used observation points
	int counter_used;



	//method
	///Allocate the obeservation points
	void allocate_obs_point(void);
	///Delete the observation points
	void delete_obs_point(void);

	///Select and count the number of relevant polysegment points in a database table
	static int select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database);

	///Count number river- and floodplain obeservation points
	void count_number_rv_fp_obs_point(void);

	///Output the observation points of river models to tecplot file
	void output_obs_point_rv2file(const string file);
	///Output the observation points of floodplain models to tecplot file
	void output_obs_point_fp2file(const string file);

	///Output the observation points of river models to csv file
	void output_obs_point_rv2csvfile(const string file);
	///Output the observation points of floodplain models to csvfile
	void output_obs_point_fp2csvfile(const string file);

	///Output the temperature observation points of river models to csv file
	void output_temp_obs_point_rv2csvfile(const string file);


	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);


};
#endif
