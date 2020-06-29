#pragma once
/**\class Dam_Hydimport_Element
	\author Kufeld et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef DAM_HYDIMPORT_ELEMENT_H
#define DAM_HYDIMPORT_ELEMENT_H



///Information of the hydraulic results in one element of a raster\ingroup dam
/**

This class is just used in the DAM-project type.

\see Dam_Hyd_Result_Raster

*/

class Dam_Hydimport_Element
{

public:
	///Default Constructor
	Dam_Hydimport_Element(void);
	///Default Deconstructor
	~Dam_Hydimport_Element(void);

	//member
	///Midpoint of the element
	Geo_Point mid_point;

	//methods
	///Set the index of the element
	void set_index(int index);
	///Set the relative waterlevel in the element
	void set_waterlevel(const double waterlevel);
	///Set the maximum flow velocity in the element
	void set_velocity(const double velocity);
	///Set the duration of flooding in the element
	void set_duration_wet(const double duration_wet);
	///Set the maximum speed of the waterlevel rise in the element
	void set_dhdt(const double dhdt);
	///Set the geodetic height of the element
	void set_height(const double height);
	///Set the area of the element
	void set_area_element(const double area);

	///Get the midpoint of the element
	Geo_Point* get_mid_point(void);
	///Get the index of the element
	int get_index(void);
	///Get the relative waterlevel in the element
	double get_waterlevel(void);
	///Get the maximum flow velocity in the element
	double get_velocity(void);
	///Get the duration of flooding in the element
	double get_duration_wet(void);
	///Get the maximum speed of the waterlevel rise in the element
	double get_dhdt(void);
	///Get the geodetic height of the element
	double get_height(void);
	///Get the area of the element
	double get_area_element(void);

private:
	//member
	int index;
	///Relative waterlevel in the element [m]
	double waterlevel;
	///The maximum flow velocity in the element [m/s]
	double velocity;
	///The duration of flooding in the element [s]
	double duration_wet;
	///The maximum speed of the waterlevel rise in the element [m/s]
	double dhdt;
	///The geodetic height of the element [m]
	double height;
	///Area of the element [m²]
	double area;

};
#endif

