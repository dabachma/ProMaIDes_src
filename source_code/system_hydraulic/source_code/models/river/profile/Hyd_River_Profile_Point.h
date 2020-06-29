#pragma once
/**\class Hyd_River_Profile_Point
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_RIVER_PROFILE_POINT_H
#define HYD_RIVER_PROFILE_POINT_H
//qt classes
#include <QFile>
//sys_classes
#include "Geo_Point.h"

///Class for a geometrical point of a river profile \ingroup hyd
/**
Each point has beside his geometrical information, data about the material, the channel-type etc.

\see _Hyd_River_Profile_Type
*/
class Hyd_River_Profile_Point : public Geo_Point
{
///Class which handles the points for output etc.
friend class _Hyd_River_Profile_Type;
///Class which handles the points for output etc.
friend class Hyd_River_Profile_Type_Standard;

public:
	///Default constructor
	Hyd_River_Profile_Point(void);
	///Default destructor
	~Hyd_River_Profile_Point(void);

	//methods
	///Input the point members per file
	bool input_members(QFile *profile_file, int *point_counter, int *line_counter);
	///Input the river profile point data from a selection of a database table with the given index
	void input_members_per_database(const QSqlQueryModel *query_result, const int index);
	///Output the members to display/console
	void output_members(ostringstream *cout);
	///Get the global x- and y-coodinates
	Geo_Point get_global_x_y_coordinates(void);
	///Get the global z-coordinate of the point
	double get_global_z_coordinate(void);
	///Set the global z-coordinate of the point
	void set_global_z_coordinate(const double height);
	///Get the material identifier
	int get_mat_id(void);
	///Get the channel type identifier
	int get_channel_type(void);


	///Copy points
	void copy_points(Hyd_River_Profile_Point *point);

private:

	//members
	///Point number
	int number;
	///Global x-coordinate for the profile point
	double x_global; 
	///Global y-coordinate for the profile point
	double y_global; 
	///Gloabl z-coordinate (height) for the profile point
	double z_global; 
	///Integer value representing the material type given in a table file (Global)
	int mat_type;
	///Integer value representing the flow field: 1=Left bank, 2=Main Channel, 3=Right bank
	int identity; 

	//methods
	///Set the error(s)
	Error set_error(const int err_type);

};
#endif

