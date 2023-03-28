#pragma once
#ifndef SYS_VERSION_UPDATE_H
#define SYS_VERSION_UPDATE_H
/**\class Sys_Version_Update
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2011
*/

///Class for check and update old ProMaIDes-versions \ingroup sys
/**

*/
class Sys_Version_Update
{
public:
	///Default constructor
	Sys_Version_Update(void);
	///Default destructor
	~Sys_Version_Update(void);

	//method
	///Check and update the hydraulic table for observation points (20.12.2011)
	void check_update_hyd_table_obs_point(QSqlDatabase *ptr_database);
	///Check and update the text of the hydraulic table of the polysegment (20.12.2011)
	void check_update_hyd_table_polysegment_name(const string project_file);
	///Check and update the hydraulic table of the hydraulic system members (21.02.2012)
	void check_update_hyd_table_system_member_sync(QSqlDatabase *ptr_database, const string project_file);

	///Check and update the result members table of the fpl section type dike (21.05.2012)
	void check_update_fpl_table_result_sec_type_dike(QSqlDatabase *ptr_database, const string project_file);
	///Check for update fpl table for results of section type dune (5.5.2012)
	void check_update_fpl_table_dune_results(QSqlDatabase *ptr_database);

	///Check and update the text of the hydraulic table of the hydraulic element result members; smax is introduced (6.07.2012)
	void check_update_hyd_table_elem_result_smax(QSqlDatabase *ptr_database, const string project_file);

	///Check and update the hydraulic global parameter table with output settings (3.9.2020)
	void check_update_hyd_table_global_param(QSqlDatabase *ptr_database, const string project_file);

	///Check and update the hydraulic table for instationary floodplain results (7.9.2020)
	void check_update_hyd_table_instat_results(QSqlDatabase *ptr_database);

	///Check and update the hydraulic table for instationary river results (16.9.2020)
	void check_update_hyd_table_instat_results_rv(QSqlDatabase *ptr_database, const string project_file);

	///Check and update the hydraulic view for boundary conditions to floodplain elements / river profile (3.2.2021)
	void check_update_hyd_view_bound2elements_profile(QSqlDatabase *ptr_database);


	///Check and update the output control parameter for FPL-module (22.2.2021)
	void check_update_fpl_output_control(QSqlDatabase *ptr_database);

	///Check and update the CI tables for DAM-module (11.3.2021)
	void check_update_dam_ci(QSqlDatabase *ptr_database, const string project_file);

	///Check and add columns to the CI result-tables of the CI-connectors for DAM-module (5.7.2021)
	void check_update_connect_results_dam_ci(QSqlDatabase *ptr_database, const string project_file);

	///Check and add columns to the CI point/polygon-tables of the CI-elements for DAM-module (21.9.2021)
	void check_update_dam_ci_elements(QSqlDatabase *ptr_database, const string project_file);

	///Check and add columns to the Dam-result and risk-result table for CI-results for DAM/RISK-module (8.3.2023)
	void check_update_ci_reults(QSqlDatabase *ptr_database, const string project_file);


private:
	///Check and update the text of the hydraulic table of the hydraulic river profile result members; width_max is introduced (18.02.2021)
	void check_update_hyd_table_river_result_width(QSqlDatabase *ptr_database, const string project_file);



	///Set error(s)
	Error set_error(const int err_type);
};
#endif