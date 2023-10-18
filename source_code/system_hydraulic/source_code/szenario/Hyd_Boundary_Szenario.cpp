#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Boundary_Szenario.h"

//Default constructor
Hyd_Boundary_Szenario::Hyd_Boundary_Szenario(void){
	this->id_sz=1;
	this->name=label::not_set;
	this->annuality=1;
	this->prob_annuality=1.0/(double)this->annuality;
	this->prob_event=1.0;
	
	this->is_selected=false;
	
	this->dam_was_output=false;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Boundary_Szenario), _sys_system_modules::HYD_SYS);

}
//Copy constructor
Hyd_Boundary_Szenario::Hyd_Boundary_Szenario(Hyd_Boundary_Szenario &copy){
	this->system_id=copy.system_id;
	this->id_sz=copy.id_sz;
	this->name=copy.name;
	this->annuality=copy.annuality;
	this->prob_annuality=copy.prob_annuality;
	this->prob_event=copy.prob_event;
	this->description=copy.description;
	this->is_selected=copy.is_selected;
	this->dam_was_output=copy.dam_was_output;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Boundary_Szenario), _sys_system_modules::HYD_SYS);
}
//Default destructor
Hyd_Boundary_Szenario::~Hyd_Boundary_Szenario(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Boundary_Szenario), _sys_system_modules::HYD_SYS);

}
//_______________________
//public
//Set the id, name and the probabilities
void Hyd_Boundary_Szenario::set_members(const int id, const double annuality, const double prob_occur, const string name){
	this->id_sz=id;
	this->name=name;
	this->annuality=annuality;
	this->prob_annuality=1.0/(double)this->annuality;
	this->prob_event=prob_occur;
	this->is_selected=false;
}
//Set the members per pointer to this class
void Hyd_Boundary_Szenario::set_members(Hyd_Boundary_Szenario *ptr){
	this->system_id=ptr->system_id;
	this->id_sz=ptr->id_sz;
	this->name=ptr->name;
	this->annuality=ptr->annuality;
	this->prob_annuality=1.0/(double)this->annuality;
	this->prob_event=ptr->prob_event;
	this->description=ptr->description;

}
//Set the id of the hydraulic boundary scenario
void Hyd_Boundary_Szenario::set_id(const int id){
	this->id_sz=id;
}
//Get the id of the szenario
int Hyd_Boundary_Szenario::get_id(void){
	return this->id_sz;
}
//Get probability per year
double Hyd_Boundary_Szenario::get_prob_year(void){
	return this->prob_annuality;
}
//Set probability per year
void Hyd_Boundary_Szenario::set_prob_year(const double prob){
	this->prob_annuality=prob;
}
//Get total occurence probability
double Hyd_Boundary_Szenario::get_prob_total(void){
	return this->prob_annuality*this->prob_event;
}
//Get the occurence probability
double Hyd_Boundary_Szenario::get_prob_occurence(void){
	return this->prob_event;
}
//Get the annuality
int Hyd_Boundary_Szenario::get_annuality(void){
	return this->annuality;
}
//Get the name
string Hyd_Boundary_Szenario::get_name(void){
	return this->name;
}
//Get the description
string Hyd_Boundary_Szenario::get_description(void){
	return this->description;
}
//Get if the scnearios is selected for handling
bool Hyd_Boundary_Szenario::get_is_selected(void){
	return this->is_selected;
}
//Set the flag for handling this scenario
void Hyd_Boundary_Szenario::set_is_selected(const bool flag){
	this->is_selected=flag;
}
//Get a text for a item of a list widget
string Hyd_Boundary_Szenario::get_sz_text(void){
	ostringstream txt;
	txt << this->id_sz << "   " << this->name << "    " << this->annuality << "   " << this->prob_event ;

	return txt.str();
}
//Set new name
void Hyd_Boundary_Szenario::set_name(const string name){
	this->name=name;
}
//Set new description
void Hyd_Boundary_Szenario::set_description(const string descript){
	this->description=descript;
}
//Update the probability of the max-event in database table
void Hyd_Boundary_Szenario::update_max_event_prob_database(QSqlDatabase *ptr_database){

	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Boundary_Szenario_Management::table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_prob_max) << " = "<< this->prob_annuality;
	query_string  << " WHERE ";
	query_string  << Hyd_Boundary_Szenario_Management::table->get_column_name(label::areastate_id) << " = "  << this->system_id.area_state;
	/*query_string  << " AND ";
	query_string  <<  Hyd_Boundary_Szenario_Management::table->get_column_name(label::measure_id) << " = " << this->system_id.area_state ;*/
	query_string  << " AND ";
	query_string  <<  Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id) << " = " << this->id_sz ;
	query_string  << " AND ";
	query_string  <<  Hyd_Boundary_Szenario_Management::table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(true); 



	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Table Name      : " << Hyd_Boundary_Szenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Get a pointer to the damage was output flag
bool* Hyd_Boundary_Szenario::get_ptr_dam_was_output(void){
	return &this->dam_was_output;
}
//copy operator
Hyd_Boundary_Szenario& Hyd_Boundary_Szenario::operator= (const Hyd_Boundary_Szenario& copy){
	this->system_id=copy.system_id;
	this->id_sz=copy.id_sz;
	this->name=copy.name;
	this->annuality=copy.annuality;
	this->prob_annuality=copy.prob_annuality;
	this->prob_event=copy.prob_event;
	//this->is_calculated=copy.is_calculated;
	this->description=copy.description;
	this->is_selected=copy.is_selected;
	this->dam_was_output=copy.dam_was_output;
	return *this;
}
//________________
//private
//set the error
Error Hyd_Boundary_Szenario::set_error(const int err_type){
		string place="Hyd_Boundary_Szenario::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){

		case 0://bad alloc
			place.append("update_max_event_prob_database(QSqlDatabase *ptr_database)");
			reason="Invalid database request";
			help="Check the database";
			type=2;
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