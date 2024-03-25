#include "Alt_Headers_Precompiled.h"
//#include "Alt_System.h"

//init static members
bool Alt_System::abort_thread_flag=false;
Tables *Alt_System::table_measure=NULL;
Tables *Alt_System::table_replacing=NULL;

//Default destructor
Alt_System::Alt_System(void){
	this->thread_type=_alt_thread_type::alt_thread_unknown;

	this->list_alt=NULL;
	this->number_alt=0;

	this->thread_is_started=false;

	this->measure_implemt_ok=false;
	this->general_alt_infos.measurestate_based_on=0;

	this->hyd_system=NULL;
	this->fpl_system=NULL;
	this->dam_system=NULL;

	this->new_state_id.area_state=0;
	this->new_state_id.measure_nr=0;
	this->switch_state_id.area_state=0;
	this->switch_state_id.measure_nr=0;

	Sys_Memory_Count::self()->add_mem(sizeof(Alt_System),_sys_system_modules::ALT_SYS);//count the memory
}
//Default destructor
Alt_System::~Alt_System(void){
	this->delete_fpl_system();
	this->delete_hyd_system();
	this->delete_dam_system();
	this->delete_alt_list();
	//close database
	if(this->qsqldatabase.isOpen()){
		this->qsqldatabase.close();
	}

	Sys_Memory_Count::self()->minus_mem(sizeof(Alt_System),_sys_system_modules::ALT_SYS);//count the memory
}
//______
//public
//Set the database table for the measures: it sets the table name and the name of the columns and allocate them (static)
void Alt_System::set_table_measure(QSqlDatabase *ptr_database){
	if(Alt_System::table_measure==NULL){
		//make specific input for this class
		const string tab_id_name=alt_label::tab_measures;
		string tab_col[8];

		tab_col[0]=label::glob_id;
		tab_col[1]=label::areastate_id;
		tab_col[2]=label::measure_id;
		tab_col[3]=alt_label::measure_name;
		tab_col[4]=alt_label::measure_cat;
		tab_col[5]=alt_label::measure_sub_cat;
		tab_col[6]=alt_label::measure_replacing;
		tab_col[7]=label::description;

		//set the table
		try{
			Alt_System::table_measure= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Alt_System::table_measure->set_name(ptr_database, _sys_table_type::alt);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Alt_System::set_table_measure(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Alt_System::close_table_measure();
			throw msg;
		}
	}
}
//Create the database table for measures (static)
void Alt_System::create_table_measure(QSqlDatabase *ptr_database){
	if(Alt_System::table_measure==NULL){
		ostringstream cout;
		cout << "Create database table of measures..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		//make specific input for this class
		const string tab_name=alt_label::tab_measures;
		const int num_col=8;
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

		tab_col[3].name=alt_label::measure_name;
		tab_col[3].type=sys_label::tab_col_type_string;

		tab_col[4].name=alt_label::measure_cat;
		tab_col[4].type=sys_label::tab_col_type_string;

		tab_col[5].name=alt_label::measure_sub_cat;
		tab_col[5].type=sys_label::tab_col_type_string;

		tab_col[6].name=alt_label::measure_replacing;
		tab_col[6].type=sys_label::tab_col_type_bool;

		tab_col[7].name=label::description;
		tab_col[7].type=sys_label::tab_col_type_string;

		try{
			Alt_System::table_measure= new Tables();
			if(Alt_System::table_measure->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::alt)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Alt_System::create_table_measure(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Alt_System::close_table_measure();
			throw msg;
		}
		Alt_System::close_table_measure();
	}
}
//Delete all data in the database table of the measures (static)
void Alt_System::delete_data_in_table_measure(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Alt_System::set_table_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Alt_System::table_measure->delete_data_in_table(ptr_database);
}
//Delete data in the database table of the measures specified by the paramters (static)
void Alt_System::delete_data_in_table_measure(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Alt_System::set_table_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "DELETE FROM ";
	test_filter << Alt_System::table_measure->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Alt_System::table_measure->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND " ;
	test_filter << Alt_System::table_measure->get_column_name(label::measure_id) << " = "  << id.measure_nr;

	Data_Base::database_request(&model, test_filter.str(), ptr_database);
}
//Close and delete the database table for the measures (static)
void Alt_System::close_table_measure(void){
	if(Alt_System::table_measure!=NULL){
		delete Alt_System::table_measure;
		Alt_System::table_measure=NULL;
	}
}
//Get the name and description of the measure from database table (static)
bool Alt_System::get_name_descript_measure_table(QSqlDatabase *ptr_database, _sys_system_id id, string *name, string *descript){
	try{
		Alt_System::set_table_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Alt_System::table_measure->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Alt_System::table_measure->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND ";
	test_filter << Alt_System::table_measure->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	Data_Base::database_request(&model, test_filter.str(), ptr_database);

	//check the request
	if(model.lastError().isValid()){
		Error msg;
		msg.set_msg("Alt_System::get_name_descript_measure_table(QSqlDatabase *ptr_database, _sys_system_id id, string *name, string *descript)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Alt_System::table_measure->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(model.rowCount()>0){
		*name=model.record(0).value((Alt_System::table_measure->get_column_name(alt_label::measure_name)).c_str()).toString().toStdString();
		*descript=model.record(0).value((Alt_System::table_measure->get_column_name(label::description)).c_str()).toString().toStdString();

		return true;
	}
	else{
		return false;
	}
}
//Load the string-list (name/id) of the current avaliable alternatives in the table (static)
void Alt_System::load_stringlist_current_alt(QSqlDatabase *ptr_database, const int area_state, QStringList *list){


	try{
		Alt_System::set_table_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Alt_System::table_measure->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Alt_System::table_measure->get_column_name(label::areastate_id) << " = "  << area_state;
	test_filter << " ORDER BY " << Alt_System::table_measure->get_column_name(label::measure_id);
	Data_Base::database_request(&model, test_filter.str(), ptr_database);

	//check the request
	if(model.lastError().isValid()){
		Error msg;
		msg.set_msg("Alt_System::load_stringlist_current_alt(QSqlDatabase *ptr_database, const int area_state, QStringList *list)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Alt_System::table_measure->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(model.rowCount()>0){
		for(int i=0; i< model.rowCount() ; i++){
			ostringstream info;
			info << model.record(i).value((Alt_System::table_measure->get_column_name(label::measure_id)).c_str()).toInt() << " ";
			info << model.record(i).value((Alt_System::table_measure->get_column_name(alt_label::measure_name)).c_str()).toString().toStdString();
			list->append(info.str().c_str());
		}
	}

}
//Check if the measure is already set in database and get the information (static)
bool Alt_System::check_measure_is_set_database(QSqlDatabase *ptr_database, const _sys_system_id id,_alt_general_info *data){
	//the table is set (the name and the column names) and allocated
	try{
		Alt_System::set_table_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Alt_System::table_measure->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Alt_System::table_measure->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND " ;
	test_filter << Alt_System::table_measure->get_column_name(label::measure_id) << " = "  << id.measure_nr;

	Data_Base::database_request(&model, test_filter.str(), ptr_database);

	//check the request
	if(model.lastError().isValid()){
		Error msg;
		msg.set_msg("Alt_System::check_measure_is_set_database(QSqlDatabase *ptr_database, const _sys_system_id id,_alt_general_info *data)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Alt_System::table_measure->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(model.rowCount()>0){
		data->name=model.record(0).value((Alt_System::table_measure->get_column_name(alt_label::measure_name)).c_str()).toString().toStdString();
		data->description=model.record(0).value((Alt_System::table_measure->get_column_name(label::description)).c_str()).toString().toStdString();
		data->id=id;
		data->category=Alt_System::convert_txt2measure_category(model.record(0).value((Alt_System::table_measure->get_column_name(alt_label::measure_cat)).c_str()).toString().toStdString());
		data->detailed_type=model.record(0).value((Alt_System::table_measure->get_column_name(alt_label::measure_sub_cat)).c_str()).toString().toStdString();
		data->replacing=model.record(0).value((Alt_System::table_measure->get_column_name(alt_label::measure_replacing)).c_str()).toBool();
		return true;
	}
	else{
		return false;
	}
}
//Check if a measure is already set in database and get the information (static)
bool Alt_System::check_measure_is_set_database(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Alt_System::set_table_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Alt_System::table_measure->get_table_name();

	Data_Base::database_request(&model, test_filter.str(), ptr_database);

	//check the request
	if(model.lastError().isValid()){
		Error msg;
		msg.set_msg("Alt_System::check_measure_is_set_database(QSqlDatabase *ptr_database,)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Alt_System::table_measure->get_table_name() << endl;
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
//Load the list of the current avaliable alternatives in table
void Alt_System::load_list_current_alt(QSqlDatabase *ptr_database, const int area_state){
	try{
		Alt_System::set_table_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Alt_System::table_measure->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Alt_System::table_measure->get_column_name(label::areastate_id) << " = "  << area_state;
	test_filter << " ORDER BY " << Alt_System::table_measure->get_column_name(label::measure_id);
	Data_Base::database_request(&model, test_filter.str(), ptr_database);

	//check the request
	if(model.lastError().isValid()){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Table Name      : " << Alt_System::table_measure->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(model.rowCount()>0){
		this->delete_alt_list();
		this->number_alt=model.rowCount();
		this->allocate_alt_list();
		for(int i=0; i< this->number_alt ; i++){
			this->list_alt[i].name=model.record(i).value((Alt_System::table_measure->get_column_name(alt_label::measure_name)).c_str()).toString().toStdString();
			this->list_alt[i].description=model.record(i).value((Alt_System::table_measure->get_column_name(label::description)).c_str()).toString().toStdString();
			this->list_alt[i].id.area_state=area_state;
			this->list_alt[i].id.measure_nr=model.record(i).value((Alt_System::table_measure->get_column_name(label::measure_id)).c_str()).toInt();
			this->list_alt[i].category=Alt_System::convert_txt2measure_category(model.record(i).value((Alt_System::table_measure->get_column_name(alt_label::measure_cat)).c_str()).toString().toStdString());
		}
	}
}
//Set the database table for the replacing measures: it sets the table name and the name of the columns and allocate them (static)
void Alt_System::set_table_replacing_measure(QSqlDatabase *ptr_database){
	if(Alt_System::table_replacing==NULL){
		//make specific input for this class
		const string tab_id_name=alt_label::tab_replacing_measures;
		string tab_col[5];

		tab_col[0]=label::glob_id;
		tab_col[1]=label::areastate_id;
		tab_col[2]=label::measure_id;
		tab_col[3]=alt_label::replacing_tab_name;
		tab_col[4]=alt_label::replacing_id;

		//set the table
		try{
			Alt_System::table_replacing= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Alt_System::table_replacing->set_name(ptr_database, _sys_table_type::alt);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Alt_System::set_table_replacing_measure(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Alt_System::close_table_replacing_measure();
			throw msg;
		}
	}
}
//Create the database table for replacing measures (static)
void Alt_System::create_table_replacing_measure(QSqlDatabase *ptr_database){
	if(Alt_System::table_replacing==NULL){
		ostringstream cout;
		cout << "Create database table of detailed data of the replacing measures..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		//make specific input for this class
		const string tab_name=alt_label::tab_replacing_measures;
		const int num_col=5;
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

		tab_col[3].name=alt_label::replacing_tab_name;
		tab_col[3].type=sys_label::tab_col_type_string;

		tab_col[4].name=alt_label::replacing_id;
		tab_col[4].type=sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag=true;

		try{
			Alt_System::table_replacing= new Tables();
			if(Alt_System::table_replacing->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::alt)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Alt_System::create_table_replacing_measure(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Alt_System::close_table_replacing_measure();
			throw msg;
		}
		Alt_System::close_table_replacing_measure();
	}
}
//Delete all data in the database table of the replacing measures (static)
void Alt_System::delete_data_in_table_replacing_measure(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Alt_System::set_table_replacing_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Alt_System::table_replacing->delete_data_in_table(ptr_database);
}
//Delete data in the database table of the replacing measures with the given system_state
void Alt_System::delete_data_in_table_replacing_measure(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Alt_System::set_table_replacing_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "DELETE FROM ";
	test_filter << Alt_System::table_replacing->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Alt_System::table_replacing->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND " ;
	test_filter << Alt_System::table_replacing->get_column_name(label::measure_id) << " = "  << id.measure_nr;

	Data_Base::database_request(&model, test_filter.str(), ptr_database);
}
//Close and delete the database table for the replacing measures (static)
void Alt_System::close_table_replacing_measure(void){
	if(Alt_System::table_replacing!=NULL){
		delete Alt_System::table_replacing;
		Alt_System::table_replacing=NULL;
	}
}
//Get a new measure state id
int Alt_System::get_new_measure_state(QSqlDatabase *ptr_database){
//the table is set (the name and the column names) and allocated
	try{
		Alt_System::set_table_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	return  Alt_System::table_measure->maximum_int_of_column(label::measure_id, ptr_database)+1;
}
//Ask and set new measure data via dialog
bool Alt_System::ask_new_measure_data(QWidget *parent, const bool fpl_required){
	_alt_general_info data;
	AltGui_Alternative_Info_Dia my_dia(parent);
	this->new_state_id=this->system_id;
	this->new_state_id.measure_nr=Alt_System::get_new_measure_state(&this->qsqldatabase);

	my_dia.set_dialog(this->new_state_id, false, data, fpl_required);

	if(my_dia.start_dialog(&this->qsqldatabase, this->system_id)==false){
		return false;
	}
	else{
		this->general_alt_infos=my_dia.get_measure_general_data();
		if(AltGui_Alternative_Type_Dia::check_measure_type_valid(this->general_alt_infos.detailed_type)==false){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"A unknown measure type is used."<< endl;
			txt<<"This type is not yet implemented"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}
		this->file_list=my_dia.replacing_files;
		this->id_list=my_dia.replacing_ids;
		if(this->file_list.count()==0 && this->id_list.count()==0 && my_dia.current_ids.count()==0){
			return false;
		}
		if(this->general_alt_infos.detailed_type==alt_replace_hyd::boundary_sc){
			this->allocate_hyd_system();
			//can not be allocated in the not-gui-thread
			this->hyd_system->sz_bound_manager.allocate_scenario_dia();
		}
		else if(this->general_alt_infos.detailed_type==alt_replace_dam::dam_eco_btype_function_polygon ||
			this->general_alt_infos.detailed_type==alt_replace_dam::dam_ecn_function_polygon ||
			this->general_alt_infos.detailed_type==alt_replace_dam::dam_resettlement){
				this->id_list_current=my_dia.current_ids;
		}

		return true;
	}
}
//Edit the current measure data via dialog
bool Alt_System::ask_edit_measure_data(QWidget *parent, const bool fpl_required){
	_alt_general_info data;
	data.measurestate_based_on=0;
	if(this->check_measure_is_set_database(&this->qsqldatabase, this->system_id, &data)==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"There is no current measure set for editing!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return false;
	}
	else{
		AltGui_Alternative_Info_Dia my_dia(parent);
		my_dia.set_dialog(this->system_id, true, data, fpl_required);

		if(my_dia.start_dialog(&this->qsqldatabase, this->system_id)==false){
			return false;
		}
		else{
			this->system_id=my_dia.get_measure_general_data().id;
			Alt_System::delete_data_in_table_measure(&this->qsqldatabase, this->system_id);
			this->set_measure_data2database(&this->qsqldatabase, my_dia.get_measure_general_data());
			return true;
		}
	}
}
//Ask to which measure state should be switched
bool Alt_System::ask_measure_state2switch(QWidget *parent){
	AltGui_Measure_Dia my_dia(parent);
	this->load_list_current_alt(&this->qsqldatabase, this->system_id.area_state);
	my_dia.set_dialog(this->number_alt, this->list_alt, this->system_id.measure_nr);

	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->switch_state_id=this->system_id;
		this->switch_state_id.measure_nr=my_dia.get_selected_information();

		return true;
	}
}
//Ask which measure state should be deleted
bool Alt_System::ask_measure_state2delete(QWidget *parent){
	AltGui_Measure_Dia my_dia(parent);
	my_dia.set_heading("Select the measure state to delete...");
	this->load_list_current_alt(&this->qsqldatabase, this->system_id.area_state);
	cout <<" test1"<<endl;
	my_dia.set_dialog_delete(this->number_alt, this->list_alt, this->system_id.measure_nr);

	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->switch_state_id=this->system_id;
		this->switch_state_id.measure_nr=my_dia.get_selected_information();

		return true;
	}
}
//Set the thread type for specifiying the task of the thread
void Alt_System::set_thread_type(_alt_thread_type type){
	this->thread_type=type;
}
//Set a pointer to the database; the copy of the database is made in this method
void Alt_System::set_ptr2database(QSqlDatabase *ptr_database){
	this->qsqldatabase=QSqlDatabase::cloneDatabase(*ptr_database, sys_label::str_alt.c_str());
	this->qsqldatabase.open();
}
//Get the thread type
_alt_thread_type Alt_System::get_thread_type(void){
	return this->thread_type;
}
//Create a whole set of cost database tables
void Alt_System::create_alt_database_tables(void){
	ostringstream cout;
	cout << "Create ALT system tables..." << endl ;
	Sys_Common_Output::output_alt->output_txt(&cout);
	this->set_exception_number_start();
	try{
		Alt_System::create_table_measure(&this->qsqldatabase);
		Alt_System::create_table_replacing_measure(&this->qsqldatabase);
	}
	catch(Error msg){
		msg.output_msg(6);
	}
	this->set_exception_number_end();
	cout << "Creation of the ALT database tables is finished" << endl ;
	Sys_Common_Output::output_alt->output_txt(&cout);
	this->output_error_statistic();
}
//Check all alt database tables, for their existence in the database and their declaration in the database table-file
void Alt_System::check_alt_database_tables(void){
	ostringstream cout;
	cout << "Check ALT system tables..." << endl ;
	Sys_Common_Output::output_alt->output_txt(&cout,false, false);
	this->set_exception_number_start();
	try{
		cout << "Check database table of the measures..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout,false, false);
		Alt_System::check_stop_thread_flag();
		Alt_System::set_table_measure(&this->qsqldatabase);

		cout << "Check database table of the replacing measures..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout,false, false);
		Alt_System::check_stop_thread_flag();
		Alt_System::set_table_replacing_measure(&this->qsqldatabase);
	}
	catch(Error msg){
		ostringstream info;
		info << "Check if the ALT tables are created!"<< endl;
		msg.make_second_info(info.str());
		msg.set_fatal_flag(false);
		msg.output_msg(6);
		this->output_error_statistic();
	}
	this->set_exception_number_end();
	cout << "Check of the ALT database tables is finished" << endl ;
	Sys_Common_Output::output_alt->output_txt(&cout,false, false);
}
//Delete the data of all alt database tables
void Alt_System::delete_data_alt_database_tables(void){
	try{
		Alt_System::delete_data_in_table_measure(&this->qsqldatabase);
		Alt_System::delete_data_in_table_replacing_measure(&this->qsqldatabase);
	}
	catch(Error msg){
		msg.output_msg(6);
	}
}
//Close all alt database tables (static)
void Alt_System::close_alt_database_tables(void){
	Alt_System::close_table_measure();
	Alt_System::close_table_replacing_measure();
}
//Set the stop thread flag
void Alt_System::set_stop_thread_flag(const bool flag){
	Alt_System::abort_thread_flag=flag;

	if(flag==true){
		if(this->hyd_system!=NULL ){
			this->hyd_system->set_stop_thread_flag(true);
		}
		//if(this->dam_calc!=NULL){
		//	this->dam_calc->set_stop_thread_flag(true);
		//}
	}
}
//Check the stop thread flag (static)
void Alt_System::check_stop_thread_flag(void){
	if(Alt_System::abort_thread_flag==true){
		Error msg;
		msg.set_msg(_sys_system_modules::ALT_SYS);
		throw msg;
	}
}
//Get the stop thread flag (static)
bool Alt_System::get_stop_thread_flag(void){
	return Alt_System::abort_thread_flag;
}
//Run the thread; the type of thread, which will be performed, has to be set before with set_thread_type( _cost_thread_type type)
void Alt_System::run(void){
/** \see set_thread_type(_alt_thread_type type), _alt_thread_type
*/

	this->thread_is_started=true;
	try{
		switch(this->thread_type){
			case(_alt_thread_type::alt_create_tab):
				this->create_alt_database_tables();
				break;
			case(_alt_thread_type::alt_check_tab):
				this->check_alt_database_tables();
				break;
			case(_alt_thread_type::alt_insert):
				this->insert_new_measure2database();
				break;
			case(_alt_thread_type::alt_switch):
				this->switch_between_measure_state();
				break;
			case(_alt_thread_type::alt_del):
				this->delete_measure_database();
				break;

			default:
				Warning msg=this->set_warning(0);
				msg.output_msg(3);
		}
	}
	catch(Error msg){
		this->thread_is_started=false;
		msg.output_msg(6);
	}
	this->thread_is_started=false;
}
//Get the number of errors
int Alt_System::get_number_error(void){
	return this->number_error.alt_error;
}
//Get number of current available alternatives
int Alt_System::get_current_alternatives(void){
	return this->number_alt;
}
//Get a pointer to a alternative info specified by the given index
_alt_general_info *Alt_System::get_ptr_alt_info(const int index){
	if(index<0 || index>=this->number_alt){
		return NULL;
	}
	else{
		return &this->list_alt[index];
	}
}
//Get a pointer to all alternative info
_alt_general_info *Alt_System::get_ptr_alt_info(void){
	return this->list_alt;
}
//Get a pointer to a alternative info specified by the identifier
_alt_general_info *Alt_System::get_ptr_alt_info_by_id(const int id){
	_alt_general_info *buffer=NULL;
	for(int i=0; i< this->number_alt; i++){
		if(id==this->list_alt[i].id.measure_nr){
			return &this->list_alt[i];
		}
	}
	return buffer;
}
//Convert a string into a measure category (_alt_measure_category) (static)
_alt_measure_category Alt_System::convert_txt2measure_category(const string txt){
	_alt_measure_category type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lower_case(&buffer);

	if(buffer==alt_label::cat_fpl){
		type=_alt_measure_category::category_fpl;
	}
	else if(buffer==alt_label::cat_hyd){
		type=_alt_measure_category::category_hyd;
	}
	else if(buffer==alt_label::cat_dam){
		type=_alt_measure_category::category_dam;
	}
	else{
		Error msg;
		msg.set_msg("_alt_measure_category Alt_System::convert_txt2measure_category(const string txt)", "Can not convert this measure category", "Check the givenmeasure category", 1, false);
		ostringstream info;
		info << "Try to convert measure category: " << txt << endl;
		info << "Possible measure categories: "<< endl;
		info << " "<<alt_label::cat_fpl  << endl;
		info << " "<<alt_label::cat_hyd << endl;
		info << " "<<alt_label::cat_dam  << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return type;
}
//Convert a measure category (_alt_measure_category) into a string (static)
string Alt_System::convert_measure_category2txt(const _alt_measure_category cat){
	string buffer;
	switch(cat){
		case _alt_measure_category::category_fpl:
			buffer=alt_label::cat_fpl;
			break;
		case _alt_measure_category::category_hyd:
			buffer=alt_label::cat_hyd;
			break;
		case _alt_measure_category::category_dam:
			buffer=alt_label::cat_dam;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//Get the flag if the thread has started
bool Alt_System::get_thread_has_started(void){
	return this->thread_is_started;
}
//Get if the measure's implemenation was ok
bool Alt_System::get_measure_implementation_ok(void){
	return this->measure_implemt_ok;
}
//Switch between measure states
void Alt_System::switch_between_measure_state(void){
	this->switch_system2base_state();
	this->switch_system2given_state(this->switch_state_id);
	this->set_systemid(this->switch_state_id);
}
//Get the system state, which should be deleted or switched
_sys_system_id Alt_System::get_system_state_del_swi(void){
	return this->switch_state_id;
}
//_______
//private
//Set new measure data to database table
void Alt_System::set_measure_data2database(QSqlDatabase *ptr_database, _alt_general_info data){
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Alt_System::set_table_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Alt_System::table_measure->maximum_int_of_column(Alt_System::table_measure->get_column_name(label::glob_id),ptr_database)+1;

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Alt_System::table_measure->get_table_name();
	query_string <<" ( ";
	query_string << Alt_System::table_measure->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Alt_System::table_measure->get_column_name(label::areastate_id) <<" , ";
	query_string << Alt_System::table_measure->get_column_name(label::measure_id) <<" , ";

	//data
	query_string << Alt_System::table_measure->get_column_name(alt_label::measure_name) <<" , ";
	query_string << Alt_System::table_measure->get_column_name(alt_label::measure_cat) <<" , ";

	query_string << Alt_System::table_measure->get_column_name(alt_label::measure_sub_cat) <<" , ";
	query_string << Alt_System::table_measure->get_column_name(alt_label::measure_replacing) <<" , ";

	query_string << Alt_System::table_measure->get_column_name(label::description) <<" ) ";

	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << data.id.area_state << " , " ;
	query_string << data.id.measure_nr << " , " ;
	//data
	query_string <<"'" << data.name<< "' , " ;
	query_string <<"'" << Alt_System::convert_measure_category2txt(data.category)<< "' , " ;

	query_string <<"'"<<data.detailed_type <<"'," ;
	query_string << functions::convert_boolean2string(data.replacing) <<"  , " ;

	if(data.measurestate_based_on>0){
		ostringstream buffer;
		buffer <<" (based on measure state " << data.measurestate_based_on<<")";
		data.description.append(buffer.str());
	}

	query_string <<"'"<< data.description<< "' ) " ;

	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if(model.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Alt_System::table_measure->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(6);
	}
}
//Set new measure detailed data to database table
void Alt_System::set_measure_detailed_data2database(QSqlDatabase *ptr_database, const _sys_system_id id, const string table_name, QList<int> list){
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Alt_System::set_table_replacing_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Alt_System::table_replacing->maximum_int_of_column(Alt_System::table_replacing->get_column_name(label::glob_id),ptr_database)+1;

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Alt_System::table_replacing->get_table_name();
	query_string <<" ( ";
	query_string << Alt_System::table_replacing->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Alt_System::table_replacing->get_column_name(label::areastate_id) <<" , ";
	query_string << Alt_System::table_replacing->get_column_name(label::measure_id) <<" , ";

	//data
	query_string << Alt_System::table_replacing->get_column_name(alt_label::replacing_tab_name) <<" , ";

	query_string << Alt_System::table_replacing->get_column_name(alt_label::replacing_id) <<" ) ";

	query_string << " VALUES ";
	for(int i=0; i< list.count(); i++){
		query_string << " ( ";
		query_string << id_glob << " , " ;
		query_string << id.area_state << " , " ;
		query_string << id.measure_nr << " , " ;
		//data
		query_string <<"'" << table_name << "' , " ;
		query_string << list.at(i) <<" ) " ;
		if(i<list.count()-1){
			query_string << " , ";
		}
		id_glob++;
	}

	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if(model.lastError().isValid()){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name                : " << Alt_System::table_replacing->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(6);
	}
}
//Set the detailed information of a measure via database
bool Alt_System::get_detailed_info_measure_database(QSqlDatabase *ptr_database, const _sys_system_id id, string *table_name, QList<int> *list){
	//mysql query with the query-class
	QSqlQueryModel model;
	//the table is set (the name and the column names) and allocated
	try{
		Alt_System::set_table_replacing_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Alt_System::table_replacing->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Alt_System::table_replacing->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND " ;
	test_filter << Alt_System::table_replacing->get_column_name(label::measure_id) << " = "  << id.measure_nr;

	Data_Base::database_request(&model, test_filter.str(), ptr_database);

	//check the request
	if(model.lastError().isValid()){
		Error msg=this->set_error(4);
		ostringstream info;
		info << "Table Name      : " << Alt_System::table_replacing->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(model.rowCount()>0){
		*table_name=model.record(0).value((Alt_System::table_replacing->get_column_name(alt_label::replacing_tab_name)).c_str()).toString().toStdString();

		for(int i=0; i<model.rowCount(); i++){
			list->append(model.record(i).value((Alt_System::table_replacing->get_column_name(alt_label::replacing_id)).c_str()).toInt());
		}

		return true;
	}
	else{
		return false;
	}
}
//Insert new measures into database
void Alt_System::insert_new_measure2database(void){
	this->set_exception_number_start();
	this->measure_implemt_ok=true;
	//begin time recording
	time(&this->start_time);
	ostringstream prefix;
	ostringstream cout;

	cout << "Implement measure "<< this->general_alt_infos.name << " ("<<this->general_alt_infos.id.measure_nr<<")..." << endl ;
	Sys_Common_Output::output_alt->output_txt(&cout);

	try{
		prefix<< "IMPL> ";
		Sys_Common_Output::output_alt->set_userprefix(&prefix);

		if(this->general_alt_infos.category==_alt_measure_category::category_hyd){
			this->implement_hyd_measures();
		}
		else if(this->general_alt_infos.category==_alt_measure_category::category_fpl){
			this->implement_fpl_measures();
		}
		else if(this->general_alt_infos.category==_alt_measure_category::category_dam){
			this->implement_dam_measures();
		}

		Sys_Common_Output::output_alt->set_userprefix(&prefix);
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		Sys_Common_Output::output_alt->set_userprefix(&prefix);
		msg.output_msg(6);
		this->switch_system2base_state();
		this->switch_system2given_state(this->system_id);
		this->switch_state_id=this->new_state_id;
		this->delete_measure_database();
	}

	this->set_exception_number_end();
	//set the actual time
	time(&this->actual_time);
	this->output_implement_error_statistic();

	if(this->measure_implemt_ok==true && this->abort_thread_flag==false && this->number_error.total_error==0){
		this->measure_implemt_ok=true;
		this->system_id=this->new_state_id;
		try{
			this->set_measure_data2database(&this->qsqldatabase, this->general_alt_infos);
		}
		catch(Error msg){
			this->measure_implemt_ok=false;
			Sys_Common_Output::output_alt->set_userprefix(&prefix);
			msg.output_msg(6);
			this->switch_system2base_state();
			this->switch_system2given_state(this->system_id);
			this->switch_state_id=this->new_state_id;
			this->delete_measure_database();
		}
	}
	else{
		this->measure_implemt_ok=false;
	}
}
//Delete ameasure state in database
void Alt_System::delete_measure_database(void){
	this->set_exception_number_start();
	//begin time recording
	time(&this->start_time);
	ostringstream prefix;
	ostringstream cout;

	cout << "Delete measure no. "<< this->switch_state_id.measure_nr <<" in database tables..." << endl ;
	Sys_Common_Output::output_alt->output_txt(&cout);

	try{
		prefix<< "DEL> ";
		Sys_Common_Output::output_alt->set_userprefix(&prefix);
		this->switch_state_id;

		//Cost System
		cout << "Delete measure no. "<< this->switch_state_id.measure_nr <<" in COST-tables..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Cost_Ecn_Cost_System::delete_data_in_table_cost(&this->qsqldatabase, this->switch_state_id);

		//Risk System
		cout << "Delete measure no. "<< this->switch_state_id.measure_nr <<" in RISK-tables..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Risk_System::delete_data_in_resulttable_detailed(&this->qsqldatabase, this->switch_state_id);
		Risk_System::delete_data_in_resulttable_cumulated(&this->qsqldatabase, this->switch_state_id);
		Risk_System::delete_data_in_resulttable(&this->qsqldatabase, this->switch_state_id);
		Risk_System::delete_data_in_resulttable_elem_ecn(&this->qsqldatabase, this->switch_state_id);
		Risk_System::delete_data_in_resulttable_elem_eco_btype(&this->qsqldatabase, this->switch_state_id);
		Risk_System::delete_data_in_resulttable_elem_eco_soil(&this->qsqldatabase, this->switch_state_id);
		Risk_System::delete_data_in_resulttable_elem_pop(&this->qsqldatabase, this->switch_state_id);
		Risk_System::delete_data_in_resulttable_elem_pys(&this->qsqldatabase, this->switch_state_id);
		Risk_System::delete_data_in_resulttable_point_sc(&this->qsqldatabase, this->switch_state_id);
		Risk_Break_Scenario_Management::delete_data_in_table(&this->qsqldatabase, this->switch_state_id);
		Risk_Break_Scenario_Management::delete_data_in_table_fpl_sec_prob(&this->qsqldatabase, this->switch_state_id);
		Risk_Damage_Predictor::delete_data_in_resulttable_predicted_detailed(&this->qsqldatabase, this->switch_state_id);

		//DAM System
		cout << "Delete measure no. "<< this->switch_state_id.measure_nr <<" in DAM-tables..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::delete_data_in_erg_table(&this->qsqldatabase, this->switch_state_id);
		Dam_Ecn_Element::delete_data_in_erg_table(&this->qsqldatabase, this->switch_state_id);
		Dam_Eco_Btype_Element::delete_data_in_erg_table(&this->qsqldatabase, this->switch_state_id);
		Dam_Eco_Soil_Element::delete_data_in_erg_table(&this->qsqldatabase, this->switch_state_id);
		Dam_People_Element::delete_data_in_erg_table(&this->qsqldatabase, this->switch_state_id);
		Dam_Pys_Element::delete_data_in_erg_table(&this->qsqldatabase, this->switch_state_id);
		Dam_Sc_Point::delete_data_in_erg_table(&this->qsqldatabase, this->switch_state_id);
		Dam_CI_Point::delete_data_in_erg_table(&this->qsqldatabase, this->switch_state_id);
		Dam_CI_Point::delete_data_in_instat_erg_table(&this->qsqldatabase, this->switch_state_id);
		Dam_CI_Polygon::delete_data_in_erg_table(&this->qsqldatabase, this->switch_state_id);
		Dam_CI_Polygon::delete_data_in_instat_erg_table(&this->qsqldatabase, this->switch_state_id);

		Dam_Ecn_Element::delete_data_in_elem_table(&this->qsqldatabase, this->switch_state_id);
		Dam_Eco_Btype_Element::delete_data_in_elem_table(&this->qsqldatabase, this->switch_state_id);
		Dam_Eco_Soil_Element::delete_data_in_elem_table(&this->qsqldatabase, this->switch_state_id);
		Dam_People_Element::delete_data_in_elem_table(&this->qsqldatabase, this->switch_state_id);
		Dam_Pys_Element::delete_data_in_elem_table(&this->qsqldatabase, this->switch_state_id);
		Dam_Sc_Point::delete_data_in_point_table(&this->qsqldatabase, this->switch_state_id);

		//FPL System
		cout << "Delete measure no. "<< this->switch_state_id.measure_nr <<" in FPL-tables..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Fpl_Frc_Curve::delete_data_in_table_intervals(&this->qsqldatabase, this->switch_state_id);

		//delete fpl-section
		QSqlQueryModel model;
		int id=0;
		Fpl_Section::select_section_database(&model, &this->qsqldatabase, this->switch_state_id);
		for(int i=0; i<model.rowCount(); i++){
			id=model.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_id)).c_str()).toInt();
			Fpl_Section::delete_data_in_table(&this->qsqldatabase, this->switch_state_id, id);
		}

		//HYD System
		cout << "Delete measure no. "<< this->switch_state_id.measure_nr <<" in HYD-tables..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);

		Hyd_Boundary_Szenario_Management::select_boundary_scenario_database(&model, &this->qsqldatabase, this->switch_state_id);
		for(int i=0; i<model.rowCount(); i++){
			id=model.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
			Hyd_Instationary_Boundary::delete_boundary_curve_by_scenario(&this->qsqldatabase,id);
		}
		Hyd_Boundary_Szenario_Management::delete_scenario(&this->qsqldatabase, this->switch_state_id);
		_Hyd_River_Profile::delete_results_by_system_state(&this->qsqldatabase,this->switch_state_id);
		_Hyd_River_Profile::delete_instat_results_by_system_state(&this->qsqldatabase, this->switch_state_id);
		Hyd_Element_Floodplain::delete_data_in_erg_table(&this->qsqldatabase,this->switch_state_id);
		_Hyd_River_Profile::delete_boundary_cond_by_system_state(&this->qsqldatabase,this->switch_state_id);
		Hyd_Element_Floodplain::delete_boundary_cond_by_system_state(&this->qsqldatabase,this->switch_state_id);
		_Hyd_Coupling_Dikebreak::delete_data_in_result_table(&this->qsqldatabase,this->switch_state_id);
		Hyd_Floodplain_Polysegment::delete_data_in_table(&this->qsqldatabase,this->switch_state_id);
		_Hyd_River_Profile::delete_profile_in_table(&this->qsqldatabase,this->switch_state_id);

		Hyd_Element_Floodplain::delete_data_in_table(&this->qsqldatabase,this->switch_state_id);

		//add profile deleting; structure deleting; dike break deleting; deviation channel deleting

		//Alt_System
		cout << "Delete measure no. "<< this->switch_state_id.measure_nr <<" in ALT-tables..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Alt_System::delete_data_in_table_measure(&this->qsqldatabase, this->switch_state_id);
		Alt_System::delete_data_in_table_replacing_measure(&this->qsqldatabase, this->switch_state_id);

		



	}
	catch(Error msg){
		msg.output_msg(6);
	}

	this->set_exception_number_end();
	//set the actual time
	time(&this->actual_time);

	this->output_error_statistic();
}
//Allocate list of current avalailabe alternatives
void Alt_System::allocate_alt_list(void){
	try{
		this->list_alt=new _alt_general_info[this->number_alt];
		Sys_Memory_Count::self()->add_mem(sizeof(_alt_general_info)*this->number_alt,_sys_system_modules::ALT_SYS);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init
	for(int i=0; i< this->number_alt; i++){
		this->list_alt[i].name=label::not_set;
		this->list_alt[i].id.area_state=-1;
		this->list_alt[i].id.measure_nr=-1;
	}
}
//Delete the list of current available alternatives
void Alt_System::delete_alt_list(void){
	if(this->list_alt!=NULL){
		delete []this->list_alt;
		this->list_alt=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_alt_general_info)*this->number_alt,_sys_system_modules::ALT_SYS);
	}
}
//Set exeptions at start of an action
void Alt_System::set_exception_number_start(void){
	Error::get_number_errors_moduls(&this->number_error);
	Warning::get_number_warning_moduls(&this->number_warning);
}
//Set exception_number at the end of an action
void Alt_System::set_exception_number_end(void){
	Error::get_diff_number_errors_moduls(&this->number_error);
	Warning::get_diff_number_warning_moduls(&this->number_warning);
}
//Output the error statistic of the alt system
void Alt_System::output_error_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_alt->set_userprefix(prefix.str());
	cout << "Error statistics of the ALT-system..." << endl ;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)             :" << W(3) << this->number_error.alt_error <<endl;
	cout << " Warning(s)           :" << W(3) << this->number_warning.alt_warning <<endl;
	if(Alt_System::abort_thread_flag==true){
		cout << " User has aborted the action " << endl;
	}
	Sys_Common_Output::output_alt->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_alt->rewind_userprefix();
}
//Output the error statistic of the alt system for implementing a measure
void Alt_System::output_implement_error_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_alt->set_userprefix(prefix.str());
	cout << "Error statistics of implementing a measure..." << endl ;
	cout << "GENERAL"<< endl;
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

	if(Alt_System::abort_thread_flag==true){
		cout << " User has aborted the calculation " << endl;
	}

	Sys_Common_Output::output_alt->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_alt->rewind_userprefix();
}
//Implement measures of the hydraulic system
void Alt_System::implement_hyd_measures(void){
	ostringstream prefix;
	ostringstream cout;

	cout << "Implement measure of the HYD-system..." << endl ;
	Sys_Common_Output::output_alt->output_txt(&cout);

	try{
		prefix<< "HYD> ";
		Sys_Common_Output::output_alt->set_userprefix(&prefix);
		if(this->system_id.measure_nr!=0){
			this->switch_system2base_state();
		}
		Sys_Common_Output::output_alt->reset_prefix_was_outputed();
		if(this->general_alt_infos.detailed_type==alt_replace_hyd::boundary_sc){
			cout << "Replace "<< this->file_list.count()<< " by new HYD-boundary scenario(s)..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			this->implement_new_hyd_boundary_scenarios();
		}
		else if(this->general_alt_infos.detailed_type==alt_replace_hyd::dike_lines){
			cout << "Replace "<< this->file_list.count()<< " by new HYD-2D-dikeline(s)..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			this->implement_hyd_dikelines(true);
		}
		else if(this->general_alt_infos.detailed_type==alt_add_hyd::dike_lines){
			cout << "Add new HYD-2D-dikeline(s) from "<< this->file_list.count()<< " file(s)..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			this->implement_hyd_dikelines(false);
		}
		else if(this->general_alt_infos.detailed_type==alt_replace_hyd::river_profiles){
			cout << "Replace by new HYD-river propfile(s)..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			this->change_hyd_river_prof();
		}

		//implement here further hyd-measures...

		Sys_Common_Output::output_alt->rewind_userprefix();
	}
	catch(Error msg){
		Sys_Common_Output::output_alt->rewind_userprefix();
		throw msg;
	}
}
//Allocate the hydraulic system class
void Alt_System::allocate_hyd_system(void){
	this->delete_hyd_system();
	try{
		this->hyd_system=new Hyd_Multiple_Hydraulic_Systems;

		this->hyd_system->set_systemid(this->new_state_id);
		this->hyd_system->set_ptr2database(&this->qsqldatabase);

		ostringstream prefix;
		prefix << "ALT> ";
		Sys_Common_Output::output_hyd->set_userprefix(&prefix);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the hydraulic system class
void Alt_System::delete_hyd_system(void){
	if(this->hyd_system!=NULL){
		delete this->hyd_system;
		this->hyd_system=NULL;
		Sys_Common_Output::output_hyd->reset_userprefix();
	}
}
//Implement measures of the fpl system
void Alt_System::implement_fpl_measures(void){
	ostringstream prefix;
	ostringstream cout;

	cout << "Implement measure of the FPL-system..." << endl ;
	Sys_Common_Output::output_alt->output_txt(&cout);

	try{
		prefix<< "FPL> ";
		Sys_Common_Output::output_alt->set_userprefix(&prefix);
		if(this->general_alt_infos.detailed_type==alt_combi_fpl::fpl_line){
			cout << "Add/replace "<< this->file_list.count()<< " FPL-section(s)..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			this->implement_fpl_section();
		}
		else if(this->general_alt_infos.detailed_type==alt_add_fpl::section){
			cout << "Add "<< this->file_list.count()<< " new FPL-section(s)..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			this->implement_fpl_section();
		}
		else if(this->general_alt_infos.detailed_type==alt_replace_fpl::section){
			cout << "Replace "<< this->file_list.count()<< " FPL-section(s)..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			this->implement_fpl_section();
		}
		else if(this->general_alt_infos.detailed_type==alt_replace_fpl::section_param){
			cout << "Replace the parameter of "<< this->file_list.count()<< " FPL-section(s)..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			this->implement_replace_fpl_section_parameter();
		}

		if(this->measure_implemt_ok==true){
			this->delete_fpl_system();
			this->allocate_fpl_system();
			this->fpl_system->set_thread_type(_fpl_thread_type::fpl_combine_fpl2hyd);
			cout << "Start combining of the FPL-section(s) to the HYD-system. Further information see FPL display..."<<endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Sys_Common_Output::output_fpl->set_focus_on_display();
			emit send_fpl_thread_runs(true);
			this->fpl_system->run();
			emit send_fpl_thread_runs(false);
			Alt_System::check_stop_thread_flag();
		}

		//implement here further fpl-measures...
		Sys_Common_Output::output_alt->rewind_userprefix();
	}
	catch(Error msg){
		Sys_Common_Output::output_alt->rewind_userprefix();
		throw msg;
	}
}
//Allocate the fpl system class
void Alt_System::allocate_fpl_system(void){
	this->delete_fpl_system();
	try{
		this->fpl_system=new Fpl_Calculation;
		this->fpl_system->set_systemid(this->new_state_id);
		this->fpl_system->set_ptr2database(&this->qsqldatabase);

		ostringstream prefix;
		prefix << "ALT> ";
		Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(5);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the fpl system class
void Alt_System::delete_fpl_system(void){
	if(this->fpl_system!=NULL){
		delete this->fpl_system;
		this->fpl_system=NULL;
		Sys_Common_Output::output_fpl->reset_userprefix();
	}
}
//Implement measures of the damage system
void Alt_System::implement_dam_measures(void){
	ostringstream prefix;
	ostringstream cout;

	cout << "Implement measure of the DAM-system..." << endl ;
	Sys_Common_Output::output_alt->output_txt(&cout);

	try{
		prefix<< "DAM> ";
		Sys_Common_Output::output_alt->set_userprefix(&prefix);

		if(this->general_alt_infos.detailed_type==alt_replace_dam::dam_resettlement){
			cout << "Resettlement the values/people in given polygons..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			if(this->file_list.count()==0 || this->file_list.at(0).isEmpty()==true){
				Error msg=this->set_error(8);
				throw msg;
			}
			this->implement_dam_resettlement();
		}
		else if(this->general_alt_infos.detailed_type==alt_replace_dam::dam_ecn_function_polygon){
			cout << "Switch ECN-damage function(s) in given polygon(s)" << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			if(this->file_list.count()==0 || this->file_list.at(0).isEmpty()==true){
				Error msg=this->set_error(8);
				throw msg;
			}
			this->switch_ecn_dam_functions();
		}
		else if(this->general_alt_infos.detailed_type==alt_replace_dam::dam_eco_btype_function_polygon){
			cout << "Switch ECO-biotope type(s) in given polygon(s)" << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			if(this->file_list.count()==0 || this->file_list.at(0).isEmpty()==true){
				Error msg=this->set_error(8);
				throw msg;
			}
			this->switch_eco_dam_biotope_type();
		}
		else if(this->general_alt_infos.detailed_type==alt_replace_dam::dam_eco_soil_function_polygon){
			cout << "Switch ECO-soil type(s) in given polygon(s)" << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			if(this->file_list.count()==0 || this->file_list.at(0).isEmpty()==true){
				Error msg=this->set_error(8);
				throw msg;
			}
			this->switch_eco_dam_soil_type();
		}
		else if(this->general_alt_infos.detailed_type==alt_replace_dam::dam_pop_categories_polygon){
			cout << "Switch POP categories in given polygon(s)" << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			if(this->file_list.count()==0 || this->file_list.at(0).isEmpty()==true){
				Error msg=this->set_error(8);
				throw msg;
			}
			this->switch_dam_people2risk_category();
		}
		else if(this->general_alt_infos.detailed_type==alt_replace_dam::dam_pys_categories_polygon){
			cout << "Switch PYS categories in given polygon(s)" << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			if(this->file_list.count()==0 || this->file_list.at(0).isEmpty()==true){
				Error msg=this->set_error(8);
				throw msg;
			}
			this->switch_dam_psychosocial_category();
		}

		//implement here further dam-measures...

		Sys_Common_Output::output_alt->rewind_userprefix();
	}
	catch(Error msg){
		Sys_Common_Output::output_alt->rewind_userprefix();
		throw msg;
	}
}
//Allocate the dam system class
void Alt_System::allocate_dam_system(void){
	this->delete_dam_system();
	try{
		this->dam_system=new Dam_Damage_System;
		this->dam_system->set_systemid(this->new_state_id);
		this->dam_system->set_ptr2database(&this->qsqldatabase);

		ostringstream prefix;
		prefix << "ALT> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(7);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the dam system class
void Alt_System::delete_dam_system(void){
	if(this->dam_system!=NULL){
		delete this->dam_system;
		this->dam_system=NULL;
		Sys_Common_Output::output_dam->reset_userprefix();
	}
}
//Implement (replacing) new hydraulic boundary scenarios
void Alt_System::implement_new_hyd_boundary_scenarios(void){
	ostringstream cout;

	_sys_system_id buff_base;
	buff_base=this->new_state_id;
	buff_base.measure_nr=0;
	Hyd_Boundary_Szenario_Management buffer;
	buffer.set_systemid(buff_base);

	buffer.set_existing_szenario_per_db(&this->qsqldatabase);

	//output
	cout << "List of replacing HYD-boundary scenario(s)"<<endl;
	for(int i=0; i< this->file_list.count();i++){
		cout << " "<<buffer.get_ptr_sz_id_given(this->id_list.at(i))->get_sz_text()<<" with ";
		cout << this->file_list.at(i).toStdString() << " to " << buffer.get_ptr_sz_id_given(this->id_list.at(i))->get_name();
		cout <<"_A"<<this->new_state_id.area_state<<"M"<< this->new_state_id.measure_nr <<endl;
	}
	Sys_Common_Output::output_alt->output_txt(&cout);

	Alt_System::check_stop_thread_flag();

	Hyd_Boundary_Scenario_List buffer_list;
	Hyd_Boundary_Szenario buff_sc;

	//import via hyd-system
	ostringstream buff_stream;

	//make a list of boundary scenarios
	for(int i=0; i< this->id_list.count(); i++){
		buff_sc=*buffer.get_ptr_sz_id_given(this->id_list.at(i));
		buff_stream<<buff_sc.get_name()<<"_A"<<this->new_state_id.area_state<<"M"<< this->new_state_id.measure_nr;
		buff_sc.set_name(buff_stream.str());
		buff_sc.set_description(this->general_alt_infos.name);
		buff_sc.set_systemid(this->new_state_id);
		buffer_list.add_scenario2list(&buff_sc);
		buff_stream.str("");
	}

	try{
		Alt_System::check_stop_thread_flag();
		this->hyd_system->set_system_number_file(&this->file_list);
		this->hyd_system->sz_bound_manager.set_new_boundary_scenario_directly(&buffer_list);
		this->hyd_system->set_thread_type(_hyd_thread_type::hyd_add_sz);
		cout << "Start import of the HYD-boundary scenario(s). Further information see HYD display..."<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Sys_Common_Output::output_hyd->set_focus_on_display();
		emit send_hyd_thread_runs(true);
		this->hyd_system->run();
		emit send_hyd_thread_runs(false);
		Alt_System::check_stop_thread_flag();
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		emit send_hyd_thread_runs(false);
		Sys_Common_Output::output_alt->set_focus_on_display();
		throw msg;
	}

	Sys_Common_Output::output_alt->set_focus_on_display();
	cout << "Import of the HYD-boundary scenario(s) is finished!"<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);

	//check if operation was successful
	if(Hyd_Multiple_Hydraulic_Systems::get_stop_thread_flag()==true){
		this->measure_implemt_ok=false;
	}
	if(this->hyd_system->get_error_number()>0){
		this->measure_implemt_ok=false;
	}

	//change applied flags
	if(this->measure_implemt_ok==true){
		//switch system to base state
		this->switch_system2base_state();

		cout << "Switch HYD- and DAM-results of the base system state to applied 'false' for the new HYD-boundary scenario(s)..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		for(int i=0; i< this->id_list.count(); i++){
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, buff_base , false, this->id_list.at(i));
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, buff_base, this->id_list.at(i), false);
		}
		cout << "Switch the HYD-boundary scenario(s) to the new system state..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		for(int i=0; i< this->id_list.count(); i++){
			Hyd_Boundary_Szenario_Management::switch_applied_flag_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
		}
		cout << "Write detailed measure information to the database..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);

		this->set_measure_detailed_data2database(&this->qsqldatabase, this->general_alt_infos.id, hyd_label::tab_sz_bound , this->id_list);
	}
	//delete already set data in database
	else{
		cout << "Implementation of the "<< this->id_list.count()<<" new HYD-boundary scenario(s) as measure ";
		cout << this->general_alt_infos.name<<" was not successful. The data are deleted!" << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
	
		//delete the istant bound curves
		QSqlQueryModel model;
		int id=0;
		Hyd_Boundary_Szenario_Management::select_boundary_scenario_database(&model, &this->qsqldatabase, this->general_alt_infos.id);
		for(int i=0; i<model.rowCount(); i++){
			id=model.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
			Hyd_Instationary_Boundary::delete_boundary_curve_by_scenario(&this->qsqldatabase,id);
		}
		//delete the rest
		Hyd_Boundary_Szenario_Management::delete_scenario(&this->qsqldatabase, this->general_alt_infos.id);
		_Hyd_River_Profile::delete_results_by_system_state(&this->qsqldatabase,this->general_alt_infos.id);
		_Hyd_River_Profile::delete_instat_results_by_system_state(&this->qsqldatabase, this->general_alt_infos.id);
		Hyd_Element_Floodplain::delete_data_in_erg_table(&this->qsqldatabase,this->general_alt_infos.id);
		_Hyd_River_Profile::delete_boundary_cond_by_system_state(&this->qsqldatabase,this->general_alt_infos.id);
		Hyd_Element_Floodplain::delete_boundary_cond_by_system_state(&this->qsqldatabase,this->general_alt_infos.id);


	}
}
//Implement hyd-dikelines
void Alt_System::implement_hyd_dikelines(const bool replace){
	ostringstream cout;

	_sys_system_id buff_base;
	buff_base=this->new_state_id;
	buff_base.measure_nr=0;

	//output
	cout << "List of new HYD-dikeline file(s)"<<endl;
	for(int i=0; i< this->file_list.count();i++){
		if(this->id_list.at(i)<0){
			cout <<" " << this->file_list.at(i).toStdString()<<endl;
		}
	}

	Sys_Common_Output::output_alt->output_txt(&cout);
	if(replace==true){
		cout << "List of replacing HYD-dikeline(s) [Id]"<<endl;
		for(int i=0; i< this->file_list.count();i++){
			if(this->id_list.at(i)>=0){
				cout <<" " << this->id_list.at(i) << " with " << this->file_list.at(i).toStdString()<<endl;
			}
		}
		Sys_Common_Output::output_alt->output_txt(&cout);
	}

	Hyd_Floodplain_Polysegment *segment_buffer=NULL;
	try{
		try{
			segment_buffer = new Hyd_Floodplain_Polysegment[this->file_list.count()];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(6);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		cout << "Start import of the HYD-dikeline(s) from file(s)..."<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
		//read the dikelines in from file
		for(int i=0; i< this->file_list.count();i++){
			segment_buffer[i].set_systemid(this->new_state_id);
			segment_buffer[i].input_members(0, this->file_list.at(i).toStdString());
		}
		//intersect dikelines with the floodplains
		QSqlTableModel results(0, this->qsqldatabase);
		int number_fp=0;
		int index_fp=0;
		number_fp=Hyd_Model_Floodplain::select_relevant_model_database(&results, false);
		Hyd_Floodplain_Raster raster_buffer;

		Alt_System::check_stop_thread_flag();
		//set the dikelines to the database
		for(int i=0; i< this->file_list.count();i++){
			for(int j=0; j<number_fp; j++){
				//set the raster
				index_fp=results.record(j).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::genmod_id)).c_str()).toInt();
				raster_buffer.set_geometric_info(&results, j);
				//chek for interception
				if(raster_buffer.geometrical_bound.check_polysegment_partly_inside(&segment_buffer[i])==true){
					segment_buffer[i].transfer_polysegment2database_table(&this->qsqldatabase, index_fp ,false);
				}
			}
		}
		Alt_System::check_stop_thread_flag();
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		msg.output_msg(6);
	}

	cout << "Import of the HYD-dikeline(s) is finished!"<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);

	if(segment_buffer!=NULL){
		delete []segment_buffer;
		segment_buffer=NULL;
	}

	//change applied flags
	if(this->measure_implemt_ok==true){
		//switch system to base state
		this->switch_system2base_state();

		cout << "Switch all HYD-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, buff_base , false);
		cout << "Switch all DAM-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, buff_base, false);

		cout << "Switch the HYD-dikeline(s) to the new system state..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		for(int i=0; i< this->id_list.count(); i++){
			if(this->id_list.at(i)>=0){
				//dikelines
				Hyd_Floodplain_Polysegment::switch_applied_flag_polysegment_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
			}
		}

		cout << "Write detailed measure information to the database..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		this->set_measure_detailed_data2database(&this->qsqldatabase, this->general_alt_infos.id, hyd_label::tab_fp_polyseg , this->id_list);
	}
	else{
		cout << "Implementation of the "<< this->id_list.count()<<" new/replacing HYD-dikeline(s) as measure ";
		cout << this->general_alt_infos.name<<" was not successful. The data are deleted!" << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Hyd_Floodplain_Polysegment::delete_data_in_table(&this->qsqldatabase, this->new_state_id);
	}
}
//Change hyd-river profile(s)
void Alt_System::change_hyd_river_prof(void){
	ostringstream cout;

	_sys_system_id buff_base;
	buff_base=this->new_state_id;
	buff_base.measure_nr=0;

	//output
		
	cout << "List of replacing HYD-river profiles(s) in river model [Id]"<<endl;
	for(int i=0; i< this->file_list.count();i++){
		if(this->id_list.at(i)>=0){
			cout <<" HYD-river id " << this->id_list.at(i) << " with " << this->file_list.at(i).toStdString()<<endl;
		}
	}
	Sys_Common_Output::output_alt->output_txt(&cout);
	

	cout <<"Input the existing HYD-river profile(s) per database..."<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);
	Hyd_Hydraulic_System source_sys;
	source_sys.set_systemid(buff_base);

	

	QSqlTableModel model;
	try{
		source_sys.input_just_river_models(&this->qsqldatabase, false);
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		throw msg;
	}



	cout <<"Read in new HYD-river profile(s) from " <<this->file_list.count()<<" file..."<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);
	Hyd_Model_River *ptr_src_rv=NULL;
	Hyd_Model_River *ptr_targ_rv=NULL;


	//allocate the new systems
	Hyd_Hydraulic_System *target_sys;
	target_sys=NULL;

	try{
		target_sys=new Hyd_Hydraulic_System[this->file_list.count()];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(10);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//read in
	cout << "Start import of the replacing HYD-river(s). Further information see HYD display..."<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);
	ostringstream prefix;
	prefix<< "ALT> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	
	Sys_Common_Output::output_hyd->set_focus_on_display();
	try{
		for(int i=0; i< this->file_list.count(); i++){
			Alt_System::check_stop_thread_flag();
			ptr_src_rv=source_sys.get_ptr_river_model(this->id_list.at(i));
			if(ptr_src_rv!=NULL){
				target_sys[i].set_systemid(this->new_state_id);
				Hyd_Param_RV target_param=ptr_src_rv->Param_RV;
				target_param.set_geometry_file(this->file_list.at(i).toStdString());
				ostringstream text;
				text<<target_param.get_river_name()<<"_M"<<this->new_state_id.measure_nr;
				target_param.set_river_name(text.str());
				//read in the new river
				target_sys[i].set_river_model_per_file(target_param);
				target_sys[i].clear_boundary_conditions();
				target_sys[i].connect_rivers(&source_sys.material_params);
				target_sys[i].set_rv_inflow_flag();

			}
			else{
				//Error
				Error msg=this->set_error(9);
				throw msg;
			}
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		Sys_Common_Output::output_alt->set_focus_on_display();
		this->measure_implemt_ok=false;

		if(target_sys!=NULL){
			delete []target_sys;
			target_sys=NULL;
		}

		throw msg;
	}

	Sys_Common_Output::output_hyd->rewind_userprefix();
	Sys_Common_Output::output_alt->set_focus_on_display();
	cout << "Import of the HYD-river(s) is finished!"<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);
	Sys_Common_Output::output_alt->reset_prefix_was_outputed();
	//check
	cout << "Check the replacing HYD-river(s)..."<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);
	this->allocate_fpl_system();
	this->fpl_system->set_systemid(buff_base);

	try{
		for(int i=0; i< this->file_list.count(); i++){
			Alt_System::check_stop_thread_flag();
			ptr_src_rv=source_sys.get_ptr_river_model(this->id_list.at(i));
			if(ptr_src_rv!=NULL){
				
				ptr_targ_rv=target_sys[i].get_ptr_river_model(this->id_list.at(i));
				
				//check and transfer it
				if(ptr_targ_rv!=NULL){
					cout << "Check the replacing HYD-river model " << ptr_targ_rv->Param_RV.get_river_name()<<"..."<<endl;
					Sys_Common_Output::output_alt->output_txt(&cout);
					//check river model
					ptr_targ_rv->check_river_model(false);
					Alt_System::check_stop_thread_flag();
					//check idealisation
					prefix<< "ALT> ";
					Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
					Sys_Common_Output::output_fpl->set_focus_on_display();
					try{
						this->fpl_system->check_ideal_hyd_system_fpl(ptr_targ_rv);
					}
					catch(Error msg){
						Sys_Common_Output::output_fpl->rewind_userprefix();
						throw msg;
					}
					Sys_Common_Output::output_alt->set_focus_on_display();
					Sys_Common_Output::output_fpl->rewind_userprefix();
					Alt_System::check_stop_thread_flag();
					//check river system
					Hyd_Model_River *check;
					
					//the new one each other
					for(int j=0; j< this->file_list.count(); j++){
						check=NULL;
						if(j!=i){
							check=target_sys[j].get_ptr_river_model(this->id_list.at(j));
							if(check!=NULL){
								ptr_targ_rv->check_river_modelsby_interception(check);
							}
						}
					}

					bool exists=false;
					for(int j=0; j< source_sys.global_parameters.get_number_river_model(); j++){
						exists=false;
						if(source_sys.my_rvmodels[j].Param_RV.get_river_number()!=ptr_targ_rv->Param_RV.get_river_number()){
							for(int k=0; k<this->file_list.count(); k++){
								if(source_sys.my_rvmodels[j].Param_RV.get_river_number()==this->id_list.at(k)){
									exists=true;
								}
							}
							if(exists==false){
								check=&(source_sys.my_rvmodels[j]);
								if(check!=NULL){
									ptr_targ_rv->check_river_modelsby_interception(check);
								}
							}
						}

					}
				}
				else{
					//Error
					Error msg=this->set_error(9);
					throw msg;
				}
			}
			else{
				//Error
				Error msg=this->set_error(9);
				throw msg;
			}
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_alt->set_focus_on_display();
		this->measure_implemt_ok=false;

		if(target_sys!=NULL){
			delete []target_sys;
			target_sys=NULL;
		}

		throw msg;
	}

	Sys_Common_Output::output_alt->set_focus_on_display();
	cout << "Check of the replacing HYD-river(s) is finished!"<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);
	Sys_Common_Output::output_alt->reset_prefix_was_outputed();
	//transfer
	cout << "Transfer the replacing HYD-river(s) to database..."<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);
	try{
		for(int i=0; i< this->file_list.count(); i++){
			Alt_System::check_stop_thread_flag();
			ptr_src_rv=source_sys.get_ptr_river_model(this->id_list.at(i));
			if(ptr_src_rv!=NULL){
				
				ptr_targ_rv=target_sys[i].get_ptr_river_model(this->id_list.at(i));
				
				//check and transfer it
				if(ptr_targ_rv!=NULL){
					ptr_targ_rv->transfer_profile_members2database(&this->qsqldatabase, ptr_src_rv, buff_base);
					Alt_System::check_stop_thread_flag();
				}
				else{
					//Error
					Error msg=this->set_error(9);
					throw msg;
				}
			}
			else{
				//Error
				Error msg=this->set_error(9);
				throw msg;
			}
		}
		Sys_Common_Output::output_alt->set_focus_on_display();
		cout << "Transfer of the replacing HYD-river(s) to database is finished!"<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
	}
	catch(Error msg){
		Sys_Common_Output::output_alt->set_focus_on_display();
		this->measure_implemt_ok=false;

		if(target_sys!=NULL){
			delete []target_sys;
			target_sys=NULL;
		}

		msg.output_msg(6);
	}


	if(target_sys!=NULL){
		delete []target_sys;
		target_sys=NULL;
	}

	//change applied flags
	if(this->measure_implemt_ok==true){
		//switch system to base state
		this->switch_system2base_state();
	
		cout << "Switch all HYD-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, buff_base , false);
		cout << "Switch all DAM-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, buff_base, false);

		cout << "Switch the HYD-river profiles(s) to the new system state..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		for(int i=0; i< this->id_list.count(); i++){
			if(this->id_list.at(i)>=0){
				//profiles
				_Hyd_River_Profile::switch_applied_flag_profile_table(&this->qsqldatabase, buff_base, this->id_list.at(i), false);
			}
		}
	
		cout << "Write detailed measure information to the database..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		this->set_measure_detailed_data2database(&this->qsqldatabase, this->general_alt_infos.id, hyd_label::tab_rvprof , this->id_list);
	}
	else{
		cout << "Implementation of the "<< this->id_list.count()<<" river model(s), where HYD-river profiles(s) should be replaced as measure ";
		cout << this->general_alt_infos.name<<" was not successful. The data are deleted!" << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);

		_Hyd_River_Profile::delete_profile_in_table(&this->qsqldatabase, this->new_state_id);
	}
}
//Implement (replacind/adding) fpl-section
void Alt_System::implement_fpl_section(void){
	ostringstream cout;

	_sys_system_id buff_base;
	buff_base=this->system_id;
	buff_base.measure_nr=0;

	QSqlQueryModel model;
	Fpl_Section section_buffer;

	//output
	cout << "List of new FPL-section file(s)"<<endl;
	for(int i=0; i< this->file_list.count();i++){
		if(this->id_list.at(i)<0){
			cout <<" " << this->file_list.at(i).toStdString()<<endl;
		}
	}
	Sys_Common_Output::output_alt->output_txt(&cout);
	cout << "List of replacing FPL-section(s)"<<endl;
	for(int i=0; i< this->file_list.count();i++){
		if(this->id_list.at(i)>=0){
			Fpl_Section::select_section_database(&model, &this->qsqldatabase, buff_base, this->id_list.at(i));
			section_buffer.input_section_perdatabase(&model, 0, &this->qsqldatabase, false);
			cout <<" " << section_buffer.get_name_section() << " ("<<this->id_list.at(i)<<")"  << " with " << this->file_list.at(i).toStdString()<<endl;
		}
	}
	Sys_Common_Output::output_alt->output_txt(&cout);

	try{
		Alt_System::check_stop_thread_flag();
		this->allocate_fpl_system();
		this->fpl_system->set_system_number_file_direct(this->file_list);
		this->fpl_system->set_thread_type(_fpl_thread_type::fpl_import_file);
		cout << "Start import of the FPL-section(s). Further information see FPL display..."<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Sys_Common_Output::output_fpl->set_focus_on_display();
		emit send_fpl_thread_runs(true);
		this->fpl_system->run();
		emit send_fpl_thread_runs(false);
		Alt_System::check_stop_thread_flag();
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		emit send_fpl_thread_runs(false);
		Sys_Common_Output::output_alt->set_focus_on_display();
		msg.output_msg(6);
	}

	Sys_Common_Output::output_alt->set_focus_on_display();
	cout << "Import of the FPL-section(s) is finished!"<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);

	//check if operation was successful
	if(Fpl_Calculation::get_stop_thread_flag()==true){
		this->measure_implemt_ok=false;
	}
	if(this->fpl_system->get_number_error()>0){
		this->measure_implemt_ok=false;
	}

	//change applied flags
	if(this->measure_implemt_ok==true){
		//switch system to base state
		this->switch_system2base_state();

		cout << "Switch all HYD-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, buff_base , false);
		cout << "Switch all DAM-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, buff_base, false);

		cout << "Switch the FPL-section(s) and their results to the new system state..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		for(int i=0; i< this->id_list.count(); i++){
			if(this->id_list.at(i)>=0){
				//section
				Fpl_Section::switch_applied_flag_section_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
				Fpl_Random_Variables::switch_applied_flag_variable_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
				Fpl_Section_Points::switch_applied_flag_point_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
				Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
				//results
				Fpl_Mc_Sim::switch_applied_flag_result_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
			}
		}

		cout << "Write detailed measure information to the database..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		this->set_measure_detailed_data2database(&this->qsqldatabase, this->general_alt_infos.id, fpl_label::tab_fpl_section , this->id_list);
	}
	else{
		cout << "Implementation of the "<< this->id_list.count()<<" new/replacing FPL-section(s) as measure ";
		cout << this->general_alt_infos.name<<" was not successful. The data are deleted!" << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		emit send_fpl_thread_runs(true);
		Fpl_Calculation my_deleter;
		my_deleter.set_ptr2database(&this->qsqldatabase);
		my_deleter.set_systemid(this->general_alt_infos.id);

		my_deleter.set_list_section_ids_current();
		my_deleter.set_thread_type(_fpl_thread_type::fpl_del_section);
		my_deleter.run();
		emit send_fpl_thread_runs(false);
	}
}
//Implement replacing parameters of one/multiple FPL-section(s)
void Alt_System::implement_replace_fpl_section_parameter(void){
	ostringstream cout;

	_sys_system_id buff_base;
	buff_base=this->system_id;
	buff_base.measure_nr=0;

	QSqlQueryModel model;
	Fpl_Section section_buffer;

	bool applied_important=false;
	if(this->general_alt_infos.measurestate_based_on==0){
		applied_important=false;
	}
	else{
		applied_important=true;
	}

	//output
	try{
		cout << "List of replacing FPL-section(s) for parameter's replacing"<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
		for(int i=0; i< this->file_list.count();i++){
			if(this->id_list.at(i)>=0){
				if(this->general_alt_infos.measurestate_based_on>0){
					Fpl_Section::select_relevant_section_database(&model, &this->qsqldatabase, this->system_id, this->id_list.at(i), false);
					section_buffer.set_systemid(this->system_id);
				}
				else{
					Fpl_Section::select_section_database(&model, &this->qsqldatabase, buff_base, this->id_list.at(i));
					section_buffer.set_systemid(buff_base);
				}
				if(this->general_alt_infos.measurestate_based_on>0){
					section_buffer.input_section_perdatabase(&model, 0, &this->qsqldatabase);
				}
				else{
					section_buffer.input_section_perdatabase(&model, 0, &this->qsqldatabase, false);
				}
				cout <<" " << section_buffer.get_name_section() << " ("<<this->id_list.at(i)<<")"  << " with " << this->file_list.at(i).toStdString()<<endl;
			}
		}
		Sys_Common_Output::output_alt->output_txt(&cout);
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		throw msg;
	}

	try{
		Alt_System::check_stop_thread_flag();
		this->allocate_fpl_system();
		cout << "Start import of the FPL-section(s). Further information see FPL display..."<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Sys_Common_Output::output_fpl->set_focus_on_display();
		emit send_fpl_thread_runs(true);
		for(int i=0; i< this->file_list.count(); i++){
			QStringList buff_list;
			buff_list.append(this->file_list.at(i));
			this->fpl_system->import_file2database_prob_params(buff_list, this->id_list.at(i));
		}
		emit send_fpl_thread_runs(false);
		Alt_System::check_stop_thread_flag();
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		emit send_fpl_thread_runs(false);
		Sys_Common_Output::output_alt->set_focus_on_display();
		msg.output_msg(6);
	}

	Sys_Common_Output::output_alt->set_focus_on_display();
	cout << "Import of the FPL-section(s) is finished!"<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);

	//check if operation was successful
	if(Fpl_Calculation::get_stop_thread_flag()==true){
		this->measure_implemt_ok=false;
	}
	if(this->fpl_system->get_number_error()>0){
		this->measure_implemt_ok=false;
	}

	//change applied flags
	if(this->measure_implemt_ok==true){
		//base state is based on measure state
		if(this->general_alt_infos.measurestate_based_on==0){
			//switch system to base state
			this->switch_system2base_state();
			//copy the results from the base measure state
			ostringstream prefix;
			prefix<<"ALT> COPY> ";
			cout << "Copy the HYD-results from the base measure state to the new one..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Sys_Common_Output::output_hyd->set_focus_on_display();
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
			Hyd_Hydraulic_System::copy_results(&this->qsqldatabase, buff_base , this->general_alt_infos.id);
			Sys_Common_Output::output_hyd->reset_userprefix();
			Sys_Common_Output::output_alt->set_focus_on_display();
			cout << "Copy the DAM-results from the base measure state to the new one..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Sys_Common_Output::output_dam->set_focus_on_display();
			Sys_Common_Output::output_dam->set_userprefix(prefix.str());
			Dam_Damage_System::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
			Sys_Common_Output::output_dam->reset_userprefix();
			Sys_Common_Output::output_alt->set_focus_on_display();

			cout << "Copy the FPL-section(s) and their results of base measure state, which are not affected, to the new one..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);

			Fpl_Calculation buffer_sections;
			buffer_sections.set_systemid(buff_base);
			buffer_sections.set_ptr2database(&this->qsqldatabase);
			buffer_sections.set_list_section_ids_current();
			for(int i=0; i< buffer_sections.get_number_section_in_list(); i++){
				bool not_in_list=true;
				for(int j=0; j< this->id_list.count(); j++){
					if(this->id_list.at(j)==buffer_sections.list_section_id[i]){
						not_in_list=false;
						break;
					}
				}
				if(not_in_list==true){
					cout << "Copy the FPL-section "<<buffer_sections.list_section_id[i]<<" and their result from the base measure state to the new one..." << endl ;
					Sys_Common_Output::output_alt->output_txt(&cout);
					Fpl_Section::copy_fpl_section(&this->qsqldatabase, buff_base,this->general_alt_infos.id, buffer_sections.list_section_id[i]);
					Fpl_Random_Variables::copy_fpl_random_variables(&this->qsqldatabase, buff_base,this->general_alt_infos.id, buffer_sections.list_section_id[i]);
					Fpl_Section_Points::copy_fpl_geo_points(&this->qsqldatabase, buff_base,this->general_alt_infos.id, buffer_sections.list_section_id[i]);
					Fpl_Dike_Geo_Materialzone::copy_fpl_matzone(&this->qsqldatabase, buff_base,this->general_alt_infos.id, buffer_sections.list_section_id[i]);
					//results
					Fpl_Mc_Sim::copy_results(&this->qsqldatabase, buff_base,this->general_alt_infos.id, buffer_sections.list_section_id[i]);
				}
			}
			cout << "Switch the FPL-section(s) and their results to the new measure state..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);

			for(int i=0; i< this->id_list.count(); i++){
				if(this->id_list.at(i)>=0){
					Fpl_Section::switch_applied_flag_section_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
					Fpl_Random_Variables::switch_applied_flag_variable_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
					Fpl_Section_Points::switch_applied_flag_point_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
					Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
					//results
					Fpl_Mc_Sim::switch_applied_flag_result_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
				}
			}
			cout << "Write detailed measure information to the database..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			this->set_measure_detailed_data2database(&this->qsqldatabase, this->general_alt_infos.id, fpl_label::tab_fpl_section , this->id_list);
		}
		//current state is based on state
		else{
			//copy the results from the current measure state
			ostringstream prefix;
			prefix<<"ALT> COPY> ";
			_alt_general_info current_info;
			this->check_measure_is_set_database(&this->qsqldatabase, this->system_id, &current_info);
			cout << "Copy the HYD-results from the current measure state to the new one..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Sys_Common_Output::output_hyd->set_focus_on_display();
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
			Hyd_Hydraulic_System::copy_results(&this->qsqldatabase, current_info.id , this->general_alt_infos.id);
			Sys_Common_Output::output_hyd->reset_userprefix();
			Sys_Common_Output::output_alt->set_focus_on_display();
			cout << "Copy the DAM-results from the current measure state to the new one..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Sys_Common_Output::output_dam->set_focus_on_display();
			Sys_Common_Output::output_dam->set_userprefix(prefix.str());
			Dam_Damage_System::copy_results(&this->qsqldatabase, current_info.id, this->general_alt_infos.id);
			Sys_Common_Output::output_dam->reset_userprefix();
			Sys_Common_Output::output_alt->set_focus_on_display();

			cout << "Copy the FPL-section(s) and their results of the current measure state, which are not affected, to the new one..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);

			Fpl_Calculation buffer_sections;
			buffer_sections.set_systemid(current_info.id);
			buffer_sections.set_ptr2database(&this->qsqldatabase);
			buffer_sections.set_list_section_ids_current();
			for(int i=0; i< buffer_sections.get_number_section_in_list(); i++){
				bool not_in_list=true;
				for(int j=0; j< this->id_list.count(); j++){
					if(this->id_list.at(j)==buffer_sections.list_section_id[i]){
						not_in_list=false;
						break;
					}
				}
				if(not_in_list==true){
					cout << "Copy the FPL-section "<<buffer_sections.list_section_id[i]<<" and their result from the current measure state to the new one..." << endl ;
					Sys_Common_Output::output_alt->output_txt(&cout);
					Fpl_Section::copy_fpl_section(&this->qsqldatabase, current_info.id,this->general_alt_infos.id, buffer_sections.list_section_id[i]);
					Fpl_Random_Variables::copy_fpl_random_variables(&this->qsqldatabase, current_info.id,this->general_alt_infos.id, buffer_sections.list_section_id[i]);
					Fpl_Section_Points::copy_fpl_geo_points(&this->qsqldatabase, current_info.id,this->general_alt_infos.id, buffer_sections.list_section_id[i]);
					Fpl_Dike_Geo_Materialzone::copy_fpl_matzone(&this->qsqldatabase, current_info.id,this->general_alt_infos.id, buffer_sections.list_section_id[i]);
					//results
					Fpl_Mc_Sim::copy_results(&this->qsqldatabase, current_info.id,this->general_alt_infos.id, buffer_sections.list_section_id[i]);
				}
			}

			//switch the section with new infos in the base measure state to false
			for(int i=0; i< this->id_list.count(); i++){
				if(this->id_list.at(i)>=0){
					Fpl_Section::switch_applied_flag_section_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
					Fpl_Random_Variables::switch_applied_flag_variable_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
					Fpl_Section_Points::switch_applied_flag_point_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));

					//results
					Fpl_Mc_Sim::switch_applied_flag_result_table(&this->qsqldatabase, buff_base, false, this->id_list.at(i));
				}
			}
			//switch the section of the current measure state to false
			//section
			Fpl_Section::switch_applied_flag_section_table(&this->qsqldatabase,current_info.id, false);
			Fpl_Random_Variables::switch_applied_flag_variable_table(&this->qsqldatabase, current_info.id, false);
			Fpl_Section_Points::switch_applied_flag_point_table(&this->qsqldatabase, current_info.id, false);
			Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_table(&this->qsqldatabase, current_info.id, false);
			//results
			Fpl_Mc_Sim::switch_applied_flag_result_table(&this->qsqldatabase, current_info.id, false);

			cout << "Write detailed measure information to the database..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			this->set_measure_detailed_data2database(&this->qsqldatabase, this->general_alt_infos.id, fpl_label::tab_fpl_section , this->id_list);
		}
	}
	else{
		cout << "Implementation of the "<< this->id_list.count()<<" new/replacing FPL-section(s) as measure ";
		cout << this->general_alt_infos.name<<" was not successful. The data are deleted!" << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		emit send_fpl_thread_runs(true);
		Fpl_Calculation my_deleter;
		my_deleter.set_ptr2database(&this->qsqldatabase);
		my_deleter.set_systemid(this->general_alt_infos.id);

		my_deleter.set_list_section_ids_current();
		my_deleter.set_thread_type(_fpl_thread_type::fpl_del_section);
		my_deleter.run();
		emit send_fpl_thread_runs(false);
	}
}
//Implement dam resettlement
void Alt_System::implement_dam_resettlement(void){
	ostringstream cout;

	_sys_system_id buff_base;
	buff_base=this->system_id;
	buff_base.measure_nr=0;

	QSqlTableModel model(NULL, this->qsqldatabase);

	//output
	cout << "DAM-polygon file"<<endl;
	for(int i=0; i< this->file_list.count();i++){
		cout <<" " << this->file_list.at(i).toStdString()<<endl;
	}
	Sys_Common_Output::output_alt->output_txt(&cout);

	cout << "New biotope-type id in the resettled area"<<endl;
	if(this->id_list.at(0)>=0){
		Dam_Eco_Btype::select_relevant_biotope_types_database(&model,this->id_list.at(0));
		cout <<" " << model.record(0).value((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_name)).c_str()).toString().toStdString() << " ("<<this->id_list.at(0)<<")" <<endl;
	}
	else{
		cout <<" Id biotope-type ("<<this->id_list.at(0)<<")" <<endl;
	}
	Sys_Common_Output::output_alt->output_txt(&cout);

	try{
		Alt_System::check_stop_thread_flag();
		this->allocate_dam_system();
		cout << "Start input of the DAM-polygon(s) for interception. Further information see DAM display..."<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Sys_Common_Output::output_dam->set_focus_on_display();
		emit send_dam_thread_runs(true);
		this->dam_system->input_polygons_per_file(this->file_list.at(0).toStdString());
		emit send_dam_thread_runs(false);
		Alt_System::check_stop_thread_flag();
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		emit send_dam_thread_runs(false);
		Sys_Common_Output::output_alt->set_focus_on_display();
		msg.output_msg(6);
	}

	Sys_Common_Output::output_alt->set_focus_on_display();
	cout << "Input of the DAM-polygon(s) for interception is finished!"<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);

	//check if operation was successful
	if(Dam_Damage_System::get_stop_thread_flag()==true){
		this->measure_implemt_ok=false;
	}
	if(this->dam_system->get_error_number()>0){
		this->measure_implemt_ok=false;
	}

	//change applied flags
	if(this->measure_implemt_ok==true){
		//switch system to base state
		this->switch_system2base_state();

		cout << "Switch all DAM-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, buff_base, false);
		Risk_Break_Scenario_Management::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
		Fpl_Frc_Curve::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
		//intercept polygons with the damage raster
		this->dam_system->intercept_raster_resettlement(this->id_list);

		cout << "Write detailed measure information to the database..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		this->set_measure_detailed_data2database(&this->qsqldatabase, this->general_alt_infos.id, "dam_tab" , this->id_list);
	}
	else{
		cout << "Implementation of resettlement as measure was not successful. The data are deleted!" << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
	}
}
//Implement dam switching of ecn-function
void Alt_System::switch_ecn_dam_functions(void){
	ostringstream cout;

	_sys_system_id buff_base;
	buff_base=this->system_id;
	buff_base.measure_nr=0;

	QSqlTableModel model(NULL, this->qsqldatabase);

	//output
	cout << "DAM-polygon file"<<endl;
	for(int i=0; i< this->file_list.count();i++){
		cout <<" " << this->file_list.at(i).toStdString()<<endl;
	}
	Sys_Common_Output::output_alt->output_txt(&cout);

	try{
		Alt_System::check_stop_thread_flag();
		this->allocate_dam_system();
		cout << "Start input of the DAM-polygon(s) for interception. Further information see DAM display..."<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Sys_Common_Output::output_dam->set_focus_on_display();
		emit send_dam_thread_runs(true);
		this->dam_system->input_polygons_per_file(this->file_list.at(0).toStdString());
		emit send_dam_thread_runs(false);
		Alt_System::check_stop_thread_flag();
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		emit send_dam_thread_runs(false);
		Sys_Common_Output::output_alt->set_focus_on_display();
		msg.output_msg(6);
	}

	Sys_Common_Output::output_alt->set_focus_on_display();
	cout << "Input of the DAM-polygon(s) for interception is finished!"<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);

	//check if operation was successful
	if(Dam_Damage_System::get_stop_thread_flag()==true){
		this->measure_implemt_ok=false;
	}
	if(this->dam_system->get_error_number()>0){
		this->measure_implemt_ok=false;
	}

	//change applied flags
	if(this->measure_implemt_ok==true){
		//switch system to base state
		this->switch_system2base_state();

		cout << "Switch all DAM-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, buff_base, false);
		Risk_Break_Scenario_Management::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
		Fpl_Frc_Curve::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
		//intercept polygons with the damage raster
		Sys_Common_Output::output_dam->set_focus_on_display();
		this->dam_system->intercept_ecn_raster(this->id_list_current, this->id_list);
		Sys_Common_Output::output_alt->set_focus_on_display();
		cout << "Write detailed measure information to the database..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		this->set_measure_detailed_data2database(&this->qsqldatabase, this->general_alt_infos.id, dam_label::tab_ecn_element , this->id_list);
	}
	else{
		cout << "Implementation of switching the ECN-damage function as measure was not successful. The data are deleted!" << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
	}
}
//Implement dam switching of eco-biotope type
void Alt_System::switch_eco_dam_biotope_type(void){
	ostringstream cout;

	_sys_system_id buff_base;
	buff_base=this->system_id;
	buff_base.measure_nr=0;

	QSqlTableModel model(NULL, this->qsqldatabase);

	//output
	cout << "DAM-polygon file"<<endl;
	for(int i=0; i< this->file_list.count();i++){
		cout <<" " << this->file_list.at(i).toStdString()<<endl;
	}
	Sys_Common_Output::output_alt->output_txt(&cout);

	try{
		Alt_System::check_stop_thread_flag();
		this->allocate_dam_system();
		cout << "Start input of the DAM-polygon(s) for interception. Further information see DAM display..."<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Sys_Common_Output::output_dam->set_focus_on_display();
		emit send_dam_thread_runs(true);
		this->dam_system->input_polygons_per_file(this->file_list.at(0).toStdString());
		emit send_dam_thread_runs(false);
		Alt_System::check_stop_thread_flag();
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		emit send_dam_thread_runs(false);
		Sys_Common_Output::output_alt->set_focus_on_display();
		msg.output_msg(6);
	}

	Sys_Common_Output::output_alt->set_focus_on_display();
	cout << "Input of the DAM-polygon(s) for interception is finished!"<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);

	//check if operation was successful
	if(Dam_Damage_System::get_stop_thread_flag()==true){
		this->measure_implemt_ok=false;
	}
	if(this->dam_system->get_error_number()>0){
		this->measure_implemt_ok=false;
	}

	//change applied flags
	if(this->measure_implemt_ok==true){
		//switch system to base state
		this->switch_system2base_state();

		cout << "Switch all DAM-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, buff_base, false);
		Risk_Break_Scenario_Management::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
		Fpl_Frc_Curve::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
		//intercept polygons with the damage raster
		Sys_Common_Output::output_dam->set_focus_on_display();
		this->dam_system->intercept_eco_btype_raster(this->id_list_current, this->id_list);
		Sys_Common_Output::output_alt->set_focus_on_display();
		cout << "Write detailed measure information to the database..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		this->set_measure_detailed_data2database(&this->qsqldatabase, this->general_alt_infos.id, dam_label::tab_ecn_element , this->id_list);
	}
	else{
		cout << "Implementation of switching the ECO-biotope type as measure was not successful. The data are deleted!" << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
	}
}
//Implement dam switching of eco-soil-type type
void Alt_System::switch_eco_dam_soil_type(void){
	ostringstream cout;

	_sys_system_id buff_base;
	buff_base=this->system_id;
	buff_base.measure_nr=0;

	QSqlTableModel model(NULL, this->qsqldatabase);

	//output
	cout << "DAM-polygon file"<<endl;
	for(int i=0; i< this->file_list.count();i++){
		cout <<" " << this->file_list.at(i).toStdString()<<endl;
	}
	Sys_Common_Output::output_alt->output_txt(&cout);

	try{
		Alt_System::check_stop_thread_flag();
		this->allocate_dam_system();
		cout << "Start input of the DAM-polygon(s) for interception. Further information see DAM display..."<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Sys_Common_Output::output_dam->set_focus_on_display();
		emit send_dam_thread_runs(true);
		this->dam_system->input_polygons_per_file(this->file_list.at(0).toStdString());
		emit send_dam_thread_runs(false);
		Alt_System::check_stop_thread_flag();
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		emit send_dam_thread_runs(false);
		Sys_Common_Output::output_alt->set_focus_on_display();
		msg.output_msg(6);
	}

	Sys_Common_Output::output_alt->set_focus_on_display();
	cout << "Input of the DAM-polygon(s) for interception is finished!"<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);

	//check if operation was successful
	if(Dam_Damage_System::get_stop_thread_flag()==true){
		this->measure_implemt_ok=false;
	}
	if(this->dam_system->get_error_number()>0){
		this->measure_implemt_ok=false;
	}

	//change applied flags
	if(this->measure_implemt_ok==true){
		//switch system to base state
		this->switch_system2base_state();

		cout << "Switch all DAM-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, buff_base, false);
		Risk_Break_Scenario_Management::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
		Fpl_Frc_Curve::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
		//intercept polygons with the damage raster
		Sys_Common_Output::output_dam->set_focus_on_display();
		this->dam_system->intercept_eco_soil_raster(this->id_list_current, this->id_list);
		Sys_Common_Output::output_alt->set_focus_on_display();
		cout << "Write detailed measure information to the database..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		this->set_measure_detailed_data2database(&this->qsqldatabase, this->general_alt_infos.id, dam_label::tab_ecn_element , this->id_list);
	}
	else{
		cout << "Implementation of switching the ECO-soil type as measure was not successful. The data are deleted!" << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
	}
}
//Implement dam switching of people2risk category
void Alt_System::switch_dam_people2risk_category(void){
	ostringstream cout;

	_sys_system_id buff_base;
	buff_base=this->system_id;
	buff_base.measure_nr=0;

	QSqlTableModel model(NULL, this->qsqldatabase);

	//output
	cout << "DAM-polygon file"<<endl;
	for(int i=0; i< this->file_list.count();i++){
		cout <<" " << this->file_list.at(i).toStdString()<<endl;
	}
	Sys_Common_Output::output_alt->output_txt(&cout);

	try{
		Alt_System::check_stop_thread_flag();
		this->allocate_dam_system();
		cout << "Start input of the DAM-polygon(s) for interception. Further information see DAM display..."<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Sys_Common_Output::output_dam->set_focus_on_display();
		emit send_dam_thread_runs(true);
		this->dam_system->input_polygons_per_file(this->file_list.at(0).toStdString());
		emit send_dam_thread_runs(false);
		Alt_System::check_stop_thread_flag();
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		emit send_dam_thread_runs(false);
		Sys_Common_Output::output_alt->set_focus_on_display();
		msg.output_msg(6);
	}

	Sys_Common_Output::output_alt->set_focus_on_display();
	cout << "Input of the DAM-polygon(s) for interception is finished!"<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);

	//check if operation was successful
	if(Dam_Damage_System::get_stop_thread_flag()==true){
		this->measure_implemt_ok=false;
	}
	if(this->dam_system->get_error_number()>0){
		this->measure_implemt_ok=false;
	}

	//change applied flags
	if(this->measure_implemt_ok==true){
		//switch system to base state
		this->switch_system2base_state();

		cout << "Switch all DAM-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, buff_base, false);
		Risk_Break_Scenario_Management::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
		Fpl_Frc_Curve::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
		//intercept polygons with the damage raster
		Sys_Common_Output::output_dam->set_focus_on_display();
		this->dam_system->intercept_pop_category_raster(this->id_list_current, this->id_list);
		Sys_Common_Output::output_alt->set_focus_on_display();
		cout << "Write detailed measure information to the database..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		this->set_measure_detailed_data2database(&this->qsqldatabase, this->general_alt_infos.id, dam_label::tab_ecn_element , this->id_list);
	}
	else{
		cout << "Implementation of switching the POP-category as measure was not successful. The data are deleted!" << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
	}
}
//Implement dam switching of psycho-social category
void Alt_System::switch_dam_psychosocial_category(void){
	ostringstream cout;

	_sys_system_id buff_base;
	buff_base=this->system_id;
	buff_base.measure_nr=0;

	QSqlTableModel model(NULL, this->qsqldatabase);

	//output
	cout << "DAM-polygon file"<<endl;
	for(int i=0; i< this->file_list.count();i++){
		cout <<" " << this->file_list.at(i).toStdString()<<endl;
	}
	Sys_Common_Output::output_alt->output_txt(&cout);

	try{
		Alt_System::check_stop_thread_flag();
		this->allocate_dam_system();
		cout << "Start input of the DAM-polygon(s) for interception. Further information see DAM display..."<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Sys_Common_Output::output_dam->set_focus_on_display();
		emit send_dam_thread_runs(true);
		this->dam_system->input_polygons_per_file(this->file_list.at(0).toStdString());
		emit send_dam_thread_runs(false);
		Alt_System::check_stop_thread_flag();
	}
	catch(Error msg){
		this->measure_implemt_ok=false;
		emit send_dam_thread_runs(false);
		Sys_Common_Output::output_alt->set_focus_on_display();
		msg.output_msg(6);
	}

	Sys_Common_Output::output_alt->set_focus_on_display();
	cout << "Input of the DAM-polygon(s) for interception is finished!"<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);

	//check if operation was successful
	if(Dam_Damage_System::get_stop_thread_flag()==true){
		this->measure_implemt_ok=false;
	}
	if(this->dam_system->get_error_number()>0){
		this->measure_implemt_ok=false;
	}

	//change applied flags
	if(this->measure_implemt_ok==true){
		//switch system to base state
		this->switch_system2base_state();

		cout << "Switch all DAM-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, buff_base, false);
		Risk_Break_Scenario_Management::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
		Fpl_Frc_Curve::copy_results(&this->qsqldatabase, buff_base, this->general_alt_infos.id);
		//intercept polygons with the damage raster
		Sys_Common_Output::output_dam->set_focus_on_display();
		this->dam_system->intercept_pys_category_raster(this->id_list_current, this->id_list);
		Sys_Common_Output::output_alt->set_focus_on_display();
		cout << "Write detailed measure information to the database..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		this->set_measure_detailed_data2database(&this->qsqldatabase, this->general_alt_infos.id, dam_label::tab_ecn_element , this->id_list);
	}
	else{
		cout << "Implementation of switching the PYS-category as measure was not successful. The data are deleted!" << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
	}
}
//Switch system to basesystem state (0)
void Alt_System::switch_system2base_state(void){
	if(this->system_id.measure_nr==0){
		return;
	}

	ostringstream cout;
	//reset existing measure
	_alt_general_info current_info;
	this->check_measure_is_set_database(&this->qsqldatabase, this->system_id, &current_info);
	QList<int> current_id_list;
	string table_name;
	this->get_detailed_info_measure_database(&this->qsqldatabase, this->system_id, &table_name, &current_id_list);

	_sys_system_id base_id;
	base_id=this->system_id;
	base_id.measure_nr=0;

	cout << "Switch in database tables from " <<current_info.name<<" to base system state..." << endl ;
	Sys_Common_Output::output_alt->output_txt(&cout);

	//hyd
	if(current_info.category==_alt_measure_category::category_hyd){
		if(current_info.detailed_type==alt_replace_hyd::boundary_sc){
			cout << "Switch HYD- and DAM-results to base system state from the current HYD-boundary scenario(s)...(may need some time!)" << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			cout << "Switch current HYD-results to false..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, current_info.id, false);
			cout << "Switch current DAM-results to false..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, current_info.id,  false );
			cout << "Switch base-state HYD-results to true..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, base_id, true);
			cout << "Switch base-state DAM-results to true..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, base_id, true);

			cout << "Switch the HYD-boundary scenario(s) to base system state..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Boundary_Szenario_Management::switch_applied_flag_table(&this->qsqldatabase, current_info.id, false);
			for(int i=0; i< current_id_list.count(); i++){
				Hyd_Boundary_Szenario_Management::switch_applied_flag_table(&this->qsqldatabase, base_id, true, current_id_list.at(i));
			}
		}
		else if(current_info.detailed_type==alt_replace_hyd::dike_lines || current_info.detailed_type==alt_add_hyd::dike_lines){
			cout << "Switch HYD- and DAM-results to base system state from the current HYD-boundary scenario(s)...(may need some time!)" << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);

			cout << "Switch current HYD-results to false..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, current_info.id, false);
			cout << "Switch base-state HYD-results to true..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, base_id, true);


			cout << "Switch current DAM-results to false..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, current_info.id,  false );
			cout << "Switch base-state DAM-results to true..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, base_id, true);

			cout << "Switch the HYD-dikeline(s) to base system state..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Floodplain_Polysegment::switch_applied_flag_polysegment_table(&this->qsqldatabase, current_info.id, false);
			for(int i=0; i< current_id_list.count(); i++){
				Hyd_Floodplain_Polysegment::switch_applied_flag_polysegment_table(&this->qsqldatabase, base_id, true, current_id_list.at(i));
			}
		}
		else if(current_info.detailed_type==alt_replace_hyd::river_profiles){
			cout << "Switch HYD- and DAM-results to base system state from the current HYD-boundary scenario(s)...(may need some time!)" << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			cout << "Switch current HYD-results to false..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, current_info.id, false);
			cout << "Switch base-state HYD-results to true..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, base_id, true);

			cout << "Switch current DAM-results to false..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, current_info.id,  false );
			cout << "Switch base-state DAM-results to true..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, base_id, true);

			cout << "Switch the HYD-river profile(s) to base system state..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);

			
			_Hyd_River_Profile::switch_applied_flag_profile_table(&this->qsqldatabase, current_info.id, false);
			for(int i=0; i< current_id_list.count(); i++){
				_Hyd_River_Profile::switch_applied_flag_profile_table(&this->qsqldatabase, base_id, current_id_list.at(i), true);
			}
		}
	}
	//dam
	else if(current_info.category==_alt_measure_category::category_dam){
			cout << "Switch HYD- and DAM-results to base system state from the current state...(may need some time!)" << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);

			cout << "Switch current HYD-results to false (may need some time)..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, current_info.id, false);

			cout << "Switch current DAM-results to false..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, current_info.id,  false );

			cout << "Switch base-state DAM-results to true..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, base_id,  true);

			if(current_info.detailed_type==alt_replace_dam::dam_resettlement){
				cout << "Switch all the DAM elements to base system state..." << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
				cout << "Switch all the ECN elements to base system state..." << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_Ecn_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
				Dam_Ecn_Element::switch_applied_flag_elem_table(&this->qsqldatabase, base_id, true);
				cout << "Switch all the ECO-biotope-type elements to base system state..." << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_Eco_Btype_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
				Dam_Eco_Btype_Element::switch_applied_flag_elem_table(&this->qsqldatabase, base_id, true);
				cout << "Switch all the ECO-soil-erosion elements to base system state..." << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_Eco_Soil_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
				Dam_Eco_Soil_Element::switch_applied_flag_elem_table(&this->qsqldatabase, base_id, true);
				cout << "Switch all the POP elements to base system state..." << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_People_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
				Dam_People_Element::switch_applied_flag_elem_table(&this->qsqldatabase, base_id, true);
				cout << "Switch all the PYS elements to base system state..." << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_Pys_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
				Dam_Pys_Element::switch_applied_flag_elem_table(&this->qsqldatabase, base_id, true);
				cout << "Switch all the SC points to base system state..." << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_Sc_Point::switch_applied_flag_point_table(&this->qsqldatabase, current_info.id, false);
				Dam_Sc_Point::switch_applied_flag_point_table(&this->qsqldatabase, base_id, true);
				cout << "Switch all the CI-points to base system state..." << endl;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_CI_Point::switch_applied_flag_point_table(&this->qsqldatabase, current_info.id, false);
				Dam_CI_Point::switch_applied_flag_point_table(&this->qsqldatabase, base_id, true);
				cout << "Switch all the CI-polygons to base system state..." << endl;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_CI_Polygon::switch_applied_flag_polygon_table(&this->qsqldatabase, current_info.id, false);
				Dam_CI_Polygon::switch_applied_flag_polygon_table(&this->qsqldatabase, base_id, true);
			}
			else if(current_info.detailed_type==alt_replace_dam::dam_ecn_function_polygon){
				cout << "Switch all the ECN elements to base system state..." << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_Ecn_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
				Dam_Ecn_Element::switch_applied_flag_elem_table(&this->qsqldatabase, base_id, true);
			}
			else if(current_info.detailed_type==alt_replace_dam::dam_eco_btype_function_polygon){
				cout << "Switch all the ECO-biotope-type elements to base system state..." << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_Eco_Btype_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
				Dam_Eco_Btype_Element::switch_applied_flag_elem_table(&this->qsqldatabase, base_id, true);
			}
			else if(current_info.detailed_type==alt_replace_dam::dam_eco_soil_function_polygon){
				cout << "Switch all the ECO-soil-type elements to base system state..." << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_Eco_Soil_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
				Dam_Eco_Soil_Element::switch_applied_flag_elem_table(&this->qsqldatabase, base_id, true);
			}
			else if(current_info.detailed_type==alt_replace_dam::dam_pop_categories_polygon){
				cout << "Switch all the POP-categories of the elements to base system state..." << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_People_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
				Dam_People_Element::switch_applied_flag_elem_table(&this->qsqldatabase, base_id, true);
			}
			else if(current_info.detailed_type==alt_replace_dam::dam_pys_categories_polygon){
				cout << "Switch all the PYS-categories of the elements to base system state..." << endl ;
				Sys_Common_Output::output_alt->output_txt(&cout);
				Dam_Pys_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
				Dam_Pys_Element::switch_applied_flag_elem_table(&this->qsqldatabase, base_id, true);
			}
	}
	//fpl
	else if(current_info.category==_alt_measure_category::category_fpl){
		if(current_info.detailed_type==alt_combi_fpl::fpl_line || current_info.detailed_type==alt_add_fpl::section ||
			current_info.detailed_type==alt_replace_fpl::section || (current_info.detailed_type==alt_replace_fpl::section_param)
			){
			cout << "Switch all HYD-results to base system state from the current system state...(may need some time)" << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			cout << "Switch current HYD-results to false..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, current_info.id , false);
			cout << "Switch base-state HYD-results to true..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, base_id , true);
			cout << "Switch all DAM-results to base system state from the current system state..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			cout << "Switch current DAM-results to false..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, current_info.id, false);
			cout << "Switch base-state DAM-results to true..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, base_id, true);

			cout << "Switch the FPL-section(s) and their results to base system state from the current system state..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			//section
			Fpl_Section::switch_applied_flag_section_table(&this->qsqldatabase, base_id, true);
			Fpl_Random_Variables::switch_applied_flag_variable_table(&this->qsqldatabase, base_id, true);
			Fpl_Section_Points::switch_applied_flag_point_table(&this->qsqldatabase, base_id, true);
			Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_table(&this->qsqldatabase, base_id, true);
			//results
			Fpl_Mc_Sim::switch_applied_flag_result_table(&this->qsqldatabase, base_id, true);

			cout << "Switch the FPL-section(s) and their results of the current system state to false..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			//section
			Fpl_Section::switch_applied_flag_section_table(&this->qsqldatabase,current_info.id, false);
			Fpl_Random_Variables::switch_applied_flag_variable_table(&this->qsqldatabase, current_info.id, false);
			Fpl_Section_Points::switch_applied_flag_point_table(&this->qsqldatabase, current_info.id, false);
			Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_table(&this->qsqldatabase, current_info.id, false);
			//results
			Fpl_Mc_Sim::switch_applied_flag_result_table(&this->qsqldatabase, current_info.id, false);
		}
	}
	cout << "Switching to base measure-state is finished..."<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);
	//implement here further measures...
}
//Switch system to a given system state from the base system state
void Alt_System::switch_system2given_state(const _sys_system_id id){
	if(id.measure_nr==0){
		return;
	}

	ostringstream cout;
	//reset existing measure
	_alt_general_info current_info;
	if(this->check_measure_is_set_database(&this->qsqldatabase, id, &current_info)==false){
		Warning msg=this->set_warning(3);
		ostringstream info;
		info<<"Measure state : "<< id.measure_nr<<endl;
		info<<"Area state    : "<<id.area_state<<endl;
		msg.make_second_info(info.str());
		msg.output_msg(6);
		return;
	}
	QList<int> current_id_list;
	string table_name;
	this->get_detailed_info_measure_database(&this->qsqldatabase, id, &table_name, &current_id_list);

	_sys_system_id base_id;
	base_id=this->system_id;
	base_id.measure_nr=0;

	cout << "Switch in database tables from base system state to " <<current_info.name<<" (current system state)..." << endl ;
	Sys_Common_Output::output_alt->output_txt(&cout);

	//hyd
	if(current_info.category==_alt_measure_category::category_hyd){
		if(current_info.detailed_type==alt_replace_hyd::boundary_sc){
			cout << "Switch HYD- and DAM-results from base system state to current system state..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, current_info.id, true);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, current_info.id,  true );
			for(int i=0; i< current_id_list.count(); i++){
				Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, base_id, false, current_id_list.at(i));
				Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, base_id, current_id_list.at(i), false);
			}
			cout << "Switch the HYD-boundary scenario(s) from base system state to current system state..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Boundary_Szenario_Management::switch_applied_flag_table(&this->qsqldatabase, current_info.id, true);
			for(int i=0; i< current_id_list.count(); i++){
				Hyd_Boundary_Szenario_Management::switch_applied_flag_table(&this->qsqldatabase, base_id, false, current_id_list.at(i));
			}
		}
		else if(current_info.detailed_type==alt_replace_hyd::dike_lines || current_info.detailed_type==alt_add_hyd::dike_lines){
			cout << "Switch HYD- and DAM-results from base system state to system state..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, current_info.id, true);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, base_id, false);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, current_info.id,  true);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, base_id, false);

			cout << "Switch the HYD-dikeline(s) to current system state..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Floodplain_Polysegment::switch_applied_flag_polysegment_table(&this->qsqldatabase, current_info.id, true);
			for(int i=0; i< current_id_list.count(); i++){
				Hyd_Floodplain_Polysegment::switch_applied_flag_polysegment_table(&this->qsqldatabase, base_id, false, current_id_list.at(i));
			}
		}
		else if(current_info.detailed_type==alt_replace_hyd::river_profiles){
			cout << "Switch HYD- and DAM-results from base system state to system state..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, current_info.id, true);
			Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, base_id, false);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, current_info.id,  true);
			Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, base_id, false);

			cout << "Switch the HYD-river profile(s) to current system state..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);

			
			_Hyd_River_Profile::switch_applied_flag_profile_table(&this->qsqldatabase, current_info.id, true);
			
			for(int i=0; i< current_id_list.count(); i++){
				_Hyd_River_Profile::switch_applied_flag_profile_table(&this->qsqldatabase, base_id, current_id_list.at(i), false);
			}
		}
	}
	//dam
	else if(current_info.category==_alt_measure_category::category_dam){
		cout << "Switch all DAM-results of the base system state to applied 'false'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, base_id, false);
		cout << "Switch all DAM-results of the current system state to applied 'true'..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, current_info.id, true);

		if(current_info.detailed_type==alt_replace_dam::dam_resettlement){
			cout << "Switch all relevant DAM-elements to the current system state to applied 'true'..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			cout << "Switch all relevant ECN-elements to the current system state to applied 'true'..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Ecn_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, true);
			Dam_Ecn_Element::reswitch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
			cout << "Switch all relevant ECO biotop-type elements to the current system state to applied 'true'..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Eco_Btype_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, true);
			Dam_Eco_Btype_Element::reswitch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
			cout << "Switch all relevant POP people2risk elements to the current system state to applied 'true'..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_People_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, true);
			Dam_People_Element::reswitch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
			cout << "Switch all relevant PYS psycho-social elements to the current system state to applied 'true'..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Pys_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, true);
			Dam_Pys_Element::reswitch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
			cout << "Switch all relevant SC simple-counting points to the current system state to applied 'true'..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Sc_Point::switch_applied_flag_point_table(&this->qsqldatabase, current_info.id, true);
			Dam_Sc_Point::reswitch_applied_flag_point_table(&this->qsqldatabase, current_info.id, false);
			cout << "Switch all relevant CI-points to the current system state to applied 'true'..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_CI_Point::switch_applied_flag_point_table(&this->qsqldatabase, current_info.id, true);
			Dam_CI_Point::reswitch_applied_flag_point_table(&this->qsqldatabase, current_info.id, false);
			cout << "Switch all relevant CI-polygons to the current system state to applied 'true'..." << endl;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_CI_Polygon::switch_applied_flag_polygon_table(&this->qsqldatabase, current_info.id, true);
			Dam_CI_Polygon::reswitch_applied_flag_polygon_table(&this->qsqldatabase, current_info.id, false);
		}
		else if(current_info.detailed_type==alt_replace_dam::dam_ecn_function_polygon){
			cout << "Switch all relevant DAM ECN-elements to the current system state to applied 'true'..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Ecn_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, true);
			Dam_Ecn_Element::reswitch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
		}
		else if(current_info.detailed_type==alt_replace_dam::dam_eco_btype_function_polygon){
			cout << "Switch all relevant DAM ECO biotop-type elements to the current system state to applied 'true'..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Eco_Btype_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, true);
			Dam_Eco_Btype_Element::reswitch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
		}
		else if(current_info.detailed_type==alt_replace_dam::dam_eco_soil_function_polygon){
			cout << "Switch all relevant DAM ECO soil-erosion elements to the current system state to applied 'true'..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Eco_Soil_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, true);
			Dam_Eco_Soil_Element::reswitch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
		}
		else if(current_info.detailed_type==alt_replace_dam::dam_pop_categories_polygon){
			cout << "Switch all relevant DAM POP people2risk elements to the current system state to applied 'true'..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_People_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, true);
			Dam_People_Element::reswitch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
		}
		else if(current_info.detailed_type==alt_replace_dam::dam_pys_categories_polygon){
			cout << "Switch all relevant DAM PYS psycho-social elements to the current system state to applied 'true'..." << endl ;
			Sys_Common_Output::output_alt->output_txt(&cout);
			Dam_Pys_Element::switch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, true);
			Dam_Pys_Element::reswitch_applied_flag_elem_table(&this->qsqldatabase, current_info.id, false);
		}
	}
	//fpl
	else if(current_info.category==_alt_measure_category::category_fpl){
		cout << "Switch all HYD-results from base system state to the current system state..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, current_info.id , true);
		Hyd_Hydraulic_System::switch_applied_flags_results(&this->qsqldatabase, base_id , false);
		cout << "Switch all DAM-results from base system state to the current system state..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, current_info.id, true);
		Dam_Damage_System::switch_applied_flag_erg_table(&this->qsqldatabase, base_id, false);

		cout << "Switch the FPL-section(s) and their results from base system state to the current system state..." << endl ;
		Sys_Common_Output::output_alt->output_txt(&cout);

		//section
		Fpl_Section::switch_applied_flag_section_table(&this->qsqldatabase, current_info.id, true);
		Fpl_Random_Variables::switch_applied_flag_variable_table(&this->qsqldatabase, current_info.id, true);
		Fpl_Section_Points::switch_applied_flag_point_table(&this->qsqldatabase, current_info.id, true);
		Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_table(&this->qsqldatabase, current_info.id, true);
		//results
		Fpl_Mc_Sim::switch_applied_flag_result_table(&this->qsqldatabase, current_info.id, true);

		for(int i=0; i< current_id_list.count(); i++){
			if(current_id_list.at(i)>=0){
				//section
				Fpl_Section::switch_applied_flag_section_table(&this->qsqldatabase, base_id, false, current_id_list.at(i));
				Fpl_Random_Variables::switch_applied_flag_variable_table(&this->qsqldatabase, base_id, false, current_id_list.at(i));
				Fpl_Section_Points::switch_applied_flag_point_table(&this->qsqldatabase, base_id, false, current_id_list.at(i));
				Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_table(&this->qsqldatabase, base_id, false, current_id_list.at(i));
				//results
				Fpl_Mc_Sim::switch_applied_flag_result_table(&this->qsqldatabase, base_id, false, current_id_list.at(i));
			}
		}
	}
	cout << "Switching between measure-states is finished..."<<endl;
	Sys_Common_Output::output_alt->output_txt(&cout);
}
//Set warning(s)
Warning Alt_System::set_warning(const int warn_type){
	string place="Alt_System::";
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
		case 1://results datas can not submitted
			place.append("set_measure_data2database(QSqlDatabase *ptr_database, _alt_general_info data)") ;
			reason="Can not submit the alternative's data to database table";
			help="Check the database";
			type=2;
			break;
		case 2://results datas can not submitted
			place.append("set_measure_detailed_data2database(QSqlDatabase *ptr_database, const _sys_system_id id, const string table_name, QList<int> list)") ;
			reason="Can not submit the alternative's detailed data to database table";
			help="Check the database";
			type=2;
			break;
		case 3://no system state available
			place.append("switch_system2given_state(const _sys_system_id id)") ;
			reason="No measure is available in database table for the given measure state";
			help="Check the database";
			type=25;
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
Error Alt_System::set_error(const int err_type){
	string place="Alt_System::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_alt_list(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://can not load the alternatives from database table
			place.append("load_list_current_alt(QSqlDatabase *ptr_database, cont int area_state)");
			reason="Can not load the list of current available alternatives";
			help="Check the database table";
			type=9;
			break;
		case 2://can not check if an alternatives is set in database table
			place.append("check_measure_is_set_database(void)");
			reason="Can not check, if an alternative is set in database table";
			help="Check the database table";
			type=9;
			break;
		case 3://bad alloc
			place.append("allocate_hyd_system(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 4://can not get the detailed measure informations from database
			place.append("get_detailed_info_measure_database(QSqlDatabase *ptr_database, const _sys_system_id id, string *table_name, QList<int> *list)");
			reason="Can not get the detailed measrue's information from database table";
			help="Check the database table";
			type=9;
			break;
		case 5://bad alloc
			place.append("allocate_fpl_system(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 6://bad alloc
			place.append("implement_hyd_dikelines(const bool replace)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 7://bad alloc
			place.append("allocate_dam_system(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 8://no damage polygon file is set
			place.append("implement_dam_measures(void)");
			reason="No damage polygon file for intercption is set. The DAM-measure can not be implemented";
			help="Set a damage polygon file";
			type=31;
			break;
		case 9://no damage polygon file is set
			place.append("change_hyd_river_prof(void)");
			reason="No river pointer is found";
			help="Can not replace the given river model";
			type=31;
			break;
		case 10://bad alloc
			place.append("change_hyd_river_prof(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
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