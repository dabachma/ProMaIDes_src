#include "Hyd_Headers_Precompiled.h"
//#include "HydGui_Bound_Scenario_Table_Wid.h"


//Default constructor
HydGui_Bound_Scenario_Table_Wid::HydGui_Bound_Scenario_Table_Wid(QWidget *parent) : QTableView(parent), column_count(6){
	this->row_count=0;
	this->model=NULL;
	this->int_delegate=NULL;
	this->dbl_delegate=NULL;

	this->model = new QStandardItemModel(0, this->column_count);
	this->setModel(model);
	this->horizontalHeader()->setStretchLastSection(true);

	QStringList hhLabels;
	hhLabels << tr("ID./No.") << tr("Source") << tr("Name") 
			 << tr("annuality [a]") << tr("probability max-event [-]")<< tr("probability form-event [-]") 
			 << tr("description");

	this->model->setHorizontalHeaderLabels(hhLabels);
	this->model->horizontalHeaderItem(0)->setToolTip(tr("For existing boundary scenarios: the id \n for new: the number"));
	this->model->horizontalHeaderItem(1)->setToolTip(tr("For existing boundary scenarios: always from database \n for new: the file name"));
	this->model->horizontalHeaderItem(2)->setToolTip(tr("Name of the hydraulic boundary scenario"));
	this->model->horizontalHeaderItem(3)->setToolTip(tr("Annuality [a] of the hydraulic boundary scenario"));
	this->model->horizontalHeaderItem(4)->setToolTip(tr("Probability of occurence of the max-event (is calculated)"));
	this->model->horizontalHeaderItem(5)->setToolTip(tr("Probability of occurence of the form-event in the given annuality"));

	this->setColumnWidth(0, 50);
	this->setColumnWidth(1, 140);
	this->setColumnWidth(2, 120);
	this->setColumnWidth(3, 100);
	this->setColumnWidth(4, 100);
	this->setColumnWidth(5, 100);
	this->setColumnWidth(6, 150);

	this->int_delegate = new Sys_SpinBox_Delegate(this);
	this->int_delegate->set_spin_box_values(10000000,1,10);
	this->dbl_delegate = new Sys_DoubleSpinBox_Delegate(this);
	this->dbl_delegate->set_spin_box_values(1.0,0.0,0.1,6);

	this->setItemDelegateForColumn(3, this->int_delegate);
	this->setItemDelegateForColumn(5, this->dbl_delegate);

	this->my_color.setColor(Qt::red);

	this->setShowGrid(true);

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_Bound_Scenario_Table_Wid), _sys_system_modules::HYD_SYS);
}
//Default destructor
HydGui_Bound_Scenario_Table_Wid::~HydGui_Bound_Scenario_Table_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_Bound_Scenario_Table_Wid), _sys_system_modules::HYD_SYS);
}
//__________
//public
///Set the number of rows; it have to be done before the rows are filled
void HydGui_Bound_Scenario_Table_Wid::set_rows(const int rowCount){
	this->model->setRowCount(rowCount);
	this->row_count=rowCount;
}
//Set rows of existing scenarios
void HydGui_Bound_Scenario_Table_Wid::set_rows_existing_sc(const int number, Hyd_Boundary_Szenario *scenario){

	this->my_color.setColor(Qt::black);
	for(int i=0; i< number; i++){
		this->model->setData(this->model->index(i, 0, QModelIndex()), scenario[i].get_id(), Qt::DisplayRole);
		this->model->item(i, 0)->setEditable(false);
		this->model->item(i, 0)->setForeground(this->my_color);
		this->model->setData(this->model->index(i, 1, QModelIndex()), "database", Qt::DisplayRole);
		this->model->item(i, 1)->setEditable(false);
		this->model->item(i, 1)->setForeground(this->my_color);
		this->model->setData(this->model->index(i, 2, QModelIndex()), scenario[i].get_name().c_str(), Qt::DisplayRole);
		this->model->item(i, 2)->setEditable(false);
		this->model->item(i, 2)->setForeground(this->my_color);
		this->model->setData(this->model->index(i, 3, QModelIndex()), scenario[i].get_annuality(), Qt::DisplayRole);
		this->model->item(i, 3)->setEditable(false);
		this->model->item(i, 3)->setForeground(this->my_color);
		this->model->setData(this->model->index(i, 4, QModelIndex()), scenario[i].get_prob_year(), Qt::DisplayRole);
		this->model->item(i, 4)->setEditable(false);
		this->model->item(i, 4)->setForeground(this->my_color);
		this->model->setData(this->model->index(i, 5, QModelIndex()), scenario[i].get_prob_occurence(), Qt::DisplayRole);
		this->model->item(i, 5)->setEditable(false);
		this->model->item(i, 5)->setForeground(this->my_color);
		this->model->setData(this->model->index(i, 6, QModelIndex()), scenario[i].get_description().c_str(), Qt::DisplayRole);
		this->model->item(i, 6)->setEditable(false);
		this->model->item(i, 6)->setForeground(this->my_color);
	}
}
//Set file names to the table widget
void HydGui_Bound_Scenario_Table_Wid::set_rows_new_sc(const int number, string *file_name, const int number_old){
	string buffer;
	this->my_color.setColor(Qt::red);
	for(int i=number_old; i< number+number_old; i++){
		this->model->setData(this->model->index(i, 0, QModelIndex()), i-number_old, Qt::DisplayRole);
		this->model->item(i, 0)->setEditable(false);
	
		this->model->item(i, 0)->setForeground(this->my_color);
		buffer=file_name[i-number_old];
		this->model->setData(this->model->index(i, 1, QModelIndex()), buffer.c_str(), Qt::DisplayRole);
		this->model->item(i, 1)->setEditable(false);
		this->model->item(i, 1)->setForeground(this->my_color);
		this->model->setData(this->model->index(i, 2, QModelIndex()), label::not_set.c_str(), Qt::EditRole);
		this->model->item(i, 2)->setEditable(true);
		this->model->item(i, 2)->setForeground(this->my_color);
		this->model->setData(this->model->index(i, 3, QModelIndex()), 100 , Qt::EditRole);
		this->model->item(i, 3)->setEditable(true);
		this->model->item(i, 3)->setForeground(this->my_color);
		this->model->setData(this->model->index(i, 4, QModelIndex()), "-", Qt::DisplayRole);
		this->model->item(i, 4)->setEditable(false);
		this->model->item(i, 4)->setForeground(this->my_color);
		this->model->setData(this->model->index(i, 5, QModelIndex()), 1.0, Qt::EditRole);
		this->model->item(i, 5)->setEditable(true);
		this->model->item(i, 5)->setForeground(this->my_color);
		this->model->setData(this->model->index(i, 6, QModelIndex()), "", Qt::EditRole);
		this->model->item(i, 6)->setEditable(true);
		this->model->item(i, 6)->setForeground(this->my_color);
	}

	this->resizeColumnsToContents();
}
//Set the data of a row to a given hydraulic boundary scenario
void HydGui_Bound_Scenario_Table_Wid::set_data2bound_scenario(const int index, Hyd_Boundary_Szenario *scenario){
	if(index<0 || index>=this->row_count){
		scenario->annuality=1.0;
		scenario->name=label::not_set;
		scenario->prob_event=1.0;
	}
	else{
		scenario->annuality=this->model->data(this->model->index(index, 3,QModelIndex())).toDouble();
		scenario->name=this->model->data(this->model->index(index, 2,QModelIndex())).toString().toStdString();
		scenario->prob_event=this->model->data(this->model->index(index, 5,QModelIndex())).toDouble();
		scenario->description=this->model->data(this->model->index(index, 6,QModelIndex())).toString().toStdString();
	}
}