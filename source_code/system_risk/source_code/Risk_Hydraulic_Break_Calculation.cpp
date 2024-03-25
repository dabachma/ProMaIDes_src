#include "Risk_Headers_Precompiled.h"
//#include "Risk_Hydraulic_Break_Calculation.h"

//init static members
bool Risk_Hydraulic_Break_Calculation::stop_hyd_by_next=false;

//Default constructor
Risk_Hydraulic_Break_Calculation::Risk_Hydraulic_Break_Calculation(void){

	this->threads=NULL;
	this->hydraulic_state.file_output_required=true;
	this->hydraulic_state.number_threads=3;
	this->ptr_database=NULL;
	this->required_threads=1;
	this->risk_type=_risk_type::scenario_risk;

	this->hyd_system_is_active=false;

	this->ptr_scenarios=NULL;

	this->set_exception_number_start();

	this->breach_has_started=false;
	this->probability_output=false;

	Sys_Memory_Count::self()->add_mem(sizeof(Risk_Hydraulic_Break_Calculation),_sys_system_modules::RISK_SYS);//count the memory

}
//Default destructor
Risk_Hydraulic_Break_Calculation::~Risk_Hydraulic_Break_Calculation(void){

	Risk_Hydraulic_Break_Calculation::stop_hyd_by_next=false;
	this->delete_hyd_threads();
	Sys_Memory_Count::self()->minus_mem(sizeof(Risk_Hydraulic_Break_Calculation),_sys_system_modules::RISK_SYS);//count the memory

}
//_________
//public
//Set the hydraulic state data
void Risk_Hydraulic_Break_Calculation::set_hyd_state_data(const _hyd_state_data hydraulic_state){
	this->hydraulic_state=hydraulic_state;
}
//Set a pointer to the database (no copy)
void Risk_Hydraulic_Break_Calculation::set_ptr2database(QSqlDatabase *ptr_database){
	this->ptr_database=ptr_database;
}
//Set a pointer to the scenarios, which sholuid be calculated
void Risk_Hydraulic_Break_Calculation::set_ptr2scenarios(Risk_Break_Scenario_Management *ptr_sc){
	this->ptr_scenarios=ptr_sc;
}
//Set the risk type
void Risk_Hydraulic_Break_Calculation::set_risk_type(const _risk_type type){
	this->risk_type=type;
	if(type==_risk_type::nobreak_risk){
		this->risk_type=_risk_type::catchment_risk;
	}
	if(this->risk_type==_risk_type::scenario_risk){
		this->required_threads=1;
	}
	else if(this->risk_type==_risk_type::catchment_risk){
		this->required_threads=this->hydraulic_state.number_threads;
		if(this->required_threads > constant::max_threads){
			//warning
			Warning msg=this->set_warning(1);
			ostringstream info;
			info <<"Required number : " << this->required_threads << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
			//reaction
			this->required_threads=constant::max_threads;
		}

		int ideal_no=-1;
		ideal_no=QThread::idealThreadCount();
		if(ideal_no<=0){
			ideal_no=1;
		}
		if(ideal_no>1){
			ideal_no=ideal_no-1;
		}
		if(this->required_threads> ideal_no){
			//warning
			Warning msg=this->set_warning(0);
			ostringstream info;
			info <<"Required number : " << this->required_threads << endl;
			info <<"Ideal number    : " << ideal_no<<endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);

		}
	}
	else{
		this->required_threads=1;
	}
}
//Allocate and load the hydraulic threads from database
void Risk_Hydraulic_Break_Calculation::alloc_load_hyd_threads(void){
	ostringstream cout;
	this->set_exception_number_start();
	//load the base hydraulic system
	Hyd_Boundary_Szenario_Management base_sc;
	base_sc.set_systemid(this->system_id);
	base_sc.set_szenario_per_db(this->ptr_database);

	//check if the number of required threads
	int number_hydraulic=this->ptr_scenarios->get_number_hyd_calculation(this->ptr_database);
	if(number_hydraulic<this->required_threads){
		this->required_threads=number_hydraulic;
	}
	if(number_hydraulic==0){
		cout << "No hydraulic calculation are required for the given scenario(s)..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		return;
	}

	try{
		if(base_sc.get_number_sz()==0){
			Error msg=this->set_error(1);
			throw msg;

		}
		try{
			this->allocate_hyd_threads();
		}
		catch(Error msg){
			throw msg;
		}

		ostringstream prefix;
		prefix << "HYD_FPL> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);
		
		//input of the base system
		try{
			cout << "Input the HYD-base system data from database for the base thread..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			cout << "Input the HYD-base system data from database for the base thread. Further information see HYD display..." << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			this->set_hydsystem_is_active(true);
			this->threads[0].set_system_number(base_sc.get_ptr_sz(0)->get_id());
			//set hydraulic boundary scenario id
			this->threads[0].set_new_hyd_bound_sz_id(*base_sc.get_ptr_sz(0));
			this->threads[0].set_system_per_database();
			if(this->risk_type==_risk_type::scenario_risk){
				this->threads[0].set_total_river2no_overflow();
			}
			this->threads[0].output_glob_models();
			
			cout << "Input of the HYD-base system data from database for the base thread is finished!" << endl ;
			Sys_Common_Output::output_hyd->output_txt(cout.str());
			this->set_hydsystem_is_active(false);
			Sys_Common_Output::output_risk->output_txt(&cout);
			cout << endl;
			Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
			Sys_Common_Output::output_risk->reset_prefix_was_outputed();
			Sys_Common_Output::output_hyd->output_txt(&cout);

		}
		catch(Error msg){
			if(msg.get_user_aborted_exception()==false){
				this->threads[0].increase_error_number();
			}
			this->threads[0].output_final_error_warning_number();
			this->set_hydsystem_is_active(false);
			Sys_Common_Output::output_risk->rewind_userprefix();
			throw msg;
		}
		

		//idealisation of first hyd thread
		try{
			prefix<<"RISK> ";
			Sys_Common_Output::output_fpl->set_userprefix(&prefix);
			if(this->risk_type==_risk_type::scenario_risk){
				Risk_Break_Scenario buffer;
				buffer=(this->ptr_scenarios->get_ptr_sc2calc())[0];
				Fpl_Section *buff_section;
				
				
				Sys_Common_Output::output_fpl->set_userprefix(&prefix);
				buff_section=this->ptr_scenarios->get_ptr_fpl_section((buffer.get_involved_section(0).id_fpl_sec));
				cout << "Idealise HYD-system with FPL-section " << buff_section->get_name_section()<<"..." << endl;
				Sys_Common_Output::output_fpl->output_txt(&cout);
				cout << "Idealise HYD-system with FPL-section " << buff_section->get_name_section()<<". Further information see FPL display..." << endl ;
				Sys_Common_Output::output_risk->output_txt(&cout);
				Sys_Common_Output::output_fpl->set_focus_on_display();
				emit send_fpl_thread_runs(true);
				Fpl_Calculation my_fpl;
				try{
					my_fpl.idealise_hyd_system_by_section(buff_section, &this->threads[0]);
				}
				catch(Error msg){
					emit send_fpl_thread_runs(false);
					my_fpl.output_statistic_idealisation();
					throw msg;
				}
				
				cout << "Idealisation of the HYD-system with the FPL-section is finished" << endl;
				Sys_Common_Output::output_fpl->output_txt(cout.str());
				my_fpl.output_statistic_idealisation();
				Sys_Common_Output::output_risk->output_txt(&cout);
				cout << endl;
				Sys_Common_Output::output_fpl->output_txt(&cout);
				emit send_fpl_thread_runs(false);
				Sys_Common_Output::output_risk->set_focus_on_display();

			}
			else if(this->risk_type==_risk_type::catchment_risk){
				//idealisize all section
				Fpl_Calculation my_fpl;
				for(int i=0; i< this->ptr_scenarios->get_number_fpl_section(); i++){
					if(i==0){
						cout << "Idealise HYD-system with "<<this->ptr_scenarios->get_number_fpl_section()<<" relevant FPL-section(s)..." << endl;
						Sys_Common_Output::output_fpl->output_txt(&cout);
						cout << "Idealise HYD-system with "<<this->ptr_scenarios->get_number_fpl_section()<<" relevant FPL-section(s). Further information see FPL display..." << endl ;
						Sys_Common_Output::output_risk->output_txt(&cout);
						Sys_Common_Output::output_fpl->set_focus_on_display();
						emit send_fpl_thread_runs(true);
					}
					try{
						my_fpl.idealise_hyd_system_by_section(this->ptr_scenarios->get_ptr_fpl_section_index(i), &this->threads[0]);
					}
					catch(Error msg){
						emit send_fpl_thread_runs(false);
						my_fpl.output_statistic_idealisation();
						throw msg;
					}
				}
				cout << "Idealisation of the HYD-system with all FPL-section(s) is finished" << endl;
				Sys_Common_Output::output_fpl->output_txt(cout.str());
				my_fpl.output_statistic_idealisation();
				Sys_Common_Output::output_risk->output_txt(&cout);
				cout << endl;
				Sys_Common_Output::output_fpl->output_txt(&cout);
				emit send_fpl_thread_runs(false);
				Sys_Common_Output::output_risk->set_focus_on_display();
			}
		}
		catch(Error msg){
			Sys_Common_Output::output_fpl->rewind_userprefix();
			Sys_Common_Output::output_risk->rewind_userprefix();
			throw msg;
		}

		Sys_Common_Output::output_fpl->rewind_userprefix();
		Sys_Common_Output::output_risk->reset_prefix_was_outputed();


		//init the first thread
		cout << "Initialize the HYD-system for the base thread..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		cout << "Initialize the HYD-system for the base thread. Further information see HYD display..." << endl ;
		Sys_Common_Output::output_risk->output_txt(&cout);
		this->set_hydsystem_is_active(true);

		try{ 
			this->threads[0].init_models();
			Risk_System::check_stop_thread_flag();
			this->threads[0].output_setted_members();
			Risk_System::check_stop_thread_flag();
			this->threads[0].init_solver();
			this->threads[0].set_preproc_flag(true);
		}
		catch(Error msg){
			if(msg.get_user_aborted_exception()==false){
				this->threads[0].increase_error_number();
			}
			this->threads[0].output_final_error_warning_number();
			this->set_hydsystem_is_active(false);
			Sys_Common_Output::output_risk->rewind_userprefix();
			throw msg;
		}
		cout << "Initialisation of the HYD-system for the base thread is finished" << endl;
		Sys_Common_Output::output_hyd->output_txt(cout.str());
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
		

		//clone
		try{
			if(this->required_threads>1){
				cout << "Clone the HYD-system of the base thread to " << this->required_threads<<" thread(s)..." << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				cout << "Clone the HYD-system of the base thread to " << this->required_threads<<" thread(s). Further information see HYD display..." << endl ;
				Sys_Common_Output::output_risk->output_txt(&cout);
				for(int i=1; i< this->required_threads; i++){
					this->threads[i].clone_system(&this->threads[0]);
					this->threads[i].set_preproc_flag(true);
				}
				cout << "Cloning of the HYD-system of the base thread to the other thread(s) is finished" << endl;
				Sys_Common_Output::output_hyd->output_txt(cout.str());
				Sys_Common_Output::output_risk->output_txt(&cout);
				cout << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
			}
		}
		catch(Error msg){
			if(msg.get_user_aborted_exception()==false){
				this->threads[0].increase_error_number();
			}
			this->threads[0].output_final_error_warning_number();
			this->set_hydsystem_is_active(false);
			Sys_Common_Output::output_risk->rewind_userprefix();
			throw msg;
		}


		//reset
		try{
			this->reset_hyd_threads();
		}
		catch(Error msg){
			Sys_Common_Output::output_risk->rewind_userprefix();;
			this->set_hydsystem_is_active(false);
			throw msg;
		}
	}
	catch(Error msg){
		this->set_exception_number_end();
		this->output_final_statistic_loading();
		throw msg;
	}

	Sys_Common_Output::output_risk->rewind_userprefix();
	this->set_hydsystem_is_active(false);

	this->set_exception_number_end();
	this->output_final_statistic_loading();
	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
}
//Calculate the hydraulic threads
void Risk_Hydraulic_Break_Calculation::calculate_hyd_threads(void){

	if(this->required_threads==0){
		return;
	}
	ostringstream cout;
	this->set_exception_number_start();
	//begin time recording	
	time(&this->start_time);
	cout << "HYD-calculation of currently " << this->ptr_scenarios->get_number_sc2calc() <<" hydraulic scenario(s) with "<< this->required_threads<<" threads..."<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	int counter=0;
	int counter_total=0;
	try{
		this->set_hydsystem_is_active(true);
		do{
			//init threads with the list of break scenarios
			this->init_break_scenario_hyd_threads(&counter);
		
			//calculate hydraulic threads
			ostringstream buffer;
			buffer << this->required_threads;
			emit emit_number_threads(buffer.str().c_str());

			for(int i=0; i<this->required_threads; i++){
				if(threads[i].get_preproc_flag()==true){
					this->threads[i].start();
				}
			}
			//wait-loop
			this->wait_loop();

			this->check_for_break_scenario_risk();

			this->set_hydsystem_is_active(true);

			//postprocessing
			this->make_postprocessing_hyd_system();

			//reset thredas
			this->reset_hyd_threads();




			counter_total=counter_total+counter;
		
			ostringstream prefix;
			if(this->ptr_risk_thread->get_thread_type()!=_risk_thread_type::risk_calc_catchment_auto){
				prefix<<"STATUS> ";
				Sys_Common_Output::output_risk->set_userprefix(&prefix);
				cout <<counter_total << " of currently " << this->ptr_scenarios->get_number_sc2calc() <<" hydraulic break scenario(s) calculated"<<endl;
				if(this->probability_output==true){
					cout << this->ptr_scenarios->total_break_list.get_prob_scenario_calculated(this->ptr_database, this->system_id);
					cout << label::percentage<< " of the total probability is reached"<<endl;
				}
				counter_total=0;
				Sys_Common_Output::output_risk->output_txt(&cout);
				Sys_Common_Output::output_risk->rewind_userprefix();
			}

			
			

			
			if(Risk_Hydraulic_Break_Calculation::stop_hyd_by_next==true){
				cout <<"User has stopped the hydraulic calculation for the catchment area risk approach."<<endl;
				Sys_Common_Output::output_risk->output_txt(&cout);
				Sys_Common_Output::output_risk->set_focus_on_display();
				this->ptr_scenarios->cut_list_sc2calc(counter);
			}
			else if(counter_total>=this->ptr_risk_thread->get_ca_control_parameter().max_hyd_calc){
				cout <<"Maximum number ("<<this->ptr_risk_thread->get_ca_control_parameter().max_hyd_calc<<") of hydraulic calculations are reached."<<endl;
				Sys_Common_Output::output_risk->output_txt(&cout);
				Sys_Common_Output::output_risk->set_focus_on_display();
				this->ptr_scenarios->cut_list_sc2calc(counter);
			}
			else if(this->ptr_risk_thread->get_thread_type()==_risk_thread_type::risk_calc_catchment_auto 
				&& counter < this->ptr_scenarios->get_number_sc2calc() ){
				//update list to calc
				cout << "Create the new DAM- and FPL-break information and update the list of hydraulic break scenario(s)..."<<endl;
				cout << "Further information see DAM- and RISK-display..."<<endl;
				

				Sys_Common_Output::output_hyd->output_txt(&cout);
				this->ptr_risk_thread->cumulate_catchment_risk(true);
				this->ptr_scenarios->set_auto_break_risk_state_list(this->ptr_database, this->ptr_risk_thread->get_ca_control_parameter());
				counter=0;
				Sys_Common_Output::output_hyd->set_focus_on_display();
				cout << "Updating of the list of hydraulic break scenario(s) is finished!"<<endl;
				ostringstream prefix;
				prefix<<"STATUS> ";
				Sys_Common_Output::output_risk->set_userprefix(&prefix);
				cout <<"Total number calculated       : " <<counter_total<<endl;
				cout <<"Estimated number to calculate : "<<this->ptr_scenarios->get_number_sc2calc() <<endl;
				if(this->probability_output==true){
					cout << this->ptr_scenarios->total_break_list.get_prob_scenario_calculated(this->ptr_database, this->system_id);
					cout << label::percentage<< " of the total probability is reached"<<endl;
				}
				Sys_Common_Output::output_risk->output_txt(&cout);
				Sys_Common_Output::output_risk->rewind_userprefix();;
				cout << "Go on with the hydraulic calculation..."<<endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				
			}



		}while(counter<this->ptr_scenarios->get_number_sc2calc());

		
	}
	catch(Error msg){
		this->set_hydsystem_is_active(false);
		this->set_exception_number_end();
		time(&this->actual_time);
		this->output_final_statistic_calculation();
		throw msg;
	}
	time(&this->actual_time);
	this->set_exception_number_end();
	this->output_final_statistic_calculation();
	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	this->set_hydsystem_is_active(false);
}
//Get hydraulic break threads are running
bool Risk_Hydraulic_Break_Calculation::get_hydraulic_break_thread_running(int *number){
	int counter=0;
	bool runs=false;
	if(this->threads!=NULL){
		if(this->hyd_system_is_active==true){
			runs=true;
		}
		for(int i=0; i< this->required_threads; i++){
			if(this->threads[i].isRunning()==true){
				counter++;
				runs=true;
			}
		}

	}
	*number=counter;
	return runs;
}
//Get hydraulic break threads are running
bool Risk_Hydraulic_Break_Calculation::get_hydraulic_break_thread_running(void){
	bool runs=false;
	if(this->threads!=NULL){
		if(this->hyd_system_is_active==true){
			runs=true;
		}
		for(int i=0; i< this->required_threads; i++){
			if(this->threads[i].isRunning()==true){
				runs=true;
			}
		}

	}
	return runs;
}
//Set if the hydraulic system classes are active
void Risk_Hydraulic_Break_Calculation::set_hydsystem_is_active(const bool flag){
	this->hyd_system_is_active=flag;
	emit send_hyd_thread_runs(this->hyd_system_is_active);
	if(this->hyd_system_is_active==true){
		Sys_Common_Output::output_hyd->set_focus_on_display();
		
	}
	else{
		Sys_Common_Output::output_risk->set_focus_on_display();
	}
}
//Set the flag for stopping the hydraulic calculation by the next occasion (static)
void Risk_Hydraulic_Break_Calculation::set_stop_next_hyd_calc(void){
	ostringstream cout;
	if(Risk_Hydraulic_Break_Calculation::stop_hyd_by_next!=true){
		cout <<"_________________________________________________"<<endl;
		cout <<"User demands a stop at the next appropriate step."<<endl;
		cout <<"Please wait..."<<endl;
		cout <<"_________________________________________________"<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Sys_Common_Output::output_risk->set_focus_on_display();
		Risk_Hydraulic_Break_Calculation::stop_hyd_by_next=true;
	}
	else{
		cout <<"_________________________________________________"<<endl;
		cout <<"Still waiting to stop the calculation without any"<<endl;
		cout <<"loss of data."<<endl;
		cout <<"Please wait..."<<endl;
		cout <<"_________________________________________________"<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		Sys_Common_Output::output_risk->set_focus_on_display();
	}
}
//Get the flag if a breach has started (just used for a scenario based approach)
bool Risk_Hydraulic_Break_Calculation::get_breach_has_started(void){
	return this->breach_has_started;
}
//Set if there should be an output for the reached probability (just use it for the automatic state geneartion of the catchment approach!) 
void Risk_Hydraulic_Break_Calculation::set_probability_output(const bool flag){
	this->probability_output=flag;
}
//Set the pointer to the risk thread
void Risk_Hydraulic_Break_Calculation::set_ptr_risk_thread(Risk_System *thread){
	this->ptr_risk_thread=thread;
}
//Manage the output of the hydraulic classes
void Risk_Hydraulic_Break_Calculation::recieve_output_required(int thread){

	
	if(this->threads[thread].get_output_is_required()==false){
		return;
	}
	//check if as thread output is running
	for(int i=0; i< this->required_threads; i++){
		if(this->threads[i].get_output_is_running()==true){
			return;
		}
	}
	this->threads[thread].set_output_is_allowed();


}
//_________
//private
//Allocate the required hydraulic threads
void Risk_Hydraulic_Break_Calculation::allocate_hyd_threads(void){
	this->delete_hyd_threads();
	try{
		this->threads=new Hyd_Hydraulic_System[this->required_threads];
		
		ostringstream prefix;
		prefix << "RISK> ";
		Sys_Common_Output::output_hyd->set_userprefix(&prefix);

		ostringstream buffer;
		buffer << this->required_threads;
		emit emit_number_threads(buffer.str().c_str());
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init the threads
	if(this->required_threads>1){
		Hyd_Hydraulic_System::qt_thread_applied=true;
	}
	else{
		Hyd_Hydraulic_System::qt_thread_applied=false;
	}
	for(int i=0; i< this->required_threads; i++){
		this->threads[i].set_systemid(this->get_sys_system_id());
		this->threads[i].set_thread_number(i);
		if(this->ptr_database!=NULL){
			this->threads[i].set_file_output_flag(this->hydraulic_state.file_output_required);
			this->threads[i].set_ptr2database(this->ptr_database,i);
		}
		QObject::connect(&this->threads[i], SIGNAL(output_required(int)), this->ptr_risk_thread, SLOT(recieve_output_required(int )));

	}
}
//Delete the hydraulic threads
void Risk_Hydraulic_Break_Calculation::delete_hyd_threads(void){
	if(this->threads!=NULL){
		Sys_Common_Output::output_hyd->reset_userprefix();
		delete []this->threads;
		this->threads=NULL;
	}
	if(this->ptr_database!=NULL){
		//delete database
		for(int i=0; i< this->required_threads; i++){
			ostringstream buffer;
			buffer<<sys_label::str_hyd<<"_"<<i;
			if(QSqlDatabase::contains(buffer.str().c_str())==true){
				if(QSqlDatabase::database(buffer.str().c_str(),false).isOpen()==true){
					QSqlDatabase::database(buffer.str().c_str(),false).close();	
				}	
				QSqlDatabase::removeDatabase(buffer.str().c_str());
			}
		}
	}
	Hyd_Hydraulic_System::qt_thread_applied=false;
}
//Perform the postprocessing of the hydraulic system(s) calculation 
void Risk_Hydraulic_Break_Calculation::make_postprocessing_hyd_system(void){
	for(int i=0; i<this->required_threads; i++){
		Risk_System::check_stop_thread_flag();
			if(threads[i].get_preproc_flag()==true){
				try{
					ostringstream cout;
					cout <<"Postprocessing of hydraulicsystem " << this->threads[i].get_identifier_prefix(false)<<"..."<< endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
					this->threads[i].output_final_model_statistics();
					Sys_Common_Output::output_hyd->insert_separator(4);
				}
				catch(Error msg){
					ostringstream info;
					info <<"Number hydraulic system : "<< this->threads[i].get_identifier_prefix(false)<< endl;
					msg.make_second_info(info.str());
					this->threads[i].increase_error_number();
					msg.output_msg(2);
				}
			}
		}
}
//Waiting loop for the threads
void Risk_Hydraulic_Break_Calculation::wait_loop(void){
		bool all_finished=true;
		int counter=0;
		ostringstream buffer;
		//waiting loop for this thread
		do{
			all_finished=true;
			counter=0;
			buffer.str("");
			if(this->ptr_risk_thread!=NULL){
				this->ptr_risk_thread->set_thread2sleep(2);
			}
			for(int i=0; i<this->required_threads; i++){
				if(this->threads[i].isRunning()==true){
					all_finished=false;
					counter++;
				}
			}
			buffer << counter;
			emit emit_number_threads(buffer.str().c_str());

		}
		while(all_finished==false);
}
//Initialise the hydraulic threads
void Risk_Hydraulic_Break_Calculation::init_break_scenario_hyd_threads(int *counter){
	
	//QSqlQueryModel model;
	Risk_Break_Scenario *buffer;
	buffer=this->ptr_scenarios->get_ptr_sc2calc();

	ostringstream cout;
	ostringstream prefix;

	for(int i=0; i< this->required_threads; i++){

		if((*counter)==this->ptr_scenarios->get_number_sc2calc()){
			break;
		}
		do{
			cout <<"Preprocessing of risk break scenario " <<buffer[*counter].get_break_scenario_string();
			cout << " combined with the hydraulic boundary scenario " << buffer[*counter].get_hydraulic_boundary_scenario().get_name()<<"..."<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			prefix <<buffer[*counter].get_description_string_with_name()<<"> ";
			Sys_Common_Output::output_hyd->set_userprefix(&prefix);
			if(Hyd_Hydraulic_System::check_hyd_results_calculated(this->ptr_database, this->system_id, buffer[*counter].get_hydraulic_boundary_scenario().get_id(), buffer[*counter].get_break_scenario_string() )==false){

				
				this->threads[i].set_new_hyd_bound_sz_id(buffer[*counter].get_hydraulic_boundary_scenario());
				this->threads[i].set_string_break_sc(buffer[*counter].get_break_scenario_string());
				this->threads[i].set_folder_name(buffer[*counter].get_description_string_with_name(), true);
				this->threads[i].set_identifier_string(buffer[*counter].get_description_string_with_name());
				this->threads[i].set_system_number(buffer[*counter].get_hydraulic_boundary_scenario().get_id());
				Risk_System::check_stop_thread_flag();
				try{
					//set boundary scenario
					this->threads[i].set_new_boundary_condition();
					Risk_System::check_stop_thread_flag();
			
					//set the breaking sections
					int number_section=buffer[*counter].get_number_involved_sections();
					_fpl_break_info *sec_id=buffer[*counter].get_ptr_involved_sections();
					int number_coast=0;
					int number_river=0;
					Fpl_Section *sec_buff;
					for(int j=0; j< number_section; j++){
						sec_buff=this->ptr_scenarios->get_ptr_fpl_section(sec_id[j].id_fpl_sec);

						if(sec_buff->get_breach_start_parameters().id_rv_model>=0 && sec_buff->get_breach_start_parameters().id_related_station>=0){
							number_river++;	
						}
						//coast
						else if(sec_buff->get_breach_start_parameters().id_rv_model<0 && sec_buff->get_breach_start_parameters().id_related_station>=0){
							number_coast++;
						}
					}
					this->threads[i].coupling_managment.set_number_rv2fp_dikebreak_fpl_alloc(number_river);
					this->threads[i].coupling_managment.set_number_fp2co_dikebreak_fpl_alloc(number_coast);
					


					//set a list with availabe scenarios (catchment area approach)
					Risk_Catchment_Break_Scenario_List list_calc;
					Risk_Catchment_Break_Scenario_List list_info;
					int max_interval=2;
					if(this->risk_type==_risk_type::catchment_risk){
						list_calc.set_risk_break_list_hyd_bound(this->ptr_database, this->system_id, &buffer[*counter].get_hydraulic_boundary_scenario());  
						//list_calc.delete_calculated_in_list();
						list_calc.sort_list_after_number_involved_section(false);
						//set list with scenarios which contains informations
						string rest_id="CA";
						rest_id=list_calc.set_new_list_nearest_hyd_calc_scenario(&list_info, &buffer[*counter]);
						list_calc.find_break_scenario_unequal_interval(rest_id, &buffer[*counter], &list_info);
						//list_info.output_list();
						max_interval=this->ptr_risk_thread->get_ca_control_parameter().number_intervals-1;

					}


					//set the break height to each section
					for(int j=0; j< number_section; j++){
						sec_buff=this->ptr_scenarios->get_ptr_fpl_section(sec_id[j].id_fpl_sec);

						//set the coupling break height
						if(this->risk_type==_risk_type::catchment_risk){
							//evaluate the interval;
							int id_interval_int=Fpl_Frc_Curve::convert_interval_string2integer(sec_id[j].id_interval);
										
							//evaluate the best break height
							int point_id=-1;
							double max_h_buff=0.0;
							string break_sc_id="CA";

							break_sc_id=this->get_break_sc_id_for_break_height(sec_id[j].id_fpl_sec, &buffer[*counter], &list_info);

							//get th breakheight from database table
							Hyd_Coupling_RV2FP_Merged::set_hmax2break_in_max_h_table(this->ptr_database, this->system_id, buffer[*counter].get_hydraulic_boundary_scenario().get_id(), break_sc_id, sec_id[j].id_fpl_sec, &point_id, &max_h_buff);
							
							//calculate the break height for different intervals of the frc-curve
							max_h_buff=(double)(max_interval-id_interval_int-1)*max_h_buff/max_interval+2.0/3.0*max_h_buff/max_interval;
							
							//set the break height to the fpl-section
							sec_buff->set_starting_breach_height(max_h_buff);
							//river
							if(sec_buff->get_breach_start_parameters().id_rv_model>=0 && sec_buff->get_breach_start_parameters().id_related_station>=0){
								this->threads[i].coupling_managment.set_start_rv2fp_dikebreak_fpl(j, sec_buff->get_breach_start_parameters(), sec_buff->get_left_bank_flag()); 
								this->threads[i].coupling_managment.init_rv2fp_dikebreak_fpl(j, sec_buff->get_id_section(), point_id);
							}
							//coast
							else if(sec_buff->get_breach_start_parameters().id_rv_model<0 && sec_buff->get_breach_start_parameters().id_related_station>=0){
								this->threads[i].coupling_managment.set_start_fp2co_dikebreak_fpl(j, sec_buff->get_breach_start_parameters()); 
								this->threads[i].coupling_managment.init_fp2co_dikebreak_fpl(j, sec_buff->get_id_section(), point_id);

							}
							
						}
						else if(this->risk_type==_risk_type::scenario_risk){
							sec_buff->set_starting_breach_height(sec_id[j].break_height);
							//river
							if(sec_buff->get_breach_start_parameters().id_rv_model>=0 && sec_buff->get_breach_start_parameters().id_related_station>=0){
								this->threads[i].coupling_managment.set_start_rv2fp_dikebreak_fpl(j, sec_buff->get_breach_start_parameters(), sec_buff->get_left_bank_flag()); 
								this->threads[i].coupling_managment.init_rv2fp_dikebreak_fpl(j, sec_buff->get_number_involved_profiles(), sec_buff->get_list_involved_river_profiles(), sec_buff->get_id_section());
							}
							//coast
							else if(sec_buff->get_breach_start_parameters().id_rv_model<0 && sec_buff->get_breach_start_parameters().id_related_station>=0){
								this->threads[i].coupling_managment.set_start_fp2co_dikebreak_fpl(j, sec_buff->get_breach_start_parameters()); 
								this->threads[i].coupling_managment.init_fp2co_dikebreak_fpl(j, sec_buff->get_number_involved_points(), sec_buff->get_list_involved_coast_dikeline_points(), sec_buff->get_id_section());
							}
						}
					}					
					this->threads[i].set_preproc_flag(true);

				}
				catch(Error msg){
					emit send_hyd_thread_runs(false);
					if(msg.get_user_aborted_exception()==false){
						ostringstream info;
						info <<"Name risk break scenario       : "<< buffer[*counter].get_break_scenario_string()<< endl;
						info <<"Name hydraulic bundary scenario: "<< buffer[*counter].get_hydraulic_boundary_scenario().get_name()<< endl;
						msg.make_second_info(info.str());
						this->threads[i].increase_error_number();
						msg.output_msg(2);
						this->threads[i].output_final_model_statistics();
						this->threads[i].reset_system();
					}
					else{
						Sys_Common_Output::output_hyd->rewind_userprefix();
						throw msg;
					}
				}
			}
			else{
				cout <<"Hydraulic results are already calculated for risk break scenario " <<buffer[*counter].get_break_scenario_string();
				cout <<" combined with the hydraulic boundary scenario " << buffer[*counter].get_hydraulic_boundary_scenario().get_name() << " in thread no. " << i<<"..."<< endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			}
			Sys_Common_Output::output_hyd->rewind_userprefix();
			(*counter)++;
		}
		while((*counter)<this->ptr_scenarios->get_number_sc2calc() && this->threads[i].get_preproc_flag()==false);
	}
}
//Reset the hydraulic threads
void Risk_Hydraulic_Break_Calculation::reset_hyd_threads(void){
	//reset
	try{
		for(int i=0; i< this->required_threads; i++){
			if(this->threads[i].get_preproc_flag()==true){
				this->threads[i].reset_system();
				this->threads[i].clear_boundary_conditions();
			}
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Output the final statistic of loading the threads
void Risk_Hydraulic_Break_Calculation::output_final_statistic_loading(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "Statistics of the loading of the hydraulic base systeme for the RISK break-calculation..." << endl ;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s) total       :" << W(3) << this->number_error.total_error <<endl;
	cout << " Warning(s) total     :" << W(3) << this->number_warning.total_warnings <<endl;
	if(this->number_error.total_error!=0 || this->number_warning.total_warnings!=0){
		cout << "DETAILED ERROR-/WARNING-NUMBER"<< endl;
	}
	if(this->number_error.risk_error>0){
		cout << " Error(s) RISK        :" << W(3) << this->number_error.risk_error <<endl;
	}
	if(this->number_warning.risk_warning>0){
		cout << " Warning(s) RISK      :" << W(3) << this->number_warning.risk_warning <<endl;
	}
	if(this->number_error.fpl_error>0){
		cout << " Error(s) FPL         :" << W(3) << this->number_error.fpl_error <<endl;
	}
	if(this->number_warning.fpl_warning>0){
		cout << " Warning(s) FPL       :" << W(3) << this->number_warning.fpl_warning <<endl;
	}
	if(this->number_error.hyd_error>0){
		cout << " Error(s) HYD         :" << W(3) << this->number_error.hyd_error <<endl;
	}
	if(this->number_warning.hyd_warning>0){
		cout << " Warning(s) HYD       :" << W(3) << this->number_warning.hyd_warning <<endl;
	}

	if(Risk_System::get_stop_thread_flag()==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the statistic of the hydraulic break-calculation
void Risk_Hydraulic_Break_Calculation::output_final_statistic_calculation(void){

	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "Statistics of the hydraulic RISK break-calculation..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Type of risk analysis:" << "  " << Risk_Break_Scenario::convert_risk_type2txt(this->risk_type) <<endl;
	cout << " Time                 :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << " Number of calculation:" << "  " << this->ptr_scenarios->get_number_sc2calc()<<endl;
	cout << " Number of threads    :" << "  " << this->required_threads<<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s) total       :" << W(3) << this->number_error.total_error <<endl;
	cout << " Warning(s) total     :" << W(3) << this->number_warning.total_warnings <<endl;

	if(Risk_System::get_stop_thread_flag()==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
///Set exeptions at start of an action
void Risk_Hydraulic_Break_Calculation::set_exception_number_start(void){
	Error::get_number_errors_moduls(&this->number_error);
	Warning::get_number_warning_moduls(&this->number_warning);
}
///Set exception_number at the end of an action
void Risk_Hydraulic_Break_Calculation::set_exception_number_end(void){
	Error::get_diff_number_errors_moduls(&this->number_error);
	Warning::get_diff_number_warning_moduls(&this->number_warning);
}
//Check in case of a scenario based risk calculation if a break was launched in the hydraulic system
void Risk_Hydraulic_Break_Calculation::check_for_break_scenario_risk(void){
	if(this->risk_type==_risk_type::scenario_risk && this->threads!=NULL){
		if(this->threads[0].coupling_managment.get_fpl_breach_has_started(this->ptr_scenarios[0].get_ptr_sc2calc(0)->get_involved_section(0).id_fpl_sec)==false){
			Warning msg=this->set_warning(2);
			ostringstream info;
			info << "Maximum waterlevel observed "<< this->threads[0].coupling_managment.get_max_observed_waterlevel_breach(this->ptr_scenarios[0].get_ptr_sc2calc(0)->get_involved_section(0).id_fpl_sec)<<label::m<<endl;
			msg.make_second_info(info.str());
			msg.output_msg(5);
			this->breach_has_started=false;
		}
		else{
			this->breach_has_started=true;
		}
	}
	else if(this->risk_type==_risk_type::catchment_risk && this->threads!=NULL){
		string id_break_sc;
		Risk_Break_Scenario buff_sc_break;
		bool is_changed=false;
		bool new_is_calculated=false;
		string old_break_sc;
		//check each thread
		for(int i=0; i<this->required_threads; i++){
			Risk_System::check_stop_thread_flag();
			is_changed=false;
			new_is_calculated=false;
			if(threads[i].get_preproc_flag()==true){

				id_break_sc=this->threads[i].get_string_break_sc();
				old_break_sc=id_break_sc;
				buff_sc_break.set_break_scenario_str(*this->threads[i].get_ptr_hyd_bound_scenario(), id_break_sc);
				//check each section
				for(int j=0; j<buff_sc_break.get_number_involved_sections(); j++){
					if(this->threads[i].coupling_managment.get_fpl_breach_has_started(buff_sc_break.get_involved_section(j).id_fpl_sec)==false){
						//no break was detected
						is_changed=true;
						buff_sc_break.reduce_involved_section(buff_sc_break.get_involved_section(j).id_fpl_sec);	
					}
				}
				//check if the break scenario is already calculated
				/*QSqlQueryModel model;*/
				if(Hyd_Hydraulic_System::check_hyd_results_calculated( this->ptr_database, this->system_id, buff_sc_break.get_hydraulic_boundary_scenario().get_id(), buff_sc_break.get_break_scenario_string())==true){
					new_is_calculated=true;
				}

				//warnings
				if(is_changed==true && new_is_calculated==false){
					Warning msg=this->set_warning(3);
					ostringstream info;
					info << "Desired break scenario : "<< old_break_sc <<endl;
					info << "New break scenario     : "<< buff_sc_break.get_break_scenario_string()<<endl;
					msg.make_second_info(info.str());
					msg.output_msg(5);
					//reaction
					//switch the break scenario string
					this->threads[i].set_string_break_sc(buff_sc_break.get_break_scenario_string());
					this->threads[i].set_identifier_string(buff_sc_break.get_description_string_with_name());

				}
				else if(is_changed==true && new_is_calculated==true){
					Warning msg=this->set_warning(4);
					ostringstream info;
					info << "Desired break scenario : "<< old_break_sc <<endl;
					info << "New break scenario     : "<< buff_sc_break.get_break_scenario_string()<<endl;
					msg.make_second_info(info.str());
					msg.output_msg(5);
					//reaction
					//switch the break scenario string
					this->threads[i].set_string_break_sc(buff_sc_break.get_break_scenario_string());
					this->threads[i].set_identifier_string(buff_sc_break.get_description_string_with_name());
					this->threads[i].set_preproc_flag(false);
				}
			}
		}
	}
}
//Get the identifer of a break scenario with the best possible break height for an section out of a list with possible calculated break scenarios
string Risk_Hydraulic_Break_Calculation::get_break_sc_id_for_break_height(const int sec_id, Risk_Break_Scenario *sc2calc, Risk_Catchment_Break_Scenario_List *sc_info){
	string buffer;
	buffer="CA";

	if(sc2calc->get_number_involved_sections()==1){
		return buffer;
	}

	sc_info->sort_list_after_number_involved_section(false);

	//take the nearest break scenario, where the section is not involved
	for(int i=0; i< sc_info->get_number_in_list(); i++){
		if(sc_info->get_ptr_scenario(i)->decide_fpl_section_involved(sec_id)==false){
			buffer=sc_info->get_ptr_scenario(i)->get_break_scenario_string();
			break;
		}
	}

	return buffer;
}
//Set warning(s)
Warning Risk_Hydraulic_Break_Calculation::set_warning(const int warn_type){
	string place="Risk_Hydraulic_Break_Calculation::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://a multithreading problem 
			place.append("set_risk_type(const _risk_type type)") ;
			reason="The required number of threads exceeds the iideal number of threads (=number of cores-1, if number of cores>1) ";
			reaction="the ideal thread number is applied";
			help= "Check the existing cores of your workstation";
			type=14;	
			break;
		case 1://a multithreading problem
			place.append("set_risk_type(const _risk_type type)") ;
			reason="The required number of threads exceeds the maximum number of threads";
			reaction="Required number of threads is set to the maximum number of threads";
			help= "Check the required number of threads";
			info << "Maximum number of threads : " <<constant::max_threads <<endl;
			type=14;	
			break;
		case 2://no break development
			place.append("check_for_break_scenario_risk(void)") ;
			reason="There was no breach development obeserved for this hydraulic risk calculation (scenario based risk)";
			reaction="No risk calculation is possible";
			help= "Decrease the breach starting waterlevel for this FPL-section and this HYD-boundary scenario";
			type=21;	
			break;
		case 3://no break development like considered, but information are used
			place.append("check_for_break_scenario_risk(void)") ;
			reason="The development of the breaches was not as considered for the break scenario (catchment area risk).";
			reaction="The information of the new break scenario are still used!";
			help= "Try to calculated the desired break scenario again";
			type=21;	
			break;
		case 4://no break development like considered; the information are already availabe
			place.append("check_for_break_scenario_risk(void)") ;
			reason="The development of the breaches was not as considered for the break scenario (catchment area risk).";
			reaction="The information of the new break scenario are not used, because they are available. No postprocessing is performed.";
			help= "Try to calculated the desired break scenario again";
			type=21;	
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
//Set error(s)
Error Risk_Hydraulic_Break_Calculation::set_error(const int err_type){
	string place="Risk_Hydraulic_Break_Calculation::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_hyd_threads(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://no base scenarios is set
			place.append("init_load_hyd_threads(void)");
			reason="No hydraulic base scenario is set; Can not initialise the risk calculation";
			help="Check the hydraulic boundary scenarios id database";
			type=29;
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
