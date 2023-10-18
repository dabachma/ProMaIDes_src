#include "Madm_Headers_Precompiled.h"
//#include "Madm_Solver_Topsis.h"


//Default constructor
Madm_Solver_Topsis::Madm_Solver_Topsis(void){
	this->solver_type=_madm_approach_type::approach_topsis;
	this->norm_type=_madm_normalisation_type::norm_vector;
	this->ideal_best=NULL;
	this->ideal_worst=NULL,
	this->distance2ideal_best=NULL;
	this->distance2ideal_worst=NULL;
	Sys_Memory_Count::self()->add_mem(sizeof(Madm_Solver_Topsis),_sys_system_modules::MADM_SYS);//count the memory
}
//Default destructor
Madm_Solver_Topsis::~Madm_Solver_Topsis(void){
	this->delete_ideal_solution();
	this->delete_dist2ideal();
	Sys_Memory_Count::self()->minus_mem(sizeof(Madm_Solver_Topsis),_sys_system_modules::MADM_SYS);//count the memory
}
//_______
//public
//Set the decision matrix
void Madm_Solver_Topsis::set_decision_matrix(Madm_Decision_Matrix *matrix){
	this->delete_ideal_solution();
	this->delete_dist2ideal();
	_Madm_Solver::set_decision_matrix(matrix);

	this->allocate_dist2ideal();
	this->allocate_ideal_solution();

}
//Solve the matrix
void Madm_Solver_Topsis::solve_matrix(void){


	//calculate the ideal solution (best/worst) per criteria
	this->init_ideal_solution();

	//calculate the distances of each alternative to the ideal solutions per alternative
	this->init_dist2ideal_solution();

	//solve the matrix
	for(int i=0; i<this->decision.get_number_alternatives();i++){
		if(this->distance2ideal_best[i]+this->distance2ideal_worst[i]!=0.0){
			this->ranking[i].score=this->distance2ideal_worst[i]/(this->distance2ideal_best[i]+this->distance2ideal_worst[i]);
		}
		else{
			this->ranking[i].score=0.0;
		}
	}

	//rank the alternatives
	this->sort_alternatives(this->number_rank, this->ranking, true);
}
//______
//private
//Allocate the ideal solutions (best case/worst case) per criteria
void Madm_Solver_Topsis::allocate_ideal_solution(void){
	try{
		this->ideal_best=new double[this->decision.get_number_criteria()];
		this->ideal_worst=new double[this->decision.get_number_criteria()];
		Sys_Memory_Count::self()->add_mem(sizeof(double)*this->decision.get_number_criteria()*2,_sys_system_modules::MADM_SYS);//count the memory
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init
	ostringstream buff;
	for(int i=0; i< this->decision.get_number_criteria(); i++){
		this->ideal_best[i]=0.0;
		this->ideal_worst[i]=0.0;
	}
}
//Delete the ideal solutions
void Madm_Solver_Topsis::delete_ideal_solution(void){
	if(this->ideal_best!=NULL){
		delete []this->ideal_best;
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->decision.get_number_criteria(),_sys_system_modules::MADM_SYS);//count the memory
		this->ideal_best=NULL;
	}
	if(this->ideal_worst!=NULL){
		delete []this->ideal_worst;
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->decision.get_number_criteria(),_sys_system_modules::MADM_SYS);//count the memory
		this->ideal_worst=NULL;
	}
}
//Allocate the distances of the alternatives to the ideal solution
void Madm_Solver_Topsis::allocate_dist2ideal(void){
	try{
		this->distance2ideal_best=new double[this->decision.get_number_alternatives()];
		this->distance2ideal_worst=new double[this->decision.get_number_alternatives()];
		Sys_Memory_Count::self()->add_mem(sizeof(double)*this->decision.get_number_alternatives()*2,_sys_system_modules::MADM_SYS);//count the memory
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init
	ostringstream buff;
	for(int i=0; i< this->decision.get_number_alternatives(); i++){
		this->distance2ideal_best[i]=0.0;
		this->distance2ideal_worst[i]=0.0;
	}
}
//delete the distances of the alternatives to the ideal solution
void Madm_Solver_Topsis::delete_dist2ideal(void){
	if(this->distance2ideal_best!=NULL){
		delete []this->distance2ideal_best;
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->decision.get_number_alternatives(),_sys_system_modules::MADM_SYS);//count the memory
		this->distance2ideal_best=NULL;
	}
	if(this->distance2ideal_worst!=NULL){
		delete []this->distance2ideal_worst;
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->decision.get_number_alternatives(),_sys_system_modules::MADM_SYS);//count the memory
		this->distance2ideal_worst=NULL;
	}
}
//Initialize and calculate the ideal solutions per criteria
void Madm_Solver_Topsis::init_ideal_solution(void){
	//positive - negative ideal solution
	double buff_value=0.0;
	for(int i=0; i<this->decision.get_number_criteria(); i++){
		//init ideal values
		this->ideal_best[i]=this->decision.get_matrix_value(0,i);
		this->ideal_worst[i]=this->decision.get_matrix_value(0,i);
		for(int j=0; j<this->decision.get_number_alternatives(); j++){
			buff_value=this->decision.get_matrix_value(j,i);
			if(this->decision.get_criteria(i).max_flag==true){
				//ídeal best
				if(this->ideal_best[i]<buff_value){
					this->ideal_best[i]=buff_value;
				}
				//ideal worst
				if(this->ideal_worst[i]>buff_value){
					this->ideal_worst[i]=buff_value;
				}
			}else{
				//ídeal best
				if(this->ideal_best[i]>buff_value){
					this->ideal_best[i]=buff_value;
				}
				//ideal worst
				if(this->ideal_worst[i]<buff_value){
					this->ideal_worst[i]=buff_value;
				}
			}
		}
	}
}
//Initialize and calculate the distance of the alternatives to the ideal solutions per alternative
void Madm_Solver_Topsis::init_dist2ideal_solution(void){
	for (int i=0; i<this->decision.get_number_alternatives(); i++){
		for (int j=0; j<this->decision.get_number_criteria(); j++){
			this->distance2ideal_best[i]=this->distance2ideal_best[i]+pow((this->decision.get_matrix_value(i,j)-this->ideal_best[j]),2);
			this->distance2ideal_worst[i]=this->distance2ideal_worst[i]+pow((this->decision.get_matrix_value(i,j)-this->ideal_worst[j]),2);
		}
		this->distance2ideal_best[i]=pow(this->distance2ideal_best[i],0.5);
		this->distance2ideal_worst[i]=pow(this->distance2ideal_worst[i],0.5);
	}
}
//Set error(s)
Error Madm_Solver_Topsis::set_error(const int err_type){
	string place="Madm_Solver_Topsis::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_ideal_solution(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("allocate_dist2ideal(void)");
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