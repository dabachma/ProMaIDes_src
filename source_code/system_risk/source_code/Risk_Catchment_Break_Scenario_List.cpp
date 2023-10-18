#include "Risk_Headers_Precompiled.h"
//#include "Risk_Catchment_Break_Scenario_List.h"


//Default constructor
Risk_Catchment_Break_Scenario_List::Risk_Catchment_Break_Scenario_List(void):block_elems(50){

	this->number=0;
	this->break_sc=NULL;
	this->total_is_set=false;
	

	Sys_Memory_Count::self()->add_mem(sizeof(Risk_Catchment_Break_Scenario_List),_sys_system_modules::RISK_SYS);//count the memory
}
///Default destructor
Risk_Catchment_Break_Scenario_List::~Risk_Catchment_Break_Scenario_List(void){
	this->delete_list();
	Sys_Memory_Count::self()->minus_mem(sizeof(Risk_Catchment_Break_Scenario_List),_sys_system_modules::RISK_SYS);//count the memory
}
//________
//public
//Add scenario to list
void Risk_Catchment_Break_Scenario_List::add_scenario2list(Risk_Break_Scenario *add){
	//check if it is already in list
	for(int i=0; i< this->number; i++){
		if(this->break_sc[i].get_break_scenario_string()==add->get_break_scenario_string() && 
			this->break_sc[i].get_sys_system_id().area_state==add->get_sys_system_id().area_state &&
			this->break_sc[i].get_sys_system_id().measure_nr==add->get_sys_system_id().measure_nr && 
			this->break_sc[i].get_hydraulic_boundary_scenario().get_id()==add->get_hydraulic_boundary_scenario().get_id()){
			return;
		}
	}

    if(this->number!=0 && this->number % this->block_elems!=0){

        this->break_sc[this->number]=*add;
        this->number++;

    }
    else{

        //allocate a buffer
        Risk_Break_Scenario *buffer=NULL;
        try{
            int full_blocks=(int)((this->number+1)/this->block_elems);
            if(((this->number+1)-full_blocks*this->block_elems)>0){
                full_blocks++;
            }
            buffer=new Risk_Break_Scenario[full_blocks*this->block_elems];

        }
        catch(bad_alloc &t){
            Error msg=this->set_error(0);
            ostringstream info;
            info<< "Info bad alloc: " << t.what() << endl;
            msg.make_second_info(info.str());
            throw msg;
        }
        //copy it
        for(int i=0; i< this->number;i++){
            buffer[i]=this->break_sc[i];
        }
        //delete
        this->delete_list();
        //count it
        this->number++;

        this->break_sc=buffer;
        //set the new one
        this->break_sc[this->number-1]=*add;
     }

//	Risk_Break_Scenario *buffer=NULL;
//	try{
//		buffer=new Risk_Break_Scenario[this->number];
//	}
//	catch(bad_alloc &t){
//		Error msg=this->set_error(0);
//		ostringstream info;
//		info<< "Info bad alloc: " << t.what() << endl;
//		msg.make_second_info(info.str());
//		throw msg;
//	}

//	//copy it
//	for(int i=0; i< this->number; i++){
//		buffer[i]=this->break_sc[i];
//	}

//	this->number++;

//	try{
//		this->allocate_list();
//		//recopy
//		for(int i=0; i< this->number-1; i++){
//			this->break_sc[i]=buffer[i];
//		}
//		this->break_sc[this->number-1]=*add;
//	}
//	catch(Error msg){
//		if(buffer!=NULL){
//			delete []buffer;
//			buffer=NULL;
//		}
//		throw msg;
//	}

//	if(buffer!=NULL){
//		delete []buffer;
//		buffer=NULL;
//	}

}
//Delete a scenario in the list
void Risk_Catchment_Break_Scenario_List::delete_scenario_from_list(Risk_Break_Scenario *delete_sc){
	
	if(delete_sc==NULL){
		return;
	}

	Risk_Catchment_Break_Scenario_List buffer;


	//copy it
	for(int i=0; i< this->number; i++){
		if(this->break_sc[i].get_break_scenario_string()==delete_sc->get_break_scenario_string() && 
			this->break_sc[i].get_sys_system_id().area_state==delete_sc->get_sys_system_id().area_state &&
			this->break_sc[i].get_sys_system_id().measure_nr==delete_sc->get_sys_system_id().measure_nr && 
			this->break_sc[i].get_hydraulic_boundary_scenario().get_id()==delete_sc->get_hydraulic_boundary_scenario().get_id()){
			//nothing to do
		}
		else{
			buffer.add_scenario2list(&this->break_sc[i]);
		}
	}

	this->clear_list();

	//recopy it
	for(int i=0; i< buffer.get_number_in_list(); i++){
		this->add_scenario2list(buffer.get_ptr_scenario(i));

	}
}
//Delete a list of scenario in the list
void Risk_Catchment_Break_Scenario_List::delete_scenario_from_list(Risk_Catchment_Break_Scenario_List *delete_list){
	if(delete_list==NULL){
		return;
	}
	
	for(int i=0; i< delete_list->get_number_in_list(); i++){
		this->delete_scenario_from_list(delete_list->get_ptr_scenario(i));

	}
}
//Clear the list
void Risk_Catchment_Break_Scenario_List::clear_list(void){
	this->number=0;
	this->delete_list();
	this->total_is_set=false;	
}
//Get the number of list members
int Risk_Catchment_Break_Scenario_List::get_number_in_list(void){
	return this->number;
}
//Get a pointer to the total list
Risk_Break_Scenario *Risk_Catchment_Break_Scenario_List::get_total_list(void){
	return this->break_sc;
}
//Get a pointer to a list member specified by the given index
Risk_Break_Scenario *Risk_Catchment_Break_Scenario_List::get_ptr_scenario(const int index){
	if(index<0 || index>=number){
		return NULL;
	}
	else{
		return &this->break_sc[index];
	}
}
//Sort the list after the probability
void Risk_Catchment_Break_Scenario_List::sort_list_after_probability(const bool total_probability){
/**
Bubble-sort algorithm for the sorting the detailed results array after the annuality.
*/
	Risk_Break_Scenario temp;
	double buffer=0.0;
	for(int i = this->number-1; i > 0; i--){
		for (int j = 1; j <= i; j++){
			if(total_probability==true){
				buffer=this->break_sc[j].get_prob_total();
			}
			else{
				buffer=this->break_sc[j].get_prob_scenario();
			}
			if (this->break_sc[j-1].get_prob_total() < buffer){
				temp = this->break_sc[j-1];
				this->break_sc[j-1] = this->break_sc[j];
				this->break_sc[j] = temp;
			}
		}
	}
}
//Sort the list after the number of involved fpl-section
void Risk_Catchment_Break_Scenario_List::sort_list_after_number_involved_section(const bool ascending){
/**
Bubble-sort algorithm for the sorting the detailed results array after the annuality.
*/
	Risk_Break_Scenario temp;
	int buffer=0;
	for(int i = this->number-1; i > 0; i--){
		for (int j = 1; j <= i; j++){
			buffer=this->break_sc[j].get_number_involved_sections();
			
			if (ascending==false && this->break_sc[j-1].get_number_involved_sections() < buffer){
				temp = this->break_sc[j-1];
				this->break_sc[j-1] = this->break_sc[j];
				this->break_sc[j] = temp;
			}
			else if (ascending==true && this->break_sc[j-1].get_number_involved_sections() > buffer){
				temp = this->break_sc[j-1];
				this->break_sc[j-1] = this->break_sc[j];
				this->break_sc[j] = temp;
			}
		}
	}
}
//Set all entries to the list as selected
void Risk_Catchment_Break_Scenario_List::set_all_in_list_selected(const bool  flag){
	for(int i=0; i< this->number; i++){
		this->break_sc[i].set_is_selected(flag);
	}
}
//Count number of selected scenarios in list
int Risk_Catchment_Break_Scenario_List::count_number_selected(void){

	int counter=0;
	for(int i=0; i< this->number; i++){
		if(this->break_sc[i].get_is_selected()==true){
			counter++;
		}
	}
		
	return counter;
}
//Count number of hydraulically calculated scenarios in list
int Risk_Catchment_Break_Scenario_List::count_number_hyd_calculated(void){
	int counter=0;
	for(int i=0; i< this->number; i++){
		if(this->break_sc[i].get_is_hydraulic_calculated()==true){
			counter++;
		}
	}
		
	return counter;
}
//Add the break height from the database table to the break scenarios stored in list
void Risk_Catchment_Break_Scenario_List::add_break_height_by_database(QSqlDatabase *ptr_database){
	for(int i=0; i< this->number; i++){
		this->break_sc[i].set_breakheight_database(ptr_database);
	}
}
//Output the list to display/console
void Risk_Catchment_Break_Scenario_List::output_list(void){
	ostringstream prefix;
	ostringstream cout;
	prefix<<"OUT> ";
	Sys_Common_Output::output_risk->set_userprefix(&prefix);
	if(this->number>0){
		cout << "List of " << this->number<<" break-scenario(s) (CA)"<<endl;
		cout << " no " << W(13) <<" hyd_bound "<<W(23)<< " break_sc " << W(12) << " prob" <<label::no_unit<< W(12)<< " total_prob" <<label::per_annus << W(10)<<" source "<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
	}
	double sum=0.0;
	for(int i=0; i< this->number; i++){
		cout <<" " << i<<"  "<<W(12);
		cout << this->break_sc[i].get_hydraulic_boundary_scenario().get_name()<<" ("<<this->break_sc[i].get_hydraulic_boundary_scenario().get_id()<<") "<<W(17);
		cout << this->break_sc[i].get_break_scenario_string()<<W(15);
		cout << FORMAT_FIXED_REAL << P(5)<< this->break_sc[i].get_prob_scenario()<<W(13);
		cout << this->break_sc[i].get_prob_total()<< W(20);
		cout << Risk_Break_Scenario::convert_source2txt(this->break_sc[i].get_source_scenario())<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		sum=sum+this->break_sc[i].get_prob_scenario();
	}
	cout <<"Sum of scenario probability : "<<sum<<label::no_unit<<endl;

	Sys_Common_Output::output_risk->output_txt(&cout);
	Sys_Common_Output::output_risk->rewind_userprefix();
}
//Cut list by a given index; the list entrees after the index arte deleted
void Risk_Catchment_Break_Scenario_List::cut_list(const int index){
	if(index >=this->number){
		//cut nothing
		return;
	}
	else if(index <=0){
		//cut all
		this->clear_list();
	}
	else{
		
		Risk_Catchment_Break_Scenario_List buffer;

		for(int i=0; i<index; i++){
			buffer.add_scenario2list(&this->break_sc[i]);
		}
		this->clear_list();
		for(int i=0; i<buffer.get_number_in_list(); i++){
			this->add_scenario2list(buffer.get_ptr_scenario(i));
		}

		buffer.clear_list();
	}
}
//Delete the scenario(s) of a list, which are not caluclated 
void Risk_Catchment_Break_Scenario_List::delete_not_calculated_in_list(void){
	Risk_Catchment_Break_Scenario_List buffer;

	for(int i=0; i<this->number; i++){
		if(this->break_sc[i].get_is_hydraulic_calculated()==true){
			buffer.add_scenario2list(&this->break_sc[i]);
		}
	}
	this->clear_list();
	for(int i=0; i<buffer.get_number_in_list(); i++){
		this->add_scenario2list(buffer.get_ptr_scenario(i));
	}

	buffer.clear_list();
}
//Delete the scenario(s) of a list, which are caluclated 
void Risk_Catchment_Break_Scenario_List::delete_calculated_in_list(void){
	Risk_Catchment_Break_Scenario_List buffer;

	for(int i=0; i<this->number; i++){
		if(this->break_sc[i].get_is_hydraulic_calculated()==false){
			buffer.add_scenario2list(&this->break_sc[i]);
		}
	}
	this->clear_list();
	for(int i=0; i<buffer.get_number_in_list(); i++){
		this->add_scenario2list(buffer.get_ptr_scenario(i));
	}

	buffer.clear_list();
}
//Set a sceenario list with all available break scenarios
void Risk_Catchment_Break_Scenario_List::set_total_risk_break_list(QSqlDatabase *ptr_database, const _sys_system_id id){
	this->clear_list();

	this->total_is_set=true;
	
	QSqlQueryModel query;
	//QSqlQueryModel query_hyd;
	int number=0;

	Hyd_Boundary_Szenario_Management hyd_bound;
	hyd_bound.set_systemid(id);
	hyd_bound.set_szenario_per_db(ptr_database);

	number=Risk_Break_Scenario_Management::select_data_in_table(&query, ptr_database, _risk_type::catchment_risk, id);
	string buffer;
	int hyd_bound_id=-1;
	double prob=0.0;
	_risk_break_sc_source src_buff;
	Risk_Break_Scenario sc_buff;
	sc_buff.set_risk_type(_risk_type::catchment_risk);
	sc_buff.set_systemid(id);


	for(int i=0; i<number; i++){
		buffer=query.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_id)).c_str()).toString().toStdString();
		hyd_bound_id=query.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
		prob=query.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_prob)).c_str()).toDouble();
		src_buff=Risk_Break_Scenario::convert_txt2scource(query.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_src)).c_str()).toString().toStdString());


		sc_buff.set_break_scenario_str(*(hyd_bound.get_ptr_sz_id_given(hyd_bound_id)),buffer);
		sc_buff.set_probability_sc(prob);
		sc_buff.set_source_scenario(src_buff);

		if(Hyd_Hydraulic_System::check_hyd_results_calculated(ptr_database, id, hyd_bound_id, buffer)==false){
			sc_buff.set_is_hydraulic_calculated(false);
			this->add_scenario2list(&sc_buff);	
		}
		else{
			sc_buff.set_is_hydraulic_calculated(true);
			this->add_scenario2list(&sc_buff);	
		}
	
	}

	this->add_break_height_by_database(ptr_database);
}
//Set a scenario list with all available break scenarios for one given hydraulic boundary scenario
void Risk_Catchment_Break_Scenario_List::set_risk_break_list_hyd_bound(QSqlDatabase *ptr_database, const _sys_system_id id, Hyd_Boundary_Szenario *bound){
	this->clear_list();

	
	QSqlQueryModel query;
	int number=0;


	number=Risk_Break_Scenario_Management::select_data_in_table(&query, ptr_database, _risk_type::catchment_risk, bound->get_id(), id);
	string buffer;

	double prob=0.0;
	_risk_break_sc_source src_buff;
	Risk_Break_Scenario sc_buff;
	sc_buff.set_risk_type(_risk_type::catchment_risk);
	sc_buff.set_systemid(id);


	for(int i=0; i<number; i++){
		buffer=query.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_id)).c_str()).toString().toStdString();
		prob=query.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_prob)).c_str()).toDouble();
		src_buff=Risk_Break_Scenario::convert_txt2scource(query.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_src)).c_str()).toString().toStdString());

		sc_buff.set_break_scenario_str(*(bound),buffer);
		sc_buff.set_probability_sc(prob);
		sc_buff.set_source_scenario(src_buff);

		if(Hyd_Hydraulic_System::check_hyd_results_calculated( ptr_database, id, bound->get_id(), buffer)==false){
			sc_buff.set_is_hydraulic_calculated(false);
			this->add_scenario2list(&sc_buff);	
		}
		else{
			sc_buff.set_is_hydraulic_calculated(true);
			this->add_scenario2list(&sc_buff);	
		}
	
	}

	this->add_break_height_by_database(ptr_database);
}
//Set a new list with the nearest hydraulical calculated break-scenarios for a given scenario
string Risk_Catchment_Break_Scenario_List::set_new_list_nearest_hyd_calc_scenario(Risk_Catchment_Break_Scenario_List *new_list, Risk_Break_Scenario *scenario){


	int number_inv_sec=scenario->get_number_involved_sections();


	if(number_inv_sec<0){
		return scenario->get_break_scenario_string();
	}


	for(int i=number_inv_sec; i>0; i--){
		for(int j=0; j< this->number; j++){
			if(this->break_sc[j].get_is_hydraulic_calculated()==true && this->break_sc[j].get_number_involved_sections()==i){
				if(scenario->compare_scenario_information(this->get_ptr_scenario(j))==true){
					new_list->add_scenario2list(this->get_ptr_scenario(j));
					Risk_Break_Scenario next;
					next=*scenario;
					next.reduce_involved_section(this->get_ptr_scenario(j));
					if(next.get_number_involved_sections()==0){
						return next.get_break_scenario_string();
					}
					else{
						return this->set_new_list_nearest_hyd_calc_scenario(new_list, &next);
					}
					return next.get_break_scenario_string();
				}
			}
		}
	}

	return scenario->get_break_scenario_string();


	//new_list->output_list();
}
//Find results of already calculated break scenarios, where the interval is unequal
void Risk_Catchment_Break_Scenario_List::find_break_scenario_unequal_interval(const string tofind, Risk_Break_Scenario *break_sc2predict, Risk_Catchment_Break_Scenario_List *new_list){

	if(tofind=="CA"){
		return;
	}

	//make a new break scenario
	Risk_Break_Scenario buffer;
	buffer=*break_sc2predict;
	buffer.set_break_scenario_str(break_sc2predict->get_hydraulic_boundary_scenario(), tofind);


	//make where just one break scenarios are in
	Risk_Catchment_Break_Scenario_List list;
	list.make_one_break_list_by_cutting_break_scenario(&buffer);

	for(int i=0; i< list.get_number_in_list(); i++){

		string buff_rest;
		int max_interval=buffer.get_max_interval();
		for(int j=max_interval; j>0; j--){

			buff_rest="CA";
			list.get_ptr_scenario(i)->reduce_interval(1);
			//buffer.reduce_interval(1);
			buff_rest=this->set_new_list_nearest_hyd_calc_scenario(new_list, list.get_ptr_scenario(i));
			if(buff_rest=="CA"){
				break;
			}

		}

	}
}
//Sum up probabilities for all scenario(s) in the list, where the given one is included
double Risk_Catchment_Break_Scenario_List::sum_prob_included_scenario(Risk_Break_Scenario *given){

	double sum_prob=0.0;
	bool involved=true;
	for(int j=0; j<this->number; j++){
		involved=true;
		if(given->get_number_involved_sections()>this->break_sc[j].get_number_involved_sections()){
			involved=false;
		}
		else{
			for(int i=0; i< given->get_number_involved_sections(); i++){
				if(this->break_sc[j].section_is_involved(given->get_involved_section(i).id_fpl_sec, given->get_involved_section(i).id_interval)==false){
					involved=false;
					break;
				}
			}
		}
		if(involved==true){
			sum_prob=sum_prob+this->break_sc[j].get_prob_scenario();

		}
	}
	return sum_prob;
}
//Merge two lists of break scenarios
void Risk_Catchment_Break_Scenario_List::merging_lists(Risk_Break_Scenario *scenario, const int number){

	bool new_sc;
	for(int i=0;i< number; i++){
		new_sc=true;
		for(int j=0; j<this->number; j++){
			if(scenario[i].get_break_scenario_string()==this->break_sc[j].get_break_scenario_string()){
				this->break_sc[j].set_probability_sc(scenario[i].get_prob_scenario());
				new_sc=false;
				break;
			}
		}
		if(new_sc==true){
			this->add_scenario2list(&scenario[i]);
		}


	}


}
//Get the flag, if the total break list is set
bool Risk_Catchment_Break_Scenario_List::get_total_break_list_is_set(void){
	return this->total_is_set;
}
//Calculate the statistic for the total break scenario list
void Risk_Catchment_Break_Scenario_List::calculate_output_statistic_total_break_list(QSqlDatabase *ptr_database, const _sys_system_id id, const double boundary, Risk_Catchment_Break_Scenario_List *one_break_list, Hyd_Boundary_Szenario_Management *buff_hyd){


	Hyd_Boundary_Scenario_List nobreak_hyd_sc;
	Hyd_Boundary_Scenario_List break_list_hyd_sc;
	Hyd_Boundary_Scenario_List onebreak_hyd_sc;
	_risk_statistic_info_break_sc *statistic_info=NULL;
	

	if(this->total_is_set==false){
		this->set_total_risk_break_list(ptr_database, id);
		this->sort_list_after_probability(true);
	}


	//alloc statistsical info
	try{
		statistic_info=new _risk_statistic_info_break_sc[buff_hyd->get_number_sz()];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//set them
	for(int i=0; i< buff_hyd->get_number_sz(); i++){
		statistic_info[i].number_calculated=0;
		statistic_info[i].sum_probability_calc=0.0;
		statistic_info[i].sum_probability=0.0;
		statistic_info[i].total_number2calc=0;
		statistic_info[i].ptr_hyd_sc=buff_hyd->get_ptr_sz(i);
	}
	int number_total_calc=0;
	double prob_total=0.0;
	double prob_total_calc=0.0;


	try{
		//list of nobreak 
		for(int i=0; i<buff_hyd->get_number_sz(); i++){
			if(Hyd_Hydraulic_System::check_hyd_results_calculated( ptr_database, id, buff_hyd->get_ptr_sz(i)->get_id(), "CA")==false){
				nobreak_hyd_sc.add_scenario2list(buff_hyd->get_ptr_sz(i));
			}
		}
		//which action have been already done
		QSqlQueryModel query;
		//list of break lists
		for(int i=0; i<buff_hyd->get_number_sz(); i++){ 
			if(Risk_Break_Scenario_Management::select_data_in_table(&query, ptr_database,_risk_type::catchment_risk, buff_hyd->get_ptr_sz(i)->get_id(), id)==0){
				break_list_hyd_sc.add_scenario2list(buff_hyd->get_ptr_sz(i));
			}
		}
		//list of one break
		for(int i=0; i<break_list_hyd_sc.get_number_in_list(); i++){
			onebreak_hyd_sc.add_scenario2list(break_list_hyd_sc.get_ptr_scenario(i));
		}
		for(int i=0; i<one_break_list->get_number_in_list(); i++){
			if(Hyd_Hydraulic_System::check_hyd_results_calculated( ptr_database, id, one_break_list->get_ptr_scenario(i)->get_hydraulic_boundary_scenario().get_id(), one_break_list->get_ptr_scenario(i)->get_break_scenario_string(), false)
				==false){
				onebreak_hyd_sc.add_scenario2list(buff_hyd->get_ptr_sz_id_given(one_break_list->get_ptr_scenario(i)->get_hydraulic_boundary_scenario().get_id()));
			}
		}


		//detailed statistic
		for(int i=0; i< this->number; i++){
			prob_total=prob_total+this->break_sc[i].get_prob_total();
			if(this->break_sc[i].get_is_hydraulic_calculated()==true){
				number_total_calc++;
				prob_total_calc=prob_total_calc+this->break_sc[i].get_prob_total();
			}
			for(int j=0; j<buff_hyd->get_number_sz(); j++){
				if(statistic_info[j].ptr_hyd_sc->get_id()==this->break_sc[i].get_hydraulic_boundary_scenario().get_id()){
					statistic_info[j].total_number2calc++;
					statistic_info[j].sum_probability=statistic_info[j].sum_probability+this->break_sc[i].get_prob_scenario();
					if(this->break_sc[i].get_is_hydraulic_calculated()==true){
						statistic_info[j].number_calculated++;
						statistic_info[j].sum_probability_calc=statistic_info[j].sum_probability_calc+this->break_sc[i].get_prob_scenario();
					}
					break;
				}
			}
		}
	}
	catch(Error msg){
		if(statistic_info!=NULL){
			delete []statistic_info;
			statistic_info=NULL;
		}
		throw msg;
	}

	//number of calculation required to reach the boundary
	int number2bound=0;
	double sum_ideal=prob_total_calc;
	if(sum_ideal<prob_total*boundary){
		for(int i=0; i< this->number; i++){
			if(this->break_sc[i].get_is_hydraulic_calculated()==false){
				sum_ideal=sum_ideal+this->break_sc[i].get_prob_total();
				number2bound++;
				if(sum_ideal>prob_total*boundary){
					break;
				}
				
			}
		}
	}

	//output
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_risk->set_userprefix(prefix.str());
	cout << "Statistics of the CATCHMENT risk approach..." << endl ;
	cout << "HYD-NOBREAK"<< endl;
	if(nobreak_hyd_sc.get_number_in_list()==0){
		cout << " All "<< buff_hyd->get_number_sz()<<" HYD-boundary nobreak scenario(s) are calculated" << endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
	}
	else{
		cout <<" "<< nobreak_hyd_sc.get_number_in_list() <<  " from "<<buff_hyd->get_number_sz() <<" HYD-boundary nobreak scenario(s) must be calculated"<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout <<" List of HYD-boundary scenario(s) " << endl;
		for(int i=0; i< nobreak_hyd_sc.get_number_in_list(); i++){
			cout << "  " << nobreak_hyd_sc.get_ptr_scenario(i)->get_id()<< "  " << nobreak_hyd_sc.get_ptr_scenario(i)->get_name()<<endl;

		}
		Sys_Common_Output::output_risk->output_txt(&cout, true);
	}
	cout << "RISK-BREAK SCENARIO LIST"<< endl;
	if(break_list_hyd_sc.get_number_in_list()==0){
		cout << " All "<< buff_hyd->get_number_sz()<<" HYD-boundary break scenario list(s) are generated" << endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
	}
	else{
		cout <<" "<< break_list_hyd_sc.get_number_in_list() <<  " from "<<buff_hyd->get_number_sz() <<" HYD-boundary break scenario list(s) must be generated"<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout <<" List of HYD-boundary scenario(s) " << endl;
		for(int i=0; i< break_list_hyd_sc.get_number_in_list(); i++){
			cout << "  " << break_list_hyd_sc.get_ptr_scenario(i)->get_id()<< "  " << break_list_hyd_sc.get_ptr_scenario(i)->get_name()<<endl;

		}
		Sys_Common_Output::output_risk->output_txt(&cout, true);
	}
	cout << "HYD-ONE-BREAK"<< endl;
	if(onebreak_hyd_sc.get_number_in_list()==0){
		cout << " All one-break scenario(s) of "<< buff_hyd->get_number_sz()<<" HYD-boundary scenario(s) are calculated" << endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
	}
	else{
		cout <<" One-break scenario(s) of "<<  onebreak_hyd_sc.get_number_in_list() << " from "<<buff_hyd->get_number_sz() <<" HYD-boundary scenario(s) must be calculated"<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		cout <<" List of HYD-boundary scenario(s) " << endl;
		for(int i=0; i< onebreak_hyd_sc.get_number_in_list(); i++){
			cout << "  " << onebreak_hyd_sc.get_ptr_scenario(i)->get_id()<< "  " << onebreak_hyd_sc.get_ptr_scenario(i)->get_name()<<endl;

		}
		Sys_Common_Output::output_risk->output_txt(&cout, true);
	}

	cout << "HYD-DETAILED STATISTICS"<< endl;
	cout << " HYD-bound info "<< W(15)<<" calc(total) "<< W(10)<< " calc_prob(total_prob) " <<label::no_unit<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	for(int i=0; i< buff_hyd->get_number_sz(); i++){
		cout << " " <<statistic_info[i].ptr_hyd_sc->get_id() <<" " <<statistic_info[i].ptr_hyd_sc->get_name();
		cout << W(15)<< statistic_info[i].number_calculated << " ("<<statistic_info[i].total_number2calc<<") ";
		cout << W(13)<< statistic_info[i].sum_probability_calc << " ("<<statistic_info[i].sum_probability<<") ";
		cout << endl;
	}
	Sys_Common_Output::output_risk->output_txt(&cout);

	cout << "HYD-TOTAL STATISTICS"<< endl;
	cout << " Total number                         : "<< this->number<<endl;
	cout << " Number calculated                    : "<< number_total_calc <<endl;
	cout << " Probability total                    : "<< prob_total << label::per_annus<<endl;
	cout << " Probability calculated               : "<< prob_total_calc << label::per_annus<<endl;
	if(number2bound==0){
		cout << " Boundary of "<<boundary*100.0<<label::percentage << "is reached"<<endl;
	}	
	else{
		cout << " Number calculation to reach boundary : "<< number2bound << " ("<<boundary*100.0<<label::percentage<<") "<<endl;
	}
	cout << "RISK-TOTAL STATISTICS"<< endl;
	if(Risk_System::check_data_in_resulttable(ptr_database, id, _risk_type::catchment_risk)==true){
		cout << " Total risk is calculated            : "<< functions::convert_boolean2string(true) << endl;
	}
	else{
		cout << " Total risk is calculated            : "<< functions::convert_boolean2string(false) << endl;
	}

	Sys_Common_Output::output_risk->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_risk->rewind_userprefix();
	cout << "End of the statistics of the CATCHMENT risk approach" << endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);
	//delete statistic info buffer
	if(statistic_info!=NULL){
		delete []statistic_info;
		statistic_info=NULL;
	}

}
//Prepare the list for the automatic scenario calculation
void Risk_Catchment_Break_Scenario_List::prepare_list_auto_generation(QSqlDatabase *ptr_database, const _sys_system_id id, const _risk_ca_scenario_control params){
	
	if(this->total_is_set==false){
		this->set_total_risk_break_list(ptr_database, id);
		this->sort_list_after_probability(true);
	}
	int number_total_calc=0;
	double prob_total=0.0;
	double prob_total_calc=0.0;

	//detailed statistic
	for(int i=0; i< this->number; i++){
		prob_total=prob_total+this->break_sc[i].get_prob_total();
		if(this->break_sc[i].get_is_hydraulic_calculated()==true){
			number_total_calc++;
			prob_total_calc=prob_total_calc+this->break_sc[i].get_prob_total();
		}
	}
	//number of calculation required to reach the boundary
	double sum_ideal=prob_total_calc;
	Risk_Catchment_Break_Scenario_List buffer;

	if(sum_ideal<prob_total*params.prob_bound){
		for(int i=0; i< this->number; i++){
			if(this->break_sc[i].get_is_hydraulic_calculated()==false){
				
				sum_ideal=sum_ideal+this->break_sc[i].get_prob_total();
				buffer.add_scenario2list(&this->break_sc[i]);
				if(sum_ideal>prob_total*params.prob_bound){
					break;
				}
				
			}
		}
	}

	//make optimisation after the predicted risk data
	if(params.use_predict_values==true){
		
		_risk_predicted_risk already_in_buffer;
		Risk_Damage_Predictor::init_risk_predicted_risk_structure(&already_in_buffer);	
		_risk_predicted_risk already_calc;
		Risk_Damage_Predictor::init_risk_predicted_risk_structure(&already_calc);

		this->delete_calculated_in_list();

		//set the predicted risks to the break scenarios
		this->set_predicted_risk_result2break_sc(ptr_database);
		//sum up total predicted risk results
		this->sum_predicted_risk_result_break_sc(&already_calc);

		//delete all already taken scenario in buffer
		this->delete_scenario_from_list(&buffer);
		//set the predicted risks to the already gathered break scenarios
		buffer.set_predicted_risk_result2break_sc(ptr_database);

		//sum up the predicted risk in buffer
		buffer.sum_predicted_risk_result_break_sc(&already_in_buffer);

		int counter=0;

		Risk_Break_Scenario *buff_sc;
		buff_sc=NULL;
		do{
			//fill up the list with relevant scenarios

			//ecn
			if(counter==0){
				if(already_in_buffer.ecn_risk_perc<(params.boundary_ecn_risk-(100.0-already_calc.ecn_risk_perc))){
					buff_sc=this->get_highest_predicted_ecn_risk();
					buffer.add_scenario2list(buff_sc);
					this->delete_scenario_from_list(buff_sc);
				}
				else{
					counter=1;
				}
			}
			//eco
			if(counter==1){
				if(already_in_buffer.eco_risk_perc<(params.boundary_eco_risk-(100.0-already_calc.eco_risk_perc))){
					buff_sc=this->get_highest_predicted_eco_risk();
					buffer.add_scenario2list(buff_sc);
					this->delete_scenario_from_list(buff_sc);
				}
				else{
					counter=2;
				}
			}

			//pop affected
			if(counter==2){
				if(already_in_buffer.pop_affected_risk_perc<(params.boundary_pop_affected_risk-(100.0-already_calc.pop_affected_risk_perc))){
					buff_sc=this->get_highest_predicted_eco_risk();
					buffer.add_scenario2list(buff_sc);
					this->delete_scenario_from_list(buff_sc);
				}
				else{
					counter=3;
				}
			}

			//pop endangered
			if(counter==3){
				if(already_in_buffer.pop_endangered_risk_perc<(params.boundary_pop_endangered_risk-(100.0-already_calc.pop_endangered_risk_perc))){
					buff_sc=this->get_highest_predicted_eco_risk();
					buffer.add_scenario2list(buff_sc);
					this->delete_scenario_from_list(buff_sc);
				}
				else{
					counter=4;
				}
			}

			//pys 
			if(counter==4){
				if(already_in_buffer.pys_without_density_risk_perc<(params.boundary_pys_risk-(100.0-already_calc.pys_without_density_risk_perc))){
					buff_sc=this->get_highest_predicted_eco_risk();
					buffer.add_scenario2list(buff_sc);
					this->delete_scenario_from_list(buff_sc);
				}
				else{
					counter=5;
				}
			}



			counter++;
			if(counter>=4){
				counter=0;
			}
			buff_sc=NULL;

			//reinit
			Risk_Damage_Predictor::init_risk_predicted_risk_structure(&already_in_buffer);
			//recalc
			buffer.sum_predicted_risk_result_break_sc(&already_in_buffer);

			if(this->number==0){
				break;
			}
		}
		while(already_in_buffer.ecn_risk_perc<(params.boundary_ecn_risk-(100.0-already_calc.ecn_risk_perc)) || 
				already_in_buffer.eco_risk_perc<(params.boundary_eco_risk-(100.0-already_calc.eco_risk_perc)) ||
				already_in_buffer.pop_affected_risk_perc<(params.boundary_pop_affected_risk-(100.0-already_calc.pop_affected_risk_perc)) ||
				already_in_buffer.pop_endangered_risk_perc<(params.boundary_pop_endangered_risk -(100.0-already_calc.pop_endangered_risk_perc)) ||
				already_in_buffer.pys_without_density_risk_perc<(params.boundary_pys_risk-(100.0-already_calc.pys_without_density_risk_perc)));

	}



	//recopy it to the main list
	this->clear_list();
	for(int i=0; i< buffer.get_number_in_list(); i++){
		this->add_scenario2list(buffer.get_ptr_scenario(i));
	}


}
///Get the state of probability, which is reach 
double Risk_Catchment_Break_Scenario_List::get_prob_scenario_calculated(QSqlDatabase *ptr_database, const _sys_system_id id){

	this->set_total_risk_break_list(ptr_database, id);
	this->sort_list_after_probability(true);

	double prob_total=0.0;
	double prob_total_calc=0.0;
	//detailed statistic
	for(int i=0; i< this->number; i++){
		prob_total=prob_total+this->break_sc[i].get_prob_total();
		if(this->break_sc[i].get_is_hydraulic_calculated()==true){
			prob_total_calc=prob_total_calc+this->break_sc[i].get_prob_total();
		}
	}
	if(prob_total==0.0){
		prob_total=1.0e-20;
	}
	double percent=0.0;
	percent=prob_total_calc/prob_total;
	return percent;
}
//Make list out of a given break scenario, where just one break scenario are in (cut break scenario)
void Risk_Catchment_Break_Scenario_List::make_one_break_list_by_cutting_break_scenario(Risk_Break_Scenario *sc2cut){
	this->delete_list();

	Risk_Break_Scenario buff;
	buff=*sc2cut;
	ostringstream buff_str;

	for(int i=0; i< sc2cut->get_number_involved_sections(); i++){
		buff_str.str("");
		buff_str<<"CA_";
		buff_str<<sc2cut->get_involved_section(i).id_fpl_sec<<"-";
		buff_str<<sc2cut->get_involved_section(i).id_interval;
		buff.set_break_scenario_str(sc2cut->get_hydraulic_boundary_scenario(), buff_str.str());
		this->add_scenario2list(&buff);
	}
}
//Set the predicted risk results to the break scenarios in the list
void Risk_Catchment_Break_Scenario_List::set_predicted_risk_result2break_sc(QSqlDatabase *ptr_database){
	for(int i=0; i< this->number; i++){
		this->break_sc[i].set_predicted_risk_data(ptr_database);
	}
}
//Sum up the predicted risk results of the break scenarios in the list
void Risk_Catchment_Break_Scenario_List::sum_predicted_risk_result_break_sc(_risk_predicted_risk *sum){
	for(int i=0; i< this->number; i++){
		if(this->break_sc[i].get_ptr_predicted_risk_values()!=NULL){
			sum->ecn_risk=sum->ecn_risk+this->break_sc[i].get_ptr_predicted_risk_values()->ecn_risk;
			sum->ecn_risk_perc=sum->ecn_risk_perc+this->break_sc[i].get_ptr_predicted_risk_values()->ecn_risk_perc;
			sum->eco_risk=sum->eco_risk+this->break_sc[i].get_ptr_predicted_risk_values()->eco_risk;
			sum->eco_risk_perc=sum->eco_risk_perc+this->break_sc[i].get_ptr_predicted_risk_values()->eco_risk_perc;

			sum->pop_affected_risk=sum->pop_affected_risk+this->break_sc[i].get_ptr_predicted_risk_values()->pop_affected_risk;
			sum->pop_affected_risk_perc=sum->pop_affected_risk_perc+this->break_sc[i].get_ptr_predicted_risk_values()->pop_affected_risk_perc;

			sum->pop_endangered_risk=sum->pop_endangered_risk+this->break_sc[i].get_ptr_predicted_risk_values()->pop_endangered_risk;
			sum->pop_endangered_risk_perc=sum->pop_endangered_risk_perc+this->break_sc[i].get_ptr_predicted_risk_values()->pop_endangered_risk_perc;

			sum->pys_with_density_risk=sum->pys_with_density_risk+this->break_sc[i].get_ptr_predicted_risk_values()->pys_with_density_risk;
			sum->pys_with_density_risk_perc=sum->pys_with_density_risk_perc+this->break_sc[i].get_ptr_predicted_risk_values()->pys_with_density_risk_perc;

			sum->pys_without_density_risk=sum->pys_without_density_risk+this->break_sc[i].get_ptr_predicted_risk_values()->pys_without_density_risk;
			sum->pys_without_density_risk_perc=sum->pys_without_density_risk_perc+this->break_sc[i].get_ptr_predicted_risk_values()->pys_without_density_risk_perc;

		}
	}
}
//_______
//private
//Allocate the break scenarios
void Risk_Catchment_Break_Scenario_List::allocate_list(void){
	this->delete_list();
	try{
		this->break_sc=new Risk_Break_Scenario[this->number];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	for(int i=0; i< this->number; i++){
		this->break_sc[i].set_risk_type(_risk_type::catchment_risk);
	}
}
//Delete the list
void Risk_Catchment_Break_Scenario_List::delete_list(void){
	if(this->break_sc!=NULL){
		delete []this->break_sc;
		this->break_sc=NULL;
		this->total_is_set=false;
	}
}
//Get the break scenario with the highest economic predicted risk
Risk_Break_Scenario *Risk_Catchment_Break_Scenario_List::get_highest_predicted_ecn_risk(void){
	Risk_Break_Scenario *buffer;
	buffer=NULL;

	double max_value=0.0;

	for(int i=0; i< this->number; i++){
		if(this->break_sc[i].get_ptr_predicted_risk_values()!=NULL){
			if(this->break_sc[i].get_ptr_predicted_risk_values()->ecn_risk_perc>max_value){
				buffer=&this->break_sc[i];
				max_value=this->break_sc[i].get_ptr_predicted_risk_values()->ecn_risk_perc;
			}
		}
	}

	return buffer;
}
//Get the break scenario with the highest ecological predicted risk
Risk_Break_Scenario *Risk_Catchment_Break_Scenario_List::get_highest_predicted_eco_risk(void){
	Risk_Break_Scenario *buffer;
	buffer=NULL;

	double max_value=0.0;

	for(int i=0; i< this->number; i++){
		if(this->break_sc[i].get_ptr_predicted_risk_values()!=NULL){
			if(this->break_sc[i].get_ptr_predicted_risk_values()->eco_risk_perc>max_value){
				buffer=&this->break_sc[i];
				max_value=this->break_sc[i].get_ptr_predicted_risk_values()->eco_risk_perc;
			}
		}
	}

	return buffer;
}
//Get the break scenario with the highest people2risk affected predicted risk
Risk_Break_Scenario *Risk_Catchment_Break_Scenario_List::get_highest_predicted_pop_affected_risk(void){
	Risk_Break_Scenario *buffer;
	buffer=NULL;

	double max_value=0.0;

	for(int i=0; i< this->number; i++){
		if(this->break_sc[i].get_ptr_predicted_risk_values()!=NULL){
			if(this->break_sc[i].get_ptr_predicted_risk_values()->pop_affected_risk_perc>max_value){
				buffer=&this->break_sc[i];
				max_value=this->break_sc[i].get_ptr_predicted_risk_values()->pop_affected_risk_perc;
			}
		}
	}

	return buffer;
}
//Get the break scenario with the highest people2risk endangered predicted risk
Risk_Break_Scenario *Risk_Catchment_Break_Scenario_List::get_highest_predicted_pop_endangered_risk(void){
	Risk_Break_Scenario *buffer;
	buffer=NULL;

	double max_value=0.0;

	for(int i=0; i< this->number; i++){
		if(this->break_sc[i].get_ptr_predicted_risk_values()!=NULL){
			if(this->break_sc[i].get_ptr_predicted_risk_values()->pop_endangered_risk_perc>max_value){
				buffer=&this->break_sc[i];
				max_value=this->break_sc[i].get_ptr_predicted_risk_values()->pop_endangered_risk_perc;
			}
		}
	}

	return buffer;
}
//Get the break scenario with the highest psycho-social predicted risk
Risk_Break_Scenario *Risk_Catchment_Break_Scenario_List::get_highest_predicted_pys_risk(void){
	Risk_Break_Scenario *buffer;
	buffer=NULL;

	double max_value=0.0;

	for(int i=0; i< this->number; i++){
		if(this->break_sc[i].get_ptr_predicted_risk_values()!=NULL){
			if(this->break_sc[i].get_ptr_predicted_risk_values()->pys_without_density_risk_perc>max_value){
				buffer=&this->break_sc[i];
				max_value=this->break_sc[i].get_ptr_predicted_risk_values()->pys_without_density_risk_perc;
			}
		}
	}

	return buffer;
}
//Set error(s)
Error Risk_Catchment_Break_Scenario_List::set_error(const int err_type){
	string place="Risk_Catchment_Break_Scenario_List::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_list(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("calculate_output_statistic_total_break_list(QSqlDatabase *ptr_database, const _sys_system_id id, const double boundary, Risk_Catchment_Break_Scenario_List *one_break_list)");
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
