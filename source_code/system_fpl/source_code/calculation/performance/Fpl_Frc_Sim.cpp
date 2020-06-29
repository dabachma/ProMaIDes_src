//#include "Fpl_Frc_Sim.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
Fpl_Frc_Sim::Fpl_Frc_Sim(void){
	this->max_calculation=fpl_control_param::max_calculation_def;
	this->stepsize_min=fpl_control_param::stepsize_min_def;
	this->stepsize_max=fpl_control_param::stepsize_max_def;
	this->quotient_boundary_reduce=fpl_control_param::quotient_boundary_reduce_def;
	this->quotient_boundary_expand=fpl_control_param::quotient_boundary_expand_def;
	this->starting_stepsize=fpl_control_param::starting_stepsize_def;
	this->stepsize_factor=fpl_control_param::stepsize_factor_def;
	this->start_value=fpl_control_param::start_value_def;
	this->reset_start_value();
	this->frc_value=NULL;
	this->mc_sim_frc=NULL;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Frc_Sim)+sizeof(frc_value), _sys_system_modules::FPL_SYS);//count the memory
}
//Constructor with given parameter-set for a fragility cruve simualtion
Fpl_Frc_Sim::Fpl_Frc_Sim(const double min_stepsize, const double max_stepsize, const int calc_max, const double start_step, const double step_factor, const double boundary_reduce
				 , const double boundary_expand, const double value_start){
	this->max_calculation=calc_max;
	this->stepsize_min=min_stepsize;
	this->stepsize_max=max_stepsize;
	this->quotient_boundary_reduce=boundary_reduce;
	this->quotient_boundary_expand=boundary_expand;
	this->starting_stepsize=start_step;
	this->stepsize_factor=step_factor;
	this->start_value=value_start;

	this->number_frc_value=int(this->stepsize_max/this->stepsize_min);

	//set all for one FRC simulation run needed values to start value
	this->reset_start_value();
	//common things
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Frc_Sim)+sizeof(frc_value), _sys_system_modules::FPL_SYS);
	//check the controll parameters
	this->check_frc_param();
}

//Default destructor
Fpl_Frc_Sim::~Fpl_Frc_Sim(void){
	this->delete_mc_sim();
	this->delete_frc_value();

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Frc_Sim)+sizeof(frc_value), _sys_system_modules::FPL_SYS);
}
//_________
//public

