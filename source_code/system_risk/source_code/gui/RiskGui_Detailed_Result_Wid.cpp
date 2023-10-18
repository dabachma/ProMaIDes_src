#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Detailed_Result_Wid.h"


//Default constructor
RiskGui_Detailed_Result_Wid::RiskGui_Detailed_Result_Wid(QWidget *parent): _Sys_Data_Wid(parent){
	this->setupUi(this);
	this->area2print=this->scrollAreaWidgetContents;
	this->head_label->setText("Detailed risk results ");
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Detailed_Result_Wid), _sys_system_modules::RISK_SYS);
}
//Default destructor
RiskGui_Detailed_Result_Wid::~RiskGui_Detailed_Result_Wid(void){

		//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Detailed_Result_Wid), _sys_system_modules::RISK_SYS);
}
//_______
//public
//Set tooltip of the header
void RiskGui_Detailed_Result_Wid::set_header_tooltip(const string tooltip, QTreeWidgetItem *item){
	this->head_label->setToolTip(tooltip.c_str());
	this->set_identifier(tooltip, item);
	string buffer;
	buffer=_Sys_Data_Wid::get_last_string_string_id(tooltip);
	this->label_id->setText(buffer.c_str());
}
//Set-up the widget
void RiskGui_Detailed_Result_Wid::set_up_widget(QSqlDatabase *ptr_database,  const int glob_id, const _sys_system_id id , const _risk_type type){


	//detailed results
	QSqlQueryModel model;
	int number=0;

	number=Risk_System::select_data_in_resulttable_detailed(&model, ptr_database, glob_id);

	if(number==0){
		this->setEnabled(false);
		this->setToolTip("No results found!");
		return;
	}

	//total results
	int number_total=0;
	QSqlQueryModel total;
	if(type!=_risk_type::scenario_risk){
		number_total=Risk_System::select_data_in_resulttable(&total, ptr_database, id, type);
	}
	//hyd-boundary prob
	int id_hyd=0;
	double prob_hyd=0.0;
	Hyd_Boundary_Szenario_Management hyd_sc;
	hyd_sc.set_systemid(id);
	hyd_sc.set_szenario_per_db(ptr_database);

	id_hyd=model.record(0).value((Risk_System::table_detailed_results->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
	prob_hyd=hyd_sc.get_ptr_sz_id_given(id_hyd)->get_prob_total();


	//detailed results
	_risk_risk_results results;
	//ecn
	results.dam_results.ecn_immob_dam=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
	results.dam_results.ecn_mob_dam=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
	results.dam_results.ecn_immob_dam_5=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::immob_5_quant)).c_str()).toDouble();
	results.dam_results.ecn_immob_dam_95=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::immob_95_quant)).c_str()).toDouble();
	results.dam_results.ecn_mob_dam_5=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::mob_5_quant)).c_str()).toDouble();
	results.dam_results.ecn_mob_dam_95=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::mob_95_quant)).c_str()).toDouble();
	//eco
	results.dam_results.eco_biotype=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
	results.dam_results.eco_soil_erosion=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
	//pop
	results.dam_results.pop_affected=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
	results.dam_results.pop_endangered=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
	//pys
	results.dam_results.pys_age_50_59=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
	results.dam_results.pys_age_50_59_dens=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
	results.dam_results.pys_age_80=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
	results.dam_results.pys_age_80_dens=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble();
	results.dam_results.pys_female=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::crit_female)).c_str()).toDouble();
	results.dam_results.pys_female_dens=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_female)).c_str()).toDouble();
	results.dam_results.pys_owner=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::crit_owner)).c_str()).toDouble();
	results.dam_results.pys_owner_dens=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble();
	
	//sc
	results.dam_results.sc_pub_build=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::sc_pub_build)).c_str()).toDouble();
	results.dam_results.sc_eco_build=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::sc_eco_build)).c_str()).toDouble();
	results.dam_results.sc_cult_build=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::sc_cult_build)).c_str()).toDouble();
	results.dam_results.sc_person_build=model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::sc_person_build)).c_str()).toDouble();

	//ci
	results.dam_results.ci_elect_pt = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_elect_pt)).c_str()).toDouble();
	results.dam_results.ci_info_tec_pt = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_info_tec_pt)).c_str()).toDouble();
	results.dam_results.ci_water_sup_pt = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_water_sup_pt)).c_str()).toDouble();
	results.dam_results.ci_water_treat_pt = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_water_treat_pt)).c_str()).toDouble();
	results.dam_results.ci_energy_pt = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_energy_pt)).c_str()).toDouble();

	results.dam_results.ci_health_pt = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_health_pt)).c_str()).toDouble();
	results.dam_results.ci_social_pt = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_social_pt)).c_str()).toDouble();
	results.dam_results.ci_mat_pt = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_mat_pt)).c_str()).toDouble();

	results.dam_results.ci_elect_p = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_elect_p)).c_str()).toDouble();
	results.dam_results.ci_info_tec_p = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_info_tec_p)).c_str()).toDouble();
	results.dam_results.ci_water_sup_p = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_water_sup_p)).c_str()).toDouble();
	results.dam_results.ci_water_treat_p = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_water_treat_p)).c_str()).toDouble();
	results.dam_results.ci_energy_p = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_energy_p)).c_str()).toDouble();

	results.dam_results.ci_health_p = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_health_p)).c_str()).toDouble();
	results.dam_results.ci_social_p = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_social_p)).c_str()).toDouble();
	results.dam_results.ci_mat_p = model.record(0).value((Risk_System::table_detailed_results->get_column_name(dam_label::ci_mat_p)).c_str()).toDouble();







	
	//max outflow
	results.max_outflow=model.record(0).value((Risk_System::table_detailed_results->get_column_name(risk_label::max_outflow)).c_str()).toDouble();

	//total results
	_risk_risk_results total_results;
	if(number_total>0){
		//ecn
		total_results.dam_results.ecn_immob_dam=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
		total_results.dam_results.ecn_mob_dam=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
		total_results.dam_results.ecn_immob_dam_5=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::immob_5_quant)).c_str()).toDouble();
		total_results.dam_results.ecn_immob_dam_95=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::immob_95_quant)).c_str()).toDouble();
		total_results.dam_results.ecn_mob_dam_5=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::mob_5_quant)).c_str()).toDouble();
		total_results.dam_results.ecn_mob_dam_95=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::mob_95_quant)).c_str()).toDouble();
		//eco
		total_results.dam_results.eco_biotype=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
		total_results.dam_results.eco_soil_erosion=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
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
		//sc
		total_results.dam_results.sc_pub_build=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_pub_build)).c_str()).toDouble();
		total_results.dam_results.sc_eco_build=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_eco_build)).c_str()).toDouble();
		total_results.dam_results.sc_cult_build=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_cult_build)).c_str()).toDouble();
		total_results.dam_results.sc_person_build=total.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_person_build)).c_str()).toDouble();
	

		//ci
		total_results.dam_results.ci_elect_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_elect_pt)).c_str()).toDouble();
		total_results.dam_results.ci_info_tec_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_info_tec_pt)).c_str()).toDouble();
		total_results.dam_results.ci_water_sup_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_water_sup_pt)).c_str()).toDouble();
		total_results.dam_results.ci_water_treat_pt= total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_water_treat_pt)).c_str()).toDouble();
		total_results.dam_results.ci_energy_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_energy_pt)).c_str()).toDouble();

		total_results.dam_results.ci_health_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_health_pt)).c_str()).toDouble();
		total_results.dam_results.ci_social_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_social_pt)).c_str()).toDouble();
		total_results.dam_results.ci_mat_pt = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_mat_pt)).c_str()).toDouble();

		total_results.dam_results.ci_elect_p = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_elect_p)).c_str()).toDouble();
		total_results.dam_results.ci_info_tec_p = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_info_tec_p)).c_str()).toDouble();
		total_results.dam_results.ci_water_sup_p = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_water_sup_p)).c_str()).toDouble();
		total_results.dam_results.ci_water_treat_p = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_water_treat_p)).c_str()).toDouble();
		total_results.dam_results.ci_energy_p = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_energy_p)).c_str()).toDouble();

		total_results.dam_results.ci_health_p = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_health_p)).c_str()).toDouble();
		total_results.dam_results.ci_social_p = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_social_p)).c_str()).toDouble();
		total_results.dam_results.ci_mat_p = total.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_mat_p)).c_str()).toDouble();

		//max outflow
		total_results.max_outflow=total.record(0).value((Risk_System::table_results->get_column_name(risk_label::max_outflow)).c_str()).toDouble();
	}



	double sum=0.0;
	double sum_dens=0.0;
	ostringstream buff_stream;

		
	//scenario base risk
	if(type==_risk_type::scenario_risk){
		buff_stream<< FORMAT_FIXED_REAL<< P(1);
		//ecn
		buff_stream << results.dam_results.ecn_immob_dam;
		this->lineEdit_immobil_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_immobil_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.ecn_mob_dam;
		this->lineEdit_mobil_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_mobil_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.ecn_immob_dam_5;
		this->lineEdit_immobil_5_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_immobil_5_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.ecn_immob_dam_95;
		this->lineEdit_immobil_95_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_immobil_95_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.ecn_mob_dam_5;
		this->lineEdit_mobil_5_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_mobil_5_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.ecn_mob_dam_95;
		this->lineEdit_mobil_95_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_mobil_95_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << (results.dam_results.ecn_mob_dam + results.dam_results.ecn_immob_dam);
		this->lineEdit_total_ecn_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_total_ecn_hyd->setToolTip("Total risk [.../a]");

		//eco
		buff_stream.str("");
		buff_stream << results.dam_results.eco_biotype;
		this->lineEdit_btype_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_btype_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.eco_soil_erosion;
		this->lineEdit_soil_ero_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_soil_ero_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << (results.dam_results.eco_soil_erosion + results.dam_results.eco_biotype);
		this->lineEdit_total_eco_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_total_eco_hyd->setToolTip("Total risk [.../a]");


		//total
		buff_stream.str("");
		buff_stream << (results.dam_results.eco_soil_erosion + results.dam_results.eco_biotype +
			results.dam_results.ecn_immob_dam+results.dam_results.ecn_mob_dam);
		this->lineEdit_total_sum_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_total_sum_hyd->setToolTip("Total risk [.../a]");


		//pop
		buff_stream.str("");
		buff_stream << results.dam_results.pop_affected;
		this->lineEdit_pop_aff_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_pop_aff_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.pop_endangered;
		this->lineEdit_pop_end_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_pop_end_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");

		//pys
		buff_stream.str("");
		buff_stream << results.dam_results.pys_age_50_59 <<" ("<< results.dam_results.pys_age_50_59_dens<<")";
		this->lineEdit_pys_50_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_pys_50_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.pys_age_80 <<" ("<< results.dam_results.pys_age_80_dens<<")";
		this->lineEdit_pys_80_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_pys_80_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.pys_female <<" ("<< results.dam_results.pys_female_dens<<")";
		this->lineEdit_pys_female_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_pys_female_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.pys_owner <<" ("<< results.dam_results.pys_owner_dens<<")";
		this->lineEdit_pys_owner_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_pys_owner_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		sum=results.dam_results.pys_owner+results.dam_results.pys_age_50_59+results.dam_results.pys_age_80+
			results.dam_results.pys_female;
		sum_dens=results.dam_results.pys_owner_dens+results.dam_results.pys_age_50_59_dens+results.dam_results.pys_age_80_dens+
			results.dam_results.pys_female_dens;
		buff_stream << sum <<" ("<< sum_dens<<")";
		this->lineEdit_pys_total_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_pys_total_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");

		//sc
		buff_stream << results.dam_results.sc_pub_build;
		this->lineEdit_sc_pub_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_sc_pub_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.sc_eco_build;
		this->lineEdit_sc_eco_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_sc_eco_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.sc_cult_build;
		this->lineEdit_sc_cult_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_sc_cult_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");
		buff_stream << results.dam_results.sc_person_build;
		this->lineEdit_sc_person_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_sc_person_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");


		//ci
		buff_stream << results.dam_results.ci_elect_pt;
		this->lineEdit_ci_elect_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_ci_elect_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");

		buff_stream << results.dam_results.ci_info_tec_pt;
		this->lineEdit_ci_info_tec_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_ci_info_tec_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");

		buff_stream << results.dam_results.ci_water_sup_pt;
		this->lineEdit_ci_water_sup_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_ci_water_sup_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");

		buff_stream << results.dam_results.ci_water_treat_pt;
		this->lineEdit_ci_water_treat_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_ci_water_treat_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");

		buff_stream << results.dam_results.ci_energy_pt;
		this->lineEdit_ci_energy_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_ci_energy_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");


		buff_stream << results.dam_results.ci_health_pt;
		this->lineEdit_ci_health_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_ci_health_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");

		buff_stream << results.dam_results.ci_social_pt;
		this->lineEdit_ci_social_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_ci_social_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");

		buff_stream << results.dam_results.ci_mat_pt;
		this->lineEdit_ci_mat_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_ci_mat_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");


		//max outflow
		buff_stream << results.max_outflow;
		this->lineEdit_outflow_hyd->setText(buff_stream.str().c_str());
		this->lineEdit_outflow_hyd->setToolTip("Total risk [.../a]");
		buff_stream.str("");

	}
	//catchment base risk an nobreak risk
	else{
		buff_stream<< FORMAT_FIXED_REAL<< P(1);
		//ecn
		buff_stream << results.dam_results.ecn_immob_dam;
		this->lineEdit_immobil->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ecn_immob_dam*prob_hyd;
		this->lineEdit_immobil_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if(total_results.dam_results.ecn_immob_dam>0.0){
			buff_stream << results.dam_results.ecn_immob_dam*prob_hyd/total_results.dam_results.ecn_immob_dam;
			this->lineEdit_immobil_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream << results.dam_results.ecn_mob_dam;
		this->lineEdit_mobil->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ecn_mob_dam*prob_hyd;
		this->lineEdit_mobil_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if(total_results.dam_results.ecn_mob_dam>0.0){
			buff_stream << results.dam_results.ecn_mob_dam*prob_hyd/total_results.dam_results.ecn_mob_dam;
			this->lineEdit_mobil_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream << results.dam_results.ecn_immob_dam_5;
		this->lineEdit_immobil_5->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ecn_immob_dam_5*prob_hyd;
		this->lineEdit_immobil_5_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if(total_results.dam_results.ecn_immob_dam_5>0.0){
			buff_stream << results.dam_results.ecn_immob_dam_5*prob_hyd/total_results.dam_results.ecn_immob_dam_5;
			this->lineEdit_immobil_5_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream << results.dam_results.ecn_immob_dam_95;
		this->lineEdit_immobil_95->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ecn_immob_dam_95*prob_hyd;
		this->lineEdit_immobil_95_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if(total_results.dam_results.ecn_immob_dam_95>0.0){
			buff_stream << results.dam_results.ecn_immob_dam_95*prob_hyd/total_results.dam_results.ecn_immob_dam_95;
			this->lineEdit_immobil_95_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream << results.dam_results.ecn_mob_dam_5;
		this->lineEdit_mobil_5->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ecn_mob_dam_5*prob_hyd;
		this->lineEdit_mobil_5_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);

		if(total_results.dam_results.ecn_mob_dam_5>0.0){
			buff_stream << results.dam_results.ecn_mob_dam_5*prob_hyd/total_results.dam_results.ecn_mob_dam_5;
			this->lineEdit_mobil_5_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream << results.dam_results.ecn_mob_dam_95;
		this->lineEdit_mobil_95->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ecn_mob_dam_95*prob_hyd;
		this->lineEdit_mobil_95_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if(total_results.dam_results.ecn_mob_dam_95>0.0){
			buff_stream << results.dam_results.ecn_mob_dam_95*prob_hyd/total_results.dam_results.ecn_mob_dam_95;
			this->lineEdit_mobil_95_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);
		buff_stream.str("");
		buff_stream << (results.dam_results.ecn_mob_dam + results.dam_results.ecn_immob_dam);
		this->lineEdit_total_ecn->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << (results.dam_results.ecn_mob_dam + results.dam_results.ecn_immob_dam)*prob_hyd;
		this->lineEdit_total_ecn_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<<P(2);
		if((total_results.dam_results.ecn_mob_dam+total_results.dam_results.ecn_immob_dam)>0.0){
			buff_stream << (results.dam_results.ecn_mob_dam+results.dam_results.ecn_immob_dam)*prob_hyd/(total_results.dam_results.ecn_mob_dam+total_results.dam_results.ecn_immob_dam);
			this->lineEdit_total_ecn_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		//eco
		buff_stream.str("");
		buff_stream << results.dam_results.eco_biotype;
		this->lineEdit_btype->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.eco_biotype*prob_hyd;
		this->lineEdit_btype_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((total_results.dam_results.eco_biotype)>0.0){
			buff_stream << (results.dam_results.eco_biotype)*prob_hyd/(total_results.dam_results.eco_biotype);
			this->lineEdit_btype_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream.str("");
		buff_stream << results.dam_results.eco_soil_erosion;
		this->lineEdit_soil_ero->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.eco_soil_erosion*prob_hyd;
		this->lineEdit_soil_ero_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((total_results.dam_results.eco_soil_erosion)>0.0){
			buff_stream << (results.dam_results.eco_soil_erosion)*prob_hyd/(total_results.dam_results.eco_soil_erosion);
			this->lineEdit_soil_ero_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream.str("");
		buff_stream << (results.dam_results.eco_soil_erosion + results.dam_results.eco_biotype);
		this->lineEdit_total_eco->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << (results.dam_results.eco_soil_erosion + results.dam_results.eco_biotype)*prob_hyd;
		this->lineEdit_total_eco_hyd->setText(buff_stream.str().c_str());
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		buff_stream.str("");
		if((total_results.dam_results.eco_soil_erosion+ total_results.dam_results.eco_biotype)>0.0){
			buff_stream << (results.dam_results.eco_soil_erosion+results.dam_results.eco_biotype)*prob_hyd/(total_results.dam_results.eco_soil_erosion+ total_results.dam_results.eco_biotype);
			this->lineEdit_total_eco_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);


		//total
		buff_stream.str("");
		sum=results.dam_results.eco_soil_erosion + results.dam_results.eco_biotype +
			results.dam_results.ecn_immob_dam+results.dam_results.ecn_mob_dam;
		buff_stream << (sum);
		this->lineEdit_total_sum->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << (sum)*prob_hyd;
		this->lineEdit_total_sum_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		sum_dens=total_results.dam_results.eco_soil_erosion + total_results.dam_results.eco_biotype +
			total_results.dam_results.ecn_immob_dam+total_results.dam_results.ecn_mob_dam;
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((sum_dens)>0.0){
			buff_stream << (sum)*prob_hyd/(sum_dens);
			this->lineEdit_total_sum_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);


		//pop
		buff_stream.str("");
		buff_stream << results.dam_results.pop_affected;
		this->lineEdit_pop_aff->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.pop_affected*prob_hyd;
		this->lineEdit_pop_aff_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((total_results.dam_results.pop_affected)>0.0){
			buff_stream << (results.dam_results.pop_affected)*prob_hyd/(total_results.dam_results.pop_affected);
			this->lineEdit_pop_aff_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream << results.dam_results.pop_endangered;
		this->lineEdit_pop_end->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.pop_endangered*prob_hyd;
		this->lineEdit_pop_end_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((total_results.dam_results.pop_endangered)>0.0){
			buff_stream << (results.dam_results.pop_endangered)*prob_hyd/(total_results.dam_results.pop_endangered);
			this->lineEdit_pop_end_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		//pys
		buff_stream.str("");
		buff_stream << results.dam_results.pys_age_50_59 <<" ("<< results.dam_results.pys_age_50_59_dens<<")";
		this->lineEdit_pys_50->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.pys_age_50_59*prob_hyd <<" ("<< results.dam_results.pys_age_50_59_dens*prob_hyd <<")";
		this->lineEdit_pys_50_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((total_results.dam_results.pys_age_50_59_dens)>0.0){
			buff_stream << (results.dam_results.pys_age_50_59_dens)*prob_hyd/(total_results.dam_results.pys_age_50_59_dens);
			this->lineEdit_pys_50_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);



		buff_stream << results.dam_results.pys_age_80 <<" ("<< results.dam_results.pys_age_80_dens<<")";
		this->lineEdit_pys_80->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.pys_age_80*prob_hyd <<" ("<< results.dam_results.pys_age_80_dens*prob_hyd<<")";
		this->lineEdit_pys_80_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((total_results.dam_results.pys_age_80_dens)>0.0){
			buff_stream << (results.dam_results.pys_age_80_dens)*prob_hyd/(total_results.dam_results.pys_age_80_dens);
			this->lineEdit_pys_80_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream << results.dam_results.pys_female <<" ("<< results.dam_results.pys_female_dens<<")";
		this->lineEdit_pys_female->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.pys_female*prob_hyd <<" ("<< results.dam_results.pys_female_dens*prob_hyd<<")";
		this->lineEdit_pys_female_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((total_results.dam_results.pys_female_dens)>0.0){
			buff_stream << (results.dam_results.pys_female_dens)*prob_hyd/(total_results.dam_results.pys_female_dens);
			this->lineEdit_pys_female_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream << results.dam_results.pys_owner <<" ("<< results.dam_results.pys_owner_dens<<")";
		this->lineEdit_pys_owner->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.pys_owner*prob_hyd <<" ("<< results.dam_results.pys_owner_dens*prob_hyd<<")";
		this->lineEdit_pys_owner_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((total_results.dam_results.pys_owner_dens)>0.0){
			buff_stream << (results.dam_results.pys_owner_dens)*prob_hyd/(total_results.dam_results.pys_owner_dens);
			this->lineEdit_pys_owner_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		sum=results.dam_results.pys_owner+results.dam_results.pys_age_50_59+results.dam_results.pys_age_80+
			results.dam_results.pys_female;
		sum_dens=results.dam_results.pys_owner_dens+results.dam_results.pys_age_50_59_dens+results.dam_results.pys_age_80_dens+
			results.dam_results.pys_female_dens;
		buff_stream << sum <<" ("<< sum_dens<<")";
		this->lineEdit_pys_total->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << sum*prob_hyd <<" ("<< sum_dens*prob_hyd<<")";
		this->lineEdit_pys_total_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		sum=total_results.dam_results.pys_owner_dens+total_results.dam_results.pys_age_50_59_dens+total_results.dam_results.pys_age_80_dens+
			total_results.dam_results.pys_female_dens;
		if((sum)>0.0){
			buff_stream << (sum_dens)*prob_hyd/(sum);
			this->lineEdit_pys_total_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);


		//sc
		buff_stream.str("");
		buff_stream << results.dam_results.sc_pub_build;
		this->lineEdit_sc_pub->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.sc_pub_build*prob_hyd;
		this->lineEdit_sc_pub_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((total_results.dam_results.sc_pub_build)>0.0){
			buff_stream << (results.dam_results.sc_pub_build)*prob_hyd/(total_results.dam_results.sc_pub_build);
			this->lineEdit_sc_pub_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream << results.dam_results.sc_eco_build;
		this->lineEdit_sc_eco->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.sc_eco_build*prob_hyd;
		this->lineEdit_sc_eco_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((total_results.dam_results.sc_eco_build)>0.0){
			buff_stream << (results.dam_results.sc_eco_build)*prob_hyd/(total_results.dam_results.sc_eco_build);
			this->lineEdit_sc_eco_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream << results.dam_results.sc_cult_build;
		this->lineEdit_sc_cult->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.sc_cult_build*prob_hyd;
		this->lineEdit_sc_cult_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((total_results.dam_results.sc_cult_build)>0.0){
			buff_stream << (results.dam_results.sc_cult_build)*prob_hyd/(total_results.dam_results.sc_cult_build);
			this->lineEdit_sc_cult_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);

		buff_stream << results.dam_results.sc_person_build;
		this->lineEdit_sc_person->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.sc_person_build*prob_hyd;
		this->lineEdit_sc_person_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if((total_results.dam_results.sc_person_build)>0.0){
			buff_stream << (results.dam_results.sc_person_build)*prob_hyd/(total_results.dam_results.sc_person_build);
			this->lineEdit_sc_person_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream<< FORMAT_FIXED_REAL<< P(1);



		//ci
		buff_stream << results.dam_results.ci_elect_pt;
		this->lineEdit_ci_elect->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ci_elect_pt*prob_hyd;
		this->lineEdit_ci_elect_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << FORMAT_SCIENTIFIC_REAL << P(2);
		if ((total_results.dam_results.ci_elect_pt) > 0.0) {
			buff_stream << (results.dam_results.ci_elect_pt)*prob_hyd / (total_results.dam_results.ci_elect_pt);
			this->lineEdit_ci_elect_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream << FORMAT_FIXED_REAL << P(1);

		buff_stream << results.dam_results.ci_info_tec_pt;
		this->lineEdit_ci_info_tec->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ci_info_tec_pt*prob_hyd;
		this->lineEdit_ci_info_tec_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << FORMAT_SCIENTIFIC_REAL << P(2);
		if ((total_results.dam_results.ci_info_tec_pt) > 0.0) {
			buff_stream << (results.dam_results.ci_info_tec_pt)*prob_hyd / (total_results.dam_results.ci_info_tec_pt);
			this->lineEdit_ci_info_tec_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream << FORMAT_FIXED_REAL << P(1);

		buff_stream << results.dam_results.ci_water_sup_pt;
		this->lineEdit_ci_water_sup->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ci_water_sup_pt*prob_hyd;
		this->lineEdit_ci_water_sup_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << FORMAT_SCIENTIFIC_REAL << P(2);
		if ((total_results.dam_results.ci_water_sup_pt) > 0.0) {
			buff_stream << (results.dam_results.ci_water_sup_pt)*prob_hyd / (total_results.dam_results.ci_water_sup_pt);
			this->lineEdit_ci_water_sup_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream << FORMAT_FIXED_REAL << P(1);

		buff_stream << results.dam_results.ci_water_treat_pt;
		this->lineEdit_ci_water_treat->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ci_water_treat_pt*prob_hyd;
		this->lineEdit_ci_water_treat_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << FORMAT_SCIENTIFIC_REAL << P(2);
		if ((total_results.dam_results.ci_water_treat_pt) > 0.0) {
			buff_stream << (results.dam_results.ci_water_treat_pt)*prob_hyd / (total_results.dam_results.ci_water_treat_pt);
			this->lineEdit_ci_water_treat_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream << FORMAT_FIXED_REAL << P(1);

		buff_stream << results.dam_results.ci_energy_pt;
		this->lineEdit_ci_energy->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ci_energy_pt*prob_hyd;
		this->lineEdit_ci_energy_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << FORMAT_SCIENTIFIC_REAL << P(2);
		if ((total_results.dam_results.ci_energy_pt) > 0.0) {
			buff_stream << (results.dam_results.ci_energy_pt)*prob_hyd / (total_results.dam_results.ci_energy_pt);
			this->lineEdit_ci_energy_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream << FORMAT_FIXED_REAL << P(1);

		buff_stream << results.dam_results.ci_health_pt;
		this->lineEdit_ci_health->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ci_health_pt*prob_hyd;
		this->lineEdit_ci_health_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << FORMAT_SCIENTIFIC_REAL << P(2);
		if ((total_results.dam_results.ci_health_pt) > 0.0) {
			buff_stream << (results.dam_results.ci_health_pt)*prob_hyd / (total_results.dam_results.ci_health_pt);
			this->lineEdit_ci_health_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream << FORMAT_FIXED_REAL << P(1);

		buff_stream << results.dam_results.ci_social_pt;
		this->lineEdit_ci_social->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ci_social_pt*prob_hyd;
		this->lineEdit_ci_social_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << FORMAT_SCIENTIFIC_REAL << P(2);
		if ((total_results.dam_results.ci_social_pt) > 0.0) {
			buff_stream << (results.dam_results.ci_social_pt)*prob_hyd / (total_results.dam_results.ci_social_pt);
			this->lineEdit_ci_social_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream << FORMAT_FIXED_REAL << P(1);


		buff_stream << results.dam_results.ci_mat_pt;
		this->lineEdit_ci_mat->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.dam_results.ci_mat_pt*prob_hyd;
		this->lineEdit_ci_mat_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << FORMAT_SCIENTIFIC_REAL << P(2);
		if ((total_results.dam_results.ci_mat_pt) > 0.0) {
			buff_stream << (results.dam_results.ci_mat_pt)*prob_hyd / (total_results.dam_results.ci_mat_pt);
			this->lineEdit_ci_mat_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}
		buff_stream << FORMAT_FIXED_REAL << P(1);




		//max outflow
		buff_stream << results.max_outflow;
		this->lineEdit_outflow->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream << results.max_outflow*prob_hyd;
		this->lineEdit_outflow_hyd->setText(buff_stream.str().c_str());
		buff_stream.str("");
		buff_stream<< FORMAT_SCIENTIFIC_REAL<< P(2);
		if(total_results.max_outflow>0.0){
			buff_stream << results.max_outflow*prob_hyd/total_results.max_outflow;
			this->lineEdit_outflow_fac->setText(buff_stream.str().c_str());
			buff_stream.str("");
		}



	}





}