#include "Madm_Headers_Precompiled.h"
//#include "Madm_Solver_Electre.h"


//Default constructor
Madm_Solver_Electre::Madm_Solver_Electre(void){
	this->solver_type=_madm_approach_type::approach_electre;
	this->norm_type=_madm_normalisation_type::norm_vector;

	this->concordance_matrix=NULL;
	this->discordance_matrix=NULL;
	this->concordance_index=NULL;
	this->discordance_index=NULL;
	this->diff_norm_attr=NULL;
	this->net_concordance_indices=NULL;
	this->net_discordance_indices=NULL;

	this->concordance_ranking=NULL;
	this->discordance_ranking=NULL;



	Sys_Memory_Count::self()->add_mem(sizeof(Madm_Solver_Electre),_sys_system_modules::MADM_SYS);//count the memory
}
//Default destructor
Madm_Solver_Electre::~Madm_Solver_Electre(void){

	this->delete_dis_concordance_matrices();
	this->delete_dis_concordance_indices();
	this->delete_net_dis_concordance_indices();

	this->delete_ranking_array();

	Sys_Memory_Count::self()->minus_mem(sizeof(Madm_Solver_Electre),_sys_system_modules::MADM_SYS);//count the memory
}
//_______
//public
//Set the decision matrix
void Madm_Solver_Electre::set_decision_matrix(Madm_Decision_Matrix *matrix){
	this->delete_dis_concordance_matrices();
	this->delete_dis_concordance_indices();
	this->delete_net_dis_concordance_indices();
	_Madm_Solver::set_decision_matrix(matrix);

	this->allocate_dis_concordance_matrices();
	this->allocate_dis_concordance_indices();
	this->allocate_net_dis_concordance_indices();
}
//Solve the matrix
void Madm_Solver_Electre::solve_matrix(void){
	
	//init and calculate the matrices
	this->init_dis_concordance_matrices();
	//init and calculate the indices
	this->init_dis_concordance_indices();
	//init and calculate the net indices
	this->init_net_dis_concordance_indices();



	//rank the alternatives
	this->calc_final_ranking();
}
//_______
//private
//Allocate the dis-/concordance matrices
void Madm_Solver_Electre::allocate_dis_concordance_matrices(void){
	try{
		this->concordance_matrix=new int*[this->decision.get_number_alternatives()*this->decision.get_number_alternatives()];
		this->discordance_matrix=new int*[this->decision.get_number_alternatives()*this->decision.get_number_alternatives()];

		for(int i=0;i<this->decision.get_number_alternatives()*this->decision.get_number_alternatives(); i++){
			this->concordance_matrix[i]=NULL;
			this->discordance_matrix[i]=NULL;
			this->concordance_matrix[i]=new int[this->decision.get_number_criteria()];
			this->discordance_matrix[i]=new int[this->decision.get_number_criteria()];
		}
		Sys_Memory_Count::self()->add_mem(sizeof(int)*this->decision.get_number_criteria()*this->decision.get_number_alternatives()*this->decision.get_number_alternatives()*2,_sys_system_modules::MADM_SYS);//count the memory

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//init matrices
	for(int i=0; i< this->decision.get_number_alternatives()*this->decision.get_number_alternatives(); i++){
		for(int j=0; j<this->decision.get_number_criteria(); j++){
			this->concordance_matrix[i][j]=0;
			this->discordance_matrix[i][j]=0;
		}
	}
}
//Delete the dis-/concordance matrices
void Madm_Solver_Electre::delete_dis_concordance_matrices(void){
	//concordance matrix
	if(this->concordance_matrix!=NULL){
		for(int i=0; i< this->decision.get_number_alternatives()*this->decision.get_number_alternatives(); i++){
			if(this->concordance_matrix[i]!=NULL){
				delete []this->concordance_matrix[i];
			}
		}
		delete []this->concordance_matrix;
		Sys_Memory_Count::self()->minus_mem(sizeof(int)*this->decision.get_number_alternatives()*this->decision.get_number_alternatives()*this->decision.get_number_criteria(),_sys_system_modules::MADM_SYS);//count the memory
		this->concordance_matrix=NULL;
	}
	//disconcordance matrix
	if(this->discordance_matrix!=NULL){
		for(int i=0; i< this->decision.get_number_alternatives()*this->decision.get_number_alternatives(); i++){
			if(this->discordance_matrix[i]!=NULL){
				delete []this->discordance_matrix[i];
			}
		}
		delete []this->discordance_matrix;
		Sys_Memory_Count::self()->minus_mem(sizeof(int)*this->decision.get_number_alternatives()*this->decision.get_number_alternatives()*this->decision.get_number_criteria(),_sys_system_modules::MADM_SYS);//count the memory
		this->discordance_matrix=NULL;
	}
}
//Allocate the dis-/concordance indices and the distances of the attribute values
void Madm_Solver_Electre::allocate_dis_concordance_indices(void){
	try{
		this->concordance_index=new double[this->decision.get_number_alternatives()*this->decision.get_number_alternatives()];
		this->discordance_index=new double[this->decision.get_number_alternatives()*this->decision.get_number_alternatives()];
		this->diff_norm_attr=new double[this->decision.get_number_alternatives()*this->decision.get_number_alternatives()];
		Sys_Memory_Count::self()->add_mem(sizeof(double)*this->decision.get_number_alternatives()*this->decision.get_number_alternatives()*3,_sys_system_modules::MADM_SYS);//count the memory
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
	for(int i=0; i< this->decision.get_number_alternatives()*this->decision.get_number_alternatives(); i++){
		this->concordance_index[i]=0.0;
		this->discordance_index[i]=0.0;
		this->diff_norm_attr[i]=0.0;
	}
}
//Delete the dis-/concordance indices and the distances of the attribute values
void Madm_Solver_Electre::delete_dis_concordance_indices(void){

	if(this->concordance_index!=NULL){
		delete []this->concordance_index;
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->decision.get_number_alternatives()*this->decision.get_number_alternatives(),_sys_system_modules::MADM_SYS);//count the memory
		this->concordance_index=NULL;
	}
	if(this->discordance_index!=NULL){
		delete []this->discordance_index;
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->decision.get_number_alternatives()*this->decision.get_number_alternatives(),_sys_system_modules::MADM_SYS);//count the memory
		this->discordance_index=NULL;
	}
	if(this->diff_norm_attr!=NULL){
		delete []this->diff_norm_attr;
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->decision.get_number_alternatives()*this->decision.get_number_alternatives(),_sys_system_modules::MADM_SYS);//count the memory
		this->diff_norm_attr=NULL;
	}
}
//Allocate the net dis-/concordance indices per alternative
void Madm_Solver_Electre::allocate_net_dis_concordance_indices(void){
	try{
		this->net_concordance_indices=new double[this->decision.get_number_alternatives()];
		this->net_discordance_indices=new double[this->decision.get_number_alternatives()];
		Sys_Memory_Count::self()->add_mem(sizeof(double)*this->decision.get_number_alternatives()*2,_sys_system_modules::MADM_SYS);//count the memory
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init
	ostringstream buff;
	for(int i=0; i< this->decision.get_number_alternatives(); i++){
		this->net_concordance_indices[i]=0.0;
		this->net_discordance_indices[i]=0.0;
	}
}
//Deletethe net dis-/concordance indices per alternative
void Madm_Solver_Electre::delete_net_dis_concordance_indices(void){
	if(this->net_concordance_indices!=NULL){
		delete []this->net_concordance_indices;
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->decision.get_number_alternatives(),_sys_system_modules::MADM_SYS);//count the memory
		this->net_concordance_indices=NULL;
	}
	if(this->net_discordance_indices!=NULL){
		delete []this->net_discordance_indices;
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->decision.get_number_alternatives(),_sys_system_modules::MADM_SYS);//count the memory
		this->net_discordance_indices=NULL;
	}
}
//Allocate the ranking array
void Madm_Solver_Electre::allocate_ranking(void){
	_Madm_Solver::allocate_ranking();

	try{
		this->concordance_ranking=new _madm_rank_values[this->number_rank];
		this->discordance_ranking=new _madm_rank_values[this->number_rank];
		Sys_Memory_Count::self()->add_mem(sizeof(_madm_rank_values)*this->number_rank*2,_sys_system_modules::MADM_SYS);//count the memory
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(3);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init
	ostringstream buff;
	for(int i=0; i< this->number_rank; i++){
		this->concordance_ranking[i].score=0.0;
		this->discordance_ranking[i].score=0.0;
		this->concordance_ranking[i].rank=-1;
		this->discordance_ranking[i].rank=-1;
		buff<<"rank_alt_"<<i;
		this->concordance_ranking[i].alternative.name=buff.str();
		this->discordance_ranking[i].alternative.name=buff.str();
		this->concordance_ranking[i].alternative.index=i;
		this->discordance_ranking[i].alternative.index=i;
		buff.str("");
	}

}
//Delete the ranking array
void Madm_Solver_Electre::delete_ranking_array(void){
	_Madm_Solver::delete_ranking_array();

	if(this->concordance_ranking!=NULL){
		delete []this->concordance_ranking;
		Sys_Memory_Count::self()->minus_mem(sizeof(_madm_rank_values)*this->number_rank,_sys_system_modules::MADM_SYS);//count the memory
		this->concordance_ranking=NULL;
	}
	if(this->discordance_ranking!=NULL){
		delete []this->discordance_ranking;
		Sys_Memory_Count::self()->minus_mem(sizeof(_madm_rank_values)*this->number_rank,_sys_system_modules::MADM_SYS);//count the memory
		this->discordance_ranking=NULL;
	}

}
//Initialize and calculate the dis-/concordance matrices
void Madm_Solver_Electre::init_dis_concordance_matrices(void){
/** 1 for as concordance values is set, if the matrix-value of alternative k is greater than the matrix value of
alternative i. Than as discordance value a 0 is applied. At the diagonal there is always a 0-value.
*/
	for (int k=0;k<this->decision.get_number_alternatives(); k++){
		for (int i=0; i<this->decision.get_number_alternatives(); i++){
			for(int j=0; j<this->decision.get_number_criteria(); j++){
			//ELECTRE convention is to use "The bigger the better":
				if(k!=i){
					//decide if attribute max or min is optimal
					if (this->decision.get_criteria(j).max_flag==true){
						if(this->decision.get_matrix_value(k,j)>this->decision.get_matrix_value(i,j)){
							this->concordance_matrix[this->decision.get_number_alternatives()*k+i][j]=1;
							this->discordance_matrix[this->decision.get_number_alternatives()*k+i][j]=0;
						}
						else if(this->decision.get_matrix_value(k,j)<this->decision.get_matrix_value(i,j)){
							this->concordance_matrix[this->decision.get_number_alternatives()*k+i][j]=0;
							this->discordance_matrix[this->decision.get_number_alternatives()*k+i][j]=1;
						}
						else{
							this->concordance_matrix[this->decision.get_number_alternatives()*k+i][j]=0;
							this->discordance_matrix[this->decision.get_number_alternatives()*k+i][j]=0;
						}
					}
					else if(this->decision.get_criteria(j).max_flag==false){
						if(this->decision.get_matrix_value(k,j)>this->decision.get_matrix_value(i,j)){
							this->concordance_matrix[this->decision.get_number_alternatives()*k+i][j]=0;
							this->discordance_matrix[this->decision.get_number_alternatives()*k+i][j]=1;
						}
						else if(this->decision.get_matrix_value(k,j)<this->decision.get_matrix_value(i,j)){
							this->concordance_matrix[this->decision.get_number_alternatives()*k+i][j]=1;
							this->discordance_matrix[this->decision.get_number_alternatives()*k+i][j]=0;
						}
						else{
							this->concordance_matrix[this->decision.get_number_alternatives()*k+i][j]=0;
							this->discordance_matrix[this->decision.get_number_alternatives()*k+i][j]=0;
						}
					}
				}
				//diagonal value
				else{
					this->concordance_matrix[this->decision.get_number_alternatives()*k+i][j]=0;
					this->discordance_matrix[this->decision.get_number_alternatives()*k+i][j]=0;
				}
			}
		}
	}
}
//Initialize and calculate the dis-/concordance indices
void Madm_Solver_Electre::init_dis_concordance_indices(void){
	//compute concordance index 
	for(int i=0; i< this->decision.get_number_alternatives()*this->decision.get_number_alternatives(); i++){
		for(int j=0; j<this->decision.get_number_criteria();j++){
			this->concordance_index[i]=this->concordance_index[i]+((double)this->concordance_matrix[i][j]*this->decision.get_criteria(j).weight);
		}
	}
	//compute discordance index 
	int counter=0;
	for (int k=0;k<this->decision.get_number_alternatives(); k++){
		for (int i=0; i<this->decision.get_number_alternatives(); i++){
			if(k!=i){
				//calculate the distance
				for(int j=0; j<this->decision.get_number_criteria(); j++){
					this->diff_norm_attr[counter]=this->diff_norm_attr[counter]+fabs(this->decision.get_matrix_value(k,j)-this->decision.get_matrix_value(i,j));
				}
				for(int j=0; j<this->decision.get_number_criteria(); j++){
					this->discordance_index[counter]=this->discordance_index[counter]+fabs((this->decision.get_matrix_value(k,j)-decision.get_matrix_value(i,j))*this->discordance_matrix[counter][j]);
				}
				if(this->discordance_index[counter]!=0.0){
					
					this->discordance_index[counter]=discordance_index[counter]/this->diff_norm_attr[counter];
				}
			}
			counter++;
		}
	}
	if(counter !=this->decision.get_number_alternatives()*this->decision.get_number_alternatives()){
		Warning msg=this->set_warning(0);
		msg.output_msg(3);
	}
}
//Initialize and calculate the net dis-/concordance indices per alternative
void Madm_Solver_Electre::init_net_dis_concordance_indices(void){
	//calculate concordance net indices
	//sum of concordance of alternativ A over all other alternatives minus the sum of all other alternatives over A, so a negativ value means A is beaten):
	double sum1=0.0;
	double sum2=0.0;
	
	for (int i=0; i<this->decision.get_number_alternatives(); i++){
		sum1=0.0;
		sum2=0.0;
		for(int j=0; j<this->decision.get_number_alternatives();j++){
			sum1=sum1+this->concordance_index[i*this->decision.get_number_alternatives()+j];
			sum2=sum2+this->concordance_index[i+this->decision.get_number_alternatives()*j];
		}
		this->net_concordance_indices[i]=sum1-sum2;
	}
	//calculate discordance net indices
	//sum of discordance of alternativ A over all other alternatives minus the sum of all other alternatives over A, so a positiv value means A is beaten):
	for (int i=0; i<this->decision.get_number_alternatives(); i++){
		sum1=0.0;
		sum2=0.0;
		for(int j=0; j<this->decision.get_number_alternatives();j++){
			sum1=sum1+this->discordance_index[i*this->decision.get_number_alternatives()+j];
			sum2=sum2+this->discordance_index[i+this->decision.get_number_alternatives()*j];
		}
		this->net_discordance_indices[i]=sum1-sum2;
	}
}
//Calculate final ranking
void Madm_Solver_Electre::calc_final_ranking(void){

	//set the alternatives
	for(int i=0; i< this->number_rank;i++){
		this->concordance_ranking[i].alternative=this->decision.get_alternative(i);
		this->discordance_ranking[i].alternative=this->decision.get_alternative(i);
	}

	//concordance ranking
	for (int i=0; i<this->decision.get_number_alternatives();i++){
		this->concordance_ranking[i].score=this->net_concordance_indices[i];
	}
	//sort
	//ostringstream cout;
	this->sort_alternatives(this->number_rank, this->concordance_ranking, true);
	/*cout << "concordance"<<endl;
	for(int i= 0 ; i< this->decision.get_number_alternatives();i++){
		cout << this->concordance_ranking[i].rank << " "<< this->concordance_ranking[i].alternative.name<<" " << this->concordance_ranking[i].score<<endl;

	}*/
	//Sys_Common_Output::output_madm->output_txt(&cout);
	//discordance ranking
	//copy calculated scores into strukture and set pointer for names:
	for (int i=0; i<this->decision.get_number_alternatives();i++){
		this->discordance_ranking[i].score=this->net_discordance_indices[i];
	}
	//sort
	this->sort_alternatives(this->number_rank, this->discordance_ranking, false);
	/*cout << "disconcordance"<<endl;
	for(int i= 0 ; i< this->decision.get_number_alternatives();i++){
		cout << this->discordance_ranking[i].rank << " "<< this->discordance_ranking[i].alternative.name<<" " << this->discordance_ranking[i].score<<endl;

	}*/
	//Sys_Common_Output::output_madm->output_txt(&cout);
	//mean ranking
	//Then get the right score for the right name (synchronize all 3 datasets)
	for(int i= 0 ; i< this->decision.get_number_alternatives();i++){
		for(int j=0; j<this->decision.get_number_alternatives();j++){
			for(int k=0; k<this->decision.get_number_alternatives();k++){
				if(this->concordance_ranking[i].alternative.index==this->discordance_ranking[j].alternative.index &&
					this->discordance_ranking[j].alternative.index==this->ranking[k].alternative.index){
						this->ranking[k].score=(double)(this->concordance_ranking[i].rank + this->discordance_ranking[j].rank)/2.0;
				}
			}			
		}
	}
	//sort
	this->sort_alternatives(this->number_rank, this->ranking, false);

}
//Set error(s)
Error Madm_Solver_Electre::set_error(const int err_type){
	string place="Madm_Solver_Topsis::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_dis_concordance_matrices(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("allocate_dis_concordance_indices(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://bad alloc
			place.append("allocate_net_dis_concordance_indices(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://bad alloc
			place.append("allocate_ranking(void)");
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
Warning Madm_Solver_Electre::set_warning(const int warn_type){
	string place="Madm_Solver_Electre::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://something wronmg with discordance counter 
			place.append("init_dis_concordance_indices(void)") ;
			reason="Something's wrong in discordance index computation";
			help= "Check the source code";
			type=5;	
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