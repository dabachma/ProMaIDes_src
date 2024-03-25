#include "Madm_Headers_Precompiled.h"
//#include "Madm_Analysis.h"



//Default constructor
Madm_Analysis::Madm_Analysis(void){

	this->total_ranking=NULL;
	this->number_total_rank=0;
	this->count_total_ranking=0;

	Sys_Memory_Count::self()->add_mem(sizeof(Madm_Analysis),_sys_system_modules::MADM_SYS);//count the memory
}
//Default destructor
Madm_Analysis::~Madm_Analysis(void){

	this->delete_total_ranking_array();
	Sys_Memory_Count::self()->minus_mem(sizeof(Madm_Analysis),_sys_system_modules::MADM_SYS);//count the memory
}
//_________
//public
//Read in the madm analysis data per file
void Madm_Analysis::read_analysis_per_file(const string filename){

	QDir my_dir;
	//set the path of the outputfile
	this->outfile_name=my_dir.filePath(filename.c_str()).toStdString();
	this->outfile_name=functions::get_file_path(this->outfile_name);
	this->outfile_name.append("/");
	//read the matrix in
	this->matrix.read_matrix_per_file(filename);

	//output the matrix
	this->matrix.output_matrix();

}
//Read in the madm analysis data per database
void Madm_Analysis::read_analysis_per_database(QSqlDatabase *ptr_database, const int number_alt, _alt_general_info *alternatives,  const int number_crit, _madm_criteria *criteria, const _risk_type type){
	ostringstream prefix;
	prefix <<"INP> ";
	Sys_Common_Output::output_madm->set_userprefix(&prefix);
	ostringstream cout;
	cout << "Set the decision matrix per database..."  << endl;
	Sys_Common_Output::output_madm->output_txt(&cout,false);

	this->matrix.init_matrix(number_alt+1, number_crit);

	//set criteria
	for(int i=0; i< number_crit; i++){
		this->matrix.set_criteria(criteria[i], i);
	}
	//set alternatives
	_madm_alternative buffer;
	for(int i=0; i< number_alt; i++){
		buffer.name=alternatives[i].name;
		buffer.index=alternatives[i].id.measure_nr;
		this->matrix.set_alternative(buffer, i);
	}
	//base-state
	buffer.name="base-state";
	buffer.index=0;
	this->matrix.set_alternative(buffer, number_alt);
	
	//read the matrix in
	this->matrix.read_matrix_per_database(ptr_database, alternatives[0].id.area_state, type);

	Sys_Common_Output::output_madm->rewind_userprefix();

	//output the matrix
	this->matrix.output_matrix();
}
//Set the values of the decision matrix into database table
bool Madm_Analysis::set_decision_matrix2database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int number_alt, _alt_general_info *alts){
	ostringstream cout;
	cout <<"Set the values of the decision-matrix for "<<number_alt <<" measures to database table..."<<endl;
	Sys_Common_Output::output_madm->output_txt(&cout);
	return this->matrix.set_matrix_values2database(ptr_database, area_state, type, number_alt, alts);
}
//Change the criteria weights of the matrix
void Madm_Analysis::change_criteria_weight_matrix(const int number_crit, _madm_criteria *criteria){
	for(int i=0; i< number_crit; i++){
		this->matrix.change_weights(i, criteria[i].weight);
	}
}
//Solve the matrix with the diverse madm-approaches for file-based analysis
int Madm_Analysis::calculate_matrix_file(void){


	//prepare the total ranking
	this->allocate_total_ranking();
	this->init_total_ranking();

	int counter_error=0;
	//open output file
	this->outfile_name.append(this->matrix.get_matrix_name());
	this->outfile_name.append("_RESULT.dat");
	this->outfile.open(this->outfile_name.c_str());
	if(this->outfile.is_open()==false){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Filename : "<<this->outfile_name<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	
	//calculation
	try{

		ostringstream cout;
		ostringstream prefix;
		prefix << "CALC"<<"> ";
		Sys_Common_Output::output_madm->set_userprefix(&prefix);
		cout << "Analyse the MADM-matrix "<<this->matrix.get_matrix_name()<< "..."<<endl;
		Sys_Common_Output::output_madm->output_txt(&cout);

		try{
			//saw standard
			prefix << "SAW_STANDARD"<<"> ";
			Sys_Common_Output::output_madm->set_userprefix(&prefix);
			//file output
			this->outfile << "###############################################"<<endl;
			this->outfile << "#  START SAW-STANDARD analysis "<<endl;
			this->outfile << endl;
			this->analysis_saw.set_decision_matrix(&this->matrix);
			cout << "Normalize the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_saw.set_normalisation_type(_madm_normalisation_type::norm_standard);
			this->analysis_saw.apply_matrix_normalisation();
			outfile <<"------NORMALISATION------"<<endl;
			this->analysis_saw.decision.output_matrix(&this->outfile);
			cout << "Apply the weights to the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_saw.apply_weighting();
			outfile <<"------WEIGHTS------"<<endl;
			this->analysis_saw.decision.output_matrix(&this->outfile);
			cout << "Solve the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_saw.solve_matrix();
			this->analysis_saw.output_ranking();
			outfile <<"------RANKING------"<<endl;
			this->analysis_saw.output_ranking(&this->outfile);
			this->outfile << "#  END SAW-STANDARD analysis "<<endl;
			this->outfile << "###############################################"<<endl;
			this->outfile << endl;
			cout <<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			Sys_Common_Output::output_madm->rewind_userprefix();

			this->sum_up_total_ranking(&this->analysis_saw);
		}
		catch(Error msg){
			this->outfile << "#  An error occured during the SAW-STANDARD analysis "<<endl;
			Sys_Common_Output::output_madm->rewind_userprefix();
			msg.output_msg(3);
			counter_error++;
		}
		Madm_System::check_stop_thread_flag();

		try{
			//saw range
			prefix << "SAW_RANGE"<<"> ";
			Sys_Common_Output::output_madm->set_userprefix(&prefix);
			//file output
			this->outfile << "###############################################"<<endl;
			this->outfile << "#  START SAW-RANGE analysis "<<endl;
			this->outfile << endl;
			this->analysis_saw.set_decision_matrix(&this->matrix);
			cout << "Normalize the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_saw.set_normalisation_type(_madm_normalisation_type::norm_range);
			this->analysis_saw.apply_matrix_normalisation();
			outfile <<"------NORMALISATION------"<<endl;
			this->analysis_saw.decision.output_matrix(&this->outfile);
			cout << "Apply the weights to the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_saw.apply_weighting();
			outfile <<"------WEIGHTS------"<<endl;
			this->analysis_saw.decision.output_matrix(&this->outfile);
			cout << "Solve the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_saw.solve_matrix();
			this->analysis_saw.output_ranking();
			outfile <<"------RANKING------"<<endl;
			this->analysis_saw.output_ranking(&this->outfile);
			this->outfile << "#  END SAW-RANGE analysis "<<endl;
			this->outfile << "###############################################"<<endl;
			this->outfile << endl;
			cout <<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			Sys_Common_Output::output_madm->rewind_userprefix();

			this->sum_up_total_ranking(&this->analysis_saw);
		}
		catch(Error msg){
			this->outfile << "#  An error occured during the SAW-RANGE analysis "<<endl;
			Sys_Common_Output::output_madm->rewind_userprefix();
			msg.output_msg(3);
			counter_error++;
		}
		Madm_System::check_stop_thread_flag();

		try{
			//topsis
			prefix << "TOPSIS"<<"> ";
			Sys_Common_Output::output_madm->set_userprefix(&prefix);
			//file output
			this->outfile << "###############################################"<<endl;
			this->outfile << "#  START TOPSIS analysis "<<endl;
			this->outfile << endl;
			this->analysis_topsis.set_decision_matrix(&this->matrix);
			cout << "Normalize the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_topsis.apply_matrix_normalisation();
			outfile <<"------NORMALISATION------"<<endl;
			this->analysis_topsis.decision.output_matrix(&this->outfile);
			cout << "Apply the weights to the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_topsis.apply_weighting();
			outfile <<"------WEIGHTS------"<<endl;
			this->analysis_topsis.decision.output_matrix(&this->outfile);
			cout << "Solve the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_topsis.solve_matrix();
			this->analysis_topsis.output_ranking();
			outfile <<"------RANKING------"<<endl;
			this->analysis_topsis.output_ranking(&this->outfile);
			this->outfile << "#  END TOPSIS analysis "<<endl;
			this->outfile << "###############################################"<<endl;
			this->outfile << endl;
			cout <<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			Sys_Common_Output::output_madm->rewind_userprefix();

			this->sum_up_total_ranking(&this->analysis_topsis);
		}
		catch(Error msg){
			this->outfile << "#  An error occured during the TOPSIS analysis "<<endl;
			Sys_Common_Output::output_madm->rewind_userprefix();
			msg.output_msg(3);
			counter_error++;
		}

		Madm_System::check_stop_thread_flag();
		try{
			//electre
			prefix << "ELECTRE"<<"> ";
			Sys_Common_Output::output_madm->set_userprefix(&prefix);
			//file output
			this->outfile << "###############################################"<<endl;
			this->outfile << "#  START ELECTRE analysis "<<endl;
			this->outfile << endl;
			this->analysis_electre.set_decision_matrix(&this->matrix);
			cout << "Normalize the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_electre.apply_matrix_normalisation();
			outfile <<"------NORMALISATION------"<<endl;
			this->analysis_electre.decision.output_matrix(&this->outfile);
			cout << "Apply the weights to the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_electre.apply_weighting();
			outfile <<"------WEIGHTS------"<<endl;
			this->analysis_electre.decision.output_matrix(&this->outfile);
			cout << "Solve the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_electre.solve_matrix();
			this->analysis_electre.output_ranking();
			outfile <<"------RANKING------"<<endl;
			this->analysis_electre.output_ranking(&this->outfile);
			this->outfile << "#  END ELECTRE analysis "<<endl;
			this->outfile << "###############################################"<<endl;
			this->outfile << endl;
			cout <<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			Sys_Common_Output::output_madm->rewind_userprefix();

			this->sum_up_total_ranking(&this->analysis_electre);
		}
		catch(Error msg){
			this->outfile << "#  An error occured during the ELECTRE analysis "<<endl;
			Sys_Common_Output::output_madm->rewind_userprefix();
			msg.output_msg(3);
			counter_error++;
		}

		this->final_total_ranking();
	
		this->output_total_ranking();
		this->outfile << "###############################################"<<endl;
		this->outfile << "#  TOTAL RANKING "<<endl;
		this->output_total_ranking(&this->outfile);

	}
	catch(Error msg){
		this->outfile.close();
		Sys_Common_Output::output_madm->rewind_userprefix();
		throw msg;
	}


	this->outfile.close();
	Sys_Common_Output::output_madm->rewind_userprefix();

	return counter_error;
}
//Solve the matrix with the diverse madm-approaches for database-based analysis
int Madm_Analysis::calculate_matrix_database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int set_id){
//prepare the total ranking
	this->delete_total_ranking_array();
	this->allocate_total_ranking();
	this->init_total_ranking();

	int counter_error=0;
	
	//calculation
	try{

		ostringstream cout;
		ostringstream prefix;
		prefix << "CALC"<<"> ";
		Sys_Common_Output::output_madm->set_userprefix(&prefix);
		cout << "Analyse the MADM-matrix "<<this->matrix.get_matrix_name()<< "..."<<endl;
		Sys_Common_Output::output_madm->output_txt(&cout);

		try{
			//saw standard
			prefix << "SAW_STANDARD"<<"> ";
			Sys_Common_Output::output_madm->set_userprefix(&prefix);
			this->analysis_saw.set_decision_matrix(&this->matrix);
			cout << "Normalize the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_saw.set_normalisation_type(_madm_normalisation_type::norm_standard);
			this->analysis_saw.apply_matrix_normalisation();
			cout << "Apply the weights to the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_saw.apply_weighting();
			cout << "Solve the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_saw.solve_matrix();
			this->analysis_saw.output_ranking(true);
			this->analysis_saw.output_ranking2database(ptr_database, area_state, type, set_id);
			Sys_Common_Output::output_madm->output_txt(&cout);
			Sys_Common_Output::output_madm->rewind_userprefix();

			this->sum_up_total_ranking(&this->analysis_saw);
		}
		catch(Error msg){
			Sys_Common_Output::output_madm->rewind_userprefix();
			msg.output_msg(3);
			counter_error++;
		}
		Madm_System::check_stop_thread_flag();

		try{
			//saw range
			prefix << "SAW_RANGE"<<"> ";
			Sys_Common_Output::output_madm->set_userprefix(&prefix);
			this->analysis_saw.set_decision_matrix(&this->matrix);
			cout << "Normalize the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_saw.set_normalisation_type(_madm_normalisation_type::norm_range);
			this->analysis_saw.apply_matrix_normalisation();
			cout << "Apply the weights to the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_saw.apply_weighting();
			cout << "Solve the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_saw.solve_matrix();
			this->analysis_saw.output_ranking(true);
			this->analysis_saw.output_ranking2database(ptr_database, area_state, type, set_id);
			Sys_Common_Output::output_madm->output_txt(&cout);
			Sys_Common_Output::output_madm->rewind_userprefix();

			this->sum_up_total_ranking(&this->analysis_saw);
		}
		catch(Error msg){
			Sys_Common_Output::output_madm->rewind_userprefix();
			msg.output_msg(3);
			counter_error++;
		}
		Madm_System::check_stop_thread_flag();

		try{
			//topsis
			prefix << "TOPSIS"<<"> ";
			Sys_Common_Output::output_madm->set_userprefix(&prefix);
			this->analysis_topsis.set_decision_matrix(&this->matrix);
			cout << "Normalize the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_topsis.apply_matrix_normalisation();
			cout << "Apply the weights to the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_topsis.apply_weighting();
			cout << "Solve the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_topsis.solve_matrix();
			this->analysis_topsis.output_ranking(true);
			this->analysis_topsis.output_ranking2database(ptr_database, area_state, type, set_id);
			Sys_Common_Output::output_madm->output_txt(&cout);
			Sys_Common_Output::output_madm->rewind_userprefix();

			this->sum_up_total_ranking(&this->analysis_topsis);
		}
		catch(Error msg){
			Sys_Common_Output::output_madm->rewind_userprefix();
			msg.output_msg(3);
			counter_error++;
		}

		Madm_System::check_stop_thread_flag();
		try{
			//electre
			prefix << "ELECTRE"<<"> ";
			Sys_Common_Output::output_madm->set_userprefix(&prefix);
			this->analysis_electre.set_decision_matrix(&this->matrix);
			cout << "Normalize the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_electre.apply_matrix_normalisation();
			cout << "Apply the weights to the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_electre.apply_weighting();
			cout << "Solve the MADM-matrix..."<<endl;
			Sys_Common_Output::output_madm->output_txt(&cout);
			this->analysis_electre.solve_matrix();
			this->analysis_electre.output_ranking(true);
			this->analysis_electre.output_ranking2database(ptr_database, area_state, type, set_id);
			Sys_Common_Output::output_madm->output_txt(&cout);
			Sys_Common_Output::output_madm->rewind_userprefix();

			this->sum_up_total_ranking(&this->analysis_electre);
		}
		catch(Error msg){
			Sys_Common_Output::output_madm->rewind_userprefix();
			msg.output_msg(3);
			counter_error++;
		}

		this->final_total_ranking();
	
		this->output_total_ranking();
		this->output_total_ranking2database(ptr_database, area_state, type, set_id);


	}
	catch(Error msg){
		Sys_Common_Output::output_madm->rewind_userprefix();
		throw msg;
	}


	Sys_Common_Output::output_madm->rewind_userprefix();

	return counter_error;
}
//Convert a string into a madm approach type (_madm_approach_type) (static)
_madm_approach_type Madm_Analysis::convert_txt2madm_approach(const string txt){
	_madm_approach_type type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lupper_case(&buffer);

	if(buffer==madm_label::analysis_topsis){
		type=_madm_approach_type::approach_topsis;
	}
	else if(buffer==madm_label::analysis_electre){
		type=_madm_approach_type::approach_electre;
	}
	else if(buffer==madm_label::analysis_saw_standard){
		type=_madm_approach_type::approach_saw_standard;
	}
	else if(buffer==madm_label::analysis_saw_range){
		type=_madm_approach_type::approach_saw_range;
	}
	else if(buffer==madm_label::analysis_total){
		type=_madm_approach_type::approach_total;
	}
	else{
		Error msg;
		msg.set_msg("_madm_approach_type Madm_Analysis::convert_txt2madm_approach(const string txt)", "Can not convert this madm approach", "Check the given madm approach", 1, false);
		ostringstream info;
		info << "Try to convert madm approach: " << txt << endl;
		info << "Possible madm approaches: "<< endl;
		info << " "<<madm_label::analysis_saw_standard  << endl;
		info << " "<<madm_label::analysis_saw_range << endl;
		info << " "<<madm_label::analysis_topsis  << endl;
		info << " "<<madm_label::analysis_electre << endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	return type;
}
//Convert a madm approach (_madm_approach_type) into a string (static)
string Madm_Analysis::convert_madm_approach2txt(const _madm_approach_type src){
	string buffer;
	switch(src){
		case _madm_approach_type::approach_saw_standard:
			buffer=madm_label::analysis_saw_standard;
			break;
		case _madm_approach_type::approach_saw_range:
			buffer=madm_label::analysis_saw_range;
			break;
		case _madm_approach_type::approach_topsis:
			buffer=madm_label::analysis_topsis;
			break;
		case _madm_approach_type::approach_electre:
			buffer=madm_label::analysis_electre;
			break;
		case _madm_approach_type::approach_total:
			buffer=madm_label::analysis_total;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//Set the counter over the different approaches
void Madm_Analysis::set_counter_approaches(const int number){
	this->count_total_ranking=number;
}
//________
//private
//Allocate the total ranking array
void Madm_Analysis::allocate_total_ranking(void){
	try{
		this->total_ranking=new _madm_rank_values[number_total_rank];
		Sys_Memory_Count::self()->add_mem(sizeof(_madm_rank_values)*this->number_total_rank,_sys_system_modules::MADM_SYS);//count the memory
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
	for(int i=0; i< this->number_total_rank; i++){
		this->total_ranking[i].score=0.0;
		this->total_ranking[i].rank=-1;
		buff<<"rank_alt_"<<i;
		this->total_ranking[i].alternative.name=buff.str();
		this->total_ranking[i].alternative.index=i;
		buff.str("");
	}
}
//Delete the total ranking array
void Madm_Analysis::delete_total_ranking_array(void){
	if(this->total_ranking!=NULL){
		delete []this->total_ranking;
		Sys_Memory_Count::self()->minus_mem(sizeof(_madm_rank_values)*this->number_total_rank,_sys_system_modules::MADM_SYS);//count the memory
		this->total_ranking=NULL;
	}
}
//Initialize the total ranking
void Madm_Analysis::init_total_ranking(void){
	this->delete_total_ranking_array();
	this->number_total_rank=this->matrix.get_number_alternatives();
	this->allocate_total_ranking();
	for(int i=0; i< this->number_total_rank; i++){
		this->total_ranking[i].alternative.index=this->matrix.get_alternative(i).index;
		this->total_ranking[i].alternative.name=this->matrix.get_alternative(i).name;
	}

}
//Sum up total ranking
void Madm_Analysis::sum_up_total_ranking(_Madm_Solver *solver){
	this->count_total_ranking++;

	for(int i=0; i< this->number_total_rank; i++){
		this->total_ranking[i].score=this->total_ranking[i].score+
			solver->get_rank_value(this->total_ranking[i].alternative.index);
	}
}
//Finalize the total ranking
void Madm_Analysis::final_total_ranking(void){
	for(int i=0; i< this->number_total_rank; i++){
		this->total_ranking[i].score=this->total_ranking[i].score/this->count_total_ranking;
	}
	_Madm_Solver::sort_alternatives(this->number_total_rank, this->total_ranking, false);
}
//Output total ranking to display/console
void Madm_Analysis::output_total_ranking(void){
	ostringstream prefix;
	ostringstream cout;
	prefix<<"OUT> ";
	Sys_Common_Output::output_madm->set_userprefix(&prefix);
	if(this->number_total_rank>0){
		cout <<"RANKING TOTAL"<<endl;
		cout <<" Number approaches   : " << this->count_total_ranking<<endl;
		cout <<" Madm-solver type    : " << Madm_Analysis::convert_madm_approach2txt(_madm_approach_type::approach_total)<<endl;
		Sys_Common_Output::output_madm->output_txt(&cout);
		cout << " rank " << W(5)<<" id " <<W(15)<<" name " <<W(15)<< " score " << FORMAT_FIXED_REAL<< P(3)<< endl;
		Sys_Common_Output::output_madm->output_txt(&cout);
		for(int j=0; j<this->number_total_rank; j++){
			cout <<" "<<this->total_ranking[j].rank << W(8);
			cout <<this->total_ranking[j].alternative.index<<W(12)<<" ";
			cout << this->total_ranking[j].alternative.name<<W(15)<<" ";
			cout << this->total_ranking[j].score<<endl;
			if(j<3){
				Sys_Common_Output::output_madm->output_txt(&cout);
			}
			else{
				Sys_Common_Output::output_madm->output_txt(&cout,true);
			}
		}	
	}
	else{
		cout <<"No alternatives for ranking are set!"<<endl;
		Sys_Common_Output::output_madm->output_txt(&cout);
	}

	Sys_Common_Output::output_madm->rewind_userprefix();
}
//Output total ranking to file
void Madm_Analysis::output_total_ranking(ofstream *outfile){
	*outfile <<"RANKING TOTAL"<<endl;
	*outfile <<" Number approaches   : " << this->count_total_ranking<<endl;
	*outfile <<" Madm-solver type    : " << Madm_Analysis::convert_madm_approach2txt(_madm_approach_type::approach_total)<<endl;
	*outfile << " rank " << W(5)<<" id " <<W(10)<<" name " <<W(10)<< " score " << FORMAT_FIXED_REAL<< P(3)<<endl;
	for(int j=0; j<this->number_total_rank; j++){
		*outfile <<" "<<this->total_ranking[j].rank << W(8);
		*outfile <<this->total_ranking[j].alternative.index<<W(12);
		*outfile << this->total_ranking[j].alternative.name<<W(10);
		*outfile << this->total_ranking[j].score<<endl;
	}
	outfile->flush();
}
//Ouput the total ranking to database
void Madm_Analysis::output_total_ranking2database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int set_id){

	try{
		_Madm_Solver::set_table_ranking(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;

	glob_id=_Madm_Solver::table_ranking->maximum_int_of_column(_Madm_Solver::table_ranking->get_column_name(label::glob_id),ptr_database)+1;


	QSqlQuery model(*ptr_database);
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << _Madm_Solver::table_ranking->get_table_name();
	query_string <<" ( ";
	query_string << _Madm_Solver::table_ranking->get_column_name(label::glob_id) <<" , ";
	query_string << _Madm_Solver::table_ranking->get_column_name(madm_label::set_id) <<" , ";
	query_string << _Madm_Solver::table_ranking->get_column_name(label::areastate_id) <<" , ";
	query_string << _Madm_Solver::table_ranking->get_column_name(label::measure_id) <<" , ";

	query_string << _Madm_Solver::table_ranking->get_column_name(risk_label::risk_type) <<" , ";
	query_string << _Madm_Solver::table_ranking->get_column_name(madm_label::analysis_type) <<" , ";

	query_string << _Madm_Solver::table_ranking->get_column_name(madm_label::rank) <<" , ";
	query_string << _Madm_Solver::table_ranking->get_column_name(madm_label::score) <<" ) ";
	query_string << " VALUES ";
	for(int i=0; i< this->number_total_rank; i++){
		query_string << "  ( ";
		query_string << glob_id << " , " ;
		query_string << set_id << " , " ;
		query_string << area_state << " , " ;
		query_string << this->total_ranking[i].alternative.index << " , " ;
		query_string << "'"<< Risk_Break_Scenario::convert_risk_type2txt(type)<< "' , " ;
		query_string << "'"<< Madm_Analysis::convert_madm_approach2txt(_madm_approach_type::approach_total)<< "' , " ;

		query_string << this->total_ranking[i].rank << " , " ;
		query_string << this->total_ranking[i].score << " ) " ;
		if(i<this->number_total_rank-1){
			query_string <<" , ";
		}
		glob_id++;
	}

	Data_Base::database_request(&model, query_string.str(), ptr_database);
	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << _Madm_Solver::table_ranking->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;	
		msg.make_second_info(info.str());
		msg.output_msg(3);	
	}


}
//Set warning(s)
Warning Madm_Analysis::set_warning(const int warn_type){
	string place="Madm_Analysis::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://the type of thread is not specified 
			place.append("output_total_ranking2database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int set_id)") ;
			reason="Can not submit the total ranked result data to the database";
			help="Check the database";
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
//Set error(s)
Error Madm_Analysis::set_error(const int err_type){
	string place="Madm_Analysis::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://can not open madm analysis outputfile
			place.append("calculate_matrix_file(void)");
			reason="Can not open the file of the MADM-analysis outputfile";
			help="Check the file";
			type=5;
			break;
		case 1://bad alloc
			place.append("allocate_total_ranking(void)");
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