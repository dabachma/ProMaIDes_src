#include "Fpl_Headers_Precompiled.h"
//#include "FplGui_Detailed_Result_Wid.h"

//Default constructor
FplGui_Detailed_Result_Wid::FplGui_Detailed_Result_Wid(QWidget *parent): _Sys_Data_Wid(parent){
	this->setupUi(this);
    this->area2print=this->scrollAreaWidgetContents;
    this->widget_single_prob->set_groupbox_title("Single failure probability");
    this->widget_tree_prob->set_groupbox_title("Tree failure probability");



    this->expander_det_mech->add_child(this->tableWidget_det_mech);
    this->expander_det_mech->set_title("Mechanisms");
    this->expander_det_mech_f->add_child(this->tableWidget_det_f_mech);
    this->expander_det_mech_f->set_title("Failure Mechanisms");
    this->expander_mc_mech->add_child(this->tableWidget_mc_mech);
    this->expander_mc_mech->set_title("Mechanisms (mean)");
    this->expander_mc_f_mech->add_child(this->tableWidget_mc_f_mech);
    this->expander_mc_f_mech->set_title("Failure Mechanisms (mean)");
    this->expander_mc_prob->add_child(this->tableWidget_mc_prob);
    this->expander_mc_prob->set_title("Probabilities");
    this->expander_frc_mech->add_child(this->widget_wave_height);
    this->expander_frc_mech->set_title("Mechanisms (h)");
    this->expander_frc_f_mech->add_child(this->widget_wave_impact);
    this->expander_frc_f_mech->set_title("Failure mechanism (h)");
    this->expander_frc_prob->set_title("Probabilities (h)");
    this->expander_frc_prob->add_child(this->widget_tree_prob);
    this->expander_frc_prob->add_child(this->widget_single_prob);


    functions::set_table_widget_flags(this->tableWidget_det_f_mech);
    functions::set_table_widget_flags(this->tableWidget_det_mech);
    functions::set_table_widget_flags(this->tableWidget_mc_f_mech);
    functions::set_table_widget_flags(this->tableWidget_mc_mech);
    functions::set_table_widget_flags(this->tableWidget_mc_prob);

    Sys_Memory_Count::self()->add_mem(sizeof(FplGui_Detailed_Result_Wid), _sys_system_modules::FPL_SYS);
}
//Default destructor
FplGui_Detailed_Result_Wid::~FplGui_Detailed_Result_Wid(void){

    Sys_Memory_Count::self()->minus_mem(sizeof(FplGui_Detailed_Result_Wid), _sys_system_modules::FPL_SYS);
}
//__________
//public
//Set tooltip of the header
void FplGui_Detailed_Result_Wid::set_header_tooltip(const string tooltip, QTreeWidgetItem *item){
	this->head_label->setToolTip(tooltip.c_str());
	this->set_identifier(tooltip, item);
	string buffer;
	buffer=_Sys_Data_Wid::get_last_string_string_id(tooltip);
	this->label_id->setText(buffer.c_str());
}
//Set the member of the widget
void FplGui_Detailed_Result_Wid::set_member(QSqlDatabase *ptr_database, const int sec_id, const _sys_system_id id){

    QSqlQueryModel model;
    int number=0;

    double struct_height=0.0;



    //set the header title
    try{
        number=Fpl_Section::select_relevant_section_database(&model, ptr_database, id, sec_id, false);
    }
    catch(Error msg){
        throw msg;
    }
	
    if(number==0){
        Sys_Diverse_Text_Dia dialog2(true);
        ostringstream txt;
        txt<<"No fpl section is found with id "<< sec_id << endl;
        dialog2.set_dialog_question(txt.str());
        dialog2.start_dialog();
        this->head_label->setText(txt.str().c_str());
    }
    else{
        //make the header
        ostringstream txt;
        txt<<"Detailed results of section  ";
        string name;
        string type;
        name=model.record(0).value((Fpl_Section::table->get_column_name(fpl_label::section_name)).c_str()).toString().toStdString();
        type=model.record(0).value((Fpl_Section::table->get_column_name(fpl_label::section_type)).c_str()).toString().toStdString();
        txt << name << " (id: "<<sec_id<<", type: "<<type<<")";
        this->head_label->setText(txt.str().c_str());

        struct_height=model.record(0).value((Fpl_Section::table->get_column_name(fpl_label::structure_height)).c_str()).toDouble();
    }

    //set the results
    //determ-results
    try{
        number=Fpl_Sec_Type_Dike::select_results_in_database(&model, ptr_database, id, sec_id, _fpl_simulation_type::sim_determ);
    }
    catch(Error msg){
        throw msg;
    }
    if(number==0){
        this->groupBox_determ->setEnabled(false);
        this->groupBox_determ->setToolTip("No results of a deterministic simulation are found");

    }
    else{
        string buffer;
        buffer=model.record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::waterlevel)).c_str()).toString().toStdString();
        this->lineEdit_waterlevel_det->setText(buffer.c_str());

        //set table deterministic mechanism
        for(int i=0;i<14;i++){
           this->value_determ_mecha(i,&model);
        }
        //set table deterministic failure mechanism
        for(int i=0;i<11;i++){
           this->value_determ_mecha_failure(i,&model);
        }

    }

    //mc-results
    try{
        number=Fpl_Sec_Type_Dike::select_results_in_database(&model, ptr_database, id, sec_id, _fpl_simulation_type::sim_mc);
    }
    catch(Error msg){
        throw msg;
    }
    if(number==0){
        this->groupBox_mc->setEnabled(false);
        this->groupBox_mc->setToolTip("No results of a Monte-Carlo simulation are found");

    }
    else{
        string buffer;
        buffer=model.record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::waterlevel)).c_str()).toString().toStdString();
        this->lineEdit_waterlevel_mc->setText(buffer.c_str());

        //set table mc mechanism
        for(int i=0;i<14;i++){
           this->value_mc_mecha(i,&model);
        }
        //set table mc failure mechanism
        for(int i=0;i<11;i++){
           this->value_mc_mecha_failure(i,&model);
        }
        //set table mc failure probabilities
        for(int i=0;i<11;i++){
           this->value_mc_prob_failure(i,&model);
        }

    }

    //set the results
    //frc-results
    try{
        number=Fpl_Sec_Type_Dike::select_results_in_database(&model, ptr_database, id, sec_id, _fpl_simulation_type::sim_frc);
    }
    catch(Error msg){
        throw msg;
    }
    if(number==0){
        this->groupBox_frc->setEnabled(false);
        this->groupBox_frc->setToolTip("No results of a FRC-simulation are found");

    }
    else{

        //set plot and curve general settings
        this->widget_tree_prob->getPlotPtr()->set_number_curves(10);
        this->widget_single_prob->getPlotPtr()->set_number_curves(12);

        this->widget_tree_prob->getPlotPtr()->set_plot_title("Tree failure probability per mechanism");
        this->widget_single_prob->getPlotPtr()->set_plot_title("Single failure probability per mechanism");

		
        this->widget_tree_prob->getPlotPtr()->set_axis_title("Waterlevel [m]", "Failure probability [-]" );
        this->widget_single_prob->getPlotPtr()->set_axis_title("Waterlevel [m]", "Failure probability [-]" );

        QwtText tit;
        tit.setText("Structure height");
        this->widget_tree_prob->getPlotPtr()->set_curve_characteristics(0, tit, QwtPlotCurve::Lines, Qt::black,2,Qt::DashLine);
        this->widget_single_prob->getPlotPtr()->set_curve_characteristics(0, tit, QwtPlotCurve::Lines, Qt::black,2,Qt::DashLine);

		

        this->widget_tree_prob->getPlotPtr()->set_curve_characteristics(1, "Wave impact", QwtPlotCurve::Lines, Qt::black);
        this->widget_tree_prob->getPlotPtr()->set_curve_symbols(1, 5 ,QwtSymbol::Diamond);

        this->widget_single_prob->getPlotPtr()->set_curve_characteristics(1, "Wave impact", QwtPlotCurve::Lines, Qt::black);
        this->widget_single_prob->getPlotPtr()->set_curve_symbols(1, 5 ,QwtSymbol::Diamond);

        this->widget_tree_prob->getPlotPtr()->set_curve_characteristics(2, "Erosion waterside", QwtPlotCurve::Lines, Qt::gray);
        this->widget_tree_prob->getPlotPtr()->set_curve_symbols(2, 5 ,QwtSymbol::Diamond);

        this->widget_single_prob->getPlotPtr()->set_curve_characteristics(2, "Erosion waterside", QwtPlotCurve::Lines, Qt::gray);
        this->widget_single_prob->getPlotPtr()->set_curve_symbols(2, 5 ,QwtSymbol::Diamond);

        this->widget_tree_prob->getPlotPtr()->set_curve_characteristics(3, "Piping Sellmeijer", QwtPlotCurve::Lines, Qt::green);
        this->widget_tree_prob->getPlotPtr()->set_curve_symbols(3, 5 ,QwtSymbol::Cross);

        this->widget_single_prob->getPlotPtr()->set_curve_characteristics(3, "Piping Sellmeijer", QwtPlotCurve::Lines, Qt::green);
        this->widget_single_prob->getPlotPtr()->set_curve_symbols(3, 5 ,QwtSymbol::Cross);

        tit.setText("Piping Lane");
        this->widget_tree_prob->getPlotPtr()->set_curve_characteristics(4, tit, QwtPlotCurve::Lines, Qt::green,1,Qt::DashLine);
        this->widget_tree_prob->getPlotPtr()->set_curve_symbols(4, 5 ,QwtSymbol::Cross);

        this->widget_single_prob->getPlotPtr()->set_curve_characteristics(4, tit, QwtPlotCurve::Lines, Qt::green,1,Qt::DashLine);
        this->widget_single_prob->getPlotPtr()->set_curve_symbols(4, 5 ,QwtSymbol::Cross);

        tit.setText("Piping Schmertmann");
        this->widget_tree_prob->getPlotPtr()->set_curve_characteristics(5, tit, QwtPlotCurve::Lines, Qt::green,1,Qt::DotLine);
        this->widget_tree_prob->getPlotPtr()->set_curve_symbols(4, 5 ,QwtSymbol::Cross);

        this->widget_single_prob->getPlotPtr()->set_curve_characteristics(5, tit, QwtPlotCurve::Lines, Qt::green,1,Qt::DotLine);
        this->widget_single_prob->getPlotPtr()->set_curve_symbols(5, 5 ,QwtSymbol::Cross);

        this->widget_tree_prob->getPlotPtr()->set_curve_characteristics(6, "MicroStab waterside", QwtPlotCurve::Lines, Qt::blue);
        this->widget_tree_prob->getPlotPtr()->set_curve_symbols(6, 5 ,QwtSymbol::Triangle);

        this->widget_single_prob->getPlotPtr()->set_curve_characteristics(6, "MicroStab waterside", QwtPlotCurve::Lines, Qt::blue);
        this->widget_single_prob->getPlotPtr()->set_curve_symbols(6, 5 ,QwtSymbol::Triangle);

        tit.setText("MacroStab waterside");
        this->widget_tree_prob->getPlotPtr()->set_curve_characteristics(7, tit, QwtPlotCurve::Lines, Qt::blue,1,Qt::DotLine);
        this->widget_tree_prob->getPlotPtr()->set_curve_symbols(7, 5 ,QwtSymbol::Triangle);

        this->widget_single_prob->getPlotPtr()->set_curve_characteristics(7, tit, QwtPlotCurve::Lines, Qt::blue,1,Qt::DotLine);
        this->widget_single_prob->getPlotPtr()->set_curve_symbols(7, 5 ,QwtSymbol::Triangle);

        tit.setText("MicroStab landside");
        this->widget_tree_prob->getPlotPtr()->set_curve_characteristics(8, tit, QwtPlotCurve::Lines, Qt::blue,1,Qt::DashLine);
        this->widget_tree_prob->getPlotPtr()->set_curve_symbols(8, 5 ,QwtSymbol::Triangle);

        this->widget_single_prob->getPlotPtr()->set_curve_characteristics(8, tit, QwtPlotCurve::Lines, Qt::blue,1,Qt::DashLine);
        this->widget_single_prob->getPlotPtr()->set_curve_symbols(8, 5 ,QwtSymbol::Triangle);

        tit.setText("MacroStab landside");
        this->widget_tree_prob->getPlotPtr()->set_curve_characteristics(9, tit, QwtPlotCurve::Lines, Qt::blue,1,Qt::DashDotDotLine);
        this->widget_tree_prob->getPlotPtr()->set_curve_symbols(9, 5 ,QwtSymbol::Triangle);

        this->widget_single_prob->getPlotPtr()->set_curve_characteristics(9, tit, QwtPlotCurve::Lines, Qt::blue,1,Qt::DashDotDotLine);
        this->widget_single_prob->getPlotPtr()->set_curve_symbols(9, 5 ,QwtSymbol::Triangle);

        this->widget_single_prob->getPlotPtr()->set_curve_characteristics(10, "Start erosion waterside", QwtPlotCurve::Lines, Qt::darkGray);
        this->widget_single_prob->getPlotPtr()->set_curve_symbols(10, 5 ,QwtSymbol::Diamond);

        this->widget_single_prob->getPlotPtr()->set_curve_characteristics(11, "Heave", QwtPlotCurve::Lines, Qt::darkGreen);
        this->widget_single_prob->getPlotPtr()->set_curve_symbols(11, 5 ,QwtSymbol::Cross);

        this->widget_tree_prob->getPlotPtr()->set_legend(true);
        this->widget_single_prob->getPlotPtr()->set_legend(true);


        //allocate the data buffer
        double *x_data=NULL;
        double *y_data=NULL;
        double *x_data_h=NULL;
        double *y_data_h=NULL;
        try{
            x_data=new double [number];
            y_data=new double [number];

            x_data_h=new double [2];
            y_data_h=new double [2];
        }
        catch(bad_alloc &t){
            Error msg=set_error(0);
            ostringstream info;
            info<< "Info bad alloc: " << t.what() << endl;
            msg.make_second_info(info.str());
            throw msg;
        }

        //set curve data
        //tree prob wave impact
        for(int i=0; i<number; i++){
            x_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::waterlevel)).c_str()).toDouble();
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_wave_impact)).c_str()).toDouble();
        }
        this->widget_tree_prob->getPlotPtr()->set_curve_data(1, number, x_data, y_data);
        //single prob wave impact
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_wave_impact)).c_str()).toDouble();
         }
        this->widget_single_prob->getPlotPtr()->set_curve_data(1, number, x_data, y_data);

        //tree prob erosion waterside
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_water_ero)).c_str()).toDouble();
         }
        this->widget_tree_prob->getPlotPtr()->set_curve_data(2, number, x_data, y_data);
        //single prob erosion waterside
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_water_ero)).c_str()).toDouble();
         }
        this->widget_single_prob->getPlotPtr()->set_curve_data(2, number, x_data, y_data);

        //tree prob piping sellmeijer
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_pip_sell)).c_str()).toDouble();
         }
        this->widget_tree_prob->getPlotPtr()->set_curve_data(3, number, x_data, y_data);
        //single prob piping sellmeijer
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_pip_sell)).c_str()).toDouble();
         }
        this->widget_single_prob->getPlotPtr()->set_curve_data(3, number, x_data, y_data);

        //tree prob piping lane
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_pip_lane)).c_str()).toDouble();
         }
        this->widget_tree_prob->getPlotPtr()->set_curve_data(4, number, x_data, y_data);
        //single prob piping lane
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_pip_lane)).c_str()).toDouble();
         }
        this->widget_single_prob->getPlotPtr()->set_curve_data(4, number, x_data, y_data);

        //tree prob piping schmertmann
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_pip_schmert)).c_str()).toDouble();
         }
        this->widget_tree_prob->getPlotPtr()->set_curve_data(5, number, x_data, y_data);
        //single prob piping schmertmann
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_pip_schmert)).c_str()).toDouble();
         }
        this->widget_single_prob->getPlotPtr()->set_curve_data(5, number, x_data, y_data);


        //tree prob  microstab waterside
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_slope_water_micro)).c_str()).toDouble();
         }
        this->widget_tree_prob->getPlotPtr()->set_curve_data(6, number, x_data, y_data);
        //single prob microstab waterside
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_slope_water_micro)).c_str()).toDouble();
         }
        this->widget_single_prob->getPlotPtr()->set_curve_data(6, number, x_data, y_data);

        //tree prob  macrostab waterside
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_slope_water_macro)).c_str()).toDouble();
         }
        this->widget_tree_prob->getPlotPtr()->set_curve_data(7, number, x_data, y_data);
        //single prob macrostab waterside
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_slope_water_macro)).c_str()).toDouble();
         }
        this->widget_single_prob->getPlotPtr()->set_curve_data(7, number, x_data, y_data);

        //tree prob  microstab landside
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_slope_land_micro)).c_str()).toDouble();
         }
        this->widget_tree_prob->getPlotPtr()->set_curve_data(8, number, x_data, y_data);
        //single prob microstab landside
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_slope_land_micro)).c_str()).toDouble();
         }
        this->widget_single_prob->getPlotPtr()->set_curve_data(8, number, x_data, y_data);

        //tree prob  macrostab landside
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_slope_land_macro)).c_str()).toDouble();
         }
        this->widget_tree_prob->getPlotPtr()->set_curve_data(9, number, x_data, y_data);
        //single prob macrostab landside
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_slope_land_macro)).c_str()).toDouble();
         }
        this->widget_single_prob->getPlotPtr()->set_curve_data(9, number, x_data, y_data);

        //single prob start erosion waterside
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_water_ero_start)).c_str()).toDouble();
         }
        this->widget_single_prob->getPlotPtr()->set_curve_data(10, number, x_data, y_data);

        //single prob heave
        for(int i=0; i<number; i++){
            y_data[i]=model.record(i).value((Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_bursting)).c_str()).toDouble();
         }
        this->widget_single_prob->getPlotPtr()->set_curve_data(11, number, x_data, y_data);





        //set structure height
        x_data_h[0]=struct_height;
        y_data_h[0]=0;
        x_data_h[1]=struct_height;
        y_data_h[1]=1.0;
        this->widget_tree_prob->getPlotPtr()->set_curve_data(0, 2, x_data_h, y_data_h);
        this->widget_single_prob->getPlotPtr()->set_curve_data(0, 2, x_data_h, y_data_h);



        this->widget_tree_prob->getPlotPtr()->show_plot();
        this->widget_single_prob->getPlotPtr()->show_plot();



        //delete the data buffer
        if(x_data!=NULL){
            delete []x_data;
            x_data=NULL;
        }
        if(y_data!=NULL){
            delete []y_data;
            y_data=NULL;
        }
        if(x_data_h!=NULL){
            delete []x_data_h;
            x_data_h=NULL;
        }
        if(y_data_h!=NULL){
            delete []y_data_h;
            y_data_h=NULL;
        }
    }
}


