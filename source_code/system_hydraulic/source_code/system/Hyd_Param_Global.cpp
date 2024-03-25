//#include "Hyd_Param_Global.h"
#include "Hyd_Headers_Precompiled.h"

//init static members
Tables *Hyd_Param_Global::global_param_table=NULL;

//TODO: file input, database input, erase file pathes in Models

//constructor
Hyd_Param_Global::Hyd_Param_Global(void):default_max_steps(40000), default_init_stepsize(0.0), default_max_h_change_fp(1000.0), default_max_h_change_rv(1000.0), default_min_internal_step(10.0), default_max_v_change_rv(1000.0) {

	this->param_set_number=0;
	//timesettings
	this->GlobTStart = 0.0;
	this->GlobTStep = 3600.0;
	this->GlobTNof=1;
	this->GlobNofITS = 10;
	//modelsettings
	this->GlobNofFP=0;
	this->GlobNofRV=0;
	this->coastmodel_applied=false;
	this->tempmodel_applied = false;
	//setted couplings
	this->number_div_channel=0;
	this->number_struc_coupling=0;
	this->number_dikebreak_rv2fp_coupling=0;
	this->number_dikebreak_fp2co_coupling=0;
	//material file
	this->material_file=label::not_set;
	this->obs_point_file=label::not_set;
	//solversettings
	this->GlobMaxNumSteps = this->default_max_steps;
	this->GlobMaxStepSize = 3000.0;
	this->GlobInitStepSize = this->default_init_stepsize;
	//preconditionersettings
	this->GlobPreType = ePREC_LEFT;
	this->GlobGramSchmidt = eCLASSICAL_GS;

	//syncronisation settings
	this->max_h_change_fp=this->default_max_h_change_fp;
	this->max_h_change_rv=this->default_max_h_change_rv;
	this->max_v_change_rv=this->default_max_v_change_rv;
	this->min_internal_step=this->default_min_internal_step;

	this->total_number_models=0;
	this->total_setted_couplings=0;


	this->output_flags.tecplot_1d_required=false;
	this->output_flags.tecplot_2d_required = false;
	this->output_flags.bluekenue_2d_required = false;
	this->output_flags.paraview_1d_required = true;
	this->output_flags.paraview_2d_required = true;
	this->output_flags.database_instat_required = true;
	this->output_flags.output_folder = label::not_set;

	//System settings
	this->opencl_available = false;
	
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Param_Global), _sys_system_modules::HYD_SYS);
}
//copy constructor
Hyd_Param_Global::Hyd_Param_Global(Hyd_Param_Global &par):default_max_steps(40000), default_init_stepsize(0.0), default_max_h_change_fp(1000.0), default_max_h_change_rv(1000.0),  default_min_internal_step(10.0), default_max_v_change_rv(1000.0){
	this->param_set_number=par.param_set_number;
	//timesettings
	this->GlobTStart = par.GlobTStart;
	this->GlobTStep = par.GlobTStep;
	this->GlobTNof=par.GlobTNof;
	this->GlobNofITS = par.GlobNofITS;
	//modelsettings
	this->GlobNofFP=par.GlobNofFP;
	this->GlobNofRV=par.GlobNofRV;
	this->coastmodel_applied=par.coastmodel_applied;
	this->tempmodel_applied = par.tempmodel_applied;
	//setted couplings
	this->number_div_channel=par.number_div_channel;
	this->number_struc_coupling=par.number_struc_coupling;

	this->number_dikebreak_rv2fp_coupling=par.number_dikebreak_rv2fp_coupling;
	this->number_dikebreak_fp2co_coupling=par.number_dikebreak_fp2co_coupling;
	//material file
	this->material_file=par.material_file;
	//observation point file
	this->obs_point_file=par.obs_point_file;
	//solversettings
	this->GlobMaxNumSteps = par.GlobMaxNumSteps;
	this->GlobMaxStepSize = par.GlobMaxStepSize;
	this->GlobInitStepSize = par.GlobInitStepSize;
	//preconditionersettings
	this->GlobPreType = par.GlobPreType;
	this->GlobGramSchmidt = par.GlobGramSchmidt;
	//syncronisation settings
	this->max_h_change_fp=par.max_h_change_fp;
	this->max_h_change_rv=par.max_h_change_rv;
	this->max_v_change_rv=par.max_v_change_rv;
	this->min_internal_step=par.min_internal_step;

	this->global_path=par.global_path;

	this->total_number_models=par.total_number_models;
	this->total_setted_couplings=par.total_setted_couplings;

	this->output_flags = par.output_flags;



	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Param_Global), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Param_Global::~Hyd_Param_Global(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Param_Global), _sys_system_modules::HYD_SYS);
}
//____________________
//public
//set global info per parser from file
void Hyd_Param_Global::globals_per_parser(const string globalfile){
	//set prefix for output
	ostringstream prefix;
	prefix << "GLOB> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	Hyd_Parse_Glob glob_parser;
	//output
	ostringstream cout;
	cout << "Read the global parameters from file " << globalfile << " ..."<< endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//set the file name 
	glob_parser.set_input_filename(globalfile);


	try{
		//open file
		glob_parser.open_input_file();
		//first check
		cout << "File check of the global file..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		glob_parser.check_valid_file();
		//read in the parameters
		glob_parser.parse_global_params();
	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		glob_parser.close_input_file();
		throw msg;
	}

	//close the file
	glob_parser.close_input_file();
	
	*this=glob_parser.get_global_params();
	this->calculate_total_numbers();
	this->set_global_path(globalfile);
	//add the global path to the material file
	ostringstream buff;
	buff<<this->get_global_path() << this->material_file;
	this->material_file=_Hyd_Parse_IO::insert_linux_slash2string(buff.str());
	buff.str("");
	if(this->obs_point_file!=label::not_set){
		buff<<this->get_global_path() << this->obs_point_file;
		this->obs_point_file=_Hyd_Parse_IO::insert_linux_slash2string(buff.str());
	}
	buff.str("");
	if (this->output_flags.output_folder != label::not_set) {
		buff << this->get_global_path() << this->output_flags.output_folder;
		this->output_flags.output_folder = _Hyd_Parse_IO::insert_linux_slash2string(buff.str());
	}
	else {
		
		this->output_flags.output_folder = this->global_path;
		

	}

	try{
		this->check_members();
	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Create the database table for the global data of the hydraulic system (static)
void Hyd_Param_Global::create_table(QSqlDatabase *ptr_database){
	if(Hyd_Param_Global::global_param_table==NULL){
			ostringstream cout;
			cout << "Create global hydraulic data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_sys_param;
			const int num_col=21;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}
			tab_col[0].name=hyd_label::nofset;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=hyd_label::tstart;
			tab_col[1].type=sys_label::tab_col_type_double;
			tab_col[1].default_value="0.0";

			tab_col[2].name=hyd_label::tstep;
			tab_col[2].type=sys_label::tab_col_type_double;
			tab_col[2].unsigned_flag=true;

			tab_col[3].name=hyd_label::tnof;
			tab_col[3].type=sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag=true;

			tab_col[4].name=hyd_label::nofits;
			tab_col[4].type=sys_label::tab_col_type_int;
			tab_col[4].unsigned_flag=true;

			tab_col[5].name=hyd_label::maxnumsteps;
			tab_col[5].type=sys_label::tab_col_type_int;
			tab_col[5].unsigned_flag=true;
			tab_col[5].default_value="200000";

			tab_col[6].name=hyd_label::maxstepsize;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].unsigned_flag=true;
			tab_col[6].default_value="100.0";

			tab_col[7].name=hyd_label::inistepsize;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].unsigned_flag=true;
			tab_col[7].default_value="0.0";

			tab_col[8].name=hyd_label::syn_maxchange_h_fp;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].unsigned_flag=true;
			tab_col[8].default_value="0.01";

			tab_col[9].name=hyd_label::syn_maxchange_h_rv;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].unsigned_flag=true;
			tab_col[9].default_value="0.01";


			tab_col[10].name=hyd_label::syn_min_int_tstep;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].unsigned_flag=true;
			tab_col[10].default_value="10.0";

			tab_col[11].name=hyd_label::syn_maxchange_v_rv;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].unsigned_flag=true;
			tab_col[11].default_value="0.1";

			tab_col[12].name=hyd_label::pretype;
			tab_col[12].type=sys_label::tab_col_type_string;

			tab_col[13].name=hyd_label::gramschmidt;
			tab_col[13].type=sys_label::tab_col_type_string;

			tab_col[14].name = hyd_label::output_tecplot_1d;
			tab_col[14].type = sys_label::tab_col_type_bool;
			tab_col[14].default_value = "false";

			tab_col[15].name = hyd_label::output_tecplot_2d;
			tab_col[15].type = sys_label::tab_col_type_bool;
			tab_col[15].default_value = "false";

			tab_col[16].name = hyd_label::output_bluekenue_2d;
			tab_col[16].type = sys_label::tab_col_type_bool;
			tab_col[16].default_value = "false";

			tab_col[17].name = hyd_label::output_paraview_1d;
			tab_col[17].type = sys_label::tab_col_type_bool;
			tab_col[17].default_value = "true";

			tab_col[18].name = hyd_label::output_paraview_2d;
			tab_col[18].type = sys_label::tab_col_type_bool;
			tab_col[18].default_value = "true";

			tab_col[19].name = hyd_label::output_instat_db;
			tab_col[19].type = sys_label::tab_col_type_bool;
			tab_col[19].default_value = "true";


			tab_col[20].name=label::description;
			tab_col[20].type=sys_label::tab_col_type_string;

			try{
				Hyd_Param_Global::global_param_table= new Tables();
				if(Hyd_Param_Global::global_param_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Param_Global::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Hyd_Param_Global::close_table();
				throw msg;
			}

		Hyd_Param_Global::close_table();
	}
}
//Set the database table for the global parameters: the table name and the name of the columns (static)
void Hyd_Param_Global::set_table(QSqlDatabase *ptr_database, const bool not_close){
	if(Hyd_Param_Global::global_param_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_sys_param;
		string tab_id_col[20];

		tab_id_col[0]=hyd_label::nofset;
		tab_id_col[1]=hyd_label::tstart;
		tab_id_col[2]=hyd_label::tstep;
		tab_id_col[3]=hyd_label::tnof;
		tab_id_col[4]=hyd_label::nofits;
		tab_id_col[5]=hyd_label::maxnumsteps;
		tab_id_col[6]=hyd_label::maxstepsize;
		tab_id_col[7]=hyd_label::inistepsize;
		tab_id_col[8]=hyd_label::pretype;
		tab_id_col[9]=hyd_label::gramschmidt;
		tab_id_col[10]=hyd_label::syn_maxchange_h_fp;
		tab_id_col[11]=hyd_label::syn_maxchange_h_rv;
		tab_id_col[12]=hyd_label::syn_min_int_tstep;
		tab_id_col[13]=hyd_label::syn_maxchange_v_rv;
		tab_id_col[14] = hyd_label::output_tecplot_1d;
		tab_id_col[15] = hyd_label::output_tecplot_2d;
		tab_id_col[16] = hyd_label::output_bluekenue_2d;
		tab_id_col[17] = hyd_label::output_paraview_1d;
		tab_id_col[18] = hyd_label::output_paraview_2d;
		tab_id_col[19] = hyd_label::output_instat_db;

		try{
			Hyd_Param_Global::global_param_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Param_Global::global_param_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("void Hyd_Param_Global::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			if(not_close==false){
				Hyd_Param_Global::close_table();
			}
			throw msg;
		}
	}
}
//Delete all data in the database table for the global parameters (static)
void Hyd_Param_Global::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Param_Global::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Hyd_Param_Global::global_param_table->delete_data_in_table(ptr_database);

}
//Close and delete the database table for the global parameters (static)
void Hyd_Param_Global::close_table(void){
	if(Hyd_Param_Global::global_param_table!=NULL){
		delete Hyd_Param_Global::global_param_table;
		Hyd_Param_Global::global_param_table=NULL;
	}
}
//Transfer the global parameter of a file into a database table
void Hyd_Param_Global::transfer_globals_file2database_table(QSqlDatabase *ptr_database, const string globalfile){
	//output
	ostringstream cout;
	cout << "Transfer the global parameters from a file to a database table ..."<< endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	try{
		//read in the globals per file
		this->globals_per_parser(globalfile);
		//set it to the database table
		this->input_globals2database_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
		
	}
}
//Transfer default global parameter to database table
void Hyd_Param_Global::transfer_default2database_table(QSqlDatabase *ptr_database){
	try{
		this->check_members();
		//set it to the database table
		this->input_globals2database_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the global parameters per database
void Hyd_Param_Global::globals_per_database(QSqlDatabase *ptr_database, const bool output){
	//set prefix for output
	ostringstream prefix;
	if(output==true){
		prefix << "GLOB> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	}

	//output
	ostringstream cout;
	if(output==true){
		cout << "Read the global parameters from database ..."<< endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	int number_result=0;
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Param_Global::set_table(ptr_database);
	}
	catch(Error msg){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}

	///give the table name to the query model
	model.setTable((Hyd_Param_Global::global_param_table->get_table_name()).c_str());

	if(output==true){
		//output
		cout << "Search for global parameters set " << this->param_set_number << " in the database table "  <<  global_param_table->get_table_name()<< " ..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}


    //set the query
	ostringstream filter;
	filter << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::nofset) << "=" << this->param_set_number;
	model.setFilter(filter.str().c_str());
	Data_Base::database_request(&model);
	//check the query
	if(model.lastError().isValid()){
		//error
		Error msg=set_error(7);
		ostringstream info;
		info << "Table Name      : " << Hyd_Param_Global::global_param_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		if(output==true){
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
		throw msg;
	}
	if(model.rowCount()==0){
		Error msg=set_error(8);
		ostringstream info;
		info << "Table Name      : " << Hyd_Param_Global::global_param_table->get_table_name() << endl;
		msg.make_second_info(info.str());
		if(output==true){
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
		throw msg;
	}
	else{
		number_result=model.rowCount();
	}
	//read out the results
	//time settings
	this->GlobTStart=model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::tstart)).c_str()).toDouble();
	this->GlobTStep=model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::tstep)).c_str()).toDouble();
	this->GlobTNof=model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::tnof)).c_str()).toInt();
	this->GlobNofITS=(model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::nofits)).c_str()).toInt());
	//integration settings
	this->GlobMaxNumSteps=(model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::maxnumsteps)).c_str()).toInt());
	this->GlobMaxStepSize=(model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::maxstepsize)).c_str()).toDouble());
	this->GlobInitStepSize=(model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::inistepsize)).c_str()).toDouble());
	//preconditioner settings
	this->GlobPreType=this->convert_txt2precontype((model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::pretype)).c_str()).toString().toStdString()));
	this->GlobGramSchmidt=this->convert_txt2gramschmidttype((model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::gramschmidt)).c_str()).toString().toStdString()));
	//syncron setting
	this->max_h_change_fp=(model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::syn_maxchange_h_fp)).c_str()).toDouble());
	this->max_h_change_rv=(model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::syn_maxchange_h_rv)).c_str()).toDouble());
	this->min_internal_step=(model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::syn_min_int_tstep)).c_str()).toDouble());
	this->max_v_change_rv=(model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::syn_maxchange_v_rv)).c_str()).toDouble());
	//output
	this->output_flags.tecplot_1d_required = (model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_tecplot_1d)).c_str()).toBool());
	this->output_flags.tecplot_2d_required = (model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_tecplot_2d)).c_str()).toBool());
	this->output_flags.bluekenue_2d_required = (model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_bluekenue_2d)).c_str()).toBool());
	this->output_flags.paraview_1d_required = (model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_paraview_1d)).c_str()).toBool());
	this->output_flags.paraview_2d_required = (model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_paraview_2d)).c_str()).toBool());
	this->output_flags.database_instat_required = (model.record(0).value((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_instat_db)).c_str()).toBool());

	if(output==true){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
	
};
//output the global members
void Hyd_Param_Global::output_members(void){
	// Put out specific information for the model
	ostringstream cout;
	cout << "General model information ..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//set prefix for output
	ostringstream prefix;
	prefix << "GLOB> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	
	//output
	cout << "MODEL SETTINGS"<< endl;
	cout << " Total number of Floodplain models             : " << W(7) << this->GlobNofFP << endl;
	cout << " Total number of Riverflow models              : " << W(7) << this->GlobNofRV << endl;
	cout << " Coastmodel applied                            : " << W(7) << functions::convert_boolean2string(this->coastmodel_applied)<< endl;
	cout << " Total number of models                        : " << W(7) << this->total_number_models << endl;
	cout << "SETTED COUPLINGS"<<endl;
	cout << " Total number of diversion channels (1d1d)     : " << W(7) << this->number_div_channel << endl;
	cout << " Total number of structure coupling (1d2d)     : " << W(7) << this->number_struc_coupling << endl;
	cout << " Total number of dikebreak coupling (1d2d)     : " << W(7) << this->number_dikebreak_rv2fp_coupling << endl;
	cout << " Total number of dikebreak coupling (co2d)     : " << W(7) << this->number_dikebreak_fp2co_coupling << endl;
	cout << " Total number of setted couplings              : " << W(7) << this->total_setted_couplings << endl;
	cout << "MATERIAL"<<endl;
	cout << " Materialfile                                  : " << W(7) << Hyd_Param_Global::get_print_filename(37, this->material_file.c_str()) << endl;
	cout << "TIME SETTINGS" << endl;
	cout << " Starttime of simulation                       : " << W(7) << P(2)<< FORMAT_FIXED_REAL  << this->GlobTStart << label::sec << endl;
	cout << " Length of output timestep                     : " << W(7) << P(2) << this->GlobTStep <<  label::sec << endl;
	cout << " Number of output timesteps                    : " << W(7) << this->GlobTNof << endl;
	cout << " Number of internal timesteps                  : " << W(7) << this->GlobNofITS << endl;
	cout << "SOLVER SETTINGS" << endl;
	cout << " Initial step size                             : " << W(7) << P(2) << this->GlobInitStepSize <<  label::sec <<endl;
	cout << " Maximum step size                             : " << W(7) << P(2) << this->GlobMaxStepSize <<  label::sec <<endl;
	cout << " Limit of internal steps                       : " << W(7) << P(2) << this->GlobMaxNumSteps << endl;
	cout << "PRECONDITIONER SETTINGS" <<endl;
	cout << " Preconditioning method                        : " << W(7) << this->convert_precontype2txt(this->GlobPreType) << endl;
	cout << " Gram-Schmidt scheme                           : " << W(7) << this->convert_gramschmidttype2txt(this->GlobGramSchmidt) << endl;
	cout << "SYNCRONISATION SETTINGS" <<endl;
	cout << " Maximum of waterlevel change (FP)             : " << W(7)<< P(6) << this->max_h_change_fp << label::m << endl;
	cout << " Maximum of waterlevel change (RV)             : " << W(7)<< P(6) << this->max_h_change_rv << label::m << endl;
	cout << " Maximum of explicit velocity head change (RV) : " << W(7)<< P(6) << this->max_h_change_rv << label::m << endl;
	cout << " Minimum of internal stepsize                  : " << W(7)<< P(2) << this->min_internal_step << label::sec<< endl;
	cout << "OUTPUT SETTINGS" << endl;
	cout << " Tecplot 1d output required                    : " << W(7) << P(6) << functions::convert_boolean2string(this->output_flags.tecplot_1d_required) << endl;
	cout << " Tecplot 2d output required                    : " << W(7) << P(6) << functions::convert_boolean2string(this->output_flags.tecplot_2d_required) << endl;
	cout << " BlueKenue 2d output required                  : " << W(7) << P(6) << functions::convert_boolean2string(this->output_flags.bluekenue_2d_required) << endl;
	cout << " ParaView / csv 1d output required             : " << W(7) << P(6) << functions::convert_boolean2string(this->output_flags.paraview_1d_required) << endl;
	cout << " ParaView 2d output required                   : " << W(7) << P(6) << functions::convert_boolean2string(this->output_flags.paraview_2d_required) << endl;
	cout << " Instationary database output required         : " << W(7) << P(6) << functions::convert_boolean2string(this->output_flags.database_instat_required) << endl;
	cout << " Path to outputfolder                          : " << W(7) << P(6) << this->output_flags.output_folder << endl;

	Sys_Common_Output::output_hyd->output_txt(&cout);

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
	
}
//check the mebers
void Hyd_Param_Global::check_members(void){
	//errors
	if(this->GlobTStep<=0.0){
		Error msg=this->set_error(0);
		throw msg;
	}	
	if(this->GlobTNof<=0){
		Error msg=this->set_error(1);
		throw msg;
	}
	if(GlobNofITS<=0){
		Error msg=this->set_error(2);
		throw msg;
	}

	if(this->GlobNofFP<0){
		Error msg=this->set_error(3);
		throw msg;
	}
	if(this->GlobNofRV<0){
		Error msg=this->set_error(4);
		throw msg;
	}
	if(this->GlobNofRV==0 && this->GlobNofFP==0){
		Error msg=this->set_error(5);
		throw msg;
	}
	if(this->number_div_channel>0){
		if(this->GlobNofRV<=this->number_div_channel){
			Error msg=this->set_error(6);
			throw msg;
		}
	}

	if((this->GlobNofRV==0 || this->GlobNofFP==0) && (this->number_dikebreak_rv2fp_coupling>0 || this->number_struc_coupling >0)){
		ostringstream info;
		info << "Models"<< endl;
		info << "Number of Floodplain models          : " << W(7) << this->GlobNofFP << endl;
		info << "Number of Riverflow models           : " << W(7) << this->GlobNofRV << endl;
		info << "Setted couplings"<<endl;
		info << "Number of structure coupling (RV2FP) : " << W(7) << this->number_struc_coupling << endl;
		info << "Number of dikebreak coupling (RV2FP) : " << W(7) << this->number_dikebreak_rv2fp_coupling << endl;
		Error msg=this->set_error(10);
		msg.make_second_info(info.str());
		throw msg;	
	}
	if((this->coastmodel_applied==false || this->GlobNofFP==0) && (this->number_dikebreak_fp2co_coupling>0)){
		ostringstream info;
		info << "Models"<< endl;
		info << "Number of Floodplain models          : " << W(7) << this->GlobNofFP << endl;
		info << "Coastmodel applied                   : " << W(7) << functions::convert_boolean2string(this->coastmodel_applied)<< endl;
		info << "Setted couplings"<<endl;
		info << "Number of dikebreak coupling (CO2FP) : " << W(7) << this->number_dikebreak_fp2co_coupling << endl;
		Error msg=this->set_error(10);
		msg.make_second_info(info.str());
		throw msg;	
	}

	//warnings
	if(this->GlobMaxNumSteps<=0){
		Warning msg=this->set_warning(2);
		stringstream info;
		info << "Default value: " << this->default_max_steps << label::no_unit << endl;
		msg.make_second_info(info.str());
		//reaction
		this->GlobMaxNumSteps=this->default_max_steps;
		msg.output_msg(2);

	}
	if(this->GlobMaxStepSize<=0.0){
		Warning msg=this->set_warning(3);
		double new_max=0.0;
		new_max=this->GlobTStep/(double)this->GlobNofITS;
		stringstream info;
		info << "Used value: " << new_max << label::sec << endl;
		msg.make_second_info(info.str());
		//reaction
		this->GlobMaxStepSize=new_max;
		msg.output_msg(2);
	}
	if(this->GlobInitStepSize<0.0){
		Warning msg=this->set_warning(4);
		stringstream info;
		info << "Default value: " << this->default_init_stepsize << label::sec <<endl;
		msg.make_second_info(info.str());
		//reaction
		this->GlobInitStepSize=this->default_init_stepsize;
		msg.output_msg(2);
	}	
	if(this->GlobInitStepSize>=this->GlobTStep/(double)this->GlobNofITS){
		Warning msg=this->set_warning(5);
		stringstream info;
		info << "Default value: " << this->default_init_stepsize << label::sec <<endl;
		msg.make_second_info(info.str());
		//reaction
		this->GlobInitStepSize=this->default_init_stepsize;
		msg.output_msg(2);
	}
	if(this->max_h_change_fp<=0.0){
		Warning msg=this->set_warning(6);
		stringstream info;
		info << "Default value: " << this->default_max_h_change_fp << label::m <<endl;
		msg.make_second_info(info.str());
		//reaction
		this->max_h_change_fp=this->default_max_h_change_fp;
		msg.output_msg(2);
	}
	if(this->max_h_change_rv<=0.0){
		Warning msg=this->set_warning(8);
		stringstream info;
		info << "Default value: " << this->default_max_h_change_rv << label::m <<endl;
		msg.make_second_info(info.str());
		//reaction
		this->max_h_change_rv=this->default_max_h_change_rv;
		msg.output_msg(2);
	}
	if(this->min_internal_step<=0.0){
		Warning msg=this->set_warning(9);
		stringstream info;
		info << "Default value: " << this->default_min_internal_step << label::m <<endl;
		msg.make_second_info(info.str());
		//reaction
		this->min_internal_step=this->default_min_internal_step;
		msg.output_msg(2);
	}
	if(this->max_v_change_rv<=0.0){
		Warning msg=this->set_warning(10);
		stringstream info;
		info << "Default value: " << this->default_max_v_change_rv << label::m <<endl;
		msg.make_second_info(info.str());
		//reaction
		this->max_v_change_rv=this->default_max_v_change_rv;
		msg.output_msg(2);
	}
	if (Sys_Project::get_project_type() != _sys_project_type::proj_hyd_temp && this->tempmodel_applied == true) {
		Warning msg = this->set_warning(11);
		stringstream info;
		info << "Project type: " << Sys_Project::convert_project_type2txt(Sys_Project::get_project_type()) << endl;
		msg.make_second_info(info.str());
		//reaction
		this->tempmodel_applied = false;
		msg.output_msg(2);

	}

}
//(static) Filename conversion functions
string Hyd_Param_Global::get_print_filename(const int MaxLength, const char *FName){
	string FNameTmp;
	int Pos=0;
	
	FNameTmp.insert(0, FName);

	if ((signed)FNameTmp.length() >= (MaxLength - 4))
	{
		Pos = FNameTmp.find_last_of((char)92, FNameTmp.length());
		if (Pos == string::npos || Pos < 0) Pos = FNameTmp.find_last_of((char)47, FNameTmp.length());
		if (Pos == string::npos || Pos < 0) Pos = 0;
		else Pos++;
		FNameTmp.erase(0, Pos);
		if (FNameTmp.length() > (unsigned)(MaxLength -4)) FNameTmp.erase(0, FNameTmp.length()-MaxLength);
		FNameTmp.insert(0, ".../");
	}
	return FNameTmp;
}
//Get the start time
double Hyd_Param_Global::get_startime(void){
	return this->GlobTStart;
}
//Get the stepsize
double Hyd_Param_Global::get_stepsize(void){
	return this->GlobTStep;
}
//Get number of timesteps
double Hyd_Param_Global::get_number_timesteps(void){
	return this->GlobTNof;
}
//Get the total number of models (river, floodplian and coast-model)
int Hyd_Param_Global::get_total_number_models(void){
	return this->total_number_models;
}
//Get the total number of setted couplings (dikebreak, diversion channel, structure coupling)
int Hyd_Param_Global::get_total_number_setted_couplings(void){
	return this->total_setted_couplings;
}
//Get the number of floodplain models in the system
int Hyd_Param_Global::get_number_floodplain_model(void){
	return this->GlobNofFP;
}
//Get the number of river models in the system
int Hyd_Param_Global::get_number_river_model(void){
	return this->GlobNofRV;
}
//Get the flag if the coast model is applied in the system
bool Hyd_Param_Global::get_coast_model_applied(void){
	return this->coastmodel_applied;
}
//Get the output flags which output is required
_hyd_output_flags Hyd_Param_Global::get_output_flags(void) {
	return this->output_flags;
}
//Set the output flags which output is required
void Hyd_Param_Global::set_opencl_available(bool boolean_flag) {
	this->opencl_available = boolean_flag;
}
//Get the output flags which output is required
bool Hyd_Param_Global::get_opencl_available(void) {
	return this->opencl_available;
}
//Set the number of river-models
void Hyd_Param_Global::set_number_river_models(const int number){
	this->GlobNofRV=number;
}
//Set the number of floodplain-models
void Hyd_Param_Global::set_number_floodplain_models(const int number){
	this->GlobNofFP=number;
}
//Set if the coast-model is applied
void Hyd_Param_Global::set_coast_model_applied(const bool applied){
	this->coastmodel_applied=applied;
}
//Set the number of diversion-channels
void Hyd_Param_Global::set_number_diversion_channels(const int number){
	this->number_div_channel=number;
}
//Set the number of the coupling between river- and floodplain-models via a dikebreak/wallbreak
void Hyd_Param_Global::set_number_rv2fp_dikebreaks(const int number){
	this->number_dikebreak_rv2fp_coupling=number;
}
//Set the number of the coupling between coast- and floodplain-models via a dikebreak/wallbreak
void Hyd_Param_Global::set_number_fp2co_dikebreaks(const int number){
	this->number_dikebreak_fp2co_coupling=number;
}
//Set the number of the coupling between river- and floodplain-models via a hydraulic structure
void Hyd_Param_Global::set_number_hydraulic_structure_coupling(const int number){
	this->number_struc_coupling=number;
}
//Get the global path
string Hyd_Param_Global::get_global_path(void){
	return this->global_path;
}
//Total reset of the global parameters
void Hyd_Param_Global::total_reset(void){
	this->param_set_number=0;
	//timesettings
	this->GlobTStart = 0.0;
	this->GlobTStep = 3600.0;
	this->GlobTNof=0;
	this->GlobNofITS = 10;
	//modelsettings
	this->GlobNofFP=0;
	this->GlobNofRV=0;
	this->coastmodel_applied=false;
	//setted couplings
	this->number_div_channel=0;
	this->number_struc_coupling=0;
	this->number_dikebreak_rv2fp_coupling=0;
	this->number_dikebreak_fp2co_coupling=0;
	//material file
	this->material_file=label::not_set;
	//solversettings
	this->GlobMaxNumSteps = this->default_max_steps;
	this->GlobMaxStepSize = 100.0;
	this->GlobInitStepSize = this->default_init_stepsize;
	//preconditionersettings
	this->GlobPreType = ePREC_LEFT;
	this->GlobGramSchmidt = eCLASSICAL_GS;

	//syncronisation settings
	this->max_h_change_fp=this->default_max_h_change_fp;
	this->max_h_change_rv=this->default_max_h_change_rv;
	this->max_v_change_rv=this->default_max_v_change_rv;
	this->min_internal_step=this->default_min_internal_step;

}
//copy operator
Hyd_Param_Global& Hyd_Param_Global::operator= (const Hyd_Param_Global& par){
	this->param_set_number=par.param_set_number;
	//timesettings
	this->GlobTStart = par.GlobTStart;
	this->GlobTStep = par.GlobTStep;
	this->GlobTNof=par.GlobTNof;
	this->GlobNofITS = par.GlobNofITS;
	//modelsettings
	this->GlobNofFP=par.GlobNofFP;
	this->GlobNofRV=par.GlobNofRV;
	this->coastmodel_applied=par.coastmodel_applied;
	this->tempmodel_applied = par.tempmodel_applied;
	//setted couplings
	this->number_div_channel=par.number_div_channel;
	this->number_struc_coupling=par.number_struc_coupling;
	this->number_dikebreak_rv2fp_coupling=par.number_dikebreak_rv2fp_coupling;
	this->number_dikebreak_fp2co_coupling=par.number_dikebreak_fp2co_coupling;
	//material file
	this->material_file=par.material_file;
	//observation point file
	this->obs_point_file=par.obs_point_file;
	//solversettings
	this->GlobMaxNumSteps = par.GlobMaxNumSteps;
	this->GlobMaxStepSize = par.GlobMaxStepSize;
	this->GlobInitStepSize = par.GlobInitStepSize;
	//preconditionersettings
	this->GlobPreType = par.GlobPreType;
	this->GlobGramSchmidt = par.GlobGramSchmidt;
	//syncronisation settings
	this->max_h_change_fp=par.max_h_change_fp;
	this->max_h_change_rv=par.max_h_change_rv;
	this->max_v_change_rv=par.max_v_change_rv;
	this->min_internal_step=par.min_internal_step;

	this->global_path=par.global_path;

	this->total_number_models=par.total_number_models;
	this->total_setted_couplings=par.total_setted_couplings;

	//output settings
	this->output_flags = par.output_flags;


	return *this;
}
//______________
//private
//Calculate the total number of models and the total number of setted coupling
void Hyd_Param_Global::calculate_total_numbers(void){
	//models
	this->total_number_models=this->GlobNofFP+this->GlobNofRV;
	if(this->coastmodel_applied==true){
		this->total_number_models++;
	}
	//setted couplings
	this->total_setted_couplings=this->number_div_channel+this->number_struc_coupling+this->number_dikebreak_rv2fp_coupling+this->number_dikebreak_fp2co_coupling;

}
//Convert the Preconditioner type enumerator from text to enum
_hyd_prec_type Hyd_Param_Global::convert_txt2precontype(string txt){
	_hyd_prec_type type;
	_Hyd_Parse_IO::string2lower(&txt);
	_Hyd_Parse_IO::erase_carriageReturn(&txt);
	_Hyd_Parse_IO::erase_leading_whitespace_tabs(&txt);
	_Hyd_Parse_IO::erase_end_whitespace_tabs(&txt);

	if(txt==hyd_label::precon_left_type){
		type=_hyd_prec_type::ePREC_LEFT;
	}
	else if(txt==hyd_label::precon_right_type){
		type=_hyd_prec_type::ePREC_RIGHT;
	}
	else{
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Try to convert text: " << txt << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
		//reaction
		type=_hyd_prec_type::ePREC_LEFT;
	}
	return type;
}
//Convert the Preconditioner type enumerator from enum to txt 
string Hyd_Param_Global::convert_precontype2txt(_hyd_prec_type type){
	string txt;	
	switch(type){
		case _hyd_prec_type::ePREC_LEFT:
			txt=hyd_label::precon_left_type;
			break;
		case _hyd_prec_type::ePREC_RIGHT:
			txt=hyd_label::precon_right_type;
			break;
		default:
			txt=label::unknown_type;
	}
	return txt;
}
//Convert the Gramschmidt type enumerator from text to enum
_hyd_gs_scheme_type Hyd_Param_Global::convert_txt2gramschmidttype(string txt){
	_hyd_gs_scheme_type type;
	_Hyd_Parse_IO::string2lower(&txt);
	_Hyd_Parse_IO::erase_carriageReturn(&txt);
	_Hyd_Parse_IO::erase_leading_whitespace_tabs(&txt);
	_Hyd_Parse_IO::erase_end_whitespace_tabs(&txt);

	if(txt==hyd_label::gs_classical_type){
		type=_hyd_gs_scheme_type::eCLASSICAL_GS;
	}
	else if(txt==hyd_label::gs_modified_type){
		type=_hyd_gs_scheme_type::eMODIFIED_GS;
	}
	else{
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Try to convert text: " << txt << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
		//reaction
		type=_hyd_gs_scheme_type::eCLASSICAL_GS;
	}
	return type;
}
//Convert the Gramschmidt type enumerator from enum to txt 
string Hyd_Param_Global::convert_gramschmidttype2txt(_hyd_gs_scheme_type  type){
	string txt;	
	switch(type){
		case _hyd_gs_scheme_type::eCLASSICAL_GS:
			txt=hyd_label::gs_classical_type;
			break;
		case _hyd_gs_scheme_type::eMODIFIED_GS:
			txt=hyd_label::gs_modified_type;
			break;
		default:
			txt=label::unknown_type;
	}
	return txt;
}
//Set the global path
void Hyd_Param_Global::set_global_path(string filename){
	int pos1=-1;
	int pos2=-1;
	pos1=filename.find_last_of("/");
	pos2=filename.find_last_of('\\');
	if(pos1>=0 && pos2<0){
		this->global_path=filename.substr(0, pos1+1);

	}
	else if(pos1<0 && pos2>=0){
		this->global_path=filename.substr(0, pos2+1);
	}
	else if(pos1>=0 && pos2>=0){
		if(pos1>pos2){
			this->global_path=filename.substr(0, pos1+1);
		}
		else{
			this->global_path=filename.substr(0, pos2+1);
		}

	}



}
//Input the global parameters to a database table
void Hyd_Param_Global::input_globals2database_table(QSqlDatabase *ptr_database, const bool output){
	//set prefix for output
	if(output==true){
		ostringstream prefix;
		prefix << "GLOB> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		//output
		ostringstream cout;
		cout << "Transfer the global parameters to the database table ..."<< endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Param_Global::set_table(ptr_database);
	}
	catch(Error msg){
		//rewind the prefix
		if(output==true){
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
		throw msg;
	}

	///give the table name to the query model
	model.setTable((Hyd_Param_Global::global_param_table->get_table_name()).c_str());

	//insert data
	model.insertRows(0, 1);
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::nofset)).c_str())),0);
	//time settings
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::tstart)).c_str())),this->GlobTStart);
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::tstep)).c_str())),this->GlobTStep);
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::tnof)).c_str())),this->GlobTNof);
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::nofits)).c_str())),this->GlobNofITS);
	//integration settings
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::maxnumsteps)).c_str())),this->GlobMaxNumSteps);
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::maxstepsize)).c_str())),this->GlobMaxStepSize);
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::inistepsize)).c_str())),this->GlobInitStepSize);
	//preconditioner settings
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::pretype)).c_str())),this->convert_precontype2txt(this->GlobPreType).c_str());
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::gramschmidt)).c_str())),this->convert_gramschmidttype2txt(this->GlobGramSchmidt).c_str());
	//syncron settings
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::syn_maxchange_h_fp)).c_str())),this->max_h_change_fp);
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::syn_maxchange_h_rv)).c_str())),this->max_h_change_rv);
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::syn_min_int_tstep)).c_str())),this->min_internal_step);
	model.setData(model.index(0,model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::syn_maxchange_v_rv)).c_str())),this->max_v_change_rv);
	//output
	model.setData(model.index(0, model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_tecplot_1d)).c_str())), functions::convert_boolean2string(this->output_flags.tecplot_1d_required).c_str());
	model.setData(model.index(0, model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_tecplot_2d)).c_str())), functions::convert_boolean2string(this->output_flags.tecplot_2d_required).c_str());
	model.setData(model.index(0, model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_paraview_1d)).c_str())), functions::convert_boolean2string(this->output_flags.paraview_1d_required).c_str());
	model.setData(model.index(0, model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_paraview_2d)).c_str())), functions::convert_boolean2string(this->output_flags.paraview_2d_required).c_str());
	model.setData(model.index(0, model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_instat_db)).c_str())), functions::convert_boolean2string(this->output_flags.database_instat_required).c_str());
	model.setData(model.index(0, model.record().indexOf((Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_bluekenue_2d)).c_str())), functions::convert_boolean2string(this->output_flags.bluekenue_2d_required).c_str());

	
	
	//submit it to the datbase
	Data_Base::database_submit(&model);
	if(model.lastError().isValid()){
		Error msg=set_error(9);
		ostringstream info;
		info << "Table Name      : " << Hyd_Param_Global::global_param_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		if(output==true){
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
		throw (msg);	
	}

	if(output==true){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}

}
//Set the warning
Warning Hyd_Param_Global::set_warning(const int warn_type){
	string place="Hyd_Param_Global::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://Type of preconditioner type is unknown
			place.append("convert_txt_precontype(const string txt)") ;
			reason="Preconditioning type is unknown";
			reaction="Preconditioning type PREC_LEFT is used";
			help= "Check the given preconditioning type";
			type=1;	
			break;
		case 1://Type of preconditioner scheme is unknown
			place.append("convert_txt_gramschmidttype(const string txt)") ;
			reason="Type of preconditioner scheme is unknown";
			reaction="Type of preconditioner scheme CLASSICAL_GS is used";
			help= "Check the given type of preconditioner scheme";
			type=1;	
			break;
		case 2://Stepnumber <=0
			place.append("check_members(void)") ;
			reason="Maximum number of steps for the solver is <=0";
			reaction="The default value is used";
			help= "Check the maximum number of solver steps";
			type=3;	
			break;
		case 3://Maximum stepsize is <=0.0
			place.append("check_members(void)") ;
			reason="Maximum stepsize for the solver is <=0.0";
			reaction="Global stepsize divided by number of internal step is used";
			help= "Check the maximum stepsize";
			type=3;	
			break;
		case 4://Minimum stepsize is <=0.0
			place.append("check_members(void)") ;
			reason="Minimum stepsize for the solver is <0.0";
			reaction="Default minimum stepsize is used";
			help= "Check the minimum stepsize";
			type=3;	
			break;
		case 5://init stepsize is > than one internal step
			place.append("check_members(void)") ;
			reason="The initial stepsize is greater than the global stepsize divided by number of internal timesteps ";
			reaction="Default minimum stepsize is used";
			help= "Check the minimum stepsize";
			type=3;	
			break;
		case 6://maximum h change is smaller or equal than 0
			place.append("check_members(void)") ;
			reason="The maximum of h change in the floodplain elements per syncronisation timestep is smaller or equal than 0.0";
			reaction="Default maximum of h change in the floodplain elements per syncronisation timestep is used";
			help= "Check the maximum of h change in the floodplain elements per syncronisation timestep";
			type=3;	
			break;
		case 7://maximum dh change is smaller or equal than 0
			place.append("check_members(void)") ;
			reason="The maximum of dh change for model coupling (FP2FP) is smaller or equal than 0.0";
			reaction="Default maximum of dh change for model coupling (FP2FP) is used";
			help= "Check the maximum of dh change for model coupling (FP2FP)";
			type=3;	
			break;
		case 8://maximum h change is smaller or equal than 0
			place.append("check_members(void)") ;
			reason="The maximum of h change in the river elements per syncronisation timestep is smaller or equal than 0.0";
			reaction="Default maximum of h change in the river elements per syncronisation timestep is used";
			help= "Check the maximum of h change in the river elements per syncronisation timestep";
			type=3;	
			break;
		case 9://minimum internal stepsize is smaller or equal than 0
			place.append("check_members(void)") ;
			reason="The minimum internal stepsize per syncronisation timestep is smaller or equal than 0.0";
			reaction="Default minimum internal stepsize per syncronisation timestep is used";
			help= "Check the minimum internal stepsize per syncronisation timestep";
			type=3;	
			break;
		case 10://maximum velocity head change is smaller or equal than 0
			place.append("check_members(void)") ;
			reason="The maximum of explicit velocity head change in the river models per syncronisation timestep is smaller or equal than 0.0";
			reaction="Default maximum of explicit velocity head change per syncronisation timestep is used";
			help= "Check the maximum of explicit velocity head change per syncronisation timestep";
			type=3;	
			break;
		case 11://temperature model applied but not the right projetc type
			place.append("check_members(void)");
			reason = "The flag for applying the temperature model ist set to true; however with the given project type no temperature model is applicable";
			reaction = "The applied flag of the temperature model is set to false";
			help = "Check the project type and the applied flag of the temperature model in the *.ilm-file";
			type = 3;
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
//set the error
Error Hyd_Param_Global::set_error(const int err_type){
		string place="Hyd_Param_Global::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://timestep is <=0.0
			place.append("check_members(void)");
			reason="Global timestepsize is <=0.0";
			help="Check the global timestepsize";
			type=3;
			break;
		case 1://number of global timesteps <=0
			place.append("check_members(void)");
			reason="Number of global timesteps is <=0";
			help="Check the number of global timesteps";
			type=3;
			break;
		case 2://number of internal timesteps <=0
			place.append("check_members(void)");
			reason="Number of internal timesteps is <=0";
			help="Check the number of internal timesteps";
			type=3;
			break;
		case 3://number of floodplainmodels <0
			place.append("check_members(void)");
			reason="Number of floodplainmodel is <0";
			help="Check the number of floodplainmodel";
			type=3;
			break;
		case 4://number of rivermodels <0
			place.append("check_members(void)");
			reason="Number of rivermodel is <0";
			help="Check the number of rivermodel";
			type=3;
			break;
		case 5://number of rivermodels and floodplainmodel==0
			place.append("check_members(void)");
			reason="No model for calculation is set";
			help="Check the number of models";
			type=3;
			break;
		case 6://number of rivermodels must be greater than number of diversion channels
			place.append("check_members(void)");
			reason="Number of rivermodel have to be > number of diversion channels; they are just a part of the rivermodels";
			help="Check the number of rivermodels and diversion channels";
			type=3;
			break;
		case 7://problems with database query
			place.append("globals_per_database(QSqlDatabase *ptr_database)") ;
			reason="Can not submit the query to the database";
			help="Check the query error";
			type=2;	
			break;
		case 8://no matching parameter set is found in the table
			place.append("globals_per_database(QSqlDatabase *ptr_database)") ;
			reason="There is no matching set of global parameters in the table";
			help="Check the number global parameter table";
			info <<"Set number : " << this->param_set_number << endl;
			type=2;	
			break;
		case 9://input datas can not submitted
			place.append("input_globals2database_table(QSqlDatabase *ptr_database)");
			reason="Can not submit the read in global input data to the database";
			help="Check the database";
			type=2;
			fatal=true;//fatal error;
			break;
		case 10://number of models
			place.append("check_members(void)");
			reason="The model and setted coupling combination is not possible";
			help="Check the number of models";
			type=3;
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
