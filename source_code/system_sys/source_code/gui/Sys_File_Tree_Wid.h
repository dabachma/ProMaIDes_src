#pragma once
/**\class Sys_File_Tree_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_FILE_TREE_WID_H
#define SYS_FILE_TREE_WID_H

//Qt-libs
#include <QtGui>
#include <QWidget>
#include <QTreeWidget>
#include <QDirModel>
//forms
#include "ui_Sys_File_Tree_Wid.h"

//system sys
#include "Sys_Text_Browser_Wid.h"


///Widget-class for displaying the files as a tree-view in the project folder \ingroup sys
/**


*/
class Sys_File_Tree_Wid : public QWidget, public Ui::Sys_File_Tree_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	Sys_File_Tree_Wid(QWidget *parent =NULL);
	///Default destructor
	~Sys_File_Tree_Wid(void);

	//method
	///Set the pointer to the text browser widget
	void set_ptr_text_wid(Sys_Text_Browser_Wid *ptr);
	///Clear the tree view and the text-browser
	void clear_view(void);


signals:
	///Send a text to the status bar
	void send_txt2statusbar(QString txt, int time);

public slots:
	///Set the tree view and the text-browser
	void set_view(void);
	///Recieve, that a tree item is double clicked
    void tree_item_clicked(QModelIndex index);
	///Context menu, provides some functionalities by mouse right click
	void context_menu_request(QPoint point);
	///Refresh the file tree
	void refresh_tree(void);
	///Delete selected file
	void delete_selected(void);
	///Set current path
	void set_current_path(void);

private:


	//members
	///File tree model for displaying in the tree widget
	 QDirModel *my_model;
	 ///Provides custom functionality to refresh the tree
	 QAction *refresh_act;
	  ///Provides custom functionality to delete
	 QAction *delete_act;
	 ///Provides custom functionality to set the current path
	 QAction *set_current_path_act;

	 ///Pointer to the text browser for diplying the selected files
	 Sys_Text_Browser_Wid *ptr_text_browser;

	 //method
	 ///Allocate directory model
	 void allocate_dirmodel(void);
	 ///Delete directory model
	 void delete_dirmodel(void);


};
#endif
