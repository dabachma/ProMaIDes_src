#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Result_Scenario_Wid.h"

//Default constructor
RiskGui_Result_Scenario_Wid::RiskGui_Result_Scenario_Wid(QWidget *parent): _Sys_Result_Summary_Wid(parent){


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

	this->head_label->setText("Generated scenarios");

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Result_Scenario_Wid), _sys_system_modules::RISK_SYS);
}
//Default destructor
RiskGui_Result_Scenario_Wid::~RiskGui_Result_Scenario_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Result_Scenario_Wid), _sys_system_modules::RISK_SYS);

}
//_______
//public
//Set-up the widget
void RiskGui_Result_Scenario_Wid::set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id id){
	
	
	Risk_Catchment_Break_Scenario_List list;
	list.set_total_risk_break_list(ptr_database, id);

	list.set_predicted_risk_result2break_sc(ptr_database);

	int number_row=list.get_number_in_list();
		
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
			this->tableWidget->item(i,j)->setData(Qt::DisplayRole, "-");
		}
	}


	QVariant buffer;
	for(int i=0; i<this->tableWidget->rowCount(); i++){

		if(list.get_ptr_scenario(i)->get_is_hydraulic_calculated()==true){
			
			Qt::ItemFlags my_flag;
			my_flag=my_flag|(Qt::ItemIsEnabled);
			for(int j=0; j<this->tableWidget->columnCount(); j++){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color);
			}
		}
		buffer=i;
		this->tableWidget->item(i,0)->setData(Qt::DisplayRole, buffer);
		buffer=list.get_ptr_scenario(i)->get_break_scenario_string().c_str();
		this->tableWidget->item(i,1)->setData(Qt::DisplayRole, buffer);
		buffer=list.get_ptr_scenario(i)->get_number_involved_sections();
		this->tableWidget->item(i,2)->setData(Qt::DisplayRole, buffer);
		buffer=list.get_ptr_scenario(i)->get_prob_scenario();
		this->tableWidget->item(i,3)->setData(Qt::DisplayRole, buffer);
		buffer=list.get_ptr_scenario(i)->get_hydraulic_boundary_scenario().get_id();
		this->tableWidget->item(i,4)->setData(Qt::DisplayRole, buffer);
		buffer=list.get_ptr_scenario(i)->get_hydraulic_boundary_scenario().get_name().c_str();
		this->tableWidget->item(i,5)->setData(Qt::DisplayRole, buffer);
		buffer=list.get_ptr_scenario(i)->get_prob_total();
		this->tableWidget->item(i,6)->setData(Qt::DisplayRole, buffer);
		buffer=Risk_Break_Scenario::convert_source2txt(list.get_ptr_scenario(i)->get_source_scenario()).c_str();
		this->tableWidget->item(i,7)->setData(Qt::DisplayRole, buffer);
		buffer=functions::convert_boolean2string(list.get_ptr_scenario(i)->get_is_hydraulic_calculated()).c_str();
		this->tableWidget->item(i,8)->setData(Qt::DisplayRole, buffer);

		//predicted risk
		if(list.get_ptr_scenario(i)->get_ptr_predicted_risk_values()!=NULL){
			buffer=list.get_ptr_scenario(i)->get_ptr_predicted_risk_values()->ecn_risk_perc;
			this->tableWidget->item(i,9)->setData(Qt::DisplayRole, buffer);
			buffer=list.get_ptr_scenario(i)->get_ptr_predicted_risk_values()->eco_risk_perc;
			this->tableWidget->item(i,10)->setData(Qt::DisplayRole, buffer);
			buffer=list.get_ptr_scenario(i)->get_ptr_predicted_risk_values()->pop_affected_risk_perc;
			this->tableWidget->item(i,11)->setData(Qt::DisplayRole, buffer);
			buffer=list.get_ptr_scenario(i)->get_ptr_predicted_risk_values()->pop_endangered_risk_perc;
			this->tableWidget->item(i,12)->setData(Qt::DisplayRole, buffer);
			buffer=list.get_ptr_scenario(i)->get_ptr_predicted_risk_values()->pys_without_density_risk_perc;
			this->tableWidget->item(i,13)->setData(Qt::DisplayRole, buffer);
		}

	}


	this->tableWidget->resizeColumnsToContents();

}