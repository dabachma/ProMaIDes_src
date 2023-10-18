#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Frc_Curve.h"

//init static members
int Fpl_Frc_Curve::number_intervals=2;
Tables *Fpl_Frc_Curve::table_intervals=NULL;

//Default constructor
Fpl_Frc_Curve::Fpl_Frc_Curve(void){

	this->number_points=0;
	this->points=NULL;

	this->intervals=NULL;
	this->in_nobreak_interval=false;

	this->current_waterlevel=0.0;
	this->current_index_intervall=-1;

	this->section_is_checked=false;


	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Frc_Curve),_sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Frc_Curve::~Fpl_Frc_Curve(void){
	this->delete_points();
	this->delete_intervals();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Frc_Curve),_sys_system_modules::FPL_SYS);//count the memory
}
//_________
//public
//Set the database table for the frc-intervals: it sets the table name and the name of the columns and allocate them
void Fpl_Frc_Curve::set_table_intervals(QSqlDatabase *ptr_database){
	if(Fpl_Frc_Curve::table_intervals==NULL){

		//make specific input for this class
		const string tab_id_name=fpl_label::tab_fpl_frc_interval;
		string tab_col[11];

		tab_col[0]=label::glob_id;
		tab_col[1]=fpl_label::section_id;
		tab_col[2]=label::areastate_id;
		tab_col[3]=label::measure_id;
		tab_col[4]=fpl_label::id_frc_interval;
		tab_col[5]=fpl_label::break_height;
		tab_col[6]=hyd_label::sz_bound_id;
		tab_col[7]=fpl_label::interval_low_prob;
		tab_col[8]=fpl_label::interval_low_h;
		tab_col[9]=fpl_label::interval_up_prob;
		tab_col[10]=fpl_label::interval_up_h;

		try{
			Fpl_Frc_Curve::table_intervals= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Frc_Curve::table_intervals->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("Fpl_Frc_Curve::set_table_intervals(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Frc_Curve::close_table_intervals();
			throw msg;
		}
	}
}
//Create the database table for the frc-intervals
void Fpl_Frc_Curve::create_table_intervals(QSqlDatabase *ptr_database){
		if(Fpl_Frc_Curve::table_intervals==NULL){
			ostringstream cout;
			cout << "Create database table of frc-interval data..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_fpl_frc_interval;
			const int num_col=11;
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

			tab_col[4].name=hyd_label::sz_bound_id;
			tab_col[4].type=sys_label::tab_col_type_int;
			tab_col[4].unsigned_flag=true;
			tab_col[4].key_flag=true;

			tab_col[5].name=fpl_label::id_frc_interval;
			tab_col[5].type=sys_label::tab_col_type_int;
			tab_col[5].unsigned_flag=true;
			tab_col[5].key_flag=true;

			tab_col[6].name=fpl_label::break_height;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].unsigned_flag=true;

			tab_col[7].name=fpl_label::interval_low_prob;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].unsigned_flag=true;

			tab_col[8].name=fpl_label::interval_low_h;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=fpl_label::interval_up_prob;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].unsigned_flag=true;

			tab_col[10].name=fpl_label::interval_up_h;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].unsigned_flag=true;




			try{
				Fpl_Frc_Curve::table_intervals= new Tables();
				if(Fpl_Frc_Curve::table_intervals->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Frc_Curve::create_table_intervals(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Frc_Curve::close_table_intervals();
				throw msg;
			}
			Fpl_Frc_Curve::close_table_intervals();		
	}
}
//Delete all data in the database table for the frc-intervals
void Fpl_Frc_Curve::delete_data_in_table_intervals(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Frc_Curve::set_table_intervals(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Fpl_Frc_Curve::table_intervals->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the frc-intervals for the given specifier
void Fpl_Frc_Curve::delete_data_in_table_intervals(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const int sz_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Frc_Curve::set_table_intervals(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Fpl_Frc_Curve::table_intervals->get_table_name();
	query_string << " WHERE " ;
	query_string << Fpl_Frc_Curve::table_intervals->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Fpl_Frc_Curve::table_intervals->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Fpl_Frc_Curve::table_intervals->get_column_name(hyd_label::sz_bound_id) << " = " << sz_bound_id;
	query_string << " AND ";
	query_string << Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::section_id) << " = " << section_id ;
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Frc_Curve::delete_data_in_table_intervals(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const int sz_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " <<Fpl_Frc_Curve::table_intervals->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for the frc-intervals for the given specifier
void Fpl_Frc_Curve::delete_data_in_table_intervals(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Frc_Curve::set_table_intervals(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Fpl_Frc_Curve::table_intervals->get_table_name();
	query_string << " WHERE " ;
	query_string << Fpl_Frc_Curve::table_intervals->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Fpl_Frc_Curve::table_intervals->get_column_name(label::measure_id) << " = " << id.measure_nr;
		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Frc_Curve::delete_data_in_table_intervals(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " <<Fpl_Frc_Curve::table_intervals->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Close and delete the database table for the frc-intervals
void Fpl_Frc_Curve::close_table_intervals(void){
	if(Fpl_Frc_Curve::table_intervals!=NULL){
		delete Fpl_Frc_Curve::table_intervals;
		Fpl_Frc_Curve::table_intervals=NULL;
	}
}
///Get the break height form database table by the given specifier (static)
double Fpl_Frc_Curve::get_break_height(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const int sz_bound_id, int interval_id){
	double break_h=0.0;
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Frc_Curve::set_table_intervals(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQueryModel query;
	ostringstream query_string;
	query_string <<"SELECT * FROM " << Fpl_Frc_Curve::table_intervals->get_table_name();
	query_string << " WHERE " ;
	query_string << Fpl_Frc_Curve::table_intervals->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Fpl_Frc_Curve::table_intervals->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Fpl_Frc_Curve::table_intervals->get_column_name(hyd_label::sz_bound_id) << " = " << sz_bound_id;
	query_string << " AND ";
	query_string << Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::section_id) << " = " << section_id ;
	query_string << " AND ";
	query_string << Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::id_frc_interval) << " = " << interval_id ;
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Frc_Curve::get_break_height(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const int sz_bound_id, int interval_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " <<Fpl_Frc_Curve::table_intervals->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(query.rowCount()==0){
		Warning msg;
		msg.set_msg("Fpl_Frc_Curve::get_break_height(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const int sz_bound_id, int interval_id)","No results are found in database table", "Check the database","Can not load the break height", 21);
		ostringstream info;
		info << "Table Name                     : " <<Fpl_Frc_Curve::table_intervals->get_table_name() << endl;
		info << "Section id                     : " << section_id << endl;
		info << "Hydraulic boundary scenario id : " << sz_bound_id << endl;
		info << "FRC-interval id                : " << interval_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(5);
		return 9999.0;
	}

	break_h=query.record(0).value((Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::break_height)).c_str()).toDouble();

	return break_h;

}
//Copy the results of a given system id to another one in database tables
void Fpl_Frc_Curve::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
//the table is set (the name and the column names) and allocated
	try{
		Fpl_Frc_Curve::set_table_intervals(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Fpl_Frc_Curve::table_intervals->maximum_int_of_column(Fpl_Frc_Curve::table_intervals->get_column_name(label::glob_id) ,ptr_database)+1;



	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Fpl_Frc_Curve::table_intervals->get_column_name(label::glob_id);
	test_filter << " from " << Fpl_Frc_Curve::table_intervals->get_table_name();
	test_filter << " WHERE ";
	test_filter << Fpl_Frc_Curve::table_intervals->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Fpl_Frc_Curve::table_intervals->get_column_name(label::measure_id) << " = " << src.measure_nr ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Frc_Curve::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Frc_Curve::table_intervals->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
	
		test_filter.str("");
		test_filter << "INSERT INTO "<< Fpl_Frc_Curve::table_intervals->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::section_id)  <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Fpl_Frc_Curve::table_intervals->get_column_name(hyd_label::sz_bound_id)  <<" , ";
		test_filter <<  Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::id_frc_interval)  <<" , ";
		test_filter <<  Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::break_height)  <<" , ";
		test_filter <<  Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::interval_low_prob)  <<" , ";
		test_filter <<  Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::interval_low_h)  <<" , ";
		test_filter <<  Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::interval_up_prob)  <<" , ";
		test_filter <<  Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::interval_up_h)  <<"  ";

		test_filter << " FROM " << Fpl_Frc_Curve::table_intervals->get_table_name() <<" ";
		test_filter << " WHERE " << Fpl_Frc_Curve::table_intervals->get_column_name(label::glob_id) << " = ";
		test_filter << model.record(i).value(Fpl_Frc_Curve::table_intervals->get_column_name(label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Fpl_Frc_Curve::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Fpl_Frc_Curve::table_intervals->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
}
//Set the curve per database
void Fpl_Frc_Curve::set_curve_data(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	QSqlQueryModel model;
	
	this->number_points=Fpl_Mc_Sim::select_results_in_database(&model, ptr_database, id, section_id, false);


	if(this->number_points<=1){
		Error msg=this->set_error(1);
		ostringstream info;
		info<<"Number of points found : " << this->number_points << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	this->allocate_points();


	for(int i=0; i< this->number_points; i++){
		this->points[i].waterlevel=model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_waterlevel)).c_str()).toDouble();
		this->points[i].failure_prob=model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_probability)).c_str()).toDouble();
		this->points[i].up_failure_prob=model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_up_bound)).c_str()).toDouble();
		this->points[i].low_failure_prob=model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_low_bound)).c_str()).toDouble();
		this->points[i].number_mc=model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_number_mc)).c_str()).toDouble();
		this->points[i].time_mc=model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_comp_time)).c_str()).toDouble();
	}
}
//Transfer the data of the intervals to the database table
void Fpl_Frc_Curve::transfer_intervals2database(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const int sz_bound_id){

	try{
		Fpl_Frc_Curve::delete_data_in_table_intervals(ptr_database, id, section_id, sz_bound_id);
		Fpl_Frc_Curve::set_table_intervals(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}


	int id_glob=Fpl_Frc_Curve::table_intervals->maximum_int_of_column(Fpl_Frc_Curve::table_intervals->get_column_name(label::glob_id), ptr_database)+1;


	//transfer the data
	ostringstream header_string;
	header_string << "INSERT INTO  " << Fpl_Frc_Curve::table_intervals->get_table_name();
	header_string <<" ( ";
	header_string << Fpl_Frc_Curve::table_intervals->get_column_name(label::glob_id) <<" , ";
	header_string << Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::section_id) <<" , ";
	header_string << Fpl_Frc_Curve::table_intervals->get_column_name(label::areastate_id) <<" , ";
	header_string << Fpl_Frc_Curve::table_intervals->get_column_name(label::measure_id) <<" , ";
	header_string << Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::id_frc_interval) <<" , ";
	header_string << Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::break_height) <<" , ";
	header_string << Fpl_Frc_Curve::table_intervals->get_column_name(hyd_label::sz_bound_id)  <<" , ";

	header_string << Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::interval_low_prob) <<" , ";
	header_string << Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::interval_low_h) <<" , ";
	header_string << Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::interval_up_prob) <<" , ";
	header_string << Fpl_Frc_Curve::table_intervals->get_column_name(fpl_label::interval_up_h);

	header_string <<" ) ";
	header_string <<" VALUES ";


	ostringstream query_string;

	for(int i=0; i< this->number_intervals-1; i++){

		query_string << " ( ";

		query_string << id_glob << " , " ;
		query_string << section_id<< " , " ;
		query_string << id.area_state << " , " ;
		query_string << id.measure_nr << " , " ;
		query_string << this->intervals[i].id_str << " , " ;
		query_string << this->intervals[i].break_waterlevel << " , " ;
		query_string << sz_bound_id << " , " ;
		query_string << this->intervals[i].interval_low.failure_prob << " , " ;
		query_string << this->intervals[i].interval_low.waterlevel << " , " ;
		query_string << this->intervals[i].interval_up.failure_prob << " , " ;
		query_string << this->intervals[i].interval_up.waterlevel << " ) " ;
		if(i<this->number_intervals-2){
			query_string << " , ";
		}

		id_glob++;
	}
	ostringstream total;
	total<<header_string.str() << query_string.str();

	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, total.str(), ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Fpl_Frc_Curve::table_intervals->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Section index             : " << section_id << endl;
		info << "Id hydraulic boundary     : " << sz_bound_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);	
	}

}
//Interpolate value
_fpl_frc_result Fpl_Frc_Curve::interpolate_value(const double waterlevel, const bool break_flag){
	_fpl_frc_result buffer;
	buffer.failure_prob=0.0;
	buffer.low_failure_prob=0.0;
	buffer.up_failure_prob=0.0;
	buffer.time_mc=0.0;
	buffer.waterlevel=waterlevel;
	buffer.number_mc=0;


	if(waterlevel<=0.0){
		buffer.failure_prob=0.0;
		buffer.low_failure_prob=0.0;
		buffer.up_failure_prob=0.0;
		buffer.waterlevel=0.0;
	}
	else if(waterlevel>this->points[this->number_points-1].waterlevel){
		buffer.failure_prob=this->points[this->number_points-1].failure_prob;
		buffer.low_failure_prob=this->points[this->number_points-1].low_failure_prob;
		buffer.up_failure_prob=this->points[this->number_points-1].up_failure_prob;
	}
	else if(waterlevel<this->points[0].waterlevel){
		buffer.failure_prob=0.0;
		buffer.low_failure_prob=0.0;
		buffer.up_failure_prob=0.0;
	}
	else{


		//search for the values
		int store_index=0;
		bool interpolate=false;
		for(int i=0; i< this->number_points; i++){
			if(this->points[i].waterlevel==waterlevel){
				buffer.failure_prob=this->points[i].failure_prob;
				buffer.low_failure_prob=this->points[i].low_failure_prob;
				buffer.up_failure_prob=this->points[i].up_failure_prob;
				break;
			}
			else if(waterlevel<this->points[i].waterlevel){
				store_index=i;
				interpolate=true;
				break;
			}
		}
		
		if(interpolate==true){
			//interpolate
			double delta_h=(this->points[store_index].waterlevel-this->points[store_index-1].waterlevel);
			double delta_x=(waterlevel-this->points[store_index-1].waterlevel);

			buffer.failure_prob=this->points[store_index-1].failure_prob+
				(delta_x*(this->points[store_index].failure_prob-this->points[store_index-1].failure_prob))/
				delta_h;

			buffer.low_failure_prob=this->points[store_index-1].low_failure_prob+
				(delta_x*(this->points[store_index].low_failure_prob-this->points[store_index-1].low_failure_prob))/
				delta_h;

			buffer.up_failure_prob=this->points[store_index-1].up_failure_prob+
				(delta_x*(this->points[store_index].up_failure_prob-this->points[store_index-1].up_failure_prob))/
				delta_h;
		}
	}

	//make non-failure prob
	if(break_flag==false){
		buffer.failure_prob=1.0-buffer.failure_prob;
		buffer.low_failure_prob=1.0-buffer.low_failure_prob;
		buffer.up_failure_prob=1.0-buffer.up_failure_prob;
	}

	return buffer;
}
//Initialize the intervals (test)
void Fpl_Frc_Curve::init_intervals(const double max_waterlevel){
	
	this->allocate_intervals();
	double relevant_max_h=max_waterlevel;

	if(relevant_max_h<0.0){
		relevant_max_h=1e-10;
	}

	ostringstream buffer;

	//low boundary of the first interval
	this->intervals[0].interval_low.waterlevel=0.0;
	this->intervals[0].interval_low.failure_prob=this->interpolate_value(this->intervals[0].interval_low.waterlevel, true).failure_prob;
	
	//check the maximum waterlevl
	if(max_waterlevel>=this->points[this->number_points-1].waterlevel){
		relevant_max_h=this->points[this->number_points-1].waterlevel;
	}
	
	//the last interval
	this->intervals[Fpl_Frc_Curve::number_intervals-1].interval_low.waterlevel=relevant_max_h;
	this->intervals[Fpl_Frc_Curve::number_intervals-1].interval_low.failure_prob=this->interpolate_value(this->intervals[Fpl_Frc_Curve::number_intervals-1].interval_low.waterlevel, true).failure_prob;

	this->intervals[Fpl_Frc_Curve::number_intervals-1].interval_up.waterlevel=relevant_max_h+0.1;
	this->intervals[Fpl_Frc_Curve::number_intervals-1].interval_up.failure_prob=this->interpolate_value(this->intervals[Fpl_Frc_Curve::number_intervals-1].interval_up.waterlevel, true).failure_prob;

	this->intervals[Fpl_Frc_Curve::number_intervals-1].interval_mid=
		(this->intervals[Fpl_Frc_Curve::number_intervals-1].interval_up.waterlevel+this->intervals[Fpl_Frc_Curve::number_intervals-1].interval_low.waterlevel)*0.5;
	buffer<< FORMAT_FIXED_REAL << P(2)<<this->intervals[Fpl_Frc_Curve::number_intervals-1].interval_mid;


	this->intervals[Fpl_Frc_Curve::number_intervals-1].id_str="N";
	this->intervals[Fpl_Frc_Curve::number_intervals-1].break_waterlevel=9999.0;
	

	buffer.str("");

	double steps=relevant_max_h/(Fpl_Frc_Curve::number_intervals-1);

	int counter_interval=Fpl_Frc_Curve::number_intervals-2;

	for(int i=0; i<Fpl_Frc_Curve::number_intervals-1; i++){
		this->intervals[i].interval_up.waterlevel=this->intervals[i].interval_low.waterlevel+steps;
		this->intervals[i].interval_up.failure_prob=this->interpolate_value(this->intervals[i].interval_up.waterlevel, true).failure_prob;

		if((i+1)<Fpl_Frc_Curve::number_intervals){
			this->intervals[i+1].interval_low.waterlevel=this->intervals[i].interval_up.waterlevel;
			this->intervals[i+1].interval_low.failure_prob=this->interpolate_value(this->intervals[i+1].interval_low.waterlevel, true).failure_prob;

		}

		this->intervals[i].interval_mid=0.5*(this->intervals[i].interval_up.waterlevel+this->intervals[i].interval_low.waterlevel);
		this->intervals[i].break_waterlevel=this->intervals[i].interval_low.waterlevel+2.0/3.0*(this->intervals[i].interval_up.waterlevel-this->intervals[i].interval_low.waterlevel);
		buffer<<counter_interval;
		counter_interval--;
		this->intervals[i].id_str=buffer.str();
		buffer.str("");
	}
}
//Get the intervals identifier for a given random value (test)
string Fpl_Frc_Curve::get_interval_identifier(_Fpl_Random_Generator *ran_gen, const int number_try, const double simil_factor){

	int index=-1;
	int index_min=-1;
	double waterlevel=0.0;
	double waterlevel_min=9999.0;
	double random=0.0;
	double old_random=0.0;
	double buff_simil_fac=simil_factor;
	if(buff_simil_fac<0.0){
		buff_simil_fac=0.0;
	}


	//applying the length effect; produce correlated random variables
	for(int i=0; i<number_try; i++){
		random=ran_gen->run_generator();
		if(i>0){
			//random=old_random*buff_simil_fac+random*pow((1.0-pow(buff_simil_fac,2.0)),0.5);
			//just use a linear combination of the random variables
			random=old_random*buff_simil_fac+random*(1.0-buff_simil_fac);
		}
		index=this->find_index_intervals(random, &waterlevel);
		if(waterlevel<waterlevel_min){
			waterlevel_min=waterlevel;
			index_min=index;
			if(index_min==this->number_intervals-2){
				break;
			}
		}
		old_random=random;
	}


	if(this->in_nobreak_interval==false){
		this->intervals[index_min].number_occ++;
	}

	this->current_waterlevel=waterlevel;
	this->current_index_intervall=index_min;

	if(index_min<0){
		this->in_nobreak_interval=true;
		return "x";
	}
	else{
		return this->intervals[index_min].id_str;
	}
}
//Get most probable interval not taken into account the nobreak interval
string Fpl_Frc_Curve::get_most_probable_nobreak_identifier(double *prob){

	double delta_p=0.0;
	double delta_p_max=-1.0;
	int index=-1;
	for(int i=0; i< this->number_intervals-1; i++){
		delta_p=this->intervals[i].interval_up.failure_prob-this->intervals[i].interval_low.failure_prob;
		if(delta_p>delta_p_max){
			index=i;
			delta_p_max=delta_p;
		}
	}
	*prob=delta_p_max;
	return this->intervals[index].id_str;
}
//Get the number of intervals (static)
int Fpl_Frc_Curve::get_number_intervals(void){
	return Fpl_Frc_Curve::number_intervals;
}
//Set the number of intervals 
void Fpl_Frc_Curve::set_number_intervals(const int number_intervals){
	Fpl_Frc_Curve::number_intervals=number_intervals;
}
//Get if the selected interval is the nobreak interval (test)
bool Fpl_Frc_Curve::get_in_nobreak_interval(void){
	return this->in_nobreak_interval;
}
//Set if this section is a nobreak section
void Fpl_Frc_Curve::set_in_nobreak_interval(const bool in_nobreak){
	this->in_nobreak_interval=in_nobreak;
}
//Get the probability of the nobreak-interval
double Fpl_Frc_Curve::get_prob_nobreak_interval(void){
	return this->intervals[this->number_intervals-1].interval_low.failure_prob;
}
//Set if the section is checked
void Fpl_Frc_Curve::set_is_checked(const bool is_checked){
	this->section_is_checked=is_checked;
}
//Get if the section is already checked
bool Fpl_Frc_Curve::get_is_checked(void){
	return this->section_is_checked;
}
//Output the interval(s) to console/display
void Fpl_Frc_Curve::output_intervals(Fpl_Section *section){
	ostringstream cout;
	cout << "FRC-INTERVALS"<<endl;
	cout << " Section name         : " << section->get_name_section()<<endl;
	cout << " Section id           : " << section->get_id_section()<<endl;
	cout << " Number intervals     : " << this->number_intervals<<endl;
	cout << " no. " << W(5)<< " id " << W(21)<< " mid_h " <<label::m<<W(15)<<" break_height "<<label::m<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout, true);
	cout << FORMAT_FIXED_REAL << P(3);
	for(int i=0; i< this->number_intervals; i++){
		if(i<this->number_intervals-1){
			cout << " " << i << W(6) << this->intervals[i].id_str << W(14)<< " " <<this->intervals[i].interval_mid<<" "<<W(14)<<this->intervals[i].break_waterlevel<< endl;
		}
		else{
			cout << " " << i << W(6) << this->intervals[i].id_str << W(15)<< "N   (N)" << endl;
		}
		Sys_Common_Output::output_risk->output_txt(&cout, true);
	}
}
//Get the current evaluated waterlevel
double Fpl_Frc_Curve::get_current_waterlevel(void){
	return this->current_waterlevel;
}
//Get the current evaluated index of the intervall
int Fpl_Frc_Curve::get_current_index_intervall(void){
	stringstream buffer;
	int buff_int;
	buffer <<this->get_current_intervall_string();
	buffer >> buff_int;
	return buff_int;
}
//Get the current evaluated intervall as string
string Fpl_Frc_Curve::get_current_intervall_string(void){
	if(this->current_index_intervall<0){
		this->in_nobreak_interval=true;
		return "x";
	}
	else{
		return this->intervals[this->current_index_intervall].id_str;
	}
}
//Convert a interval string to an integer (static)
int Fpl_Frc_Curve::convert_interval_string2integer(const string interval_id){
	int buffer=0;
	stringstream id_interval;
	id_interval << interval_id;
	id_interval >> buffer;

	if(id_interval.fail()==true){
		buffer=0;
		return buffer;
	}

	return buffer;
}
//_________
//private
//Allocate the points
void Fpl_Frc_Curve::allocate_points(void){
	try{
		this->points=new _fpl_frc_result[this->number_points];
	}
	catch(bad_alloc&t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(_fpl_frc_values)*this->number_points, _sys_system_modules::FPL_SYS);

	//init
	for(int i=0; i< this->number_points; i++){
		this->points[i].failure_prob=0.0;
		this->points[i].low_failure_prob=0.0;
		this->points[i].up_failure_prob=0.0;
		this->points[i].time_mc=0.0;
		this->points[i].waterlevel=0.0;
		this->points[i].number_mc=0;
	}

}
//Delete the points
void Fpl_Frc_Curve::delete_points(void){
	if(this->points!=NULL){
		delete []this->points;
		this->points=NULL;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_frc_values)*this->number_points, _sys_system_modules::FPL_SYS);
	}
}
//Allocate the intervals (test)
void Fpl_Frc_Curve::allocate_intervals(void){
	this->delete_intervals();
	try{
		this->intervals=new _fpl_frc_intervals[Fpl_Frc_Curve::number_intervals];
	}
	catch(bad_alloc&t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(_fpl_frc_intervals)*Fpl_Frc_Curve::number_intervals, _sys_system_modules::FPL_SYS);

	//init
	for(int i=0; i< Fpl_Frc_Curve::number_intervals; i++){
		this->intervals[i].id_str="";
		this->intervals[i].interval_low.failure_prob=0.0;
		this->intervals[i].interval_up.failure_prob=0.0;
		this->intervals[i].interval_low.waterlevel=0.0;
		this->intervals[i].interval_up.waterlevel=0.0;
		this->intervals[i].interval_mid=0.0;
		this->intervals[i].break_waterlevel=0.0;
		this->intervals[i].number_occ=0;
	}
}
//Delete the intervals (test)
void Fpl_Frc_Curve::delete_intervals(void){

	if(this->intervals!=NULL){
		delete []this->intervals;
		this->intervals=NULL;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_frc_intervals)*Fpl_Frc_Curve::number_intervals, _sys_system_modules::FPL_SYS);
	}
}
//Find the index of the intervals
int Fpl_Frc_Curve::find_index_intervals(const double random_value, double *waterlevel){
	this->in_nobreak_interval=false;
	int buffer=-1;
	int store_index=0;
	double waterlevel_inter=0.0;
	if(random_value<=0.0){
		buffer=0;
	}
	else if(random_value>=this->points[this->number_points-1].failure_prob){
		waterlevel_inter=this->points[this->number_points-1].waterlevel;
	}
	else{

		for(int i=0; i<this->number_points; i++){
			if(random_value<=this->points[i].failure_prob){
				store_index=i-1;
				break;
			}
		}

		waterlevel_inter=this->points[store_index].waterlevel+
			((this->points[store_index+1].waterlevel-this->points[store_index].waterlevel)/
			(this->points[store_index+1].failure_prob-this->points[store_index].failure_prob))*
			(random_value-this->points[store_index].failure_prob);
	}



	//find the interval
	if(waterlevel_inter>=this->intervals[Fpl_Frc_Curve::number_intervals-1].interval_low.waterlevel){
		buffer=Fpl_Frc_Curve::number_intervals-1;
		this->in_nobreak_interval=true;
	}
	else{
		for(int i=0; i< Fpl_Frc_Curve::number_intervals; i++){
			if(waterlevel_inter>=this->intervals[i].interval_low.waterlevel && waterlevel_inter<this->intervals[i].interval_up.waterlevel){
				buffer=i;
				this->in_nobreak_interval=false;
				break;
			}

		}
	}


	*waterlevel=waterlevel_inter;
	

	return buffer;

}
//Set error(s)
Error Fpl_Frc_Curve::set_error(const int err_type){
	string place="Fpl_Frc_Curve::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_points(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://no points found for the frc-curve
			place.append("set_curve_data(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id)");
			reason="Do not find enough data points for the frc-curve in database table";
			help="Check the database table of the reliability results";
			type=2;
			break;
		case 2://bad alloc
			place.append("allocate_intervals(void)");
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
Warning Fpl_Frc_Curve::set_warning(const int warn_type){
	string place="Fpl_Frc_Curve::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_intervals2database(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const int sz_bound_id)");
			reason="Can not submit the FRC interval data to the database";
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
}