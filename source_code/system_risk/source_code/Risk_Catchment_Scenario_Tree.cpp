#include "Risk_Headers_Precompiled.h"
//#include "Risk_Catchment_Scenario_Tree.h"


//Default constructor
Risk_Catchment_Scenario_Tree::Risk_Catchment_Scenario_Tree(void){

	this->control_params.number_mc=100000;
	this->control_params.number_intervals=2;
	this->random_generator=NULL;
	this->random_generator_max_time=NULL;
	this->control_params.gen_type=_fpl_random_generator_type::gen_type_ranmar;
	this->control_params.prob_bound=0.9;
	this->control_params.use_predict_values=true;

	this->number_sc2bound=0;

	this->number_mc_sc=0;
	this->mc_sc_list=NULL;

	this->number_break_sc=0;
	this->break_sc_list=NULL;

	this->hyd_bound_sc=NULL;

	this->number_blocks=0;
	this->sc_blocks=NULL;

	this->additional_hyd_stress=NULL;
	this->number_additional_hyd_result=0;

	Sys_Memory_Count::self()->add_mem(sizeof(Risk_Catchment_Scenario_Tree),_sys_system_modules::RISK_SYS);//count the memory

}
//Default destructor
Risk_Catchment_Scenario_Tree::~Risk_Catchment_Scenario_Tree(void){

	this->delete_random_generator();
	this->delete_random_generator_max_time();
	this->delete_list_mc_sc();
	this->delete_break_scenario_list();
	this->delete_blocks_mc_sc();
	this->delete_additional_hyd_info();
	Sys_Memory_Count::self()->minus_mem(sizeof(Risk_Catchment_Scenario_Tree),_sys_system_modules::RISK_SYS);//count the memory

}
//_________
//public
//Set the configuartion parameters for the break scenario generation
void Risk_Catchment_Scenario_Tree::set_break_scenario_generation_param(const _risk_ca_scenario_control params){
	this->control_params=params;
	Fpl_Frc_Curve::set_number_intervals(this->control_params.number_intervals);
}
//Generate list of the break scenarios for the catchment area risk approach via the monte carlo analysis
void Risk_Catchment_Scenario_Tree::generate_break_scenario_list(QSqlDatabase *ptr_database, const int number_fpl_section, Fpl_Section *fpl_section, Hyd_Boundary_Szenario *hyd_bound_sc){
		

	this->hyd_bound_sc=hyd_bound_sc;
	try{
		this->allocate_random_generator();
	}
	catch(Error msg){
		throw msg;
	}

	//set prefix
	ostringstream prefix;
	prefix << "LIST> ";
	Sys_Common_Output::output_risk->set_userprefix(&prefix);
	ostringstream cout;
	cout << "Generate monte-carlo break scenarios for hydraulic boundary scenario "<< this->hyd_bound_sc->get_name()<<"..."<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);

	try{
		//init frc-curves
		cout << "Load FRC-curve data of "<<number_fpl_section<<" FPL-section(s)..."<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		double max_h=0.0;
		for(int i=0; i<number_fpl_section; i++){
			max_h=fpl_section[i].evaluate_max_waterlevel_involved_section(ptr_database, this->hyd_bound_sc->get_id(), "CA");
			fpl_section[i].init_intervals_frc_curve(ptr_database, max_h);
		}
		//make mc
		ostringstream id_stream;
		int counter_out=1;
		cout << "Start analysing "<<number_fpl_section <<" fpl-section(s) via the monte-carlo analysis..."<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout << " "<< 0<<label::percentage << " ("<< 0<<")"<<" is done"<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		string id_str;
		for(int j=0; j<this->control_params.number_mc; j++){
			Risk_System::check_stop_thread_flag();
			if(j==(int)(this->control_params.number_mc*0.25*counter_out)){
				cout << " "<< (int)(0.25*counter_out*100) << label::percentage << " ("<< j<<")"<<" is done"<<endl;
				Sys_Common_Output::output_risk->output_txt(&cout);
				counter_out++;
			}
			id_stream.str("");
			id_stream<<"CA_";

			for(int i=0; i<number_fpl_section; i++){
				id_str=fpl_section[i].get_relevant_interval_frc_curve(this->random_generator);
				if(fpl_section[i].get_ptr_frc_data()->get_in_nobreak_interval()==false){
					id_stream<<fpl_section[i].get_id_section()<<"-";
					id_stream<<id_str;
					id_stream<<"_";
				}

			}
			id_str=id_stream.str();
			id_str=id_str.substr(0,id_str.length()-1);


			//this->update_list_mc_sc(id_str);
			this->update_blocks(id_str);
		}
		cout << " "<< 100<<label::percentage << " ("<< this->control_params.number_mc<<")"<<" is done"<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);

		this->transfer_blocks2list();
		this->delete_blocks_mc_sc();
		this->number_blocks=0;

		//calc occurence probability
		for(int i=0; i< this->number_mc_sc; i++){
			this->mc_sc_list[i].generated_flag=true;
			this->mc_sc_list[i].occ_prob=(double)this->mc_sc_list[i].number/(double)this->control_params.number_mc;
		}

		
		
		int index_mc_sc=-1;
		double prob_nobreak=1.0;
		//add nobreak scenarios
		//build the identifier
		id_stream.str("");
		id_stream<<"CA";
		id_str=id_stream.str();
		for(int j=0; j<number_fpl_section;j++){

			prob_nobreak=prob_nobreak*(1.0-fpl_section[j].get_ptr_frc_data()->get_prob_nobreak_interval());
		}
		index_mc_sc=this->update_list_mc_sc(id_str);
		if(this->mc_sc_list[index_mc_sc].occ_prob==0.0){
			this->mc_sc_list[index_mc_sc].occ_prob=prob_nobreak;
		}
		this->mc_sc_list[index_mc_sc].generated_flag=false;

		//add all one break scenarios
		double prob_one_break=1.0;
		for(int i=0; i<number_fpl_section; i++){
			prob_one_break=1.0;
			index_mc_sc=-1;
			//build the identifier
			id_stream.str("");
			id_str=fpl_section[i].get_ptr_frc_data()->get_most_probable_nobreak_identifier(&prob_one_break);
			id_stream<<"CA_"<<fpl_section[i].get_id_section()<<"-";
			id_stream<<id_str;
			id_str=id_stream.str();
			
			for(int j=0; j<number_fpl_section;j++){
				if(i!=j){
					prob_one_break=prob_one_break*(1.0-fpl_section[j].get_ptr_frc_data()->get_prob_nobreak_interval());
				}
			}
			if(prob_one_break==0.0 && this->check_one_break_sc_in_list(id_str)==false){
				continue;
			}

			index_mc_sc=this->update_list_mc_sc(id_str);
			
			if(this->mc_sc_list[index_mc_sc].occ_prob==0.0){
				this->mc_sc_list[index_mc_sc].occ_prob=prob_one_break;
			}
			this->mc_sc_list[index_mc_sc].generated_flag=false;
		}

		this->sort_list_mc_sc();
		cout << "Generation of monte-carlo break scenarios for hydraulic boundary scenario "<< hyd_bound_sc->get_name()<<" is finished"<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);

		//count number until boundary is reached
		this->number_sc2bound=this->get_index_sum_prob_reach();

		//transfer it to the break scenario list
		this->transfer_mc_sc2break_sc_list();
		
	}
	catch(Error msg){
		Sys_Common_Output::output_risk->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_risk->rewind_userprefix();
}
//Generate a new list for updating the probabilities, including new hydraulic information
void Risk_Catchment_Scenario_Tree::generate_new_break_scenario_list_update(QSqlDatabase *ptr_database, const int number_fpl_section, Fpl_Section *fpl_section, Hyd_Boundary_Szenario *hyd_bound_sc, Risk_Catchment_Break_Scenario_List *existing_sc, const bool predict_prob){
	this->hyd_bound_sc=hyd_bound_sc;
	try{
		this->allocate_random_generator();
		this->allocate_random_generator_max_time();
	}
	catch(Error msg){
		throw msg;
	}

	//for ordering the fpl-section pointer
	Fpl_Section **ordered_fpl_section=NULL;
	ordered_fpl_section=new Fpl_Section*[number_fpl_section];

	//structure for predicting the waterlevel reduction in case of not calculated scenarios
	Risk_Fpl_Stress generated_stress;
	generated_stress.set_number_fpl_section(number_fpl_section);
	Risk_Fpl_Stress base_stress;
	generated_stress.set_number_fpl_section(number_fpl_section);
	Risk_Fpl_Stress part_stress;
	part_stress.set_number_fpl_section(number_fpl_section);



	//set prefix
	ostringstream prefix;
	prefix << "UPDATE> ";
	Sys_Common_Output::output_risk->set_userprefix(&prefix);
	if(predict_prob==true){
		prefix << "PRE> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);
	}
	else{
		prefix << "NO_PRE> ";
		Sys_Common_Output::output_risk->set_userprefix(&prefix);
	}
	ostringstream cout;
	cout << "Update break scenario list via Monte-Carlo-analysis..."<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);


	try{
		//init frc-curves
		cout << "Load FRC-curve data of "<<number_fpl_section<<" FPL-section(s)..."<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		double max_h=0.0;
		//set the frc-data of the fpl-section
		for(int i=0; i<number_fpl_section; i++){
			max_h=fpl_section[i].evaluate_max_waterlevel_involved_section(ptr_database, this->hyd_bound_sc->get_id(), "CA");
			fpl_section[i].init_intervals_frc_curve(ptr_database, max_h);
		}
		//allocate and set the additional hydraulic information, which are available
		try{
			this->allocate_additional_hyd_info_database(ptr_database, number_fpl_section, fpl_section, hyd_bound_sc, existing_sc);
		}
		catch(Error msg){
			Sys_Common_Output::output_risk->rewind_userprefix();
			Sys_Common_Output::output_risk->rewind_userprefix();
			throw msg;
		}

		

		//make mc output
		ostringstream id_stream;
		int counter_out=1;
		cout << "Start analysing "<<number_fpl_section <<" fpl-section(s) via the monte-carlo analysis..."<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout << " "<< 0<<label::percentage << " ("<< 0<<")"<<" is done"<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		string id_str;
		int counter_break=0;

		bool new_found=false;


		//set the base stress
		base_stress.copy_infos(this->find_relevant_break_scenario_hyd_info("CA", NULL, &new_found));
		if(new_found!=true){
			Error msg=this->set_error(6);
			ostringstream info;
			info<<"Hyd-Boundary scenario name: " << hyd_bound_sc->get_name()<<endl;
			info<<"Hyd-Boundary scenario id  : " << hyd_bound_sc->get_id()<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}


		//start mc-simulation
		for(int j=0; j<this->control_params.number_mc; j++){
			Risk_System::check_stop_thread_flag();
			if(j==(int)(this->control_params.number_mc*0.25*counter_out)){
				cout << " "<< (int)(0.25*counter_out*100) << label::percentage << " ("<< j<<")"<<" is done"<<endl;
				Sys_Common_Output::output_risk->output_txt(&cout);
				counter_out++;
			}
			id_stream.str("");
			id_stream<<"CA_";
	
			Risk_System::check_stop_thread_flag();
			if(j==(int)(this->control_params.number_mc*0.25*counter_out)){
				cout << " "<< (int)(0.25*counter_out*100) << label::percentage << " ("<< j<<")"<<" is done"<<endl;
				Sys_Common_Output::output_risk->output_txt(&cout);
				counter_out++;
			}
			id_stream.str("");
			counter_break=0;
			id_stream<<"CA_";

			//generate random number/waterlevel for each section
			for(int i=0; i<number_fpl_section; i++){
				fpl_section[i].get_ptr_frc_data()->set_is_checked(false);
				id_str=fpl_section[i].get_relevant_interval_frc_curve(this->random_generator);
				if(fpl_section[i].get_ptr_frc_data()->get_in_nobreak_interval()==false){
					id_stream<<fpl_section[i].get_id_section()<<"-";
					id_stream<<id_str;
					id_stream<<"_";
					counter_break++;
				}
			}
			id_str=id_stream.str();
			id_str=id_str.substr(0,id_str.length()-1);

			//no predicted probability
			if(counter_break>=2 && predict_prob==false){
				Risk_Fpl_Stress *new_info=NULL;
				double max_waterlevel=0.0;
				ostringstream second_id;
				second_id<<"CA";
				new_info=this->find_relevant_break_scenario_hyd_info(second_id.str(), new_info, &new_found);
				this->sort_result_fpl_section_time(new_info, fpl_section);
				
				for(int i=0; i<number_fpl_section; i++){
					ordered_fpl_section[i]=&fpl_section[new_info->get_section_stress(i)->used_index];
				}


				//further check with new hydraulic results
				for(int i=0; i<number_fpl_section; i++){
					ordered_fpl_section[i]->get_ptr_frc_data()->set_is_checked(true);
					if(ordered_fpl_section[i]->get_ptr_frc_data()->get_in_nobreak_interval()==false){
						new_info=this->find_relevant_break_scenario_hyd_info(second_id.str(), new_info, &new_found);
						if(new_info!=NULL){
							this->sort_result_fpl_section_time(new_info, fpl_section);
							max_waterlevel=Risk_Fpl_Stress::get_maximum_waterlevel(new_info, ordered_fpl_section[i]->get_id_section()); 
							if(max_waterlevel<=ordered_fpl_section[i]->get_ptr_frc_data()->get_current_waterlevel()){
								ordered_fpl_section[i]->get_ptr_frc_data()->set_in_nobreak_interval(true);
							}
						}

												
						//make ordered string after section_ids 
						second_id.str("");
						second_id<<"CA";
						for(int n=0; n<number_fpl_section; n++){
							if(fpl_section[n].get_ptr_frc_data()->get_in_nobreak_interval()==false && fpl_section[n].get_ptr_frc_data()->get_is_checked()==true){
								second_id<<"_";
								second_id <<fpl_section[n].get_id_section()<<"-";
								second_id<< fpl_section[n].get_ptr_frc_data()->get_current_intervall_string();
	
			
							}
						}
					}
				}
				this->update_blocks(second_id.str());
			}
			//predicted probability
			else if(counter_break>=2 && predict_prob==true){
				Risk_Fpl_Stress *new_info=NULL;
				double max_waterlevel=0.0;
				ostringstream second_id;
				second_id<<"CA";
				new_info=this->find_relevant_break_scenario_hyd_info(second_id.str(), new_info, &new_found);
				this->sort_result_fpl_section_time(new_info, fpl_section);
			
				for(int i=0; i<number_fpl_section; i++){
					ordered_fpl_section[i]=&fpl_section[new_info->get_section_stress(i)->used_index];
				}


				//further check with new hydraulic results
				ostringstream part_info_id;
				int part_info_sec_break_id=-1;
				for(int i=0; i<number_fpl_section; i++){
					
					ordered_fpl_section[i]->get_ptr_frc_data()->set_is_checked(true);
					if(ordered_fpl_section[i]->get_ptr_frc_data()->get_in_nobreak_interval()==false){
						new_info=this->find_relevant_break_scenario_hyd_info(second_id.str(), new_info, &new_found);
						
					
						if(new_info!=NULL && new_found==true){
							this->sort_result_fpl_section_time(new_info, fpl_section);
							max_waterlevel=Risk_Fpl_Stress::get_maximum_waterlevel(new_info, ordered_fpl_section[i]->get_id_section()); 
							if(max_waterlevel<=ordered_fpl_section[i]->get_ptr_frc_data()->get_current_waterlevel()){
								ordered_fpl_section[i]->get_ptr_frc_data()->set_in_nobreak_interval(true);
							}
							generated_stress.copy_infos(new_info);
						}
						else{
							bool new_found_part=false;
							part_stress.copy_infos(this->find_relevant_break_scenario_hyd_info(part_info_id.str(), NULL,&new_found_part));
							//predict new hydraulic information
							if(new_found_part==true){
								//predict 
								generated_stress.append2id_string_break_sc(part_info_id.str());
								//update the stress with the new information; the before predicted values are then the base
								generated_stress.predict_values(&base_stress, part_info_sec_break_id, &part_stress);
								//use it
								this->sort_result_fpl_section_time(&generated_stress, fpl_section);
								max_waterlevel=Risk_Fpl_Stress::get_maximum_waterlevel(&generated_stress, ordered_fpl_section[i]->get_id_section()); 
								if(max_waterlevel<=ordered_fpl_section[i]->get_ptr_frc_data()->get_current_waterlevel()){
									ordered_fpl_section[i]->get_ptr_frc_data()->set_in_nobreak_interval(true);
								}


							}
							else{
								//use the old hydraulic information, because no part-information are found
								this->sort_result_fpl_section_time(new_info, fpl_section);
								max_waterlevel=Risk_Fpl_Stress::get_maximum_waterlevel(new_info, ordered_fpl_section[i]->get_id_section()); 
								if(max_waterlevel<=ordered_fpl_section[i]->get_ptr_frc_data()->get_current_waterlevel()){
									ordered_fpl_section[i]->get_ptr_frc_data()->set_in_nobreak_interval(true);
								}

							}

							
						}
						//set string of part infos; maybe it is needed the next step; use just one break scenario for the prediction
						part_info_id.str("");
						part_info_id<<"CA_"<<ordered_fpl_section[i]->get_id_section()<<"-";
						part_info_id<<ordered_fpl_section[i]->get_ptr_frc_data()->get_current_intervall_string();
						part_info_sec_break_id=ordered_fpl_section[i]->get_id_section();
						
						//make ordered string after section_ids 
						second_id.str("");
						second_id<<"CA";
						for(int n=0; n<number_fpl_section; n++){
							if(fpl_section[n].get_ptr_frc_data()->get_in_nobreak_interval()==false && fpl_section[n].get_ptr_frc_data()->get_is_checked()==true){
								second_id<<"_";
								second_id <<fpl_section[n].get_id_section()<<"-";
								second_id<< fpl_section[n].get_ptr_frc_data()->get_current_intervall_string();
							}
						}
					}
				}
				this->update_blocks(second_id.str());

			}
			else{
				this->update_blocks(id_str);
			}
		}
		cout << " "<< 100<<label::percentage << " ("<< this->control_params.number_mc<<")"<<" is done"<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);

		this->transfer_blocks2list();
		this->delete_blocks_mc_sc();
		this->number_blocks=0;

		//calc occurence probability
		double sum=0.0;
		for(int i=0; i< this->number_mc_sc; i++){
			this->mc_sc_list[i].generated_flag=true;
			this->mc_sc_list[i].occ_prob=(double)this->mc_sc_list[i].number/(double)this->control_params.number_mc;
			sum=sum+this->mc_sc_list[i].occ_prob;
		}


		this->sort_list_mc_sc();
		cout << "Updating the break scenario list via Monte-Carlo-analysis is finished"<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);


		//count number until boundary is reached
		this->number_sc2bound=this->get_index_sum_prob_reach();

		//transfer it to the break scenario list
		this->transfer_mc_sc2break_sc_list();



	}
	catch(Error msg){
		Sys_Common_Output::output_risk->rewind_userprefix();
		Sys_Common_Output::output_risk->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_risk->rewind_userprefix();
	Sys_Common_Output::output_risk->rewind_userprefix();

	if(ordered_fpl_section!=NULL){
		delete []ordered_fpl_section;
	}
}
//Outpur the generated list of scenarios to display/console
void Risk_Catchment_Scenario_Tree::output_scenario_list(void){
	//output
	ostringstream cout;
	cout << "LIST OF MC-SC"<<endl;
	cout << " Name HYD-boundary scenario : " << this->hyd_bound_sc->get_name()<<endl;
	cout << " Id HYD-boundary scenario   : " << this->hyd_bound_sc->get_id()<<endl;
	cout << " Number intervals           : " << Fpl_Frc_Curve::get_number_intervals()<<endl;
	cout << " Number total               : " << this->number_mc_sc<<endl;
	cout << " Number with boundary       : " << this->number_sc2bound<< " ("<<this->control_params.prob_bound<<")"<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	if(this->number_mc_sc>0){
		cout << " i " << W(9)<< " prob " <<label::no_unit<< W(11) << " generated "<< W(17) << " name "<<endl;
		for(int i=0; i< this->number_mc_sc; i++){
			cout << " " << i << W(5);
			cout <<FORMAT_FIXED_REAL << P(5) << " " <<this->mc_sc_list[i].occ_prob;
			cout << W(10)<<functions::convert_boolean2string(this->mc_sc_list[i].generated_flag);
			cout <<"    "<< W(17)<< this->mc_sc_list[i].id_str<< endl;
			if(i<this->number_sc2bound){
				Sys_Common_Output::output_risk->output_txt(&cout);
			}
			else{
				Sys_Common_Output::output_risk->output_txt(&cout,true);
			}
		}
	}
}
//Reset the scenario tree
void Risk_Catchment_Scenario_Tree::reset_scenario_tree(void){
	this->delete_random_generator();
	this->delete_list_mc_sc();
	this->delete_break_scenario_list();
	this->number_sc2bound=0;
	this->number_mc_sc=0;
	this->number_break_sc=0;
	this->hyd_bound_sc=NULL;
}
//Get the number of break scenarios
int Risk_Catchment_Scenario_Tree::get_number_break_sc(void){
	return this->number_break_sc;
}
//Get a pointer to the break scenario list
Risk_Break_Scenario* Risk_Catchment_Scenario_Tree::get_ptr_break_sc_list(void){
	return this->break_sc_list;
}
//Get a pointer to the control parameter
_risk_ca_scenario_control * Risk_Catchment_Scenario_Tree::get_ptr_control_params(void){
	return &this->control_params;
}
//___________
//private
//Allocate the random generator
void Risk_Catchment_Scenario_Tree::allocate_random_generator(void){
	this->delete_random_generator();
	try{
		
		if(this->control_params.gen_type==_fpl_random_generator_type::gen_type_zikurat){
			this->random_generator=new Fpl_Random_Generator_Zikurat;
			Fpl_Random_Generator_Zikurat::reinit_random();
		}
		else if(this->control_params.gen_type==_fpl_random_generator_type::gen_type_MofAll){
			this->random_generator=new Fpl_Random_Generator_MofAll;
			Fpl_Random_Generator_MofAll::reinit_random();
		}
		else if(this->control_params.gen_type==_fpl_random_generator_type::gen_type_sfmt){
			this->random_generator=new Fpl_Random_Generator_Sfmt;
			Fpl_Random_Generator_Sfmt::reinit_generator();
		}
		else{//ranmar is used
			this->random_generator=new Fpl_Random_Generator_Ranmar;
			Fpl_Random_Generator_Ranmar::reinit_random();
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the random generator
void Risk_Catchment_Scenario_Tree::delete_random_generator(void){
	if(this->random_generator!=NULL){
		delete this->random_generator;
		this->random_generator=NULL;
	}
}
//Allocate the random generator for same time of maximum waterlevel occurence
void Risk_Catchment_Scenario_Tree::allocate_random_generator_max_time(void){
	this->delete_random_generator_max_time();
	try{
		//use always another one than the random generator; there are static values in the random generator classe
		if(this->control_params.gen_type==_fpl_random_generator_type::gen_type_zikurat){
			this->random_generator_max_time=new Fpl_Random_Generator_Ranmar;
			Fpl_Random_Generator_Zikurat::reinit_random();
		}
		else if(this->control_params.gen_type==_fpl_random_generator_type::gen_type_MofAll){
			this->random_generator_max_time=new Fpl_Random_Generator_Sfmt;
			Fpl_Random_Generator_MofAll::reinit_random();
		}
		else if(this->control_params.gen_type==_fpl_random_generator_type::gen_type_sfmt){
			this->random_generator_max_time=new Fpl_Random_Generator_Ranmar;
			Fpl_Random_Generator_Sfmt::reinit_generator();
		}
		else{//ranmar is used
			this->random_generator_max_time=new Fpl_Random_Generator_MofAll;
			Fpl_Random_Generator_Ranmar::reinit_random();
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(7);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the random generator for same time of maximum waterlevel occurence
void Risk_Catchment_Scenario_Tree::delete_random_generator_max_time(void){
	if(this->random_generator_max_time!=NULL){
		delete this->random_generator_max_time;
		this->random_generator_max_time=NULL;
	}
}
//Allocate the break scenario list
void Risk_Catchment_Scenario_Tree::allocate_break_scenario_list(void){
	this->delete_break_scenario_list();
	try{
		this->break_sc_list=new Risk_Break_Scenario[this->number_break_sc];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	for(int i=0; i< this->number_break_sc; i++){
		this->break_sc_list[i].set_risk_type(_risk_type::catchment_risk);
		this->break_sc_list[i].set_systemid(this->system_id);
	}
}
//Delete the break scenario list
void Risk_Catchment_Scenario_Tree::delete_break_scenario_list(void){
	if(this->break_sc_list!=NULL){
		delete []this->break_sc_list;
		this->break_sc_list=NULL;
	}
}
//Update the list of the monte-carlo scenarios
int Risk_Catchment_Scenario_Tree::update_list_mc_sc(const string sc_id_str){

	bool exists=false;
	//search if existing
	for(int i=0; i< this->number_mc_sc; i++){
		if(sc_id_str==this->mc_sc_list[i].id_str){
			this->mc_sc_list[i].number++;
			exists=true;
			return i;
		}
	}
	if(exists==false){
		//alloc buffer
		_risk_mc_scenario *buffer=NULL;
		buffer=new _risk_mc_scenario[this->number_mc_sc];
		//copy to buffer
		for(int i=0; i< this->number_mc_sc; i++){
			buffer[i]=this->mc_sc_list[i];	
		}

		//delete
		this->delete_list_mc_sc();
		this->number_mc_sc++;
		this->allocate_list_mc_sc();
		//recopy
		for(int i=0; i< this->number_mc_sc-1; i++){
			this->mc_sc_list[i]=buffer[i];	
		}
		this->mc_sc_list[this->number_mc_sc-1].id_str=sc_id_str;
		this->mc_sc_list[this->number_mc_sc-1].number++;

		if(buffer!=NULL){
			delete []buffer;
			buffer=NULL;
		}

	}
	return this->number_mc_sc-1;
}
//Allocate the list of the monte-carlo scenarios
void Risk_Catchment_Scenario_Tree::allocate_list_mc_sc(void){
	try{
		this->mc_sc_list=new _risk_mc_scenario[this->number_mc_sc];
		Sys_Memory_Count::self()->add_mem(sizeof(_risk_mc_scenario)*this->number_mc_sc,_sys_system_modules::RISK_SYS);//count the memory

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	for(int i=0; i<this->number_mc_sc; i++){

		this->mc_sc_list[i].number=0;
		this->mc_sc_list[i].occ_prob=0.0;
		this->mc_sc_list[i].generated_flag=true;
	}
}
//Delete the list of the monte-carlo scenarios
void Risk_Catchment_Scenario_Tree::delete_list_mc_sc(void){
	if(this->mc_sc_list!=NULL){
		delete []this->mc_sc_list;
		this->mc_sc_list=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_risk_mc_scenario)*this->number_mc_sc,_sys_system_modules::RISK_SYS);//count the memory

	}
}
//Sort the list of monte-carlo scenarios after their occurence
void Risk_Catchment_Scenario_Tree::sort_list_mc_sc(void){
/**
Bubble-sort algorithm for the sorting the detailed results array after the annuality.
*/
	_risk_mc_scenario temp;

	for(int i = this->number_mc_sc-1; i > 0; i--){
		for (int j = 1; j <= i; j++){
			if (this->mc_sc_list[j-1].occ_prob < this->mc_sc_list[j].occ_prob){
				temp = this->mc_sc_list[j-1];
				this->mc_sc_list[j-1] = this->mc_sc_list[j];
				this->mc_sc_list[j] = temp;
			}
		}
	}
}
//Check if one break scenario is in list
bool Risk_Catchment_Scenario_Tree::check_one_break_sc_in_list(const string sc_id_str){

	
	//CA is cutted
	string just_id=sc_id_str.substr(2);
	for(int i=0; i< this->number_mc_sc; i++){
		if(this->mc_sc_list[i].id_str.find(just_id)!=-1){
			return true;
		}
	}

	return false;
}
//Get the index when a given boundary of sum of occurence probability is reached
int Risk_Catchment_Scenario_Tree::get_index_sum_prob_reach(void){
	double sum=0.0;
	for(int i=0; i< this->number_mc_sc; i++){
		sum=sum+this->mc_sc_list[i].occ_prob;
		if(sum>this->control_params.prob_bound){
			return i+1;
		}
	}
	return this->number_mc_sc;
}
//Allocate the blocks of the monte-carlo scenarios
void Risk_Catchment_Scenario_Tree::allocate_blocks_mc_sc(void){
	try{
		this->sc_blocks=new _risk_mc_scenario_block[this->number_blocks];
		Sys_Memory_Count::self()->add_mem(sizeof(_risk_mc_scenario_block)*this->number_blocks,_sys_system_modules::RISK_SYS);//count the memory

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	for(int i=0; i<this->number_blocks; i++){
		this->sc_blocks[i].full=false;
		this->sc_blocks[i].number_set=0;
		for(int j=0; j<constant::risk_sc_block_number; j++){
			this->sc_blocks[i].scenarios[j].generated_flag=true;
			this->sc_blocks[i].scenarios[j].number=0;
			this->sc_blocks[i].scenarios[j].occ_prob=0.0;
		}
	}
}
//Delete the blocks of the monte-carlo scenarios
void Risk_Catchment_Scenario_Tree::delete_blocks_mc_sc(void){
	if(this->sc_blocks!=NULL){
		delete []this->sc_blocks;
		this->sc_blocks=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_risk_mc_scenario_block)*this->number_blocks,_sys_system_modules::RISK_SYS);//count the memory

	}
}
//Transfer the blocks to list
void Risk_Catchment_Scenario_Tree::transfer_blocks2list(void){
	int sum=0;
	for(int i=0; i<this->number_blocks; i++){
		sum=sum+this->sc_blocks[i].number_set;
	}

	this->delete_list_mc_sc();
	this->number_mc_sc=sum;
	this->allocate_list_mc_sc();
	int counter=0;
	for(int i=0; i<this->number_blocks; i++){
		for(int j=0; j<this->sc_blocks[i].number_set; j++){

			this->mc_sc_list[counter]=this->sc_blocks[i].scenarios[j];
			counter++;

		}
	}
}
//Copy the blocks 
void Risk_Catchment_Scenario_Tree::copy_blocks(_risk_mc_scenario_block *src, _risk_mc_scenario_block *dest){
	dest->full=src->full;
	dest->number_set=src->number_set;

	for(int i=0; i< constant::risk_sc_block_number; i++){
		dest->scenarios[i]=src->scenarios[i];
	}
}
//Update the blocks
void Risk_Catchment_Scenario_Tree::update_blocks(const string sc_id_str){
	bool exists=false;
	//search if existing
	for(int i=0; i< this->number_blocks; i++){
		for(int j=0; j<this->sc_blocks[i].number_set; j++){
			if(sc_id_str==this->sc_blocks[i].scenarios[j].id_str){
				this->sc_blocks[i].scenarios[j].number++;
				exists=true;
				return;
			}
		}
	}

	
	//set scenario into existing block
	for(int i=0; i< this->number_blocks; i++){
		if(this->sc_blocks[i].full==false){
			this->sc_blocks[i].scenarios[this->sc_blocks[i].number_set].id_str=sc_id_str;
			this->sc_blocks[i].scenarios[this->sc_blocks[i].number_set].number++;
			this->sc_blocks[i].number_set++;
			if(this->sc_blocks[i].number_set==constant::risk_sc_block_number){
				this->sc_blocks[i].full=true;
			}
			return;
		}
	}
	//set new block
	_risk_mc_scenario_block *buffer=NULL;
	buffer = new _risk_mc_scenario_block[this->number_blocks];

	//copy
	for(int i=0; i< this->number_blocks; i++){
		this->copy_blocks(&this->sc_blocks[i], &buffer[i]);
	}
	this->delete_blocks_mc_sc();
	this->number_blocks++;
	this->allocate_blocks_mc_sc();
	
	//recopy
	for(int i=0; i< this->number_blocks-1; i++){
		this->copy_blocks(&buffer[i], &this->sc_blocks[i]);
	}

	this->sc_blocks[this->number_blocks-1].scenarios[0].id_str=sc_id_str;
	this->sc_blocks[this->number_blocks-1].scenarios[0].number++;
	this->sc_blocks[this->number_blocks-1].number_set++;
	if(this->sc_blocks[this->number_blocks-1].number_set==constant::risk_sc_block_number){
		this->sc_blocks[this->number_blocks-1].full=true;
	}
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}

}
//Allocate and set from database the additional hydraulic result information
void Risk_Catchment_Scenario_Tree::allocate_additional_hyd_info_database(QSqlDatabase *ptr_database, const int number_fpl_section, Fpl_Section *fpl_section, Hyd_Boundary_Szenario *hyd_bound_sc, Risk_Catchment_Break_Scenario_List *existing_sc){
	this->delete_additional_hyd_info();
	this->number_additional_hyd_result=existing_sc->get_number_in_list();

	//allocate the infos for each scenario
	try{
		this->additional_hyd_stress = new Risk_Fpl_Stress[this->number_additional_hyd_result];

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(4);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	QSqlQueryModel query;

	for(int i=0; i < this->number_additional_hyd_result;i++){
		//set the infos
		this->additional_hyd_stress[i].set_number_fpl_section(number_fpl_section);
		this->additional_hyd_stress[i].set_id_string_break_sc(existing_sc->get_ptr_scenario(i)->get_break_scenario_string());
		
		//set the infos for each section
		int number_res=0;
		number_res=Hyd_Coupling_RV2FP_Merged::set_hmax2break_in_max_h_table(ptr_database, this->system_id,hyd_bound_sc->get_id() , existing_sc->get_ptr_scenario(i)->get_break_scenario_string(), &query);
		if(number_res!=this->additional_hyd_stress[i].get_number_fpl_section()){
			Error msg=this->set_error(5);
			ostringstream info;
			info << "Hydraulic boundary scenario : " << hyd_bound_sc->get_name()<<endl;
			info << "Break scenario name         : " << existing_sc->get_ptr_scenario(i)->get_break_scenario_string() <<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//set the infos for each section
		_risk_section_stress buffer;
		for(int j=0; j< this->additional_hyd_stress[i].get_number_fpl_section(); j++){
			buffer.id_section=query.record(j).value((Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(fpl_label::section_id)).c_str()).toInt();
			buffer.waterlevel=query.record(j).value((Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::maxh)).c_str()).toDouble();
			buffer.time=query.record(j).value((Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::time_maxh)).c_str()).toDouble();
			buffer.rand_time=query.record(j).value((Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::time_maxh)).c_str()).toDouble();
			//set the index of the fpl-section
			for(int n=0; n<number_fpl_section; n++){
				if(buffer.id_section==fpl_section[n].get_id_section()){
					buffer.used_index=n;
					break;
				}
			}
			this->additional_hyd_stress[i].set_section_stress(j, &buffer);

		}
	}

}
//Delete the additional hydraulic result information 
void Risk_Catchment_Scenario_Tree::delete_additional_hyd_info(void){
	if(this->additional_hyd_stress!=NULL){
		delete []this->additional_hyd_stress;
		this->additional_hyd_stress=NULL;
	}
}
//Find the relevant break scenario
Risk_Fpl_Stress * Risk_Catchment_Scenario_Tree::find_relevant_break_scenario_hyd_info(const string break_string, Risk_Fpl_Stress *old, bool *new_found){
	*new_found=false;
	for(int i=0; i< this->number_additional_hyd_result; i++){
		if(break_string==this->additional_hyd_stress[i].get_id_string_break_sc()){
			*new_found=true;
			return &this->additional_hyd_stress[i];

		}
	}

	return old;
}
//Sort information of the fpl-section after the time; if the time is equal sort it randomly
void Risk_Catchment_Scenario_Tree::sort_result_fpl_section_time(Risk_Fpl_Stress *tosort, Fpl_Section *fpl_section){

	_risk_section_stress temp;
	double rand=0.0;
	int interval=-1;
	for(int i=0; i<tosort->get_number_fpl_section(); i++){
		rand=this->random_generator_max_time->run_generator()/100.0;
		tosort->get_section_stress(i)->rand_time=tosort->get_section_stress(i)->time+rand;
		if(fpl_section[tosort->get_section_stress(i)->used_index].get_ptr_frc_data()->get_in_nobreak_interval()==false &&
			fpl_section[tosort->get_section_stress(i)->used_index].get_ptr_frc_data()->get_is_checked()==false){
				interval=fpl_section[tosort->get_section_stress(i)->used_index].get_ptr_frc_data()->get_current_index_intervall();
				if(interval>0){
				//scale the time
					tosort->get_section_stress(i)->rand_time=tosort->get_section_stress(i)->rand_time-interval*tosort->get_section_stress(i)->rand_time/(this->control_params.number_intervals-1);
				}
		}
	}
	for(int i = tosort->get_number_fpl_section()-1; i > 0; i--){
		for (int j = 1; j <= i; j++){
			if (tosort->get_section_stress(j-1)->rand_time > tosort->get_section_stress(j)->rand_time){
				temp = *tosort->get_section_stress(j-1);
				tosort->set_section_stress(j-1,tosort->get_section_stress(j));
				tosort->set_section_stress(j,&temp);
			}
		}
	}

	//cout << "_________"<<endl;
	//cout << tosort->break_sc<<endl;
	//for(int i=0; i<tosort->number; i++){
	//	cout<< " id "<<tosort->section_stress[i].id_section<<" "<< tosort->section_stress[i].rand_time<<endl;
	//
	//}

}
//Transfer monte-carlo scenario list to break scenario list
void Risk_Catchment_Scenario_Tree::transfer_mc_sc2break_sc_list(void){

	this->number_break_sc=this->number_mc_sc;
	this->allocate_break_scenario_list();

	for(int i=0; i<this->number_break_sc; i++){
		this->break_sc_list[i].set_break_scenario_str(*this->hyd_bound_sc, this->mc_sc_list[i].id_str);
		this->break_sc_list[i].set_probability_sc(this->mc_sc_list[i].occ_prob);
		if(this->mc_sc_list[i].generated_flag==true){
			this->break_sc_list[i].set_source_scenario(_risk_break_sc_source::sc_generated);
		}
		else{
			this->break_sc_list[i].set_source_scenario(_risk_break_sc_source::sc_fixed);
		}
	}
}
//Set error(s)
Error Risk_Catchment_Scenario_Tree::set_error(const int err_type){
	string place="Risk_Catchment_Scenario_Tree::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_random_generator(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("allocate_list_mc_sc(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://bad alloc
			place.append("allocate_break_scenario_list(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://bad alloc
			place.append("allocate_blocks_mc_sc(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 4://bad alloc
			place.append("allocate_additional_hyd_info_database(QSqlDatabase *ptr_database, const int number_fpl_section, Fpl_Section *fpl_section, Hyd_Boundary_Szenario *hyd_bound_sc, Risk_Catchment_Break_Scenario_List *existing_sc)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 5://not for all fpl-sections are results available
			place.append("allocate_additional_hyd_info_database(QSqlDatabase *ptr_database, const int number_fpl_section, Fpl_Section *fpl_section, Hyd_Boundary_Szenario *hyd_bound_sc, Risk_Catchment_Break_Scenario_List *existing_sc)");
			reason="The number of FPL-section(s) and available results in database for a break scenario are not equal";
			help="Check the database table and the number of FPL-section(s)";
			type=29;
			break;
		case 6://nobase scenario is found
			place.append("generate_new_break_scenario_list_update(QSqlDatabase *ptr_database, const int number_fpl_section, Fpl_Section *fpl_section, Hyd_Boundary_Szenario *hyd_bound_sc, Risk_Catchment_Break_Scenario_List *existing_sc, const bool max_prob)");
			reason="No hydraulic information to the base scenario CA are found";
			help="Check the database table and the data of the hydraulic calculation";
			type=29;
			break;
		case 7://bad alloc
			place.append("allocate_random_generator_max_time(void)");
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