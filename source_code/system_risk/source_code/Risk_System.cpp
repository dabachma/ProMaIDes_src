#include "Risk_Headers_Precompiled.h"
//#include "Risk_System.h"

//init static members
bool Risk_System::abort_thread_flag=false;
Tables *Risk_System::table_config=NULL;
Tables *Risk_System::table_detailed_results=NULL;
Tables *Risk_System::table_results=NULL;
Tables *Risk_System::table_cumulated_results=NULL;
Tables *Risk_System::table_results_elem_ecn=NULL;
Tables *Risk_System::table_results_elem_eco_btype=NULL;
Tables *Risk_System::table_results_elem_eco_soil=NULL;
Tables *Risk_System::table_results_elem_pys=NULL;
Tables *Risk_System::table_results_elem_pop=NULL;
Tables *Risk_System::table_results_point_sc=NULL;

//Default constructor
Risk_System::Risk_System(void){

	this->thread_type=_risk_thread_type::risk_thread_unknown;
	

	//control params
	this->id_main_river=-1;
	this->control_params_ca.max_hyd_calc=risk_control_param::max_hydraulic_def;
	this->control_params_ca.prob_bound=risk_control_param::boundary_prob_def;
	this->control_params_ca.boundary_ecn_risk=risk_control_param::boundary_risk_predicted_def;
	this->control_params_ca.boundary_eco_risk=risk_control_param::boundary_risk_predicted_def;
	this->control_params_ca.boundary_pop_affected_risk=risk_control_param::boundary_risk_predicted_def;
	this->control_params_ca.boundary_pop_endangered_risk=risk_control_param::boundary_risk_predicted_def;
	this->control_params_ca.boundary_pys_risk=risk_control_param::boundary_risk_predicted_def;
	this->control_params_ca.number_intervals=2;
	this->control_params_ca.number_mc=risk_control_param::no_mc_def;
	this->control_params_ca.use_predict_values=true;
	this->control_params_ca.gen_type=_fpl_random_generator_type::gen_type_ranmar;

	//deleting flags
	this->restore_config=false;
	this->del_nobreak_result=false;
	this->del_scenario_result=false;
	this->del_catchment_result=false;
	this->del_dam_results=false;
	this->del_hyd_results=false;
	this->del_fpl_results=false;

	this->hyd_dam_fpl_del_flag = false;

	this->risk_states.reliability_dam_result=false;
	this->risk_states.reliability_fpl_result=false;
	this->risk_states.reliability_hyd_result=false;
	this->risk_states.risk_state_flag=false;

	this->hydraulic_state.file_output_required=true;
	this->hydraulic_state.number_threads=3;

	this->hyd_calc=NULL;
	this->dam_calc=NULL;
	this->fpl_calc=NULL;
	this->fpl_system_working=false;
	this->hyd_break_calc=NULL;

	this->risk_type=_risk_type::catchment_risk;

	this->detailed_results=NULL;
	this->number_detailed_results=0;
	this->cumulated_results=NULL;
	this->number_cumulated_results=0;
	this->init_risk_result_struct(&this->total_results);
	this->total_results.number_applied_result=0;
	
	this->fpl_thread_is_used_extern=false;

	this->thread_is_started=false;

	this->calc_ca_nobreak_step=false;
	this->calc_ca_sc_lists_step=false;
	this->calc_ca_one_break_step=false;
	this->calc_ca_auto_step=false;

	Risk_Damage_Predictor::init_risk_predicted_risk_structure(&this->total_predicted);
	Risk_Damage_Predictor::init_risk_predicted_risk_structure(&this->calculated_predicted);

	

	Sys_Memory_Count::self()->add_mem(sizeof(Risk_System)-sizeof(Risk_Break_Scenario_Management)+sizeof(_risk_ca_scenario_control),_sys_system_modules::RISK_SYS);//count the memory

}
//Default destructor
Risk_System::~Risk_System(void){

	//close database
	if(this->qsqldatabase.isOpen()){
		this->qsqldatabase.close();
	}
	this->delete_dam_system();
	this->delete_fpl_system();
	this->delete_hyd_system();
	this->delete_detailed_results();
	this->delete_cumulated_results();
	this->delete_hyd_break_calculation();
	this->set_stop_thread_flag(false);
	Fpl_Calculation::set_stop_thread_flag(false);
	Hyd_Multiple_Hydraulic_Systems::set_stop_thread_flag(false);
	Dam_Damage_System::set_stop_thread_flag(false);

	Sys_Memory_Count::self()->minus_mem(sizeof(Risk_System)-sizeof(Risk_Break_Scenario_Management)+sizeof(_risk_ca_scenario_control),_sys_system_modules::RISK_SYS);//count the memory

}
//________
//public
//Set the database table for the configuration: it sets the table name and the name of the columns and allocate them (static)
void Risk_System::set_table_config(QSqlDatabase *ptr_database){
	if(Risk_System::table_config==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_risk_config;
		string tab_col[4];
		tab_col[0]=label::glob_id;
		tab_col[1]=risk_label::name_param;
		tab_col[2]=risk_label::value_param;
		tab_col[3]=label::description;

		
		//set the table
		try{
			Risk_System::table_config= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_System::table_config->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_System::set_table_config(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Risk_System::close_table_config();
			throw msg;
		}
	}
}
//Create the database table for configuration (static)
bool Risk_System::create_table_config(QSqlDatabase *ptr_database){
		if(Risk_System::table_config==NULL){
			ostringstream cout;
			cout << "Create database table of risk configuration..." << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			//make specific input for this class
			const string tab_name=risk_label::tab_risk_config;
			const int num_col=4;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=risk_label::name_param;
			tab_col[1].type=sys_label::tab_col_type_string;

			tab_col[2].name=risk_label::value_param;
			tab_col[2].type=sys_label::tab_col_type_string;

			tab_col[3].name=label::description;
			tab_col[3].type=sys_label::tab_col_type_string;



			try{
				Risk_System::table_config= new Tables();
				if(Risk_System::table_config->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_risk->output_txt(&cout);
					Risk_System::close_table_config();
					return false;
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Risk_System::create_table_config(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Risk_System::close_table_config();
				throw msg;
			}
			Risk_System::close_table_config();	
			return true;
	}
	return false;
}
//Reset all data in the database table for the configuration to the default values (static)
void Risk_System::reset_data_in_table_config(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_config(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlTableModel model(0,*ptr_database);
	int number_result=0;
	//give the complet table of control parameters FPL
	model.setTable(Risk_System::table_config->get_table_name().c_str());
    //set the query
	Data_Base::database_request(&model);
	number_result=model.rowCount();
	string buffer;
	int river_id=-1;

	for(int i=0; i< number_result; i++){
		buffer=model.record(i).value((Risk_System::table_config->get_column_name(risk_label::name_param)).c_str()).toString().toStdString();
		if(buffer==risk_control_param::main_outflow_rv){
			river_id=model.record(i).value((Risk_System::table_config->get_column_name(risk_label::value_param)).c_str()).toInt();	
		}
	}


	//delete the table
	Risk_System::table_config->delete_data_in_table(ptr_database);
	//write the dfault data
	Risk_System::set_predefined_data2config_table(ptr_database, river_id);

}
//Close and delete the database table for the configuration (static)
void Risk_System::close_table_config(void){
	if(Risk_System::table_config!=NULL){
		delete Risk_System::table_config;
		Risk_System::table_config=NULL;
	}
}
//Set the predefined data to the configuration database table; it is called after the creation of this table (static)
void Risk_System::set_predefined_data2config_table(QSqlDatabase *ptr_database , const int river_id){
	try{
		Risk_System::set_table_config(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);

	//evaluate the global identifier
	int id_glob=Risk_System::table_config->maximum_int_of_column(Risk_System::table_config->get_column_name(label::glob_id),ptr_database)+1;
	ostringstream total;

	//set the query via a query string
	ostringstream query_string_fix;
	query_string_fix << "INSERT INTO  " << Risk_System::table_config->get_table_name();
	query_string_fix <<" ( ";
	query_string_fix << Risk_System::table_config->get_column_name(label::glob_id) <<" , ";
	query_string_fix << Risk_System::table_config->get_column_name(risk_label::name_param) <<" , ";
	query_string_fix << Risk_System::table_config->get_column_name(risk_label::value_param) <<" , ";
	query_string_fix << Risk_System::table_config->get_column_name(label::description) <<" ) ";

	//mc
	//Keystring for river id of maximum outflow discharge 
	ostringstream query_string;
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< risk_control_param::main_outflow_rv <<"'"<< " , " ;
	query_string << river_id << " , " ;
	query_string << "'The id of the river, which outflow discharge is relevant for the downstream riparian'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//Keystring for the maximum of hydraulic calculation before the damage calculation is performed and the search for new break scenarios restarts
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< risk_control_param::max_hydraulic <<"'"<< " , " ;
	query_string <<risk_control_param::max_hydraulic_def << " , " ;
	query_string << "'Maximum of hydraulic calculation for the catchment risk calculation'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;


	//Keystring for the boundary of cummulated probability when a search for further break scenarios is stopped (maximum is 1.0)
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< risk_control_param::boundary_prob <<"'"<< " , " ;
	query_string <<risk_control_param::boundary_prob_def << " , " ;
	query_string << "'Boundary of sum up probability of catchment area risk scenarios, which should be calculated (maximum is 1.0)'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;


	//Keystring for the number of the monte carlo simulation for the generation of catchment area risk scenarios
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< risk_control_param::number_mc_sim <<"'"<< " , " ;
	query_string <<risk_control_param::no_mc_def << " , " ;
	query_string << "'Number of monte-carlo runs for the generation of the break scenarios of the catchment area risk approach'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//Keystring for the type of random generator for the monte carlo simualtion for the generation of catchment area risk scenarios
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< risk_control_param::rand_generator <<"'"<< " , " ;
	query_string <<"'"<< Fpl_Mc_Sim::convert_generator_type2text(_fpl_random_generator_type::gen_type_ranmar) << "' , " ;
	query_string << "'Type of random generator for the generation of the break scenarios of the catchment area risk approach [RANMAR, MOTHER_OF_ALL, SFMT]'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//Keystring for the number of intervals a frc curve is divided for the generation of catchment area risk scenarios
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< risk_control_param::number_intervals <<"'"<< " , " ;
	query_string << 2 << " , " ;
	query_string << "'Number of intervals a frc curve is divided for the generation of the catchment area risk [min. 2]'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//Keystring if the predicted values should be used in the catchment area risk approach
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< risk_control_param::use_prediction <<"'"<< " , " ;
	query_string << "true" << " , " ;
	query_string << "'Flag if the predicted values are integrated in the catchment area risk approach'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

}
//Set the database table for detailed results of the risk calculation: it sets the table name and the name of the columns and allocate them (static)
void Risk_System::set_resulttable_detailed(QSqlDatabase *ptr_database, const bool not_close){
	if(Risk_System::table_detailed_results==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_risk_detailed;
		string tab_col[46];
		tab_col[0]=label::glob_id;
		tab_col[1]=label::areastate_id;
		tab_col[2]=label::measure_id;
		tab_col[3]=hyd_label::sz_bound_id;
		tab_col[4]=risk_label::sz_break_id;
		tab_col[5]=risk_label::risk_type;
		tab_col[6]=risk_label::max_outflow;
		tab_col[7]=dam_label::immob_dam;
		tab_col[8]=dam_label::mob_dam;
		tab_col[9]=dam_label::immob_5_quant;
		tab_col[10]=dam_label::immob_95_quant;
		tab_col[11]=dam_label::mob_5_quant;
		tab_col[12]=dam_label::mob_95_quant;
		tab_col[13]=dam_label::btype_cost;
		tab_col[14]=dam_label::soil_cost;
		tab_col[15]=dam_label::pop_affected;
		tab_col[16]=dam_label::pop_endangered;
		tab_col[17]=dam_label::crit_age_50_59;
		tab_col[18]=dam_label::crit_age_80;
		tab_col[19]=dam_label::crit_female;
		tab_col[20]=dam_label::crit_owner;
		tab_col[21]=dam_label::denscrit_age50_59;
		tab_col[22]=dam_label::denscrit_age_80;
		tab_col[23]=dam_label::denscrit_female;
		tab_col[24]=dam_label::denscrit_owner;
		tab_col[25]=hyd_label::sz_bound_name;
		tab_col[26]=dam_label::sc_pub_build;
		tab_col[27]=dam_label::sc_eco_build;
		tab_col[28]=dam_label::sc_cult_build;
		tab_col[29]=dam_label::sc_person_build;

		tab_col[45] = dam_label::ci_elect_pt;
		tab_col[30] = dam_label::ci_info_tec_pt;
		tab_col[31] = dam_label::ci_water_sup_pt;
		tab_col[32] = dam_label::ci_water_treat_pt;
		tab_col[33] = dam_label::ci_energy_pt;
		tab_col[34] = dam_label::ci_health_pt;
		tab_col[35] = dam_label::ci_social_pt;
		tab_col[36] = dam_label::ci_mat_pt;
		tab_col[37] = dam_label::ci_elect_p;
		tab_col[38] = dam_label::ci_info_tec_p;
		tab_col[39] = dam_label::ci_water_sup_p;
		tab_col[40] = dam_label::ci_water_treat_p;
		tab_col[41] = dam_label::ci_energy_p;
		tab_col[42] = dam_label::ci_health_p;
		tab_col[43] = dam_label::ci_social_p;
		tab_col[44] = dam_label::ci_mat_p;



		
		//set the table
		try{
			Risk_System::table_detailed_results= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_System::table_detailed_results->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_System::set_resulttable_detailed(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			if (not_close == false) {
				Risk_System::close_resulttable_detailed();
			}
			throw msg;
		}
	}
}
//Create the database table for detailed results of the risk calculation (static)
void Risk_System::create_resulttable_detailed(QSqlDatabase *ptr_database){
	if(Risk_System::table_detailed_results==NULL){
			ostringstream cout;
			cout << "Create database table of detailed risk results..." << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			//make specific input for this class
			const string tab_name=risk_label::tab_risk_detailed;
			const int num_col=46;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=label::areastate_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=label::measure_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].key_flag=true;

			tab_col[3].name=hyd_label::sz_bound_id;
			tab_col[3].type=sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag=true;
			tab_col[3].key_flag=true;

			tab_col[4].name=hyd_label::sz_bound_name;
			tab_col[4].type=sys_label::tab_col_type_string;

			tab_col[5].name=risk_label::sz_break_id;
			tab_col[5].type=sys_label::tab_col_type_string;
			tab_col[5].default_value="";
			tab_col[5].key_flag=true;

			tab_col[6].name=risk_label::risk_type;
			tab_col[6].type=sys_label::tab_col_type_string;
			tab_col[6].key_flag=true;

			tab_col[7].name=risk_label::max_outflow;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].default_value="0.0";
			tab_col[7].unsigned_flag=true;

			tab_col[8].name=dam_label::immob_dam;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=dam_label::mob_dam;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";
			tab_col[9].unsigned_flag=true;

			tab_col[10].name=dam_label::immob_5_quant;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].default_value="0.0";
			tab_col[10].unsigned_flag=true;

			tab_col[11].name=dam_label::immob_95_quant;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].default_value="0.0";
			tab_col[11].unsigned_flag=true;

			tab_col[12].name=dam_label::mob_5_quant;
			tab_col[12].type=sys_label::tab_col_type_double;
			tab_col[12].default_value="0.0";
			tab_col[12].unsigned_flag=true;

			tab_col[13].name=dam_label::mob_95_quant;
			tab_col[13].type=sys_label::tab_col_type_double;
			tab_col[13].default_value="0.0";
			tab_col[13].unsigned_flag=true;

			tab_col[14].name=dam_label::btype_cost;
			tab_col[14].type=sys_label::tab_col_type_double;
			tab_col[14].default_value="0.0";
			tab_col[14].unsigned_flag=true;

			tab_col[15].name=dam_label::soil_cost;
			tab_col[15].type=sys_label::tab_col_type_double;
			tab_col[15].default_value="0.0";
			tab_col[15].unsigned_flag=true;

			tab_col[16].name=dam_label::pop_affected;
			tab_col[16].type=sys_label::tab_col_type_double;
			tab_col[16].default_value="0.0";
			tab_col[16].unsigned_flag=true;

			tab_col[17].name=dam_label::pop_endangered;
			tab_col[17].type=sys_label::tab_col_type_double;
			tab_col[17].default_value="0.0";
			tab_col[17].unsigned_flag=true;

			tab_col[18].name=dam_label::crit_age_50_59;
			tab_col[18].type=sys_label::tab_col_type_double;
			tab_col[18].default_value="0.0";
			tab_col[18].unsigned_flag=true;

			tab_col[19].name=dam_label::crit_age_80;
			tab_col[19].type=sys_label::tab_col_type_double;
			tab_col[19].default_value="0.0";
			tab_col[19].unsigned_flag=true;

			tab_col[20].name=dam_label::crit_female;
			tab_col[20].type=sys_label::tab_col_type_double;
			tab_col[20].default_value="0.0";
			tab_col[20].unsigned_flag=true;

			tab_col[21].name=dam_label::crit_owner;
			tab_col[21].type=sys_label::tab_col_type_double;
			tab_col[21].default_value="0.0";
			tab_col[21].unsigned_flag=true;

			tab_col[22].name=dam_label::denscrit_age50_59;
			tab_col[22].type=sys_label::tab_col_type_double;
			tab_col[22].default_value="0.0";
			tab_col[22].unsigned_flag=true;

			tab_col[23].name=dam_label::denscrit_age_80;
			tab_col[23].type=sys_label::tab_col_type_double;
			tab_col[23].default_value="0.0";
			tab_col[23].unsigned_flag=true;

			tab_col[24].name=dam_label::denscrit_female;
			tab_col[24].type=sys_label::tab_col_type_double;
			tab_col[24].default_value="0.0";
			tab_col[24].unsigned_flag=true;

			tab_col[25].name=dam_label::denscrit_owner;
			tab_col[25].type=sys_label::tab_col_type_double;
			tab_col[25].default_value="0.0";
			tab_col[25].unsigned_flag=true;

			tab_col[26].name=dam_label::sc_pub_build;
			tab_col[26].type=sys_label::tab_col_type_double;
			tab_col[26].default_value="0.0";
			tab_col[26].unsigned_flag=true;

			tab_col[27].name=dam_label::sc_eco_build;
			tab_col[27].type=sys_label::tab_col_type_double;
			tab_col[27].default_value="0.0";
			tab_col[27].unsigned_flag=true;

			tab_col[28].name=dam_label::sc_cult_build;
			tab_col[28].type=sys_label::tab_col_type_double;
			tab_col[28].default_value="0.0";
			tab_col[28].unsigned_flag=true;

			tab_col[29].name=dam_label::sc_person_build;
			tab_col[29].type=sys_label::tab_col_type_double;
			tab_col[29].default_value="0.0";
			tab_col[29].unsigned_flag=true;


			tab_col[30].name = dam_label::ci_elect_pt;
			tab_col[30].type = sys_label::tab_col_type_double;
			tab_col[30].default_value = "0.0";
			tab_col[30].unsigned_flag = true;

			tab_col[31].name = dam_label::ci_info_tec_pt;
			tab_col[31].type = sys_label::tab_col_type_double;
			tab_col[31].default_value = "0.0";
			tab_col[31].unsigned_flag = true;

			tab_col[32].name = dam_label::ci_water_sup_pt;
			tab_col[32].type = sys_label::tab_col_type_double;
			tab_col[32].default_value = "0.0";
			tab_col[32].unsigned_flag = true;

			tab_col[33].name = dam_label::ci_water_treat_pt;
			tab_col[33].type = sys_label::tab_col_type_double;
			tab_col[33].default_value = "0.0";
			tab_col[33].unsigned_flag = true;

			tab_col[34].name = dam_label::ci_energy_pt;
			tab_col[34].type = sys_label::tab_col_type_double;
			tab_col[34].default_value = "0.0";
			tab_col[34].unsigned_flag = true;



			tab_col[35].name = dam_label::ci_health_pt;
			tab_col[35].type = sys_label::tab_col_type_double;
			tab_col[35].default_value = "0.0";
			tab_col[35].unsigned_flag = true;

			tab_col[36].name = dam_label::ci_social_pt;
			tab_col[36].type = sys_label::tab_col_type_double;
			tab_col[36].default_value = "0.0";
			tab_col[36].unsigned_flag = true;

			tab_col[37].name = dam_label::ci_mat_pt;
			tab_col[37].type = sys_label::tab_col_type_double;
			tab_col[37].default_value = "0.0";
			tab_col[37].unsigned_flag = true;



			tab_col[38].name = dam_label::ci_elect_p;
			tab_col[38].type = sys_label::tab_col_type_double;
			tab_col[38].default_value = "0.0";
			tab_col[38].unsigned_flag = true;

			tab_col[39].name = dam_label::ci_info_tec_p;
			tab_col[39].type = sys_label::tab_col_type_double;
			tab_col[39].default_value = "0.0";
			tab_col[39].unsigned_flag = true;

			tab_col[40].name = dam_label::ci_water_sup_p;
			tab_col[40].type = sys_label::tab_col_type_double;
			tab_col[40].default_value = "0.0";
			tab_col[40].unsigned_flag = true;

			tab_col[41].name = dam_label::ci_water_treat_p;
			tab_col[41].type = sys_label::tab_col_type_double;
			tab_col[41].default_value = "0.0";
			tab_col[41].unsigned_flag = true;

			tab_col[42].name = dam_label::ci_energy_p;
			tab_col[42].type = sys_label::tab_col_type_double;
			tab_col[42].default_value = "0.0";
			tab_col[42].unsigned_flag = true;



			tab_col[43].name = dam_label::ci_health_p;
			tab_col[43].type = sys_label::tab_col_type_double;
			tab_col[43].default_value = "0.0";
			tab_col[43].unsigned_flag = true;

			tab_col[44].name = dam_label::ci_social_p;
			tab_col[44].type = sys_label::tab_col_type_double;
			tab_col[44].default_value = "0.0";
			tab_col[44].unsigned_flag = true;

			tab_col[45].name = dam_label::ci_mat_p;
			tab_col[45].type = sys_label::tab_col_type_double;
			tab_col[45].default_value = "0.0";
			tab_col[45].unsigned_flag = true;




			try{
				Risk_System::table_detailed_results= new Tables();
				if(Risk_System::table_detailed_results->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_risk->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Risk_System::create_resulttable_detailed(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Risk_System::close_resulttable_detailed();
				throw msg;
			}
			Risk_System::close_resulttable_detailed();
		
	}
}
//Delete all data in the database table for detailed results of the risk calculation (static)
void Risk_System::delete_data_in_resulttable_detailed(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_resulttable_detailed(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Risk_System::table_detailed_results->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for detailed results of the risk calculation for specified parameters (static)
void Risk_System::delete_data_in_resulttable_detailed(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_resulttable_detailed(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_detailed_results->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_detailed_results->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_detailed_results->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_detailed_results->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_detailed(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_detailed_results->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results of the risk calculation for specified parameters (static)
void Risk_System::delete_data_in_resulttable_detailed(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_resulttable_detailed(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_detailed_results->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_detailed_results->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_detailed_results->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_detailed_results->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_detailed_results->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_detailed_results->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_detailed(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_detailed_results->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results of the risk calculation for specified parameters (static)
void Risk_System::delete_data_in_resulttable_detailed(QSqlDatabase *ptr_database, _sys_system_id id){
	Risk_System::delete_data_in_resulttable_detailed(ptr_database, id, _risk_type::catchment_risk);
	Risk_System::delete_data_in_resulttable_detailed(ptr_database, id, _risk_type::nobreak_risk);
	Risk_System::delete_data_in_resulttable_detailed(ptr_database, id, _risk_type::scenario_risk);
}
//Close and delete the database table for detailed results of the risk calculation (static)
void Risk_System::close_resulttable_detailed(void){
	if(Risk_System::table_detailed_results!=NULL){
		delete Risk_System::table_detailed_results;
		Risk_System::table_detailed_results=NULL;
	}
}
//Select the data in the database table for detailed results of the risk calculation for specified parameters (static)
int Risk_System::select_data_in_resulttable_detailed(QSqlQueryModel *model, QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){
	try{
		Risk_System::set_resulttable_detailed(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Risk_System::table_detailed_results->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Risk_System::table_detailed_results->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND ";
	test_filter << Risk_System::table_detailed_results->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	test_filter << " AND " ;
	test_filter << Risk_System::table_detailed_results->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type)<< "' ";
	test_filter << " ORDER BY " <<  Risk_System::table_detailed_results->get_column_name(label::glob_id);

	Data_Base::database_request(model, test_filter.str(), ptr_database);

	//check the request
	if(model->lastError().isValid()){
		Error msg;
		msg.set_msg("Risk_System::select_data_in_resulttable_detailed(QSqlQueryModel *model, QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_detailed_results->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return model->rowCount();
}
//Select the data in the database table for detailed results of the risk calculation for specified parameters (static)
int Risk_System::select_data_in_resulttable_detailed(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int area_state, _risk_type type){
	try{
		Risk_System::set_resulttable_detailed(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Risk_System::table_detailed_results->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Risk_System::table_detailed_results->get_column_name(label::areastate_id) << " = "  << area_state;
	test_filter << " AND " ;
	test_filter << Risk_System::table_detailed_results->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type)<< "' ";
	test_filter << " ORDER BY " <<  Risk_System::table_detailed_results->get_column_name(label::glob_id);

	Data_Base::database_request(model, test_filter.str(), ptr_database);

	//check the request
	if(model->lastError().isValid()){
		Error msg;
		msg.set_msg("Risk_System::select_data_in_resulttable_detailed(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int area_state, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_detailed_results->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return model->rowCount();
}
//Select the data in the database table for detailed results of the risk calculation for specified parameters (static)
int Risk_System::select_data_in_resulttable_detailed(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int glob_id){
	
	try{
		Risk_System::set_resulttable_detailed(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Risk_System::table_detailed_results->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Risk_System::table_detailed_results->get_column_name(label::glob_id) << " = "  << glob_id;
	
	Data_Base::database_request(model, test_filter.str(), ptr_database);
		
	//check the request
	if(model->lastError().isValid()){
		Error msg;
		msg.set_msg("Risk_System::select_data_in_resulttable_detailed(QSqlQueryModel *model, const int glob_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_detailed_results->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return model->rowCount();
}
//Check if there are detailed risk results available in database table for the given parameters (static)
bool Risk_System::check_detailed_risk_results_available(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel model;
	try{
		Risk_System::set_resulttable_detailed(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT "<<Risk_System::table_detailed_results->get_column_name(label::glob_id);
	test_filter<< " FROM ";
	test_filter << Risk_System::table_detailed_results->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Risk_System::table_detailed_results->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND ";
	test_filter << Risk_System::table_detailed_results->get_column_name(label::measure_id) << " = " << id.measure_nr ;


	Data_Base::database_request(&model, test_filter.str(), ptr_database);

	//check the request
	if(model.lastError().isValid()){
		Error msg;
		msg.set_msg("Risk_System::select_data_in_resulttable_detailed(QSqlQueryModel *model, QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_detailed_results->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(model.rowCount()>0){
		return true;
	}
	else{
		return false;
	}
}
//Check if there are detailed risk results available in database table for the given parameters (static)
bool Risk_System::check_detailed_risk_results_available(QSqlDatabase *ptr_database, const _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	QSqlQueryModel model;
	try{
		Risk_System::set_resulttable_detailed(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Risk_System::table_detailed_results->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Risk_System::table_detailed_results->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND ";
	test_filter << Risk_System::table_detailed_results->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	test_filter << " AND ";
	test_filter << Risk_System::table_detailed_results->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	test_filter << " AND ";
	test_filter << Risk_System::table_detailed_results->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	test_filter << " AND ";
	test_filter << Risk_System::table_detailed_results->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";


	Data_Base::database_request(&model, test_filter.str(), ptr_database);

	//check the request
	if(model.lastError().isValid()){
		Error msg;
		msg.set_msg("Risk_System::select_data_in_resulttable_detailed(QSqlQueryModel *model, QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_detailed_results->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(model.rowCount()>0){
		return true;
	}
	else{
		return false;
	}
}
//Set the database table for cumulated results of the risk calculation: it sets the table name and the name of the columns and allocate them (static)
void Risk_System::set_resulttable_cumulated(QSqlDatabase *ptr_database, const bool not_close){
	if(Risk_System::table_cumulated_results==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_risk_cumulated;
		string tab_col[45];
		tab_col[0]=label::glob_id;
		tab_col[1]=label::areastate_id;
		tab_col[2]=label::measure_id;
		tab_col[3]=hyd_label::sz_bound_id;
		tab_col[4]=risk_label::risk_type;
		tab_col[5]=risk_label::max_outflow;
		tab_col[6]=dam_label::immob_dam;
		tab_col[7]=dam_label::mob_dam;
		tab_col[8]=dam_label::immob_5_quant;
		tab_col[9]=dam_label::immob_95_quant;
		tab_col[10]=dam_label::mob_5_quant;
		tab_col[11]=dam_label::mob_95_quant;
		tab_col[12]=dam_label::btype_cost;
		tab_col[13]=dam_label::soil_cost;
		tab_col[14]=dam_label::pop_affected;
		tab_col[15]=dam_label::pop_endangered;
		tab_col[16]=dam_label::crit_age_50_59;
		tab_col[17]=dam_label::crit_age_80;
		tab_col[18]=dam_label::crit_female;
		tab_col[19]=dam_label::crit_owner;
		tab_col[20]=dam_label::denscrit_age50_59;
		tab_col[21]=dam_label::denscrit_age_80;
		tab_col[22]=dam_label::denscrit_female;
		tab_col[23]=dam_label::denscrit_owner;
		tab_col[24]=risk_label::number_detailed;
		tab_col[25]=dam_label::sc_pub_build;
		tab_col[26]=dam_label::sc_eco_build;
		tab_col[27]=dam_label::sc_cult_build;
		tab_col[28]=dam_label::sc_person_build;

		tab_col[29] = dam_label::ci_elect_pt;
		tab_col[30] = dam_label::ci_info_tec_pt;
		tab_col[31] = dam_label::ci_water_sup_pt;
		tab_col[32] = dam_label::ci_water_treat_pt;
		tab_col[33] = dam_label::ci_energy_pt;
		tab_col[34] = dam_label::ci_health_pt;
		tab_col[35] = dam_label::ci_social_pt;
		tab_col[36] = dam_label::ci_mat_pt;
		tab_col[37] = dam_label::ci_elect_p;
		tab_col[38] = dam_label::ci_info_tec_p;
		tab_col[39] = dam_label::ci_water_sup_p;
		tab_col[40] = dam_label::ci_water_treat_p;
		tab_col[41] = dam_label::ci_energy_p;
		tab_col[42] = dam_label::ci_health_p;
		tab_col[43] = dam_label::ci_social_p;
		tab_col[44] = dam_label::ci_mat_p;

		
		//set the table
		try{
			Risk_System::table_cumulated_results= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_System::table_cumulated_results->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_System::set_resulttable_cumulated(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			if (not_close == false) {
				Risk_System::close_resulttable_cumulated();
			}
			throw msg;
		}
	}
}
//Create the database table for cumulated results of the risk calculation (static)
void Risk_System::create_resulttable_cumulated(QSqlDatabase *ptr_database){
		if(Risk_System::table_cumulated_results==NULL){
			ostringstream cout;
			cout << "Create database table of cumulated risk results..." << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			//make specific input for this class
			const string tab_name=risk_label::tab_risk_cumulated;
			const int num_col=45;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=label::areastate_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=label::measure_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].key_flag=true;

			tab_col[3].name=hyd_label::sz_bound_id;
			tab_col[3].type=sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag=true;
			tab_col[3].key_flag=true;

			tab_col[4].name=risk_label::risk_type;
			tab_col[4].type=sys_label::tab_col_type_string;
			tab_col[4].key_flag=true;

			tab_col[5].name=risk_label::number_detailed;
			tab_col[5].type=sys_label::tab_col_type_int;
			tab_col[5].unsigned_flag=true;

			tab_col[6].name=risk_label::max_outflow;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].default_value="0.0";
			tab_col[6].unsigned_flag=true;

			tab_col[7].name=dam_label::immob_dam;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].default_value="0.0";
			tab_col[7].unsigned_flag=true;

			tab_col[8].name=dam_label::mob_dam;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=dam_label::immob_5_quant;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";
			tab_col[9].unsigned_flag=true;

			tab_col[10].name=dam_label::immob_95_quant;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].default_value="0.0";
			tab_col[10].unsigned_flag=true;

			tab_col[11].name=dam_label::mob_5_quant;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].default_value="0.0";
			tab_col[11].unsigned_flag=true;

			tab_col[12].name=dam_label::mob_95_quant;
			tab_col[12].type=sys_label::tab_col_type_double;
			tab_col[12].default_value="0.0";
			tab_col[12].unsigned_flag=true;

			tab_col[13].name=dam_label::btype_cost;
			tab_col[13].type=sys_label::tab_col_type_double;
			tab_col[13].default_value="0.0";
			tab_col[13].unsigned_flag=true;

			tab_col[14].name=dam_label::soil_cost;
			tab_col[14].type=sys_label::tab_col_type_double;
			tab_col[14].default_value="0.0";
			tab_col[14].unsigned_flag=true;

			tab_col[15].name=dam_label::pop_affected;
			tab_col[15].type=sys_label::tab_col_type_double;
			tab_col[15].default_value="0.0";
			tab_col[15].unsigned_flag=true;

			tab_col[16].name=dam_label::pop_endangered;
			tab_col[16].type=sys_label::tab_col_type_double;
			tab_col[16].default_value="0.0";
			tab_col[16].unsigned_flag=true;

			tab_col[17].name=dam_label::crit_age_50_59;
			tab_col[17].type=sys_label::tab_col_type_double;
			tab_col[17].default_value="0.0";
			tab_col[17].unsigned_flag=true;

			tab_col[18].name=dam_label::crit_age_80;
			tab_col[18].type=sys_label::tab_col_type_double;
			tab_col[18].default_value="0.0";
			tab_col[18].unsigned_flag=true;

			tab_col[19].name=dam_label::crit_female;
			tab_col[19].type=sys_label::tab_col_type_double;
			tab_col[19].default_value="0.0";
			tab_col[19].unsigned_flag=true;

			tab_col[20].name=dam_label::crit_owner;
			tab_col[20].type=sys_label::tab_col_type_double;
			tab_col[20].default_value="0.0";
			tab_col[20].unsigned_flag=true;

			tab_col[21].name=dam_label::denscrit_age50_59;
			tab_col[21].type=sys_label::tab_col_type_double;
			tab_col[21].default_value="0.0";
			tab_col[21].unsigned_flag=true;

			tab_col[22].name=dam_label::denscrit_age_80;
			tab_col[22].type=sys_label::tab_col_type_double;
			tab_col[22].default_value="0.0";
			tab_col[22].unsigned_flag=true;

			tab_col[23].name=dam_label::denscrit_female;
			tab_col[23].type=sys_label::tab_col_type_double;
			tab_col[23].default_value="0.0";
			tab_col[23].unsigned_flag=true;

			tab_col[24].name=dam_label::denscrit_owner;
			tab_col[24].type=sys_label::tab_col_type_double;
			tab_col[24].default_value="0.0";
			tab_col[24].unsigned_flag=true;

			tab_col[25].name=dam_label::sc_pub_build;
			tab_col[25].type=sys_label::tab_col_type_double;
			tab_col[25].default_value="0.0";
			tab_col[25].unsigned_flag=true;

			tab_col[26].name=dam_label::sc_eco_build;
			tab_col[26].type=sys_label::tab_col_type_double;
			tab_col[26].default_value="0.0";
			tab_col[26].unsigned_flag=true;

			tab_col[27].name=dam_label::sc_cult_build;
			tab_col[27].type=sys_label::tab_col_type_double;
			tab_col[27].default_value="0.0";
			tab_col[27].unsigned_flag=true;

			tab_col[28].name=dam_label::sc_person_build;
			tab_col[28].type=sys_label::tab_col_type_double;
			tab_col[28].default_value="0.0";
			tab_col[28].unsigned_flag=true;

			tab_col[29].name = dam_label::ci_elect_pt;
			tab_col[29].type = sys_label::tab_col_type_double;
			tab_col[29].default_value = "0.0";
			tab_col[29].unsigned_flag = true;

			tab_col[30].name = dam_label::ci_info_tec_pt;
			tab_col[30].type = sys_label::tab_col_type_double;
			tab_col[30].default_value = "0.0";
			tab_col[30].unsigned_flag = true;

			tab_col[31].name = dam_label::ci_water_sup_pt;
			tab_col[31].type = sys_label::tab_col_type_double;
			tab_col[31].default_value = "0.0";
			tab_col[31].unsigned_flag = true;

			tab_col[32].name = dam_label::ci_water_treat_pt;
			tab_col[32].type = sys_label::tab_col_type_double;
			tab_col[32].default_value = "0.0";
			tab_col[32].unsigned_flag = true;

			tab_col[33].name = dam_label::ci_energy_pt;
			tab_col[33].type = sys_label::tab_col_type_double;
			tab_col[33].default_value = "0.0";
			tab_col[33].unsigned_flag = true;



			tab_col[34].name = dam_label::ci_health_pt;
			tab_col[34].type = sys_label::tab_col_type_double;
			tab_col[34].default_value = "0.0";
			tab_col[34].unsigned_flag = true;

			tab_col[35].name = dam_label::ci_social_pt;
			tab_col[35].type = sys_label::tab_col_type_double;
			tab_col[35].default_value = "0.0";
			tab_col[35].unsigned_flag = true;

			tab_col[36].name = dam_label::ci_mat_pt;
			tab_col[36].type = sys_label::tab_col_type_double;
			tab_col[36].default_value = "0.0";
			tab_col[36].unsigned_flag = true;



			tab_col[37].name = dam_label::ci_elect_p;
			tab_col[37].type = sys_label::tab_col_type_double;
			tab_col[37].default_value = "0.0";
			tab_col[37].unsigned_flag = true;

			tab_col[38].name = dam_label::ci_info_tec_p;
			tab_col[38].type = sys_label::tab_col_type_double;
			tab_col[38].default_value = "0.0";
			tab_col[38].unsigned_flag = true;

			tab_col[39].name = dam_label::ci_water_sup_p;
			tab_col[39].type = sys_label::tab_col_type_double;
			tab_col[39].default_value = "0.0";
			tab_col[39].unsigned_flag = true;

			tab_col[40].name = dam_label::ci_water_treat_p;
			tab_col[40].type = sys_label::tab_col_type_double;
			tab_col[40].default_value = "0.0";
			tab_col[40].unsigned_flag = true;

			tab_col[41].name = dam_label::ci_energy_p;
			tab_col[41].type = sys_label::tab_col_type_double;
			tab_col[41].default_value = "0.0";
			tab_col[41].unsigned_flag = true;



			tab_col[42].name = dam_label::ci_health_p;
			tab_col[42].type = sys_label::tab_col_type_double;
			tab_col[42].default_value = "0.0";
			tab_col[42].unsigned_flag = true;

			tab_col[43].name = dam_label::ci_social_p;
			tab_col[43].type = sys_label::tab_col_type_double;
			tab_col[43].default_value = "0.0";
			tab_col[43].unsigned_flag = true;

			tab_col[44].name = dam_label::ci_mat_p;
			tab_col[44].type = sys_label::tab_col_type_double;
			tab_col[44].default_value = "0.0";
			tab_col[44].unsigned_flag = true;

			try{
				Risk_System::table_cumulated_results= new Tables();
				if(Risk_System::table_cumulated_results->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_risk->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Risk_System::create_resulttable_cumulated(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Risk_System::close_resulttable_cumulated();
				throw msg;
			}
			Risk_System::close_resulttable_cumulated();
		
	}
}
//Delete all data in the database table for cumulated results of the risk calculation (static)
void Risk_System::delete_data_in_resulttable_cumulated(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_resulttable_cumulated(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Risk_System::table_cumulated_results->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for cumulated results of the risk calculation for specified parameters (static)
void Risk_System::delete_data_in_resulttable_cumulated(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_resulttable_cumulated(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_cumulated_results->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_cumulated_results->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_cumulated_results->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_cumulated_results->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_cumulated(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_cumulated_results->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for cumulated results of the risk calculation for specified parameters (static)
void Risk_System::delete_data_in_resulttable_cumulated(QSqlDatabase *ptr_database, _sys_system_id id){
	Risk_System::delete_data_in_resulttable_cumulated(ptr_database, id, _risk_type::catchment_risk);
	Risk_System::delete_data_in_resulttable_cumulated(ptr_database, id, _risk_type::nobreak_risk);
	Risk_System::delete_data_in_resulttable_cumulated(ptr_database, id, _risk_type::scenario_risk);
}
//Close and delete the database table for cumulated results of the risk calculation (static)
void Risk_System::close_resulttable_cumulated(void){
	if(Risk_System::table_cumulated_results!=NULL){
		delete Risk_System::table_cumulated_results;
		Risk_System::table_cumulated_results=NULL;
	}
}
//Select the data in the database table for cumulated results of the risk calculation for specified parameters
int Risk_System::select_data_in_resulttable_cumulated(QSqlQueryModel *model, QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){
	try{
		Risk_System::set_resulttable_cumulated(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Risk_System::table_cumulated_results->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Risk_System::table_cumulated_results->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND ";
	test_filter << Risk_System::table_cumulated_results->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	test_filter << " AND " ;
	test_filter << Risk_System::table_cumulated_results->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type)<< "' ";
	test_filter << " ORDER BY " <<  Risk_System::table_cumulated_results->get_column_name(label::glob_id);

	Data_Base::database_request(model, test_filter.str(), ptr_database);

	//check the request
	if(model->lastError().isValid()){
		Error msg;
		msg.set_msg("Risk_System::select_data_in_resulttable_cumulated(QSqlQueryModel *model, QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_cumulated_results->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	return model->rowCount();
}
//Select the data in the database table for cumulated results of the risk calculation for specified parameters
int Risk_System::select_data_in_resulttable_cumulated(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int glob_id){
	try{
		Risk_System::set_resulttable_cumulated(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Risk_System::table_cumulated_results->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Risk_System::table_cumulated_results->get_column_name(label::glob_id) << " = "  << glob_id;
	
	Data_Base::database_request(model, test_filter.str(), ptr_database);
		
	//check the request
	if(model->lastError().isValid()){
		Error msg;
		msg.set_msg("Risk_System::select_data_in_resulttable_cumulated(QSqlQueryModel *model, const int glob_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_cumulated_results->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return model->rowCount();
}
//Set the database table for results of the risk calculation: it sets the table name and the name of the columns and allocate them (static)
void Risk_System::set_resulttable(QSqlDatabase *ptr_database, const bool not_close){
	if(Risk_System::table_results==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_risk_result;
		string tab_col[45];
		tab_col[0]=label::glob_id;
		tab_col[1]=label::areastate_id;
		tab_col[2]=label::measure_id;
		tab_col[3]=risk_label::risk_type;

		tab_col[4]=risk_label::max_outflow;
		tab_col[5]=dam_label::immob_dam;
		tab_col[6]=dam_label::mob_dam;
		tab_col[7]=dam_label::immob_5_quant;
		tab_col[8]=dam_label::immob_95_quant;
		tab_col[9]=dam_label::mob_5_quant;
		tab_col[10]=dam_label::mob_95_quant;
		tab_col[11]=dam_label::btype_cost;
		tab_col[12]=dam_label::soil_cost;
		tab_col[13]=dam_label::pop_affected;
		tab_col[14]=dam_label::pop_endangered;
		tab_col[15]=dam_label::crit_age_50_59;
		tab_col[16]=dam_label::crit_age_80;
		tab_col[17]=dam_label::crit_female;
		tab_col[18]=dam_label::crit_owner;
		tab_col[19]=dam_label::denscrit_age50_59;
		tab_col[20]=dam_label::denscrit_age_80;
		tab_col[21]=dam_label::denscrit_female;
		tab_col[22]=dam_label::denscrit_owner;
		tab_col[23]=risk_label::number_cumulated;
		tab_col[24]=risk_label::number_detailed;
		tab_col[25]=dam_label::sc_pub_build;
		tab_col[26]=dam_label::sc_eco_build;
		tab_col[27]=dam_label::sc_cult_build;
		tab_col[28]=dam_label::sc_person_build;

		tab_col[29] = dam_label::ci_elect_pt;
		tab_col[30] = dam_label::ci_info_tec_pt;
		tab_col[31] = dam_label::ci_water_sup_pt;
		tab_col[32] = dam_label::ci_water_treat_pt;
		tab_col[33] = dam_label::ci_energy_pt;
		tab_col[34] = dam_label::ci_health_pt;
		tab_col[35] = dam_label::ci_social_pt;
		tab_col[36] = dam_label::ci_mat_pt;
		tab_col[37] = dam_label::ci_elect_p;
		tab_col[38] = dam_label::ci_info_tec_p;
		tab_col[39] = dam_label::ci_water_sup_p;
		tab_col[40] = dam_label::ci_water_treat_p;
		tab_col[41] = dam_label::ci_energy_p;
		tab_col[42] = dam_label::ci_health_p;
		tab_col[43] = dam_label::ci_social_p;
		tab_col[44] = dam_label::ci_mat_p;
		


		
		//set the table
		try{
			Risk_System::table_results= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_System::table_results->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_System::set_resulttable(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			if (not_close == false) {
				Risk_System::close_resulttable();
			}
			throw msg;
		}
	}
}
//Create the database table for results of the risk calculation (static)
void Risk_System::create_resulttable(QSqlDatabase *ptr_database){
	if(Risk_System::table_results==NULL){
			ostringstream cout;
			cout << "Create database table of risk results..." << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			//make specific input for this class
			const string tab_name=risk_label::tab_risk_result;
			const int num_col=45;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=label::areastate_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=label::measure_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].key_flag=true;

			tab_col[3].name=risk_label::risk_type;
			tab_col[3].type=sys_label::tab_col_type_string;
			tab_col[3].key_flag=true;

			tab_col[4].name=risk_label::number_cumulated;
			tab_col[4].type=sys_label::tab_col_type_int;
			tab_col[4].unsigned_flag=true;

			tab_col[5].name=risk_label::number_detailed;
			tab_col[5].type=sys_label::tab_col_type_int;
			tab_col[5].unsigned_flag=true;

			tab_col[6].name=risk_label::max_outflow;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].default_value="0.0";
			tab_col[6].unsigned_flag=true;

			tab_col[7].name=dam_label::immob_dam;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].default_value="0.0";
			tab_col[7].unsigned_flag=true;

			tab_col[8].name=dam_label::mob_dam;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=dam_label::immob_5_quant;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";
			tab_col[9].unsigned_flag=true;

			tab_col[10].name=dam_label::immob_95_quant;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].default_value="0.0";
			tab_col[10].unsigned_flag=true;

			tab_col[11].name=dam_label::mob_5_quant;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].default_value="0.0";
			tab_col[11].unsigned_flag=true;

			tab_col[12].name=dam_label::mob_95_quant;
			tab_col[12].type=sys_label::tab_col_type_double;
			tab_col[12].default_value="0.0";
			tab_col[12].unsigned_flag=true;

			tab_col[13].name=dam_label::btype_cost;
			tab_col[13].type=sys_label::tab_col_type_double;
			tab_col[13].default_value="0.0";
			tab_col[13].unsigned_flag=true;

			tab_col[14].name=dam_label::soil_cost;
			tab_col[14].type=sys_label::tab_col_type_double;
			tab_col[14].default_value="0.0";
			tab_col[14].unsigned_flag=true;

			tab_col[15].name=dam_label::pop_affected;
			tab_col[15].type=sys_label::tab_col_type_double;
			tab_col[15].default_value="0.0";
			tab_col[15].unsigned_flag=true;

			tab_col[16].name=dam_label::pop_endangered;
			tab_col[16].type=sys_label::tab_col_type_double;
			tab_col[16].default_value="0.0";
			tab_col[16].unsigned_flag=true;

			tab_col[17].name=dam_label::crit_age_50_59;
			tab_col[17].type=sys_label::tab_col_type_double;
			tab_col[17].default_value="0.0";
			tab_col[17].unsigned_flag=true;

			tab_col[18].name=dam_label::crit_age_80;
			tab_col[18].type=sys_label::tab_col_type_double;
			tab_col[18].default_value="0.0";
			tab_col[18].unsigned_flag=true;

			tab_col[19].name=dam_label::crit_female;
			tab_col[19].type=sys_label::tab_col_type_double;
			tab_col[19].default_value="0.0";
			tab_col[19].unsigned_flag=true;

			tab_col[20].name=dam_label::crit_owner;
			tab_col[20].type=sys_label::tab_col_type_double;
			tab_col[20].default_value="0.0";
			tab_col[20].unsigned_flag=true;

			tab_col[21].name=dam_label::denscrit_age50_59;
			tab_col[21].type=sys_label::tab_col_type_double;
			tab_col[21].default_value="0.0";
			tab_col[21].unsigned_flag=true;

			tab_col[22].name=dam_label::denscrit_age_80;
			tab_col[22].type=sys_label::tab_col_type_double;
			tab_col[22].default_value="0.0";
			tab_col[22].unsigned_flag=true;

			tab_col[23].name=dam_label::denscrit_female;
			tab_col[23].type=sys_label::tab_col_type_double;
			tab_col[23].default_value="0.0";
			tab_col[23].unsigned_flag=true;

			tab_col[24].name=dam_label::denscrit_owner;
			tab_col[24].type=sys_label::tab_col_type_double;
			tab_col[24].default_value="0.0";
			tab_col[24].unsigned_flag=true;

			tab_col[25].name=dam_label::sc_pub_build;
			tab_col[25].type=sys_label::tab_col_type_double;
			tab_col[25].default_value="0.0";
			tab_col[25].unsigned_flag=true;

			tab_col[26].name=dam_label::sc_eco_build;
			tab_col[26].type=sys_label::tab_col_type_double;
			tab_col[26].default_value="0.0";
			tab_col[26].unsigned_flag=true;

			tab_col[27].name=dam_label::sc_cult_build;
			tab_col[27].type=sys_label::tab_col_type_double;
			tab_col[27].default_value="0.0";
			tab_col[27].unsigned_flag=true;

			tab_col[28].name=dam_label::sc_person_build;
			tab_col[28].type=sys_label::tab_col_type_double;
			tab_col[28].default_value="0.0";
			tab_col[28].unsigned_flag=true;

			tab_col[29].name = dam_label::ci_elect_pt;
			tab_col[29].type = sys_label::tab_col_type_double;
			tab_col[29].default_value = "0.0";
			tab_col[29].unsigned_flag = true;

			tab_col[30].name = dam_label::ci_info_tec_pt;
			tab_col[30].type = sys_label::tab_col_type_double;
			tab_col[30].default_value = "0.0";
			tab_col[30].unsigned_flag = true;

			tab_col[31].name = dam_label::ci_water_sup_pt;
			tab_col[31].type = sys_label::tab_col_type_double;
			tab_col[31].default_value = "0.0";
			tab_col[31].unsigned_flag = true;

			tab_col[32].name = dam_label::ci_water_treat_pt;
			tab_col[32].type = sys_label::tab_col_type_double;
			tab_col[32].default_value = "0.0";
			tab_col[32].unsigned_flag = true;

			tab_col[33].name = dam_label::ci_energy_pt;
			tab_col[33].type = sys_label::tab_col_type_double;
			tab_col[33].default_value = "0.0";
			tab_col[33].unsigned_flag = true;



			tab_col[34].name = dam_label::ci_health_pt;
			tab_col[34].type = sys_label::tab_col_type_double;
			tab_col[34].default_value = "0.0";
			tab_col[34].unsigned_flag = true;

			tab_col[35].name = dam_label::ci_social_pt;
			tab_col[35].type = sys_label::tab_col_type_double;
			tab_col[35].default_value = "0.0";
			tab_col[35].unsigned_flag = true;

			tab_col[36].name = dam_label::ci_mat_pt;
			tab_col[36].type = sys_label::tab_col_type_double;
			tab_col[36].default_value = "0.0";
			tab_col[36].unsigned_flag = true;



			tab_col[37].name = dam_label::ci_elect_p;
			tab_col[37].type = sys_label::tab_col_type_double;
			tab_col[37].default_value = "0.0";
			tab_col[37].unsigned_flag = true;

			tab_col[38].name = dam_label::ci_info_tec_p;
			tab_col[38].type = sys_label::tab_col_type_double;
			tab_col[38].default_value = "0.0";
			tab_col[38].unsigned_flag = true;

			tab_col[39].name = dam_label::ci_water_sup_p;
			tab_col[39].type = sys_label::tab_col_type_double;
			tab_col[39].default_value = "0.0";
			tab_col[39].unsigned_flag = true;

			tab_col[40].name = dam_label::ci_water_treat_p;
			tab_col[40].type = sys_label::tab_col_type_double;
			tab_col[40].default_value = "0.0";
			tab_col[40].unsigned_flag = true;

			tab_col[41].name = dam_label::ci_energy_p;
			tab_col[41].type = sys_label::tab_col_type_double;
			tab_col[41].default_value = "0.0";
			tab_col[41].unsigned_flag = true;



			tab_col[42].name = dam_label::ci_health_p;
			tab_col[42].type = sys_label::tab_col_type_double;
			tab_col[42].default_value = "0.0";
			tab_col[42].unsigned_flag = true;

			tab_col[43].name = dam_label::ci_social_p;
			tab_col[43].type = sys_label::tab_col_type_double;
			tab_col[43].default_value = "0.0";
			tab_col[43].unsigned_flag = true;

			tab_col[44].name = dam_label::ci_mat_p;
			tab_col[44].type = sys_label::tab_col_type_double;
			tab_col[44].default_value = "0.0";
			tab_col[44].unsigned_flag = true;



			try{
				Risk_System::table_results= new Tables();
				if(Risk_System::table_results->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_risk->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Risk_System::create_resulttable(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Risk_System::close_resulttable();
				throw msg;
			}
			Risk_System::close_resulttable();
		
	}
}
//Delete all data in the database table for results of the risk calculation (static)
void Risk_System::delete_data_in_resulttable(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_resulttable(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Risk_System::table_results->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for results of the risk calculation for specified parameters (static)
void Risk_System::delete_data_in_resulttable(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_resulttable(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete all data in the database table for results of the risk calculation for specified parameters (static)
void Risk_System::delete_data_in_resulttable(QSqlDatabase *ptr_database, _sys_system_id id){
	Risk_System::delete_data_in_resulttable(ptr_database,id, _risk_type::catchment_risk);
	Risk_System::delete_data_in_resulttable(ptr_database,id, _risk_type::nobreak_risk);
	Risk_System::delete_data_in_resulttable(ptr_database,id, _risk_type::scenario_risk);
}
//Close and delete the database table for results of the risk calculation (static)
void Risk_System::close_resulttable(void){
	if(Risk_System::table_results!=NULL){
		delete Risk_System::table_results;
		Risk_System::table_results=NULL;
	}
}
//Select the data in the database table for results of the risk calculation for specified parameters (static)
int Risk_System::select_data_in_resulttable(QSqlQueryModel *model, QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){
	try{
		Risk_System::set_resulttable(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Risk_System::table_results->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Risk_System::table_results->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND ";
	test_filter << Risk_System::table_results->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	test_filter << " AND " ;
	test_filter << Risk_System::table_results->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type)<< "' ";
	test_filter << " ORDER BY " <<  Risk_System::table_results->get_column_name(label::glob_id);

	Data_Base::database_request(model, test_filter.str(), ptr_database);

	//check the request
	if(model->lastError().isValid()){
		Error msg;
		msg.set_msg("Risk_System::select_data_in_resulttable(QSqlQueryModel *model, QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return model->rowCount();
}
//Check if data in the database table for results of the risk calculation for specified parameters is set (static)
bool Risk_System::check_data_in_resulttable(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){

	QSqlQueryModel model;
	try{
		Risk_System::set_resulttable(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT "<<Risk_System::table_results->get_column_name(label::glob_id);
	test_filter<<" FROM ";
	test_filter << Risk_System::table_results->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Risk_System::table_results->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND ";
	test_filter << Risk_System::table_results->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	test_filter << " AND " ;
	test_filter << Risk_System::table_results->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type)<< "' ";
	
	Data_Base::database_request(&model, test_filter.str(), ptr_database);

	//check the request
	if(model.lastError().isValid()){
		Error msg;
		msg.set_msg("Risk_System::check_data_in_resulttable(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(model.rowCount()>0){
		return true;
	}
	else{
		return false;
	}
}
//Select the data in the database table for results of the risk calculation for specified parameters (static)
int Risk_System::select_data_in_resulttable(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int glob_id){
	try{
		Risk_System::set_resulttable(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Risk_System::table_results->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Risk_System::table_results->get_column_name(label::glob_id) << " = "  << glob_id;
	
	Data_Base::database_request(model, test_filter.str(), ptr_database);
		
	//check the request
	if(model->lastError().isValid()){
		Error msg;
		msg.set_msg("Risk_System::select_data_in_resulttable(QSqlQueryModel *model, const int glob_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return model->rowCount();
}
//Set the table for detailed results per damage raster-element of the economical damage raster: it sets the table name and the name of the columns and allocate them (static)
void Risk_System::set_table_result_elem_ecn(QSqlDatabase *ptr_database){
	if(Risk_System::table_results_elem_ecn==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_risk_ecn_elem;
		string tab_col[16];
		tab_col[0]=label::glob_id;
		tab_col[1]=dam_label::elem_id;
		tab_col[2]=dam_label::raster_id;
		tab_col[3]=label::areastate_id;
		tab_col[4]=label::measure_id;
		tab_col[5]=hyd_label::sz_bound_id;
		tab_col[6]=risk_label::sz_break_id;
		tab_col[7]=risk_label::risk_type;
		tab_col[8]=dam_label::immob_dam;
		tab_col[9]=risk_label::risk_immob_with_hyd;
		tab_col[10]=dam_label::mob_dam;
		tab_col[11]=risk_label::risk_mob_with_hyd;
		tab_col[12]=dam_label::total_dam;
		tab_col[13]=risk_label::risk_ecn_total_with_hyd;
		tab_col[14]=risk_label::risk_ecn_total_fac;
		tab_col[15]=dam_label::elem_poly;


		//set the table
		try{
			Risk_System::table_results_elem_ecn= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_System::table_results_elem_ecn->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_System::set_table_result_elem_ecn(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Risk_System::close_table_result_elem_ecn();
			throw msg;
		}
	}
}
//Create the table for detailed results per damage raster-element of the economical damage raster (static)
void Risk_System::create_table_result_elem_ecn(QSqlDatabase *ptr_database){
	if(Risk_System::table_results_elem_ecn==NULL){
		ostringstream cout;
		cout << "Create database table of the detailed risk results for the economical elements..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		//make specific input for this class
		const string tab_name=risk_label::tab_risk_ecn_elem;
		const int num_col=16;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}

		tab_col[0].name=label::glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=dam_label::elem_id;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag=true;
		tab_col[1].key_flag=true;

		tab_col[2].name=dam_label::raster_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;
		tab_col[2].key_flag=true;

		tab_col[3].name=label::areastate_id;
		tab_col[3].type=sys_label::tab_col_type_int;
		tab_col[3].unsigned_flag=true;
		tab_col[3].key_flag=true;

		tab_col[4].name=label::measure_id;
		tab_col[4].type=sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag=true;
		tab_col[4].key_flag=true;

		tab_col[5].name=hyd_label::sz_bound_id;
		tab_col[5].type=sys_label::tab_col_type_int;
		tab_col[5].unsigned_flag=true;
		tab_col[5].key_flag=true;
		
		tab_col[6].name=risk_label::sz_break_id;
		tab_col[6].type=sys_label::tab_col_type_string;
		tab_col[6].key_flag=true;

		tab_col[7].name=risk_label::risk_type;
		tab_col[7].type=sys_label::tab_col_type_string;
		tab_col[7].key_flag=true;

		tab_col[8].name=dam_label::immob_dam;
		tab_col[8].type=sys_label::tab_col_type_double;
		tab_col[8].default_value="0.0";
		tab_col[8].unsigned_flag=true;

		tab_col[9].name=risk_label::risk_immob_with_hyd;
		tab_col[9].type=sys_label::tab_col_type_double;
		tab_col[9].default_value="0.0";
		tab_col[9].unsigned_flag=true;

		tab_col[10].name=dam_label::mob_dam;
		tab_col[10].type=sys_label::tab_col_type_double;
		tab_col[10].default_value="0.0";
		tab_col[10].unsigned_flag=true;

		tab_col[11].name=risk_label::risk_mob_with_hyd;
		tab_col[11].type=sys_label::tab_col_type_double;
		tab_col[11].default_value="0.0";
		tab_col[11].unsigned_flag=true;

		tab_col[12].name=dam_label::total_dam;
		tab_col[12].type=sys_label::tab_col_type_double;
		tab_col[12].default_value="0.0";
		tab_col[12].unsigned_flag=true;

		tab_col[13].name=risk_label::risk_ecn_total_with_hyd;
		tab_col[13].type=sys_label::tab_col_type_double;
		tab_col[13].default_value="0.0";
		tab_col[13].unsigned_flag=true;

		tab_col[14].name=risk_label::risk_ecn_total_fac;
		tab_col[14].type=sys_label::tab_col_type_double;
		tab_col[14].default_value="0.0";
		tab_col[14].unsigned_flag=true;

		tab_col[15].name=dam_label::elem_poly;
		tab_col[15].type=sys_label::tab_col_type_polygon;

		try{
			Risk_System::table_results_elem_ecn= new Tables();
			if(Risk_System::table_results_elem_ecn->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_risk->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Risk_System::create_table_result_elem_ecn(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Risk_System::close_table_result_elem_ecn();
			throw msg;
		}
		Risk_System::close_table_result_elem_ecn();
	}
}
//Delete all data in the table for detailed results per damage raster-element of the economical damage raster (static)
void Risk_System::delete_data_in_table_result_elem_ecn(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_ecn(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Risk_System::table_results_elem_ecn->delete_data_in_table(ptr_database);
}
//Close and delete the table for detailed results per damage raster-element of the economical damage raster (static)
void Risk_System::close_table_result_elem_ecn(void){
	if(Risk_System::table_results_elem_ecn!=NULL){
		delete Risk_System::table_results_elem_ecn;
		Risk_System::table_results_elem_ecn=NULL;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the economical damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_ecn(QSqlDatabase *ptr_database, _sys_system_id id){
		//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_ecn(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_ecn->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_ecn->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(label::measure_id) << " = " << id.measure_nr;
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_ecn(QSqlDatabase *ptr_database, _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_ecn->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the economical damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_ecn(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_ecn(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_ecn->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_ecn->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_ecn(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_ecn->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the economical damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_ecn(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_ecn(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_ecn->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_ecn->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_ecn(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_ecn->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check if there are data available in the database table for detailed results per damage raster-element of the economical damage raster for specified parameters 
bool Risk_System::check_data_in_resulttable_elem_ecn(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_ecn(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"SELECT " << Risk_System::table_results_elem_ecn->get_column_name(label::glob_id);
	query_string <<" FROM " << Risk_System::table_results_elem_ecn->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_ecn->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::check_data_in_resulttable_elem_ecn(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_ecn->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(query.size()>0){
		return true;
	}
	else{
		return false;
	}
}
//Set the table for detailed results per damage raster-element of the ecological biotope-type damage raster: it sets the table name and the name of the columns and allocate them (static)
void Risk_System::set_table_result_elem_eco_btype(QSqlDatabase *ptr_database){
	if(Risk_System::table_results_elem_eco_btype==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_risk_eco_btype_elem;
		string tab_col[12];
		tab_col[0]=label::glob_id;
		tab_col[1]=dam_label::elem_id;
		tab_col[2]=dam_label::raster_id;
		tab_col[3]=label::areastate_id;
		tab_col[4]=label::measure_id;
		tab_col[5]=hyd_label::sz_bound_id;
		tab_col[6]=risk_label::sz_break_id;
		tab_col[7]=risk_label::risk_type;
		tab_col[8]=dam_label::btype_cost;
		tab_col[9]=risk_label::risk_btype_with_hyd;
		tab_col[10]=risk_label::risk_btype_fac;
		tab_col[11]=dam_label::elem_poly;


		//set the table
		try{
			Risk_System::table_results_elem_eco_btype= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_System::table_results_elem_eco_btype->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_System::set_table_result_elem_eco_btype(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Risk_System::close_table_result_elem_eco_btype();
			throw msg;
		}
	}
}
//Create the table for detailed results per damage raster-element of the ecological biotope-type damage raster (static)
void Risk_System::create_table_result_elem_eco_btype(QSqlDatabase *ptr_database){
	if(Risk_System::table_results_elem_eco_btype==NULL){
		ostringstream cout;
		cout << "Create database table of the detailed risk results for the ecological biotope-type elements..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		//make specific input for this class
		const string tab_name=risk_label::tab_risk_eco_btype_elem;
		const int num_col=12;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}

		tab_col[0].name=label::glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=dam_label::elem_id;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag=true;
		tab_col[1].key_flag=true;

		tab_col[2].name=dam_label::raster_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;
		tab_col[2].key_flag=true;

		tab_col[3].name=label::areastate_id;
		tab_col[3].type=sys_label::tab_col_type_int;
		tab_col[3].unsigned_flag=true;
		tab_col[3].key_flag=true;

		tab_col[4].name=label::measure_id;
		tab_col[4].type=sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag=true;
		tab_col[4].key_flag=true;

		tab_col[5].name=hyd_label::sz_bound_id;
		tab_col[5].type=sys_label::tab_col_type_int;
		tab_col[5].unsigned_flag=true;
		tab_col[5].key_flag=true;
		
		tab_col[6].name=risk_label::sz_break_id;
		tab_col[6].type=sys_label::tab_col_type_string;
		tab_col[6].key_flag=true;

		tab_col[7].name=risk_label::risk_type;
		tab_col[7].type=sys_label::tab_col_type_string;
		tab_col[7].key_flag=true;

		tab_col[8].name=dam_label::btype_cost;
		tab_col[8].type=sys_label::tab_col_type_double;
		tab_col[8].default_value="0.0";
		tab_col[8].unsigned_flag=true;

		tab_col[9].name=risk_label::risk_btype_with_hyd;
		tab_col[9].type=sys_label::tab_col_type_double;
		tab_col[9].default_value="0.0";
		tab_col[9].unsigned_flag=true;

		tab_col[10].name=risk_label::risk_btype_fac;
		tab_col[10].type=sys_label::tab_col_type_double;
		tab_col[10].default_value="0.0";
		tab_col[10].unsigned_flag=true;

		tab_col[11].name=dam_label::elem_poly;
		tab_col[11].type=sys_label::tab_col_type_polygon;

		try{
			Risk_System::table_results_elem_eco_btype= new Tables();
			if(Risk_System::table_results_elem_eco_btype->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_risk->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Risk_System::create_table_result_elem_eco_btype(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Risk_System::close_table_result_elem_eco_btype();
			throw msg;
		}
		Risk_System::close_table_result_elem_eco_btype();
	}
}
//Delete all data in the table for detailed results per damage raster-element of the ecological biotope-type damage raster (static)
void Risk_System::delete_data_in_table_result_elem_eco_btype(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_eco_btype(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Risk_System::table_results_elem_eco_btype->delete_data_in_table(ptr_database);
}
//Close and delete the table for detailed results per damage raster-element of the ecological biotope-type damage raster (static)
void Risk_System::close_table_result_elem_eco_btype(void){
	if(Risk_System::table_results_elem_eco_btype!=NULL){
		delete Risk_System::table_results_elem_eco_btype;
		Risk_System::table_results_elem_eco_btype=NULL;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the ecological biotope-type damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_eco_btype(QSqlDatabase *ptr_database, _sys_system_id id){
		//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_eco_btype(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_eco_btype->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(label::measure_id) << " = " << id.measure_nr;
		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_eco_btype(QSqlDatabase *ptr_database, _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_eco_btype->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the ecological biotope-type damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_eco_btype(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_eco_btype(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_eco_btype->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_eco_btype(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_eco_btype->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the ecological biotope-type damage raster for specified parameters (static) 
void Risk_System::delete_data_in_resulttable_elem_eco_btype(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_eco_btype(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_eco_btype->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_eco_btype(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_eco_btype->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check if there are data available in the database table for detailed results per damage raster-element of the ecological biotope-type damage raster for specified parameters (static) 
bool Risk_System::check_data_in_resulttable_elem_eco_btype(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_eco_btype(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"SELECT " << Risk_System::table_results_elem_eco_btype->get_column_name(label::glob_id);
	query_string <<" FROM " << Risk_System::table_results_elem_eco_btype->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::check_data_in_resulttable_elem_eco_btype(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_eco_btype->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(query.size()>0){
		return true;
	}
	else{
		return false;
	}
}
//Set the table for detailed results per damage raster-element of the ecological soil-erosion damage raster: it sets the table name and the name of the columns and allocate them (static)
void Risk_System::set_table_result_elem_eco_soil(QSqlDatabase *ptr_database){
	if(Risk_System::table_results_elem_eco_soil==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_risk_eco_soil_elem;
		string tab_col[12];
		tab_col[0]=label::glob_id;
		tab_col[1]=dam_label::elem_id;
		tab_col[2]=dam_label::raster_id;
		tab_col[3]=label::areastate_id;
		tab_col[4]=label::measure_id;
		tab_col[5]=hyd_label::sz_bound_id;
		tab_col[6]=risk_label::sz_break_id;
		tab_col[7]=risk_label::risk_type;
		tab_col[8]=dam_label::soil_cost;
		tab_col[9]=risk_label::risk_soil_with_hyd;
		tab_col[10]=risk_label::risk_soil_fac;
		tab_col[11]=dam_label::elem_poly;

		//set the table
		try{
			Risk_System::table_results_elem_eco_soil= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_System::table_results_elem_eco_soil->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_System::set_table_result_elem_eco_soil(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Risk_System::close_table_result_elem_eco_soil();
			throw msg;
		}
	}
}
//Create the table for detailed results per damage raster-element of the ecological soil-erosion damage raster (static)
void Risk_System::create_table_result_elem_eco_soil(QSqlDatabase *ptr_database){
	if(Risk_System::table_results_elem_eco_soil==NULL){
		ostringstream cout;
		cout << "Create database table of the detailed risk results for the ecological soil-erosion elements..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		//make specific input for this class
		const string tab_name=risk_label::tab_risk_eco_soil_elem;
		const int num_col=12;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}

		tab_col[0].name=label::glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=dam_label::elem_id;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag=true;
		tab_col[1].key_flag=true;

		tab_col[2].name=dam_label::raster_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;
		tab_col[2].key_flag=true;

		tab_col[3].name=label::areastate_id;
		tab_col[3].type=sys_label::tab_col_type_int;
		tab_col[3].unsigned_flag=true;
		tab_col[3].key_flag=true;

		tab_col[4].name=label::measure_id;
		tab_col[4].type=sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag=true;
		tab_col[4].key_flag=true;

		tab_col[5].name=hyd_label::sz_bound_id;
		tab_col[5].type=sys_label::tab_col_type_int;
		tab_col[5].unsigned_flag=true;
		tab_col[5].key_flag=true;
		
		tab_col[6].name=risk_label::sz_break_id;
		tab_col[6].type=sys_label::tab_col_type_string;
		tab_col[6].key_flag=true;

		tab_col[7].name=risk_label::risk_type;
		tab_col[7].type=sys_label::tab_col_type_string;
		tab_col[7].key_flag=true;

		tab_col[8].name=dam_label::soil_cost;
		tab_col[8].type=sys_label::tab_col_type_double;
		tab_col[8].default_value="0.0";
		tab_col[8].unsigned_flag=true;

		tab_col[9].name=risk_label::risk_soil_with_hyd;
		tab_col[9].type=sys_label::tab_col_type_double;
		tab_col[9].default_value="0.0";
		tab_col[9].unsigned_flag=true;

		tab_col[10].name=risk_label::risk_soil_fac;
		tab_col[10].type=sys_label::tab_col_type_double;
		tab_col[10].default_value="0.0";
		tab_col[10].unsigned_flag=true;

		tab_col[11].name=dam_label::elem_poly;
		tab_col[11].type=sys_label::tab_col_type_polygon;

		try{
			Risk_System::table_results_elem_eco_soil= new Tables();
			if(Risk_System::table_results_elem_eco_soil->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_risk->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Risk_System::create_table_result_elem_eco_soil(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Risk_System::close_table_result_elem_eco_soil();
			throw msg;
		}
		Risk_System::close_table_result_elem_eco_soil();
	}
}
//Delete all data in the table for detailed results per damage raster-element of the ecological soil-erosion damage raster (static)
void Risk_System::delete_data_in_table_result_elem_eco_soil(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_eco_soil(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Risk_System::table_results_elem_eco_soil->delete_data_in_table(ptr_database);
}
//Close and delete the table for detailed results per damage raster-element of the ecological soil-erosion damage raster (static)
void Risk_System::close_table_result_elem_eco_soil(void){
	if(Risk_System::table_results_elem_eco_soil!=NULL){
		delete Risk_System::table_results_elem_eco_soil;
		Risk_System::table_results_elem_eco_soil=NULL;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the ecological soil-erosion damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_eco_soil(QSqlDatabase *ptr_database, _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_eco_soil(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_eco_soil->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(label::measure_id) << " = " << id.measure_nr;
		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_eco_soil(QSqlDatabase *ptr_database, _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_eco_soil->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the ecological soil-erosion damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_eco_soil(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_eco_soil(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_eco_soil->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_eco_soil(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_eco_soil->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the ecological soil-erosion damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_eco_soil(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_eco_soil(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_eco_soil->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_eco_soil(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_eco_soil->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check if there are data available in the database table for detailed results per damage raster-element of the ecological soil-erosion damage raster for specified parameters (static)
bool Risk_System::check_data_in_resulttable_elem_eco_soil(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_eco_soil(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"SELECT " << Risk_System::table_results_elem_eco_soil->get_column_name(label::glob_id);
	query_string <<" FROM " << Risk_System::table_results_elem_eco_soil->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::check_data_in_resulttable_elem_eco_soil(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_eco_soil->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(query.size()>0){
		return true;
	}
	else{
		return false;
	}
}
//Set the table for detailed results per damage raster-element of the people2risk damage raster: it sets the table name and the name of the columns and allocate them (static)
void Risk_System::set_table_result_elem_pop(QSqlDatabase *ptr_database){
	if(Risk_System::table_results_elem_pop==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_risk_pop_elem;
		string tab_col[15];
		tab_col[0]=label::glob_id;
		tab_col[1]=dam_label::elem_id;
		tab_col[2]=dam_label::raster_id;
		tab_col[3]=label::areastate_id;
		tab_col[4]=label::measure_id;
		tab_col[5]=hyd_label::sz_bound_id;
		tab_col[6]=risk_label::sz_break_id;
		tab_col[7]=risk_label::risk_type;
		tab_col[8]=dam_label::pop_affected;
		tab_col[9]=risk_label::risk_pop_aff_with_hyd;
		tab_col[10]=risk_label::risk_pop_aff_fac;
		tab_col[11]=dam_label::pop_endangered;
		tab_col[12]=risk_label::risk_pop_dan_with_hyd;
		tab_col[13]=risk_label::risk_pop_dan_fac;
		tab_col[14]=dam_label::elem_poly;


		//set the table
		try{
			Risk_System::table_results_elem_pop= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_System::table_results_elem_pop->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_System::set_table_result_elem_pop(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Risk_System::close_table_result_elem_pop();
			throw msg;
		}
	}
}
//Create the table for detailed results per damage raster-element of the people2risk damage raster (static)
void Risk_System::create_table_result_elem_pop(QSqlDatabase *ptr_database){
	if(Risk_System::table_results_elem_pop==NULL){
		ostringstream cout;
		cout << "Create database table of the detailed risk results for the people2risk elements..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		//make specific input for this class
		const string tab_name=risk_label::tab_risk_pop_elem;
		const int num_col=15;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}

		tab_col[0].name=label::glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=dam_label::elem_id;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag=true;
		tab_col[1].key_flag=true;

		tab_col[2].name=dam_label::raster_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;
		tab_col[2].key_flag=true;

		tab_col[3].name=label::areastate_id;
		tab_col[3].type=sys_label::tab_col_type_int;
		tab_col[3].unsigned_flag=true;
		tab_col[3].key_flag=true;

		tab_col[4].name=label::measure_id;
		tab_col[4].type=sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag=true;
		tab_col[4].key_flag=true;

		tab_col[5].name=hyd_label::sz_bound_id;
		tab_col[5].type=sys_label::tab_col_type_int;
		tab_col[5].unsigned_flag=true;
		tab_col[5].key_flag=true;
		
		tab_col[6].name=risk_label::sz_break_id;
		tab_col[6].type=sys_label::tab_col_type_string;
		tab_col[6].key_flag=true;

		tab_col[7].name=risk_label::risk_type;
		tab_col[7].type=sys_label::tab_col_type_string;
		tab_col[7].key_flag=true;

		tab_col[8].name=dam_label::pop_affected;
		tab_col[8].type=sys_label::tab_col_type_double;
		tab_col[8].default_value="0.0";
		tab_col[8].unsigned_flag=true;

		tab_col[9].name=risk_label::risk_pop_aff_with_hyd;
		tab_col[9].type=sys_label::tab_col_type_double;
		tab_col[9].default_value="0.0";
		tab_col[9].unsigned_flag=true;

		tab_col[10].name=risk_label::risk_pop_aff_fac;
		tab_col[10].type=sys_label::tab_col_type_double;
		tab_col[10].default_value="0.0";
		tab_col[10].unsigned_flag=true;

		tab_col[11].name=dam_label::pop_endangered;
		tab_col[11].type=sys_label::tab_col_type_double;
		tab_col[11].default_value="0.0";
		tab_col[11].unsigned_flag=true;

		tab_col[12].name=risk_label::risk_pop_dan_with_hyd;
		tab_col[12].type=sys_label::tab_col_type_double;
		tab_col[12].default_value="0.0";
		tab_col[12].unsigned_flag=true;

		tab_col[13].name=risk_label::risk_pop_dan_fac;
		tab_col[13].type=sys_label::tab_col_type_double;
		tab_col[13].default_value="0.0";
		tab_col[13].unsigned_flag=true;

		tab_col[14].name=dam_label::elem_poly;
		tab_col[14].type=sys_label::tab_col_type_polygon;

		try{
			Risk_System::table_results_elem_pop= new Tables();
			if(Risk_System::table_results_elem_pop->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_risk->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Risk_System::create_table_result_elem_pop(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Risk_System::close_table_result_elem_pop();
			throw msg;
		}
		Risk_System::close_table_result_elem_pop();
	}
}
//Delete all data in the table for detailed results per damage raster-element of the people2risk damage raster (static)
void Risk_System::delete_data_in_table_result_elem_pop(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_pop(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Risk_System::table_results_elem_pop->delete_data_in_table(ptr_database);
}
//Close and delete the table for detailed results per damage raster-element of the people2risk damage raster (static)
void Risk_System::close_table_result_elem_pop(void){
	if(Risk_System::table_results_elem_pop!=NULL){
		delete Risk_System::table_results_elem_pop;
		Risk_System::table_results_elem_pop=NULL;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the people2risk damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_pop(QSqlDatabase *ptr_database, _sys_system_id id){
		//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_pop(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_pop->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_pop->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(label::measure_id) << " = " << id.measure_nr;
		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_pop(QSqlDatabase *ptr_database, _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_pop->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the people2risk damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_pop(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_pop(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_pop->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_pop->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_pop(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_pop->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the people2risk damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_pop(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_pop(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_pop->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_pop->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_pop(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_pop->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check if there are data available in the database table for detailed results per damage raster-element of the people2risk damage raster for specified parameters (static)
bool Risk_System::check_data_in_resulttable_elem_pop(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
		//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_pop(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"SELECT " << Risk_System::table_results_elem_pop->get_column_name(label::glob_id);
	query_string <<" FROM " << Risk_System::table_results_elem_pop->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_pop->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::check_data_in_resulttable_elem_pop(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_pop->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(query.size()>0){
		return true;
	}
	else{
		return false;
	}
}
//Set the table for detailed results per damage raster-element of the psycho-social damage raster: it sets the table name and the name of the columns and allocate them (static)
void Risk_System::set_table_result_elem_pys(QSqlDatabase *ptr_database){
	if(Risk_System::table_results_elem_pys==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_risk_pys_elem;
		string tab_col[31];
		tab_col[0]=label::glob_id;
		tab_col[1]=dam_label::elem_id;
		tab_col[2]=dam_label::raster_id;
		tab_col[3]=label::areastate_id;
		tab_col[4]=label::measure_id;
		tab_col[5]=hyd_label::sz_bound_id;
		tab_col[6]=risk_label::sz_break_id;
		tab_col[7]=risk_label::risk_type;
		tab_col[8]=dam_label::crit_age_50_59;
		tab_col[9]=risk_label::risk_crit_age_50_59_with_hyd;
		tab_col[10]=dam_label::crit_age_80;
		tab_col[11]=risk_label::risk_crit_age_80_with_hyd;
		tab_col[12]=dam_label::crit_female;
		tab_col[13]=risk_label::risk_crit_female_with_hyd;
		tab_col[14]=dam_label::crit_owner;
		tab_col[15]=risk_label::risk_crit_owner_with_hyd;
		tab_col[16]=dam_label::total_score;
		tab_col[17]=risk_label::risk_pys_total_score;
		tab_col[18]=risk_label::risk_pys_total_fac;
		tab_col[19]=dam_label::denscrit_age50_59;
		tab_col[20]=risk_label::risk_dens_crit_age_50_59_with_hyd;
		tab_col[21]=dam_label::denscrit_age_80;
		tab_col[22]=risk_label::risk_dens_crit_age_80_with_hyd;
		tab_col[23]=dam_label::denscrit_female;
		tab_col[24]=risk_label::risk_dens_crit_female_with_hyd;
		tab_col[25]=dam_label::denscrit_owner;
		tab_col[26]=risk_label::risk_dens_crit_owner_with_hyd;
		tab_col[27]=dam_label::total_dens_score;
		tab_col[28]=risk_label::risk_pys_total_dens_score;
		tab_col[29]=risk_label::risk_pys_total_dens_fac;
		tab_col[30]=dam_label::elem_poly;

		//set the table
		try{
			Risk_System::table_results_elem_pys= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_System::table_results_elem_pys->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_System::set_table_result_elem_pys(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Risk_System::close_table_result_elem_pys();
			throw msg;
		}
	}
}
//Create the table for detailed results per damage raster-element of the psycho-social damage raster (static)
void Risk_System::create_table_result_elem_pys(QSqlDatabase *ptr_database){
	if(Risk_System::table_results_elem_pys==NULL){
		ostringstream cout;
		cout << "Create database table of the detailed risk results for the psycho-social elements..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		//make specific input for this class
		const string tab_name=risk_label::tab_risk_pys_elem;
		const int num_col=31;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}

		tab_col[0].name=label::glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=dam_label::elem_id;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag=true;
		tab_col[1].key_flag=true;

		tab_col[2].name=dam_label::raster_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;
		tab_col[2].key_flag=true;

		tab_col[3].name=label::areastate_id;
		tab_col[3].type=sys_label::tab_col_type_int;
		tab_col[3].unsigned_flag=true;
		tab_col[3].key_flag=true;

		tab_col[4].name=label::measure_id;
		tab_col[4].type=sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag=true;
		tab_col[4].key_flag=true;

		tab_col[5].name=hyd_label::sz_bound_id;
		tab_col[5].type=sys_label::tab_col_type_int;
		tab_col[5].unsigned_flag=true;
		tab_col[5].key_flag=true;
		
		tab_col[6].name=risk_label::sz_break_id;
		tab_col[6].type=sys_label::tab_col_type_string;
		tab_col[6].key_flag=true;

		tab_col[7].name=risk_label::risk_type;
		tab_col[7].type=sys_label::tab_col_type_string;
		tab_col[7].key_flag=true;

		tab_col[8].name=dam_label::crit_age_50_59;
		tab_col[8].type=sys_label::tab_col_type_double;
		tab_col[8].default_value="0.0";
		tab_col[8].unsigned_flag=true;

		tab_col[9].name=risk_label::risk_crit_age_50_59_with_hyd;
		tab_col[9].type=sys_label::tab_col_type_double;
		tab_col[9].default_value="0.0";
		tab_col[9].unsigned_flag=true;

		tab_col[10].name=dam_label::crit_age_80;
		tab_col[10].type=sys_label::tab_col_type_double;
		tab_col[10].default_value="0.0";
		tab_col[10].unsigned_flag=true;

		tab_col[11].name=risk_label::risk_crit_age_80_with_hyd;
		tab_col[11].type=sys_label::tab_col_type_double;
		tab_col[11].default_value="0.0";
		tab_col[11].unsigned_flag=true;

		tab_col[12].name=dam_label::crit_female;
		tab_col[12].type=sys_label::tab_col_type_double;
		tab_col[12].default_value="0.0";
		tab_col[12].unsigned_flag=true;

		tab_col[13].name=risk_label::risk_crit_female_with_hyd;
		tab_col[13].type=sys_label::tab_col_type_double;
		tab_col[13].default_value="0.0";
		tab_col[13].unsigned_flag=true;

		tab_col[14].name=dam_label::crit_owner;
		tab_col[14].type=sys_label::tab_col_type_double;
		tab_col[14].default_value="0.0";
		tab_col[14].unsigned_flag=true;

		tab_col[15].name=risk_label::risk_crit_owner_with_hyd;
		tab_col[15].type=sys_label::tab_col_type_double;
		tab_col[15].default_value="0.0";
		tab_col[15].unsigned_flag=true;

		tab_col[16].name=dam_label::total_score;
		tab_col[16].type=sys_label::tab_col_type_double;
		tab_col[16].default_value="0.0";
		tab_col[16].unsigned_flag=true;

		tab_col[17].name=risk_label::risk_pys_total_score;
		tab_col[17].type=sys_label::tab_col_type_double;
		tab_col[17].default_value="0.0";
		tab_col[17].unsigned_flag=true;

		tab_col[18].name=risk_label::risk_pys_total_fac;
		tab_col[18].type=sys_label::tab_col_type_double;
		tab_col[18].default_value="0.0";
		tab_col[18].unsigned_flag=true;

		tab_col[19].name=dam_label::denscrit_age50_59;
		tab_col[19].type=sys_label::tab_col_type_double;
		tab_col[19].default_value="0.0";
		tab_col[19].unsigned_flag=true;

		tab_col[20].name=risk_label::risk_dens_crit_age_50_59_with_hyd;
		tab_col[20].type=sys_label::tab_col_type_double;
		tab_col[20].default_value="0.0";
		tab_col[20].unsigned_flag=true;

		tab_col[21].name=dam_label::denscrit_age_80;
		tab_col[21].type=sys_label::tab_col_type_double;
		tab_col[21].default_value="0.0";
		tab_col[21].unsigned_flag=true;

		tab_col[22].name=risk_label::risk_dens_crit_age_80_with_hyd;
		tab_col[22].type=sys_label::tab_col_type_double;
		tab_col[22].default_value="0.0";
		tab_col[22].unsigned_flag=true;


		tab_col[23].name=dam_label::denscrit_female;
		tab_col[23].type=sys_label::tab_col_type_double;
		tab_col[23].default_value="0.0";
		tab_col[23].unsigned_flag=true;

		tab_col[24].name=risk_label::risk_dens_crit_female_with_hyd;
		tab_col[24].type=sys_label::tab_col_type_double;
		tab_col[24].default_value="0.0";
		tab_col[24].unsigned_flag=true;

		tab_col[25].name=dam_label::denscrit_owner;
		tab_col[25].type=sys_label::tab_col_type_double;
		tab_col[25].default_value="0.0";
		tab_col[25].unsigned_flag=true;

		tab_col[26].name=risk_label::risk_dens_crit_owner_with_hyd;
		tab_col[26].type=sys_label::tab_col_type_double;
		tab_col[26].default_value="0.0";
		tab_col[26].unsigned_flag=true;

		tab_col[27].name=dam_label::total_dens_score;
		tab_col[27].type=sys_label::tab_col_type_double;
		tab_col[27].default_value="0.0";
		tab_col[27].unsigned_flag=true;

		tab_col[28].name=risk_label::risk_pys_total_dens_score;
		tab_col[28].type=sys_label::tab_col_type_double;
		tab_col[28].default_value="0.0";
		tab_col[28].unsigned_flag=true;

		tab_col[29].name=risk_label::risk_pys_total_dens_fac;
		tab_col[29].type=sys_label::tab_col_type_double;
		tab_col[29].default_value="0.0";
		tab_col[29].unsigned_flag=true;

		tab_col[30].name=dam_label::elem_poly;
		tab_col[30].type=sys_label::tab_col_type_polygon;

		try{
			Risk_System::table_results_elem_pys= new Tables();
			if(Risk_System::table_results_elem_pys->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_risk->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Risk_System::create_table_result_elem_pys(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Risk_System::close_table_result_elem_pys();
			throw msg;
		}
		Risk_System::close_table_result_elem_pys();
	}
}
//Delete all data in the table for detailed results per damage raster-element of the psycho-social damage raster (static)
void Risk_System::delete_data_in_table_result_elem_pys(QSqlDatabase *ptr_database){
		//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_pys(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Risk_System::table_results_elem_pys->delete_data_in_table(ptr_database);
}
//Close and delete the table for detailed results per damage raster-element of the psycho-social damage raster (static)
void Risk_System::close_table_result_elem_pys(void){
	if(Risk_System::table_results_elem_pys!=NULL){
		delete Risk_System::table_results_elem_pys;
		Risk_System::table_results_elem_pys=NULL;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the psycho-social damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_pys(QSqlDatabase *ptr_database, _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_pys(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_pys->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_pys->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(label::measure_id) << " = " << id.measure_nr;
		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_pys(QSqlDatabase *ptr_database, _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_pys->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the psycho-social damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_pys(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_pys(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_pys->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_pys->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_pys(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_pys->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results per damage raster-element of the psycho-social damage raster for specified parameters (static)
void Risk_System::delete_data_in_resulttable_elem_pys(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_pys(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_elem_pys->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_pys->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_elem_pys(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_pys->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check if there are data available in the database table for detailed results per damage raster-element of the psycho-social damage raster for specified parameters (static)
bool Risk_System::check_data_in_resulttable_elem_pys(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_pys(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"SELECT " << Risk_System::table_results_elem_pys->get_column_name(label::glob_id);
	query_string <<" FROM " << Risk_System::table_results_elem_pys->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_elem_pys->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::check_data_in_resulttable_elem_pys(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_elem_pys->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(query.size()>0){
		return true;
	}
	else{
		return false;
	}
}
//Set the table for detailed results per damage point of the simple counting damage: it sets the table name and the name of the columns and allocate them (static)
void Risk_System::set_table_result_point_sc(QSqlDatabase *ptr_database){
	if(Risk_System::table_results_point_sc==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_risk_sc_point;
		string tab_col[17];

		tab_col[0]=label::glob_id;
		tab_col[1]=dam_label::point_id;
		tab_col[2]=label::areastate_id;
		tab_col[3]=label::measure_id;
		tab_col[4]=hyd_label::sz_bound_id;	
		tab_col[5]=risk_label::sz_break_id;
		tab_col[6]=risk_label::risk_type;
		tab_col[7]=dam_label::sc_pub_build;
		tab_col[8]=risk_label::risk_sc_pub_build_with_hyd;
		tab_col[9]=dam_label::sc_eco_build;
		tab_col[10]=risk_label::risk_sc_eco_build_with_hyd;
		tab_col[11]=dam_label::sc_cult_build;
		tab_col[12]=risk_label::risk_sc_cult_build_with_hyd;
		tab_col[13]=dam_label::sc_person_build;
		tab_col[14]=risk_label::risk_sc_person_build_with_hyd;
		tab_col[15]=risk_label::risk_sc_fac;
		tab_col[16]=dam_label::sc_point;


		//set the table
		try{
			Risk_System::table_results_point_sc= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_System::table_results_point_sc->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_System::set_table_result_point_sc(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Risk_System::close_table_result_point_sc();
			throw msg;
		}
	}
}
//Create the table for detailed results per damage point of the simple counting damage (static)
void Risk_System::create_table_result_point_sc(QSqlDatabase *ptr_database){
	if(Risk_System::table_results_point_sc==NULL){
		ostringstream cout;
		cout << "Create database table of the detailed risk results for the simple counting points..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		//make specific input for this class
		const string tab_name=risk_label::tab_risk_sc_point;
		const int num_col=17;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}

		tab_col[0].name=label::glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=dam_label::point_id;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag=true;
		tab_col[1].key_flag=true;

		tab_col[2].name=label::areastate_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;
		tab_col[2].key_flag=true;

		tab_col[3].name=label::measure_id;
		tab_col[3].type=sys_label::tab_col_type_int;
		tab_col[3].unsigned_flag=true;
		tab_col[3].key_flag=true;

		tab_col[4].name=hyd_label::sz_bound_id;
		tab_col[4].type=sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag=true;
		tab_col[4].key_flag=true;
		
		tab_col[5].name=risk_label::sz_break_id;
		tab_col[5].type=sys_label::tab_col_type_string;
		tab_col[5].key_flag=true;

		tab_col[6].name=risk_label::risk_type;
		tab_col[6].type=sys_label::tab_col_type_string;
		tab_col[6].key_flag=true;

		tab_col[7].name=dam_label::sc_pub_build;
		tab_col[7].type=sys_label::tab_col_type_double;
		tab_col[7].default_value="0.0";
		tab_col[7].unsigned_flag=true;

		tab_col[8].name=risk_label::risk_sc_pub_build_with_hyd;
		tab_col[8].type=sys_label::tab_col_type_double;
		tab_col[8].default_value="0.0";
		tab_col[8].unsigned_flag=true;

		tab_col[9].name=dam_label::sc_eco_build;
		tab_col[9].type=sys_label::tab_col_type_double;
		tab_col[9].default_value="0.0";
		tab_col[9].unsigned_flag=true;

		tab_col[10].name=risk_label::risk_sc_eco_build_with_hyd;
		tab_col[10].type=sys_label::tab_col_type_double;
		tab_col[10].default_value="0.0";
		tab_col[10].unsigned_flag=true;

		tab_col[11].name=dam_label::sc_cult_build;
		tab_col[11].type=sys_label::tab_col_type_double;
		tab_col[11].default_value="0.0";
		tab_col[11].unsigned_flag=true;

		tab_col[12].name=risk_label::risk_sc_cult_build_with_hyd;
		tab_col[12].type=sys_label::tab_col_type_double;
		tab_col[12].default_value="0.0";
		tab_col[12].unsigned_flag=true;

		tab_col[13].name=dam_label::sc_person_build;
		tab_col[13].type=sys_label::tab_col_type_double;
		tab_col[13].default_value="0.0";
		tab_col[13].unsigned_flag=true;

		tab_col[14].name=risk_label::risk_sc_person_build_with_hyd;
		tab_col[14].type=sys_label::tab_col_type_double;
		tab_col[14].default_value="0.0";
		tab_col[14].unsigned_flag=true;

		tab_col[15].name=risk_label::risk_sc_fac;
		tab_col[15].type=sys_label::tab_col_type_double;
		tab_col[15].default_value="0.0";
		tab_col[15].unsigned_flag=true;

		tab_col[16].name=dam_label::sc_point;
		tab_col[16].type=sys_label::tab_col_type_point;

		try{
			Risk_System::table_results_point_sc= new Tables();
			if(Risk_System::table_results_point_sc->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_risk->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Risk_System::create_table_result_point_sc(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Risk_System::close_table_result_point_sc();
			throw msg;
		}
		Risk_System::close_table_result_point_sc();
	}
}
//Delete all data in the table for detailed results per damage point of the simple counting damage (static)
void Risk_System::delete_data_in_table_result_point_sc(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_point_sc(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Risk_System::table_results_point_sc->delete_data_in_table(ptr_database);
}
//Close and delete the table for detailed results per damage point of the simple counting damage (static)
void Risk_System::close_table_result_point_sc(void){
	if(Risk_System::table_results_point_sc!=NULL){
		delete Risk_System::table_results_point_sc;
		Risk_System::table_results_point_sc=NULL;
	}
}
//Delete all data in the database table for detailed results per damage point of the simple counting damage for specified parameters (static)
void Risk_System::delete_data_in_resulttable_point_sc(QSqlDatabase *ptr_database, _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_point_sc(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_point_sc->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_point_sc->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_point_sc->get_column_name(label::measure_id) << " = " << id.measure_nr;
		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_point_sc(QSqlDatabase *ptr_database, _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " <<Risk_System::table_results_point_sc->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results per damage point of the simple counting damage for specified parameters (static)
void Risk_System::delete_data_in_resulttable_point_sc(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_point_sc(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_point_sc->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_point_sc->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_point_sc->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_point_sc->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_point_sc(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " <<Risk_System::table_results_point_sc->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for detailed results per damage point of the simple counting damage for specified parameters (static)
void Risk_System::delete_data_in_resulttable_point_sc(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_point_sc(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_System::table_results_point_sc->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_point_sc->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_point_sc->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_point_sc->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_point_sc->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_point_sc->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::delete_data_in_resulttable_point_sc(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " <<Risk_System::table_results_point_sc->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check if there are data available in the database table for detailed results per damage point of the simple counting damage for specified parameters (static) 
bool Risk_System::check_data_in_resulttable_point_sc(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_point_sc(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"SELECT " << Risk_System::table_results_point_sc->get_column_name(label::glob_id);
	query_string <<" FROM " << Risk_System::table_results_point_sc->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_System::table_results_point_sc->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_System::table_results_point_sc->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_System::table_results_point_sc->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_point_sc->get_column_name(risk_label::sz_break_id) << " = '" << break_id <<"'";
	query_string << " AND ";
	query_string << Risk_System::table_results_point_sc->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_System::check_data_in_resulttable_point_sc(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_results_point_sc->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(query.size()>0){
		return true;
	}
	else{
		return false;
	}
}
//Input the control parameters per database table
void Risk_System::set_input(QSqlDatabase *ptr_database, const bool with_output){
	//output
	ostringstream prefix;
	prefix <<"INP> ";
	Sys_Common_Output::output_risk->set_userprefix(&prefix);
	ostringstream cout;
	cout << "Set control parameter for the risk calculation per database..."  << endl;
	if(with_output==true){
		Sys_Common_Output::output_risk->output_txt(&cout,false);
	}

	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	int number_result=0;
	//the table is set the name and the column names
	try{
		Risk_System::set_table_config(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the complet table of control parameters FPL
	model.setTable(Risk_System::table_config->get_table_name().c_str());
    //set the query
	Data_Base::database_request(&model);
	//check the query
	if(model.lastError().isValid()){
		//warn msg
		Warning msg=set_warning(5);
		stringstream info;
		info << "Table Name       : " << Risk_System::table_config->get_table_name() << endl;
		info << "Table error info : " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(5);
		//reaction use all default values
		number_result=0;
	}
	else{
		number_result=model.rowCount();
	}
	//read out the results
	 //the set of the name-column has to match to this parameter
	//output
	if(number_result>0){
		cout << "Search for the RISK control parameters in table " <<  Risk_System::table_config->get_table_name() <<"..."<< endl;
		if(with_output==true){
			Sys_Common_Output::output_risk->output_txt(cout.str(),false);
		}
		//read out from the search result
		string buffer;

		for(int i=0; i< number_result; i++){
			buffer=model.record(i).value((Risk_System::table_config->get_column_name(risk_label::name_param)).c_str()).toString().toStdString();
			if(buffer==risk_control_param::main_outflow_rv){
				this->id_main_river=model.record(i).value((Risk_System::table_config->get_column_name(risk_label::value_param)).c_str()).toInt();
				cout << "Control parameter " << risk_control_param::main_outflow_rv <<"  is found"  << endl;	
			}
			else if(buffer==risk_control_param::boundary_prob){
				this->control_params_ca.prob_bound=model.record(i).value((Risk_System::table_config->get_column_name(risk_label::value_param)).c_str()).toDouble();
				cout << "Control parameter " << risk_control_param::boundary_prob <<"  is found"  << endl;
				
			}
			else if(buffer==risk_control_param::max_hydraulic){
				this->control_params_ca.max_hyd_calc=model.record(i).value((Risk_System::table_config->get_column_name(risk_label::value_param)).c_str()).toInt();
				cout << "Control parameter " << risk_control_param::max_hydraulic <<"  is found"  << endl;
				
			}
			else if(buffer==risk_control_param::number_mc_sim){
				this->control_params_ca.number_mc=model.record(i).value((Risk_System::table_config->get_column_name(risk_label::value_param)).c_str()).toInt();
				cout << "Control parameter " << risk_control_param::number_mc_sim <<"  is found"  << endl;
			}
			else if(buffer==risk_control_param::number_intervals){
				this->control_params_ca.number_intervals=model.record(i).value((Risk_System::table_config->get_column_name(risk_label::value_param)).c_str()).toInt();
				cout << "Control parameter " << risk_control_param::number_intervals <<"  is found"  << endl;
			}
			else if(buffer==risk_control_param::use_prediction){
				this->control_params_ca.use_predict_values=model.record(i).value((Risk_System::table_config->get_column_name(risk_label::value_param)).c_str()).toBool();
				cout << "Control parameter " << risk_control_param::use_prediction <<"  is found"  << endl;
			}
			else if(buffer==risk_control_param::rand_generator){
				string buffer;
				buffer=model.record(i).value((Risk_System::table_config->get_column_name(risk_label::value_param)).c_str()).toString().toStdString();
				this->control_params_ca.gen_type=Fpl_Mc_Sim::convert_text2generator_type(buffer);
				cout << "Control parameter " << risk_control_param::rand_generator <<"  is found"  << endl;
			}
		}
		if(with_output==true){
			Sys_Common_Output::output_risk->output_txt(cout.str(),true);
		}
	}
	//check the parameter
	this->check_control_params();
	Sys_Common_Output::output_risk->rewind_userprefix();
}
//Ask flags which part of the database tables should be restored with the default values and which risk results should be deleted
bool Risk_System::ask_delete_result_restore_database(QWidget *parent){
	Sys_Multi_CheckBox_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":risk_icon");
	my_dia.set_number_check_boxes(4,icon);
	stringstream buffer;
	buffer << "Choose which risk result data should be deleted. Choose if the risk configuration \n data should be restored..."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose deleting/restoring risk data");
	my_dia.get_ptr_check_box(0)->setText("Delete risk results nobreak risk type");
	my_dia.get_ptr_check_box(1)->setText("Delete risk results scenario risk type (SC)");
	my_dia.get_ptr_check_box(2)->setText("Delete risk results and break scenarios of the catchment risk type (CA)");
	my_dia.get_ptr_check_box(3)->setText("Restore risk configuration data");

	
	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{


			
		this->del_nobreak_result=my_dia.get_bool_check_box(0);
		this->del_scenario_result=my_dia.get_bool_check_box(1);
		this->del_catchment_result=my_dia.get_bool_check_box(2);
		this->restore_config=my_dia.get_bool_check_box(3);

		if(this->del_nobreak_result==false && this->del_scenario_result==false &&
			this->del_catchment_result==false && this->restore_config==false){
			return false;
		}
		else if((this->del_nobreak_result==true || this->restore_config==true) && this->del_scenario_result==false &&
			this->del_catchment_result==false ){
				Sys_Diverse_Text_Dia dialog2;
				ostringstream txt;
				txt<<"Do you want to delete/restore following results/parameters: "<< endl;
				if(this->del_nobreak_result==true){
					txt<<" Delete the nobreak risk results "<< endl;
				}
				if(this->restore_config==true){
					txt<<" Restore the risk calculation control parameters "<< endl;
				}
				txt<<"The data will be irrecoverable. Do you want to continue?"<< endl;
				dialog2.set_dialog_question(txt.str());
				if(dialog2.start_dialog()==false){
					this->del_nobreak_result=false;
					this->del_scenario_result=false;
					this->del_catchment_result=false;
					this->restore_config=false;
					return false;
				}
				else{
					return true;
				}
		}
		else{
			Sys_Multi_CheckBox_Dia my_dia1(parent);
			//set up dialog
			QIcon icon;
			icon.addFile(":risk_icon");
			my_dia1.set_number_check_boxes(3,icon);
			stringstream buffer;
			buffer << "Choose which appending results should be deleted besides the risk result data."<< endl;
			my_dia1.set_main_text_label(buffer.str());
			buffer.str("");
			my_dia1.set_window_title("Choose deleting appending risk data");
			my_dia1.get_ptr_check_box(0)->setText("Delete the appending FPL-results");
			my_dia1.get_ptr_check_box(1)->setText("Delete the appending HYD-results");
			my_dia1.get_ptr_check_box(2)->setText("Delete the appending DAM-results");
			if(my_dia1.start_dialog()==false){
				this->del_dam_results=false;
				this->del_hyd_results=false;
				this->del_fpl_results=false;
				return false;
			}
			else{
				this->del_fpl_results=my_dia1.get_bool_check_box(0);
				this->del_hyd_results=my_dia1.get_bool_check_box(1);
				this->del_dam_results=my_dia1.get_bool_check_box(2);

				string buff_implicit;
				if(this->del_hyd_results==true && this->del_dam_results==false){
					this->del_dam_results=true;
					buff_implicit =" (implicit) ";
				}

				Sys_Diverse_Text_Dia dialog2;
				ostringstream txt;
				txt<<"Do you want to delete/restore following results/parameters: "<< endl;
				if(this->del_nobreak_result==true){
					txt<<" Delete the nobreak risk results "<< endl;
				}
				if(this->del_scenario_result==true){
					txt<<" Delete the scenario risk results "<< endl;
					if(this->del_fpl_results==true){
						txt<<"  ...the appending FPL-results"<<endl;

					}
					if(this->del_hyd_results==true){
						txt<<"  ...the appending HYD-results"<<endl;

					}
					if(this->del_dam_results==true){
						txt<<"  ...the appending DAM-results" << buff_implicit <<endl;
					}
					
				}
				if(this->del_catchment_result==true){
					txt<<" Delete the catchment based risk results "<< endl;
					if(this->del_fpl_results==true){
						txt<<"  ...the appending FPL-results"<<endl;

					}
					if(this->del_hyd_results==true){
						txt<<"  ...the appending HYD-results"<<endl;

					}
					if(this->del_dam_results==true){
						txt<<"  ...the appending DAM-results"<<endl;
					}
					
				}
				if(this->restore_config==true){
					txt<<" Restore the risk calculation control parameters "<< endl;
				}
				txt<<"The data will be irrecoverable. Do you want to continue?"<< endl;
				dialog2.set_dialog_question(txt.str());
				if(dialog2.start_dialog()==false){
					this->del_nobreak_result=false;
					this->del_scenario_result=false;
					this->del_catchment_result=false;
					this->restore_config=false;
					this->del_dam_results=false;
					this->del_hyd_results=false;
					this->del_fpl_results=false;
					return false;
				}
				else{
					return true;
				}
			}


		}
		
	}
}
//Ask the flags via dialog, which steps of the risk analysis should be calculated in serial
bool Risk_System::ask_steps_serial_calculation(QWidget *parent){

	RiskGui_Calculate_All_Dia my_dia(parent);
	if(my_dia.start_dialog()==true){
		my_dia.set_flags(&this->calc_ca_nobreak_step, &this->calc_ca_sc_lists_step, &this->calc_ca_one_break_step, &this->calc_ca_auto_step);
		return true;
	}
	else{
		return false;
	}

}
//Ask for the boundaries of the predicted risk for an automatic risk calculation in caase of the catchment area risk approach
bool Risk_System::ask_boundries_predict_risk(QWidget *parent){

	this->set_input(&this->qsqldatabase, false);
	

	if(this->control_params_ca.use_predict_values==false){
		return true;
	}

	RiskGui_Optimisation_Prediction_Dia my_dia(parent);

	if(my_dia.start_dialog()==true){
		my_dia.set_boundaries(&this->control_params_ca);
		return true;
	}
	else{
		return false;
	}

}
//Set and connect the damage results and the probabilities to the detailed results for the nobreak risk approach
void Risk_System::set_connect_dam_prob2detailed_result_nobreak(void){

	ostringstream cout;
	cout << "Set the damage- and probability data for the risk calculation..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
	if(this->id_main_river<0){
		if(Hyd_Model_River::check_relevant_model_database(&this->qsqldatabase)>0){
			//Warning
			Warning msg=this->set_warning(9);
			ostringstream info;
			info<<"Outflow profile of river id      : " << this->id_main_river<<endl;
			msg.make_second_info(info.str());
			msg.output_msg(5);
		}
	}
	
	Hyd_Boundary_Szenario_Management my_hyd_sc;
	my_hyd_sc.set_systemid(this->system_id);
	my_hyd_sc.set_szenario_per_db(&this->qsqldatabase);

	this->number_detailed_results=my_hyd_sc.get_number_sz();
	this->allocate_detailed_results();

	QSqlQueryModel model;

	for(int i=0; i< this->number_detailed_results; i++){
		this->detailed_results[i].prob_hyd_bound_sc=my_hyd_sc.get_ptr_sz(i)->get_prob_occurence();
		this->detailed_results[i].reocc_prob_hyd_bound_sc=my_hyd_sc.get_ptr_sz(i)->get_prob_year();
		this->detailed_results[i].annuality_hyd_bound_sc=my_hyd_sc.get_ptr_sz(i)->get_annuality();
		this->detailed_results[i].dam_results.id_break_sc="CA";
		this->detailed_results[i].dam_results.id_hyd_bound_sc=my_hyd_sc.get_ptr_sz(i)->get_id();
		this->detailed_results[i].dam_results.hyd_bound_sc_name=my_hyd_sc.get_ptr_sz(i)->get_name();
		this->detailed_results[i].prob_break_sc=1;
		int number=0;
		number=Dam_Damage_System::select_data_in_erg_table(&model, &this->qsqldatabase, this->system_id, my_hyd_sc.get_ptr_sz(i)->get_id(), "CA");
		if(number==0){
			//Warning
			Warning msg=this->set_warning(5);
			ostringstream info;
			info<<"Hydraulic boundary scenario id   : " <<my_hyd_sc.get_ptr_sz(i)->get_id()<<endl;
			info<<"Hydraulic boundary scenario name : " <<my_hyd_sc.get_ptr_sz(i)->get_name()<<endl;
			msg.make_second_info(info.str());
			msg.output_msg(5);
		}
		else{
			//read them out
			this->detailed_results[i].dam_results.ecn_immob_dam=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ecn_mob_dam=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ecn_immob_dam_5=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::immob_5_quant)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ecn_immob_dam_95=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::immob_95_quant)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ecn_mob_dam_5=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::mob_5_quant)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ecn_mob_dam_95=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::mob_95_quant)).c_str()).toDouble();

			this->detailed_results[i].dam_results.eco_biotype=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
			this->detailed_results[i].dam_results.eco_soil_erosion=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
			
			this->detailed_results[i].dam_results.pop_affected=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pop_endangered=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();

			this->detailed_results[i].dam_results.pys_age_50_59=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_age_50_59_dens=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_age_80=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_age_80_dens=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_female=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_female)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_female_dens=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_female)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_owner=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_owner)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_owner_dens=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble();

			this->detailed_results[i].dam_results.sc_cult_build=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_cult_build)).c_str()).toDouble();
			this->detailed_results[i].dam_results.sc_eco_build=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_eco_build)).c_str()).toDouble();
			this->detailed_results[i].dam_results.sc_pub_build=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_pub_build)).c_str()).toDouble();
			this->detailed_results[i].dam_results.sc_person_build=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_person_build)).c_str()).toDouble();

			//ci
			this->detailed_results[i].dam_results.ci_elect_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_elect_pt)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_info_tec_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_info_tec_pt)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_water_sup_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_sup_pt)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_water_treat_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_treat_pt)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_energy_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_energy_pt)).c_str()).toDouble();

			this->detailed_results[i].dam_results.ci_health_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_health_pt)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_social_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_social_pt)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_mat_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_mat_pt)).c_str()).toDouble();


			this->detailed_results[i].dam_results.ci_elect_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_elect_p)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_info_tec_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_info_tec_p)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_water_sup_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_sup_p)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_water_treat_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_treat_p)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_energy_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_energy_p)).c_str()).toDouble();

			this->detailed_results[i].dam_results.ci_health_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_health_p)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_social_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_social_p)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_mat_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_mat_p)).c_str()).toDouble();

		}
		double discharge=0.0;
		if(this->id_main_river>=0){
			discharge=_Hyd_River_Profile::get_maximum_outflow_discharge(&this->qsqldatabase, this->system_id, this->id_main_river,my_hyd_sc.get_ptr_sz(i)->get_id(), "CA");
			if(discharge<0.0){
				//Warning
				Warning msg=this->set_warning(6);
				ostringstream info;
				info<<"Outflow profile of river id      : " << this->id_main_river<<endl;
				info<<"Hydraulic boundary scenario id   : " <<my_hyd_sc.get_ptr_sz(i)->get_id()<<endl;
				info<<"Hydraulic boundary scenario name : " <<my_hyd_sc.get_ptr_sz(i)->get_name()<<endl;
				msg.make_second_info(info.str());
				msg.output_msg(5);
			}
			else{
				//read them out
				this->detailed_results[i].max_outflow=discharge;
			}
		}
	}
	this->sort_detailed_result();
	cout << "Setting of the damage- and probability data for the risk calculation is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
}
//Set and connect the damage results and the probabilities to the detailed results for the scenario- and catchment area base risk approach
void Risk_System::set_connect_dam_prob2detailed_result_break(void){
	ostringstream cout;
	cout << "Set the damage- and probability data for the risk calculation..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
	if(this->id_main_river<0){
		if(Hyd_Model_River::check_relevant_model_database(&this->qsqldatabase)>0){
			//Warning
			Warning msg=this->set_warning(10);
			ostringstream info;
			info<<"Outflow profile of river id      : " << this->id_main_river<<endl;
			msg.make_second_info(info.str());
			msg.output_msg(5);
		}
	}
	this->delete_detailed_results();
	this->number_detailed_results=this->break_sc_manager.get_number_sc2calc();
	this->allocate_detailed_results();

	QSqlQueryModel model;

	for(int i=0; i< this->number_detailed_results; i++){
		this->detailed_results[i].prob_hyd_bound_sc=this->break_sc_manager.get_ptr_sc2calc(i)->get_hydraulic_boundary_scenario().get_prob_occurence();
		this->detailed_results[i].reocc_prob_hyd_bound_sc=this->break_sc_manager.get_ptr_sc2calc(i)->get_hydraulic_boundary_scenario().get_prob_year();
		this->detailed_results[i].annuality_hyd_bound_sc=this->break_sc_manager.get_ptr_sc2calc(i)->get_hydraulic_boundary_scenario().get_annuality();
		this->detailed_results[i].dam_results.id_break_sc=this->break_sc_manager.get_ptr_sc2calc(i)->get_break_scenario_string();
		this->detailed_results[i].dam_results.id_hyd_bound_sc=this->break_sc_manager.get_ptr_sc2calc(i)->get_hydraulic_boundary_scenario().get_id();
		this->detailed_results[i].dam_results.hyd_bound_sc_name=this->break_sc_manager.get_ptr_sc2calc(i)->get_hydraulic_boundary_scenario().get_name();
		this->detailed_results[i].prob_break_sc=this->break_sc_manager.get_ptr_sc2calc(i)->get_prob_scenario();
		
		int number=0;
		number=Dam_Damage_System::select_data_in_erg_table(&model, &this->qsqldatabase, this->system_id, this->detailed_results[i].dam_results.id_hyd_bound_sc, this->detailed_results[i].dam_results.id_break_sc);
		if(number==0){
			//Warning
			Error msg=this->set_error(2);
			ostringstream info;
			info<<"Hydraulic boundary scenario id   : " <<this->detailed_results[i].dam_results.id_hyd_bound_sc<<endl;
			info<<"Hydraulic boundary scenario name : " <<this->detailed_results[i].dam_results.hyd_bound_sc_name<<endl;
			info<<"Break scenario name              : " <<this->detailed_results[i].dam_results.id_break_sc<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		else{
			//read them out
			this->detailed_results[i].dam_results.ecn_immob_dam=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ecn_mob_dam=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ecn_immob_dam_5=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::immob_5_quant)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ecn_immob_dam_95=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::immob_95_quant)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ecn_mob_dam_5=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::mob_5_quant)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ecn_mob_dam_95=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::mob_95_quant)).c_str()).toDouble();

			this->detailed_results[i].dam_results.eco_biotype=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
			this->detailed_results[i].dam_results.eco_soil_erosion=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
			
			this->detailed_results[i].dam_results.pop_affected=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pop_endangered=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();

			this->detailed_results[i].dam_results.pys_age_50_59=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_age_50_59_dens=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_age_80=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_age_80_dens=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_female=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_female)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_female_dens=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_female)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_owner=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_owner)).c_str()).toDouble();
			this->detailed_results[i].dam_results.pys_owner_dens=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble();

			this->detailed_results[i].dam_results.sc_cult_build=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_cult_build)).c_str()).toDouble();
			this->detailed_results[i].dam_results.sc_eco_build=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_eco_build)).c_str()).toDouble();
			this->detailed_results[i].dam_results.sc_pub_build=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_pub_build)).c_str()).toDouble();
			this->detailed_results[i].dam_results.sc_person_build=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_person_build)).c_str()).toDouble();


			
			//ci
			this->detailed_results[i].dam_results.ci_elect_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_elect_pt)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_info_tec_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_info_tec_pt)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_water_sup_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_sup_pt)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_water_treat_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_treat_pt)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_energy_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_energy_pt)).c_str()).toDouble();

			this->detailed_results[i].dam_results.ci_health_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_health_pt)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_social_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_social_pt)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_mat_pt = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_mat_pt)).c_str()).toDouble();


			this->detailed_results[i].dam_results.ci_elect_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_elect_p)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_info_tec_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_info_tec_p)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_water_sup_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_sup_p)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_water_treat_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_treat_p)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_energy_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_energy_p)).c_str()).toDouble();

			this->detailed_results[i].dam_results.ci_health_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_health_p)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_social_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_social_p)).c_str()).toDouble();
			this->detailed_results[i].dam_results.ci_mat_p = model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_mat_p)).c_str()).toDouble();

		}
		double discharge=0.0;
		if(this->id_main_river>=0){
			discharge=_Hyd_River_Profile::get_maximum_outflow_discharge(&this->qsqldatabase, this->system_id, this->id_main_river,this->detailed_results[i].dam_results.id_hyd_bound_sc, this->detailed_results[i].dam_results.id_break_sc);
			if(discharge<0.0){
				//Warning
				Warning msg=this->set_warning(12);
				ostringstream info;
				info<<"Outflow profile of river id      : " << this->id_main_river<<endl;
				info<<"Hydraulic boundary scenario id   : " <<this->detailed_results[i].dam_results.id_hyd_bound_sc<<endl;
				info<<"Hydraulic boundary scenario name : " <<this->detailed_results[i].dam_results.hyd_bound_sc_name<<endl;
				info<<"Break scenario name              : " <<this->detailed_results[i].dam_results.id_break_sc<<endl;
				msg.make_second_info(info.str());
				msg.output_msg(5);

			}
			else{
				//read them out
				this->detailed_results[i].max_outflow=discharge;
			}
		}
	}
	this->sort_detailed_result();
	cout << "Setting of the damage- and probability data for the risk calculation is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
}
//Calculate the detailed risk 
void Risk_System::calculate_detailed_result(void){
	ostringstream cout;
	cout << "Calculate the detailed risk results..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
	for(int i=0; i< this->number_detailed_results; i++){
		this->risk_result_struct2detailed(&this->detailed_results[i]);
	}
	cout << "Calculation of the detailed risk results is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
}
//Calculate the cumulated risk result
void Risk_System::calculate_cumulated_result(void){
	ostringstream cout;
	cout << "Calculate the cumulated risk results..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);

	if(this->number_detailed_results>0){
		this->delete_cumulated_results();
		this->number_cumulated_results=1;
		this->allocate_cumulated_results();
		this->cumulated_results[0]=this->detailed_results[0];
		this->cumulated_results[0].number_applied_result++;
	}
	for(int i=1; i<this->number_detailed_results; i++){
		int index=-1;
		for(int j=0; j< this->number_cumulated_results; j++){
			if(this->cumulated_results[j].annuality_hyd_bound_sc==this->detailed_results[i].annuality_hyd_bound_sc){
				index=j;
				break;
			}
		}
		if(index<0){
			this->add_entry2cumulated_results();
			this->cumulated_results[this->number_cumulated_results-1]=this->detailed_results[i];
			this->cumulated_results[this->number_cumulated_results-1].number_applied_result++;
		}
		else{
			this->add2risk_result_cumulation(&this->cumulated_results[index], &this->detailed_results[i]);
		}
	}
	cout << "Calculation of the cumulated risk results is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
}
//Calculate the total risk result
void Risk_System::calculate_total_result(void){
	ostringstream cout;
	cout << "Calculate the total risk result..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
	this->init_risk_result_struct(&this->total_results);
	//double upper=0.0;
	//double lower=0.0;

	for(int i=0; i<this->number_cumulated_results; i++){
		this->integration2total_result(this->cumulated_results[i].reocc_prob_hyd_bound_sc, 0.0, &this->cumulated_results[i]);


		//if(i==0 && i!=this->number_cumulated_results-1){
		//	upper=this->cumulated_results[i].reocc_prob_hyd_bound_sc*2.0;

		//	lower=(this->cumulated_results[i].reocc_prob_hyd_bound_sc+this->cumulated_results[i+1].reocc_prob_hyd_bound_sc)*0.5;
		//	this->integration2total_result(lower, upper, &this->cumulated_results[i]);
		//}
		//else if(i!=0 && i==this->number_cumulated_results-1){
		//	upper=(this->cumulated_results[i].reocc_prob_hyd_bound_sc+this->cumulated_results[i-1].reocc_prob_hyd_bound_sc)*0.5;;
		//	lower=0.0;
		//	this->integration2total_result(lower, upper, &this->cumulated_results[i]);
		//}
		//else if(i==0 && i==this->number_cumulated_results-1){
		//	upper=this->cumulated_results[i].reocc_prob_hyd_bound_sc*2.0;
		//	lower=0.0;
		//	this->integration2total_result(lower, upper, &this->cumulated_results[i]);
		//}
		//else{
		//	upper=(this->cumulated_results[i].reocc_prob_hyd_bound_sc+this->cumulated_results[i-1].reocc_prob_hyd_bound_sc)*0.5;;
		//	lower=(this->cumulated_results[i].reocc_prob_hyd_bound_sc+this->cumulated_results[i+1].reocc_prob_hyd_bound_sc)*0.5;
		//	this->integration2total_result(lower, upper, &this->cumulated_results[i]);

		//}

	}

	cout << "Calculation of the total risk results is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
}
//Set the hydraulic state data
void Risk_System::set_hyd_state_data(const _hyd_state_data hydraulic_state){
	this->hydraulic_state=hydraulic_state;
}
//Get the number of detailed risk results
int Risk_System::get_number_detailed_results(void){
	return this->number_detailed_results;
}
//Get a pointer to the detailed risk results
_risk_risk_results* Risk_System::get_ptr_detailed_results(void){
	return this->detailed_results;
}
//Get the detailed risk results by a given index
_risk_risk_results Risk_System::get_detailed_results(const int index){
	_risk_risk_results buffer;
	if(index < 0 || index >=this->number_detailed_results){
		this->init_risk_result_struct(&buffer);
		return buffer;
	}
	else{
		buffer=this->detailed_results[index];
		return buffer;
	}
}
//Get the number of cumulated risk results
int Risk_System::get_number_cumulated_results(void){
	return this->number_cumulated_results;
}
//Get a pointer to the cumulated risk results
_risk_risk_results* Risk_System::get_ptr_cumulated_results(void){
	return this->cumulated_results;
}
//Get the cumulated risk results by a given index
_risk_risk_results Risk_System::get_cumulated_results(const int index){
	_risk_risk_results buffer;
	if(index < 0 || index >=this->number_cumulated_results){
		this->init_risk_result_struct(&buffer);
		return buffer;
	}
	else{
		buffer=this->cumulated_results[index];
		return buffer;
	}
}
//Get the total risk results
_risk_risk_results Risk_System::get_total_result(void){
	return this->total_results;
}
//Set the thread type for specifiying the task of the thread
void Risk_System::set_thread_type(_risk_thread_type type){
	this->thread_type=type;
}
//Set a pointer to the database; the copy of the database is made in this method
void Risk_System::set_ptr2database(QSqlDatabase *ptr_database){
	this->qsqldatabase=QSqlDatabase::cloneDatabase(*ptr_database, sys_label::str_risk.c_str());
	this->qsqldatabase.open();
}
//Get the thread type
_risk_thread_type Risk_System::get_thread_type(void){
	return this->thread_type;
}
//Create a whole set of risk database tables
void Risk_System::create_risk_database_tables(void){
	ostringstream cout;
	this->set_exception_number_start();
	cout << "Create RISK system tables..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
	try{
		if(Risk_System::create_table_config(&this->qsqldatabase)==true){
			Risk_System::set_predefined_data2config_table(&this->qsqldatabase, -1);
		}
		Risk_System::create_resulttable_detailed(&this->qsqldatabase);
		Risk_Damage_Predictor::create_resulttable_predicted_detailed(&this->qsqldatabase);
		Risk_System::create_resulttable_cumulated(&this->qsqldatabase);
		Risk_System::create_resulttable(&this->qsqldatabase);

		Risk_System::create_table_result_elem_ecn(&this->qsqldatabase);
		Risk_System::create_table_result_elem_eco_btype(&this->qsqldatabase);
		Risk_System::create_table_result_elem_eco_soil(&this->qsqldatabase);
		Risk_System::create_table_result_elem_pop(&this->qsqldatabase);
		Risk_System::create_table_result_elem_pys(&this->qsqldatabase);
		Risk_System::create_table_result_point_sc(&this->qsqldatabase);

		Risk_Break_Scenario_Management::create_table(&this->qsqldatabase);
		Risk_Break_Scenario_Management::create_table_fpl_sec_prob(&this->qsqldatabase);

		

	}
	catch(Error msg){
		msg.output_msg(5);
	}
	this->set_exception_number_end();
	cout << "Creation of the RISK database tables is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
	this->output_error_statistic();
}
//Check all risk database tables, for their existence in the database and their declaration in the database table-file
void Risk_System::check_risk_database_tables(void){

	ostringstream cout;
	this->set_exception_number_start();
	cout << "Check RISK system tables..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout,false, false);
	try{
		cout << "Check database table of risk configuration..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_System::set_table_config(&this->qsqldatabase);
		cout << "Check database table of detailed risk results..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_System::set_resulttable_detailed(&this->qsqldatabase);
		cout << "Check database table of detailed predicted risk results..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_Damage_Predictor::set_resulttable_predicted_detailed(&this->qsqldatabase);
		cout << "Check database table of detailed risk results for the economical elements..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_System::set_table_result_elem_ecn(&this->qsqldatabase);
		cout << "Check database table of detailed risk results for the ecological biotope-type elements..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_System::set_table_result_elem_eco_btype(&this->qsqldatabase);
		cout << "Check database table of detailed risk results for the ecological soil-erosion elements..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_System::set_table_result_elem_eco_soil(&this->qsqldatabase);
		cout << "Check database table of detailed risk results for the people2risk elements..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_System::set_table_result_elem_pop(&this->qsqldatabase);
		cout << "Check database table of detailed risk results for the psycho-social elements..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_System::set_table_result_elem_pys(&this->qsqldatabase);
		cout << "Check database table of detailed risk results for the simple counting points..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_System::set_table_result_point_sc(&this->qsqldatabase);

		cout << "Check database table of cumulated risk results..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_System::set_resulttable_cumulated(&this->qsqldatabase);
		cout << "Check database table of risk results..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_System::set_resulttable(&this->qsqldatabase);
		cout << "Check database table of break scenarios..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_Break_Scenario_Management::set_table(&this->qsqldatabase);

		cout << "Check database table of probabilities of the FPL-section(s)..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout,false, false);
		Risk_System::check_stop_thread_flag();
		Risk_Break_Scenario_Management::set_table_fpl_sec_prob(&this->qsqldatabase);
	}
	catch(Error msg){
		ostringstream info;
		info << "Check if the RISK tables are created!"<< endl;
		msg.make_second_info(info.str());
		msg.set_fatal_flag(false);
		msg.output_msg(5);
		this->output_error_statistic();
	}
	this->set_exception_number_end();
	cout << "Check of the RISK database tables is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout,false, false);
	

}
//Delete the data of all risk database tables
void Risk_System::delete_data_risk_database_tables(void){
	try{
		Risk_System::delete_data_in_resulttable(&this->qsqldatabase);
		Risk_System::delete_data_in_resulttable_detailed(&this->qsqldatabase);
		Risk_Damage_Predictor::delete_data_in_resulttable_predicted_detailed(&this->qsqldatabase);
		Risk_System::delete_data_in_resulttable_cumulated(&this->qsqldatabase);
		Risk_System::reset_data_in_table_config(&this->qsqldatabase);
		Risk_Break_Scenario_Management::delete_data_in_table(&this->qsqldatabase);
		Risk_Break_Scenario_Management::delete_data_in_table_fpl_sec_prob(&this->qsqldatabase);
		Risk_System::delete_data_in_table_result_elem_ecn(&this->qsqldatabase);
		Risk_System::delete_data_in_table_result_elem_eco_btype(&this->qsqldatabase);
		Risk_System::delete_data_in_table_result_elem_eco_soil(&this->qsqldatabase);
		Risk_System::delete_data_in_table_result_elem_pop(&this->qsqldatabase);
		Risk_System::delete_data_in_table_result_elem_pys(&this->qsqldatabase);
		Risk_System::delete_data_in_table_result_point_sc(&this->qsqldatabase);
	}
	catch(Error msg){
		msg.output_msg(5);
	}
}
//Close all risk database tables (static)
void Risk_System::close_risk_database_tables(void){
	Risk_System::close_resulttable();
	Risk_System::close_resulttable_detailed();
	Risk_Damage_Predictor::close_resulttable_predicted_detailed();
	Risk_System::close_resulttable_cumulated();
	Risk_System::close_table_config();
	Risk_Break_Scenario_Management::close_table();
	Risk_Break_Scenario_Management::close_table_fpl_sec_prob();

	Risk_System::close_table_result_elem_ecn();
	Risk_System::close_table_result_elem_eco_btype();
	Risk_System::close_table_result_elem_eco_soil();
	Risk_System::close_table_result_elem_pop();
	Risk_System::close_table_result_elem_pys();
	Risk_System::close_table_result_point_sc();
}
//Set the stop thread flag 
void Risk_System::set_stop_thread_flag(const bool flag){
	Risk_System::abort_thread_flag=flag;
	if(flag==true){
		//give the stop-command to the other threads
		if(this->hyd_calc!=NULL ){
			this->hyd_calc->set_stop_thread_flag(true);
		}
		if(this->dam_calc!=NULL){
			this->dam_calc->set_stop_thread_flag(true);
		}
		if(this->fpl_calc!=NULL && (this->fpl_calc->isRunning()==true || this->fpl_system_working==true)){
			this->fpl_calc->set_stop_thread_flag(true);
		}
		if(this->hyd_break_calc!=NULL && this->hyd_break_calc->get_hydraulic_break_thread_running()==true){
			Hyd_Multiple_Hydraulic_Systems::set_stop_thread_flag(true);
		}
		if(this->fpl_thread_is_used_extern==true){
			Fpl_Calculation::set_stop_thread_flag(true);
		}
	}
}
//Set just the risk stop thread flag (static)
void Risk_System::set_risk_stop_thread_flag(const bool flag){
	Risk_System::abort_thread_flag=flag;
}
//Check the stop thread flag (static)
 void Risk_System::check_stop_thread_flag(void){
	if(Risk_System::abort_thread_flag==true){
		Error msg;
		msg.set_msg(_sys_system_modules::RISK_SYS);
		throw msg;
	}
}
//Get the stop thread flag (static)
bool Risk_System::get_stop_thread_flag(void){
	return Risk_System::abort_thread_flag;
}
//Get the control parameters for the catchment area based risk calculation
_risk_ca_scenario_control Risk_System::get_ca_control_parameter(void){
	return this->control_params_ca;
}
//Get the number of the main outflow river
int Risk_System::get_number_main_outflow_river(void){
	return this->id_main_river;
}
//Set the risk state flag 
void Risk_System::set_risk_state_flag(const _risk_state_flags flags){
	this->risk_states=flags;
}
//Get the risk state flag 
_risk_state_flags Risk_System::get_risk_state_flag(void){
	return this->risk_states;
}
//Run the thread; the type of thread, which will be performed, has to be set before with set_thread_type(_risk_thread_type type)
void Risk_System::run(void){
/** \see set_thread_type(_risk_thread_type type), _risk_thread_type
*/
	this->thread_is_started=true;
	try{
		switch(this->thread_type){
			//database tables
			case(_risk_thread_type::risk_create_tab):
				this->create_risk_database_tables();
				break;
			case(_risk_thread_type::risk_check_tab):
				this->check_risk_database_tables();
				break;
			case(_risk_thread_type::risk_calc_nobreak):
				this->calculate_nobreak_risk();
				break;
			case(_risk_thread_type::risk_calc_scenario):
				this->calculate_scenario_based_risk();
				break;
			case(_risk_thread_type::risk_generate_catchment_state):
				this->generate_risk_states_catchment();
				break;
			case(_risk_thread_type::risk_calc_catchment_hand):
				this->calculate_catchment_risk_user_defined();
				break;
			case(_risk_thread_type::risk_calc_catchment_auto):
				this->calculate_catchment_risk_auto();
				break;
			case(_risk_thread_type::risk_calc_catchment_nobreak):
				this->calculate_catchment_risk_nobreak();
				break;
			case(_risk_thread_type::risk_calc_catchment_one_break):
				this->calculate_catchment_risk_one_break();
				break;
			case(_risk_thread_type::risk_calc_catchment_cumulated):
				this->cumulate_catchment_risk(false);
				break;
			case(_risk_thread_type::risk_del):
				this->delete_risk_data();
				break;
			case(_risk_thread_type::risk_del_all_result):
				this->delete_all_relevant_result();
				break;
			case(_risk_thread_type::risk_del_risk_result):
				this->delete_risk_result();
				break;
			case(_risk_thread_type::risk_output_stat_result):
				this->output_statistics_generated_detailed_results();
				break;
			case(_risk_thread_type::risk_catchment_statistic):
				this->calc_output_catchment_risk_statistic(false);
				break;
			case(_risk_thread_type::risk_calc_serial_steps):
				this->calc_steps_risk_analysis_serial();
				break;
				
			default:
				Warning msg=this->set_warning(0);
				msg.output_msg(5);
		}
	}
	catch(Error msg){
		this->thread_is_started=false;
		msg.output_msg(5);
	}
	this->thread_is_started=false;

}
//Get the number of errors
int Risk_System::get_number_error(void){
	return this->number_error.risk_error;
}
//Output the configuration members of the risk calculation to display/console
void Risk_System::output_members(void){
	ostringstream prefix;
	prefix<<"OUT> ";
	Sys_Common_Output::output_risk->set_userprefix(&prefix);
	ostringstream cout;
	cout << "RISK CONFIGURATION"<<endl;
	if(this->id_main_river>=0){
		cout << " Id of main river (outflow discharge) : " <<this->id_main_river<<endl;
	}
	else{
		cout << " Id of main river (outflow discharge) : " <<label::not_defined<<endl;
	}
	if(this->risk_type==_risk_type::catchment_risk){
		cout << "CATCHMENT AREA APPROACH"<<endl;
		cout << " Maximum number hydraulic calculation : "<<this->control_params_ca.max_hyd_calc<<endl;
		cout << " Boundary of probability              : " <<this->control_params_ca.prob_bound<< endl;
		cout << " Number of MC-runs                    : " <<this->control_params_ca.number_mc<< endl;
		cout << " Generator type                       : " <<Fpl_Mc_Sim::convert_generator_type2text(this->control_params_ca.gen_type)<< endl;
		cout << " Number of FRC-intervals              : " <<this->control_params_ca.number_intervals<< endl;
		cout << " Use predicted values                 : " <<functions::convert_boolean2string(this->control_params_ca.use_predict_values)<< endl;
	}
	Sys_Common_Output::output_risk->output_txt(&cout);
	Sys_Common_Output::output_risk->rewind_userprefix();
}
//Set the system-id (_sys_system_id) of the object
void Risk_System::set_systemid(const _sys_system_id actuel_id){
	_Sys_Common_System::set_systemid(actuel_id);
	this->break_sc_manager.set_systemid(actuel_id);
}
//Set this thread to sleeping-modus
void Risk_System::set_thread2sleep(const int sec){
	emit send_risk_thread_sleeps(true);
	this->sleep(sec);
	emit send_risk_thread_sleeps(false);
}
//Get the flag if the thread has started
bool Risk_System::get_thread_has_started(void){
	return this->thread_is_started;
}
//Cumulate the generate catchment area risk states to one risk result
void Risk_System::cumulate_catchment_risk(const bool internaly){
	if(internaly==false){
		this->risk_type=_risk_type::catchment_risk;
		this->break_sc_manager.set_risk_type(this->risk_type);
		this->set_exception_number_start();
		//begin time recording	
		time(&this->start_time);
	}


	ostringstream prefix;
	ostringstream cout;



	try{

		//delete results
		Risk_System::delete_data_in_resulttable_detailed(&this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		Risk_System::delete_data_in_resulttable_cumulated(&this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		Risk_System::delete_data_in_resulttable(&this->qsqldatabase, this->system_id, _risk_type::catchment_risk);

		//set prefix
		prefix << "CA> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);
		//set the list for calculation
		this->break_sc_manager.set_total_sc2calc_catchment_risk(&this->qsqldatabase);
		this->check_calc_dam_sc();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();


		if(internaly==false){
			this->set_input(&this->qsqldatabase);
			this->output_members();
		}

		if(internaly==true){
			//update the probabilities
			try{
				this->break_sc_manager.update_scenario_probability(&this->qsqldatabase, this->control_params_ca);
			}
			catch(Error msg){
				throw msg;
			}
			
			//update the risk prediction
			try{
				this->break_sc_manager.predict_detailed_risk_results(&this->qsqldatabase, this->control_params_ca);
			}
			catch(Error msg){
				throw msg;
			}
		}
		

		//output
		cout << "Calculate and cumulate the catchment area risk..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

			try{
				prefix << "CALC> ";
				Sys_Common_Output::output_risk->set_userprefix(&prefix);
				//risk calculation
				Risk_System::check_stop_thread_flag();
				this->set_connect_dam_prob2detailed_result_break();
				Risk_System::check_stop_thread_flag();
				this->calculate_detailed_result();
				Risk_System::check_stop_thread_flag();
				this->calculate_cumulated_result();
				Risk_System::check_stop_thread_flag();
				this->calculate_total_result();
				Sys_Common_Output::output_risk->reset_prefix_was_outputed();
				Sys_Common_Output::output_risk->rewind_userprefix();
			}
			catch(Error msg){
				Sys_Common_Output::output_risk->rewind_userprefix();
				throw msg;
			}
			//output
			try{
				prefix << "OUT> ";
				Sys_Common_Output::output_risk->set_userprefix(&prefix);
				this->output_detailed_risk_results();
				Risk_System::check_stop_thread_flag();
				this->output_detailed_risk_results2database();
				Risk_System::check_stop_thread_flag();
				this->output_cumulated_risk_results();
				Risk_System::check_stop_thread_flag();
				this->output_cumulated_risk_results2database();
				Risk_System::check_stop_thread_flag();
				this->output_total_results();
				Risk_System::check_stop_thread_flag();
				this->output_total_result2database();
				Sys_Common_Output::output_risk->rewind_userprefix();
			}
			catch(Error msg){
				Sys_Common_Output::output_risk->rewind_userprefix();
				throw msg;
			}


			Sys_Common_Output::output_risk->rewind_userprefix();


			this->calc_detailed_result_element();



			this->calc_output_catchment_risk_statistic(internaly);

	}
	catch(Error msg){
		if(internaly==false){
			msg.output_msg(5);
		}
		else{
			throw msg;
		}
	}
	cout << "Calculation and cumulation of the catchment area risk is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);

	
	

	if(internaly==false){
		this->set_exception_number_end();
		//set the actual time
		time(&this->actual_time);
		this->output_final_statistic_calculation();
		
	}
}
//Check if the probability boundary is reached (catchement area approach)
bool Risk_System::check_probability_boundary_is_reached(void){
	this->set_input(&this->qsqldatabase, false);

	Risk_Catchment_Break_Scenario_List buffer;
	double percent=0.0;
	percent=buffer.get_prob_scenario_calculated(&this->qsqldatabase,this->system_id);

	if(percent>=this->control_params_ca.prob_bound){
		return true;
	}
	else{
		return false;
	}

}
//Set the flag if also the HYD-, DAM- and FPL-resuts should be deleted from database
void Risk_System::set_del_hyd_dam_fpl_flag(const bool del_flag) {
	this->hyd_dam_fpl_del_flag = del_flag;
}
//___________
//public slots
//Send the status of the fpl-thread
void Risk_System::send_status_fpl_thread(void){
	if(this->fpl_calc!=NULL && (this->fpl_calc->isRunning() || this->fpl_system_working==true)){
		emit send_fpl_thread_runs(true);
	}
	else{
		emit send_fpl_thread_runs(false);
	}
}
//Send the status of the fpl-thread, if it is used in the hydraulic break calculation
void Risk_System::sent_status_fpl_thread_break_calc(bool flag){
		this->fpl_thread_is_used_extern=flag;
		emit send_fpl_thread_runs(flag);
}
//Send the status of the hyd-thread, if it is used in the hydraulic break calculation
void Risk_System::sent_status_hyd_thread_break_calc(bool flag){
		emit send_hyd_thread_runs(flag);
}
//Send the status of the hyd-thread, if it is used in the hydraulic break calculation
void Risk_System::sent_number_hyd_thread_break_calc(QString number){
	emit emit_number_threads(number);
}
//Recieve and output the number of hydraulic calculation, which are already done from the hydraulic thread (just nobreak risk approach)
void Risk_System::recieve_output_hyd_calc_done(QString done){
	ostringstream prefix;
	ostringstream cout;
	prefix<<"STATUS> ";
	Sys_Common_Output::output_risk->set_userprefix(&prefix);
	cout <<done.toStdString() <<" hydraulic scenario(s) calculated"<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	Sys_Common_Output::output_risk->rewind_userprefix();
}
//Set the risk thread to sleeping-status
void Risk_System::set_status_risk_thread(bool flag){
	emit send_risk_thread_sleeps(flag);
}
//Set the hydraulic thread to sleeping-status
void Risk_System::set_status_hydraulic_thread(bool flag){
	emit send_hydraulic_thread_sleeps(flag);
}
//_______
//private slots
//Manage the output of the hydraulic classes
void Risk_System::recieve_output_required(int thread){
	if(this->hyd_break_calc!=NULL){
		this->hyd_break_calc->recieve_output_required(thread);
	}
	if(this->hyd_calc!=NULL){
		this->hyd_calc->recieve_output_required(thread);
	}
}
//Reconnect the hydraulic thread of the HYD-system for the output requirement
void Risk_System::reconnect_hyd_threads(void){

	if(this->hyd_calc!=NULL){
		for(int i=0; i< this->hyd_calc->get_number_threads(); i++){
			if(this->hyd_calc->get_ptr_hyd_threads(i)!=NULL){
				QObject::disconnect(this->hyd_calc->get_ptr_hyd_threads(i), SIGNAL(output_required(int)), this->hyd_calc, SLOT(recieve_output_required(int )));
				QObject::connect(this->hyd_calc->get_ptr_hyd_threads(i), SIGNAL(output_required(int)), this, SLOT(recieve_output_required(int )));

			}
		}
	}
}
//____________
//private
//Calculate nobreak risk 
void Risk_System::calculate_nobreak_risk(void){
	this->risk_type=_risk_type::nobreak_risk;
	this->set_exception_number_start();

	try{
		//set risk members
		this->set_input(&this->qsqldatabase);
		this->output_members();
	}
	catch(Error msg){
		msg.output_msg(5);

	}


	//begin time recording	
	time(&this->start_time);
	ostringstream prefix;
	ostringstream cout;


	
	try{
		//delete results
		Risk_System::delete_data_in_resulttable_detailed(&this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		Risk_System::delete_data_in_resulttable(&this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		Risk_System::delete_data_in_resulttable_cumulated(&this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		//set prefix
		prefix << "CA_NOB> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);

		//output
		cout << "Calculate the nobreak risk..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		//hydraulic
		Risk_System::check_stop_thread_flag();
		this->check_calc_hyd_nobreak_sc();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();
		//damage
		Risk_System::check_stop_thread_flag();
		this->check_calc_dam_nobreak_sc();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();
		try{
			prefix << "CALC> ";
			Sys_Common_Output::output_risk->set_userprefix(&prefix);
			//risk calculation
			Risk_System::check_stop_thread_flag();
			this->set_connect_dam_prob2detailed_result_nobreak();
			Risk_System::check_stop_thread_flag();
			this->calculate_detailed_result();
			Risk_System::check_stop_thread_flag();
			this->calculate_cumulated_result();
			Risk_System::check_stop_thread_flag();
			this->calculate_total_result();
			Sys_Common_Output::output_risk->reset_prefix_was_outputed();
			Sys_Common_Output::output_risk->rewind_userprefix();
		}
		catch(Error msg){
			Sys_Common_Output::output_risk->rewind_userprefix();
			throw msg;
		}
		
		//output
		try{
			prefix << "OUT> ";
			Sys_Common_Output::output_risk->set_userprefix(&prefix);
			this->output_detailed_risk_results();
			Risk_System::check_stop_thread_flag();
			this->output_detailed_risk_results2database();
			Risk_System::check_stop_thread_flag();
			this->output_cumulated_risk_results();
			Risk_System::check_stop_thread_flag();
			this->output_cumulated_risk_results2database();
			Risk_System::check_stop_thread_flag();
			this->output_total_results();
			Risk_System::check_stop_thread_flag();
			this->output_total_result2database();
			Sys_Common_Output::output_risk->rewind_userprefix();

			this->calc_detailed_result_element();
		}
		catch(Error msg){
			Sys_Common_Output::output_risk->rewind_userprefix();
			throw msg;
		}
		

	}
	catch(Error msg){
		msg.output_msg(5);

	}
	cout << "Calculation of the nobreak risk is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);

	Sys_Common_Output::output_risk->rewind_userprefix();
	this->set_exception_number_end();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_calculation();


}
//Calculate scenario based risk
void Risk_System::calculate_scenario_based_risk(void){
	this->risk_type=_risk_type::scenario_risk;
	this->break_sc_manager.set_risk_type(this->risk_type);
	this->set_exception_number_start();

	try{
		//set risk members
		this->set_input(&this->qsqldatabase);
		this->output_members();
	}
	catch(Error msg){
		msg.output_msg(5);

	}

	//begin time recording	
	time(&this->start_time);
	ostringstream prefix;
	ostringstream cout;


	if(this->break_sc_manager.get_number_sc2calc()==0){	
		cout << "No risk break scenario is set" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		return;
	}
	Risk_Break_Scenario buffer;
	
	buffer=*this->break_sc_manager.get_ptr_sc2calc(0);
	
	if(buffer.get_number_involved_sections()==0){	
		cout << "No FPL-section is set for a breaking" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		return;
	}
	if(buffer.get_is_calculated()==true){
		cout << "This selected risk break scenario is already calculated" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		return;
	}

	try{

		//delete results
		//Risk_System::delete_data_in_resulttable_detailed(&this->qsqldatabase, this->system_id, _risk_type::scenario_risk, buffer.get_break_scenario_string(), buffer.get_hydraulic_boundary_scenario().get_id());
		
		//set prefix
		prefix << "SC> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);
		//output
		cout << "Calculate the scenario based risk for HYD-boundary scenario "<< buffer.get_hydraulic_boundary_scenario().get_name();
		cout <<" with a break in FPL-section "<< (buffer.get_involved_section(0).id_fpl_sec) <<" with break height "<< buffer.get_involved_section(0).id_interval<<label::m<< " ("<<buffer.get_break_scenario_string()<<")..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

		//load the hydraulic threads and idealize
		this->allocate_hyd_break_calculation();
		cout << "Load the the HYD break calculation thread and idealise the HYD-system with the selected FPL-section..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		this->hyd_break_calc->alloc_load_hyd_threads();

		Risk_System::check_stop_thread_flag();

		//start calculation fpl
		this->allocate_fpl_system();
		this->fpl_calc->set_list_section_ids(buffer.get_number_involved_sections(), buffer.get_ptr_involved_sections());
		this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_mc_selected);
		this->fpl_calc->set_overwrite_flag(false);
		cout << "Start the FPL calculation. Further information see FPL display..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		this->fpl_calc->start();
		this->send_status_fpl_thread();

		Risk_System::check_stop_thread_flag();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();

		//start hydraulic calculation
		cout << "Start the HYD break calculation. Further information see HYD display..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Sys_Common_Output::output_hyd->set_focus_on_display();
		this->hyd_break_calc->calculate_hyd_threads();


		//waiting for the threads
		this->wait_loop();
		Sys_Common_Output::output_risk->set_focus_on_display();
		cout << "FPL- and HYD break calculation are finished" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		this->delete_fpl_system();
		
		bool result_ok=this->hyd_break_calc->get_breach_has_started();

		this->delete_hyd_break_calculation();
		Risk_System::check_stop_thread_flag();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();

		if(result_ok==false){
			cout << "No breach development is launched for the selected scenario based hydraulic risk calulation" << endl ;
			cout << "A scenario base risk calculation is not possible! The hydraulic results are deleted from database table."<<endl;
			cout << "See the warning for further information."<<endl;
			_Hyd_River_Profile::delete_results_in_table(&this->qsqldatabase, this->system_id, buffer.get_hydraulic_boundary_scenario().get_id(), buffer.get_break_scenario_string());
			_Hyd_River_Profile::delete_instat_results_in_table(&this->qsqldatabase, this->system_id, buffer.get_hydraulic_boundary_scenario().get_id(), buffer.get_break_scenario_string());
			Hyd_Element_Floodplain::delete_results_in_table(&this->qsqldatabase, this->system_id,buffer.get_hydraulic_boundary_scenario().get_id(), buffer.get_break_scenario_string());
			Sys_Common_Output::output_risk->output_txt(&cout);
		}
		else{
			//probability to the probability table
			cout << "Calculate and set the break scenario probability to database table..." << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			this->break_sc_manager.set_scenario_prob2table(&this->qsqldatabase);
			Sys_Common_Output::output_risk->reset_prefix_was_outputed();

			//damages
			Risk_System::check_stop_thread_flag();
			this->check_calc_dam_sc();
			Sys_Common_Output::output_risk->reset_prefix_was_outputed();

			//risk
			try{
				prefix << "CALC> ";
				Sys_Common_Output::output_risk->set_userprefix(&prefix);
				Risk_System::check_stop_thread_flag();
				this->set_connect_dam_prob2detailed_result_break();
				Risk_System::check_stop_thread_flag();
				this->calculate_detailed_result();
				Sys_Common_Output::output_risk->reset_prefix_was_outputed();
			}
			catch(Error msg){
				Sys_Common_Output::output_risk->rewind_userprefix();
				throw msg;
			}
			Sys_Common_Output::output_risk->rewind_userprefix();

			//output
			try{
				prefix << "OUT> ";
				Sys_Common_Output::output_risk->set_userprefix(&prefix);
				this->output_detailed_risk_results();
				Risk_System::check_stop_thread_flag();
				this->output_detailed_risk_results2database();
				Sys_Common_Output::output_risk->rewind_userprefix();

				this->calc_detailed_result_element();


			}
			catch(Error msg){
				Sys_Common_Output::output_risk->rewind_userprefix();
				throw msg;
			}
		
			
		}

	}
	catch(Error msg){
		msg.output_msg(5);

	}
	cout << "Calculation of the scenario based risk is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);

	Sys_Common_Output::output_risk->rewind_userprefix();
	this->set_exception_number_end();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_calculation();


}
//Generate the risk states for the catchment area risk approach via a Monte Carlo analysis
void Risk_System::generate_risk_states_catchment(void){
	this->risk_type=_risk_type::catchment_risk;
	this->break_sc_manager.set_risk_type(this->risk_type);

	ostringstream cout;

	ostringstream prefix;
	prefix << "CA_SC_GEN> ";
	Sys_Common_Output::output_risk->set_userprefix(&prefix);

	if(this->break_sc_manager.hyd_bound_manager.counter_number_selected_scenarios()==0){
		if(this->thread_type==_risk_thread_type::risk_calc_serial_steps){
			cout << "For all hydraulic boundary scenario(s) the list of the break scenarios are generated!" << endl ;

		}
		else{
			cout << "No hydraulic boundary scenarios are selected for the generation of the break scenarios!" << endl ;
		}
		Sys_Common_Output::output_risk->output_txt(&cout);	
		Sys_Common_Output::output_risk->rewind_userprefix();
		return;
	}


	this->set_exception_number_start();

	//begin time recording	
	time(&this->start_time);
	
	

	try{

		//set prefix
		this->set_input(&this->qsqldatabase);
		this->output_members();
		//start calculation fpl
		this->allocate_fpl_system();
		this->fpl_calc->set_list_section_ids();
		this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_frc_selected);
		this->fpl_calc->set_overwrite_flag(false);
		cout << "Start the FPL calculation for " <<this->fpl_calc->get_number_section_in_list() <<" FPL-section(s). Further information see FPL display..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Sys_Common_Output::output_fpl->set_focus_on_display();
		this->fpl_system_working=true;
		this->send_status_fpl_thread();
		this->fpl_calc->run();
		this->delete_fpl_system();

		Risk_System::check_stop_thread_flag();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();
		Sys_Common_Output::output_risk->set_focus_on_display();

		//output
		cout << "Generate the break scenarios for the selected hydraulic boundary scenarios..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

		this->break_sc_manager.make_list_sc_via_monte_carlo(&this->qsqldatabase, this->control_params_ca);


	}
	catch(Error msg){
		Sys_Common_Output::output_risk->rewind_userprefix();
		msg.output_msg(5);

	}
	cout << "Generation of the break scenarios for the selected hydraulic boundary scenarios is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);

	Sys_Common_Output::output_risk->rewind_userprefix();
	this->set_exception_number_end();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_calculation();
}
//Calculate catchment area risk for nobreak state (detailed risk result)
void Risk_System::calculate_catchment_risk_nobreak(void){
	this->risk_type=_risk_type::catchment_risk;
	this->break_sc_manager.set_risk_type(this->risk_type);
	this->set_exception_number_start();

	//begin time recording	
	time(&this->start_time);
	ostringstream prefix;
	ostringstream cout;


	try{
		//set prefix
		prefix << "CA_NOBREAK> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);
		//generate list of nobreak risk states
		this->break_sc_manager.set_nobreak_risk_state_list(&this->qsqldatabase);
		
		if(this->break_sc_manager.get_number_sc2calc()==0){	
			cout << "All nobreak risk states for the catchment area risk approach are already calculated" << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			Sys_Common_Output::output_risk->rewind_userprefix();
			return;
		}

		//output
		cout << "Calculate "<<this->break_sc_manager.get_number_sc2calc()<<" nobreak risk states of the HYD-boundary scenario for the catchment area risk approach"<< endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

		//load the hydraulic threads and idealize
		this->allocate_hyd_break_calculation();
		cout << "Load the HYD-calculation thread and idealise the HYD-system with the all relevant FPL-section(s)..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		this->hyd_break_calc->alloc_load_hyd_threads();


		//start hydraulic calculation
		cout << "Start the HYD-calculation for the nobreak risk states. Further information see HYD display..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Sys_Common_Output::output_hyd->set_focus_on_display();
		this->emit_stop_next_hyd_calc(true);
		this->hyd_break_calc->calculate_hyd_threads();
		this->emit_stop_next_hyd_calc(false);


		//waiting for the threads
		this->wait_loop();
		Sys_Common_Output::output_risk->set_focus_on_display();
		cout << "HYD-calculation is finished" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

		this->delete_hyd_break_calculation();
		Risk_System::check_stop_thread_flag();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();


		//damages
		Risk_System::check_stop_thread_flag();
		this->check_calc_dam_sc();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();

		Risk_System::check_stop_thread_flag();

	}
	catch(Error msg){
		msg.output_msg(5);

	}
	cout << "Calculation of the nobreak risk states for the catchment area risk approach is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);

	Sys_Common_Output::output_risk->rewind_userprefix();
	this->set_exception_number_end();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_calculation();

}
//Calculate catchment area risk for one break states (detailed risk result)
void Risk_System::calculate_catchment_risk_one_break(void){
	this->risk_type=_risk_type::catchment_risk;
	this->break_sc_manager.set_risk_type(this->risk_type);
	this->set_exception_number_start();

	//begin time recording	
	time(&this->start_time);
	ostringstream prefix;
	ostringstream cout;

	try{
		//set prefix
		prefix << "CA_ONEBREAK> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);
		//generate list of onebreak risk states
		this->break_sc_manager.set_one_break_risk_state_list(&this->qsqldatabase);

		
		if(this->break_sc_manager.get_number_sc2calc()==0){	
			cout << "All one-break risk states for the catchment area risk approach are already calculated" << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			Sys_Common_Output::output_risk->rewind_userprefix();
			return;
		}
	
		this->set_input(&this->qsqldatabase);
		this->output_members();

		//output
		cout << "Calculate "<<this->break_sc_manager.get_number_sc2calc()<<" one-break risk states of the HYD-boundary scenario for the catchment area risk approach"<< endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

		//load the hydraulic threads and idealize
		this->allocate_hyd_break_calculation();
		cout << "Load the HYD-calculation thread and idealise the HYD-system with the all relevant FPL-section(s)..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		this->hyd_break_calc->alloc_load_hyd_threads();

		Risk_System::check_stop_thread_flag();
		//start hydraulic calculation
		cout << "Start the HYD-calculation for the one-break risk states. Further information see HYD display..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Sys_Common_Output::output_hyd->set_focus_on_display();
		this->emit_stop_next_hyd_calc(true);
		this->hyd_break_calc->calculate_hyd_threads();
		this->emit_stop_next_hyd_calc(false);


		//waiting for the threads
		this->wait_loop();
		Sys_Common_Output::output_risk->set_focus_on_display();
		cout << "HYD-calculation is finished" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

		this->delete_hyd_break_calculation();
		Risk_System::check_stop_thread_flag();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();

		Risk_System::check_stop_thread_flag();

		Sys_Common_Output::output_risk->rewind_userprefix();


		this->cumulate_catchment_risk(true);
		Risk_System::check_stop_thread_flag();
		cout << "Calculation of the one-break risk states for the catchment area risk approach is finished" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

	}
	catch(Error msg){
		msg.output_msg(5);
	}

	this->set_exception_number_end();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_calculation();

}
//Calculate catchment area risk user defind scenarios (detailed risk result) 
void Risk_System::calculate_catchment_risk_user_defined(void){
	this->risk_type=_risk_type::catchment_risk;
	this->break_sc_manager.set_risk_type(this->risk_type);
	this->set_exception_number_start();

	//begin time recording	
	time(&this->start_time);
	ostringstream prefix;
	ostringstream cout;

	if(this->break_sc_manager.get_number_sc2calc()==0){	
		cout << "No user-defined risk states for calculation are set" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		return;
	}
	
	try{
		//set prefix
		prefix << "CA_USER> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);
		this->set_input(&this->qsqldatabase);
		this->output_members();
		//output
		cout << "Calculate "<<this->break_sc_manager.get_number_sc2calc()<<" user-defined risk state(s) of the HYD-boundary scenario for the catchment area risk approach"<< endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

		//load the hydraulic threads and idealize
		this->allocate_hyd_break_calculation();
		cout << "Load the the HYD-calculation thread and idealise the HYD-system with the all relevant FPL-section(s)..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		this->hyd_break_calc->alloc_load_hyd_threads();

		Risk_System::check_stop_thread_flag();
		//start hydraulic calculation
		cout << "Start the HYD-calculation for the user-defined risk states. Further information see HYD display..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Sys_Common_Output::output_hyd->set_focus_on_display();
		this->emit_stop_next_hyd_calc(true);
		this->hyd_break_calc->calculate_hyd_threads();
		this->emit_stop_next_hyd_calc(false);


		//waiting for the threads
		this->wait_loop();
		Sys_Common_Output::output_risk->set_focus_on_display();
		cout << "HYD-calculation is finished" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		this->delete_hyd_break_calculation();
		Risk_System::check_stop_thread_flag();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();

		
		Risk_System::check_stop_thread_flag();
		Sys_Common_Output::output_risk->rewind_userprefix();

		this->cumulate_catchment_risk(true);
		Risk_System::check_stop_thread_flag();
		cout << "Calculation of the user-defined risk state for the catchment area risk approach is finished" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

	
	}
	catch(Error msg){
		msg.output_msg(5);

	}

	this->set_exception_number_end();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_calculation();

}
//Calculate catchment area risk automatically (detailed risk result)
void Risk_System::calculate_catchment_risk_auto(void){
	this->risk_type=_risk_type::catchment_risk;
	this->break_sc_manager.set_risk_type(this->risk_type);
	this->set_exception_number_start();
	//begin time recording	
	time(&this->start_time);
	ostringstream prefix;
	ostringstream cout;

	
	try{
		//set prefix
		prefix << "CA_AUTO> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);
		this->output_members();

		//load list
		this->break_sc_manager.set_auto_break_risk_state_list(&this->qsqldatabase, this->control_params_ca);
		if(this->break_sc_manager.get_number_sc2calc()==0){	
			cout << "No risk states for automatic calculation are set. Probability boundary of " <<this->control_params_ca.prob_bound*100.0<<label::percentage<<"is already reached!"<< endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			Sys_Common_Output::output_risk->rewind_userprefix();
			this->calc_output_catchment_risk_statistic(true);
			return;
		}

		//output
		cout << "Calculate "<<this->break_sc_manager.get_number_sc2calc()<<" risk state(s) of the HYD-boundary scenario for the catchment area risk approach"<< endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

		//load the hydraulic threads and idealize
		this->allocate_hyd_break_calculation();
		cout << "Load the the HYD-calculation thread and idealise the HYD-system with the all relevant FPL-section(s)..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		this->hyd_break_calc->alloc_load_hyd_threads();

		Risk_System::check_stop_thread_flag();
		//start hydraulic calculation
		cout << "Start the HYD-calculation for the automatic risk state calculation. Further information see HYD display..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Sys_Common_Output::output_hyd->set_focus_on_display();
		this->emit_stop_next_hyd_calc(true);
		this->hyd_break_calc->calculate_hyd_threads();
		this->emit_stop_next_hyd_calc(false);


		//waiting for the threads
		this->wait_loop();
		Sys_Common_Output::output_risk->set_focus_on_display();
		cout << "HYD-calculation is finished" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		this->delete_hyd_break_calculation();
		Risk_System::check_stop_thread_flag();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();

	
		Risk_System::check_stop_thread_flag();

		Sys_Common_Output::output_risk->rewind_userprefix();

		this->cumulate_catchment_risk(true);
		Risk_System::check_stop_thread_flag();
		cout << "Automatic calculation of the risk states for the catchment area risk approach is finished" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

	}
	catch(Error msg){
		msg.output_msg(5);

	}

	this->set_exception_number_end();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_calculation();
}
//Calculate and output the statistics of the catchment risk approach
void Risk_System::calc_output_catchment_risk_statistic(const bool internaly){
	if(internaly==false){
		this->risk_type=_risk_type::catchment_risk;
		this->break_sc_manager.set_risk_type(this->risk_type);
	}
	ostringstream prefix;


	try{
		//set prefix
		prefix << "CA> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);
		if(internaly==false){
			this->set_input(&this->qsqldatabase);
			this->output_members();
		}
		this->break_sc_manager.calc_output_catchment_area_risk_statistic(&this->qsqldatabase, this->control_params_ca);
		if(internaly==true){
			this->calculate_predicted_risk_statistic(true);
		}
		Sys_Common_Output::output_risk->rewind_userprefix();
	}
	catch(Error msg){
		Sys_Common_Output::output_risk->rewind_userprefix();
		if(internaly==false){
			msg.output_msg(5);	
		}
		else{
			throw msg;
		}
	}
}
//Calculate different steps of the risk analysis serial. The steps are ask via a dialog.
void Risk_System::calc_steps_risk_analysis_serial(void){

	ostringstream prefix;
	ostringstream cout;
	

	this->risk_type=_risk_type::catchment_risk;
	this->break_sc_manager.hyd_bound_manager.set_szenario_per_db(&this->qsqldatabase);
	this->break_sc_manager.hyd_bound_manager.set_all_sc_bound2selected();

	try{
		//set prefix
		cout <<"Calculate the catchment area risk in serial steps..."<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		prefix << "SERIAL> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);

		//nobreak
		if(this->calc_ca_nobreak_step==true){
		
			this->calculate_catchment_risk_nobreak();
		}
		Risk_System::check_stop_thread_flag();
		//list generation
		if(this->number_error.total_error==0 && this->calc_ca_sc_lists_step==true){
			QSqlQueryModel query;
	
			for(int i=0; i<this->break_sc_manager.hyd_bound_manager.get_number_sz(); i++){ 
				if(Risk_Break_Scenario_Management::select_data_in_table(&query, &this->qsqldatabase, this->risk_type, this->break_sc_manager.hyd_bound_manager.get_ptr_sz(i)->get_id(), this->system_id)!=0){
					this->break_sc_manager.hyd_bound_manager.get_ptr_sz(i)->set_is_selected(false);
				}
			}
		
			this->generate_risk_states_catchment();
			
		}
		this->break_sc_manager.hyd_bound_manager.set_all_sc_bound2selected();
		Risk_System::check_stop_thread_flag();
		//one break
		if(this->number_error.total_error==0 && this->calc_ca_one_break_step==true){
			
			this->calculate_catchment_risk_one_break();
		}
		Risk_System::check_stop_thread_flag();
		//auto calculation
		if(this->number_error.total_error==0 && this->calc_ca_auto_step==true){
			this->calculate_catchment_risk_auto();
		}
		Risk_System::check_stop_thread_flag();

	}
	catch(Error msg){
		msg.output_msg(5);
	}

	cout <<"Serial calculation of the catchment area risk is finished"<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);


}
//Output detailed risk results to database table
void Risk_System::output_detailed_risk_results2database(void){

	ostringstream cout;
	cout << "Output the detailed risk results to database..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);

	//mysql query with the query-class
	QSqlQuery model(this->qsqldatabase);
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_resulttable_detailed(&this->qsqldatabase);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Risk_System::table_detailed_results->maximum_int_of_column(Risk_System::table_detailed_results->get_column_name(label::glob_id),&this->qsqldatabase)+1;
		
	for(int i=0; i< this->number_detailed_results; i++){
		//set the query via a query string
		ostringstream query_string;
		query_string << "INSERT INTO  " << Risk_System::table_detailed_results->get_table_name();
		query_string <<" ( ";
		query_string << Risk_System::table_detailed_results->get_column_name(label::glob_id) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(label::areastate_id) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(label::measure_id) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(hyd_label::sz_bound_id) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(hyd_label::sz_bound_name) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(risk_label::sz_break_id) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(risk_label::risk_type) <<" , ";
		//results
		query_string << Risk_System::table_detailed_results->get_column_name(risk_label::max_outflow) <<" , ";
		//ecn
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::immob_dam) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::mob_dam) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::immob_5_quant) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::immob_95_quant) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::mob_5_quant) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::mob_95_quant) <<" , ";
		//eco
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::btype_cost) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::soil_cost) <<" , ";
		//pop
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::pop_affected) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::pop_endangered) <<" , ";
		//pys
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::crit_age_50_59) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::crit_age_80) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::crit_female) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::crit_owner) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_age50_59) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_age_80) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_female) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_owner) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::sc_pub_build) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::sc_eco_build) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::sc_cult_build) <<" , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::sc_person_build) <<" , ";


		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_elect_pt) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_info_tec_pt) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_water_sup_pt) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_water_treat_pt) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_energy_pt) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_health_pt) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_social_pt) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_mat_pt) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_elect_p) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_info_tec_p) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_water_sup_p) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_water_treat_p) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_energy_p) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_health_p) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_social_p) << " , ";
		query_string << Risk_System::table_detailed_results->get_column_name(dam_label::ci_mat_p) << " ) ";

		query_string << " VALUES ( ";
		query_string << id_glob << " , " ;
		query_string << this->system_id.area_state << " , " ;
		query_string << this->system_id.measure_nr << " , " ;
		query_string << this->detailed_results[i].dam_results.id_hyd_bound_sc << " , " ;
		query_string << "'"<<this->detailed_results[i].dam_results.hyd_bound_sc_name<< "' , " ;
		query_string << "'"<<this->detailed_results[i].dam_results.id_break_sc<< "' , " ;
		query_string << "'"<<Risk_Break_Scenario::convert_risk_type2txt(this->risk_type)<< "' , " ;
		//results
		//max outflow
		query_string << this->detailed_results[i].max_outflow << " , " ;
		//ecn
		query_string << this->detailed_results[i].dam_results.ecn_immob_dam << " , " ;
		query_string << this->detailed_results[i].dam_results.ecn_mob_dam  << " , " ;
		query_string << this->detailed_results[i].dam_results.ecn_immob_dam_5 << " , " ;
		query_string << this->detailed_results[i].dam_results.ecn_immob_dam_95 << " , " ;
		query_string << this->detailed_results[i].dam_results.ecn_mob_dam_5  << " , " ;
		query_string << this->detailed_results[i].dam_results.ecn_mob_dam_95 << " , " ;
		//eco
		query_string <<  this->detailed_results[i].dam_results.eco_biotype << " , " ;
		query_string << this->detailed_results[i].dam_results.eco_soil_erosion << " , " ;
		//pop
		query_string << this->detailed_results[i].dam_results.pop_affected << " , " ;
		query_string << this->detailed_results[i].dam_results.pop_endangered << " , " ;
		//pys
		query_string << this->detailed_results[i].dam_results.pys_age_50_59 << " , " ;
		query_string << this->detailed_results[i].dam_results.pys_age_80 << " , " ;
		query_string << this->detailed_results[i].dam_results.pys_female << " , " ;
		query_string << this->detailed_results[i].dam_results.pys_owner << " , " ;
		query_string << this->detailed_results[i].dam_results.pys_age_50_59_dens << " , " ;
		query_string << this->detailed_results[i].dam_results.pys_age_80_dens << " , " ;
		query_string << this->detailed_results[i].dam_results.pys_female_dens << " , " ;
		query_string << this->detailed_results[i].dam_results.pys_owner_dens << " , " ;
		query_string << this->detailed_results[i].dam_results.sc_pub_build << " , " ;
		query_string << this->detailed_results[i].dam_results.sc_eco_build << " , " ;
		query_string << this->detailed_results[i].dam_results.sc_cult_build << " , " ;
		query_string << this->detailed_results[i].dam_results.sc_person_build << " , " ;

		query_string << this->detailed_results[i].dam_results.ci_elect_pt << " , ";
		query_string << this->detailed_results[i].dam_results.ci_info_tec_pt << " , ";
		query_string << this->detailed_results[i].dam_results.ci_water_sup_pt << " , ";
		query_string << this->detailed_results[i].dam_results.ci_water_treat_pt << " , ";
		query_string << this->detailed_results[i].dam_results.ci_energy_pt << " , ";
		query_string << this->detailed_results[i].dam_results.ci_health_pt << " , ";
		query_string << this->detailed_results[i].dam_results.ci_social_pt << " , ";
		query_string << this->detailed_results[i].dam_results.ci_mat_pt << " , ";
		query_string << this->detailed_results[i].dam_results.ci_elect_p << " , ";
		query_string << this->detailed_results[i].dam_results.ci_info_tec_p << " , ";
		query_string << this->detailed_results[i].dam_results.ci_water_sup_p << " , ";
		query_string << this->detailed_results[i].dam_results.ci_water_treat_p << " , ";
		query_string << this->detailed_results[i].dam_results.ci_energy_p << " , ";
		query_string << this->detailed_results[i].dam_results.ci_health_p << " , ";
		query_string << this->detailed_results[i].dam_results.ci_social_p << " , ";
		query_string << this->detailed_results[i].dam_results.ci_mat_p << " ) ";

		Data_Base::database_request(&model, query_string.str(), &this->qsqldatabase);

		
		if(model.lastError().isValid()){
			Warning msg=this->set_warning(7);
			ostringstream info;
			info << "Table Name                : " << Risk_System::table_detailed_results->get_table_name() << endl;
			info << "Table error info          : " << model.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(5);	
		}
		else{
			id_glob++;
		}
	}
}
//Output detailed risk results to display/console
void Risk_System::output_detailed_risk_results(void){
	ostringstream prefix;
	string unit;
	bool no_output=true;
	if(this->risk_type==_risk_type::scenario_risk){
		no_output=false;
	}
	ostringstream cout;
	cout << "Output the detailed risk results..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout, no_output);
	
	for(int i=0; i< this->number_detailed_results; i++){
		if(this->detailed_results[i].dam_results.id_break_sc!=""){
			prefix<<this->detailed_results[i].dam_results.hyd_bound_sc_name<<"_"<<this->detailed_results[i].dam_results.id_break_sc<<"> ";
		}
		else{
			prefix<<this->detailed_results[i].dam_results.hyd_bound_sc_name<<"> ";
		}
		Sys_Common_Output::output_risk->set_userprefix(&prefix);
		cout << "DETAILED RISK RESULTS" << endl;
		Sys_Common_Output::output_risk->output_txt(&cout,no_output);
		cout << " PROBABILITIES" << endl;
		string comment;
		if(this->risk_type==_risk_type::scenario_risk){
			comment="(not required for the scenario based risk approach!)";
		}
		else{
			comment="";
		}

		cout << "  Event HYD-boundary sc.      : " << this->detailed_results[i].prob_hyd_bound_sc << label::no_unit<< comment <<endl;
		cout << "  Reoccurence HYD-boundary sc.: " << this->detailed_results[i].reocc_prob_hyd_bound_sc << label::per_annus<< comment << endl;
		cout << "  Annuality  HYD-boundary sc. : " << this->detailed_results[i].annuality_hyd_bound_sc << label::annus<< comment << endl;

		if(this->detailed_results[i].dam_results.id_break_sc!=""){
			//decide the unit string
			if(this->risk_type==_risk_type::scenario_risk){
				unit=label::per_annus;
			}
			else{
				unit=label::no_unit;
			}
			cout << "  Event break sc.             : "<< this->detailed_results[i].prob_break_sc << unit <<endl;
		}
		Sys_Common_Output::output_risk->output_txt(&cout,no_output);
		//decide the unit string
		if(this->risk_type==_risk_type::scenario_risk){
			unit=label::monetary_per_annus;
		}
		else{
			unit=label::euro;
		}
		cout <<P(2)<< FORMAT_FIXED_REAL;
		cout << " ECONOMIC" << endl;
		cout << "  Immobile (Mid/5%/95%)         : " <<  this->detailed_results[i].dam_results.ecn_immob_dam << "  "<< this->detailed_results[i].dam_results.ecn_immob_dam_5<< "  "<< this->detailed_results[i].dam_results.ecn_immob_dam_95 << unit << endl;
		cout << "  Mobile (Mid/5%/95%)           : " <<  this->detailed_results[i].dam_results.ecn_mob_dam << "  "<< this->detailed_results[i].dam_results.ecn_mob_dam_5<< "  "<< this->detailed_results[i].dam_results.ecn_mob_dam_95 << unit << endl;
		cout << "  Total (Mid/5%/95%)            : " <<  this->detailed_results[i].dam_results.ecn_mob_dam + this->detailed_results[i].dam_results.ecn_immob_dam;
		cout << "  "<< this->detailed_results[i].dam_results.ecn_mob_dam_5+ this->detailed_results[i].dam_results.ecn_immob_dam_5<< "  "<< this->detailed_results[i].dam_results.ecn_mob_dam_95+this->detailed_results[i].dam_results.ecn_immob_dam_95<< unit << endl;
		Sys_Common_Output::output_risk->output_txt(&cout,no_output);
		cout << " ECOLOGIC" << endl;
		cout << "  Biotope-type                  : " <<  this->detailed_results[i].dam_results.eco_biotype << unit << endl;
		cout << "  Soil-erosion                  : " <<  this->detailed_results[i].dam_results.eco_soil_erosion << unit << endl;
		cout << "  Total                         : " <<  this->detailed_results[i].dam_results.eco_biotype + this->detailed_results[i].dam_results.eco_soil_erosion << unit << endl;
		Sys_Common_Output::output_risk->output_txt(&cout,no_output);
		//decide the unit string
		if(this->risk_type==_risk_type::scenario_risk){
			unit=label::person_per_annus;
		}
		else{
			unit=label::person;
		}

		cout << " PEOPLE2RISK" << endl;
		cout << "  Affected                       : " <<  this->detailed_results[i].dam_results.pop_affected << unit << endl;
		cout << "  Endangered                     : " <<  this->detailed_results[i].dam_results.pop_endangered << unit << endl;
		Sys_Common_Output::output_risk->output_txt(&cout,no_output);
		//decide the unit string
		if(this->risk_type==_risk_type::scenario_risk){
			unit=label::score_per_annus;
		}
		else{
			unit=label::score;
		}

		cout << " PSYCHO-SOCIAL" << endl;
		cout << "  Age 50-59 a (with pop-dens)    : " <<  this->detailed_results[i].dam_results.pys_age_50_59 <<" ("<<this->detailed_results[i].dam_results.pys_age_50_59_dens<<")"<< unit << endl;
		cout << "  Age >80 a (with pop-dens)      : " <<  this->detailed_results[i].dam_results.pys_age_80 <<" ("<<this->detailed_results[i].dam_results.pys_age_80_dens<<")"<< unit << endl;
		cout << "  Female gender (with pop-dens)  : " <<  this->detailed_results[i].dam_results.pys_female <<" ("<<this->detailed_results[i].dam_results.pys_female_dens<<")"<< unit << endl;
		cout << "  Home owner (with pop-dens)     : " <<  this->detailed_results[i].dam_results.pys_owner <<" ("<<this->detailed_results[i].dam_results.pys_owner_dens<<")"<< unit << endl;
				Sys_Common_Output::output_risk->output_txt(&cout, no_output);

		//decide the unit string
		if(this->risk_type==_risk_type::scenario_risk){
			unit=label::score_per_annus;
		}
		else{
			unit=label::score;
		}

		cout << " SIMPLE-COUNTING" << endl;
		cout << "  Public                         : " <<  this->detailed_results[i].dam_results.sc_pub_build << unit << endl;
		cout << "  Ecologic perilous              : " <<  this->detailed_results[i].dam_results.sc_eco_build << unit << endl;
		cout << "  Cultural heritage              : " <<  this->detailed_results[i].dam_results.sc_cult_build << unit << endl;
		cout << "  Highly vulnerable person       : " <<  this->detailed_results[i].dam_results.sc_person_build << unit << endl;

	
		Sys_Common_Output::output_risk->output_txt(&cout, no_output);

		cout << " CRITICAL INFRASTRUCTURES" << endl;
		cout << "  POPULATION TIME" << endl;
		cout << "   Sec Electricity (1)           : " << this->detailed_results[i].dam_results.ci_elect_pt << label::person_sec_per_annus << endl;
		cout << "   Sec Information technology (2): " << this->detailed_results[i].dam_results.ci_info_tec_pt << label::person_sec_per_annus << endl;
		cout << "   Sec Water supply (3)          : " << this->detailed_results[i].dam_results.ci_water_sup_pt << label::person_sec_per_annus << endl;
		cout << "   Sec Water treatment (4)       : " << this->detailed_results[i].dam_results.ci_water_treat_pt << label::person_sec_per_annus << endl;
		cout << "   Sec Energy (5)                : " << this->detailed_results[i].dam_results.ci_energy_pt << label::person_sec_per_annus << endl;
		cout << "   Cum-Sec Health (10+11)        : " << this->detailed_results[i].dam_results.ci_water_sup_pt << label::person_sec_per_annus << endl;
		cout << "   Cum-Sec Social (14+17+18+19)  : " << this->detailed_results[i].dam_results.ci_water_treat_pt << label::person_sec_per_annus << endl;
		cout << "   Cum-Sec Material (12+13+15+16): " << this->detailed_results[i].dam_results.ci_energy_pt << label::person_sec_per_annus << endl;
		cout << "  POPULATION" << endl;
		cout << "   Sec Electricity (1)           : " << this->detailed_results[i].dam_results.ci_elect_p << label::person_per_annus << endl;
		cout << "   Sec Information technology (2): " << this->detailed_results[i].dam_results.ci_info_tec_p << label::person_per_annus << endl;
		cout << "   Sec Water supply (3)          : " << this->detailed_results[i].dam_results.ci_water_sup_p << label::person_per_annus << endl;
		cout << "   Sec Water treatment (4)       : " << this->detailed_results[i].dam_results.ci_water_treat_p << label::person_per_annus << endl;
		cout << "   Sec Energy (5)                : " << this->detailed_results[i].dam_results.ci_energy_p << label::person_per_annus << endl;
		cout << "   Cum-Sec Health (10+11)        : " << this->detailed_results[i].dam_results.ci_water_sup_p << label::person_per_annus << endl;
		cout << "   Cum-Sec Social (14+17+18+19)  : " << this->detailed_results[i].dam_results.ci_water_treat_p << label::person_per_annus << endl;
		cout << "   Cum-Sec Material (12+13+15+16): " << this->detailed_results[i].dam_results.ci_energy_p << label::person_per_annus << endl;

		Sys_Common_Output::output_risk->output_txt(&cout, no_output);

		//decide the unit string
		if(this->risk_type==_risk_type::scenario_risk){
			unit=label::qm_per_sec_annus;
		}
		else{
			unit=label::qm_per_sec;
		}

		cout << " OUTFLOW" << endl;
		cout << "  Maximum discharge            : " <<  this->detailed_results[i].max_outflow << unit << endl;
		Sys_Common_Output::output_risk->output_txt(&cout, no_output);

		Sys_Common_Output::output_risk->rewind_userprefix();
	}

}
//Output cumulated risk results to database table
void Risk_System::output_cumulated_risk_results2database(void){
	ostringstream cout;
	cout << "Output the cumulated risk results to database..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
	//mysql query with the query-class
	QSqlQuery model(this->qsqldatabase);
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_resulttable_cumulated(&this->qsqldatabase);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Risk_System::table_cumulated_results->maximum_int_of_column(Risk_System::table_cumulated_results->get_column_name(label::glob_id),&this->qsqldatabase)+1;
		
	for(int i=0; i< this->number_cumulated_results; i++){
		//set the query via a query string
		ostringstream query_string;
		query_string << "INSERT INTO  " << Risk_System::table_cumulated_results->get_table_name();
		query_string <<" ( ";
		query_string << Risk_System::table_cumulated_results->get_column_name(label::glob_id) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(label::areastate_id) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(label::measure_id) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(hyd_label::sz_bound_id) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(risk_label::risk_type) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(risk_label::number_detailed) <<" , ";
		//results
		query_string << Risk_System::table_cumulated_results->get_column_name(risk_label::max_outflow) <<" , ";
		//ecn
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::immob_dam) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::mob_dam) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::immob_5_quant) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::immob_95_quant) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::mob_5_quant) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::mob_95_quant) <<" , ";
		//eco
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::btype_cost) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::soil_cost) <<" , ";
		//pop
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::pop_affected) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::pop_endangered) <<" , ";
		//pys
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::crit_age_50_59) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::crit_age_80) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::crit_female) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::crit_owner) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_age50_59) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_age_80) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_female) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_owner) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::sc_pub_build) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::sc_eco_build) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::sc_cult_build) <<" , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::sc_person_build) <<" , ";


		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_elect_pt) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_info_tec_pt) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_water_sup_pt) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_water_treat_pt) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_energy_pt) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_health_pt) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_social_pt) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_mat_pt) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_elect_p) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_info_tec_p) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_water_sup_p) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_water_treat_p) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_energy_p) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_health_p) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_social_p) << " , ";
		query_string << Risk_System::table_cumulated_results->get_column_name(dam_label::ci_mat_p) << " ) ";

		query_string << " VALUES ( ";
		query_string << id_glob << " , " ;
		query_string << this->system_id.area_state << " , " ;
		query_string << this->system_id.measure_nr << " , " ;
		query_string << this->cumulated_results[i].dam_results.id_hyd_bound_sc << " , " ;
		query_string << "'"<<Risk_Break_Scenario::convert_risk_type2txt(this->risk_type)<< "' , " ;
		query_string << this->cumulated_results[i].number_applied_result<< " , " ;
		//results
		//max outflow
		query_string << this->cumulated_results[i].max_outflow << " , " ;
		//ecn
		query_string << this->cumulated_results[i].dam_results.ecn_immob_dam << " , " ;
		query_string << this->cumulated_results[i].dam_results.ecn_mob_dam  << " , " ;
		query_string << this->cumulated_results[i].dam_results.ecn_immob_dam_5 << " , " ;
		query_string << this->cumulated_results[i].dam_results.ecn_immob_dam_95 << " , " ;
		query_string << this->cumulated_results[i].dam_results.ecn_mob_dam_5  << " , " ;
		query_string << this->cumulated_results[i].dam_results.ecn_mob_dam_95 << " , " ;
		//eco
		query_string <<  this->cumulated_results[i].dam_results.eco_biotype << " , " ;
		query_string << this->cumulated_results[i].dam_results.eco_soil_erosion << " , " ;
		//pop
		query_string << this->cumulated_results[i].dam_results.pop_affected << " , " ;
		query_string << this->cumulated_results[i].dam_results.pop_endangered << " , " ;
		//pys
		query_string << this->cumulated_results[i].dam_results.pys_age_50_59 << " , " ;
		query_string << this->cumulated_results[i].dam_results.pys_age_80 << " , " ;
		query_string << this->cumulated_results[i].dam_results.pys_female << " , " ;
		query_string << this->cumulated_results[i].dam_results.pys_owner << " , " ;
		query_string << this->cumulated_results[i].dam_results.pys_age_50_59_dens << " , " ;
		query_string << this->cumulated_results[i].dam_results.pys_age_80_dens << " , " ;
		query_string << this->cumulated_results[i].dam_results.pys_female_dens << " , " ;
		query_string << this->cumulated_results[i].dam_results.pys_owner_dens << " , " ;
		query_string << this->cumulated_results[i].dam_results.sc_pub_build << " , " ;
		query_string << this->cumulated_results[i].dam_results.sc_eco_build << " , " ;
		query_string << this->cumulated_results[i].dam_results.sc_cult_build << " , " ;
		query_string << this->cumulated_results[i].dam_results.sc_person_build << " , " ;
		query_string << this->cumulated_results[i].dam_results.ci_elect_pt << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_info_tec_pt << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_water_sup_pt << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_water_treat_pt << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_energy_pt << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_health_pt << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_social_pt << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_mat_pt << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_elect_p << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_info_tec_p << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_water_sup_p << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_water_treat_p << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_energy_p << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_health_p << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_social_p << " , ";
		query_string << this->cumulated_results[i].dam_results.ci_mat_p << " ) ";

		Data_Base::database_request(&model, query_string.str(), &this->qsqldatabase);

		
		if(model.lastError().isValid()){
			Warning msg=this->set_warning(8);
			ostringstream info;
			info << "Table Name                : " << Risk_System::table_cumulated_results->get_table_name() << endl;
			info << "Table error info          : " << model.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(5);	
		}
		else{
			id_glob++;
		}
	}
}
//Output cumulated risk results to display/console
void Risk_System::output_cumulated_risk_results(void){
	ostringstream prefix;
	ostringstream cout;
	cout << "Output the cumulated risk results..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);

	for(int i=0; i< this->number_cumulated_results; i++){
		prefix<<"HQ_"<<this->cumulated_results[i].annuality_hyd_bound_sc<<"> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);
		cout << "CUMULATED RISK RESULTS" << endl;
		cout << " Number of detailed results     : " << this->total_results.number_applied_result << endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout << " PROBABILITY" << endl;
		cout << "  Reoccurence HYD-boundary sc.  : " << this->cumulated_results[i].reocc_prob_hyd_bound_sc << label::per_annus << endl;
		cout << "  Annuality  HYD-boundary sc.   : " << this->cumulated_results[i].annuality_hyd_bound_sc << label::annus<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout <<P(2)<< FORMAT_FIXED_REAL;
		cout << " ECONOMIC" << endl;
		cout << "  Immobile (Mid/5%/95%)         : " <<  this->cumulated_results[i].dam_results.ecn_immob_dam << "  "<< this->cumulated_results[i].dam_results.ecn_immob_dam_5<< "  "<< this->cumulated_results[i].dam_results.ecn_immob_dam_95 << label::euro << endl;
		cout << "  Mobile (Mid/5%/95%)           : " <<  this->cumulated_results[i].dam_results.ecn_mob_dam << "  "<< this->cumulated_results[i].dam_results.ecn_mob_dam_5<< "  "<< this->cumulated_results[i].dam_results.ecn_mob_dam_95 << label::euro << endl;
		cout << "  Total (Mid/5%/95%)            : " <<  this->cumulated_results[i].dam_results.ecn_mob_dam + this->cumulated_results[i].dam_results.ecn_immob_dam;
		cout << "  "<< this->cumulated_results[i].dam_results.ecn_mob_dam_5+ this->cumulated_results[i].dam_results.ecn_immob_dam_5<< "  "<< this->cumulated_results[i].dam_results.ecn_mob_dam_95+this->cumulated_results[i].dam_results.ecn_immob_dam_95<< label::euro << endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout << " ECOLOGIC" << endl;
		cout << "  Biotope-type                  : " <<  this->cumulated_results[i].dam_results.eco_biotype << label::euro << endl;
		cout << "  Soil-erosion                  : " <<  this->cumulated_results[i].dam_results.eco_soil_erosion << label::euro << endl;
		cout << "  Total                         : " <<  this->cumulated_results[i].dam_results.eco_biotype + this->cumulated_results[i].dam_results.eco_soil_erosion << label::euro << endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout << " PEOPLE2RISK" << endl;
		cout << "  Affected                       : " <<  this->cumulated_results[i].dam_results.pop_affected << label::person << endl;
		cout << "  Endangered                     : " <<  this->cumulated_results[i].dam_results.pop_endangered << label::person << endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout << " PSYCHO-SOCIAL" << endl;
		cout << "  Age 50-59 a (with pop-dens)    : " <<  this->cumulated_results[i].dam_results.pys_age_50_59 <<" ("<<this->cumulated_results[i].dam_results.pys_age_50_59_dens<<")"<< label::score << endl;
		cout << "  Age >80 a (with pop-dens)      : " <<  this->cumulated_results[i].dam_results.pys_age_80 <<" ("<<this->cumulated_results[i].dam_results.pys_age_80_dens<<")"<< label::score << endl;
		cout << "  Female gender (with pop-dens)  : " <<  this->cumulated_results[i].dam_results.pys_female <<" ("<<this->cumulated_results[i].dam_results.pys_female_dens<<")"<< label::score << endl;
		cout << "  Home owner (with pop-dens)     : " <<  this->cumulated_results[i].dam_results.pys_owner <<"( "<<this->cumulated_results[i].dam_results.pys_owner_dens<<")"<< label::score << endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout << " SIMPLE-COUNTING" << endl;
		cout << "  Public                         : " <<  this->cumulated_results[i].dam_results.sc_pub_build << label::score << endl;
		cout << "  Ecologic perilous              : " <<  this->cumulated_results[i].dam_results.sc_eco_build << label::score << endl;
		cout << "  Cultural heritage              : " <<  this->cumulated_results[i].dam_results.sc_cult_build<< label::score << endl;
		cout << "  Highly vulnerable person       : " <<  this->cumulated_results[i].dam_results.sc_person_build<< label::score << endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout << " CRITICAL INFRASTRUCTURES" << endl;
		cout << "  POPULATION TIME" << endl;
		cout << "   Sec Electricity (1)           : " << this->cumulated_results[i].dam_results.ci_elect_pt << label::person_sec_per_annus << endl;
		cout << "   Sec Information technology (2): " << this->cumulated_results[i].dam_results.ci_info_tec_pt << label::person_sec_per_annus << endl;
		cout << "   Sec Water supply (3)          : " << this->cumulated_results[i].dam_results.ci_water_sup_pt << label::person_sec_per_annus << endl;
		cout << "   Sec Water treatment (4)       : " << this->cumulated_results[i].dam_results.ci_water_treat_pt << label::person_sec_per_annus << endl;
		cout << "   Sec Energy (5)                : " << this->cumulated_results[i].dam_results.ci_energy_pt << label::person_sec_per_annus << endl;
		cout << "   Cum-Sec Health (10+11)        : " << this->cumulated_results[i].dam_results.ci_water_sup_pt << label::person_sec_per_annus << endl;
		cout << "   Cum-Sec Social (14+17+18+19)  : " << this->cumulated_results[i].dam_results.ci_water_treat_pt << label::person_sec_per_annus << endl;
		cout << "   Cum-Sec Material (12+13+15+16): " << this->cumulated_results[i].dam_results.ci_energy_pt << label::person_sec_per_annus << endl;
		cout << "  POPULATION" << endl;
		cout << "   Sec Electricity (1)           : " << this->cumulated_results[i].dam_results.ci_elect_p << label::person_per_annus << endl;
		cout << "   Sec Information technology (2): " << this->cumulated_results[i].dam_results.ci_info_tec_p << label::person_per_annus << endl;
		cout << "   Sec Water supply (3)          : " << this->cumulated_results[i].dam_results.ci_water_sup_p << label::person_per_annus << endl;
		cout << "   Sec Water treatment (4)       : " << this->cumulated_results[i].dam_results.ci_water_treat_p << label::person_per_annus << endl;
		cout << "   Sec Energy (5)                : " << this->cumulated_results[i].dam_results.ci_energy_p << label::person_per_annus << endl;
		cout << "   Cum-Sec Health (10+11)        : " << this->cumulated_results[i].dam_results.ci_water_sup_p << label::person_per_annus << endl;
		cout << "   Cum-Sec Social (14+17+18+19)  : " << this->cumulated_results[i].dam_results.ci_water_treat_p << label::person_per_annus << endl;
		cout << "   Cum-Sec Material (12+13+15+16): " << this->cumulated_results[i].dam_results.ci_energy_p << label::person_per_annus << endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout << " OUTFLOW" << endl;
		cout << "  Maximum discharge              : " <<  this->cumulated_results[i].max_outflow << label::qm_per_sec << endl;
		Sys_Common_Output::output_risk->output_txt(&cout);

		Sys_Common_Output::output_risk->rewind_userprefix();
	}
}
//Output total risk result to database table
void Risk_System::output_total_result2database(void){
	ostringstream cout;
	cout << "Output the total risk results to database..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
	//mysql query with the query-class
	QSqlQuery model(this->qsqldatabase);
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_resulttable(&this->qsqldatabase);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Risk_System::table_results->maximum_int_of_column(Risk_System::table_results->get_column_name(label::glob_id),&this->qsqldatabase)+1;
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Risk_System::table_results->get_table_name();
	query_string <<" ( ";
	query_string << Risk_System::table_results->get_column_name(label::glob_id) <<" , ";
	query_string << Risk_System::table_results->get_column_name(label::areastate_id) <<" , ";
	query_string << Risk_System::table_results->get_column_name(label::measure_id) <<" , ";
	query_string << Risk_System::table_results->get_column_name(risk_label::risk_type) <<" , ";
	query_string << Risk_System::table_results->get_column_name(risk_label::number_cumulated) <<" , ";
	query_string << Risk_System::table_results->get_column_name(risk_label::number_detailed) <<" , ";
	//results
	query_string << Risk_System::table_results->get_column_name(risk_label::max_outflow) <<" , ";
	//ecn
	query_string << Risk_System::table_results->get_column_name(dam_label::immob_dam) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::mob_dam) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::immob_5_quant) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::immob_95_quant) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::mob_5_quant) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::mob_95_quant) <<" , ";
	//eco
	query_string << Risk_System::table_results->get_column_name(dam_label::btype_cost) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::soil_cost) <<" , ";
	//pop
	query_string << Risk_System::table_results->get_column_name(dam_label::pop_affected) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::pop_endangered) <<" , ";
	//pys
	query_string << Risk_System::table_results->get_column_name(dam_label::crit_age_50_59) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::crit_age_80) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::crit_female) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::crit_owner) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::denscrit_age50_59) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::denscrit_age_80) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::denscrit_female) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::denscrit_owner) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::sc_pub_build) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::sc_eco_build) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::sc_cult_build) <<" , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::sc_person_build) <<" , ";


	query_string << Risk_System::table_results->get_column_name(dam_label::ci_elect_pt) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_info_tec_pt) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_water_sup_pt) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_water_treat_pt) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_energy_pt) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_health_pt) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_social_pt) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_mat_pt) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_elect_p) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_info_tec_p) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_water_sup_p) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_water_treat_p) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_energy_p) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_health_p) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_social_p) << " , ";
	query_string << Risk_System::table_results->get_column_name(dam_label::ci_mat_p) << " ) ";





	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << "'"<<Risk_Break_Scenario::convert_risk_type2txt(this->risk_type)<< "' , " ;
	query_string << this->total_results.number_applied_result<< " , " ;
	query_string << this->number_detailed_results<< " , " ;
	//results
	//max outflow
	query_string << this->total_results.max_outflow << " , " ;
	//ecn
	query_string << this->total_results.dam_results.ecn_immob_dam << " , " ;
	query_string << this->total_results.dam_results.ecn_mob_dam  << " , " ;
	query_string << this->total_results.dam_results.ecn_immob_dam_5 << " , " ;
	query_string << this->total_results.dam_results.ecn_immob_dam_95 << " , " ;
	query_string << this->total_results.dam_results.ecn_mob_dam_5  << " , " ;
	query_string << this->total_results.dam_results.ecn_mob_dam_95 << " , " ;
	//eco
	query_string <<  this->total_results.dam_results.eco_biotype << " , " ;
	query_string << this->total_results.dam_results.eco_soil_erosion << " , " ;
	//pop
	query_string << this->total_results.dam_results.pop_affected << " , " ;
	query_string << this->total_results.dam_results.pop_endangered << " , " ;
	//pys
	query_string << this->total_results.dam_results.pys_age_50_59 << " , " ;
	query_string << this->total_results.dam_results.pys_age_80 << " , " ;
	query_string << this->total_results.dam_results.pys_female << " , " ;
	query_string << this->total_results.dam_results.pys_owner << " , " ;
	query_string << this->total_results.dam_results.pys_age_50_59_dens << " , " ;
	query_string << this->total_results.dam_results.pys_age_80_dens << " , " ;
	query_string << this->total_results.dam_results.pys_female_dens << " , " ;
	query_string << this->total_results.dam_results.pys_owner_dens << " , " ;
	query_string << this->total_results.dam_results.sc_pub_build << " , " ;
	query_string << this->total_results.dam_results.sc_eco_build << " , " ;
	query_string << this->total_results.dam_results.sc_cult_build << " , " ;
	query_string << this->total_results.dam_results.sc_person_build << " , " ;

	query_string << this->total_results.dam_results.ci_elect_pt << " , ";
	query_string << this->total_results.dam_results.ci_info_tec_pt << " , ";
	query_string << this->total_results.dam_results.ci_water_sup_pt << " , ";
	query_string << this->total_results.dam_results.ci_water_treat_pt << " , ";
	query_string << this->total_results.dam_results.ci_energy_pt << " , ";
	query_string << this->total_results.dam_results.ci_health_pt << " , ";
	query_string << this->total_results.dam_results.ci_social_pt << " , ";
	query_string << this->total_results.dam_results.ci_mat_pt << " , ";
	query_string << this->total_results.dam_results.ci_elect_p << " , ";
	query_string << this->total_results.dam_results.ci_info_tec_p << " , ";
	query_string << this->total_results.dam_results.ci_water_sup_p << " , ";
	query_string << this->total_results.dam_results.ci_water_treat_p << " , ";
	query_string << this->total_results.dam_results.ci_energy_p << " , ";
	query_string << this->total_results.dam_results.ci_health_p << " , ";
	query_string << this->total_results.dam_results.ci_social_p << " , ";
	query_string << this->total_results.dam_results.ci_mat_p << " ) ";




	Data_Base::database_request(&model, query_string.str(), &this->qsqldatabase);

	if(model.lastError().isValid()){
		Warning msg=this->set_warning(9);
		ostringstream info;
		info << "Table Name                : " << Risk_System::table_results->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(5);	
	}

}
//Output total risk results to display/console
void Risk_System::output_total_results(void){
	
	ostringstream cout;
	cout << "Output the total risk results..." << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
	cout << "TOTAL RISK RESULTS" << endl;
	cout << " Number of cumulated results    : " << this->total_results.number_applied_result << endl;
	cout << " Number of detailed results     : " << this->number_detailed_results << endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	cout <<P(2)<< FORMAT_FIXED_REAL;
	cout << " ECONOMIC" << endl;
	cout << "  Immobile (Mid/5%/95%)         : " <<  this->total_results.dam_results.ecn_immob_dam << "  "<< this->total_results.dam_results.ecn_immob_dam_5<< "  "<< this->total_results.dam_results.ecn_immob_dam_95 << label::monetary_per_annus << endl;
	cout << "  Mobile (Mid/5%/95%)           : " <<  this->total_results.dam_results.ecn_mob_dam << "  "<< this->total_results.dam_results.ecn_mob_dam_5<< "  "<< this->total_results.dam_results.ecn_mob_dam_95 << label::monetary_per_annus  << endl;
	cout << "  Total (Mid/5%/95%)            : " <<  this->total_results.dam_results.ecn_mob_dam + this->total_results.dam_results.ecn_immob_dam;
	cout << "  "<< this->total_results.dam_results.ecn_mob_dam_5+ this->total_results.dam_results.ecn_immob_dam_5<< "  "<< this->total_results.dam_results.ecn_mob_dam_95+this->total_results.dam_results.ecn_immob_dam_95<< label::monetary_per_annus  << endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	cout << " ECOLOGIC" << endl;
	cout << "  Biotope-type                  : " <<  this->total_results.dam_results.eco_biotype << label::monetary_per_annus  << endl;
	cout << "  Soil-erosion                  : " <<  this->total_results.dam_results.eco_soil_erosion << label::monetary_per_annus  << endl;
	cout << "  Total                         : " <<  this->total_results.dam_results.eco_biotype + this->total_results.dam_results.eco_soil_erosion << label::monetary_per_annus  << endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	cout << " PEOPLE2RISK" << endl;
	cout << "  Affected                       : " <<  this->total_results.dam_results.pop_affected << label::person_per_annus  << endl;
	cout << "  Endangered                     : " <<  this->total_results.dam_results.pop_endangered << label::person_per_annus << endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	cout << " PSYCHO-SOCIAL" << endl;
	cout << "  Age 50-59 a (with pop-dens)    : " <<  this->total_results.dam_results.pys_age_50_59 <<" ("<<this->total_results.dam_results.pys_age_50_59_dens<<")"<< label::score_per_annus << endl;
	cout << "  Age >80 a (with pop-dens)      : " <<  this->total_results.dam_results.pys_age_80 <<" ("<<this->total_results.dam_results.pys_age_80_dens<<")"<< label::score_per_annus << endl;
	cout << "  Female gender (with pop-dens)  : " <<  this->total_results.dam_results.pys_female <<" ("<<this->total_results.dam_results.pys_female_dens<<")"<< label::score_per_annus << endl;
	cout << "  Home owner (with pop-dens)     : " <<  this->total_results.dam_results.pys_owner <<" ("<<this->total_results.dam_results.pys_owner_dens<<")"<< label::score_per_annus << endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	cout << " SIMPLE-COUNTING" << endl;
	cout << "  Public                         : " <<  this->total_results.dam_results.sc_pub_build << label::score_per_annus << endl;
	cout << "  Ecologic perilous              : " <<  this->total_results.dam_results.sc_eco_build << label::score_per_annus << endl;
	cout << "  Cultural heritage              : " <<  this->total_results.dam_results.sc_cult_build<< label::score_per_annus << endl;
	cout << "  Highly vulnerable person       : " <<  this->total_results.dam_results.sc_person_build<< label::score_per_annus << endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	cout << " CRITICAL INFRASTRUCTURES" << endl;
	cout << "  POPULATION TIME" << endl;
	cout << "   Sec Electricity (1)           : " << this->total_results.dam_results.ci_elect_pt << label::person_sec_per_annus << endl;
	cout << "   Sec Information technology (2): " << this->total_results.dam_results.ci_info_tec_pt << label::person_sec_per_annus << endl;
	cout << "   Sec Water supply (3)          : " << this->total_results.dam_results.ci_water_sup_pt<< label::person_sec_per_annus << endl;
	cout << "   Sec Water treatment (4)       : " << this->total_results.dam_results.ci_water_treat_pt << label::person_sec_per_annus << endl;
	cout << "   Sec Energy (5)                : " << this->total_results.dam_results.ci_energy_pt << label::person_sec_per_annus << endl;
	cout << "   Cum-Sec Health (10+11)        : " << this->total_results.dam_results.ci_water_sup_pt << label::person_sec_per_annus << endl;
	cout << "   Cum-Sec Social (14+17+18+19)  : " << this->total_results.dam_results.ci_water_treat_pt << label::person_sec_per_annus << endl;
	cout << "   Cum-Sec Material (12+13+15+16): " << this->total_results.dam_results.ci_energy_pt << label::person_sec_per_annus << endl;
	cout << "  POPULATION" << endl;
	cout << "   Sec Electricity (1)           : " << this->total_results.dam_results.ci_elect_p << label::person_per_annus << endl;
	cout << "   Sec Information technology (2): " << this->total_results.dam_results.ci_info_tec_p << label::person_per_annus << endl;
	cout << "   Sec Water supply (3)          : " << this->total_results.dam_results.ci_water_sup_p << label::person_per_annus << endl;
	cout << "   Sec Water treatment (4)       : " << this->total_results.dam_results.ci_water_treat_p << label::person_per_annus << endl;
	cout << "   Sec Energy (5)                : " << this->total_results.dam_results.ci_energy_p << label::person_per_annus << endl;
	cout << "   Cum-Sec Health (10+11)        : " << this->total_results.dam_results.ci_water_sup_p << label::person_per_annus << endl;
	cout << "   Cum-Sec Social (14+17+18+19)  : " << this->total_results.dam_results.ci_water_treat_p << label::person_per_annus << endl;
	cout << "   Cum-Sec Material (12+13+15+16): " << this->total_results.dam_results.ci_energy_p << label::person_per_annus << endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	cout << " OUTFLOW" << endl;
	cout << "  Maximum discharge            : " <<  this->total_results.max_outflow << label::qm_per_sec_annus << endl;
	Sys_Common_Output::output_risk->output_txt(&cout);

}
//Delete the risk result data and restore the configuartion in database table
void Risk_System::delete_risk_data(void){

	ostringstream cout;
	ostringstream prefix;
	prefix << "DEL> ";
	Sys_Common_Output::output_risk->set_userprefix(&prefix);

	if(this->restore_config==true){
		cout << "Restoring risk configuration..."<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Risk_System::reset_data_in_table_config(&this->qsqldatabase);	
	}
	if(this->del_nobreak_result==true){
		cout << "Deleting no-break risk result data in database..."<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Risk_System::delete_data_in_resulttable_detailed(&this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		Risk_System::delete_data_in_resulttable_cumulated(&this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		Risk_System::delete_data_in_resulttable(&this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		Risk_System::delete_data_in_resulttable_elem_ecn(&this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		Risk_System::delete_data_in_resulttable_elem_eco_btype(&this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		Risk_System::delete_data_in_resulttable_elem_eco_soil(&this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		Risk_System::delete_data_in_resulttable_elem_pop(&this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		Risk_System::delete_data_in_resulttable_elem_pys(&this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		Risk_System::delete_data_in_resulttable_point_sc(&this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);

	}
	if(this->del_scenario_result==true){
		cout << "Deleting scenario based risk result data in database..."<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Risk_System::delete_data_in_resulttable_detailed(&this->qsqldatabase, this->system_id, _risk_type::scenario_risk);
		Risk_System::delete_data_in_resulttable_cumulated(&this->qsqldatabase, this->system_id, _risk_type::scenario_risk);
		Risk_System::delete_data_in_resulttable(&this->qsqldatabase, this->system_id, _risk_type::scenario_risk);
		Risk_Break_Scenario_Management::delete_data_in_table(&this->qsqldatabase, _risk_type::scenario_risk, this->system_id);
		Risk_System::delete_data_in_resulttable_elem_ecn(&this->qsqldatabase, this->system_id, _risk_type::scenario_risk);
		Risk_System::delete_data_in_resulttable_elem_eco_btype(&this->qsqldatabase, this->system_id, _risk_type::scenario_risk);
		Risk_System::delete_data_in_resulttable_elem_eco_soil(&this->qsqldatabase, this->system_id, _risk_type::scenario_risk);
		Risk_System::delete_data_in_resulttable_elem_pop(&this->qsqldatabase, this->system_id, _risk_type::scenario_risk);
		Risk_System::delete_data_in_resulttable_elem_pys(&this->qsqldatabase, this->system_id, _risk_type::scenario_risk);
		Risk_System::delete_data_in_resulttable_point_sc(&this->qsqldatabase, this->system_id, _risk_type::scenario_risk);


		//hyd
		if(this->del_hyd_results==true){
			cout << " Deleting appending HYD-results data in database..."<<endl;
			Sys_Common_Output::output_risk->output_txt(&cout);
			_Hyd_River_Profile::delete_results_in_table(&this->qsqldatabase, this->system_id, "SC%", true);
			_Hyd_River_Profile::delete_instat_results_in_table(&this->qsqldatabase, this->system_id, "SC%", true);
			Hyd_Element_Floodplain::delete_results_in_table(&this->qsqldatabase, this->system_id, "SC%", true);
			_Hyd_Coupling_Dikebreak::delete_data_in_result_table(&this->qsqldatabase, this->system_id, "SC%", true);
			Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(&this->qsqldatabase, this->system_id, "SC%", true);
		}
		//fpl
		if(this->del_fpl_results==true){
			cout << " Deleting appending FPL-results data in database..."<<endl;
			Sys_Common_Output::output_risk->output_txt(&cout);
			Fpl_Mc_Sim::delete_data_in_result_table(&this->qsqldatabase, this->system_id, true, true);
		}

		//dam
		if(this->del_dam_results==true){
			cout << " Deleting appending DAM-results data in database..."<<endl;
			Sys_Common_Output::output_risk->output_txt(&cout);
			Dam_Damage_System::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "SC%", true);
			Dam_Ecn_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "SC%", true);
			Dam_Eco_Btype_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "SC%", true);
			Dam_Eco_Soil_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "SC%", true);
			Dam_People_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "SC%", true);
			Dam_Pys_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "SC%", true);
			Dam_Sc_Point::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "SC%", true);
		}
	}
	if(this->del_catchment_result==true){
		cout << "Deleting catchment based risk result data in database..."<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Risk_System::delete_data_in_resulttable_detailed(&this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		Risk_System::delete_data_in_resulttable_cumulated(&this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		Risk_System::delete_data_in_resulttable(&this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		Risk_Break_Scenario_Management::delete_data_in_table(&this->qsqldatabase, _risk_type::catchment_risk, this->system_id);
		Risk_Break_Scenario_Management::delete_data_in_table_fpl_sec_prob(&this->qsqldatabase, this->system_id);
		Risk_System::delete_data_in_resulttable_elem_ecn(&this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		Risk_System::delete_data_in_resulttable_elem_eco_btype(&this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		Risk_System::delete_data_in_resulttable_elem_eco_soil(&this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		Risk_System::delete_data_in_resulttable_elem_pop(&this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		Risk_System::delete_data_in_resulttable_elem_pys(&this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		Risk_System::delete_data_in_resulttable_point_sc(&this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		Risk_Damage_Predictor::delete_data_in_resulttable_predicted_detailed(&this->qsqldatabase, this->system_id);
	
		//hyd
		if(this->del_hyd_results==true){
			cout << " Deleting appending HYD-results data in database..."<<endl;
			Sys_Common_Output::output_risk->output_txt(&cout);
			Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(&this->qsqldatabase, this->system_id, "CA%", true);
			_Hyd_River_Profile::delete_results_in_table(&this->qsqldatabase, this->system_id, "CA%", true);
			_Hyd_River_Profile::delete_instat_results_in_table(&this->qsqldatabase, this->system_id, "CA%", true);
			Hyd_Element_Floodplain::delete_results_in_table(&this->qsqldatabase, this->system_id, "CA%", true);
			_Hyd_Coupling_Dikebreak::delete_data_in_result_table(&this->qsqldatabase, this->system_id, "CA%", true);
		}
		//fpl
		if(this->del_fpl_results==true){
			cout << " Deleting appending FPL-results data in database..."<<endl;
			Sys_Common_Output::output_risk->output_txt(&cout);
			Fpl_Mc_Sim::delete_data_in_result_table(&this->qsqldatabase, this->system_id, false, true);
		}

		//dam
		if(this->del_dam_results==true){
			cout << " Deleting appending DAM-results data in database..."<<endl;
			Sys_Common_Output::output_risk->output_txt(&cout);
			Dam_Damage_System::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "CA%", true);
			Dam_Ecn_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "CA%", true);
			Dam_Eco_Btype_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "CA%", true);
			Dam_Eco_Soil_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "CA%", true);
			Dam_People_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "CA%", true);
			Dam_Pys_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "CA%", true);
			Dam_Sc_Point::delete_data_in_erg_table(&this->qsqldatabase, this->system_id, "CA%", true);
		}
		
	}
	cout << "Deleting is finished..."<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	Sys_Common_Output::output_risk->rewind_userprefix();
}
//Allocate the hydraulic system
void Risk_System::allocate_hyd_system(void){
	this->delete_hyd_system();
	try{
		this->hyd_calc=new Hyd_Multiple_Hydraulic_Systems;
		this->hyd_calc->set_systemid(this->system_id);
		this->hyd_calc->set_required_output2file(this->hydraulic_state.file_output_required);
		this->hyd_calc->set_number_required_threads(this->hydraulic_state.number_threads);
		this->hyd_calc->set_ptr2database(&this->qsqldatabase);

		QObject::connect(this->hyd_calc, SIGNAL(emit_number_threads(QString )), this, SLOT(sent_number_hyd_thread_break_calc(QString )));
		QObject::connect(this->hyd_calc, SIGNAL(emit_number_performed_calculation(QString )), this, SLOT(recieve_output_hyd_calc_done(QString )));
		QObject::connect(this->hyd_calc, SIGNAL(emit_threads_allocated()), this, SLOT(reconnect_hyd_threads()));

		
		ostringstream prefix;
		prefix << "RISK> ";
		Sys_Common_Output::output_hyd->set_userprefix(&prefix);

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the hydraulic system
void Risk_System::delete_hyd_system(void){
	if(this->hyd_calc!=NULL){
		QObject::disconnect(this->hyd_calc, SIGNAL(emit_number_performed_calculation(QString )), this, SLOT(recieve_output_hyd_calc_done(QString )));
		QObject::disconnect(this->hyd_calc, SIGNAL(emit_number_threads(QString )), this, SLOT(sent_number_hyd_thread_break_calc(QString )));
		QObject::disconnect(this->hyd_calc, SIGNAL(emit_threads_allocated()), this, SLOT(reconnect_hyd_threads()));
		delete this->hyd_calc;
		this->hyd_calc=NULL;
		Sys_Common_Output::output_hyd->reset_userprefix();
	}
}
//Allocate the hydraulic scenario break calcuation (needed for the scenario and catchment area risk approach)
void Risk_System::allocate_hyd_break_calculation(void){
	this->delete_hyd_break_calculation();
	try{
		this->hyd_break_calc=new Risk_Hydraulic_Break_Calculation;
		this->hyd_break_calc->set_systemid(this->system_id);
		this->hyd_break_calc->set_hyd_state_data(this->hydraulic_state);
		this->hyd_break_calc->set_ptr2database(&this->qsqldatabase);
		this->hyd_break_calc->set_risk_type(this->risk_type);
		this->hyd_break_calc->set_ptr2scenarios(&this->break_sc_manager);
		this->hyd_break_calc->set_ptr_risk_thread(this);
		this->break_sc_manager.load_fpl_section(&this->qsqldatabase);
		QObject::connect(this->hyd_break_calc, SIGNAL(send_fpl_thread_runs(bool )), this, SLOT(sent_status_fpl_thread_break_calc(bool)));
		QObject::connect(this->hyd_break_calc, SIGNAL(send_hyd_thread_runs(bool )), this, SLOT(sent_status_hyd_thread_break_calc(bool)));
		QObject::connect(this->hyd_break_calc, SIGNAL(emit_number_threads(QString )), this, SLOT(sent_number_hyd_thread_break_calc(QString )));
		QObject::connect(this->hyd_break_calc, SIGNAL(send_risk_thread_sleep(bool )), this, SLOT(set_status_risk_thread(bool )));
		QObject::connect(this->hyd_break_calc, SIGNAL(send_hydraulic_thread_sleep(bool )), this, SLOT(set_status_hydraulic_thread(bool )));

		if(this->thread_type==_risk_thread_type::risk_calc_catchment_auto ||
			this->thread_type==_risk_thread_type::risk_calc_catchment_one_break || 
			this->thread_type==_risk_thread_type::risk_calc_catchment_hand){
			this->hyd_break_calc->set_probability_output(true);
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the hydraulic break calculation (needed for the scenario and catchment area risk approach)
void Risk_System::delete_hyd_break_calculation(void){
	if(this->hyd_break_calc!=NULL){
		QObject::disconnect(this->hyd_break_calc, SIGNAL(send_fpl_thread_runs(bool )), this, SLOT(sent_status_fpl_thread_break_calc(bool)));
		QObject::disconnect(this->hyd_break_calc, SIGNAL(send_hyd_thread_runs(bool )), this, SLOT(sent_status_hyd_thread_break_calc(bool)));
		QObject::disconnect(this->hyd_break_calc, SIGNAL(emit_number_threads(QString )), this, SLOT(sent_number_hyd_thread_break_calc(QString )));

		delete this->hyd_break_calc;
		this->hyd_break_calc=NULL;
	}
}
//Allocate the damage system
void Risk_System::allocate_dam_system(void){
	this->delete_dam_system();
	try{
		this->dam_calc=new Dam_Damage_System;
		this->dam_calc->set_systemid(this->system_id);
		this->dam_calc->set_ptr2database(&this->qsqldatabase);
		ostringstream prefix;
		prefix << "RISK> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the damage system
void Risk_System::delete_dam_system(void){
	if(this->dam_calc!=NULL){
		delete this->dam_calc;
		this->dam_calc=NULL;
		Sys_Common_Output::output_dam->reset_userprefix();
	}
}
//Allocate the fpl system
void Risk_System::allocate_fpl_system(void){
	this->delete_fpl_system();
	try{
		this->fpl_calc=new Fpl_Calculation;
		this->fpl_calc->set_systemid(this->system_id);
		this->fpl_calc->set_ptr2database(&this->qsqldatabase);
		QObject::connect(this->fpl_calc, SIGNAL(finished()), this, SLOT(send_status_fpl_thread()));
		QObject::connect(this->fpl_calc, SIGNAL(started()), this, SLOT(send_status_fpl_thread()));
		ostringstream prefix;
		prefix << "RISK> ";
		Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the fpl system
void Risk_System::delete_fpl_system(void){
	if(this->fpl_calc!=NULL){
		QObject::disconnect(this->fpl_calc, SIGNAL(finished()), this, SLOT(send_status_fpl_thread()));
		QObject::disconnect(this->fpl_calc, SIGNAL(started()), this, SLOT(send_status_fpl_thread()));
		delete this->fpl_calc;
		this->fpl_calc=NULL;
		Sys_Common_Output::output_fpl->reset_userprefix();
		this->fpl_system_working=false;
		this->send_status_fpl_thread();

	}
}
//Allocate the risk result array
void Risk_System::allocate_detailed_results(void){
	try{
		this->detailed_results=new _risk_risk_results[this->number_detailed_results];
		Sys_Memory_Count::self()->add_mem(sizeof(_risk_risk_results)*this->number_detailed_results,_sys_system_modules::RISK_SYS);//count the memory

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init it
	for(int i=0; i< this->number_detailed_results; i++){
		this->init_risk_result_struct(&this->detailed_results[i]);
	}
}
//Delete the risk result array
void Risk_System::delete_detailed_results(void){
	if(this->detailed_results!=NULL){
		delete []this->detailed_results;
		this->detailed_results=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_risk_risk_results)*this->number_detailed_results,_sys_system_modules::RISK_SYS);//count the memory
	}
}
//Sort the detailed results after the probability of reoccurence of the hydraulic boundary scenario
void Risk_System::sort_detailed_result(void){
/**
Bubble-sort algorithm for the sorting the detailed results array after the annuality.
*/
	_risk_risk_results temp;

	for(int i = this->number_detailed_results-1; i > 0; i--){
		for (int j = 1; j <= i; j++){
			if (this->detailed_results[j-1].annuality_hyd_bound_sc > this->detailed_results[j].annuality_hyd_bound_sc){
				temp = this->detailed_results[j-1];
				this->detailed_results[j-1] = this->detailed_results[j];
				this->detailed_results[j] = temp;
			}
		}
	}
}
//Allocate the cumulated risk result array
void Risk_System::allocate_cumulated_results(void){
	try{
		this->cumulated_results=new _risk_risk_results[this->number_cumulated_results];
		Sys_Memory_Count::self()->add_mem(sizeof(_risk_risk_results)*this->number_cumulated_results,_sys_system_modules::RISK_SYS);//count the memory

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init it
	for(int i=0; i< this->number_cumulated_results; i++){
		this->init_risk_result_struct(&this->cumulated_results[i]);
	}
}
//Delete the cumulated risk result array
void Risk_System::delete_cumulated_results(void){
	if(this->cumulated_results!=NULL){
		delete []this->cumulated_results;
		this->cumulated_results=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_risk_risk_results)*this->number_cumulated_results,_sys_system_modules::RISK_SYS);//count the memory
	}
}
//Add antry to the cumulated risk result array
void Risk_System::add_entry2cumulated_results(void){
	_risk_risk_results *buffer=NULL;
	buffer=new _risk_risk_results[this->number_cumulated_results];

	//copy
	for(int i=0; i< this->number_cumulated_results; i++){
		buffer[i]=this->cumulated_results[i];
	}
	//delete
	this->delete_cumulated_results();
	this->number_cumulated_results++;
	this->allocate_cumulated_results();
	//recopy
	for(int i=0; i< this->number_cumulated_results-1; i++){
		this->cumulated_results[i]=buffer[i];
	}
	//delete buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}
}
//Initialize the risk result structure
void Risk_System::init_risk_result_struct(_risk_risk_results *init){
	init->dam_results.hyd_bound_sc_name="";
		init->max_outflow=0.0;
		init->prob_break_sc=1.0;
		init->prob_hyd_bound_sc=1.0;
		init->reocc_prob_hyd_bound_sc=0.0;
		init->annuality_hyd_bound_sc=0;
		init->number_applied_result=0;
		init->dam_results.ecn_immob_dam=0.0;
		init->dam_results.ecn_immob_dam_5=0.0;
		init->dam_results.ecn_immob_dam_95=0.0;
		init->dam_results.ecn_mob_dam=0.0;
		init->dam_results.ecn_mob_dam_5=0.0;
		init->dam_results.ecn_mob_dam_95=0.0;
		init->dam_results.eco_biotype=0.0;
		init->dam_results.eco_soil_erosion=0.0;
		init->dam_results.pop_affected=0.0;
		init->dam_results.pop_endangered=0.0;
		init->dam_results.pys_age_50_59=0.0;
		init->dam_results.pys_age_50_59_dens=0.0;
		init->dam_results.pys_age_80=0.0;
		init->dam_results.pys_age_80_dens=0.0;
		init->dam_results.pys_female=0.0;
		init->dam_results.pys_female_dens=0.0;
		init->dam_results.pys_owner=0.0;
		init->dam_results.pys_owner_dens=0.0;
		init->dam_results.id_break_sc="";
		init->dam_results.id_hyd_bound_sc=0;
		init->dam_results.hyd_bound_sc_name="";
		init->dam_results.sc_cult_build=0.0;
		init->dam_results.sc_person_build=0.0;
		init->dam_results.sc_pub_build=0.0;
		init->dam_results.sc_eco_build=0.0;

		init->dam_results.ci_elect_pt= 0.0;
		init->dam_results.ci_info_tec_pt = 0.0;
		init->dam_results.ci_water_sup_pt = 0.0;
		init->dam_results.ci_water_treat_pt= 0.0;
		init->dam_results.ci_energy_pt= 0.0;
		init->dam_results.ci_health_pt = 0.0;
		init->dam_results.ci_social_pt = 0.0;
		init->dam_results.ci_mat_pt = 0.0;


		init->dam_results.ci_elect_p = 0.0;
		init->dam_results.ci_info_tec_p = 0.0;
		init->dam_results.ci_water_sup_p = 0.0;
		init->dam_results.ci_water_treat_p = 0.0;
		init->dam_results.ci_energy_p = 0.0;
		init->dam_results.ci_health_p = 0.0;
		init->dam_results.ci_social_p = 0.0;
		init->dam_results.ci_mat_p = 0.0;


}
//Set the risk result structure to the detailed risk
void Risk_System::risk_result_struct2detailed(_risk_risk_results *result){

	double prob_total=1.0;

	if(this->risk_type==_risk_type::nobreak_risk){
		prob_total=result->prob_hyd_bound_sc;
	}
	else if(this->risk_type==_risk_type::scenario_risk){
		prob_total=result->prob_break_sc;

	}
	else if(this->risk_type==_risk_type::catchment_risk){
		prob_total=result->prob_break_sc*result->prob_hyd_bound_sc;
	}

	//qmax
	result->max_outflow=result->max_outflow*prob_total;
	//ecn
	result->dam_results.ecn_immob_dam=result->dam_results.ecn_immob_dam*prob_total;
	result->dam_results.ecn_mob_dam=result->dam_results.ecn_mob_dam*prob_total;
	result->dam_results.ecn_immob_dam_5=result->dam_results.ecn_immob_dam_5*prob_total;
	result->dam_results.ecn_immob_dam_95=result->dam_results.ecn_immob_dam_95*prob_total;
	result->dam_results.ecn_mob_dam_5=result->dam_results.ecn_mob_dam_5*prob_total;
	result->dam_results.ecn_mob_dam_95=result->dam_results.ecn_mob_dam_95*prob_total;
	//eco
	result->dam_results.eco_biotype=result->dam_results.eco_biotype*prob_total;
	result->dam_results.eco_soil_erosion=result->dam_results.eco_soil_erosion*prob_total;
	//pop
	result->dam_results.pop_affected=result->dam_results.pop_affected*prob_total;
	result->dam_results.pop_endangered=result->dam_results.pop_endangered*prob_total;
	//pys
	result->dam_results.pys_age_50_59=result->dam_results.pys_age_50_59*prob_total;
	result->dam_results.pys_age_50_59_dens=result->dam_results.pys_age_50_59_dens*prob_total;	
	result->dam_results.pys_age_80=result->dam_results.pys_age_80*prob_total;
	result->dam_results.pys_age_80_dens=result->dam_results.pys_age_80_dens*prob_total;	
	result->dam_results.pys_female=result->dam_results.pys_female*prob_total;
	result->dam_results.pys_female_dens=result->dam_results.pys_female_dens*prob_total;
	result->dam_results.pys_owner=result->dam_results.pys_owner*prob_total;
	result->dam_results.pys_owner_dens=result->dam_results.pys_owner_dens*prob_total;
	//sc
	result->dam_results.sc_cult_build=result->dam_results.sc_cult_build*prob_total;
	result->dam_results.sc_eco_build=result->dam_results.sc_eco_build*prob_total;
	result->dam_results.sc_pub_build=result->dam_results.sc_pub_build*prob_total;
	result->dam_results.sc_person_build=result->dam_results.sc_person_build*prob_total;
	//ci
	result->dam_results.ci_elect_pt= result->dam_results.ci_elect_pt*prob_total;
	result->dam_results.ci_info_tec_pt = result->dam_results.ci_info_tec_pt*prob_total;
	result->dam_results.ci_water_sup_pt = result->dam_results.ci_water_sup_pt*prob_total;
	result->dam_results.ci_water_treat_pt = result->dam_results.ci_water_treat_pt*prob_total;
	result->dam_results.ci_energy_pt = result->dam_results.ci_energy_pt*prob_total;

	result->dam_results.ci_health_pt = result->dam_results.ci_health_pt*prob_total;
	result->dam_results.ci_social_pt = result->dam_results.ci_social_pt*prob_total;
	result->dam_results.ci_mat_pt = result->dam_results.ci_mat_pt*prob_total;

	result->dam_results.ci_elect_p = result->dam_results.ci_elect_p*prob_total;
	result->dam_results.ci_info_tec_p = result->dam_results.ci_info_tec_p*prob_total;
	result->dam_results.ci_water_sup_p = result->dam_results.ci_water_sup_p*prob_total;
	result->dam_results.ci_water_treat_p = result->dam_results.ci_water_treat_p*prob_total;
	result->dam_results.ci_energy_p = result->dam_results.ci_energy_p*prob_total;

	result->dam_results.ci_health_p = result->dam_results.ci_health_p*prob_total;
	result->dam_results.ci_social_p = result->dam_results.ci_social_p*prob_total;
	result->dam_results.ci_mat_p = result->dam_results.ci_mat_p*prob_total;

}
//Add to a base risk result a given risk result for cumulation
void Risk_System::add2risk_result_cumulation(_risk_risk_results *base, _risk_risk_results *given){


    base->max_outflow=base->max_outflow+given->max_outflow;
	base->number_applied_result++;
	//ecn
	base->dam_results.ecn_immob_dam=base->dam_results.ecn_immob_dam+given->dam_results.ecn_immob_dam;
	base->dam_results.ecn_mob_dam=base->dam_results.ecn_mob_dam+given->dam_results.ecn_mob_dam;
	base->dam_results.ecn_immob_dam_5=base->dam_results.ecn_immob_dam_5+given->dam_results.ecn_immob_dam_5;
	base->dam_results.ecn_immob_dam_95=base->dam_results.ecn_immob_dam_95+given->dam_results.ecn_immob_dam_95;
	base->dam_results.ecn_mob_dam_5=base->dam_results.ecn_mob_dam_5+given->dam_results.ecn_mob_dam_5;
	base->dam_results.ecn_mob_dam_95=base->dam_results.ecn_mob_dam_95+given->dam_results.ecn_mob_dam_95;
	//eco
	base->dam_results.eco_biotype=base->dam_results.eco_biotype+given->dam_results.eco_biotype;
	base->dam_results.eco_soil_erosion=base->dam_results.eco_soil_erosion+given->dam_results.eco_soil_erosion;
	//pop
	base->dam_results.pop_affected=base->dam_results.pop_affected+given->dam_results.pop_affected;
	base->dam_results.pop_endangered=base->dam_results.pop_endangered+given->dam_results.pop_endangered;
	//pys
	base->dam_results.pys_age_50_59=base->dam_results.pys_age_50_59+given->dam_results.pys_age_50_59;
	base->dam_results.pys_age_50_59_dens=base->dam_results.pys_age_50_59_dens+given->dam_results.pys_age_50_59_dens;	
	base->dam_results.pys_age_80=base->dam_results.pys_age_80+given->dam_results.pys_age_80;
	base->dam_results.pys_age_80_dens=base->dam_results.pys_age_80_dens+given->dam_results.pys_age_80_dens;	
	base->dam_results.pys_female=base->dam_results.pys_female+given->dam_results.pys_female;
	base->dam_results.pys_female_dens=base->dam_results.pys_female_dens+given->dam_results.pys_female_dens;
	base->dam_results.pys_owner=base->dam_results.pys_owner+given->dam_results.pys_owner;
	base->dam_results.pys_owner_dens=base->dam_results.pys_owner_dens+given->dam_results.pys_owner_dens;
	//sc
	base->dam_results.sc_cult_build=base->dam_results.sc_cult_build+given->dam_results.sc_cult_build;
	base->dam_results.sc_eco_build=base->dam_results.sc_eco_build+given->dam_results.sc_eco_build;
	base->dam_results.sc_person_build=base->dam_results.sc_person_build+given->dam_results.sc_person_build;
	base->dam_results.sc_pub_build=base->dam_results.sc_pub_build+given->dam_results.sc_pub_build;

	//ci
	base->dam_results.ci_elect_pt= base->dam_results.ci_elect_pt + given->dam_results.ci_elect_pt;
	base->dam_results.ci_info_tec_pt = base->dam_results.ci_info_tec_pt + given->dam_results.ci_info_tec_pt;
	base->dam_results.ci_water_sup_pt = base->dam_results.ci_water_sup_pt + given->dam_results.ci_water_sup_pt;
	base->dam_results.ci_water_treat_pt = base->dam_results.ci_water_treat_pt + given->dam_results.ci_water_treat_pt;
	base->dam_results.ci_energy_pt = base->dam_results.ci_energy_pt + given->dam_results.ci_energy_pt;
	base->dam_results.ci_health_pt = base->dam_results.ci_health_pt + given->dam_results.ci_health_pt;
	base->dam_results.ci_social_pt = base->dam_results.ci_social_pt + given->dam_results.ci_social_pt;
	base->dam_results.ci_mat_pt = base->dam_results.ci_mat_pt + given->dam_results.ci_mat_pt;

	base->dam_results.ci_elect_p = base->dam_results.ci_elect_p + given->dam_results.ci_elect_p;
	base->dam_results.ci_info_tec_p = base->dam_results.ci_info_tec_p + given->dam_results.ci_info_tec_p;
	base->dam_results.ci_water_sup_p = base->dam_results.ci_water_sup_p + given->dam_results.ci_water_sup_p;
	base->dam_results.ci_water_treat_p = base->dam_results.ci_water_treat_p + given->dam_results.ci_water_treat_p;
	base->dam_results.ci_energy_p = base->dam_results.ci_energy_p + given->dam_results.ci_energy_p;
	base->dam_results.ci_health_p = base->dam_results.ci_health_p + given->dam_results.ci_health_p;
	base->dam_results.ci_social_p = base->dam_results.ci_social_p + given->dam_results.ci_social_p;
	base->dam_results.ci_mat_p = base->dam_results.ci_mat_p + given->dam_results.ci_mat_p;


}
//Stepwise integration of the total risk result
void Risk_System::integration2total_result(const double lower_bound, const double upper_bound, _risk_risk_results *given){
	double delta=abs(lower_bound-upper_bound);
	this->total_results.number_applied_result++;
	this->total_results.max_outflow=this->total_results.max_outflow+given->max_outflow*delta;
	//ecn
	this->total_results.dam_results.ecn_immob_dam=this->total_results.dam_results.ecn_immob_dam+given->dam_results.ecn_immob_dam*delta;
	this->total_results.dam_results.ecn_mob_dam=this->total_results.dam_results.ecn_mob_dam+given->dam_results.ecn_mob_dam*delta;
	this->total_results.dam_results.ecn_immob_dam_5=this->total_results.dam_results.ecn_immob_dam_5+given->dam_results.ecn_immob_dam_5*delta;
	this->total_results.dam_results.ecn_immob_dam_95=this->total_results.dam_results.ecn_immob_dam_95+given->dam_results.ecn_immob_dam_95*delta;
	this->total_results.dam_results.ecn_mob_dam_5=this->total_results.dam_results.ecn_mob_dam_5+given->dam_results.ecn_mob_dam_5*delta;
	this->total_results.dam_results.ecn_mob_dam_95=this->total_results.dam_results.ecn_mob_dam_95+given->dam_results.ecn_mob_dam_95*delta;
	//eco
	this->total_results.dam_results.eco_biotype=this->total_results.dam_results.eco_biotype+given->dam_results.eco_biotype*delta;
	this->total_results.dam_results.eco_soil_erosion=this->total_results.dam_results.eco_soil_erosion+given->dam_results.eco_soil_erosion*delta;
	//pop
	this->total_results.dam_results.pop_affected=this->total_results.dam_results.pop_affected+given->dam_results.pop_affected*delta;
	this->total_results.dam_results.pop_endangered=this->total_results.dam_results.pop_endangered+given->dam_results.pop_endangered*delta;
	//pys
	this->total_results.dam_results.pys_age_50_59=this->total_results.dam_results.pys_age_50_59+given->dam_results.pys_age_50_59*delta;
	this->total_results.dam_results.pys_age_50_59_dens=this->total_results.dam_results.pys_age_50_59_dens+given->dam_results.pys_age_50_59_dens*delta;	
	this->total_results.dam_results.pys_age_80=this->total_results.dam_results.pys_age_80+given->dam_results.pys_age_80*delta;
	this->total_results.dam_results.pys_age_80_dens=this->total_results.dam_results.pys_age_80_dens+given->dam_results.pys_age_80_dens*delta;	
	this->total_results.dam_results.pys_female=this->total_results.dam_results.pys_female+given->dam_results.pys_female*delta;
	this->total_results.dam_results.pys_female_dens=this->total_results.dam_results.pys_female_dens+given->dam_results.pys_female_dens*delta;
	this->total_results.dam_results.pys_owner=this->total_results.dam_results.pys_owner+given->dam_results.pys_owner*delta;
	this->total_results.dam_results.pys_owner_dens=this->total_results.dam_results.pys_owner_dens+given->dam_results.pys_owner_dens*delta;
	//sc
	this->total_results.dam_results.sc_cult_build=this->total_results.dam_results.sc_cult_build+given->dam_results.sc_cult_build*delta;
	this->total_results.dam_results.sc_pub_build=this->total_results.dam_results.sc_pub_build+given->dam_results.sc_pub_build*delta;
	this->total_results.dam_results.sc_eco_build=this->total_results.dam_results.sc_eco_build+given->dam_results.sc_eco_build*delta;
	this->total_results.dam_results.sc_person_build=this->total_results.dam_results.sc_person_build+given->dam_results.sc_person_build*delta;

	//ci
	this->total_results.dam_results.ci_elect_pt = this->total_results.dam_results.ci_elect_pt + given->dam_results.ci_elect_pt *delta;
	this->total_results.dam_results.ci_info_tec_pt = this->total_results.dam_results.ci_info_tec_pt + given->dam_results.ci_info_tec_pt *delta;
	this->total_results.dam_results.ci_water_sup_pt = this->total_results.dam_results.ci_water_sup_pt + given->dam_results.ci_water_sup_pt *delta;
	this->total_results.dam_results.ci_water_treat_pt = this->total_results.dam_results.ci_water_treat_pt + given->dam_results.ci_water_treat_pt *delta;
	this->total_results.dam_results.ci_energy_pt = this->total_results.dam_results.ci_energy_pt + given->dam_results.ci_energy_pt *delta;
	this->total_results.dam_results.ci_health_pt = this->total_results.dam_results.ci_health_pt + given->dam_results.ci_health_pt *delta;
	this->total_results.dam_results.ci_social_pt = this->total_results.dam_results.ci_social_pt + given->dam_results.ci_social_pt *delta;
	this->total_results.dam_results.ci_mat_pt = this->total_results.dam_results.ci_mat_pt + given->dam_results.ci_mat_pt *delta;

	this->total_results.dam_results.ci_elect_p = this->total_results.dam_results.ci_elect_p + given->dam_results.ci_elect_p *delta;
	this->total_results.dam_results.ci_info_tec_p = this->total_results.dam_results.ci_info_tec_p + given->dam_results.ci_info_tec_p *delta;
	this->total_results.dam_results.ci_water_sup_p = this->total_results.dam_results.ci_water_sup_p + given->dam_results.ci_water_sup_p *delta;
	this->total_results.dam_results.ci_water_treat_p = this->total_results.dam_results.ci_water_treat_p + given->dam_results.ci_water_treat_p *delta;
	this->total_results.dam_results.ci_energy_p = this->total_results.dam_results.ci_energy_p + given->dam_results.ci_energy_p *delta;
	this->total_results.dam_results.ci_health_p = this->total_results.dam_results.ci_health_p + given->dam_results.ci_health_p *delta;
	this->total_results.dam_results.ci_social_p = this->total_results.dam_results.ci_social_p + given->dam_results.ci_social_p *delta;
	this->total_results.dam_results.ci_mat_p = this->total_results.dam_results.ci_mat_p + given->dam_results.ci_mat_p *delta;

}
//Set exeptions at start of an action
void Risk_System::set_exception_number_start(void){
	Error::get_number_errors_moduls(&this->number_error);
	Warning::get_number_warning_moduls(&this->number_warning);
}
//Set exception_number at the end of an action
void Risk_System::set_exception_number_end(void){
	Error::get_diff_number_errors_moduls(&this->number_error);
	Warning::get_diff_number_warning_moduls(&this->number_warning);
}
//Output the error statistic of the risk system
void Risk_System::output_error_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA> ";
	Sys_Common_Output::output_risk->set_userprefix(prefix.str());
	cout << "Error statistics of the RISK-system..." << endl ;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)                 :" << W(3) << this->number_error.risk_error <<endl;
	cout << " Warning(s)               :" << W(3) << this->number_warning.risk_warning <<endl;

	if(Risk_System::get_stop_thread_flag()==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_risk->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_risk->rewind_userprefix();
}
//Output the statistic of the risk-calculation
void Risk_System::output_final_statistic_calculation(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_risk->set_userprefix(prefix.str());
	cout << "Statistics of the RISK calculation..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Type of risk analysis:" << "  " << Risk_Break_Scenario::convert_risk_type2txt(this->risk_type) <<endl;
	cout << " Time                 :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s) total       :" << W(3) << this->number_error.total_error <<endl;
	cout << " Warning(s) total     :" << W(3) << this->number_warning.total_warnings <<endl;
	if(this->number_error.total_error!=0 || this->number_warning.total_warnings!=0){
		cout << "DETAILED ERROR-/WARNING-NUMBER"<< endl;
	}
	if(this->number_error.risk_error>0){
		cout << " Error(s) RISK        :" << W(3) << this->number_error.risk_error <<endl;
	}
	if(this->number_warning.risk_warning>0){
		cout << " Warning(s) RISK      :" << W(3) << this->number_warning.risk_warning <<endl;
	}
	if(this->number_error.fpl_error>0){
		cout << " Error(s) FPL         :" << W(3) << this->number_error.fpl_error <<endl;
	}
	if(this->number_warning.fpl_warning>0){
		cout << " Warning(s) FPL       :" << W(3) << this->number_warning.fpl_warning <<endl;
	}
	if(this->number_error.hyd_error>0){
		cout << " Error(s) HYD         :" << W(3) << this->number_error.hyd_error <<endl;
	}
	if(this->number_warning.hyd_warning>0){
		cout << " Warning(s) HYD       :" << W(3) << this->number_warning.hyd_warning <<endl;
	}
	if(this->number_error.dam_error>0){
		cout << " Error(s) DAM         :" << W(3) << this->number_error.dam_error <<endl;
	}
	if(this->number_warning.dam_warning>0){
		cout << " Warning(s) DAM       :" << W(3) << this->number_warning.dam_warning <<endl;
	}


	if(Risk_System::abort_thread_flag==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_risk->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_risk->rewind_userprefix();
}
//Check controll parameters
void Risk_System::check_control_params(void){
	if(this->control_params_ca.max_hyd_calc < 10){
		Warning msg=this->set_warning(1);
		msg.output_msg(5);
		//reaction use default values
		this->control_params_ca.max_hyd_calc=risk_control_param::max_hydraulic_def;
	}
	if(this->control_params_ca.prob_bound > 1.0 || this->control_params_ca.prob_bound <0.0){
		Warning msg=this->set_warning(2);
		msg.output_msg(5);
		//reaction use default values
		this->control_params_ca.prob_bound=risk_control_param::boundary_prob_def;
	}
	if(this->control_params_ca.number_mc > risk_control_param::max_mc_def ){
		Warning msg=this->set_warning(3);
		msg.output_msg(5);
		//reaction use default values
		this->control_params_ca.number_mc = risk_control_param::no_mc_def;
	}
	if(this->control_params_ca.number_mc < risk_control_param::min_mc_def ){
		Warning msg=this->set_warning(4);
		msg.output_msg(5);
		//reaction use default values
		this->control_params_ca.number_mc = risk_control_param::no_mc_def;
	}

	if(this->control_params_ca.number_intervals < 2 ){
		Warning msg=this->set_warning(11);
		msg.output_msg(5);
		//reaction use default values
		this->control_params_ca.number_intervals = 2;
	}
}
//Check if already calculated and calculate hydraulic boundary scenario (nobreak scenarios)
void Risk_System::check_calc_hyd_nobreak_sc(void){
	this->break_sc_manager.set_risk_type(_risk_type::catchment_risk);
	

	ostringstream prefix;
	ostringstream cout;


	try{
		//set prefix
		
		//generate list of nobreak risk states
		this->break_sc_manager.set_nobreak_risk_state_list(&this->qsqldatabase);
		
		if(this->break_sc_manager.get_number_sc2calc()==0){	
			cout << "All nobreak risk states for the nobreak risk approach are already calculated" << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			return;
		}

		//output
		cout << "Calculate "<<this->break_sc_manager.get_number_sc2calc()<<" nobreak risk states of the HYD-boundary scenario for the nobreak risk approach"<< endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

		//load the hydraulic threads and idealize
		this->allocate_hyd_break_calculation();
		cout << "Load the HYD-calculation thread and idealise the HYD-system with the all relevant FPL-section(s)..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		this->hyd_break_calc->alloc_load_hyd_threads();


		//start hydraulic calculation
		cout << "Start the HYD-calculation for the nobreak risk states. Further information see HYD display..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Sys_Common_Output::output_hyd->set_focus_on_display();
		this->emit_stop_next_hyd_calc(true);
		this->hyd_break_calc->calculate_hyd_threads();
		this->emit_stop_next_hyd_calc(false);


		//waiting for the threads
		this->wait_loop();
		Sys_Common_Output::output_risk->set_focus_on_display();
		cout << "HYD-calculation is finished" << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);

		this->delete_hyd_break_calculation();
		Risk_System::check_stop_thread_flag();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();


		//damages
		Risk_System::check_stop_thread_flag();
		this->check_calc_dam_sc();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();

		Risk_System::check_stop_thread_flag();

	}
	catch(Error msg){
		msg.output_msg(5);

	}
	cout << "Calculation of the nobreak risk states for the nobreak risk approach is finished" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
}
//Check if already calculated and calculate damage results for hydraulic boundary scenario (nobreak scenarios)
void Risk_System::check_calc_dam_nobreak_sc(void){
	ostringstream cout;
	try{
		this->allocate_dam_system();
		this->dam_calc->set_string_break_sc("CA");
		int counter=0;
		cout << "Start damage calculation. Further information see DAM display..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		counter=this->dam_calc->select_scenarios_without_dam_result(&this->qsqldatabase, true);
		if(counter!=0){
			emit send_dam_thread_runs(true);
			Sys_Common_Output::output_dam->set_focus_on_display();
			this->dam_calc->set_thread_type(_dam_thread_type::dam_sys_calc_no_break_sz);
			this->dam_calc->run();
			emit send_dam_thread_runs(false);
			Sys_Common_Output::output_risk->set_focus_on_display();
			cout << "Damage calculation is finished" << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
		}
		else{
			cout << "All damage are already calculated" << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
		}
	}
	catch(Error msg){
		this->delete_dam_system();
		ostringstream info;
		info <<"Error occured in the damage calculation"<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	this->delete_dam_system();

}
//Check if already calculated and calculate damage results for hydraulic boundary scenario (scenario based and catchment area based risk approach)
void Risk_System::check_calc_dam_sc(void){
	ostringstream cout;
	_dam_break2nobreak_sc *my_list=NULL;
	int counter=0;
	try{
		this->allocate_dam_system();
		cout << "Start damage calculation. Further information see DAM display..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		//set the list for damage calculation
		counter=this->break_sc_manager.get_list_break_sc2calc(&my_list);
		if(counter==0){
			cout << "No scenarios are set for the damage calculation" << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
		}
		else{
			this->dam_calc->set_break_sc_array(counter, my_list);
			counter=this->dam_calc->select_scenarios_without_dam_result(&this->qsqldatabase, false);

			if(counter!=0){
				emit send_dam_thread_runs(true);
				
				Sys_Common_Output::output_dam->set_focus_on_display();

				this->dam_calc->set_thread_type(_dam_thread_type::dam_sys_calc_break_sz);
				this->dam_calc->run();
				emit send_dam_thread_runs(false);
				Sys_Common_Output::output_risk->set_focus_on_display();
				cout << "Damage calculation is finished" << endl ;
				Sys_Common_Output::output_risk->output_txt(&cout);
			}
			else{
				cout << "All damage are already calculated" << endl ;
				Sys_Common_Output::output_risk->output_txt(&cout);
			}
		}
	}
	catch(Error msg){
		this->delete_dam_system();
		if(my_list!=NULL){
			delete []my_list;
			my_list=NULL;
		}
		ostringstream info;
		info <<"Error occured in the damage calculation"<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	this->delete_dam_system();
	if(my_list!=NULL){
		delete []my_list;
		my_list=NULL;
	}

}
//Delete all risk results of a system state in database 
void Risk_System::delete_risk_result(void){
	Risk_System::delete_data_in_resulttable(&this->qsqldatabase, this->system_id);
	Risk_System::delete_data_in_resulttable_cumulated(&this->qsqldatabase, this->system_id);
	Risk_System::delete_data_in_resulttable_detailed(&this->qsqldatabase, this->system_id);
	Risk_Damage_Predictor::delete_data_in_resulttable_predicted_detailed(&this->qsqldatabase, this->system_id);
	Risk_Break_Scenario_Management::delete_data_in_table(&this->qsqldatabase, this->system_id);
	Risk_Break_Scenario_Management::delete_data_in_table_fpl_sec_prob(&this->qsqldatabase, this->system_id);
	Risk_System::delete_data_in_resulttable_elem_ecn(&this->qsqldatabase, this->system_id);
	Risk_System::delete_data_in_resulttable_elem_eco_btype(&this->qsqldatabase, this->system_id);
	Risk_System::delete_data_in_resulttable_elem_eco_soil(&this->qsqldatabase, this->system_id);
	Risk_System::delete_data_in_resulttable_elem_pop(&this->qsqldatabase, this->system_id);
	Risk_System::delete_data_in_resulttable_elem_pys(&this->qsqldatabase, this->system_id);
	Risk_System::delete_data_in_resulttable_point_sc(&this->qsqldatabase, this->system_id);

	this->del_catchment_result=true;
	this->del_nobreak_result=true;
	this->del_scenario_result=true;

	this->del_dam_results= hyd_dam_fpl_del_flag;
	this->del_hyd_results= hyd_dam_fpl_del_flag;
	this->del_fpl_results= hyd_dam_fpl_del_flag;

	this->delete_risk_data();

	this->del_catchment_result=false;
	this->del_nobreak_result=false;
	this->del_scenario_result=false;

	this->del_dam_results=false;
	this->del_hyd_results=false;
	this->del_fpl_results=false;	


}
//Delete all risk relevant results (fpl, hyd, dam, risk) of a system state in database 
void Risk_System::delete_all_relevant_result(void){
	//delete the relevant result data for the risk calculation
	//fpl
	if(this->risk_states.reliability_fpl_result==false){
		this->break_sc_manager.load_fpl_section(&this->qsqldatabase, false);

		for(int i=0; i<this->break_sc_manager.get_number_fpl_section(); i++){
			if(this->break_sc_manager.get_ptr_fpl_section_index(i)->get_type_section()!=_fpl_section_types::by_hand_sec){
				Fpl_Mc_Sim::delete_data_in_result_table(&this->qsqldatabase, this->break_sc_manager.get_ptr_fpl_section_index(i)->get_id_section(), this->system_id,  true);	
				Fpl_Mc_Sim::delete_data_in_result_table(&this->qsqldatabase, this->break_sc_manager.get_ptr_fpl_section_index(i)->get_id_section(), this->system_id, false);
			}
		}
	
		Fpl_Sec_Type_Highground::delete_data_in_result_table(&this->qsqldatabase, this->system_id);
		Fpl_Sec_Type_Dike::delete_data_in_result_table(&this->qsqldatabase, this->system_id);
		Fpl_Sec_Type_Dune::delete_data_in_result_table(&this->qsqldatabase, this->system_id);
	}
	//hyd
	if(this->risk_states.reliability_hyd_result==false){
		Hyd_Element_Floodplain::delete_data_in_erg_table(&this->qsqldatabase, this->system_id);
		_Hyd_River_Profile::delete_results_by_system_state(&this->qsqldatabase, this->system_id);
		_Hyd_Coupling_Dikebreak::delete_data_in_result_table(&this->qsqldatabase, this->system_id);
		Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(&this->qsqldatabase, this->system_id);

	}
	//dam
	if(this->risk_states.reliability_dam_result==false){
		Dam_Damage_System::delete_data_in_erg_table(&this->qsqldatabase, this->system_id);
		Dam_Ecn_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id);
		Dam_Eco_Btype_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id);
		Dam_Eco_Soil_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id);
		Dam_People_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id);
		Dam_Pys_Element::delete_data_in_erg_table(&this->qsqldatabase, this->system_id);
		Dam_Sc_Point::delete_data_in_erg_table(&this->qsqldatabase, this->system_id);
		Dam_CI_Point::delete_data_in_erg_table(&this->qsqldatabase, this->system_id);
		Dam_CI_Point::delete_data_in_instat_erg_table(&this->qsqldatabase, this->system_id);
		Dam_CI_Polygon::delete_data_in_erg_table(&this->qsqldatabase, this->system_id);
		Dam_CI_Polygon::delete_data_in_instat_erg_table(&this->qsqldatabase, this->system_id);
	}
}
//Waiting loop for the threads
void Risk_System::wait_loop(void){

	int number=0;
	ostringstream buffer;

	emit send_risk_thread_sleeps(true);
	bool all_finished=true;
	//waiting loop for this thread
	do{
		all_finished=true;
		
		if(this->fpl_calc!=NULL && this->fpl_calc->isRunning()==true){
			all_finished=false;
		}
		if(this->hyd_calc!=NULL && (this->hyd_calc->get_hydraulic_thread_running(&number)==true || this->hyd_calc->isRunning()==true )){
			all_finished=false;
		}
		if(this->hyd_break_calc!=NULL && this->hyd_break_calc->get_hydraulic_break_thread_running(&number)==true){
			all_finished=false;
		}
		if(all_finished==false){
			if(number!=0){
				buffer.str("");
				buffer << number;
				emit sent_number_hyd_thread_break_calc(buffer.str().c_str()); 
			}
			this->sleep(3);
		}
	}
	while(all_finished==false);
	emit send_risk_thread_sleeps(false);
}
//Output the statistics of already generated detailed risk results
void Risk_System::output_statistics_generated_detailed_results(void){
	try{
		QSqlQueryModel model_detailed;
		QSqlQueryModel model_cumul;

		//hydraulic boundary manager
		Hyd_Boundary_Szenario_Management my_hyd_sc;
		my_hyd_sc.set_systemid(this->system_id);
		my_hyd_sc.set_szenario_per_db(&this->qsqldatabase);

		int number=0;
		ostringstream prefix;
		ostringstream cout;
		prefix << "STA"<<"> ";
		Sys_Common_Output::output_risk->set_userprefix(prefix.str());
		cout << "Statistics of the existing RISK results..." << endl ;

		//nobreak
		cout << functions::convert_string2lupper_case(risk_label::risk_nobreak)<<endl;
		number=Risk_System::select_data_in_resulttable_detailed(&model_detailed, &this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		cout << " Number detailed results   : " <<number<<endl;

		number=Risk_System::select_data_in_resulttable_cumulated(&model_cumul, &this->qsqldatabase, this->system_id, _risk_type::nobreak_risk);
		cout << " Number cumulated results  : " <<number<<endl;
		if(number>0){
			cout << " Annualities of cumulation (number detailed result):"<<endl;
			int hyd_sc_id=0;
			int number_detailed=0;
			for(int i=0; i< number; i++){
				hyd_sc_id=model_cumul.record(i).value((Risk_System::table_cumulated_results->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
				number_detailed=model_cumul.record(i).value((Risk_System::table_cumulated_results->get_column_name(risk_label::number_detailed)).c_str()).toInt();
				cout << "  "<<(double)my_hyd_sc.get_ptr_sz_id_given(hyd_sc_id)->get_annuality()<<label::annus<<" (" <<number_detailed<<")"<< endl;
			}
		}

		Sys_Common_Output::output_risk->output_txt(&cout);
		//scenario based
		cout << functions::convert_string2lupper_case(risk_label::risk_scenario)<<endl;
		number=Risk_System::select_data_in_resulttable_detailed(&model_detailed, &this->qsqldatabase, this->system_id, _risk_type::scenario_risk);
		cout << " Number detailed results   : " <<number<<endl;
		if(number>0){
			cout << " HYD-bound info "<< W(10)<<" break_id "<<endl;
			int hyd_sc_id=0;
			string hyd_sc_str;
			string break_id;
			for(int i=0; i< number; i++){
				hyd_sc_id=model_detailed.record(i).value((Risk_System::table_detailed_results->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
				break_id=model_detailed.record(i).value((Risk_System::table_detailed_results->get_column_name(risk_label::sz_break_id)).c_str()).toString().toStdString();
				hyd_sc_str=my_hyd_sc.get_ptr_sz_id_given(hyd_sc_id)->get_name();
				cout << "  "<<hyd_sc_id<<"  " << hyd_sc_str << W(25) << break_id <<endl;
			}
		}

		Sys_Common_Output::output_risk->output_txt(&cout);
		//catchment area
		cout << functions::convert_string2lupper_case(risk_label::risk_catchment)<<endl;
		number=Risk_System::select_data_in_resulttable_detailed(&model_detailed, &this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		cout << " Number detailed results   : " <<number<<endl;
		if(number>0){
			cout << " HYD-bound info "<< W(10)<<" break_id "<<endl;
			int hyd_sc_id=0;
			string hyd_sc_str;
			string break_id;
			for(int i=0; i< number; i++){
				hyd_sc_id=model_detailed.record(i).value((Risk_System::table_detailed_results->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
				break_id=model_detailed.record(i).value((Risk_System::table_detailed_results->get_column_name(risk_label::sz_break_id)).c_str()).toString().toStdString();
				hyd_sc_str=my_hyd_sc.get_ptr_sz_id_given(hyd_sc_id)->get_name();
				cout << "  "<<hyd_sc_id<<"  " << hyd_sc_str << W(25) << break_id <<endl;
			}
		}
		number=Risk_System::select_data_in_resulttable_cumulated(&model_cumul, &this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
		cout << " Number cumulated results  : " <<number<<endl;
		if(number>0){
			cout << " Annualities of cumulation (number detailed result):"<<endl;
			int hyd_sc_id=0;
			int number_detailed=0;
			for(int i=0; i< number; i++){
				hyd_sc_id=model_cumul.record(i).value((Risk_System::table_cumulated_results->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
				number_detailed=model_cumul.record(i).value((Risk_System::table_cumulated_results->get_column_name(risk_label::number_detailed)).c_str()).toInt();
				cout << "  "<<(double)my_hyd_sc.get_ptr_sz_id_given(hyd_sc_id)->get_annuality()<<label::annus<<" (" <<number_detailed<<")"<< endl;
			}
		}

		Sys_Common_Output::output_risk->output_txt(&cout);

	}
	catch(Error msg){
		msg.output_msg(5);
	}
	Sys_Common_Output::output_risk->rewind_userprefix();
}
//Calculate the statistic of the predicted risk
void Risk_System::calculate_predicted_risk_statistic(const bool with_output){

	if(this->control_params_ca.use_predict_values==false){
		return;
	}

	Risk_Damage_Predictor::init_risk_predicted_risk_structure(&this->total_predicted);
	Risk_Damage_Predictor::init_risk_predicted_risk_structure(&this->calculated_predicted);


	Hyd_Boundary_Szenario_Management buff_hyd;
	buff_hyd.set_systemid(this->system_id);
	buff_hyd.set_szenario_per_db(&this->qsqldatabase);

	QSqlQueryModel model;
	int number;
	number=Risk_System::select_data_in_resulttable_cumulated(&model, &this->qsqldatabase, this->system_id, _risk_type::catchment_risk);
	

	double prob=0.0;
	int id_hyd_sc=0;
	double value=0.0;
	for(int i=0; i< number; i++){
		//search for the probability
		id_hyd_sc=model.record(i).value((Risk_System::table_cumulated_results->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
		prob=buff_hyd.get_ptr_sz_id_given(id_hyd_sc)->get_prob_year();

		//set the calculated_predicted values
		//ecn
		value=model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
		value=value+model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
		this->calculated_predicted.ecn_risk=this->calculated_predicted.ecn_risk+value*prob;
		//eco
		value=model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
		value=value+model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
		this->calculated_predicted.eco_risk=this->calculated_predicted.eco_risk+value*prob;

		//pop
		//affected
		value=model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
		this->calculated_predicted.pop_affected_risk=this->calculated_predicted.pop_affected_risk+value*prob;
		//endangered
		value=model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
		this->calculated_predicted.pop_endangered_risk=this->calculated_predicted.pop_endangered_risk+value*prob;

		//pys
		//without density function
		value=model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
		value=value+model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
		value=value+model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::crit_female)).c_str()).toDouble();
		value=value+model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::crit_owner)).c_str()).toDouble();
		
		this->calculated_predicted.pys_without_density_risk=this->calculated_predicted.pys_without_density_risk+value*prob;

		//with density function
		value=model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
		value=value+model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble();
		value=value+model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_female)).c_str()).toDouble();
		value=value+model.record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble();
		
		this->calculated_predicted.pys_with_density_risk=this->calculated_predicted.pys_with_density_risk+value*prob;

	}

	this->total_predicted=this->calculated_predicted;


	Risk_Catchment_Break_Scenario_List list;

	list.set_total_risk_break_list(&this->qsqldatabase, this->system_id);
	list.delete_calculated_in_list();

	//set the predicted risk data
	for(int i=0; i<list.get_number_in_list(); i++){
		list.get_ptr_scenario(i)->set_predicted_risk_data(&this->qsqldatabase);
	}

	//sum it up
	for(int i=0; i<list.get_number_in_list(); i++){
		list.get_ptr_scenario(i)->add_predicted_risk_data(&total_predicted);
	}
	//calculate the percentage
	for(int i=0; i<list.get_number_in_list(); i++){
		list.get_ptr_scenario(i)->calculated_percentage_predicted_risk_data(&total_predicted);
	}
	int count=6;
	//calculate the percentage of already calculated_predicted one
	if(this->total_predicted.ecn_risk!=0.0){
		this->calculated_predicted.ecn_risk_perc=this->calculated_predicted.ecn_risk/this->total_predicted.ecn_risk*100.0;
	}
	else{
		this->calculated_predicted.ecn_risk_perc=0.0;
		count--;
	}
	if(this->total_predicted.eco_risk!=0.0){
		this->calculated_predicted.eco_risk_perc=this->calculated_predicted.eco_risk/this->total_predicted.eco_risk*100.0;
	}
	else{
		this->calculated_predicted.eco_risk_perc=0.0;
		count--;
	}
	if(this->total_predicted.pop_affected_risk!=0.0){
		this->calculated_predicted.pop_affected_risk_perc=this->calculated_predicted.pop_affected_risk/this->total_predicted.pop_affected_risk*100.0;
	}
	else{
		this->calculated_predicted.pop_affected_risk_perc=0.0;
		count--;
	}
	if(this->total_predicted.pop_endangered_risk!=0){
		this->calculated_predicted.pop_endangered_risk_perc=this->calculated_predicted.pop_endangered_risk/this->total_predicted.pop_endangered_risk*100.0;
	}
	else{
		this->calculated_predicted.pop_endangered_risk_perc=0.0;
		count--;
	}
	if(this->total_predicted.pys_with_density_risk!=0){
		this->calculated_predicted.pys_with_density_risk_perc=this->calculated_predicted.pys_with_density_risk/this->total_predicted.pys_with_density_risk*100.0;
	}
	else{
		this->calculated_predicted.pys_with_density_risk_perc=0.0;
		count--;
	}
	if(this->total_predicted.pys_without_density_risk!=0.0){
		this->calculated_predicted.pys_without_density_risk_perc=this->calculated_predicted.pys_without_density_risk/this->total_predicted.pys_without_density_risk*100.0;
	}
	else{
		this->calculated_predicted.pys_without_density_risk_perc=0.0;
		count--;
	}

	//calculated the middle
	this->calculated_predicted.middle_perc=this->calculated_predicted.pys_without_density_risk_perc+this->calculated_predicted.pys_with_density_risk_perc+
		this->calculated_predicted.pop_endangered_risk_perc+this->calculated_predicted.pop_affected_risk_perc+
		this->calculated_predicted.eco_risk_perc+this->calculated_predicted.ecn_risk_perc;
	if(count==0){
		this->calculated_predicted.middle_perc=0.0;
	}
	else{
		this->calculated_predicted.middle_perc=this->calculated_predicted.middle_perc/(double)count;
	}


	//output results to database
	try{
		_risk_predicted_risk predict;
		Risk_Damage_Predictor::init_risk_predicted_risk_structure(&predict);
		for(int i=0; i<list.get_number_in_list(); i++){
			if(list.get_ptr_scenario(i)->get_ptr_predicted_risk_values()==NULL){
				continue;
			}
			else{
				predict=*list.get_ptr_scenario(i)->get_ptr_predicted_risk_values();
				Risk_Damage_Predictor::update_predicted_result_by_percentage(&this->qsqldatabase, this->system_id, list.get_ptr_scenario(i)->get_hydraulic_boundary_scenario().get_id(), list.get_ptr_scenario(i)->get_break_scenario_string(), predict);
			}
		}
	}
	catch(Error msg){
		throw msg;
	}

	if(with_output==true){
		this->output_statistic_predicted_risk(&list);
	}
}
//Output the statistic of the predicted risk
void Risk_System::output_statistic_predicted_risk(Risk_Catchment_Break_Scenario_List *list){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_risk->set_userprefix(prefix.str());
	cout << "Statistics of the predicted catchment-area based RISK results..." << endl ;
	if(list->get_number_in_list()>0){
		cout << " "<<"DETAILED CALCULATED PERCENTAGE PER SCENARIO [%]"<<endl;

		//header
		cout<<"  No. "<<" Ecn "<< label::percentage<<W(15)<<" Eco " << label::percentage<<W(15);
		cout <<" Pop_affected "<< label::percentage<<" Pop_endangered " <<label::percentage;
		cout <<" Pys_score " << label::percentage<< " Pys_score_dens " << label::percentage<<" Mid_of_perc"<<label::percentage;
		cout << " ID_Hyd_Sc. " << " ID_Break_Sc. " <<endl;
		for(int i=0; i< list->get_number_in_list(); i++){
			if(list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()!=NULL){
				cout << P(2)<< FORMAT_FIXED_REAL;
				cout << "   "<< i<<W(5);
				cout << " "<<list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()->ecn_risk_perc<<W(15);
				cout << " "<<list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()->eco_risk_perc<<W(15);
				cout << " "<<list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()->pop_affected_risk_perc<<W(15);
				cout << " "<<list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()->pop_endangered_risk_perc<<W(15);
				cout << " "<<list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()->pys_without_density_risk_perc<<W(15);
				cout << " "<<list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()->pys_with_density_risk_perc<<W(15);
				cout << " "<<list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()->middle_perc <<W(10);
				cout << " "<<list->get_ptr_scenario(i)->get_hydraulic_boundary_scenario().get_name()<<W(5);
				cout << " "<<list->get_ptr_scenario(i)->get_break_scenario_string();
				cout << endl;
				Sys_Common_Output::output_risk->output_txt(&cout);
			}

		}
		
		cout << " "<<"TOTAL CALCULATED PERCENTAGE [%]"<<endl;
		cout << "  "<<"Economical                       : "<<this->calculated_predicted.ecn_risk_perc<<label::percentage<<endl;
		cout << "  "<<"Ecological                       : "<<this->calculated_predicted.eco_risk_perc<<label::percentage<<endl;
		cout << "  "<<"People2risk affected             : "<<this->calculated_predicted.pop_affected_risk_perc<<label::percentage<<endl;
		cout << "  "<<"People2risk endangered           : "<<this->calculated_predicted.pop_endangered_risk_perc<<label::percentage<<endl;
		cout << "  "<<"Psycho-social (without density)  : "<<this->calculated_predicted.pys_without_density_risk_perc<<label::percentage<<endl;
		cout << "  "<<"Psycho-social (with density)     : "<<this->calculated_predicted.pys_with_density_risk_perc<<label::percentage<<endl;
		cout << "  "<<"Mid-value                        : "<<this->calculated_predicted.middle_perc<<label::percentage<<endl;
		cout << "  "<<"Simple counting values are not involved in prediction!"<<endl;
	}
	else{
		cout << "There are no predicted catchment-area based RISK results available!" << endl ;

	}
	Sys_Common_Output::output_risk->output_txt(&cout);

	Sys_Common_Output::output_risk->rewind_userprefix();
}
//Calculate, generate and insert to database table the detailed risk result of the damage elements
void Risk_System::calc_detailed_result_element(void){

	ostringstream cout;
	ostringstream prefix;
	cout << "Calculation and transfer of the detailed risk results per damage elements..."<< endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	prefix << "RES_ELEM> ";
	Sys_Common_Output::output_risk->set_userprefix(&prefix);

	_risk_risk_results buffer;
	this->init_risk_result_struct(&buffer);

	for(int i=0; i<this->number_detailed_results; i++){
		if(this->check_detailed_risk_element_results(this->detailed_results[i].dam_results.id_break_sc, this->detailed_results[i].dam_results.id_hyd_bound_sc)==false){
			cout << "Calculate and transfer the detailed risk results per damage elements for HYD-bound scenario "<< this->detailed_results[i].dam_results.hyd_bound_sc_name;
			if(this->detailed_results[i].dam_results.id_break_sc!=""){
				cout << " and break scenario " << this->detailed_results[i].dam_results.id_break_sc;
			}
			cout << "..."<< endl;
			Sys_Common_Output::output_risk->output_txt(&cout);
			buffer=this->detailed_results[i];
			//search for the total results
			if(this->risk_type!=_risk_type::scenario_risk){
				QSqlQueryModel model;
				if(Risk_System::select_data_in_resulttable(&model, &this->qsqldatabase, this->system_id, this->risk_type)>0){	
					buffer.dam_results.ecn_immob_dam=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble();	
					buffer.dam_results.ecn_mob_dam=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
					
					buffer.dam_results.eco_biotype=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble();	
					buffer.dam_results.eco_soil_erosion=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble();

					buffer.dam_results.pop_affected=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::pop_affected)).c_str()).toDouble();	
					buffer.dam_results.pop_endangered=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
					
					buffer.dam_results.pys_age_50_59=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();	
					buffer.dam_results.pys_age_80=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
					buffer.dam_results.pys_female=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::crit_female)).c_str()).toDouble();	
					buffer.dam_results.pys_owner=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::crit_owner)).c_str()).toDouble();

					buffer.dam_results.pys_age_50_59_dens=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();	
					buffer.dam_results.pys_age_80_dens=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble();
					buffer.dam_results.pys_female_dens=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::denscrit_female)).c_str()).toDouble();	
					buffer.dam_results.pys_owner_dens=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble();

					buffer.dam_results.sc_pub_build=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::sc_pub_build)).c_str()).toDouble();	
					buffer.dam_results.sc_eco_build=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::sc_eco_build)).c_str()).toDouble();
					buffer.dam_results.sc_cult_build=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::sc_cult_build)).c_str()).toDouble();	
					buffer.dam_results.sc_person_build=model.record(i).value((Risk_System::table_results->get_column_name(dam_label::sc_person_build)).c_str()).toDouble();
				}
			}

			this->calc_detailed_result_element_ecn(&buffer);
			this->calc_detailed_result_element_eco_btype(&buffer);
			this->calc_detailed_result_element_eco_soil(&buffer);
			this->calc_detailed_result_element_pop(&buffer);
			this->calc_detailed_result_element_pys(&buffer);
			this->calc_detailed_result_point_sc(&buffer);

		}
	}

	Sys_Common_Output::output_risk->rewind_userprefix();
}
//Calculate, generate and insert to database table the detailed risk result of the economical damage elements
void Risk_System::calc_detailed_result_element_ecn(_risk_risk_results *total){
	

	string query_header;
	query_header=this->generate_sql_header_string_elem_ecn();
	int glob_id=Risk_System::table_results_elem_ecn->maximum_int_of_column(Risk_System::table_results_elem_ecn->get_column_name(label::glob_id), &this->qsqldatabase)+1;

	QSqlQueryModel model;
	int number=0;
	number=Dam_Ecn_Element::select_data_in_erg_table(&model,&this->qsqldatabase, this->system_id, total->dam_results.id_hyd_bound_sc, total->dam_results.id_break_sc);


	ostringstream cout;
	cout << "Transfer "<<number<<" detailed economical risk results of the elements to database..."<< endl;
	Sys_Common_Output::output_risk->output_txt(&cout);




	if(number!=0){
		//Set the query
		QSqlQuery query_buff(this->qsqldatabase);
		int raster=0;
		int elem=0;
		double buff_mob=0.0;
		double buff_immob=0.0;
		double prob_total=total->prob_break_sc*total->prob_hyd_bound_sc*total->reocc_prob_hyd_bound_sc;
		string buff_str;

		int counter=0;

		ostringstream query_data;
		ostringstream query_total;


		for(int i=0; i< number; i++){
			if(i%10000==0 && i>0){
				cout << i <<" ("<<number<<") detailed risk results of economical raster elements are transfered to database..."<< endl;
				Sys_Common_Output::output_risk->output_txt(&cout);
				Risk_System::check_stop_thread_flag();
			}

			raster=model.record(i).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			elem=model.record(i).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
			buff_immob=model.record(i).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
			buff_mob=model.record(i).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
			buff_str=model.record(i).value("ST_ASTEXT").toString().toStdString();

			query_data << " ( ";
			query_data << glob_id<<" , ";
			query_data << raster<<" , ";
			query_data << elem<<" , ";
			query_data << this->system_id.area_state<<" , ";
			query_data << this->system_id.measure_nr<<" , ";
			query_data << total->dam_results.id_hyd_bound_sc<<" , ";
			query_data << " '"<< total->dam_results.id_break_sc<<"' , ";
			query_data << " '"<< Risk_Break_Scenario::convert_risk_type2txt(this->risk_type)<<"' , ";

			//data
			query_data << buff_immob*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff_immob*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			query_data << buff_mob*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff_mob*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			query_data << buff_mob*total->prob_break_sc+buff_immob*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff_mob*prob_total+buff_immob*prob_total<<" , ";

				if((total->dam_results.ecn_immob_dam+total->dam_results.ecn_mob_dam)>0.0){
					query_data << (buff_mob*prob_total+buff_immob*prob_total)/(total->dam_results.ecn_immob_dam+total->dam_results.ecn_mob_dam)<<" , ";
				}
				else{
					query_data << 0.0 <<" , ";
				}
			}
			else{
					query_data << 0.0 <<" , ";
					query_data << 0.0 <<" , ";
			}
	
			query_data << "ST_GeomFromText('"<< buff_str<<"')";

			query_data << " ) ,";

			glob_id++;
			counter++;
			if(counter==100){
				query_total<< query_header << query_data.str();
				//delete last komma
				string buff=query_total.str();
				buff.erase(buff.length()-1);
				Data_Base::database_request(&query_buff, buff, &this->qsqldatabase);
				//delete them
				query_total.str("");
				query_data.str("");
				counter=0;
				if(query_buff.lastError().isValid()){
					Warning msg=this->set_warning(13);
					ostringstream info;
					info << "Table Name                : " << Risk_System::table_results_elem_ecn->get_table_name() << endl;
					info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
					msg.make_second_info(info.str());
					msg.output_msg(5);	
				}
			}
		}
		//send the rest
		if(counter!=0){
			query_total<< query_header << query_data.str();
			//delete last komma
			string buff=query_total.str();
			buff.erase(buff.length()-1);
			Data_Base::database_request(&query_buff, buff, &this->qsqldatabase);
			//delete them
			query_total.str("");
			query_data.str("");
			counter=0;
			if(query_buff.lastError().isValid()){
				Warning msg=this->set_warning(13);
				ostringstream info;
				info << "Table Name                : " << Risk_System::table_results_elem_ecn->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(5);	
			}
		}
	}
}
//Calculate, generate and insert to database table the detailed risk result of the ecological biotope-type damage elements
void Risk_System::calc_detailed_result_element_eco_btype(_risk_risk_results *total){
	string query_header;
	query_header=this->generate_sql_header_string_elem_eco_btype();
	int glob_id=Risk_System::table_results_elem_eco_btype->maximum_int_of_column(Risk_System::table_results_elem_eco_btype->get_column_name(label::glob_id), &this->qsqldatabase)+1;

	QSqlQueryModel model;
	int number=0;
	number=Dam_Eco_Btype_Element::select_data_in_erg_table(&model,&this->qsqldatabase, this->system_id, total->dam_results.id_hyd_bound_sc, total->dam_results.id_break_sc);


	ostringstream cout;
	cout << "Transfer "<<number<<" detailed ecological biotope-type risk results of the elements to database..."<< endl;
	Sys_Common_Output::output_risk->output_txt(&cout);




	if(number!=0){
		//Set the query
		QSqlQuery query_buff(this->qsqldatabase);
		int raster=0;
		int elem=0;
		double buff_cost=0.0;
		double prob_total=total->prob_break_sc*total->prob_hyd_bound_sc*total->reocc_prob_hyd_bound_sc;
		string buff_str;

		int counter=0;

		ostringstream query_data;
		ostringstream query_total;


		for(int i=0; i< number; i++){
			if(i%10000==0 && i>0){
				cout << i <<" ("<<number<<") detailed risk results of ecological biotope-type raster elements are transfered to database..."<< endl;
				Sys_Common_Output::output_risk->output_txt(&cout);
				Risk_System::check_stop_thread_flag();
			}

			raster=model.record(i).value((Dam_Eco_Btype_Element::elem_erg_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			elem=model.record(i).value((Dam_Eco_Btype_Element::elem_erg_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
			buff_cost=model.record(i).value((Dam_Eco_Btype_Element::elem_erg_table->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
			buff_str=model.record(i).value("ST_ASTEXT").toString().toStdString();

			query_data << " ( ";
			query_data << glob_id<<" , ";
			query_data << raster<<" , ";
			query_data << elem<<" , ";
			query_data << this->system_id.area_state<<" , ";
			query_data << this->system_id.measure_nr<<" , ";
			query_data << total->dam_results.id_hyd_bound_sc<<" , ";
			query_data << " '"<< total->dam_results.id_break_sc<<"' , ";
			query_data << " '"<< Risk_Break_Scenario::convert_risk_type2txt(this->risk_type)<<"' , ";

			//data
			query_data << buff_cost*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff_cost*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}

			if(this->risk_type!=_risk_type::scenario_risk){
				if((total->dam_results.eco_biotype)>0.0){
					query_data << (buff_cost*prob_total)/(total->dam_results.eco_biotype)<<" , ";
				}
				else{
					query_data << 0.0 <<" , ";
				}
			}
			else{
					query_data << 0.0 <<" , ";
			}
	
			query_data << "ST_GeomFromText('"<< buff_str<<"')";

			query_data << " ) ,";

			glob_id++;
			counter++;
			if(counter==100){
				query_total<< query_header << query_data.str();
				//delete last komma
				string buff=query_total.str();
				buff.erase(buff.length()-1);
				Data_Base::database_request(&query_buff, buff, &this->qsqldatabase);
				//delete them
				query_total.str("");
				query_data.str("");
				counter=0;
				if(query_buff.lastError().isValid()){
					Warning msg=this->set_warning(13);
					ostringstream info;
					info << "Table Name                : " << Risk_System::table_results_elem_eco_btype->get_table_name() << endl;
					info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
					msg.make_second_info(info.str());
					msg.output_msg(5);	
				}
			}
		}
		//send the rest
		if(counter!=0){
			query_total<< query_header << query_data.str();
			//delete last komma
			string buff=query_total.str();
			buff.erase(buff.length()-1);
			Data_Base::database_request(&query_buff, buff, &this->qsqldatabase);
			//delete them
			query_total.str("");
			query_data.str("");
			counter=0;
			if(query_buff.lastError().isValid()){
				Warning msg=this->set_warning(13);
				ostringstream info;
				info << "Table Name                : " << Risk_System::table_results_elem_eco_btype->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(5);	
			}
		}
	}
}
//Calculate, generate and insert to database table the detailed risk result of the ecological soil-erosion damage elements
void Risk_System::calc_detailed_result_element_eco_soil(_risk_risk_results *total){
	string query_header;
	query_header=this->generate_sql_header_string_elem_eco_soil();
	int glob_id=Risk_System::table_results_elem_eco_soil->maximum_int_of_column(Risk_System::table_results_elem_eco_soil->get_column_name(label::glob_id), &this->qsqldatabase)+1;

	QSqlQueryModel model;
	int number=0;
	number=Dam_Eco_Soil_Element::select_data_in_erg_table(&model,&this->qsqldatabase, this->system_id, total->dam_results.id_hyd_bound_sc, total->dam_results.id_break_sc);


	ostringstream cout;
	cout << "Transfer "<<number<<" detailed ecological soil-erosion risk results of the elements to database..."<< endl;
	Sys_Common_Output::output_risk->output_txt(&cout);




	if(number!=0){
		//Set the query
		QSqlQuery query_buff(this->qsqldatabase);
		int raster=0;
		int elem=0;
		double buff_cost=0.0;
		double prob_total=total->prob_break_sc*total->prob_hyd_bound_sc*total->reocc_prob_hyd_bound_sc;
		string buff_str;

		int counter=0;

		ostringstream query_data;
		ostringstream query_total;


		for(int i=0; i< number; i++){
			if(i%10000==0 && i>0){
				cout << i <<" ("<<number<<") detailed risk results of ecological soil-erosion raster elements are transfered to database..."<< endl;
				Sys_Common_Output::output_risk->output_txt(&cout);
				Risk_System::check_stop_thread_flag();
			}

			raster=model.record(i).value((Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			elem=model.record(i).value((Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
			buff_cost=model.record(i).value((Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
			buff_str=model.record(i).value("ST_ASTEXT").toString().toStdString();

			query_data << " ( ";
			query_data << glob_id<<" , ";
			query_data << raster<<" , ";
			query_data << elem<<" , ";
			query_data << this->system_id.area_state<<" , ";
			query_data << this->system_id.measure_nr<<" , ";
			query_data << total->dam_results.id_hyd_bound_sc<<" , ";
			query_data << " '"<< total->dam_results.id_break_sc<<"' , ";
			query_data << " '"<< Risk_Break_Scenario::convert_risk_type2txt(this->risk_type)<<"' , ";

			//data
			query_data << buff_cost*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff_cost*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}

			if(this->risk_type!=_risk_type::scenario_risk){
				if((total->dam_results.eco_soil_erosion)>0.0){
					query_data << (buff_cost*prob_total)/(total->dam_results.eco_soil_erosion)<<" , ";
				}
				else{
					query_data << 0.0 <<" , ";
				}
			}
			else{
					query_data << 0.0 <<" , ";
			}
	
			query_data << "ST_GeomFromText('"<< buff_str<<"')";

			query_data << " ) ,";

			glob_id++;
			counter++;
			if(counter==100){
				query_total<< query_header << query_data.str();
				//delete last komma
				string buff=query_total.str();
				buff.erase(buff.length()-1);
				Data_Base::database_request(&query_buff, buff, &this->qsqldatabase);
				//delete them
				query_total.str("");
				query_data.str("");
				counter=0;
				if(query_buff.lastError().isValid()){
					Warning msg=this->set_warning(13);
					ostringstream info;
					info << "Table Name                : " << Risk_System::table_results_elem_eco_soil->get_table_name() << endl;
					info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
					msg.make_second_info(info.str());
					msg.output_msg(5);	
				}
			}
		}
		//send the rest
		if(counter!=0){
			query_total<< query_header << query_data.str();
			//delete last komma
			string buff=query_total.str();
			buff.erase(buff.length()-1);
			Data_Base::database_request(&query_buff, buff, &this->qsqldatabase);
			//delete them
			query_total.str("");
			query_data.str("");
			counter=0;
			if(query_buff.lastError().isValid()){
				Warning msg=this->set_warning(13);
				ostringstream info;
				info << "Table Name                : " << Risk_System::table_results_elem_eco_soil->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(5);	
			}
		}
	}
}
//Calculate, generate and insert to database table the detailed risk result of the people2risk  damage elements
void Risk_System::calc_detailed_result_element_pop(_risk_risk_results *total){
	

	string query_header;
	query_header=this->generate_sql_header_string_elem_pop();
	int glob_id=Risk_System::table_results_elem_pop->maximum_int_of_column(Risk_System::table_results_elem_pop->get_column_name(label::glob_id), &this->qsqldatabase)+1;

	QSqlQueryModel model;
	int number=0;
	number=Dam_People_Element::select_data_in_erg_table(&model,&this->qsqldatabase, this->system_id, total->dam_results.id_hyd_bound_sc, total->dam_results.id_break_sc);


	ostringstream cout;
	cout << "Transfer "<<number<<" detailed people2risk risk results of the elements to database..."<< endl;
	Sys_Common_Output::output_risk->output_txt(&cout);




	if(number!=0){
		//Set the query
		QSqlQuery query_buff(this->qsqldatabase);
		int raster=0;
		int elem=0;
		double buff_affected=0.0;
		double buff_endangered=0.0;
		double prob_total=total->prob_break_sc*total->prob_hyd_bound_sc*total->reocc_prob_hyd_bound_sc;
		string buff_str;

		int counter=0;

		ostringstream query_data;
		ostringstream query_total;


		for(int i=0; i< number; i++){
			if(i%10000==0 && i>0){
				cout << i <<" ("<<number<<") detailed risk results of people2risk raster elements are transfered to database..."<< endl;
				Sys_Common_Output::output_risk->output_txt(&cout);
				Risk_System::check_stop_thread_flag();
			}

			raster=model.record(i).value((Dam_People_Element::elem_erg_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			elem=model.record(i).value((Dam_People_Element::elem_erg_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
			buff_affected=model.record(i).value((Dam_People_Element::elem_erg_table->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
			buff_endangered=model.record(i).value((Dam_People_Element::elem_erg_table->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
			buff_str=model.record(i).value("ST_ASTEXT").toString().toStdString();

			query_data << " ( ";
			query_data << glob_id<<" , ";
			query_data << raster<<" , ";
			query_data << elem<<" , ";
			query_data << this->system_id.area_state<<" , ";
			query_data << this->system_id.measure_nr<<" , ";
			query_data << total->dam_results.id_hyd_bound_sc<<" , ";
			query_data << " '"<< total->dam_results.id_break_sc<<"' , ";
			query_data << " '"<< Risk_Break_Scenario::convert_risk_type2txt(this->risk_type)<<"' , ";

			//data
			query_data << buff_affected*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff_affected*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			if(this->risk_type!=_risk_type::scenario_risk){
				if((total->dam_results.pop_affected)>0.0){
					query_data << (buff_affected*prob_total)/(total->dam_results.pop_affected)<<" , ";
				}
				else{
					query_data << 0.0 <<" , ";
				}

			}
			else{
				query_data << 0.0 <<" , ";
			}

			query_data << buff_endangered*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff_endangered*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			if(this->risk_type!=_risk_type::scenario_risk){
				if((total->dam_results.pop_endangered)>0.0){
					query_data << (buff_endangered*prob_total)/(total->dam_results.pop_endangered)<<" , ";
				}
				else{
					query_data << 0.0 <<" , ";
				}

			}
			else{
				query_data << 0.0 <<" , ";
			}
	
			query_data << "ST_GeomFromText('"<< buff_str<<"')";

			query_data << " ) ,";

			glob_id++;
			counter++;
			if(counter==100){
				query_total<< query_header << query_data.str();
				//delete last komma
				string buff=query_total.str();
				buff.erase(buff.length()-1);
				Data_Base::database_request(&query_buff, buff, &this->qsqldatabase);
				//delete them
				query_total.str("");
				query_data.str("");
				counter=0;
				if(query_buff.lastError().isValid()){
					Warning msg=this->set_warning(13);
					ostringstream info;
					info << "Table Name                : " << Risk_System::table_results_elem_pop->get_table_name() << endl;
					info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
					msg.make_second_info(info.str());
					msg.output_msg(5);	
				}
			}
		}
		//send the rest
		if(counter!=0){
			query_total<< query_header << query_data.str();
			//delete last komma
			string buff=query_total.str();
			buff.erase(buff.length()-1);
			Data_Base::database_request(&query_buff, buff, &this->qsqldatabase);
			//delete them
			query_total.str("");
			query_data.str("");
			counter=0;
			if(query_buff.lastError().isValid()){
				Warning msg=this->set_warning(13);
				ostringstream info;
				info << "Table Name                : " << Risk_System::table_results_elem_pop->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(5);	
			}
		}
	}
}
//Calculate, generate and insert to database table the detailed risk result of the psycho-social damage elements
void Risk_System::calc_detailed_result_element_pys(_risk_risk_results *total){
	string query_header;
	query_header=this->generate_sql_header_string_elem_pys();
	int glob_id=Risk_System::table_results_elem_pys->maximum_int_of_column(Risk_System::table_results_elem_pys->get_column_name(label::glob_id), &this->qsqldatabase)+1;

	QSqlQueryModel model;
	int number=0;
	number=Dam_Pys_Element::select_data_in_erg_table(&model,&this->qsqldatabase, this->system_id, total->dam_results.id_hyd_bound_sc, total->dam_results.id_break_sc);


	ostringstream cout;
	cout << "Transfer "<<number<<" detailed pyscho-social risk results of the elements to database..."<< endl;
	Sys_Common_Output::output_risk->output_txt(&cout);




	if(number!=0){
		//Set the query
		QSqlQuery query_buff(this->qsqldatabase);
		int raster=0;
		int elem=0;
		_dam_damage_results buff;
		double sum=0.0;
		double sum_total=0.0;
		double prob_total=total->prob_break_sc*total->prob_hyd_bound_sc*total->reocc_prob_hyd_bound_sc;
		string buff_str;

		int counter=0;

		ostringstream query_data;
		ostringstream query_total;


		for(int i=0; i< number; i++){
			if(i%10000==0 && i>0){
				cout << i <<" ("<<number<<") detailed risk results of pyscho-social raster elements are transfered to database..."<< endl;
				Sys_Common_Output::output_risk->output_txt(&cout);
				Risk_System::check_stop_thread_flag();
			}

			raster=model.record(i).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			elem=model.record(i).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
			buff.pys_age_50_59=model.record(i).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
			buff.pys_age_80=model.record(i).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
			buff.pys_female=model.record(i).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_female)).c_str()).toDouble();
			buff.pys_female=model.record(i).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_owner)).c_str()).toDouble();
			buff.pys_age_50_59_dens=model.record(i).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
			buff.pys_age_80_dens=model.record(i).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble();
			buff.pys_female_dens=model.record(i).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_female)).c_str()).toDouble();
			buff.pys_owner_dens=model.record(i).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble();

			buff_str=model.record(i).value("ST_ASTEXT").toString().toStdString();

			query_data << " ( ";
			query_data << glob_id<<" , ";
			query_data << raster<<" , ";
			query_data << elem<<" , ";
			query_data << this->system_id.area_state<<" , ";
			query_data << this->system_id.measure_nr<<" , ";
			query_data << total->dam_results.id_hyd_bound_sc<<" , ";
			query_data << " '"<< total->dam_results.id_break_sc<<"' , ";
			query_data << " '"<< Risk_Break_Scenario::convert_risk_type2txt(this->risk_type)<<"' , ";

			//data
			query_data << buff.pys_age_50_59*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff.pys_age_50_59*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			query_data << buff.pys_age_80*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff.pys_age_80*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			query_data << buff.pys_female*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff.pys_female*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			query_data << buff.pys_owner*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff.pys_owner*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}

			sum=buff.pys_owner+buff.pys_female+buff.pys_age_50_59+buff.pys_age_80;
			sum_total=total->dam_results.pys_age_50_59+total->dam_results.pys_age_80+
				total->dam_results.pys_female+total->dam_results.pys_owner;

			query_data << sum*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << sum*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			if(this->risk_type!=_risk_type::scenario_risk){
				if((sum_total)>0.0){
					query_data << (sum*prob_total)/(sum_total)<<" , ";
				}
				else{
					query_data << 0.0 <<" , ";
				}

			}
			else{
				query_data << 0.0 <<" , ";
			}

			//with density
			query_data << buff.pys_age_50_59_dens*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff.pys_age_50_59_dens*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			query_data << buff.pys_age_80_dens*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff.pys_age_80_dens*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			query_data << buff.pys_female_dens*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff.pys_female_dens*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			query_data << buff.pys_owner_dens*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff.pys_owner_dens*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}

			sum=buff.pys_owner_dens+buff.pys_female_dens+buff.pys_age_50_59_dens+buff.pys_age_80_dens;
			sum_total=total->dam_results.pys_age_50_59_dens+total->dam_results.pys_age_80_dens+
				total->dam_results.pys_female_dens+total->dam_results.pys_owner_dens;

			query_data << sum*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << sum*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			if(this->risk_type!=_risk_type::scenario_risk){
				if((sum_total)>0.0){
					query_data << (sum*prob_total)/(sum_total)<<" , ";
				}
				else{
					query_data << 0.0 <<" , ";
				}

			}
			else{
				query_data << 0.0 <<" , ";
			}

			query_data << "ST_GeomFromText('"<< buff_str<<"')";

			query_data << " ) ,";

			glob_id++;
			counter++;
			if(counter==100){
				query_total<< query_header << query_data.str();
				//delete last komma
				string buff=query_total.str();
				buff.erase(buff.length()-1);
				Data_Base::database_request(&query_buff, buff, &this->qsqldatabase);
				//delete them
				query_total.str("");
				query_data.str("");
				counter=0;
				if(query_buff.lastError().isValid()){
					Warning msg=this->set_warning(13);
					ostringstream info;
					info << "Table Name                : " << Risk_System::table_results_elem_pys->get_table_name() << endl;
					info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
					msg.make_second_info(info.str());
					msg.output_msg(5);	
				}
			}
		}
		//send the rest
		if(counter!=0){
			query_total<< query_header << query_data.str();
			//delete last komma
			string buff=query_total.str();
			buff.erase(buff.length()-1);
			Data_Base::database_request(&query_buff, buff, &this->qsqldatabase);
			//delete them
			query_total.str("");
			query_data.str("");
			counter=0;
			if(query_buff.lastError().isValid()){
				Warning msg=this->set_warning(13);
				ostringstream info;
				info << "Table Name                : " << Risk_System::table_results_elem_pys->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(5);	
			}
		}
	}
}
//Calculate, generate and insert to database table the detailed risk result of the simple counting points
void Risk_System::calc_detailed_result_point_sc(_risk_risk_results *total){
	string query_header;
	query_header=this->generate_sql_header_string_point_sc();
	int glob_id=Risk_System::table_results_point_sc->maximum_int_of_column(Risk_System::table_results_point_sc->get_column_name(label::glob_id), &this->qsqldatabase)+1;

	QSqlQueryModel model;
	int number=0;
	number=Dam_Sc_Point::select_data_in_erg_table(&model,&this->qsqldatabase, this->system_id, total->dam_results.id_hyd_bound_sc, total->dam_results.id_break_sc);


	ostringstream cout;
	cout << "Transfer "<<number<<" detailed simple counting risk results of the points to database..."<< endl;
	Sys_Common_Output::output_risk->output_txt(&cout);




	if(number!=0){
		//Set the query
		QSqlQuery query_buff(this->qsqldatabase);
		int elem=0;
		_dam_damage_results buff;
		double sum=0.0;
		double sum_total=0.0;
		double prob_total=total->prob_break_sc*total->prob_hyd_bound_sc*total->reocc_prob_hyd_bound_sc;
		string buff_str;

		int counter=0;

		ostringstream query_data;
		ostringstream query_total;

		int cat_buff=0;
		double value_buff=0.0;

		sum_total=total->dam_results.sc_cult_build+total->dam_results.sc_eco_build+
				total->dam_results.sc_person_build+total->dam_results.sc_pub_build;

		for(int i=0; i< number; i++){
			buff.sc_pub_build=0.0;
			buff.sc_eco_build=0.0;
			buff.sc_cult_build=0.0;
			buff.sc_person_build=0.0;

			if(i%10000==0 && i>0){
				cout << i <<" ("<<number<<") detailed risk results of simple counting points are transfered to database..."<< endl;
				Sys_Common_Output::output_risk->output_txt(&cout);
				Risk_System::check_stop_thread_flag();
			}

			elem=model.record(i).value((Dam_Sc_Point::point_erg_table->get_column_name(dam_label::point_id)).c_str()).toInt();
			cat_buff=model.record(i).value((Dam_Sc_Point::point_erg_table->get_column_name(dam_label::category_id)).c_str()).toInt();
			value_buff=model.record(i).value((Dam_Sc_Point::point_erg_table->get_column_name(dam_label::affected_score)).c_str()).toDouble();
			
			if(cat_buff==1){
				buff.sc_pub_build=value_buff;
			}
			else if(cat_buff==2){
				buff.sc_eco_build=value_buff;
			}
			else if(cat_buff==3){
				buff.sc_cult_build=value_buff;
			}
			else if(cat_buff==4){
				buff.sc_person_build=value_buff;
			}
			
			buff_str=model.record(i).value("ST_ASTEXT").toString().toStdString();

			query_data << " ( ";
			query_data << glob_id<<" , ";
			query_data << elem<<" , ";
			query_data << this->system_id.area_state<<" , ";
			query_data << this->system_id.measure_nr<<" , ";
			query_data << total->dam_results.id_hyd_bound_sc<<" , ";
			query_data << " '"<< total->dam_results.id_break_sc<<"' , ";
			query_data << " '"<< Risk_Break_Scenario::convert_risk_type2txt(this->risk_type)<<"' , ";

			//data
			query_data << buff.sc_pub_build*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff.sc_pub_build*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			query_data << buff.sc_eco_build*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff.sc_eco_build*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			query_data << buff.sc_cult_build*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff.sc_cult_build*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}
			query_data << buff.sc_person_build*total->prob_break_sc<<" , ";
			if(this->risk_type!=_risk_type::scenario_risk){
				query_data << buff.sc_person_build*prob_total<<" , ";
			}
			else{
				query_data << 0.0 <<" , ";
			}

			sum=buff.sc_cult_build+buff.sc_eco_build+buff.sc_person_build+buff.sc_pub_build;


			if(this->risk_type!=_risk_type::scenario_risk){
				if(cat_buff==1){
					sum_total=total->dam_results.sc_pub_build;
				}
				else if(cat_buff==2){
					sum_total=total->dam_results.sc_eco_build;
				}
				else if(cat_buff==3){
					sum_total=total->dam_results.sc_cult_build;
				}
				else if(cat_buff==4){
					sum_total=total->dam_results.sc_person_build;
				}
				
				if((sum_total)>0.0){
					query_data << (sum*prob_total)/(sum_total)<<" , ";
				}
				else{
					query_data << 0.0 <<" , ";
				}

			}
			else{
				query_data << 0.0 <<" , ";
			}

			query_data << "ST_GeomFromText('"<< buff_str<<"')";

			query_data << " ) ,";

			glob_id++;
			counter++;
			if(counter==100){
				query_total<< query_header << query_data.str();
				//delete last komma
				string buff=query_total.str();
				buff.erase(buff.length()-1);
				Data_Base::database_request(&query_buff, buff, &this->qsqldatabase);
				//delete them
				query_total.str("");
				query_data.str("");
				counter=0;
				if(query_buff.lastError().isValid()){
					Warning msg=this->set_warning(14);
					ostringstream info;
					info << "Table Name                : " << Risk_System::table_results_point_sc->get_table_name() << endl;
					info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
					msg.make_second_info(info.str());
					msg.output_msg(5);	
				}
			}
		}
		//send the rest
		if(counter!=0){
			query_total<< query_header << query_data.str();
			//delete last komma
			string buff=query_total.str();
			buff.erase(buff.length()-1);
			Data_Base::database_request(&query_buff, buff, &this->qsqldatabase);
			//delete them
			query_total.str("");
			query_data.str("");
			counter=0;
			if(query_buff.lastError().isValid()){
				Warning msg=this->set_warning(14);
				ostringstream info;
				info << "Table Name                : " << Risk_System::table_results_point_sc->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(5);	
			}
		}
	}

}
//Check if there are data in one of the detailed risk results element table
bool Risk_System::check_detailed_risk_element_results(const string break_id, const int hyd_bound_id){

	if(Risk_System::check_data_in_resulttable_elem_ecn(&this->qsqldatabase, this->system_id, this->risk_type, break_id, hyd_bound_id)==true){
		return true;
	}
	if(Risk_System::check_data_in_resulttable_elem_eco_btype(&this->qsqldatabase, this->system_id, this->risk_type, break_id, hyd_bound_id)==true){
		return true;
	}
	if(Risk_System::check_data_in_resulttable_elem_eco_soil(&this->qsqldatabase, this->system_id, this->risk_type, break_id, hyd_bound_id)==true){
		return true;
	}
	if(Risk_System::check_data_in_resulttable_elem_pop(&this->qsqldatabase, this->system_id, this->risk_type, break_id, hyd_bound_id)==true){
		return true;
	}
	if(Risk_System::check_data_in_resulttable_elem_pys(&this->qsqldatabase, this->system_id, this->risk_type, break_id, hyd_bound_id)==true){
		return true;
	}
	if(Risk_System::check_data_in_resulttable_point_sc(&this->qsqldatabase, this->system_id, this->risk_type, break_id, hyd_bound_id)==true){
		return true;
	}

	return false;

}
//Generate sql-header string for inserting data to the database table for detailed results per damage raster-element of the economical damage raster 
string Risk_System::generate_sql_header_string_elem_ecn(void){
	string buffer;
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_ecn(&this->qsqldatabase);
	}
	catch(Error msg){
		throw msg;
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Risk_System::table_results_elem_ecn->get_table_name();
	query_string <<" ( ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(label::areastate_id) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(label::measure_id) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(risk_label::sz_break_id) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(risk_label::risk_type) <<" , ";
	
	//data
	query_string << Risk_System::table_results_elem_ecn->get_column_name(dam_label::immob_dam) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(risk_label::risk_immob_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(dam_label::mob_dam) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(risk_label::risk_mob_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(dam_label::total_dam) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(risk_label::risk_ecn_total_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_ecn->get_column_name(risk_label::risk_ecn_total_fac) <<" , ";

	query_string << Risk_System::table_results_elem_ecn->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ";

	buffer=query_string.str();

	return buffer;

}
//Generate sql-header string for inserting data to the database table for detailed results per damage raster-element of the ecological biotope-type damage raster 
string Risk_System::generate_sql_header_string_elem_eco_btype(void){
		string buffer;
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_eco_btype(&this->qsqldatabase);
	}
	catch(Error msg){
		throw msg;
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Risk_System::table_results_elem_eco_btype->get_table_name();
	query_string <<" ( ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(label::areastate_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(label::measure_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(risk_label::sz_break_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(risk_label::risk_type) <<" , ";
	

	//data
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(dam_label::btype_cost) <<" , ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(risk_label::risk_btype_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(risk_label::risk_btype_fac) <<" , ";

	query_string << Risk_System::table_results_elem_eco_btype->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ";

	buffer=query_string.str();

	return buffer;
}
//Generate sql-header string for inserting data to the database table for detailed results per damage raster-element of the ecological soil-erosion damage raster 
string Risk_System::generate_sql_header_string_elem_eco_soil(void){
	string buffer;
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_eco_soil(&this->qsqldatabase);
	}
	catch(Error msg){
		throw msg;
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Risk_System::table_results_elem_eco_soil->get_table_name();
	query_string <<" ( ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(label::areastate_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(label::measure_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(risk_label::sz_break_id) <<" , ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(risk_label::risk_type) <<" , ";
	

	//data
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(dam_label::soil_cost) <<" , ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(risk_label::risk_soil_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(risk_label::risk_soil_fac) <<" , ";

	query_string << Risk_System::table_results_elem_eco_soil->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ";

	buffer=query_string.str();

	return buffer;
}
//Generate sql-header string for inserting data to the database table for detailed results per damage raster-element of the people2risk damage raster 
string Risk_System::generate_sql_header_string_elem_pop(void){
	string buffer;
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_pop(&this->qsqldatabase);
	}
	catch(Error msg){
		throw msg;
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Risk_System::table_results_elem_pop->get_table_name();
	query_string <<" ( ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(label::areastate_id) <<" , ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(label::measure_id) <<" , ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(risk_label::sz_break_id) <<" , ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(risk_label::risk_type) <<" , ";
	
	//data
	query_string << Risk_System::table_results_elem_pop->get_column_name(dam_label::pop_affected) <<" , ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(risk_label::risk_pop_aff_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(risk_label::risk_pop_aff_fac) <<" , ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(dam_label::pop_endangered) <<" , ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(risk_label::risk_pop_dan_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_pop->get_column_name(risk_label::risk_pop_dan_fac) <<" , ";

	query_string << Risk_System::table_results_elem_pop->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ";

	buffer=query_string.str();

	return buffer;
}
//Generate sql-header string for inserting data to the database table for detailed results per damage raster-element of the psycho-social damage raster
string Risk_System::generate_sql_header_string_elem_pys(void){
		string buffer;
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_elem_pys(&this->qsqldatabase);
	}
	catch(Error msg){
		throw msg;
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Risk_System::table_results_elem_pys->get_table_name();
	query_string <<" ( ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(label::areastate_id) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(label::measure_id) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::sz_break_id) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_type) <<" , ";


	//data
	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::crit_age_50_59) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_crit_age_50_59_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::crit_age_80) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_crit_age_80_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::crit_female) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_crit_female_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::crit_owner) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_crit_owner_with_hyd) <<" , ";

	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::total_score) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_pys_total_score) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_pys_total_fac) <<" , ";

	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::denscrit_age50_59) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_dens_crit_age_50_59_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::denscrit_age_80) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_dens_crit_age_80_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::denscrit_female) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_dens_crit_female_with_hyd) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::denscrit_owner) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_dens_crit_owner_with_hyd) <<" , ";

	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::total_dens_score) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_pys_total_dens_score) <<" , ";
	query_string << Risk_System::table_results_elem_pys->get_column_name(risk_label::risk_pys_total_dens_fac) <<" , ";

	query_string << Risk_System::table_results_elem_pys->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ";

	buffer=query_string.str();

	return buffer;
}
//Generate sql-header string for inserting data to the database table for detailed results per damage point of the simple counting damage 
string Risk_System::generate_sql_header_string_point_sc(void){
	string buffer;
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_result_point_sc(&this->qsqldatabase);
	}
	catch(Error msg){
		throw msg;
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Risk_System::table_results_point_sc->get_table_name();
	query_string <<" ( ";
	query_string << Risk_System::table_results_point_sc->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(dam_label::point_id) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(label::areastate_id) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(label::measure_id) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(risk_label::sz_break_id) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(risk_label::risk_type) <<" , ";


	//data
	query_string << Risk_System::table_results_point_sc->get_column_name(dam_label::sc_pub_build) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(risk_label::risk_sc_pub_build_with_hyd) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(dam_label::sc_eco_build) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(risk_label::risk_sc_eco_build_with_hyd) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(dam_label::sc_cult_build) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(risk_label::risk_sc_cult_build_with_hyd) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(dam_label::sc_person_build) <<" , ";
	query_string << Risk_System::table_results_point_sc->get_column_name(risk_label::risk_sc_person_build_with_hyd) <<" , ";


	query_string << Risk_System::table_results_point_sc->get_column_name(risk_label::risk_sc_fac) <<" , ";


	query_string << Risk_System::table_results_point_sc->get_column_name(dam_label::sc_point) <<" ) ";

	query_string << " VALUES ";

	buffer=query_string.str();

	return buffer;
}
//Set warning(s)
Warning Risk_System::set_warning(const int warn_type){
	string place="Risk_System::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://the type of thread is not specified 
			place.append("run(void)") ;
			reason="The thread type is unknown";
			reaction="No thread is launched";
			help= "Check the source code";
			type=5;	
		case 1://breakmax_hydraulic < 10
			place.append("check_control_params(void)") ;
			reason="Maximum of hydraulic calculation is < 10";
			reaction="The default value is taken";
			help= "Check the parameter MAX_HYDRAULIC_SIM";
			info << "Default value : " << risk_control_param::max_hydraulic_def << endl;
			type=21;	
			break;
		case 2://boundary prob must between 0.0 and 1.0
			place.append("check_control_params(void)") ;
			reason="The boundary of the probability must be between 0.0 and 1.0";
			reaction="The default value is taken";
			help= "Check the parameter BOUNDARY_PROBABILITY";
			info << "Default value : " << risk_control_param::boundary_prob_def << endl;
			type=21;	
			break;
		case 3://maximal mc runs is exceeded
			place.append("check_control_params(void)") ;
			reason="The number of monte-carlo runs exceeds the maximal number of runs";
			reaction="The default value is taken";
			help= "Check the parameter NUMBER_MC";
			info << "Maximal value : " << risk_control_param::max_mc_def<< endl;
			info << "Default value : " << risk_control_param::no_mc_def<< endl;
			type=21;	
			break;
		case 4://minimal mc-runs is under-run
			place.append("check_control_params(void)") ;
			reason="The number of monte-carlo runs under-runs the minimal number of runs";
			reaction="The default value is taken";
			help= "Check the parameter NUMBER_MC";
			info << "Minimal value : " << risk_control_param::min_mc_def<< endl;
			info << "Default value : " << risk_control_param::no_mc_def<< endl;
			type=21;	
			break;
		case 5://no damages found to the given hydraulic boundary scenario
			place.append("set_connect_dam_prob2detailed_result_nobreak(void)") ;
			reason="No damage results are found in database to the given hydraulic boundary scenario";
			reaction="All damages are set to zero";
			help= "Check the damage result table and the hydraulic boundary scenario table";
			type=22;	
			break;
		case 6://no maximum outflow discharge is found 
			place.append("set_connect_dam_prob2detailed_result_nobreak(void)") ;
			reason="No maximum outflow discharge is found in database to the given hydraulic boundary scenario";
			reaction="Maximum outflow discharge is set to zero";
			help= "Check the river maximum result tables and the hydraulic boundary scenario table";
			type=22;	
			break;
		case 7://results datas can not submitted
			place.append("output_detailed_risk_results2database(void)");
			reason="Can not submit the detailed risk results data to the database";
			help="Check the database";
			type=2;
			break;
		case 8://results datas can not submitted
			place.append("output_cumulated_risk_results2database(void)");
			reason="Can not submit the cumulated risk results data to the database";
			help="Check the database";
			type=2;
			break;
		case 9://no river is set
			place.append("set_connect_dam_prob2detailed_result_nobreak(void)");
			reason="No relevant river id is set for the calculation of the maximum outflow";
			reaction="Maximum outflow discharge is set to zero";
			help="Check the risk setting of the river id";
			type=22;
			break;
		case 10://no river is set
			place.append("set_connect_dam_prob2detailed_result_break(void)");
			reason="No relevant river id is set for the calculation of the maximum outflow";
			reaction="Maximum outflow discharge is set to zero";
			help="Check the risk setting of the river id";
			type=21;
			break;
		case 11://number of frc intervals is under-run
			place.append("check_control_params(void)") ;
			reason="The minimal number of intervals for the frc-curve division have to be 2";
			reaction="The minimal value of 2 intervals is taken";
			help= "Check the parameter NUMBER_FRC_INTERVALS";
			type=21;	
			break;
		case 12://no maximum outflow discharge is found 
			place.append("set_connect_dam_prob2detailed_result_break(void)") ;
			reason="No maximum outflow discharge is found in database to the given break scenario";
			reaction="Maximum outflow discharge is set to zero";
			help= "Check the river maximum result tables";
			type=21;	
			break;
		case 13://input datas can not submitted
			place.append("calc_detailed_result_element_...(_risk_risk_results *total)");
			reason="Can not submit the element risk result data of the damage raster to the database";
			help="Check the database";
			type=2;
			break;
		case 14://input datas can not submitted
			place.append("calc_detailed_result_point_sc(_risk_risk_results *total)");
			reason="Can not submit the point risk result data of the damage to the database";
			help="Check the database";
			type=2;
			break;
		default:
			place.append("set_warning(const int warn_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;
}
//Set error(s)
Error Risk_System::set_error(const int err_type){
	string place="Risk_System::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_..._system(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("allocate_..._results(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://no damages found to the given hydraulic boundary scenario
			place.append("set_connect_dam_prob2detailed_result_break(void)") ;
			reason="No damage results are found in database to the given break scenario. Can not calculate the risk";
			help= "Check the damage result table";
			type=29;	
			break;
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
