#include "Madm_Headers_Precompiled.h"
//#include "Madm_Solver_Saw.h"


//Default constructor
Madm_Solver_Saw::Madm_Solver_Saw(void){
	this->norm_type=_madm_normalisation_type::norm_standard;
	this->solver_type=_madm_approach_type::approach_saw_standard;
	Sys_Memory_Count::self()->add_mem(sizeof(Madm_Solver_Saw),_sys_system_modules::MADM_SYS);//count the memory
}
//Default destructor
Madm_Solver_Saw::~Madm_Solver_Saw(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Madm_Solver_Saw),_sys_system_modules::MADM_SYS);//count the memory
}
//_______
//public
//Set normalisation type
void Madm_Solver_Saw::set_normalisation_type(const _madm_normalisation_type type){
	this->norm_type=type;
	if(this->norm_type==_madm_normalisation_type::norm_standard){
		this->solver_type=_madm_approach_type::approach_saw_standard;
	}
	else if(this->norm_type==_madm_normalisation_type::norm_range){
		this->solver_type=_madm_approach_type::approach_saw_range;
	}
	else{
		this->solver_type=_madm_approach_type::approach_saw_standard;
		this->norm_type=_madm_normalisation_type::norm_standard;
	}
}
//Solve the matrix
void Madm_Solver_Saw::solve_matrix(void){

	//solve the matrix	
	double sum_alt=0.0;
	for(int i=0; i< this->decision.get_number_alternatives(); i++){
		sum_alt=0.0;
		for(int j=0; j < this->decision.get_number_criteria(); j++){
			sum_alt=sum_alt+this->decision.get_matrix_value(i,j);
		}
		this->ranking[i].score=sum_alt;
	}

	//rank the alternatives
	this->sort_alternatives(this->number_rank, this->ranking, true);
}
//_______
//private
//Set warning(s)
Warning Madm_Solver_Saw::set_warning(const int warn_type){
	string place="Madm_Solver_Saw::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://the type of thread is not specified 
			place.append("set_normalisation_type(const _madm_normalisation_type type)") ;
			reason="Normalisation scheme is not applicable for the SAW-analysis";
			reaction="The standard normalisation scheme is applied";
			help= "Check the normalisation scheme";
			type=24;	
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