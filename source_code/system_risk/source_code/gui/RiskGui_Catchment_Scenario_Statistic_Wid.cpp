#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Catchment_Scenario_Statistic_Wid.h"


//Default constructor
RiskGui_Catchment_Scenario_Statistic_Wid::RiskGui_Catchment_Scenario_Statistic_Wid(QWidget *parent): _Sys_Result_Summary_Wid(parent){

	//set number columns
	this->tableWidget->setColumnCount(5);

	QStringList my_list;
	string buffer;
	buffer="No";
	my_list.append(buffer.c_str());
	buffer="Name";
	my_list.append(buffer.c_str());
	buffer="Calculated";
	my_list.append(buffer.c_str());
	buffer="Total";
	my_list.append(buffer.c_str());
	buffer="Percentage [%]";
	my_list.append(buffer.c_str());

	this->tableWidget->setHorizontalHeaderLabels(my_list);

	this->head_label->setText("Statistics of the catchment risk approach");
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Catchment_Scenario_Statistic_Wid), _sys_system_modules::RISK_SYS);

}
//Default destructor
RiskGui_Catchment_Scenario_Statistic_Wid::~RiskGui_Catchment_Scenario_Statistic_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Catchment_Scenario_Statistic_Wid), _sys_system_modules::RISK_SYS);

}
//________
//public
//Set-up the widget
void RiskGui_Catchment_Scenario_Statistic_Wid::set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id id){


	//hydraulic boundary manager
	Hyd_Boundary_Szenario_Management my_hyd_sc;
	my_hyd_sc.set_systemid(id);
	my_hyd_sc.set_szenario_per_db(ptr_database);

	//evaluate number of rows
	int number_rows=7;
	number_rows=number_rows+my_hyd_sc.get_number_sz()*2;
	this->tableWidget->setRowCount(number_rows);
	//init table widget
	QTableWidgetItem *item;
	Qt::ItemFlags my_flag;
	my_flag=my_flag|(Qt::ItemIsEnabled);
	my_flag=my_flag|(Qt::ItemIsSelectable);
	my_flag=my_flag|(Qt::ItemIsDragEnabled);


	QColor my_color1(Qt::lightGray);

	for(int i=0; i<this->tableWidget->rowCount(); i++){
		for(int j=0; j<this->tableWidget->columnCount(); j++){
			item=new QTableWidgetItem;
			item->setFlags(my_flag);
			this->tableWidget->setItem(i,j,item);
			this->tableWidget->item(i,j)->setData(Qt::DisplayRole, "-");

			if(i>=this->tableWidget->rowCount()-5){
				//ecn
				this->tableWidget->item(i,j)->setBackgroundColor(my_color1);	
			}
		}
	}

	Risk_Catchment_Break_Scenario_List list;
	list.set_total_risk_break_list(ptr_database, id);

	int number_total=0;
	int number_calc=0;
	double prob_total=0.0;
	double prob_calc=0.0;
	QVariant buffer;
	int counter=1;

	//count total
	number_total=list.get_number_in_list();
	for(int i=0; i< list.get_number_in_list(); i++){
		if(list.get_ptr_scenario(i)->get_is_hydraulic_calculated()==true){
			prob_calc=prob_calc+list.get_ptr_scenario(i)->get_prob_total();
			number_calc++;
		}
		prob_total=prob_total+list.get_ptr_scenario(i)->get_prob_total();
	}

	//set it to the wid
	//scenario total
	buffer=counter;
	
	this->tableWidget->item(counter-1,0)->setData(Qt::DisplayRole, buffer);
	buffer="Scenario total [-]";
	this->tableWidget->item(counter-1,1)->setData(Qt::DisplayRole, buffer);
	buffer=number_calc;
	this->tableWidget->item(counter-1,2)->setData(Qt::DisplayRole, buffer);
	buffer=number_total;
	this->tableWidget->item(counter-1,3)->setData(Qt::DisplayRole, buffer);
	if(number_total!=0){
		buffer=((double)number_calc/(double)number_total)*100.0;
		this->tableWidget->item(counter-1,4)->setData(Qt::DisplayRole, buffer);
	}
	counter++;
	//prob total
	buffer=counter;
	this->tableWidget->item(counter-1,0)->setData(Qt::DisplayRole, buffer);
	buffer="Probability total [1/a]";
	this->tableWidget->item(counter-1,1)->setData(Qt::DisplayRole, buffer);
	buffer=prob_calc;
	this->tableWidget->item(counter-1,2)->setData(Qt::DisplayRole, buffer);
	buffer=prob_total;
	this->tableWidget->item(counter-1,3)->setData(Qt::DisplayRole, buffer);
	if(prob_total!=0.0){
		buffer=(prob_calc/prob_total)*100.0;
		this->tableWidget->item(counter-1,4)->setData(Qt::DisplayRole, buffer);
	}
	counter++;


	ostringstream text;
	//per hyd boundary
	for(int i=0; i< my_hyd_sc.get_number_sz(); i++){
		number_total=0;
		number_calc=0;
		prob_total=0.0;
		prob_calc=0.0;
		//count them
		for(int j=0; j< list.get_number_in_list(); j++){
			if(my_hyd_sc.get_ptr_sz(i)->get_id()==list.get_ptr_scenario(j)->get_hydraulic_boundary_scenario().get_id()){
				if(list.get_ptr_scenario(j)->get_is_hydraulic_calculated()==true){
					prob_calc=prob_calc+list.get_ptr_scenario(j)->get_prob_scenario();
					number_calc++;
				}
				prob_total=prob_total+list.get_ptr_scenario(j)->get_prob_scenario();
				number_total++;
			}
		}
		

		//scenario total
		buffer=counter;
		
		this->tableWidget->item(counter-1,0)->setData(Qt::DisplayRole, buffer);
		text.str("");
		text<< "Scenario "<<my_hyd_sc.get_ptr_sz(i)->get_name()<<" [-]";
		buffer=text.str().c_str();
		this->tableWidget->item(counter-1,1)->setData(Qt::DisplayRole, buffer);
		buffer=number_calc;
		this->tableWidget->item(counter-1,2)->setData(Qt::DisplayRole, buffer);
		buffer=number_total;
		this->tableWidget->item(counter-1,3)->setData(Qt::DisplayRole, buffer);
		if(number_total!=0){
			buffer=((double)number_calc/(double)number_total)*100.0;
			this->tableWidget->item(counter-1,4)->setData(Qt::DisplayRole, buffer);
		}
		counter++;
		//prob total
		buffer=counter;
		this->tableWidget->item(counter-1,0)->setData(Qt::DisplayRole, buffer);
		text.str("");
		text<< "Probability "<<my_hyd_sc.get_ptr_sz(i)->get_name()<<" [-]";
		buffer=text.str().c_str();
		this->tableWidget->item(counter-1,1)->setData(Qt::DisplayRole, buffer);
		buffer=prob_calc;
		this->tableWidget->item(counter-1,2)->setData(Qt::DisplayRole, buffer);
		buffer=prob_total;
		this->tableWidget->item(counter-1,3)->setData(Qt::DisplayRole, buffer);
		if(prob_total!=0.0){
			buffer=(prob_calc/prob_total)*100.0;
			this->tableWidget->item(counter-1,4)->setData(Qt::DisplayRole, buffer);
		}
		counter++;
	}


	//predcited risk
	_risk_predicted_risk total_predicted;
	_risk_predicted_risk not_calculated_predicted;
	Risk_Damage_Predictor::init_risk_predicted_risk_structure(&total_predicted);
	Risk_Damage_Predictor::init_risk_predicted_risk_structure(&not_calculated_predicted);

	list.set_predicted_risk_result2break_sc(ptr_database);
	list.sum_predicted_risk_result_break_sc(&not_calculated_predicted);

	if(not_calculated_predicted.ecn_risk_perc!=0.0){
		total_predicted.ecn_risk=not_calculated_predicted.ecn_risk*100.0/not_calculated_predicted.ecn_risk_perc;
	}
	if(not_calculated_predicted.eco_risk_perc!=0.0){
		total_predicted.eco_risk=not_calculated_predicted.eco_risk*100.0/not_calculated_predicted.eco_risk_perc;
	}
	if(not_calculated_predicted.pop_affected_risk_perc!=0.0){
		total_predicted.pop_affected_risk=not_calculated_predicted.pop_affected_risk*100.0/not_calculated_predicted.pop_affected_risk_perc;
	}
	if(not_calculated_predicted.pop_endangered_risk_perc!=0.0){
		total_predicted.pop_endangered_risk=not_calculated_predicted.pop_endangered_risk*100.0/not_calculated_predicted.pop_endangered_risk_perc;
	}
	if(not_calculated_predicted.pys_without_density_risk_perc!=0.0){
		total_predicted.pys_without_density_risk=not_calculated_predicted.pys_without_density_risk*100.0/not_calculated_predicted.pys_without_density_risk_perc;
	}

	//ecn risk
	buffer=counter;
	this->tableWidget->item(counter-1,0)->setData(Qt::DisplayRole, buffer);
	text.str("");
	text<< "Predicted economical risk [€/a]";
	buffer=text.str().c_str();
	this->tableWidget->item(counter-1,1)->setData(Qt::DisplayRole, buffer);
	buffer=total_predicted.ecn_risk-not_calculated_predicted.ecn_risk;
	this->tableWidget->item(counter-1,2)->setData(Qt::DisplayRole, buffer);
	buffer=total_predicted.ecn_risk;
	this->tableWidget->item(counter-1,3)->setData(Qt::DisplayRole, buffer);
	if(total_predicted.ecn_risk!=0.0){
		buffer=(1.0-(not_calculated_predicted.ecn_risk/total_predicted.ecn_risk))*100.0;
		this->tableWidget->item(counter-1,4)->setData(Qt::DisplayRole, buffer);
	}
	counter++;
	//eco risk
	buffer=counter;
	this->tableWidget->item(counter-1,0)->setData(Qt::DisplayRole, buffer);
	text.str("");
	text<< "Predicted ecological risk [€/a]";
	buffer=text.str().c_str();
	this->tableWidget->item(counter-1,1)->setData(Qt::DisplayRole, buffer);
	buffer=total_predicted.eco_risk-not_calculated_predicted.eco_risk;
	this->tableWidget->item(counter-1,2)->setData(Qt::DisplayRole, buffer);
	buffer=total_predicted.eco_risk;
	this->tableWidget->item(counter-1,3)->setData(Qt::DisplayRole, buffer);
	if(total_predicted.eco_risk!=0.0){
		buffer=(1.0-(not_calculated_predicted.eco_risk/total_predicted.eco_risk))*100.0;
		this->tableWidget->item(counter-1,4)->setData(Qt::DisplayRole, buffer);
	}
	counter++;
	//pop affected risk
	buffer=counter;
	this->tableWidget->item(counter-1,0)->setData(Qt::DisplayRole, buffer);
	text.str("");
	text<< "Predicted people2risk affected person risk [Per/a]";
	buffer=text.str().c_str();
	this->tableWidget->item(counter-1,1)->setData(Qt::DisplayRole, buffer);
	buffer=total_predicted.pop_affected_risk-not_calculated_predicted.pop_affected_risk;
	this->tableWidget->item(counter-1,2)->setData(Qt::DisplayRole, buffer);
	buffer=total_predicted.pop_affected_risk;
	this->tableWidget->item(counter-1,3)->setData(Qt::DisplayRole, buffer);
	if(total_predicted.pop_affected_risk!=0.0){
		buffer=(1.0-(not_calculated_predicted.pop_affected_risk/total_predicted.pop_affected_risk))*100.0;
		this->tableWidget->item(counter-1,4)->setData(Qt::DisplayRole, buffer);
	}
	counter++;
	//pop endangered risk
	buffer=counter;
	this->tableWidget->item(counter-1,0)->setData(Qt::DisplayRole, buffer);
	text.str("");
	text<< "Predicted people2risk endangered person risk [Per/a]";
	buffer=text.str().c_str();
	this->tableWidget->item(counter-1,1)->setData(Qt::DisplayRole, buffer);
	buffer=total_predicted.pop_endangered_risk-not_calculated_predicted.pop_endangered_risk;
	this->tableWidget->item(counter-1,2)->setData(Qt::DisplayRole, buffer);
	buffer=total_predicted.pop_endangered_risk;
	this->tableWidget->item(counter-1,3)->setData(Qt::DisplayRole, buffer);
	if(total_predicted.pop_endangered_risk!=0.0){
		buffer=(1.0-(not_calculated_predicted.pop_endangered_risk/total_predicted.pop_endangered_risk))*100.0;
		this->tableWidget->item(counter-1,4)->setData(Qt::DisplayRole, buffer);
	}
	counter++;
	//pys risk
	buffer=counter;
	this->tableWidget->item(counter-1,0)->setData(Qt::DisplayRole, buffer);
	text.str("");
	text<< "Predicted psycho-social risk [score/a]";
	buffer=text.str().c_str();
	this->tableWidget->item(counter-1,1)->setData(Qt::DisplayRole, buffer);
	buffer=total_predicted.pys_without_density_risk-not_calculated_predicted.pys_without_density_risk;
	this->tableWidget->item(counter-1,2)->setData(Qt::DisplayRole, buffer);
	buffer=total_predicted.pys_without_density_risk;
	this->tableWidget->item(counter-1,3)->setData(Qt::DisplayRole, buffer);
	if(total_predicted.pys_without_density_risk!=0.0){
		buffer=(1.0-(not_calculated_predicted.pys_without_density_risk/total_predicted.pys_without_density_risk))*100.0;
		this->tableWidget->item(counter-1,4)->setData(Qt::DisplayRole, buffer);
	}
	counter++;

	this->tableWidget->resizeColumnsToContents();
}