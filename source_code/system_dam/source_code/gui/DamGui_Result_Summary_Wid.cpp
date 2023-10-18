#include "Dam_Headers_Precompiled.h"
//#include "DamGui_Result_Summary_Wid.h"


//Default constructor
DamGui_Result_Summary_Wid::DamGui_Result_Summary_Wid(QWidget *parent): _Sys_Result_Summary_Wid(parent){


	//set number columns
	this->tableWidget->setColumnCount(34);

	QStringList my_list;
	string buffer;
	buffer="No";
	my_list.append(buffer.c_str());
	buffer="HYD-boundary id";
	my_list.append(buffer.c_str());
	buffer="HYD-boundary name";
	my_list.append(buffer.c_str());
	buffer="Break scenario name";

	my_list.append(buffer.c_str());
	buffer="ECN immobile damages [monetary]";
	my_list.append(buffer.c_str());
	buffer="ECN mobile damages [monetary]";
	my_list.append(buffer.c_str());
	buffer="ECN total damages [monetary]";

	my_list.append(buffer.c_str());
	buffer="ECO biotope-type damages [monetary]";
	my_list.append(buffer.c_str());
	buffer="ECO soil-erosion damages [monetary]";
	my_list.append(buffer.c_str());
	buffer="ECO total damages [monetary]";
	my_list.append(buffer.c_str());

	buffer="POP affected [person]";
	my_list.append(buffer.c_str());
	buffer="POP endangered [person]";
	my_list.append(buffer.c_str());

	buffer="PYS crit 50<age<59 [score]";
	my_list.append(buffer.c_str());
	buffer="PYS crit 50<age<59 (density) [score]";
	my_list.append(buffer.c_str());
	buffer="PYS crit 80<age [score]";
	my_list.append(buffer.c_str());
	buffer="PYS crit 80<age (density) [score]";
	my_list.append(buffer.c_str());
	buffer="PYS crit female gender [score]";
	my_list.append(buffer.c_str());
	buffer="PYS crit female gender (density) [score]";
	my_list.append(buffer.c_str());
	buffer="PYS crit home owner [score]";
	my_list.append(buffer.c_str());
	buffer="PYS crit home owner (density) [score]";
	my_list.append(buffer.c_str());

	buffer="PYS total [score]";
	my_list.append(buffer.c_str());
	buffer="PYS total (density) [score]";
	my_list.append(buffer.c_str());

	buffer="SC Public buildings [score]";
	my_list.append(buffer.c_str());
	buffer="SC Ecologic perilous sites/buildings [score]";
	my_list.append(buffer.c_str());
	buffer="SC Cultural heritage [score]";
	my_list.append(buffer.c_str());
	buffer="SC Buildings with highly vulnerable person [score]";
	my_list.append(buffer.c_str());


	buffer = "CI Population time Sector Electricity 1 [P x s]";
	my_list.append(buffer.c_str());
	buffer = "Population time Sector Information techology 2 [P x s]";
	my_list.append(buffer.c_str());
	buffer = "Population time Sector Water supply 3 [P x s]";
	my_list.append(buffer.c_str());

	buffer = "Population time Sector Water treatment 4 [P x s]";
	my_list.append(buffer.c_str());

	buffer = "Population time Sector Energy 5 [P x s]";
	my_list.append(buffer.c_str());

	buffer = "Population time Cum-Sector Health (10+11) [P x s]";
	my_list.append(buffer.c_str());

	buffer = "Population time Cum-Sector Social (14+17+18+19) [P x s]";
	my_list.append(buffer.c_str());

	buffer = "Population time Cum-Sector Material (12+13+15+16) [P x s]";
	my_list.append(buffer.c_str());


	this->tableWidget->setHorizontalHeaderLabels(my_list);

	this->head_label->setText("Detailed damage results");

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Result_Summary_Wid), _sys_system_modules::DAM_SYS);
}
//Default destructor
DamGui_Result_Summary_Wid::~DamGui_Result_Summary_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Result_Summary_Wid), _sys_system_modules::DAM_SYS);

}
//_______
//public
//Set-up the widget
void DamGui_Result_Summary_Wid::set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id id){

	//detailed results
	QSqlQueryModel model;
	int number=0;

	number=Dam_Damage_System::select_data_in_erg_table(&model, ptr_database, id, this->risk_id);

	if(number==0){
		this->setEnabled(false);
		this->setToolTip("No results found!");
		return;
	}

	

	this->tableWidget->setRowCount(number);
	//init table widget
	QTableWidgetItem *item;
	Qt::ItemFlags my_flag;
	my_flag=my_flag|(Qt::ItemIsEnabled);
	my_flag=my_flag|(Qt::ItemIsSelectable);
	my_flag=my_flag|(Qt::ItemIsDragEnabled);


	QColor my_color1(Qt::red);
	QColor my_color2(Qt::green);
	QColor my_color3(Qt::magenta);
	QColor my_color4(Qt::darkMagenta);
	QColor my_color5(Qt::lightGray);
	QColor my_color6(Qt::darkYellow);


	for(int i=0; i<this->tableWidget->rowCount(); i++){
		for(int j=0; j<this->tableWidget->columnCount(); j++){
			item=new QTableWidgetItem;
			item->setFlags(my_flag);
			this->tableWidget->setItem(i,j,item);
			this->tableWidget->item(i,j)->setData(Qt::DisplayRole, "-");

			if(j>=4 && j<=6){
				//ecn
				this->tableWidget->item(i,j)->setBackgroundColor(my_color1);	

			}
			else if(j>=7 && j<=9){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color2);
			}
			else if(j>=10 && j<=11){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color3);
			}
			else if(j>=12 && j<=21){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color4);

			}
			else if(j>=22 && j<=25){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color5);

			}
			else if (j >= 26 && j <= 33) {
				this->tableWidget->item(i, j)->setBackgroundColor(my_color6);

			}
	
		}
	}

	//detailed results
	_dam_damage_results dam_results;
	int id_hyd=0;
	double sum=0.0;
	string break_str;
	Hyd_Boundary_Szenario_Management hyd_sc;
	hyd_sc.set_systemid(id);
	hyd_sc.set_szenario_per_db(ptr_database);


	QVariant buffer;
	for(int i=0; i< number; i++){
		buffer=i+1;
		this->tableWidget->item(i,0)->setData(Qt::DisplayRole, buffer);

		id_hyd=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
		buffer=id_hyd;
		this->tableWidget->item(i,1)->setData(Qt::DisplayRole, buffer);

		buffer=hyd_sc.get_ptr_sz_id_given(id_hyd)->get_name().c_str();
		this->tableWidget->item(i,2)->setData(Qt::DisplayRole, buffer);

		break_str=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(risk_label::sz_break_id)).c_str()).toString().toStdString();
		buffer=break_str.c_str();
		this->tableWidget->item(i,3)->setData(Qt::DisplayRole, buffer);
	
		//ecn
		dam_results.ecn_immob_dam=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
		dam_results.ecn_mob_dam=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
		sum=dam_results.ecn_immob_dam+dam_results.ecn_mob_dam;
		
		buffer=dam_results.ecn_immob_dam;
		this->tableWidget->item(i,4)->setData(Qt::DisplayRole, buffer);
		buffer=dam_results.ecn_mob_dam;
		this->tableWidget->item(i,5)->setData(Qt::DisplayRole, buffer);
		buffer=sum;
		this->tableWidget->item(i,6)->setData(Qt::DisplayRole, buffer);


		//eco
		dam_results.eco_biotype=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
		dam_results.eco_soil_erosion=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
		sum=dam_results.eco_biotype+dam_results.eco_soil_erosion;
	
		buffer=dam_results.eco_biotype;
		this->tableWidget->item(i,7)->setData(Qt::DisplayRole, buffer);
		buffer=dam_results.eco_soil_erosion;
		this->tableWidget->item(i,8)->setData(Qt::DisplayRole, buffer);
		buffer=sum;
		this->tableWidget->item(i,9)->setData(Qt::DisplayRole, buffer);

		
		//pop
		dam_results.pop_affected=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
		buffer=dam_results.pop_affected;
		this->tableWidget->item(i,10)->setData(Qt::DisplayRole, buffer);
		dam_results.pop_endangered=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
		buffer=dam_results.pop_endangered;
		this->tableWidget->item(i,11)->setData(Qt::DisplayRole, buffer);

		//pys
		dam_results.pys_age_50_59=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
		buffer=dam_results.pys_age_50_59;
		this->tableWidget->item(i,12)->setData(Qt::DisplayRole, buffer);
		dam_results.pys_age_50_59_dens=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
		buffer=dam_results.pys_age_50_59_dens;
		this->tableWidget->item(i,13)->setData(Qt::DisplayRole, buffer);

		dam_results.pys_age_80=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
		buffer=dam_results.pys_age_80;
		this->tableWidget->item(i,14)->setData(Qt::DisplayRole, buffer);
		dam_results.pys_age_80_dens=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble();
		buffer=dam_results.pys_age_80_dens;
		this->tableWidget->item(i,15)->setData(Qt::DisplayRole, buffer);

		dam_results.pys_female=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_female)).c_str()).toDouble();
		buffer=dam_results.pys_female;
		this->tableWidget->item(i,16)->setData(Qt::DisplayRole, buffer);
		dam_results.pys_female_dens=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_female)).c_str()).toDouble();
		buffer=dam_results.pys_female_dens;
		this->tableWidget->item(i,17)->setData(Qt::DisplayRole, buffer);

		dam_results.pys_owner=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_owner)).c_str()).toDouble();
		buffer=dam_results.pys_owner;
		this->tableWidget->item(i,18)->setData(Qt::DisplayRole, buffer);
		dam_results.pys_owner_dens=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble();
		buffer=dam_results.pys_owner_dens;
		this->tableWidget->item(i,19)->setData(Qt::DisplayRole, buffer);

		sum=dam_results.pys_age_50_59+
			dam_results.pys_age_80+dam_results.pys_female+dam_results.pys_owner;
		buffer=sum;
		this->tableWidget->item(i,20)->setData(Qt::DisplayRole, buffer);	
		sum=dam_results.pys_age_50_59_dens+
			dam_results.pys_age_80_dens+dam_results.pys_female_dens+dam_results.pys_owner_dens;
		buffer=sum;
		this->tableWidget->item(i,21)->setData(Qt::DisplayRole, buffer);

		//sc
		dam_results.sc_pub_build=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_pub_build)).c_str()).toDouble();
		buffer=dam_results.sc_pub_build;
		this->tableWidget->item(i,22)->setData(Qt::DisplayRole, buffer);

		dam_results.sc_eco_build=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_eco_build)).c_str()).toDouble();
		buffer=dam_results.sc_eco_build;
		this->tableWidget->item(i,23)->setData(Qt::DisplayRole, buffer);

		dam_results.sc_cult_build=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_cult_build)).c_str()).toDouble();
		buffer=dam_results.sc_cult_build;
		this->tableWidget->item(i,24)->setData(Qt::DisplayRole, buffer);

		dam_results.sc_person_build=model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_person_build)).c_str()).toDouble();
		buffer=dam_results.sc_person_build;
		this->tableWidget->item(i,25)->setData(Qt::DisplayRole, buffer);

		//ci

		dam_results.ci_elect_pt = model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_elect_pt)).c_str()).toDouble();
		buffer = dam_results.ci_elect_pt;
		this->tableWidget->item(i, 26)->setData(Qt::DisplayRole, buffer);

		dam_results.ci_info_tec_pt = model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_info_tec_pt)).c_str()).toDouble();
		buffer = dam_results.ci_info_tec_pt;
		this->tableWidget->item(i, 27)->setData(Qt::DisplayRole, buffer);

		dam_results.ci_water_sup_pt = model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_sup_pt)).c_str()).toDouble();
		buffer = dam_results.ci_water_sup_pt;
		this->tableWidget->item(i, 28)->setData(Qt::DisplayRole, buffer);

		dam_results.ci_water_treat_pt = model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_treat_pt)).c_str()).toDouble();
		buffer = dam_results.ci_water_treat_pt;
		this->tableWidget->item(i, 29)->setData(Qt::DisplayRole, buffer);

		dam_results.ci_energy_pt = model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_energy_pt)).c_str()).toDouble();
		buffer = dam_results.ci_energy_pt;
		this->tableWidget->item(i, 30)->setData(Qt::DisplayRole, buffer);

		dam_results.ci_health_pt = model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_health_pt)).c_str()).toDouble();
		buffer = dam_results.ci_health_pt;
		this->tableWidget->item(i, 31)->setData(Qt::DisplayRole, buffer);

		dam_results.ci_social_pt = model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_social_pt)).c_str()).toDouble();
		buffer = dam_results.ci_social_pt;
		this->tableWidget->item(i, 32)->setData(Qt::DisplayRole, buffer);

		dam_results.ci_mat_pt = model.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_mat_pt)).c_str()).toDouble();
		buffer = dam_results.ci_mat_pt;
		this->tableWidget->item(i, 33)->setData(Qt::DisplayRole, buffer);

	}

	this->tableWidget->resizeColumnsToContents();

}
//Set the risk approach identifier
void DamGui_Result_Summary_Wid::set_risk_approach_id(const string id_str){
	this->risk_id=id_str;
}