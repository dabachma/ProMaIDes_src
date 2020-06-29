#pragma once
/**\class _Dam_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef _DAM_SYSTEM_H
#define _DAM_SYSTEM_H



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"

//dam-libs
#include "Dam_Impact_Value_Floodplain.h"
#include "DamGui_Number_Raster_Dia.h"
#include "DamGui_Raster_Dia.h"

///Base class for the damage systems of various categories like economical or ecological damage systems  \ingroup dam
/**

*/
class _Dam_System
{
public:
	///Default constructor
	_Dam_System(void);
	///Default destructor
	~_Dam_System(void);

	//method
	///Calculate the damages
	/*virtual void calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, , const int dam_raster_id)=0;
	///Output the result members to a database table
	virtual void output_result_member2database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int dam_raster_id)=0;*/

	///Output the members
	virtual void output_members(void)=0;
	///Output statistic of economical damage system
	virtual void output_statistic(void)=0;
	///Output the sum over the raster of the damage results to display/console
	virtual void output_result_damage(void)=0;

	///Check the damage system 
	virtual void check_system(void)=0;

	//Get the error number
	int get_error_number(void);

	///Initialize the damage raster(s)
	//virtual void init_damage_rasters(const int index, QSqlDatabase *ptr_database, const _sys_system_id id)=0;

protected:
	//members
	///Number of errors occurred
	int number_error;
	///Number warning at the beginning of an action
	int number_warning_begin;
	///Number warning at the end of an action
	int number_warning;

	///Dialog for multiple raster import (number of raster)
	DamGui_Number_Raster_Dia *number_raster_dia;
	///Dialog for multiple raster import (path of the rasters)
	Sys_Multi_Filechooser_Dia *mulit_raster_import_dia;


	//methods
	///Set the number of warnings before an action
	void set_start_warnings_number(void);
	///Set the warning number, which occured during an action
	void set_warning_number(void);
	///Get the warning number, which occurred during an action
	int get_occured_warning(void);

	///Allocate the dialog for the number of raster
	void allocate_number_raster_dia(void);
	///Delete the dialog for the number of raster
	void delete_number_raster_dia(void);

	///Allocate the dialog of the raster path for a multiple raster import
	void allocate_multi_raster_path_dia(void);
	///Delete the dialog of the raster path for a multiple raster import
	void delete_multi_raster_path_dia(void);


	///Allocate and sort the floodplian models after the size of the elements (from big to small elements)
	void sort_fp_models_elem_size(int **rank, Hyd_Model_Floodplain *fp_models, const int number_fp);

private:
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
