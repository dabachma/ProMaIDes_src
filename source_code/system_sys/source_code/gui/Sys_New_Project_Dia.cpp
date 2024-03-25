#include "Sys_Headers_Precompiled.h"
//#include "Sys_New_Project_Dia.h"


//Default constructor
Sys_New_Project_Dia::Sys_New_Project_Dia(QWidget *parent) : QDialog(parent){

	this->project_name=label::not_set;
	this->author_name=label::not_set;
	this->description=label::not_set;
	this->project_type=_sys_project_type::proj_all;
	this->file_path=label::not_set;
	this->create_new_dir=true;
	this->database_conn_params.host_name="";
	this->database_conn_params.database_name="";
	this->database_conn_params.user_name="";
	this->database_conn_params.password="";
	this->database_conn_params.driver_name="";
	this->database_conn_params.driver_type=_sys_driver_type::UNKNOWN_DR;
	this->database_conn_params.password_saved=true;
	this->project_name_is_set=false;
	this->logfile_save=true;
	this->just_editing=false;
	this->just_copy=false;
	
	ui.setupUi(this);
	this->ui.checkBox_db_is_set->setCheckable(true);
	this->ui.checkBox_db_is_checked->setCheckable(true);
	//slot connectiong
	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(check_parameter()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(ui.lineEdit_pro_name, SIGNAL(textChanged(QString )), this, SLOT(check_project_name_set(QString )));
	
	QObject::connect(ui.pushButton_set_db, SIGNAL(clicked()), this, SLOT(set_database_connection()));
	QObject::connect(ui.pushButton_check_db, SIGNAL(clicked()), this, SLOT(check_database_connection()));
	QObject::connect(ui.radioButton_hyd_file, SIGNAL(toggled(bool )), this, SLOT(decide_database_conn(bool )));

	this->ui.widget_browser->set_text_label("Choose the project directory");
	this->ui.widget_browser->set_tooltip("Choose the project directory");
	this->ui.widget_browser->transfer_file2path_chooser(true);


	//for development
	#ifdef development
        this->database_conn_params.host_name="localhost";
        this->database_conn_params.database_name="promaides";
        this->database_conn_params.user_name="postgres";
		this->database_conn_params.password="";
	#endif
	this->database_conn_params.driver_name="QPSQL";
	this->database_conn_params.driver_type=_sys_driver_type::POSTGRESQL;

	//set tab-order
	QWidget::setTabOrder(this->ui.lineEdit_pro_name, this->ui.lineEdit_author);
	QWidget::setTabOrder(this->ui.lineEdit_author, this->ui.pushButton_set_db);
	QWidget::setTabOrder(this->ui.pushButton_set_db, this->ui.pushButton_check_db);
	QWidget::setTabOrder(this->ui.pushButton_check_db, this->ui.textEdit_descript);
	QWidget::setTabOrder(this->ui.textEdit_descript, this->ui.okButton);


	//set licenses
	this->ui.radioButton_all_types->setEnabled(false);
	this->ui.radioButton_hyd->setEnabled(false);
	this->ui.radioButton_hyd_dam->setEnabled(false);
	this->ui.radioButton_dam_typ->setEnabled(false);
	this->ui.radioButton_risk_typ->setEnabled(false);
	this->ui.radioButton_fpl_typ->setEnabled(false);
	this->ui.radioButton_hyd_file->setEnabled(false);

	#ifdef proj_all_license
		this->ui.radioButton_all_types->setEnabled(true);
		this->ui.radioButton_hyd->setEnabled(true);
		this->ui.radioButton_hyd_dam->setEnabled(true);
		this->ui.radioButton_dam_typ->setEnabled(true);
		this->ui.radioButton_risk_typ->setEnabled(true);
		this->ui.radioButton_fpl_typ->setEnabled(true);
		this->ui.radioButton_hyd_file->setEnabled(true);
		this->ui.radioButton_hydrol->setEnabled(true);
		this->decide_radio_button2projecttype(_sys_project_type::proj_all);
	#endif
	#ifdef proj_risk_license
		this->ui.radioButton_hyd->setEnabled(true);
		this->ui.radioButton_hyd_dam->setEnabled(true);
		this->ui.radioButton_dam_typ->setEnabled(true);
		this->ui.radioButton_risk_typ->setEnabled(true);
		this->ui.radioButton_fpl_typ->setEnabled(true);
		this->ui.radioButton_hyd_file->setEnabled(true);
		this->decide_radio_button2projecttype(_sys_project_type::proj_risk);
	#endif
	#ifdef proj_dam_hyd_license
		this->ui.radioButton_hyd->setEnabled(true);
		this->ui.radioButton_hyd_dam->setEnabled(true);
		this->ui.radioButton_dam_typ->setEnabled(true);
		this->ui.radioButton_hyd_file->setEnabled(true);
		this->decide_radio_button2projecttype(_sys_project_type::proj_dam_hyd);
	#endif
	#ifdef proj_hyd_license
		this->ui.radioButton_hyd->setEnabled(true);
		this->ui.radioButton_hyd_file->setEnabled(true);
		this->decide_radio_button2projecttype(_sys_project_type::proj_hyd);
	#endif
	#ifdef proj_dam_license
		this->ui.radioButton_dam_typ->setEnabled(true);
		this->decide_radio_button2projecttype(_sys_project_type::proj_dam);
	#endif
	#ifdef proj_fpl_license
		this->ui.radioButton_fpl_typ->setEnabled(true);
		this->decide_radio_button2projecttype(_sys_project_type::proj_fpl);
	#endif

	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_New_Project_Dia)), _sys_system_modules::SYS_SYS);
	
}
//Default destructor
Sys_New_Project_Dia::~Sys_New_Project_Dia(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_New_Project_Dia), _sys_system_modules::SYS_SYS);
}
//_________
//public
//Get project name
string Sys_New_Project_Dia::get_project_name(void){
	return this->project_name;
}
//Get project author name
string Sys_New_Project_Dia::get_author_name(void){
	return this->author_name;
}
//Get project description
string Sys_New_Project_Dia::get_description(void){
	return this->description;
}
//Get the project type
_sys_project_type Sys_New_Project_Dia::get_project_type(void){
	return this->project_type;
}
//Get the flag if the logfile should be saved
bool Sys_New_Project_Dia::get_logfile_save_flag(void){
	return this->logfile_save;
}
//Get the file path of the project
string Sys_New_Project_Dia::get_file_path(void){
	return this->file_path;
}
//Get the flag, if a new project directory should be created
bool Sys_New_Project_Dia::get_directory_create_flag(void){
	return this->create_new_dir;
}
//Get the database connection parameters
_sys_database_params Sys_New_Project_Dia::get_database_connection(void){
	return this->database_conn_params;
}
//Start the dialog
bool Sys_New_Project_Dia::start_dialog(void){

	int decision=this->exec();
	//rejected
	if(decision ==0){
		return false;
	}
	//accepted; read the path and file name out via get_file_path(const int index) 
	else{
		return true;
	}
}
//Set the dialog just for editing a project
void Sys_New_Project_Dia::set_for_editing(const string pro_name, const string author, const string descr, const _sys_project_type type, const _sys_database_params conn, const bool logfile_save){

	this->just_editing=true;
	ostringstream info;
	info <<"Edit project "<< pro_name;
	this->setWindowTitle(info.str().c_str());
	this->ui.lineEdit_pro_name->setText(pro_name.c_str());
	this->ui.lineEdit_pro_name->setEnabled(false);
	this->decide_radio_button2projecttype(type);

	this->ui.lineEdit_author->setText(author.c_str());
	this->ui.groupBox_logfile->setEnabled(true);

	
	this->ui.groupBox_pro_type->setEnabled(true);
	this->decide_radio_button_upgrade(type);
	this->ui.groupBox_path->setEnabled(false);

	this->ui.groupBox_logfile->setEnabled(true);
	this->ui.checkBox_logfile->setChecked(logfile_save);

	this->ui.textEdit_descript->setText(descr.c_str());


	this->database_conn_params.host_name=conn.host_name;
	this->database_conn_params.database_name=conn.database_name;
	this->database_conn_params.user_name=conn.user_name;
	this->database_conn_params.password=conn.password;
	this->database_conn_params.driver_name=conn.driver_name;
	this->database_conn_params.driver_type=conn.driver_type;

	if(type!=_sys_project_type::proj_hyd_file){
		this->ui.groupBox_database->setEnabled(false);
		this->ui.checkBox_db_is_set->setChecked(true);
		this->ui.checkBox_db_is_checked->setChecked(true);
	}
	else{
		this->ui.groupBox_database->setEnabled(false);
		this->ui.checkBox_db_is_set->setChecked(false);
		this->ui.checkBox_db_is_checked->setChecked(false);
	}


}
//Set the dialog just for copying a project
void Sys_New_Project_Dia::set_for_copying(const string pro_name, const string author, const string descr, const _sys_project_type type, const _sys_database_params conn, const bool logfile_save){

	this->just_copy=true;
	ostringstream info;
	info <<"Copy project "<< pro_name;
	this->setWindowTitle(info.str().c_str());
	this->ui.lineEdit_pro_name->setText(pro_name.c_str());
	this->ui.lineEdit_pro_name->setEnabled(true);
	this->decide_radio_button2projecttype(type);

	this->ui.lineEdit_author->setText(author.c_str());
	this->ui.lineEdit_author->setEnabled(false);
	this->ui.groupBox_logfile->setEnabled(false);
	this->ui.groupBox_pro_type->setEnabled(false);

	this->ui.groupBox_pro_type->setEnabled(false);
	this->decide_radio_button_upgrade(type);
	this->ui.groupBox_path->setEnabled(true);

	this->ui.groupBox_logfile->setEnabled(false);
	this->ui.checkBox_logfile->setChecked(logfile_save);

	this->ui.textEdit_descript->setText(descr.c_str());
	this->ui.textEdit_descript->setEnabled(false);

	this->database_conn_params.host_name=conn.host_name;
	this->database_conn_params.database_name=conn.database_name;
	this->database_conn_params.user_name=conn.user_name;
	this->database_conn_params.password=conn.password;
	this->database_conn_params.driver_name=conn.driver_name;
	this->database_conn_params.driver_type=conn.driver_type;

	if(type!=_sys_project_type::proj_hyd_file){
		this->ui.groupBox_database->setEnabled(false);
		this->ui.checkBox_db_is_set->setChecked(true);
		this->ui.checkBox_db_is_checked->setChecked(true);
	}
	else{
		this->ui.groupBox_database->setEnabled(false);
		this->ui.checkBox_db_is_set->setChecked(false);
		this->ui.checkBox_db_is_checked->setChecked(false);
	}
}
//____________
//private
//Find project type from the radio buttons
_sys_project_type Sys_New_Project_Dia::find_project_type(void){
	_sys_project_type buffer;
	if(this->ui.radioButton_all_types->isChecked()==true){
		buffer=_sys_project_type::proj_all;
	}
	else if(this->ui.radioButton_risk_typ->isChecked()==true){
		buffer=_sys_project_type::proj_risk;
	}
	else if(this->ui.radioButton_hyd->isChecked()==true){
		buffer=_sys_project_type::proj_hyd;
	}
	else if(this->ui.radioButton_hyd_dam->isChecked()==true){
		buffer=_sys_project_type::proj_dam_hyd;
	}
	else if(this->ui.radioButton_dam_typ->isChecked()==true){
		buffer=_sys_project_type::proj_dam;
	}
	else if(this->ui.radioButton_fpl_typ->isChecked()==true){
		buffer=_sys_project_type::proj_fpl;
	}
	else if(this->ui.radioButton_hyd_file->isChecked()==true){
		buffer=_sys_project_type::proj_hyd_file;
	}
	else if (this->ui.radioButton_hyd_temp->isChecked() == true) {
		buffer = _sys_project_type::proj_hyd_temp;
	}

	return buffer;
}
//____________
//private slots
//Check the given parameter before
bool Sys_New_Project_Dia::check_parameter(void){
	bool ok_flag=true;
	this->project_name=this->ui.lineEdit_pro_name->text().toStdString();
	if(this->project_name.empty()==true){
		this->project_name=label::not_set;
		ok_flag=false;
	}
	this->author_name=this->ui.lineEdit_author->text().toStdString();
	this->logfile_save=this->ui.checkBox_logfile->isChecked();
	this->description=this->ui.textEdit_descript->toPlainText().toStdString();
	this->file_path=this->ui.widget_browser->get_current_path();
	if(this->file_path==label::not_set){
		this->file_path=Sys_Project::get_current_path();
	}
	this->project_type=this->find_project_type();
	if(this->project_type!=_sys_project_type::proj_hyd_file){
		if(this->ui.checkBox_db_is_set->isChecked()==false || this->ui.checkBox_db_is_checked->isChecked()==false){
			ok_flag=false;
		}

		if(this->ui.checkBox_db_is_set->isChecked()==true && this->ui.checkBox_db_is_checked->isChecked()==true){
			ok_flag=true;
		}
		else if(this->ui.checkBox_db_is_set->isChecked()==true && this->ui.checkBox_db_is_checked->isChecked()==false){
			this->check_database_connection();
			if(this->ui.checkBox_db_is_checked->isChecked()==true){
				ok_flag=true;
			}
			else{
				ok_flag=false;
				QMessageBox::critical(this, "Error", "The database connection isn't set properly!");
			}
		}
		else{
			ok_flag=false;
			QMessageBox::critical(this, "Error", "The database connection isn't set properly!");
		}
	}
	else{
		ok_flag=true;
		
	}
	//check file name
	if(ok_flag==true &&this->just_editing==false){

		ok_flag=this->check_exist_project_name();
	}

	if(ok_flag==true){
		this->accept();
	}

	return ok_flag;
}
//Chek if the project name is set and recieve the name
void Sys_New_Project_Dia::check_project_name_set(QString pro_name){


	bool contain_flag=false;
	int cursor_pos=ui.lineEdit_pro_name->cursorPosition();

	contain_flag=this->check_project_name_prefix_modul(pro_name);
	functions::convert_string2lower_case(pro_name.toStdString());
	ui.lineEdit_pro_name->setText(functions::convert_string2lower_case(pro_name.toStdString()).c_str());

	if(pro_name.length()==0 || pro_name==label::not_set.c_str() || contain_flag==true){
		this->ui.lineEdit_author->setEnabled(false);
		this->ui.okButton->setEnabled(false);
		this->ui.groupBox_database->setEnabled(false);
		this->ui.groupbox_descript->setEnabled(false);
		this->ui.groupBox_pro_type->setEnabled(false);
		this->ui.groupBox_path->setEnabled(false);
		this->project_name_is_set=false;
		this->ui.groupBox_logfile->setEnabled(false);
		this->ui.lineEdit_pro_name->setToolTip("Incorrect project name!\nDo not use any special characters in the project name like -,.:#+* tab, whitespace etc.\nDo not use modul names at the end of the project name like ...hyd or ...risk etc.\nDo not use the modul name within the project name followed by a _ sign like ...madm_... or fpl_... etc.\nThe modul names are: hyd, fpl, dam, madm, alt, cost, risk");

	}
	else{
		if(this->just_copy==false){
			this->ui.lineEdit_author->setEnabled(true);
			this->ui.groupbox_descript->setEnabled(true);
			this->ui.groupBox_pro_type->setEnabled(true);
			this->ui.groupBox_logfile->setEnabled(true);
			if(ui.radioButton_hyd_file->isChecked()==false){
				this->ui.groupBox_database->setEnabled(true);
			}
		}
		this->ui.okButton->setEnabled(true);

		
		this->ui.groupBox_path->setEnabled(true);
		this->project_name_is_set=true;
		
		this->ui.lineEdit_pro_name->setToolTip("Correct project name!");

	}

	ui.lineEdit_pro_name->setCursorPosition(cursor_pos);

}
//Check project name for prefixes of modules
bool Sys_New_Project_Dia::check_project_name_prefix_modul(QString pro_name){
	//check for the prefix of the modules
	bool contain_flag=false;
	ostringstream buff;
	QString buffer;

	//check for that the modul names are not the last
	//three letters
	buffer=pro_name;
	buffer.remove(0,pro_name.count()-3);
	if(buffer==functions::convert_string2lower_case(sys_label::str_dam).c_str() ||
		buffer==functions::convert_string2lower_case(sys_label::str_hyd).c_str() ||
		buffer==functions::convert_string2lower_case(sys_label::str_fpl).c_str() ||
		buffer==functions::convert_string2lower_case(sys_label::str_alt).c_str()){
		return contain_flag=true;

	}
	//four letters
	buffer=pro_name;
	buffer.remove(0,pro_name.count()-4);
	if(	buffer==functions::convert_string2lower_case(sys_label::str_risk).c_str() ||
		buffer==functions::convert_string2lower_case(sys_label::str_cost).c_str() ||
		buffer==functions::convert_string2lower_case(sys_label::str_madm).c_str() ){
		return contain_flag=true;

	}

	//check if the modul names with "_" at the end are inside the project name
	buff<<functions::convert_string2lower_case(sys_label::str_dam)<<"_";
	buffer=buff.str().c_str();
	contain_flag=pro_name.contains(buffer,Qt::CaseInsensitive);
	if(contain_flag==true){
		return contain_flag;
	}
	buff.str("");
	buff<<functions::convert_string2lower_case(sys_label::str_sys)<<"_";
	buffer=buff.str().c_str();
	contain_flag=pro_name.contains(buffer,Qt::CaseInsensitive);
	if(contain_flag==true){
		return contain_flag;
	}
	buff.str("");
	buff<<functions::convert_string2lower_case(sys_label::str_hyd)<<"_";
	buffer=buff.str().c_str();
	contain_flag=pro_name.contains(buffer,Qt::CaseInsensitive);
	if(contain_flag==true){
		return contain_flag;
	}
	buff.str("");
	buff<<functions::convert_string2lower_case(sys_label::str_fpl)<<"_";
	buffer=buff.str().c_str();
	contain_flag=pro_name.contains(buffer,Qt::CaseInsensitive);
	if(contain_flag==true){
		return contain_flag;
	}
	buff.str("");
	buff<<functions::convert_string2lower_case(sys_label::str_madm)<<"_";
	buffer=buff.str().c_str();
	contain_flag=pro_name.contains(buffer,Qt::CaseInsensitive);
	if(contain_flag==true){
		return contain_flag;
	}
	buff.str("");
	buff<<functions::convert_string2lower_case(sys_label::str_risk)<<"_";
	buffer=buff.str().c_str();
	contain_flag=pro_name.contains(buffer,Qt::CaseInsensitive);
	if(contain_flag==true){
		return contain_flag;
	}
	buff.str("");

	buff<<functions::convert_string2lower_case(sys_label::str_cost)<<"_";
	buffer=buff.str().c_str();
	contain_flag=pro_name.contains(buffer,Qt::CaseInsensitive);
	if(contain_flag==true){
		return contain_flag;
	}

	buff<<functions::convert_string2lower_case(sys_label::str_alt)<<"_";
	buffer=buff.str().c_str();
	contain_flag=pro_name.contains(buffer,Qt::CaseInsensitive);
	if(contain_flag==true){
		return contain_flag;
	}
	buff.str("");

	buff.str("");

	return contain_flag;
}
//Set the database connection
void Sys_New_Project_Dia::set_database_connection(void){
	Sys_Database_Dia database_dialog(this);
	database_dialog.hide_table_file_browser();
	database_dialog.set_predef_parameter2dialog(this->database_conn_params);
	

	//execute the dia
	if(database_dialog.exec()!=0){//if ok set the parameter	
		this->database_conn_params.host_name=database_dialog.host_name().toStdString();
		this->database_conn_params.database_name=database_dialog.database_name().toStdString();
		this->database_conn_params.user_name=database_dialog.user_name().toStdString();
		this->database_conn_params.password=database_dialog.password().toStdString();
		this->database_conn_params.driver_name=database_dialog.get_driver().toStdString();
		this->database_conn_params.driver_type=Data_Base::convert_txt2drivertype(this->database_conn_params.driver_name);
		this->database_conn_params.password_saved=database_dialog.password_save_flag();
		this->ui.checkBox_db_is_set->setChecked(true);
		this->ui.pushButton_check_db->setEnabled(true);
		this->ui.checkBox_db_is_checked->setChecked(false);
	}	
}
//Check database connection
void Sys_New_Project_Dia::check_database_connection(void){
	Data_Base db(this->database_conn_params);
	bool conn_ok=true;
	ostringstream cout;
	cout << "Check database connection of new project" <<endl;
	Sys_Common_Output::output_system->output_txt(&cout, true);
	try{
		db.open_database();
	}
	catch(Error msg){
		msg.output_msg(0);
		this->ui.checkBox_db_is_checked->setChecked(false);
		conn_ok=false;
	}
	if(conn_ok==true){
		
		this->ui.checkBox_db_is_checked->setChecked(true);

		db.close_database();
	}
	else{
		this->ui.checkBox_db_is_set->setChecked(false);
	}
}
//Check if project name exists already
bool Sys_New_Project_Dia::check_exist_project_name(void){
	bool not_exists=true;
	QDir my_dir;
	ostringstream buffer;
	buffer <<this->file_path<<"/"<<this->project_name<<"/";
	if(my_dir.exists(buffer.str().c_str())==true){
		not_exists=false;
		QMessageBox::critical(this, "Error", "This project folder already exists! \nChange the project name");
		return not_exists;
	}


	if(this->project_type!=_sys_project_type::proj_hyd_file){
		//Set database
		ostringstream cout;
		Data_Base db(this->database_conn_params.host_name,this->database_conn_params.database_name,this->database_conn_params.user_name,this->database_conn_params.password,this->database_conn_params.driver_name);
		cout << "Check new project name in database" <<endl;
		Sys_Common_Output::output_system->output_txt(&cout, true);
		try{
			db.open_database();
		}
		catch(Error msg){
			msg.output_msg(0);
		}
		if(Data_Base::get_driver_type()==_sys_driver_type::MYSQL){
			QStringList list_tables;
			Data_Base::database_tables(&list_tables, db.get_database());
			ostringstream buffer;
			buffer<<this->project_name<<"_";
			for(int i=0; i< list_tables.count(); i++){
				cout << list_tables.at(i).toStdString()<<endl;

				if(list_tables.at(i).contains(buffer.str().c_str())==true){
					not_exists=false;
					break;
				}
			}
			if(not_exists==false){
				QMessageBox::critical(this, "Error", "Check the projectname\n 1. There are already tables in the database with this project name!\n 2. Do not use any special character in the projectname (-,.,+,:,*,#, whitespace, tab etc.)");
				return not_exists;
			}
		}
		else if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
			QSqlQuery my_query(*db.get_database());

			ostringstream buffer;
			buffer<<this->project_name<<"_prm";

			ostringstream filter;
			filter << "CREATE SCHEMA " << buffer.str();

			my_query.exec(filter.str().c_str());

			if(my_query.lastError().isValid()==true){
				not_exists=false;
				QMessageBox::critical(this, "Error", "Check the projectname\n 1. There are already tables in the database with this project name!\n 2. Do not use any special character in the projectname (-,.,+,:,*,#, whitespace, tab etc.)");
				return not_exists;
			}
			else{
				//remove the schema
				filter.str("");
				filter << "DROP SCHEMA " << buffer.str();
				my_query.exec(filter.str().c_str());
			}
		}
	}
	return not_exists;
}
//Decide radio button with a given project type (_sys_project_type)
void Sys_New_Project_Dia::decide_radio_button2projecttype(_sys_project_type type){
	
		if(type==_sys_project_type::proj_all){
			this->ui.radioButton_all_types->setChecked(true);
		}
		else if(type==_sys_project_type::proj_hyd){
			this->ui.radioButton_hyd->setChecked(true);
		}
		else if(type==_sys_project_type::proj_dam_hyd){
			this->ui.radioButton_hyd_dam->setChecked(true);
		}
		else if(type==_sys_project_type::proj_risk){
			this->ui.radioButton_risk_typ->setChecked(true);
		}
		else if(type==_sys_project_type::proj_fpl){
			this->ui.radioButton_fpl_typ->setChecked(true);
		}
		else if(type==_sys_project_type::proj_hyd_file){
			this->ui.radioButton_hyd_file->setChecked(true);
		}
		else if(type==_sys_project_type::proj_dam){
			this->ui.radioButton_dam_typ->setChecked(true);
		}
		else if (type == _sys_project_type::proj_hyd_temp) {
			this->ui.radioButton_hyd_temp->setChecked(true);
		}

}
//Decide radion buttons for a project upgrade
void Sys_New_Project_Dia::decide_radio_button_upgrade(_sys_project_type type){
	this->ui.radioButton_all_types->setEnabled(false);
	this->ui.radioButton_hyd->setEnabled(false);
	this->ui.radioButton_hyd_dam->setEnabled(false);
	this->ui.radioButton_risk_typ->setEnabled(false);
	this->ui.radioButton_fpl_typ->setEnabled(false);
	this->ui.radioButton_hyd_file->setEnabled(false);
	this->ui.radioButton_hydrol->setEnabled(false);
	this->ui.radioButton_dam_typ->setEnabled(false);


	#ifdef proj_all_license
		if(type==_sys_project_type::proj_all){
			this->ui.radioButton_all_types->setChecked(true);
		}
		else if(type==_sys_project_type::proj_hyd){
			this->ui.radioButton_hyd->setChecked(true);
			this->ui.radioButton_hyd_dam->setEnabled(true);
			this->ui.radioButton_risk_typ->setEnabled(true);
			this->ui.radioButton_all_types->setEnabled(true);
		}
		else if(type==_sys_project_type::proj_dam_hyd){
			this->ui.radioButton_hyd_dam->setChecked(true);
			this->ui.radioButton_risk_typ->setEnabled(true);
			this->ui.radioButton_all_types->setEnabled(true);
		}
		else if(type==_sys_project_type::proj_risk){
			this->ui.radioButton_risk_typ->setChecked(true);
			this->ui.radioButton_all_types->setEnabled(true);
		}
		else if(type==_sys_project_type::proj_fpl){
			this->ui.radioButton_fpl_typ->setChecked(true);
			this->ui.radioButton_risk_typ->setEnabled(true);
			this->ui.radioButton_all_types->setEnabled(true);
		}
		else if(type==_sys_project_type::proj_hyd_file){
			this->ui.radioButton_hyd_file->setChecked(true);
		}
		else if (type == _sys_project_type::proj_hyd_temp) {
			this->ui.radioButton_hyd_temp->setChecked(true);
		}
		else if(type==_sys_project_type::proj_dam){
			this->ui.radioButton_dam_typ->setChecked(true);
		}
	#endif

	#ifdef proj_risk_license
		if(type==_sys_project_type::proj_hyd){
			this->ui.radioButton_hyd->setChecked(true);
			this->ui.radioButton_hyd_dam->setEnabled(true);
			this->ui.radioButton_risk_typ->setEnabled(true);
			this->ui.radioButton_all_types->setEnabled(true);
		}
		else if(type==_sys_project_type::proj_dam_hyd){
			this->ui.radioButton_hyd_dam->setChecked(true);
			this->ui.radioButton_risk_typ->setEnabled(true);
			this->ui.radioButton_all_types->setEnabled(true);
		}
		else if(type==_sys_project_type::proj_risk){
			this->ui.radioButton_risk_typ->setChecked(true);
			this->ui.radioButton_all_types->setEnabled(true);
		}
		else if(type==_sys_project_type::proj_fpl){
			this->ui.radioButton_fpl_typ->setChecked(true);
			this->ui.radioButton_risk_typ->setEnabled(true);
			this->ui.radioButton_all_types->setEnabled(true);
		}
		else if(type==_sys_project_type::proj_hyd_file){
			this->ui.radioButton_hyd_file->setChecked(true);
		}
		else if(type==_sys_project_type::proj_dam){
			this->ui.radioButton_dam_typ->setChecked(true);
		}
	#endif

	#ifdef proj_dam_hyd_license
		if(type==_sys_project_type::proj_hyd){
			this->ui.radioButton_hyd->setChecked(true);
			this->ui.radioButton_hyd_dam->setEnabled(true);
			this->ui.radioButton_risk_typ->setEnabled(true);
			this->ui.radioButton_all_types->setEnabled(true);
		}
		else if(type==_sys_project_type::proj_dam_hyd){
			this->ui.radioButton_hyd_dam->setChecked(true);
			this->ui.radioButton_risk_typ->setEnabled(true);
			this->ui.radioButton_all_types->setEnabled(true);
		}
		else if(type==_sys_project_type::proj_hyd_file){
			this->ui.radioButton_hyd_file->setChecked(true);
		}
		else if(type==_sys_project_type::proj_dam){
			this->ui.radioButton_dam_typ->setChecked(true);
		}
	#endif

	#ifdef proj_hyd_license
		if(type==_sys_project_type::proj_hyd){
			this->ui.radioButton_hyd->setChecked(true);
			this->ui.radioButton_hyd_dam->setEnabled(true);
			this->ui.radioButton_risk_typ->setEnabled(true);
			this->ui.radioButton_all_types->setEnabled(true);
		}
		else if(type==_sys_project_type::proj_hyd_file){
			this->ui.radioButton_hyd_file->setChecked(true);
		}
	#endif

	#ifdef proj_dam_license
		if(type==_sys_project_type::proj_dam){
			this->ui.radioButton_dam_typ->setChecked(true);
		}
	#endif

	#ifdef proj_fpl_license
		if(type==_sys_project_type::proj_fpl){
			this->ui.radioButton_fpl_typ->setChecked(true);
			this->ui.radioButton_risk_typ->setEnabled(true);
			this->ui.radioButton_all_types->setEnabled(true);
		}
	#endif

}
//Decide database connection, when the projecttype hyd_file is selected/not selected
void Sys_New_Project_Dia::decide_database_conn(bool flag){
	ui.groupBox_database->setEnabled(!flag);
}
