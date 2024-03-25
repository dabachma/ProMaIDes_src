#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Detailed_Risk_Summary_Wid.h"


//Default constructor
RiskGui_Detailed_Risk_Summary_Wid::RiskGui_Detailed_Risk_Summary_Wid(QWidget *parent): _Sys_Result_Summary_Wid(parent){
	//set number columns
	this->tableWidget->setColumnCount(61);

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
	buffer="ECN risk [monetary]";
	my_list.append(buffer.c_str());
	buffer="with HYD-prob [monetary/a]";
	my_list.append(buffer.c_str());
	buffer="risk-factor [-]";
	my_list.append(buffer.c_str());
	buffer="ECO risk [monetary]";
	my_list.append(buffer.c_str());
	buffer="with HYD-prob [monetary/a]";
	my_list.append(buffer.c_str());
	buffer="risk-factor [-]";
	my_list.append(buffer.c_str());
	buffer="POP affected risk [person]";
	my_list.append(buffer.c_str());
	buffer="with HYD-prob [person/a]";
	my_list.append(buffer.c_str());
	buffer="risk-factor [-]";
	my_list.append(buffer.c_str());
	buffer="POP endangered risk [person]";
	my_list.append(buffer.c_str());
	buffer="with HYD-prob [person/a]";
	my_list.append(buffer.c_str());
	buffer="risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer="PYS risk [score]";
	my_list.append(buffer.c_str());
	buffer="with HYD-prob [score/a]";
	my_list.append(buffer.c_str());
	buffer="risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer="PYS risk (density) [score]";
	my_list.append(buffer.c_str());
	buffer="with HYD-prob [score/a]";
	my_list.append(buffer.c_str());
	buffer="risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer="SC public building risk [score]";
	my_list.append(buffer.c_str());
	buffer="with HYD-prob [score/a]";
	my_list.append(buffer.c_str());
	buffer="risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer="SC ecologic perilous sites/buildings risk [score]";
	my_list.append(buffer.c_str());
	buffer="with HYD-prob [score/a]";
	my_list.append(buffer.c_str());
	buffer="risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer="SC cultural heritage risk [score]";
	my_list.append(buffer.c_str());
	buffer="with HYD-prob [score/a]";
	my_list.append(buffer.c_str());
	buffer="risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer="SC buildings with highly vulnerable person risk [score]";
	my_list.append(buffer.c_str());
	buffer="with HYD-prob [score/a]";
	my_list.append(buffer.c_str());
	buffer="risk-factor [-]";
	my_list.append(buffer.c_str());


	buffer = "CI Population time Sector Electricity 1 [P x s]";
	my_list.append(buffer.c_str());
	buffer = "with HYD-prob [(P x s)/a]";
	my_list.append(buffer.c_str());
	buffer = "risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer = "Population time Sector Information techology 2 [P x s]";
	my_list.append(buffer.c_str());
	buffer = "with HYD-prob [(P x s)/a]";
	my_list.append(buffer.c_str());
	buffer = "risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer = "Population time Sector Water supply 3  [P x s]";
	my_list.append(buffer.c_str());
	buffer = "with HYD-prob [(P x s)/a]";
	my_list.append(buffer.c_str());
	buffer = "risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer = "Population time Sector Water treatment 4  [P x s]";
	my_list.append(buffer.c_str());
	buffer = "with HYD-prob [(P x s)/a]";
	my_list.append(buffer.c_str());
	buffer = "risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer = "Population time Sector Energy 5  [P x s]";
	my_list.append(buffer.c_str());
	buffer = "with HYD-prob [(P x s)/a]";
	my_list.append(buffer.c_str());
	buffer = "risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer = "Population time Cum-Sector Health (10+11)  [P x s]";
	my_list.append(buffer.c_str());
	buffer = "with HYD-prob [(P x s)/a]";
	my_list.append(buffer.c_str());
	buffer = "risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer = "Population time Cum-Sector Social (14+17+18+19) [P x s]";
	my_list.append(buffer.c_str());
	buffer = "with HYD-prob [(P x s)/a]";
	my_list.append(buffer.c_str());
	buffer = "risk-factor [-]";
	my_list.append(buffer.c_str());

	buffer = "Population time Cum-Sector Material (12+13+15+16) [P x s]";
	my_list.append(buffer.c_str());
	buffer = "with HYD-prob [(P x s)/a]";
	my_list.append(buffer.c_str());
	buffer = "risk-factor [-]";
	my_list.append(buffer.c_str());



	buffer="MAX-outflow risk [(m³/s)]";
	my_list.append(buffer.c_str());
	buffer="with HYD-prob [(m³/s)/a]";
	my_list.append(buffer.c_str());
	buffer="risk-factor [-]";
	my_list.append(buffer.c_str());

	this->tableWidget->setHorizontalHeaderLabels(my_list);
	this->risk_type=_risk_type::nobreak_risk;

	this->head_label->setText("Detailed risk results");
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Detailed_Risk_Summary_Wid), _sys_system_modules::RISK_SYS);
}
//Default destructor
RiskGui_Detailed_Risk_Summary_Wid::~RiskGui_Detailed_Risk_Summary_Wid(void){

	
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Detailed_Risk_Summary_Wid), _sys_system_modules::RISK_SYS);
}
//_______
//public
//Set-up the widget
void RiskGui_Detailed_Risk_Summary_Wid::set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id id){

	//detailed results
	QSqlQueryModel model;
	int number=0;

	number=Risk_System::select_data_in_resulttable_detailed(&model, ptr_database, id, this->risk_type);

	if(number==0){
		this->setEnabled(false);
		this->setToolTip("No results found!");
		return;
	}

	//total results
	int number_total=0;
	QSqlQueryModel total;
	if(this->risk_type!=_risk_type::scenario_risk){
		number_total=Risk_System::select_data_in_resulttable(&total, ptr_database, id,this->risk_type);
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
	QColor my_color5(Qt::cyan);
	QColor my_color6(Qt::lightGray);
	QColor my_color7(Qt::darkYellow);

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
			else if(j>=10 && j<=15){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color3);
			}
			else if(j>=16 && j<=21){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color4);
			}
			else if(j>=22 && j<=33){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color6);
			}
			else if(j>=58 && j<=62){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color5);
			}
			else if (j >= 34 && j <= 57) {
				this->tableWidget->item(i, j)->setBackgroundColor(my_color7);
			}
		}
	}



	//read in total results
	_risk_risk_results total_results;
	if(number_total>0){
		//ecn
		total_results.dam_results.ecn_immob_dam=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
		total_results.dam_results.ecn_mob_dam=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
		total_results.dam_results.ecn_immob_dam_5=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::immob_5_quant)).c_str()).toDouble();
		total_results.dam_results.ecn_immob_dam_95=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::immob_95_quant)).c_str()).toDouble();
		total_results.dam_results.ecn_mob_dam_5=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::mob_5_quant)).c_str()).toDouble();
		total_results.dam_results.ecn_mob_dam_95=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::mob_95_quant)).c_str()).toDouble();
		total_results.dam_results.ecn_immob_dam=total_results.dam_results.ecn_immob_dam+total_results.dam_results.ecn_mob_dam;
		//eco
		total_results.dam_results.eco_biotype=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
		total_results.dam_results.eco_soil_erosion=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
		total_results.dam_results.eco_biotype=total_results.dam_results.eco_biotype+total_results.dam_results.eco_soil_erosion;
		//pop
		total_results.dam_results.pop_affected=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
		total_results.dam_results.pop_endangered=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
		//pys
		total_results.dam_results.pys_age_50_59=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
		total_results.dam_results.pys_age_50_59_dens=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
		total_results.dam_results.pys_age_80=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
		total_results.dam_results.pys_age_80_dens=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble();
		total_results.dam_results.pys_female=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::crit_female)).c_str()).toDouble();
		total_results.dam_results.pys_female_dens=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::denscrit_female)).c_str()).toDouble();
		total_results.dam_results.pys_owner=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::crit_owner)).c_str()).toDouble();
		total_results.dam_results.pys_owner_dens=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble();
		total_results.dam_results.pys_age_50_59=total_results.dam_results.pys_age_50_59+
			total_results.dam_results.pys_age_80+total_results.dam_results.pys_female+total_results.dam_results.pys_owner;
		total_results.dam_results.pys_age_50_59_dens=total_results.dam_results.pys_age_50_59_dens+
			total_results.dam_results.pys_age_80_dens+total_results.dam_results.pys_female_dens+total_results.dam_results.pys_owner_dens;
		
		//sc
		total_results.dam_results.sc_pub_build=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_pub_build)).c_str()).toDouble();
		total_results.dam_results.sc_eco_build=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_eco_build)).c_str()).toDouble();
		total_results.dam_results.sc_cult_build=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_cult_build)).c_str()).toDouble();
		total_results.dam_results.sc_person_build=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_person_build)).c_str()).toDouble();

		//ci
		total_results.dam_results.ci_elect_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_elect_pt)).c_str()).toDouble();
		total_results.dam_results.ci_info_tec_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_info_tec_pt)).c_str()).toDouble();
		total_results.dam_results.ci_water_sup_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_water_sup_pt)).c_str()).toDouble();
		total_results.dam_results.ci_water_treat_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_water_treat_pt)).c_str()).toDouble();
		total_results.dam_results.ci_energy_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_energy_pt)).c_str()).toDouble();
		total_results.dam_results.ci_health_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_health_pt)).c_str()).toDouble();
		total_results.dam_results.ci_social_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_social_pt)).c_str()).toDouble();
		total_results.dam_results.ci_mat_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_mat_pt)).c_str()).toDouble();
	
		//max outflow
		total_results.max_outflow=total.record(0).value((Risk_System::table_results->get_column_name(risk_label::max_outflow)).c_str()).toDouble();
	}



	//detailed results
	_risk_risk_results results;
	int id_hyd=0;
	double prob_hyd=0.0;
	string break_str;
	Hyd_Boundary_Szenario_Management hyd_sc;
	hyd_sc.set_systemid(id);
	hyd_sc.set_szenario_per_db(ptr_database);


	if(this->risk_type!=_risk_type::scenario_risk){
		QVariant buffer;
		for(int i=0; i< number; i++){
			buffer=i+1;
			this->tableWidget->item(i,0)->setData(Qt::DisplayRole, buffer);

			id_hyd=model.record(i).value((Risk_System::table_detailed_results->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
			buffer=id_hyd;
			this->tableWidget->item(i,1)->setData(Qt::DisplayRole, buffer);

			buffer=hyd_sc.get_ptr_sz_id_given(id_hyd)->get_name().c_str();
			this->tableWidget->item(i,2)->setData(Qt::DisplayRole, buffer);

            prob_hyd=hyd_sc.get_ptr_sz_id_given(id_hyd)->get_prob_year();
			break_str=model.record(i).value((Risk_System::table_detailed_results->get_column_name(risk_label::sz_break_id)).c_str()).toString().toStdString();
			buffer=break_str.c_str();
			this->tableWidget->item(i,3)->setData(Qt::DisplayRole, buffer);

			
			//ecn
			results.dam_results.ecn_immob_dam=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
			results.dam_results.ecn_mob_dam=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
			results.dam_results.ecn_immob_dam_5=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::immob_5_quant)).c_str()).toDouble();
			results.dam_results.ecn_immob_dam_95=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::immob_95_quant)).c_str()).toDouble();
			results.dam_results.ecn_mob_dam_5=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::mob_5_quant)).c_str()).toDouble();
			results.dam_results.ecn_mob_dam_95=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::mob_95_quant)).c_str()).toDouble();
			
			results.dam_results.ecn_immob_dam=results.dam_results.ecn_immob_dam+results.dam_results.ecn_mob_dam;
			
			buffer=results.dam_results.ecn_immob_dam;
			this->tableWidget->item(i,4)->setData(Qt::DisplayRole, buffer);
			buffer=results.dam_results.ecn_immob_dam*prob_hyd;
			this->tableWidget->item(i,5)->setData(Qt::DisplayRole, buffer);
			if(total_results.dam_results.ecn_immob_dam>0.0){
				buffer=results.dam_results.ecn_immob_dam*prob_hyd/total_results.dam_results.ecn_immob_dam;
				this->tableWidget->item(i,6)->setData(Qt::DisplayRole, buffer);
			}

			//eco
			results.dam_results.eco_biotype=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
			results.dam_results.eco_soil_erosion=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
			results.dam_results.eco_biotype=results.dam_results.eco_biotype+results.dam_results.eco_soil_erosion;
		
			buffer=results.dam_results.eco_biotype;
			this->tableWidget->item(i,7)->setData(Qt::DisplayRole, buffer);
			buffer=results.dam_results.eco_biotype*prob_hyd;
			this->tableWidget->item(i,8)->setData(Qt::DisplayRole, buffer);
			if(total_results.dam_results.eco_biotype>0.0){
				buffer=results.dam_results.eco_biotype*prob_hyd/total_results.dam_results.eco_biotype;
				this->tableWidget->item(i,9)->setData(Qt::DisplayRole, buffer);
			}
			
			//pop
			results.dam_results.pop_affected=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
			buffer=results.dam_results.pop_affected;
			this->tableWidget->item(i,10)->setData(Qt::DisplayRole, buffer);
			buffer=results.dam_results.pop_affected*prob_hyd;
			this->tableWidget->item(i,11)->setData(Qt::DisplayRole, buffer);
			if(total_results.dam_results.pop_affected>0.0){
				buffer=results.dam_results.pop_affected*prob_hyd/total_results.dam_results.pop_affected;
				this->tableWidget->item(i,12)->setData(Qt::DisplayRole, buffer);
			}
			
			
			results.dam_results.pop_endangered=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
			buffer=results.dam_results.pop_endangered;
			this->tableWidget->item(i,13)->setData(Qt::DisplayRole, buffer);
			buffer=results.dam_results.pop_endangered*prob_hyd;
			this->tableWidget->item(i,14)->setData(Qt::DisplayRole, buffer);
			if(total_results.dam_results.pop_endangered>0.0){
				buffer=results.dam_results.pop_endangered*prob_hyd/total_results.dam_results.pop_endangered;
				this->tableWidget->item(i,15)->setData(Qt::DisplayRole, buffer);
			}
			//pys
			results.dam_results.pys_age_50_59=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
			results.dam_results.pys_age_50_59_dens=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
			results.dam_results.pys_age_80=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
			results.dam_results.pys_age_80_dens=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble();
			results.dam_results.pys_female=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::crit_female)).c_str()).toDouble();
			results.dam_results.pys_female_dens=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_female)).c_str()).toDouble();
			results.dam_results.pys_owner=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::crit_owner)).c_str()).toDouble();
			results.dam_results.pys_owner_dens=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble();
			results.dam_results.pys_age_50_59=results.dam_results.pys_age_50_59+
				results.dam_results.pys_age_80+results.dam_results.pys_female+results.dam_results.pys_owner;
			buffer=results.dam_results.pys_age_50_59;
			this->tableWidget->item(i,16)->setData(Qt::DisplayRole, buffer);
			buffer=results.dam_results.pys_age_50_59*prob_hyd;
			this->tableWidget->item(i,17)->setData(Qt::DisplayRole, buffer);
			if(total_results.dam_results.pys_age_50_59>0.0){
				buffer=results.dam_results.pys_age_50_59*prob_hyd/total_results.dam_results.pys_age_50_59;
				this->tableWidget->item(i,18)->setData(Qt::DisplayRole, buffer);
			}

			results.dam_results.pys_age_50_59_dens=results.dam_results.pys_age_50_59_dens+
				results.dam_results.pys_age_80_dens+results.dam_results.pys_female_dens+results.dam_results.pys_owner_dens;

			buffer=results.dam_results.pys_age_50_59_dens;
			this->tableWidget->item(i,19)->setData(Qt::DisplayRole, buffer);
			buffer=results.dam_results.pys_age_50_59_dens*prob_hyd;
			this->tableWidget->item(i,20)->setData(Qt::DisplayRole, buffer);
			if(total_results.dam_results.pys_age_50_59_dens>0.0){
				buffer=results.dam_results.pys_age_50_59_dens*prob_hyd/total_results.dam_results.pys_age_50_59_dens;
				this->tableWidget->item(i,21)->setData(Qt::DisplayRole, buffer);
			}

			//sc
			results.dam_results.sc_pub_build=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::sc_pub_build)).c_str()).toDouble();
			buffer=results.dam_results.sc_pub_build;
			this->tableWidget->item(i,22)->setData(Qt::DisplayRole, buffer);
			buffer=results.dam_results.sc_pub_build*prob_hyd;
			this->tableWidget->item(i,23)->setData(Qt::DisplayRole, buffer);
			if(total_results.dam_results.sc_pub_build>0.0){
				buffer=results.dam_results.sc_pub_build*prob_hyd/total_results.dam_results.sc_pub_build;
				this->tableWidget->item(i,24)->setData(Qt::DisplayRole, buffer);
			}

			results.dam_results.sc_eco_build=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::sc_eco_build)).c_str()).toDouble();
			buffer=results.dam_results.sc_eco_build;
			this->tableWidget->item(i,25)->setData(Qt::DisplayRole, buffer);
			buffer=results.dam_results.sc_eco_build*prob_hyd;
			this->tableWidget->item(i,26)->setData(Qt::DisplayRole, buffer);
			if(total_results.dam_results.sc_eco_build>0.0){
				buffer=results.dam_results.sc_eco_build*prob_hyd/total_results.dam_results.sc_eco_build;
				this->tableWidget->item(i,27)->setData(Qt::DisplayRole, buffer);
			}

			results.dam_results.sc_cult_build=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::sc_cult_build)).c_str()).toDouble();
			buffer=results.dam_results.sc_cult_build;
			this->tableWidget->item(i,28)->setData(Qt::DisplayRole, buffer);
			buffer=results.dam_results.sc_cult_build*prob_hyd;
			this->tableWidget->item(i,29)->setData(Qt::DisplayRole, buffer);
			if(total_results.dam_results.sc_cult_build>0.0){
				buffer=results.dam_results.sc_cult_build*prob_hyd/total_results.dam_results.sc_cult_build;
				this->tableWidget->item(i,30)->setData(Qt::DisplayRole, buffer);
			}

			results.dam_results.sc_person_build=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::sc_person_build)).c_str()).toDouble();
			buffer=results.dam_results.sc_person_build;
			this->tableWidget->item(i,31)->setData(Qt::DisplayRole, buffer);
			buffer=results.dam_results.sc_person_build*prob_hyd;
			this->tableWidget->item(i,32)->setData(Qt::DisplayRole, buffer);
			if(total_results.dam_results.sc_person_build>0.0){
				buffer=results.dam_results.sc_person_build*prob_hyd/total_results.dam_results.sc_person_build;
				this->tableWidget->item(i,33)->setData(Qt::DisplayRole, buffer);
			}

			//ci
			results.dam_results.ci_elect_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_elect_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_elect_pt;
			this->tableWidget->item(i, 34)->setData(Qt::DisplayRole, buffer);
			buffer = results.dam_results.ci_elect_pt*prob_hyd;
			this->tableWidget->item(i, 35)->setData(Qt::DisplayRole, buffer);
			if (total_results.dam_results.ci_elect_pt > 0.0) {
				buffer = results.dam_results.ci_elect_pt*prob_hyd / total_results.dam_results.ci_elect_pt;
				this->tableWidget->item(i, 36)->setData(Qt::DisplayRole, buffer);
			}

			results.dam_results.ci_info_tec_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_info_tec_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_info_tec_pt;
			this->tableWidget->item(i, 37)->setData(Qt::DisplayRole, buffer);
			buffer = results.dam_results.ci_info_tec_pt*prob_hyd;
			this->tableWidget->item(i, 38)->setData(Qt::DisplayRole, buffer);
			if (total_results.dam_results.ci_info_tec_pt > 0.0) {
				buffer = results.dam_results.ci_info_tec_pt*prob_hyd / total_results.dam_results.ci_info_tec_pt;
				this->tableWidget->item(i, 39)->setData(Qt::DisplayRole, buffer);
			}

			results.dam_results.ci_water_sup_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_water_sup_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_water_sup_pt;
			this->tableWidget->item(i, 40)->setData(Qt::DisplayRole, buffer);
			buffer = results.dam_results.ci_water_sup_pt*prob_hyd;
			this->tableWidget->item(i, 41)->setData(Qt::DisplayRole, buffer);
			if (total_results.dam_results.ci_water_sup_pt > 0.0) {
				buffer = results.dam_results.ci_water_sup_pt*prob_hyd / total_results.dam_results.ci_water_sup_pt;
				this->tableWidget->item(i, 42)->setData(Qt::DisplayRole, buffer);
			}

			results.dam_results.ci_water_treat_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_water_treat_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_water_treat_pt;
			this->tableWidget->item(i, 43)->setData(Qt::DisplayRole, buffer);
			buffer = results.dam_results.ci_water_treat_pt*prob_hyd;
			this->tableWidget->item(i, 44)->setData(Qt::DisplayRole, buffer);
			if (total_results.dam_results.ci_water_treat_pt > 0.0) {
				buffer = results.dam_results.ci_water_treat_pt*prob_hyd / total_results.dam_results.ci_water_treat_pt;
				this->tableWidget->item(i, 45)->setData(Qt::DisplayRole, buffer);
			}

			results.dam_results.ci_energy_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_energy_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_energy_pt;
			this->tableWidget->item(i, 46)->setData(Qt::DisplayRole, buffer);
			buffer = results.dam_results.ci_energy_pt*prob_hyd;
			this->tableWidget->item(i, 47)->setData(Qt::DisplayRole, buffer);
			if (total_results.dam_results.ci_energy_pt > 0.0) {
				buffer = results.dam_results.ci_energy_pt*prob_hyd / total_results.dam_results.ci_energy_pt;
				this->tableWidget->item(i, 48)->setData(Qt::DisplayRole, buffer);
			}

			results.dam_results.ci_health_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_health_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_health_pt;
			this->tableWidget->item(i, 49)->setData(Qt::DisplayRole, buffer);
			buffer = results.dam_results.ci_health_pt*prob_hyd;
			this->tableWidget->item(i, 50)->setData(Qt::DisplayRole, buffer);
			if (total_results.dam_results.ci_health_pt > 0.0) {
				buffer = results.dam_results.ci_health_pt*prob_hyd / total_results.dam_results.ci_health_pt;
				this->tableWidget->item(i, 51)->setData(Qt::DisplayRole, buffer);
			}


			results.dam_results.ci_social_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_social_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_social_pt;
			this->tableWidget->item(i, 52)->setData(Qt::DisplayRole, buffer);
			buffer = results.dam_results.ci_social_pt*prob_hyd;
			this->tableWidget->item(i, 53)->setData(Qt::DisplayRole, buffer);
			if (total_results.dam_results.ci_social_pt > 0.0) {
				buffer = results.dam_results.ci_social_pt*prob_hyd / total_results.dam_results.ci_social_pt;
				this->tableWidget->item(i, 54)->setData(Qt::DisplayRole, buffer);
			}


			results.dam_results.ci_mat_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_mat_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_mat_pt;
			this->tableWidget->item(i, 55)->setData(Qt::DisplayRole, buffer);
			buffer = results.dam_results.ci_mat_pt*prob_hyd;
			this->tableWidget->item(i, 56)->setData(Qt::DisplayRole, buffer);
			if (total_results.dam_results.ci_mat_pt > 0.0) {
				buffer = results.dam_results.ci_mat_pt*prob_hyd / total_results.dam_results.ci_mat_pt;
				this->tableWidget->item(i, 57)->setData(Qt::DisplayRole, buffer);
			}



			//max outflow
			results.max_outflow=model.record(i).value((Risk_System::table_detailed_results->get_column_name(risk_label::max_outflow)).c_str()).toDouble();
			buffer=results.max_outflow;
			this->tableWidget->item(i,58)->setData(Qt::DisplayRole, buffer);
			buffer=results.max_outflow*prob_hyd;
			this->tableWidget->item(i,59)->setData(Qt::DisplayRole, buffer);
			if(total_results.max_outflow>0.0){
				buffer=results.max_outflow*prob_hyd/total_results.max_outflow;
				this->tableWidget->item(i,60)->setData(Qt::DisplayRole, buffer);
			}

		}
	}
	else{
				QVariant buffer;
		for(int i=0; i< number; i++){
			buffer=i+1;
			this->tableWidget->item(i,0)->setData(Qt::DisplayRole, buffer);

			id_hyd=model.record(i).value((Risk_System::table_detailed_results->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
			buffer=id_hyd;
			this->tableWidget->item(i,1)->setData(Qt::DisplayRole, buffer);

			buffer=hyd_sc.get_ptr_sz_id_given(id_hyd)->get_name().c_str();
			this->tableWidget->item(i,2)->setData(Qt::DisplayRole, buffer);

			prob_hyd=hyd_sc.get_ptr_sz_id_given(id_hyd)->get_prob_total();
			break_str=model.record(i).value((Risk_System::table_detailed_results->get_column_name(risk_label::sz_break_id)).c_str()).toString().toStdString();
			buffer=break_str.c_str();
			this->tableWidget->item(i,3)->setData(Qt::DisplayRole, buffer);

			
			//ecn
			results.dam_results.ecn_immob_dam=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
			results.dam_results.ecn_mob_dam=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
			results.dam_results.ecn_immob_dam_5=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::immob_5_quant)).c_str()).toDouble();
			results.dam_results.ecn_immob_dam_95=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::immob_95_quant)).c_str()).toDouble();
			results.dam_results.ecn_mob_dam_5=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::mob_5_quant)).c_str()).toDouble();
			results.dam_results.ecn_mob_dam_95=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::mob_95_quant)).c_str()).toDouble();
			
			results.dam_results.ecn_immob_dam=results.dam_results.ecn_immob_dam+results.dam_results.ecn_mob_dam;
			
			buffer=results.dam_results.ecn_immob_dam;
			this->tableWidget->item(i,5)->setData(Qt::DisplayRole, buffer);
	

			//eco
			results.dam_results.eco_biotype=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
			results.dam_results.eco_soil_erosion=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
			results.dam_results.eco_biotype=results.dam_results.eco_biotype+results.dam_results.eco_soil_erosion;
		
			buffer=results.dam_results.eco_biotype;
			this->tableWidget->item(i,8)->setData(Qt::DisplayRole, buffer);

			
			//pop
			results.dam_results.pop_affected=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
			buffer=results.dam_results.pop_affected;
			this->tableWidget->item(i,11)->setData(Qt::DisplayRole, buffer);

			
			
			results.dam_results.pop_endangered=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
			buffer=results.dam_results.pop_endangered;
			this->tableWidget->item(i,14)->setData(Qt::DisplayRole, buffer);

			//pys
			results.dam_results.pys_age_50_59=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
			results.dam_results.pys_age_50_59_dens=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
			results.dam_results.pys_age_80=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
			results.dam_results.pys_age_80_dens=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble();
			results.dam_results.pys_female=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::crit_female)).c_str()).toDouble();
			results.dam_results.pys_female_dens=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_female)).c_str()).toDouble();
			results.dam_results.pys_owner=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::crit_owner)).c_str()).toDouble();
			results.dam_results.pys_owner_dens=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble();
			results.dam_results.pys_age_50_59=results.dam_results.pys_age_50_59+
				results.dam_results.pys_age_80+results.dam_results.pys_female+results.dam_results.pys_owner;
			buffer=results.dam_results.pys_age_50_59;
			this->tableWidget->item(i,16)->setData(Qt::DisplayRole, buffer);


			results.dam_results.pys_age_50_59_dens=results.dam_results.pys_age_50_59_dens+
				results.dam_results.pys_age_80_dens+results.dam_results.pys_female_dens+results.dam_results.pys_owner_dens;

			buffer=results.dam_results.pys_age_50_59_dens;
			this->tableWidget->item(i,20)->setData(Qt::DisplayRole, buffer);


			//sc
			results.dam_results.sc_pub_build=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::sc_pub_build)).c_str()).toDouble();
			buffer=results.dam_results.sc_pub_build;
			this->tableWidget->item(i,23)->setData(Qt::DisplayRole, buffer);

			results.dam_results.sc_eco_build=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::sc_eco_build)).c_str()).toDouble();
			buffer=results.dam_results.sc_eco_build;
			this->tableWidget->item(i,26)->setData(Qt::DisplayRole, buffer);

			results.dam_results.sc_cult_build=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::sc_cult_build)).c_str()).toDouble();
			buffer=results.dam_results.sc_cult_build;
			this->tableWidget->item(i,29)->setData(Qt::DisplayRole, buffer);

			results.dam_results.sc_person_build=model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::sc_person_build)).c_str()).toDouble();
			buffer=results.dam_results.sc_person_build;
			this->tableWidget->item(i,32)->setData(Qt::DisplayRole, buffer);


			//ci
			results.dam_results.ci_elect_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_elect_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_elect_pt;
			this->tableWidget->item(i, 35)->setData(Qt::DisplayRole, buffer);

			results.dam_results.ci_info_tec_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_info_tec_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_info_tec_pt;
			this->tableWidget->item(i, 38)->setData(Qt::DisplayRole, buffer);


			results.dam_results.ci_water_sup_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_water_sup_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_water_sup_pt;
			this->tableWidget->item(i, 41)->setData(Qt::DisplayRole, buffer);


			results.dam_results.ci_water_treat_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_water_treat_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_water_treat_pt;
			this->tableWidget->item(i, 44)->setData(Qt::DisplayRole, buffer);


			results.dam_results.ci_energy_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_energy_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_energy_pt;
			this->tableWidget->item(i, 47)->setData(Qt::DisplayRole, buffer);

			results.dam_results.ci_health_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_health_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_health_pt;
			this->tableWidget->item(i, 50)->setData(Qt::DisplayRole, buffer);


			results.dam_results.ci_social_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_social_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_social_pt;
			this->tableWidget->item(i, 53)->setData(Qt::DisplayRole, buffer);


			results.dam_results.ci_mat_pt = model.record(i).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_mat_pt)).c_str()).toDouble();
			buffer = results.dam_results.ci_mat_pt;
			this->tableWidget->item(i, 56)->setData(Qt::DisplayRole, buffer);



			//max outflow
			results.max_outflow=model.record(i).value((Risk_System::table_detailed_results->get_column_name(risk_label::max_outflow)).c_str()).toDouble();
			buffer=results.max_outflow;
			this->tableWidget->item(i,59)->setData(Qt::DisplayRole, buffer);


		}

	}


	this->tableWidget->resizeColumnsToContents();

}
//Set the risk type
void RiskGui_Detailed_Risk_Summary_Wid::set_risk_type(const _risk_type type){
	this->risk_type=type;
}
