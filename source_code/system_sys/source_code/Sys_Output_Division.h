#pragma once
/**\class Sys_Output_Division
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_OUTPUT_DIVISION_H
#define SYS_OUTPUT_DIVISION_H

//libs
//qt-class
#include <QTextEdit>
#include <QTabWidget>
#include <QObject>

//system sys classes
#include "Common_Const.h"
#include "Sys_Output_Flag_Dia.h"
#include "Sys_Output_Logfile_Dia.h"


///Output-class for the output of ProMaiDes to display (GUI), console and logfile \ingroup sys
/**
The output is seperated after the given module (_sys_system_modules). Each module has a own
logfile and a display in the main widget.

*/
class Sys_Output_Division : public QObject
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Constructor
	Sys_Output_Division(void);
	///Constructor with a given module type (_sys_system_modules)
	Sys_Output_Division(const _sys_system_modules type);
	///Destructor
	~Sys_Output_Division(void);

	//methods
	///Set the type of module for the output
	void set_module_type(const _sys_system_modules type);
	///Set the file-name of the output-logfile
	void set_file(const string filename);
	///Set a new logfile per gui
	void set_new_logfile_gui(QWidget *parent = NULL);
	///Close the output-logfile
	void close_file(void);
	///Delete the output-logfile
	void remove_file(void);
	///Set and open a new output-logfile
	void open_new_file(const string filename);
	///Set a display-pointer (QTextEdit) for the GUI-output
	void set_gui_texteditor(QTextEdit *ptr_output_display);

	///Set the output detail flag per console
	void set_detailflag_console(void);
	///Set the output detail flag per GUI
	void set_detailflag_gui(QWidget *parent = NULL);
	///Set the output detail flag directly (true => all to console and to file; false => all to file and some things to console)
	void set_detailflag(const bool detail);
	///Get the output detail flag
	bool get_detailflag(void);

	///Output the text to logfile, console and GUI (QTextEdit) without deleting the string
	void output_txt(const string txt, const bool detail_output=false, const bool file_output=true);
	///Output the text to logfile, console and GUI (QTextEdit) with deleting the stringstream
	void output_txt(ostringstream *stream, const bool detail_output=false, const bool file_output=true);
	///Insert a separator, specifeid with the type to the output media
	void insert_separator(const int type, const bool detail_output=false);
	///Get a separator, specifeid with the type
	string get_separator(const int type);
	///Add one part to the user-defined prefix without deleting the string
	void set_userprefix(const string userprefix);
	///Add one part to the user-defined prefix and delete the stringstream
	void set_userprefix(ostringstream *prefix);
	///Reset the user-defined prefix
	void reset_userprefix(void);
	///Reset the flag that the prefix was outputed; the prefix will be newly outputed
	void reset_prefix_was_outputed(void);

	///Rewind one stage of the user-defined prefix
	void rewind_userprefix(void);
	///Get the total prefix
	string get_totalprefix(void);
	///Set the welcome text to the GUI
	void welcome_to_gui(void);

	///Switch the logfile to the given path
	void switch_logfile_path(const string path, const bool del_flag);

	///Set the time, when the output is generated
	static string set_time(void);

	///Save the current logfile to the logfile archiv
	void save_logfile2archiv(void);

	///Set the focus on the display, where the output comes
	void set_focus_on_display(void);
	///Set the pointer to the parent tabwidget of the display output widget
	void set_ptr_parent_display_output(QTabWidget *parent);

signals:
	///Is emitted when new gui txt is available (signal)
	void gui_text(const QString txt);

	
private:
	//members
	///Stream class for the output file
	ofstream fileoutput;
	///Filename for the output
	string name_file;

	///Output display in the GUI for the output
	QTextEdit *guioutput;
	///Pointer to the tabwidget, to which the output display is belonging
	QTabWidget *parent_display;

	///Flag if the detailed to display/console is detailed
	bool detail_display;

	///Flag if the output is also to the display of the GUI
	const bool gui_flag;
	///Flag if the output is also to the console
	const bool console_flag;

	///Type specifier for which module the output is
	_sys_system_modules module_type;

	///String of the user-prefix
	string user_prefix;
	///The userprefix was already one time outputted
	bool prefix_was_output;

	//method
	///Open the output-logfile 
	void open_file(void);
	///Set the welcome text to the file
	void welcome_to_file(void);
	///Get the standard prefix
	string standard_prefix(void);


	///Get the logfile name depending to the module type (_sys_system_modules)
	string get_logfile_name(void);

	///Get the date as string for saving the logfiles
	string get_current_date(void);
	
	///Set error(s)
	void error(const int err_type);

};

#endif