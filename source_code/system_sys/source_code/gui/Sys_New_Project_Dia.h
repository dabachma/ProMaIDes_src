#pragma once
#ifndef SYS_NEW_PROJECT_DIA_H
#define SYS_NEW_PROJECT_DIA_H
/**\class Sys_New_Project_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

//QT libs
#include <QDialog>
#include <QMessageBox>
//system sys
#include "Common_Const.h"
#include "Sys_One_Filechooser_Wid.h"
#include "Data_Base.h"

//forms
#include "ui_Sys_New_Project_Dia.h"



///Enumerator for specifiying the project type of promaides \ingroup sys
enum _sys_project_type{
	///All moduls are availabe
	proj_all,
	///Hydraulic modul (HYD) is availabe
	proj_hyd,
	///Hydraulic modul (HYD) just with file is availabe
	proj_hyd_file,
	///Hydraulic modul (HYD) with temperature availabe
	proj_hyd_temp,
	///Risk modul, which includes HYD, DAM and FPL is availabe
	proj_risk,
	///Damage (DAM) and hydraulic (HYD) modul are available
	proj_dam_hyd,
	///Damage (DAM) modul is available
	proj_dam,
	///Failure probabilty modul (FPL) is available
	proj_fpl,
	///Failure probabilty modul (FPL) just with file is available
	proj_fpl_file,
	///Hydrology (HYDROL) is available
	proj_hydrol,
	///Not know project type
	proj_not,
};





///Dialog-class for creating new projects of ProMaiDes \ingroup sys
/**

*/
class Sys_New_Project_Dia : public QDialog
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT


public:
	///Default constructor
	Sys_New_Project_Dia(QWidget *parent = 0);
	///Default destructor
	~Sys_New_Project_Dia(void);

	//method
	///Get project name
	string get_project_name(void);
	///Get project author name
	string get_author_name(void);
	///Get project description
	string get_description(void);
	///Get the project type
	_sys_project_type get_project_type(void);
	///Get the flag if the logfile should be saved
	bool get_logfile_save_flag(void);
	///Get the file path of the project
	string get_file_path(void);
	///Get the flag, if a new project directory should be created
	bool get_directory_create_flag(void);
	///Get the database connection parameters
	_sys_database_params get_database_connection(void);

	///Start the dialog
	bool start_dialog(void);

	///Set the dialog just for editing a project
	void set_for_editing(const string pro_name, const string author, const string descr, const _sys_project_type type, const _sys_database_params conn, const bool logfile_save);

	///Set the dialog just for copying a project
	void set_for_copying(const string pro_name, const string author, const string descr, const _sys_project_type type, const _sys_database_params conn, const bool logfile_save);


private:

	//members
	///Form class made with the QT-designer for the layout of the dialog
	Ui::Sys_New_Project_Dia ui;

	///Project name
	string project_name;
	///Author name of the project
	string author_name;
	///Project description
	string description;
	///Project type
	_sys_project_type project_type;
	///Project file path
	string file_path;
	///Flag for new project directory creating
	bool create_new_dir;
	///Flag for saving the logfiles in an archive
	bool logfile_save;
	///Database connection parameters
	_sys_database_params database_conn_params;
	///Flag if the project name is set
	bool project_name_is_set;

	///Find project type from the radio buttons
	_sys_project_type find_project_type(void);

	///Flag that the dialog is used to edit the project
	bool just_editing;

	///Flag that the dialog is used to copy a project
	bool just_copy;

private slots:
	///Check the given parameter before
	bool check_parameter(void);
	///Chek if the project name is set and recieve the name
	void check_project_name_set(QString pro_name);
	///Check project name for prefixes of modules
	bool check_project_name_prefix_modul(QString pro_name);
	///Set the database connection
	void set_database_connection(void);
	///Check database connection
	void check_database_connection(void);
	///Check if project name exists already
	bool check_exist_project_name(void);
	///Decide radio button with a given project type (_sys_project_type)
	void decide_radio_button2projecttype(_sys_project_type type);
	///Decide radion buttons for a project upgrade
	void decide_radio_button_upgrade(_sys_project_type type);
	///Decide database connection, when the projecttype hyd_file is selected/not selected
	void decide_database_conn(bool flag);

};
#endif
