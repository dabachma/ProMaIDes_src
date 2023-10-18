#include "Dam_Headers_Precompiled.h"
//#include "Dam_Impact_Value_Floodplain.h"

//Default constructor
Dam_Impact_Value_Floodplain::Dam_Impact_Value_Floodplain(void){
	this->number_element=0;
	this->index_floodplain=-1;
	this->impact_values=NULL;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Impact_Value_Floodplain), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Impact_Value_Floodplain::~Dam_Impact_Value_Floodplain(void){
	this->delete_impact_values();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Impact_Value_Floodplain), _sys_system_modules::DAM_SYS);
}
//__________
//public
//Set the number of element
void Dam_Impact_Value_Floodplain::set_number_element(const int number){
	this->number_element=number;
}
//Get the number of elements
int Dam_Impact_Value_Floodplain::get_number_element(void){
	return this->number_element;
}
//Set the index of the floodplain
void Dam_Impact_Value_Floodplain::set_floodplain_index(const int index){
	this->index_floodplain=index;
}
//Get the index of the floodplain
int Dam_Impact_Value_Floodplain::get_index_floodplain(void){
	return this->index_floodplain;
}
//Analyse the time-date string (static)
double Dam_Impact_Value_Floodplain::analyse_date_time(const QStringList list) {




	QString buffer;
	buffer = list.at(0);
	
	//Analyse timestep
	double step = 0.0;
	QStringList date_time=buffer.split(" ");
	QString date = date_time.at(0);
	QString time = date_time.at(1);
	QStringList date_l = date.split("-");
	QStringList time_l = time.split(":");

	tm time_struct;

	time_struct.tm_year = date_l.at(0).toInt()-1900;
	time_struct.tm_mon = date_l.at(1).toInt()-1;
	time_struct.tm_mday = date_l.at(2).toInt();

	time_struct.tm_hour = time_l.at(0).toInt();
	time_struct.tm_min = time_l.at(1).toInt();
	time_struct.tm_sec = time_l.at(2).toInt();
	time_struct.tm_wday = 0;
	time_struct.tm_yday = 0;
	time_struct.tm_isdst = 0;

	step= mktime(&time_struct);
	return step;
}
//Set the impact values from database
void Dam_Impact_Value_Floodplain::set_impact_values_database(const int bound_sz, const string break_sz, QSqlDatabase *ptr_database, const _sys_system_id sys_id){
	QSqlQueryModel results;
	int number_found_elem=0;

	try{
		number_found_elem=Hyd_Element_Floodplain::count_relevant_results_elements_database(&results, ptr_database, sys_id, this->index_floodplain, bound_sz, break_sz, false);
		
	}
	catch(Error msg){
		throw msg;
	}
    ostringstream cout;
    cout <<number_found_elem<< " hydraulic impact values are found" << endl ;
    Sys_Common_Output::output_dam->output_txt(&cout);
	this->allocate_impact_values();
	//set the impact values
	int elem_id=0;
	int counter=0;
	int counter2=0;
	for(int i=0; i< number_found_elem; i++){
		if(i==counter*constant::max_rows){
			results.clear();
			Hyd_Element_Floodplain::select_relevant_results_elements_database(&results, ptr_database, sys_id, this->index_floodplain, bound_sz, break_sz, i, constant::max_rows, false);
			counter++;
			counter2=0;
		}
		elem_id=results.record(counter2).value((Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id)).c_str()).toInt();
		this->impact_values[elem_id].readout_data_from_database_model(&results, counter2);
		counter2++;
	}


	results.clear();
	ptr_database->close();
	ptr_database->open();
}
//Set the instationary impact values from database
void Dam_Impact_Value_Floodplain::set_instationary_impact_values_database(const int bound_sz, const string break_sz, QSqlDatabase *ptr_database, const _sys_system_id sys_id, const string time_date) {
	QSqlQueryModel results;
	int number_found_elem = 0;

	try {
		number_found_elem = Hyd_Element_Floodplain::count_relevant_instat_results_elements_database(&results, ptr_database, sys_id, this->index_floodplain, bound_sz, break_sz, time_date, false);

	}
	catch (Error msg) {
		throw msg;
	}
	//ostringstream cout;
	//cout << number_found_elem << " hydraulic instationary impact values are found" << endl;
	//Sys_Common_Output::output_dam->output_txt(&cout);
	this->allocate_impact_values();
	//set the impact values
	int elem_id = 0;
	int counter = 0;
	int counter2 = 0;
	for (int i = 0; i < number_found_elem; i++) {
		if (i == counter * constant::max_rows) {
			results.clear();
			Hyd_Element_Floodplain::select_relevant_instat_results_elements_database(&results, ptr_database, sys_id, this->index_floodplain, bound_sz, break_sz, i, constant::max_rows, time_date, false);
			counter++;
			counter2 = 0;
		}
		elem_id = results.record(counter2).value((Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_id)).c_str()).toInt();
		this->impact_values[elem_id].readout_instat_data_from_database_model(&results, counter2);
		counter2++;
	}


	results.clear();
	ptr_database->close();
	ptr_database->open();

}
//__________
//private
//Allocate the impact values for the elements 
void Dam_Impact_Value_Floodplain::allocate_impact_values(void){
	this->delete_impact_values();
	try{
		this->impact_values=new Dam_Impact_Values[this->number_element];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the impact values for the elements
void Dam_Impact_Value_Floodplain::delete_impact_values(void){
	if(this->impact_values!=NULL){
		delete []this->impact_values;
		this->impact_values=NULL;
	}
}
//Set error(s)
Error Dam_Impact_Value_Floodplain::set_error(const int err_type){
	string place="Dam_Impact_Value_Floodplain::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_impact_values(void)");
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
