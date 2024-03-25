#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Catchment_Scenario_Table_Wid.h"


//Default constructor
RiskGui_Catchment_Scenario_Table_Wid::RiskGui_Catchment_Scenario_Table_Wid(QWidget *parent): QWidget(parent){


	this->setupUi(this);

	//set number columns
	this->tableWidget->setColumnCount(14);

	QStringList my_list;
	string buffer;
	buffer="No";
	my_list.append(buffer.c_str());
	buffer="Break-id";
	my_list.append(buffer.c_str());
	buffer="Involved FPL-sections";
	my_list.append(buffer.c_str());
	buffer="Probability [-]";
	my_list.append(buffer.c_str());
	buffer="HYD-boundary id";
	my_list.append(buffer.c_str());
	buffer="HYD-boundary name";
	my_list.append(buffer.c_str());
	buffer="Probability with HYD [1/a]";
	my_list.append(buffer.c_str());
	buffer="Source";
	my_list.append(buffer.c_str());
	buffer="Calculated";
	my_list.append(buffer.c_str());

	//predicted risk [%]
	buffer="Predicted ECN risk [%]";
	my_list.append(buffer.c_str());
	buffer="Predicted ECO risk [%]";
	my_list.append(buffer.c_str());
	buffer="Predicted POP-affected risk [%]";
	my_list.append(buffer.c_str());
	buffer="Predicted POP-endangered risk [%]";
	my_list.append(buffer.c_str());
	buffer="Predicted PYS risk [%]";
	my_list.append(buffer.c_str());

	this->tableWidget->setHorizontalHeaderLabels(my_list);

	this->tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->tableWidget->setContextMenuPolicy(Qt::DefaultContextMenu);

	QObject::connect(this->radioButton_sort_prob, SIGNAL(toggled(bool )), this, SLOT(sort_prob(bool )));
	QObject::connect(this->radioButton_sort_prob_hyd, SIGNAL(toggled(bool )), this, SLOT(sort_prob_with_hyd(bool )));
	QObject::connect(this->radioButton_pre_risk, SIGNAL(toggled(bool )), this, SLOT(sort_predicted_risk(bool )));
	QObject::connect(this->comboBox_pre_risk, SIGNAL(currentIndexChanged(int )), this, SLOT(combo_box_changed(int )));

	
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Catchment_Scenario_Table_Wid), _sys_system_modules::RISK_SYS);
}
//Default destructor
RiskGui_Catchment_Scenario_Table_Wid::~RiskGui_Catchment_Scenario_Table_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Catchment_Scenario_Table_Wid), _sys_system_modules::RISK_SYS);
}
//________
//public
//Set-up the widget
void RiskGui_Catchment_Scenario_Table_Wid::set_up_widget(Risk_Catchment_Break_Scenario_List *list){


	int number_row=list->get_number_in_list();
		
	this->tableWidget->setRowCount(number_row);
	//init table widget
	QTableWidgetItem *item;
	Qt::ItemFlags my_flag;
	my_flag=my_flag|(Qt::ItemIsEnabled);
	my_flag=my_flag|(Qt::ItemIsSelectable);
	my_flag=my_flag|(Qt::ItemIsDragEnabled);
	QColor my_color(Qt::cyan);

	for(int i=0; i<this->tableWidget->rowCount(); i++){
		for(int j=0; j<this->tableWidget->columnCount(); j++){
			item=new QTableWidgetItem;
			item->setFlags(my_flag);
			this->tableWidget->setItem(i,j,item);
		}
	}


	QVariant buffer;
	for(int i=0; i<this->tableWidget->rowCount(); i++){

		if(list->get_ptr_scenario(i)->get_is_hydraulic_calculated()==true){
			Qt::ItemFlags my_flag;
			my_flag=my_flag|(Qt::ItemIsEnabled);
			for(int j=0; j<this->tableWidget->columnCount(); j++){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color);
			}
		}
		buffer=i;
		this->tableWidget->item(i,0)->setData(Qt::DisplayRole, buffer);
		buffer=list->get_ptr_scenario(i)->get_break_scenario_string().c_str();
		this->tableWidget->item(i,1)->setData(Qt::DisplayRole, buffer);
		buffer=list->get_ptr_scenario(i)->get_number_involved_sections();
		this->tableWidget->item(i,2)->setData(Qt::DisplayRole, buffer);
		buffer=list->get_ptr_scenario(i)->get_prob_scenario();
		this->tableWidget->item(i,3)->setData(Qt::DisplayRole, buffer);
		buffer=list->get_ptr_scenario(i)->get_hydraulic_boundary_scenario().get_id();
		this->tableWidget->item(i,4)->setData(Qt::DisplayRole, buffer);
		buffer=list->get_ptr_scenario(i)->get_hydraulic_boundary_scenario().get_name().c_str();
		this->tableWidget->item(i,5)->setData(Qt::DisplayRole, buffer);
		buffer=list->get_ptr_scenario(i)->get_prob_total();
		this->tableWidget->item(i,6)->setData(Qt::DisplayRole, buffer);
		buffer=Risk_Break_Scenario::convert_source2txt(list->get_ptr_scenario(i)->get_source_scenario()).c_str();
		this->tableWidget->item(i,7)->setData(Qt::DisplayRole, buffer);
		buffer=functions::convert_boolean2string(list->get_ptr_scenario(i)->get_is_hydraulic_calculated()).c_str();
		this->tableWidget->item(i,8)->setData(Qt::DisplayRole, buffer);



		//predicted risk
		if(list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()!=NULL){
			buffer=list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()->ecn_risk_perc;
			this->tableWidget->item(i,9)->setData(Qt::DisplayRole, buffer);
			buffer=list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()->eco_risk_perc;
			this->tableWidget->item(i,10)->setData(Qt::DisplayRole, buffer);
			buffer=list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()->pop_affected_risk_perc;
			this->tableWidget->item(i,11)->setData(Qt::DisplayRole, buffer);
			buffer=list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()->pop_endangered_risk_perc;
			this->tableWidget->item(i,12)->setData(Qt::DisplayRole, buffer);
			buffer=list->get_ptr_scenario(i)->get_ptr_predicted_risk_values()->pys_without_density_risk_perc;
			this->tableWidget->item(i,13)->setData(Qt::DisplayRole, buffer);
		}

	}

	this->tableWidget->sortByColumn(3,Qt::DescendingOrder);
	this->tableWidget->resizeColumnsToContents();
}
//Get a pointer to the table widget
QTableWidget* RiskGui_Catchment_Scenario_Table_Wid::get_ptr_table_widget(void){
		return this->tableWidget;
}
//________
//private slots
//Sort after probability
void RiskGui_Catchment_Scenario_Table_Wid::sort_prob(bool flag){
	if(flag==true){
		this->tableWidget->sortByColumn(3,Qt::DescendingOrder);
	}
}
//Sort after probability with the probability of the hydraulic boundary scenario (with HYD)
void RiskGui_Catchment_Scenario_Table_Wid::sort_prob_with_hyd(bool flag){
	if(flag==true){
		this->tableWidget->sortByColumn(6, Qt::DescendingOrder);
	}
}
//Sort after the predicted risk percentage
void RiskGui_Catchment_Scenario_Table_Wid::sort_predicted_risk(bool flag){
	if(flag==true){
		
		if(this->comboBox_pre_risk->currentText()=="ECN"){
			this->tableWidget->sortByColumn(9, Qt::DescendingOrder);
		}
		else if(this->comboBox_pre_risk->currentText()=="ECO"){
			this->tableWidget->sortByColumn(10, Qt::DescendingOrder);
		}
		else if(this->comboBox_pre_risk->currentText()=="POP affected"){
			this->tableWidget->sortByColumn(11, Qt::DescendingOrder);
		}
		else if(this->comboBox_pre_risk->currentText()=="POP endangered"){
			this->tableWidget->sortByColumn(12, Qt::DescendingOrder);
		}
		else if(this->comboBox_pre_risk->currentText()=="PYS"){
			this->tableWidget->sortByColumn(13, Qt::DescendingOrder);
		}
	}
}
//Combo box text has changed
void RiskGui_Catchment_Scenario_Table_Wid::combo_box_changed(int ){
	this->sort_predicted_risk(this->radioButton_pre_risk->isChecked());
}