#pragma once
/**\class Sys_Output_Text_Wid
	\author Yann Wilfried Labou et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/

#ifndef SYS_OUTPUT_TEXT_WID_H
#define SYS_OUTPUT_TEXT_WID_H

//qt-libs
#include <QMenu>
#include <QAction>
#include <QTextEdit>
#include <QContextMenuEvent>
#include <QString>
#include <QPoint>
#include <QShortcut>

//system libs
#include "Common_Const.h"
#include "Sys_Text_Searcher_Wid.h"

//forward declaration
class Main_Wid;

///Textedit-class (parent QTextEdit) for displaying the output of the modules to the main window \ingroup sys
/**
In contrast to the QTextEdit-class a custom context menu is added.
*/
class Sys_Output_Text_Wid : public QTextEdit
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	Sys_Output_Text_Wid(QWidget *parent = NULL);
	///Default destructor
	~Sys_Output_Text_Wid(void);

	///Text searcher dialog (window modal)
	Sys_Text_Searcher_Wid my_searcher;

	//methods
	///Custom connect function, provides option to clear QTextEdit
	void connect_clear_act(Main_Wid *receiver, const _sys_system_modules type);
	///Custom connect function, provides option to set some details on QTextEdit
	void connect_setDetailed_act(Main_Wid *receiver, const _sys_system_modules type);

private slots:
	///Start the text-searcher dialog
	void start_text_searcher(void);

private:
	//members
	///Provides custom functionality to select all the text
	QAction *selectAll_act;
	///Provides custom functionality to copy a selected text
	QAction *copy_act;
	///Provides custom functionality to clear a selected text
	QAction *clear_act;
	//Short cut for the searching act
	QShortcut *short_cut_clear;
	///Provides custom functionality to set a details flag
	QAction *setDetailed_act;

	///Provides custom functionality to search for a string in the text display
	QAction *searching_act;
	//Short cut for the searching act
	QShortcut *short_cut_search;

	///Flag if context menu clear is required
	bool flag_clear_required;
	///Flag if context menu set_detailed required
	bool flag_detailed_set_required;

	//method
	///Set up all the actions in constructor
	void createActions(void);
	///Connect all the signals to all the slots
	void connectActions(void);
	///Context menu, provides some functionalities by mouse right click
    void contextMenuEvent(QContextMenuEvent *event);
};
#endif
