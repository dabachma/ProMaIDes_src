#pragma once
/**\class Hyd_Floodplain_Dikeline_Point
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef HYD_FLOODPLAIN_DIKELINE_POINT_H
#define HYD_FLOODPLAIN_DIKELINE_POINT_H

//system sys
#include "Geo_Point.h"
#include "_Hyd_River_Profile.h"

struct _hyd_break_parameters;
struct _hyd_idealisation_parameters;


///Geometrical class for the point of a dikeline or coastline \ingroup hyd
/**
Here all the relevant information are stored, like the relative height [m],
the break parameters etc.

\see Hyd_Floodplain_Polysegment

*/
class Hyd_Floodplain_Dikeline_Point : public Geo_Point,public _Sys_Common_System
{
public:
	///Default constructor
	Hyd_Floodplain_Dikeline_Point(void);
	///Copy constructor
	Hyd_Floodplain_Dikeline_Point(const Hyd_Floodplain_Dikeline_Point& object);
	///Default destructor
	~Hyd_Floodplain_Dikeline_Point(void);

	//method
	///Set the absolute height of the point (crest)
	void set_abs_height(const double h);
	///Get the absolute height of the point (crest)
	double get_abs_height(void);
	///Set the absolute height of the point (base)
	void set_base_height(const double h);
	///Get the absolute base height of the point (base)
	double get_base_height(void);

	///Set the poleni factor of the point
	void set_poleni_fac(const double pol);
	///Get the poleni factor of the point
	double get_poleni_fac(void);
	///Set the overflow flag of the point
	void set_overflow_flag(const bool flag);
	///Get the overflow flag of the point
	bool get_overflow_flag(void);

	///Set the break flag of the point
	void set_break_flag(const bool flag);
	///Get the break flag of the point
	bool get_break_flag(void);
	///Set the break parameters of the point
	void set_break_param(const _hyd_break_parameters param);
	///Get the break parameters of the point
	_hyd_break_parameters get_break_param(void);

	///Get a pointer to the break parameters of the point
	_hyd_break_parameters * get_ptr_break_param(void);

	///Check the dikeline point
	void check_dikeline_point(void);

	///Set the distance to the beginning of a polysegment from the dikeline point
	void set_distance2begin(const double distance);
	///Get the distance to the beginning of a polysegment from the dikeline point
	double get_distance2begin(void);

	///Get the fpl-section id (if existing )of the dikeline point
	int get_fpl_section_id(void);

	///Idealise the dikeline points with given parameters of a fpl-section
	void idealise_points_fpl(_hyd_idealisation_parameters ideal_param,  const int section_id, const bool last_flag);

	///Output the header for the setted member (static)
	static void output_header_setted_member(ostringstream *cout);
	///Output the setted members
	void output_setted_members(ostringstream *cout);


	///Copy operator
	Hyd_Floodplain_Dikeline_Point& operator=(const Hyd_Floodplain_Dikeline_Point& object);






private:
	//members
	///Absolute height of the point (crest)
	double abs_height;
	///Absolute height of the point (base)
	double base_height;

	///Poleni factor
	double poleni_fac;

	///Overflow flag
	bool overflow_flag;

	///Break flag
	bool break_flag;
	///Break parameters
	_hyd_break_parameters break_param;

	///Distance to the begin of a dikeline
	double distance2begin;

	///Flag if the point is idealised by a FPL-section
	bool point2fpl;

	///Section id of a connected FPL-section
	int fpl_sec_id;


	//method
	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

};
#endif
