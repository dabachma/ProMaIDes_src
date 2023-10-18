#include "Risk_Headers_Precompiled.h"
//#include "Risk_Damage_Predictor.h"

//init static members
Tables *Risk_Damage_Predictor::table_detailed_predicted_results=NULL;

//Default constructor
Risk_Damage_Predictor::Risk_Damage_Predictor(void){
	this->number=0;

	this->ecn_results=NULL;
	this->eco_btype_results=NULL;
	this->eco_soil_results=NULL;
	this->pop_affected_results=NULL;
	this->pop_endangered_results=NULL;
	this->pys_results=NULL;
	this->pys_dens_results=NULL;

	this->break_sc2predict=NULL;

	this->total_pred_ecn_dam=0.0;
	this->total_pred_eco_dam=0.0;
	this->total_pred_pop_aff_dam=0.0;
	this->total_pred_pop_end_dam=0.0;
	this->total_pred_pys_without_dens_dam=0.0;
	this->total_pred_pys_with_dens_dam=0.0;


	Sys_Memory_Count::self()->add_mem(sizeof(Risk_Damage_Predictor),_sys_system_modules::RISK_SYS);//count the memory

}
//Default destructor
Risk_Damage_Predictor::~Risk_Damage_Predictor(void){
	this->delete_dam_result();
	Sys_Memory_Count::self()->minus_mem(sizeof(Risk_Damage_Predictor),_sys_system_modules::RISK_SYS);//count the memory
}
//_________
//public
//Set the database table for detailed predicted results of the risk calculation: it sets the table name and the name of the columns and allocate them (static)
void Risk_Damage_Predictor::set_resulttable_predicted_detailed(QSqlDatabase *ptr_database){
	if(Risk_Damage_Predictor::table_detailed_predicted_results==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_risk_predict_detailed;
		string tab_col[17];
		tab_col[0]=label::glob_id;
		tab_col[1]=label::areastate_id;
		tab_col[2]=label::measure_id;
		tab_col[3]=hyd_label::sz_bound_id;
		tab_col[4]=risk_label::sz_break_id;
		tab_col[5]=risk_label::risk_total_ecn_predict;
		tab_col[6]=risk_label::risk_total_ecn_predict_perc;
		tab_col[7]=risk_label::risk_total_eco_predict;
		tab_col[8]=risk_label::risk_total_eco_predict_perc;
		tab_col[9]=dam_label::pop_affected;
		tab_col[10]=risk_label::risk_pop_affected_perc;
		tab_col[11]=dam_label::pop_endangered;
		tab_col[12]=risk_label::risk_pop_endangered_perc;
		tab_col[13]=dam_label::total_score;
		tab_col[14]=risk_label::risk_total_score_perc;
		tab_col[15]=dam_label::total_dens_score;
		tab_col[16]=risk_label::risk_total_dens_score_perc;


		
		//set the table
		try{
			Risk_Damage_Predictor::table_detailed_predicted_results= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_Damage_Predictor::table_detailed_predicted_results->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_Damage_Predictor::set_resulttable_predicted_detailed(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Risk_Damage_Predictor::close_resulttable_predicted_detailed();
			throw msg;
		}
	}
}
//Create the database table for detailed predicted  results of the risk calculation (static)
void Risk_Damage_Predictor::create_resulttable_predicted_detailed(QSqlDatabase *ptr_database){
	if(Risk_Damage_Predictor::table_detailed_predicted_results==NULL){
			ostringstream cout;
			cout << "Create database table of detailed predicted risk results..." << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			//make specific input for this class
			const string tab_name=risk_label::tab_risk_predict_detailed;
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

			tab_col[4].name=risk_label::sz_break_id;
			tab_col[4].type=sys_label::tab_col_type_string;
			tab_col[4].default_value="";
			tab_col[4].key_flag=true;

			tab_col[5].name=risk_label::risk_total_ecn_predict;
			tab_col[5].type=sys_label::tab_col_type_double;
			tab_col[5].default_value="0.0";
			tab_col[5].unsigned_flag=true;

			tab_col[6].name=risk_label::risk_total_ecn_predict_perc;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].default_value="0.0";
			tab_col[6].unsigned_flag=true;

			tab_col[7].name=risk_label::risk_total_eco_predict;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].default_value="0.0";
			tab_col[7].unsigned_flag=true;

			tab_col[8].name=risk_label::risk_total_eco_predict_perc;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=dam_label::pop_affected;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";
			tab_col[9].unsigned_flag=true;

			tab_col[10].name=risk_label::risk_pop_affected_perc;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].default_value="0.0";
			tab_col[10].unsigned_flag=true;

			tab_col[11].name=dam_label::pop_endangered;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].default_value="0.0";
			tab_col[11].unsigned_flag=true;

			tab_col[12].name=risk_label::risk_pop_endangered_perc;
			tab_col[12].type=sys_label::tab_col_type_double;
			tab_col[12].default_value="0.0";
			tab_col[12].unsigned_flag=true;

			tab_col[13].name=dam_label::total_score;
			tab_col[13].type=sys_label::tab_col_type_double;
			tab_col[13].default_value="0.0";
			tab_col[13].unsigned_flag=true;

			tab_col[14].name=risk_label::risk_total_score_perc;
			tab_col[14].type=sys_label::tab_col_type_double;
			tab_col[14].default_value="0.0";
			tab_col[14].unsigned_flag=true;

			tab_col[15].name=dam_label::total_dens_score;
			tab_col[15].type=sys_label::tab_col_type_double;
			tab_col[15].default_value="0.0";
			tab_col[15].unsigned_flag=true;

			tab_col[16].name=risk_label::risk_total_dens_score_perc;
			tab_col[16].type=sys_label::tab_col_type_double;
			tab_col[16].default_value="0.0";
			tab_col[16].unsigned_flag=true;




			try{
				Risk_Damage_Predictor::table_detailed_predicted_results= new Tables();
				if(Risk_Damage_Predictor::table_detailed_predicted_results->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_risk->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Risk_Damage_Predictor::create_resulttable_predicted_detailed(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Risk_Damage_Predictor::close_resulttable_predicted_detailed();
				throw msg;
			}
			Risk_Damage_Predictor::close_resulttable_predicted_detailed();
		
	}
}
//Delete all data in the database table for detailed predicted results of the risk calculation (static)
void Risk_Damage_Predictor::delete_data_in_resulttable_predicted_detailed(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Damage_Predictor::set_resulttable_predicted_detailed(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Risk_Damage_Predictor::table_detailed_predicted_results->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for detailed predicted results of the risk calculation (static)
void Risk_Damage_Predictor::delete_data_in_resulttable_predicted_detailed(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Damage_Predictor::set_resulttable_predicted_detailed(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_Damage_Predictor::table_detailed_predicted_results->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_Damage_Predictor::delete_data_in_resulttable_predicted_detailed(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Damage_Predictor::table_detailed_predicted_results->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for detailed predicted results of the risk calculation (static)
void Risk_Damage_Predictor::close_resulttable_predicted_detailed(void){
	if(Risk_Damage_Predictor::table_detailed_predicted_results!=NULL){
		delete Risk_Damage_Predictor::table_detailed_predicted_results;
		Risk_Damage_Predictor::table_detailed_predicted_results=NULL;
	}
}
//Select the predicted risk results for the given identifiers from database table (static)
int Risk_Damage_Predictor::select_predicted_risk_result(QSqlQueryModel *model, QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc_id, const string break_sc_id){
	try{
		Risk_Damage_Predictor::set_resulttable_predicted_detailed(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Risk_Damage_Predictor::table_detailed_predicted_results->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND ";
	test_filter << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	test_filter << " AND " ;
	test_filter << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc_id<< " ";
	test_filter << " AND " ;
	test_filter << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::sz_break_id) << " = '" << break_sc_id<< "' ";

	
	Data_Base::database_request(model, test_filter.str(), ptr_database);

	//check the request
	if(model->lastError().isValid()){
		Error msg;
		msg.set_msg("Risk_Damage_Predictor::select_predicted_risk_result(QSqlQueryModel *model, QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc_id, const string break_sc_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Damage_Predictor::table_detailed_predicted_results->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return model->rowCount();
}
//Update the database table of the predicted damages with the percentage (static)
void Risk_Damage_Predictor::update_predicted_result_by_percentage(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const string break_sc, _risk_predicted_risk predict){
	try{
		Risk_Damage_Predictor::set_resulttable_predicted_detailed(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel model;
	ostringstream query_string;


	//send it to the database
	query_string <<"UPDATE ";
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_table_name();
	query_string << " SET " ;
	query_string  << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_total_ecn_predict_perc) << " = "<< predict.ecn_risk_perc <<" , " ;
	query_string  << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_total_eco_predict_perc) << " = "<< predict.eco_risk_perc <<" , " ;
	query_string  << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_pop_affected_perc) << " = "<< predict.pop_affected_risk_perc <<" , " ;
	query_string  << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_pop_endangered_perc) << " = "<< predict.pop_endangered_risk_perc <<" , " ;
	query_string  << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_total_score_perc) << " = "<< predict.pys_without_density_risk_perc <<" , " ;
	query_string  << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_total_dens_score_perc) << " = "<< predict.pys_with_density_risk_perc <<"  " ;
	
	query_string  << " WHERE " ;
	query_string  << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND " ;
	query_string  << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc<< " ";
	query_string  << " AND " ;
	query_string  << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::sz_break_id) << " = '" << break_sc<< "' ";

	Data_Base::database_request(&model, query_string.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_Damage_Predictor::update_predicted_result_by_percentage(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const string break_sc, _risk_predicted_risk predict)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Damage_Predictor::table_detailed_predicted_results->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Predict the damage for a given break scenario
void Risk_Damage_Predictor::predict_damages(QSqlDatabase *ptr_database, Risk_Break_Scenario *break_sc, Risk_Catchment_Break_Scenario_List *list_sc_break){
	this->break_sc2predict=break_sc;
	this->delete_dam_result();

	this->info_list.clear_list();
	string buff_rest="CA";
	buff_rest=list_sc_break->set_new_list_nearest_hyd_calc_scenario(&this->info_list, this->break_sc2predict);

	//this->info_list.output_list();
	list_sc_break->find_break_scenario_unequal_interval(buff_rest, this->break_sc2predict, &this->info_list);


	//output
	this->info_list.output_list();

	this->number=this->info_list.get_number_in_list();
	 


	this->allocate_dam_result();

	ostringstream cout;

	//output
	cout <<"Load damage data for the risk prediction of break scenario " <<break_sc->get_break_scenario_string()<<"..."<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);

	//set damages
	this->set_economical_damages(ptr_database);
	this->set_ecological_btype_damages(ptr_database);
	this->set_ecological_soil_damages(ptr_database);
	this->set_pop_damages(ptr_database);
	this->set_pys_damages(ptr_database);
	//output
	cout <<"Loading of damage data for the risk prediction is finished!"<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);


	cout <<"Predict the risk of break scenario " <<break_sc->get_break_scenario_string()<<"..."<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	//compare damages and predict the damages
	this->predict_economical_damages();
	this->predict_ecological_btype_damages();
	this->predict_ecological_soil_damages();
	this->predict_pop_affected_damages();
	this->predict_pop_endangered_damages();
	this->predict_pys_without_dens_damages();
	this->predict_pys_with_dens_damages();
	cout <<"Risk prediction is finished!"<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);


	cout <<"Transfer predicted results to database..."<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	this->transfer_predicted_result2database(ptr_database);

}
//Initialize the data structure for the predicted risk storing (\see _risk_predicted_risk) (static)
void Risk_Damage_Predictor::init_risk_predicted_risk_structure(_risk_predicted_risk *init){

	init->ecn_risk=0.0;
	init->ecn_risk_perc=0.0;
	init->eco_risk=0.0;
	init->eco_risk_perc=0.0;
	init->pop_affected_risk=0.0;
	init->pop_affected_risk_perc=0.0;
	init->pop_endangered_risk=0.0;
	init->pop_endangered_risk_perc=0.0;
	init->pys_with_density_risk=0.0;
	init->pys_with_density_risk_perc=0.0;
	init->pys_without_density_risk=0.0;
	init->pys_without_density_risk_perc=0.0;
	init->middle_perc=0.0;


}
//_________
//private
//Allocate the data storing structure of the damage results
void Risk_Damage_Predictor::allocate_dam_result(void){

	try{
		this->ecn_results=new Risk_Dam_Results[this->number];
		this->eco_btype_results=new Risk_Dam_Results[this->number];
		this->eco_soil_results=new Risk_Dam_Results[this->number];
		this->pop_affected_results=new Risk_Dam_Results[this->number];
		this->pop_endangered_results=new Risk_Dam_Results[this->number];
		this->pys_results=new Risk_Dam_Results[this->number];
		this->pys_dens_results=new Risk_Dam_Results[this->number];
		

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Delete the data storing structure of the damage results
void Risk_Damage_Predictor::delete_dam_result(void){

	if(this->ecn_results!=NULL){
		delete []this->ecn_results;
		this->ecn_results=NULL;
	}
	if(this->eco_btype_results!=NULL){
		delete []this->eco_btype_results;
		this->eco_btype_results=NULL;
	}
	if(this->eco_soil_results!=NULL){
		delete []this->eco_soil_results;
		this->eco_soil_results=NULL;
	}
	if(this->pop_affected_results!=NULL){
		delete []this->pop_affected_results;
		this->pop_affected_results=NULL;
	}
	if(this->pop_endangered_results!=NULL){
		delete []this->pop_endangered_results;
		this->pop_endangered_results=NULL;
	}
	if(this->pys_results!=NULL){
		delete []this->pys_results;
		this->pys_results=NULL;
	}
	if(this->pys_dens_results!=NULL){
		delete []this->pys_dens_results;
		this->pys_dens_results=NULL;
	}	
}
//Set the economical damages from database table
void Risk_Damage_Predictor::set_economical_damages(QSqlDatabase *ptr_database){

	QSqlQueryModel model;
	ostringstream id;
	int number_res=0;
	int elem_id=-1;
	int rast_id=-1;
	double value=0.0;

	for(int i=0; i<this->number; i++){

		id.str("");
		id<<this->info_list.get_ptr_scenario(i)->get_break_scenario_string();
		number_res=Dam_Ecn_Element::count_data_in_erg_table(&model, ptr_database, this->system_id, this->break_sc2predict->get_hydraulic_boundary_scenario().get_id(), id.str());
		
		this->ecn_results[i].set_number_results(number_res);
		int counter=0;
		int counter2=0;

		for(int j=0; j< number_res; j++){
			if(i==counter*constant::max_rows){
				model.clear();
				Dam_Ecn_Element::select_data_in_erg_table(&model, ptr_database, this->system_id, this->break_sc2predict->get_hydraulic_boundary_scenario().get_id(), id.str(),j, constant::max_rows);
				counter++;
				counter2=0;
			}
			value=0.0;
			elem_id=model.record(counter2).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
			rast_id=model.record(counter2).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			value=value+model.record(counter2).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
			value=value+model.record(counter2).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
			this->ecn_results[i].set_elem_result(counter2,rast_id,elem_id,value);
			counter2++;
		}
	}
}
//Set the ecological biotope-type damages from database table
void Risk_Damage_Predictor::set_ecological_btype_damages(QSqlDatabase *ptr_database){
	QSqlQueryModel model;
	ostringstream id;
	int number_res=0;
	int elem_id=-1;
	int rast_id=-1;
	double value=0.0;
	for(int i=0; i<this->number; i++){

		id.str("");
		//id<<"CA_"<<this->break_sc2predict->get_involved_section(i).id_fpl_sec<<"-0";
		id<<this->info_list.get_ptr_scenario(i)->get_break_scenario_string();
		number_res=Dam_Eco_Btype_Element::count_data_in_erg_table(&model, ptr_database, this->system_id, this->break_sc2predict->get_hydraulic_boundary_scenario().get_id(), id.str());
		
		this->eco_btype_results[i].set_number_results(number_res);
		int counter=0;
		int counter2=0;

		for(int j=0; j< number_res; j++){
			if(i==counter*constant::max_rows){
				model.clear();
				Dam_Eco_Btype_Element::select_data_in_erg_table(&model, ptr_database, this->system_id, this->break_sc2predict->get_hydraulic_boundary_scenario().get_id(), id.str(),j, constant::max_rows);
				counter++;
				counter2=0;
			}
			value=0.0;
			elem_id=model.record(counter2).value((Dam_Eco_Btype_Element::elem_erg_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
			rast_id=model.record(counter2).value((Dam_Eco_Btype_Element::elem_erg_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			value=value+model.record(counter2).value((Dam_Eco_Btype_Element::elem_erg_table->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
			this->eco_btype_results[i].set_elem_result(counter2,rast_id,elem_id,value);
			counter2++;
		}
	}
}
//Set the ecological soil-erosion damages from database table
void Risk_Damage_Predictor::set_ecological_soil_damages(QSqlDatabase *ptr_database){
	QSqlQueryModel model;
	ostringstream id;
	int number_res=0;
	int elem_id=-1;
	int rast_id=-1;
	double value=0.0;
	for(int i=0; i<this->number; i++){

		id.str("");
		//id<<"CA_"<<this->break_sc2predict->get_involved_section(i).id_fpl_sec<<"-0";
		id<<this->info_list.get_ptr_scenario(i)->get_break_scenario_string();
		number_res=Dam_Eco_Soil_Element::count_data_in_erg_table(&model, ptr_database, this->system_id, this->break_sc2predict->get_hydraulic_boundary_scenario().get_id(), id.str());
		
		this->eco_soil_results[i].set_number_results(number_res);
		int counter=0;
		int counter2=0;
		for(int j=0; j< number_res; j++){
			if(i==counter*constant::max_rows){
				model.clear();
				Dam_Eco_Soil_Element::select_data_in_erg_table(&model, ptr_database, this->system_id, this->break_sc2predict->get_hydraulic_boundary_scenario().get_id(), id.str(),j, constant::max_rows);
				counter++;
				counter2=0;
			}
			value=0.0;
			elem_id=model.record(counter2).value((Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
			rast_id=model.record(counter2).value((Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			value=value+model.record(counter2).value((Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
			this->eco_soil_results[i].set_elem_result(counter2,rast_id,elem_id,value);
			counter2++;
		}
	}
}
//Set the people2risk affected people damages from database table
void Risk_Damage_Predictor::set_pop_damages(QSqlDatabase *ptr_database){
	QSqlQueryModel model;
	ostringstream id;
	int number_res=0;
	int elem_id=-1;
	int rast_id=-1;
	double value1=0.0;
	double value2=0.0;
	for(int i=0; i<this->number; i++){

		id.str("");
		//id<<"CA_"<<this->break_sc2predict->get_involved_section(i).id_fpl_sec<<"-0";
		id<<this->info_list.get_ptr_scenario(i)->get_break_scenario_string();
		number_res=Dam_People_Element::count_data_in_erg_table(&model, ptr_database, this->system_id, this->break_sc2predict->get_hydraulic_boundary_scenario().get_id(), id.str());
		
		this->pop_affected_results[i].set_number_results(number_res);
		this->pop_endangered_results[i].set_number_results(number_res);
		int counter=0;
		int counter2=0;
		for(int j=0; j< number_res; j++){
			if(i==counter*constant::max_rows){
				model.clear();
				Dam_People_Element::select_data_in_erg_table(&model, ptr_database, this->system_id, this->break_sc2predict->get_hydraulic_boundary_scenario().get_id(), id.str(),j, constant::max_rows);
				counter++;
				counter2=0;
			}
			value1=0.0;
			value2=0.0;
			elem_id=model.record(counter2).value((Dam_People_Element::elem_erg_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
			rast_id=model.record(counter2).value((Dam_People_Element::elem_erg_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			value1=value1+model.record(counter2).value((Dam_People_Element::elem_erg_table->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
			this->pop_affected_results[i].set_elem_result(counter2,rast_id,elem_id,value1);
			value2=value2+model.record(counter2).value((Dam_People_Element::elem_erg_table->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
			this->pop_endangered_results[i].set_elem_result(counter2,rast_id,elem_id,value2);
			counter2++;
		}
	}
}
//Set the psycho-social results from database table
void Risk_Damage_Predictor::set_pys_damages(QSqlDatabase *ptr_database){
	QSqlQueryModel model;
	ostringstream id;
	int number_res=0;
	int elem_id=-1;
	int rast_id=-1;
	double value1=0.0;
	double value2=0.0;
	for(int i=0; i<this->number; i++){

		id.str("");
		//id<<"CA_"<<this->break_sc2predict->get_involved_section(i).id_fpl_sec<<"-0";
		id<<this->info_list.get_ptr_scenario(i)->get_break_scenario_string();
		number_res=Dam_Pys_Element::count_data_in_erg_table(&model, ptr_database, this->system_id, this->break_sc2predict->get_hydraulic_boundary_scenario().get_id(), id.str());
		
		this->pys_results[i].set_number_results(number_res);
		this->pys_dens_results[i].set_number_results(number_res);
		int counter=0;
		int counter2=0;
		for(int j=0; j< number_res; j++){
			if(i==counter*constant::max_rows){
				model.clear();
				Dam_Pys_Element::select_data_in_erg_table(&model, ptr_database, this->system_id, this->break_sc2predict->get_hydraulic_boundary_scenario().get_id(), id.str(),j, constant::max_rows);
				counter++;
				counter2=0;
			}
			value1=0.0;
			value2=0.0;
			elem_id=model.record(counter2).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
			rast_id=model.record(counter2).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			value1=value1+model.record(counter2).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::total_score)).c_str()).toDouble();
			this->pys_results[i].set_elem_result(counter2,rast_id,elem_id,value1);
			value2=value2+model.record(counter2).value((Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::total_dens_score)).c_str()).toDouble();
			this->pys_dens_results[i].set_elem_result(counter2,rast_id,elem_id,value2);
			counter2++;
		}
	}
}
//Predict the economical damages
void Risk_Damage_Predictor::predict_economical_damages(void){
	//compare the damage results
	for(int i=0; i< this->number; i++){
		for(int j=i; j< this->number; j++){
			if(i==j){
				continue;
			}
			this->ecn_results[i].compare_results(&this->ecn_results[j]);

		}
	}
	this->total_pred_ecn_dam=0.0;
	//calculate the total damages
	for(int i=0; i< this->number; i++){
		this->total_pred_ecn_dam=this->total_pred_ecn_dam+this->ecn_results[i].get_sum_value();
	}
}
//Predict the ecological biotope-type damages
void Risk_Damage_Predictor::predict_ecological_btype_damages(void){
	//compare the damage results
	for(int i=0; i< this->number; i++){
		for(int j=i; j< this->number; j++){
			if(i==j){
				continue;
			}
			this->eco_btype_results[i].compare_results(&this->eco_btype_results[j]);

		}
	}
	this->total_pred_eco_dam=0.0;
	//calculate the total damages
	for(int i=0; i< this->number; i++){
		this->total_pred_eco_dam=this->total_pred_eco_dam+this->eco_btype_results[i].get_sum_value();
	}
}
//Predict the ecological soil-erosion damages
void Risk_Damage_Predictor::predict_ecological_soil_damages(void){
	//compare the damage results
	for(int i=0; i< this->number; i++){
		for(int j=i; j< this->number; j++){
			if(i==j){
				continue;
			}
			this->eco_soil_results[i].compare_results(&this->eco_soil_results[j]);

		}
	}
	//calculate the total damages
	for(int i=0; i< this->number; i++){
		this->total_pred_eco_dam=this->total_pred_eco_dam+this->eco_soil_results[i].get_sum_value();
	}
}
//Predict the people2risk affected damages
void Risk_Damage_Predictor::predict_pop_affected_damages(void){
	//compare the damage results
	for(int i=0; i< this->number; i++){
		for(int j=i; j< this->number; j++){
			if(i==j){
				continue;
			}
			this->pop_affected_results[i].compare_results(&this->pop_affected_results[j]);

		}
	}
	this->total_pred_pop_aff_dam=0.0;
	//calculate the total damages
	for(int i=0; i< this->number; i++){
		this->total_pred_pop_aff_dam=this->total_pred_pop_aff_dam+this->pop_affected_results[i].get_sum_value();
	}
}
//Predict the people2risk endangered damages
void Risk_Damage_Predictor::predict_pop_endangered_damages(void){
	//compare the damage results
	for(int i=0; i< this->number; i++){
		for(int j=i; j< this->number; j++){
			if(i==j){
				continue;
			}
			this->pop_endangered_results[i].compare_results(&this->pop_endangered_results[j]);

		}
	}
	this->total_pred_pop_end_dam=0.0;
	//calculate the total damages
	for(int i=0; i< this->number; i++){
		this->total_pred_pop_end_dam=this->total_pred_pop_end_dam+this->pop_endangered_results[i].get_sum_value();
	}
}
//Predict the psycho-social damages without density criteria 
void Risk_Damage_Predictor::predict_pys_without_dens_damages(void){
	//compare the damage results
	for(int i=0; i< this->number; i++){
		for(int j=i; j< this->number; j++){
			if(i==j){
				continue;
			}
			this->pys_results[i].compare_results(&this->pys_results[j]);

		}
	}
	this->total_pred_pys_without_dens_dam=0.0;
	//calculate the total damages
	for(int i=0; i< this->number; i++){
		this->total_pred_pys_without_dens_dam=this->total_pred_pys_without_dens_dam+this->pys_results[i].get_sum_value();
	}
}
//Predict the psycho-social damages with density criteria 
void Risk_Damage_Predictor::predict_pys_with_dens_damages(void){
	//compare the damage results
	for(int i=0; i< this->number; i++){
		for(int j=i; j< this->number; j++){
			if(i==j){
				continue;
			}
			this->pys_dens_results[i].compare_results(&this->pys_dens_results[j]);

		}
	}
	this->total_pred_pys_with_dens_dam=0.0;
	//calculate the total damages
	for(int i=0; i< this->number; i++){
		this->total_pred_pys_with_dens_dam=this->total_pred_pys_with_dens_dam+this->pys_dens_results[i].get_sum_value();
	}
}
//Transfer results to database table
void Risk_Damage_Predictor::transfer_predicted_result2database(QSqlDatabase *ptr_database){

	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Damage_Predictor::set_resulttable_predicted_detailed(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Risk_Damage_Predictor::table_detailed_predicted_results->maximum_int_of_column(Risk_System::table_results->get_column_name(label::glob_id),ptr_database)+1;
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Risk_Damage_Predictor::table_detailed_predicted_results->get_table_name();
	query_string <<" ( ";
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(label::glob_id) <<" , ";
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(label::areastate_id) <<" , ";
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(label::measure_id) <<" , ";
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::sz_break_id) <<" , ";

	//results
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_total_ecn_predict) <<" , ";
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_total_eco_predict) <<" , ";
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(dam_label::pop_affected) <<" , ";
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(dam_label::pop_endangered) <<" , ";
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(dam_label::total_score) <<" , ";
	query_string << Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(dam_label::total_dens_score) <<" ) ";


	double prob=this->break_sc2predict->get_prob_total();

	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << this->break_sc2predict->get_hydraulic_boundary_scenario().get_id()<< " , " ;
	query_string << "'"<<this->break_sc2predict->get_break_scenario_string()<< "' , " ;
	//results
	query_string << this->total_pred_ecn_dam*prob<< " , " ;
	query_string << this->total_pred_eco_dam*prob<< " , " ;
	query_string << this->total_pred_pop_aff_dam*prob<< " , " ;
	query_string << this->total_pred_pop_end_dam*prob<< " , " ;
	query_string << this->total_pred_pys_without_dens_dam*prob<< " , " ;
	query_string << this->total_pred_pys_with_dens_dam*prob << " ) " ;


	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if(model.lastError().isValid()){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Table Name                : " << Risk_Damage_Predictor::table_detailed_predicted_results->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;	
	}
}
//Set error(s)
Error Risk_Damage_Predictor::set_error(const int err_type){
	string place="Risk_Damage_Predictor::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_dam_result(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://wrong database request
			place.append("transfer_predicted_result2database(QSqlDatabase *ptr_database)");
			reason="Invalid database request";
			help="Check the database";
			type=2;
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