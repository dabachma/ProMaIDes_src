//#include "Fpl_Distribution_Class_Mean.h"
#include "Fpl_Headers_Precompiled.h"

//init static variables
Tables *Fpl_Distribution_Class_Mean::table=NULL;
Tables *Fpl_Distribution_Class_Mean::default_table=NULL;

//Default constructor
Fpl_Distribution_Class_Mean::Fpl_Distribution_Class_Mean(void){
	this->mean=0.0;
	this->std_dev=0.0;
	this->var_koef=0.0;
	this->constant=0.0;
	this->type_distribution=_fpl_mean_distribution_type::deterministic_type;
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Distribution_Class_Mean), _sys_system_modules::FPL_SYS);//count the memory
}
//Constructor with all parameters given
Fpl_Distribution_Class_Mean::Fpl_Distribution_Class_Mean(const double var_mean , const double second_moment, const double var_constant, const bool calculated_mean, const _fpl_boundaries boundaries, _fpl_mean_distribution_type type){
	this->mean=var_mean;
	this->boundaries=boundaries;
	this->constant=var_constant;
	this->type_distribution=type;
	if(calculated_mean==false){
		this->std_dev=second_moment;
		this->var_koef=0.0;
		this->calculate_varkoef();
	}
	else{
		this->var_koef=second_moment;
		this->std_dev=0.0;
		this->calculate_stddev();
	}
	
	this->check_input(calculated_mean);
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Distribution_Class_Mean), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Distribution_Class_Mean::~Fpl_Distribution_Class_Mean(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Distribution_Class_Mean), _sys_system_modules::FPL_SYS);//count the memory
}
//___________
//public
//Set the database table for the parameters of the mean distribution types (user-values): it sets the table name and the name of the columns and allocate them (static)
void Fpl_Distribution_Class_Mean::set_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Mean::table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_rand_mean_user;
		string tab_col[6];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::variable_id;
		tab_col[2]=fpl_label::mean_type;
		tab_col[3]=fpl_label::mean_mean;
		tab_col[4]=fpl_label::mean_second_moment;
		tab_col[5]=fpl_label::constant;

		//set new table
		try{
			Fpl_Distribution_Class_Mean::table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Distribution_Class_Mean::table->set_name( ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("void Fpl_Distribution_Class_Mean::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Distribution_Class_Mean::close_table();
			throw msg;
		}
	}
}
//Create the database table for the parameters of the mean distribution types (user-values) (static)
void Fpl_Distribution_Class_Mean::create_table(QSqlDatabase *ptr_database){
		if(Fpl_Distribution_Class_Mean::table==NULL){
			ostringstream cout;
			cout << "Create random mean type distributed variable database table (user defined)..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_rand_mean_user;
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

			tab_col[1].name=fpl_label::variable_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=fpl_label::mean_type;
			tab_col[2].type=sys_label::tab_col_type_string;

			tab_col[3].name=fpl_label::mean_mean;
			tab_col[3].type=sys_label::tab_col_type_double;

			tab_col[4].name=fpl_label::mean_second_moment;
			tab_col[4].unsigned_flag=true;
			tab_col[4].type=sys_label::tab_col_type_double;

			tab_col[5].name=fpl_label::constant;
			tab_col[5].type=sys_label::tab_col_type_double;
			tab_col[5].default_value="0.0";

			try{
				Fpl_Distribution_Class_Mean::table= new Tables();
				if(Fpl_Distribution_Class_Mean::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Distribution_Class_Mean::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Distribution_Class_Mean::close_table();
				throw msg;
			}

		Fpl_Distribution_Class_Mean::close_table();
	}
}
//Delete all data in the database table for the parameters of the mean distribution types (user-values) (static)
void Fpl_Distribution_Class_Mean::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Distribution_Class_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in table
	Fpl_Distribution_Class_Mean::table->delete_data_in_table(ptr_database);
}
//Delete data in the database table for the parameters of the mean distribution types (user-values) for a given random variable id (static)
void Fpl_Distribution_Class_Mean::delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id){
	try{
		Fpl_Distribution_Class_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Delete ";
	test_filter << " from " << Fpl_Distribution_Class_Mean::table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::variable_id) << " = " << rand_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);
	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Distribution_Class_Mean::delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Mean::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Set the database table for the parameters of the mean distribution types (default-values): it sets the table name and the name of the columns and allocate them (static)
