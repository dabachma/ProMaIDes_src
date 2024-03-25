#include "Risk_Headers_Precompiled.h"
//#include "Risk_Break_Scenario.h"


//Default constructor
Risk_Break_Scenario::Risk_Break_Scenario(void){

	this->break_sc_id="";
	this->number_section=0;
	this->fpl_section_ids=NULL;
	this->risk_type=_risk_type::scenario_risk;
	this->prob_without_hyd_sc=1.0;
	this->is_calculated=false;
	this->is_selected=false;
	this->hyd_is_calculated=false;
	this->ptr_predicted_risk=NULL;

	this->source=_risk_break_sc_source::sc_user_defined;


	Sys_Memory_Count::self()->add_mem(sizeof(Risk_Break_Scenario)-sizeof(Hyd_Boundary_Szenario),_sys_system_modules::RISK_SYS);//count the memory
}
//Copy constructor
Risk_Break_Scenario::Risk_Break_Scenario(const Risk_Break_Scenario &copy){


    this->fpl_section_ids=NULL;
    this->ptr_predicted_risk=NULL;
	this->system_id=copy.system_id;
	this->break_sc_id=copy.break_sc_id;
	this->number_section=copy.number_section;
	this->risk_type=copy.risk_type;
	this->id_hydraulic_bound=copy.id_hydraulic_bound;
	this->prob_without_hyd_sc=copy.prob_without_hyd_sc;
	this->is_calculated=copy.is_calculated;
	this->is_selected=copy.is_selected;
	this->hyd_is_calculated=copy.hyd_is_calculated;
	this->source=copy.source;

	this->allocate_array_section();
	for(int i=0; i<this->number_section; i++){
		this->fpl_section_ids[i]=copy.fpl_section_ids[i];
	}

	if(copy.ptr_predicted_risk!=NULL){
		this->allocate_predicted_risk_result();
		*this->ptr_predicted_risk=*copy.ptr_predicted_risk;
	}


	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Risk_Break_Scenario)-sizeof(Hyd_Boundary_Szenario), _sys_system_modules::RISK_SYS);
}
//Default destructor
Risk_Break_Scenario::~Risk_Break_Scenario(void){

	this->delete_array_section();
	this->delete_predicted_risk_result();
	Sys_Memory_Count::self()->minus_mem(sizeof(Risk_Break_Scenario)-sizeof(Hyd_Boundary_Szenario),_sys_system_modules::RISK_SYS);//count the memory
}
//_________
//public
//Get the break scenario string
string Risk_Break_Scenario::get_break_scenario_string(void){
	return this->break_sc_id;
}
//Get the hydraulic boundary scenario 
Hyd_Boundary_Szenario& Risk_Break_Scenario::get_hydraulic_boundary_scenario(void){
	return this->id_hydraulic_bound;
}
//Set the hydraulic boundary scenario 
void Risk_Break_Scenario::set_hydraulic_boundary_scenario(Hyd_Boundary_Szenario sc){
	 this->id_hydraulic_bound=sc;
}
//Get the number of involved fpl-sections
int Risk_Break_Scenario::get_number_involved_sections(void){
	return this->number_section;
}
//Get if a given section id is involved in this break scenario
bool Risk_Break_Scenario::section_is_involved(const int section_id){
	for(int i=0; i< this->number_section; i++){
		if(section_id==this->fpl_section_ids[i].id_fpl_sec){
			return true;
		}
	}
	return false;
}
//Check if a given section is involved in this break scenario
bool Risk_Break_Scenario::section_is_involved(const int section_id, const string interval_id){
	for(int i=0; i< this->number_section; i++){
		if(section_id==this->fpl_section_ids[i].id_fpl_sec && interval_id==this->fpl_section_ids[i].id_interval){
			return true;
		}
	}
	return false;
}
//Get a pointer to the identifier of the involved fpl-sections
_fpl_break_info *Risk_Break_Scenario::get_ptr_involved_sections(void){
	return this->fpl_section_ids;
}
//Get the id of the involved fpl-section specified by the index
_fpl_break_info Risk_Break_Scenario::get_involved_section(const int index){
	if(index<0 ||index>=this->number_section){
		_fpl_break_info buffer;
		buffer.break_height=0.0;
		buffer.id_fpl_sec=-1;
		buffer.id_interval=-1;
		return buffer;
	}
	return this->fpl_section_ids[index];
}
//Get the string id for a involved fpl-section specified by the index
string Risk_Break_Scenario::get_string_id_involved_section(const int index){
	
	ostringstream buffer;
	if(index>0 && index<this->number_section){
		buffer<<"_"<<this->fpl_section_ids[index].id_fpl_sec<<"-"<<this->fpl_section_ids[index].id_interval;
	}


	return buffer.str();


}
//Set a break scenario with the given fpl-section
void Risk_Break_Scenario::set_break_scenario(const int number_section, _fpl_break_info *section_ids, Hyd_Boundary_Szenario *hydraulic_bound_sc){
	this->id_hydraulic_bound=*hydraulic_bound_sc;
	this->delete_array_section();
	this->number_section=number_section;
	try{
		this->allocate_array_section();
	}
	catch(Error msg){
		throw msg;
	}
	//set it
	if(this->risk_type==_risk_type::scenario_risk){
		for(int i=0; i< this->number_section; i++){
			this->fpl_section_ids[i]=section_ids[i];
			
			ostringstream buffer;
			buffer << FORMAT_FIXED_REAL << P(1) << this->fpl_section_ids[i].break_height;
			this->fpl_section_ids[i].id_interval=buffer.str();
			
		}
	}
	else{
		for(int i=0; i< this->number_section; i++){
			this->fpl_section_ids[i]=section_ids[i];
		}

	}
	this->sort_involved_section_id();
	this->generate_break_scenario_string();

}
//Set a brak scenario string and translate it to the involved section
void Risk_Break_Scenario::set_break_scenario_str(const Hyd_Boundary_Szenario hydraulic_bound_sc, const string break_sc){
	this->id_hydraulic_bound=hydraulic_bound_sc;
	this->break_sc_id=break_sc;
	this->translate_string2involved_section();
}
//Compare two break scenarios, if the given one includes information for this scenario
bool Risk_Break_Scenario::compare_scenario_information(Risk_Break_Scenario *given){
	if(this->number_section>=given->get_number_involved_sections()){
		bool all_found=true;
		//allocate bool-array
		bool *found=NULL;
		try{
			found=new bool[given->get_number_involved_sections()];
			//initialize bool-array
			for(int i=0; i<given->get_number_involved_sections(); i++){
				found[i]=false;
			}
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(2);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;

		}
		//check if the same one are included
		for(int i=0; i<given->get_number_involved_sections(); i++){
			for(int j=0; j<this->number_section; j++){
				if(given->get_involved_section(i).id_fpl_sec==this->fpl_section_ids[j].id_fpl_sec &&
					given->get_involved_section(i).id_interval==this->fpl_section_ids[j].id_interval){
					found[i]=true;
					break;
				}
			}
		}


		//check the bool-array
		for(int i=0; i<given->get_number_involved_sections(); i++){
			if(found[i]==false){
				all_found=false;	
			}
		}

		//delete bool array
		if(found!=NULL){
			delete []found;
			found=NULL;
		}

		return all_found;

	}
	else{
		return false;
	}
}
//Get the risk type
_risk_type Risk_Break_Scenario::get_risk_type(void){
	return this->risk_type;
}
//Set the risk type
void Risk_Break_Scenario::set_risk_type(_risk_type type){
	this->risk_type=type;
}
//Output the members to display/console
void Risk_Break_Scenario::output_members(void){
	ostringstream cout;
	cout << "Hydraulic boundary scenario id   : " <<this->id_hydraulic_bound.get_id()<<endl;
	cout << "Hydraulic boundary scenario name : " <<this->id_hydraulic_bound.get_name()<<endl;
	cout << "Break scenario id                : "<<this->break_sc_id<<endl;
	cout << "Type of risk                     : " <<Risk_Break_Scenario::convert_risk_type2txt(this->risk_type)<<endl;
	cout << "Number involved fpl-section(s)   : " <<this->number_section<<endl;
	cout << "Involved fpl-section(s)          : " <<endl;
	for(int i=0; i<this->number_section; i++){
		cout << this->fpl_section_ids[i].id_fpl_sec<<"   ("<<this->fpl_section_ids[i].break_height<<label::m<<")"<<endl;
	}
	if(this->number_section>0){
		cout <<endl;
	}
	Sys_Common_Output::output_risk->output_txt(&cout);
}
//Convert a string into a risk type (_risk_type) (static)
_risk_type Risk_Break_Scenario::convert_txt2risk_type(const string txt){
	_risk_type type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lower_case(&buffer);

	if(buffer==risk_label::risk_nobreak){
		type=_risk_type::nobreak_risk;
	}
	else if(buffer==risk_label::risk_scenario){
		type=_risk_type::scenario_risk;
	}
	else if(buffer==risk_label::risk_catchment){
		type=_risk_type::catchment_risk;
	}
	else{
		Error msg;
		msg.set_msg("_risk_type Risk_System::convert_txt2risk_type(const string txt)", "Can not convert this risk type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert risk type: " << txt << endl;
		info << "Possible types: "<< endl;
		info << " "<<risk_label::risk_nobreak  << endl;
		info << " "<<risk_label::risk_scenario  << endl;
		info << " "<<risk_label::risk_catchment  << endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	return type;
}
//Convert a risk type (_risk_type) into a string (static)
string Risk_Break_Scenario::convert_risk_type2txt(const _risk_type type){
	string buffer;
	switch(type){
		case _risk_type::nobreak_risk:
			buffer=risk_label::risk_nobreak;
			break;
		case _risk_type::scenario_risk:
			buffer=risk_label::risk_scenario;
			break;
		case _risk_type::catchment_risk:
			buffer=risk_label::risk_catchment;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//Convert a string into a source type (_risk_break_sc_source) (static)
_risk_break_sc_source Risk_Break_Scenario::convert_txt2scource(const string txt){
	_risk_break_sc_source type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lower_case(&buffer);

	if(buffer==risk_label::src_sc_user){
		type=_risk_break_sc_source::sc_user_defined;
	}
	else if(buffer==risk_label::src_sc_fixed){
		type=_risk_break_sc_source::sc_fixed;
	}
	else if(buffer==risk_label::src_sc_gen){
		type=_risk_break_sc_source::sc_generated;
	}
	else{
		Error msg;
		msg.set_msg("_risk_break_sc_source Risk_Break_Scenario::convert_txt2scource(const string txt)", "Can not convert this break scenario source type", "Check the given source type", 1, false);
		ostringstream info;
		info << "Try to convert source type: " << txt << endl;
		info << "Possible source types: "<< endl;
		info << " "<<risk_label::src_sc_user  << endl;
		info << " "<<risk_label::src_sc_fixed  << endl;
		info << " "<<risk_label::src_sc_gen  << endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	return type;
}
//Convert a source type (_risk_break_sc_source) into a string (static)
 string Risk_Break_Scenario::convert_source2txt(const _risk_break_sc_source src){
	string buffer;
	switch(src){
		case _risk_break_sc_source::sc_user_defined:
			buffer=risk_label::src_sc_user;
			break;
		case _risk_break_sc_source::sc_fixed:
			buffer=risk_label::src_sc_fixed;
			break;
		case _risk_break_sc_source::sc_generated:
			buffer=risk_label::src_sc_gen;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
 }
//Get the total probability of the scenario
double Risk_Break_Scenario::get_prob_total(void){

	

	if(this->risk_type==_risk_type::scenario_risk){
		return (this->prob_without_hyd_sc);
	}
	else if(this->risk_type==_risk_type::catchment_risk){
		return (this->prob_without_hyd_sc)*this->id_hydraulic_bound.get_prob_total();
	}
	else{
		return this->id_hydraulic_bound.get_prob_total();
	}
}
//Get the probability of the scenario without the hydraulic boundary probability
double Risk_Break_Scenario::get_prob_scenario(void){
	return this->prob_without_hyd_sc;
}
//Calculate the probability
void Risk_Break_Scenario::calculate_probability(QSqlDatabase *ptr_database){

	if(this->risk_type==_risk_type::scenario_risk){
		QSqlQueryModel model;
		int number;
		number=Fpl_Mc_Sim::select_results_in_database(&model, ptr_database, this->system_id, this->fpl_section_ids[0].id_fpl_sec, true);
		
		if(number<1){
			Error msg=this->set_error(1);
			ostringstream info;
			info << "Name of the hydraulic boundary scenario : " << this->id_hydraulic_bound.get_name()<<endl;
			info << "Id of the hydraulic boundary scenario   : " << this->id_hydraulic_bound.get_id()<<endl;
			info << "Id of the break scenario                : " << this->break_sc_id<<endl;
			msg.make_second_info(info.str());
			throw msg;

		}
		
		this->prob_without_hyd_sc=model.record(0).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_probability)).c_str()).toDouble();
		
	}
	else if(this->risk_type==_risk_type::catchment_risk){
		//nothing to do
	}
	else{
		this->prob_without_hyd_sc=0.0;

	}
}
//Set the probability of the scenario
void Risk_Break_Scenario::set_probability_sc(const double prob){
	this->prob_without_hyd_sc=prob;
}
//Set the frc-interval break height by database table
void Risk_Break_Scenario::set_breakheight_database(QSqlDatabase *ptr_database){
	QString buff;
	if(this->risk_type==_risk_type::catchment_risk){
		for(int i=0; i< this->number_section; i++){	
			buff=this->fpl_section_ids[i].id_interval.c_str();
			this->fpl_section_ids[i].break_height=Fpl_Frc_Curve::get_break_height(ptr_database, this->system_id, this->fpl_section_ids[i].id_fpl_sec, this->id_hydraulic_bound.get_id(), buff.toInt() );
		}
	}
			
}
//Get a description string of the break scenario with the name f the hydraulic boundary scenario
string Risk_Break_Scenario::get_description_string_with_name(void){
	ostringstream buff_stream;
	buff_stream <<this->id_hydraulic_bound.get_id()<<"-"<<this->id_hydraulic_bound.get_name()<<"."<<this->break_sc_id;
	//<< "A"<<this->system_id.area_state<<"-M"<<this->system_id.measure_nr<<"."
	return buff_stream.str();		
}
//Get a description string of the break scenario just with the identifier
string Risk_Break_Scenario::get_description_string(void){
	ostringstream buff_stream;
	buff_stream << this->id_hydraulic_bound.get_id()<<"-"<<this->break_sc_id;
	return buff_stream.str();
}
//Get if the scenario is already calculated
bool Risk_Break_Scenario::get_is_calculated(void){
	return this->is_calculated;
}
//Check if the sceneraio is already calculated
void Risk_Break_Scenario::check_is_calculated(QSqlDatabase *ptr_database){
	this->is_calculated=Risk_System::check_detailed_risk_results_available(ptr_database, this->system_id, this->risk_type, this->break_sc_id, this->id_hydraulic_bound.get_id());
}
//Get if the scenario is selected
bool Risk_Break_Scenario::get_is_selected(void){
	return this->is_selected;
}
//Set if the scenario is slected
void Risk_Break_Scenario::set_is_selected(const bool flag){
	this->is_selected=flag;
}
//Set the flag if the break scenario is already hydaulically calculated
void Risk_Break_Scenario::set_is_hydraulic_calculated(const bool flag){
	this->hyd_is_calculated=flag;
}
//Get the flag if the break scenario is already hydaulically calculated
bool Risk_Break_Scenario::get_is_hydraulic_calculated(void){
	return this->hyd_is_calculated;
}
//Set the source of the scenario
void Risk_Break_Scenario::set_source_scenario(const _risk_break_sc_source src){
	this->source=src;
}
//Get the source of the scenario
_risk_break_sc_source Risk_Break_Scenario::get_source_scenario(void){
	return this->source;
}
//Reduce the involved section of a break scenario with a given one
void Risk_Break_Scenario::reduce_involved_section(Risk_Break_Scenario *given){

	bool is_same=false;
	ostringstream new_id;
	new_id<<"CA";
	for(int i=0; i< this->number_section; i++){
		is_same=false;
		for(int j=0; j< given->get_number_involved_sections(); j++){
			if(this->fpl_section_ids[i].id_fpl_sec==given->get_involved_section(j).id_fpl_sec &&
				this->fpl_section_ids[i].id_interval==given->get_involved_section(j).id_interval){
				is_same=true;
				break;
			}
		}
		if(is_same==false){
			new_id<<"_"<<this->fpl_section_ids[i].id_fpl_sec<<"-"<<this->fpl_section_ids[i].id_interval;
		}
	}

    this->set_break_scenario_str(this->get_hydraulic_boundary_scenario(), new_id.str());
}
//Reduce the involved section of a break scenario by the given section id
void Risk_Break_Scenario::reduce_involved_section(const int given){
	ostringstream new_id;
	new_id<<"CA";
	for(int i=0; i< this->number_section; i++){
		if(this->fpl_section_ids[i].id_fpl_sec!=given){
			new_id<<"_"<<this->fpl_section_ids[i].id_fpl_sec<<"-"<<this->fpl_section_ids[i].id_interval;
		}
	}
	this->set_break_scenario_str(this->get_hydraulic_boundary_scenario(), new_id.str());
}
//Get the number of the maximal interval number
int Risk_Break_Scenario::get_max_interval(void){
	stringstream buffer;
	int interval=0;
	int max_interval=0;
	for(int i=0; i< this->number_section; i++){
		buffer.str("");
		buffer << this->fpl_section_ids[i].id_interval;
		buffer >> interval;
		if(interval>max_interval){
			max_interval=interval;
		}
	}
	
	return max_interval;
}	
//Reduce all interval by the given integer; smallest value is 0
void Risk_Break_Scenario::reduce_interval(const int reduce){
	stringstream buffer;
	int interval=0;
	for(int i=0; i< this->number_section; i++){
		buffer.str("");
		buffer << this->fpl_section_ids[i].id_interval;
		buffer >> interval;
		interval=interval-reduce;
		if(interval<0){
			interval=0;
		}
		buffer.str("");
		buffer.clear();
		buffer << interval;
		this->fpl_section_ids[i].id_interval=buffer.str();
	}

	//make new string
	buffer.str("");
	buffer.clear();
	buffer << "CA";

	for(int i=0; i< this->number_section; i++){
		buffer<<"_"<<this->fpl_section_ids[i].id_fpl_sec;
		buffer<<"-"<<this->fpl_section_ids[i].id_interval;

	}

	this->break_sc_id=buffer.str();

}
//Set the predicted risk data from database for the break scenario
void Risk_Break_Scenario::set_predicted_risk_data(QSqlDatabase *ptr_database){



	QSqlQueryModel model;

	int number=0;

	try{
		number=Risk_Damage_Predictor::select_predicted_risk_result(&model, ptr_database, this->system_id, this->get_hydraulic_boundary_scenario().get_id(), this->get_break_scenario_string());

	}
	catch(Error msg){
		throw msg;
	}


	if(number<=0){
		this->delete_predicted_risk_result();
	}
	else{
		this->delete_predicted_risk_result();
		this->allocate_predicted_risk_result();
		this->ptr_predicted_risk->ecn_risk=model.record(0).value((Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_total_ecn_predict)).c_str()).toDouble();
		this->ptr_predicted_risk->ecn_risk_perc=model.record(0).value((Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_total_ecn_predict_perc)).c_str()).toDouble();
		this->ptr_predicted_risk->eco_risk=model.record(0).value((Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_total_eco_predict)).c_str()).toDouble();
		this->ptr_predicted_risk->eco_risk_perc=model.record(0).value((Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_total_eco_predict_perc)).c_str()).toDouble();
		
		this->ptr_predicted_risk->pop_affected_risk=model.record(0).value((Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
		this->ptr_predicted_risk->pop_affected_risk_perc=model.record(0).value((Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_pop_affected_perc)).c_str()).toDouble();

		this->ptr_predicted_risk->pop_endangered_risk=model.record(0).value((Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
		this->ptr_predicted_risk->pop_endangered_risk_perc=model.record(0).value((Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_pop_endangered_perc)).c_str()).toDouble();

		this->ptr_predicted_risk->pys_without_density_risk=model.record(0).value((Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(dam_label::total_score)).c_str()).toDouble();
		this->ptr_predicted_risk->pys_without_density_risk_perc=model.record(0).value((Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_total_score_perc)).c_str()).toDouble();

		this->ptr_predicted_risk->pys_with_density_risk=model.record(0).value((Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(dam_label::total_dens_score)).c_str()).toDouble();
		this->ptr_predicted_risk->pys_with_density_risk_perc=model.record(0).value((Risk_Damage_Predictor::table_detailed_predicted_results->get_column_name(risk_label::risk_total_dens_score_perc)).c_str()).toDouble();

	}
}
//Add predicted risk data to a given one
void Risk_Break_Scenario::add_predicted_risk_data(_risk_predicted_risk *addto){
	if(this->ptr_predicted_risk==NULL){
		return;
	}
	else{
		addto->ecn_risk=addto->ecn_risk+this->ptr_predicted_risk->ecn_risk;
		addto->eco_risk=addto->eco_risk+this->ptr_predicted_risk->eco_risk;
		addto->pop_affected_risk=addto->pop_affected_risk+this->ptr_predicted_risk->pop_affected_risk;
		addto->pop_endangered_risk=addto->pop_endangered_risk+this->ptr_predicted_risk->pop_endangered_risk;
		addto->pys_with_density_risk=addto->pys_with_density_risk+this->ptr_predicted_risk->pys_with_density_risk;
		addto->pys_without_density_risk=addto->pys_without_density_risk+this->ptr_predicted_risk->pys_without_density_risk;
	}

}
//Calculate the percentage of the predicted risk data
void Risk_Break_Scenario::calculated_percentage_predicted_risk_data(_risk_predicted_risk *total){

	if(this->ptr_predicted_risk==NULL){
		return;
	}
	else{
		int count=6;
		if(total->ecn_risk!=0.0){
			this->ptr_predicted_risk->ecn_risk_perc=this->ptr_predicted_risk->ecn_risk/total->ecn_risk*100.0;
		}
		else{
			this->ptr_predicted_risk->ecn_risk_perc=0.0;
			count--;
		}
		if(total->eco_risk!=0.0){
			this->ptr_predicted_risk->eco_risk_perc=this->ptr_predicted_risk->eco_risk/total->eco_risk*100.0;
		}
		else{
			this->ptr_predicted_risk->eco_risk_perc=0.0;
			count--;
		}
		if(total->pop_affected_risk!=0.0){
			this->ptr_predicted_risk->pop_affected_risk_perc=this->ptr_predicted_risk->pop_affected_risk/total->pop_affected_risk*100.0;
		}
		else{
			this->ptr_predicted_risk->pop_affected_risk_perc=0.0;
			count--;
		}
		if(total->pop_endangered_risk!=0.0){
			this->ptr_predicted_risk->pop_endangered_risk_perc=this->ptr_predicted_risk->pop_endangered_risk/total->pop_endangered_risk*100.0;
		}
		else{
			this->ptr_predicted_risk->pop_endangered_risk_perc=0.0;
			count--;
		}
		if(total->pys_with_density_risk!=0.0){
			this->ptr_predicted_risk->pys_with_density_risk_perc=this->ptr_predicted_risk->pys_with_density_risk/total->pys_with_density_risk*100.0;
		}
		else{
			this->ptr_predicted_risk->pys_with_density_risk_perc=0.0;
			count--;
		}
		if(total->pys_without_density_risk!=0.0){
			this->ptr_predicted_risk->pys_without_density_risk_perc=this->ptr_predicted_risk->pys_without_density_risk/total->pys_without_density_risk*100.0;
		}
		else{
			this->ptr_predicted_risk->pys_without_density_risk_perc=0.0;
			count--;
		}

		this->ptr_predicted_risk->middle_perc=this->ptr_predicted_risk->ecn_risk_perc+this->ptr_predicted_risk->eco_risk_perc+
			this->ptr_predicted_risk->pop_affected_risk_perc+this->ptr_predicted_risk->pop_endangered_risk_perc+
			this->ptr_predicted_risk->pys_with_density_risk_perc+this->ptr_predicted_risk->pys_without_density_risk_perc;
		this->ptr_predicted_risk->middle_perc=this->ptr_predicted_risk->middle_perc/double(count);
	}
}
//Get the pointer to the predicted risk values, if available
_risk_predicted_risk* Risk_Break_Scenario::get_ptr_predicted_risk_values(void){
	return this->ptr_predicted_risk;
}
//Decide if a fpl-section is involved (as break location) in this break scenario
bool Risk_Break_Scenario::decide_fpl_section_involved(const int sec_id){
	
	for(int i=0; i< this->number_section;i++){
		if(sec_id==this->fpl_section_ids[i].id_fpl_sec){
			return true;
		}
	}

	return false;
}
//copy operator
Risk_Break_Scenario Risk_Break_Scenario::operator= (const Risk_Break_Scenario& copy){
	this->delete_array_section();
    this->delete_predicted_risk_result();
	this->system_id=copy.system_id;
	this->break_sc_id=copy.break_sc_id;
	this->number_section=copy.number_section;
	this->risk_type=copy.risk_type;
	this->id_hydraulic_bound=copy.id_hydraulic_bound;
	this->prob_without_hyd_sc=copy.prob_without_hyd_sc;
	this->is_calculated=copy.is_calculated;
	this->is_selected=copy.is_selected;
	this->hyd_is_calculated=copy.hyd_is_calculated;
	this->source=copy.source;

	this->allocate_array_section();
	for(int i=0; i<this->number_section; i++){
		this->fpl_section_ids[i]=copy.fpl_section_ids[i];
	}

	if(copy.ptr_predicted_risk!=NULL){

		this->allocate_predicted_risk_result();
		*this->ptr_predicted_risk=*copy.ptr_predicted_risk;
	}
    else{
        this->ptr_predicted_risk=NULL;
    }

	return *this;
}
//_________
//protected
//Generate identifier for the break scenario
void Risk_Break_Scenario::generate_break_scenario_string(void){
	ostringstream buffer;
	if(this->risk_type==_risk_type::catchment_risk){
		buffer <<"CA_";
	}
	else if(this->risk_type==_risk_type::scenario_risk){
		buffer <<"SC_";
	}
	for(int i=0; i<this->number_section; i++){
		buffer<<this->fpl_section_ids[i].id_fpl_sec<<"-"<<this->fpl_section_ids[i].id_interval<<"_";
	}
	//cut the last one
	string buff;
	buff=buffer.str();
	buff=buff.substr(0,buff.length()-1);
	this->break_sc_id=buff;

}
//Translate the identifer of the break scenarios to the involved fpl-sections
void Risk_Break_Scenario::translate_string2involved_section(void){
	//count number of "_"
	string buffer;
	string without_prefix;
	buffer=this->break_sc_id;
	this->find_risk_type_idstring(&buffer);
	without_prefix=buffer;
	int pos=0;
	int counter=0;
	do{
		pos=buffer.find("_");
		if(pos>0){
			buffer=buffer.substr(pos+1);
			counter++;
		}
		else{
			break;
		}
	}
	while(buffer.length()>1);

	if(buffer.empty()==false){
		counter++;
	}

	this->delete_array_section();
	this->number_section=counter;
	try{
		this->allocate_array_section();
	}
	catch(Error msg){
		throw msg;
	}

	//set the section numbers
	pos=0;
	counter=0;
	buffer=without_prefix;
	string buffer2=without_prefix;
	
	do{
		pos=buffer.find("_");
		if(pos>0){
			stringstream buffer3;
			string buff3;
			buffer2=buffer.substr(0,pos);
			buffer=buffer.substr(pos+1);
			pos=buffer2.find("-");
			if(pos>0){
				buffer3<<buffer2.substr(0,pos);
				buffer3>>this->fpl_section_ids[counter].id_fpl_sec;
				buffer3.str("");
				buff3=buffer2.substr(pos+1);
				this->fpl_section_ids[counter].id_interval=buff3;
				if(this->risk_type==_risk_type::scenario_risk){
					buffer3>>this->fpl_section_ids[counter].break_height;
				}
	
				counter++;
			}
		}
		else{
			break;
		}
	}
	while(buffer.length()>0);

	if(buffer.empty()==false){
		stringstream buffer3;
		string buff3;
		pos=buffer.find("-");
		if(pos>0){
			buffer3<<buffer.substr(0,pos);
			buffer3>>this->fpl_section_ids[counter].id_fpl_sec;
			buffer3.str("");
			buff3=buffer.substr(pos+1);
			this->fpl_section_ids[counter].id_interval=buff3;
			if(this->risk_type==_risk_type::scenario_risk){
				buffer3>>this->fpl_section_ids[counter].break_height;
			}
			counter++;
		}	
	}

	this->sort_involved_section_id();
	this->generate_break_scenario_string();
}
//Find the risk type out of the id-string
void Risk_Break_Scenario::find_risk_type_idstring(string *buffer){
	int pos=0;
	pos=buffer->find("SC");
	if(pos>=0 &&buffer->length()==2){
		*buffer=buffer->substr(pos+2);
		this->risk_type=_risk_type::scenario_risk;
		return;
	}
	pos=buffer->find("SC_");
	if(pos>=0){
		*buffer=buffer->substr(pos+3);
		this->risk_type=_risk_type::scenario_risk;
		return;
	}
	pos=buffer->find("CA");
	if(pos>=0 && buffer->length()==2){
		*buffer=buffer->substr(pos+2);
		this->risk_type=_risk_type::catchment_risk;
		return;
	}
	pos=buffer->find("CA_");
	if(pos>=0){
		*buffer=buffer->substr(pos+3);
		this->risk_type=_risk_type::catchment_risk;
		return;
	}

	this->risk_type=_risk_type::nobreak_risk;
}
//Allocate array for the involved fpl-sections
void Risk_Break_Scenario::allocate_array_section(void){

	
	try{
		this->fpl_section_ids=new _fpl_break_info[this->number_section];
		Sys_Memory_Count::self()->add_mem(sizeof(_fpl_break_info)*this->number_section, _sys_system_modules::RISK_SYS);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init
	for(int i=0; i< this->number_section; i++){
		this->fpl_section_ids[i].break_height=0.0;
		this->fpl_section_ids[i].id_fpl_sec=-1;
		this->fpl_section_ids[i].id_interval="N";
	}
}
//Delete array for the involved fpl-sections
void Risk_Break_Scenario::delete_array_section(void){
	if(this->fpl_section_ids!=NULL){
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_break_info)*this->number_section, _sys_system_modules::RISK_SYS);
		delete []this->fpl_section_ids;
		this->fpl_section_ids=NULL;
	}
}
//Sort involved section after the section-id
void Risk_Break_Scenario::sort_involved_section_id(void){
	
/**
Bubble-sort algorithm for the sorting the detailed results array after the annuality.
*/
	_fpl_break_info temp;
	int buffer=0;
	for(int i = this->number_section-1; i > 0; i--){
		for (int j = 1; j <= i; j++){
			buffer=this->fpl_section_ids[j].id_fpl_sec;
			
			if (this->fpl_section_ids[j-1].id_fpl_sec > buffer){
				temp = this->fpl_section_ids[j-1];
				this->fpl_section_ids[j-1] = this->fpl_section_ids[j];
				this->fpl_section_ids[j] = temp;
			}
		}
	}
}
//Allocate the predicted risk result structure
void Risk_Break_Scenario::allocate_predicted_risk_result(void){
	try{
		this->ptr_predicted_risk=new _risk_predicted_risk;
		Sys_Memory_Count::self()->add_mem(sizeof(_risk_predicted_risk), _sys_system_modules::RISK_SYS);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	Risk_Damage_Predictor::init_risk_predicted_risk_structure(this->ptr_predicted_risk);
}
//Delete the predicted risk result structure
void Risk_Break_Scenario::delete_predicted_risk_result(void){
	if(this->ptr_predicted_risk!=NULL){
		delete this->ptr_predicted_risk;
		Sys_Memory_Count::self()->minus_mem(sizeof(_risk_predicted_risk), _sys_system_modules::RISK_SYS);
		this->ptr_predicted_risk=NULL;
	}
}
//Set error(s)
Error Risk_Break_Scenario::set_error(const int err_type){
	string place="Risk_Break_Scenario::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_array_section(void)/allocate_array_section2calc(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://no mc result is found
			place.append("calculate_probability(QSqlDatabase *ptr_database)");
			reason="No monte-carlo result is found in database table for the given break scenario";
			help="Check the table of FPL_RELIABILITY in database";
			type=29;
			break;
		case 2://bad alloc
			place.append("compare_scenario_information(Risk_Break_Scenario *given)");
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
