#pragma once
/**\class Sys_Text_Browser_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_TEXT_BROWSER_WID_H
#define SYS_TEXT_BROWSER_WID_H

//Qt-libs
#include <QtGui>
#include <QWidget>
#include <QFileInfo>
#include <QPoint>
#include <QToolButton>
#include <QFlags>
#include <QPrinter>
#include <QShortcut>
//forms
#include "ui_Sys_Text_Browser_Wid.h"
//system sys
#include "Sys_Text_Searcher_Wid.h"



///Widget-class for displaying and manipulate files\ingroup sys
/**


*/
class Sys_Text_Browser_Wid : public QWidget, Ui::Sys_Text_Browser_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	Sys_Text_Browser_Wid(QWidget *parent =NULL);
	///Default destructor
	~Sys_Text_Browser_Wid(void);

	//method
	///Load a file
	bool load(const QFileInfo file);
	///Ask for saving by closing a changed file
    bool maybe_save(void);
	///Set the current file name
    void set_current_file_name(const QFileInfo file);
	///Get the current file info
	QFileInfo get_current_file_info(void);
	
public slots:
	 ///Close document
	 bool close_document(void);
	 ///Context menu, provides some functionalities by mouse right click
	 void context_menu_request(QPoint point);

private slots:

	 ///Save a file
     bool file_save(void);
	 ///New a file
     void file_new(void);
	 ///Save a file with a new name
     bool file_save_as(void);
	 ///Print a file
     void file_print(void);
	 ///Print preview
     void file_print_preview(void);
	 ///Print the preview
	 void print_preview(QPrinter *printer);
	 ///Print a file to pdf
     void file_print_pdf(void);

	 ///Check if the file is modified
	void check_for_modification(void);

	///Start the text-searcher dialog
	void start_text_searcher(void);


private:

	//member
	///File name with path
	QString file_name;
	///Pure file name
	QString pure_file_name;
	///File info of the current
	QFileInfo file_info;

	///File is read only
	bool file_read_only;

	///Text searcher
	Sys_Text_Searcher_Wid my_searcher;

	///Provides custom functionality to select all the text
	QAction *selectAll_act;
	///Provides custom functionality to copy a selected text
	QAction *copy_act;
	///Provides custom functionality to paste
	QAction *paste_act;

	///Provides custom functionality to sreach for a string in the text display
	QAction *searching_act;
	//Short cut for the searching act
	QShortcut *short_cut_search;

	///Provides custom functionality for saving
	QAction *save_act;
	//Short cut for the save act
	QShortcut *short_cut_save;
    ///Provides custom functionality for saving as
	QAction *save_as_act;
	///Provides custom functionality for closing
	QAction *close_act;
	///Provides custom functionality a new file
	QAction *new_act;

	///Provides custom functionality for printing
	QAction *print_act;
	///Provides custom functionality for print preview
	QAction *print_preview_act;
	///Provides custom functionality for printing to pdf
	QAction *print2pdf_act;

	///Provides custom functionality for undo
	QAction *undo_act;
	///Provides custom functionality for redo
	QAction *redo_act;

	//method
	///Set up all the actions for the context menu
	void create_action_for_context_menu(void);

};
#endif
