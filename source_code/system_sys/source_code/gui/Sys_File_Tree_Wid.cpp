#include "Sys_Headers_Precompiled.h"
//#include "Sys_File_Tree_Wid.h"

//Default constructor
Sys_File_Tree_Wid::Sys_File_Tree_Wid(QWidget *parent) : QWidget(parent){
	this->setupUi(this);
	this->ptr_text_browser=NULL;
	this->my_model=NULL;
	//context menu
	this->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	this->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->refresh_act= new QAction(tr("&Refresh"), this);
	QObject::connect(this->refresh_act, SIGNAL(triggered()), this, SLOT(refresh_tree()));
	this->delete_act= new QAction(tr("&Delete"), this);
	QObject::connect(this->delete_act, SIGNAL(triggered()), this, SLOT(delete_selected()));
	this->set_current_path_act= new QAction(tr("&Set current path"), this);
	QObject::connect(this->set_current_path_act, SIGNAL(triggered()), this, SLOT(set_current_path()));
	
	//connect
	QObject::connect(this->treeView, SIGNAL(doubleClicked(QModelIndex  )) , this, SLOT(tree_item_clicked(QModelIndex )));
	QObject::connect(this->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_File_Tree_Wid), _sys_system_modules::SYS_SYS);

}
//Default destructor
Sys_File_Tree_Wid::~Sys_File_Tree_Wid(void){
	this->delete_dirmodel();
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_File_Tree_Wid), _sys_system_modules::SYS_SYS);
}
//__________
//public
//Set the pointer to the text browser widget
void Sys_File_Tree_Wid::set_ptr_text_wid(Sys_Text_Browser_Wid *ptr){
	this->ptr_text_browser=ptr;	
}
//Clear the tree view and the text-browser
void Sys_File_Tree_Wid::clear_view(void){
	this->setEnabled(false);
	this->delete_dirmodel();
	if(this->ptr_text_browser!=NULL){
		this->ptr_text_browser->close_document();
		this->ptr_text_browser->setEnabled(false);
	}
}

//______
//public slots
//Set the tree view and the text-browser
void Sys_File_Tree_Wid::set_view(void){
	this->setEnabled(false);
	this->ptr_text_browser->setEnabled(false);
	emit send_txt2statusbar("Load data to file project tree view...", 0);
	this->allocate_dirmodel();
	this->treeView->setModel(this->my_model);

	this->treeView->setColumnWidth(0,220);
	this->treeView->setColumnWidth(1,55);
	this->treeView->setColumnWidth(2,55);
	this->treeView->setColumnWidth(3,110);
	QDir my_dir;
	my_dir.cd(Sys_Project::get_main_path().c_str());

	
	this->treeView->setRootIndex(this->my_model->index(my_dir.absolutePath()));
	
	//this->my_model->refresh(this->my_model->index(my_dir.absolutePath()));
	this->setEnabled(true);
	if(this->ptr_text_browser!=NULL){
		this->ptr_text_browser->setEnabled(true);
	}
	emit send_txt2statusbar("Ready", 0);

}
//Recieve, that a tree item is double clicked
void Sys_File_Tree_Wid::tree_item_clicked(QModelIndex index){
	if(this->my_model->fileInfo(index).exists()==true){
		if(this->my_model->fileInfo(index).isDir()==false){
			this->ptr_text_browser->load(this->my_model->fileInfo(index).absoluteFilePath());
		}
	}
}
//Context menu, provides some functionalities by mouse right click
void Sys_File_Tree_Wid::context_menu_request(QPoint point){
	QMenu *contextMenu = new QMenu(this);
	contextMenu->addAction(this->refresh_act);
	contextMenu->addAction(this->delete_act);
	contextMenu->addAction(this->set_current_path_act);
	QPoint trans;
	trans = this->mapToGlobal(point);
	contextMenu->popup(trans);
}
//Refresh the file tree
void Sys_File_Tree_Wid::refresh_tree(void){
	this->setEnabled(false);
	this->delete_dirmodel();
	this->set_view();
	QFileInfo buffer=this->ptr_text_browser->get_current_file_info();
	this->ptr_text_browser->load(buffer);
}
//Delete selected file
void Sys_File_Tree_Wid::delete_selected(void){
	int counter=0;
	QModelIndexList my_list;
	my_list=this->treeView->selectionModel()->selectedIndexes();
	counter=my_list.count();
	QFileInfo my_info;
	QDir my_dir;
	for(int i=0; i<counter; i++){
		my_info=this->my_model->fileInfo(my_list.at(i));
		if(my_info.isFile()==true && my_info.completeSuffix()!="prm"){
			my_dir.remove(my_info.absoluteFilePath());
		}

	}
	if(counter>0){
		this->refresh_tree();
	}
}
//Set current path
void Sys_File_Tree_Wid::set_current_path(void){
	int counter=0;
	QModelIndexList my_list;
	my_list=this->treeView->selectionModel()->selectedIndexes();
	counter=my_list.count();
	QFileInfo my_info;
	QDir my_dir;
	for(int i=0; i<counter; i++){
		my_info=this->my_model->fileInfo(my_list.at(i));
		if(my_info.isDir()==true){
			Sys_Project::set_current_path(my_info.absoluteFilePath().toStdString());
		}
		else{
			Sys_Project::set_current_path(my_info.absolutePath().toStdString());
		}
		break;
	}
}
//_______
//private
//Allocate directory model
void Sys_File_Tree_Wid::allocate_dirmodel(void){
	this->delete_dirmodel();
	this->my_model=new QDirModel;
	this->my_model->setSorting(QDir::DirsLast);

}
//Delete directory model
void Sys_File_Tree_Wid::delete_dirmodel(void){
	if(this->my_model!=NULL){
		this->treeView->setModel(NULL);
		delete this->my_model;
		this->my_model=NULL;
	}
}