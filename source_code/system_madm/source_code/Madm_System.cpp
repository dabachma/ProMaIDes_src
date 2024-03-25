#include "Madm_Headers_Precompiled.h"
//#include "Madm_System.h"

//init static members
bool Madm_System::abort_thread_flag=false;
Tables *Madm_System::table_sets=NULL;



//Default constructor
Madm_System::Madm_System(void): number_defined_criteria(19){
	this->thread_type=_madm_thread_type::madm_thread_unknown;
	this->number_error=0;
	this->number_warning_begin=0;
	this->number_warning=0;
	this->file_names=NULL;
	this->number_analysis=0;
	this->analysis=NULL;
	this->weight_set_info.name=madm_label::weight_standard;
	this->weight_set_info.id=-1;

	this->defined_criteria=NULL;
	this->number_weight_sets=0;
	this->list_weight_set_id=NULL;
	this->thread_is_started=false;


	Sys_Memory_Count::self()->add_mem(sizeof(Madm_System),_sys_system_modules::MADM_SYS);//count the memory
}
//Default destructor
Madm_System::~Madm_System(void){

	//close database
	if(this->qsqldatabase.isOpen()){
		this->qsqldatabase.close();
	}
	this->delete_file_names();
	this->delete_analysis();
	this->delete_defined_criteria();
	this->delete_list_weighting_set_id();
	this->set_stop_thread_flag(false);
	Sys_Memory_Count::self()->minus_mem(sizeof(Madm_System),_sys_system_modules::MADM_SYS);//count the memory
}
//_____________
//public
//Set the database table for the weight sets: it sets the table name and the name of the columns and allocate them (static)
void Madm_System::set_table_sets(QSqlDatabase *ptr_database,const bool not_close ){
	if(Madm_System::table_sets==NULL){
		//make specific input for this class
		const string tab_id_name=madm_label::tab_set;
		string tab_col[23];
		tab_col[0]=label::glob_id;
		tab_col[1]=madm_label::set_id;
		tab_col[2]=madm_label::crit_risk_ecn;
		tab_col[3]=madm_label::crit_risk_eco;
		tab_col[4]=madm_label::crit_risk_pop_aff;
		tab_col[5]=madm_label::crit_risk_pys;
		tab_col[6]=madm_label::crit_risk_outflow;
		tab_col[7]=madm_label::crit_cost_ecn;
		tab_col[8]=madm_label::set_name;
		tab_col[9]=madm_label::crit_risk_pop_dan;
		tab_col[10]=label::description;
		tab_col[11]=madm_label::crit_risk_sc_pub;
		tab_col[12]=madm_label::crit_risk_sc_eco;
		tab_col[13]=madm_label::crit_risk_sc_cult;
		tab_col[14]=madm_label::crit_risk_sc_person;

		tab_col[15] = madm_label::crit_risk_ci_elect_pt;
		tab_col[16] = madm_label::crit_risk_ci_info_tec_pt;
		tab_col[17] = madm_label::crit_risk_ci_water_sup_pt;
		tab_col[18] = madm_label::crit_risk_ci_water_treat_pt;
		tab_col[19] = madm_label::crit_risk_ci_energy_pt;
		tab_col[20] = madm_label::crit_risk_ci_health_pt;
		tab_col[21] = madm_label::crit_risk_ci_social_pt;
		tab_col[22] = madm_label::crit_risk_ci_mat_pt;
		


		
		//set the table
		try{
			Madm_System::table_sets= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Madm_System::table_sets->set_name(ptr_database, _sys_table_type::madm);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Madm_System::set_table_sets(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){

			if (not_close == false) {
				Madm_System::close_table_sets();
			}
			throw msg;
		}
	}
}
//Create the database table for the weight sets (static)
void Madm_System::create_table_sets(QSqlDatabase *ptr_database){
	if(Madm_System::table_sets==NULL){
		ostringstream cout;
		cout << "Create database table of the weighting sets..." << endl ;
		Sys_Common_Output::output_madm->output_txt(&cout);
		//make specific input for this class
		const string tab_name=madm_label::tab_set;
		const int num_col=23;
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

		tab_col[1].name=madm_label::set_id;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].key_flag=false;
		tab_col[1].unsigned_flag=true;

		
		tab_col[2].name=madm_label::set_name;
		tab_col[2].type=sys_label::tab_col_type_string;

		tab_col[3].name=madm_label::crit_risk_ecn;
		tab_col[3].type=sys_label::tab_col_type_double;

		tab_col[4].name=madm_label::crit_risk_eco;
		tab_col[4].type=sys_label::tab_col_type_double;

		tab_col[5].name=madm_label::crit_risk_pop_aff;
		tab_col[5].type=sys_label::tab_col_type_double;

		tab_col[6].name=madm_label::crit_risk_pop_dan;
		tab_col[6].type=sys_label::tab_col_type_double;

		tab_col[7].name=madm_label::crit_risk_pys;
		tab_col[7].type=sys_label::tab_col_type_double;

		tab_col[8].name=madm_label::crit_risk_sc_pub;
		tab_col[8].type=sys_label::tab_col_type_double;

		tab_col[9].name=madm_label::crit_risk_sc_eco;
		tab_col[9].type=sys_label::tab_col_type_double;

		tab_col[10].name=madm_label::crit_risk_sc_cult;
		tab_col[10].type=sys_label::tab_col_type_double;

		tab_col[11].name=madm_label::crit_risk_sc_person;
		tab_col[11].type=sys_label::tab_col_type_double;
	

		tab_col[12].name = madm_label::crit_risk_ci_elect_pt;
		tab_col[12].type = sys_label::tab_col_type_double;

		tab_col[13].name = madm_label::crit_risk_ci_info_tec_pt;
		tab_col[13].type = sys_label::tab_col_type_double;

		tab_col[14].name = madm_label::crit_risk_ci_water_sup_pt;
		tab_col[14].type = sys_label::tab_col_type_double;


		tab_col[15].name = madm_label::crit_risk_ci_water_treat_pt;
		tab_col[15].type = sys_label::tab_col_type_double;

		tab_col[16].name = madm_label::crit_risk_ci_energy_pt;
		tab_col[16].type = sys_label::tab_col_type_double;

		tab_col[17].name = madm_label::crit_risk_ci_health_pt;
		tab_col[17].type = sys_label::tab_col_type_double;

		tab_col[18].name = madm_label::crit_risk_ci_social_pt;
		tab_col[18].type = sys_label::tab_col_type_double;

		tab_col[19].name = madm_label::crit_risk_ci_mat_pt;
		tab_col[19].type = sys_label::tab_col_type_double;


		tab_col[20].name=madm_label::crit_risk_outflow;
		tab_col[20].type=sys_label::tab_col_type_double;

		tab_col[21].name=madm_label::crit_cost_ecn;
		tab_col[21].type=sys_label::tab_col_type_double;
		tab_col[21].unsigned_flag=true;

		tab_col[22].name=label::description;
		tab_col[22].type=sys_label::tab_col_type_string;

		try{
			Madm_System::table_sets= new Tables();
			if(Madm_System::table_sets->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::madm)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_madm->output_txt(&cout);
				Madm_System::close_table_sets();
				return;
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Madm_System::create_table_sets(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Madm_System::close_table_sets();
			throw msg;
		}
		Madm_System::close_table_sets();
		
		Madm_System::close_table_sets();
	}
}
//Delete all data in the database table for the weight sets
void Madm_System::delete_data_in_table_sets(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Madm_System::set_table_sets(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Madm_System::table_sets->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the weight sets (static)
void Madm_System::close_table_sets(void){
	if(Madm_System::table_sets!=NULL){
		delete Madm_System::table_sets;
		Madm_System::table_sets=NULL;
	}
}
//Set the default weighting sets to database (static)
void Madm_System::set_default_weighting_set2datbase(QSqlDatabase *ptr_database){

	try{
		Madm_System::set_table_sets(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);

	//evaluate the global identifier
	int id_glob=Madm_System::table_sets->maximum_int_of_column(Madm_System::table_sets->get_column_name(label::glob_id),ptr_database)+1;
	ostringstream total;

	//set the query via a query string
	ostringstream query_string_fix;
	query_string_fix << "INSERT INTO  " << Madm_System::table_sets->get_table_name();
	query_string_fix <<" ( ";
	query_string_fix << Madm_System::table_sets->get_column_name(label::glob_id) <<" , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::set_id) <<" , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::set_name) <<" , ";
	//weights
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ecn) <<" , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_eco) <<" , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_pop_aff) <<" , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_pop_dan) <<" , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_pys) <<" , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_pub) <<" , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_eco) <<" , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_cult) <<" , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_person) <<" , ";

	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_elect_pt) << " , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_info_tec_pt) << " , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_water_sup_pt) << " , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_water_treat_pt) << " , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_energy_pt) << " , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_health_pt) << " , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_social_pt) << " , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_mat_pt) << " , ";

	

	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_risk_outflow) <<" , ";
	query_string_fix << Madm_System::table_sets->get_column_name(madm_label::crit_cost_ecn) <<" , ";

	query_string_fix << Madm_System::table_sets->get_column_name(label::description) <<" ) ";


	//standard weighting sets
	ostringstream query_string;
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << id_glob << " , " ;
	query_string <<"'"<< madm_label::weight_standard <<"'"<< " , " ;
	//weights
	query_string << 10.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 10.0 << " , ";
	query_string << 10.0 << " , ";
	query_string << 10.0 << " , ";
	query_string << 10.0 << " , ";
	query_string << 10.0 << " , ";
	query_string << 10.0 << " , ";
	query_string << 10.0 << " , ";
	query_string << 10.0 << " , ";
	query_string << "'All weights are similar'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//ecn standard weighting sets
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << id_glob << " , " ;
	query_string <<"'"<< madm_label::weight_ecn_standard <<"'"<< " , " ;
	//weights
	query_string << 10.0 << " , " ;
	query_string << 5.0 << " , " ;
	query_string << 4.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 4.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 5.0 << " , " ;
	query_string << 5.0 << " , " ;
	query_string << 8.0 << " , " ;
	query_string << 8.0 << " , ";
	query_string << 8.0 << " , ";
	query_string << 8.0 << " , ";
	query_string << 8.0 << " , ";
	query_string << 8.0 << " , ";
	query_string << 5.0 << " , ";
	query_string << 5.0 << " , ";
	query_string << 10.0 << " , ";


	query_string << 3.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << "'Weight set with economical preferences'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//eco standard weighting sets
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << id_glob << " , " ;
	query_string <<"'"<< madm_label::weight_eco_standard <<"'"<< " , " ;
	//weights
	query_string << 6.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 4.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 4.0 << " , " ;
	query_string << 6.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 6.0 << " , " ;
	query_string << 8.0 << " , " ;

	query_string << 8.0 << " , ";
	query_string << 5.0 << " , ";
	query_string << 8.0 << " , ";
	query_string << 8.0 << " , ";
	query_string << 5.0 << " , ";
	query_string << 5.0 << " , ";
	query_string << 5.0 << " , ";
	query_string << 3.0 << " , ";


	query_string << 3.0 << " , " ;
	query_string << 6.0 << " , " ;
	query_string << "'Weight set with ecological preferences'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//pop standard weighting sets
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << id_glob << " , " ;
	query_string <<"'"<< madm_label::weight_pop_standard <<"'"<< " , " ;
	//weights
	query_string << 6.0 << " , " ;
	query_string << 6.0 << " , " ;
	query_string << 7.0 << " , " ;
	query_string << 12.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 6.0 << " , " ;
	query_string << 10.0 << " , " ;
	query_string << 12.0 << " , " ;

	query_string << 8.0 << " , ";
	query_string << 8.0 << " , ";
	query_string << 8.0 << " , ";
	query_string << 8.0 << " , ";
	query_string << 8.0 << " , ";
	query_string << 10.0 << " , ";
	query_string << 10.0 << " , ";
	query_string << 3.0 << " , ";

	query_string << 3.0 << " , " ;
	query_string << 6.0 << " , " ;
	query_string << "'Weight set with people2risk and psycho-social preferences'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;



}
//Select the data in the database table for weighting sets (static)
int Madm_System::select_data_in_settable(QSqlQueryModel *model, QSqlDatabase *ptr_database){
	try{
		Madm_System::set_table_sets(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Madm_System::table_sets->get_table_name();
		
	Data_Base::database_request(model, test_filter.str(), ptr_database);
		
	//check the request
	if(model->lastError().isValid()){
		Error msg;
		msg.set_msg("Madm_System::select_data_in_settable(QSqlQueryModel *model, QSqlDatabase *ptr_database)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Madm_System::table_sets->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return model->rowCount();
}
//Select the data in the database table for weighting sets for specified parameters (static)
int Madm_System::select_data_in_settable(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int set_id){
	try{
		Madm_System::set_table_sets(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Madm_System::table_sets->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Madm_System::table_sets->get_column_name(madm_label::set_id) << " = "  << set_id;
	
	Data_Base::database_request(model, test_filter.str(), ptr_database);
		
	//check the request
	if(model->lastError().isValid()){
		Error msg;
		msg.set_msg("Madm_System::select_data_in_settable(QSqlQueryModel *model, QSqlDatabase *ptr_database)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Madm_System::table_sets->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return model->rowCount();
}
//Check if weighting sets are set in database table (static)
bool Madm_System::check_data_in_settable(QSqlDatabase *ptr_database){
	QSqlQueryModel model;
	try{
		Madm_System::set_table_sets(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Madm_System::table_sets->get_table_name();

	
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
		
	//check the request
	if(model.lastError().isValid()){
		Error msg;
		msg.set_msg("Madm_System::check_data_in_settable(QSqlDatabase *ptr_database)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Madm_System::table_sets->get_table_name() << endl;
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
//Ask per file dialog to delete weighting sets
bool Madm_System::ask_weighting_set2delete(QWidget *parent){
	
	MadmGui_Weighting_Set_Dia my_dia(parent);
	int number=0;

	number=my_dia.set_dialog(&this->qsqldatabase);

	ostringstream txt;
	//set the header
	txt << "Select from " << number << " MADM-weighting sets for deleting..." << endl;
	my_dia.set_txt(txt.str());

	int *buffer=NULL;

	if(my_dia.start_dialog(&buffer, &this->number_weight_sets)==false){
		return false;
	}
	
	if(buffer==NULL || this->number_weight_sets==0){
		return false;
	}

	//copy it
	this->allocate_list_weighting_set_id();
	for(int i=0; i<this->number_weight_sets; i++){
		this->list_weight_set_id[i]=buffer[i];
	}

	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}



	return true;
}
//Set the number of files for an analysis and the global filenames per dialog (gui)
bool Madm_System::set_analysis_number_file_gui(QWidget *parent){
	Sys_Multipathsettings_Dia dialog(parent);
	ostringstream text;
	QIcon icon;
	icon.addFile(":madm_icon");


	switch(this->thread_type){
		case (_madm_thread_type::madm_calc_file):
			text << "Choose the madm system file(s), which " << endl;
			text << "should be analysed"<< endl;
			dialog.set_txt2headerlabel(text.str(),icon);
			break;
		default:
			text << "" << endl;
			dialog.set_txt2headerlabel(text.str(),icon);
	}

	bool flag=false;
	flag=dialog.asked_path_filenames();

	if(flag==true){
		//read out the number of files
		this->number_analysis=dialog.getFilesnumber();
		try{
			this->allocate_file_names();
		}
		catch(Error msg){
			throw msg;
		}

		for(int i=0; i<this->number_analysis; i++){
			this->file_names[i]=dialog.get_filename(i);
		}
		return true;
	}
	else{
		return false;
	}
}
//Set the number of files for an import of weighting sets and the global filenames per dialog (gui)
bool Madm_System::set_weight_set_number_file_gui(QWidget *parent){
	Sys_Multipathsettings_Dia dialog(parent);
	ostringstream text;
	QIcon icon;
	icon.addFile(":madm_icon");


	switch(this->thread_type){
		case (_madm_thread_type::madm_calc_file):
			text << "Choose the madm weighting set file(s), which " << endl;
			text << "should be analysed"<< endl;
			dialog.set_txt2headerlabel(text.str(),icon);
			break;
		default:
			text << "" << endl;
			dialog.set_txt2headerlabel(text.str(),icon);
	}

	bool flag=false;
	flag=dialog.asked_path_filenames();

	if(flag==true){
		//read out the number of files
		this->number_analysis=dialog.getFilesnumber();
		try{
			this->allocate_file_names();
		}
		catch(Error msg){
			throw msg;
		}

		for(int i=0; i<this->number_analysis; i++){
			this->file_names[i]=dialog.get_filename(i);
		}

		return true;
	}
	else{
		return false;
	}
}
//Set the thread type for specifiying the task of the thread
void Madm_System::set_thread_type(_madm_thread_type type){
	this->thread_type=type;
}
//Set a pointer to the database; the copy of the database is made in this method
void Madm_System::set_ptr2database(QSqlDatabase *ptr_database){
	this->qsqldatabase=QSqlDatabase::cloneDatabase(*ptr_database, sys_label::str_madm.c_str());
	this->qsqldatabase.open();	
}
//Get the thread type
_madm_thread_type Madm_System::get_thread_type(void){
	return this->thread_type;
}
//Create a whole set of madm database tables
void Madm_System::create_madm_database_tables(void){
	ostringstream cout;
	cout << "Create MADM system tables..." << endl ;
	Sys_Common_Output::output_madm->output_txt(&cout);
	this->set_start_warnings_number();
	try{
		
		Madm_System::create_table_sets(&this->qsqldatabase);
		Madm_Decision_Matrix::create_table_matrix(&this->qsqldatabase);
		_Madm_Solver::create_table_ranking(&this->qsqldatabase);
		Madm_System::set_default_weighting_set2datbase(&this->qsqldatabase);
	}
	catch(Error msg){
		this->number_error++;
		msg.output_msg(3);
	}
	this->set_warning_number();
	cout << "Creation of the MADM database tables is finished" << endl ;
	Sys_Common_Output::output_madm->output_txt(&cout);
	this->output_error_statistic();
}
//Check all madm database tables, for their existence in the database and their declaration in the database table-file
void Madm_System::check_madm_database_tables(void){
	ostringstream cout;
	cout << "Check MADM system tables..." << endl ;
	Sys_Common_Output::output_madm->output_txt(&cout,false, false);
	this->set_start_warnings_number();
	try{
		cout << "Check database table of the weighting sets..." << endl ;
		Sys_Common_Output::output_madm->output_txt(&cout,false, false);
		Madm_System::check_stop_thread_flag();
		Madm_System::set_table_sets(&this->qsqldatabase);

		cout << "Check database table of the decision matrix..." << endl ;
		Sys_Common_Output::output_madm->output_txt(&cout,false, false);
		Madm_System::check_stop_thread_flag();
		Madm_Decision_Matrix::set_table_matrix(&this->qsqldatabase);

		cout << "Check database table of the alternative's ranking..." << endl ;
		Sys_Common_Output::output_madm->output_txt(&cout,false, false);
		Madm_System::check_stop_thread_flag();
		_Madm_Solver::set_table_ranking(&this->qsqldatabase);

	}
	catch(Error msg){
		this->number_error++;
		ostringstream info;
		info << "Check if the MADM tables are created!"<< endl;
		msg.make_second_info(info.str());
		msg.set_fatal_flag(false);
		this->output_error_statistic();
		msg.output_msg(3);
	}
	this->set_warning_number();
	cout << "Check of the MADM database tables is finished" << endl ;
	Sys_Common_Output::output_madm->output_txt(&cout,false, false);
	//this->output_error_statistic();
}
//Delete the data of all madm database tables
void Madm_System::delete_data_madm_database_tables(void){
	try{
		_Madm_Solver::delete_data_in_table_ranking(&this->qsqldatabase);
		Madm_Decision_Matrix::delete_data_in_table_matrix(&this->qsqldatabase);
		Madm_System::delete_data_in_table_sets(&this->qsqldatabase);
	}
	catch(Error msg){
		msg.output_msg(3);
	}
}
//Close all madm database tables (static)
void Madm_System::close_madm_database_tables(void){
	_Madm_Solver::close_table_ranking();
	Madm_Decision_Matrix::close_table_matrix();
	Madm_System::close_table_sets();
}
//Initialize the defined criteria
void Madm_System::init_defined_criteria(void){
	this->delete_defined_criteria();
	this->allocate_defined_criteria();

	this->weight_set_info.name=madm_label::weight_standard;

	this->defined_criteria[0].index=0;
	this->defined_criteria[0].name=madm_label::crit_risk_ecn;
	this->defined_criteria[0].max_flag=madm_crit_min_max::max_crit_risk_ecn;
	this->defined_criteria[0].weight=1.0;

	this->defined_criteria[1].index=1;
	this->defined_criteria[1].name=madm_label::crit_risk_eco;
	this->defined_criteria[1].max_flag=madm_crit_min_max::max_crit_risk_eco;
	this->defined_criteria[1].weight=1.0;

	this->defined_criteria[2].index=2;
	this->defined_criteria[2].name=madm_label::crit_risk_pop_aff;
	this->defined_criteria[2].max_flag=madm_crit_min_max::max_crit_risk_pop_aff;
	this->defined_criteria[2].weight=1.0;

	this->defined_criteria[3].index=3;
	this->defined_criteria[3].name=madm_label::crit_risk_pop_dan;
	this->defined_criteria[3].max_flag=madm_crit_min_max::max_crit_risk_pop_dan;
	this->defined_criteria[3].weight=1.0;

	this->defined_criteria[4].index=4;
	this->defined_criteria[4].name=madm_label::crit_risk_pys;
	this->defined_criteria[4].max_flag=madm_crit_min_max::max_crit_risk_pys;
	this->defined_criteria[4].weight=1.0;

	this->defined_criteria[5].index=5;
	this->defined_criteria[5].name=madm_label::crit_risk_sc_pub;
	this->defined_criteria[5].max_flag=madm_crit_min_max::max_crit_risk_sc_pub;
	this->defined_criteria[5].weight=1.0;

	this->defined_criteria[6].index=6;
	this->defined_criteria[6].name=madm_label::crit_risk_sc_eco;
	this->defined_criteria[6].max_flag=madm_crit_min_max::max_crit_risk_sc_eco;
	this->defined_criteria[6].weight=1.0;

	this->defined_criteria[7].index=7;
	this->defined_criteria[7].name=madm_label::crit_risk_sc_cult;
	this->defined_criteria[7].max_flag=madm_crit_min_max::max_crit_risk_sc_cult;
	this->defined_criteria[7].weight=1.0;

	this->defined_criteria[8].index=8;
	this->defined_criteria[8].name=madm_label::crit_risk_sc_person;
	this->defined_criteria[8].max_flag=madm_crit_min_max::max_crit_risk_sc_person;
	this->defined_criteria[8].weight=1.0;

	this->defined_criteria[9].index = 9;
	this->defined_criteria[9].name = madm_label::crit_risk_ci_elect_pt;
	this->defined_criteria[9].max_flag = madm_crit_min_max::max_crit_risk_ci;
	this->defined_criteria[9].weight = 1.0;

	this->defined_criteria[10].index = 10;
	this->defined_criteria[10].name = madm_label::crit_risk_ci_info_tec_pt;
	this->defined_criteria[10].max_flag = madm_crit_min_max::max_crit_risk_ci;
	this->defined_criteria[10].weight = 1.0;

	this->defined_criteria[11].index = 11;
	this->defined_criteria[11].name = madm_label::crit_risk_ci_water_sup_pt;
	this->defined_criteria[11].max_flag = madm_crit_min_max::max_crit_risk_ci;
	this->defined_criteria[11].weight = 1.0;

	this->defined_criteria[12].index = 12;
	this->defined_criteria[12].name = madm_label::crit_risk_ci_water_treat_pt;
	this->defined_criteria[12].max_flag = madm_crit_min_max::max_crit_risk_ci;
	this->defined_criteria[12].weight = 1.0;

	this->defined_criteria[13].index = 13;
	this->defined_criteria[13].name = madm_label::crit_risk_ci_energy_pt;
	this->defined_criteria[13].max_flag = madm_crit_min_max::max_crit_risk_ci;
	this->defined_criteria[13].weight = 1.0;

	this->defined_criteria[14].index = 14;
	this->defined_criteria[14].name = madm_label::crit_risk_ci_health_pt;
	this->defined_criteria[14].max_flag = madm_crit_min_max::max_crit_risk_ci;
	this->defined_criteria[14].weight = 1.0;

	this->defined_criteria[15].index = 15;
	this->defined_criteria[15].name = madm_label::crit_risk_ci_social_pt;
	this->defined_criteria[15].max_flag = madm_crit_min_max::max_crit_risk_ci;
	this->defined_criteria[15].weight = 1.0;

	this->defined_criteria[16].index = 16;
	this->defined_criteria[16].name = madm_label::crit_risk_ci_mat_pt;
	this->defined_criteria[16].max_flag = madm_crit_min_max::max_crit_risk_ci;
	this->defined_criteria[16].weight = 1.0;


	this->defined_criteria[17].index=17;
	this->defined_criteria[17].name=madm_label::crit_risk_outflow;
	this->defined_criteria[17].max_flag=madm_crit_min_max::max_crit_risk_outflow;
	this->defined_criteria[17].weight=1.0;

	this->defined_criteria[18].index=18;
	this->defined_criteria[18].name=madm_label::crit_cost_ecn;
	this->defined_criteria[18].max_flag=madm_crit_min_max::max_crit_cost_ecn;
	this->defined_criteria[18].weight=1.0;



}
//Set the stop thread flag (static)
void Madm_System::set_stop_thread_flag(const bool flag){
	Madm_System::abort_thread_flag=flag;
}
//Check the stop thread flag (static)
void Madm_System::check_stop_thread_flag(void){
	if(Madm_System::abort_thread_flag==true){
		Error msg;
		msg.set_msg(_sys_system_modules::MADM_SYS);
		throw msg;
	}
}
//Get the stop thread flag (static)
bool Madm_System::get_stop_thread_flag(void){
	return Madm_System::abort_thread_flag;
}
///Run the thread; the type of thread, which will be performed, has to be set before with set_thread_type(_madm_thread_type type)
void Madm_System::run(void){
/** \see set_thread_type(_madm_thread_type type), _madm_thread_type
*/
	this->thread_is_started=true;
	try{
		switch(this->thread_type){
			case(_madm_thread_type::madm_create_tab):
				this->create_madm_database_tables();
				break;
			case(_madm_thread_type::madm_check_tab):
				this->check_madm_database_tables();
				break;
			case(_madm_thread_type::madm_calc_database):
				this->analysis_per_database();
				break;
			case(_madm_thread_type::madm_calc_file):
				this->analysis_per_file();
				break;
			case(_madm_thread_type::madm_imp_weight):
				this->import_weighting_set();
				break;
			case(_madm_thread_type::madm_del_weight):
				this->delete_weighting_sets_in_database();
				break;
				
			default:
				Warning msg=this->set_warning(0);
				msg.output_msg(3);
		}
	}
	catch(Error msg){
		this->thread_is_started=false;
		msg.output_msg(3);
	}
	this->thread_is_started=false;
}
//Get the number of errors
int Madm_System::get_number_error(void){
	return this->number_error;
}
//Check if all required data are available in database for an madm-analysis
bool Madm_System::check_data_available_in_database(const bool nobreak_applied, const bool catchment_applied){

	bool ok_flag=true;
	Alt_System my_alt;
	my_alt.load_list_current_alt(&this->qsqldatabase, this->system_id.area_state);

	ostringstream total;
	total<< "A MADM-analysis is not possible. Not all "<< endl;
	total<< "required data are set. Check the data!"<<endl<<endl;
	bool catchment_found=true;
	ostringstream catchment;
	catchment<< "Catchment risk data are not set  : "<<endl;
	bool nobreak_found=true;
	ostringstream nobreak;
	nobreak<< "Nobreak risk data are not set    : "<<endl;
	bool cost_found=true;
	ostringstream cost;
	cost <<   "Following cost data are not set  : "<<endl;

	_sys_system_id buff_id;
	buff_id.area_state=this->system_id.area_state;
	buff_id.measure_nr=0;
	//check risk data base
	if(catchment_applied==true && Risk_System::check_data_in_resulttable(&this->qsqldatabase, buff_id, _risk_type::catchment_risk)==false){
		catchment_found=false;
		catchment << " Base areastate (measure-id 0) "<<endl;
	}
	if(nobreak_applied==true && Risk_System::check_data_in_resulttable(&this->qsqldatabase, buff_id, _risk_type::nobreak_risk)==false){
		nobreak_found=false;
		nobreak << " Base areastate (measure-id 0) "<<endl;
	}



	for(int i=0; i< my_alt.get_current_alternatives(); i++){
		buff_id.measure_nr=my_alt.get_ptr_alt_info(i)->id.measure_nr;
		//check risk data
		if(catchment_applied==true && Risk_System::check_data_in_resulttable(&this->qsqldatabase, buff_id, _risk_type::catchment_risk)==false){
			catchment_found=false;
			catchment << " "<< my_alt.get_ptr_alt_info(i)->name<<" (measure-id "<< my_alt.get_ptr_alt_info(i)->id.measure_nr<<")"<<endl;
		}
		if(nobreak_applied==true && Risk_System::check_data_in_resulttable(&this->qsqldatabase, buff_id, _risk_type::nobreak_risk)==false){
			nobreak_found=false;
			nobreak << " "<< my_alt.get_ptr_alt_info(i)->name<<" (measure-id "<< my_alt.get_ptr_alt_info(i)->id.measure_nr<<")"<<endl;
		}
		//check cost data
		if(Cost_Ecn_Cost_System::check_data_in_cost_table(&this->qsqldatabase, buff_id)==false){
			cost_found=false;
			cost << " "<< my_alt.get_ptr_alt_info(i)->name<<" (measure-id "<< my_alt.get_ptr_alt_info(i)->id.measure_nr<<")"<<endl;
		}
	}

	if(cost_found==false){
		total<< cost.str();
		ok_flag=false;
		
	}


	if(nobreak_applied==true && nobreak_found==false){
		ok_flag=false;
		total<<nobreak.str();
	}
	if(catchment_applied==true && catchment_found==false){
		ok_flag=false;
		total<<catchment.str();
	}


	if(ok_flag==false){
		Sys_Diverse_Text_Dia dialog2(true);
		dialog2.set_dialog_question(total.str());
		dialog2.start_dialog();
	}

	return ok_flag;
}
//Get the flag if the thread has started
bool Madm_System::get_thread_has_started(void){
	return this->thread_is_started;
}
//_______
//private
//Delete the data in the database table for the weighting set for a specified set-id
void Madm_System::delete_data_in_set_table(const int set_id){
	try{
		Madm_System::set_table_sets(&this->qsqldatabase);	
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "DELETE FROM ";
	test_filter << Madm_System::table_sets->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Madm_System::table_sets->get_column_name(madm_label::set_id) << " = "  << set_id;
	
	Data_Base::database_request(&model, test_filter.str(), &this->qsqldatabase);
		
}
//Delete the data in the database table for measures ranking for a specified set-id
void Madm_System::delete_data_in_ranking_table(const int set_id){
	try{
		_Madm_Solver::set_table_ranking(&this->qsqldatabase);	
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "DELETE FROM ";
	test_filter << _Madm_Solver::table_ranking->get_table_name();
	test_filter << " WHERE " ;
	test_filter << _Madm_Solver::table_ranking->get_column_name(madm_label::set_id) << " = "  << set_id;
	
	Data_Base::database_request(&model, test_filter.str(), &this->qsqldatabase);
}
//Delete the data in the database table for the measures ranking
void Madm_System::delete_data_in_ranking_table(void){
	try{
		_Madm_Solver::set_table_ranking(&this->qsqldatabase);	
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "DELETE FROM ";
	test_filter << _Madm_Solver::table_ranking->get_table_name();
	test_filter << " WHERE " ;
	test_filter << _Madm_Solver::table_ranking->get_column_name(label::areastate_id) << " = "  << this->system_id.area_state;

	
	Data_Base::database_request(&model, test_filter.str(), &this->qsqldatabase);

}
//Delete the data in the database table for the decision
void Madm_System::delete_data_in_matrix_table(void){
	try{
		Madm_Decision_Matrix::set_table_matrix(&this->qsqldatabase);	
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "DELETE FROM ";
	test_filter << Madm_Decision_Matrix::table_matrix->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Madm_Decision_Matrix::table_matrix->get_column_name(label::areastate_id) << " = "  << this->system_id.area_state;
	
	Data_Base::database_request(&model, test_filter.str(), &this->qsqldatabase);


}
//Set the current weight set from database table
void Madm_System::set_current_weight_set(const int set_id){
	QSqlQueryModel model;
	Madm_System::select_data_in_settable(&model, &this->qsqldatabase, set_id);

	this->weight_set_info.id=set_id;
	this->weight_set_info.name=model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::set_name)).c_str()).toString().toStdString();

	this->defined_criteria[0].weight=model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ecn)).c_str()).toDouble();
	this->defined_criteria[1].weight=model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_eco)).c_str()).toDouble();
	this->defined_criteria[2].weight=model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_pop_aff)).c_str()).toDouble();
	this->defined_criteria[3].weight=model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_pop_dan)).c_str()).toDouble();
	this->defined_criteria[4].weight=model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_pys)).c_str()).toDouble();
	this->defined_criteria[5].weight=model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_pub)).c_str()).toDouble();
	this->defined_criteria[6].weight=model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_eco)).c_str()).toDouble();
	this->defined_criteria[7].weight=model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_cult)).c_str()).toDouble();
	this->defined_criteria[8].weight=model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_person)).c_str()).toDouble();

	this->defined_criteria[9].weight = model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_elect_pt)).c_str()).toDouble();

	this->defined_criteria[10].weight = model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_info_tec_pt)).c_str()).toDouble();
	this->defined_criteria[11].weight = model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_water_sup_pt)).c_str()).toDouble();
	this->defined_criteria[12].weight = model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_water_treat_pt)).c_str()).toDouble();
	this->defined_criteria[13].weight = model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_energy_pt)).c_str()).toDouble();
	this->defined_criteria[14].weight = model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_health_pt)).c_str()).toDouble();
	this->defined_criteria[15].weight = model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_social_pt)).c_str()).toDouble();
	this->defined_criteria[16].weight = model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_mat_pt)).c_str()).toDouble();

	this->defined_criteria[17].weight=model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_outflow)).c_str()).toDouble();
	this->defined_criteria[18].weight=model.record(0).value((Madm_System::table_sets->get_column_name(madm_label::crit_cost_ecn)).c_str()).toDouble();
}
//Allocate the file names for calculation
void Madm_System::allocate_file_names(void){
	try{
		this->file_names=new string[this->number_analysis];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the file names for calculation
void Madm_System::delete_file_names(void){
	if(this->file_names!=NULL){
		delete []this->file_names;
		this->file_names=NULL;
	}
}
//Allocate the madm-analysis class
void Madm_System::allocate_analysis(void){
	try{
		this->analysis=new Madm_Analysis;
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the madm-analysis class
void Madm_System::delete_analysis(void){
	if(this->analysis!=NULL){
		delete this->analysis;
		this->analysis=NULL;
	}
}
//Allocate the madm-analysis class
void Madm_System::allocate_defined_criteria(void){
	try{
		this->defined_criteria=new _madm_criteria[this->number_defined_criteria];
		Sys_Memory_Count::self()->add_mem(sizeof(_madm_criteria)*this->number_defined_criteria,_sys_system_modules::MADM_SYS);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(5);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the defined criteria for the flood-protection DSS
void Madm_System::delete_defined_criteria(void){
	if(this->defined_criteria!=NULL){
		delete []this->defined_criteria;
		this->defined_criteria=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_madm_criteria)*this->number_defined_criteria,_sys_system_modules::MADM_SYS);
	}
}
//Allocate the list of weighting set ids
void Madm_System::allocate_list_weighting_set_id(void){
	try{
		this->list_weight_set_id=new int[this->number_weight_sets];
		Sys_Memory_Count::self()->add_mem(sizeof(int)*this->number_weight_sets,_sys_system_modules::MADM_SYS);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(9);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the list of weighting set ids
void Madm_System::delete_list_weighting_set_id(void){
	if(this->list_weight_set_id!=NULL){
		delete []this->list_weight_set_id;
		this->list_weight_set_id=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(int)*this->number_weight_sets,_sys_system_modules::MADM_SYS);
	}
}
//Import a weighting set to database table
void Madm_System::import_weighting_set(void){
	ostringstream cout;
	ostringstream prefix;
	if(this->number_analysis==0){
		cout << "No MADM-weighting set file is set for analysis!" << endl;
		Sys_Common_Output::output_madm->output_txt(&cout);
		return;
	}
	cout << "Import " << this->number_analysis <<" MADM-weighting set(s) from file(s)..." << endl;
	Sys_Common_Output::output_madm->output_txt(&cout);


	this->set_start_warnings_number();
	try{


		//initalize the defined criteria
		this->init_defined_criteria();
		//start import
		for(int i=0; i< this->number_analysis ; i++){
			try{
				//read them in and transfer it to database table
				this->read_in_weighting_set_file(this->file_names[i]);
			}
			catch(Error msg){
				this->number_error++;
				msg.output_msg(3);
			}
		}
	}
	catch(Error msg){
		msg.output_msg(3);
	}

	cout << "Import of MADM-weighting set(s) is finished" << endl;
	Sys_Common_Output::output_madm->output_txt(&cout);
	this->set_warning_number();
	this->output_error_statistic();

}
//Delete weighting sets in database table and all appending results in the database; the list of weighting sets ids have to be set before
void Madm_System::delete_weighting_sets_in_database(void){
	ostringstream cout;
	if(this->number_weight_sets==0){
		cout << "No MADM-weighting set are set for deleting!" << endl;
		Sys_Common_Output::output_madm->output_txt(&cout);
		return;
	}
	cout << "Delete " << this->number_weight_sets <<" MADM-weighting set(s) from database and the appending results..." << endl;
	Sys_Common_Output::output_madm->output_txt(&cout);


	try{
		for(int i=0; i< this->number_weight_sets; i++){
			Madm_System::delete_data_in_set_table(this->list_weight_set_id[i]);
			//Madm_System::delete_data_in_score_table(this->list_weight_set_id[i]);
			Madm_System::delete_data_in_ranking_table(this->list_weight_set_id[i]);
		}
	}
	catch(Error msg){
		msg.output_msg(3);
	}
	cout << "Deleting of " << this->number_weight_sets <<" MADM-weighting set(s) from database is finished!" << endl;
	Sys_Common_Output::output_madm->output_txt(&cout);

}
//Perform a madm-analysis per file
void Madm_System::analysis_per_file(void){
	ostringstream cout;
	ostringstream prefix;
	if(this->number_analysis==0){
		cout << "No MADM-system file is set for analysis!" << endl;
		Sys_Common_Output::output_madm->output_txt(&cout);
		return;
	}
	cout << "Analyse MADM-system(s) " << this->number_analysis <<" from file(s)..." << endl;
	Sys_Common_Output::output_madm->output_txt(&cout);



	this->set_start_warnings_number();
	//begin time recording	
	time(&this->start_time);
	try{
		//start calculation
		for(int i=0; i< this->number_analysis ; i++){
			try{
				//allocate the section
				this->allocate_analysis();
				Madm_System::check_stop_thread_flag();
				//input per file
				this->analysis->read_analysis_per_file(this->file_names[i]);
				Madm_System::check_stop_thread_flag();
				//output to display
				this->analysis->set_counter_approaches(0);
				this->number_error=this->analysis->calculate_matrix_file();
				Madm_System::check_stop_thread_flag();
			}
			catch(Error msg){
				if(Madm_System::abort_thread_flag==true){
					this->delete_analysis();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(3);
			}
			this->delete_analysis();
		}
	}
	catch(Error msg){
		if(Madm_System::abort_thread_flag==false){
			this->number_error++;
		}
		msg.output_msg(3);
	}

	cout << "Analysis of MADM-system file(s) is finished" << endl;
	Sys_Common_Output::output_madm->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_calculation();
}
//Perform a madm-analysis per database
void Madm_System::analysis_per_database(void){
	ostringstream cout;
	ostringstream prefix;
	int counter=0;
	try{
		this->delete_data_in_matrix_table();
		this->delete_data_in_ranking_table();
		//set alternatives
		Alt_System my_alt;
		my_alt.load_list_current_alt(&this->qsqldatabase, this->system_id.area_state);
		Madm_System::check_stop_thread_flag();

		if(my_alt.get_current_alternatives()==0){
			cout << "No measures are set for a MADM-analysis!" << endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			return;
		}
		//set defined criteria 
		this->init_defined_criteria();
		//set weighting sets
		this->load_weighting_set_list();
		if(this->number_weight_sets==0){
			cout << "No weighting sets are set for a MADM-analysis!" << endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			return;
		}
		cout << "Analyse " << my_alt.get_current_alternatives()<<" measures via a MADM-analysis for " << this->number_weight_sets<<" weighting sets..." << endl;
		Sys_Common_Output::output_madm->output_txt(&cout);

		this->set_start_warnings_number();
		//begin time recording	
		time(&this->start_time);

		//delete existing results
		for(int i=0; i< this->number_weight_sets; i++){
			this->delete_data_in_ranking_table(this->list_weight_set_id[i]);
		}
	
		bool matrix_ok=false;
	
		this->allocate_analysis();
		//for nobreak risk criteria
		prefix << "NOB_RISK> ";
		Sys_Common_Output::output_madm->set_userprefix(&prefix);
		matrix_ok=this->analysis->set_decision_matrix2database(&this->qsqldatabase, this->system_id.area_state, _risk_type::nobreak_risk, my_alt.get_current_alternatives(), my_alt.get_ptr_alt_info());
		if(matrix_ok==true){
			this->analysis->read_analysis_per_database(&this->qsqldatabase, my_alt.get_current_alternatives(), my_alt.get_ptr_alt_info(), this->number_defined_criteria, defined_criteria, _risk_type::nobreak_risk);
			for(int i=0; i< this->number_weight_sets; i++){
				this->analysis->set_counter_approaches(0);
				try{
					Madm_System::check_stop_thread_flag();
					counter++;
					this->set_current_weight_set(this->list_weight_set_id[i]);
					prefix << this->weight_set_info.name<<"> ";
					Sys_Common_Output::output_madm->set_userprefix(&prefix);
					this->analysis->change_criteria_weight_matrix(this->number_defined_criteria, defined_criteria);
					this->analysis->calculate_matrix_database(&this->qsqldatabase, this->system_id.area_state, _risk_type::nobreak_risk, this->weight_set_info.id);
					
					
					Sys_Common_Output::output_madm->rewind_userprefix();
				}
				catch(Error msg){
					Sys_Common_Output::output_madm->rewind_userprefix();
					if(Madm_System::get_stop_thread_flag()==true){
						throw msg;
					}
					else{
						this->number_error++;
						ostringstream info;
						info<< "Risk type    : nobreak risk criteria"<<endl;
						info<< "Set name     : " <<this->weight_set_info.name<<endl;
						msg.output_msg(3);
					}
				}

			}
		}
		else{
			cout <<"Decision-matrix could not be set completly. No MADM-analysis is performed."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
		}
		Sys_Common_Output::output_madm->rewind_userprefix();

		//for catchment risk criteria
		prefix << "CA_RISK> ";
		Sys_Common_Output::output_madm->set_userprefix(&prefix);
		matrix_ok=this->analysis->set_decision_matrix2database(&this->qsqldatabase, this->system_id.area_state, _risk_type::catchment_risk, my_alt.get_current_alternatives(), my_alt.get_ptr_alt_info());
		if(matrix_ok==true){
			this->delete_analysis();
			this->allocate_analysis();
			this->analysis->read_analysis_per_database(&this->qsqldatabase, my_alt.get_current_alternatives(), my_alt.get_ptr_alt_info(), this->number_defined_criteria, defined_criteria, _risk_type::catchment_risk);
			for(int i=0; i< this->number_weight_sets; i++){
				this->analysis->set_counter_approaches(0);
				try{
					Madm_System::check_stop_thread_flag();
					counter++;
					this->set_current_weight_set(this->list_weight_set_id[i]);
					prefix << this->weight_set_info.name<<"> ";
					Sys_Common_Output::output_madm->set_userprefix(&prefix);
					this->analysis->change_criteria_weight_matrix(this->number_defined_criteria, defined_criteria);
					this->analysis->calculate_matrix_database(&this->qsqldatabase, this->system_id.area_state, _risk_type::catchment_risk, this->weight_set_info.id);

					Sys_Common_Output::output_madm->rewind_userprefix();
				}
				catch(Error msg){
					Sys_Common_Output::output_madm->rewind_userprefix();
					if(Madm_System::get_stop_thread_flag()==true){
						throw msg;
					}
					else{
						this->number_error++;
						ostringstream info;
						info<< "Risk type    : catchment area risk criteria"<<endl;
						info<< "Set name     : " <<this->weight_set_info.name<<endl;
						msg.output_msg(3);
					}
				}	
			}
		}
		else{
			cout <<"Decision-matrix could not be set completly. No MADM-analysis is performed."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
		}

		Sys_Common_Output::output_madm->rewind_userprefix();

		
	}
	catch(Error msg){
		Sys_Common_Output::output_madm->rewind_userprefix();
		if(Madm_System::abort_thread_flag==false){
			this->number_error++;
		}
		msg.output_msg(3);
	}

	cout << "Analysis of measures via a MADM-analysis is finished" << endl;
	Sys_Common_Output::output_madm->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_calculation_database(counter);

}
//Set the number of warnings before an action
void Madm_System::set_start_warnings_number(void){
	this->number_warning_begin=Warning::get_number_madm_warnings();
}
//Set the warning number, which occured during an action
void Madm_System::set_warning_number(void){
	this->number_warning=Warning::get_number_madm_warnings()-this->number_warning_begin;
}
//Get the warning number, which occurred during an action
int Madm_System::get_occured_warning(void){
	return this->number_warning;
}
//Output the error statistic of the madm system
void Madm_System::output_error_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA> ";
	Sys_Common_Output::output_madm->set_userprefix(prefix.str());
	cout << "Error statistics of the MADM-system..." << endl ;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)                 :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)               :" << W(3) << this->get_occured_warning() <<endl;

	if(Madm_System::get_stop_thread_flag()==true){
		cout << " User has aborted the action " << endl;
	}
	Sys_Common_Output::output_madm->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_madm->rewind_userprefix();
}
//Output the statistic of the madm-analysis
void Madm_System::output_final_statistic_calculation(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_madm->set_userprefix(prefix.str());
	cout << "Statistics of the MADM-analysis..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Number analysis      :" << W(3) << this->number_analysis <<endl;
	cout << " Time                 :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)             :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)           :" << W(3) << this->number_warning <<endl;
	if(Madm_System::abort_thread_flag==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_madm->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_madm->rewind_userprefix();
}
//Output the statistic of the madm-analysis per database
void Madm_System::output_final_statistic_calculation_database(const int counter){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_madm->set_userprefix(prefix.str());
	cout << "Statistics of the MADM-analysis..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Number analysis      :" << W(3) << counter <<endl;
	cout << " Time                 :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)             :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)           :" << W(3) << this->number_warning <<endl;
	if(Madm_System::abort_thread_flag==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_madm->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_madm->rewind_userprefix();
}
//Transfer a weighting set to database
void Madm_System::transfer_weighting2database(void){

	try{
		Madm_System::set_table_sets(&this->qsqldatabase);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream prefix;
	ostringstream cout;
	prefix << "TRANS> ";
	Sys_Common_Output::output_madm->set_userprefix(prefix.str());
	cout << "Transfer MADM-weighting set " << this->weight_set_info.name << " to database..." << endl ;
	Sys_Common_Output::output_madm->output_txt(&cout);
	int glob_id=0;

	glob_id=Madm_System::table_sets->maximum_int_of_column(Madm_System::table_sets->get_column_name(label::glob_id),&this->qsqldatabase)+1;


	QSqlQuery model(this->qsqldatabase);
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Madm_System::table_sets->get_table_name();
	query_string <<" ( ";
	query_string << Madm_System::table_sets->get_column_name(label::glob_id) <<" , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::set_id) <<" , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::set_name) <<" , ";
	//weights
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ecn) <<" , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_eco) <<" , ";

	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_pop_aff) <<" , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_pop_dan) <<" , ";

	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_pys) <<" , ";

	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_pub) <<" , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_eco) <<" , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_cult) <<" , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_person) <<" , ";

	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_elect_pt) << " , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_info_tec_pt) << " , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_water_sup_pt) << " , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_water_treat_pt) << " , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_energy_pt) << " , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_health_pt) << " , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_social_pt) << " , ";
	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_mat_pt) << " , ";


	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_risk_outflow) <<" , ";

	query_string << Madm_System::table_sets->get_column_name(madm_label::crit_cost_ecn) ;

	query_string <<" ) ";
	
	query_string << " VALUES ( ";
	query_string << glob_id << " , " ;
	query_string << glob_id << " , " ;
	query_string << "'"<< this->weight_set_info.name<< "' , " ;
	//weights
	query_string << this->defined_criteria[0].weight << " , " ;
	query_string << this->defined_criteria[1].weight << " , " ;
	query_string << this->defined_criteria[2].weight << " , " ;
	query_string << this->defined_criteria[3].weight << " , " ;
	query_string << this->defined_criteria[4].weight << " , " ;
	query_string << this->defined_criteria[5].weight << " , " ;
	query_string << this->defined_criteria[6].weight << " , " ;
	query_string << this->defined_criteria[7].weight << " , " ;
	query_string << this->defined_criteria[8].weight << " , " ;

	query_string << this->defined_criteria[9].weight << " , ";
	query_string << this->defined_criteria[10].weight << " , ";
	query_string << this->defined_criteria[11].weight << " , ";
	query_string << this->defined_criteria[12].weight << " , ";
	query_string << this->defined_criteria[13].weight << " , ";
	query_string << this->defined_criteria[14].weight << " , ";
	query_string << this->defined_criteria[15].weight << " , ";
	query_string << this->defined_criteria[16].weight << " , ";


	query_string << this->defined_criteria[17].weight << " , " ;
	query_string << this->defined_criteria[18].weight << " ) " ;

	Data_Base::database_request(&model, query_string.str(), &this->qsqldatabase);
	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Madm_System::table_sets->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Name of the weighting set : " << this->weight_set_info.name <<endl;	
		msg.make_second_info(info.str());
		msg.output_msg(3);	
	}

	//rewind the prefix
	Sys_Common_Output::output_madm->rewind_userprefix();
}
//Check the file for the madm-weighting sets if the !$BEGIN... always are close with !$END...
void Madm_System::check_input_file_weighting_sets(const string file_name){
	int line_counter=0;
	QFile ifile;
	//open file
	ifile.setFileName(file_name.c_str());
	QIODevice::OpenMode my_flag;
	my_flag=my_flag|(QIODevice::ReadOnly);
	my_flag=my_flag|(QIODevice::Text);
	ifile.open(my_flag);
	if(ifile.isOpen()==false){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Filename: " << file_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//check the file
	string myline;
	QString qmyline;
	QStringList list_begin;
	QString id_str;
	bool flag;
	try{
		do{
			qmyline=ifile.readLine();
			myline=qmyline.toStdString();
			line_counter++;
			functions::clean_string(&myline);
			id_str=myline.c_str();
			flag=functions::decide_begin_end_keyword(&id_str);
			if(flag==true && id_str.isEmpty()==false){
				list_begin.append(id_str);

			}
			else if(flag==false && id_str.isEmpty()==false){
				if(list_begin.last()==id_str){
					list_begin.removeLast();
				}
				else{
					Error msg=this->set_error(11);
					ostringstream info;
					info << "Error occurs near line: "<< line_counter << endl;
					info << "Filename              : "<< file_name << endl;
					info << "Key                   : "<< "!$END_"<<id_str.toStdString()<< endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
		while(ifile.atEnd()==false);
		
		if(list_begin.count()!=0){
			Error msg=this->set_error(10);
			ostringstream info;
			info << "Filename                 : "<< file_name << endl;
			info << "Number blocks not closed : "<< list_begin.count()<< endl;
			for(int i=0; i< list_begin.count(); i++){
				info << "Key                      : "<< "!$BEGIN_"<<list_begin.at(i).toStdString()<< endl;	
			}
			msg.make_second_info(info.str());
			throw msg;
		}

	}
	catch(Error msg){
		ifile.close();
		throw msg;
	}

	//close file
	ifile.close();
}
//Read in weighting set file
void Madm_System::read_in_weighting_set_file(const string filename){
	ostringstream cout;
	try{
		cout << "Check the input-file of the MADM-weighting sets " << filename << "..."<<endl;
		Sys_Common_Output::output_madm->output_txt(&cout);
		this->check_input_file_weighting_sets(filename);
	}
	catch(Error msg){
		throw msg;
	}


	ostringstream prefix;
	int line_counter=0;
	QFile ifile;
	prefix << "INP> ";
	Sys_Common_Output::output_madm->set_userprefix(&prefix);
	cout << "Input the weighting set(s) from file " << filename << endl;
	Sys_Common_Output::output_madm->output_txt(&cout);
	try{	
		//open file
		ifile.setFileName(filename.c_str());
		QIODevice::OpenMode my_flag;
		my_flag=my_flag|(QIODevice::ReadOnly);
		my_flag=my_flag|(QIODevice::Text);

		ifile.open(my_flag);
		if(ifile.isOpen()==false){
			Error msg=this->set_error(2);
			ostringstream info;
			info << "Filename: " << filename << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//read it in
		string myline;
		QString qmyline;
		stringstream my_stream;
		bool found_flag=false;
	
		//search for the number of sets in file
		do{
			qmyline=ifile.readLine();
			myline=qmyline.toStdString();
			line_counter++;
			functions::clean_string(&myline);

			if(myline==madm_label::begin_weight_set){
				break;
			}
			else if(myline==madm_label::end_weight_set){
				break;
			}
			else if(myline.empty()==false){
				if(this->find_number_weighting_set_file(myline)==true){
					found_flag=true;
					break;
				}
			}
		}
		while(ifile.atEnd()==false);

		if(found_flag==false){
			Error msg=this->set_error(4);
			throw msg;
		}
		else{
			found_flag=false;
		}


		//search for the weighting set
		this->read_in_weighting_set_block(&ifile, &line_counter);


	}
	catch(Error msg){
		ifile.close();
		Sys_Common_Output::output_madm->rewind_userprefix();
		ostringstream info;
		info << "Error occurs near line: "<< line_counter << endl;
		info << "Filename              : "<< filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	ifile.close();
	Sys_Common_Output::output_madm->rewind_userprefix();

}
//Read in the weighting set block per file
void Madm_System::read_in_weighting_set_block(QFile *ifile, int *line_counter){
	//read it in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	int set_counter=0;

	do{
		//search for start of the set
		do{
			qmyline=ifile->readLine();
			myline=qmyline.toStdString();
			line_counter++;
			functions::clean_string(&myline);

			if(myline==madm_label::begin_weight_set){
				found_flag=true;
				set_counter++;
				break;
			}
			else if(myline==madm_label::end_weight_set){
				break;
			}
		}
		while(ifile->atEnd()==false);

		if(found_flag==false){
			Error msg=this->set_error(6);
			throw msg;
		}
		else{
			found_flag=false;
		}

		//read in the set
		int must_found_counter=0;
		do{
			qmyline=ifile->readLine();
			myline=qmyline.toStdString();
			line_counter++;
			functions::clean_string(&myline);

			if(myline==madm_label::begin_weight_set){
				break;
			}
			else if(myline==madm_label::end_weight_set){
				break;
			}
			else if(myline.empty()==false){
				this->find_keywords_file_weighting_set(myline, &must_found_counter);

			}
		}
		while(ifile->atEnd()==false);


		if(must_found_counter!=this->number_defined_criteria+1){
			Error msg=this->set_error(7);
			ostringstream info;
			info <<"Number found criteria : " <<must_found_counter-1 << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//transfer it to database
		this->transfer_weighting2database();
	}
	while(ifile->atEnd()==false && set_counter<this->number_weight_sets);


	if(set_counter < this->number_weight_sets){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Found weighting sets   : " << set_counter << endl;
		info <<"Required weighting sets: " << this->number_weight_sets<<endl;
		msg.make_second_info(info.str());
		msg.output_msg(3);
	}

}
//Find the number of weighting sets in file
bool Madm_System::find_number_weighting_set_file(const string myline){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(madm_label::no_weight_set );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::no_weight_set.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->number_weight_sets;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return true;
		}
	}


	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(3);
		msg.make_second_info(info.str());
		throw msg;
	}

	return false;
}
//Find keywords in file for the weighting set
void Madm_System::find_keywords_file_weighting_set(const string myline, int *must_found){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	//set name
	pos=myline.find(madm_label::weight_set_name);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::weight_set_name.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->weight_set_info.name;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	//risk crit
	pos=myline.find(madm_label::weight_risk_ecn);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::weight_risk_ecn.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[0].weight;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	pos=myline.find(madm_label::weight_risk_eco);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::weight_risk_eco.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[1].weight;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	pos=myline.find(madm_label::weight_risk_pop_aff);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::weight_risk_pop_aff.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[2].weight;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	pos=myline.find(madm_label::weight_risk_pop_dan);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::weight_risk_pop_dan.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[3].weight;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	pos=myline.find(madm_label::weight_risk_pys);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::weight_risk_pys.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[4].weight;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	pos=myline.find(madm_label::weight_risk_sc_pub);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::weight_risk_sc_pub.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[5].weight;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	pos=myline.find(madm_label::weight_risk_sc_eco);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::weight_risk_sc_eco.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[6].weight;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	pos=myline.find(madm_label::weight_risk_sc_cult);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::weight_risk_sc_cult.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[7].weight;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	pos=myline.find(madm_label::weight_risk_sc_person);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::weight_risk_sc_person.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[8].weight;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	pos = myline.find(madm_label::weight_risk_ci_elect);
	if (pos >= 0 && wrong_input == false) {
		buffer = myline.substr(madm_label::weight_risk_ci_elect.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[9].weight;
		if (buffer1.fail() == true) {
			wrong_input = true;
		}
		else {
			(*must_found)++;
			return;
		}
	}

	pos = myline.find(madm_label::weight_risk_ci_info_tec);
	if (pos >= 0 && wrong_input == false) {
		buffer = myline.substr(madm_label::weight_risk_ci_info_tec.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[10].weight;
		if (buffer1.fail() == true) {
			wrong_input = true;
		}
		else {
			(*must_found)++;
			return;
		}
	}


	pos = myline.find(madm_label::weight_risk_wat_sup);
	if (pos >= 0 && wrong_input == false) {
		buffer = myline.substr(madm_label::weight_risk_wat_sup.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[11].weight;
		if (buffer1.fail() == true) {
			wrong_input = true;
		}
		else {
			(*must_found)++;
			return;
		}
	}

	pos = myline.find(madm_label::weight_risk_ci_wat_treat);
	if (pos >= 0 && wrong_input == false) {
		buffer = myline.substr(madm_label::weight_risk_ci_wat_treat.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[12].weight;
		if (buffer1.fail() == true) {
			wrong_input = true;
		}
		else {
			(*must_found)++;
			return;
		}
	}

	pos = myline.find(madm_label::weight_risk_ci_energy);
	if (pos >= 0 && wrong_input == false) {
		buffer = myline.substr(madm_label::weight_risk_ci_energy.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[13].weight;
		if (buffer1.fail() == true) {
			wrong_input = true;
		}
		else {
			(*must_found)++;
			return;
		}
	}

	pos = myline.find(madm_label::weight_risk_ci_health);
	if (pos >= 0 && wrong_input == false) {
		buffer = myline.substr(madm_label::weight_risk_ci_health.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[14].weight;
		if (buffer1.fail() == true) {
			wrong_input = true;
		}
		else {
			(*must_found)++;
			return;
		}
	}

	pos = myline.find(madm_label::weight_risk_ci_social);
	if (pos >= 0 && wrong_input == false) {
		buffer = myline.substr(madm_label::weight_risk_ci_social.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[15].weight;
		if (buffer1.fail() == true) {
			wrong_input = true;
		}
		else {
			(*must_found)++;
			return;
		}
	}

	pos = myline.find(madm_label::weight_risk_ci_mat);
	if (pos >= 0 && wrong_input == false) {
		buffer = myline.substr(madm_label::weight_risk_ci_mat.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[16].weight;
		if (buffer1.fail() == true) {
			wrong_input = true;
		}
		else {
			(*must_found)++;
			return;
		}
	}




	pos=myline.find(madm_label::weight_risk_outflow);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::weight_risk_outflow.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[17].weight;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	//cost
	pos=myline.find(madm_label::weight_cost_ecn);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::weight_cost_ecn.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->defined_criteria[18].weight;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(8);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Load weighting set id-list per database
void Madm_System::load_weighting_set_list(void){

	QSqlQueryModel model;
	int number=Madm_System::select_data_in_settable(&model, &this->qsqldatabase);
	if(number==0){
		Error msg=this->set_error(12);
		throw msg;

	}
	else{

		this->delete_list_weighting_set_id();
		this->number_weight_sets=number;
		this->allocate_list_weighting_set_id();

		for(int i=0; i< this->number_weight_sets; i++){
			this->list_weight_set_id[i]=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::set_id)).c_str()).toInt();
		}
	}
}
//Set warning(s)
Warning Madm_System::set_warning(const int warn_type){
	string place="Madm_System::";
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
		case 1://input datas can not submitted
			place.append("transfer_weighting2database(void)");
			reason="Can not submit the weighting set data to the database";
			help="Check the database";
			type=2;
			break;
		case 2://not all weighting sets are found
			place.append("read_in_weighting_set_block(QFile *ifile, int *line_counter)");
			reason="Not all weighting sets are found in file";
			help="Check the database";
			reaction="Not all weighting sets can be set";
			type=1;
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
Error Madm_System::set_error(const int err_type){
	string place="Madm_System::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_file_names(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("allocate_analysis(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://can not open madm-weight-set file
			place.append("read_in_weighting_set_file(const sting filename)");
			reason="Can not open the file of the MADM-weighting sets";
			help="Check the file";
			type=5;
			break;
		case 3://wrong input
			place.append("find_number_weighting_set_file(QFile *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the number of weighting set in file";
			type=1;
			break;
		case 4://do not find the number of weighting sets in file
			place.append("read_in_weighting_set_file(const string filename)");
			reason="Can not find the number of weighting sets in file";
			help="The weighting set number have to be the first information in file and are declareted with !$no_weight_set";
			type=1;
			break;
		case 5://bad alloc
			place.append("allocate_defined_criteria(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 6://do not find the begin of the weighting ser block in file
			place.append("read_in_weighting_set_block(QFile *ifile, int *line_counter)");
			reason="Can not find the begin of the weighting set block in file";
			help="The matrix information has to start with !$BEGIN_SET";
			type=1;
			break;
		case 7://do not find all weights
			place.append("read_in_weighting_set_block(QFile *ifile, int *line_counter)");
			reason="Can not find all required weights for the defined criteria";
			help="Check if all defined criteria are set in the file";
			info << "Required information :"<<endl;
			info << " " <<madm_label::weight_set_name<<endl;
			info << "Required criteria ("<<this->number_defined_criteria<<")  :"<<endl;
			info << " " << madm_label::crit_risk_ecn<< endl;
			info << " " << madm_label::crit_risk_eco<< endl;
			info << " " << madm_label::crit_risk_pop_aff<< endl;
			info << " " << madm_label::crit_risk_pop_dan<< endl;
			info << " " << madm_label::crit_risk_pys<< endl;
			info << " " << madm_label::crit_risk_outflow<< endl;
			info << " " << madm_label::crit_cost_ecn<< endl;
			type=1;
			break;
		case 8://wrong input
			place.append("find_keywords_file_weighting_set(const string myline, const int  *must_found)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the criteria weight block in file";
			type=1;
			break;
		case 9://bad alloc
			place.append("allocate_list_weighting_set_id(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 10://!$BEGIN is not correctly closed
			place.append("check_input_file_weighting_sets(const string file_name)");
			reason="The !$BEGIN-key is not correctly closed with the corresponding !$END-key";
			help="Check the file and close the blocks correctly";
			type=5;
			break;
		case 11://!$END is not correctly opened
			place.append("check_input_file_weighting_sets(const string file_name)");
			reason="The !$END-key is not correctly opened with the corresponding !$BEGIN-key";
			help="Check the file and start the blocks correctly";
			type=5;
			break;
		case 12://no weighting sets are found in database table
			place.append("load_weighting_set_list(void)");
			reason="No weighting sets are found in database table";
			help="Check the weighting sets in database table";
			type=30;
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