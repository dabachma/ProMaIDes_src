#include "Sys_Headers_Precompiled.h"
//#include "Sys_Delete_Logfile_Dia.h"

//Default constructor
Sys_Delete_Logfile_Dia::Sys_Delete_Logfile_Dia(QWidget *parent): QDialog(parent){

	this->setupUi(this);

	this->treeView_logfile->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->treeView_logfile->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->treeView_logfile->setSortingEnabled(true);

	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QObject::connect(this->checkBox_select_all, SIGNAL(toggled(bool )), this, SLOT(select_all_entries(bool)));
	

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Delete_Logfile_Dia),_sys_system_modules::SYS_SYS);//count the memory

}
//Default destructor
Sys_Delete_Logfile_Dia::~Sys_Delete_Logfile_Dia(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Delete_Logfile_Dia),_sys_system_modules::SYS_SYS);//count the memory
}
//_________
//public
//Set up the dialog
void Sys_Delete_Logfile_Dia::set_dialog(QDirModel *archiv, QDir *path){

	this->treeView_logfile->setModel(archiv);

	this->treeView_logfile->setColumnWidth(0,220);
	this->treeView_logfile->setColumnWidth(1,55);
	this->treeView_logfile->setColumnWidth(2,55);
	this->treeView_logfile->setColumnWidth(3,110);

	this->treeView_logfile->setRootIndex(archiv->index(path->absolutePath()));
	

}
//Start the dialog; it returns true by acception, false by rejection
bool Sys_Delete_Logfile_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		return true;
	}
}
//Get the model of the tree view
QTreeView *Sys_Delete_Logfile_Dia::get_model(void){
	return this->treeView_logfile;
}
//_______
//privateslot
//Select all entries
void Sys_Delete_Logfile_Dia::select_all_entries(bool flag){
	
	
	if(flag==true){
		
		this->treeView_logfile->selectAll();

	}
	else{
		this->treeView_logfile->clearSelection();
	}

}
