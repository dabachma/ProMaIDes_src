//#include "_Fpl_Section_Type.h"
#include "Fpl_Headers_Precompiled.h"

//init static members
Tables *_Fpl_Section_Type::table_mechanism=NULL;

//Default constructor
_Fpl_Section_Type::_Fpl_Section_Type(void){

	this->section_direction=0.0;
	//set the point name
	this->base_water.set_point_name(fpl_label::base_water);
	this->base_water.set_point_coordinate(0.0,0.0);
	this->number_mech=0;

	this->low_waterlevel_flag=false;
	this->output_flags.excel_output = true;
	this->output_flags.para_output = true;
	this->output_flags.tec_output = false;

}
//Default destructor
_Fpl_Section_Type::~_Fpl_Section_Type(void){
}
//__________
//public
//Read the mechanisms of the fault-tree per file
void _Fpl_Section_Type::read_mechanism_faulttree_perfile(QFile *, int *){
/**This method is not required for all section types, e.g. asection type highground.
For the other section types it is reimplemented.
*/
}
//Check which mechanisms of the fault-tree are required per file
void _Fpl_Section_Type::check_mechanism_faulttree_perfile(QFile *, int *){
/**This method is not required for all section types, e.g. asection type highground.
For the other section types it is reimplemented.
*/
}
//Read the waterside geometry of the section
void _Fpl_Section_Type::read_waterside_geometry(QFile *, int *){
/**This method is not required for all section types, e.g. asection type highground.
For the other section types it is reimplemented.
*/
}
//Read the landside geometry of the section
void _Fpl_Section_Type::read_landside_geometry(QFile *, int *){
/**This method is not required for all section types, e.g. asection type highground.
For the other section types it is reimplemented.
*/
}
//Read the geometrical material zone(s) of the section per file
void _Fpl_Section_Type::read_geo_matzones(QFile *, int *){
/**This method is not required for all section types, e.g. asection type highground.
For the other section types it is reimplemented.
*/
}
//Read the material variable zone(s) of the section per file
void _Fpl_Section_Type::read_var_matzones(QFile *, int *){
/**This method is not required for all section types, e.g. asection type highground.
For the other section types it is reimplemented.
*/
}
//Read the waterside material variable zone(s) of the section per file
void _Fpl_Section_Type::read_var_waterside_matzones(QFile *, int *){

}
//Check and set the section geometry out of the read in geometry members
void _Fpl_Section_Type::check_set_geometry(void){
/**This method is not required for all section types, e.g. asection type highground.
For the other section types it is reimplemented.
*/
}
//Set the database table for the applied mechanism: it sets the table name and the name of the columns and allocate them (static)
void _Fpl_Section_Type::set_table_mechanism(QSqlDatabase *ptr_database){
	if(_Fpl_Section_Type::table_mechanism==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_fpl_mechanism;
		string tab_col[6];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::mecha_name;
		tab_col[2]=fpl_label::mecha_sec_type;
		tab_col[3]=fpl_label::mecha_applied;
		tab_col[4]=fpl_label::mecha_applied_tree;
		tab_col[5]=label::description;

		
		//set the table
		try{
			_Fpl_Section_Type::table_mechanism= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			_Fpl_Section_Type::table_mechanism->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("_Fpl_Section_Type::set_table_mechanism(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Fpl_Section_Type::close_table();
			throw msg;
		}
	}
}
//Create the database table for the applied mechanisms (static)
bool _Fpl_Section_Type::create_table_mechanism(QSqlDatabase *ptr_database){
		if(_Fpl_Section_Type::table_mechanism==NULL){
			ostringstream cout;
			cout << "Create database table of mechanism..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_fpl_mechanism;
			const int num_col=6;
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

			tab_col[1].name=fpl_label::mecha_name;
			tab_col[1].type=sys_label::tab_col_type_string;

			tab_col[2].name=fpl_label::mecha_sec_type;
			tab_col[2].type=sys_label::tab_col_type_string;

			tab_col[3].name=fpl_label::mecha_applied;
			tab_col[3].type=sys_label::tab_col_type_bool;
			tab_col[3].default_value="true";

			tab_col[4].name=fpl_label::mecha_applied_tree;
			tab_col[4].type=sys_label::tab_col_type_bool;
			tab_col[4].default_value="true";

			tab_col[5].name=label::description;
			tab_col[5].type=sys_label::tab_col_type_string;



			try{
				_Fpl_Section_Type::table_mechanism= new Tables();
				if(_Fpl_Section_Type::table_mechanism->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
					_Fpl_Section_Type::close_table();
					return false;
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("_Fpl_Section_Type::create_table_mechanism(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				_Fpl_Section_Type::close_table();
				throw msg;
			}
			_Fpl_Section_Type::close_table();
			
			return true;
	}
	return false;

}
//Reset all data in the database table for the applied mechanism to the default values (static)
void _Fpl_Section_Type::reset_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		_Fpl_Section_Type::set_table_mechanism(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	
	_Fpl_Section_Type::set_predefined_data2mechanism_table(ptr_database);
}
//Close and delete the database table for the applied mechanism (static)
void _Fpl_Section_Type::close_table(void){
	if(_Fpl_Section_Type::table_mechanism!=NULL){
		delete _Fpl_Section_Type::table_mechanism;
		_Fpl_Section_Type::table_mechanism=NULL;
	}
}
//Set the predefined data to the mechanism database table; it is called after the creation of this table (static)
void _Fpl_Section_Type::set_predefined_data2mechanism_table(QSqlDatabase *ptr_database){
	try{
		_Fpl_Section_Type::set_table_mechanism(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	_Fpl_Section_Type::table_mechanism->delete_data_in_table(ptr_database);

	//mysql query with the query-class
	QSqlQuery model(*ptr_database);

	//evaluate the global identifier
	int id_glob=_Fpl_Section_Type::table_mechanism->maximum_int_of_column(_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::glob_id),ptr_database)+1;
	ostringstream total;

	//set the query via a query string
	ostringstream query_string_fix;
	query_string_fix << "INSERT INTO  " << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string_fix <<" ( ";
	query_string_fix << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::glob_id) <<" , ";
	query_string_fix << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) <<" , ";
	query_string_fix << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) <<" , ";
	query_string_fix << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) <<" , ";
	query_string_fix << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied_tree) <<" , ";
	query_string_fix << _Fpl_Section_Type::table_mechanism->get_column_name(label::description) <<" ) ";


	//highground
	//mechanism overflow highground
	ostringstream query_string;
	//query_string << " VALUES ( ";
	//query_string << id_glob << " , " ;
	//query_string <<"'"<< fpl_label::mech_overflow <<"'"<< " , " ;
	//query_string <<"'"<< fpl_label::sec_highground <<"'"<< " , " ;
	//query_string << "true" << " , " ;
	//query_string << "true" << " , " ;
	//query_string << "'Overflow of an highground where h_water>h_crest'" <<" ) ";
	//
	//total <<query_string_fix.str() <<  query_string.str();

	//Data_Base::database_request(&model, total.str(), ptr_database);
	//total.str("");
	//query_string.str("");
	//id_glob++;

	//dike
	//mechanism overflow dike
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_overflow <<"'"<< " , " ;
	query_string << "'"<<fpl_label::sec_dike <<"'"<< " , " ;
	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Overflow of a dike after modified Polenis formulas'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();


	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//mechanism wind wave for a dike
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_wind_wave<<"'"<< " , " ;
	query_string << "'"<<fpl_label::sec_dike << "'"<<" , " ;
	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Wave generation induced by wind for a dike section after KrylowII (DVWK246/1997); it is no failure mechanism'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//mechanism wave runup of a dike
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_wave_runup << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dike <<"'"<< " , " ;
	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Wave runup at the waterside of a dike after Eurotop-manual (2007)'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//mechanism bursting after Din19712 at the landside dike toe
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_bursting_din << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dike <<"'"<< " , " ;
	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Bursting at the landside dike toe after DIN19712'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//mechanism piping after Sellmeijer in the dike underground
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_piping_sell << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dike <<"'"<< " , " ;

	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Piping in the dike underground after Sellmeijer (theoretical method)'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//mechanism piping after Lane in the dike underground
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_piping_lane << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dike <<"'"<< " , " ;

	query_string << "true" << " , " ;
	query_string << "false" << " , " ;
	query_string << "'Piping in the dike underground after Lane (empirical method)'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//mechanism piping after Lane in the dike underground
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_piping_schmert << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dike <<"'"<< " , " ;

	query_string << "true" << " , " ;
	query_string << "false" << " , " ;
	query_string << "'Piping in the dike underground after Schmertmann (theoretical method)'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//mechanism slope landside dike
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_slope_landside << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dike <<"'"<< " , " ;

	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Slope at the landside of a dike after Krey-method'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//mechanism slope waterside dike
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_slope_waterside << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dike <<"'"<< " , " ;

	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Slope at the waterside of a dike after Krey-method'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//mechanism landside erosion dike
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_erosion_landside << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dike <<"'"<< " , " ;

	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Erosion of the landside dike slope'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//mechanism waterside erosion dike
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_erosion_waterside << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dike <<"'"<< " , " ;

	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Erosion of the waterside dike slope'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//mechanism start waterside erosion dike
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_erosion_waterside_start << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dike <<"'"<< " , " ;

	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Start the erosion of the waterside dike slope'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//mechanism wave impact dike
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_wave_impact << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dike <<"'"<< " , " ;

	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Wave impact on the waterside dike slope'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;


	//dune
	//mechanism overflow dune
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_overflow <<"'"<< " , " ;
	query_string << "'"<<fpl_label::sec_dune <<"'"<< " , " ;
	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Overflow of a dike after modified Polenis formulas'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();


	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//mechanism wind wave for a dune
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_wind_wave<<"'"<< " , " ;
	query_string << "'"<<fpl_label::sec_dune << "'"<<" , " ;
	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Wave generation induced by wind for a dike section after KrylowII (DVWK246/1997); it is no failure mechanism'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;
	//mechanism wave runup of a dune
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_wave_runup << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dune <<"'"<< " , " ;
	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Wave runup at the waterside of a dike after Eurotop-manual (2007)'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//mechanism landside erosion dune
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_erosion_landside << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dune <<"'"<< " , " ;

	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Erosion of the landside dike slope'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//mechanism erosion waterside of a dune
	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << "'"<<fpl_label::mech_erosion_waterside_gent << "'"<<" , " ;
	query_string << "'"<<fpl_label::sec_dune <<"'"<< " , " ;

	query_string << "true" << " , " ;
	query_string << "true" << " , " ;
	query_string << "'Erosion waterside of the dune due to waves'" <<" ) ";
	
	total <<query_string_fix.str() <<  query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;


	//gate

	//wall

	//mobile wall

	
}
//Select the relevant mechanisms for a specific section type and return the number of found mechanism(s) (static)
int _Fpl_Section_Type::select_relevant_mechanism(QSqlQueryModel *results, QSqlDatabase *ptr_database, _fpl_section_types sec_type){

	int number=0;
	try{
		_Fpl_Section_Type::set_table_mechanism(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "SELECT * ";
	test_filter << " FROM " << _Fpl_Section_Type::table_mechanism->get_table_name();
	test_filter << " WHERE " ;
	test_filter << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(sec_type)<<"'";
	test_filter << " AND " ;
	test_filter << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = true";
	test_filter << " AND " ;
	test_filter << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied_tree) << " = true";

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Warning msg;
		msg.set_msg("_Fpl_Section_Type::select_relevant_mechanism(QSqlQueryModel *results, QSqlDatabase *ptr_database, _fpl_section_types sec_type)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	
	number= results->rowCount();


	return number;


}
//Select the mechanisms for a specific section type and return the number of found mechanism(s) (static)
int _Fpl_Section_Type::select_mechanism(QSqlQueryModel *results, QSqlDatabase *ptr_database, _fpl_section_types sec_type){
	int number=0;
	try{
		_Fpl_Section_Type::set_table_mechanism(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "SELECT * ";
	test_filter << " FROM " << _Fpl_Section_Type::table_mechanism->get_table_name();
	test_filter << " WHERE " ;
	test_filter << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(sec_type)<<"'";


	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Warning msg;
		msg.set_msg("_Fpl_Section_Type::select_mechanism(QSqlQueryModel *results, QSqlDatabase *ptr_database, _fpl_section_types sec_type)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	
	number= results->rowCount();


	return number;


}
//Set the waterlevel in case of an frc-sim
void _Fpl_Section_Type::set_frc_waterlevel(const double frc_waterlevel){
	waterlevel.set_new_middle(frc_waterlevel);
}
//Set the direction of the section
void _Fpl_Section_Type::set_section_direction(const double direction){
	this->section_direction=direction;
}
//Set the pointer to the water side crest point
void _Fpl_Section_Type::set_ptr_waterside_crest(Fpl_Section_Points *crest_water){
	this->crest_water=crest_water;
}


//Get output control flags
output_control _Fpl_Section_Type::get_output_control_flags(void) {
	return this->output_flags;
}
//Set output control flags per table
void _Fpl_Section_Type::set_output_control_flags(QSqlDatabase *ptr_database) {

	Fpl_Mc_Sim::get_output_control_from_table(ptr_database, &this->output_flags);


}
//Convert a string into a section type (_fpl_section_types) (static)
_fpl_section_types _Fpl_Section_Type::convert_txt2section_type(const string txt){
	_fpl_section_types type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lower_case(&buffer);

	if(buffer==fpl_label::sec_highground){
		type=_fpl_section_types::high_ground_sec;
	}
	else if(buffer==fpl_label::sec_dune){
		type=_fpl_section_types::dune_sec;
	}
	else if(buffer==fpl_label::sec_gate){
		type=_fpl_section_types::gate_sec;
	}
	else if(buffer==fpl_label::sec_dike){
		type=_fpl_section_types::dike_sec;
	}
	else if(buffer==fpl_label::sec_wall){
		type=_fpl_section_types::wall_sec;
	}
	else if(buffer==fpl_label::sec_mobile_wall){
		type=_fpl_section_types::mob_wall_sec;
	}
	else if(buffer==fpl_label::sec_set_by_hand){
		type=_fpl_section_types::by_hand_sec;
	}
	else if(buffer==fpl_label::sec_all){
		type=_fpl_section_types::all_sec;
	}
	else{
		Error msg;
		msg.set_msg("_Fpl_Section_Type::convert_txt2section_type(const string txt)", "Can not convert this section type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert section type: " << txt << endl;
		info << "Possible types: "<< endl;
		info << " "<<fpl_label::sec_highground  << endl;
		info << " "<<fpl_label::sec_dike  << endl;
		info << " "<<fpl_label::sec_wall  << endl;
		info << " "<<fpl_label::sec_mobile_wall  << endl;
		info << " "<<fpl_label::sec_set_by_hand  << endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	return type;
}
//Convert a section type (_fpl_section_types) into a string (static)
string _Fpl_Section_Type::convert_section_type2txt(const _fpl_section_types type){
	string buffer;
	switch(type){
		case _fpl_section_types::high_ground_sec:
			buffer=fpl_label::sec_highground;
			break;
		case _fpl_section_types::dike_sec:
			buffer=fpl_label::sec_dike;
			break;
		case _fpl_section_types::dune_sec:
			buffer=fpl_label::sec_dune;
			break;
		case _fpl_section_types::gate_sec:
			buffer=fpl_label::sec_gate;
			break;
		case _fpl_section_types::wall_sec:
			buffer=fpl_label::sec_wall;
			break;
		case _fpl_section_types::mob_wall_sec:
			buffer=fpl_label::sec_mobile_wall;
			break;
		case _fpl_section_types::by_hand_sec:
			buffer=fpl_label::sec_set_by_hand;
			break;
		case _fpl_section_types::all_sec:
			buffer=fpl_label::sec_all;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//Convert a string into a mechanism type (_fpl_mechanism_types) (static)
_fpl_mechanism_types _Fpl_Section_Type::convert_txt2mechanism_type(const string txt){
	_fpl_mechanism_types type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lower_case(&buffer);

	if(buffer==fpl_label::mech_overflow){
		type=_fpl_mechanism_types::overflow;
	}
	else if(buffer==fpl_label::mech_wave_runup){
		type=_fpl_mechanism_types::wave_runup;
	}
	else if(buffer==fpl_label::mech_wind_wave){
		type=_fpl_mechanism_types::wind_wave;
	}
	else if(buffer==fpl_label::mech_bursting_din){
		type=_fpl_mechanism_types::bursting;
	}
	else if(buffer==fpl_label::mech_piping_sell){
		type=_fpl_mechanism_types::piping_selli;
	}
	else if(buffer==fpl_label::mech_piping_lane){
		type=_fpl_mechanism_types::piping_lane;
	}
	else if(buffer==fpl_label::mech_piping_schmert){
		type=_fpl_mechanism_types::piping_schmert;
	}
	else if(buffer==fpl_label::mech_slope_landside){
		type=_fpl_mechanism_types::landside_stability;
	}
	else if(buffer==fpl_label::mech_slope_waterside){
		type=_fpl_mechanism_types::waterside_stability;
	}
	else if(buffer==fpl_label::mech_erosion_landside){
		type=_fpl_mechanism_types::landside_erosion;
	}
	else if(buffer==fpl_label::mech_erosion_waterside){
		type=_fpl_mechanism_types::waterside_erosion;
	}
	else if(buffer==fpl_label::mech_erosion_waterside_start){
		type=_fpl_mechanism_types::start_waterside_erosion;
	}
	else if(buffer==fpl_label::mech_wave_impact){
		type=_fpl_mechanism_types::wave_impact;
	}
	else if(buffer==fpl_label::mech_erosion_waterside_gent){
		type=_fpl_mechanism_types::waterside_erosion_gent;
	}
	else{
		Error msg;
		msg.set_msg("_Fpl_Section_Type::convert_txt2mechanism_type(const string txt)", "Can not convert this section type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert mechanism type: " << txt << endl;
		info << "Possible mechanisms: "<< endl;
		info << " "<<fpl_label::mech_overflow  << "(all)"<<endl;
		info << " "<<fpl_label::mech_wave_runup  << "(dike, dune)"<<endl;
		info << " "<<fpl_label::mech_wind_wave  << "(all)"<<endl;
		info << " "<<fpl_label::mech_bursting_din  << "(dike)"<<endl;
		

		info << " "<<fpl_label::mech_piping_sell  << "(dike)"<<endl;
		info << " "<<fpl_label::mech_piping_lane  << "(dike)"<<endl;
		info << " "<<fpl_label::mech_piping_schmert  << "(dike)"<<endl;

		info << " "<<fpl_label::mech_slope_landside  << "(dike)"<<endl;
		info << " "<<fpl_label::mech_slope_waterside  << "(dike)"<<endl;
		
		info << " "<<fpl_label::mech_erosion_landside  << "(dike)"<<endl;
		info << " "<<fpl_label::mech_erosion_waterside  << "(dike)"<<endl;
		info << " "<<fpl_label::mech_erosion_waterside_start  << "(dike)"<<endl;

		info << " "<<fpl_label::mech_erosion_waterside_gent  <<"(dune)"<< endl;

		msg.make_second_info(info.str());
		throw msg;

	}

	return type;
}
//Convert a mechanism type (_fpl_mechanism_types) into a string (static)
string _Fpl_Section_Type::convert_mechanism_type2txt(const _fpl_mechanism_types type){
	string buffer;
	switch(type){
		case _fpl_mechanism_types::overflow:
			buffer=fpl_label::mech_overflow;
			break;
		case _fpl_mechanism_types::wave_runup:
			buffer=fpl_label::mech_wave_runup;
			break;
		case _fpl_mechanism_types::wind_wave:
			buffer=fpl_label::mech_wind_wave;
			break;
		case _fpl_mechanism_types::bursting:
            buffer=fpl_label::mech_bursting_din;
			break;
		case _fpl_mechanism_types::piping_selli:
            buffer=fpl_label::mech_piping_sell;
			break;
		case _fpl_mechanism_types::piping_lane:
            buffer=fpl_label::mech_piping_lane;
			break;
		case _fpl_mechanism_types::piping_schmert:
            buffer=fpl_label::mech_piping_schmert;
			break;
		case _fpl_mechanism_types::landside_stability:
            buffer=fpl_label::mech_slope_landside;
			break;
		case _fpl_mechanism_types::waterside_stability:
            buffer=fpl_label::mech_slope_waterside;
			break;
		case _fpl_mechanism_types::landside_erosion:
            buffer=fpl_label::mech_erosion_landside;
			break;
		case _fpl_mechanism_types::waterside_erosion:
            buffer=fpl_label::mech_erosion_waterside;
			break;
		case _fpl_mechanism_types::start_waterside_erosion:
            buffer=fpl_label::mech_erosion_waterside_start;
			break;
		case _fpl_mechanism_types::wave_impact:
            buffer=fpl_label::mech_wave_impact;
			break;
		case _fpl_mechanism_types::waterside_erosion_gent:
            buffer=fpl_label::mech_erosion_waterside_gent;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//_____________
//protected
//Find the start of an input block of the fpl-section in file
qint64 _Fpl_Section_Type::find_start_block_file(QFile *ifile, int *line_counter, const string begin, const string end, const bool must_found){
	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	qint64 pos_file=0;

	//search for the begin
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==begin){
			found_flag=true;
			pos_file=ifile->pos();
			break;
		}
		if(myline==end){
			break;
		}
	}
	while(ifile->atEnd()==false);


	if(found_flag==false && must_found==true){
		Error msg=this->set_error(3);
		ostringstream info;
		info << "Start keyword : " << begin << endl;
		info << "End keyword   : " << end<<endl;
		msg.make_second_info(info.str());
 		throw msg;

	}
	else{
		found_flag=false;
	}

	return pos_file;

}
//Reset the flags for the random generation
void _Fpl_Section_Type::reset_random_flag(void){
	this->waterlevel.reset_random_generated();
}
//____________
//private
//Set error(s)
Error _Fpl_Section_Type::set_error(const int err_type){
		string place="_Fpl_Section_Type::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://table not found (fatal error)
			place.append("set_input_mechanism(QSqlDatabase *ptr_database)");
			reason="Table not found";
			help="Check the table name";
			type=2;
			fatal=true;//fatal error;
			break;
		case 1://no results found (non fatal error)
			place.append("set_input_mechanism(QSqlDatabase *ptr_database))");
			reason="No results found in table";
			help="Check the table";
			info << "Table Name: " << table_mechanism->table_name.name << endl;
			type=2;
			break;
		case 2://bad_alloc
			place.append("set_input../set_table_mechanism..");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			fatal=true;//fatal error;
			break;
		case 3://do not find the begin of the general section-type information in file
			place.append("find_start_block_file(QFile *ifile, int *line_counter, const string begin, const string end, const bool must_found)");
			reason="Can not find the begin of the data block in file";
			help="The section block data has to start with the keys !$BEGIN_...";
			type=1;
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
