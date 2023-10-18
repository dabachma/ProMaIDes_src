#include "Madm_Headers_Precompiled.h"
//#include "_Madm_Solver.h"

//init static members
Tables *_Madm_Solver::table_ranking=NULL;

//Default constructor
_Madm_Solver::_Madm_Solver(void){
	this->norm_type=_madm_normalisation_type::norm_vector;
	this->ranking=NULL;
	this->number_rank=0;
	this->solver_type=_madm_approach_type::approach_unknown;
}
//Default destructor
_Madm_Solver::~_Madm_Solver(void){
	this->delete_ranking_array();
}
//_________
//public
//Set the database table for the alternative's ranking: it sets the table name and the name of the columns and allocate them (static)
void _Madm_Solver::set_table_ranking(QSqlDatabase *ptr_database){
	if(_Madm_Solver::table_ranking==NULL){
		//make specific input for this class
		const string tab_id_name=madm_label::tab_ranking;
		string tab_col[8];
		tab_col[0]=label::glob_id;
		tab_col[1]=label::areastate_id;
		tab_col[2]=label::measure_id;
		tab_col[3]=madm_label::set_id;
		tab_col[4]=risk_label::risk_type;
		tab_col[5]=madm_label::analysis_type;
		tab_col[6]=madm_label::rank;
		tab_col[7]=madm_label::score;

		
		//set the table
		try{
			_Madm_Solver::table_ranking= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			_Madm_Solver::table_ranking->set_name(ptr_database, _sys_table_type::madm);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("_Madm_Solver::set_table_ranking(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Madm_Solver::close_table_ranking();
			throw msg;
		}
	}
}
//Create the database table for the alternative's ranking (static)
void _Madm_Solver::create_table_ranking(QSqlDatabase *ptr_database){
	if(_Madm_Solver::table_ranking==NULL){
		ostringstream cout;
		cout << "Create database table of the alternative's ranking..." << endl ;
		Sys_Common_Output::output_madm->output_txt(&cout);
		//make specific input for this class
		const string tab_name=madm_label::tab_ranking;
		const int num_col=8;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}

		tab_col[0].name=label::glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=label::areastate_id;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag=true;
		tab_col[1].key_flag=true;

		tab_col[2].name=label::measure_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;
		tab_col[2].key_flag=true;

		tab_col[3].name=madm_label::set_id;
		tab_col[3].type=sys_label::tab_col_type_int;
		tab_col[3].unsigned_flag=true;

		tab_col[4].name=risk_label::risk_type;
		tab_col[4].type=sys_label::tab_col_type_string;

		tab_col[5].name=madm_label::analysis_type;
		tab_col[5].type=sys_label::tab_col_type_string;

		tab_col[6].name=madm_label::rank;
		tab_col[6].type=sys_label::tab_col_type_int;
		tab_col[6].unsigned_flag=true;

		tab_col[7].name=madm_label::score;
		tab_col[7].type=sys_label::tab_col_type_double;




		try{
			_Madm_Solver::table_ranking= new Tables();
			if(_Madm_Solver::table_ranking->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::madm)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_madm->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Madm_Solver::create_table_ranking(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			_Madm_Solver::close_table_ranking();
			throw msg;
		}
		_Madm_Solver::close_table_ranking();
		
	}
}
//Delete all data in the database table for the alternative's ranking (static)
void _Madm_Solver::delete_data_in_table_ranking(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		_Madm_Solver::set_table_ranking(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	_Madm_Solver::table_ranking->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the alternative's ranking for a given system id (static)
void _Madm_Solver::delete_data_in_table_ranking(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		_Madm_Solver::set_table_ranking(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "DELETE FROM ";
	test_filter << _Madm_Solver::table_ranking->get_table_name();
	test_filter << " WHERE " ;
	test_filter << _Madm_Solver::table_ranking->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND " ;
	test_filter << _Madm_Solver::table_ranking->get_column_name(label::measure_id) << " = "  << id.measure_nr;

	Data_Base::database_request(&model, test_filter.str(), ptr_database);


}
//Close and delete the database table for the alternative's ranking (static)
void _Madm_Solver::close_table_ranking(void){
	if(_Madm_Solver::table_ranking!=NULL){
		delete _Madm_Solver::table_ranking;
		_Madm_Solver::table_ranking=NULL;
	}
}
//Select the ranking data of the database table for the specific paramters
int _Madm_Solver::select_data_in_ranking_table(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int areastate, const int set_id, const _risk_type risktype, const _madm_approach_type ana_type){
	try{
		_Madm_Solver::set_table_ranking(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << _Madm_Solver::table_ranking->get_table_name();
	test_filter << " WHERE " ;
	test_filter << _Madm_Solver::table_ranking->get_column_name(madm_label::set_id) << " = "  << set_id; 
	test_filter << " AND " ;
	test_filter << _Madm_Solver::table_ranking->get_column_name(label::areastate_id) << " = "  << areastate; 
	test_filter << " AND " ;
	test_filter << _Madm_Solver::table_ranking->get_column_name(risk_label::risk_type) << " = '"  << Risk_Break_Scenario::convert_risk_type2txt(risktype)<<"'"; 
	test_filter << " AND " ;
	test_filter << _Madm_Solver::table_ranking->get_column_name(madm_label::analysis_type) << " = '"  << Madm_Analysis::convert_madm_approach2txt(ana_type)<<"'"; 
	test_filter << " ORDER BY " <<	_Madm_Solver::table_ranking->get_column_name(madm_label::rank);

	Data_Base::database_request(model, test_filter.str(), ptr_database);
		
	//check the request
	if(model->lastError().isValid()){
		Error msg;
		msg.set_msg("_Madm_Solver::select_data_in_ranking_table(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int areastate, const int set_id, const _risk_type risktype, const _madm_approach_type ana_type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Madm_Solver::table_ranking->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return model->rowCount();
}
//Check if resluts are available for a given risk type
bool _Madm_Solver::check_data_in_ranking_table(QSqlDatabase *ptr_database, const int areastate,  const _risk_type risktype){
	QSqlQueryModel model;
	try{
		_Madm_Solver::set_table_ranking(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << _Madm_Solver::table_ranking->get_table_name();
	test_filter << " WHERE " ;
	test_filter << _Madm_Solver::table_ranking->get_column_name(label::areastate_id) << " = "  << areastate; 
	test_filter << " AND " ;
	test_filter << _Madm_Solver::table_ranking->get_column_name(risk_label::risk_type) << " = '"  << Risk_Break_Scenario::convert_risk_type2txt(risktype)<<"'"; 
			
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
		
	//check the request
	if(model.lastError().isValid()){
		Error msg;
		msg.set_msg("_Madm_Solver::check_data_in_ranking_table(QSqlDatabase *ptr_database, const int areastate,  const _risk_type risktype)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Madm_Solver::table_ranking->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if( model.rowCount()>0){
		return true;
	}
	else{
		return false;
	}
}
//Set the decision matrix
void _Madm_Solver::set_decision_matrix(Madm_Decision_Matrix *matrix){
	this->decision.clear_matrix();
	this->decision=*matrix;

	this->delete_ranking_array();
	this->number_rank=this->decision.get_number_alternatives();
	this->allocate_ranking();

	for(int i=0; i< this->number_rank;i++){
		this->ranking[i].alternative=this->decision.get_alternative(i);
	}
}
//Apply the weigthing of the matrix (use it after the normalisation!)
void _Madm_Solver::apply_weighting(void){
	this->decision.apply_weighting();

}
//Set the normalisation type and apply the normalsation
void _Madm_Solver::apply_matrix_normalisation(void){
	this->decision.apply_matrix_normalisation(this->norm_type);
}
//Set normalisation type
void _Madm_Solver::set_normalisation_type(const _madm_normalisation_type ){
	//nothing to do! Standard is norm_vector; just important for the saw-analysis
}
//Output the ranking to display/console
void _Madm_Solver::output_ranking(const bool output_detail){
	ostringstream prefix;
	ostringstream cout;
	prefix<<"OUT> ";
	Sys_Common_Output::output_madm->set_userprefix(&prefix);
	if(this->number_rank>0){
		cout <<"RANKING"<<endl;
		cout <<" Number alternatives : " << this->number_rank<<endl;
		cout <<" Madm-solver type    : " << Madm_Analysis::convert_madm_approach2txt(this->solver_type)<<endl;
		cout <<" Normalisation type  : " << Madm_Decision_Matrix::convert_norm_scheme2txt(this->norm_type)<<endl;
		Sys_Common_Output::output_madm->output_txt(&cout, output_detail);
		cout << " rank " << W(5)<<" id " <<W(10)<<" name " <<W(10)<< " score " << FORMAT_FIXED_REAL<< P(3)<< endl;
		Sys_Common_Output::output_madm->output_txt(&cout, output_detail);
		for(int j=0; j<this->number_rank; j++){
			cout <<" "<<this->ranking[j].rank << W(8);
			cout <<this->ranking[j].alternative.index<<W(12)<<" ";
			cout << this->ranking[j].alternative.name<<W(10)<<" ";
			cout << this->ranking[j].score<<endl;
			if(j<3){
				Sys_Common_Output::output_madm->output_txt(&cout,output_detail);
			}
			else{
				Sys_Common_Output::output_madm->output_txt(&cout,true);
			}
		}	
	}
	else{
		cout <<"No alternatives for ranking are set!"<<endl;
		Sys_Common_Output::output_madm->output_txt(&cout,output_detail);
	}

	Sys_Common_Output::output_madm->rewind_userprefix();
}
//Output the ranking to file
void _Madm_Solver::output_ranking(ofstream *outfile){
	*outfile <<"RANKING"<<endl;
	*outfile <<" Number alternatives : " << this->number_rank<<endl;
	*outfile <<" Madm-solver type    : " << Madm_Analysis::convert_madm_approach2txt(this->solver_type)<<endl;
	*outfile <<" Normalisation type  : " << Madm_Decision_Matrix::convert_norm_scheme2txt(this->norm_type)<<endl;
	*outfile << " rank " << W(5)<<" id " <<W(10)<<" name " <<W(10)<< " score " << FORMAT_FIXED_REAL<< P(3)<<endl;
	for(int j=0; j<this->number_rank; j++){
		*outfile <<" "<<this->ranking[j].rank << W(8);
		*outfile <<this->ranking[j].alternative.index<<W(12);
		*outfile << this->ranking[j].alternative.name<<W(10);
		*outfile << this->ranking[j].score<<endl;
	}
	outfile->flush();
}
//Output the ranking to database
void _Madm_Solver::output_ranking2database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int set_id){


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
	for(int i=0; i< this->number_rank; i++){
		query_string << "  ( ";
		query_string << glob_id << " , " ;
		query_string << set_id << " , " ;
		query_string << area_state << " , " ;
		query_string << this->ranking[i].alternative.index << " , " ;
		query_string << "'"<< Risk_Break_Scenario::convert_risk_type2txt(type)<< "' , " ;
		query_string << "'"<< Madm_Analysis::convert_madm_approach2txt(this->solver_type)<< "' , " ;

		query_string << this->ranking[i].rank << " , " ;
		query_string << this->ranking[i].score << " ) " ;
		if(i<this->number_rank-1){
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
//Get the score values with a given alternative index
double _Madm_Solver::get_score_value(const int alt_index){
	double buffer=0.0;
	for(int i=0; i< this->number_rank; i++){
		if(alt_index==this->ranking[i].alternative.index){
			buffer=ranking[i].score;
			return buffer;
		}

	}
	return buffer;
}
//Get the rank values with a given alternative index as double
double _Madm_Solver::get_rank_value(const int alt_index){
	double buffer=999.00;
	for(int i=0; i< this->number_rank; i++){
		if(alt_index==this->ranking[i].alternative.index){
			buffer=(double)ranking[i].rank;
			return buffer;
		}

	}
	return buffer;
}
//Sort the alternatives after the score (static)
void _Madm_Solver::sort_alternatives(const int number_rank, _madm_rank_values *rank, const bool max_first){
	_madm_rank_values swap;
	int j=0;
	if(max_first==true){
		for (int i = 1; i < number_rank; i++) {
			swap=rank[i];
			//the higher value goes first => ">" ??
			//the smaller value goes first => "<" 
			j = i-1;
			while ( j >= 0 && swap.score > rank[j].score){
				rank[j+1]= rank[j];
				j--;
			}
			rank[j+1] = swap;
		}
	}else{
		for (int i = 1; i < number_rank; i++) {
			int j=0;
			swap=rank[i];
			//the higher value goes first => ">" ??
			//the smaller value goes first => "<" 
			for (j = i-1; j >= 0 && swap.score < rank[j].score; j--){
				rank[j+1] = rank[j];
			}
			rank[j+1] = swap;
		}
	}

	//set rank number
	for (int i = 0; i < number_rank; i++){
		if(i==0){
			rank[i].rank=i;
		}
		else if(rank[i].score!=rank[i-1].score){
			rank[i].rank=i;
		}
		else{
			rank[i].rank=i-1;
		}


	}
}
//__________
//protected
//Allocate the ranking array
void _Madm_Solver::allocate_ranking(void){
	try{
		this->ranking=new _madm_rank_values[this->number_rank];
		Sys_Memory_Count::self()->add_mem(sizeof(_madm_rank_values)*this->number_rank,_sys_system_modules::MADM_SYS);//count the memory
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
	for(int i=0; i< this->number_rank; i++){
		this->ranking[i].score=0.0;
		this->ranking[i].rank=-1;
		buff<<"rank_alt_"<<i;
		this->ranking[i].alternative.name=buff.str();
		this->ranking[i].alternative.index=i;
		buff.str("");
	}
}
//Delete the ranking array
void _Madm_Solver::delete_ranking_array(void){
	if(this->ranking!=NULL){
		delete []this->ranking;
		Sys_Memory_Count::self()->minus_mem(sizeof(_madm_rank_values)*this->number_rank,_sys_system_modules::MADM_SYS);//count the memory
		this->ranking=NULL;
	}
}
//_________
//private
//Set warning(s)
Warning _Madm_Solver::set_warning(const int warn_type){
	string place="_Madm_Solver::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://the type of thread is not specified 
			place.append("output_ranking2database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int set_id)") ;
			reason="Can not submit the ranked result data to the database";
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
Error _Madm_Solver::set_error(const int err_type){
	string place="_Madm_Solver::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
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