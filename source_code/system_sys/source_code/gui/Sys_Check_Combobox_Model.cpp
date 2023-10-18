#include "Sys_Headers_Precompiled.h"
//#include "Sys_Check_Combobox_Model.h"


//Default constructor
Sys_Check_Combobox_Model::Sys_Check_Combobox_Model(QObject* parent): QStandardItemModel(parent){
	//At least we need to insert a single column to our model
	this->insertColumn(0);
}
//Default destructor
Sys_Check_Combobox_Model::~Sys_Check_Combobox_Model(void){
}
//___________
//public
//Get the flags of the model
Qt::ItemFlags Sys_Check_Combobox_Model::flags(const QModelIndex& index){
	return QStandardItemModel::flags(index) | Qt::ItemIsUserCheckable;
}