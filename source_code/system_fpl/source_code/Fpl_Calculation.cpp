//#include "Fpl_Calculation.h"
#include "Fpl_Headers_Precompiled.h"

//init static members
bool Fpl_Calculation::abort_thread_flag=false;

//Default constructor
Fpl_Calculation::Fpl_Calculation(void){
	this->thread_type=_fpl_thread_type::fpl_thread_unknown;
	this->number_error=0;
	this->number_warning_begin=0;
	this->number_warning=0;

	this->random_checker=NULL;
	this->random2check=NULL;

	this->mc_simulation=NULL;
	this->frc_simulation=NULL;
	this->section2calc=NULL;

	this->number_section=0;
	this->list_section_id=NULL;

	this->flag_restore_default=false;
	this->flag_restore_prob_param=false;
	this->flag_restore_fault_tree=false;

	this->file_names=NULL;

	this->output_folder=label::not_set;

	this->overwrite_flag=true;

	this->thread_is_started=false;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Calculation),_sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Calculation::~Fpl_Calculation(void){
	//close database
	if(this->qsqldatabase.isOpen()){
		this->qsqldatabase.close();
	}
	//delete the class
	this->delete_fpl_section();
	this->delete_frc_sim();
	this->delete_mc_sim();
	this->delete_rand_checker();
	this->delete_file_names();

	this->delete_list_sec_id();
	this->set_stop_thread_flag(false);

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Calculation), _sys_system_modules::FPL_SYS);//count the memory
}
//___________
//public
//Set the number of files for calculation and the global filenames per dialog (gui)
bool Fpl_Calculation::set_system_number_file_gui(void){
	Sys_Multipathsettings_Dia dialog;
	ostringstream text;
	QIcon icon;
	icon.addFile(":fpl_icon");

	switch(this->thread_type){
		//case (_fpl_thread_type::hyd_calculation):
		//	text << "Choose the fpl-section file(s), which " << endl;
		//	text << "should be calculated"<< endl;
		//	dialog.set_txt2headerlabel(text.str());
		//	break;
		case (_fpl_thread_type::fpl_check_sec_file):
			text << "Choose the fpl-section file(s), which " << endl;
			text << "should be checked"<< endl;
			dialog.set_txt2headerlabel(text.str(),icon);
			break;
		case (_fpl_thread_type::fpl_import_file):
			text << "Choose the fpl-section file(s), which " << endl;
			text << "should be imported to database"<< endl;
			text << "The section id(s) are incremented by 1"<< endl;
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
		this->number_section=dialog.getFilesnumber();
		if(this->number_section==0){
			flag=false;
			return flag;
		}
		try{
			this->allocate_file_names();
		}
		catch(Error msg){
			throw msg;
		}

		for(int i=0; i<this->number_section; i++){
			this->file_names[i]=dialog.get_filename(i);
		}
	}
	return flag;
}
//Set the number of files for calculation and the global filenames direct
void Fpl_Calculation::set_system_number_file_direct(const QStringList files2import){
	this->number_section=files2import.count();
	this->allocate_file_names();
	for(int i=0; i<this->number_section; i++){
		this->file_names[i]=files2import.at(i).toStdString();
	}
}
//Set the thread type for specifiying the task of the thread
void Fpl_Calculation::set_thread_type(_fpl_thread_type type){
	this->thread_type=type;
	if(this->thread_type==_fpl_thread_type::fpl_test_random){
		try{
			this->allocate_rand_checker();
		}
		catch(Error msg){
			msg.output_msg(1);
		}
	}
}
//Set a pointer to the database; the copy of the database is made in this method
void Fpl_Calculation::set_ptr2database(QSqlDatabase *ptr_database){
	//if (QSqlDatabase::contains(sys_label::str_fpl.c_str()) == true) {
	//	return;
	//}
	this->qsqldatabase=QSqlDatabase::cloneDatabase(*ptr_database, sys_label::str_fpl.c_str());
	this->qsqldatabase.open();
}
//Set the list of the section -ids, which should be handled
void Fpl_Calculation::set_list_section_ids(QList<int> list){
	this->number_section=list.count();
	this->allocate_list_sec_id();
	for(int i=0; i<this->number_section; i++){
		this->list_section_id[i]=list.at(i);
	}
}
//Set the list of the section-ids, which should be handled
void Fpl_Calculation::set_list_section_ids(const int number, _fpl_break_info* section_ids){
	this->number_section=number;
	this->allocate_list_sec_id();
	for(int i=0; i<this->number_section; i++){
		this->list_section_id[i]=section_ids[i].id_fpl_sec;
	}
}
//Set the list of the section-ids, which should be handled. Here all relevant section for a risk calculation are set to the list
void Fpl_Calculation::set_list_section_ids(void){
	QSqlQueryModel model;
	int number=0;
	int counter=0;
	_fpl_section_types type;
	number=Fpl_Section::select_relevant_section_database(&model, &this->qsqldatabase, this->system_id, false);

	//count relevant sections
	for(int i=0; i<number; i++){
		type=_Fpl_Section_Type::convert_txt2section_type(model.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_type)).c_str()).toString().toStdString());
		if(type==_fpl_section_types::dike_sec || type==_fpl_section_types::mob_wall_sec ||
			type==_fpl_section_types::wall_sec){
			counter++;
		}
	}

	//set list relevant section
	this->number_section=counter;
	this->allocate_list_sec_id();
	counter=0;
	for(int i=0; i<number; i++){
		type=_Fpl_Section_Type::convert_txt2section_type(model.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_type)).c_str()).toString().toStdString());
		if(type==_fpl_section_types::dike_sec || type==_fpl_section_types::mob_wall_sec ||
			type==_fpl_section_types::wall_sec){
			this->list_section_id[counter]=model.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_id)).c_str()).toInt();
			counter++;
		}
	}
}
//Set the list of the section-ids, just for the current measure state
void Fpl_Calculation::set_list_section_ids_current(void){
	QSqlQueryModel model;
	int number=0;
	number=Fpl_Section::select_section_database(&model, &this->qsqldatabase, this->system_id);
	//set list relevant section
	this->number_section=number;
	this->allocate_list_sec_id();
	for(int i=0; i<number; i++){
		this->list_section_id[i]=model.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_id)).c_str()).toInt();
	}
}
//Set the list of the section-ids, just for the current measure state and for section with the type set by hand
void Fpl_Calculation::set_list_section_ids_current_set_by_hand(void){
	QSqlQueryModel model;
	int number=0;
	number=Fpl_Section::select_section_database(&model, &this->qsqldatabase, this->system_id, _fpl_section_types::by_hand_sec);
	//set list relevant section
	this->number_section=number;
	this->allocate_list_sec_id();
	for(int i=0; i<number; i++){
		this->list_section_id[i]=model.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_id)).c_str()).toInt();
	}
}
///Get the number of section in the list
int Fpl_Calculation::get_number_section_in_list(void){
	return this->number_section;
}
///Get the thread type
_fpl_thread_type Fpl_Calculation::get_thread_type(void){
	return this->thread_type;
}
//Create a whole set of fpl database tables
void Fpl_Calculation::create_fpl_database_tables(void){
	ostringstream cout;
	cout << "Create FPL system tables..." << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_start_warnings_number();
	try{
		if(_Fpl_Section_Type::create_table_mechanism(&this->qsqldatabase)==true){
			_Fpl_Section_Type::set_predefined_data2mechanism_table(&this->qsqldatabase);
		}
		//section
		Fpl_Section::create_table(&this->qsqldatabase);
		Fpl_Sec_Type_Highground::create_result_table(&this->qsqldatabase);
		Fpl_Sec_Type_Dike::create_result_table(&this->qsqldatabase);
		Fpl_Sec_Type_Dune::create_result_table(&this->qsqldatabase);

		//performance
		if(Fpl_Mc_Sim::create_table(&this->qsqldatabase)==true){
			Fpl_Mc_Sim::set_predefined_data2control_table(&this->qsqldatabase);
		}
		Fpl_Mc_Sim::create_result_table(&this->qsqldatabase);
		Fpl_Frc_Curve::create_table_intervals(&this->qsqldatabase);
		//random variables
		Fpl_Random_Variables::create_table(&this->qsqldatabase);
		//distribution types
		Fpl_Distribution_Class_Mean::create_table(&this->qsqldatabase);
		if(Fpl_Distribution_Class_Mean::create_default_table(&this->qsqldatabase)==true){
			Fpl_Distribution_Class_Mean::write_default_variables2table(&this->qsqldatabase);
		}
		Fpl_Distribution_Class_Triangle::create_table(&this->qsqldatabase);
		if(Fpl_Distribution_Class_Triangle::create_default_table(&this->qsqldatabase)==true){
			Fpl_Distribution_Class_Triangle::write_default_variables2table(&this->qsqldatabase);
		}
		Fpl_Distribution_Class_Discret::create_table(&this->qsqldatabase);
		if(Fpl_Distribution_Class_Discret::create_default_table(&this->qsqldatabase)==true){
			Fpl_Distribution_Class_Discret::write_default_variables2table(&this->qsqldatabase);
		}
		Fpl_Distribution_Class_Discret_Mean::create_table(&this->qsqldatabase);
		if(Fpl_Distribution_Class_Discret_Mean::create_default_table(&this->qsqldatabase)==true){
			Fpl_Distribution_Class_Discret_Mean::write_default_variables2table(&this->qsqldatabase);
		}
		Fpl_Distribution_Class_Mean_Mean::create_table(&this->qsqldatabase);
		if(Fpl_Distribution_Class_Mean_Mean::create_default_table(&this->qsqldatabase)==true){
			Fpl_Distribution_Class_Mean_Mean::write_default_variables2table(&this->qsqldatabase);
		}

		Fpl_Section_Points::create_point_table(&this->qsqldatabase);
		Fpl_Mech_Slope_Stability_Dike::create_control_table(&this->qsqldatabase);

		Fpl_Dike_Geo_Materialzone::create_matzone_table(&this->qsqldatabase);
		Fpl_Dike_Geo_Materialzone::create_matzone_point_table(&this->qsqldatabase);
	}
	catch(Error msg){
		this->number_error++;
		msg.output_msg(1);
	}
	this->set_warning_number();
	cout << "Creation of the FPL database tables is finished" << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->output_error_statistic();
}
//Check all fpl database tables, for their existence in the database and their declaration in the database table-file
void Fpl_Calculation::check_fpl_database_tables(void){
	ostringstream cout;
	cout << "Check FPL system tables..." << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
	this->set_start_warnings_number();
	try{
		cout << "Check fpl section database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Section::set_table(&this->qsqldatabase);
		cout << "Check highground section result database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Sec_Type_Highground::set_result_table(&this->qsqldatabase);
		cout << "Check dike section result database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Sec_Type_Dike::set_result_table(&this->qsqldatabase);
		cout << "Check dune section result database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Sec_Type_Dune::set_result_table(&this->qsqldatabase);

		cout << "Check fault tree database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);

		//performance
		_Fpl_Section_Type::set_table_mechanism(&this->qsqldatabase);
		cout << "Check reliability control parameter database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Mc_Sim::set_table(&this->qsqldatabase);
		cout << "Check reliability results database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Mc_Sim::set_result_table(&this->qsqldatabase);
		cout << "Check interval table of the frc-curve..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Frc_Curve::set_table_intervals(&this->qsqldatabase);

		//random variables
		cout << "Check random variables database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Random_Variables::set_table(&this->qsqldatabase);

		//distribution type
		cout << "Check mean type distribution user data database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Distribution_Class_Mean::set_table(&this->qsqldatabase);
		cout << "Check mean type distribution default data database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Distribution_Class_Mean::set_default_table(&this->qsqldatabase);
		cout << "Check triangle type distribution user data database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Distribution_Class_Triangle::set_table(&this->qsqldatabase);
		cout << "Check triangle type distribution default database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Distribution_Class_Triangle::set_default_table(&this->qsqldatabase);
		cout << "Check discrete type distribution user data database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Distribution_Class_Discret::set_table(&this->qsqldatabase);
		cout << "Check discrete type distribution default database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Distribution_Class_Discret::set_default_table(&this->qsqldatabase);
		cout << "Check discrete2mean type distribution user data database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Distribution_Class_Discret_Mean::set_table(&this->qsqldatabase);
		cout << "Check discrete2mean type distribution default database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Distribution_Class_Discret_Mean::set_default_table(&this->qsqldatabase);
		cout << "Check mean2mean type distribution user data database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Distribution_Class_Mean_Mean::set_table(&this->qsqldatabase);
		cout << "Check mean2mean type distribution default database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Distribution_Class_Mean_Mean::set_default_table(&this->qsqldatabase);
		cout << "Check geometric point database table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Section_Points::set_point_table(&this->qsqldatabase);

		cout << "Check slope control parameter table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Mech_Slope_Stability_Dike::set_control_table(&this->qsqldatabase);

		cout << "Check material zone table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Dike_Geo_Materialzone::set_matzone_table(&this->qsqldatabase);

		cout << "Check material zone point table..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
		Fpl_Dike_Geo_Materialzone::set_matzone_point_table(&this->qsqldatabase);
	}
	catch(Error msg){
		this->number_error++;
		ostringstream info;
		info << "Check if the FPL tables are created!"<< endl;
		msg.make_second_info(info.str());
		//switch to non-fatal, because is handled later as fatal error
		msg.set_fatal_flag(false);
		msg.output_msg(1);
		this->output_error_statistic();
	}
	this->set_warning_number();
	cout << "Check of the FPL database tables is finished" << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false, false);
}
//Delete the data of all fpl database tables
void Fpl_Calculation::delete_data_fpl_database_tables(void){
	try{
		//section type
		Fpl_Section::delete_data_in_table(&this->qsqldatabase);
		Fpl_Sec_Type_Highground::delete_data_in_result_table(&this->qsqldatabase);
		Fpl_Sec_Type_Dike::delete_data_in_result_table(&this->qsqldatabase);
		Fpl_Sec_Type_Dune::delete_data_in_result_table(&this->qsqldatabase);
		_Fpl_Section_Type::reset_data_in_table(&this->qsqldatabase);

		//performance
		Fpl_Mc_Sim::reset_data_in_table(&this->qsqldatabase);
		Fpl_Mc_Sim::delete_data_in_result_table(&this->qsqldatabase);
		Fpl_Frc_Curve::delete_data_in_table_intervals(&this->qsqldatabase);

		//random variables
		Fpl_Random_Variables::delete_data_in_table(&this->qsqldatabase);

		//distribution type
		Fpl_Distribution_Class_Mean::delete_data_in_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Mean::delete_data_in_default_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Triangle::delete_data_in_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Triangle::delete_data_in_default_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Discret::delete_data_in_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Discret::delete_data_in_default_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Discret_Mean::delete_data_in_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Discret_Mean::delete_data_in_default_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Mean_Mean::delete_data_in_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Mean_Mean::delete_data_in_default_table(&this->qsqldatabase);

		Fpl_Section_Points::delete_data_in_point_table(&this->qsqldatabase);

		Fpl_Mech_Slope_Stability_Dike::delete_data_in_control_table(&this->qsqldatabase);

		Fpl_Dike_Geo_Materialzone::delete_data_in_matzone_point_table(&this->qsqldatabase);
		Fpl_Dike_Geo_Materialzone::delete_data_in_matzone_table(&this->qsqldatabase);
	}
	catch(Error msg){
		msg.output_msg(1);
	}
}
//Close all fpl database tables (static)
void Fpl_Calculation::close_fpl_database_tables(void){
	_Fpl_Section_Type::close_table();
	Fpl_Sec_Type_Highground::close_result_table();
	Fpl_Sec_Type_Dike::close_result_table();
	Fpl_Sec_Type_Dune::close_result_table();
	Fpl_Section::close_table();
	Fpl_Mc_Sim::close_result_table();
	Fpl_Mc_Sim::close_table();
	Fpl_Random_Variables::close_table();
	Fpl_Frc_Curve::close_table_intervals();

	Fpl_Section_Points::close_point_table();

	Fpl_Mech_Slope_Stability_Dike::close_control_table();

	Fpl_Dike_Geo_Materialzone::close_matzone_point_table();
	Fpl_Dike_Geo_Materialzone::close_matzone_table();
}
//Ask for the section which are handled in the fpl system with a dialog
bool Fpl_Calculation::ask_section2handle(QWidget *parent, const _fpl_thread_type type){
	bool accepted=false;
	this->delete_list_sec_id();
	this->set_thread_type(type);

	FplGui_Section_Dia my_dia(parent);

	string header;

	switch(type){
		case _fpl_thread_type::fpl_determ_selected:
				header="Deterministic calculation";
				break;
		case _fpl_thread_type::fpl_mc_selected:
				header="Monte-Carlo calculation";
				break;
		case _fpl_thread_type::fpl_frc_selected:
				header="Fragility curve calculation";
				break;
		case _fpl_thread_type::fpl_frc_by_hand:
				header="Fragility curve by user input";
				break;
		case _fpl_thread_type::fpl_del_section:
				header="Delete fpl-section(s)";
				break;
		case _fpl_thread_type::fpl_check_sec_db:
				header="Check fpl-section(s)";
				break;
		case _fpl_thread_type::fpl_export_determ_sec:
				header = "Export determinstic results of fpl-section(s)";
				break;
		case _fpl_thread_type::fpl_export_mc_sec:
				header = "Export MC-results of fpl-section(s)";
				break;
		case _fpl_thread_type::fpl_export_frc_sec:
				header = "Export FRC-results of fpl-section(s)";
				break;
		default:
			accepted=false;
			return accepted;
	}

	try{
		//my_dia
		my_dia.set_dia_header_txt(header);
		my_dia.set_dialog(&this->qsqldatabase, this->system_id, type);

		accepted=my_dia.start_dialog(&this->list_section_id, &this->number_section);
		if(this->number_section==0){
			accepted=false;
		}
	}
	catch(Error msg){
		msg.output_msg(1);
		accepted=false;
	}

	return accepted;
}
//Ask flags which part of the database tables should be restored with the default values
bool Fpl_Calculation::ask_restore_default_database(QWidget *parent){
	Sys_Multi_CheckBox_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":fpl_icon");

	my_dia.set_number_check_boxes(3,icon);
	stringstream buffer;
	buffer << "Choose which data should be restored to the default values..."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose restoring FPL-system default data");
	my_dia.get_ptr_check_box(0)->setText("Restore FPL-control parameters");
	my_dia.get_ptr_check_box(1)->setText("Restore FPL-fault tree");
	my_dia.get_ptr_check_box(2)->setText("Restore FPL-default variables");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->flag_restore_prob_param=my_dia.get_bool_check_box(0);
		this->flag_restore_fault_tree=my_dia.get_bool_check_box(1);
		this->flag_restore_default=my_dia.get_bool_check_box(2);
		return true;
	}
}
//Set the stop thread flag  (static)
void Fpl_Calculation::set_stop_thread_flag(const bool flag){
	Fpl_Calculation::abort_thread_flag=flag;
}
//Check the stop thread flag (static)
void Fpl_Calculation::check_stop_thread_flag(void){
	if(Fpl_Calculation::abort_thread_flag==true){
		Error msg;
		msg.set_msg(_sys_system_modules::FPL_SYS);
		throw msg;
	}
}
//Get the stop thread flag (static)
bool Fpl_Calculation::get_stop_thread_flag(void){
	return Fpl_Calculation::abort_thread_flag;
}
//Run the thread; the type of thread, which will be performed, has to be set before with set_thread_type(_fpl_thread_type type)
void Fpl_Calculation::run(void){
/** \see set_thread_type(_fpl_thread_type type), _fpl_thread_type
*/
	this->thread_is_started=true;
	try{
		switch(this->thread_type){
			//database tables
			case(_fpl_thread_type::fpl_create_tab):
				this->create_fpl_database_tables();
				break;
			case(_fpl_thread_type::fpl_check_tab):
				this->check_fpl_database_tables();
				break;
			case(_fpl_thread_type::fpl_determ_selected):
				this->deterministic_sim();
				break;
			case(_fpl_thread_type::fpl_mc_selected):
				this->monte_carlo_sim();
				break;
			case(_fpl_thread_type::fpl_frc_selected):
				this->fragility_curve_sim();
				break;
			case(_fpl_thread_type::fpl_test_random):
				this->test_random_generator();
				break;
			case(_fpl_thread_type::fpl_del_section):
				this->delete_selected_section_database();
				break;
			case(_fpl_thread_type::fpl_restore_default):
				this->restore_default_database_table();
				break;
			case(_fpl_thread_type::fpl_frc_by_hand):
				this->set_frc_dialog_data();
				break;
			case(_fpl_thread_type::fpl_import_file):
				this->import_file2database();
				break;
			case(_fpl_thread_type::fpl_check_sec_file):
				this->check_section_file();
				break;
			case(_fpl_thread_type::fpl_check_sec_db):
				this->check_section_database();
				break;
			case(_fpl_thread_type::fpl_export_determ_sec):
				this->export_results_determ_section();
				break;
			case(_fpl_thread_type::fpl_export_mc_sec):
				this->export_results_mc_section();
				break;
			case(_fpl_thread_type::fpl_export_frc_sec):
				this->export_results_frc_section();
				break;
			case(_fpl_thread_type::fpl_combine_fpl2hyd):
				this->combine_fpl2hyd_system();
				break;
			case(_fpl_thread_type::fpl_check_ideal_fpl2hyd):
				this->check_ideal_hyd_system_fpl();
				break;

			default:
				Warning msg=this->set_warning(0);
				msg.output_msg(1);
		}
	}
	catch(Error msg){
		this->thread_is_started=false;
		msg.output_msg(1);
	}
	this->thread_is_started=false;
}
//Get the number of errors
int Fpl_Calculation::get_number_error(void){
	return this->number_error;
}
//Set the flag if results should be overwritten
void Fpl_Calculation::set_overwrite_flag(const bool flag){
	this->overwrite_flag=flag;
}
//Idealise the hydraulic system by a given fpl-section
void Fpl_Calculation::idealise_hyd_system_by_section(Fpl_Section *section, Hyd_Hydraulic_System *system){
	this->set_start_warnings_number();
	this->number_section++;

	try{
		section->idealise_hyd_by_section(system);
	}
	catch(Error msg){
		if(Fpl_Calculation::get_stop_thread_flag()==false){
			this->number_error++;
		}
		Sys_Common_Output::output_fpl->rewind_userprefix();
		this->set_warning_number();
		throw msg;
	}
	this->set_warning_number();
}
//Output the final statistic of idealisation
void Fpl_Calculation::output_statistic_idealisation(void){
	ostringstream cout;
	ostringstream prefix;
	prefix<<"STA> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << "Total exception statistics of the FPL-system(s)..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Number fpl-section(s)    :" << W(3) << this->number_section <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)                 :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)               :" << W(3) << this->get_occured_warning() <<endl;

	if(Fpl_Calculation::get_stop_thread_flag()==true){
		cout << " User has aborted the action " << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Get the flag if the thread has started
bool Fpl_Calculation::get_thread_has_started(void){
	return this->thread_is_started;
}
//Import data of an fpl-section but just the probabilistic parameters from file to database (measure change fpl-section parameters)
void Fpl_Calculation::import_file2database_prob_params(const QStringList new_file, const int sec_id_based_on){
	ostringstream cout;
	ostringstream prefix;
	this->set_system_number_file_direct(new_file);

	if(this->number_section==0){
		cout << "No FPL-section file is set for an import to database!" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}
	cout << "Import FPL-section(s) " << this->number_section <<" from file(s) to database..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	prefix << "IMPO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);
	try{
		//start calculation
		for(int i=0; i< this->number_section ; i++){
			try{
				//allocate the section
				this->allocate_fpl_section();
				this->section2calc->set_start_exception_number();
				this->section2calc->check_input_file_section(this->file_names[i]);
				//input per file
				this->section2calc->read_section_per_file(this->file_names[i] ,false, &this->qsqldatabase);
				Fpl_Calculation::check_stop_thread_flag();
				//output to display
				this->section2calc->output_members();
				Fpl_Calculation::check_stop_thread_flag();
				//read base on section
				Fpl_Section base_on_sec;
				QSqlQueryModel results;
				base_on_sec.select_section_database(&results, &this->qsqldatabase, sec_id_based_on);
				base_on_sec.input_section_perdatabase(&results, 0, &this->qsqldatabase, false);
				this->section2calc->overwrite_main_section_parameter(&base_on_sec);
				this->section2calc->output_members();

				//transfer it to database
				this->section2calc->transfer_member2database(&this->qsqldatabase);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}
			catch(Error msg){
				if(Fpl_Calculation::abort_thread_flag==true){
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(1);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}
			this->delete_fpl_section();
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_fpl->rewind_userprefix();
		this->number_error++;
		msg.output_msg(1);
	}

	cout << "Import to database is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);
	this->output_import_statistic();
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Check the idealization of the hydraulic system by the fpl-system by a given river model; it is used in the Alt-System
void Fpl_Calculation::check_ideal_hyd_system_fpl(Hyd_Model_River *river){
	QSqlQueryModel result;
	ostringstream cout;
	ostringstream prefix;

	try{
		this->number_section=Fpl_Section::select_section_database(&result, &this->qsqldatabase, this->system_id, false);

		if(this->number_section==0){
			return;
		}
		cout << "Check of the idealisation of the HYD-river model "<< river->Param_RV.get_river_name()<<" with the FPL-section(s)..." << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);

		for(int i=0; i< this->number_section;i++){
			try{
				this->allocate_fpl_section();
				this->section2calc->set_start_exception_number();
				this->section2calc->input_section_perdatabase(&result, i, &this->qsqldatabase);
				this->section2calc->check_connection_params();
				cout << "Idealise HYD-river model with FPL-section " << this->section2calc->get_name_section()<<"..." << endl;
				Sys_Common_Output::output_fpl->output_txt(&cout);
				prefix << "IDEAL_"<<this->section2calc->get_name_section()<<"> ";
				Sys_Common_Output::output_fpl->set_userprefix(&prefix);
				this->section2calc->idealise_hyd_by_section(river);
			}
			catch(Error msg){
				Sys_Common_Output::output_fpl->rewind_userprefix();
				this->delete_fpl_section();
				throw msg;
			}

			Sys_Common_Output::output_fpl->rewind_userprefix();
			this->delete_fpl_section();
		}
	}
	catch(Error msg){
		throw msg;
	}

	cout << "Check of the idealisation of the HYD-river model "<< river->Param_RV.get_river_name()<<" with the FPL-section(s) is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Set the section id to handle
void Fpl_Calculation::set_section_id(const int id) {
	this->number_section;
}
//_____________
//private
//Allocate the file names
void Fpl_Calculation::allocate_file_names(void){
	try{
		this->file_names=new string[this->number_section];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(4);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the file names
void Fpl_Calculation::delete_file_names(void){
	if(this->file_names!=NULL){
		delete []this->file_names;
		this->file_names=NULL;
	}
}
//Allocate the Monte-Carlo simulation
void Fpl_Calculation::allocate_mc_sim(void){
	this->delete_mc_sim();
	try{
		this->mc_simulation=new Fpl_Mc_Sim;
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the Monte-Carlo simulation
void Fpl_Calculation::delete_mc_sim(void){
	if(this->mc_simulation!=NULL){
		delete this->mc_simulation;
		this->mc_simulation=NULL;
	}
}
//Allocate the fragility curve calculation
void Fpl_Calculation::allocate_frc_sim(void){
	this->delete_frc_sim();
	try{
		this->frc_simulation=new Fpl_Frc_Sim;
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the fragility curve calculation
void Fpl_Calculation::delete_frc_sim(void){
	if(this->frc_simulation!=NULL){
		delete this->frc_simulation;
		this->frc_simulation=NULL;
	}
}
//Allocate the section, which is handled in calculation
void Fpl_Calculation::allocate_fpl_section(void){
	this->delete_fpl_section();
	try{
		this->section2calc=new Fpl_Section;
		this->section2calc->set_systemid(this->system_id);
		this->create_output_folder_name();
		this->section2calc->set_output_folder_name(this->output_folder);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the section, which is handled in calculation
void Fpl_Calculation::delete_fpl_section(void){
	if(this->section2calc!=NULL){
		delete this->section2calc;
		this->section2calc=NULL;
	}
}
//Allocate the class for checking the random generation and the transformation of the random variables
void Fpl_Calculation::allocate_rand_checker(void){
	this->delete_rand_checker();
	try{
		this->random_checker=new Fpl_Check_Distribution;
		this->random2check=new Fpl_Random_Variables;
		random2check->set_name("test_variable");
		this->random_checker->set_ptr_random_variables(this->random2check);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the class for checking the random generation and the transformation of the random variables
void Fpl_Calculation::delete_rand_checker(void){
	if(this->random_checker!=NULL){
		delete this->random_checker;
		this->random_checker=NULL;
		delete this->random2check;;
		this->random2check=NULL;
	}
}
//Allocate the list of the section ids
void Fpl_Calculation::allocate_list_sec_id(void){
	this->delete_list_sec_id();
	try{
		this->list_section_id=new int[this->number_section];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(5);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the list of the section ids
void Fpl_Calculation::delete_list_sec_id(void){
	if(this->list_section_id!=NULL){
		delete []this->list_section_id;
		this->list_section_id=NULL;
	}
}
//Import data of an fpl-section from file to database
void Fpl_Calculation::import_file2database(void){
	ostringstream cout;
	ostringstream prefix;
	if(this->number_section==0){
		cout << "No FPL-section file is set for an import to database!" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}
	cout << "Import FPL-section(s) " << this->number_section <<" from file(s) to database..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	prefix << "IMPO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);
	try{
		//start calculation
		for(int i=0; i< this->number_section ; i++){
			try{
				if(this->file_names[i]==label::not_set){
					continue;
				}
				//allocate the section
				this->allocate_fpl_section();
				this->section2calc->set_start_exception_number();
				this->section2calc->check_input_file_section(this->file_names[i]);
				//input per file
				this->section2calc->read_section_per_file(this->file_names[i] ,false ,&this->qsqldatabase);
				Fpl_Calculation::check_stop_thread_flag();
				//check it with existing sections
				Fpl_Calculation::check_for_identical_section(this->section2calc);

				//output to display
				this->section2calc->set_section_id(&this->qsqldatabase);
				this->section2calc->output_members();
				this->section2calc->output_geometry2tecplot();
				this->section2calc->output_geometry2excel();
				this->section2calc->output_geometry2paraview();
				Fpl_Calculation::check_stop_thread_flag();
				//transfer it to database
				this->section2calc->transfer_member2database(&this->qsqldatabase);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}
			catch(Error msg){
				if(Fpl_Calculation::abort_thread_flag==true){
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(1);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}
			this->delete_fpl_section();
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_fpl->rewind_userprefix();

		this->number_error++;
		msg.output_msg(1);
	}

	cout << "Import to database is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);
	this->output_import_statistic();
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Perform a deterministic simulation for selected sections
void Fpl_Calculation::deterministic_sim(void){
	ostringstream cout;
	if(this->number_section==0){
		cout << "No FPL-section is selected for a deterministc calculation!" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}

	ostringstream prefix;

	QSqlQueryModel db_section;
	int number_found=0;

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);
	try{
		//start calculation
		for(int i=0; i< this->number_section ; i++){
			cout << "Deterministic analysis for FPL-section number " << this->list_section_id[i]<<"..." << endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			prefix << "DET_"<<this->list_section_id[i]<<"> ";
			Sys_Common_Output::output_fpl->set_userprefix(&prefix);

			try{
				number_found=Fpl_Section::select_relevant_section_database(&db_section, &this->qsqldatabase, this->system_id, this->list_section_id[i], false);
				if(number_found==0){
					Warning msg=this->set_warning(1);
					ostringstream info;
					info <<"FPL-section id   : "<< this->list_section_id[i] << endl;
					msg.make_second_info(info.str());
					msg.output_msg(1);
				}
				else{
					//input the section
					this->allocate_fpl_section();
					this->section2calc->set_start_exception_number();
					this->section2calc->input_section_total_perdatabase(&db_section, 0,&this->qsqldatabase, false);
					this->section2calc->delete_detailed_results_database(&this->qsqldatabase, _fpl_simulation_type::sim_determ);
					Sys_Common_Output::output_fpl->rewind_userprefix();
					prefix << "DET_"<<this->section2calc->get_name_section()<<"> ";
					Sys_Common_Output::output_fpl->set_userprefix(&prefix);
					this->section2calc->output_members();
					Fpl_Calculation::check_stop_thread_flag();
					//start calculation
					this->section2calc->make_faulttree(false);
					this->section2calc->check_statistic();
					this->section2calc->output_reliability();
					this->section2calc->output_result2table(&this->qsqldatabase, _fpl_simulation_type::sim_determ, 1);
					Fpl_Calculation::check_stop_thread_flag();
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
				}
			}
			catch(Error msg){
				if(Fpl_Calculation::abort_thread_flag==true){
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
					Sys_Common_Output::output_fpl->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(1);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}
			Sys_Common_Output::output_fpl->rewind_userprefix();
			this->delete_fpl_section();
		}
	}
	catch(Error msg){
		msg.output_msg(1);
	}

	cout << "Deterministic analysis is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);

	this->output_final_statistic_calculation();
}
//Perform a monte-carlo simulation for selected sections
void Fpl_Calculation::monte_carlo_sim(void){
	ostringstream cout;
	if(this->number_section==0){
		cout << "No FPL-section is selected for a monte-carlo calculation!" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}

	this->set_start_warnings_number();
	//set-up the mc-analysis
	try{
		this->allocate_mc_sim();
		this->mc_simulation->set_input(&this->qsqldatabase);
		this->mc_simulation->output_members();
	}
	catch(Error msg){
		this->number_error++;
		ostringstream info;
		info <<"Can not set-up the monte-carlo analysis"<< endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
		this->output_error_statistic();
		return;
	}

	Fpl_Calculation::check_stop_thread_flag();
	ostringstream prefix;
	QSqlQueryModel db_section;
	int number_found=0;
	//begin time recording
	time(&this->start_time);
	try{
		//start calculation
		for(int i=0; i< this->number_section ; i++){
			if(this->overwrite_flag==false){
				//check if there is an result
				QSqlQueryModel model;
				Fpl_Mc_Sim::select_results_in_database(&model, &this->qsqldatabase, this->system_id, this->list_section_id[i], true);
				if(model.rowCount()>0){
					cout << "No monte-carlo analysis for FPL-section number " << this->list_section_id[i]<<" is required. The results are already generated." << endl;
					Sys_Common_Output::output_fpl->output_txt(&cout);
					continue;
				}
			}
			cout << "Monte-carlo analysis for FPL-section number " << this->list_section_id[i]<<"..." << endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			prefix << "MC_"<<this->list_section_id[i]<<"> ";
			Sys_Common_Output::output_fpl->set_userprefix(&prefix);

			try{
				number_found=Fpl_Section::select_relevant_section_database(&db_section, &this->qsqldatabase, this->system_id, this->list_section_id[i], false);
				if(number_found==0){
					Warning msg=this->set_warning(1);
					ostringstream info;
					info <<"FPL-section id   : "<< this->list_section_id[i] << endl;
					msg.make_second_info(info.str());
					msg.output_msg(1);
				}
				else{
					//input the section
					this->allocate_fpl_section();
					this->section2calc->set_start_exception_number();
					this->section2calc->input_section_total_perdatabase(&db_section, 0, &this->qsqldatabase, false);
					Sys_Common_Output::output_fpl->rewind_userprefix();
					prefix << "MC_"<<this->section2calc->get_name_section()<<"> ";
					Sys_Common_Output::output_fpl->set_userprefix(&prefix);
					this->section2calc->output_members();
					Fpl_Calculation::check_stop_thread_flag();
					//start calculation
					this->mc_simulation->make_mc_sim(this->section2calc, &this->qsqldatabase);
					Fpl_Calculation::check_stop_thread_flag();
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
				}
			}
			catch(Error msg){
				if(Fpl_Calculation::abort_thread_flag==true){
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
					Sys_Common_Output::output_fpl->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(1);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}
			Sys_Common_Output::output_fpl->rewind_userprefix();
			this->delete_fpl_section();
		}
	}
	catch(Error msg){
		msg.output_msg(1);
	}
	cout << "Monte-carlo analysis is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);

	this->output_final_statistic_calculation();
}
//Perform a fragility curve simulation for selected sections
void Fpl_Calculation::fragility_curve_sim(void){
	ostringstream cout;
	if(this->number_section==0){
		cout << "No FPL-section is selected for a fragility curve calculation!" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}

	this->set_start_warnings_number();
	//set-up the mc-analysis
	try{
		this->allocate_frc_sim();
		this->frc_simulation->init_frc_calculation();
		this->frc_simulation->set_input(&this->qsqldatabase);
		this->frc_simulation->output_members();
	}
	catch(Error msg){
		this->number_error++;
		ostringstream info;
		info <<"Can not set-up the frc-analysis"<< endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
		this->output_error_statistic();
		return;
	}

	Fpl_Calculation::check_stop_thread_flag();
	ostringstream prefix;
	QSqlQueryModel db_section;
	int number_found=0;
	//begin time recording
	time(&this->start_time);
	try{
		//start calculation
		for(int i=0; i< this->number_section ; i++){
			if(this->overwrite_flag==false){
				//check if there is an result
				QSqlQueryModel model;
				Fpl_Mc_Sim::select_results_in_database(&model, &this->qsqldatabase, this->system_id, this->list_section_id[i], false);
				if(model.rowCount()>0){
					cout << "No fragility curve analysis for FPL-section number " << this->list_section_id[i]<<" is required. The results are already generated." << endl;
					Sys_Common_Output::output_fpl->output_txt(&cout);
					continue;
				}
			}

			cout << "Fragility curve analysis for FPL-section number " << this->list_section_id[i]<<"..." << endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			prefix << "FRC_"<<this->list_section_id[i]<<"> ";
			Sys_Common_Output::output_fpl->set_userprefix(&prefix);

			try{
				number_found=Fpl_Section::select_relevant_section_database(&db_section, &this->qsqldatabase, this->system_id, this->list_section_id[i], false);
				if(number_found==0){
					Warning msg=this->set_warning(1);
					ostringstream info;
					info <<"FPL-section id   : "<< this->list_section_id[i] << endl;
					msg.make_second_info(info.str());
					msg.output_msg(1);
				}
				else{
					//input the section
					this->allocate_fpl_section();
					this->section2calc->set_start_exception_number();
					this->section2calc->input_section_total_perdatabase(&db_section, 0, &this->qsqldatabase,true);
					Sys_Common_Output::output_fpl->rewind_userprefix();
					prefix << "FRC_"<<this->section2calc->get_name_section()<<"> ";
					Sys_Common_Output::output_fpl->set_userprefix(&prefix);
					this->section2calc->output_members();
					Fpl_Calculation::check_stop_thread_flag();
					//start calculation
					this->frc_simulation->make_frc_sim(this->section2calc, &this->qsqldatabase);
					Fpl_Calculation::check_stop_thread_flag();
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
				}
			}
			catch(Error msg){
				Fpl_Mc_Sim::delete_data_in_result_table(&this->qsqldatabase, this->section2calc->get_id_section(), this->system_id, false);
				if(Fpl_Calculation::abort_thread_flag==true){
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
					Sys_Common_Output::output_fpl->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(1);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}

			Sys_Common_Output::output_fpl->rewind_userprefix();
			this->delete_fpl_section();
		}
	}
	catch(Error msg){
		msg.output_msg(1);
	}

	cout << "Fragility curve analysis is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);

	this->output_final_statistic_calculation();
}
//Perform a test of the random generator
void Fpl_Calculation::test_random_generator(void){
	ostringstream cout;

	this->set_start_warnings_number();

	//set-up the mc-analysis
	try{
		this->allocate_mc_sim();
		if(this->qsqldatabase.isOpen()==true){
			try{
				this->mc_simulation->set_input(&this->qsqldatabase);
			}
			catch(Error msg){
				cout << "The default control values for a monte-carlo simulation are taken..."<<endl;
				Sys_Common_Output::output_fpl->output_txt(&cout);
			}
		}
	}
	catch(Error msg){
		this->number_error++;
		ostringstream info;
		info <<"Can not set-up the monte-carlo analysis"<< endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
		this->output_error_statistic();
		this->delete_rand_checker();
		return;
	}

	ostringstream prefix;
	//begin time recording
	time(&this->start_time);

	//start calculation

	cout << "Testing of the random generator and the transformation algorithms..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	prefix << "TEST_RAND> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);

	try{
		//start calculation
		this->mc_simulation->make_mc_test_random(this->random_checker, this->random2check);
	}
	catch(Error msg){
		this->number_error++;
		msg.output_msg(1);
		this->set_warning_number();
		//set the actual time
		time(&this->actual_time);
		return;
	}
	Sys_Common_Output::output_fpl->rewind_userprefix();

	cout << "The random generator test is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);

	this->output_final_statistic_calculation();
}
//Set a fragility curve from dialog data
void Fpl_Calculation::set_frc_dialog_data(void){
	int number_found=0;
	QSqlQueryModel db_section;
	for(int i=0; i< this->number_section ; i++){
		try{
			number_found=Fpl_Section::select_relevant_section_database(&db_section, &this->qsqldatabase, this->system_id, this->list_section_id[i], false);
			if(number_found==0){
				Warning msg=this->set_warning(1);
				ostringstream info;
				info <<"Fpl-section id   : "<< this->list_section_id[i] << endl;
				msg.make_second_info(info.str());
				msg.output_msg(1);
			}
			else{
				//input the section
				this->allocate_fpl_section();
				this->section2calc->input_section_perdatabase(&db_section, 0, &this->qsqldatabase);
				this->section2calc->output_members();
				this->section2calc->set_probabilistic_result_per_dialog(&this->qsqldatabase);
				//make dialog
			}
		}
		catch(Error msg){
				msg.output_msg(1);
		}
		this->delete_fpl_section();
	}
}
//Check fpl-section(s) per file
void Fpl_Calculation::check_section_file(void){
	ostringstream cout;
	ostringstream prefix;
	if(this->number_section==0){
		cout << "No FPL-section file is set for checking!" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}
	cout << "Check FPL-section(s) " << this->number_section <<" from file(s)..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	prefix << "CHECK> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);
	try{
		//start calculation
		for(int i=0; i< this->number_section ; i++){
			try{
				if(this->file_names[i]==label::not_set){
					continue;
				}
				//allocate the section
				this->allocate_fpl_section();
				this->section2calc->check_input_file_section(this->file_names[i]);
				//input per file
				this->section2calc->read_section_per_file(this->file_names[i] ,false, &this->qsqldatabase);
				Fpl_Calculation::check_stop_thread_flag();
				//output to display
				this->section2calc->output_members();
				this->section2calc->output_geometry2excel();
				this->section2calc->output_geometry2paraview();
				Fpl_Calculation::check_stop_thread_flag();
				this->section2calc->output_geometry2tecplot();
			}
			catch(Error msg){
				if(Fpl_Calculation::abort_thread_flag==true){
					throw msg;
				}
				this->number_error++;
				msg.output_msg(1);
			}
			this->delete_fpl_section();
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_fpl->rewind_userprefix();
		this->number_error++;
		msg.output_msg(1);
	}

	cout << "Check of section file(s) is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);
	this->output_import_statistic();
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Check fpl-section(s) per database
void Fpl_Calculation::check_section_database(void){
	ostringstream cout;
	if(this->number_section==0){
		cout << "No FPL-section is selected for a checking!" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}

	ostringstream prefix;

	QSqlQueryModel db_section;
	int number_found=0;

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);
	try{
		//start calculation
		for(int i=0; i< this->number_section ; i++){
			cout << "Check for FPL-section number " << this->list_section_id[i]<<"..." << endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			prefix << "CECK_"<<this->list_section_id[i]<<"> ";
			Sys_Common_Output::output_fpl->set_userprefix(&prefix);

			try{
				number_found=Fpl_Section::select_relevant_section_database(&db_section, &this->qsqldatabase, this->system_id, this->list_section_id[i], false);
				if(number_found==0){
					Warning msg=this->set_warning(1);
					ostringstream info;
					info <<"Fpl-section id   : "<< this->list_section_id[i] << endl;
					msg.make_second_info(info.str());
					msg.output_msg(1);
				}
				else{
					//input the section
					this->allocate_fpl_section();
					this->section2calc->set_start_exception_number();
					this->section2calc->input_section_total_perdatabase(&db_section, 0,&this->qsqldatabase, false);
					Fpl_Calculation::check_stop_thread_flag();
					this->section2calc->output_members();
					Fpl_Calculation::check_stop_thread_flag();
					if(Sys_Project::get_project_type()!=_sys_project_type::proj_fpl || Sys_Project::get_project_type()!=_sys_project_type::proj_fpl_file){
						this->section2calc->check_connection_params();
					}
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
				}
			}
			catch(Error msg){
				if(Fpl_Calculation::abort_thread_flag==true){
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
					Sys_Common_Output::output_fpl->rewind_userprefix();
					throw msg;
				}

				this->number_error++;
				msg.output_msg(1);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}

			Sys_Common_Output::output_fpl->rewind_userprefix();
			this->delete_fpl_section();
		}
	}
	catch(Error msg){
		this->number_error++;
		msg.output_msg(1);
	}

	cout << "Check of FPL-section(s) is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);

	this->output_check_statistic();
}
//Export deterministic results of a fpl-section(s)
void Fpl_Calculation::export_results_determ_section(void) {
	ostringstream cout;
	if (this->number_section == 0) {
		cout << "No FPL-section is selected for export!" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}

	ostringstream prefix;

	QSqlQueryModel db_section;
	int number_found = 0;

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);
	try {
		//start export
		for (int i = 0; i < this->number_section; i++) {
			cout << "Export deterministic results for FPL-section number " << this->list_section_id[i] << "..." << endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			prefix << "EXPORT_" << this->list_section_id[i] << "> ";
			Sys_Common_Output::output_fpl->set_userprefix(&prefix);

			try {
				number_found = Fpl_Section::select_relevant_section_database(&db_section, &this->qsqldatabase, this->system_id, this->list_section_id[i], false);
				if (number_found == 0) {
					Warning msg = this->set_warning(1);
					ostringstream info;
					info << "Fpl-section id   : " << this->list_section_id[i] << endl;
					msg.make_second_info(info.str());
					msg.output_msg(1);
				}
				else {
					//input the section
					this->allocate_fpl_section();
					this->section2calc->set_start_exception_number();
					
					this->section2calc->input_section_total_perdatabase(&db_section, 0, &this->qsqldatabase, false, true);
					this->section2calc->output_determ_results2tecplot(&this->qsqldatabase);
					this->section2calc->output_determ_results2paraview(&this->qsqldatabase);
					this->section2calc->output_determ_results2excel(&this->qsqldatabase);

					Fpl_Calculation::check_stop_thread_flag();
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
				}
			}
			catch (Error msg) {
				if (Fpl_Calculation::abort_thread_flag == true) {
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
					Sys_Common_Output::output_fpl->rewind_userprefix();
					throw msg;
				}

				this->number_error++;
				msg.output_msg(1);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}

			Sys_Common_Output::output_fpl->rewind_userprefix();
			this->delete_fpl_section();
		}
	}
	catch (Error msg) {
		this->number_error++;
		msg.output_msg(1);
	}

	cout << "Export of deterministic results of FPL-section(s) is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);

	this->output_export_statistic();
}
//Export MC-results of a fpl-section(s)
void Fpl_Calculation::export_results_mc_section(void) {
	ostringstream cout;
	if (this->number_section == 0) {
		cout << "No FPL-section is selected for export!" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}

	ostringstream prefix;

	QSqlQueryModel db_section;
	int number_found = 0;

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);
	try {
		//start export
		for (int i = 0; i < this->number_section; i++) {
			cout << "Export MC results for FPL-section number " << this->list_section_id[i] << "..." << endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			prefix << "EXPORT_" << this->list_section_id[i] << "> ";
			Sys_Common_Output::output_fpl->set_userprefix(&prefix);

			try {
				number_found = Fpl_Section::select_relevant_section_database(&db_section, &this->qsqldatabase, this->system_id, this->list_section_id[i], false);
				if (number_found == 0) {
					Warning msg = this->set_warning(1);
					ostringstream info;
					info << "Fpl-section id   : " << this->list_section_id[i] << endl;
					msg.make_second_info(info.str());
					msg.output_msg(1);
				}
				else {
					//input the section
					this->allocate_fpl_section();
					this->section2calc->set_start_exception_number();

					this->section2calc->input_section_total_perdatabase(&db_section, 0, &this->qsqldatabase, false, true);
					this->section2calc->output_mc_results2tecplot(&this->qsqldatabase);
					this->section2calc->output_mc_results2paraview(&this->qsqldatabase);
					this->section2calc->output_mc_results2excel(&this->qsqldatabase);

					Fpl_Calculation::check_stop_thread_flag();
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
				}
			}
			catch (Error msg) {
				if (Fpl_Calculation::abort_thread_flag == true) {
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
					Sys_Common_Output::output_fpl->rewind_userprefix();
					throw msg;
				}

				this->number_error++;
				msg.output_msg(1);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}

			Sys_Common_Output::output_fpl->rewind_userprefix();
			this->delete_fpl_section();
		}
	}
	catch (Error msg) {
		this->number_error++;
		msg.output_msg(1);
	}

	cout << "Export of MC results of FPL-section(s) is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);

	this->output_export_statistic();
}
//Export FRC-results of a fpl-section(s)
void Fpl_Calculation::export_results_frc_section(void) {
	ostringstream cout;
	if (this->number_section == 0) {
		cout << "No FPL-section is selected for export!" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}

	ostringstream prefix;

	QSqlQueryModel db_section;
	int number_found = 0;

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);
	try {
		//start export
		for (int i = 0; i < this->number_section; i++) {
			cout << "Export FRC results for FPL-section number " << this->list_section_id[i] << "..." << endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			prefix << "EXPORT_" << this->list_section_id[i] << "> ";
			Sys_Common_Output::output_fpl->set_userprefix(&prefix);

			try {
				number_found = Fpl_Section::select_relevant_section_database(&db_section, &this->qsqldatabase, this->system_id, this->list_section_id[i], false);
				if (number_found == 0) {
					Warning msg = this->set_warning(1);
					ostringstream info;
					info << "Fpl-section id   : " << this->list_section_id[i] << endl;
					msg.make_second_info(info.str());
					msg.output_msg(1);
				}
				else {
					//input the section
					this->allocate_fpl_section();
					this->section2calc->set_start_exception_number();

					this->section2calc->input_section_total_perdatabase(&db_section, 0, &this->qsqldatabase, false, true);
					this->section2calc->output_frc_results2tecplot(&this->qsqldatabase);
					this->section2calc->output_frc_results2paraview(&this->qsqldatabase);
					this->section2calc->output_frc_results2excel(&this->qsqldatabase);

					Fpl_Calculation::check_stop_thread_flag();
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
				}
			}
			catch (Error msg) {
				if (Fpl_Calculation::abort_thread_flag == true) {
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
					Sys_Common_Output::output_fpl->rewind_userprefix();
					throw msg;
				}

				this->number_error++;
				msg.output_msg(1);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}

			Sys_Common_Output::output_fpl->rewind_userprefix();
			this->delete_fpl_section();
		}
	}
	catch (Error msg) {
		this->number_error++;
		msg.output_msg(1);
	}

	cout << "Export of FRC results of FPL-section(s) is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);

	this->output_export_statistic();

}
//Combine the fpl-system with the hydraulic system
void Fpl_Calculation::combine_fpl2hyd_system(void){
	QSqlQueryModel result;
	ostringstream cout;
	ostringstream prefix;
	emit send_hyd_thread_enable(true);

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);
	try{
		this->number_section=Fpl_Section::select_relevant_section_database(&result, &this->qsqldatabase, this->system_id, false);

		if(this->number_section==0){
			cout << "No FPL-section(s) are set for combining to the HYD-system!" << endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			return;
		}

		//read in the hydraulic river models
		emit send_hyd_thread_runs(true);
		cout << "Combine the FPL-section with the HYD-river models..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		//init the hydraulic system
		cout << "Initialize the HYD-river model(s) for combining..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		Hyd_Hydraulic_System my_hyd;
		my_hyd.set_systemid(this->system_id);
		my_hyd.set_ptr2database(this->qsqldatabase,0);
		my_hyd.set_river_coast_model_per_database(false);
		emit send_hyd_thread_runs(false);
		Sys_Common_Output::output_fpl->set_focus_on_display();
		if(my_hyd.global_parameters.get_number_river_model()==0 && my_hyd.global_parameters.get_coast_model_applied()==false){
			cout << "No hydraulic river-model(s)/coast-model are set for combining with the FPL-section(s)!" << endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			return;
		}

		for(int i=0; i< this->number_section;i++){
			try{
				this->allocate_fpl_section();
				this->section2calc->set_start_exception_number();
				this->section2calc->input_section_perdatabase(&result, i, &this->qsqldatabase);
				this->section2calc->check_connection_params();
				cout << "Combine FPL-section " << this->section2calc->get_name_section()<<" to HYD-system..." << endl;
				Sys_Common_Output::output_fpl->output_txt(&cout);
				prefix << "COMB_"<<this->section2calc->get_name_section()<<"> ";
				Sys_Common_Output::output_fpl->set_userprefix(&prefix);
				this->section2calc->combine_section2hydraulic(&my_hyd, &this->qsqldatabase);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}
			catch(Error msg){
				if(Fpl_Calculation::abort_thread_flag==true){
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
					Sys_Common_Output::output_fpl->rewind_userprefix();
					throw msg;
				}

				this->number_error++;
				msg.output_msg(1);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}

			Sys_Common_Output::output_fpl->rewind_userprefix();
			this->delete_fpl_section();
		}
	}
	catch(Error msg){
		this->number_error++;
		msg.output_msg(1);
	}

	cout << "Combining of FPL-section(s) to the HYD-system is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);

	this->output_check_statistic();

	emit send_hyd_thread_enable(false);
	emit send_hyd_thread_runs(false);
}
//Check the idealization of the hydraulic system by the fpl-system
void Fpl_Calculation::check_ideal_hyd_system_fpl(void){
	QSqlQueryModel result;
	ostringstream cout;
	ostringstream prefix;
	emit send_hyd_thread_enable(true);

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);
	try{
		this->number_section=Fpl_Section::select_relevant_section_database(&result, &this->qsqldatabase, this->system_id, false);

		if(this->number_section==0){
			cout << "No FPL-section is set for an idealisation!" << endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			return;
		}

		//read in the hydraulic river models
		emit send_hyd_thread_runs(true);
		cout << "Check the idealisation of the HYD-system with the FPL-section(s)..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		//init the hydraulic system
		cout << "Initialize the HYD-river models for idealisation..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		Hyd_Hydraulic_System my_hyd;
		my_hyd.set_systemid(this->system_id);
		my_hyd.set_ptr2database(this->qsqldatabase,0);
		my_hyd.set_river_coast_model_per_database(false);
		emit send_hyd_thread_runs(false);
		Sys_Common_Output::output_fpl->set_focus_on_display();
		if(my_hyd.global_parameters.get_number_river_model()==0 && my_hyd.global_parameters.get_coast_model_applied()==false){
			cout << "No hydraulic river-model(s)/coast-model are set for idealisation with the FPL-section(s)!" << endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			return;
		}

		for(int i=0; i< this->number_section;i++){
			try{
				this->allocate_fpl_section();
				this->section2calc->set_start_exception_number();
				this->section2calc->input_section_perdatabase(&result, i, &this->qsqldatabase);
				this->section2calc->check_connection_params();
				cout << "Idealise HYD-system with FPL-section " << this->section2calc->get_name_section()<<"..." << endl;
				Sys_Common_Output::output_fpl->output_txt(&cout);
				prefix << "IDEAL_"<<this->section2calc->get_name_section()<<"> ";
				Sys_Common_Output::output_fpl->set_userprefix(&prefix);
				this->section2calc->idealise_hyd_by_section(&my_hyd);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}
			catch(Error msg){
				if(Fpl_Calculation::abort_thread_flag==true){
					this->section2calc->set_exception_number();
					this->section2calc->output_section_statistics();
					Sys_Common_Output::output_fpl->rewind_userprefix();
					throw msg;
				}

				this->number_error++;
				msg.output_msg(1);
				this->section2calc->set_exception_number();
				this->section2calc->output_section_statistics();
			}

			Sys_Common_Output::output_fpl->rewind_userprefix();
			this->delete_fpl_section();
		}
	}
	catch(Error msg){
		msg.output_msg(1);
	}

	cout << "Check of the idealisation of the HYD-system with the FPL-section(s) is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);

	this->output_check_statistic();

	emit send_hyd_thread_enable(false);
	emit send_hyd_thread_runs(false);
}
//Delete selected setcion in database table
void Fpl_Calculation::delete_selected_section_database(void){
	ostringstream cout;
	try{
		//start calculation
		for(int i=0; i< this->number_section ; i++){
			cout << "Delete fpl-section number " << this->list_section_id[i]<<" in database..." << endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);

			try{
				Fpl_Section::delete_data_in_table(&this->qsqldatabase, this->system_id, this->list_section_id[i]);
			}
			catch(Error msg){
				if(Fpl_Calculation::abort_thread_flag==true){
					Sys_Common_Output::output_fpl->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(1);
			}
		}
	}
	catch(Error msg){
		this->number_error++;
		msg.output_msg(1);
	}
}
//Restore default values in database table
void Fpl_Calculation::restore_default_database_table(void){
	if(this->flag_restore_default==true){
		Fpl_Distribution_Class_Mean::delete_data_in_default_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Triangle::delete_data_in_default_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Discret::delete_data_in_default_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Discret_Mean::delete_data_in_default_table(&this->qsqldatabase);
		Fpl_Distribution_Class_Mean_Mean::delete_data_in_default_table(&this->qsqldatabase);
	}
	if(this->flag_restore_prob_param==true){
		Fpl_Mc_Sim::reset_data_in_table(&this->qsqldatabase);
	}
	if(this->flag_restore_fault_tree==true){
		_Fpl_Section_Type::reset_data_in_table(&this->qsqldatabase);
	}
}
//Set the number of warnings before an action
void Fpl_Calculation::set_start_warnings_number(void){
	this->number_warning_begin=Warning::get_number_fpl_warnings();
}
//Set the warning number, which occured during an action
void Fpl_Calculation::set_warning_number(void){
	this->number_warning=this->number_warning+Warning::get_number_fpl_warnings()-this->number_warning_begin;
}
//Get the warning number, which occurred during an action
int Fpl_Calculation::get_occured_warning(void){
	return this->number_warning;
}
//Output the error statistic of the fpl system
void Fpl_Calculation::output_error_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << "Error statistics of the FPL-system..." << endl ;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)                 :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)               :" << W(3) << this->get_occured_warning() <<endl;

	if(Fpl_Calculation::get_stop_thread_flag()==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the error statistic of the fpl system import from file to database
void Fpl_Calculation::output_import_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << "Error statistics of the FPL-system file(s)..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Number fpl-section(s)    :" << W(3) << this->number_section <<endl;
	cout << " Time                     :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)                 :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)               :" << W(3) << this->get_occured_warning() <<endl;

	if(Fpl_Calculation::get_stop_thread_flag()==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the error statistic of the fpl system check from database
void Fpl_Calculation::output_check_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << "Total exception statistics of the FPL-system(s)..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Number fpl-section(s)    :" << W(3) << this->number_section <<endl;
	cout << " Time                     :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)                 :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)               :" << W(3) << this->get_occured_warning() <<endl;

	if(Fpl_Calculation::get_stop_thread_flag()==true){
		cout << " User has aborted the action " << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the export statistic of the fpl system check from database
void Fpl_Calculation::output_export_statistic(void) {
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << "Total exception statistics of the FPL-system(s)..." << endl;
	cout << "GENERAL" << endl;
	cout << " Number fpl-section(s)    :" << W(3) << this->number_section << endl;
	cout << " Time                     :" << "  " << functions::convert_seconds2string(this->actual_time - this->start_time) << label::time_unit_output << endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER" << endl;
	cout << " Error(s)                 :" << W(3) << this->number_error << endl;
	cout << " Warning(s)               :" << W(3) << this->get_occured_warning() << endl;

	if (Fpl_Calculation::get_stop_thread_flag() == true) {
		cout << " User has aborted the action " << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_fpl->rewind_userprefix();

}
//Output the statistic of the fpl-calculation
void Fpl_Calculation::output_final_statistic_calculation(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << "Statistics of the fpl-calculation..." << endl ;
	cout << "GENERAL"<< endl;
	if(this->number_section!=0){
		cout << " Number fpl-section(s):" << W(3) << this->number_section <<endl;
	}
	cout << " Time                 :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)             :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)           :" << W(3) << this->number_warning <<endl;
	if(Fpl_Calculation::abort_thread_flag==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Check for identical section in case of a new input per file
void Fpl_Calculation::check_for_identical_section(Fpl_Section *new_sec){
	ostringstream cout;
	cout << "Check for identical FPL-section in database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	int number=0;
	QSqlQueryModel model;
	number=Fpl_Section::select_relevant_section_database(&model, &this->qsqldatabase, this->system_id, false);

	Fpl_Section buffer;
	buffer.set_systemid(this->system_id);

	for(int i=0; i< number; i++){
		buffer.input_section_perdatabase(&model, i, &this->qsqldatabase);
		buffer.compare_geometrical_settings(new_sec);
	}
}
//Create the name of the output folder
void Fpl_Calculation::create_output_folder_name(void){
	if(this->output_folder==label::not_set){
		ostringstream buffer;

		QDir my_dir;
		buffer<<Sys_Project::get_main_path()<<sys_label::folder_outputfile<<"/";
		if(my_dir.exists(buffer.str().c_str())==false){
			my_dir.mkdir(buffer.str().c_str());
		}
		//add modul
		buffer<<functions::convert_system_module2txt(_sys_system_modules::FPL_SYS)<<"/";
		if(my_dir.exists(buffer.str().c_str())==false){
			my_dir.mkdir(buffer.str().c_str());
		}

		this->output_folder=buffer.str();
	}
}
//Set the warning
Warning Fpl_Calculation::set_warning(const int warn_type){
	string place="Fpl_Calculation::";
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
			break;
		case 1://no section is found
			place.append("deterministic_sim()/monte_carlo_sim()/fragilty_curve_sim()") ;
			reason="No fpl-section is found for calculation";
			reaction="No calculation is performed";
			help= "Check the fpl-section id in the database table";
			type=8;
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
//set the error
Error Fpl_Calculation::set_error(const int err_type){
		string place="Fpl_Calculation::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_mc_sim(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("allocate_frc_sim(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://bad alloc
			place.append("allocate_fpl_section(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://bad alloc
			place.append("allocate_rand_checker(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 4://bad alloc
			place.append("allocate_file_names(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 5://bad alloc
			place.append("allocate_list_sec_id(void)");
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