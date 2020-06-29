#pragma once
/**\class _Dam_Element
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef _DAM_ELEMENT_H
#define _DAM_ELEMENT_H

//sys-libs
#include "_Sys_Common_System.h"

///Base class for the damage element information, mainly the indices of the connected floodplain/ -elements  \ingroup dam
/**

*/
class _Dam_Element : public _Sys_Common_System 
{
public:
	///Default constructor
	_Dam_Element(void);
	///Default destructor
	virtual ~_Dam_Element(void);

	//members
	///Midpoint of the element
	Geo_Point mid_point;
	///Polygon-string for inserting the geometrical data of the element (polygon to database)
	string polygon_string;

	//method
	///Set the index of the connected floodplain
	void set_index_floodplain(const int index);
	///Get the index of the connected floodplain
	int get_index_floodplain(void);

	///Get the midpoint of the element
	Geo_Point* get_mid_point(void);

	///Set the index of the connected floodplain element
	void set_index_floodplain_element(const int index);
	///Get the index of the connected floodplain element
	int get_index_floodplain_element(void);

	///Set the area of the element
	void set_area_element(const double area);
	///Get the area of the element
	double get_area_element(void);

	///Set the index of the element
	void set_index_element(const int index);

	///Get the index of the element
	int get_element_index(void);


protected:
	//members
	///Index of the element
	int elem_index;
	///Index of the floodplain, to which the damage element is connected
	int index_fp;
	///Index of the floodplain element, to which the damage element is connected
	int index_fp_elem;
	///Area of the element
	double area;
	///Global index of the element in the database
	int global_index;

    ///Get the sql string for a raster element polygon
    string get_elem_ploy_string(const double mid_x, const double mid_y, const double half_width_x,const double half_width_y);


private:

	//members


};
#endif
