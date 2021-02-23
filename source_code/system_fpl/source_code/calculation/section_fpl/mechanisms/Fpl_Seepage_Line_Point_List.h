#pragma once
/**\class Fpl_Seepage_Line_Point_List
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef FPL_SEEPAGE_LINE_POINT_LIST_H
#define FPL_SEEPAGE_LINE_POINT_LIST_H

//system sys

//system fpl
#include "Fpl_Seepage_Line_Point.h"





///List class to gather the relevant points of a seepage line \ingroup fpl
/**
The indices of the points are stored in the geometrical classes of the
mechanisms like the slices of a slope.

\ see Fpl_Seepage_Line_Point, Fpl_Slope_Slices
*/
class Fpl_Seepage_Line_Point_List
{
public:
	///Default constructor
	Fpl_Seepage_Line_Point_List(void);
	///Default destructor
	~Fpl_Seepage_Line_Point_List(void);

	//method
	///Add a new point to the list
	int add_new_point(const double x, const double h, const bool inside_body, Geo_Polysegment *cubature);
	///Get a pointer to a point specified by a given index
	Fpl_Seepage_Line_Point *get_list_point(const int index);
	///Get the number of points
	int get_number_points(void);

	///Output member to display/console
	void output_members(Geo_Polysegment *cubature);

	///Output members to tecplot
	void output_members2tecplot(ofstream *file_name, Geo_Polysegment *cubature);

	///Output members to paraview
	void output_members2paraview(ofstream *file_name, Geo_Polysegment *cubature, const int before, const int after);
	///Output members to excel
	void output_members2excel(ofstream *file_name, Geo_Polysegment *cubature, const int before, const int after);

	///Count the number of points inside the section body#
	int count_number_inside_points(void);


private:

	//member
	///Number of points
	int number_points;
	///Pointer to the points
	Fpl_Seepage_Line_Point *points;


	//method
	///Delete the points of the list
	void delete_points(void);
	///Allocate the points of the list
	void allocate_points(void);

	///Sort the list after the ascending x-values
	/**This method is just needed for an readable output of the list. Don not use it
	for a list, which is connected to the geometrical classes of the mechanism, because the 
	indices are stored there. By sorting the indices are mixed.
	*/
	void sort_list(Fpl_Seepage_Line_Point_List *list);

	///Set the error(s)
	Error set_error(const int err_type);

};
#endif
