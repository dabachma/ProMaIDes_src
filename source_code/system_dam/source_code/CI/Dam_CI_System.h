#pragma once
/**\class _Dam_System
	\author produced by AG FRM HS-M
	\version 0.0.1
	\date 2021
*/

#ifndef DAM_CI_SYSTEM
#define DAM_CI_SYSTEM



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"
#include "Geo_Point.h"

//dam-libs

#include "Dam_CI_Point.h"
#include "Dam_CI_Polygon.h"

///Class for managing the CI-damage calculation  \ingroup dam
/**
This class manages the input, the calculation and the output of the CI-damage system.
*/
class Dam_CI_System
{
public:
	///Default constructor
	Dam_CI_System(void);
	///Default destructor
	~Dam_CI_System(void);


	//member
	///The CI polygons of the system
	Dam_CI_Polygon *dam_ci_polygon;
	///The CI points of the system
	Dam_CI_Point *dam_ci_point;

	//method
	///Import CI data from a file to the database
	void ci_data_file2database(QSqlDatabase *ptr_database);
	///Delete all CI data in the database: points, polygons, connections, results etc.
	void del_ci_data_database(QSqlDatabase *ptr_database);


	///Ask for the files of the CI-data per dialog
	bool ask_file_CI_data(QWidget *parent = NULL);
	///Ask for the flag, what should be deleted (points, subcategories)
	bool ask_deleting_flag(QWidget *parent);

	///Get a text for deleting CI information in database
	string get_deleting_text(void);

	///Output the members
	void output_members(void);

	///Output statistic of CI system
	void output_statistic(void);

private:

	//members
	///Number of CI points in the system
	int no_ci_point;
	///Number of CI polygons in the system
	int no_ci_polygon;


	///Current filename of the damage function file
	/** It is set via dialog. \see ask_file_CI_data(QWidget *parent=NULL)*/
	string file_ci_point;
	///Current filename of the raster for immobile damages land-use categories
	/** It is set via dialog. \see ask_file_CI_data(QWidget *parent=NULL) */
	string file_ci_polygon;
	///Current filename of the raster for mobile damages land-use categories
	/** It is set via dialog. \see ask_file_CI_data(QWidget *parent=NULL) */
	string file_ci_connection;

	///Flag for deleting the CI data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_data_flag;
	///Flag for deleting the CI result data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_res_flag;

	//methods
	///Allocate the CI points
	void allocate_CI_point(void);
	///Delete the CI points
	void delete_CI_point(void);

	///Allocate the CI polygon
	void allocate_CI_polygon(void);
	///Delete the CI polygon
	void delete_CI_polygon(void);


	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif


