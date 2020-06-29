#pragma once
/**\class Fpl_Seepage_Calculator_Dike
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef FPL_SEEPAGE_CALCULATOR_DIKE_H
#define FPL_SEEPAGE_CALCULATOR_DIKE_H

//system fpl
#include "Fpl_Cub_Dike_Waterside.h"
#include "Fpl_Cub_Dike_Landside.h"
#include "Fpl_Seepage_Line_Point_List.h"

///Enumerator for specifiying the seepage type at maximum waterlevel \ingroup fpl
enum _fpl_max_waterlevel_seepage{
	///Seepage line to the landside base point
	land_base,
	///Seepage line to one third of the waterlevel
	one_third_waterlevel,
	///Seepage calculation after Kozeny
	kozeny,
	///Unknown seepage
	unknown_max_seepage,
};

///Enumerator for specifiying the seepage type at minimum waterlevel \ingroup fpl
enum _fpl_min_waterlevel_seepage{
	///Seepage line to the waterside base point
	water_base,
	///Seepage line from midwaterlevel to one third of the waterlevel at the waterside
	one_third_mid_waterlevel,
	///Unknown seepage
	unknown_min_seepage,
};
///Namespace for the keystring in a database table of the seepage calculation method \ingroup fpl
namespace fpl_seepage_method{

	///Calculation method for the maximum waterlevel
	const string seepage_maximum("SEEPAGE_MAXIMUM");
	///Calculation method for the maximum waterlevel
	const string seepage_minimum("SEEPAGE_MINIMUM");

};


///Calculator-class for the seepage line through a dike section \ingroup fpl
/**


*/
class Fpl_Seepage_Calculator_Dike
{

//widget for displaying the parameter
friend class FplGui_System_Member_Wid; 

public:
	///Default constructor
	Fpl_Seepage_Calculator_Dike(void);
	///Default destructor
	~Fpl_Seepage_Calculator_Dike(void);


	//method
	///Set the pointer to waterside cubature;
	void set_ptr_waterside_cub(Fpl_Cub_Dike_Waterside *cub);
	///Set the pointer to landside cubature;
	void set_ptr_landside_cub(Fpl_Cub_Dike_Landside *cub);

	///Set the pointer to the cubature
	void set_ptr_cubature(Geo_Polysegment *cubature);

	///Calculate new waterlevels of the seepage line
	void calculate_waterlevel_seepage_line(const double water_level, Fpl_Seepage_Line_Point_List *ascending, Fpl_Seepage_Line_Point_List *descending, const bool for_output);

	///Input the control parameters per database table
	void set_input(QSqlDatabase *ptr_database, const bool frc_sim, const bool output);

	///Output the control parameters of the seepage calculation to display/console
	void output_members(void);

	///Convert a string into seepage type at maximum waterlevel (_fpl_max_waterlevel_seepage)
	static _fpl_max_waterlevel_seepage convert_txt2seepagetype_max_waterlevel(const string txt);
	///Convert a seepage type at maximum waterlevel (_fpl_max_waterlevel_seepage) into a string 
	static string convert_seepagetype_max_waterlevel2txt(const _fpl_max_waterlevel_seepage type);

	///Convert a string into seepage type at minimum waterlevel (_fpl_min_waterlevel_seepage)
	static _fpl_min_waterlevel_seepage convert_txt2seepagetype_min_waterlevel(const string txt);
	///Convert a seepage type at minimum waterlevel (_fpl_min_waterlevel_seepage) into a string 
	static string convert_seepagetype_min_waterlevel2txt(const _fpl_min_waterlevel_seepage type);

	///Write the default value of the control parameters into the database table (static)
	static void set_predefined_data2control_table(QSqlDatabase *ptr_database, QSqlQuery *model, int *id_glob, ostringstream *fix_string);




private:

	//member
	///Pointer to the waterside cubature
	Fpl_Cub_Dike_Waterside *ptr_waterside_cub;
	///Pointer to the landside cubature
	Fpl_Cub_Dike_Landside *ptr_landside_cub;

	///Pointer to the cubature
	Geo_Polysegment *ptr_cubature;

	///Seepage type calculation for the maximum waterlevel
	_fpl_max_waterlevel_seepage seepage_calc_max;

	///Seepage type calculation for the minimum waterlevel
	_fpl_min_waterlevel_seepage seepage_calc_min;



	//method
	///Calculate the seepage line: horizontal
	void calculate_seepage_line_horizontal(double h_rel, Fpl_Seepage_Line_Point_List *point_list, const bool for_output);
	///Calculate the seepage line to the mid: one third of the waterlevel at the crest mid at the waterside to waterlevel at crest mid; the rest have to be calculated before
	void calculate_seepage_line_mid_water_one_third(double h_rel, Fpl_Seepage_Line_Point_List *point_list, const bool for_output);
	///Calculate the seepage line to the mid: base point at the waterside to the waterlevel at the crest mid; the rest have to be calculated before
	void calculate_seepage_line_mid_water_base(double h_rel, Fpl_Seepage_Line_Point_List *point_list, const bool for_output);


	///Calculate the seepage line: to the landside base point
	void calculate_seepage_line_land_base(double h_rel, Fpl_Seepage_Line_Point_List *point_list, const bool for_output);
	///Calculate the seepage line: to the landside one third of the waterlevel
	void calculate_seepage_line_land_one_third(double h_rel, Fpl_Seepage_Line_Point_List *point_list, const bool for_output);
	///Calculate the seepage line: to the landside after the Kozeny
	void calculate_seepage_line_land_kozeny(double h_rel, Fpl_Seepage_Line_Point_List *point_list, const bool for_output);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set the error(s)
	Error set_error(const int err_type);
	


};

#endif