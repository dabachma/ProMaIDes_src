#pragma once
/**\class Sys_Text_Searcher_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef SYS_TEXT_SEARCHER_WID_H
#define SYS_TEXT_SEARCHER_WID_H

//qt libs
#include <QDialog>
#include <QTextEdit>
//forms
#include "ui_Sys_Text_Searcher_Wid.h"
//system sys
#include "Common_Const.h"

///Widget-class for a text search in the output text widgets \ingroup sys
/**

*/
class Sys_Text_Searcher_Wid : public QWidget, Ui::Sys_Text_Searcher_Wid 
{
//Macro for using signals and slots (Qt)in this class
	Q_OBJECT
public:
	///Default constructor
	Sys_Text_Searcher_Wid(QWidget *parent=NULL);
	///Default destructor
	~Sys_Text_Searcher_Wid(void);

	//method
	///Set a pointer to the text editor, where the search should be performed
	void set_ptr_text_edit(QTextEdit *ptr);
	///Set the title of the window
	void set_window_title(const _sys_system_modules type);
	///Set the title of the window
	void set_window_title(const string txt);
	///Set start search at begin-flag 
	void set_start_at_begin_flag(const bool flag);
	///Set upwards search at flag 
	void set_upwards_search_flag(const bool flag);

public slots:
	///Start the dialog
	void start_dialog(void);

private:

	//members
	///Pointer to the text editor, where the search should be performed
	QTextEdit *ptr_text_editor;

private slots:
	///Perform a search
	void perform_search(void);
};
#endif