#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Check_Distribution.h"

//Default constructor
Fpl_Check_Distribution::Fpl_Check_Distribution(void):max_storing(10000001){
	this->gen_result=NULL;
	this->number_gen_run=0;
	this->step_interval=0.0;
	this->number_intervals=1000;
	this->check_classes=NULL;
	this->variable2check=NULL;
	this->counter_setting=0;
	this->error=0.0;
	this->max_occurence_prob=0.0;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Check_Distribution)-sizeof(FplGui_Random_Check_Dia),_sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Check_Distribution::~Fpl_Check_Distribution(void){
	
	this->delete_check_classes();
	this->delete_result_array();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Check_Distribution)-sizeof(FplGui_Random_Check_Dia),_sys_system_modules::FPL_SYS);//count the memory
}
//_________
//public
//Set the pointer to the random variables, which should be checked
void Fpl_Check_Distribution::set_ptr_random_variables(Fpl_Random_Variables *variable2check){
	this->variable2check=variable2check;
}
//Set the number of generation runs
int Fpl_Check_Distribution::set_number_generation_runs(const int number){
	this->counter_setting=0;
	if(number>=this->max_storing){
		this->number_gen_run=this->max_storing;
		Warning msg=this->set_warning(0);
		msg.output_msg(1);
	}
	else{
		this->number_gen_run=number;
	}
	this->allocate_result_array();

	return this->number_gen_run;
}
//Set the result value for one generation run
void Fpl_Check_Distribution::set_result_one_run(const double result){
	if(this->counter_setting<this->number_gen_run){
		this->gen_result[this->counter_setting]=result;
		this->counter_setting++;
	}
}
//Get the number of simulation runs
int Fpl_Check_Distribution::get_number_sim_runs(void){
	return this->number_gen_run;
}
//Ask the checking varaibles per dialog
bool Fpl_Check_Distribution::ask_check_setting_per_dia(void){
	try{
		if(this->check_dia.start_dialog()==false){
			return false;
		}
		else{
			this->set_number_generation_runs(this->check_dia.get_number_runs());
			this->number_intervals=this->check_dia.get_number_classes();
			if(this->variable2check->set_input_check(this->check_dia.get_distribution_class_type())==false){
				return false;
			}
			else{
				return true;
			}
		}
	}
	catch(Error msg){
		msg.output_msg(1);
		return false;
	}
}
//Perform the analysis after the random generation process
void Fpl_Check_Distribution::perform_end_analysis(void){
	this->analyse_random_results();
	this->analyse_calculated_results();
	this->calculate_error();
}
//Get results as plot widget
void Fpl_Check_Distribution::get_results2plot(QWidget *parent){
	double *x_data;
	double *y_data;
	Sys_Plot_Frame *my_plot;
	Sys_Plot_Frame *my_plot_diff;
	Sys_Plot_Frame *my_plot_int;
	QVBoxLayout *my_layout;
	QVBoxLayout *my_layout_1;
	QWidget *my_widget;
	QScrollArea *my_scrollarea;
	
	try{
		my_layout=new QVBoxLayout;
		my_layout_1=new QVBoxLayout;
		my_widget=new QWidget;
		my_scrollarea=new QScrollArea;
		my_scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		my_scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		my_plot=new Sys_Plot_Frame;
		my_plot_diff=new Sys_Plot_Frame;
		my_plot_int=new Sys_Plot_Frame;
		x_data=new double[this->number_intervals];
		y_data=new double[this->number_intervals];
	}
	catch(bad_alloc &t){
		Error msg=set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}

	//set plot test
	ostringstream info;
	info<<"Probability density function \n of distribution type ";
	info <<_Fpl_Distribution_Class::convert_dist_type2txt(this->variable2check->get_distribution_type())<<endl;
	if(this->variable2check->get_distribution_type()==_fpl_distribution_type::mean_type){
		info << " ("<<this->variable2check->distribution->get_mean_distribution_type()<<")";
	}
	//set plot curve result generated, calculated
	my_plot->getPlotPtr()->set_plot_title(info.str());
	my_plot->getPlotPtr()->set_axis_title("x", "p(x)");
	
	try{
		my_plot->getPlotPtr()->set_number_curves(2);
	}
	catch(Error msg){
		throw msg;
	}

	//set curves 
	QwtPlotCurve::CurveStyle my_style;
	if(this->variable2check->get_distribution_type()!=_fpl_distribution_type::discret_type){
		my_style=QwtPlotCurve::CurveStyle::Lines;
	}
	else{
		my_style=QwtPlotCurve::CurveStyle::Steps;

	}
	my_plot->getPlotPtr()->set_curve_characteristics(0,"Exactly calculated",my_style,Qt::red);
	my_plot->getPlotPtr()->set_curve_symbols(0, 4 ,QwtSymbol::Style::Cross);
	my_plot->getPlotPtr()->set_curve_characteristics(1,"Randomly generated",my_style,Qt::black);
	my_plot->getPlotPtr()->set_curve_symbols(1, 3 ,QwtSymbol::Style::Diamond);
	my_plot->getPlotPtr()->set_legend(true);

	//generate data: exact
	for(int i=0; i< this->number_intervals; i++){
		x_data[i]=this->check_classes[i].class_mid;
		y_data[i]=this->check_classes[i].prob_occurence_calc;
	}
	my_plot->getPlotPtr()->set_curve_data(0, this->number_intervals, x_data, y_data);
	//generate data: generated
	for(int i=0; i< this->number_intervals; i++){
		y_data[i]=this->check_classes[i].prob_occurence_gen;
	}
	my_plot->getPlotPtr()->set_curve_data(1, this->number_intervals, x_data, y_data);

	//set plot differences
	my_plot_diff->getPlotPtr()->set_plot_title("Differences plot");
	my_plot_diff->getPlotPtr()->set_axis_title("x", "p(x)_calc-p(x)_gen");
	try{
		my_plot_diff->getPlotPtr()->set_number_curves(1);
	}
	catch(Error msg){
		throw msg;
	}
	my_plot_diff->getPlotPtr()->set_curve_characteristics(0,"Differences: calculated-generated",QwtPlotCurve::CurveStyle::Lines,Qt::black);
	my_plot_diff->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Style::NoSymbol);
	my_plot_diff->getPlotPtr()->set_legend(false);
	//generate data: generated
	for(int i=0; i< this->number_intervals; i++){
		y_data[i]=this->check_classes[i].prob_occurence_calc-this->check_classes[i].prob_occurence_gen;
	}
	my_plot_diff->getPlotPtr()->set_curve_data(0, this->number_intervals, x_data, y_data);


	//Set plot integration
	my_plot_int->getPlotPtr()->set_plot_title("Cumulative distribution function (generated)");
	my_plot_int->getPlotPtr()->set_axis_title("x", "P(x)");
	try{
		my_plot_int->getPlotPtr()->set_number_curves(1);
	}
	catch(Error msg){
		throw msg;
	}
	my_plot_int->getPlotPtr()->set_curve_characteristics(0,"Cumulative function",QwtPlotCurve::CurveStyle::Lines,Qt::black);
	my_plot_int->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Style::NoSymbol);
	my_plot_int->getPlotPtr()->set_legend(false);
	//generate data: generated
	double sum=0.0;
	y_data[0]=0.0;
	for(int i=1; i< this->number_intervals; i++){
		sum=sum+(this->check_classes[i].prob_occurence_calc+this->check_classes[i-1].prob_occurence_calc)*0.5*this->step_interval;
		y_data[i]=sum;
	}
	my_plot_int->getPlotPtr()->set_curve_data(0, this->number_intervals, x_data, y_data);

	if(parent==NULL){
		my_layout->addWidget(my_plot);
		my_layout->addWidget(my_plot_diff);
		my_layout->addWidget(my_plot_int);
		my_plot->getPlotPtr()->show_plot();
		my_plot_diff->getPlotPtr()->show_plot();
		my_plot_int->getPlotPtr()->show_plot();
		QApplication::exec();
		delete my_plot;
		delete my_plot_diff;
		delete my_plot_int;
		delete my_layout;
		delete []x_data;
		delete []y_data;;
	}
	else{
		//clear the parent widget
		parent->close();
		if(parent->layout()!=NULL){
			delete parent->layout();
		}
		QObjectList list;
		list=parent->children();
		for(int i=0; i< list.count(); i++){
			if(list.at(i)!=NULL){
				delete list.at(i);
			}
		}

		//parent of the plot
		my_scrollarea->setParent(parent);
		my_scrollarea->setWidgetResizable(true);
		my_widget->setParent(my_scrollarea);
		my_plot->setParent(my_widget);
		my_plot_diff->setParent(my_widget);
		my_plot_int->setParent(my_widget);

		my_plot->getPlotPtr()->set_scale_y_axis(0.0, this->max_occurence_prob);
		my_plot->getPlotPtr()->show_plot();
		my_plot_diff->getPlotPtr()->show_plot();
		my_plot_int->getPlotPtr()->show_plot();

		my_layout->addWidget(my_plot);
		my_layout->addWidget(my_plot_diff);
		my_layout->addWidget(my_plot_int);
		my_widget->setLayout(my_layout);
		my_layout_1->addWidget(my_scrollarea);
		my_scrollarea->setWidget(my_widget);
		/*my_widget->setMinimumSize(500,800);
		my_widget->adjustSize();*/

		parent->setLayout(my_layout_1);

		//delete data arrays
		delete []x_data;
		delete []y_data;

		parent->show();

	}
}
//Get the error of the calculation
double Fpl_Check_Distribution::get_error(void){
	return this->error;
}
//Get a pointer to the dialog for the checking settings
FplGui_Random_Check_Dia* Fpl_Check_Distribution::get_ptr_check_dia(void){
	return &this->check_dia;
}
//________
//private
//Analyse the randomly generated results
void Fpl_Check_Distribution::analyse_random_results(void){
	ostringstream cout;
	cout <<"Analyse the random generated set of result value..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout, false);

	//find min/max values for the upper/lower boundary
	double min=0.0;
	double max=0.0;
	for(int i=0; i< this->number_gen_run; i++){
		if(i==0){
			min=this->gen_result[i];
			max=this->gen_result[i];
		}
		//min
		if(this->gen_result[i]<min){
			min=this->gen_result[i];
		}
		//max
		if(this->gen_result[i]>max){
			max=this->gen_result[i];
		}
		Fpl_Calculation::check_stop_thread_flag();
	}
	//stepsize
	double delta=max-min;
	if(delta==0.0){
		delta=this->number_intervals;
		min=(max+min)*0.5-delta*0.5-0.001;
		max=min+delta;
		this->step_interval=(delta)/this->number_intervals;
	}
	else{
		this->step_interval=(delta)/this->number_intervals;
		max=max+this->step_interval;
		min=min-this->step_interval;
		delta=max-min;
		this->step_interval=(delta)/this->number_intervals;
	}
	
	
	try{
		this->allocate_check_classes();
	}
	catch(Error msg){
		throw msg;
	}
	//set mid values of classes
	double mid=min+this->step_interval*0.5;
	for(int i=0; i< this->number_intervals; i++){
		this->check_classes[i].class_mid=mid;
		mid=mid+this->step_interval;
	}

	//sort the result values to classes
	for(int i=0; i< this->number_gen_run; i++){
		for(int j=0; j<this->number_intervals; j++){
			if(this->gen_result[i]>=this->check_classes[j].class_mid-this->step_interval*0.5 &&
				this->gen_result[i]<this->check_classes[j].class_mid+this->step_interval*0.5){
					this->check_classes[j].prob_occurence_gen=this->check_classes[j].prob_occurence_gen+1.0;
					Fpl_Calculation::check_stop_thread_flag();
					break;
			}
		}
	}
	//divide by total calculation runs
	for(int j=0; j<this->number_intervals; j++){
		this->check_classes[j].prob_occurence_gen=this->check_classes[j].prob_occurence_gen/(this->number_gen_run*this->step_interval);
		Fpl_Calculation::check_stop_thread_flag();
	}
	//calculate the maximum of the occurence probabilty
	for(int j=0; j<this->number_intervals; j++){
		if(this->check_classes[j].prob_occurence_gen>this->max_occurence_prob){
			this->max_occurence_prob=this->check_classes[j].prob_occurence_gen;	
			Fpl_Calculation::check_stop_thread_flag();
		}
	}
	//add ten percent
	this->max_occurence_prob=this->max_occurence_prob+this->max_occurence_prob*0.1;
}
//Analyse by calculation of the results
void Fpl_Check_Distribution::analyse_calculated_results(void){
	ostringstream cout;
	cout <<"Analyse the distribution by exact calculation..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout, false);

	for(int j=0; j<this->number_intervals; j++){
		Fpl_Calculation::check_stop_thread_flag();
		this->check_classes[j].prob_occurence_calc=this->variable2check->distribution->get_calulculated_exact_prob(this->check_classes[j].class_mid, this->step_interval);
	}
}
//Calculate the error of the analysis
void Fpl_Check_Distribution::calculate_error(void){
	this->error=0.0;
	for(int j=1; j<this->number_intervals-1; j++){
		this->error=this->error+abs(this->check_classes[j].prob_occurence_gen-this->check_classes[j].prob_occurence_calc);
	}
	this->error=this->error/(this->number_intervals-2);
}
//Allocate the array of randomly generated results
void Fpl_Check_Distribution::allocate_result_array(void){
	this->delete_result_array();
	try{
		this->gen_result= new double[this->number_gen_run];
		Sys_Memory_Count::self()->add_mem(sizeof(double)*this->number_gen_run,_sys_system_modules::FPL_SYS);//count the memory
		
	}
	catch (bad_alloc &t){
		Error msg=set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
}
//Delete the array of randomly generated results
void Fpl_Check_Distribution::delete_result_array(void){
	if(this->gen_result!=NULL){
		delete []this->gen_result;
		this->gen_result=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->number_gen_run,_sys_system_modules::FPL_SYS);
	}
}
//Allocate the classes for sorting the results
void Fpl_Check_Distribution::allocate_check_classes(void){
	this->delete_check_classes();
	try{
		this->check_classes= new _fpl_check_interval[this->number_intervals];
		Sys_Memory_Count::self()->add_mem(sizeof(_fpl_check_interval)*this->number_intervals,_sys_system_modules::FPL_SYS);
	}
	catch (bad_alloc &t){
		Error msg=set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
	for(int i=0; i<this->number_intervals; i++){
		this->check_classes[i].class_mid=0.0;
		this->check_classes[i].prob_occurence_calc=0.0;
		this->check_classes[i].prob_occurence_gen=0.0;
	}
}
//Delete the classes for sorting the results
void Fpl_Check_Distribution::delete_check_classes(void){
	if(this->check_classes!=NULL){
		delete []this->check_classes;
		this->check_classes=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_check_interval)*this->number_intervals,_sys_system_modules::FPL_SYS);

	}
}
//Set error(s)
Error Fpl_Check_Distribution::set_error(const int err_type){
		string place="Fpl_Check_Distribution::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_result_array(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("allocate_check_classes(void)") ;
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://bad alloc
			place.append("get_results2plot(QWidget *parent)") ;
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
//Set warning(s)
Warning Fpl_Check_Distribution::set_warning(const int warn_type){
		string place="Fpl_Check_Distribution::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no unique result found
			place.append("set_number_generation_runs(const int number)") ;
			reason="The number of generation exceeds the maximum number of reults to store";
			reaction="There will just the maximum number of results stored for the analysis";
			help= "Check the number of generations";
			info <<"Maximum number of stored results : " << this->max_storing<<endl;
			type=6;	
			break;
		default:
			place.append("set_warning(const int warn_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;
}