//Initialize the fragility curve calculation
void Fpl_Frc_Sim::init_frc_calculation(void){
	try{
		this->allocate_mc_sim();
		this->number_frc_value=int(this->stepsize_max/this->stepsize_min);
		this->allocate_frc_value();
	}
	catch(Error msg){
		ostringstream info;
		info << "Try to initialize the fragility curve calculation" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Input the control parameters per database table
void Fpl_Frc_Sim::set_input(QSqlDatabase *ptr_database, const bool output){
	//output
	ostringstream prefix;
	ostringstream cout;
	if(output==true){
		prefix <<"INP> ";
		Sys_Common_Output::output_fpl->set_userprefix(&prefix);

		cout << "Set control parameter for the FRC simulation per database..."  << endl;
		Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		cout.str("");
	}

	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	int number_result=0;
	//the table is set the name and the column names: it is the same than for the mc-parameters
	try{
		Fpl_Mc_Sim::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the complet table of control parameters FPL
	model.setTable(Fpl_Mc_Sim::table->get_table_name().c_str());
    //set the query; select all in table
	Data_Base::database_request(&model);
	//check the query
	if(model.lastError().isValid()){
		//warn msg
		Warning msg=set_warning(6);
		stringstream info;
		info << "Table Name       : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info : " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
		number_result=0;
	}
	else{
		number_result=model.rowCount();
	}
	//read out the results
	 //the set of the name-column has to match to this parameter
	//output
	if(number_result>0){
		cout << "Search for the FRC control parameters in table " <<  Fpl_Mc_Sim::table->get_table_name() <<"..."<< endl;

		//read out from the search result
		string buffer;
		for(int i=0; i< number_result; i++){
			buffer=model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name)).c_str()).toString().toStdString();
			if(buffer==fpl_control_param::frc_max){
				max_calculation=model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toInt();
				cout << "Control parameter " << fpl_control_param::frc_max <<"  is found"  << endl;
			}
			else if(buffer==fpl_control_param::frc_step_min){
				stepsize_min=model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toDouble();
				cout << "Control parameter " << fpl_control_param::frc_step_min <<"  is found"  << endl;
			}
			else if(buffer==fpl_control_param::frc_step_max){
				stepsize_max=model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toDouble();
				cout << "Control parameter " << fpl_control_param::frc_step_max <<"  is found"  << endl;
			}
			else if(buffer==fpl_control_param::frc_step_start){
				starting_stepsize=(model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toDouble());
				cout << "Control parameter " << fpl_control_param::frc_step_start <<"  is found"  << endl;
			}
			else if(buffer==fpl_control_param::frc_height_start){
				start_value=(model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toDouble());
				cout << "Control parameter " << fpl_control_param::frc_height_start <<"  is found"  << endl;
			}
			else if(buffer==fpl_control_param::frc_step_expand){
				quotient_boundary_expand=(model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toDouble());
				cout << "Control parameter " << fpl_control_param::frc_step_expand <<"  is found"  << endl;
			}
			else if(buffer==fpl_control_param::frc_step_reduce){
				quotient_boundary_reduce=(model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toDouble());
				cout << "Control parameter " << fpl_control_param::frc_step_reduce <<"  is found"  << endl;
			}
			else if(buffer==fpl_control_param::frc_step_factor){
				stepsize_factor=(model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toDouble());
				cout << "Control parameter " << fpl_control_param::frc_step_factor <<"  is found"  << endl;
			}
		}
		//output
		if(output==true){
			Sys_Common_Output::output_fpl->output_txt(cout.str(),true);
		}
	}
	if(output==true){
		Sys_Common_Output::output_fpl->rewind_userprefix();
	}
	//check the control parameter
	this->check_frc_param();
	//set the control parameter for the mc-sim
	try{
		this->mc_sim_frc->set_input(ptr_database, output);
	}catch(Error msg){
		throw msg;
	}
}
//Output the control parameters of the fragility curve simualtion to display/console
void Fpl_Frc_Sim::output_members(void){
	ostringstream prefix;
	prefix << "OUT> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	ostringstream cout;
	cout << "CONTROLPARAMETER FRC-CALCULATION " <<  endl;
	cout << " Maximum number of calculation    : " << this->max_calculation << label::no_unit<< endl;
	cout << " Minimum stepsize                 : " << this->stepsize_min << label::m << endl;
	cout << " Maximum stepsize                 : " << this->stepsize_max << label::m << endl;
	cout << " Starting stepsize                : " << this->starting_stepsize << label::m << endl;
	cout << " Starting waterlevel              : " << this->start_value <<label::m << endl;
	cout << " Quotient for stepsize expanding  : " << this->quotient_boundary_expand <<label::no_unit<< endl;
	cout << " Quotient for stepsize reducing   : " << this->quotient_boundary_reduce <<label::no_unit << endl;
	cout << " Stepsize factor                  : " << this->stepsize_factor <<label::no_unit << endl;
	//output
	Sys_Common_Output::output_fpl->output_txt(cout.str(),true);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Perform a fragility curve calculation for an given fpl-section
void Fpl_Frc_Sim::make_frc_sim(Fpl_Section *section_to_calc, QSqlDatabase *ptr_database){
	//output begin for analysis
	this->out_text_frcsampling(0);

	//check if the section is not set by hand or has an unknown type
	if(section_to_calc->get_type_section()!=_fpl_section_types::by_hand_sec && section_to_calc->get_type_section()!=_fpl_section_types::unknown_sec){
		//delete the frc result from the section
		try{
			Fpl_Mc_Sim::delete_data_in_result_table(ptr_database, section_to_calc->get_id_section(),section_to_calc->get_sys_system_id(),false);
			section_to_calc->delete_detailed_results_database(ptr_database, _fpl_simulation_type::sim_frc);
		}
		catch(Error msg){
			throw msg;
		}
		try{
			//set all for one FRC simulation run needed values to start value
			this->reset_start_value();
			this->reset_frc_value();
			//for time recording
			time_t start,end;
			//start the frc-loop
			time(&start);//begin time recording
			for(int i=0; i<this->max_calculation; i++){
				Fpl_Calculation::check_stop_thread_flag();
				//begin output for each run
				ostringstream userprefix;
				userprefix << "R_" <<i<<"_H_"<<FORMAT_FIXED_REAL<< P(2)<<this->frc_value_actuel.waterlevel<<"> ";
				Sys_Common_Output::output_fpl->set_userprefix(&userprefix);
				//check if the waterlevel is already calculated
				if(this->check_calculated() && this->frc_value_actuel.waterlevel!=0.0){
					//output
					this->out_text_frcsampling(1);
					//caluclate the gradient and the qoutient of the gradients
					this->calc_grad_quo();
				}
				else{
					//it is not calculted=> make a mc-run
					this->result_of_mc=this->mc_sim_frc->make_mc_sim(this->frc_value_actuel.waterlevel  , section_to_calc, ptr_database );
					this->frc_value_actuel.failure_prob=this->result_of_mc.failure_prob;

					//caluclate the gradient and the qoutient of the gradients
					this->calc_grad_quo();
					//copy the result vector
					this->copy_frc_value();
					//count the mc-runs
					this->counter_mc++;
				}

				//output
				this->output_result();
				//check the abortion of calculation and update the stepsize
				this->update_stepsize_waterlevel(i);

				this->counter_calc++;//count the frc-loops

				//check if it is abortion of calculation
				if(this->abort_flag==true){
					Sys_Common_Output::output_fpl->rewind_userprefix();
					break;
				}
				Fpl_Calculation::check_stop_thread_flag();
				//end output each run
				Sys_Common_Output::output_fpl->rewind_userprefix();
			}//end the frc_sloop

			//time calcul
			time(&end);//end time recording
			this->time_diff=end-start;//time difference

			//end output analysis
			this->out_text_frcsampling(2);
			//output the statistics
			this->output_statistics();
		}
		catch(Error msg){
			throw msg;
		}
	}
	//it is an unkown section type (-1) or it is set by hand(0)
	else{
		//output
		ostringstream cout;
		cout << "No MC-sampling is possible for section type "<< _Fpl_Section_Type::convert_section_type2txt(section_to_calc->get_type_section()) << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
	}
};
//_________
//privat
//Allocate the monte carlo simulation
void Fpl_Frc_Sim::allocate_mc_sim(void){
	try{
		this->mc_sim_frc=new Fpl_Mc_Sim();
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the monte carlo simulation
void Fpl_Frc_Sim::delete_mc_sim(void){
	if(this->mc_sim_frc!=NULL){
		delete this->mc_sim_frc;
		this->mc_sim_frc=NULL;
	}
}
//Allocate pair of variates of the fragility curve to avoid double calculation of a waterlevel
void Fpl_Frc_Sim::allocate_frc_value(void){
	try{
		this->frc_value=new _fpl_frc_values[this->number_frc_value];
		Sys_Memory_Count::self()->add_mem(sizeof(_fpl_frc_values)*this->number_frc_value, _sys_system_modules::FPL_SYS);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init the values
	for(int i=0; i< this->number_frc_value; i++){
		frc_value[i].failure_prob=0.0;
		frc_value[i].waterlevel=0.0;
	}
}
//Delete the pair of variates of the fragility curve to avoid double calculation of a waterlevel
void Fpl_Frc_Sim::delete_frc_value(void){
	if(this->frc_value!=NULL){
		delete []frc_value;
		this->frc_value=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_frc_values)*this->number_frc_value, _sys_system_modules::FPL_SYS);
	}
}
//Reset the pair of variates of the fragility curve to avoid double calculation of a waterlevel
void Fpl_Frc_Sim::reset_frc_value(void){
	//init the vector of the pair variates of the frc to avoid multiple calculation with a mc-analysis
	for(int i=0; i<this->number_frc_value;i++){
		this->frc_value[i].failure_prob=0.0;
		this->frc_value[i].waterlevel=0.0;
	}
}
//Check the control parameters for a fragility curve simulation
void Fpl_Frc_Sim::check_frc_param(void){
	if( max_calculation <= 0){
		Warning msg=set_warning(0);
		msg.output_msg(1);
		//reaction use default values
		max_calculation=fpl_control_param::max_calculation_def;
	}
	if(stepsize_min <= 0.0){
		Warning msg=set_warning(1);
		msg.output_msg(1);
		//reaction use default values
		stepsize_min=fpl_control_param::stepsize_min_def;
	}
	if(stepsize_max <= 0.0){
		Warning msg=set_warning(2);
		msg.output_msg(1);
		//reaction use default values
		stepsize_max=fpl_control_param::stepsize_max_def;
	}
	if(stepsize_min>stepsize_max){
		Warning msg=set_warning(3);
		msg.output_msg(1);
		//reaction
		stepsize_max=stepsize_min;
	}
	if(starting_stepsize <= 0.0){
		Warning msg=set_warning(4);
		msg.output_msg(1);
		//reaction use default values
		starting_stepsize=fpl_control_param::starting_stepsize_def;
	}
	if(stepsize_factor <= 0.0){
		Warning msg=set_warning(5);
		msg.output_msg(1);
		//reaction use default values
		stepsize_factor=fpl_control_param::stepsize_factor_def;
	}
	if(quotient_boundary_expand < 1.0){
		Warning msg=set_warning(7);
		msg.output_msg(1);
		//reaction use default values
		quotient_boundary_expand=fpl_control_param::quotient_boundary_expand_def;
	}
	if(quotient_boundary_reduce < 1.0){
		Warning msg=set_warning(8);
		msg.output_msg(1);
		//reaction use default values
		quotient_boundary_reduce=fpl_control_param::quotient_boundary_reduce_def;
	}
	if(quotient_boundary_reduce < quotient_boundary_expand){
		Warning msg=set_warning(9);
		msg.output_msg(1);
		//reaction use default values
		quotient_boundary_expand=quotient_boundary_reduce;
	}
	ostringstream cout;
}
//Reset the start values of the fragility curve simulation
void Fpl_Frc_Sim::reset_start_value(void){
	//set all for one FRC simulation run needed values to 0
	//dynamic stepsize handling
	stepsize=starting_stepsize;
	//for the gradient calculation
	this->frc_value_actuel.failure_prob=0.0;
	this->frc_value_actuel.waterlevel=0.0;
	this->frc_value_mid_1.failure_prob=0.0;
	this->frc_value_mid_1.waterlevel=0.0;
	this->frc_value_mid_2.failure_prob=0.0;
	this->frc_value_mid_2.waterlevel=0.0;
	this->frc_value_last.failure_prob=0.0;
	this->frc_value_last.waterlevel=0.0;
	this->gradient=0.0;
	this->gradient_old=0.0;
	this->quotient_grad=0.0;

	this->abort_counter=0;

	//set the start value of waterlevel
	this->frc_value_actuel.waterlevel=start_value;
	//statistic values
	this->counter_calc=0;
	this->counter_mc=0;

	this->abort_flag=false;
	//result structure of the mc-simulation
	this->result_of_mc.failure_prob=0.0;
	this->result_of_mc.low_failure_prob=0.0;
	this->result_of_mc.up_failure_prob=0.0;
	this->result_of_mc.waterlevel=0.0;
}
//Check the criterion for abortion of the calculation and update the stepsize
void Fpl_Frc_Sim::update_stepsize_waterlevel(const int number_of_calc){
	if(number_of_calc>2){//(three steps are necessary!)
		if((quotient_grad>quotient_boundary_reduce || quotient_grad<0.0) && stepsize!=stepsize_min){//stepsize to stepsize_min
			//update the stepsize
			stepsize=stepsize_min;
			//reset the copy of the waterlevel and probabilties
			//mid_2 value to actuel value
			frc_value_actuel.failure_prob=frc_value_mid_2.failure_prob;
			frc_value_actuel.waterlevel=frc_value_mid_2.waterlevel;
			//mid_2 value to mid_1 values
			frc_value_mid_1.failure_prob=frc_value_mid_2.failure_prob;
			frc_value_mid_1.waterlevel=frc_value_mid_2.waterlevel;
			//last value to mid_2 values
			frc_value_mid_2.failure_prob=frc_value_last.failure_prob;
			frc_value_mid_2.waterlevel=frc_value_last.waterlevel;
		}
		else if(quotient_grad<=quotient_boundary_expand && quotient_grad > 0.0){//expand the stepsize
			//update the stepsize
			stepsize=stepsize*stepsize_factor;
			if(stepsize>stepsize_max){//boundaries of the maximum stepsize
				stepsize=stepsize_max;
			}
		}
		else if(quotient_grad>quotient_boundary_expand && quotient_grad<=quotient_boundary_reduce){//let the stepsize constant
			//update the stepsize
			stepsize=stepsize;
		}
	}
	//abort criterion
	if(frc_value_actuel.failure_prob>=1.0-(1.0e-10) && quotient_grad<=quotient_boundary_reduce){
		this->abort_counter++;
		if(this->abort_counter==3){
			abort_flag=true;
		}
	}
	//calculate new actuel waterlevel
	frc_value_actuel.waterlevel=frc_value_actuel.waterlevel+stepsize;
}
//Calculate the gradient and the quotient of the gradient of the curve
void Fpl_Frc_Sim::calc_grad_quo(void){
	//calculate the gradients
	if(frc_value_mid_1.waterlevel==0.0 && frc_value_mid_2.waterlevel==0.0){
		gradient_old=0.0;
		gradient=0.0;
	}
	else if(frc_value_mid_1.waterlevel!=0.0 && frc_value_mid_2.waterlevel==0.0){
		gradient_old=0.0;
		gradient=(frc_value_actuel.failure_prob-frc_value_mid_1.failure_prob)/(frc_value_actuel.waterlevel-frc_value_mid_1.waterlevel);
	}
	else{
		gradient_old=(frc_value_mid_1.failure_prob-frc_value_mid_2.failure_prob)/(frc_value_mid_1.waterlevel-frc_value_mid_2.waterlevel);
		gradient=(frc_value_actuel.failure_prob-frc_value_mid_1.failure_prob)/(frc_value_actuel.waterlevel-frc_value_mid_1.waterlevel);
	}
	//calculate the quotient of the gradient
	if(gradient_old!=0.0 && gradient !=0.0 && gradient>=gradient_old){
		quotient_grad=(gradient/gradient_old);
	}
	else if(gradient_old!=0.0 && gradient !=0.0 && gradient<gradient_old){
		quotient_grad=(gradient_old/gradient);
	}
	else if(gradient_old==0.0 && gradient ==0.0){
		quotient_grad=1.0;
	}
	else{
		quotient_grad=gradient/0.000000001;
	}
	//copy the values
	//mid_2 value to last values
	frc_value_last.failure_prob=frc_value_mid_2.failure_prob;
	frc_value_last.waterlevel=frc_value_mid_2.waterlevel;
	//mid_1 value to mid_2 values
	frc_value_mid_2.failure_prob=frc_value_mid_1.failure_prob;
	frc_value_mid_2.waterlevel=frc_value_mid_1.waterlevel;
	//actuel value to mid value
	frc_value_mid_1.failure_prob=frc_value_actuel.failure_prob;
	frc_value_mid_1.waterlevel=frc_value_actuel.waterlevel;
}

//The stored frc-values are copied for a new calculation step
void Fpl_Frc_Sim::copy_frc_value(void){
	//copy the vector to avoid the multiple calculation of already calculated waterlevel
	for (int i=number_frc_value-1; i>0; i--){
		frc_value[i].failure_prob=frc_value[i-1].failure_prob;
		frc_value[i].waterlevel=frc_value[i-1].waterlevel;
	}
	frc_value[0].failure_prob=frc_value_actuel.failure_prob;
	frc_value[0].waterlevel=frc_value_actuel.waterlevel;
}
//Check if the waterlevel to calculate is already calculated and stored in frc_value
bool Fpl_Frc_Sim::check_calculated(void){
	bool found_flag=false;
	for(int i=0; i<number_frc_value; i++){
		//check if the waterlevel is already calculated and stroed with the help of an epsilon-interval
		if(frc_value_actuel.waterlevel<=frc_value[i].waterlevel+stepsize_min/100000.0 && frc_value_actuel.waterlevel>=frc_value[i].waterlevel-stepsize_min/100000.0){
			frc_value_actuel.failure_prob=frc_value[i].failure_prob;
			found_flag=true;
		}
	}
	return found_flag;
}
//Output the statistics of the fragility curve calculation to display/console
void Fpl_Frc_Sim::output_statistics(void){
		ostringstream cout;
		cout << "STATISTICS FRC-SIMULATION" << endl;
		cout << " Number of points             : " << this->counter_calc<< endl;
		cout << "  MC-runs/stored values       : " << this->counter_mc<<"/"<< (this->counter_calc-this->counter_mc) <<endl;
		cout << " Computation time             : " << this->time_diff << label::sec << endl;
		if(this->counter_calc==this->max_calculation){
			cout << " Total numbers of FRC-Simulation is reached" << endl;
		}
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
}
//Output the results of the fragility curve calculation to display/console
void Fpl_Frc_Sim::output_result(void){
	ostringstream cout;
	//always
	cout << "RESULT FRC POINT CALCULATION "<< endl;
	cout << " Waterlevel   : " << this->frc_value_actuel.waterlevel<< label::m << endl;
	cout << " Probability  : " << this->frc_value_actuel.failure_prob << label::no_unit <<endl;;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	//detailed
	cout << "CURVE BEHAVIOUR " << endl;
	cout << " Gradient (i-1)       : " << this->gradient_old << endl;
	cout << " Gradient (i)         : " << this->gradient << endl;
	cout << " Quotient of gradient : " << this->quotient_grad  << endl;
	cout << " Stepsize             : " << this->stepsize  << label::m<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
}
//Output a text header for the fragility curve simulation corresponding the given type to display/console
void Fpl_Frc_Sim::out_text_frcsampling(const int type){
	//output
	ostringstream cout;
	switch(type){
		case 0://begin
			cout << "Start the FRC-simulation..."<< endl ;
			break;
		case 1://waterlevel found
			cout <<"No MC-Sampling required, stored values are used" << endl;
			cout << " Waterlevel " << frc_value_actuel.waterlevel << label::m<< " has been already calculated" << endl;
			break;
		case 2://normal finished
			cout << "FRC-simulation finished"<< endl ;
			break;
		default:
			break;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
}

//Set warning(s)
Warning Fpl_Frc_Sim::set_warning(const int warn_type){
		string place="Fpl_Frc_Sim::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0:
			place.append("check_frc_param(void)") ;
			reason="Maximal number of FRC-Calculation is <=0";
			reaction="The default value is taken";
			help= "Check the Parameter FRC_MAX";
			info << "Default Value " << fpl_control_param::max_calculation_def << endl;
			type=7;
			break;
		case 1:
			place.append("check_frc_param(void)") ;
			reason="Minimal stepsize is  <= 0";
			reaction="The default value is taken";
			help= "Check the Parameter FRC_STEP_MIN";
			info << "Default Value " << fpl_control_param::stepsize_min_def << endl;
			type=7;
			break;
		case 2:
			place.append("check_frc_param(void)") ;
			reason="Maximal stepsize is  <= 0";
			reaction="The default value is taken";
			help= "Check the Parameter FRC_STEP_MAX";
			info << "Default Value " << fpl_control_param::stepsize_max_def << endl;
			type=7;
			break;
		case 3:
			place.append("check_frc_param(void)") ;
			reason="Maximal stepsize is  < Minimal stepsize";
			reaction="The maximal stepsize is set to the minimal one => no dynamic stepsize control";
			help= "Check the Parameter FRC_STEP_MAX and FRC_STEP_MIN";
			info << "FRC_STEP_MAX " << stepsize_max << " FRC_STEP_MIN " << stepsize_min << endl;
			type=7;
			break;
		case 4:
			place.append("check_frc_param(void)") ;
			reason="Starting stepsize is  <= 0";
			reaction="The default value is taken";
			help= "Check the Parameter FRC_STEP_START";
			info << "Default Value " << fpl_control_param::starting_stepsize_def << endl;
			type=7;
			break;
		case 5:
			place.append("check_frc_param(void)") ;
			reason="Stepsize factor is  <= 0";
			reaction="The default value is taken";
			help= "Check the Parameter FRC_STEP_FACTOR";
			info << "Default Value " << fpl_control_param::stepsize_factor_def << endl;
			type=7;
			break;
		case 6:
			place.append("set_input(QSqlDatabase *ptr_database)") ;
			reason="Table not found";
			help="Check the table name";
			reaction="All default values are taken";
			type=2;
			break;
		case 7:
			place.append("check_frc_param(void)") ;
			reason="Quotient for stepsize expanding < 1.0";
			reaction="The default value is taken";
			help= "Check the Parameter FRC_STEP_EXPAND";
			info << "Default Value " << fpl_control_param::quotient_boundary_expand_def << endl;
			type=7;
			break;
		case 8:
			place.append("check_frc_param(void)") ;
			reason="Quotient for stepsize reducing < 1.0";
			reaction="The default value is taken";
			help= "Check the Parameter FRC_STEP_REDUCE";
			info << "Default Value " << fpl_control_param::quotient_boundary_reduce_def << endl;
			type=7;
			break;
		case 9:
			place.append("check_frc_param(void)") ;
			reason="Quotient for stepsize reducing < Quotient for stepsize expanding";
			reaction="The expanding Quotient is set to the reducing one => no constant stepsize";
			help= "Check the Parameter FRC_STEP_REDUCE and FRC_STEP_EXPAND";
			info << "FRC_STEP_REDUCE " << quotient_boundary_reduce <<" FRC_STEP_EXPAND " << quotient_boundary_expand <<  endl;
			type=7;
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
};
//Set error(s)
Error Fpl_Frc_Sim::set_error(const int err_type){
		string place="Fpl_Frc_Sim::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;
	switch (err_type){
		case 0://bad alloc
			place.append("allocate_mc_sim(void)/allocate_frc_value(void)");
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