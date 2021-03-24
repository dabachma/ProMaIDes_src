#pragma once
/**\class Dam_Impact_Values
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

//sys-libs
#include "_Sys_Common_System.h"

#ifndef DAM_IMPACT_VALUES_H
#define DAM_IMPACT_VALUES_H

///Enumerator for specifiying the type of the impact value
enum _dam_impact_type{
	///The local waterlevel as impact value
	WATER_H,
	///The flow velocity as impact value
	WATER_V,
	///The wet periode as impact value
	WATER_D,
	///The product of waterlevel times flow velocity
	WATER_VH,
	///Being wet
	WET_DRY
};


///This class handles the impact values for a damage calculation for one floodplain element, e.g. waterlevel of the floodplain elements, which are calculated in the hyd-module \ingroup dam
/**

*/
class Dam_Impact_Values
{
public:
	///Default constructor
	Dam_Impact_Values(void);
	///Default destructor
	~Dam_Impact_Values(void);

	//methods
	///Get the impact value of waterlevel [m]
	double get_impact_h(void);
	///Get the impact value of total flow velocity [m/s]
	double get_impact_v_tot(void);
	///Get the impact value of the wet period [s]
	double get_impact_duration(void);
	///Get the impact value of the product of waterlevel times flow velocity [m²/s]
	double get_impact_vh(void);
	///Get the impact value of the waterlevel rise [m/min]
	double get_impact_dsdt(void);
	///Get the impact value of the first arrival time of water [s]
	double get_impact_first_t(void);
	///Get the watervolume [m³]
	double get_watervolume(void);
	///Get the id of the floodplain element [m³]
	int get_fp_elem_id(void);
	///Get the flag if the hydraulic element was wet
	bool get_was_wet_flag(void);
	///Get the date-time string in instationary case
	string get_date_time_str(void);

	///Read out the member data from a given QSqlTableModel
	void readout_data_from_database_model(QSqlQueryModel *model, const int model_index);
	///Read out the member data from a given QSqlTableModel
	void readout_instat_data_from_database_model(QSqlQueryModel *model, const int model_index);


	///Convert the enumerator (_dam_impact_type) to a text
	static string convert_dam_impact_type2txt(const _dam_impact_type value);
	///Convert a text to the enumerator (_dam_impact_type) 
	static _dam_impact_type convert_txt2dam_impact_type(const string txt);

private:
	///Impact value of the waterlevel [m]
	double impact_h;
	///Impact value of the total flow velocity [m/s]
	double impact_v_tot;
	///Impact value of the wet period [s]
	double impact_duration;
	///Impact value of the product of waterlevel times flow velocity [m²/s]
	double impact_vh;
	///Impact value of the waterlevel rise [m/min]
	double impact_dsdt;
	///Impact value of the first arrival time of water [s]
	double impact_first_t;
	///Watervolume left [m³]
	double watervolume;
	///Element id of the floodplain element
	int fp_elem_id;
	///Flag if the element was wet
	bool was_wet_flag;
	///String for date-time for instationary cases
	string date_time;

};
#endif
