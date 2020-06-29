#pragma once
/**\class Risk_Damage_Predictor
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef RISK_DAMAGE_PREDICTOR_H
#define RISK_DAMAGE_PREDICTOR_H

//system sys
#include "_Sys_Common_System.h"
#include "Common_Const.h"

//system risk
#include "Risk_Dam_Results.h"


///Data structure for storing the predicted risk results and their part of the total predicted risk
struct _risk_predicted_risk{

	///Economical risk
	double ecn_risk;
	///Percentage of the predicted economical risk
	double ecn_risk_perc;
	///Ecological risk
	double eco_risk;
	///Percentage of the predicted ecological risk
	double eco_risk_perc;

	///People2risk of the affected population
	double pop_affected_risk;
	///Percentage of the people2risk of the affected population
	double pop_affected_risk_perc;
	///People2risk of the endangered population
	double pop_endangered_risk;
	///Percentage of the people2risk of the endangered population
	double pop_endangered_risk_perc;

	///Psycho-social risk without the density criteria
	double pys_without_density_risk;
	///Percentage of the psycho-social risk without the density criteria
	double pys_without_density_risk_perc;
	///Psycho-social risk with the density criteria
	double pys_with_density_risk;
	///Percentage of the psycho-social risk with the density criteria
	double pys_with_density_risk_perc;

	///Middle of percentage
	double middle_perc;
};


///Class for the prediction of damages for the catchment area risk scenarios \ingroup risk
/**


\see Risk_Break_Scenario_Management, Dam_Damage_System, Risk_Dam_Results

*/
class Risk_Damage_Predictor : public _Sys_Common_System
{
public:
	///Default constructor
	Risk_Damage_Predictor(void);
	///Default destructor
	~Risk_Damage_Predictor(void);

	//member
	///Pointer to the table for detailed predicted results of the risk calculation (risk per scenario)
	static Tables *table_detailed_predicted_results;

	//method

	///Set the database table for detailed predicted results of the risk calculation: it sets the table name and the name of the columns and allocate them
	static void set_resulttable_predicted_detailed(QSqlDatabase *ptr_database);
	///Create the database table for detailed predicted  results of the risk calculation
	static void create_resulttable_predicted_detailed(QSqlDatabase *ptr_database);
	///Delete all data in the database table for detailed predicted results of the risk calculation
	static void delete_data_in_resulttable_predicted_detailed(QSqlDatabase *ptr_database);
	///Delete all data in the database table for detailed predicted results of the risk calculation
	static void delete_data_in_resulttable_predicted_detailed(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for detailed predicted results of the risk calculation
	static void close_resulttable_predicted_detailed(void);

	///Select the predicted risk results for the given identifiers from database table
	static int select_predicted_risk_result(QSqlQueryModel *model, QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc_id, const string break_sc_id);

	///Update the database table of the predicted damages with the percentage
	static void update_predicted_result_by_percentage(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const string break_sc, _risk_predicted_risk predict);

	///Predict the damage for a given break scenario
	void predict_damages(QSqlDatabase *ptr_database, Risk_Break_Scenario *break_sc, Risk_Catchment_Break_Scenario_List *list_sc_break);


	///Initialize the data structure for the predicted risk storing (\see _risk_predicted_risk)
	static void init_risk_predicted_risk_structure(_risk_predicted_risk *init);

private:

	//member
	///Number of involved one break scenarios
	int number;
	///Pointer to the break scenario, which results should be predicted
	Risk_Break_Scenario *break_sc2predict;

	///Pointer to economical damage results
	Risk_Dam_Results *ecn_results;
	///Pointer to ecological biotope-type damage results
	Risk_Dam_Results *eco_btype_results;
	///Pointer to ecological soil-erosion damage results
	Risk_Dam_Results *eco_soil_results;
	///Pointer to people2risk affected people results
	Risk_Dam_Results *pop_affected_results;
	///Pointer to people2risk endangered people results
	Risk_Dam_Results *pop_endangered_results;
	///Pointer to psycho-social results without the density criteria
	Risk_Dam_Results *pys_results;
	///Pointer to psycho-social results with the density criteria
	Risk_Dam_Results *pys_dens_results;

	///Total predicted economical damages
	double total_pred_ecn_dam;
	///Total predicted ecological damages
	double total_pred_eco_dam;
	///Total predicted people2risk affected damages
	double total_pred_pop_aff_dam;
	///Total predicted people2risk endangered damages
	double total_pred_pop_end_dam;
	///Total predicted psycho-social without density criteria damages
	double total_pred_pys_without_dens_dam;
	///Total predicted psycho-social with density criteria damages
	double total_pred_pys_with_dens_dam;

	///BNreak scenario list with the best information to get for the damage prediction
	Risk_Catchment_Break_Scenario_List info_list;


	//method
	///Allocate the data storing structure of the damage results
	void allocate_dam_result(void);
	///Delete the data storing structure of the damage results
	void delete_dam_result(void);

	///Set the economical damages from database table
	void set_economical_damages(QSqlDatabase *ptr_database);
	///Set the ecological biotope-type damages from database table
	void set_ecological_btype_damages(QSqlDatabase *ptr_database);
	///Set the ecological soil-erosion damages from database table
	void set_ecological_soil_damages(QSqlDatabase *ptr_database);
	///Set the people2risk damages from database table
	void set_pop_damages(QSqlDatabase *ptr_database);
	///Set the psycho-social results  from database table
	void set_pys_damages(QSqlDatabase *ptr_database);

	///Predict the economical damages
	void predict_economical_damages(void);
	///Predict the ecological biotope-type damages
	void predict_ecological_btype_damages(void);
	///Predict the ecological soil-erosion damages
	void predict_ecological_soil_damages(void);
	///Predict the people2risk affected damages
	void predict_pop_affected_damages(void);
	///Predict the people2risk endangered damages
	void predict_pop_endangered_damages(void);
	///Predict the psycho-social damages without density criteria 
	void predict_pys_without_dens_damages(void);
	///Predict the psycho-social damages with density criteria 
	void predict_pys_with_dens_damages(void);

	///Transfer results to database table
	void transfer_predicted_result2database(QSqlDatabase *ptr_database);

	///Set error(s)
	Error set_error(const int err_type);

};
#endif
