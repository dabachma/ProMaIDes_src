#pragma once
#ifndef SYS_ONE_FILECHOOSER_WID_H
#define SYS_ONE_FILECHOOSER_WID_H

/**\class Sys_Multi_Filechooser_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

//QT libs
#include <QWidget>
#include <QFile>
#include <QStringList>
//forms
#include "ui_Sys_One_Filechooser_Wid.h"

//system sys
#include "Common_Const.h"

///Dialog-class for one file-browser line. It is used dynamically in Sys_Multi_Filechooser_Dia \ingroup sys
/**

*/
class Sys_One_Filechooser_Wid : public QWidget
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	Sys_One_Filechooser_Wid(QWidget *parent = 0);
	///Default destructor
	~Sys_One_Filechooser_Wid(void);

	///Set the text-label of the file-browser
	void set_text_label(const string text);
	///Set the tool tip for the widget
	void set_tooltip(const string tooltip);
	///Set flag if the filechooser is just for choosing an existing file (true) or creating a new file (false)
	void set_choose_existing_file_flag(const bool flag);

	///Clear the line edit
	void clear_line_edit(void);

	///Transfer the file chooser into a path chooser (true) or other direction (false)
	void transfer_file2path_chooser(const bool path_chooser_flag);

	///Set filter for the file chooser
	void set_filter(const string filter);



	///Get the current path
	string get_current_path(void);
	///Get the filename and the pathname 
	string get_file_name(void);

public slots:
	///Recieve and set the current path 
	void set_current_path(QString path);

signals:
	///Emit the current path
	void send_current_path(QString path);

private:
	//members
	///Form class made with the QT-designer for the layout of the widget
	Ui::Sys_One_Filechooser_Wid ui;

	///File name an path
	string file_name;
	///Current path
	string path;
	///String of the text-label
	string text_label;

	///List for the filters
	string filter_string;

	///Flag if the filechooser is just for choosing an existing file (true) or creating a new file (false)
	bool flag_choose_exist_file;

private slots:
	///Slot for the pushButton of the file_browser; it allows the user to browse a file
	void choose_file(void);
	///Slot for the pushButton of the path_browser; it allows the user to browse a path
	void choose_path(void);
	
};
#endif
