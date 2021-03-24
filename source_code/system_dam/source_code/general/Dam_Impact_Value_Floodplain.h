#pragma once
/**\class Dam_Impact_Value_Floodplain
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

//sys-libs
#include "_Sys_Common_System.h"
//dam-libs
#include "Dam_Impact_Values.h"

#ifndef DAM_IMPACT_VALUE_FLOODPLAIN_H
#define DAM_IMPACT_VALUE_FLOODPLAIN_H

///This class handles the impact values for a damage calculation for one floodplain, e.g. waterlevel of the floodplain elements, which are calculated in the hyd-module \ingroup dam
/**

*/
class Dam_Impact_Value_Floodplain
{
public:
	///Default constructor
	Dam_Impact_Value_Floodplain(void);
	///Default destructor
	~Dam_Impact_Value_Floodplain(void);

	//members
	///Pointer to the impact values of one element of the floodplain; it is allocated and deleted in this class
	Dam_Impact_Values *impact_values;

	//method
	///Set the number of elements
	void set_number_element(const int number);
	///Get the number of elements
	int get_number_element(void);

	///Set the index of the floodplain
	void set_floodplain_index(const int index);
	///Get the index of the floodplain
	int get_index_floodplain(void);

	///Analyse the time-date string
	static double analyse_date_time(const QStringList list);

	///Set the impact values from database
	void set_impact_values_database(const int bound_sz, const string break_sz, QSqlDatabase *ptr_database, const _sys_system_id sys_id);

	///Set the instationary impact values from database
	void set_instationary_impact_values_database(const int bound_sz, const string break_sz, QSqlDatabase *ptr_database, const _sys_system_id sys_id, const string time_date);

private:

	//members
	///Number of elements, which are managed for this floodplain
	int number_element;

	///Index of the floodplain
	int index_floodplain;

	//methods
	///Allocate the impact values for the elements 
	void allocate_impact_values(void);
	///Delete the impact values for the elements
	void delete_impact_values(void);

	///Set error(s)
	Error set_error(const int err_type);



};
#endif
