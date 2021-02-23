//#include "Fpl_Mc_Sim.h"
#include "Fpl_Headers_Precompiled.h"

//init static members
Tables *Fpl_Mc_Sim::table=NULL;
Tables *Fpl_Mc_Sim::result_table=NULL;
_fpl_random_generator_type Fpl_Mc_Sim::generator=_fpl_random_generator_type::gen_type_ranmar;

//Default constructor
Fpl_Mc_Sim::Fpl_Mc_Sim(void){
	
	this->mc_max=fpl_control_param::mc_max_def;
	this->mc_min=fpl_control_param::mc_min_def;
	
	this->epsilon_absolut=fpl_control_param::epsilon_absolut_def;
	this->epsilon_relativ=fpl_control_param::epsilon_relativ_def;
	this->confidence=fpl_control_param::confidence_def;
	this->calculate_alpha();
	this->failure=0;
	this->stat_output=false;

	this->reset_start_value();
	
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mc_Sim), _sys_system_modules::FPL_SYS);//count the memory	
}
//Constructor with given control parameter-set for a monte-carlo simulation
Fpl_Mc_Sim::Fpl_Mc_Sim(const double mc_max_var, const double mc_min_var, const double error_abs, const double error_rel_perc, const double confid_perc){

	this->mc_max=mc_max_var;
	this->mc_min=mc_min_var;

	this->epsilon_absolut=error_abs; 
	this->epsilon_relativ=error_rel_perc/100.0;
	this->confidence=confid_perc/100.0;
	this->calculate_alpha();
	this->failure=0;
	this->stat_output=false;

	this->reset_start_value();
	
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mc_Sim), _sys_system_modules::FPL_SYS);//count the memory
	//check the control paramter
	this->check_mc_param();
		
}
//Default destructor
Fpl_Mc_Sim::~Fpl_Mc_Sim(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mc_Sim), _sys_system_modules::FPL_SYS);//count the memory	
}
//___________
//public
//Create the database table for the calculation parameters (static)
bool Fpl_Mc_Sim::create_table(QSqlDatabase *ptr_database){

	if(Fpl_Mc_Sim::table==NULL){
		ostringstream cout;
		cout << "Create database table for the probability calculation control parameter..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		//make specific input for this class
		const string tab_name=fpl_label::tab_fpl_control;
		const int num_col=4;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}

		tab_col[0].name=fpl_label::glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=fpl_label::control_name;
		tab_col[1].type=sys_label::tab_col_type_string;

		tab_col[2].name=fpl_label::control_value;
		tab_col[2].type=sys_label::tab_col_type_string;

		tab_col[3].name=label::description;
		tab_col[3].type=sys_label::tab_col_type_string;

		try{
			Fpl_Mc_Sim::table= new Tables();
			if(Fpl_Mc_Sim::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout);
				Fpl_Mc_Sim::close_table();
				return false;
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Fpl_Mc_Sim::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Mc_Sim::close_table();
			throw msg;
		}
		Fpl_Mc_Sim::close_table();
		return true;
		
	}
	return false;
}
//Reset all data in the database table for the calculation parameters to the default values (static)
void Fpl_Mc_Sim::reset_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Mc_Sim::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	
	Fpl_Mc_Sim::set_predefined_data2control_table(ptr_database);
}
//Set the database table for the calculation parameters: it sets the table name and the name of the columns and allocate them (static)
void Fpl_Mc_Sim::set_table(QSqlDatabase *ptr_database){
	if(Fpl_Mc_Sim::table==NULL){

		//make specific input for this class
		const string tab_id_name=fpl_label::tab_fpl_control;
		string tab_col[4];

		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::control_name;
		tab_col[2]=fpl_label::control_value;
		tab_col[3]=label::description;


		try{
			Fpl_Mc_Sim::table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Mc_Sim::table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("void Fpl_Mc_Sim::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Mc_Sim::close_table();
			throw msg;
		}
	}
}
//Write the default value of the control parameters into the database table (static)
void Fpl_Mc_Sim::set_predefined_data2control_table(QSqlDatabase *ptr_database){
	try{
		Fpl_Mc_Sim::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Fpl_Mc_Sim::table->delete_data_in_table(ptr_database);

	//mysql query with the query-class
	QSqlQuery model(*ptr_database);

	//evaluate the global identifier
	int id_glob=Fpl_Mc_Sim::table->maximum_int_of_column(Fpl_Mc_Sim::table->get_column_name(fpl_label::glob_id),ptr_database)+1;
	ostringstream total;

	//set the query via a query string
	ostringstream query_string_fix;
	query_string_fix << "INSERT INTO  " << Fpl_Mc_Sim::table->get_table_name();
	query_string_fix <<" ( ";
	query_string_fix << Fpl_Mc_Sim::table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string_fix << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) <<" , ";
	query_string_fix << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) <<" , ";
	query_string_fix << Fpl_Mc_Sim::table->get_column_name(label::description) <<" ) ";

	//mc
	//Keystring for maximal number of a monte-carlo simulations 
	ostringstream query_string;
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::mc_max <<"'"<< " , " ;
	query_string << fpl_control_param::mc_max_def << " , " ;
	query_string << "'Maximal number of a monte-carlo simulations'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//Keystring for minimal number of a monte-carlo simulations before the abort criterion is checked
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::mc_min <<"'"<< " , " ;
	query_string << fpl_control_param::mc_min_def << " , " ;
	query_string << "'Minimal number of a monte-carlo simulations before the abort criterion is checked'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//Keystring for absolute accurancy-criteria for the abort criterion of a monte-carlo simulations
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::mc_error_abs <<"'"<< " , " ;
	query_string << fpl_control_param::epsilon_absolut_def << " , " ;
	query_string << "'Absolute accurancy-criteria for the abort criterion of a monte-carlo simulations'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//Keystring for relative accurancy-criteria for the abort criterion of a monte-carlo simulations
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::mc_error_rel <<"'"<< " , " ;
	query_string << fpl_control_param::epsilon_relativ_def*100.0 << " , " ;
	query_string << "'Relative accurancy-criteria for the abort criterion of a monte-carlo simulations [%]'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//Keystring for the confidence interval for the abort criterion of a monte-carlo simulations
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::mc_confident <<"'"<< " , " ;
	query_string << fpl_control_param::confidence_def*100.0 << " , " ;
	query_string << "'The confidence interval for the abort criterion of a monte-carlo simulations [%]'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//Keystring for the random generator type of a monte-carlo simulations
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::mc_rand_generator <<"'"<< " , " ;
	query_string <<"'"<< Fpl_Mc_Sim::convert_generator_type2text(fpl_control_param::random_def) << "' , " ;
	query_string << "'The type of random generator, which is used (RANMAR, MOTHER_OF_ALL, SFMT)'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;


	//frc
	//Keystring for maximal number of fragility curve steps
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::frc_max <<"'"<< " , " ;
	query_string << fpl_control_param::max_calculation_def << " , " ;
	query_string << "'Maximal number of fragility curve steps or maximal number of waterlevels, which are calculated'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//Keystring for the maximal stepsize [m] of fragility curve calculation
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::frc_step_max <<"'"<< " , " ;
	query_string << fpl_control_param::stepsize_max_def << " , " ;
	query_string << "'Maximal stepsize [m] of fragility curve calculation'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//Keystring for the minimal stepsize [m] of fragility curve calculation
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::frc_step_min <<"'"<< " , " ;
	query_string << fpl_control_param::stepsize_min_def << " , " ;
	query_string << "'Minimal stepsize [m] of fragility curve calculation'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//Keystring for the starting stepsize [m] of fragility curve calculation
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::frc_step_start <<"'"<< " , " ;
	query_string << fpl_control_param::starting_stepsize_def << " , " ;
	query_string << "'Starting stepsize [m] of fragility curve calculation'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//Keystring for the starting waterlevel [m] of fragility curve calculation
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::frc_height_start <<"'"<< " , " ;
	query_string << fpl_control_param::start_value_def << " , " ;
	query_string << "'Starting waterlevel [m] of fragility curve calculation'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//Keystring for the boundary value for a stepsize reduction for fragility curve simulations 
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::frc_step_reduce <<"'"<< " , " ;
	query_string << fpl_control_param::quotient_boundary_reduce_def << " , " ;
	query_string << "'Boundary value for a stepsize reduction for fragility curve simulations'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//Keystring for the boundary value for a stepsize expansion for fragility curve simulations  
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::frc_step_expand <<"'"<< " , " ;
	query_string << fpl_control_param::quotient_boundary_expand_def << " , " ;
	query_string << "'Boundary value for a stepsize expansion for fragility curve simulations'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//Keystring for the step-size factor for fragility curve simulations   
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string <<"'"<< fpl_control_param::frc_step_factor <<"'"<< " , " ;
	query_string << fpl_control_param::stepsize_factor_def << " , " ;
	query_string << "'Step-size factor for fragility curve simulations'" <<" ) ";
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//Keystring for the output setting to tecplot   
	query_string << " VALUES ( ";
	query_string << id_glob << " , ";
	query_string << "'" << fpl_control_param::output_tecplot<< "'" << " , ";
	query_string << fpl_control_param::output_tecplot_def << " , ";
	query_string << "'Tecplot output required 1 := true ; 0 := false'" << " ) ";
	total << query_string_fix.str() << query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//Keystring for the output setting to paraview   
	query_string << " VALUES ( ";
	query_string << id_glob << " , ";
	query_string << "'" << fpl_control_param::output_paraview << "'" << " , ";
	query_string << fpl_control_param::output_paraview_def << " , ";
	query_string << "'ParaView output required 1 := true ; 0 := false'" << " ) ";
	total << query_string_fix.str() << query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//Keystring for the output setting to excel  
	query_string << " VALUES ( ";
	query_string << id_glob << " , ";
	query_string << "'" << fpl_control_param::output_excel << "'" << " , ";
	query_string << fpl_control_param::output_excel_def << " , ";
	query_string << "'Excel output required 1 := true ; 0 := false'" << " ) ";
	total << query_string_fix.str() << query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;



	Fpl_Seepage_Calculator_Dike::set_predefined_data2control_table(ptr_database, &model, &id_glob, &query_string_fix);


}
///Get output control parameters from table (static)
void Fpl_Mc_Sim::get_output_control_from_table(QSqlDatabase *ptr_database, output_control *control_flags) {
	//mysql query with the table_model
	QSqlTableModel model(0, *ptr_database);
	int number_result = 0;
	//the table is set the name and the column names: it is the same than for the mc-parameters
	try {
		Fpl_Mc_Sim::set_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//give the complet table of control parameters FPL
	model.setTable(Fpl_Mc_Sim::table->get_table_name().c_str());
	//set the query; select all in table
	Data_Base::database_request(&model);
	//check the query
	if (model.lastError().isValid()) {
		Error msg;
		msg.set_msg("Fpl_Mc_Sim::get_output_control_from_table(QSqlDatabase *ptr_database, output_control *control_flags)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	else {
		number_result = model.rowCount();
	}
	//read out the results
	 //the set of the name-column has to match to this parameter
	//output
	if (number_result > 0) {
		//read out from the search result
		string buffer;
		for (int i = 0; i < number_result; i++) {
			buffer = model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name)).c_str()).toString().toStdString();
			if (buffer == fpl_control_param::output_excel) {
				control_flags->excel_output = model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toInt();

			}
			else if (buffer == fpl_control_param::output_paraview) {
				control_flags->para_output = model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toInt();

			}
			else if (buffer == fpl_control_param::output_tecplot) {
				control_flags->tec_output = model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toInt();

			}
		}
	}



}
///Set output control parameters to table (static)
void Fpl_Mc_Sim::set_output_control2table(QSqlDatabase *ptr_database, output_control control_flags) {
	//the table is set (the name and the column names) and allocated
	try {
		Fpl_Mc_Sim::set_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}


	QSqlQueryModel query;
	ostringstream query_string;

	//output
	query_string << "UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET ";
	query_string << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = '" << int(control_flags.excel_output) << "' ";
	query_string << " WHERE ";
	query_string << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '" << fpl_control_param::output_excel << "'";
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid()) {
		Error msg;
		msg.set_msg("Fpl_Mc_Sim::set_output_control2table(QSqlDatabase *ptr_database, output_control control_flags)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	query_string.str("");

	query_string << "UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET ";
	query_string << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = '" << int(control_flags.para_output) << "' ";
	query_string << " WHERE ";
	query_string << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '" << fpl_control_param::output_paraview << "'";
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid()) {
		Error msg;
		msg.set_msg("Fpl_Mc_Sim::set_output_control2table(QSqlDatabase *ptr_database, output_control control_flags)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	query_string.str("");

	query_string << "UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET ";
	query_string << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = '" << int(control_flags.tec_output) << "' ";
	query_string << " WHERE ";
	query_string << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '" << fpl_control_param::output_tecplot << "'";
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid()) {
		Error msg;
		msg.set_msg("Fpl_Mc_Sim::set_output_control2table(QSqlDatabase *ptr_database, output_control control_flags)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	query_string.str("");

}
///Close and delete the database table for the calculation data (static)
void Fpl_Mc_Sim::close_table(void){
	if(Fpl_Mc_Sim::table!=NULL){
		delete Fpl_Mc_Sim::table;
		Fpl_Mc_Sim::table=NULL;
	}
}
//Create the database table for the results of a reliability calculation (Mc/Frc) (static)
void Fpl_Mc_Sim::create_result_table(QSqlDatabase *ptr_database){
	if(Fpl_Mc_Sim::result_table==NULL){
			ostringstream cout;
			cout << "Create reliability result database table..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_fpl_reliability;
			const int num_col=12;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}


			tab_col[0].name=fpl_label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=fpl_label::section_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=label::areastate_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].key_flag=true;

			tab_col[3].name=label::measure_id;
			tab_col[3].type=sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag=true;
			tab_col[3].key_flag=true;

			tab_col[4].name=label::applied_flag;
			tab_col[4].type=sys_label::tab_col_type_bool;
			tab_col[4].default_value="true";
			tab_col[4].key_flag=true;

			tab_col[5].name=fpl_label::reli_mc_frc_flag;
			tab_col[5].type=sys_label::tab_col_type_bool;
			tab_col[5].key_flag=true;

			tab_col[6].name=fpl_label::reli_waterlevel;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].unsigned_flag=true;

			tab_col[7].name=fpl_label::reli_probability;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].unsigned_flag=true;

			tab_col[8].name=fpl_label::reli_low_bound;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=fpl_label::reli_up_bound;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].unsigned_flag=true;

			tab_col[10].name=fpl_label::reli_comp_time;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].unsigned_flag=true;

			tab_col[11].name=fpl_label::reli_number_mc;
			tab_col[11].type=sys_label::tab_col_type_int;
			tab_col[11].unsigned_flag=true;
			

			try{
				Fpl_Mc_Sim::result_table= new Tables();
				if(Fpl_Mc_Sim::result_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Mc_Sim::create_result_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Mc_Sim::close_result_table();
				throw msg;
			}

		Fpl_Mc_Sim::close_result_table();
	}
}
//Delete all data in the database table for the results of a reliability calculation (Mc/Frc) (static)
void Fpl_Mc_Sim::delete_data_in_result_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Mc_Sim::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Fpl_Mc_Sim::result_table->delete_data_in_table(ptr_database);
}
//Delete a data set in the database table for the results of a reliability calculation for a specified system state (Mc/Frc) (static)
void Fpl_Mc_Sim::delete_data_in_result_table(QSqlDatabase *ptr_database, const _sys_system_id id){
		//the table is set (the name and the column names) and allocated
	try{
		Fpl_Mc_Sim::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Fpl_Mc_Sim::result_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Fpl_Mc_Sim::result_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Mc_Sim::delete_data_in_result_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete a data set in the database table for the results of a reliability calculation for a specified section (Mc/Frc) (static)
void Fpl_Mc_Sim::delete_data_in_result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const bool mc_flag){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Mc_Sim::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Fpl_Mc_Sim::result_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Fpl_Mc_Sim::result_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::section_id) << " = " << section_id;
	query_string << " AND ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_mc_frc_flag) << " = " << functions::convert_boolean2string(mc_flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Mc_Sim::delete_data_in_result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const bool mc_flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete a data set in the database table for the results of a reliability calculation (Mc/Frc) (static)
void Fpl_Mc_Sim::delete_data_in_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool mc_flag, const bool just_generated){
		//the table is set (the name and the column names) and allocated
	try{
		Fpl_Mc_Sim::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Fpl_Mc_Sim::result_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Fpl_Mc_Sim::result_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_mc_frc_flag) << " = " << functions::convert_boolean2string(mc_flag);
	if(just_generated==true){
		query_string << " AND ";
		query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_number_mc) << " > 0 ";
	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Mc_Sim::delete_data_in_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool mc_flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the database table for the results of a reliability calculation (Mc/Frc): it sets the table name and the name of the columns and allocate them (static)
void Fpl_Mc_Sim::set_result_table(QSqlDatabase *ptr_database){
	if(Fpl_Mc_Sim::result_table==NULL){

		//make specific input for this class
		const string tab_id_name=fpl_label::tab_fpl_reliability;
		string tab_col[12];

		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::section_id;
		tab_col[2]=label::areastate_id;
		tab_col[3]=label::measure_id;
		tab_col[4]=fpl_label::reli_mc_frc_flag;
		tab_col[5]=fpl_label::reli_waterlevel;
		tab_col[6]=fpl_label::reli_probability;
		tab_col[7]=fpl_label::reli_low_bound;
		tab_col[8]=fpl_label::reli_up_bound;
		tab_col[9]=fpl_label::reli_comp_time;
		tab_col[10]=fpl_label::reli_number_mc;
		tab_col[11]=label::applied_flag;


		try{
			Fpl_Mc_Sim::result_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Mc_Sim::result_table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("void Fpl_Mc_Sim::set_result_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Mc_Sim::close_result_table();
			throw msg;
		}
	}
}
//Close and delete the database table for the results of a reliability calculation (Mc/Frc) (static)
void Fpl_Mc_Sim::close_result_table(void){
	if(Fpl_Mc_Sim::result_table!=NULL){
		delete Fpl_Mc_Sim::result_table;
		Fpl_Mc_Sim::result_table=NULL;
	}
}
//Select results of given fpl section in a database table (static)
int Fpl_Mc_Sim::select_results_in_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id, const bool mc_flag){
	int number=0;
	try{
		Fpl_Mc_Sim::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT *";
	test_filter << " FROM " << Fpl_Mc_Sim::result_table->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::section_id) << " = " << sec_id;
	test_filter << " AND ";
	test_filter << Fpl_Mc_Sim::result_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Fpl_Mc_Sim::result_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND (";
	test_filter <<  Fpl_Mc_Sim::result_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " OR " ;
	test_filter << Fpl_Mc_Sim::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " AND " ;
	test_filter << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_mc_frc_flag) << " = " << functions::convert_boolean2string(mc_flag) <<"";
	test_filter << " ORDER BY " <<  Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_waterlevel);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Warning msg;
		msg.set_msg("Fpl_Mc_Sim::select_results_in_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id, const bool mc_flag)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::result_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	number=results->rowCount();

	return number;
	
}
//Check if there are results available for a specific section-id (static)
void Fpl_Mc_Sim::check_results_available_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id, bool *mc_available, bool *frc_available){
	QSqlQueryModel results;
	//reset given pointer
	*mc_available=false;
	*frc_available=false;

	int number=0;

	//mc-results
	number=Fpl_Mc_Sim::select_results_in_database(&results, ptr_database, id, sec_id, true);
	if(number>0){
		*mc_available=true;	
	}
	//mc-results
	number=Fpl_Mc_Sim::select_results_in_database(&results, ptr_database, id, sec_id, false);
	if(number>0){
		*frc_available=true;	
	}
}
//Switch the applied-flag for the results in the database table for a defined system state (static)
void Fpl_Mc_Sim::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Mc_Sim::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::result_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::result_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::result_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Mc_Sim::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Mc_Sim::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//appending table
	Fpl_Sec_Type_Dike::switch_applied_flag_result_table(ptr_database, id, flag);
	Fpl_Sec_Type_Dune::switch_applied_flag_result_table(ptr_database, id, flag);
	Fpl_Sec_Type_Highground::switch_applied_flag_result_table(ptr_database, id, flag);
}
//Switch the applied-flag for the results in the database table for a defined system state and id (static)
void Fpl_Mc_Sim::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id){
//the table is set (the name and the column names) and allocated
	try{
		Fpl_Mc_Sim::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::result_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::result_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::result_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Mc_Sim::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Fpl_Mc_Sim::result_table->get_column_name(fpl_label::section_id) << " = " << sec_id ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Mc_Sim::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//appending table
	Fpl_Sec_Type_Dike::switch_applied_flag_result_table(ptr_database, id, flag, sec_id);
	Fpl_Sec_Type_Dune::switch_applied_flag_result_table(ptr_database, id, flag, sec_id);
	Fpl_Sec_Type_Highground::switch_applied_flag_result_table(ptr_database, id, flag, sec_id);
}
//Copy the results of a given system id to another one in database tables (static)
void Fpl_Mc_Sim::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id){
//the table is set (the name and the column names) and allocated
	try{
		Fpl_Mc_Sim::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Fpl_Mc_Sim::result_table->maximum_int_of_column(Fpl_Mc_Sim::result_table->get_column_name(fpl_label::glob_id) ,ptr_database)+1;


	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::glob_id);
	test_filter << " from " << Fpl_Mc_Sim::result_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Fpl_Mc_Sim::result_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Fpl_Mc_Sim::result_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;
	test_filter << " AND ";
	test_filter << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::section_id) << " = " << sec_id ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Mc_Sim::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::result_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
	
		test_filter.str("");
		test_filter << "INSERT INTO "<< Fpl_Mc_Sim::result_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  Fpl_Mc_Sim::result_table->get_column_name(fpl_label::section_id)  <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Fpl_Mc_Sim::result_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_mc_frc_flag)  <<" , ";
		test_filter <<  Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_waterlevel)  <<" , ";
		test_filter <<  Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_probability)  <<" , ";
		test_filter <<  Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_low_bound)  <<" , ";
		test_filter <<  Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_up_bound)  <<" , ";
		test_filter <<  Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_comp_time)  <<" , ";
		test_filter <<  Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_number_mc)  <<"  ";


		test_filter << " FROM " << Fpl_Mc_Sim::result_table->get_table_name() <<" ";
		test_filter << " WHERE " << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::glob_id) << " = ";
		test_filter << model.record(i).value(Fpl_Mc_Sim::result_table->get_column_name(fpl_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Fpl_Mc_Sim::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Fpl_Mc_Sim::result_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
	try{

		Fpl_Mc_Sim::switch_applied_flag_result_table(ptr_database, src, false,sec_id);
	}
	catch(Error msg){
		throw msg;

	}
}
//Set result data to the database result table
void Fpl_Mc_Sim::set_result2result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const bool mc_flag){
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Mc_Sim::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	//evaluate the global identifier
	int id_glob=Fpl_Mc_Sim::result_table->maximum_int_of_column(Fpl_Mc_Sim::result_table->get_column_name(fpl_label::glob_id),ptr_database)+1;
			
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Fpl_Mc_Sim::result_table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::section_id) <<" , ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(label::measure_id) <<" , ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_mc_frc_flag) <<" , ";
	//data
	query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_waterlevel) <<" , ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_probability) <<" , ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_low_bound) <<" , ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_up_bound) <<" , ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_comp_time) <<" , ";
	query_string << Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_number_mc) <<" ) ";

	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << section_id << " , " ;
	query_string << id.area_state << " , " ;
	query_string << id.measure_nr<< " , " ;
	query_string << functions::convert_boolean2string(mc_flag)<< " , " ;
	//data
	query_string << this->result_complete.waterlevel<< " , " ;
	query_string << this->result_complete.failure_prob<< " , " ;
	query_string << this->result_complete.low_failure_prob<< " , " ;
	query_string << this->result_complete.up_failure_prob<< " , " ;
	query_string << this->result_complete.time_mc<< " , " ;
	query_string << this->result_complete.number_mc<<" ) ";

	Data_Base::database_request(&model, query_string.str(), ptr_database);

	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(7);
		ostringstream info;
		info << "Table Name                : " << Fpl_Mc_Sim::result_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Fpl section id            : " << section_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);	
	}


}
//Set a result to the database result table directly
void Fpl_Mc_Sim::set_result2resulttable_direct(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const bool mc_flag, const _fpl_frc_result result){
	this->result_complete=result;
	this->set_result2result_table(ptr_database, section_id, id, mc_flag);
}
//Input the control parameters per database table
void Fpl_Mc_Sim::set_input(QSqlDatabase *ptr_database, const bool output){
	//output
	ostringstream prefix;
	ostringstream cout;
	if(output==true){
		prefix <<"INP> ";
		Sys_Common_Output::output_fpl->set_userprefix(&prefix);
		cout << "Set control parameter for the MC simulation per database..."  << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
	}
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	int number_result=0;
	//the table is set the name and the column names
	try{
		Fpl_Mc_Sim::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the complet table of control parameters FPL
	model.setTable(Fpl_Mc_Sim::table->get_table_name().c_str());
    //set the query
	Data_Base::database_request(&model);
	//check the query
	if(model.lastError().isValid()){
		//warn msg
		Warning msg=set_warning(6);
		stringstream info;
		info << "Table Name       : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info : " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
		//reaction use all default values
		number_result=0;
	}
	else{
		number_result=model.rowCount();
	}
	//read out the results
	 //the set of the name-column has to match to this parameter
	//output
	if(number_result>0){
		cout << "Search for the MC control parameters in table " <<  Fpl_Mc_Sim::table->get_table_name() <<"..."<< endl;


		//read out from the search result
		string buffer;
		for(int i=0; i< number_result; i++){
			buffer=model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name)).c_str()).toString().toStdString();
			if(buffer==fpl_control_param::mc_max){
				this->mc_max=model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toInt();
				cout << "Control parameter " << fpl_control_param::mc_max <<"  is found"  << endl;	
			}
			else if(buffer==fpl_control_param::mc_min){
				this->mc_min=model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toInt();
				cout << "Control parameter " << fpl_control_param::mc_min <<"  is found"  << endl;
				
			}
			else if(buffer==fpl_control_param::mc_error_abs){
				this->epsilon_absolut=model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toDouble();
				cout << "Control parameter " << fpl_control_param::mc_error_abs <<"  is found"  << endl;
				
			}
			else if(buffer==fpl_control_param::mc_error_rel){
				this->epsilon_relativ=(model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toDouble())/100.0;
				cout << "Control parameter " << fpl_control_param::mc_error_rel <<"  is found"  << endl;
			}
			else if(buffer==fpl_control_param::mc_confident){
				this->confidence=(model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toDouble())/100.0;
				cout << "Control parameter " << fpl_control_param::mc_confident <<"  is found"  << endl;	
			}
			else if(buffer==fpl_control_param::mc_rand_generator){
				try{
					Fpl_Mc_Sim::generator=Fpl_Mc_Sim::convert_text2generator_type((model.record(i).value((table->get_column_name(fpl_label::control_value)).c_str()).toString().toStdString()));
				}
				catch(Error msg){
					throw msg;
				}
				cout << "Control parameter " << fpl_control_param::mc_rand_generator <<"  is found"  << endl;	
			}
		}
		if(output==true){
			Sys_Common_Output::output_fpl->output_txt(cout.str(), true);
		}
	}
	//check the parameter
	this->check_mc_param();
	if(output==true){
		Sys_Common_Output::output_fpl->rewind_userprefix();
	}
	
}
//Output the control parameters of the monte-carlo simualtion to display/console
void Fpl_Mc_Sim::output_members(void){
	ostringstream prefix;
	prefix << "OUT> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	ostringstream cout;
	cout << "CONTROLPARAMETER MC-CALCULATION " <<  endl;
	cout << " Maximum number of simulation runs  : " << this->mc_max << label::no_unit<< endl;
	cout << " Minimum number of simulation runs  : " << this->mc_min << label::no_unit<< endl;
	cout << " Absolute error of abort criterion  : " << this->epsilon_absolut << label::no_unit<<  endl; 
	cout << " Relative error of abort criterion  : " << this->epsilon_relativ*100.0 << label::percentage <<  endl;
	cout << " Confidence interval                : " << this->confidence*100.0 << label::percentage << endl;
	cout << " Random generator                   : " << Fpl_Mc_Sim::convert_generator_type2text(Fpl_Mc_Sim::generator) << endl;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Perform a monte-carlo simulation started by an fragility curve simualtion (Frc_Sim) for an given fpl-section
_fpl_frc_result Fpl_Mc_Sim::make_mc_sim(const double waterlevel_frc, Fpl_Section *section_to_calc, QSqlDatabase *ptr_database){

	
	//output
	this->out_text_mcsampling(0);
	//set the waterlevel
	section_to_calc->type_of_section->set_frc_waterlevel(waterlevel_frc);
	this->reset_start_value();
	//reinitialize the random generator
	_Fpl_Distribution_Class::reinit_random();
	//for time recording
	time_t start,end;//begin and end-time of mc-simulation
	double counter_output=0.01;

	//start mc_loop
	time(&start);//begin time recording
	try{
		for(int i=0; i<this->mc_max; i++){
			this->failure=0;
			this->failure=section_to_calc->make_faulttree(true);
			//this rsult is checked for the statistic
			this->result=(double)this->failure;

			
			//check the statistics of the random variables in the section at every run for the output to the database tables
			section_to_calc->check_statistic();
	
			//check and update the statistics of the mc_sim
			this->check_statistic();
			//output the advance
			this->output_advance(i, counter_output, start);
			
			//check the abort criterion
			if((i+1)>=this->mc_min){
				this->abort_criterion();
				if(this->abort_flag){
					break;
				};
			}
			Fpl_Calculation::check_stop_thread_flag();
		}//end mc_loop
	}
	catch(Error msg){
		Sys_Common_Output::output_fpl->output_txt("\n",false);
		throw msg;
		
	}
	Sys_Common_Output::output_fpl->output_txt("\n",false);

	//time calcul
	time(&end);//end time recording
	this->time_diff=end-start;//time difference
	
	//output
	this->out_text_mcsampling(1);
	//output the stastistics of the mc-sim
	this->output_statistic();
	section_to_calc->output_result2table(ptr_database, _fpl_simulation_type::sim_frc, this->counter_calc);

	
	///make the complete result
	this->result_complete.failure_prob=this->mean_calc;
	this->result_complete.low_failure_prob=this->lower_interval_bound;
	this->result_complete.time_mc=this->time_diff;
	this->result_complete.up_failure_prob=this->upper_interval_bound;
	this->result_complete.waterlevel=waterlevel_frc;
	this->result_complete.number_mc=this->counter_calc;
	//set the results to database
	this->set_result2result_table(ptr_database, section_to_calc->get_id_section(), section_to_calc->get_sys_system_id(), false);
	
	

	if(this->stat_output==false && this->counter_calc>this->mc_min+2){
		//output the statistics of the random variables in the section
		section_to_calc->output_statistic();
		this->stat_output=true;
	}
	section_to_calc->reset_statistics();

	return this->result_complete;
}

//Perform a pure monte-carlo simulation for a given fpl-section
_fpl_frc_result Fpl_Mc_Sim::make_mc_sim(Fpl_Section *section_to_calc, QSqlDatabase *ptr_database) {
	
	//output
	this->out_text_mcsampling(0);
	this->reset_start_value();
	//reinitialize the random generator
	_Fpl_Distribution_Class::reinit_random();

	//check if the section is not set by hand or has an unknown type
	if(section_to_calc->get_type_section()!=_fpl_section_types::by_hand_sec && section_to_calc->get_type_section()!=_fpl_section_types::unknown_sec){
		//reinitialize the random generator
		_Fpl_Distribution_Class::reinit_random();
		try{
			Fpl_Mc_Sim::delete_data_in_result_table(ptr_database,section_to_calc->get_id_section(),section_to_calc->get_sys_system_id(), true);
			section_to_calc->delete_detailed_results_database(ptr_database, _fpl_simulation_type::sim_mc);
		}catch(Error msg){
			throw msg;
		}
		//for time recording
		time_t start,end;//begin and end-time of mc-simulation
		double counter_output=0.01;//for the advance to output

		//start mc_loop
		time(&start);//begin time recording
		try{
			for(int i=0; i<this->mc_max; i++){
				this->failure=0;
				this->failure=section_to_calc->make_faulttree(true);
				//this result is checked for the statistic
				this->result=(double)this->failure;

				//check the statistics of the random variables in the section
				section_to_calc->check_statistic();
				//check and update the statistics of the mc_sim
				this->check_statistic();
				//output the advance
				this->output_advance(i, counter_output, start);
			
				//check the abort criterion
				if((i+1)>=this->mc_min){
					this->abort_criterion();
					if(this->abort_flag){
						break;
					};
				}
				Fpl_Calculation::check_stop_thread_flag();

			}//end mc_loop
		}
		catch(Error msg){
			Sys_Common_Output::output_fpl->output_txt("\n",false);
			throw msg;
		}
		Sys_Common_Output::output_fpl->output_txt("\n",false);
		//time calcul
		time(&end);//end time recording
		this->time_diff=end-start;//time difference
		
		//output
		this->out_text_mcsampling(1);
		//output the stastistics of the mc-sim
		this->output_statistic();
		//output the statistics of the random variables in the section
		section_to_calc->output_statistic();
		section_to_calc->output_result2table(ptr_database, _fpl_simulation_type::sim_mc, this->counter_calc);


		
		///make the complete result
		this->result_complete.failure_prob=this->mean_calc;
		this->result_complete.low_failure_prob=this->lower_interval_bound;
		this->result_complete.time_mc=this->time_diff;
		this->result_complete.up_failure_prob=this->upper_interval_bound;
		this->result_complete.waterlevel=section_to_calc->type_of_section->waterlevel.get_calculated_mean();
		this->result_complete.number_mc=this->counter_calc;
		//set the results to database
		this->set_result2result_table(ptr_database, section_to_calc->get_id_section(), section_to_calc->get_sys_system_id(), true);
		section_to_calc->reset_statistics();
	}
	//it is an unkown section type or it is set by hand
	else{
		ostringstream cout;
		cout << "No MC-sampling is possible for section type "<< _Fpl_Section_Type::convert_section_type2txt(section_to_calc->get_type_section()) << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
			
	}
	return this->result_complete;
}
//Test the random generator and the probabilistic ditribution type with a monte-carlo analysis
void Fpl_Mc_Sim::make_mc_test_random(Fpl_Check_Distribution *random_checker, Fpl_Random_Variables *random2check){
	ostringstream cout;
	time_t start,end;//begin and end-time of mc-simulation
	double counter_output=0.01;//for the advance to output
		//reinitialize the random generator
	_Fpl_Distribution_Class::reinit_random();

	try{	
		//change mc-settings by the chekcer settings
		this->mc_max=random_checker->get_number_sim_runs();
		Fpl_Mc_Sim::generator=random_checker->get_ptr_check_dia()->get_random_generator_type();
		this->output_check_members();

		//set a Random Variable by dialog
		random2check->output_member( false);

		//set all for one MC simulation run (e.g. 1000000 samplings) needed values to start value
		this->reset_start_value();
		//reinitialize the random generator
		_Fpl_Distribution_Class::reinit_random();

		cout <<"Start the random generation process..." << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
		time(&start);//begin time recording
	}
	catch(Error msg){
		throw msg;
	}

	try{
		for(int i=0; i<this->mc_max; i++){
			
			//generate the random numbers and transform them
			this->result=random2check->calculate_variables(true).end_result;
			random_checker->set_result_one_run(this->result);
			//check the numbers
			random2check->check_statistic();
			random2check->reset_random_generated();
			//check and update the statistics of the mc_sim
			this->check_statistic();
			//output the advance of the mc
			this->output_advance(i, counter_output, start);
			Fpl_Calculation::check_stop_thread_flag();
		}//end mc_loop
	}
	catch(Error msg){
		cout << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
		throw msg;
	}
	try{
		cout << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
		//time calcul
		time(&end);
		this->time_diff=end-start;
		//output end
		cout <<"Random generation process is finished" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
		random_checker->perform_end_analysis();
		//output the statistics of the checked variable
		random2check->output_statistic( false);
		//output the statistics of the mc-sim
		this->output_statistic_checking(random_checker->get_error());
	}
	catch(Error msg){
		throw msg;
	}
}
//Convert a simulation type (_fpl_simulation_type) into a text string (static)
string Fpl_Mc_Sim::convert_sim_type2text(_fpl_simulation_type type){
	string buffer;
	switch(type){
		case _fpl_simulation_type::sim_determ:
			buffer=fpl_label::determ_simulation;
			break;
		case _fpl_simulation_type::sim_mc:
			buffer=fpl_label::mc_simulation;
			break;
		case _fpl_simulation_type::sim_frc:
			buffer=fpl_label::frc_simulation;
			break;
		default:
			buffer=label::unknown_type;
	}


	return buffer;
}
//Convert a text string into a simulation type (_fpl_simulation_type) (static)
_fpl_simulation_type Fpl_Mc_Sim::convert_text2sim_type(const string buffer){

	_fpl_simulation_type type;

	if(buffer==fpl_label::determ_simulation){
		type=_fpl_simulation_type::sim_determ;
	}
	else if(buffer==fpl_label::mc_simulation){
		type=_fpl_simulation_type::sim_mc;
	}
	else if(buffer==fpl_label::frc_simulation){
		type=_fpl_simulation_type::sim_frc;
	}
	else{
		Error msg;
		msg.set_msg("Fpl_Mc_Sim::convert_text2sim_type(const string buffer)", "Can not convert this simulation type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert simulation type: " << buffer << endl;
		info << "Possible types are: "<< endl;
		info << " "<<fpl_label::determ_simulation  << endl;
		info << " "<<fpl_label::mc_simulation  << endl;
		info << " "<<fpl_label::frc_simulation  << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	return type;
}
//Convert a random generator type (_fpl_random_generator_type) into a text string (static)
string Fpl_Mc_Sim::convert_generator_type2text(_fpl_random_generator_type type){
	string buffer;
	switch(type){
		case _fpl_random_generator_type::gen_type_zikurat:
			buffer=fpl_label::generator_zikurat;
			break;
		case _fpl_random_generator_type::gen_type_ranmar:
			buffer=fpl_label::generator_ranmar;
			break;
		case _fpl_random_generator_type::gen_type_MofAll:
			buffer=fpl_label::generator_MofAll;
			break;
		case _fpl_random_generator_type::gen_type_sfmt:
			buffer=fpl_label::generator_sfmt;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//Convert a text string into a random generator type (_fpl_random_generator_type) (static)
_fpl_random_generator_type Fpl_Mc_Sim::convert_text2generator_type(const string buffer){
	_fpl_random_generator_type type;

	if(buffer==fpl_label::generator_zikurat){
		type=_fpl_random_generator_type::gen_type_zikurat;
	}
	else if(buffer==fpl_label::generator_ranmar){
		type=_fpl_random_generator_type::gen_type_ranmar;
	}
	else if(buffer==fpl_label::generator_MofAll){
		type=_fpl_random_generator_type::gen_type_MofAll;
	}
	else if(buffer==fpl_label::generator_sfmt){
		type=_fpl_random_generator_type::gen_type_sfmt;
	}
	else{
		Error msg;
		msg.set_msg("Fpl_Mc_Sim::convert_text2generator_type(const string buffer)", "Can not convert this random generator type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert random generator type: " << buffer << endl;
		info << "Possible types are: "<< endl;
		info << " "<<fpl_label::generator_ranmar  << endl;
		info << " "<<fpl_label::generator_zikurat  << endl;
		info << " "<<fpl_label::generator_MofAll << endl;
		info << " "<<fpl_label::generator_sfmt<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return type;
}
//Get the type of the random generator (static)
_fpl_random_generator_type Fpl_Mc_Sim::get_generator_type(void){
	return Fpl_Mc_Sim::generator;
}
//___________
//private
//Determine the alpha-factor from the given conconfidence intervall (e.g. 95%)
void Fpl_Mc_Sim::calculate_alpha(void){
/** The determination of the alpha-factor is done with the Standard Normal distribution
approximated by the inverse of the error-function.
*/
		double prob=0.0;
		double x_value=0.0;
		const double a_value=0.147;
		const double coeff1=2.0/(constant::Cpi*a_value);
		//calculated prob for input in the inverse errorfunction as an approximation for the N(0,1)
		prob=1.0-(1.0-confidence)/2.0;//divided by 2, because of a an interval where the mean_calc is (upper; lower boundary)
		prob=2.0*prob-1.0;
		//approximation
		prob=1.0-pow(prob,2.0);
		prob=log(prob);
		x_value=pow(2.0,0.5)*pow((-coeff1-prob/2.0+pow((pow((coeff1+prob/2.0),2.0)-1.0/a_value*prob),0.5)),0.5);
		this->alpha=x_value;		
}
//Check the control parameter of the monte carlo simulation
void Fpl_Mc_Sim::check_mc_param(void){
	if(this->mc_max < 100){
		Warning msg=this->set_warning(0);
		msg.output_msg(1);
		//reaction use default values
		this->mc_max=fpl_control_param::mc_max_def;
	}
	if(this->mc_min < 0){
		Warning msg=this->set_warning(1);
		msg.output_msg(1);
		//reaction use default values
		this->mc_min=fpl_control_param::mc_min_def;
	}
	//mc_max have to be greater than mc_min
	if(this->mc_min>this->mc_max){
		Warning msg=this->set_warning(2);
		msg.output_msg(1);
		//reaction
		this->mc_min=this->mc_max/100.0;
	}
	if(this->confidence< 0.0 || this->confidence >=1.0){
		Warning msg=this->set_warning(3);
		msg.output_msg(1);
		//reaction
		this->confidence=fpl_control_param::confidence_def;
	}
	if(this->epsilon_absolut<=0.0){
		Warning msg=this->set_warning(4);
		msg.output_msg(1);
		//reaction
		this->epsilon_absolut=fpl_control_param::epsilon_absolut_def;
	}
	if(this->epsilon_relativ<=0.0){
		Warning msg=this->set_warning(5);
		msg.output_msg(1);
		//reaction
		this->epsilon_relativ=fpl_control_param::epsilon_relativ_def;
	}
}

//Update the mean value and the standard deviation (statistics) per simulation run
void Fpl_Mc_Sim::check_statistic(void){

	//counting the numbers of generation steps
	this->counter_calc++;
	//meanvalue updating
	this->mean_calc=this->mean_calc_old+((this->result)-this->mean_calc_old)/(double)this->counter_calc;
	//varianz updating first by 2 generation steps
	if(this->counter_calc>2){
		this->varianz_calc=this->varianz_calc*((double)(this->counter_calc-2)/(double)(this->counter_calc-1))+(double)this->counter_calc*pow((this->mean_calc-this->mean_calc_old),2);
		this->std_dev_calc=pow(this->varianz_calc,0.5);
	}
	//storing of the actuell values to the old ones
	this->mean_calc_old=this->mean_calc;
}
//Output to the statistics of the monte carlo simulation to display/console
void Fpl_Mc_Sim::output_statistic(void){
	ostringstream cout;
	cout << "STATISTICS MC-SIMULATION" << endl;
	cout << " Number of generation         : " << this->counter_calc<< endl;
	cout << " Mean value                   : " << this->mean_calc << endl;
	cout << " Standard deviation           : " << this->std_dev_calc << endl;
	cout << " Lower boundary  (confidence) : " << this->lower_interval_bound << " ("<<this->confidence*100.0<<label::percentage<<")"<<endl;
	cout << " Upper boundary  (confidence) : " << this->upper_interval_bound << " ("<<this->confidence*100.0<<label::percentage<<")"<<endl;
	cout << " Computation time             : " << this->time_diff << label::sec << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);

	
	cout << "ABORT CRITERION MC-SIMULATION" <<endl;
	if(this->counter_calc==this->mc_max){
		cout << " Maximum Numbers of MC-Simulations are reached" << endl;
		cout << "  Absolut epsilon given       : " << this->epsilon_absolut << endl;
		cout << "  Absolut epsilon calculated  : " << this->epsilon_calc_absolut <<  endl;
		cout << "  Relative epsilon given      : " << this->epsilon_relativ *100.0 << label::percentage<< endl;
		cout << "  Relative epsilon calculated :" << this->epsilon_calc_relativ*100.0 << label::percentage<< endl;
	}
	else if(this->abort_flag==true && this->mean_calc!=0.0){
		cout << " Abort Criterion is reached" << endl;
	}
	else if(this->abort_flag==true && this->mean_calc==0.0){
		cout << " The calculated mean is constant 0.0 => abort the MC-Simulation " << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	
}
//Output to the statistics of the monte carlo simulation for random variable checking to display/console
void Fpl_Mc_Sim::output_statistic_checking(const double error_check){
	ostringstream cout;
	cout << "STATISTICS MC-SIMULATION CHECK" << endl;
	cout << " Number of generation         : " << this->counter_calc<< endl;
	cout << " Mean value                   : " << this->mean_calc << endl;
	cout << " Standard deviation           : " << this->std_dev_calc << endl;
	cout << " Computation time             : " <<  this->time_diff << label::sec << endl;
	cout << " Mid error (gen2calc)         : " <<error_check << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
}
//Reset the start values of the monte carlo simulation, which are used for the abort criterion
void Fpl_Mc_Sim::reset_start_value(void){
	//for the calculation of the statistics
	this->mean_calc=0.0;
	this->std_dev_calc=0.0;
	this->counter_calc=0;
	this->mean_calc_old=0.0;
	this->varianz_calc=0.0;
	//for the abort criterion
	this->abort_flag=false;
	this->epsilon_calc_absolut=0.0;
	this->epsilon_calc_relativ=0.0;
	this->lower_interval_bound=0.0;
	this->upper_interval_bound=0.0;
	//results
	this->failure=0;
	this->result=0.0;
	this->result_complete.failure_prob=0.0;
	this->result_complete.low_failure_prob=0.0;
	this->result_complete.time_mc=0.0;
	this->result_complete.up_failure_prob=0.0;
	this->result_complete.waterlevel=0.0;
	this->result_complete.number_mc=0;
}
//Check the abort criterion for abort the simulation
void Fpl_Mc_Sim::abort_criterion(void){
	if(this->mean_calc==0.0){
		this->abort_flag=true;
	}
	else{
		this->epsilon_calc_absolut=this->std_dev_calc*this->alpha/(pow(this->counter_calc,0.5));
		this->epsilon_calc_relativ=this->std_dev_calc*this->alpha/(this->mean_calc*pow(this->counter_calc,0.5));
		this->lower_interval_bound=this->mean_calc-this->alpha*this->std_dev_calc/pow(this->counter_calc,0.5);
		if(this->lower_interval_bound<0.0){
			this->lower_interval_bound=0.0;
		}
		this->upper_interval_bound=this->mean_calc+this->alpha*this->std_dev_calc/pow(this->counter_calc,0.5);
		if(this->epsilon_calc_absolut<=this->epsilon_absolut && this->epsilon_calc_relativ <= this->epsilon_relativ){//abort criterion is reached
			this->abort_flag=true;
		}
	}
}
//Output the advance of the monte carlo simualtion to display/console
void Fpl_Mc_Sim::output_advance(const int counter_calc_var, double& counter_output, time_t start_time){
	double step=0.01;
	ostringstream cout;
	int output=(int)((double)mc_max*counter_output);
	if(output<1){
		output=1;
	}
	if(counter_calc_var+1==1){
		//get time
		struct tm *timeinfo;
		char buffer [60];
		int whitespaces=(int)(1.0/counter_output);
		timeinfo = localtime (&start_time);
		strftime (buffer,60,"%X",timeinfo);
		ostringstream prefix;
		prefix << "CALC"<<"> ";
		Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
		cout << "Starttime " << buffer << endl;
		cout << " 0 %"<<setw(whitespaces)<<"100 % MC_MAX " << endl;
		cout << " >";
		Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		Sys_Common_Output::output_fpl->rewind_userprefix();
	}
	else if(counter_calc_var==output){
		cout << ">";
		Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		counter_output=counter_output+step;
	}

}
//output a text header for the monte carlo simulation corresponding the given type to display/console
void Fpl_Mc_Sim::out_text_mcsampling(const int type){
	ostringstream cout;
	//output
	Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
	switch(type){
		case 0://begin	
			cout << "Start the MC-sampling..."<< endl ;
			break;
		case 1://normal end
			cout << "MC-sampling finished"<< endl ;
			break;
		default:
			break;
	}
	Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
}
//Output the members for the checking of the random genertaion and the transformation via a monte carlo simualtion to display/console
void Fpl_Mc_Sim::output_check_members(void){
	ostringstream cout;
	cout << "CONTROLPARAMETER MC-CALCULATION CHECKING" <<  endl;
	cout << " Maximum number of simulation runs  : " << this->mc_max << label::no_unit<< endl;
	cout << " Random generator                   : " << Fpl_Mc_Sim::convert_generator_type2text(Fpl_Mc_Sim::generator) << endl;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
}
//Set warning(s)
Warning Fpl_Mc_Sim::set_warning(const int warn_type){
		string place="McSim::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0:
			place.append("check_mc_param(void)") ;
			reason="Maximal number of MC-Calculation is < 100";
			reaction="The default value is taken";
			help= "Check the parameter MC_MAX";
			info << "Default value : " << fpl_control_param::mc_max_def << endl;
			type=6;	
			break;
		case 1:
			place.append("check_mc_param(void)") ;
			reason="Minimal number of MC-Calculation before the Abort criterion is checked is < 0";
			reaction="The default value is taken";
			help= "Check the parameter MC_MIN";
			info << "Default value : " << fpl_control_param::mc_min_def << endl;
			type=6;	
			break;
		case 2:
			place.append("check_mc_param(void)") ;
			reason="Minimal number of MC-Calculation is greater than maximal number";
			reaction="The minimal number is to 1/100 of the maximal number";
			help= "Check the parameters MC_MIN and MC_MAX" ;
			info << "New minimal number : " << mc_max/100.0 << endl;
			type=6;	
			break;
		case 3:
			place.append("check_mc_param(void)") ;
			reason="The confidence number must be in the interval (0,1)";
			reaction="The default value is taken";
			help= "Check the parameter MC_CONFIDENCE_INTERVALL_[%]" ;
			info << "Default value : " << fpl_control_param::confidence_def << endl;
			type=6;	
			break;
		case 4:
			place.append("check_mc_param(void)") ;
			reason="The absolute error is <= 0";
			reaction="The default value is taken";
			help= "Check the parameter MC_ERROR_ABSOLUTE";
			info << "Default value : " << fpl_control_param::epsilon_absolut_def << endl;
			type=6;	
			break;
		case 5:
			place.append("check_mc_param(void)") ;
			reason="The relativ error is not in the intervall (0,100)[%]";
			reaction="The default value is taken";
			help= "Check the parameter MC_ERROR_RELATIVE_[%]";
			info << "Default value : " << fpl_control_param::epsilon_relativ_def*100.0 << " %"<< endl;
			type=6;	
			break;
		case 6:
			place.append("set_input(QSqlDatabase *ptr_database)") ;
			reason="Table not found";
			help="Check the table name";
			reaction="All default values are taken";
			type=2;	
			break;
		case 7://results datas can not submitted
			place.append("set_result2result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const bool mc_flag)");
			reason="Can not submit the reliability result data to the database";
			help="Check the database";
			type=2;
			break;
		default:
			place.append("set_warning(const int warn_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;
};