//_________
//private
//Set value by a given index for the deterministic mechanisms table
void FplGui_Detailed_Result_Wid::value_determ_mecha(const int id, QSqlQueryModel *model){


    double db_buff;
    string buffer;
    switch (id){
        case 0:
            buffer=fpl_label::wave_height;
            break;
        case 1:
            buffer=fpl_label::wave_length;
            break;
        case 2:
            buffer=fpl_label::wave_period;
            break;
        case 3:
            buffer=fpl_label::wave_setup;
            break;
        case 4:
            buffer=fpl_label::mean_depth;
            break;

        case 6:
            buffer=fpl_label::gamma_berms;
            break;
        case 7:
            buffer=fpl_label::gamma_wavedirection;
            break;
        case 8:
            buffer=fpl_label::mid_gradient_waterside;
            break;
        case 9:
            buffer=fpl_label::height_runup;
            break;
        case 10:
            buffer=fpl_label::q_waveoverflow;
            break;

        case 12:
            buffer=fpl_label::mue_factor;
            break;
        case 13:
            buffer=fpl_label::q_overflow;
            break;

         default:
            return;

    }



    db_buff=model->record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(buffer)).c_str()).toDouble();
    this->tableWidget_det_mech->item(id+1,0)->setData(Qt::DisplayRole, db_buff);


}
//Set value by a given index for the deterministic failure mechanisms table
void FplGui_Detailed_Result_Wid::value_determ_mecha_failure(const int id, QSqlQueryModel *model){


    double db_buff;
    double db_buff1;
    string buffer;
    string buffer1;

    switch (id){
        case 0:
            buffer=fpl_label::res_waterside_erosion_start;
            buffer1=fpl_label::stress_waterside_erosion_start;
            break;
        case 1:
            buffer=fpl_label::res_waterside_erosion;
            buffer1=fpl_label::stress_waterside_erosion;
            break;
        case 2:
            buffer=fpl_label::res_wave_impact;
            buffer1=fpl_label::stress_wave_impact;
            break;
        case 3:
            buffer=fpl_label::res_piping_sell;
            buffer1=fpl_label::waterlevel;
            break;
        case 4:
            buffer=fpl_label::res_piping_lane;
            buffer1=fpl_label::waterlevel;
            break;
        case 5:
            buffer=fpl_label::res_piping_schmert;
            buffer1=fpl_label::waterlevel;
            break;
        case 6:
            buffer=fpl_label::res_burst_din;
            buffer1=fpl_label::stress_landside_discharge;
            break;
        case 7:
            buffer=fpl_label::stab_slope_water_micro;
            break;
        case 8:
            buffer=fpl_label::stab_slope_water_macro;
            break;
        case 9:
            buffer=fpl_label::stab_slope_land_micro;
            break;
        case 10:
            buffer=fpl_label::stab_slope_land_macro;
            break;
;

        default:
            return;

    }



    if(id<=6){
        db_buff=model->record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(buffer)).c_str()).toDouble();
        db_buff1=model->record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(buffer1)).c_str()).toDouble();
        this->tableWidget_det_f_mech->item(id,0)->setData(Qt::DisplayRole, db_buff);
        this->tableWidget_det_f_mech->item(id,1)->setData(Qt::DisplayRole, db_buff1);
        if(db_buff1!=0.0){
           this->tableWidget_det_f_mech->item(id,2)->setData(Qt::DisplayRole, db_buff/db_buff1);

        }
        this->tableWidget_det_f_mech->item(id,3)->setData(Qt::DisplayRole, db_buff-db_buff1);

    }
    else{
        db_buff=model->record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(buffer)).c_str()).toDouble();
        this->tableWidget_det_f_mech->item(id,2)->setData(Qt::DisplayRole, db_buff);

    }
}
//Set value by a given index for the Monte-Carlo mechanisms table
void FplGui_Detailed_Result_Wid::value_mc_mecha(const int id, QSqlQueryModel *model){

    double db_buff;
    string buffer;
    switch (id){
        case 0:
            buffer=fpl_label::wave_height;
            break;
        case 1:
            buffer=fpl_label::wave_length;
            break;
        case 2:
            buffer=fpl_label::wave_period;
            break;
        case 3:
            buffer=fpl_label::wave_setup;
            break;
        case 4:
            buffer=fpl_label::mean_depth;
            break;

        case 6:
            buffer=fpl_label::gamma_berms;
            break;
        case 7:
            buffer=fpl_label::gamma_wavedirection;
            break;
        case 8:
            buffer=fpl_label::mid_gradient_waterside;
            break;
        case 9:
            buffer=fpl_label::height_runup;
            break;
        case 10:
            buffer=fpl_label::q_waveoverflow;
            break;

        case 12:
            buffer=fpl_label::mue_factor;
            break;
        case 13:
            buffer=fpl_label::q_overflow;
            break;

         default:
            return;

    }



    db_buff=model->record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(buffer)).c_str()).toDouble();
    this->tableWidget_mc_mech->item(id+1,0)->setData(Qt::DisplayRole, db_buff);

}
//Set value by a given index for the Monte-Carlo failure mechanisms table
void FplGui_Detailed_Result_Wid::value_mc_mecha_failure(const int id, QSqlQueryModel *model){


    double db_buff;
    double db_buff1;
    string buffer;
    string buffer1;

    switch (id){
        case 0:
            buffer=fpl_label::res_waterside_erosion_start;
            buffer1=fpl_label::stress_waterside_erosion_start;
            break;
        case 1:
            buffer=fpl_label::res_waterside_erosion;
            buffer1=fpl_label::stress_waterside_erosion;
            break;
        case 2:
            buffer=fpl_label::res_wave_impact;
            buffer1=fpl_label::stress_wave_impact;
            break;
        case 3:
            buffer=fpl_label::res_piping_sell;
            buffer1=fpl_label::waterlevel;
            break;
        case 4:
            buffer=fpl_label::res_piping_lane;
            buffer1=fpl_label::waterlevel;
            break;
        case 5:
            buffer=fpl_label::res_piping_schmert;
            buffer1=fpl_label::waterlevel;
            break;
        case 6:
            buffer=fpl_label::res_burst_din;
            buffer1=fpl_label::stress_landside_discharge;
            break;
        case 7:
            buffer=fpl_label::stab_slope_water_micro;
            break;
        case 8:
            buffer=fpl_label::stab_slope_water_macro;
            break;
        case 9:
            buffer=fpl_label::stab_slope_land_micro;
            break;
        case 10:
            buffer=fpl_label::stab_slope_land_macro;
            break;
;

        default:
            return;

    }



    if(id<=6){
        db_buff=model->record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(buffer)).c_str()).toDouble();
        db_buff1=model->record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(buffer1)).c_str()).toDouble();
        this->tableWidget_mc_f_mech->item(id,0)->setData(Qt::DisplayRole, db_buff);
        this->tableWidget_mc_f_mech->item(id,1)->setData(Qt::DisplayRole, db_buff1);
        if(db_buff1!=0.0){
           this->tableWidget_mc_f_mech->item(id,2)->setData(Qt::DisplayRole, db_buff/db_buff1);

        }
        this->tableWidget_mc_f_mech->item(id,3)->setData(Qt::DisplayRole, db_buff-db_buff1);

    }
    else{
        db_buff=model->record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(buffer)).c_str()).toDouble();
        this->tableWidget_mc_f_mech->item(id,2)->setData(Qt::DisplayRole, db_buff);

    }

}
//Set value by a given index for the Monte-Carlo failure probability table
void FplGui_Detailed_Result_Wid::value_mc_prob_failure(const int id, QSqlQueryModel *model){

    double db_buff;
    double db_buff1;
    string buffer;
    string buffer1;

    switch (id){
        case 0:
            buffer=fpl_label::single_prob_water_ero_start;

            break;
        case 1:
            buffer=fpl_label::single_prob_water_ero;
            buffer1=fpl_label::tree_prob_water_ero;
            break;
        case 2:
            buffer=fpl_label::single_prob_wave_impact;
            buffer1=fpl_label::tree_prob_wave_impact;
            break;
        case 3:
            buffer=fpl_label::single_prob_pip_sell;
            buffer1=fpl_label::tree_prob_pip_sell;
            break;
        case 4:
            buffer=fpl_label::single_prob_pip_lane;
            buffer1=fpl_label::tree_prob_pip_lane;
            break;
        case 5:
            buffer=fpl_label::single_prob_pip_schmert;
            buffer1=fpl_label::tree_prob_pip_schmert;
            break;
        case 6:
            buffer=fpl_label::single_prob_bursting;

            break;
        case 7:
            buffer=fpl_label::single_prob_slope_water_micro;
            buffer1=fpl_label::tree_prob_slope_water_micro;
            break;
        case 8:
            buffer=fpl_label::single_prob_slope_water_macro;
            buffer1=fpl_label::tree_prob_slope_water_macro;
            break;
        case 9:
            buffer=fpl_label::single_prob_slope_land_micro;
            buffer1=fpl_label::tree_prob_slope_land_micro;
            break;
        case 10:
            buffer=fpl_label::single_prob_slope_land_macro;
            buffer1=fpl_label::tree_prob_slope_land_macro;
            break;


        default:
            return;

    }



    if(id==0 || id==6){
        db_buff=model->record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(buffer)).c_str()).toDouble();
        this->tableWidget_mc_prob->item(id,0)->setData(Qt::DisplayRole, db_buff);
    }
    else{
        db_buff=model->record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(buffer)).c_str()).toDouble();
        db_buff1=model->record(0).value((Fpl_Sec_Type_Dike::result_table->get_column_name(buffer1)).c_str()).toDouble();
        this->tableWidget_mc_prob->item(id,0)->setData(Qt::DisplayRole, db_buff);
        this->tableWidget_mc_prob->item(id,1)->setData(Qt::DisplayRole, db_buff1);


    }



}

//Set error(s)
Error FplGui_Detailed_Result_Wid::set_error(const int err_type){
        string place="FplGui_Detailed_Result_Wid_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("set_member(QSqlDatabase *ptr_database, const int sec_id, const _sys_system_id id)");
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
