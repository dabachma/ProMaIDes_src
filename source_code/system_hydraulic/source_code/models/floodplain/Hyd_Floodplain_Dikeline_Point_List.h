#pragma once
/**\class Hyd_Floodplain_Dikeline_Point_List
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2013 
*/
#ifndef HYD_FLOODPLAIN_DIKELINE_POINT_LIST_H
#define HYD_FLOODPLAIN_DIKELINE_POINT_LIST_H

//system_hyd
#include "Hyd_Floodplain_Dikeline_Point.h"


///List-class where the floodplain dikeline points are stored and managed \ingroup hyd
/**

\see Hyd_Floodplain_Dikeline_Point
*/
class Hyd_Floodplain_Dikeline_Point_List
{
public:
	///Default constructor
	Hyd_Floodplain_Dikeline_Point_List(void);
	///Default destructor
	~Hyd_Floodplain_Dikeline_Point_List(void);

	//methods
	///Get the number of points
	int get_number_points(void);
	///Get the total list as a pointer
	Hyd_Floodplain_Dikeline_Point_List* get_total_list(void);
	///Get a specific  points with the given index
	Hyd_Floodplain_Dikeline_Point get_point(const int index);
	///Get a specific point pointer with the given index
	Hyd_Floodplain_Dikeline_Point* get_ptr_point(const int index);


	///Set a new points
	void set_new_point(Hyd_Floodplain_Dikeline_Point *new_point);

	///Delete the list
	void delete_list(void);

	///Output the setted members
	void output_setted_members(ostringstream *cout);

	///Use offset of coordinates
	void use_offset_coordinates(const double x_off, const double y_off);

private:

	//members
	///Coupling points in list
	Hyd_Floodplain_Dikeline_Point *points;

	///Number of list-elements in a block for allocation
	const int block_elems;
	///Number of points in list
	int number;


	//methods
	///Delete the allocated points
	void delete_points(void);
	


	///Set error(s)
	Error set_error(const int err_type);
	
};
#endif
