#pragma once
/**\class Dam_Raster
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef DAM_RASTER_H
#define DAM_RASTER_H

//sys-libs
#include "_Sys_Common_System.h"
#include "Geo_Raster.h"
//dam-libs
#include "Dam_Impact_Value_Floodplain.h"
#include "Dam_Polygon.h"



///Enumerator for specifiying the type of the damage raster
enum _dam_raster_types{
	///Economical raster with the information about the immobile land-use type [-]
	ecn_immob,
	///Economical raster with the information about the mobile land-use type [-]
	ecn_mob,
	///Economical raster with the information about the individual immobile stock-values [€/m²]
	ecn_immob_stock,
	///Economical raster with the information about the individual mobile stock-values [€/m²]
	ecn_mob_stock,
	///Economical raster with all required information for the damage calculation
	ecn_total,
	
	///Ecological raster with the information about the biotope-type [-]
	eco_btype,
	///Ecological raster with the information about the soil-cost type [-]
	eco_soil_cost,
	///ecological raster with the information about the soil-erosion type [-]
	eco_soil_erosion,
	///Ecological raster with all required information for the soil-erosion damage calculation
	eco_soil_total,
	///All economical raster, biotpe-type raster as well as soil-erosion raster
	eco_all,

	///People2risk raster with the information about the damage category [-]
	pop_dam_category,
	///People2risk/Psycho-social raster with the information about the population density [P/m²]
	pop_density,
	///People2risk raster with all required information for the people2risk damage calculation
	pop_total,

	///Psycho-social raster with the information about the category [-]
	pys_category,
	///Psycho-social raster with the information for the criteria age class 50 to 59 [%]
	pys_crit_age50_59,
	///Psycho-social raster with the information for the criteria age class greater 80 [%]
	pys_crit_age80,
	///Psycho-social raster with the information for the criteria female gender [%]
	pys_crit_female,
	///Psycho-social raster with the information for the criteria home owner [%]
	pys_crit_owner,

	///Psycho-social raster with all required information for the psycho-socia damage calculation
	pys_pys_total,

	///Simple counting points
	sc_sc_total,

	///CI-data
	ci_ci_total,

	//hydraulic results
	///Relative waterlevel h [m]
	hyd_waterlevel,
	///Geodetic height z [m]
	hyd_height,
	///Maximum flow velocity vmax [m/s]
	hyd_velocity,
	///Duration of flooding d [s]
	hyd_duration,
	///Speed of waterlevel rise dh/dt [m/s]
	hyd_dhdt,

	///All raster types
	all_raster

};



///Geometrical class for the raster of the damage calculation \ingroup dam
/**

*/
class Dam_Raster : public Geo_Raster
{

//class for displaying the data as widget
friend class DamGui_Raster_Wid;

public:
	///Default constructor
	Dam_Raster(void);
	///Default destructor
	virtual ~Dam_Raster(void);

	//method
	///Read in the raster data from file
	void read_damage_raster_per_file(const string file_dam_raster);

	///Initialize the raster: the points, segments and characteristics
	void init_raster(void);
	///Delete the raster information: points and segments and characteristics
	void clear_raster(void);

	///Output the members of geometrical damage raster to display/console
	virtual void output_member(void);

	///Get the characteristics as double value
	double get_characteristics_asdouble(const int index);
	///Get the characteristics as integer value
	int get_characteristics_asint(const int index);

	///Get the characteristics as double value
	double get_no_info_asdouble(void);
	///Get the characteristics as integer value
	int get_no_info_asint(void);

	///Get the raster name
	string get_raster_name(void);

	///Set the raster number
	void set_raster_number(const int number);
	///Get the raster number
	int get_raster_number(void);

	///Compare two raster for equality of the raster informations
	bool compare_raster_infos(Dam_Raster *buffer);

	///Check if two rasters are intercepting each other 0:=no interception; 
	bool check_interception(Dam_Raster *buffer);
	///Check if there are an interception between a raster and a polygon
	bool check_interception(Dam_Polygon *buffer);

	///Set intercepted elements to no-info value
	void set_intercepted_elem2noinfo(Dam_Raster *raster);

	///Get the type of the raster
	_dam_raster_types get_raster_type(void);
	///Set the type of the raster
	void set_raster_type(_dam_raster_types type);


	///Convert a string into a raster type (_dam_raster_types)
	static _dam_raster_types convert_txt2raster_type(const string txt);
	///Convert a raster type (_dam_raster_types) into a string 
	static string convert_raster_type2txt(const _dam_raster_types type);

	///Get the flag if the raster is connected
	bool get_connected_flag(void);

	///Get the flag if the raster is selected
	bool get_is_selected(void);
	///Set the flag if the raster is selected
	void set_raster_is_selected(const bool flag);


protected:
	//members
	///Index/Number of the raster
	int number;
	///Name of the raster
	string name;
	///Value for no available information of an element
	double no_info_value;

	///Flag if the raster is connected
	bool connected_flag;

	///Type of the raster (_dam_raster_types)
	_dam_raster_types type;
	
private:

	//member
	///Characteristic for each element
    double *characteristic;

	///Flag if the raster is selected
	bool is_selected;

	//method
	///Allocate the characteristics for each element
	void allocate_characteristics(void);
	///Delete the characteristics for each element
	void delete_characteristics(void);

	///Delete the element information
	virtual void delete_raster_elem_info(void)=0;

	///Read the raster information per file
	virtual void read_raster_infos(ifstream *ifile, int *line_counter);
	///Read the charactersistics of the elements per file
	void read_raster_elem_characteristic(ifstream *ifile, int *line_counter);
	///Find the key values for the file input
	virtual void find_key_values_file(string myline, int *must_found_counter);

	///Set the characteristics of an element with the given index to no-info
	void set_charcacteristic2no_info(const int index);

	///Sort raster characteristics from read down-right to up-left
	void sort_raster_characteristics(void);

	
	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif
