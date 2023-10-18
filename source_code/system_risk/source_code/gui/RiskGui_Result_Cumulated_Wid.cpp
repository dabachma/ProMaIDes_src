#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Result_Cumulated_Wid.h"

//Default constructor
RiskGui_Result_Cumulated_Wid::RiskGui_Result_Cumulated_Wid(QWidget *parent) : _Sys_Data_Wid(parent){
	this->setupUi(this);
	this->array_y=NULL;
	this->array_x=NULL;
	this->number_data=0;
	this->area2print=this->scrollAreaWidgetContents;

	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Result_Cumulated_Wid), _sys_system_modules::RISK_SYS);
}
//Default destructor
RiskGui_Result_Cumulated_Wid::~RiskGui_Result_Cumulated_Wid(void){
	this->delete_data();
	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Result_Cumulated_Wid), _sys_system_modules::RISK_SYS);
}
//__________
//public
//Set tooltip of the header
void RiskGui_Result_Cumulated_Wid::set_header_tooltip(const string tooltip, QTreeWidgetItem *item){
	this->head_label->setToolTip(tooltip.c_str());
	this->set_identifier(tooltip, item);
	string buffer;
	buffer=_Sys_Data_Wid::get_last_string_string_id(tooltip);
	this->label_id->setText(buffer.c_str());
}
//Set the member of the widget
void RiskGui_Result_Cumulated_Wid::set_member(QSqlDatabase *ptr_database, const _sys_system_id id, const _risk_type type){
	QSqlQueryModel model;

	try{
		this->number_data=Risk_System::select_data_in_resulttable_cumulated(&model, ptr_database, id, type);
		this->allocate_data();
	}
	catch(Error msg){
		throw msg;
	}
	//check for data
	if(this->number_data==0){
		this->setEnabled(false);
		this->setToolTip("No results are found");
		this->delete_data();
		return;
	}


	//set x data
	int hyd_sc;
	//hydraulic boundary manager
	Hyd_Boundary_Szenario_Management my_hyd_sc;
	my_hyd_sc.set_systemid(id);
	my_hyd_sc.set_szenario_per_db(ptr_database);

	for(int i=0; i< this->number_data; i++){
		hyd_sc=model.record(i).value((Risk_System::table_cumulated_results->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
		this->array_x[i]=(double)my_hyd_sc.get_ptr_sz_id_given(hyd_sc)->get_annuality();

	}

	this->generate_ecn_plots(&model);
	this->generate_eco_plots(&model);
	this->generate_monetary_plots(&model);
	this->generate_pop_plots(&model);
	this->generate_pys_plots(&model);
	this->generate_sc_plots(&model);
	this->generate_ci_plots(&model);
	this->generate_max_q_plot(&model);

	this->delete_data();
}
//________
//private
//Generate the plot for the economic risk
void RiskGui_Result_Cumulated_Wid::generate_ecn_plots(QSqlQueryModel *model){

	//set plot and curve general settings
	this->widget_immob->getPlotPtr()->set_number_curves(3);
	this->widget_mob->getPlotPtr()->set_number_curves(3);
	this->widget_ecn_total->getPlotPtr()->set_number_curves(3);

	this->widget_immob->getPlotPtr()->set_plot_title("Immobile risk");
	this->widget_mob->getPlotPtr()->set_plot_title("Mobile risk");
	this->widget_ecn_total->getPlotPtr()->set_plot_title("Sum economical risk");

	this->widget_immob->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [Monetary]" );
	this->widget_mob->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [Monetary]" );
	this->widget_ecn_total->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [Monetary]" );

	this->widget_immob->getPlotPtr()->set_curve_characteristics(0, "Mid", QwtPlotCurve::Lines, Qt::green);
	this->widget_immob->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
	this->widget_immob->getPlotPtr()->set_curve_characteristics(1, "5%-quantile", QwtPlotCurve::Lines, Qt::black);
	this->widget_immob->getPlotPtr()->set_curve_symbols(1, 5 ,QwtSymbol::Cross);
	this->widget_immob->getPlotPtr()->set_curve_characteristics(2, "95%-quantile", QwtPlotCurve::Lines, Qt::gray);
	this->widget_immob->getPlotPtr()->set_curve_symbols(2, 5 ,QwtSymbol::RTriangle);
	this->widget_immob->getPlotPtr()->set_legend(true);


	this->widget_mob->getPlotPtr()->set_curve_characteristics(0, "Mid", QwtPlotCurve::Lines, Qt::blue);
	this->widget_mob->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
	this->widget_mob->getPlotPtr()->set_curve_characteristics(1, "5%-quantile", QwtPlotCurve::Lines, Qt::black);
	this->widget_mob->getPlotPtr()->set_curve_symbols(1, 5 ,QwtSymbol::Cross);
	this->widget_mob->getPlotPtr()->set_curve_characteristics(2, "95%-quantile", QwtPlotCurve::Lines, Qt::gray);
	this->widget_mob->getPlotPtr()->set_curve_symbols(2, 5 ,QwtSymbol::RTriangle);
	this->widget_mob->getPlotPtr()->set_legend(true);

	this->widget_ecn_total->getPlotPtr()->set_curve_characteristics(0, "Mobile", QwtPlotCurve::Lines, Qt::blue);
	this->widget_ecn_total->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
	this->widget_ecn_total->getPlotPtr()->set_curve_characteristics(1, "Immobile", QwtPlotCurve::Lines, Qt::green);
	this->widget_ecn_total->getPlotPtr()->set_curve_symbols(1, 5 ,QwtSymbol::Cross);
	this->widget_ecn_total->getPlotPtr()->set_curve_characteristics(2, "Total", QwtPlotCurve::Lines, Qt::red);
	this->widget_ecn_total->getPlotPtr()->set_curve_symbols(2, 5 ,QwtSymbol::RTriangle);
	this->widget_ecn_total->getPlotPtr()->set_legend(true);

	//generate immob curve data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
	}
	this->widget_immob->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);
	this->widget_ecn_total->getPlotPtr()->set_curve_data(1, this->number_data, this->array_x, this->array_y);
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::immob_5_quant)).c_str()).toDouble();
	}
	this->widget_immob->getPlotPtr()->set_curve_data(1, this->number_data, this->array_x, this->array_y);

	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::immob_95_quant)).c_str()).toDouble();
	}
	this->widget_immob->getPlotPtr()->set_curve_data(2, this->number_data, this->array_x, this->array_y);

	//generate mob curve data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
	}
	this->widget_mob->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);
	this->widget_ecn_total->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::mob_5_quant)).c_str()).toDouble();
	}
	this->widget_mob->getPlotPtr()->set_curve_data(1, this->number_data, this->array_x, this->array_y);
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::mob_95_quant)).c_str()).toDouble();
	}
	this->widget_mob->getPlotPtr()->set_curve_data(2, this->number_data, this->array_x, this->array_y);

	//generate total curve data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble()+
			model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
	}
	this->widget_ecn_total->getPlotPtr()->set_curve_data(2, this->number_data, this->array_x, this->array_y);

	//this->widget_immob->set

	this->widget_immob->getPlotPtr()->show_plot();
	this->widget_mob->getPlotPtr()->show_plot();
	this->widget_ecn_total->getPlotPtr()->show_plot();


}
//Generate the plot for the ecological risk
void RiskGui_Result_Cumulated_Wid::generate_eco_plots(QSqlQueryModel *model){
	//set plot and curve general settings
	this->widget_btype->getPlotPtr()->set_number_curves(1);
	this->widget_soil->getPlotPtr()->set_number_curves(1);
	this->widget_eco_total->getPlotPtr()->set_number_curves(3);

	this->widget_btype->getPlotPtr()->set_plot_title("Biotope-type risk");
	this->widget_soil->getPlotPtr()->set_plot_title("Soil-erosion risk");
	this->widget_eco_total->getPlotPtr()->set_plot_title("Sum ecological risk");

	this->widget_btype->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [Monetary]" );
	this->widget_soil->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [Monetary]" );
	this->widget_eco_total->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [Monetary]" );

	this->widget_btype->getPlotPtr()->set_curve_characteristics(0, "Biotope-type", QwtPlotCurve::Lines, Qt::black);
	this->widget_btype->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
	
	this->widget_soil->getPlotPtr()->set_curve_characteristics(0, "Biotope-type", QwtPlotCurve::Lines, Qt::black);
	this->widget_soil->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);


	this->widget_eco_total->getPlotPtr()->set_curve_characteristics(0, "Biotope-type", QwtPlotCurve::Lines, Qt::black);
	this->widget_eco_total->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
	this->widget_eco_total->getPlotPtr()->set_curve_characteristics(1, "Soil-erosion", QwtPlotCurve::Lines, Qt::gray);
	this->widget_eco_total->getPlotPtr()->set_curve_symbols(1, 5 ,QwtSymbol::Cross);
	this->widget_eco_total->getPlotPtr()->set_curve_characteristics(2, "Total", QwtPlotCurve::Lines, Qt::red);
	this->widget_eco_total->getPlotPtr()->set_curve_symbols(2, 5 ,QwtSymbol::RTriangle);
	this->widget_eco_total->getPlotPtr()->set_legend(true);

	//generate btype curve data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
	}
	this->widget_btype->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);
	this->widget_eco_total->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);

	//generate soil-erosion curve data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
	}
	this->widget_soil->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);
	this->widget_eco_total->getPlotPtr()->set_curve_data(1, this->number_data, this->array_x, this->array_y);

	//generate total
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble()+
			model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
	}
	this->widget_eco_total->getPlotPtr()->set_curve_data(2, this->number_data, this->array_x, this->array_y);

	this->widget_btype->getPlotPtr()->show_plot();
	this->widget_soil->getPlotPtr()->show_plot();
	this->widget_eco_total->getPlotPtr()->show_plot();
	
}
//Generate the plot for the monetary summary risk
void RiskGui_Result_Cumulated_Wid::generate_monetary_plots(QSqlQueryModel *model){
	//set plot and curve general settings
	this->widget_monetary_total->getPlotPtr()->set_number_curves(5);

	this->widget_monetary_total->getPlotPtr()->set_plot_title("Sum monetary risk");

	this->widget_monetary_total->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [Monetary]" );


	this->widget_monetary_total->getPlotPtr()->set_curve_characteristics(0, "ECN-immob", QwtPlotCurve::Lines, Qt::black);
	this->widget_monetary_total->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
	this->widget_monetary_total->getPlotPtr()->set_curve_characteristics(1, "ECN-mob", QwtPlotCurve::Lines, Qt::gray);
	this->widget_monetary_total->getPlotPtr()->set_curve_symbols(1, 5 ,QwtSymbol::Cross);
	this->widget_monetary_total->getPlotPtr()->set_curve_characteristics(2, "ECO-Biotope-type", QwtPlotCurve::Lines, Qt::green);
	this->widget_monetary_total->getPlotPtr()->set_curve_symbols(2, 5 ,QwtSymbol::RTriangle);
	this->widget_monetary_total->getPlotPtr()->set_curve_characteristics(3, "ECO-Soil-erosion", QwtPlotCurve::Lines, Qt::darkGreen);
	this->widget_monetary_total->getPlotPtr()->set_curve_symbols(3, 5 ,QwtSymbol::LTriangle);
	this->widget_monetary_total->getPlotPtr()->set_curve_characteristics(4, "Total", QwtPlotCurve::Lines, Qt::magenta);
	this->widget_monetary_total->getPlotPtr()->set_curve_symbols(4, 5 ,QwtSymbol::Ellipse);
	this->widget_monetary_total->getPlotPtr()->set_legend(true);


	//generate immob curve data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
	}
	this->widget_monetary_total->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);
	//generate mob curve data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
	}
	this->widget_monetary_total->getPlotPtr()->set_curve_data(1, this->number_data, this->array_x, this->array_y);
	//generate btype curve data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
	}
	this->widget_monetary_total->getPlotPtr()->set_curve_data(2, this->number_data, this->array_x, this->array_y);
	//generate soil-erosion curve data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
	}
	this->widget_monetary_total->getPlotPtr()->set_curve_data(3, this->number_data, this->array_x, this->array_y);

	//generate total curve data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble()+
				model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble()+
				model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble()+
				model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble();

	}
	this->widget_monetary_total->getPlotPtr()->set_curve_data(4, this->number_data, this->array_x, this->array_y);
	this->widget_monetary_total->getPlotPtr()->show_plot();

}
//Generate the plot for the people2risk risk
void RiskGui_Result_Cumulated_Wid::generate_pop_plots(QSqlQueryModel *model){
	//set plot and curve general settings
	this->widget_affected_pop->getPlotPtr()->set_number_curves(1);
	this->widget_endangered_pop->getPlotPtr()->set_number_curves(1);

	this->widget_affected_pop->getPlotPtr()->set_plot_title("Affected population risk");
	this->widget_endangered_pop->getPlotPtr()->set_plot_title("Endangered population risk");

	this->widget_affected_pop->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [Per.]" );
	this->widget_endangered_pop->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [Per.]" );

	this->widget_affected_pop->getPlotPtr()->set_curve_characteristics(0, "Affected", QwtPlotCurve::Lines, Qt::black);
	this->widget_affected_pop->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
	this->widget_endangered_pop->getPlotPtr()->set_curve_characteristics(0, "Endangered", QwtPlotCurve::Lines, Qt::black);
	this->widget_endangered_pop->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);

	//generate affected curve data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
	}
	this->widget_affected_pop->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);
	//generate endangered curve data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
	}
	this->widget_endangered_pop->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);

	this->widget_affected_pop->getPlotPtr()->show_plot();
	this->widget_endangered_pop->getPlotPtr()->show_plot();

}
//Generate the plot for the psycho-social risk
void RiskGui_Result_Cumulated_Wid::generate_pys_plots(QSqlQueryModel *model){
	//set plot and curve general settings
	this->widget_pys_crit->getPlotPtr()->set_number_curves(5);
	this->widget_pys_crit_dens->getPlotPtr()->set_number_curves(5);

	this->widget_pys_crit->getPlotPtr()->set_plot_title("Score risk");
	this->widget_pys_crit_dens->getPlotPtr()->set_plot_title("Score with pop-dens-factor risk");
	
	this->widget_pys_crit->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [Score]" );
	this->widget_pys_crit_dens->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [Score]" );

	this->widget_pys_crit->getPlotPtr()->set_curve_characteristics(0, "Age 50-59 a", QwtPlotCurve::Lines, Qt::black);
	this->widget_pys_crit->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
	this->widget_pys_crit->getPlotPtr()->set_curve_characteristics(1, "Age >80 a", QwtPlotCurve::Lines, Qt::gray);
	this->widget_pys_crit->getPlotPtr()->set_curve_symbols(1, 5 ,QwtSymbol::Cross);
	this->widget_pys_crit->getPlotPtr()->set_curve_characteristics(2, "Female gender", QwtPlotCurve::Lines, Qt::green);
	this->widget_pys_crit->getPlotPtr()->set_curve_symbols(2, 5 ,QwtSymbol::RTriangle);
	this->widget_pys_crit->getPlotPtr()->set_curve_characteristics(3, "Home owner", QwtPlotCurve::Lines, Qt::red);
	this->widget_pys_crit->getPlotPtr()->set_curve_symbols(3, 5 ,QwtSymbol::LTriangle);
	this->widget_pys_crit->getPlotPtr()->set_curve_characteristics(4, "Total", QwtPlotCurve::Lines, Qt::magenta);
	this->widget_pys_crit->getPlotPtr()->set_curve_symbols(4, 5 ,QwtSymbol::Ellipse);
	this->widget_pys_crit->getPlotPtr()->set_legend(true);

	this->widget_pys_crit_dens->getPlotPtr()->set_curve_characteristics(0, "Age 50-59 a", QwtPlotCurve::Lines, Qt::black);
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_characteristics(1, "Age >80 a", QwtPlotCurve::Lines, Qt::gray);
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_symbols(1, 5 ,QwtSymbol::Cross);
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_characteristics(2, "Female gender", QwtPlotCurve::Lines, Qt::green);
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_symbols(2, 5 ,QwtSymbol::RTriangle);
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_characteristics(3, "Home owner", QwtPlotCurve::Lines, Qt::red);
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_symbols(3, 5 ,QwtSymbol::LTriangle);
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_characteristics(4, "Total", QwtPlotCurve::Lines, Qt::magenta);
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_symbols(4, 5 ,QwtSymbol::Ellipse);
	this->widget_pys_crit_dens->getPlotPtr()->set_legend(true);


	//age 50-59
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
	}
	this->widget_pys_crit->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);
	//age >80
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
	}
	this->widget_pys_crit->getPlotPtr()->set_curve_data(1, this->number_data, this->array_x, this->array_y);
	//female
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::crit_female)).c_str()).toDouble();
	}
	this->widget_pys_crit->getPlotPtr()->set_curve_data(2, this->number_data, this->array_x, this->array_y);
	//home
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::crit_owner)).c_str()).toDouble();
	}
	this->widget_pys_crit->getPlotPtr()->set_curve_data(3, this->number_data, this->array_x, this->array_y);
	//total
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::crit_owner)).c_str()).toDouble()+
			model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::crit_female)).c_str()).toDouble()+
			model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::crit_age_80)).c_str()).toDouble()+
			model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
	}
	this->widget_pys_crit->getPlotPtr()->set_curve_data(4, this->number_data, this->array_x, this->array_y);


	//age 50-59 density
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
	}
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);
	//age >80 density
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble();
	}
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_data(1, this->number_data, this->array_x, this->array_y);
	//female density
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_female)).c_str()).toDouble();
	}
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_data(2, this->number_data, this->array_x, this->array_y);
	//home density
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble();
	}
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_data(3, this->number_data, this->array_x, this->array_y);
	//total density
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble()+
			model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_female)).c_str()).toDouble()+
			model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble()+
			model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
	}
	this->widget_pys_crit_dens->getPlotPtr()->set_curve_data(4, this->number_data, this->array_x, this->array_y);

	this->widget_pys_crit->getPlotPtr()->show_plot();
	this->widget_pys_crit_dens->getPlotPtr()->show_plot();
}
//Generate the plot for the simple counting risk
void RiskGui_Result_Cumulated_Wid::generate_sc_plots(QSqlQueryModel *model){
	//set plot and curve general settings
	this->widget_sc->getPlotPtr()->set_number_curves(4);
	
	this->widget_sc->getPlotPtr()->set_plot_title("SC Score risk");
		
	this->widget_sc->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [Score]" );
	
	this->widget_sc->getPlotPtr()->set_curve_characteristics(0, "Public buildings", QwtPlotCurve::Lines, Qt::black);
	this->widget_sc->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
	this->widget_sc->getPlotPtr()->set_curve_characteristics(1, "Ecologic perilous sites/buildings", QwtPlotCurve::Lines, Qt::gray);
	this->widget_sc->getPlotPtr()->set_curve_symbols(1, 5 ,QwtSymbol::Cross);
	this->widget_sc->getPlotPtr()->set_curve_characteristics(2, "Cultural heritage", QwtPlotCurve::Lines, Qt::green);
	this->widget_sc->getPlotPtr()->set_curve_symbols(2, 5 ,QwtSymbol::RTriangle);
	this->widget_sc->getPlotPtr()->set_curve_characteristics(3, "Buildings with highly vulnerable person", QwtPlotCurve::Lines, Qt::red);
	this->widget_sc->getPlotPtr()->set_curve_symbols(3, 5 ,QwtSymbol::LTriangle);
	this->widget_sc->getPlotPtr()->set_legend(true);



	//set data
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::sc_pub_build)).c_str()).toDouble();
	}
	this->widget_sc->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);

	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::sc_eco_build)).c_str()).toDouble();
	}
	this->widget_sc->getPlotPtr()->set_curve_data(1, this->number_data, this->array_x, this->array_y);
		for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::sc_cult_build)).c_str()).toDouble();
	}
	this->widget_sc->getPlotPtr()->set_curve_data(2, this->number_data, this->array_x, this->array_y);
	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::sc_person_build)).c_str()).toDouble();
	}
	this->widget_sc->getPlotPtr()->set_curve_data(3, this->number_data, this->array_x, this->array_y);

	this->widget_sc->getPlotPtr()->show_plot();

}
//Generate the plot for the ci risk
void RiskGui_Result_Cumulated_Wid::generate_ci_plots(QSqlQueryModel *model) {

	//set plot and curve general settings
	this->widget_ci->getPlotPtr()->set_number_curves(8);

	this->widget_ci->getPlotPtr()->set_plot_title("CI Population time risk");

	this->widget_ci->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [P x s]");

	this->widget_ci->getPlotPtr()->set_curve_characteristics(0, "Sector Electricity 1", QwtPlotCurve::Lines, Qt::black);
	this->widget_ci->getPlotPtr()->set_curve_symbols(0, 5, QwtSymbol::Diamond);
	this->widget_ci->getPlotPtr()->set_curve_characteristics(1, "Sector Information techology 2", QwtPlotCurve::Lines, Qt::gray);
	this->widget_ci->getPlotPtr()->set_curve_symbols(1, 5, QwtSymbol::Cross);
	this->widget_ci->getPlotPtr()->set_curve_characteristics(2, "Sector Water supply 3", QwtPlotCurve::Lines, Qt::blue);
	this->widget_ci->getPlotPtr()->set_curve_symbols(2, 5, QwtSymbol::RTriangle);
	this->widget_ci->getPlotPtr()->set_curve_characteristics(3, "Sector Water treatment 4", QwtPlotCurve::Lines, Qt::darkBlue);
	this->widget_ci->getPlotPtr()->set_curve_symbols(3, 5, QwtSymbol::LTriangle);
	this->widget_ci->getPlotPtr()->set_curve_characteristics(4, "Sector Energy 5", QwtPlotCurve::Lines, Qt::darkGray);
	this->widget_ci->getPlotPtr()->set_curve_symbols(4, 5, QwtSymbol::DTriangle);

	this->widget_ci->getPlotPtr()->set_curve_characteristics(5, "Sector Health (10+11)", QwtPlotCurve::Lines, Qt::red);
	this->widget_ci->getPlotPtr()->set_curve_symbols(5, 5, QwtSymbol::Ellipse);

	this->widget_ci->getPlotPtr()->set_curve_characteristics(6, "Sector Social (14+17+18+19)", QwtPlotCurve::Lines, Qt::green);
	this->widget_ci->getPlotPtr()->set_curve_symbols(6, 5, QwtSymbol::Hexagon);


	this->widget_ci->getPlotPtr()->set_curve_characteristics(7, "Sector Material (12+13+15+16)", QwtPlotCurve::Lines, Qt::lightGray);
	this->widget_ci->getPlotPtr()->set_curve_symbols(7, 5, QwtSymbol::Star1);




	this->widget_ci->getPlotPtr()->set_legend(true);



	//set data
	for (int i = 0; i < this->number_data; i++) {
		this->array_y[i] = model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::ci_elect_pt)).c_str()).toDouble();
	}
	this->widget_ci->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);

	for (int i = 0; i < this->number_data; i++) {
		this->array_y[i] = model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::ci_info_tec_pt)).c_str()).toDouble();
	}
	this->widget_ci->getPlotPtr()->set_curve_data(1, this->number_data, this->array_x, this->array_y);

	for (int i = 0; i < this->number_data; i++) {
		this->array_y[i] = model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::ci_water_sup_pt)).c_str()).toDouble();
	}
	this->widget_ci->getPlotPtr()->set_curve_data(2, this->number_data, this->array_x, this->array_y);

	for (int i = 0; i < this->number_data; i++) {
		this->array_y[i] = model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::ci_water_treat_pt)).c_str()).toDouble();
	}
	this->widget_ci->getPlotPtr()->set_curve_data(3, this->number_data, this->array_x, this->array_y);

	for (int i = 0; i < this->number_data; i++) {
		this->array_y[i] = model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::ci_energy_pt)).c_str()).toDouble();
	}
	this->widget_ci->getPlotPtr()->set_curve_data(4, this->number_data, this->array_x, this->array_y);

	for (int i = 0; i < this->number_data; i++) {
		this->array_y[i] = model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::ci_health_pt)).c_str()).toDouble();
	}
	this->widget_ci->getPlotPtr()->set_curve_data(5, this->number_data, this->array_x, this->array_y);

	for (int i = 0; i < this->number_data; i++) {
		this->array_y[i] = model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::ci_social_pt)).c_str()).toDouble();
	}
	this->widget_ci->getPlotPtr()->set_curve_data(6, this->number_data, this->array_x, this->array_y);

	for (int i = 0; i < this->number_data; i++) {
		this->array_y[i] = model->record(i).value((Risk_System::table_cumulated_results->get_column_name(dam_label::ci_mat_pt)).c_str()).toDouble();
	}
	this->widget_ci->getPlotPtr()->set_curve_data(7, this->number_data, this->array_x, this->array_y);



	this->widget_ci->getPlotPtr()->show_plot();




}
//Generate the plot for the maximum dicharge risk
void RiskGui_Result_Cumulated_Wid::generate_max_q_plot(QSqlQueryModel *model){
	this->widget_outflow->getPlotPtr()->set_number_curves(1);
	this->widget_outflow->getPlotPtr()->set_plot_title("Maximum outflow discharge");
	this->widget_outflow->getPlotPtr()->set_axis_title("Annuality [a]", "Risk [m³/s]" );
	this->widget_outflow->getPlotPtr()->set_curve_characteristics(0, "Maximum discharge", QwtPlotCurve::Lines, Qt::black);
	this->widget_outflow->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);

	for(int i=0; i<this->number_data; i++){
		this->array_y[i]=model->record(i).value((Risk_System::table_cumulated_results->get_column_name(risk_label::max_outflow)).c_str()).toDouble();
	}
	this->widget_outflow->getPlotPtr()->set_curve_data(0, this->number_data, this->array_x, this->array_y);
	this->widget_outflow->getPlotPtr()->show_plot();
}
//Alloacte data arrays
void RiskGui_Result_Cumulated_Wid::allocate_data(void){
		try{
			this->array_x=new double [this->number_data];
			this->array_y=new double [this->number_data];
		}
		catch(bad_alloc &t){
			Error msg=set_error(0);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());	
			throw msg;
		}
		//init
		for(int i=0; i< this->number_data; i++){
			this->array_y[i]=0.0;
			this->array_x[i]=0.0;
		}
}
//Delete data arrays
void RiskGui_Result_Cumulated_Wid::delete_data(void){
	if(this->array_x!=NULL){
		delete []this->array_x;
		this->array_x=NULL;
	}
	if(this->array_y!=NULL){
		delete []this->array_y;
		this->array_y=NULL;
	}
}
///Set error(s)
Error RiskGui_Result_Cumulated_Wid::set_error(const int err_type){
		string place="RiskGui_Result_Cumulated_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_data(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}