void Fpl_Distribution_Class_Mean::set_default_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Mean::default_table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_rand_mean_default;
		string tab_col[8];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::variable_name;
		tab_col[2]=fpl_label::default_name;
		tab_col[3]=fpl_label::mean_type;
		tab_col[4]=fpl_label::mean_mean;
		tab_col[5]=fpl_label::mean_second_moment;
		tab_col[6]=fpl_label::constant;
		tab_col[7]=label::description;



		//set new table
		try{
			Fpl_Distribution_Class_Mean::default_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Distribution_Class_Mean::default_table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("Fpl_Distribution_Class_Mean::set_default_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Distribution_Class_Mean::close_table();
			throw msg;
		}
	}
}
//Create the database table for the parameters of the mean distribution types (default-values) (static)
bool Fpl_Distribution_Class_Mean::create_default_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Mean::default_table==NULL){
			ostringstream cout;
			cout << "Create random mean type distributed variable database table (default)..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_rand_mean_default;
			const int num_col=8;
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

			tab_col[1].name=fpl_label::variable_name;
			tab_col[1].type=sys_label::tab_col_type_string;
			tab_col[1].key_flag=true;

			tab_col[2].name=fpl_label::default_name;
			tab_col[2].type=sys_label::tab_col_type_string;
			tab_col[2].key_flag=true;

			tab_col[3].name=fpl_label::mean_type;
			tab_col[3].type=sys_label::tab_col_type_string;

			tab_col[4].name=fpl_label::mean_mean;
			tab_col[4].type=sys_label::tab_col_type_double;

			tab_col[5].name=fpl_label::mean_second_moment;
			tab_col[5].unsigned_flag=true;
			tab_col[5].type=sys_label::tab_col_type_double;

			tab_col[6].name=fpl_label::constant;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].default_value="0.0";

			tab_col[7].name=label::description;
			tab_col[7].type=sys_label::tab_col_type_string;

			try{
				Fpl_Distribution_Class_Mean::default_table= new Tables();
				if(Fpl_Distribution_Class_Mean::default_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
					Fpl_Distribution_Class_Mean::close_table();
					return false;
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Distribution_Class_Mean::create_default_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Distribution_Class_Mean::close_table();
				throw msg;
			}

		Fpl_Distribution_Class_Mean::close_table();
		return true;
	}
	return false;
}
//Delete all data in the database table for the parameters of the mean distribution types (default-values) (static)
void Fpl_Distribution_Class_Mean::delete_data_in_default_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Distribution_Class_Mean::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in table
	Fpl_Distribution_Class_Mean::default_table->delete_data_in_table(ptr_database);
	//restore predefined default variables
	Fpl_Distribution_Class_Mean::write_default_variables2table(ptr_database);
}
//Prepare and write the predefined default variables to the database table (static)
void Fpl_Distribution_Class_Mean::write_default_variables2table(QSqlDatabase *ptr_database){
	const int number=33;
	_fpl_default_mean buff[number];

	buff[0].variable_name=fpl_label::water_density;
	buff[0].default_name=fpl_label::def_water;
	buff[0].mean=1000.0;
	buff[0].second_moment=5.0;
	buff[0].constant=0.0;
	buff[0].type=_fpl_mean_distribution_type::normal_type;
	buff[0].description="Density of normal water [kg/m^2]";

	buff[1].variable_name=fpl_label::water_density;
	buff[1].default_name="salty";
	buff[1].mean=1010.0;
	buff[1].second_moment=5.0;
	buff[1].constant=0.0;
	buff[1].type=_fpl_mean_distribution_type::normal_type;
	buff[1].description="Density of salty water, e.g. sea water [kg/m^2]";

	buff[2].variable_name=fpl_label::lane_factor;
	buff[2].default_name=fpl_label::def_silt;
	buff[2].mean=8.5;
	buff[2].second_moment=0.2;
	buff[2].constant=0.0;
	buff[2].type=_fpl_mean_distribution_type::normal_type;
	buff[2].description="Lane factor for silt or very fine sand [-]";

	buff[3].variable_name=fpl_label::lane_factor;
	buff[3].default_name=fpl_label::def_fine_sand;
	buff[3].mean=7.0;
	buff[3].second_moment=0.2;
	buff[3].constant=0.0;
	buff[3].type=_fpl_mean_distribution_type::normal_type;
	buff[3].description="Lane factor for fine sand [-]";

	buff[4].variable_name=fpl_label::lane_factor;
	buff[4].default_name=fpl_label::def_medium_sand;
	buff[4].mean=6.0;
	buff[4].second_moment=0.2;
	buff[4].constant=0.0;
	buff[4].type=_fpl_mean_distribution_type::normal_type;
	buff[4].description="Lane factor for medium sand [-]";

	buff[5].variable_name=fpl_label::lane_factor;
	buff[5].default_name=fpl_label::def_coarse_sand;
	buff[5].mean=5.0;
	buff[5].second_moment=0.2;
	buff[5].constant=0.0;
	buff[5].type=_fpl_mean_distribution_type::normal_type;
	buff[5].description="Lane factor for coarse sand [-]";

	buff[6].variable_name=fpl_label::lane_factor;
	buff[6].default_name=fpl_label::def_fine_gravel;
	buff[6].mean=4.0;
	buff[6].second_moment=0.2;
	buff[6].constant=0.0;
	buff[6].type=_fpl_mean_distribution_type::normal_type;
	buff[6].description="Lane factor for fine gravel [-]";

	buff[7].variable_name=fpl_label::lane_factor;
	buff[7].default_name=fpl_label::def_medium_gravel;
	buff[7].mean=3.5;
	buff[7].second_moment=0.2;
	buff[7].constant=0.0;
	buff[7].type=_fpl_mean_distribution_type::normal_type;
	buff[7].description="Lane factor for medium gravel [-]";

	buff[8].variable_name=fpl_label::lane_factor;
	buff[8].default_name=fpl_label::def_coarse_gravel;
	buff[8].mean=3.0;
	buff[8].second_moment=0.2;
	buff[8].constant=0.0;
	buff[8].type=_fpl_mean_distribution_type::normal_type;
	buff[8].description="Lane factor for coarse gravel [-]";

	buff[9].variable_name=fpl_label::lane_factor;
	buff[9].default_name=fpl_label::def_boulders;
	buff[9].mean=2.5;
	buff[9].second_moment=0.2;
	buff[9].constant=0.0;
	buff[9].type=_fpl_mean_distribution_type::normal_type;
	buff[9].description="Lane factor for boulders with some cobbles and gravel [-]";

	buff[10].variable_name=fpl_label::dens_imperm_lay;
	buff[10].default_name=fpl_label::def_silt;
	buff[10].mean=2000.0;
	buff[10].second_moment=100.0;
	buff[10].constant=0.0;
	buff[10].type=_fpl_mean_distribution_type::normal_type;
	buff[10].description="Density for silt or very fine sand [kg/m^2]";
	
	buff[11].variable_name=fpl_label::dens_imperm_lay;
	buff[11].default_name=fpl_label::def_clay;
	buff[11].mean=2100.0;
	buff[11].second_moment=105.0;
	buff[11].constant=0.0;
	buff[11].type=_fpl_mean_distribution_type::normal_type;
	buff[11].description="Density for clay [kg/m^2]" ;
	
	buff[12].variable_name=fpl_label::dens_perm_lay;
	buff[12].default_name=fpl_label::def_fine_sand;
	buff[12].mean=2050.0;
	buff[12].second_moment=102.5;
	buff[12].constant=0.0;
	buff[12].type=_fpl_mean_distribution_type::normal_type;
	buff[12].description="Density for fine sand [kg/m^2]";
	
	buff[13].variable_name=fpl_label::dens_perm_lay;
	buff[13].default_name=fpl_label::def_medium_sand;
	buff[13].mean=2070.0;
	buff[13].second_moment=103.5;
	buff[13].constant=0.0;
	buff[13].type=_fpl_mean_distribution_type::normal_type;
	buff[13].description="Density for medium sand [kg/m^2]";
	
	buff[14].variable_name=fpl_label::dens_perm_lay;
	buff[14].default_name=fpl_label::def_coarse_sand;
	buff[14].mean=2100.0;
	buff[14].second_moment=105.0;
	buff[14].constant=0.0;
	buff[14].type=_fpl_mean_distribution_type::normal_type;
	buff[14].description="Density for coarse sand [kg/m^2]";
	
	buff[15].variable_name=fpl_label::dens_perm_lay;
	buff[15].default_name=fpl_label::def_fine_gravel;
	buff[15].mean=2150.0;
	buff[15].second_moment=107.5;
	buff[15].constant=0.0;
	buff[15].type=_fpl_mean_distribution_type::normal_type;
	buff[15].description="Density for fine gravel [kg/m^2]";

	buff[16].variable_name=fpl_label::hyd_head_loss;
	buff[16].default_name=fpl_label::standard;
	buff[16].mean=0.001;
	buff[16].second_moment=0.0009;
	buff[16].constant=0.0;
	buff[16].type=_fpl_mean_distribution_type::normal_type;
	buff[16].description="Hydraulic head loss [m/m] (standard value)";

	buff[17].variable_name=fpl_label::kf_perm_lay;
	buff[17].default_name=fpl_label::def_silt;
	buff[17].mean=1e-9;
	buff[17].second_moment=1.3e-9;
	buff[17].constant=0.0;
	buff[17].type=_fpl_mean_distribution_type::log_normal_type;
	buff[17].description="k_f value of silt [m/s]";

	buff[18].variable_name=fpl_label::kf_perm_lay;
	buff[18].default_name=fpl_label::def_fine_sand;
	buff[18].mean=1e-8;
	buff[18].second_moment=1.3e-8;
	buff[18].constant=0.0;
	buff[18].type=_fpl_mean_distribution_type::log_normal_type;
	buff[18].description="k_f value of fine sand [m/s]";

	buff[19].variable_name=fpl_label::kf_perm_lay;
	buff[19].default_name=fpl_label::def_medium_sand;
	buff[19].mean=1e-6;
	buff[19].second_moment=1.3e-6;
	buff[19].constant=0.0;
	buff[19].type=_fpl_mean_distribution_type::log_normal_type;
	buff[19].description="k_f value of medium sand [m/s]";
	
	buff[20].variable_name=fpl_label::kf_perm_lay;
	buff[20].default_name=fpl_label::def_coarse_sand;
	buff[20].mean=5e-5;
	buff[20].second_moment=6.5e-5;
	buff[20].constant=0.0;
	buff[20].type=_fpl_mean_distribution_type::log_normal_type;
	buff[20].description="k_f value of coarse sand [m/s]";

	buff[21].variable_name=fpl_label::kf_perm_lay;
	buff[21].default_name=fpl_label::def_fine_gravel;
	buff[21].mean=4e-4;
	buff[21].second_moment=5.1e-4;
	buff[21].constant=0.0;
	buff[21].type=_fpl_mean_distribution_type::log_normal_type;
	buff[21].description="k_f value of fine gravel [m/s]";
	
	buff[22].variable_name=fpl_label::dragforce_fac;
	buff[22].default_name=fpl_label::standard;
	buff[22].mean=0.25;
	buff[22].second_moment=0.1;
	buff[22].constant=0.0;
	buff[22].type=_fpl_mean_distribution_type::normal_type;
	buff[22].description="Dragforce factor [-] (standard value)";
	
	buff[23].variable_name=fpl_label::roll_res_angle;
	buff[23].default_name=fpl_label::standard;
	buff[23].mean=41.0;
	buff[23].second_moment=2.05;
	buff[23].constant=0.0;
	buff[23].type=_fpl_mean_distribution_type::normal_type;
	buff[23].description="Rolling resistance angle [°] (standard value)";

	buff[24].variable_name=fpl_label::d_70;
	buff[24].default_name=fpl_label::def_silt;
	buff[24].mean=2.5e-5;
	buff[24].second_moment=5e-6;
	buff[24].constant=0.0;
	buff[24].type=_fpl_mean_distribution_type::log_normal_type;
	buff[24].description="d_70 [m] of silt";

	buff[25].variable_name=fpl_label::d_70;
	buff[25].default_name=fpl_label::def_fine_sand;
	buff[25].mean=1e-4;
	buff[25].second_moment=2.0e-5;
	buff[25].constant=0.0;
	buff[25].type=_fpl_mean_distribution_type::log_normal_type;
	buff[25].description="d_70 [m] of fine sand";

	buff[26].variable_name=fpl_label::d_70;
	buff[26].default_name=fpl_label::def_medium_sand;
	buff[26].mean=5e-4;
	buff[26].second_moment=1.0e-4;
	buff[26].constant=0.0;
	buff[26].type=_fpl_mean_distribution_type::log_normal_type;
	buff[26].description="d_70 [m] of medium sand";
	
	buff[27].variable_name=fpl_label::d_70;
	buff[27].default_name=fpl_label::def_coarse_sand;
	buff[27].mean=4e-3;
	buff[27].second_moment=8e-4;
	buff[27].constant=0.0;
	buff[27].type=_fpl_mean_distribution_type::log_normal_type;
	buff[27].description="d_70 [m] of coarse sand";

	buff[28].variable_name=fpl_label::d_70;
	buff[28].default_name=fpl_label::def_fine_gravel;
	buff[28].mean=8e-3;
	buff[28].second_moment=1.6e-3;
	buff[28].constant=0.0;
	buff[28].type=_fpl_mean_distribution_type::log_normal_type;
	buff[28].description="d_70 [m] of fine gravel";


	buff[29].variable_name=fpl_label::crit_landside_discharge;
	buff[29].default_name=fpl_label::def_weak_ero_res;
	buff[29].mean=1e-4;
	buff[29].second_moment=1.6e-5;
	buff[29].constant=0.0;
	buff[29].type=_fpl_mean_distribution_type::log_normal_type;
	buff[29].description="Erosion resistance of the landside slope [m³/(sm)]: weak";


	buff[30].variable_name=fpl_label::crit_landside_discharge;
	buff[30].default_name=fpl_label::def_medium_ero_res;
	buff[30].mean=1e-3;
	buff[30].second_moment=1.6e-4;
	buff[30].constant=0.0;
	buff[30].type=_fpl_mean_distribution_type::log_normal_type;
	buff[30].description="Erosion resistance of the landside slope [m³/(sm)]: medium";

	buff[31].variable_name=fpl_label::crit_landside_discharge;
	buff[31].default_name=fpl_label::def_strong_ero_res;
	buff[31].mean=5e-2;
	buff[31].second_moment=1.6e-3;
	buff[31].constant=0.0;
	buff[31].type=_fpl_mean_distribution_type::log_normal_type;
	buff[31].description="Erosion resistance of the landside slope [m³/(sm)]: strong";

	buff[32].variable_name=fpl_label::crit_landside_discharge;
	buff[32].default_name=fpl_label::def_very_strong_ero_res;
	buff[32].mean=2e-1;
	buff[32].second_moment=1.6e-2;
	buff[32].constant=0.0;
	buff[32].type=_fpl_mean_distribution_type::log_normal_type;
	buff[32].description="Erosion resistance of the landside slope [m³/(sm)]: very strong";

	//...add more values; think to change the number

	try{
		Fpl_Distribution_Class_Mean::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	//set them to database table
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);

	//evaluate the global identifier
	int id_glob=Fpl_Distribution_Class_Mean::default_table->maximum_int_of_column(Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::glob_id),ptr_database)+1;
	ostringstream total;

	//set the query via a query string
	ostringstream query_string_fix;
	query_string_fix << "INSERT INTO  " << Fpl_Distribution_Class_Mean::default_table->get_table_name();
	query_string_fix <<" ( ";
	query_string_fix << Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::variable_name) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::default_name) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::mean_type) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::mean_mean) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::mean_second_moment) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::constant) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean::default_table->get_column_name(label::description) <<" ) ";


	for(int i=0; i< number; i++){
		total.str("");
		ostringstream query_string;
		query_string << " VALUES ( ";
		query_string <<  id_glob << " , " ;
		query_string <<"'"<< buff[i].variable_name <<"'"<< " , " ;
		query_string <<"'"<< buff[i].default_name <<"'"<< " , " ;
		query_string <<"'"<< Fpl_Distribution_Class_Mean::convert_meandist_type2txt(buff[i].type) <<"'"<< " , " ;
		query_string << buff[i].mean << " , " ;
		query_string << buff[i].second_moment<< " , " ;
		query_string << buff[i].constant <<" , ";
		query_string <<"'"<< buff[i].description <<"'"<< " ) " ;
		
	
		total <<query_string_fix.str() <<  query_string.str();

		Data_Base::database_request(&model, total.str(), ptr_database);
		id_glob++;
	}

}
//Close and delete the database table for the parameters of the mean distribution types the user-value-tables as well as the default table (static)
void Fpl_Distribution_Class_Mean::close_table(void){
	if(Fpl_Distribution_Class_Mean::table!=NULL){
		delete Fpl_Distribution_Class_Mean::table;
		Fpl_Distribution_Class_Mean::table=NULL;
	}
	if(Fpl_Distribution_Class_Mean::default_table!=NULL){
		delete Fpl_Distribution_Class_Mean::default_table;
		Fpl_Distribution_Class_Mean::default_table=NULL;
	}
}
//Copy the parameters of the distribution type in the database table (static)
void Fpl_Distribution_Class_Mean::copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id){
	try{
		Fpl_Distribution_Class_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Fpl_Distribution_Class_Mean::table->maximum_int_of_column(Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::glob_id) ,ptr_database)+1;

	
	ostringstream test_filter;

	

	QSqlQueryModel model1;
	

	test_filter.str("");
	test_filter << "INSERT INTO "<< Fpl_Distribution_Class_Mean::table->get_table_name();
	test_filter << " SELECT " << glob_id <<" , " << new_rand_id <<" , ";
	test_filter <<  Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::mean_type)  <<" , ";
	test_filter <<  Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::mean_mean)  <<" , ";
	test_filter <<  Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::mean_second_moment)  <<" , ";
	test_filter <<  Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::constant)  <<"  ";


	test_filter << " FROM " << Fpl_Distribution_Class_Mean::table->get_table_name() <<" ";
	test_filter << " WHERE " << Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::glob_id) << " = ";
	test_filter << old_rand_id;
	Data_Base::database_request(&model1, test_filter.str(), ptr_database);
	if(model1.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Distribution_Class_Mean::copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Mean::table->get_table_name() << endl;
		info << "Table error info: " << model1.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Set the input via console
void Fpl_Distribution_Class_Mean::set_input(void){
	//read input
	bool flag=false;
	cout << "You chose a distribution class mean!" << endl;
	_fpl_mean_distribution_type buffer_type=_fpl_mean_distribution_type::unknown_mean_type;

	//distribution type
	do{ 
		cout << "Give the distribution type please!" << endl;
		cout << "(0) deterministic, (1) uniform, (2) normal, (3) expo, (4) lognormal, (5) gumbel, (6) weibull" << endl;
		
		int buffer=0;
		//if((flag=(cin  >> buffer))==false){//check if the input is valif
		//	cout << "Wrong input! Do it again" << endl;
		//	cin.clear();//clear the error flags
		//	cin.ignore();//clear cin
		//}
		//else if((buffer_type=Fpl_Distribution_Class_Mean::convert_int2meandist_type(buffer))==_fpl_mean_distribution_type::unknown_mean_type){
		//	cout << "Unknwon mean-distribution type! Do it again! " << endl;
		//}
		//else{
		//	this->type_distribution=buffer_type;
		//}
	}while(flag==false || buffer_type==_fpl_mean_distribution_type::unknown_mean_type);

	//mean value
	do{
		cout << "Give the mean please! " << endl;
		//if((flag=(cin  >> this->mean))==false){//check if the input is valif
		//	cout << "Wrong input! Do it again" << endl;
		//	cin.clear();//clear the error flags
		//	cin.ignore();//clear cin
		//}
	}while(flag==false);

	//standard deviation
	do{
		cout << "Give the standard deviation please!" << endl;
		//if((flag=(cin  >> this->std_dev))==false){//check if the input is valif
		//	cout << "Wrong input! Do it again" << endl;
		//	cin.clear();//clear the error flags
		//	cin.ignore();//clear cin
		//}
		//else if(this->std_dev <0.0){
		//	cout << "No negative Standard deviation is possible! Do it again! " << endl;
		//}
	}while(flag==false || this->std_dev <0.0);

	//constant
	do{
		cout << "Give the constant please!" << endl;
		//if((flag=(cin  >> constant))==false){//check if the input is valif
		//		cout << "Wrong input! Do it again" << endl;
		//		cin.clear();//clear the error flags
		//		cin.ignore();//clear cin
		//}
	}while(flag==false);

	//set the varkoef
	this->calculate_varkoef();
	//check the parameters
	this->check_input(false);
}
//Set the input values per given parameters
void Fpl_Distribution_Class_Mean::set_input(const double var_mean , const double second_moment, const double var_constant, const bool calculated_mean, const _fpl_boundaries boundaries, _fpl_mean_distribution_type type){
	this->mean=var_mean;
	this->boundaries=boundaries;
	this->constant=var_constant;
	this->type_distribution=type;
	if(calculated_mean==false){
		this->std_dev=second_moment;
		this->var_koef=0.0;
		this->calculate_varkoef();
	}
	else{
		this->var_koef=second_moment;
		this->std_dev=0.0;
		this->calculate_stddev();
	}
	
	this->check_input(calculated_mean);
}
///Set the parameter via database table for a user defined variable
void Fpl_Distribution_Class_Mean::set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;
	QSqlQueryModel results;

	int number=0;
	try{
		Fpl_Distribution_Class_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Distribution_Class_Mean::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::variable_id) << " = " << position;
	
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Mean::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results.rowCount();

	if(number==0){
		Error msg=this->set_error(1);	
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Mean::table->get_table_name() << endl;
		info << "Variable id     : " << position << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(number>1){
		Warning msg=this->set_warning(0);	
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Mean::table->get_table_name() << endl;
		info << "Variable id     : " << position << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	//input
	this->mean=results.record(0).value((Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::mean_mean)).c_str()).toDouble();
	this->constant=results.record(0).value((Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::constant)).c_str()).toDouble();
	this->type_distribution=Fpl_Distribution_Class_Mean::convert_txt2meandist_type(results.record(0).value((Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::mean_type)).c_str()).toString().toStdString());
	if(calculated_mean==false){
		this->std_dev=results.record(0).value((Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::mean_second_moment)).c_str()).toDouble();
		this->calculate_varkoef();
	}
	else{
		this->var_koef=results.record(0).value((Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::mean_second_moment)).c_str()).toDouble();
		this->calculate_stddev();
	}

	this->check_input(calculated_mean);

}
//Set the parameter via database table for a default variable
void Fpl_Distribution_Class_Mean::set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;
	QSqlQueryModel results;
	int number=0;
	try{
		Fpl_Distribution_Class_Mean::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Distribution_Class_Mean::default_table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::variable_name) << " = '" << name <<"'";
	test_filter << " and ";
	test_filter << Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::default_name) << " = '" << default_name<<"'";

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Mean::default_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results.rowCount();

	if(number==0){
		Error msg=this->set_error(3);	
		ostringstream info;
		info << "Table Name            : " << Fpl_Distribution_Class_Mean::default_table->get_table_name() << endl;
		info << "Variable name         : " << name << endl;
		info << "Variable default name : " << default_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(number>1){
		Warning msg=this->set_warning(1);	
		ostringstream info;
		info << "Table Name            : " << Fpl_Distribution_Class_Mean::default_table->get_table_name() << endl;
		info << "Variable name         : " << name << endl;
		info << "Variable default name : " << default_name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	//input
	this->mean=results.record(0).value((Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::mean_mean)).c_str()).toDouble();
	this->constant=results.record(0).value((Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::constant)).c_str()).toDouble();
	this->type_distribution=Fpl_Distribution_Class_Mean::convert_txt2meandist_type(results.record(0).value((Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::mean_type)).c_str()).toString().toStdString());
	this->description=results.record(0).value((Fpl_Distribution_Class_Mean::default_table->get_column_name(label::description)).c_str()).toString().toStdString();
	if(calculated_mean==false){
		this->std_dev=results.record(0).value((Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::mean_second_moment)).c_str()).toDouble();
		this->calculate_varkoef();
	}
	else{
		this->var_koef=results.record(0).value((Fpl_Distribution_Class_Mean::default_table->get_column_name(fpl_label::mean_second_moment)).c_str()).toDouble();
		this->calculate_stddev();
	}

	this->check_input(calculated_mean);
}
//Set the parameter via file for a user defined variable
void Fpl_Distribution_Class_Mean::set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;

	//read in
	string myline;
	QString qmyline;
	int must_found_counter=0;
	int must_found=2;
	bool found_flag=false;
	bool second_moment_found=false;

	//search for the keywords
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::end_dist_type){
			found_flag=true;
			break;
		}
		else if(myline==fpl_label::end_random){
			break;
		}
		else if(myline==fpl_label::begin_random){
			break;
		}
		else if(myline==fpl_label::begin_dist_type){
			break;
		}
		else if(myline.empty()==false){
			this->find_key_values_file(myline, &must_found_counter, calculated_mean, &second_moment_found);
		}

	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(4);
		throw msg;
	}

	if(calculated_mean==true){
		must_found=2;
		if(this->type_distribution==_fpl_mean_distribution_type::deterministic_type){
			must_found=1;
			if(second_moment_found==true){
				must_found_counter=must_found_counter-1;
			}
		}
	}
	else{
		must_found=3;
		if(this->type_distribution==_fpl_mean_distribution_type::deterministic_type){
			must_found=2;
			if(second_moment_found==true){
				must_found_counter=must_found_counter-1;
			}
		}
	}

	if(must_found_counter!=must_found){
		Error msg=this->set_error(6);
		throw msg;
	}

	if(calculated_mean==true){
		this->calculate_stddev();
	}
	else{
		this->calculate_varkoef();
	}

	this->check_input(calculated_mean);
}
//Transfer the data of the distribution type to database table
void Fpl_Distribution_Class_Mean::transfer_disttype2database(const int position, QSqlDatabase *ptr_database, const bool calculated_mean){

	try{
		Fpl_Distribution_Class_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int glob_id=Fpl_Distribution_Class_Mean::table->maximum_int_of_column(Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::glob_id), ptr_database)+1;


	//transfer the data
	ostringstream query_string;
	query_string << "INSERT INTO  " << Fpl_Distribution_Class_Mean::table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::variable_id) <<" , ";
	query_string << Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::mean_type) <<" , ";
	query_string << Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::mean_mean) <<" , ";
	query_string << Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::mean_second_moment) <<" , ";
	query_string << Fpl_Distribution_Class_Mean::table->get_column_name(fpl_label::constant) <<" ) ";
	query_string << " VALUES ( ";

	query_string << glob_id << " , " ;
	query_string << position<< " , " ;
	query_string <<"'"<< Fpl_Distribution_Class_Mean::convert_meandist_type2txt(this->type_distribution) << "' , " ;
	query_string << this->mean << " , " ;
	if(calculated_mean==false){
		query_string << this->std_dev << " , " ;
	}
	else{
		query_string << this->var_koef << " , " ;
	}
	query_string << this->constant << " ) " ;

	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, query_string.str(), ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(10);
		ostringstream info;
		info << "Table Name                : " << Fpl_Distribution_Class_Mean::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);	
	}
}
//Set the input of the distribution type via dialog for checking the random generator and the transformation
bool Fpl_Distribution_Class_Mean::set_input_check(void){
	FplGui_Check_Disttype_Mean_Dia my_dia;

	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->mean=my_dia.get_mean();
		this->std_dev=my_dia.get_std_dev();
		this->constant=my_dia.get_constant();
		this->type_distribution=my_dia.get_mean_dist_type();
		this->calculate_varkoef();
		try{
			this->check_input(false);
		}
		catch(Error msg){
			throw msg;
		}
		return true;
	}
}
//Output the member of the distribution type to console/display
void Fpl_Distribution_Class_Mean::output_member(bool detailed){
	ostringstream cout;
	cout << " Mean class            : " << Fpl_Distribution_Class_Mean::convert_meandist_type2txt(this->type_distribution) << endl;
	if(this->type_distribution!=_fpl_mean_distribution_type::weibull_type){
		cout << " Mean value            : " << this->mean << endl;
		if(this->type_distribution!=_fpl_mean_distribution_type::deterministic_type){
			cout <<" Standarddeviation     : " << this->std_dev <<endl;
			if(this->mean!=0.0){
				cout <<" Variationcoefficient  : " << this->var_koef << endl;
			}
		}
		cout <<" Constant              : "  << this->constant << endl;
	}
	else{
		cout << " Alpha value           : " << this->mean << endl;
		cout << " Beta value            : " << this->std_dev << endl;
		cout <<" Constant              : "  << this->constant << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
}
//Generate for the distribution a determinstic value (mean_value+constant)
_fpl_result Fpl_Distribution_Class_Mean::make_deterministic(void){
	this->random_result.inter_result=this->mean+this->constant;
	this->random_result.end_result=this->mean+this->constant;
	return this->random_result;
}
//Generate for the distribution a random distributed value
_fpl_result Fpl_Distribution_Class_Mean::make_random(void){
	double res=0.0;
	if(this->std_dev==0.0){
		res=this->mean;
		this->random_result.inter_result=res+this->constant;
		this->random_result.end_result=res+this->constant;
		return this->random_result;
	 
	}
	switch(this->type_distribution){
		case (_fpl_mean_distribution_type::deterministic_type) ://deterministic
			res=this->mean;
			break;
		case (_fpl_mean_distribution_type::uniform_type) ://uniformly distributed
			res=this->uniformly_dist();
			break;
		case(_fpl_mean_distribution_type::normal_type) ://normaly distrubuted
			res=this->normal_dist();
			break;
		case (_fpl_mean_distribution_type::exponentiel_type) ://exponentiell distributed 
			res=this->expo_dist();
			break;
		case (_fpl_mean_distribution_type::log_normal_type)://lognormal distributed
			res=this->lognormal_dist();
			break;
		case (_fpl_mean_distribution_type::gumbel_type)://gumbel distributed
			res=this->gumbel_dist();
			break;
		case (_fpl_mean_distribution_type::weibull_type)://weibull distributed
			res=this->weibull_dist();
			break;
		default:
			res=this->mean;
	}

	this->random_result.inter_result=res+this->constant;
	this->random_result.end_result=res+this->constant;
	return this->random_result;
}
//Statistic of the generation process: output it to console/display
void Fpl_Distribution_Class_Mean::output_statistic(const bool calc_flag, bool detailed){//output to the console
	ostringstream cout;
	cout << " Distribution class    : " << Fpl_Distribution_Class_Mean::convert_meandist_type2txt(this->type_distribution) << endl;
	cout << " Numbers of generation : " << this->counter_calc << endl;
	if(calc_flag==false){
		if(this->type_distribution!=_fpl_mean_distribution_type::weibull_type){
			cout << " Mean calculated       : " << this->mean_calc<< " ("<< this->mean+this->constant<<") "<<"["<<(this->mean+this->constant)-this->mean_calc<<"]"<<endl;
			cout << " Standard calculated   : " << this->std_dev_calc << " ("<< this->std_dev<<") "<<"["<<this->std_dev-this->std_dev_calc<<"]"<<endl;	
		}
		else{
			cout << " Attention: weibull-distribution alpha and beta given; not the mean and the standard deviation!" << endl;
			cout << " Alpha given           : " << this->mean<< endl;
			cout << " Beta given            : " << this->std_dev<< endl;
			cout << " Mean calculated       : " << this->mean_calc<< endl;
			cout << " Standard calculated   : " << this->std_dev_calc<< endl;
			
		}
	}
	else{
		cout << " Dynamic mean settings : " << this->counter_set_mid << endl;

		if(this->type_distribution!=_fpl_mean_distribution_type::weibull_type){
			if(this->counter_set_mid!=0){
				cout << " Mean of dynamic mean  : " << this->mid_calc/(double)this->counter_set_mid << " (" << this->mean_calc <<")"<< endl;
			}
			else{
				cout << " No dynamic mean is set"<<endl;
			}
		}
		else{
			if(this->counter_set_mid!=0){
				cout << " Mean of dynamic mean  : " << this->mid_calc/(double)this->counter_set_mid<<endl;
			}
			else{
				cout << " No dynamic mean is set"<<endl;
			}
		}
	}
	if(this->counter_calc=!0){
		cout << " Minimum               : " << this->min_calc << endl;
		cout << " Maximum               : " << this->max_calc << endl;
	}
	
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
}
//Set a new mean value of the distribution (dynamic mean-setting)
void Fpl_Distribution_Class_Mean::set_new_middle(const double new_mean){
	this->mean=new_mean;

	//sumup the new middle
	this->mid_calc=this->mid_calc+mean; //for statistical reason
	this->counter_set_mid++; //for statistical reason

	this->calculate_stddev();

	//lognormal with 0 or <0 mean is not possible
	if (this->type_distribution==_fpl_mean_distribution_type::log_normal_type && this->mean<=0.0){
		//warning
		Warning msg=this->set_warning(3);
		msg.set_during_calculation_flag(false);
		msg.output_msg(1);
		//reaction
		this->type_distribution=_fpl_mean_distribution_type::deterministic_type;
	}
}
//Convert the mean distribution type (_fpl_mean_distribution_type) to a string (static)
string Fpl_Distribution_Class_Mean::convert_meandist_type2txt(_fpl_mean_distribution_type type){
	string buffer;
	switch(type){
		case _fpl_mean_distribution_type::deterministic_type:
			buffer=fpl_label::dist_mean_determ;
			break;
		case _fpl_mean_distribution_type::normal_type:
			buffer=fpl_label::dist_mean_normal;
			break;
		case _fpl_mean_distribution_type::log_normal_type:
			buffer=fpl_label::dist_mean_log_normal;
			break;
		case _fpl_mean_distribution_type::uniform_type:
			buffer=fpl_label::dist_mean_uniform;
			break;
		case _fpl_mean_distribution_type::exponentiel_type:
			buffer=fpl_label::dist_mean_exponentiel;
			break;
		case _fpl_mean_distribution_type::gumbel_type:
			buffer=fpl_label::dist_mean_gumbel;
			break;
		case _fpl_mean_distribution_type::weibull_type:
			buffer=fpl_label::dist_mean_weibull;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//Convert a string to the mean distribution type (_fpl_mean_distribution_type) (static)
_fpl_mean_distribution_type Fpl_Distribution_Class_Mean::convert_txt2meandist_type(const string txt){
	_fpl_mean_distribution_type type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lower_case(&buffer);

	if(buffer==fpl_label::dist_mean_determ){
		type=_fpl_mean_distribution_type::deterministic_type;
	}
	else if(buffer==fpl_label::dist_mean_normal){
		type=_fpl_mean_distribution_type::normal_type;
	}
	else if(buffer==fpl_label::dist_mean_uniform){
		type=_fpl_mean_distribution_type::uniform_type;
	}
	else if(buffer==fpl_label::dist_mean_exponentiel){
		type=_fpl_mean_distribution_type::exponentiel_type;
	}
	else if(buffer==fpl_label::dist_mean_gumbel){
		type=_fpl_mean_distribution_type::gumbel_type;
	}
	else if(buffer==fpl_label::dist_mean_weibull){
		type=_fpl_mean_distribution_type::weibull_type;
	}
	else if(buffer==fpl_label::dist_mean_log_normal){
		type=_fpl_mean_distribution_type::log_normal_type;
	}
	else{
		Error msg;
		msg.set_msg("Fpl_Distribution_Class_Mean::convert_txt2meandist_type(const string txt)", "Can not convert this mean distribution type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert mean distribution type type: " << txt << endl;
		info << "Possible types: "<< endl;
		info << " "<<fpl_label::dist_mean_determ << endl;
		info << " "<<fpl_label::dist_mean_normal  << endl;
		info << " "<<fpl_label::dist_mean_uniform << endl;
		info << " "<<fpl_label::dist_mean_exponentiel  << endl;
		info << " "<<fpl_label::dist_mean_gumbel  << endl;
		info << " "<<fpl_label::dist_mean_weibull  << endl;
		info << " "<<fpl_label::dist_mean_log_normal<<endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	return type;
}
//Convert an integer to the mean distribution type (_fpl_mean_distribution_type)
_fpl_mean_distribution_type Fpl_Distribution_Class_Mean::convert_int2meandist_type(const int type){
	_fpl_mean_distribution_type buffer;
	switch(type){
		case(0):
			buffer=_fpl_mean_distribution_type::deterministic_type;
			break;
		case(1):
			buffer=_fpl_mean_distribution_type::uniform_type;
			break;
		case(2):
			buffer=_fpl_mean_distribution_type::normal_type;
			break;
		case(3):
			buffer=_fpl_mean_distribution_type::exponentiel_type;
			break;
		case(4):
			buffer=_fpl_mean_distribution_type::log_normal_type;
			break;
		case(5):
			buffer=_fpl_mean_distribution_type::gumbel_type;
			break;
		case(6):
			buffer=_fpl_mean_distribution_type::weibull_type;
			break;
		default:
			buffer=_fpl_mean_distribution_type::unknown_mean_type;
	}
		
	return buffer;
}
//Get calculated exact probability for the distribution for the random generator check
double Fpl_Distribution_Class_Mean::get_calulculated_exact_prob(const double value, const double stepsize){
	double res=0.0;
	double buff=value-this->constant;

	if(this->std_dev==0.0){
		res=direct_deterministic_dist(buff, stepsize);
		return res;
	}

	switch(this->type_distribution){
		case (_fpl_mean_distribution_type::deterministic_type) ://deterministic
			res=direct_deterministic_dist(buff, stepsize);
			break;
		case (_fpl_mean_distribution_type::uniform_type) ://uniformly distributed
			res=this->direct_uniformly_dist(buff);
			break;
		case(_fpl_mean_distribution_type::normal_type) ://normaly distrubuted
			res=this->direct_normal_dist(buff);
			break;
		case (_fpl_mean_distribution_type::exponentiel_type) ://exponentiell distributed 
			res=this->direct_expo_dist(buff);
			break;
		case (_fpl_mean_distribution_type::log_normal_type)://lognormal distributed
			res=this->direct_lognormal_dist(buff);
			break;
		case (_fpl_mean_distribution_type::gumbel_type)://gumbel distributed
			res=this->direct_gumbel_dist(buff);
			break;
		case (_fpl_mean_distribution_type::weibull_type)://weibull distributed
			res=this->direct_weibull_dist(buff);
			break;
		default:
			//like deterministic
			res=direct_deterministic_dist(buff, stepsize);
	}


	return res;
}
//Get mean distribution type as text
string Fpl_Distribution_Class_Mean::get_mean_distribution_type(void){
	return Fpl_Distribution_Class_Mean::convert_meandist_type2txt(this->type_distribution);
}
//Get the mean-value
double Fpl_Distribution_Class_Mean::get_mean_value(void){
	return this->mean;
}
//Get the standard deviation
double Fpl_Distribution_Class_Mean::get_std_dev(void){
	return this->std_dev;
}
//Get the variation coefficient
double Fpl_Distribution_Class_Mean::get_var_coeff(void){
	return this->var_koef;
}
//Get constant
double Fpl_Distribution_Class_Mean::get_constant(void){
	return this->constant;
}
//___________
//private
//Check the parameter of the distribution type
void Fpl_Distribution_Class_Mean::check_input(bool calculated_mean){
	if(this->type_distribution==_fpl_mean_distribution_type::deterministic_type){
		this->std_dev=0.0;
		this->var_koef=0.0;
		return;
	}
	if(calculated_mean==true){
		this->mean=0.0;
		if(this->var_koef<0.0){
			//warning
			Warning msg=this->set_warning(11);
			msg.output_msg(1);
			//reaction
			this->std_dev=0.0;
			this->var_koef=0.0;
		}
		return;
	}
	//check if there is an negative std_dev; it makes no sense
	if(this->std_dev<0.0){
		//warning
		Warning msg=this->set_warning(2);
		msg.output_msg(1);
		//reaction
		this->std_dev=0.0;
		this->var_koef=0.0;
	}
	if(this->type_distribution==_fpl_mean_distribution_type::unknown_mean_type){
		//external warning
		Warning msg=set_warning(4);
		msg.output_msg(1);
		//reaction
		this->type_distribution=_fpl_mean_distribution_type::deterministic_type;
	}
	if(this->mean<this->boundaries.min){
		//external warning
		Warning msg=this->set_warning(6);
		msg.output_msg(1);
		//reaction
		this->mean=this->boundaries.min;
	}
	if(this->mean>this->boundaries.max){
		//external warning
		Warning msg=this->set_warning(7);
		msg.output_msg(1);
		//reaction
		this->mean=this->boundaries.max;
	}
	//lognormal with 0 or <0 mean is not possible
	if (this->type_distribution==_fpl_mean_distribution_type::log_normal_type && this->mean<=0.0){
		//warning
		Warning msg=this->set_warning(3);
		msg.output_msg(1);
		//reaction
		this->type_distribution=_fpl_mean_distribution_type::deterministic_type;;
	}

	//weibull and alpha(mean) <=0 
	if (this->type_distribution==_fpl_mean_distribution_type::weibull_type && this->mean<=0.0){
		//warning
		Warning msg=this->set_warning(8);
		msg.output_msg(1);
		//reaction
		this->type_distribution=_fpl_mean_distribution_type::deterministic_type;;
	}
	//weibull and beta(std_dev) <0 
	if (this->type_distribution==_fpl_mean_distribution_type::weibull_type && this->std_dev<=0.0){
		//warning
		Warning msg=this->set_warning(9);
		msg.output_msg(1);
		//reaction
		this->type_distribution=_fpl_mean_distribution_type::deterministic_type;;
	}
}
//Find the key-values for the file input of the distribution type
void Fpl_Distribution_Class_Mean::find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag, bool *second_moment_found){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(fpl_label::key_mean_value);
	if(pos>=0 && wrong_input==false && calculated_flag==false){
		buffer=myline.substr(fpl_label::key_mean_value.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->mean;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(fpl_label::key_second_moment);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_second_moment.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		if(calculated_flag==false){
			buffer1 >> this->std_dev;
		}
		else{
			buffer1>>this->var_koef;
		}
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			*second_moment_found=true;
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_constant);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_constant.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1>>this->constant;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}
	pos=myline.find(fpl_label::key_mean_type);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_mean_type.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			this->type_distribution=Fpl_Distribution_Class_Mean::convert_txt2meandist_type(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
			throw msg;
		}
		(*must_found_counter)++;
		return;
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(5);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Calculate the variation coefficient with the standard deviation
void Fpl_Distribution_Class_Mean::calculate_varkoef(void){

	if(this->mean==0.0 && this->std_dev==0.0){
		this->var_koef=0.0;
	}
	else if(this->mean==0.0 && this->std_dev!=0.0){
		this->var_koef=this->std_dev/(1e-22);
	}
	else{
		this->var_koef=this->std_dev/this->mean;
	}

}
//Calculate the standard deviation with the variation coefficient
void Fpl_Distribution_Class_Mean::calculate_stddev(void){
	this->std_dev=this->mean*this->var_koef;
	if(this->std_dev<0.0){//no negative std_dev
		this->std_dev=abs(this->std_dev);
	}
}
//Transform the variable corresponding the distribution type: uniform distribution
double Fpl_Distribution_Class_Mean::uniformly_dist(void){
	double b,a;
	double random;
	//generation of the uniformly-distributed numbers [0..1]
	random=this->ran1();
	//transformation to uniformly-distributed numbers
	b=0.5*(this->std_dev*sqrt(12.0)+2.0*this->mean);
	a=2.0*this->mean-b;
	random=(b-a)*(random+a*(1.0/(b-a)));

	return random;
};
//Transform the variable corresponding the distribution type: normal distribution with the Box-Mueller-algorithm
double Fpl_Distribution_Class_Mean::normal_dist(void){
	double random1, random2;
	double result=0.0;

	//generation of the uniformly-distributed numbers [0..1]
	random1=this->ran1();
	random2=this->ran1();
	//case random1<=0: it is not possible for the log-function
	while(random1<=0){
		random1=this->ran1();
	}
	//transformation to normal-distributed numbers
	result=pow(-2.0*log(random1),0.5)*cos(2.0*constant::Cpi*random2);
	result=result*this->std_dev+this->mean;
	return result;
};
//Transform the variable corresponding the distribution type: exponential distribution
double Fpl_Distribution_Class_Mean::expo_dist(void){
	/**
	Transformation to exponentiel-distributed varaiable:
	y=1-e^(-(x-b)/a)<=> -log(1-y)*a+b=x (inverse) with a=std_dev, b=mean-std_dev
	*/
	double a, b;
	double random;

	//generation of the uniformly-distributed numbers [0..1]
	random=(double)this->ran1();
	//case random>=1: it is not possible for the log-function (1-random)
	while(random>=1){
		random=(double)this->ran1();
	}

	//transformation to exponentiell-distributed numbers
	a=this->std_dev;
	b=this->mean-this->std_dev;
	random=-log((1.0-random))*a+b;
	return random;

}
//Transform the variable corresponding the distribution type: log-normal distribution
double Fpl_Distribution_Class_Mean::lognormal_dist(void){
		double a,b;
		double random1, random2;
		double result=0.0;

		//generation of the uniformly-distributed numbers [0..1]
		random1=this->ran1();
		random2=this->ran1();
		//case random1<=0: it is not possible for the log-function
		while(random1<=0){
			random1=this->ran1();
		}

		//transformation to lognormal-distributed numbers
		//1. a=(ln(1+(std_dev/mean)^2))^0.5; b=ln(mean/(exp^(a^2))^0.5)
		a=sqrt(log(1.0+pow((this->std_dev/this->mean),2)));
		//2. b=ln(mean/(exp^(a^2))^0.5)
		b=log(this->mean/sqrt(exp(pow(a,2))));

		//Box-Mueller like by nomral-distributed
		result=pow(-2*log(random1),0.5)*cos(2*constant::Cpi*random2);
		result=result*a+b;
		result=exp(result);
		return result;

}

//Transform the variable corresponding the distribution type: gumbel distribution
double Fpl_Distribution_Class_Mean::gumbel_dist(void){

	double a;
	double b;
	double random;

	//generation of the uniformly-distributed numbers [0..1]
	random=this->ran1();

	//case random1<=0: it is not possible for the log-function
	while(random<=0){
		random=this->ran1();
	}
	
	//transformation to gumbel-distributed numbers
	a=pow(6.0,0.5)*this->std_dev/constant::Cpi;
	b=this->mean-(a*0.5772);
	random=(-log(-log(random)))*a+b;
	return random;
}
//Transform the variable corresponding the distribution type: weibull distribution
double Fpl_Distribution_Class_Mean::weibull_dist(void){
	//!!attention: here a and b is used; not the mean and the std_dev!!
	double a;
	double b;
	double random;

	//generation of the uniformly-distributed numbers [0..1]
	random=this->ran1();

	//case random1<=0: it is not possible for the log-function
	while(random>=1){
		random=this->ran1();
	}

	//input is alpha as mean und beta as std_dev
	a=this->mean; 
	b=this->std_dev; 
	//Transformation to weibull-distribution
	random=pow((1.0/a)*(-log(1.0-random)),1.0/b);
	return random;
}

//Direct calculation of the distribution type for checking the random- and transformation process: deterministic
double Fpl_Distribution_Class_Mean::direct_deterministic_dist(const double value, const double stepsize){
	double res=0.0;
	if(this->mean>value-stepsize*0.5 && this->mean<=value+stepsize*0.5){
		res=1.0;
	}
	return res;
}
//Direct calculation of the distribution type for checking the random- and transformation process: uniform distribution
double Fpl_Distribution_Class_Mean::direct_uniformly_dist(const double value){
	double res=0.0;
	double z_right;
	double z_left;

	//calculation of alpha and beta
	z_right=(pow(12.0,0.5)*this->std_dev+2.0*this->mean)/2.0;
	z_left=2.0*this->mean-z_right;

	//calculation of the uniform-distribution
	if(value>z_left && value<=z_right){
		res=(1.0/(z_right-z_left));
	}
	else{
		res=0.0;
	}

	return res;
}
//Direct calculation of the distribution type for checking the random- and transformation process: normal distribution
double Fpl_Distribution_Class_Mean::direct_normal_dist(const double value){
	double res=0.0;
	
	res=(1.0/(pow(2.0*constant::Cpi,0.5)*this->std_dev))* exp(-(pow((value-this->mean),2)/(2.0*pow(this->std_dev,2))));
	return res;
}
//Direct calculation of the distribution type for checking the random- and transformation process: exponential distribution
double Fpl_Distribution_Class_Mean::direct_expo_dist(const double value){
	double res=0.0;
	if(value < this->mean-this->std_dev){
		res=0.0;
	}
	else{
		res=1.0/this->std_dev*exp(-1.0/this->std_dev*(value-this->mean+this->std_dev));
	}

	return res;
}
//Direct calculation of the distribution type for checking the random- and transformation process: log-normal distribution 
double Fpl_Distribution_Class_Mean::direct_lognormal_dist(const double value){
	double res=0.0;
	double alpha;
	double beta;

	//calculation of alpha and beta
	alpha=pow(log(1.0+pow((this->std_dev/this->mean),2)),0.5);
	beta=log(this->mean/pow(exp(pow(alpha,2)),0.5));

	if(value<=0.0){//no negative or zero value for the log_normal calculation
		res=0.0;
	}
	else{
		res=1.0/(value*pow(2.0*constant::Cpi,0.5)*alpha)*exp(-pow(log(value)-beta,2)/(2*pow(alpha,2)));
	}

	return res;
}
//Direct calculation of the distribution type for checking the random- and transformation process: gumbel distribution
double Fpl_Distribution_Class_Mean::direct_gumbel_dist(const double value){
	double res=0.0;
	double alpha;
	double beta;

	//calculation of alpha and beta
	alpha=this->std_dev*pow(6.0,0.5)/constant::Cpi;
	beta=this->mean-0.5772*alpha;

	//calculation of the gumbel-distribution
	res=1.0/alpha*exp(-(value-beta)/alpha-exp(-(value-beta)/alpha));


	return res;
}
//Direct calculation of the distribution type for checking the random- and transformation process: weibull distribution
double Fpl_Distribution_Class_Mean::direct_weibull_dist(const double value){
	double res=0.0;
	double alpha;
	double beta;
	double buffer=value;
	alpha=this->mean;
	beta=this->std_dev;
	if(buffer<0.0){
		buffer=0.0;
	}

	//calculation of the weibull-distribution
	res=(alpha*beta*pow(buffer,(beta-1.0))*exp(-alpha*pow(buffer,beta)));

	return res;
}
//Set error(s)
Error Fpl_Distribution_Class_Mean::set_error(const int err_type){
		string place="Fpl_Distribution_Class_Mean::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){	
		case 0://wrong sql request
			place.append("set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Invalid database request";
			help="Check the database";
			break;
		case 1://no variable found
			place.append("set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="No distribution type is found to the random variable";
			help="Check the random variable and the distribution type";
			break;
		case 2://wrong sql request
			place.append("set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Invalid database request";
			help="Check the database";
			break;
		case 3://no variable found
			place.append("set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="No distribution type is found to the random variable";
			help="Check the random variable and the distribution type";
			break;
		case 4://do not find the end of the distribution type information in file
			place.append("set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Can not find the end of the distribution type information in file";
			help="The the distribution type information has to end with !$END_DIST_TYPE";
			type=1;
			break;
		case 5://wrong input
			place.append("find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the distribution type information of the mean-type in file";
			type=1;
			break;
		case 6://do not find all required section infos
			place.append("set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Can not find all required mean type distribution information";
			help="Check if all mean type distribution information are set in the file";
			info << "Required information:"<<endl;
			info << " " << fpl_label::key_mean_type<< endl;
			info << "Optional if calculated mean, else required:"<<endl;
			info << " " << fpl_label::key_mean_value<< endl;
			info << "Required, expect for deterministic mean-distribution type:"<<endl;
			info << " " << fpl_label::key_second_moment<< endl;
			info << "Optional information:"<<endl;
			info << " " << fpl_label::key_constant<< endl;
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
//Set warning(s)
Warning Fpl_Distribution_Class_Mean::set_warning(const int warn_type){
		string place="Fpl_Distribution_Class_Mean::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no unique result found
			place.append("set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)") ;
			reason="No unique result is found";
			reaction="First result is taken";
			help= "Check the random variable mean type distribution";
			type=2;	
			break;
		case 1://no unique result found
			place.append("set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)") ;
			reason="No unique result is found";
			reaction="First result is taken";
			help= "Check the random variable default mean type distribution";
			type=2;	
			break;
		case 2://std_dev<0 (warning)
			place.append("check_input(bool calculated_mean)") ;
			reason="No negativ standard deviation possible";
			reaction="Standard deviation is set to 0.0";
			help= "Check the variable";
			type=3;
			break;
		case 3://log norm mean <=0 (warning)
			place.append("check_input(bool calculated_mean)") ;
			reason="Mean of lognormal distribution is negative or 0";
			reaction="Mean-distribution type is set to deterministic";
			help= "Check the variable";
			type=3;
			break;
		case 4://not a mean class distribution type (warning)
			place.append("check_input(bool calculated_mean)") ;
			reason="Mean-distribution type is not available";
			reaction="Mean-distribution type is set to deterministic";
			help= "Check the variable";
			type=3;
			break;
		case 6://mean is < boundary min
			place.append("check_input(bool calculated_mean)") ;
			reason="The mean value is smaller than the minimum boundary";
			reaction="Mean value is set to minimum boundary";
			help= "Check the variable";
			info << "New mean value "<< this->boundaries.min << endl;
			type=3;
			break;
		case 7://mean is mean is > boundary max
			place.append("check_input(bool calculated_mean)") ;
			reason="The mean value is greater than the maximum boundary";
			reaction="Mean value is set to maximum boundary";
			help= "Check the variable";
			info << "New mean value "<< this->boundaries.max << endl;
			type=3;
			break;
		case 8://weibull and alpha(mean) <=0 
			place.append("check_input(bool calculated_mean)") ;
			reason="The alpha(mean) value by a weibull distribution is <= 0.0";
			reaction="Mean-distribution type is set to deterministic";
			help= "Check the variable";
			type=3;
			break;
		case 9://weibull and beta(stddev) <=0 
			place.append("check_input(bool calculated_mean)") ;
			reason="The beta(standard deviation) value by a weibull distribution is <= 0.0";
			reaction="Mean-distribution type is set to deterministic";
			help= "Check the variable";
			type=3;
			break;
		case 10://input datas can not submitted
			place.append("transfer_disttype2database(const int position, QSqlDatabase *ptr_database, const bool calculated_mean)");
			reason="Can not submit the distribution type data (mean-type) to the database";
			help="Check the database";
			type=2;
			break;
		case 11://var_koeff<0 (warning)
			place.append("check_input(bool calculated_mean)") ;
			reason="No negativ variation coefficient possible";
			reaction="Variation coefficient is set to 0.0";
			help= "Check the variable";
			type=3;
			break;
		default:
			place.append("set_warning(int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;
};
