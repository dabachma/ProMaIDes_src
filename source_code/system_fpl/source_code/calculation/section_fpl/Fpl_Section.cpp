//#include "Fpl_Section.h"
#include "Fpl_Headers_Precompiled.h"

//init static members
Tables *Fpl_Section::table=NULL;

//Default constructor
Fpl_Section::Fpl_Section(void){
	this->id_section=-1;
	this->type=_fpl_section_types::high_ground_sec;
	this->section_name=label::not_set;
	this->direction=0.0;
	this->left_flag=true;
	this->section_length=0.0;
	this->station_start=0.0;
	this->station_end=0.0;
	this->type_of_section=NULL;
	this->flag_frc_sim=false;
	this->glob_id=-1;
	this->structure_height=0.0;
	this->crest_water.set_point_name(fpl_label::crest_water);
	this->break_params.abrupt_fails_flag=false;
	this->break_params.resistance=0.2;
	this->break_params.abrupt_opening=0.0;
	this->breach_start.max_breach_width=constant::maximum_breach_width;
	this->breach_start.id_related_station=-1;
	this->breach_start.id_rv_model=-1;
	this->breach_start.distance2related=0.0;
	this->breach_start.start_height=-1.0;
	this->must_found=0;
	this->number_error_begin=0;
	this->number_warning_begin=0;
	this->number_error=0;
	this->number_warning=0;

	this->list_involved_profiles=NULL;
	this->number_involved_profiles=0;

	this->list_involved_points=NULL;
	this->number_involved_points=0;

	this->frc_data=NULL;

	this->similarity_lenght=-1.0;
	this->similarity_factor=0.0;

	this->output_folder=label::not_set;

	this->inp_river_sec=false;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Section), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Section::~Fpl_Section(void){
	this->delete_section_type();
	this->delete_list_involved_profiles();
	this->delete_list_involved_points();
	this->delete_frc_data();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Section), _sys_system_modules::FPL_SYS);//count the memory
}
//_______________
//public
//Read in the fpl-section from file
void Fpl_Section::read_section_per_file(const string file_section, const bool flag_frc_sim, QSqlDatabase *ptr_database){
	int line_counter=0;
	QFile ifile;

	ostringstream cout;
	ostringstream prefix;
	prefix << "INP"<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	cout << "Input the FPL-section from file " << file_section << "..."<<endl;

	Sys_Common_Output::output_fpl->output_txt(&cout);
	//read in damage function
	try{
		//open file
		ifile.setFileName(file_section.c_str());
		QIODevice::OpenMode my_flag;
		my_flag=my_flag|(QIODevice::ReadOnly);
		my_flag=my_flag|(QIODevice::Text);
		ifile.open(my_flag);
		if(ifile.isOpen()==false){
			Error msg=this->set_error(1);
			ostringstream info;
			info << "Filename: " << file_section << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//read in general section data
		try{
			this->read_general_section_data_per_file(&ifile, &line_counter);
			//to begin of the file
			ifile.reset();
			line_counter=0;
		}
		catch(Error msg){
			ifile.close();
			ostringstream info;
			info << "Error occurs near line: "<< line_counter << endl;
			info << "Filename              : "<< file_section << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		Fpl_Calculation::check_stop_thread_flag();

		Geo_Point buff_p;
		buff_p.set_point_coordinate(0.0,0.0);
		this->structure_height=this->crest_water.vertical_distance(&buff_p);

		try{
			this->allocate_section_type();
			this->type_of_section->set_output_control_flags(ptr_database);
		}
		catch(Error msg){
			throw msg;
		}

		//read in hydraulic connection data
		try{
			prefix << this->section_name<<"> ";
			Sys_Common_Output::output_fpl->set_userprefix(&prefix);
			this->read_hyd_connection_data_per_file(&ifile, &line_counter);
			Sys_Common_Output::output_fpl->rewind_userprefix();
			//to begin of the file
			ifile.reset();
			line_counter=0;
		}
		catch(Error msg){
			Sys_Common_Output::output_fpl->rewind_userprefix();
			ifile.close();
			ostringstream info;
			info << "Error occurs near line: "<< line_counter << endl;
			info << "Filename              : "<< file_section << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		if(this->type!=_fpl_section_types::by_hand_sec){
			//read in section type data
			try{
				prefix << this->section_name<<"> ";
				Sys_Common_Output::output_fpl->set_userprefix(&prefix);

				this->type_of_section->read_section_type_per_file(&ifile, &line_counter, flag_frc_sim);
				//to begin of the file
				ifile.reset();
				line_counter=0;
				Fpl_Calculation::check_stop_thread_flag();
				this->type_of_section->read_waterside_geometry(&ifile,&line_counter);

				ifile.reset();
				line_counter=0;
				Fpl_Calculation::check_stop_thread_flag();
				this->type_of_section->read_landside_geometry(&ifile,&line_counter);

				ifile.reset();
				line_counter=0;
				Fpl_Calculation::check_stop_thread_flag();
				this->type_of_section->check_mechanism_faulttree_perfile(&ifile,&line_counter);

				ifile.reset();
				line_counter=0;
				Fpl_Calculation::check_stop_thread_flag();
				this->type_of_section->read_geo_matzones(&ifile, &line_counter);

				this->type_of_section->check_set_geometry();

				ifile.reset();
				line_counter=0;
				Fpl_Calculation::check_stop_thread_flag();
				this->type_of_section->read_var_matzones(&ifile, &line_counter);

				ifile.reset();
				line_counter=0;
				Fpl_Calculation::check_stop_thread_flag();
				this->type_of_section->read_var_waterside_matzones(&ifile, &line_counter);

				ifile.reset();
				line_counter=0;
				Fpl_Calculation::check_stop_thread_flag();
				this->type_of_section->read_mechanism_faulttree_perfile(&ifile, &line_counter);

				Sys_Common_Output::output_fpl->rewind_userprefix();
			}
			catch(Error msg){
				this->output_geometry2tecplot();
				this->output_geometry2excel();
				this->output_geometry2paraview();
				Sys_Common_Output::output_fpl->rewind_userprefix();
				ifile.close();
				ostringstream info;
				info << "Error occurs near line : "<< line_counter << endl;
				info << "Filename               : "<< file_section << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}
	catch(Error msg){
		ifile.close();
		Sys_Common_Output::output_fpl->rewind_userprefix();
		throw msg;
	}

	//set the start breach height if it wasn't set
	if(this->breach_start.start_height<0.0){
		this->breach_start.start_height=this->structure_height*2.0/3.0;
	}

	cout << "Input of the FPL-section " << this->section_name<<" from file " << file_section << " is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	Sys_Common_Output::output_fpl->rewind_userprefix();
	ifile.close();
}
//Transfer the members read in per file to database
void Fpl_Section::transfer_member2database(QSqlDatabase *ptr_database){
	ostringstream cout;
	ostringstream prefix;
	prefix << "TRANS"<<"> ";
	prefix << this->section_name <<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	cout << "Transfer the fpl-section " << this->section_name << " to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	this->transfer_section2database(ptr_database);
	this->transfer_crest_water2database(ptr_database);
	if(this->type!=_fpl_section_types::by_hand_sec){
		this->type_of_section->transfer_sectiontype2database(this->id_section, ptr_database);
	}

	cout << "Transfer of the fpl-section " << this->section_name<<" to database is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	Sys_Common_Output::output_fpl->rewind_userprefix();
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Create the database table for the section data (static)
void Fpl_Section::create_table(QSqlDatabase *ptr_database){
		if(Fpl_Section::table==NULL){
			ostringstream cout;
			cout << "Create section data database table..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_fpl_section;
			const int num_col=24;
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

			tab_col[5].name=fpl_label::section_name;
			tab_col[5].type=sys_label::tab_col_type_string;

			tab_col[6].name=fpl_label::section_type;
			tab_col[6].type=sys_label::tab_col_type_string;

			tab_col[7].name=fpl_label::section_length;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].default_value="0.0";

			tab_col[8].name=fpl_label::section_similarity_length;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="-1.0";

			tab_col[9].name=fpl_label::section_similarity_factor;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";

			tab_col[10].name=fpl_label::section_direction;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].default_value="0.0";

			tab_col[11].name=fpl_label::structure_height;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].default_value="0.0";

			tab_col[12].name=fpl_label::section_conn_rv_id;
			tab_col[12].type=sys_label::tab_col_type_int;
			tab_col[12].default_value="-1";

			tab_col[13].name=fpl_label::section_rv_bank_flag;
			tab_col[13].type=sys_label::tab_col_type_bool;

			tab_col[14].name=fpl_label::section_station_start;
			tab_col[14].type=sys_label::tab_col_type_double;
			tab_col[14].default_value="0.0";

			tab_col[15].name=fpl_label::section_station_end;
			tab_col[15].type=sys_label::tab_col_type_double;
			tab_col[15].default_value="0.0";

			tab_col[16].name=fpl_label::section_max_breach;
			tab_col[16].type=sys_label::tab_col_type_double;
			tab_col[16].default_value="200.0";

			tab_col[17].name=fpl_label::section_start_breach_h;
			tab_col[17].type=sys_label::tab_col_type_double;

			tab_col[18].name=fpl_label::section_abrupt_breach;
			tab_col[18].type=sys_label::tab_col_type_bool;
			tab_col[18].default_value="false";

			tab_col[19].name=fpl_label::section_resistance_breach;
			tab_col[19].type=sys_label::tab_col_type_double;
			tab_col[19].default_value="0.0";

			tab_col[20].name=fpl_label::section_abrupt_open_width;
			tab_col[20].type=sys_label::tab_col_type_double;
			tab_col[20].default_value="0.0";

			tab_col[21].name=fpl_label::section_line;
			tab_col[21].type=sys_label::tab_col_type_polyline;

			tab_col[22].name=label::description;
			tab_col[22].type=sys_label::tab_col_type_string;

			tab_col[23].name=label::link;
			tab_col[23].type=sys_label::tab_col_type_string;

			try{
				Fpl_Section::table= new Tables();
				if(Fpl_Section::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Section::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Fpl_Section::close_table();
				throw msg;
			}

		Fpl_Section::close_table();
	}
}
//Delete all data in the database table for the section data (static)
void Fpl_Section::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Fpl_Section::table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the section data for a specific section (static)
void Fpl_Section::delete_data_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id){
/**
Also all appending data in the database, like results and variables will be deleted.
*/

	//first delete other appending tables
	//results
	try{
		Fpl_Mc_Sim::delete_data_in_result_table(ptr_database, sec_id, id, true);
		Fpl_Mc_Sim::delete_data_in_result_table(ptr_database, sec_id, id, false);
		Fpl_Calculation::check_stop_thread_flag();
	}
	catch(Error msg){
		throw msg;
	}
	//detailed results
	try{
		//dike
		Fpl_Sec_Type_Dike::delete_data_in_result_table(ptr_database, sec_id, id, _fpl_simulation_type::sim_determ);
		Fpl_Sec_Type_Dike::delete_data_in_result_table(ptr_database, sec_id, id, _fpl_simulation_type::sim_mc);
		Fpl_Sec_Type_Dike::delete_data_in_result_table(ptr_database, sec_id, id, _fpl_simulation_type::sim_frc);
		Fpl_Calculation::check_stop_thread_flag();
		//dune
		Fpl_Sec_Type_Dune::delete_data_in_result_table(ptr_database, sec_id, id, _fpl_simulation_type::sim_determ);
		Fpl_Sec_Type_Dune::delete_data_in_result_table(ptr_database, sec_id, id, _fpl_simulation_type::sim_mc);
		Fpl_Sec_Type_Dune::delete_data_in_result_table(ptr_database, sec_id, id, _fpl_simulation_type::sim_frc);
		Fpl_Calculation::check_stop_thread_flag();
		//highground
		Fpl_Sec_Type_Highground::delete_data_in_result_table(ptr_database, sec_id, id, _fpl_simulation_type::sim_determ);
		Fpl_Sec_Type_Highground::delete_data_in_result_table(ptr_database, sec_id, id, _fpl_simulation_type::sim_mc);
		Fpl_Sec_Type_Highground::delete_data_in_result_table(ptr_database, sec_id, id, _fpl_simulation_type::sim_frc);
	}
	catch(Error msg){
		throw msg;
	}
	//Random variables
	try{
		Fpl_Random_Variables::delete_data_in_table(ptr_database, id, sec_id);
		Fpl_Calculation::check_stop_thread_flag();
	}
	catch(Error msg){
		throw msg;
	}
	//geometry
	try{
		Fpl_Section_Points::delete_data_in_point_table(ptr_database, id, sec_id);
		Fpl_Dike_Geo_Materialzone::delete_data_in_matzone_table(ptr_database, id, sec_id);
	}
	catch(Error msg){
		throw msg;
	}

	//slope control
	try{
		Fpl_Mech_Slope_Stability_Dike::delete_data_in_control_table(ptr_database,sec_id);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete
	test_filter<< "Delete ";
	test_filter << " from " << Fpl_Section::table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Section::table->get_column_name(fpl_label::section_id) << " = " << sec_id << " and ";
	test_filter << Fpl_Section::table->get_column_name(label::areastate_id) << " = " << id.measure_nr << " and ";
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << id.area_state;
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Section::delete_data_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the database table for the section data: it sets the table name and the name of the columns and allocate them (static)
void Fpl_Section::set_table(QSqlDatabase *ptr_database){
	if(Fpl_Section::table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_fpl_section;//FPL:=FloodProtectionLine
		string tab_col[24];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::section_id;
		tab_col[2]=label::areastate_id;
		tab_col[3]=label::measure_id;
		tab_col[4]=label::applied_flag;
		tab_col[5]=fpl_label::section_name;
		tab_col[6]=fpl_label::section_type;
		tab_col[7]=fpl_label::section_length;
		tab_col[8]=fpl_label::section_direction;
		tab_col[9]=fpl_label::section_conn_rv_id;;
		tab_col[10]=fpl_label::section_rv_bank_flag;
		tab_col[11]=fpl_label::section_station_start;
		tab_col[12]=fpl_label::section_station_end;
		tab_col[13]=fpl_label::section_line;
		tab_col[14]=label::description;
		tab_col[15]=label::link;
		tab_col[16]=fpl_label::section_max_breach;
		tab_col[17]=fpl_label::section_abrupt_breach;
		tab_col[18]=fpl_label::section_resistance_breach;
		tab_col[19]=fpl_label::section_abrupt_open_width;
		tab_col[20]=fpl_label::structure_height;
		tab_col[21]=fpl_label::section_start_breach_h;
		tab_col[22]=fpl_label::section_similarity_length;
		tab_col[23]=fpl_label::section_similarity_factor;

		//set the table
		try{
			Fpl_Section::table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Section::table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Fpl_Section::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory","Check the memory",10,false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Fpl_Section::close_table();
			throw msg;
		}
	}
}
//Close and delete the database table section data (static)
void Fpl_Section::close_table(void){
	if(Fpl_Section::table!=NULL){
		delete Fpl_Section::table;
		Fpl_Section::table=NULL;
	}
}
//Check if all section are combined to the hydraulic system (static)
bool Fpl_Section::check_all_combined2hyd_system(QSqlDatabase *ptr_database, const _sys_system_id id){
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "SELECT ST_ASTEXT(";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_line);
	test_filter << ") FROM " << Fpl_Section::table->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Fpl_Section::table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Fpl_Section::table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND (";
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " OR " ;
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;

	QSqlQuery results(*ptr_database);
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Section::all_combined2hyd_system(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	string buffer;
	results.first();
	for(int i=0; i< results.size(); i++){
		//buffer=results.record().value((Fpl_Section::table->get_column_name(fpl_label::section_line)).c_str()).toString().toStdString();
		buffer=results.value(0).toString().toStdString();

		results.next();
		if(buffer.empty()==true){
			return false;
		}
	}
	return true;
}
//Reset the combining to the hydraulic system (static)
void Fpl_Section::reset_combined2hyd_system(QSqlDatabase *ptr_database, const _sys_system_id id){
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQuery model(0,*ptr_database);
	//generate the filter
	ostringstream filter;
	filter << "UPDATE " << Fpl_Section::table->get_table_name().c_str();
	filter << " SET ";
	filter << Fpl_Section::table->get_column_name(fpl_label::section_line).c_str() << " = NULL";
	filter << " WHERE " ;
	filter << Fpl_Section::table->get_column_name(label::applied_flag) << "= true";
	filter << " AND ";
	filter << Fpl_Section::table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	filter << " AND (";
	filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << 0 ;
	filter << " OR " ;
	filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	filter << " ) " ;
	Data_Base::database_request(&model, filter.str(), ptr_database);
}
//Check if there are fpl-section set in database (static)
bool Fpl_Section::check_some_section_set_in_database(QSqlDatabase *ptr_database, const _sys_system_id id){
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::glob_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_id) << "  ";

	test_filter << " FROM " << Fpl_Section::table->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Fpl_Section::table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Fpl_Section::table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND (";
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " OR " ;
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;

	QSqlQueryModel results;
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Section::check_some_section_set_in_database(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(results.rowCount()>0){
		return true;
	}
	else{
		return false;
	}
}
//Select and count the number of relevant fpl section in a database table (static)
int Fpl_Section::select_relevant_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output){
	int number=0;
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Search for relevant fpl-sections in database..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::glob_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::areastate_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::measure_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::applied_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_name) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_type) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_length) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_direction) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_length) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_factor) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_conn_rv_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_rv_bank_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_start) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_end) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::description) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_max_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_start_breach_h) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_resistance_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_open_width) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::structure_height) << " , ";

	test_filter <<  Fpl_Section::table->get_column_name(label::link);

	test_filter << " FROM " << Fpl_Section::table->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Fpl_Section::table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Fpl_Section::table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND (";
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " OR " ;
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " ORDER BY " <<  Fpl_Section::table->get_column_name(fpl_label::glob_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Section::select_relevant_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant fpl-sections are found in database" << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);
	}

	return number;
}
//Select one specific fpl section in a database table (static)
int Fpl_Section::select_relevant_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id,  const bool with_output){
	int number=0;
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Search for fpl-section with the id "<< sec_id <<" in database..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::glob_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::areastate_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::measure_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::applied_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_name) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_type) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_length) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_direction) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_length) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_factor) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_conn_rv_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_rv_bank_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_start) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_end) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::description) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_max_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_start_breach_h) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_resistance_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_open_width) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::structure_height) << " , ";

	test_filter <<  Fpl_Section::table->get_column_name(label::link);

	test_filter << " from " << Fpl_Section::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Section::table->get_column_name(fpl_label::section_id) << " = " << sec_id;
	test_filter << " and ";
	test_filter << Fpl_Section::table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Fpl_Section::table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " order by " <<  Fpl_Section::table->get_column_name(fpl_label::glob_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Warning msg;
		msg.set_msg("int Fpl_Section::select_relevant_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id,  const bool with_output)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout <<"Fpl-section is found" << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);
	}

	return number;
}
//Select and count the number of fpl section for the given measure state in a database table (static)
int Fpl_Section::select_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id){
	int number=0;
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::glob_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::areastate_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::measure_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::applied_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_name) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_type) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_length) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_direction) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_length) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_factor) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_conn_rv_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_rv_bank_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_start) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_end) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::description) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_max_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_start_breach_h) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_resistance_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_open_width) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::structure_height) << " , ";

	test_filter <<  Fpl_Section::table->get_column_name(label::link);

	test_filter << " from " << Fpl_Section::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Section::table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " and ";
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	test_filter << " order by " <<  Fpl_Section::table->get_column_name(fpl_label::glob_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Warning msg;
		msg.set_msg("Fpl_Section::select_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	number= results->rowCount();

	return number;
}
//Select and count the number of fpl section with a specific section type for the given measure state in a database table (static)
int Fpl_Section::select_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const _fpl_section_types type){
	int number=0;
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::glob_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::areastate_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::measure_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::applied_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_name) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_type) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_length) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_direction) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_length) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_factor) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_conn_rv_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_rv_bank_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_start) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_end) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::description) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_max_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_start_breach_h) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_resistance_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_open_width) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::structure_height) << " , ";

	test_filter <<  Fpl_Section::table->get_column_name(label::link);

	test_filter << " from " << Fpl_Section::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Section::table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " and ";
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " and ";
	test_filter << Fpl_Section::table->get_column_name(fpl_label::section_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(type)<<"'";

	test_filter << " order by " <<  Fpl_Section::table->get_column_name(fpl_label::glob_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Warning msg;
		msg.set_msg("Fpl_Section::select_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const _fpl_section_types type)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	number= results->rowCount();

	return number;
}
//Get a list of all available section ids (static)
QList<int> Fpl_Section::get_list_relevant_section_database(QSqlDatabase *ptr_database, const _sys_system_id id) {
	QList<int> buff_id;
	QSqlQueryModel result;
	int number = 0;
	number = Fpl_Section::select_relevant_section_database(&result, ptr_database, id, false);
	for (int i = 0; i < number; i++) {
		buff_id.append(result.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_id)).c_str()).toInt());
	}
	return buff_id;
}
//Get a list of last x section ids  (static)
QList<int> Fpl_Section::get_list_last_section_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int last) {
	QList<int> buff_id;
	QSqlQueryModel result;
	int number = 0;
	number = Fpl_Section::select_relevant_section_database(&result, ptr_database, id, false);
	for (int i = number-last; i < number; i++) {
		buff_id.append(result.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_id)).c_str()).toInt());
	}
	return buff_id;

}
//Select one specific fpl section in a database table (static)
int Fpl_Section::select_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id){
	int number=0;
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::glob_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::areastate_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::measure_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::applied_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_name) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_type) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_length) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_direction) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_length) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_factor) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_conn_rv_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_rv_bank_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_start) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_end) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::description) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_max_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_start_breach_h) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_resistance_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_open_width) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::structure_height) << " , ";

	test_filter <<  Fpl_Section::table->get_column_name(label::link);

	test_filter << " from " << Fpl_Section::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Section::table->get_column_name(fpl_label::section_id) << " = " << sec_id;
	test_filter << " and ";
	test_filter << Fpl_Section::table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " and ";
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	test_filter << " order by " <<  Fpl_Section::table->get_column_name(fpl_label::glob_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Warning msg;
		msg.set_msg("Fpl_Section::select_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	number= results->rowCount();

	return number;
}
//Select one specific fpl section in a database table
int Fpl_Section::select_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int sec_id){
	int number=0;
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::glob_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::areastate_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::measure_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::applied_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_name) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_type) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_length) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_direction) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_length) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_factor) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_conn_rv_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_rv_bank_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_start) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_end) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::description) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_max_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_start_breach_h) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_resistance_breach) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_open_width) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::structure_height) << " , ";

	test_filter <<  Fpl_Section::table->get_column_name(label::link);

	test_filter << " from " << Fpl_Section::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Section::table->get_column_name(fpl_label::section_id) << " = " << sec_id;

	test_filter << " order by " <<  Fpl_Section::table->get_column_name(fpl_label::glob_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Warning msg;
		msg.set_msg("Fpl_Section::select_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	number= results->rowCount();

	return number;
}
//Switch the applied-flag for the section in the database table for a defined system state (static)
void Fpl_Section::switch_applied_flag_section_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Section::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Section::table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Section::table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Section::table->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Section::switch_applied_flag_section_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//appending tables
	/*_Hyd_River_Profile::switch_applied_flag_boundary_table(ptr_database, id,flag);
	Hyd_Element_Floodplain::switch_applied_flag_boundary_table(ptr_database, id,flag);*/
}
//Switch the applied-flag for the section in the database table for a defined system state and id (static)
void Fpl_Section::switch_applied_flag_section_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Section::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Section::table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Section::table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Section::table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Fpl_Section::table->get_column_name(fpl_label::section_id) << " = " << sec_id ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Section::switch_applied_flag_section_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Copy the fpl-section parameters of a given system id to another one in database tables (static)
void Fpl_Section::copy_fpl_section(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id){
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Fpl_Section::table->maximum_int_of_column(Fpl_Section::table->get_column_name(fpl_label::glob_id) ,ptr_database)+1;

	ostringstream test_filter;
	QSqlQueryModel model1;

	test_filter.str("");
	test_filter << "INSERT INTO "<< Fpl_Section::table->get_table_name();
	test_filter << " SELECT " << glob_id <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_id)  <<" , ";
	test_filter <<  dest.area_state  <<" , ";
	test_filter <<  dest.measure_nr  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::applied_flag)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_name)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_type)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_length)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_length)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_similarity_factor)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_direction)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::structure_height)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_conn_rv_id)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_rv_bank_flag)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_start)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_station_end)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_max_breach)  <<" , ";

	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_start_breach_h)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_breach)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_resistance_breach)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_abrupt_open_width)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_line)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::description)  <<" , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::link)  <<"  ";

	test_filter << " FROM " << Fpl_Section::table->get_table_name() <<" ";
	test_filter << " WHERE " << Fpl_Section::table->get_column_name(fpl_label::section_id) << " = ";
	test_filter << sec_id;
	test_filter << " AND " << Fpl_Section::table->get_column_name(label::areastate_id) << " = ";
	test_filter << src.area_state;
	test_filter << " AND " << Fpl_Section::table->get_column_name(label::measure_id) << " = ";
	test_filter << src.measure_nr;
	Data_Base::database_request(&model1, test_filter.str(), ptr_database);
	if(model1.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Section::copy_fpl_section(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << model1.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	try{
		Fpl_Section::switch_applied_flag_section_table(ptr_database, src, false,sec_id);
	}
	catch(Error msg){
		throw msg;
	}
}
//Check if all section results (frc/mc), where the results are set by hand, are set (static)
bool Fpl_Section::check_set_by_hand_section_result(QSqlDatabase *ptr_database, const _sys_system_id id, string *txt){
	QSqlQueryModel results;
	//reset txt
	*txt="";
	int number=0;
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::glob_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::areastate_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::measure_id) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(label::applied_flag) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_type) << " , ";
	test_filter <<  Fpl_Section::table->get_column_name(fpl_label::section_name);

	test_filter << " from " << Fpl_Section::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Section::table->get_column_name(fpl_label::section_type) << "= '"<<fpl_label::sec_set_by_hand <<"'";
	test_filter << " and ";
	test_filter << Fpl_Section::table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Fpl_Section::table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Fpl_Section::table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " order by " <<  Fpl_Section::table->get_column_name(fpl_label::glob_id);

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	if(results.lastError().isValid()){
		Warning msg;
		msg.set_msg("Fpl_Section::check_set_by_hand_section_result(QSqlDatabase *ptr_database, const _sys_system_id id, string *txt)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	number= results.rowCount();

	if(number>0){
		ostringstream failure_txt;
		bool mc_available=false;
		bool frc_available=false;
		int sec_id=-1;
		string sec_name;
		for(int i=0; i< number; i++){
			mc_available=false;
			frc_available=false;
			sec_name="";
			sec_id=-1;
			sec_id=results.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_id)).c_str()).toInt();
			//set the element members
			sec_name=results.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_name)).c_str()).toString().toStdString();
			Fpl_Mc_Sim::check_results_available_in_database(ptr_database, id, sec_id, &mc_available, &frc_available);

			if(mc_available==false){
				failure_txt<<"No Monte-carlo results available for section "<< sec_name <<" (Id " << sec_id<<")"<<endl;
			}
			if(frc_available==false){
				failure_txt<<"No Fragility-curve results available for section "<< sec_name <<" (Id " << sec_id<<")"<<endl;
			}
		}
		*txt=failure_txt.str();
	}

	if(txt->empty()==true){
		return false;
	}
	else{
		return true;
	}
}
//Delete the detailed result values of the section types for a given section
void Fpl_Section::delete_detailed_results_database(QSqlDatabase *ptr_database, const _fpl_simulation_type sim_type){
		switch(this->type){
			case _fpl_section_types::by_hand_sec://is set by hand todo
				break;
			case _fpl_section_types::high_ground_sec://highground
				Fpl_Sec_Type_Highground::delete_data_in_result_table(ptr_database,this->id_section,this->system_id, sim_type);
				break;
			case _fpl_section_types::dike_sec://dike
				Fpl_Sec_Type_Dike::delete_data_in_result_table(ptr_database,this->id_section,this->system_id, sim_type);
				break;
			case _fpl_section_types::dune_sec://dune
				Fpl_Sec_Type_Dune::delete_data_in_result_table(ptr_database,this->id_section,this->system_id, sim_type);
				break;
			case _fpl_section_types::wall_sec://wall

				break;
			case _fpl_section_types::mob_wall_sec://mobile_wall

				break;
			default://unknown type; it is set to zero
				break;
		}
}
//Calculate the fault tree with random distributed variables (true) or with deterministic variables (false)
int Fpl_Section::make_faulttree(const bool random_calculation){
	int buffer=0;
	ostringstream prefix;
	prefix <<this->section_name<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	buffer=this->type_of_section->make_faulttree(random_calculation);
	Sys_Common_Output::output_fpl->rewind_userprefix();
	return buffer;
}
//Input the fpl-section data form a query model per database
void Fpl_Section::input_section_perdatabase(const QSqlQueryModel *results, const int glob_index, QSqlDatabase *ptr_database, const bool applied_important){
	try{
		this->id_section=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_id)).c_str()).toInt();
		//set the element members
		this->section_name=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_name)).c_str()).toString().toStdString();
		this->type=_Fpl_Section_Type::convert_txt2section_type(results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_type)).c_str()).toString().toStdString());
		this->section_length=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_length)).c_str()).toDouble();
		this->direction=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_direction)).c_str()).toDouble();
		this->similarity_lenght=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_similarity_length)).c_str()).toDouble();
		this->similarity_factor=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_similarity_factor)).c_str()).toDouble();
		this->breach_start.id_rv_model=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_conn_rv_id)).c_str()).toInt();
		this->left_flag=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_rv_bank_flag)).c_str()).toBool();
		this->station_start=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_station_start)).c_str()).toDouble();
		this->station_end=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_station_end)).c_str()).toDouble();
		this->glob_id=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::glob_id)).c_str()).toInt();
		this->breach_start.max_breach_width=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_max_breach)).c_str()).toDouble();
		this->breach_start.start_height=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_start_breach_h)).c_str()).toDouble();

		this->break_params.resistance=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_resistance_breach)).c_str()).toDouble();
		this->break_params.abrupt_opening=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_abrupt_open_width)).c_str()).toDouble();
		this->break_params.abrupt_fails_flag=results->record(glob_index).value((Fpl_Section::table->get_column_name(fpl_label::section_abrupt_breach)).c_str()).toBool();
	}
	catch(Error msg){
		ostringstream info;
		info << "Section index            : " << this->id_section << endl;
		info << "Section name             : " << this->section_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	try{
		this->set_crest_water_database(ptr_database, applied_important);
	}
	catch(Error msg){
		ostringstream info;
		info << "Section index            : " << this->id_section << endl;
		info << "Section name             : " << this->section_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Input the fpl-section and the appending data form a query model per database
void Fpl_Section::input_section_total_perdatabase(const QSqlQueryModel *results, const int glob_index, QSqlDatabase *ptr_database, const bool flag_frc_sim, const bool output){
	this->flag_frc_sim=flag_frc_sim;
	ostringstream prefix;
	prefix <<"INP> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	try{
		this->input_section_perdatabase(results, glob_index, ptr_database);
		this->set_crest_water_database(ptr_database);
	}
	catch(Error msg){
		Sys_Common_Output::output_fpl->rewind_userprefix();
		throw msg;
	}
	try {
		prefix << this->section_name << "> ";
		Sys_Common_Output::output_fpl->set_userprefix(&prefix);
		//allocate the type
		this->allocate_section_type();
		this->type_of_section->set_output_control_flags(ptr_database);
		if (output == false) {
			this->type_of_section->set_input(this->id_section, this->flag_frc_sim, ptr_database);
		
			this->output_geometry2tecplot();
			this->output_geometry2excel();
			this->output_geometry2paraview();
		}

	}
	catch(Error msg){
		ostringstream info;
		info<<" Section name : "<<this->section_name<<endl;
		info<<" Section id   : "<<this->id_section<<endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_fpl->rewind_userprefix();
		Sys_Common_Output::output_fpl->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_fpl->rewind_userprefix();
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the section members to display/console
void Fpl_Section::output_members(void){
	ostringstream cout;
	ostringstream prefix;
	prefix << "OUT"<<"> ";
	prefix << this->section_name <<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << "GENERAL  " <<  endl;
	cout << " Number                        : ";
	if(this->id_section<0){
		cout << "Not yet set!"<<endl;
	}
	else{
		cout << this->id_section  << endl;
	}

	cout << " Name                          : " << this->section_name << endl;
	cout << " Type                          : " << _Fpl_Section_Type::convert_section_type2txt(this->type)  << endl;
	cout << " Length                        : ";
	if(this->section_length<=0.0){
		cout << "Not yet calculated!"<<endl;
	}
	else{
		cout << this->section_length << label::m<<endl;
	}
	cout << " Direction                     : " << this->direction <<label::degree<<endl;
	cout << " Similarity length             : " << this->similarity_lenght <<label::m<<endl;
	cout << " Similarity factor             : " << this->similarity_factor <<label::no_unit<<endl;
	cout << "HYDRAULIC CONNECTION"<<endl;
	if(this->breach_start.id_rv_model>=0){
		cout << " Id-river                      : " << this->breach_start.id_rv_model << endl;
		cout << " Left (true)/rigth bank (false): " << functions::convert_boolean2string(this->left_flag) << endl;
	}
	else{
		cout << " Coastal section                   " <<  endl;
	}
	cout << " Station start                 : " << this->station_start << label::m << endl;
	cout << " Station end                   : " << this->station_end << label::m << endl;
	cout << "BREAK PARAMETERS"<<endl;
	cout << " Structure height              : " << this->structure_height<<label::m << endl;
	cout << " Starting breach height        : " << this->breach_start.start_height<<label::m << endl;
	cout << " Maximum breach width          : " << this->breach_start.max_breach_width<<label::m << endl;
	if(this->type!=_fpl_section_types::high_ground_sec){
		if(this->break_params.abrupt_fails_flag==false){
			cout << " Abrupt fails                  : " << "false"<< endl;
			cout << " Resistance                    : " << this->break_params.resistance<<label::m_per_sec<< endl;
		}
		else{
			cout << " Abrupt fails                  : " << "true"<< endl;
			cout << " Resistance                    : " << this->break_params.resistance<<label::stress_unit<< endl;
			cout << " Abrupt opening                : " << this->break_params.resistance<<label::m<< endl;
		}
	}
	Sys_Common_Output::output_fpl->output_txt(&cout);

	//output the members of the section type
	if(type_of_section!=NULL){
		type_of_section->output_member();
	}
	else{
		cout << "The section type is not further specified " << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
	}
	Sys_Common_Output::output_fpl->rewind_userprefix();
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the deterministic reliability of the section
void Fpl_Section::output_reliability(void){
	if(this->type_of_section!=NULL){
		
		this->type_of_section->output_reliability(this->output_folder, this->id_section, this->section_name);
	}
}
//Output the section geometry to a tecplot file
void Fpl_Section::output_geometry2tecplot(void){
	if (type_of_section != NULL) {
		if (type_of_section->get_output_control_flags().tec_output == false) {
			return;
		}
	}
	else {
		return;
	}
	//check folder
	string folder_buff;
	folder_buff=Fpl_Section::check_generate_folder("tecplot",&this->output_folder);


	//create name
	ostringstream buffer;
	buffer << folder_buff <<"GEO_TEC_"<<this->id_section<<"_"<<this->section_name<<".dat";

	//get the file name
	string filename;
	filename=buffer.str();
	//open the file
	ofstream tecplot_output;
	tecplot_output.open(filename.c_str());
	if(tecplot_output.is_open()==false){
		Error msg=this->set_error(23);
		ostringstream info;
		info << "Filename : " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//output the file header
	tecplot_output << "TITLE = "<< "\" Geometry of " << this->section_name << "; Id. " << this->id_section<< "; ";
	if(this->left_flag==true){
		tecplot_output << " left " ;
	}
	else{
		tecplot_output << " right " ;
	}
	tecplot_output << "; Start " <<this->station_start<<label::m<< "; End "<< this->station_end <<label::m << " \"" << endl;
	tecplot_output << "VARIABLES = " <<endl;
	tecplot_output << "\" x " << label::m << "\" "<<endl;
	tecplot_output << "\" z " << label::m << "\" "<< endl;
	tecplot_output <<endl<<endl<<endl;

	if(type_of_section!=NULL){
		this->type_of_section->output_geometry2tecplot(&tecplot_output);
	}

	//close the file
	tecplot_output.close();
}
//Output the section geometry to a paraview file
void Fpl_Section::output_geometry2paraview(void) {
	if (type_of_section != NULL) {
		if (type_of_section->get_output_control_flags().para_output == false){
			return;
		}
	}
	else {
		return;
	}

	//check folder
	string folder_buff;
	folder_buff = Fpl_Section::check_generate_folder("paraview", &this->output_folder);
	//create name
	ostringstream buffer;
	buffer << folder_buff << "GEO_PARA_" << this->id_section << "_" << this->section_name << ".csv";

	//get the file name
	string filename;
	filename = buffer.str();
	//open the file
	ofstream tecplot_output;
	tecplot_output.open(filename.c_str());
	if (tecplot_output.is_open() == false) {
		Error msg = this->set_error(27);
		ostringstream info;
		info << "Filename : " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	if (type_of_section != NULL) {
		this->type_of_section->output_geometry2paraview(&tecplot_output);
	}

	//close the file
	tecplot_output.close();


}
//Output the section geometry to a excel file
void Fpl_Section::output_geometry2excel(void) {
	if (type_of_section != NULL) {
		if (type_of_section->get_output_control_flags().excel_output == false) {
			return;
		}
	}
	else {
		return;
	}
	//check folder
	string folder_buff;
	folder_buff = Fpl_Section::check_generate_folder("excel", &this->output_folder);
	//create name
	ostringstream buffer;
	buffer << folder_buff << "GEO_EXCEL_" << this->id_section << "_" << this->section_name << ".csv";

	//get the file name
	string filename;
	filename = buffer.str();
	//open the file
	ofstream tecplot_output;
	tecplot_output.open(filename.c_str());
	if (tecplot_output.is_open() == false) {
		Error msg = this->set_error(28);
		ostringstream info;
		info << "Filename : " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}



	if (type_of_section != NULL) {
		this->type_of_section->output_geometry2excel(&tecplot_output);
	}

	//close the file
	tecplot_output.close();

}
//Output the deterministic results to a tecplot file
void Fpl_Section::output_determ_results2tecplot(QSqlDatabase *ptr_database) {
	if (type_of_section != NULL) {
		if (type_of_section->get_output_control_flags().tec_output == false) {
			return;
		}
	}
	else {
		return;
	}
	//check folder
	string folder_buff;
	folder_buff = Fpl_Section::check_generate_folder("tecplot", &this->output_folder);


	//create name
	ostringstream buffer;
	buffer << folder_buff << "RES_DETERM_TEC_" << this->id_section << "_" << this->section_name << ".dat";

	//get the file name
	string filename;
	filename = buffer.str();
	//open the file
	ofstream tecplot_output;
	tecplot_output.open(filename.c_str());
	if (tecplot_output.is_open() == false) {
		Error msg = this->set_error(29);
		ostringstream info;
		info << "Filename : " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//output the file header
	tecplot_output << "TITLE = " << "\" Deterministic result of " << this->section_name << "; Id. " << this->id_section << "; ";
	if (this->left_flag == true) {
		tecplot_output << " left ";
	}
	else {
		tecplot_output << " right ";
	}
	tecplot_output << "; Start " << this->station_start << label::m << "; End " << this->station_end << label::m << " \"" << endl;


	if (type_of_section != NULL) {
		this->type_of_section->output_determ_res2tecplot(&tecplot_output, ptr_database, this->system_id, this->id_section);
	}

	//close the file
	tecplot_output.close();

	

}
//Output the deterministic results to a paraview file
void Fpl_Section::output_determ_results2paraview(QSqlDatabase *ptr_database) {
	if (type_of_section != NULL) {
		if (type_of_section->get_output_control_flags().para_output == false) {
			return;
		}
	}
	else {
		return;
	}

	//check folder
	string folder_buff;
	folder_buff = Fpl_Section::check_generate_folder("paraview", &this->output_folder);
	//create name
	ostringstream buffer;
	buffer << folder_buff << "RES_DETERM_PARA_" << this->id_section << "_" << this->section_name << ".csv";

	//get the file name
	string filename;
	filename = buffer.str();
	//open the file
	ofstream tecplot_output;
	tecplot_output.open(filename.c_str());
	if (tecplot_output.is_open() == false) {
		Error msg = this->set_error(29);
		ostringstream info;
		info << "Filename : " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	if (type_of_section != NULL) {
		this->type_of_section->output_determ_res2paraview(&tecplot_output, ptr_database, this->system_id, this->id_section);
	}

	//close the file
	tecplot_output.close();


}
//Output the deterministic results to a excel file
void Fpl_Section::output_determ_results2excel(QSqlDatabase *ptr_database) {
	if (type_of_section != NULL) {
		if (type_of_section->get_output_control_flags().excel_output == false) {
			return;
		}
	}
	else {
		return;
	}
	//check folder
	string folder_buff;
	folder_buff = Fpl_Section::check_generate_folder("excel", &this->output_folder);
	//create name
	ostringstream buffer;
	buffer << folder_buff << "RES_DETERM_EXCEL_" << this->id_section << "_" << this->section_name << ".csv";

	//get the file name
	string filename;
	filename = buffer.str();
	//open the file
	ofstream tecplot_output;
	tecplot_output.open(filename.c_str());
	if (tecplot_output.is_open() == false) {
		Error msg = this->set_error(29);
		ostringstream info;
		info << "Filename : " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}



	if (type_of_section != NULL) {
		this->type_of_section->output_determ_res2excel(&tecplot_output, ptr_database, this->system_id, this->id_section);
	}

	//close the file
	tecplot_output.close();

}
//Output the MC results to a tecplot file
void Fpl_Section::output_mc_results2tecplot(QSqlDatabase *ptr_database) {
	if (type_of_section != NULL) {
		if (type_of_section->get_output_control_flags().tec_output == false) {
			return;
		}
	}
	else {
		return;
	}
	//check folder
	string folder_buff;
	folder_buff = Fpl_Section::check_generate_folder("tecplot", &this->output_folder);


	//create name
	ostringstream buffer;
	buffer << folder_buff << "RES_DETERM_TEC_" << this->id_section << "_" << this->section_name << ".dat";

	//get the file name
	string filename;
	filename = buffer.str();
	//open the file
	ofstream tecplot_output;
	tecplot_output.open(filename.c_str());
	if (tecplot_output.is_open() == false) {
		Error msg = this->set_error(29);
		ostringstream info;
		info << "Filename : " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//output the file header
	tecplot_output << "TITLE = " << "\" MC result of " << this->section_name << "; Id. " << this->id_section << "; ";
	if (this->left_flag == true) {
		tecplot_output << " left ";
	}
	else {
		tecplot_output << " right ";
	}
	tecplot_output << "; Start " << this->station_start << label::m << "; End " << this->station_end << label::m << " \"" << endl;


	if (type_of_section != NULL) {
		this->type_of_section->output_mc_res2tecplot(&tecplot_output, ptr_database, this->system_id, this->id_section);
	}

	//close the file
	tecplot_output.close();

}
//Output the MC results to a paraview file
void Fpl_Section::output_mc_results2paraview(QSqlDatabase *ptr_database) {
	if (type_of_section != NULL) {
		if (type_of_section->get_output_control_flags().para_output == false) {
			return;
		}
	}
	else {
		return;
	}

	//check folder
	string folder_buff;
	folder_buff = Fpl_Section::check_generate_folder("paraview", &this->output_folder);
	//create name
	ostringstream buffer;
	buffer << folder_buff << "RES_MC_PARA_" << this->id_section << "_" << this->section_name << ".csv";

	//get the file name
	string filename;
	filename = buffer.str();
	//open the file
	ofstream tecplot_output;
	tecplot_output.open(filename.c_str());
	if (tecplot_output.is_open() == false) {
		Error msg = this->set_error(29);
		ostringstream info;
		info << "Filename : " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	if (type_of_section != NULL) {
		this->type_of_section->output_mc_res2paraview(&tecplot_output, ptr_database, this->system_id, this->id_section);
	}

	//close the file
	tecplot_output.close();

}
//Output the MC results to a excel file
void Fpl_Section::output_mc_results2excel(QSqlDatabase *ptr_database) {
	if (type_of_section != NULL) {
		if (type_of_section->get_output_control_flags().excel_output == false) {
			return;
		}
	}
	else {
		return;
	}
	//check folder
	string folder_buff;
	folder_buff = Fpl_Section::check_generate_folder("excel", &this->output_folder);
	//create name
	ostringstream buffer;
	buffer << folder_buff << "RES_MC_EXCEL_" << this->id_section << "_" << this->section_name << ".csv";

	//get the file name
	string filename;
	filename = buffer.str();
	//open the file
	ofstream tecplot_output;
	tecplot_output.open(filename.c_str());
	if (tecplot_output.is_open() == false) {
		Error msg = this->set_error(29);
		ostringstream info;
		info << "Filename : " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}



	if (type_of_section != NULL) {
		this->type_of_section->output_mc_res2excel(&tecplot_output, ptr_database, this->system_id, this->id_section);
	}

	//close the file
	tecplot_output.close();

}
//Output the FRC results to a tecplot file
void Fpl_Section::output_frc_results2tecplot(QSqlDatabase *ptr_database) {
	if (type_of_section != NULL) {
		if (type_of_section->get_output_control_flags().tec_output == false) {
			return;
		}
	}
	else {
		return;
	}
	//check folder
	string folder_buff;
	folder_buff = Fpl_Section::check_generate_folder("tecplot", &this->output_folder);


	//create name
	ostringstream buffer;
	buffer << folder_buff << "RES_DETERM_TEC_" << this->id_section << "_" << this->section_name << ".dat";

	//get the file name
	string filename;
	filename = buffer.str();
	//open the file
	ofstream tecplot_output;
	tecplot_output.open(filename.c_str());
	if (tecplot_output.is_open() == false) {
		Error msg = this->set_error(29);
		ostringstream info;
		info << "Filename : " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//output the file header
	tecplot_output << "TITLE = " << "\" FRC result of " << this->section_name << "; Id. " << this->id_section << "; ";
	if (this->left_flag == true) {
		tecplot_output << " left ";
	}
	else {
		tecplot_output << " right ";
	}
	tecplot_output << "; Start " << this->station_start << label::m << "; End " << this->station_end << label::m << " \"" << endl;


	if (type_of_section != NULL) {
		this->type_of_section->output_frc_res2tecplot(&tecplot_output, ptr_database, this->system_id, this->id_section);
	}

	//close the file
	tecplot_output.close();

}
//Output the FRC results to a paraview file
void Fpl_Section::output_frc_results2paraview(QSqlDatabase *ptr_database) {
	if (type_of_section != NULL) {
		if (type_of_section->get_output_control_flags().para_output == false) {
			return;
		}
	}
	else {
		return;
	}

	//check folder
	string folder_buff;
	folder_buff = Fpl_Section::check_generate_folder("paraview", &this->output_folder);
	//create name
	ostringstream buffer;
	buffer << folder_buff << "RES_FRC_PARA_" << this->id_section << "_" << this->section_name << ".csv";

	//get the file name
	string filename;
	filename = buffer.str();
	//open the file
	ofstream tecplot_output;
	tecplot_output.open(filename.c_str());
	if (tecplot_output.is_open() == false) {
		Error msg = this->set_error(29);
		ostringstream info;
		info << "Filename : " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	if (type_of_section != NULL) {
		this->type_of_section->output_frc_res2paraview(&tecplot_output, ptr_database, this->system_id, this->id_section);
	}

	//close the file
	tecplot_output.close();

}
//Output the FRC results to a excel file
void Fpl_Section::output_frc_results2excel(QSqlDatabase *ptr_database) {
	if (type_of_section != NULL) {
		if (type_of_section->get_output_control_flags().excel_output == false) {
			return;
		}
	}
	else {
		return;
	}
	//check folder
	string folder_buff;
	folder_buff = Fpl_Section::check_generate_folder("excel", &this->output_folder);
	//create name
	ostringstream buffer;
	buffer << folder_buff << "RES_FRC_EXCEL_" << this->id_section << "_" << this->section_name << ".csv";

	//get the file name
	string filename;
	filename = buffer.str();
	//open the file
	ofstream tecplot_output;
	tecplot_output.open(filename.c_str());
	if (tecplot_output.is_open() == false) {
		Error msg = this->set_error(29);
		ostringstream info;
		info << "Filename : " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}



	if (type_of_section != NULL) {
		this->type_of_section->output_frc_res2excel(&tecplot_output, ptr_database, this->system_id, this->id_section);
	}

	//close the file
	tecplot_output.close();

}
//Check and generate folder (static)
string Fpl_Section::check_generate_folder(const string folder_name, string *path) {
	QDir my_dir;
	ostringstream buffer;
	buffer << *path<<folder_name<<"/";
	if (my_dir.exists(buffer.str().c_str()) == false) {
		my_dir.mkdir(buffer.str().c_str());
		
	}
	return buffer.str();


}
//Output result members to database table
void Fpl_Section::output_result2table(QSqlDatabase *ptr_database, _fpl_simulation_type simulation_type, const int counter_mc_sim){
	ostringstream cout;
	Sys_Common_Output::output_fpl->reset_prefix_was_outputed();
	cout <<"Mechanism result members of section " << this->section_name << " to database table..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	if(this->type_of_section!=NULL){
		this->type_of_section->output_result2table(ptr_database, simulation_type, this->system_id, this->id_section, counter_mc_sim);
	}
}
//Set probabilistic results of a section per dialog
void Fpl_Section::set_probabilistic_result_per_dialog(QSqlDatabase *ptr_database){
	FplGui_Frc_Values_Dia my_dia;
	ostringstream cout;
	cout << "GENERAL  " <<  endl;
	cout << " Number                         " << this->id_section  << endl;
	cout << " Name                           " << this->section_name << endl;
	cout << " Type                           " << _Fpl_Section_Type::convert_section_type2txt(this->type)  << endl;
	cout << " Length                         " << this->section_length << label::m<<endl;
	cout << " Direction                      " << this->direction <<label::degree<<endl;
	cout << "HYDRAULIC CONNECTION"<<endl;
	cout << " Id-river                       " << this->breach_start.id_rv_model << endl;
	cout << " Left (true)/rigth bank (false) " << functions::convert_boolean2string(this->left_flag) << endl;
	cout << " Riverstation start             " << this->station_start << label::m << endl;
	cout << " Riverstation end               " << this->station_end << label::m << endl;
	cout << "BREAK PARAMETERS"<<endl;
	cout << " Structure height               " << this->structure_height<<label::m << endl;
	cout << " Maximum breach width           " << this->breach_start.max_breach_width<<label::m << endl;
	if(this->break_params.abrupt_fails_flag==false){
		cout << " Abrupt fails                   " << "false"<< endl;
		cout << " Resistance                     " << this->break_params.resistance<<label::m_per_sec<< endl;
	}
	else{
		cout << " Abrupt fails                   " << "true"<< endl;
		cout << " Resistance                     " << this->break_params.resistance<<label::stress_unit<< endl;
		cout << " Abrupt opening                 " << this->break_params.resistance<<label::m<< endl;
	}
	my_dia.show_frc_infos(cout.str().c_str());

	//init result
	_fpl_frc_result result;
	result.failure_prob=0.0;
	result.low_failure_prob=0.0;
	result.number_mc=0.0;
	result.time_mc=0.0;
	result.up_failure_prob=0.0;
	result.waterlevel=0.0;
	int number=0;
	//query model
	QSqlQueryModel my_model;

	//set already existing data
	//mc
	number=Fpl_Mc_Sim::select_results_in_database(&my_model, ptr_database, this->system_id, this->id_section, true);
	if(number>0){
		result.failure_prob=my_model.record(0).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_probability)).c_str()).toDouble();
		my_dia.set_annuality_failure(result.failure_prob);
	}
	//frc
	number=Fpl_Mc_Sim::select_results_in_database(&my_model, ptr_database, this->system_id,this->id_section, false);
	for(int i=1; i<number-1; i++){
		result.failure_prob=my_model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_probability)).c_str()).toDouble();
		result.waterlevel=my_model.record(i).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_waterlevel)).c_str()).toDouble();
		my_dia.set_data(result.waterlevel, result.failure_prob);
	}
	if(number>0){
		result.waterlevel=my_model.record(number-1).value((Fpl_Mc_Sim::result_table->get_column_name(fpl_label::reli_waterlevel)).c_str()).toDouble();
		my_dia.set_maximum_waterlevel(result.waterlevel);
	}

	if(my_dia.start_dialog()==true){
		//delete old data
		Fpl_Mc_Sim::delete_data_in_result_table(ptr_database, this->id_section, this->system_id, true);
		Fpl_Mc_Sim::delete_data_in_result_table(ptr_database, this->id_section, this->system_id, false);
		//set new data to table
		Fpl_Mc_Sim my_mc;

		//mc-result
		result.failure_prob=my_dia.get_annuality_failure();
		my_mc.set_result2resulttable_direct(ptr_database, this->id_section, this->system_id,true, result);

		//frc-result
		number=my_dia.get_number_points();
		for(int i=0; i<number; i++){
			result.failure_prob=my_dia.get_fail_prob(i);
			result.low_failure_prob=my_dia.get_fail_prob(i);
			result.up_failure_prob=my_dia.get_fail_prob(i);
			result.waterlevel=my_dia.get_water_level(i);
			my_mc.set_result2resulttable_direct(ptr_database, this->id_section, this->system_id,false, result);
		}
	}
	else{
		return;
	}
}
//Check the statistics of the random variables used in the section types
void Fpl_Section::check_statistic(void){
	if(type_of_section!=NULL){
		this->type_of_section->check_statistic();
	}
}
//Reset the statistic important values
void Fpl_Section::reset_statistics(void){
	if(type_of_section!=NULL){
		this->type_of_section->reset_statistics();
	}
}
//Output the statistics of the random variables used in the section types to display/console
void Fpl_Section::output_statistic(void){
	ostringstream prefix;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	if(type_of_section!=NULL){
		this->type_of_section->output_statistic();
	}

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Get the section id
int Fpl_Section::get_id_section(void){
	return this->id_section;
}
//Set the section id
void Fpl_Section::set_section_id(QSqlDatabase *ptr_database){
	this->id_section=Fpl_Section::evaluate_next_section_database(ptr_database);
}
//Get the name of the section
string Fpl_Section::get_name_section(void){
	return this->section_name;
}
//Get the type of the section
_fpl_section_types Fpl_Section::get_type_section(void){
	return this->type;
}
//Set the flag for a fragility curve (frc)-calculation
void Fpl_Section::set_frc_flag(const bool frc_flag){
	this->flag_frc_sim=frc_flag;
}
//Get a string for displaying the section for a selection
string Fpl_Section::get_string_selection(void){
	ostringstream buffer;

	buffer << this->id_section << "    " << this->section_name << "     " <<this->station_start<<label::m <<"    "<<_Fpl_Section_Type::convert_section_type2txt(this->type)<<"   "<<this->breach_start.id_rv_model;

	if(this->left_flag==true){
		buffer << "    " << "left";
	}
	else{
		buffer << "    " << "right";
	}

	return buffer.str();
}
//Check the file for the fpl-section if the !$BEGIN... always are close with !$END...
void Fpl_Section::check_input_file_section(const string file_name){
	ostringstream cout;
	cout << "Check FPL-file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	int line_counter=0;
	this->set_start_exception_number();
	QFile ifile;
	//open file
	ifile.setFileName(file_name.c_str());
	QIODevice::OpenMode my_flag;
	my_flag=my_flag|(QIODevice::ReadOnly);
	my_flag=my_flag|(QIODevice::Text);
	ifile.open(my_flag);
	if(ifile.isOpen()==false){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Filename: " << file_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//check the file
	string myline;
	QString qmyline;
	QStringList list_begin;
	QList<bool> list_flag;
	QString id_str;
	QString orgi;
	bool flag;
	try{
		do{
			qmyline=ifile.readLine();
			myline=qmyline.toStdString();
			line_counter++;
			functions::clean_string(&myline);
			id_str=myline.c_str();
			orgi=id_str;
			flag=functions::decide_begin_end_keyword(&id_str);
			if(flag==true && id_str.isEmpty()==false){
				if(list_flag.count()>0){
					if(list_flag.last()==true && list_begin.last()==id_str){
						Error msg=this->set_error(11);
						ostringstream info;
						info << "Error occurs near line: "<< line_counter << endl;
						info << "Filename              : "<< file_name << endl;
						info << "Key                   : "<< orgi.toStdString()<< endl;
						msg.make_second_info(info.str());
						throw msg;
					}
				}
				list_flag.append(true);
				list_begin.append(id_str);
				//cout << id_str.toStdString()<< " no_list "<< list_begin.count()<<endl;
			}
			else if(flag==false && id_str.isEmpty()==false){
				if(list_begin.count()>0 && list_begin.last()==id_str){
					list_begin.removeLast();
					list_flag.removeLast();
					//cout << id_str.toStdString()<< " no_list" << list_begin.count()<<endl;
				}
				else{
					Error msg=this->set_error(11);
					ostringstream info;
					info << "Error occurs near line: "<< line_counter << endl;
					info << "Filename              : "<< file_name << endl;
					info << "Key                   : "<< orgi.toStdString()<< endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
		while(ifile.atEnd()==false);

		if(list_begin.count()!=0){
			Error msg=this->set_error(12);
			ostringstream info;
			info << "Filename                 : "<< file_name << endl;
			info << "Number blocks not closed : "<< list_begin.count()<< endl;
			for(int i=0; i< list_begin.count(); i++){
				info << "Key                      : "<< "!$BEGIN_"<<list_begin.at(i).toStdString()<< endl;
			}
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	catch(Error msg){
		ifile.close();
		throw msg;
	}

	//close file
	ifile.close();
	this->set_exception_number();
	cout << "Check FPL-file is done." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->output_section_statistics();
}
//Output section statistics to display/console
void Fpl_Section::output_section_statistics(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << "Exception statistics of the FPL-system..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Section name             : " << W(3) << this->section_name <<endl;
	if(this->id_section<0){
		cout << " Section index            : " << W(3) << label::not_set <<endl;
	}
	else{
		cout << " Section index            : " << W(3) << this->id_section <<endl;
	}
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)                 : " << W(3) << this->number_error <<endl;
	cout << " Warning(s)               : " << W(3) << this->number_warning <<endl;

	if(Fpl_Calculation::get_stop_thread_flag()==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Set the number of exceptions (error/warning) before an action
void Fpl_Section::set_start_exception_number(void){
	this->number_warning_begin=Warning::get_number_fpl_warnings();
	this->number_error_begin=Error::get_number_fpl_errors();
}
//Set the exception number (error/warning), which occured during an action
void Fpl_Section::set_exception_number(void){
	this->number_warning=Warning::get_number_fpl_warnings()-this->number_warning_begin;
	this->number_error=Error::get_number_fpl_errors()-this->number_error_begin;
}
//Combine the section to the hydraulic river profiles or the coast dikeline points
void Fpl_Section::combine_section2hydraulic(Hyd_Hydraulic_System *hyd_system, QSqlDatabase *ptr_database){
	//river connection
	if(this->breach_start.id_rv_model>=0 && hyd_system->global_parameters.get_number_river_model()>this->breach_start.id_rv_model){
		int number_rv=hyd_system->global_parameters.get_number_river_model();
		bool river_found=false;
		int index_rv=0;

		//search for the river model
		for(int i=0; i< number_rv; i++){
			if(this->breach_start.id_rv_model==hyd_system->my_rvmodels[i].Param_RV.get_river_number()){
				index_rv=i;
				river_found=true;
				break;
			}
		}
		if(river_found==false){
			Warning msg=this->set_warning(2);
			ostringstream info;
			info << "Section name        : " << this->section_name << endl;
			info << "Section index       : " << this->id_section << endl;
			info << "River connection id : " << this->breach_start.id_rv_model << endl;
			msg.make_second_info(info.str());
			msg.output_msg(1);
			return;
		}
		else{
			this->set_length_polyline(&(hyd_system->my_rvmodels[index_rv]), ptr_database);
		}
	}
	//coastline connection
	else if(this->breach_start.id_rv_model<0 && hyd_system->global_parameters.get_coast_model_applied()==true){
		bool found=hyd_system->global_parameters.get_coast_model_applied();
		if(hyd_system->my_comodel==NULL){
			found=false;
		}
		if(found==false){
			Warning msg=this->set_warning(2);
			ostringstream info;
			info << "Section name        : " << this->section_name << endl;
			info << "Section index       : " << this->id_section << endl;
			info << "Coast model is not applied!" << endl;
			msg.make_second_info(info.str());
			msg.output_msg(1);
			return;
		}
		else{
			this->set_length_polyline(&(hyd_system->my_comodel->coastline_polysegment), ptr_database);
		}
	}
	//no connection
	else{
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Section name        : " << this->section_name << endl;
		info << "Section index       : " << this->id_section << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
		return;
	}
}
//Idealise the hydraulic system by the fpl-section
void Fpl_Section::idealise_hyd_by_section(Hyd_Hydraulic_System *hyd_system){
	if(this->type==_fpl_section_types::high_ground_sec){
		ostringstream cout;
		cout << "Highground-sections are not for the idealisation" << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}

	//river connection
	if(this->breach_start.id_rv_model>=0 && hyd_system->global_parameters.get_number_river_model()>this->breach_start.id_rv_model){
		int number_rv=hyd_system->global_parameters.get_number_river_model();
		bool river_found=false;
		int index_rv=0;

		//search for the river model
		for(int i=0; i< number_rv; i++){
			if(this->breach_start.id_rv_model==hyd_system->my_rvmodels[i].Param_RV.get_river_number()){
				index_rv=i;
				river_found=true;
				break;
			}
		}
		if(river_found==false){
			Warning msg=this->set_warning(9);
			ostringstream info;
			info << "Section name        : " << this->section_name << endl;
			info << "Section index       : " << this->id_section << endl;
			info << "River connection id : " << this->breach_start.id_rv_model << endl;
			msg.make_second_info(info.str());
			msg.output_msg(1);
			return;
		}
		else{
			try{
				//search for the profile in the river model (begin/end of the fpl-section)
				_Hyd_River_Profile *start_up=NULL;
				_Hyd_River_Profile *start_down=NULL;
				bool start_inbetween=false;

				start_inbetween=hyd_system->my_rvmodels[index_rv].find_ptr_upstream_downstream_prof(this->station_start, &start_up, &start_down);

				_Hyd_River_Profile *end_up=NULL;
				_Hyd_River_Profile *end_down=NULL;
				bool end_inbetween=false;

				end_inbetween=hyd_system->my_rvmodels[index_rv].find_ptr_upstream_downstream_prof(this->station_end, &end_up, &end_down);

				//set the parameters for idealisation
				_hyd_idealisation_parameters ideal_buffer;
				ideal_buffer.name=this->section_name;
				ideal_buffer.index=this->id_section;
				ideal_buffer.struct_height=this->structure_height;
				ideal_buffer.break_params=this->break_params;
				ostringstream cout;
				cout << "Idealise profile(s) of river " << hyd_system->my_rvmodels[index_rv].Param_RV.get_river_name() << " (Id "<<hyd_system->my_rvmodels[index_rv].Param_RV.get_river_number()<<")..."<<endl;
				Sys_Common_Output::output_fpl->output_txt(&cout);
				//idealisation
				hyd_system->my_rvmodels[index_rv].idealise_profile_by_fpl(this->left_flag, start_down, end_up, ideal_buffer, &this->breach_start, this->id_section);

				//transfer the involved river profile list
				this->number_involved_profiles=hyd_system->my_rvmodels[index_rv].get_number_involved_profiles();
				this->allocate_list_involved_profiles();
				for(int i=0; i< this->number_involved_profiles; i++){
					this->list_involved_profiles[i]=hyd_system->my_rvmodels[index_rv].get_ptr_involved_profile(i);
				}
			}
			catch(Error msg){
				ostringstream info;
				info << "River index : " << this->breach_start.id_rv_model << endl;
				info << "River name  : " <<hyd_system->my_rvmodels[index_rv].Param_RV.get_river_name()<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}
	//coast model
	else if(this->breach_start.id_rv_model<0 && hyd_system->global_parameters.get_coast_model_applied()==true){
		bool found=hyd_system->global_parameters.get_coast_model_applied();
		if(hyd_system->my_comodel==NULL){
			found=false;
		}
		if(found==false){
			Warning msg=this->set_warning(9);
			ostringstream info;
			info << "Section name        : " << this->section_name << endl;
			info << "Section index       : " << this->id_section << endl;
			info << "Coast model is not applied!" << endl;
			msg.make_second_info(info.str());
			msg.output_msg(1);
			return;
		}
		else{
			try{
				//search for the point in the coast model
				Hyd_Floodplain_Dikeline_Point *start_up=NULL;
				Hyd_Floodplain_Dikeline_Point *start_down=NULL;
				bool start_inbetween=false;

				start_inbetween=hyd_system->my_comodel->coastline_polysegment.find_ptr_up_down_point(this->station_start, &start_up, &start_down);

				Hyd_Floodplain_Dikeline_Point *end_up=NULL;
				Hyd_Floodplain_Dikeline_Point *end_down=NULL;
				bool end_inbetween=false;

				end_inbetween=hyd_system->my_comodel->coastline_polysegment.find_ptr_up_down_point(this->station_end, &end_up, &end_down);

				//set the parameters for idealisation
				_hyd_idealisation_parameters ideal_buffer;
				ideal_buffer.name=this->section_name;
				ideal_buffer.index=this->id_section;
				ideal_buffer.struct_height=this->structure_height;
				ideal_buffer.break_params=this->break_params;
				ostringstream cout;
				cout << "Idealise point(s) of the coast dikeline " << hyd_system->my_comodel->coastline_polysegment.get_name() << " (Id "<<hyd_system->my_comodel->coastline_polysegment.get_index()<<")..."<<endl;
				Sys_Common_Output::output_fpl->output_txt(&cout);
				//idealisation
				hyd_system->my_comodel->coastline_polysegment.idealise_points_by_fpl(start_down, end_up, ideal_buffer, &this->breach_start, this->id_section);

				//transfer the involved points list
				this->number_involved_points=hyd_system->my_comodel->coastline_polysegment.get_number_involved_points();
				this->allocate_list_involved_points();
				for(int i=0; i< this->number_involved_points; i++){
					this->list_involved_points[i]=hyd_system->my_comodel->coastline_polysegment.get_ptr_involved_points(i);
				}
			}
			catch(Error msg){
				ostringstream info;
				info << "Coast model  "<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}
	//no connection
	else{
		Warning msg=this->set_warning(9);
		ostringstream info;
		info << "Section name        : " << this->section_name << endl;
		info << "Section index       : " << this->id_section << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
		return;
	}
}
//Idealise the hydraulic river model by the fpl-section
void Fpl_Section::idealise_hyd_by_section(Hyd_Model_River *hyd_river){
	if(this->type==_fpl_section_types::high_ground_sec){
		ostringstream cout;
		cout << "Highground-sections are not for the idealisation" << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}

	//river connection
	if(this->breach_start.id_rv_model==hyd_river->Param_RV.get_river_number()){
		try{
			//search for the profile in the river model (begin/end of the fpl-section)
			_Hyd_River_Profile *start_up=NULL;
			_Hyd_River_Profile *start_down=NULL;
			bool start_inbetween=false;

			start_inbetween=hyd_river->find_ptr_upstream_downstream_prof(this->station_start, &start_up, &start_down);

			_Hyd_River_Profile *end_up=NULL;
			_Hyd_River_Profile *end_down=NULL;
			bool end_inbetween=false;

			end_inbetween=hyd_river->find_ptr_upstream_downstream_prof(this->station_end, &end_up, &end_down);

			//set the parameters for idealisation
			_hyd_idealisation_parameters ideal_buffer;
			ideal_buffer.name=this->section_name;
			ideal_buffer.index=this->id_section;
			ideal_buffer.struct_height=this->structure_height;
			ideal_buffer.break_params=this->break_params;
			ostringstream cout;
			cout << "Idealise profile(s) of river " << hyd_river->Param_RV.get_river_name() << " (Id "<<hyd_river->Param_RV.get_river_number()<<")..."<<endl;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//idealisation
			hyd_river->idealise_profile_by_fpl(this->left_flag, start_down, end_up, ideal_buffer, &this->breach_start, this->id_section);

			//transfer the involved river profile list
			this->number_involved_profiles=hyd_river->get_number_involved_profiles();
			this->allocate_list_involved_profiles();
			for(int i=0; i< this->number_involved_profiles; i++){
				this->list_involved_profiles[i]=hyd_river->get_ptr_involved_profile(i);
			}
		}
		catch(Error msg){
			ostringstream info;
			info << "River index : " << this->breach_start.id_rv_model << endl;
			info << "River name  : " <<hyd_river->Param_RV.get_river_name()<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	//no connection
	else{
		return;
	}
}
//Load the list of involved river profile/dikeline points for the fpl-section
void Fpl_Section::load_list_involved_profiles_points(Hyd_Hydraulic_System *hyd_system){
	if(this->type==_fpl_section_types::high_ground_sec){
		return;
	}

	//river model
	if(this->breach_start.id_rv_model>=0 && hyd_system->global_parameters.get_number_river_model()>this->breach_start.id_rv_model){
		int number_rv=hyd_system->global_parameters.get_number_river_model();
		bool river_found=false;
		int index_rv=0;

		//search for the river model
		for(int i=0; i< number_rv; i++){
			if(this->breach_start.id_rv_model==hyd_system->my_rvmodels[i].Param_RV.get_river_number()){
				index_rv=i;
				river_found=true;
				break;
			}
		}
		if(river_found==false){
			Error msg=this->set_error(18);
			ostringstream info;
			info << "Section name        : " << this->section_name << endl;
			info << "Section index       : " << this->id_section << endl;
			info << "River connection id : " << this->breach_start.id_rv_model << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		else{
			try{
				//search for the profile in the river model (begin/end of the fpl-section)
				_Hyd_River_Profile *start_up=NULL;
				_Hyd_River_Profile *start_down=NULL;
				bool start_inbetween=false;

				start_inbetween=hyd_system->my_rvmodels[index_rv].find_ptr_upstream_downstream_prof(this->station_start, &start_up, &start_down);

				_Hyd_River_Profile *end_up=NULL;
				_Hyd_River_Profile *end_down=NULL;
				bool end_inbetween=false;

				end_inbetween=hyd_system->my_rvmodels[index_rv].find_ptr_upstream_downstream_prof(this->station_end, &end_up, &end_down);

				//idealisation
				Geo_Polysegment buff_seg;
				hyd_system->my_rvmodels[index_rv].get_polyline_fpl_section(this->left_flag, &buff_seg, start_down, end_up);

				//transfer the involved river profile list
				this->number_involved_profiles=hyd_system->my_rvmodels[index_rv].get_number_involved_profiles();
				this->allocate_list_involved_profiles();
				for(int i=0; i< this->number_involved_profiles; i++){
					this->list_involved_profiles[i]=hyd_system->my_rvmodels[index_rv].get_ptr_involved_profile(i);
				}
			}
			catch(Error msg){
				ostringstream info;
				info << "River index : " << this->breach_start.id_rv_model << endl;
				info << "River name  : " <<hyd_system->my_rvmodels[index_rv].Param_RV.get_river_name()<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}
	//coast model
	else if(this->breach_start.id_rv_model<0 && hyd_system->global_parameters.get_coast_model_applied()==true){
		bool found=false;
		if(hyd_system->my_comodel==NULL){
			found=false;
		}
		if(found==false){
			Error msg=this->set_error(18);
			ostringstream info;
			info << "Section name        : " << this->section_name << endl;
			info << "Section index       : " << this->id_section << endl;
			info << "Coast model is not applied!" << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		else{
			try{
				//search for the profile in the river model (begin/end of the fpl-section)
				Hyd_Floodplain_Dikeline_Point *start_up=NULL;
				Hyd_Floodplain_Dikeline_Point *start_down=NULL;
				bool start_inbetween=false;

				start_inbetween=hyd_system->my_comodel->coastline_polysegment.find_ptr_up_down_point(this->station_start, &start_up, &start_down);

				Hyd_Floodplain_Dikeline_Point *end_up=NULL;
				Hyd_Floodplain_Dikeline_Point *end_down=NULL;
				bool end_inbetween=false;

				end_inbetween=hyd_system->my_comodel->coastline_polysegment.find_ptr_up_down_point(this->station_end, &end_up, &end_down);

				//idealisation
				Geo_Polysegment buff_seg;
				hyd_system->my_comodel->coastline_polysegment.get_polyline_fpl_section(&buff_seg, start_down, end_up);

				//transfer the involved river profile list
				this->number_involved_points=hyd_system->my_comodel->coastline_polysegment.get_number_involved_points();
				this->allocate_list_involved_points();
				for(int i=0; i< this->number_involved_points; i++){
					this->list_involved_points[i]=hyd_system->my_comodel->coastline_polysegment.get_ptr_involved_points(i);
				}
			}
			catch(Error msg){
				ostringstream info;
				info << "Coast model  "<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}
	//no connection
	else{
		Error msg=this->set_error(18);
		ostringstream info;
		info << "Section name        : " << this->section_name << endl;
		info << "Section index       : " << this->id_section << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check the hydraulic connection parameters
void Fpl_Section::check_connection_params(void){
	if(this->breach_start.id_rv_model>=0){
		if(this->station_start<=this->station_end){
			Error msg=this->set_error(13);
			ostringstream info;
			info <<"Station start : "<< this->station_start << label::m << endl;
			info <<"Station end   : "<< this->station_end << label::m << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	else{
		if(this->station_start>=this->station_end){
			Error msg=this->set_error(13);
			ostringstream info;
			info <<"Station start : "<< this->station_start << label::m << endl;
			info <<"Station end   : "<< this->station_end << label::m << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	//if(this->breach_start.id_rv_model<0){
	//	Warning msg=this->set_warning(4);
	//	ostringstream info;
	//	info <<"Given river id     : "<< this->breach_start.id_rv_model << endl;
	//	info <<"Section name       : "<< this->section_name << endl;
	//	info <<"Section index      : "<< this->id_section << endl;
	//	msg.make_second_info(info.str());
	//	msg.output_msg(1);
	//}

	if(this->breach_start.max_breach_width<0.0){
		Warning msg=this->set_warning(3);
		ostringstream info;
		info <<"Given breach width : "<< this->station_start << label::m << endl;
		info <<"Section name       : "<< this->section_name << endl;
		info <<"Section index      : "<< this->id_section << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
		this->breach_start.max_breach_width=0.0;
	}

	if(this->similarity_factor<0.0 ){
		Warning msg=this->set_warning(11);
		ostringstream info;
		info <<"Section name       : "<< this->section_name << endl;
		info <<"Section index      : "<< this->id_section << endl;
		info <<"Similarity factor  : "<< this->similarity_factor<< endl;
		this->similarity_lenght=-1.0;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	if(this->similarity_factor>1.0 ){
		Warning msg=this->set_warning(12);
		ostringstream info;
		info <<"Section name       : "<< this->section_name << endl;
		info <<"Section index      : "<< this->id_section << endl;
		info <<"Similarity factor  : "<< this->similarity_factor<< endl;
		this->similarity_lenght=-1.0;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
}
//Get the breach starting parameter structure
_hyd_breach_start_parameters Fpl_Section::get_breach_start_parameters(void){
	return this->breach_start;
}
//Get the left bank flag
bool Fpl_Section::get_left_bank_flag(void){
	return this->left_flag;
}
//Set the starting breach height
void Fpl_Section::set_starting_breach_height(const double breach_height_start){
	this->breach_start.start_height=breach_height_start;
}
//Get the number of involved river profiles
int Fpl_Section::get_number_involved_profiles(void){
	return this->number_involved_profiles;
}
//Get the list of the involved river profiles
_Hyd_River_Profile** Fpl_Section::get_list_involved_river_profiles(void){
	return this->list_involved_profiles;
}
//Get the number of involved coast dikeline points
int Fpl_Section::get_number_involved_points(void){
	return this->number_involved_points;
}
//Get the list of the involved coast dikeline points
Hyd_Floodplain_Dikeline_Point** Fpl_Section::get_list_involved_coast_dikeline_points(void){
	return this->list_involved_points;
}
//Calculate the probability per fragility curve with the information of the waterlevel of the involved river sections
_fpl_frc_result Fpl_Section::calculate_probability_per_frc(QSqlDatabase *ptr_database, const int id_hyd_sc, const string break_id, const bool break_flag){
	_fpl_frc_result prob;
	prob.failure_prob=1.0;
	prob.low_failure_prob=1.0;
	prob.up_failure_prob=1.0;

	try{
		if(this->allocate_frc_data()==true){
			this->frc_data->set_curve_data(ptr_database, this->system_id, this->id_section);
		}
		double max_waterlevel=0.0;
		max_waterlevel=this->evaluate_max_waterlevel_involved_section(ptr_database, id_hyd_sc, break_id);
		prob=this->frc_data->interpolate_value(max_waterlevel, break_flag);
	}
	catch(Error msg){
		ostringstream info;
		info << "Section name        : " << this->section_name << endl;
		info << "Section index       : " << this->id_section << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return prob;
}
//Load and initialize the frc-curve and their dividing intervals
void Fpl_Section::init_intervals_frc_curve(QSqlDatabase *ptr_database, const double max_waterlevel){
	try{
		if(this->allocate_frc_data()==true){
			this->frc_data->set_curve_data(ptr_database, this->system_id, this->id_section);
		}
		this->frc_data->init_intervals(max_waterlevel);
	}
	catch(Error msg){
		ostringstream info;
		info << "Section name        : " << this->section_name << endl;
		info << "Section index       : " << this->id_section << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Generate the interval and the break height via Monte Carlo analysis and get the identifier
string Fpl_Section::get_relevant_interval_frc_curve(_Fpl_Random_Generator *ran_gen){
	int tries=1;
	if(this->section_length>0.0){
		tries=(int)(this->section_length/this->similarity_lenght);
	}
	if(tries<1){
		tries=1;
	}

	return this->frc_data->get_interval_identifier(ran_gen, tries, this->similarity_factor);
}
//Transfer the interval data to database
void Fpl_Section::transfer_interval_data2database(QSqlDatabase *ptr_database, const int sz_bound_id){
	if(this->frc_data==NULL){
		return;
	}

	this->frc_data->output_intervals(this);
	this->frc_data->transfer_intervals2database(ptr_database, this->system_id, this->id_section, sz_bound_id);
}
//Get the pointer to the frc-data
Fpl_Frc_Curve* Fpl_Section::get_ptr_frc_data(void){
	return this->frc_data;
}
//Evaluate the maximal waterlevel in the involved river section for a given scenario
double Fpl_Section::evaluate_max_waterlevel_involved_section(QSqlDatabase *ptr_database, const int id_hyd_sc, const string break_id){
	QSqlQueryModel model;
	//double max_h=0.0;
	double h_buff=0.0;
	int index_buff=0;
	int number=0;
	number=Hyd_Coupling_RV2FP_Merged::set_hmax_in_max_h_table(ptr_database, this->system_id, id_hyd_sc, break_id, this->id_section, &index_buff, &h_buff);
	if(number==0){
		Error msg=this->set_error(17);
		ostringstream info;
		info << "Fpl section id       : " << this->id_section <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return h_buff;
}
//Overwrite the main section parameters with the given one
void Fpl_Section::overwrite_main_section_parameter(Fpl_Section *given_sec){
	if(this->type!=given_sec->type){
		Error msg=this->set_error(19);
		ostringstream info;
		info <<"Section type given in file   : "<< _Fpl_Section_Type::convert_section_type2txt(this->type) << endl;
		info <<"Section-id to replace        : "<< given_sec->id_section << endl;
		info <<"Section-name to replace      : "<< given_sec->section_name  << endl;
		info <<"Section type in database     : "<< _Fpl_Section_Type::convert_section_type2txt(given_sec->type)  << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(this->check_fpl_section_similar_hyd_param(given_sec)==false){
		Warning msg=this->set_warning(13);
		ostringstream info;
		info <<"Id of the relevant section   : "<< given_sec->id_section << endl;
		info <<"Name of the relevant section : "<< given_sec->section_name  << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	this->copy_main_section_parameter(given_sec);
}
//Compare the geometrical settings of a section
void Fpl_Section::compare_geometrical_settings(Fpl_Section *section){
	if(this->left_flag!=section->left_flag){
		return;
	}
	if(this->breach_start.id_rv_model!=section->breach_start.id_rv_model){
		return;
	}
	if(this->system_id.measure_nr!=section->system_id.measure_nr){
		return;
	}

	if(abs(this->station_end-section->station_end)< constant::meter_epsilon &&
		abs(this->station_start-section->station_start)< constant::meter_epsilon){
			Error msg=this->set_error(20);
			ostringstream info;
			info<<"Existing Fpl-Section (id)    : " << this->section_name <<" ("<< this->id_section<<")"<<endl;
			info<<"New Fpl-Section (id)         : " << section->section_name <<" ("<< section->id_section<<")"<<endl;
			msg.make_second_info(info.str());
			throw msg;
	}

	if(this->station_start<=section->station_start && this->station_end>=section->station_end){
		Error msg=this->set_error(21);
		ostringstream info;
		info<<"Existing Fpl-Section (id)    : " << this->section_name <<" ("<< this->id_section<<")"<<endl;
		info<<"New Fpl-Section (id)         : " << section->section_name <<" ("<< section->id_section<<")"<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(section->station_start<=this->station_start && section->station_end>=this->station_end){
		Error msg=this->set_error(22);
		ostringstream info;
		info<<"Existing Fpl-Section (id)    : " << this->section_name <<" ("<< this->id_section<<")"<<endl;
		info<<"New Fpl-Section (id)         : " << section->section_name <<" ("<< section->id_section<<")"<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the name of the output folder
void Fpl_Section::set_output_folder_name(const string name){
	this->output_folder=name;
}
//Evaluate the next section id from the database table (static)
int Fpl_Section::evaluate_next_section_database(QSqlDatabase *ptr_database){
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	return Fpl_Section::table->maximum_int_of_column(Fpl_Section::table->get_column_name(fpl_label::section_id), ptr_database)+1;
}
//______________
//private
//Allocate the section type
void Fpl_Section::allocate_section_type(void){
	try{
		switch(this->type){
			case _fpl_section_types::by_hand_sec://is set by hand todo
				break;
				//it is not more supported
			//case _fpl_section_types::high_ground_sec://highground
			//	this->type_of_section= new Fpl_Sec_Type_Highground();
			//	this->type_of_section->set_section_direction(this->direction);
			//	this->type_of_section->set_ptr_waterside_crest(&this->crest_water);
			//	this->type_of_section->set_systemid(this->system_id);
			//	this->breach_start.max_breach_width=0.0;
			//	this->must_found=4;
			//	break;
			case _fpl_section_types::dike_sec://dike
				this->type_of_section= new Fpl_Sec_Type_Dike();
				this->type_of_section->set_section_direction(this->direction);
				this->type_of_section->set_ptr_waterside_crest(&this->crest_water);
				this->type_of_section->set_systemid(this->system_id);
				this->break_params.abrupt_fails_flag=false;
				this->must_found=5;
				break;
			case _fpl_section_types::dune_sec://dune
				this->type_of_section= new Fpl_Sec_Type_Dune();
				this->type_of_section->set_section_direction(this->direction);
				this->type_of_section->set_ptr_waterside_crest(&this->crest_water);
				this->type_of_section->set_systemid(this->system_id);
				this->break_params.abrupt_fails_flag=false;
				this->must_found=5;
				break;
			case _fpl_section_types::gate_sec://gate
				this->type_of_section= new Fpl_Sec_Type_Gate();
				this->type_of_section->set_section_direction(this->direction);
				this->type_of_section->set_ptr_waterside_crest(&this->crest_water);
				this->type_of_section->set_systemid(this->system_id);
				this->break_params.abrupt_fails_flag=true;
				this->must_found=5;
				break;

			case _fpl_section_types::wall_sec://wall
				//this->type_of_section= new Fpl_Sec_Type_Wall();
				//this->type_of_section->set_section_direction(this->direction);
				//this->type_of_section->set_ptr_waterside_crest(&this->crest_water);
				//this->type_of_section->set_systemid(this->system_id);
				this->break_params.abrupt_fails_flag=true;
				this->must_found=6;
				break;
			case _fpl_section_types::mob_wall_sec://mobile_wall
				//this->type_of_section= new Fpl_Sec_Type_Mobil_Wall();
				//this->type_of_section->set_section_direction(this->direction);
				//this->type_of_section->set_ptr_waterside_crest(&this->crest_water);
				//this->type_of_section->set_systemid(this->system_id);
				this->break_params.abrupt_fails_flag=true;
				this->must_found=6;
				break;
			default://unknown type; it is set to zero
				Warning msg=set_warning(0);
				msg.output_msg(1);
		}
	}
	catch(bad_alloc &t){
		Error msg=set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the section type
void Fpl_Section::delete_section_type(void){
	if(this->type_of_section!=NULL){
		delete this->type_of_section;
		this->type_of_section=NULL;
	}
}
//Allocate the list of involved river profiles
void Fpl_Section::allocate_list_involved_profiles(void){
	this->delete_list_involved_profiles();

	try{
		this->list_involved_profiles=new _Hyd_River_Profile*[this->number_involved_profiles];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(15);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//init
	for(int i=0; i< this->number_involved_profiles; i++){
		this->list_involved_profiles[i]=NULL;
	}
}
//Delete the list of involved river profiles
void Fpl_Section::delete_list_involved_profiles(void){
	if(this->list_involved_profiles!=NULL){
		delete []this->list_involved_profiles;
		this->list_involved_profiles=NULL;
	}
}
//Allocate the list of involved coast dikeline-points
void Fpl_Section::allocate_list_involved_points(void){
	this->delete_list_involved_points();

	try{
		this->list_involved_points=new Hyd_Floodplain_Dikeline_Point*[this->number_involved_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(24);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//init
	for(int i=0; i< this->number_involved_points; i++){
		this->list_involved_points[i]=NULL;
	}
}
//Delete the list of involved coast dikeline-points
void Fpl_Section::delete_list_involved_points(void){
	if(this->list_involved_points!=NULL){
		delete []this->list_involved_points;
		this->list_involved_points=NULL;
	}
}
//Allocate the frc-curve data
bool Fpl_Section::allocate_frc_data(void){
	if(this->frc_data==NULL){
		try{
			this->frc_data=new Fpl_Frc_Curve;
			return true;
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(16);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	else{
		return false;
	}
}
//Delete the frc-curve data
void Fpl_Section::delete_frc_data(void){
	if(this->frc_data!=NULL){
		delete this->frc_data;
		this->frc_data=NULL;
	}
}
//Set the waterside crest point per database table
void Fpl_Section::set_crest_water_database(QSqlDatabase *ptr_database, const bool applied_important){
	string buffer;
	try{
		buffer=_Fpl_Section_Type::convert_section_type2txt(this->type);

		this->crest_water.set_point_per_database(this->system_id, this->id_section, fpl_label::crest_water, ptr_database, fpl_label::region_cubature, buffer, false, applied_important);
	}
	catch(Error msg){
		throw msg;
	}
	Geo_Point buff_p;
	buff_p.set_point_coordinate(0.0,0.0);
	this->structure_height=this->crest_water.vertical_distance(&buff_p);
}
//Transfer crest point to database
void Fpl_Section::transfer_crest_water2database(QSqlDatabase *ptr_database){
	this->crest_water.transfer_point2database(this->system_id, this->id_section, ptr_database, fpl_label::region_cubature, _Fpl_Section_Type::convert_section_type2txt(this->type));
}
//Read general section data from file
void Fpl_Section::read_general_section_data_per_file(QFile *ifile, int *line_counter){
	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	int must_found_counter=0;
	ostringstream cout;
	cout << "Input the general fpl-section data..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	//search for the begin
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==fpl_label::begin_sectioninfo){
			found_flag=true;
			break;
		}
	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(2);
		throw msg;
	}
	else{
		found_flag=false;
	}
	//search for the keywords
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::end_sectioninfo){
			found_flag=true;
			break;
		}
		else if(myline==fpl_label::begin_hyd_connection){
			break;
		}
		else if(myline.empty()==false){
			this->find_key_values_general_file(myline, &must_found_counter);
		}
	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(3);
		throw msg;
	}

	this->must_found=5;
	if(must_found_counter!=this->must_found){
		Error msg=this->set_error(4);
		throw msg;
	}

	cout << "Input of the general fpl-section data is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Read hydraulic connection data of the section from file
void Fpl_Section::read_hyd_connection_data_per_file(QFile *ifile, int *line_counter){
	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	int must_found_counter=0;
	ostringstream cout;
	cout << "Input the hydraulic-connection fpl-section data..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	//search for the begin
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==fpl_label::begin_hyd_connection){
			found_flag=true;
			break;
		}
	}
	while(ifile->atEnd()==false);

	if(found_flag==false && Sys_Project::get_project_type()==_sys_project_type::proj_fpl_file){
		return;
	}
	else if(found_flag==false && Sys_Project::get_project_type()==_sys_project_type::proj_fpl){
		return;
	}
	else if(found_flag==false){
		Error msg=this->set_error(6);
		throw msg;
	}
	else{
		found_flag=false;
	}
	//search for the keywords
	if(this->type==_fpl_section_types::high_ground_sec){
		this->must_found=2;
	}
	else if(this->type==_fpl_section_types::dike_sec || this->type==_fpl_section_types::dune_sec){
		this->must_found=3;
	}
	else if(this->type==_fpl_section_types::by_hand_sec){
		this->must_found=3;
	}
	else if(this->type==_fpl_section_types::wall_sec || this->type==_fpl_section_types::mob_wall_sec){
		this->must_found=4;
	}

	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::end_hyd_connection){
			found_flag=true;
			break;
		}
		else if(myline.empty()==false){
			this->find_key_hyd_connection_file(myline, &must_found_counter);
		}
	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(7);
		throw msg;
	}

	if(must_found_counter!=this->must_found){
		Error msg=this->set_error(8);
		throw msg;
	}
	cout << "Input of the hydraulic-connection fpl-section data is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Find the key-values for the file input of the general section data
void Fpl_Section::find_key_values_general_file(const string myline, int *must_found_counter){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(fpl_label::key_sec_name );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_sec_name.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->section_name;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_sec_type);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_sec_type.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			this->type=_Fpl_Section_Type::convert_txt2section_type(buffer1.str());
		}
		catch(Error msg){
			msg.output_msg(1);
			wrong_input=true;
			return;
		}
		(*must_found_counter)++;
		return;
	}

	pos=myline.find(fpl_label::key_sec_direction );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_sec_direction.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->direction;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_sec_crestwater_x );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_sec_crestwater_x.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		double buff_coor=0.0;
		buffer1 >> buff_coor;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			this->crest_water.set_point_x_coordinate(buff_coor);
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_sec_crestwater_y );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_sec_crestwater_y.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		double buff_coor=0.0;
		buffer1 >> buff_coor;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			this->crest_water.set_point_y_coordinate(buff_coor);
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_sec_crestwater_zone );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_sec_crestwater_zone.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		int buff_zone=0;
		buffer1 >> buff_zone;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			this->crest_water.set_zone_number(buff_zone);
			return;
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(5);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Find the key-values for the file input of the hydraulic connection data of the section
void Fpl_Section::find_key_hyd_connection_file(const string myline, int *must_found_counter){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(fpl_label::key_rv_conn_id );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_rv_conn_id.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->breach_start.id_rv_model;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			if(this->inp_river_sec==false){
				this->inp_river_sec=true;
				this->must_found=this->must_found+2;
				(*must_found_counter)++;
			}
			else{
				(*must_found_counter)++;
			}
			return;
		}
	}

	pos=myline.find(fpl_label::key_leftdirection);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_leftdirection.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			this->left_flag=functions::convert_string2boolean(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
		}
		if(wrong_input!=true){
			if(this->inp_river_sec==false){
				this->inp_river_sec=true;
				this->must_found=this->must_found+2;
				(*must_found_counter)++;
			}
			else{
				(*must_found_counter)++;
			}
			return;
		}
	}

	pos=myline.find(fpl_label::key_simil_length);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_simil_length.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->similarity_lenght;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	pos=myline.find(fpl_label::key_simil_factor);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_simil_factor.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->similarity_factor;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	pos=myline.find(fpl_label::key_station_start );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_station_start.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->station_start;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_station_end );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_station_end.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->station_end;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_max_breach_width );
	if(pos>=0 && wrong_input==false && this->type!=_fpl_section_types::high_ground_sec ){
		buffer=myline.substr(fpl_label::key_max_breach_width.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->breach_start.max_breach_width;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}
	pos=myline.find(fpl_label::key_start_breach_height );
	if(pos>=0 && wrong_input==false ){
		buffer=myline.substr(fpl_label::key_start_breach_height.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->breach_start.start_height;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	pos=myline.find(fpl_label::key_critical_velocity );
	if(pos>=0 && wrong_input==false && this->type!=_fpl_section_types::high_ground_sec && this->type!=_fpl_section_types::wall_sec &&
		this->type!=_fpl_section_types::mob_wall_sec){
		buffer=myline.substr(fpl_label::key_critical_velocity.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->break_params.resistance;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			this->break_params.abrupt_fails_flag=false;
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_wall_resistance);
	if(pos>=0 && wrong_input==false && this->type!=_fpl_section_types::high_ground_sec && this->type!=_fpl_section_types::dike_sec){
		buffer=myline.substr(fpl_label::key_wall_resistance.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->break_params.resistance;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			if(this->break_params.abrupt_fails_flag==false && this->type==_fpl_section_types::by_hand_sec){
				this->must_found++;
			}
			this->break_params.abrupt_fails_flag=true;

			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_abrupt_breach_open );
	if(pos>=0 && wrong_input==false && this->type!=_fpl_section_types::high_ground_sec && this->type!=_fpl_section_types::dike_sec){
		buffer=myline.substr(fpl_label::key_abrupt_breach_open.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->break_params.abrupt_opening;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			if(this->break_params.abrupt_fails_flag==false && this->type==_fpl_section_types::by_hand_sec){
				this->must_found++;
			}
			this->break_params.abrupt_fails_flag=true;
			(*must_found_counter)++;
			return;
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(9);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Transfer section members to database
void Fpl_Section::transfer_section2database(QSqlDatabase *ptr_database){
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	if(this->id_section<=0){
		this->id_section=Fpl_Section::evaluate_next_section_database(ptr_database);
	}
	int glob_id=Fpl_Section::table->maximum_int_of_column(Fpl_Section::table->get_column_name(fpl_label::glob_id), ptr_database)+1;

	//transfer the data
	ostringstream query_string;
	query_string << "INSERT INTO  " << Fpl_Section::table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Section::table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Section::table->get_column_name(fpl_label::section_id) <<" , ";
	query_string << Fpl_Section::table->get_column_name(label::areastate_id) <<" , ";
	query_string << Fpl_Section::table->get_column_name(label::measure_id) <<" , ";
	query_string << Fpl_Section::table->get_column_name(label::applied_flag) <<" , ";

	query_string << Fpl_Section::table->get_column_name(fpl_label::section_name) <<" , ";
	query_string << Fpl_Section::table->get_column_name(fpl_label::section_type) <<" , ";
	query_string << Fpl_Section::table->get_column_name(fpl_label::section_direction) <<" , ";

	query_string << Fpl_Section::table->get_column_name(fpl_label::section_similarity_length) <<" , ";
	query_string << Fpl_Section::table->get_column_name(fpl_label::section_similarity_factor) <<" , ";

	query_string << Fpl_Section::table->get_column_name(fpl_label::section_conn_rv_id) <<" , ";
	query_string << Fpl_Section::table->get_column_name(fpl_label::section_rv_bank_flag) <<" , ";
	query_string << Fpl_Section::table->get_column_name(fpl_label::section_station_start) <<" , ";
	query_string << Fpl_Section::table->get_column_name(fpl_label::section_station_end) <<" , ";

	query_string << Fpl_Section::table->get_column_name(fpl_label::section_max_breach) <<" , ";
	query_string << Fpl_Section::table->get_column_name(fpl_label::section_start_breach_h) <<" , ";

	query_string << Fpl_Section::table->get_column_name(fpl_label::section_abrupt_breach) <<" , ";
	query_string << Fpl_Section::table->get_column_name(fpl_label::section_resistance_breach) <<" , ";
	query_string << Fpl_Section::table->get_column_name(fpl_label::section_abrupt_open_width)<< " , ";
	query_string << Fpl_Section::table->get_column_name(fpl_label::structure_height);
	query_string <<" ) ";
	query_string << " VALUES ( ";

	query_string << glob_id << " , " ;
	query_string << this->id_section<< " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << "true" << " , " ;

	query_string <<"'"<< this->section_name << "' , " ;
	query_string <<"'"<< _Fpl_Section_Type::convert_section_type2txt(this->type) << "' , " ;
	query_string << this->direction << " , ";
	query_string << this->similarity_lenght << " , ";
	query_string << this->similarity_factor << " , ";

	query_string << this->breach_start.id_rv_model << " , ";
	query_string << functions::convert_boolean2string(this->left_flag) << " , " ;
	query_string << this->station_start << " , " ;
	query_string << this->station_end << " , " ;

	query_string << this->breach_start.max_breach_width << " , " ;
	query_string << this->breach_start.start_height << " , " ;
	query_string << functions::convert_boolean2string(this->break_params.abrupt_fails_flag) << " , " ;
	query_string << this->break_params.resistance << " , " ;
	query_string << this->break_params.abrupt_opening << " , " ;
	query_string << this->structure_height << " ) " ;

	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, query_string.str(), ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Section index             : " << this->id_section << endl;
		info << "Section name              : " << this->section_name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
}
//Set the polyline-string and the section length due to combining with the river profiles of the section
void Fpl_Section::set_length_polyline(Hyd_Model_River *river, QSqlDatabase *ptr_database){
	_Hyd_River_Profile *start_up=NULL;
	_Hyd_River_Profile *start_down=NULL;
	bool start_inbetween=false;

	start_inbetween=river->find_ptr_upstream_downstream_prof(this->station_start, &start_up, &start_down);

	_Hyd_River_Profile *end_up=NULL;
	_Hyd_River_Profile *end_down=NULL;
	bool end_inbetween=false;

	end_inbetween=river->find_ptr_upstream_downstream_prof(this->station_end, &end_up, &end_down);

	if(start_inbetween ==false && end_inbetween==false){
		Error msg=this->set_error(14);
		ostringstream info;
		info << "Section index     : " <<  this->id_section << endl;
		info << "Section name      : " << this->section_name<< endl;
		info << "Start-riverstation: " << this->station_start<<label::m<<endl;
		info << "End-riverstation  : " << this->station_end<<label::m<<endl;
		info << "River model index : " << river->Param_RV.get_river_number()<<endl;
		info << "River model name  : " << river->Param_RV.get_river_name()<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(start_inbetween==false){
		Warning msg=this->set_warning(5);
		ostringstream info;
		info << "Section index     : " <<  this->id_section << endl;
		info << "Section name      : " << this->section_name<< endl;
		info << "Start-riverstation: " << this->station_start<<label::m<<endl;
		info << "River model index : " << river->Param_RV.get_river_number()<<endl;
		info << "River model name  : " << river->Param_RV.get_river_name()<<endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	if(end_inbetween==false){
		Warning msg=this->set_warning(6);
		ostringstream info;
		info << "Section index     : " <<  this->id_section << endl;
		info << "Section name      : " << this->section_name<< endl;
		info << "End-riverstation  : " << this->station_end<<label::m<<endl;
		info << "River model index : " << river->Param_RV.get_river_number()<<endl;
		info << "River model name  : " << river->Param_RV.get_river_name()<<endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	//update the table
	Geo_Polysegment poly_buff;
	string polyline_buffer;
	river->get_polyline_fpl_section(this->left_flag, &poly_buff,start_down, end_up);
	//make the string
	Sys_Database_Browser polyline_producer;
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	polyline_producer.set_tablename(Fpl_Section::table->get_table_name());
	//set a where filter
	ostringstream  filter_buff;
	filter_buff<< Fpl_Section::table->get_column_name(fpl_label::section_id) << " = " << this->id_section;
	filter_buff<< " AND " <<Fpl_Section::table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	filter_buff<< " AND " <<Fpl_Section::table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;
	polyline_producer.set_where_filter( filter_buff.str());
	polyline_buffer=polyline_producer.get_inserting_polyline_filter(Fpl_Section::table->get_column_name(fpl_label::section_line), &poly_buff);

	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, polyline_buffer, ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(7);
		ostringstream info;
		info << "Table Name                : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Section index             : " << this->id_section << endl;
		info << "Section name              : " << this->section_name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	//set the section length
	filter_buff.str("");
	filter_buff << "UPDATE " << Fpl_Section::table->get_table_name();
	filter_buff << " SET "  << Fpl_Section::table->get_column_name(fpl_label::section_length) << " = ";
	filter_buff << poly_buff.get_total_distance();
	filter_buff << " WHERE ";
	filter_buff<< Fpl_Section::table->get_column_name(fpl_label::section_id) << " = " << this->id_section;
	filter_buff<< " AND " <<Fpl_Section::table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	filter_buff<< " AND " <<Fpl_Section::table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;

	//launch the request
	Data_Base::database_request(&query_buff, filter_buff.str(), ptr_database);
	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(8);
		ostringstream info;
		info << "Table Name                : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Section index             : " << this->id_section << endl;
		info << "Section name              : " << this->section_name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	//set the structure height
	filter_buff.str("");
	filter_buff << "UPDATE " << Fpl_Section::table->get_table_name();
	filter_buff << " SET "  << Fpl_Section::table->get_column_name(fpl_label::structure_height) << " = ";
	filter_buff << this->structure_height;
	filter_buff << " WHERE ";
	filter_buff<< Fpl_Section::table->get_column_name(fpl_label::section_id) << " = " << this->id_section;
	filter_buff<< " AND " <<Fpl_Section::table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	filter_buff<< " AND " <<Fpl_Section::table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;

	//launch the request
	Data_Base::database_request(&query_buff, filter_buff.str(), ptr_database);
	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(10);
		ostringstream info;
		info << "Table Name                : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Section index             : " << this->id_section << endl;
		info << "Section name              : " << this->section_name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
}
//Set the polyline-string and the section length due to combining with the coast dikeline
void Fpl_Section::set_length_polyline(Hyd_Floodplain_Polysegment *coast_line, QSqlDatabase *ptr_database){
	Hyd_Floodplain_Dikeline_Point *start_up=NULL;
	Hyd_Floodplain_Dikeline_Point *start_down=NULL;
	bool start_inbetween=false;

	start_inbetween=coast_line->find_ptr_up_down_point(this->station_start, &start_up, &start_down);

	Hyd_Floodplain_Dikeline_Point *end_up=NULL;
	Hyd_Floodplain_Dikeline_Point *end_down=NULL;
	bool end_inbetween=false;

	end_inbetween=coast_line->find_ptr_up_down_point(this->station_end, &end_up, &end_down);

	if(start_inbetween ==false && end_inbetween==false){
		Error msg=this->set_error(25);
		ostringstream info;
		info << "Section index     : " <<  this->id_section << endl;
		info << "Section name      : " << this->section_name<< endl;
		info << "Start-station     : " << this->station_start<<label::m<<endl;
		info << "End-station       : " << this->station_end<<label::m<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(start_inbetween==false){
		Warning msg=this->set_warning(14);
		ostringstream info;
		info << "Section index     : " <<  this->id_section << endl;
		info << "Section name      : " << this->section_name<< endl;
		info << "Start-station     : " << this->station_start<<label::m<<endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	if(end_inbetween==false){
		Warning msg=this->set_warning(15);
		ostringstream info;
		info << "Section index     : " <<  this->id_section << endl;
		info << "Section name      : " << this->section_name<< endl;
		info << "End-station       : " << this->station_end<<label::m<<endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	//update the table
	Geo_Polysegment poly_buff;
	string polyline_buffer;
	coast_line->get_polyline_fpl_section(&poly_buff, start_down, end_up);
	//make the string
	Sys_Database_Browser polyline_producer;
	try{
		Fpl_Section::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	polyline_producer.set_tablename(Fpl_Section::table->get_table_name());
	//set a where filter
	ostringstream  filter_buff;
	filter_buff<< Fpl_Section::table->get_column_name(fpl_label::section_id) << " = " << this->id_section;
	filter_buff<< " AND " <<Fpl_Section::table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	filter_buff<< " AND " <<Fpl_Section::table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;
	polyline_producer.set_where_filter( filter_buff.str());
	polyline_buffer=polyline_producer.get_inserting_polyline_filter(Fpl_Section::table->get_column_name(fpl_label::section_line), &poly_buff);

	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, polyline_buffer, ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(7);
		ostringstream info;
		info << "Table Name                : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Section index             : " << this->id_section << endl;
		info << "Section name              : " << this->section_name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	//set the section length
	filter_buff.str("");
	filter_buff << "UPDATE " << Fpl_Section::table->get_table_name();
	filter_buff << " SET "  << Fpl_Section::table->get_column_name(fpl_label::section_length) << " = ";
	filter_buff << poly_buff.get_total_distance();
	filter_buff << " WHERE ";
	filter_buff<< Fpl_Section::table->get_column_name(fpl_label::section_id) << " = " << this->id_section;
	filter_buff<< " AND " <<Fpl_Section::table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	filter_buff<< " AND " <<Fpl_Section::table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;

	//launch the request
	Data_Base::database_request(&query_buff, filter_buff.str(), ptr_database);
	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(8);
		ostringstream info;
		info << "Table Name                : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Section index             : " << this->id_section << endl;
		info << "Section name              : " << this->section_name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	//set the structure height
	filter_buff.str("");
	filter_buff << "UPDATE " << Fpl_Section::table->get_table_name();
	filter_buff << " SET "  << Fpl_Section::table->get_column_name(fpl_label::structure_height) << " = ";
	filter_buff << this->structure_height;
	filter_buff << " WHERE ";
	filter_buff<< Fpl_Section::table->get_column_name(fpl_label::section_id) << " = " << this->id_section;
	filter_buff<< " AND " <<Fpl_Section::table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	filter_buff<< " AND " <<Fpl_Section::table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;

	//launch the request
	Data_Base::database_request(&query_buff, filter_buff.str(), ptr_database);
	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(10);
		ostringstream info;
		info << "Table Name                : " << Fpl_Section::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Section index             : " << this->id_section << endl;
		info << "Section name              : " << this->section_name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
}
//Check if a fpl-section is concerning the hydraulic parameters (structure height, start-, end-point etc.) similar to the given section
bool Fpl_Section::check_fpl_section_similar_hyd_param(Fpl_Section *section2check){
	bool similar=true;

	if(this->structure_height!=section2check->structure_height){
		return similar=false;
	}
	if(this->break_params.abrupt_fails_flag!= section2check->break_params.abrupt_fails_flag){
		return similar=false;
	}
	if(this->break_params.abrupt_opening!= section2check->break_params.abrupt_opening){
		return similar=false;
	}
	if(this->break_params.resistance!= section2check->break_params.resistance){
		return similar=false;
	}
	if(abs(this->station_start-section2check->station_start)>constant::meter_epsilon){
		return similar=false;
	}
	if(abs(this->station_end-section2check->station_end)>constant::meter_epsilon){
		return similar=false;
	}
	if(this->left_flag!=section2check->left_flag){
		return similar=false;
	}

	return similar;
}
//Copy the main section parameters
void Fpl_Section::copy_main_section_parameter(Fpl_Section *copy){
	this->structure_height=copy->structure_height;
	this->break_params.abrupt_fails_flag= copy->break_params.abrupt_fails_flag;
	this->break_params.abrupt_opening= copy->break_params.abrupt_opening;
	this->break_params.resistance= copy->break_params.resistance;
	this->station_start=copy->station_start;
	this->station_end=copy->station_end;
	this->left_flag=copy->left_flag;
	this->crest_water.set_point_y_coordinate(copy->crest_water.get_ycoordinate());
	this->id_section=copy->id_section;
}
//Set error(s)
Error Fpl_Section::set_error(const int err_type){
		string place="Fpl_Section::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("allocate_section_type(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			info << "Section-id   : " <<  this->id_section << endl;
			info << "Section name : " << this->section_name<< endl;
			type=10;
			break;
		case 1://can not open section function file
			place.append("read_section_per_file(const string file_section)");
			reason="Can not open the file of the FPL-section";
			help="Check the file";
			type=5;
			break;
		case 2://do not find the begin of the general section information in file
			place.append("read_general_section_data_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find the begin of the section information in file";
			help="The section information has to start with !$BEGIN_SECTION";
			type=1;
			break;
		case 3://do not find the end of the general section information in file
			place.append("read_general_section_data_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find the end of the section information in file";
			help="The section information has to end with !$END_SECTION";
			type=1;
			break;
		case 4://do not find all required section infos
			place.append("read_general_section_data_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find all required section information";
			help="Check if all section information are set in the file";
			info << "Required information:"<<endl;
			info << " " << fpl_label::key_sec_name<< endl;
			info << " " << fpl_label::key_sec_type<< endl;
			info << " " << fpl_label::key_sec_direction<< endl;
			info << " " << fpl_label::key_sec_crestwater_x<< endl;
			info << " " << fpl_label::key_sec_crestwater_y<< endl;
			info << "Optional information:"<<endl;
			info << " " << fpl_label::key_max_breach_width<< endl;
			type=1;
			break;
		case 5://wrong input
			place.append("find_key_values_general_file(const string myline, int *found_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the general section information in file";
			type=1;
			break;
		case 6://do not find the begin of the hydraulic connection infos in file
			place.append("read_general_section_data_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find the begin of the hydraulic connection information in file";
			help="The hydraulic connection information has to start with !$BEGIN_HYD_CONNECTION";
			type=1;
			break;
		case 7://do not find the end of the hydraulic connection infos in file
			place.append("read_hyd_connection_data_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find the end of the hydraulic connection information in file";
			help="The hydraulic connection information has to end with !$END_HYD_CONNECTION";
			type=1;
			break;
		case 8://do not find all required hydraulic connection infos in file
			place.append("read_hyd_connection_data_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find all required section information";
			help="Check if all section information are set in the file";
			info << "Required information in case of HYD river connection:"<<endl;
			info << " " << fpl_label::key_rv_conn_id<< endl;
			info << " " << fpl_label::key_leftdirection<< endl;
			info << "Required information:"<<endl;
			info << " " << fpl_label::key_station_start<< endl;
			info << " " << fpl_label::key_station_end<< endl;
			info << " " << fpl_label::key_max_breach_width<< endl;
			info << "either"<<endl;
			info << " " << fpl_label::key_critical_velocity<< endl;
			info << "or"<<endl;
			info << " " << fpl_label::key_wall_resistance<< endl;
			info << " " << fpl_label::key_abrupt_breach_open<< endl;
			type=1;
			break;
		case 9://wrong input
			place.append("find_key_hyd_connection_general_file(const string myline, int *found_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the hydraulic connection information in file";
			type=1;
			break;
		case 10://can not open section file
			place.append("check_input_file_section(const string file_name)");
			reason="Can not open the file of the fpl section for checking";
			help="Check the file";
			type=5;
			break;
		case 11://!$END is not correctly opened
			place.append("check_input_file_section(const string file_name)");
			reason="The !$END-key is not correctly opened with the corresponding !$BEGIN-key";
			help="Check the file and start the blocks correctly";
			type=5;
			break;
		case 12://!$BEGIN is not correctly closed
			place.append("check_input_file_section(const string file_name)");
			reason="The !$BEGIN-key is not correctly closed with the corresponding !$END-key";
			help="Check the file and close the blocks correctly";
			type=5;
			break;
		case 13://riverstations are not correctly set
			place.append("check_connection_params(void)");
			reason="The start-riverstation have to be greater than the end-riverstation, because there are decreasing riverstation in flow direction";
			help="Check the riverstations parameter start/end";
			type=27;
			break;
		case 14://riverstations are not correctly set
			place.append("set_length_polyline(Hyd_Model_River *river, QSqlDatabase *ptr_database)");
			reason="The given river station of the FPL-section is not located in the given river model";
			help="Check the riverstations parameter start/end and the given river model";
			type=27;
			break;
		case 15://bad_alloc
			place.append("allocate_list_involved_profiles(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 16://bad_alloc
			place.append("allocate_frc_data(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 17://no max results found in database table
			place.append("evaluate_max_waterlevel_involved_section(QSqlDatabase *ptr_database, const int id_hyd_sc, const string break_id)");
			reason ="No maximum waterlevel is found in the database table of the river profiles for the calculation of the probability per frc-curve";
			help="Check the reults in the result table of the river profile";
			type=2;
			break;
		case 18://no hydraulic model found
			place.append("load_list_involved_profiles_points(Hyd_Hydraulic_System *hyd_system)");
			reason="Did not find the hydraulic model of the FPL-section";
			help="Check the river connection id/related station id of the FPL-section";
			type=27;
			break;
		case 19://no the same section type
			place.append("overwrite_main_section_parameter(Fpl_Section *given_sec)");
			reason="The type of the FPL-section are not the same; a changing of the probabilistic parameters is not possible";
			help="Check the FPL-section given in file and the FPL-section to replace the parameters";
			type=27;
			break;
		case 20://same geometrical fpl-section
			place.append("compare_geometrical_settings(Fpl_Section *section)");
			reason="The station of the starting and the ending point of the two FPL-section are identical; the FPL-section is not tranfered to database";
			help="Check the FPL-section given in file";
			type=27;
			break;
		case 21://same geometrical fpl-section
			place.append("compare_geometrical_settings(Fpl_Section *section)");
			reason="The existing FPL-section is completly in the new given FPL-section; the FPL-section is not tranfered to database";
			help="Check the FPL-section given in file";
			type=27;
			break;
		case 22://same geometrical fpl-section
			place.append("compare_geometrical_settings(Fpl_Section *section)");
			reason="The new given FPL-section is completly in the existing FPL-section; the FPL-section is not tranfered to database";
			help="Check the FPL-section given in file";
			type=27;
			break;
		case 23://could not open the tecplot file
			place.append("output_geometrie2tecplot(void)");
			reason="Could not open the file for the tecplot output (geometry) of the FPL-section";
			help="Check the file";
			type=5;
			break;
		case 24://bad_alloc
			place.append("allocate_list_involved_points(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 25://stations are not correctly set
			place.append("set_length_polyline(Hyd_Floodplain_Polysegment *coast_line, QSqlDatabase *ptr_database)");
			reason="The given station of the FPL-section is not located in the given dikeline of the coast model";
			help="Check the stations parameter start/end and the given coast model";
			type=27;
			break;
		case 26://stations are not correctly set
			place.append("check_connection_params(void)");
			reason="The start-station have to be smallrt than the end-station, because there are increasing station in dikeline direction";
			help="Check the stations parameter start/end";
			type=27;
			break;
		case 27://could not open the paraview file
			place.append("output_geometrie2paraview(void)");
			reason = "Could not open the file for the ParaView output (geometry) of the FPL-section";
			help = "Check the file";
			type = 5;
			break;
		case 28://could not open the excel file
			place.append("output_geometrie2excel(void)");
			reason = "Could not open the file for the Excel output (geometry) of the FPL-section";
			help = "Check the file";
			type = 5;
			break;
		case 29://could not open the tecplot file
			place.append("output_XX_result2YY(void)");
			reason = "Could not open the file for the result output of the FPL-section";
			help = "Check the file";
			type = 5;
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
Warning Fpl_Section::set_warning(const int warn_type){
		string place="Fpl_Section::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://section_type is not know
			place.append("allocate_section_type(void)") ;
			reason="Section type is not known";
			reaction="No performance analysis can be done";
			help= "Check the section";
			info << "Table Name   : " << Fpl_Section::table->table_name.name << endl;
			info << "Section-id   : " <<  this->id_section << endl;
			info << "Section name : " << this->section_name<< endl;
			type=8;
			break;
		case 1://input datas can not submitted
			place.append("transfer_section2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the FPL-section data to the database";
			help="Check the database";
			type=2;
			break;
		case 2://no model found
			place.append("combine_section2hydraulic(Hyd_Hydraulic_System *hyd_system)");
			reason="Did not find the HYD-model for the connection of the FPL-section to the Hyd-system";
			help="Check the river connection id/related station id of the FPL-section";
			reaction="The section can not be connected to the HYD-system";
			type=8;
			break;
		case 3://max breach width is <0.0
			place.append("check_connection_params(void)");
			reason="The maximum breach width is <0.0";
			help="Check the given maximum breach width of the FPL-section";
			reaction="The maximum breach width is set to 0.0";
			type=8;
			break;
		case 4://no river found
			place.append("check_connection_params(void)");
			reason="There is no river set for the combining to the HYD-system";
			help="Check the river connection id of the FPL-section";
			reaction="The section can not be connected to the HYD-system";
			type=8;
			break;
		case 5://start riverstation is not in the river model
			place.append("set_length_polyline(Hyd_Model_River *river, QSqlDatabase *ptr_database)");
			reason="The given start-riverstation of the section is not in the given HYD-river model; it is beyond the inflow profile";
			help="Check the given start-riverstation and the given HYD-river model";
			reaction="The section is adapted to the existing HYD-river model";
			type=8;
			break;
		case 6://end riverstation is not in the river model
			place.append("set_length_polyline(Hyd_Model_River *river, QSqlDatabase *ptr_database)");
			reason="The given end-riverstation of the section is not in the given HYD-river model; it is beyond the outflow profile";
			help="Check the given end-riverstation and the given HYD-river model";
			reaction="The section is adapted to the existing HYD-river model";
			type=8;
			break;
		case 7://input datas can not submitted
			place.append("set_length_polyline(Hyd_Model_River *river, QSqlDatabase *ptr_database)/set_length_polyline(Hyd_Floodplain_Polysegment *coast_line, QSqlDatabase *ptr_database)");
			reason="Can not submit the FPL-section polyline to the database";
			help="Check the database";
			type=2;
			break;
		case 8://input datas can not submitted
			place.append("set_length_polyline(Hyd_Model_River *river, QSqlDatabase *ptr_database)/set_length_polyline(Hyd_Floodplain_Polysegment *coast_line, QSqlDatabase *ptr_database)");
			reason="Can not submit the FPL-section polyline length to the database";
			help="Check the database";
			type=2;
			break;
		case 9://no river found
			place.append("idealise_hyd_by_section(Hyd_Hydraulic_System *hyd_system, QSqlDatabase *ptr_database)");
			reason="Did not find the hydraulic model of the FPL-section";
			help="Check the river connection id/related station id of the FPL-section";
			reaction="The HYD-system can not be idealised by this FPL-section";
			type=8;
			break;
		case 10://input datas can not submitted
			place.append("set_length_polyline(Hyd_Model_River *river, QSqlDatabase *ptr_database)/set_length_polyline(Hyd_Floodplain_Polysegment *coast_line, QSqlDatabase *ptr_database)");
			reason="Can not submit the FPL-section structure height to the database";
			help="Check the database";
			type=2;
			break;
		case 11://similarity factor is < 0.0
			place.append("check_conn_params(void)");
			reason="The similarity factor of the FPL-section is < 0.0";
			help="Check the connection parameters of the FPL-section";
			reaction="No length effect are taken into account";
			type=8;
			break;
		case 12://similarity factor is > 1.0
			place.append("check_conn_params(void)");
			reason="The similarity factor of the FPL-section is > 1.0";
			help="Check the connection parameters of the FPL-section";
			reaction="No length effect are taken into account";
			type=8;
			break;
		case 13://no the same main parameters
			place.append("overwrite_main_section_parameter(Fpl_Section *given_sec)");
			reason="The main parameters of the FPL-section are not the same";
			help="Check FPL-section given in file and the FPL-section to replace the probabilistic parameters";
			reaction="The main parameters of the FPL-section in database are relevant";
			type=8;
			break;
		case 14://start station is not in the coast model
			place.append("set_length_polyline(Hyd_Floodplain_Polysegment *coast_line, QSqlDatabase *ptr_database)");
			reason="The given start-station of the section is not in the given HYD-coast model; it is beyond the start point of the coastline";
			help="Check the given start-station and the given HYD-coast model";
			reaction="The section is adapted to the existing HYD-coast model";
			type=8;
			break;
		case 15://end riverstation is not in the river model
			place.append("set_length_polyline((Hyd_Floodplain_Polysegment *coast_line, QSqlDatabase *ptr_database)");
			reason="The given end-station of the section is not in the given HYD-coast model; it is beyond the end point of the coastline";
			help="Check the given end-station and the given HYD-coast model";
			reaction="The section is adapted to the existing HYD-coast model";
			type=8;
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