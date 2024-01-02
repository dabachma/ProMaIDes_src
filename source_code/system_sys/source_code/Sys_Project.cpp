#include "Sys_Headers_Precompiled.h"
//#include "Sys_Project.h"

//init static members
string Sys_Project::main_path=QDir::currentPath().toStdString();
string Sys_Project::current_path=QDir::currentPath().toStdString();
string Sys_Project::project_name=label::not_set;
string Sys_Project::version="ProMaIDes_0_11_vc";
string Sys_Project::version_date="19.12.2023";
_sys_project_type Sys_Project::project_type=_sys_project_type::proj_all;
bool Sys_Project::save_logfile=true;

//Default constructor
Sys_Project::Sys_Project(void){

	this->project_file_name=label::not_set;
	this->author_name=label::not_set;
	this->description=label::not_set;
	this->file_path=label::not_set;
	this->create_new_dir=true;
	this->database_conn_params.host_name="";
	this->database_conn_params.database_name="";
	this->database_conn_params.user_name="";
	this->database_conn_params.password="";
	this->database_conn_params.driver_name="";
	this->database_conn_params.password_saved=true;
	this->database_conn_params.driver_type=_sys_driver_type::UNKNOWN_DR;
	this->generation_time=label::not_set;

	this->name_changed_once=false;

	this->counter_name_change=1;

	this->risk_state_flag.reliability_dam_result=false;
	this->risk_state_flag.reliability_fpl_result=false;
	this->risk_state_flag.reliability_hyd_result=false;
	this->risk_state_flag.risk_state_flag=false;
	this->risk_state_flag.catchment_risk_applied=true;
	this->risk_state_flag.nobreak_risk_applied=true;

	this->hyd_state.file_output_required=true;
	this->hyd_state.number_threads=3;

	this->system_id.measure_nr=0;
	this->system_id.area_state=0;

	this->thread_pro_copy=NULL;


	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Project)), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Project::~Sys_Project(void){

	this->delete_pro_copy_thread();
	//count the memory
	Sys_Memory_Count::self()->minus_mem((sizeof(Sys_Project)), _sys_system_modules::SYS_SYS);
}
//____________
//public
//Ask and load an existing project
bool Sys_Project::ask_existing_project(QWidget *parent){

	QString file;

	file=QFileDialog::getOpenFileName(parent, "Open existing project",QDir::currentPath(),tr("Project File (*.prm)")) ;
	this->project_file_name=file.toStdString();


	if(this->project_file_name.empty()==true){
		this->project_file_name=label::not_set;

		return false;
	}
	else{
		string buff1;
		QDir mydir;
		buff1=mydir.absoluteFilePath(this->project_file_name.c_str()).toStdString();
		buff1=functions::get_file_path(buff1);
		Sys_Project::set_main_path(buff1);
		return true;
	}	
}
//Set the project file directly
bool Sys_Project::set_project_file(const string filename){
	this->project_file_name=filename;
	int pos=-1;
	pos=this->project_file_name.find(".prm");
	if(pos<0){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"This is no valid ProMaIDes project file."<< endl;
		txt<<"It has to end with .prm"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return false;
	}
	QDir mydir;
	string buff1;
	buff1=mydir.absoluteFilePath(this->project_file_name.c_str()).toStdString();
	buff1=functions::get_file_path(buff1);
	Sys_Project::set_main_path(buff1);
	return true;
}
//Build a new project
bool Sys_Project::build_new_project(QWidget *parent){
	Sys_New_Project_Dia my_dia(parent);
	this->create_new_dir=true;

	if(my_dia.start_dialog()==true){
		emit send_txt2statusbar("Create new project...", 0);
		//read out the data from dialog
		Sys_Project::project_name=my_dia.get_project_name();
		this->author_name=my_dia.get_author_name();
		this->description=my_dia.get_description();
		Sys_Project::save_logfile=my_dia.get_logfile_save_flag();
		this->remove_endl_description();
		Sys_Project::project_type=my_dia.get_project_type();
		this->file_path=my_dia.get_file_path();
		
		this->create_new_dir=my_dia.get_directory_create_flag();
		if(Sys_Project::project_type != _sys_project_type::proj_hyd_file){
			this->database_conn_params.host_name=my_dia.get_database_connection().host_name;
			this->database_conn_params.database_name=my_dia.get_database_connection().database_name;
			this->database_conn_params.user_name=my_dia.get_database_connection().user_name;
			this->database_conn_params.password=my_dia.get_database_connection().password;
			this->database_conn_params.driver_name=my_dia.get_database_connection().driver_name;
			this->database_conn_params.driver_type=my_dia.get_database_connection().driver_type;
			this->database_conn_params.password_saved=my_dia.get_database_connection().password_saved;
		}
		//create the project
		this->create_new_project(false);
		return true;
	}
	else{
		return false;
	}
}
//Get the project file name
string Sys_Project::get_project_file_name(void){
	return this->project_file_name;
}
//Get complete project file name with path and suffix
string Sys_Project::get_complete_project_file_name(void){
	ostringstream buffer;
	if(this->create_new_dir==true){
		buffer << this->get_complete_project_folder_name()<<"/"<<Sys_Project::project_name<<".prm";
	}
	else{
		buffer << this->file_path <<"/"<<Sys_Project::project_name<<".prm";
	}
	return buffer.str();
}
//Get the project type (static)
_sys_project_type Sys_Project::get_project_type(void){
	return Sys_Project::project_type;
}
//Get the main path of the project (static)
string Sys_Project::get_main_path(void){
	return Sys_Project::main_path;
}
//Set the main path of the project (static)
void Sys_Project::set_main_path(const string main_path){
	Sys_Project::main_path=main_path;
}
//Get the database connection paramater
_sys_database_params Sys_Project::get_database_connection_param(void){
	return this->database_conn_params;
}
//Get complete project name used as schemata name on psql (static)
string Sys_Project::get_complete_project_database_schemata_name(void){
	ostringstream buffer;
	buffer<<Sys_Project::project_name<<"_prm";
	return buffer.str();	
}
//Get complete project name used a prefix for the database table names (static)
string Sys_Project::get_complete_project_database_table_prefix_name(void){
	ostringstream buffer;
	buffer<<Sys_Project::project_name<<"$";
	return buffer.str();
	
}
//Get version number (static)
string Sys_Project::get_version_number(void){
    return Sys_Project::version;
}
//Get version date (static)
string Sys_Project::get_version_date(void){
    return Sys_Project::version_date;
}
//Get the current path (static)
string Sys_Project::get_current_path(void){
	return Sys_Project::current_path;
}
//Set the current path (static)
void Sys_Project::set_current_path(const string path){
	Sys_Project::current_path=path;
}
///Get the project name (static)
string Sys_Project::get_project_name(void){
	return Sys_Project::project_name;
}
//Convert string to the project type (_sys_project_type) (static)
_sys_project_type Sys_Project::convert_txt2project_type(const string txt){
	_sys_project_type type;
	string buffer;
	
	buffer=functions::convert_string2lupper_case(txt);

	#ifdef proj_all_license
		if(txt==sys_label::proj_typ_all){
			type=_sys_project_type::proj_all;
		}
		else if(txt==sys_label::str_hyd){
			type=_sys_project_type::proj_hyd;
		}
		else if(txt==sys_label::str_risk){
			type=_sys_project_type::proj_risk;
		}
		else if(txt==sys_label::proj_typ_dam_hyd){
			type=_sys_project_type::proj_dam_hyd;
		}
		else if(txt==sys_label::str_fpl){
			type=_sys_project_type::proj_fpl;
		}
		else if(txt==sys_label::str_dam){
			type=_sys_project_type::proj_dam;
		}
		else if(txt==sys_label::proj_typ_hyd_file){
			type=_sys_project_type::proj_hyd_file;
		}
		else if(txt==sys_label::proj_typ_fpl_file){
			type=_sys_project_type::proj_fpl_file;
		}
		else if (txt == sys_label::proj_typ_hydrol) {
			type = _sys_project_type::proj_hydrol;
		}
		else if (txt == sys_label::proj_typ_hyd_temp) {
			type = _sys_project_type::proj_hyd_temp;
		}
		else{
			type=_sys_project_type::proj_not;
		}
	#endif
	#ifdef proj_risk_license
		if(txt==sys_label::str_hyd){
			type=_sys_project_type::proj_hyd;
		}
		else if(txt==sys_label::str_risk){
			type=_sys_project_type::proj_risk;
		}
		else if(txt==sys_label::proj_typ_dam_hyd){
			type=_sys_project_type::proj_dam_hyd;
		}
		else if(txt==sys_label::str_fpl){
			type=_sys_project_type::proj_fpl;
		}
		else if(txt==sys_label::str_dam){
			type=_sys_project_type::proj_dam;
		}
		else if(txt==sys_label::proj_typ_hyd_file){
			type=_sys_project_type::proj_hyd_file;
		}
		else if(txt==sys_label::proj_typ_fpl_file){
			type=_sys_project_type::proj_fpl_file;
		}
		else if (txt == sys_label::proj_typ_hydrol) {
			type = _sys_project_type::proj_hydrol;
		}
		else{
			type=_sys_project_type::proj_not;
		}
	#endif
	#ifdef proj_dam_hyd_license
		if(txt==sys_label::str_hyd){
			type=_sys_project_type::proj_hyd;
		}
		else if(txt==sys_label::proj_typ_dam_hyd){
			type=_sys_project_type::proj_dam_hyd;
		}
		else if(txt==sys_label::str_dam){
			type=_sys_project_type::proj_dam;
		}
		else if(txt==sys_label::proj_typ_hyd_file){
			type=_sys_project_type::proj_hyd_file;
		}
		else{
			type=_sys_project_type::proj_not;
		}
	#endif
	#ifdef proj_hyd_license
		if(txt==sys_label::str_hyd){
			type=_sys_project_type::proj_hyd;
		}
		else if(txt==sys_label::proj_typ_hyd_file){
			type=_sys_project_type::proj_hyd_file;
		}
		else if (txt == sys_label::proj_typ_hyd_temp) {
			type = _sys_project_type::proj_hyd_temp;
		}
		else{
			type=_sys_project_type::proj_not;
		}
	#endif
	#ifdef proj_dam_license
		if(txt==sys_label::str_dam){
			type=_sys_project_type::proj_dam;
		}
		else{
			type=_sys_project_type::proj_not;
		}
	#endif
	#ifdef proj_fpl_license
		if(txt==sys_label::str_fpl){
			type=_sys_project_type::proj_fpl;
		}
		else if(txt==sys_label::proj_typ_fpl_file){
			type=_sys_project_type::proj_fpl_file;
		}
		else{
			type=_sys_project_type::proj_not;
		}
	#endif
	#ifdef proj_hydrol_license
			if (txt == sys_label::proj_typ_hydrol) {
				type = _sys_project_type::proj_hydrol;
			}
			else {
				type = _sys_project_type::proj_not;
			}
	#endif
				


	return type;
}
//Convert project type (_sys_project_type) to string (static) 
string Sys_Project::convert_project_type2txt(const _sys_project_type type){
	string buffer;
	switch(type){
		case _sys_project_type::proj_all:
			buffer=sys_label::proj_typ_all;
			break;
		case _sys_project_type::proj_hyd:
			buffer=sys_label::str_hyd;
			break;
		case _sys_project_type::proj_risk:
			buffer=sys_label::str_risk;
			break;
		case _sys_project_type::proj_dam_hyd:
			buffer=sys_label::proj_typ_dam_hyd;
			break;
		case _sys_project_type::proj_fpl:
			buffer=sys_label::str_fpl;
			break;
		case _sys_project_type::proj_dam:
			buffer=sys_label::str_dam;
			break;
		case _sys_project_type::proj_hyd_file:
			buffer=sys_label::proj_typ_hyd_file;
			break;
		case _sys_project_type::proj_fpl_file:
			buffer=sys_label::proj_typ_fpl_file;
			break;
		case _sys_project_type::proj_hydrol:
			buffer = sys_label::proj_typ_hydrol;
			break;
		case _sys_project_type::proj_hyd_temp:
			buffer = sys_label::proj_typ_hyd_temp;
			break;
		default:
			buffer=label::not_defined;
	}
	return buffer;
}
//Get the falg if the logfile should be saved (static)
bool Sys_Project::get_logfile_save_flag(void){
	return Sys_Project::save_logfile;
}
//Set the flag if the logfile should be saved (static)
void Sys_Project::set_logfile_save_flag(const bool flag){
	Sys_Project::save_logfile=flag;
}
//Read an existing project in
void Sys_Project::read_existing_project(void){
	emit send_txt2statusbar("Read project...", 0);
	int line_counter=0;
	ifstream ifile;
	string myline;
	int must_found_counter=0;

	//read in damage function
	//open file
	ifile.open(this->project_file_name.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		emit send_txt2statusbar("Error", 0);
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Filename: " << this->project_file_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	QDir my_dir;
	
	Sys_Project::set_main_path(functions::get_file_path(my_dir.absoluteFilePath(this->project_file_name.c_str()).toStdString()));
	//change the logfile to the directory
	Sys_Common_Output::output_system->switch_logfile_path(Sys_Project::main_path,true);
	Sys_Common_Output::output_fpl->switch_logfile_path(Sys_Project::main_path,true);
	Sys_Common_Output::output_excep->switch_logfile_path(Sys_Project::main_path,true);
	Sys_Common_Output::output_hyd->switch_logfile_path(Sys_Project::main_path,true);
	Sys_Common_Output::output_madm->switch_logfile_path(Sys_Project::main_path,true);
	Sys_Common_Output::output_dam->switch_logfile_path(Sys_Project::main_path,true);
	Sys_Common_Output::output_risk->switch_logfile_path(Sys_Project::main_path,true);
	Sys_Common_Output::output_alt->switch_logfile_path(Sys_Project::main_path,true);
	Sys_Common_Output::output_cost->switch_logfile_path(Sys_Project::main_path,true);
	//..introduce further modules

	try{
		//read in project parameter
		do{
			getline(ifile, myline,'\n');
			line_counter++;
			functions::clean_string(&myline);

			this->find_key_values_file(myline, &must_found_counter);

		}
		while(ifile.eof()==false);
	}
	catch(Error msg){
		ostringstream info;
		info << "Error occurs near line : " << line_counter << endl;
		info << "File name              : " << this->project_file_name.c_str() << endl;
		msg.make_second_info(info.str());
		ifile.close();
		throw msg;
	}

	int must_found=0;

	if(this->project_type==_sys_project_type::proj_all){
		must_found=25;
	}
	else if(this->project_type==_sys_project_type::proj_risk){
		must_found=22;
	}
	else if(this->project_type==_sys_project_type::proj_dam_hyd){
		must_found=14;
	}
	else if(this->project_type==_sys_project_type::proj_hyd_file || this->project_type==_sys_project_type::proj_hyd || this->project_type == _sys_project_type::proj_hyd_temp){
		must_found=13;
	}
	else if(this->project_type==_sys_project_type::proj_fpl || this->project_type==_sys_project_type::proj_dam){
		must_found=11;
	}

	if(must_found_counter!=must_found){
		Error msg=this->set_error(4);
		ostringstream info;
		info << "File name              : " << this->project_file_name.c_str() << endl;
		msg.make_second_info(info.str());
		ifile.close();
		throw msg;
	}

	this->file_path=Sys_Project::main_path;

	ifile.close();
	emit send_txt2statusbar("Ready", 0);

}
//Close and reset the project
void Sys_Project::close_project(void){
	emit send_txt2statusbar("Close project...", 0);
	if(Sys_Project::save_logfile==true){
		//change the logfile to the directory
		Sys_Common_Output::output_system->save_logfile2archiv();
		Sys_Common_Output::output_fpl->save_logfile2archiv();
		Sys_Common_Output::output_excep->save_logfile2archiv();
		Sys_Common_Output::output_hyd->save_logfile2archiv();
		Sys_Common_Output::output_madm->save_logfile2archiv();
		Sys_Common_Output::output_dam->save_logfile2archiv();
		Sys_Common_Output::output_alt->save_logfile2archiv();
		Sys_Common_Output::output_cost->save_logfile2archiv();
		Sys_Common_Output::output_risk->save_logfile2archiv();
		//...introduce further modules
	}


	Sys_Project::project_name=label::not_set;
	Sys_Project::save_logfile=true;
	Sys_Project::main_path=QDir::currentPath().toStdString();
	Sys_Project::project_type=_sys_project_type::proj_all;
	Sys_Project::current_path=QDir::currentPath().toStdString();

	//reset parameter
	this->project_file_name=label::not_set;
	this->author_name=label::not_set;
	this->description=label::not_set;
	this->file_path=label::not_set;
	this->create_new_dir=true;
	this->database_conn_params.host_name="";
	this->database_conn_params.database_name="";
	this->database_conn_params.user_name="";
	this->database_conn_params.password="";
	this->database_conn_params.driver_name="";
	this->database_conn_params.driver_type=_sys_driver_type::UNKNOWN_DR;
	this->generation_time=label::not_set;

	this->name_changed_once=false;

	this->counter_name_change=1;

	this->risk_state_flag.reliability_dam_result=false;
	this->risk_state_flag.reliability_fpl_result=false;
	this->risk_state_flag.reliability_hyd_result=false;
	this->risk_state_flag.risk_state_flag=false;

	this->hyd_state.file_output_required=true;
	this->hyd_state.number_threads=3;

	this->system_id.measure_nr=0;
	this->system_id.area_state=0;

	emit send_txt2statusbar("Ready", 0);

}
//Output the project data to display/console
void Sys_Project::output_project_param(void){
	Sys_Common_Output::output_system->reset_prefix_was_outputed();
	ostringstream cout;
	string buff;
	cout << "GENERAL PROJECT"<< endl;
	cout<<  " Name                  : " << Sys_Project::project_name<<endl;
	cout << " Author                : " << this->author_name<< endl;
	Sys_Common_Output::output_system->output_txt(&cout,false);
	cout << " Time                  : " << this->generation_time <<endl;;
	Sys_Common_Output::output_system->output_txt(&cout,false);
	cout << " Project type          : " << Sys_Project::convert_project_type2txt(Sys_Project::project_type) << endl;
	Sys_Common_Output::output_system->output_txt(&cout,false);
	buff =this->insert_endl2descritpion();
	cout << " Description           : " << buff  << endl;
	Sys_Common_Output::output_system->output_txt(&cout,false);
	cout << " Current path          : " << Sys_Project::current_path << endl;
	Sys_Common_Output::output_system->output_txt(&cout,false);
	if(this->project_type==_sys_project_type::proj_all || this->project_type==_sys_project_type::proj_risk ||
		this->project_type==_sys_project_type::proj_dam_hyd || this->project_type==_sys_project_type::proj_hyd){
		cout<<"HYDRAULIC SETTINGS"<<endl;
		cout<< " Fileoutput database    : " << functions::convert_boolean2string(this->hyd_state.file_output_required) <<endl;
		cout<< " Number threads         : " << this->hyd_state.number_threads <<endl;
	}
	if(this->project_type==_sys_project_type::proj_all || this->project_type==_sys_project_type::proj_risk){
		cout <<"RISK SETTINGS"<<endl;
		cout<< " Risk state             : " << functions::convert_boolean2string(this->risk_state_flag.risk_state_flag) <<endl;
		cout<< " Reliability FPL-result : " << functions::convert_boolean2string(this->risk_state_flag.reliability_fpl_result) <<endl;
		cout<< " Reliability HYD-result : " << functions::convert_boolean2string(this->risk_state_flag.reliability_hyd_result) <<endl;
		cout<< " Reliability DAM-result : " << functions::convert_boolean2string(this->risk_state_flag.reliability_dam_result) <<endl;
	}
	



}
//Get the title for the main window
string Sys_Project::get_main_window_title(void){
	ostringstream buffer;

	if(Sys_Project::project_name==label::not_set){
        buffer << Sys_Project::version;
	}
	else{
		
        buffer << Sys_Project::project_name << " - " <<Sys_Project::version;
	}

	return buffer.str();
}
//Save the project parameter to file
void Sys_Project::save_project_paramater(void){
	emit send_txt2statusbar("Save project...", 0);
	if(Sys_Project::project_name==label::not_set){
		return;
	}
	ifstream ifile;
	string myline;
	string table_info;
	if(this->project_type!=_sys_project_type::proj_hyd_file){
		//open file for reading
		ifile.open(this->project_file_name.c_str(), ios_base::in);
		if(ifile.is_open()==false){
			Error msg=this->set_error(5);
			ostringstream info;
			info << "Filename: " << this->project_file_name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		
		bool table_begin=false;
		int pos=0;
		//read in tables info
		do{
			getline(ifile, myline,'\n');
			if(table_begin==false){
				pos=myline.find(sys_label::table_begin);
				if(pos>=0){
					table_begin=true;
				}
			}
			if(table_begin==true){
				table_info.append(myline);
				table_info.append("\n");
			}

		}
		while(ifile.eof()==false);
		ifile.close();

		if(table_info.find_last_of("\n")==table_info.length()-1){
			table_info.erase(table_info.length()-1);
		}
	}

	ofstream ofile;
	//open for writing
	ofile.open(this->project_file_name.c_str(), ios_base::out);
	if(ofile.is_open()==false){
		Error msg=this->set_error(5);
		ostringstream info;
		info << "Filename: " << this->project_file_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	

	ostringstream txt;
	ofile << this->generate_project_txt2file();
	ofile.flush();

	if(this->project_type!=_sys_project_type::proj_hyd_file){
		//database connection data
		txt<<"#Database connection data"<<endl;
		ofile<< txt.str();
		ofile.flush();
		txt.str("");

		Data_Base my_database(this->database_conn_params);
		my_database.output_database_connection2file(&ofile);

		txt<<endl<<endl;
		ofile << txt.str();
		ofile.flush();
		txt.str("");

		ofile << table_info;
		ofile.flush();
		//close file
		ofile.close();
	}
	else{
		//close file
		ofile.close();
	}

	emit send_txt2statusbar("Ready", 0);
}
//Edit project parameter
bool Sys_Project::edit_project_parameter(QWidget *parent, bool *is_upgrade){
	
	*is_upgrade=false;

	Sys_New_Project_Dia my_dia(parent);
	my_dia.set_for_editing(Sys_Project::project_name, this->author_name, this->description, Sys_Project::project_type, this->database_conn_params, Sys_Project::save_logfile);
	if(my_dia.start_dialog()==true){
		_sys_project_type old_type=Sys_Project::project_type;
		this->author_name=my_dia.get_author_name();
		this->description=my_dia.get_description();
		Sys_Project::save_logfile=my_dia.get_logfile_save_flag();
		Sys_Project::project_type=my_dia.get_project_type();
		this->remove_endl_description();
		this->database_conn_params.host_name=my_dia.get_database_connection().host_name;
		this->database_conn_params.database_name=my_dia.get_database_connection().database_name;
		this->database_conn_params.user_name=my_dia.get_database_connection().user_name;
		this->database_conn_params.password=my_dia.get_database_connection().password;
		this->database_conn_params.driver_name=my_dia.get_database_connection().driver_name;
		this->database_conn_params.driver_type=my_dia.get_database_connection().driver_type;
		this->save_project_paramater();
		if(old_type!=Sys_Project::project_type){
			*is_upgrade=true;
			ostringstream cout;
			cout << "The project is upgraded from type " << Sys_Project::convert_project_type2txt(old_type)<< " to type "<< Sys_Project::convert_project_type2txt(Sys_Project::project_type) <<endl;
			Sys_Common_Output::output_system->output_txt(&cout,false);
			this->create_project_folder_data();
			this->create_project_folder_output();
		}
		return true;
	}
	else{
		return false;
	}
}
//Set the database connection parameter
void Sys_Project::set_database_connection_param(_sys_database_params conn_param){
	this->database_conn_params.host_name=conn_param.host_name;
	this->database_conn_params.database_name=conn_param.database_name;
	this->database_conn_params.user_name=conn_param.user_name;
	this->database_conn_params.password=conn_param.password;
	this->database_conn_params.driver_name=conn_param.driver_name;
	this->database_conn_params.driver_type=conn_param.driver_type;
	this->database_conn_params.password_saved=conn_param.password_saved;
}
//Close and delete the project
void Sys_Project::close_delete_project(QSqlDatabase *ptr_database){
	emit send_txt2statusbar("Delete project...", 0);
	ofstream ifile;
	ifile.open(this->get_project_file_name().c_str());
	if(ifile.is_open()==true){
		ifile.close();
	}
	//remove project file
	QDir my_dir;
	my_dir.remove(this->get_project_file_name().c_str());

	if(Sys_Project::project_type!=_sys_project_type::proj_hyd_file){
	
		if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
			ostringstream query_str;
			query_str << "DROP SCHEMA " <<  Sys_Project::get_complete_project_database_schemata_name() << " CASCADE";
			QSqlQuery query(*ptr_database);
			query.exec(query_str.str().c_str());
		}
		else{
			QSqlQuery query(*ptr_database);
			ostringstream query_str;
			string buff;
			int pos;
			QStringList tables;
			tables=ptr_database->tables(QSql::Tables);
			for(int i=0; i<tables.count(); i++){
				query_str.str("");
				buff=tables.at(i).toStdString();
				pos=buff.find(Sys_Project::get_complete_project_database_table_prefix_name());
				if(pos==0){
					query_str << "DROP TABLE " << buff << " CASCADE";
					query.exec(query_str.str().c_str());
				}
			}
		}
	}

	//reset the parameter
	Sys_Project::project_name=label::not_set;
	this->project_file_name=label::not_set;
	this->author_name=label::not_set;
	this->description=label::not_set;
	Sys_Project::project_type=_sys_project_type::proj_all;
	this->file_path=label::not_set;
	this->create_new_dir=true;
	this->database_conn_params.host_name="";
	this->database_conn_params.database_name="";
	this->database_conn_params.user_name="";
	this->database_conn_params.password="";
	this->database_conn_params.driver_name="";
	this->database_conn_params.driver_type=_sys_driver_type::UNKNOWN_DR;
	this->generation_time=label::not_set;

	this->counter_name_change=1;
	this->name_changed_once=false;

	this->risk_state_flag.reliability_dam_result=false;
	this->risk_state_flag.reliability_fpl_result=false;
	this->risk_state_flag.reliability_hyd_result=false;
	this->risk_state_flag.risk_state_flag=false;

	this->hyd_state.file_output_required=true;
	this->hyd_state.number_threads=3;

	this->system_id.measure_nr=0;
	this->system_id.area_state=0;

	
	emit send_txt2statusbar("Ready", 0);
}
//Set current system state
void Sys_Project::set_current_system_state(const _sys_system_id state){
	this->system_id=state;
}
//Set current risk state
void Sys_Project::set_current_risk_state(const _risk_state_flags flag){
	this->risk_state_flag=flag;
}
//Set hydraulic system data
void Sys_Project::set_current_hydraulic_state(const _hyd_state_data state){
	this->hyd_state=state;
}
//Get stored system state
_sys_system_id Sys_Project::get_stored_system_state(void){
	return this->system_id;
}
//Get stored risk state flags
_risk_state_flags Sys_Project::get_stored_risk_state(void){
	return this->risk_state_flag;
}
//Get hydraulic system data
_hyd_state_data Sys_Project::get_stored_hydraulic_state(void){
	return this->hyd_state;
}
//Copy an open project
void Sys_Project::copy_open_project(Data_Base *ptr_database, QWidget *parent){


	ostringstream cout;
	if(ptr_database==NULL){
		cout<<  "No database connection is set. To copy is not possible!" <<endl;
		Sys_Common_Output::output_system->output_txt(&cout,false);	
	}

	ostringstream prefix;
	string old_project_name=Sys_Project::project_name;
	string old_file_path=Sys_Project::file_path;
	string old_current_path=this->current_path;
	string old_main_path=this->main_path;
	ostringstream old_project_file_name;
	old_project_file_name<< this->file_path <<"/"<<Sys_Project::project_name<<".prm";

	try{
		
		if(this->project_type==_sys_project_type::proj_hyd_file){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"Copy of the project is not required!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return;
		}



		//get new project params via a dialog
		Sys_New_Project_Dia my_dia(parent);

		my_dia.set_for_copying(this->project_name, this->author_name, this->description, this->project_type, this->database_conn_params, this->save_logfile);

		if(my_dia.start_dialog()==true){
			prefix <<"COPY> ";
		
			cout<<  "Start project copy..."<<endl;
			Sys_Common_Output::output_system->output_txt(&cout,false);
		
			Sys_Common_Output::output_system->set_userprefix(prefix.str());


			//copy new values to the current one
			Sys_Project::project_name=my_dia.get_project_name();
			this->file_path=my_dia.get_file_path();

			cout<<  "Copy project file " << old_project_name<< " to new project file " << this->project_name <<"..."<<endl;
			Sys_Common_Output::output_system->output_txt(&cout,false);


			this->create_new_project(true);
			this->copy_tables_data_project_file(old_project_file_name.str(), this->get_complete_project_file_name());

			//reopen database
			ptr_database->open_database();

			//recopy it
			Sys_Project::project_name=old_project_name;
			Sys_Project::file_path=old_file_path;
			this->current_path=old_current_path;
			this->main_path=old_main_path;
			this->project_file_name=old_project_file_name.str();

			string new_project_name=my_dia.get_project_name();
			


			this->allocate_pro_copy_thread();
			this->thread_pro_copy->set_copy_member(ptr_database, new_project_name, this->project_name);


			//this->copy_relevant_database_tables(ptr_database, new_project_name);


			Sys_Common_Output::output_system->reset_prefix_was_outputed();
			cout<<  "Project file copy is finished!"<<endl;
			Sys_Common_Output::output_system->output_txt(&cout,false);
		}
		

	}
	catch(Error msg){
		//recopy it
		Sys_Project::project_name=old_project_name;
		Sys_Project::file_path=old_file_path;
		this->current_path=old_current_path;
		this->main_path=old_main_path;
		this->project_file_name=old_project_file_name.str();
		msg.output_msg(0);
	}



	Sys_Common_Output::output_system->rewind_userprefix();

}
//Delete the thread for project copying
void Sys_Project::delete_pro_copy_thread(void){
	if(this->thread_pro_copy!=NULL){
		delete this->thread_pro_copy;
		this->thread_pro_copy=NULL;
	}
}
//___________
//private
//Create a new project
void Sys_Project::create_new_project(const bool copy){
	//check and eventualy change the project name
	this->check_project_name();

	//create new folder
	if(this->create_new_dir==true){
		QDir my_dir;
		//change directores
		if(my_dir.mkdir(this->get_complete_project_folder_name().c_str())==false){
			Error msg=this->set_error(2);
			ostringstream info;
			info <<"Directory name :" <<this->get_complete_project_folder_name() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		my_dir.cd(this->get_complete_project_folder_name().c_str());
		Sys_Project::set_current_path(my_dir.absolutePath().toStdString());
		Sys_Project::set_main_path(my_dir.absolutePath().toStdString());
		this->create_project_folder_data();
		this->create_project_folder_output();


	}
	else{
		QDir my_dir;
		Sys_Project::set_main_path(functions::get_file_path(my_dir.absoluteFilePath(this->get_complete_project_file_name().c_str()).toStdString()));
		Sys_Project::set_current_path(functions::get_file_path(my_dir.absoluteFilePath(this->get_complete_project_file_name().c_str()).toStdString()));

	}
	//create new file
	this->write_project_file(copy);

}
//Check project name
void Sys_Project::check_project_name(void){
	bool all_ok=false;
	bool ok_db=false;
	bool ok_file=false;
	bool somewhere_change=false;
	
	do{
		all_ok=false;
		ok_db=false;
		ok_file=false;
		somewhere_change=false;
		//first folder
		if(this->create_new_dir==true){
			QDir my_dir;
			do{
				if(my_dir.exists(this->get_complete_project_folder_name().c_str())==true){
					this->change_project_name();
					ok_file=false;
					somewhere_change=true;
				}
				else{
					ok_file=true;
				}
			}
			while(ok_file==false);
		}
		//second file
		if(this->create_new_dir==false){
			QFile my_file;
			do{
				if(my_file.exists(this->get_complete_project_file_name().c_str())==true){
					this->change_project_name();
					ok_file=false;
					somewhere_change=true;
				}
				else{
					ok_file=true;
				}
			}
			while(ok_file==false);
		}
		//database
		if(Sys_Project::project_type!=_sys_project_type::proj_hyd_file){
			do{
				if(this->database_conn_params.driver_type==_sys_driver_type::MYSQL){
					//check mysql-database
					if(this->check_mysql_table_exist()==true){
						this->change_project_name();
						ok_db=false;
						somewhere_change=true;
					}
					else{
						ok_db=true;
					}

				}
				else if(this->database_conn_params.driver_type==_sys_driver_type::POSTGRESQL){
					//check psql-database
					if(this->check_psql_schmeta_exist()==true){
						this->change_project_name();
						ok_db=false;
						somewhere_change=true;
					}
					else{
						ok_db=true;
					}
				}

			}
			while(ok_db==false);
		}
		else{
			ok_db=true;
		}

		if(ok_db==true && ok_file==true){
			all_ok=true;
		}

	}
	while(all_ok==false || somewhere_change==true);
}
//Get complete project folder name with path
string Sys_Project::get_complete_project_folder_name(void){
	ostringstream buffer;
	buffer << this->file_path <<"/"<<Sys_Project::project_name;
	return buffer.str();
}
//Change project name if a folder, file or schemate/table already exits
void Sys_Project::change_project_name(void){
	ostringstream buffer;
	if(this->name_changed_once==true){
		int pos;
		pos=Sys_Project::project_name.find_last_of("_");
		if(pos>0){
			Sys_Project::project_name=Sys_Project::project_name.substr(0,pos);	
		}
	}
	buffer << Sys_Project::project_name<<"_"<<this->counter_name_change;
	this->name_changed_once=true;
	Sys_Project::project_name=buffer.str();
	this->counter_name_change++;
}
//Check if the mysql database table exists
bool Sys_Project::check_mysql_table_exist(void){
	bool exist_flag=false;
	//Set database
	Data_Base db(this->database_conn_params);
	//open it
	try{
		db.open_database();
	}
	catch(Error msg){
		throw msg;
	}

	QStringList list_tables;
	Data_Base::database_tables(&list_tables, db.get_database());
	for(int i=0; i< list_tables.count(); i++){
		cout << list_tables.at(i).toStdString()<<endl;

		if(list_tables.at(i).contains(this->get_complete_project_database_table_prefix_name().c_str())==true){
			exist_flag=true;
			break;
		}
	}

	//close db
	db.close_database();

	return exist_flag;
}
//Check if the psql database schemata exists
bool Sys_Project::check_psql_schmeta_exist(void){
	bool exist_flag=false;

	//Set database
	Data_Base db(this->database_conn_params);
	//open it
	try{
		db.open_database();
	}
	catch(Error msg){
		throw msg;
	}


	QSqlQuery my_query(*db.get_database());

	ostringstream filter;
	filter << "CREATE SCHEMA " << this->get_complete_project_database_schemata_name();

	my_query.exec(filter.str().c_str());

	if(my_query.lastError().isValid()==true){
		exist_flag=true;
	}
	else{
		exist_flag=false;
		//remove the schema
		filter.str("");
		filter << "DROP SCHEMA " << this->get_complete_project_database_schemata_name();
		my_query.exec(filter.str().c_str());
	}




	//close db
	db.close_database();

	return exist_flag;
}
//Write the project file
void Sys_Project::write_project_file(const bool copy){
	ofstream ifile;
	ifile.open(this->get_complete_project_file_name().c_str());
	this->project_file_name=this->get_complete_project_file_name().c_str();
	if(ifile.is_open()==false){
		Error msg=this->set_error(3);
		ostringstream info;
		info << "Filename: " << this->get_complete_project_file_name() << endl;
		msg.make_second_info(info.str());
		throw msg;		
	}
	
	if(copy==false){
		//change the logfile to the directory
		Sys_Common_Output::output_system->switch_logfile_path(Sys_Project::current_path,true);
		Sys_Common_Output::output_fpl->switch_logfile_path(Sys_Project::current_path,true);
		Sys_Common_Output::output_excep->switch_logfile_path(Sys_Project::current_path,true);
		Sys_Common_Output::output_hyd->switch_logfile_path(Sys_Project::current_path,true);
		Sys_Common_Output::output_madm->switch_logfile_path(Sys_Project::current_path,true);
		Sys_Common_Output::output_dam->switch_logfile_path(Sys_Project::current_path,true);
		Sys_Common_Output::output_risk->switch_logfile_path(Sys_Project::current_path,true);
		Sys_Common_Output::output_alt->switch_logfile_path(Sys_Project::current_path,true);
		Sys_Common_Output::output_cost->switch_logfile_path(Sys_Project::current_path,true);
		//Sys_Common_Output::output_hydrol->switch_logfile_path(Sys_Project::current_path, true);
		//..introduce further modules
	}

	ostringstream txt;
	ifile << this->generate_project_txt2file();
	ifile.flush();
	
	if(this->project_type!=_sys_project_type::proj_hyd_file){
		//database connection data
		txt<<"#Database connection data"<<endl;
		ifile << txt.str();
		txt.str("");

		Data_Base my_database(this->database_conn_params);
		my_database.output_database_connection2file(&ifile);

		txt<<endl<<endl;
		ifile << txt.str();
		txt.str("");
		//close file
		ifile.close();
	}
	else{
		//close file
		ifile.close();
	}
}

//set the time 
string Sys_Project::get_current_time(void){
	//fot the time
	ostringstream occ_time;
	time_t log_time;
	struct tm *timeinfo;
	//get time
	time(&log_time);
	timeinfo=localtime(&log_time);
	occ_time << asctime(timeinfo);
	this->generation_time=occ_time.str();
	int pos=0;
	pos=this->generation_time.rfind("\n");
	this->generation_time=this->generation_time.substr(0,pos);
	return occ_time.str();
}
///Find the key values for the file input
void Sys_Project::find_key_values_file(string myline , int *must_found_counter){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(sys_label::key_project_name );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_name.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> Sys_Project::project_name;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_author );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_author.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		this->author_name=buffer1.str();
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_time );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_time.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		this->generation_time=buffer1.str();
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_description );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_description.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		this->description=buffer1.str();
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_current_path );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_current_path.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		Sys_Project::current_path=buffer1.str();
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_type );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_type.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		Sys_Project::project_type=Sys_Project::convert_txt2project_type(buffer1.str());
		if(Sys_Project::project_type==_sys_project_type::proj_not){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_logfile_save );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_logfile_save.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			Sys_Project::save_logfile=functions::convert_string2boolean(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}
	//risk data
	pos=myline.find(sys_label::key_project_risk_state);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_risk_state.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			this->risk_state_flag.risk_state_flag=functions::convert_string2boolean(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_reli_hyd_risk);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_reli_hyd_risk.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			this->risk_state_flag.reliability_hyd_result=functions::convert_string2boolean(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_reli_dam_risk);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_reli_dam_risk.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			this->risk_state_flag.reliability_dam_result=functions::convert_string2boolean(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_reli_fpl_risk);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_reli_fpl_risk.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			this->risk_state_flag.reliability_fpl_result=functions::convert_string2boolean(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(sys_label::key_project_catchment_risk_applied);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_catchment_risk_applied.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			this->risk_state_flag.catchment_risk_applied=functions::convert_string2boolean(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(sys_label::key_project_nobreak_risk_applied);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_nobreak_risk_applied.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			this->risk_state_flag.nobreak_risk_applied=functions::convert_string2boolean(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}


	//system data
	pos=myline.find(sys_label::key_project_area_state);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_area_state.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->system_id.area_state;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_measure_state);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_measure_state.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->system_id.measure_nr;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}
	//hyd data
	pos=myline.find(sys_label::key_project_hyd_thread);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_hyd_thread.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->hyd_state.number_threads;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_hyd_file_out);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_hyd_file_out.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			this->hyd_state.file_output_required=functions::convert_string2boolean(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}

	//detailed output flags
	pos=myline.find(sys_label::key_project_sys_out_detailed);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_sys_out_detailed.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			bool buff;

			buff=functions::convert_string2boolean(buffer1.str());
			Sys_Common_Output::output_system->set_detailflag(buff);
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_fpl_out_detailed);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_fpl_out_detailed.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			bool buff;

			buff=functions::convert_string2boolean(buffer1.str());
			Sys_Common_Output::output_fpl->set_detailflag(buff);
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_hyd_out_detailed);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_hyd_out_detailed.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			bool buff;

			buff=functions::convert_string2boolean(buffer1.str());
			Sys_Common_Output::output_hyd->set_detailflag(buff);
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_dam_out_detailed);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_dam_out_detailed.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			bool buff;

			buff=functions::convert_string2boolean(buffer1.str());
			Sys_Common_Output::output_dam->set_detailflag(buff);
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_risk_out_detailed);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_risk_out_detailed.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			bool buff;

			buff=functions::convert_string2boolean(buffer1.str());
			Sys_Common_Output::output_risk->set_detailflag(buff);
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(sys_label::key_project_madm_out_detailed);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_madm_out_detailed.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			bool buff;

			buff=functions::convert_string2boolean(buffer1.str());
			Sys_Common_Output::output_madm->set_detailflag(buff);
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(sys_label::key_project_cost_out_detailed);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_cost_out_detailed.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			bool buff;

			buff=functions::convert_string2boolean(buffer1.str());
			Sys_Common_Output::output_cost->set_detailflag(buff);
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(sys_label::key_project_alt_out_detailed);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(sys_label::key_project_alt_out_detailed.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			bool buff;

			buff=functions::convert_string2boolean(buffer1.str());
			Sys_Common_Output::output_alt->set_detailflag(buff);
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			(*must_found_counter)++;
			return;
		}
	}


	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(0);
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Remove end-of-line from the descrition string
void Sys_Project::remove_endl_description(void){
	ostringstream buffer;


	for(unsigned int i=0; i< this->description.length();i++){
		
		if(this->description.at(i)!='\n'){
			buffer << this->description.at(i);
		}
		else{
			buffer << " ";
		}
	}
	this->description=buffer.str();

}
//Insert end-of-line into the descritpion string
string Sys_Project::insert_endl2descritpion(void){
	unsigned int counter=1;
	ostringstream buff1;
	string buffer;

	for(unsigned int i=0; i< this->description.length(); i++){
		buffer = this->description.at(i);

		if(i>=counter*30 && (buffer==" " || buffer=="\t")){
			counter++;
			buff1<<endl;
		}
		else{
			buff1<<buffer;
		}
	}

	return buff1.str();
}
//Create project folder for the data
void Sys_Project::create_project_folder_data(void){
	QDir my_dir;
	ostringstream buffer;
	//general gis folder
	buffer << this->main_path << "/"<<sys_label::str_gis<<"/";
	if(my_dir.exists(buffer.str().c_str())==false){
		if(my_dir.mkdir(buffer.str().c_str())==false){
			Error msg=this->set_error(2);
			ostringstream info;
			info <<"Directory name :" << buffer.str() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	buffer.str("");
	//general data folder
	buffer << this->main_path << sys_label::folder_datafile<<"/";
	if(my_dir.exists(buffer.str().c_str())==false){
		if(my_dir.mkdir(buffer.str().c_str())==false){
			Error msg=this->set_error(2);
			ostringstream info;
			info <<"Directory name :" << buffer.str() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	ostringstream buffer2;
	if(this->project_type==_sys_project_type::proj_all){
		//hyd
		buffer2<<buffer.str()<< sys_label::str_hyd<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//fpl
		buffer2<<buffer.str()<< sys_label::str_fpl<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//risk
		buffer2<<buffer.str()<< sys_label::str_risk<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//madm
		buffer2<<buffer.str()<< sys_label::str_madm<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//dam
		buffer2<<buffer.str()<< sys_label::str_dam<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");	

		//alt
		buffer2<<buffer.str()<< sys_label::str_alt<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");

		//cost
		buffer2<<buffer.str()<< sys_label::str_cost<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
	}
	else if(this->project_type==_sys_project_type::proj_hyd || this->project_type == _sys_project_type::proj_hyd_temp){
		//hyd
		buffer2<<buffer.str()<< sys_label::str_hyd<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");


	}
	else if(this->project_type==_sys_project_type::proj_hyd_file){
		//hyd
		buffer2<<buffer.str()<< sys_label::str_hyd<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");

	}
	else if(this->project_type==_sys_project_type::proj_risk){
		//hyd
		buffer2<<buffer.str()<< sys_label::str_hyd<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//fpl
		buffer2<<buffer.str()<< sys_label::str_fpl<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//risk
		buffer2<<buffer.str()<< sys_label::str_risk<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//dam
		buffer2<<buffer.str()<< sys_label::str_dam<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");

	}
	else if(this->project_type==_sys_project_type::proj_dam_hyd){
		//dam
		buffer2<<buffer.str()<< sys_label::str_dam<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//hyd
		buffer2<<buffer.str()<< sys_label::str_hyd<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");

	}
	else if(this->project_type==_sys_project_type::proj_dam){
		//dam
		buffer2<<buffer.str()<< sys_label::str_dam<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");

	}
	else if(this->project_type==_sys_project_type::proj_fpl){
		//fpl
		buffer2<<buffer.str()<< sys_label::str_fpl<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
	}
}
//Create project folder for the output
void Sys_Project::create_project_folder_output(void){
	QDir my_dir;
	ostringstream buffer;
	//general folder
	buffer << this->main_path << sys_label::folder_outputfile<<"/";
	if(my_dir.exists(buffer.str().c_str())==false){
		if(my_dir.mkdir(buffer.str().c_str())==false){
			Error msg=this->set_error(6);
			ostringstream info;
			info <<"Directory name :" << buffer.str() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	ostringstream buffer2;
	if(this->project_type==_sys_project_type::proj_all){
		//hyd
		buffer2<<buffer.str()<< sys_label::str_hyd<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//fpl
		buffer2<<buffer.str()<< sys_label::str_fpl<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//risk
		buffer2<<buffer.str()<< sys_label::str_risk<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//madm
		buffer2<<buffer.str()<< sys_label::str_madm<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//dam
		buffer2<<buffer.str()<< sys_label::str_dam<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");	
		//alt
		buffer2<<buffer.str()<< sys_label::str_alt<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");

		//cost
		buffer2<<buffer.str()<< sys_label::str_cost<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(2);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
	}
	else if(this->project_type==_sys_project_type::proj_hyd || this->project_type == _sys_project_type::proj_hyd_temp){
		//hyd
		buffer2<<buffer.str()<< sys_label::str_hyd<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");


	}
	else if(this->project_type==_sys_project_type::proj_hyd_file){
		//hyd
		buffer2<<buffer.str()<< sys_label::str_hyd<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");

	}
	else if(this->project_type==_sys_project_type::proj_risk){
		//hyd
		buffer2<<buffer.str()<< sys_label::str_hyd<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//fpl
		buffer2<<buffer.str()<< sys_label::str_fpl<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//risk
		buffer2<<buffer.str()<< sys_label::str_risk<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//dam
		buffer2<<buffer.str()<< sys_label::str_dam<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");

	}
	else if(this->project_type==_sys_project_type::proj_dam_hyd){
		//dam
		buffer2<<buffer.str()<< sys_label::str_dam<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
		//hyd
		buffer2<<buffer.str()<< sys_label::str_hyd<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");

	}
	else if(this->project_type==_sys_project_type::proj_dam){
		//dam
		buffer2<<buffer.str()<< sys_label::str_dam<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");

	}
	else if(this->project_type==_sys_project_type::proj_fpl){
		//fpl
		buffer2<<buffer.str()<< sys_label::str_fpl<<"/";
		if(my_dir.exists(buffer2.str().c_str())==false){
			if(my_dir.mkdir(buffer2.str().c_str())==false){
				Error msg=this->set_error(6);
				ostringstream info;
				info <<"Directory name :" << buffer2.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		buffer2.str("");
	}
}
//Generate project paramter text for the project file
string Sys_Project::generate_project_txt2file(void){
	string buffer;

	ostringstream txt;
	//write header
	txt <<"##########################################################################"<< endl;
    txt <<"#This automatically generated project file of " << Sys_Project::version <<endl;
    txt <<"#produced by IWW, RWTH Aachen University                                 "<<endl;
    txt <<"#Do not change this file manually. The project can be corrupted after!   "<<endl;
    txt <<"#Comments are marked with '#'                                            "<<endl;
	txt <<"##########################################################################"<< endl<<endl;
	txt <<"#Time of saving: " << this->get_current_time()<<endl<<endl;  
	//general project data
	txt<<"#General project data"<<endl;
	txt<< sys_label::key_project_name << " " << Sys_Project::project_name<<endl;
	txt<< sys_label::key_project_author << " " << this->author_name <<endl;
	txt<< sys_label::key_project_time << " " << this->generation_time <<endl;
	txt<< sys_label::key_project_description << " " << this->description <<endl;
	txt<< sys_label::key_project_type << " " << Sys_Project::convert_project_type2txt(Sys_Project::project_type) <<endl;
	txt<< sys_label::key_project_current_path << " " << Sys_Project::current_path <<endl;
	txt<< sys_label::key_project_logfile_save << " " << functions::convert_boolean2string(Sys_Project::save_logfile)<<endl;
	txt<<endl;
	txt<<"#System data"<<endl;
	txt<< sys_label::key_project_area_state << " " << this->system_id.area_state <<endl;
	txt<< sys_label::key_project_measure_state << " " << this->system_id.measure_nr <<endl;
	txt<<"#Detailed output flags"<<endl;
	txt<< sys_label::key_project_sys_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_system->get_detailflag()) <<endl;
	if(this->project_type==_sys_project_type::proj_hyd_file || this->project_type==_sys_project_type::proj_hyd || this->project_type == _sys_project_type::proj_hyd_temp){
		txt<< sys_label::key_project_hyd_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_hyd->get_detailflag()) <<endl;
	}
	else if(this->project_type==_sys_project_type::proj_fpl){
		txt<< sys_label::key_project_fpl_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_fpl->get_detailflag()) <<endl;
	}
	else if(this->project_type==_sys_project_type::proj_dam_hyd){
		txt<< sys_label::key_project_hyd_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_hyd->get_detailflag()) <<endl;
		txt<< sys_label::key_project_dam_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_dam->get_detailflag()) <<endl;
	}
	else if(this->project_type==_sys_project_type::proj_dam){
		txt<< sys_label::key_project_dam_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_dam->get_detailflag()) <<endl;
	}
	else if(this->project_type==_sys_project_type::proj_risk){
		txt<< sys_label::key_project_fpl_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_fpl->get_detailflag()) <<endl;
		txt<< sys_label::key_project_hyd_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_hyd->get_detailflag()) <<endl;
		txt<< sys_label::key_project_dam_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_dam->get_detailflag()) <<endl;
		txt<< sys_label::key_project_risk_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_risk->get_detailflag()) <<endl;
	}
	else if(this->project_type==_sys_project_type::proj_all){
		txt<< sys_label::key_project_fpl_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_fpl->get_detailflag()) <<endl;
		txt<< sys_label::key_project_hyd_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_hyd->get_detailflag()) <<endl;
		txt<< sys_label::key_project_dam_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_dam->get_detailflag()) <<endl;
		txt<< sys_label::key_project_risk_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_risk->get_detailflag()) <<endl;
		txt<< sys_label::key_project_alt_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_alt->get_detailflag()) <<endl;
		txt<< sys_label::key_project_cost_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_cost->get_detailflag()) <<endl;
		txt<< sys_label::key_project_madm_out_detailed << " " << functions::convert_boolean2string(Sys_Common_Output::output_madm->get_detailflag()) <<endl;

	}

	if(this->project_type==_sys_project_type::proj_all || this->project_type==_sys_project_type::proj_risk ||
		this->project_type==_sys_project_type::proj_dam_hyd || this->project_type==_sys_project_type::proj_hyd ||
		this->project_type==_sys_project_type::proj_hyd_file || this->project_type == _sys_project_type::proj_hyd_temp){
		txt<<"#Hydraulic data"<<endl;
		txt<< sys_label::key_project_hyd_file_out << " " << functions::convert_boolean2string(this->hyd_state.file_output_required) <<endl;
		txt<< sys_label::key_project_hyd_thread << " " << this->hyd_state.number_threads <<endl;
	}
	if(this->project_type==_sys_project_type::proj_all || this->project_type==_sys_project_type::proj_risk){
		txt<<"#Risk data"<<endl;
		txt<< sys_label::key_project_risk_state << " " << functions::convert_boolean2string(this->risk_state_flag.risk_state_flag) <<endl;
		txt<< sys_label::key_project_reli_fpl_risk << " " << functions::convert_boolean2string(this->risk_state_flag.reliability_fpl_result) <<endl;
		txt<< sys_label::key_project_reli_hyd_risk << " " << functions::convert_boolean2string(this->risk_state_flag.reliability_hyd_result) <<endl;
		txt<< sys_label::key_project_reli_dam_risk << " " << functions::convert_boolean2string(this->risk_state_flag.reliability_dam_result) <<endl;
		txt<< sys_label::key_project_nobreak_risk_applied << " " << functions::convert_boolean2string(this->risk_state_flag.nobreak_risk_applied) <<endl;
		txt<< sys_label::key_project_catchment_risk_applied << " " << functions::convert_boolean2string(this->risk_state_flag.catchment_risk_applied) <<endl;
			
	}



	txt<<endl<<endl;

	buffer= txt.str();

	return buffer;

}
//Copy the table data in the project file to another project file
void Sys_Project::copy_tables_data_project_file(const string src, const string dest){

	ifstream ifile_src;
	ifile_src.open(src.c_str(), ios_base::in);
	if(ifile_src.is_open()==false){
		Error msg=this->set_error(8);
		ostringstream info;
		info << "Filename: " << src << endl;
		msg.make_second_info(info.str());
		throw msg;		
	}

	ofstream ifile_dest;
	ifile_dest.open(dest.c_str(), ios_base::app);
	if(ifile_dest.is_open()==false){
		Error msg=this->set_error(9);
		ostringstream info;
		info << "Filename: " << src << endl;
		msg.make_second_info(info.str());
		throw msg;		
	}

	int pos=-1;
	string myline;
	bool start=false;
	do{
		getline(ifile_src, myline,'\n');
		if(start==false){
			pos=myline.find(sys_label::table_begin);
			if(pos>=0){
				start=true;
			}
		}

		if(start==true){
			ifile_dest << myline <<endl;
		}

	}
	while(ifile_src.eof()==false);


	ifile_src.close();
	ifile_dest.close();
}
//Allocate the thread for project copying
void Sys_Project::allocate_pro_copy_thread(void){
	try{
		this->delete_pro_copy_thread();
		this->thread_pro_copy=new Sys_Project_Copier;
		
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(10);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Set error(s)
Error Sys_Project::set_error(const int err_type){
	string place="Sys_Project::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://wrong input
			place.append("find_key_values_file(string myline , int *must_found_counter)");
			reason="There is a problem with the input; wrong sign is read in: Can not open the project";
			help="Check the project information in file";
			fatal=true;
			type=1;
			break;
		case 1://can not open project
			place.append("read_existing_project(void)");
			reason="Can not open the file of the project to read: Can not open the project";
			help="Check the file";
			fatal=true;
			type=5;
			break;
		case 2://can not create directoies
			place.append("create_project_folder_data(void)");
			reason="Can not create the directories";
			help="Check the directory name";
			type=26;
			break;
		case 3://can not open project
			place.append("write_project_file(void)");
			reason="Can not open the file of the project to write";
			help="Check the file";
			type=5;
			break;
		case 4://can not open project
			place.append("read_existing_project(void)");
			reason="Do not find all keywords in the project file: Can not open the project";
			help="Check the file";
			fatal=true;
			type=1;
			break;
		case 5://can not open project
			place.append("save_project_paramater(void)");
			reason="Can not open the file of the project to save settings";
			help="Check the file";
			type=5;
			break;
		case 6://can not create directoies
			place.append("create_project_folder_output(void)");
			reason="Can not create the directories";
			help="Check the directory name";
			type=26;
			break;
		case 7://wrong sql syntax
			place.append("copy_one_database_table(const string src, const string dest, QSqlQueryModel *model, Data_Base *ptr_database)");
			reason="Invalid database request";
			help="Check the database";
			type=2;
			break;
		case 8://can not open project
			place.append("copy_tables_data_project_file(const string src, const string dest)");
			reason="Can not open the source file of the project to copy the table data";
			help="Check the file";
			type=5;
			break;
		case 9://can not open project
			place.append("copy_tables_data_project_file(const string src, const string dest)");
			reason="Can not open the destination file of the project to copy the table data";
			help="Check the file";
			type=5;
			break;
		case 10://bad alloc
			place.append("allocate_pro_copy_thread(void)");
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
