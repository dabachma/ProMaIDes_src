#include "Fpl_Headers_Precompiled.h"
//#include "FplGui_Prob_Result_Wid.h"

//Default constructor
FplGui_Prob_Result_Wid::FplGui_Prob_Result_Wid(QWidget *parent): _Sys_Data_Wid(parent){
	this->setupUi(this);
	this->area2print=this->scrollAreaWidgetContents;
	this->widget_frc->set_groupbox_title("Results");
	this->widget_time->set_groupbox_title("Time");
	this->widget_frc_generation->set_groupbox_title("Number");

	Sys_Memory_Count::self()->add_mem(sizeof(FplGui_Prob_Result_Wid), _sys_system_modules::FPL_SYS);
}
//Default destructor
FplGui_Prob_Result_Wid::~FplGui_Prob_Result_Wid(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(FplGui_Prob_Result_Wid), _sys_system_modules::FPL_SYS);
}
//__________
//public
//Set tooltip of the header
void FplGui_Prob_Result_Wid::set_header_tooltip(const string tooltip, QTreeWidgetItem *item){
	this->head_label->setToolTip(tooltip.c_str());
	this->set_identifier(tooltip, item);
	string buffer;
	buffer=_Sys_Data_Wid::get_last_string_string_id(tooltip);
	this->label_id->setText(buffer.c_str());
}
//Set the member of the widget
void FplGui_Prob_Result_Wid::set_member(QSqlDatabase *ptr_database, const int sec_id, const _sys_system_id id){

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
		txt<<"Probabilistic results of section  ";
		string name;
		string type;
		name=model.record(0).value((Fpl_Section::table->get_column_name(fpl_label::section_name)).c_str()).toString().toStdString();
		type=model.record(0).value((Fpl_Section::table->get_column_name(fpl_label::section_type)).c_str()).toString().toStdString();
		txt << name << " (id: "<<sec_id<<", type: "<<type<<")";
		this->head_label->setText(txt.str().c_str());

        struct_height=model.record(0).value((Fpl_Section::table->get_column_name(fpl_label::structure_height)).c_str()).toDouble();
	}

	//set the results
	//mc-results
	try{
		number=Fpl_Mc_Sim::select_results_in_database(&model, ptr_database, id, sec_id, true);
	}
	catch(Error msg){
		throw msg;
	}




	if(number==0){
		this->groupBox_mc->setEnabled(false);
		this->groupBox_mc->setToolTip("No results of a Monte-carlo simulation are found"); 
	}
	else{
		string buffer;
		buffer=model.record(0).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_waterlevel)).c_str()).toString().toStdString();
		this->lineEdit_waterlevel->setText(buffer.c_str());
		buffer=model.record(0).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_probability)).c_str()).toString().toStdString();
		this->lineEdit_failure_prob->setText(buffer.c_str());
		buffer=model.record(0).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_low_bound)).c_str()).toString().toStdString();
		this->lineEdit_lower_bound->setText(buffer.c_str());
		buffer=model.record(0).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_up_bound)).c_str()).toString().toStdString();
		this->lineEdit_upper_bound->setText(buffer.c_str());
		buffer=model.record(0).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_number_mc)).c_str()).toString().toStdString();
		this->lineEdit_number_gen->setText(buffer.c_str());
		buffer=model.record(0).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_comp_time)).c_str()).toString().toStdString();
		this->lineEdit_time->setText(buffer.c_str());

	}

	//set the results
	//frc-results
	try{
		number=Fpl_Mc_Sim::select_results_in_database(&model, ptr_database, id, sec_id, false);
	}
	catch(Error msg){
		throw msg;
	}
	if(number==0){
		this->groupBox_frc->setEnabled(false);
		this->groupBox_frc->setToolTip("No results of a FRC-simulation are found"); 
		this->widget_frc->getPlotPtr()->close();
		this->widget_frc_generation->close();
		this->widget_time->close();
	}
	else{

		//set plot and curve general settings
        this->widget_frc->getPlotPtr()->set_number_curves(4);
		this->widget_frc_generation->getPlotPtr()->set_number_curves(1);
		this->widget_time->getPlotPtr()->set_number_curves(1);

		this->widget_frc->getPlotPtr()->set_plot_title("Fragility curve (FRC)");
		this->widget_frc_generation->getPlotPtr()->set_plot_title("Number of generation");
		this->widget_time->getPlotPtr()->set_plot_title("Time of generation");
		
		this->widget_frc->getPlotPtr()->set_axis_title("Waterlevel [m]", "Failure probability [-]" );
		this->widget_frc_generation->getPlotPtr()->set_axis_title("Waterlevel [m]", "Number [-]");
		this->widget_time->getPlotPtr()->set_axis_title("Waterlevel [m]", "Time [s]");
		

		this->widget_frc->getPlotPtr()->set_curve_characteristics(0, "Frc", QwtPlotCurve::Lines, Qt::black);
		this->widget_frc->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
		this->widget_frc->getPlotPtr()->set_curve_characteristics(1, "Upper bound", QwtPlotCurve::Lines, Qt::gray);
		this->widget_frc->getPlotPtr()->set_curve_symbols(1, 3 ,QwtSymbol::Cross);
		this->widget_frc->getPlotPtr()->set_curve_characteristics(2, "Lower bound", QwtPlotCurve::Lines, Qt::gray);
		this->widget_frc->getPlotPtr()->set_curve_symbols(2, 3 ,QwtSymbol::RTriangle);
        QwtText tit;
        tit.setText("Structure height");
        this->widget_frc->getPlotPtr()->set_curve_characteristics(3, tit, QwtPlotCurve::Lines, Qt::gray,1,Qt::DashLine);

		this->widget_time->getPlotPtr()->set_curve_characteristics(0, "Time", QwtPlotCurve::Lines, Qt::black);
		this->widget_frc_generation->getPlotPtr()->set_curve_characteristics(0, "Runs", QwtPlotCurve::Lines, Qt::black);


		this->widget_frc->getPlotPtr()->set_legend(true);
		this->widget_time->getPlotPtr()->set_legend(true);
		this->widget_frc_generation->getPlotPtr()->set_legend(true);

		//allocate the data buffer
		double *x_data=NULL;
		double *y_data=NULL;
        double *x_data_h=NULL;
        double *y_data_h=NULL;
		try{
			x_data=new double [number];
			y_data=new double [number];

            x_data_h=new double [number];
            y_data_h=new double [number];
		}
		catch(bad_alloc &t){
			Error msg=set_error(0);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());	
			throw msg;
		}

		//set curve data
		//frc main values
		for(int i=0; i<number; i++){
			x_data[i]=model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_waterlevel)).c_str()).toDouble();
			y_data[i]=model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_probability)).c_str()).toDouble();
		}
		this->widget_frc->getPlotPtr()->set_curve_data(0, number, x_data, y_data);
		//frc upper bound
		for(int i=0; i<number; i++){
			y_data[i]=model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_up_bound)).c_str()).toDouble();
		}
		this->widget_frc->getPlotPtr()->set_curve_data(1, number, x_data, y_data);
		//frc lower bound
		for(int i=0; i<number; i++){
			y_data[i]=model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_low_bound)).c_str()).toDouble();
		}
		this->widget_frc->getPlotPtr()->set_curve_data(2, number, x_data, y_data);

        //set structure height
		for (int i = 0; i < number; i++) {
			x_data_h[i] = struct_height;
			y_data_h[i] = 1.0;
		}

        x_data_h[0]=struct_height;
        y_data_h[0]=0;

        this->widget_frc->getPlotPtr()->set_curve_data(3, number, x_data_h, y_data_h);


		//number generation
		for(int i=0; i<number; i++){
			y_data[i]=model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_number_mc)).c_str()).toDouble();
		}
		this->widget_frc_generation->getPlotPtr()->set_curve_data(0, number, x_data, y_data);

		//generation time
		for(int i=0; i<number; i++){
			y_data[i]=model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_comp_time)).c_str()).toDouble();
		}
		this->widget_time->getPlotPtr()->set_curve_data(0, number, x_data, y_data);

		this->widget_frc->getPlotPtr()->show_plot();
		this->widget_frc_generation->getPlotPtr()->show_plot();
		this->widget_time->getPlotPtr()->show_plot();
		//this->groupBox_frc->adjustSize();

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
//Set error(s)
Error FplGui_Prob_Result_Wid::set_error(const int err_type){
		string place="FplGui_Prob_Result_Wid::";
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
