#include "Dam_Headers_Precompiled.h"
//#include "Dam_Damage_System.h"

//init static members
bool Dam_Damage_System::abort_thread_flag=false;
bool Dam_Damage_System::hyd_thread_runs=false;
Tables *Dam_Damage_System::erg_table=NULL;
Tables *Dam_Damage_System::system_table = NULL;

//Default constructor
Dam_Damage_System::Dam_Damage_System(void){
	this->thread_type=_dam_thread_type::dam_thread_unknown;

	this->number_error=0;
	this->number_warning_begin=0;
	this->number_warning=0;
	this->number_floodplain_impact=0;
	this->impact_floodplain=NULL;
	this->impact_floodplain_set=false;
	this->specific_raster_type=_dam_raster_types::all_raster;

	this->break_sz="CA";
	this->number_break_sc=0;
	this->number_break_sc2calc=0;
	this->ptr_break_sc_data=NULL;

	this->number_hyd_warnings=0;
	this->number_hyd_errors=0;
	this->thread_is_started=false;

	this->polygons=NULL;
	this->number_polygons=0;

	this->hyd_base_sc_import=false;

	this->multi_fp_hyd_raster_import_dia=NULL;
	this->number_hyd_imp_dia=0;

	this->system_param.max_timesteps = 1000;
	this->system_param.until_all_active = true;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Damage_System), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Damage_System::~Dam_Damage_System(void){
	this->delete_impact_floodplain();
	this->delete_polygons();
	this->delete_hyd_res_import_dia_multi_fp();

	if(this->qsqldatabase.isOpen()){
		this->qsqldatabase.close();
	}

	this->set_stop_thread_flag(false);
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Damage_System), _sys_system_modules::DAM_SYS);
}
//________________
//public
//Output the members
void Dam_Damage_System::output_member(void){
	ostringstream prefix;
	prefix << "OUT>";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	//economical system
	this->ecn_sys.output_members();
	//ecological system
	this->eco_sys.output_members();
	//people system
	this->people_sys.output_members();
	//psycho-social
	this->pys_sys.output_members();
	//simple-counting
	this->sc_sys.output_members();
	//CI
	this->ci_sys.output_members();

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Connect via interception the raster with the raster of the same damage type and the hydraulic raster
void Dam_Damage_System::connect_damage_raster(void){
/**
There are two types of interception requiered. The first interception is between the raster of the
same damage type. There the area of intercepted elements will be reduced to avoid multi counting of the damages.
The second interception is between the damage raster and the floodplain raster (Hyd_Model_Floodplain) to establish
a connection between the hydraulic data and the damage data.
\see Hyd_Floodplain_Raster
*/
	ostringstream prefix;
	prefix << "CON> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	emit send_hyd_thread_enable(true);

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);

	//needed for interception with the damage system
	Hyd_Hydraulic_System my_hyd;
	my_hyd.set_systemid(this->system_id);

	try{
		ostringstream cout;
		cout << "Reset the damage raster elements of each damage type..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
		Dam_Damage_System::reset_raster_hyd_connection_elements(&this->qsqldatabase, this->specific_raster_type);
		

		cout << "Intercept the damage raster of each damage type among each other..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//ecn
		//read it in
		if(this->specific_raster_type==_dam_raster_types::all_raster || this->specific_raster_type==_dam_raster_types::ecn_total){
			this->ecn_sys.read_damage_raster_per_database_general(&this->qsqldatabase);
			Dam_Damage_System::check_stop_thread_flag();
			//intercept it and reduce area
			this->ecn_sys.reduce_area_intercepted_elems(&this->qsqldatabase, this->system_id);
			Dam_Damage_System::check_stop_thread_flag();
		}

		//eco
		//read biotope-type raster in
		if(this->specific_raster_type==_dam_raster_types::all_raster || this->specific_raster_type==_dam_raster_types::eco_all){
			this->eco_sys.read_biotope_raster_per_database_general(&this->qsqldatabase);
			Dam_Damage_System::check_stop_thread_flag();
			//read soil-erosion raster in
			this->eco_sys.read_soil_type_erosion_raster_per_database_general(&this->qsqldatabase);
			Dam_Damage_System::check_stop_thread_flag();
			//intercept it and reduce area
			this->eco_sys.reduce_area_intercepted_elems(&this->qsqldatabase, this->system_id);
			Dam_Damage_System::check_stop_thread_flag();
		}

		//pop
		//read it in
		if(this->specific_raster_type==_dam_raster_types::all_raster || this->specific_raster_type==_dam_raster_types::pop_total){
			this->people_sys.read_people2risk_raster_per_database_general(&this->qsqldatabase);
			Dam_Damage_System::check_stop_thread_flag();
			//intercept it and reduce area
			this->people_sys.reduce_area_intercepted_elems(&this->qsqldatabase, this->system_id);
			Dam_Damage_System::check_stop_thread_flag();
		}

		//pys
		//read it in
		if(this->specific_raster_type==_dam_raster_types::all_raster || this->specific_raster_type==_dam_raster_types::pys_pys_total){
			this->pys_sys.read_damage_raster_per_database_general(&this->qsqldatabase);
			Dam_Damage_System::check_stop_thread_flag();
			//intercept it and reduce area
			this->pys_sys.reduce_area_intercepted_elems(&this->qsqldatabase, this->system_id);
			Dam_Damage_System::check_stop_thread_flag();
		}

		//interception with the hydraulic system
		emit send_hyd_thread_runs(true);
		Dam_Damage_System::hyd_thread_runs=true;
		cout << "Intercept the DAM-raster with the HYD-raster..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//init the hydraulic system
		cout << "Initialize the HYD-system for interception. Further information see HYD display..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
		Sys_Common_Output::output_hyd->set_focus_on_display();
		prefix << "DAM> ";
		Sys_Common_Output::output_hyd->set_userprefix(&prefix);
		cout << "Initialize the HYD-system for interception with the DAM-system..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		emit send_dam_thread_sleeps(true);

		bool hyd_is_ok=true;
		try{
			my_hyd.set_ptr2database(this->qsqldatabase,0);
			if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
				my_hyd.set_2Dsystem_per_database(true);
			}
			else{
				my_hyd.set_system_per_database(true);
				my_hyd.init_models(true);
			}

			my_hyd.output_final_error_warning_number();
		}
		catch(Error msg){
			hyd_is_ok=false;
			msg.output_msg(2);
			my_hyd.output_final_error_warning_number();
			throw msg;
		}

		emit send_dam_thread_sleeps(false);
		cout << "Initialization of the HYD-system for interception with the DAM-system is finished" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		Sys_Common_Output::output_dam->set_focus_on_display();
		if(hyd_is_ok==true){
			//ecn raster
			if(this->specific_raster_type==_dam_raster_types::all_raster || this->specific_raster_type==_dam_raster_types::ecn_total){
				this->ecn_sys.intercept_hydraulic2damage(my_hyd.my_fpmodels, my_hyd.global_parameters.get_number_floodplain_model(), &this->qsqldatabase, this->system_id);
				Dam_Damage_System::check_stop_thread_flag();
			}
			//eco
			if(this->specific_raster_type==_dam_raster_types::all_raster || this->specific_raster_type==_dam_raster_types::eco_all){
				this->eco_sys.intercept_hydraulic2damage(my_hyd.my_fpmodels, my_hyd.global_parameters.get_number_floodplain_model(), &this->qsqldatabase, this->system_id);
				Dam_Damage_System::check_stop_thread_flag();
			}
			//pop
			if(this->specific_raster_type==_dam_raster_types::all_raster || this->specific_raster_type==_dam_raster_types::pop_total){
				this->people_sys.intercept_hydraulic2damage(my_hyd.my_fpmodels, my_hyd.global_parameters.get_number_floodplain_model(), &this->qsqldatabase, this->system_id);
				Dam_Damage_System::check_stop_thread_flag();
			}
			//pys
			if(this->specific_raster_type==_dam_raster_types::all_raster || this->specific_raster_type==_dam_raster_types::pys_pys_total){
				this->pys_sys.intercept_hydraulic2damage(my_hyd.my_fpmodels, my_hyd.global_parameters.get_number_floodplain_model(), &this->qsqldatabase, this->system_id);
				Dam_Damage_System::check_stop_thread_flag();
			}
			//sc
			if(this->specific_raster_type==_dam_raster_types::all_raster || this->specific_raster_type==_dam_raster_types::sc_sc_total){
				this->sc_sys.intercept_hydraulic2damage(my_hyd.my_fpmodels, my_hyd.global_parameters.get_number_floodplain_model(), &this->qsqldatabase, this->system_id);
				Dam_Damage_System::check_stop_thread_flag();
			}
			//ci 
			if (this->specific_raster_type == _dam_raster_types::all_raster || this->specific_raster_type == _dam_raster_types::ci_ci_total) {
				this->ci_sys.intercept_hydraulic2damage(my_hyd.my_fpmodels, my_hyd.global_parameters.get_number_floodplain_model(), &this->qsqldatabase, this->system_id);
				Dam_Damage_System::check_stop_thread_flag();
			}
		}
		emit send_hyd_thread_runs(false);
		Dam_Damage_System::hyd_thread_runs=false;
		cout << "Interception of the DAM-system with the HYD-system is finished" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		cout << "Interception with HYD-system is finished" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
		Sys_Common_Output::output_hyd->reset_userprefix();
	}
	catch(Error msg){
		if(Dam_Damage_System::get_stop_thread_flag()==false){
			this->number_error++;
		}
		msg.output_msg(4);
	}
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);

	this->number_hyd_warnings=my_hyd.get_number_warnings();
	this->number_hyd_errors=my_hyd.get_number_errors();

	this->output_error_time_statistic();
	emit send_hyd_thread_enable(false);
	emit send_hyd_thread_runs(false);

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Run the thread; the type of thread, which will be performed, has to be set before with set_thread_type(_dam_thread_type type)
void Dam_Damage_System::run(void){
/** \see set_thread_type(_dam_thread_type type), _dam_thread_type
*/
	this->thread_is_started=true;
	try{
		switch(this->thread_type){
			//import hyd-results
			case(_dam_thread_type::dam_imp_hyd_res_raster_base_sc):
				this->import_multi_fp_hyd_res_raster_file2database();
				break;
			case(_dam_thread_type::dam_add_hyd_res_raster_sc):
				this->import_multi_fp_hyd_res_raster_file2database();
				break;
			//database tables
			case(_dam_thread_type::dam_create_tab):
				this->create_dam_database_tables();
				break;
			case(_dam_thread_type::dam_check_tab):
				this->check_dam_database_tables();
				break;
			//ecn
			case(_dam_thread_type::dam_imp_ecn_dam_func):
				this->ecn_sys.import_damage_func_file2database(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_add_ecn_dam_raster):
				this->ecn_sys.add_damage_raster_file2database_multi(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_del_ecn):
				this->ecn_sys.del_damage_data_database(&this->qsqldatabase);
				break;
			//eco
			case(_dam_thread_type::dam_imp_eco_b_type):
				this->eco_sys.import_biotope_types_file2database(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_add_eco_b_type_raster):
				this->eco_sys.add_biotope_raster_file2database_multi(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_del_eco_btype):
				this->eco_sys.del_biotope_data_database(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_imp_eco_soil_cost):
				this->eco_sys.import_soil_type_cost_function_file2database(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_imp_eco_soil_ero):
				this->eco_sys.import_soil_type_erosion_function_file2database(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_add_eco_soil_raster):
				this->eco_sys.add_soil_type_erosion_raster_file2database_multi(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_del_eco_soil):
				this->eco_sys.del_soil_erosion_data_database(&this->qsqldatabase);
				break;
			//pop
			case(_dam_thread_type::dam_imp_pop_dam_func):
				this->people_sys.import_damage_func_file2database(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_add_pop_raster):
				this->people_sys.add_people2risk_raster_file2database_multi(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_del_pop):
				this->people_sys.del_damage_data_database(&this->qsqldatabase);
				break;
			//sc
			case(_dam_thread_type::dam_imp_sc_subcat):
				this->sc_sys.sc_subcategory_file2database(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_imp_sc_point):
				this->sc_sys.add_damage_points_file2database(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_del_sc):
				this->sc_sys.del_damage_data_database(&this->qsqldatabase);
				break;
			//CI
			case(_dam_thread_type::dam_imp_ci_data):
				this->ci_sys.ci_data_file2database(&this->qsqldatabase,this->system_id);

				break;
			case(_dam_thread_type::dam_del_ci):
				this->ci_sys.del_ci_data_database(&this->qsqldatabase);
				break;

			//pys
			case(_dam_thread_type::dam_imp_pys_category):
				this->pys_sys.import_categories_file2database(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_imp_pys_dens_func):
				this->pys_sys.import_density_function_file2database(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_add_pys_raster):
				this->pys_sys.add_damage_raster_file2database_multi(&this->qsqldatabase);
				break;
			case(_dam_thread_type::dam_del_pys):
				this->pys_sys.del_damage_data_database(&this->qsqldatabase);
				break;
			//sys
			case(_dam_thread_type::dam_sys_connect):
				this->connect_damage_raster();
				break;
			case(_dam_thread_type::dam_sys_stat):
				this->read_damage_data_per_database();
				this->output_statistic();
				break;
			case(_dam_thread_type::dam_sys_calc_no_break_sz):
				this->calc_damage_nobreak_sz();
				break;
			case(_dam_thread_type::dam_sys_calc_break_sz):
				this->calc_damage_break_sz();
				break;
			case(_dam_thread_type::dam_sys_instat_calc_no_break_sz):
				this->calc_instat_damage_nobreak_sz();
				break;
			case(_dam_thread_type::dam_sys_instat_calc_break_sz):
				this->calc_instat_damage_break_sz();
				break;
			case(_dam_thread_type::dam_check_sys):
				this->read_damage_data_per_database();
				this->check_damage_system();
				break;

			default:
				Warning msg=this->set_warning(0);
				msg.output_msg(4);
		}
	}
	catch(Error msg){
		this->thread_is_started=false;
		msg.output_msg(4);
	}
	this->thread_is_started=false;
}
//Set the thread type for specifiying the task of the thread
void Dam_Damage_System::set_thread_type(_dam_thread_type type){
	this->thread_type=type;
}
//Get the thread type for specifiying the task of the thread
_dam_thread_type Dam_Damage_System::get_thread_type(void){
	return this->thread_type;
}
//Set a pointer to the database
void Dam_Damage_System::set_ptr2database(QSqlDatabase *qsqldatabase){
	this->qsqldatabase=QSqlDatabase::cloneDatabase(*qsqldatabase, sys_label::str_dam.c_str());
	this->qsqldatabase.open();
}
//Set a specific raster type before the thread-calculation
void Dam_Damage_System::set_specific_raster_type(_dam_raster_types types){
	this->specific_raster_type=types;
}
//Set the stop thread flag  (static)
void Dam_Damage_System::set_stop_thread_flag(const bool flag){
	Dam_Damage_System::abort_thread_flag=flag;
	if(Dam_Damage_System::hyd_thread_runs==true){
		Hyd_Multiple_Hydraulic_Systems::set_stop_thread_flag(flag);
	}
}
//Check the stop thread flag (static)
void Dam_Damage_System::check_stop_thread_flag(void){
	if(Dam_Damage_System::abort_thread_flag==true){
		Error msg;
		msg.set_msg(_sys_system_modules::DAM_SYS);
		throw msg;
	}
}
//Get the stop thread flag (static)
bool Dam_Damage_System::get_stop_thread_flag(void){
	return Dam_Damage_System::abort_thread_flag;
}
//Get the number of errors
int Dam_Damage_System::get_error_number(void){
	return this->number_error;
}
//Create a whole set of damage database tables
void Dam_Damage_System::create_dam_database_tables(void){
	ostringstream cout;
	cout << "Create DAM system tables..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->set_start_warnings_number();
	try{
		//ecn
		Dam_Ecn_Raster::create_raster_table(&this->qsqldatabase);
		Dam_Ecn_Element::create_elem_table(&this->qsqldatabase);
		Dam_Ecn_Element::create_erg_table(&this->qsqldatabase);
		Dam_Ecn_Damage_Function::create_function_table(&this->qsqldatabase);
		//eco
		Dam_Eco_Btype_Raster::create_raster_table(&this->qsqldatabase);
		Dam_Eco_Btype_Element::create_elem_table(&this->qsqldatabase);
		Dam_Eco_Btype_Element::create_erg_table(&this->qsqldatabase);
		Dam_Eco_Btype::create_table(&this->qsqldatabase);
		Dam_Eco_Btype_Damage_Function::create_function_table(&this->qsqldatabase);
		Dam_Eco_Soil_Raster::create_raster_table(&this->qsqldatabase);
		Dam_Eco_Soil_Element::create_elem_table(&this->qsqldatabase);
		Dam_Eco_Soil_Element::create_erg_table(&this->qsqldatabase);
		Dam_Eco_Soil_Cost_Function::create_function_table(&this->qsqldatabase);
		Dam_Eco_Soil_Erosion_Function::create_function_table(&this->qsqldatabase);

		//pop
		Dam_People_Raster::create_raster_table(&this->qsqldatabase);
		Dam_People_Element::create_elem_table(&this->qsqldatabase);
		Dam_People_Element::create_erg_table(&this->qsqldatabase);
		Dam_People_Damage_Function::create_function_table(&this->qsqldatabase);

		//sc
		Dam_Sc_Subcategory::create_table(&this->qsqldatabase);
		Dam_Sc_Point::create_point_table(&this->qsqldatabase);
		Dam_Sc_Point::create_erg_table(&this->qsqldatabase);

		//pys
		Dam_Pys_Category::create_table(&this->qsqldatabase);
		Dam_Pys_Raster::create_raster_table(&this->qsqldatabase);
		Dam_Pys_Element::create_elem_table(&this->qsqldatabase);
		Dam_Pys_Element::create_erg_table(&this->qsqldatabase);
		Dam_Pys_Function::create_function_table(&this->qsqldatabase);
		Dam_Pys_Density_Function::create_point_table(&this->qsqldatabase);

		//CI
		Dam_CI_Point::create_point_table(&this->qsqldatabase);
		Dam_CI_Point::create_erg_table(&this->qsqldatabase);
		Dam_CI_Point::create_instat_erg_table(&this->qsqldatabase);
		Dam_CI_Polygon::create_polygon_table(&this->qsqldatabase);
		Dam_CI_Polygon::create_erg_table(&this->qsqldatabase);
		Dam_CI_Polygon::create_instat_erg_table(&this->qsqldatabase);
		Dam_CI_Element_List::create_connection_table(&this->qsqldatabase);
		Dam_CI_Element_List::create_erg_table(&this->qsqldatabase);
		Dam_CI_Element_List::create_instat_erg_table(&this->qsqldatabase);

		//results
		Dam_Damage_System::create_erg_table(&this->qsqldatabase);

		//system
		Dam_Damage_System::create_Dam_system_table(&this->qsqldatabase);
	}
	catch(Error msg){
		this->number_error++;
		msg.output_msg(4);
	}
	this->set_warning_number();
	cout << "Creation of the DAM database tables is finished" << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
}
//Check all damage database tables, for their existence in the database and their declaration in the database table-file
void Dam_Damage_System::check_dam_database_tables(void){
	ostringstream cout;
	cout << "Check DAM system tables..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout,false, false);
	this->set_start_warnings_number();
	try{
		//ecn
		cout << "Check economical raster database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Ecn_Raster::set_raster_table(&this->qsqldatabase);
		cout << "Check economical raster element database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Ecn_Element::set_elem_table(&this->qsqldatabase);
		cout << "Check economical raster element result database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Ecn_Element::set_erg_table(&this->qsqldatabase);
		cout << "Check economical damage function database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Ecn_Damage_Function::set_function_table(&this->qsqldatabase);

		//eco
		cout << "Check ecological biotope-type raster database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Eco_Btype_Raster::set_raster_table(&this->qsqldatabase);
		cout << "Check ecological biotope-type raster element database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Eco_Btype_Element::set_elem_table(&this->qsqldatabase);
		cout << "Check ecological biotope-type raster element result database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Eco_Btype_Element::set_erg_table(&this->qsqldatabase);
		cout << "Check ecological biotope-type database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Eco_Btype::set_table(&this->qsqldatabase);
		cout << "Check ecological biotope-type damage function database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Eco_Btype_Damage_Function::set_function_table(&this->qsqldatabase);
		cout << "Check ecological soil-erosion raster database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Eco_Soil_Raster::set_raster_table(&this->qsqldatabase);
		cout << "Check ecological soil-erosion raster element database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Eco_Soil_Element::set_elem_table(&this->qsqldatabase);
		cout << "Check ecological soil-erosion raster element result database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Eco_Soil_Element::set_erg_table(&this->qsqldatabase);
		cout << "Check ecological soil-cost function database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Eco_Soil_Cost_Function::set_function_table(&this->qsqldatabase);
		cout << "Check ecological soil-erosion function database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Eco_Soil_Erosion_Function::set_function_table(&this->qsqldatabase);

		//pop
		cout << "Check people2risk raster database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_People_Raster::set_raster_table(&this->qsqldatabase);
		cout << "Check people2risk raster element database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_People_Element::set_elem_table(&this->qsqldatabase);
		cout << "Check people2risk raster element result database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_People_Element::set_erg_table(&this->qsqldatabase);
		cout << "Check people2risk damage function database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_People_Damage_Function::set_function_table(&this->qsqldatabase);

		//sc
		cout << "Check simple-counting subcategory database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Sc_Subcategory::set_table(&this->qsqldatabase);
		cout << "Check simple-counting point database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Sc_Point::set_point_table(&this->qsqldatabase);
		cout << "Check simple-counting point result database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Sc_Point::set_erg_table(&this->qsqldatabase);

		//pys
		cout << "Check psycho-social category database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Pys_Category::set_table(&this->qsqldatabase);
		cout << "Check psycho-social raster database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Pys_Raster::set_raster_table(&this->qsqldatabase);
		cout << "Check psycho-social raster element database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Pys_Element::set_elem_table(&this->qsqldatabase);
		cout << "Check psycho-social raster element result database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Pys_Element::set_erg_table(&this->qsqldatabase);
		cout << "Check psycho-social damage function database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Pys_Function::set_function_table(&this->qsqldatabase);
		cout << "Check psycho-social density function database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Pys_Density_Function::set_point_table(&this->qsqldatabase);

		//CI
		cout << "Check CI-point database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout, false, false);
		Dam_CI_Point::set_point_table(&this->qsqldatabase);
		cout << "Check CI-polygon database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout, false, false);
		Dam_CI_Polygon::set_polygon_table(&this->qsqldatabase);
		cout << "Check CI-connection database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout, false, false);
		Dam_CI_Element_List::set_connection_table(&this->qsqldatabase);
		cout << "Check CI-point database result table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout, false, false);
		Dam_CI_Point::set_erg_table(&this->qsqldatabase);
		cout << "Check CI-point database instationary result table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout, false, false);
		Dam_CI_Point::set_instat_erg_table(&this->qsqldatabase);
		cout << "Check CI-polygon database result table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout, false, false);
		Dam_CI_Polygon::set_erg_table(&this->qsqldatabase);
		cout << "Check CI-polygon database instationary result table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout, false, false);
		Dam_CI_Polygon::set_instat_erg_table(&this->qsqldatabase);
		cout << "Check CI-connection database result table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout, false, false);
		Dam_CI_Element_List::set_erg_table(&this->qsqldatabase);
		cout << "Check CI-connection database instationary result table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout, false, false);
		Dam_CI_Element_List::set_instat_erg_table(&this->qsqldatabase);
		



		//results
		cout << "Check the damage system result database table..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout,false, false);
		Dam_Damage_System::set_erg_table(&this->qsqldatabase);

		cout << "Check the damage system database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout, false, false);
		Dam_Damage_System::set_Dam_system_table(&this->qsqldatabase);
	}
	catch(Error msg){
		this->number_error++;
		ostringstream info;
		info << "Check if the DAM tables are created!"<< endl;
		msg.make_second_info(info.str());
		//switch to non-fatal, because is handled later as fatal error
		msg.set_fatal_flag(false);
		msg.output_msg(4);
		this->output_error_statistic();
	}
	this->set_warning_number();
	cout << "Check of the DAM database tables is finished" << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout,false, false);
}
//Delete the data of all damage database tables
void Dam_Damage_System::delete_data_dam_database_tables(void){
	try{
		//ecn
		Dam_Ecn_Raster::delete_data_in_raster_table(&this->qsqldatabase);
		Dam_Ecn_Element::delete_data_in_elem_table(&this->qsqldatabase);
		Dam_Ecn_Damage_Function::delete_data_in_function_table(&this->qsqldatabase);

		//eco
		Dam_Eco_Btype_Raster::delete_data_in_raster_table(&this->qsqldatabase);
		Dam_Eco_Btype_Element::delete_data_in_elem_table(&this->qsqldatabase);
		Dam_Eco_Btype::delete_data_in_table(&this->qsqldatabase);
		Dam_Eco_Btype_Damage_Function::delete_data_in_function_table(&this->qsqldatabase);
		Dam_Eco_Soil_Raster::delete_data_in_raster_table(&this->qsqldatabase);
		Dam_Eco_Soil_Element::delete_data_in_elem_table(&this->qsqldatabase);
		Dam_Eco_Soil_Cost_Function::delete_data_in_function_table(&this->qsqldatabase);
		Dam_Eco_Soil_Erosion_Function::delete_data_in_function_table(&this->qsqldatabase);

		//pop
		Dam_People_Raster::delete_data_in_raster_table(&this->qsqldatabase);
		Dam_People_Element::delete_data_in_elem_table(&this->qsqldatabase);
		Dam_People_Damage_Function::delete_data_in_function_table(&this->qsqldatabase);

		//sc
		Dam_Sc_Subcategory::delete_data_in_table(&this->qsqldatabase);
		Dam_Sc_Point::delete_data_in_point_table(&this->qsqldatabase);
		Dam_Sc_Point::delete_data_in_erg_table(&this->qsqldatabase);

		//pys
		Dam_Pys_Category::delete_data_in_table(&this->qsqldatabase);
		Dam_Pys_Raster::delete_data_in_raster_table(&this->qsqldatabase);
		Dam_Pys_Element::delete_data_in_elem_table(&this->qsqldatabase);
		Dam_Pys_Function::delete_data_in_function_table(&this->qsqldatabase);
		Dam_Pys_Density_Function::delete_data_in_point_table(&this->qsqldatabase);

		//CI
		Dam_CI_Point::delete_data_in_point_table(&this->qsqldatabase);
		Dam_CI_Polygon::delete_data_in_polygon_table(&this->qsqldatabase);
		Dam_CI_Element_List::delete_data_in_connection_table(&this->qsqldatabase);
		Dam_CI_Point::delete_data_in_erg_table(&this->qsqldatabase);
		Dam_CI_Point::delete_data_in_instat_erg_table(&this->qsqldatabase);
		Dam_CI_Polygon::delete_data_in_erg_table(&this->qsqldatabase);
		Dam_CI_Polygon::delete_data_in_instat_erg_table(&this->qsqldatabase);
		Dam_CI_Element_List::delete_data_in_erg_table(&this->qsqldatabase);
		Dam_CI_Element_List::delete_data_in_instat_erg_table(&this->qsqldatabase);

		//results
		Dam_Damage_System::delete_data_in_erg_table(&this->qsqldatabase);
	}
	catch(Error msg){
		throw msg;
	}
}
//Close all damage database tables (static)
void Dam_Damage_System::close_dam_database_tables(void){
	//ecn
	Dam_Ecn_Raster::close_table();
	Dam_Ecn_Element::close_elem_table();
	Dam_Ecn_Element::close_erg_table();
	Dam_Ecn_Damage_Function::close_function_table();

	//eco
	Dam_Eco_Btype_Raster::close_table();
	Dam_Eco_Btype_Element::close_elem_table();
	Dam_Eco_Btype_Element::close_erg_table();
	Dam_Eco_Btype::close_table();
	Dam_Eco_Btype_Damage_Function::close_function_table();
	Dam_Eco_Soil_Raster::close_table();
	Dam_Eco_Soil_Element::close_elem_table();
	Dam_Eco_Soil_Element::close_erg_table();
	Dam_Eco_Soil_Cost_Function::close_function_table();
	Dam_Eco_Soil_Erosion_Function::close_function_table();

	//pop
	Dam_People_Raster::close_table();
	Dam_People_Element::close_elem_table();
	Dam_People_Element::close_erg_table();
	Dam_People_Damage_Function::close_function_table();

	//sc
	Dam_Sc_Subcategory::close_table();
	Dam_Sc_Point::close_point_table();
	Dam_Sc_Point::close_erg_table();

	//pys
	Dam_Pys_Category::close_table();
	Dam_Pys_Raster::close_table();
	Dam_Pys_Element::close_elem_table();
	Dam_Pys_Element::close_erg_table();
	Dam_Pys_Function::close_function_table();
	Dam_Pys_Density_Function::close_point_table();

	//ci
	Dam_CI_Point::close_point_table();
	Dam_CI_Polygon::close_polygon_table();
	Dam_CI_Point::close_erg_table();
	Dam_CI_Polygon::close_erg_table();
	Dam_CI_Point::close_instat_erg_table();
	Dam_CI_Polygon::close_instat_erg_table();
	Dam_CI_Element_List::close_connection_table();
	Dam_CI_Element_List::close_erg_table();
	Dam_CI_Element_List::close_instat_erg_table();

	//results
	Dam_Damage_System::close_erg_table();

	//system
	Dam_Damage_System::close_Dam_system_table();
}
//Check if some damage raster are set in the database in the damage system (static)
bool Dam_Damage_System::check_some_raster_set(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel results;

	if(Dam_Ecn_Raster::select_relevant_raster_database(&results,ptr_database, false)>0){
		return true;
	}
	if(Dam_Eco_Btype_Raster::select_relevant_raster_database(&results,ptr_database, false)>0){
		return true;
	}
	if(Dam_Eco_Soil_Raster::select_relevant_raster_database(&results,ptr_database, false)>0){
		return true;
	}
	if(Dam_People_Raster::select_relevant_raster_database(&results,ptr_database, false)>0){
		return true;
	}
	if(Dam_Pys_Raster::select_relevant_raster_database(&results,ptr_database, false)>0){
		return true;
	}
	if(Dam_Sc_Point::count_relevant_points_database(&results, ptr_database, id,false)>0){
		return true;
	}
	if (Dam_CI_Point::count_relevant_points_database(&results, ptr_database, id, false) > 0) {
		return true;
	}

	return false;
}
//Calculate the damage for the nobreak scenario
void Dam_Damage_System::calc_damage_nobreak_sz(void){
	ostringstream prefix;
	prefix << "CALC> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);
	emit send_hyd_thread_enable(true);
	this->read_damage_data_per_database();
	int current_counter;

	ostringstream cout;
	cout << "Calculate the damages for selected nobreak scenarios..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);
	//QSqlQueryModel *model;
	//model=NULL;
	//model=new QSqlQueryModel;
	this->break_sz="CA";

	if(Sys_Project::get_project_type()!=_sys_project_type::proj_dam){
		//check first for hydraulic results
		for(int i=0; i< this->sz_bound_manager.get_number_sz(); i++){
			if(Hyd_Hydraulic_System::check_hyd_results_calculated(&this->qsqldatabase, this->system_id, this->sz_bound_manager.get_ptr_sz(i)->get_id(), "CA" )==false){
				Warning msg=this->set_warning(2);
				ostringstream info;
				info<<"Scenario name  : " << this->sz_bound_manager.get_ptr_sz(i)->get_name() << endl;
				info<<"Scenario id    : " << this->sz_bound_manager.get_ptr_sz(i)->get_id() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
				this->sz_bound_manager.get_ptr_sz(i)->set_is_selected(false);
			}
		}
	}
	//if(model!=NULL){
	//	delete model;
	//}

	if(this->sz_bound_manager.counter_number_selected_scenarios()!=0){
		try{
			try{
				//ecn
				ostringstream prefix;
				prefix << "ECN> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for(int i=0; i< this->sz_bound_manager.get_number_sz(); i++){
					if(this->sz_bound_manager.get_ptr_sz(i)->get_is_selected()==false){
						continue;
					}
					this->ecn_sys.delete_result_members_in_database(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
				}
				//calculation for each ecn raster
				for(int i=0; i< this->ecn_sys.get_number_raster(); i++){
					current_counter=i;
					this->ecn_sys.init_damage_rasters(i, &this->qsqldatabase, this->system_id);

					for(int j=0; j< this->sz_bound_manager.get_number_sz(); j++){
						if(this->sz_bound_manager.get_ptr_sz(j)->get_is_selected()==false){
							continue;
						}
						cout << "Read in the hydraulic impact values for nobreak scenario "<<this->sz_bound_manager.get_ptr_sz(j)->get_name()<<"..." << endl ;
						Sys_Common_Output::output_dam->output_txt(&cout);
						this->set_impact_values_hyd(this->sz_bound_manager.get_ptr_sz(j)->get_id(),this->break_sz);
						this->ecn_sys.calculate_damages(this->impact_floodplain, this->number_floodplain_impact, i);
						Dam_Damage_System::check_stop_thread_flag();
						this->ecn_sys.output_result_member2database(&this->qsqldatabase, this->sz_bound_manager.get_ptr_sz(j)->get_id(),this->break_sz,i, this->sz_bound_manager.get_ptr_sz(j)->get_ptr_dam_was_output());
					}
					this->ecn_sys.damage_raster[i].clear_raster();
				}
				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch(Error msg){
				this->ecn_sys.damage_raster[current_counter].clear_raster();
				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			try{
				//eco biotope-types
				prefix << "ECO> ";
				prefix << "B_TYP> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for(int i=0; i< this->sz_bound_manager.get_number_sz(); i++){
					if(this->sz_bound_manager.get_ptr_sz(i)->get_is_selected()==false){
						continue;
					}
					this->eco_sys.delete_biotope_result_members_in_database(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
				}
				//calculation for each eco biotope-type raster
				for(int i=0; i< this->eco_sys.get_number_biotope_raster(); i++){
					current_counter=i;
					this->eco_sys.init_biotope_rasters(i, &this->qsqldatabase, this->system_id);

					for(int j=0; j< this->sz_bound_manager.get_number_sz(); j++){
						if(this->sz_bound_manager.get_ptr_sz(j)->get_is_selected()==false){
							continue;
						}
						cout << "Read in the hydraulic impact values for nobreak scenario "<<this->sz_bound_manager.get_ptr_sz(j)->get_name()<<"..." << endl ;
						Sys_Common_Output::output_dam->output_txt(&cout);
						this->set_impact_values_hyd(this->sz_bound_manager.get_ptr_sz(j)->get_id(),this->break_sz);
						this->eco_sys.calculate_biotope_damages(this->impact_floodplain, this->number_floodplain_impact, i);
						Dam_Damage_System::check_stop_thread_flag();
						this->eco_sys.output_biotope_result_member2database(&this->qsqldatabase, this->sz_bound_manager.get_ptr_sz(j)->get_id(),this->break_sz,i, this->sz_bound_manager.get_ptr_sz(j)->get_ptr_dam_was_output());
					}
					this->eco_sys.raster_btypes[i].clear_raster();
				}

				Sys_Common_Output::output_dam->rewind_userprefix();
				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch(Error msg){
				this->eco_sys.raster_btypes[current_counter].clear_raster();
				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}

				msg.output_msg(4);
				prefix.str("");
				this->number_error++;
				Sys_Common_Output::output_dam->rewind_userprefix();
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			try{
				//eco soil-type
				prefix << "ECO> ";
				prefix << "SOIL> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for(int i=0; i< this->sz_bound_manager.get_number_sz(); i++){
					if(this->sz_bound_manager.get_ptr_sz(i)->get_is_selected()==false){
						continue;
					}
					this->eco_sys.delete_soil_result_members_in_database(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
				}
				//calculation for each eco soil-type raster
				for(int i=0; i< this->eco_sys.get_number_soil_raster(); i++){
					current_counter=i;
					this->eco_sys.init_soil_rasters(i, &this->qsqldatabase, this->system_id);

					for(int j=0; j< this->sz_bound_manager.get_number_sz(); j++){
						if(this->sz_bound_manager.get_ptr_sz(j)->get_is_selected()==false){
							continue;
						}
						cout << "Read in the hydraulic impact values for nobreak scenario "<<this->sz_bound_manager.get_ptr_sz(j)->get_name()<<"..." << endl ;
						Sys_Common_Output::output_dam->output_txt(&cout);
						this->set_impact_values_hyd(this->sz_bound_manager.get_ptr_sz(j)->get_id(),this->break_sz);
						this->eco_sys.calculate_soil_damages(this->impact_floodplain, this->number_floodplain_impact, i);
						Dam_Damage_System::check_stop_thread_flag();
						this->eco_sys.output_soil_result_member2database(&this->qsqldatabase, this->sz_bound_manager.get_ptr_sz(j)->get_id(),this->break_sz,i, this->sz_bound_manager.get_ptr_sz(j)->get_ptr_dam_was_output());
					}
					this->eco_sys.soil_raster[i].clear_raster();
				}

				Sys_Common_Output::output_dam->rewind_userprefix();
				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch(Error msg){
				this->eco_sys.soil_raster[current_counter].clear_raster();
				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			try{
				//pop
				prefix << "POP> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for(int i=0; i< this->sz_bound_manager.get_number_sz(); i++){
					if(this->sz_bound_manager.get_ptr_sz(i)->get_is_selected()==false){
						continue;
					}
					this->people_sys.delete_result_members_in_database(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
				}
				//calculation for each people2risk raster
				for(int i=0; i< this->people_sys.get_number_raster(); i++){
					current_counter=i;
					this->people_sys.init_damage_rasters(i, &this->qsqldatabase, this->system_id);

					for(int j=0; j< this->sz_bound_manager.get_number_sz(); j++){
						if(this->sz_bound_manager.get_ptr_sz(j)->get_is_selected()==false){
							continue;
						}
						cout << "Read in the hydraulic impact values for nobreak scenario "<<this->sz_bound_manager.get_ptr_sz(j)->get_name()<<"..." << endl ;
						Sys_Common_Output::output_dam->output_txt(&cout);
						this->set_impact_values_hyd(this->sz_bound_manager.get_ptr_sz(j)->get_id(),this->break_sz);
						this->people_sys.calculate_damages(this->impact_floodplain, this->number_floodplain_impact,i);
						Dam_Damage_System::check_stop_thread_flag();
						this->people_sys.output_result_member2database(&this->qsqldatabase, this->sz_bound_manager.get_ptr_sz(j)->get_id(),this->break_sz,i, this->sz_bound_manager.get_ptr_sz(j)->get_ptr_dam_was_output());
					}
					this->people_sys.raster[i].clear_raster();
				}
				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch(Error msg){
				this->people_sys.raster[current_counter].clear_raster();

				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			try{
				//pys
				prefix << "PYS> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for(int i=0; i< this->sz_bound_manager.get_number_sz(); i++){
					if(this->sz_bound_manager.get_ptr_sz(i)->get_is_selected()==false){
						continue;
					}
					this->pys_sys.delete_result_members_in_database(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
				}
				//calculation for each pys raster
				for(int i=0; i< this->pys_sys.get_number_raster(); i++){
					current_counter=i;
					this->pys_sys.init_damage_raster(i, &this->qsqldatabase, this->system_id);

					for(int j=0; j< this->sz_bound_manager.get_number_sz(); j++){
						if(this->sz_bound_manager.get_ptr_sz(j)->get_is_selected()==false){
							continue;
						}
						cout << "Read in the hydraulic impact values for nobreak scenario "<<this->sz_bound_manager.get_ptr_sz(j)->get_name()<<"..." << endl ;
						Sys_Common_Output::output_dam->output_txt(&cout);
						this->set_impact_values_hyd(this->sz_bound_manager.get_ptr_sz(j)->get_id(),this->break_sz);
						this->pys_sys.calculate_damages(this->impact_floodplain, this->number_floodplain_impact,i);
						Dam_Damage_System::check_stop_thread_flag();
						this->pys_sys.output_result_member2database(&this->qsqldatabase, this->sz_bound_manager.get_ptr_sz(j)->get_id(),this->break_sz,i, this->sz_bound_manager.get_ptr_sz(j)->get_ptr_dam_was_output());
					}
					this->pys_sys.raster[i].clear_raster();
				}
				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch(Error msg){
				this->pys_sys.raster[current_counter].clear_raster();
				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			try{
				//sc
				prefix << "SC> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for(int i=0; i< this->sz_bound_manager.get_number_sz(); i++){
					if(this->sz_bound_manager.get_ptr_sz(i)->get_is_selected()==false){
						continue;
					}
					this->sc_sys.delete_result_members_in_database(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
				}
				//calculation for sc points
				for(int j=0; j< this->sz_bound_manager.get_number_sz(); j++){
					if(this->sz_bound_manager.get_ptr_sz(j)->get_is_selected()==false){
						continue;
					}
					cout << "Read in the hydraulic impact values for nobreak scenario "<<this->sz_bound_manager.get_ptr_sz(j)->get_name()<<"..." << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
					this->set_impact_values_hyd(this->sz_bound_manager.get_ptr_sz(j)->get_id(),this->break_sz);
					this->sc_sys.calculate_damages(this->impact_floodplain, this->number_floodplain_impact);
					Dam_Damage_System::check_stop_thread_flag();
					this->sc_sys.output_result_member2database(&this->qsqldatabase, this->sz_bound_manager.get_ptr_sz(j)->get_id(),this->break_sz, this->sz_bound_manager.get_ptr_sz(j)->get_ptr_dam_was_output());
				}

				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch(Error msg){
				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			try {
				//sc
				prefix << "CI> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for (int i = 0; i < this->sz_bound_manager.get_number_sz(); i++) {
					if (this->sz_bound_manager.get_ptr_sz(i)->get_is_selected() == false) {
						continue;
					}
					this->ci_sys.delete_result_members_in_database(&this->qsqldatabase, this->system_id, this->sz_bound_manager.get_ptr_sz(i)->get_id(), this->break_sz);
				}
				//calculation for sc points
				for (int j = 0; j < this->sz_bound_manager.get_number_sz(); j++) {
					if (this->sz_bound_manager.get_ptr_sz(j)->get_is_selected() == false) {
						continue;
					}
					cout << "Read in the hydraulic impact values for nobreak scenario " << this->sz_bound_manager.get_ptr_sz(j)->get_name() << "..." << endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					this->set_impact_values_hyd(this->sz_bound_manager.get_ptr_sz(j)->get_id(), this->break_sz);
					this->ci_sys.calculate_damages(this->impact_floodplain, this->number_floodplain_impact);
					Dam_Damage_System::check_stop_thread_flag();
					this->ci_sys.output_result_member2database(&this->qsqldatabase, this->sz_bound_manager.get_ptr_sz(j)->get_id(), this->break_sz, this->sz_bound_manager.get_ptr_sz(j)->get_ptr_dam_was_output());
				}

				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch (Error msg) {
				if (Dam_Damage_System::abort_thread_flag == true) {
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			//calculate the total results
			try{
				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix << "RES> ";
				Sys_Common_Output::output_dam->set_userprefix(&prefix);

				//output the results
				for(int i=0; i< this->sz_bound_manager.get_number_sz(); i++){
					if(this->sz_bound_manager.get_ptr_sz(i)->get_is_selected()==false){
						continue;
					}

					cout << "Damage results of nobreak scenario " << this->sz_bound_manager.get_ptr_sz(i)->get_name()<< endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					prefix <<this->sz_bound_manager.get_ptr_sz(i)->get_name()<<"> ";
					Sys_Common_Output::output_dam->set_userprefix(&prefix);
					//ecn
					this->ecn_sys.sum_total_results(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
					this->ecn_sys.calculate_damage_landuse_id(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
					this->ecn_sys.output_result_damage();
					//eco
					this->eco_sys.sum_total_results(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
					this->eco_sys.output_result_damage();
					//pop
					this->people_sys.sum_total_results(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
					this->people_sys.output_result_damage();
					//pys
					this->pys_sys.sum_total_results(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
					this->pys_sys.output_result_damage();
					//sc
					this->sc_sys.sum_total_results(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
					this->sc_sys.output_result_damage();
					//ci
					this->ci_sys.sum_total_results(&this->qsqldatabase, this->system_id, this->sz_bound_manager.get_ptr_sz(i)->get_id(), this->break_sz);
					this->ci_sys.output_result_damage();

					//to database
					Dam_Damage_System::delete_data_in_erg_table(&this->qsqldatabase, this->system_id,this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
					this->output_result_members2database(&this->qsqldatabase, this->sz_bound_manager.get_ptr_sz(i)->get_id(),this->break_sz);
					Sys_Common_Output::output_dam->rewind_userprefix();
				}
			}
			catch(Error msg){
				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				Sys_Common_Output::output_dam->rewind_userprefix();
			}
		}
		catch(Error msg){
			msg.output_msg(4);
			Sys_Common_Output::output_dam->rewind_userprefix();
		}
	}
	else{
		cout << "No hydraulic results are available"<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_multi_nobreak_hydsz();

	emit send_hyd_thread_enable(false);
}
//Calculate the damage for the break scenario
void Dam_Damage_System::calc_damage_break_sz(void){
	ostringstream prefix;
	prefix << "CALC> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);
	emit send_hyd_thread_enable(true);
	this->read_damage_data_per_database();
	int current_counter;

	ostringstream cout;
	cout << "Calculate the damages for generated break scenarios..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);

	//QSqlQueryModel *model;
	//model=NULL;
	//model=new QSqlQueryModel;
	//check first for hydraulic results
	for(int i=0; i< this->number_break_sc; i++){
		if(this->ptr_break_sc_data[i].must_calc==true){
			if(Hyd_Hydraulic_System::check_hyd_results_calculated(&this->qsqldatabase, this->system_id, this->ptr_break_sc_data[i].id_hyd_sc, this->ptr_break_sc_data[i].break_sc )==false){
				Warning msg=this->set_warning(3);
				ostringstream info;
				info<<"Scenario name  : " << this->ptr_break_sc_data[i].combi_name << endl;
				info<<"Scenario id    : " << this->ptr_break_sc_data[i].break_sc << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
				this->ptr_break_sc_data[i].must_calc=false;
			}
		}
	}
	//if(model!=NULL){
	//	delete model;
	//}
	//count the number of scenario for calculation
	this->number_break_sc2calc=0;
	for(int i=0; i< this->number_break_sc; i++){
		if(this->ptr_break_sc_data[i].must_calc==true){
			this->number_break_sc2calc++;
		}
	}

	if(this->number_break_sc2calc!=0){
		try{
			try{
				//ecn
				ostringstream prefix;
				prefix << "ECN> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for(int i=0; i< this->number_break_sc; i++){
					if(this->ptr_break_sc_data[i].must_calc==false){
						continue;
					}
					this->ecn_sys.delete_result_members_in_database(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
				}
				//calculation for each ecn raster
				for(int i=0; i< this->ecn_sys.get_number_raster(); i++){
					current_counter=i;
					this->ecn_sys.init_damage_rasters(i, &this->qsqldatabase, this->system_id);

					for(int j=0; j< this->number_break_sc; j++){
						if(this->ptr_break_sc_data[j].must_calc==false){
							continue;
						}
						cout << "Read in the hydraulic impact values for break scenario "<<this->ptr_break_sc_data[j].combi_name<<"..." << endl ;
						Sys_Common_Output::output_dam->output_txt(&cout);
						this->set_impact_values_hyd(this->ptr_break_sc_data[j].id_hyd_sc,this->ptr_break_sc_data[j].break_sc);
						this->ecn_sys.calculate_damages(this->impact_floodplain, this->number_floodplain_impact, i);
						Dam_Damage_System::check_stop_thread_flag();
						this->ecn_sys.output_result_member2database(&this->qsqldatabase, this->ptr_break_sc_data[j].id_hyd_sc,this->ptr_break_sc_data[j].break_sc,i, &(this->ptr_break_sc_data[j].dam_was_output));
					}
					this->ecn_sys.damage_raster[i].clear_raster();
				}
				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch(Error msg){
				this->ecn_sys.damage_raster[current_counter].clear_raster();
				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			try{
				//eco biotope-types
				prefix << "ECO> ";
				prefix << "B_TYP> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for(int i=0; i< this->number_break_sc; i++){
					if(this->ptr_break_sc_data[i].must_calc==false){
						continue;
					}
					this->eco_sys.delete_biotope_result_members_in_database(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
				}
				//calculation for each eco biotope-type raster
				for(int i=0; i< this->eco_sys.get_number_biotope_raster(); i++){
					current_counter=i;
					this->eco_sys.init_biotope_rasters(i, &this->qsqldatabase, this->system_id);

					for(int j=0; j< this->number_break_sc; j++){
						if(this->ptr_break_sc_data[j].must_calc==false){
							continue;
						}
						cout << "Read in the hydraulic impact values for break scenario "<<this->ptr_break_sc_data[j].combi_name<<"..." << endl ;
						Sys_Common_Output::output_dam->output_txt(&cout);
						this->set_impact_values_hyd(this->ptr_break_sc_data[j].id_hyd_sc,this->ptr_break_sc_data[j].break_sc);
						this->eco_sys.calculate_biotope_damages(this->impact_floodplain, this->number_floodplain_impact, i);
						Dam_Damage_System::check_stop_thread_flag();
						this->eco_sys.output_biotope_result_member2database(&this->qsqldatabase, this->ptr_break_sc_data[j].id_hyd_sc,this->ptr_break_sc_data[j].break_sc,i, &(this->ptr_break_sc_data[j].dam_was_output));
					}
					this->eco_sys.raster_btypes[i].clear_raster();
				}

				Sys_Common_Output::output_dam->rewind_userprefix();
				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch(Error msg){
				this->eco_sys.raster_btypes[current_counter].clear_raster();
				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}

				msg.output_msg(4);
				prefix.str("");
				this->number_error++;
				Sys_Common_Output::output_dam->rewind_userprefix();
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			try{
				//eco soil-type
				prefix << "ECO> ";
				prefix << "SOIL> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for(int i=0; i< this->number_break_sc; i++){
					if(this->ptr_break_sc_data[i].must_calc==false){
						continue;
					}
					this->eco_sys.delete_soil_result_members_in_database(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
				}
				//calculation for each eco soil-type raster
				for(int i=0; i< this->eco_sys.get_number_soil_raster(); i++){
					current_counter=i;
					this->eco_sys.init_soil_rasters(i, &this->qsqldatabase, this->system_id);

					for(int j=0; j< this->number_break_sc; j++){
						if(this->ptr_break_sc_data[j].must_calc==false){
							continue;
						}
						cout << "Read in the hydraulic impact values for break scenario "<<this->ptr_break_sc_data[j].combi_name<<"..." << endl ;
						Sys_Common_Output::output_dam->output_txt(&cout);
						this->set_impact_values_hyd(this->ptr_break_sc_data[j].id_hyd_sc,this->ptr_break_sc_data[j].break_sc);
						this->eco_sys.calculate_soil_damages(this->impact_floodplain, this->number_floodplain_impact, i);
						Dam_Damage_System::check_stop_thread_flag();
						this->eco_sys.output_soil_result_member2database(&this->qsqldatabase, this->ptr_break_sc_data[j].id_hyd_sc,this->ptr_break_sc_data[j].break_sc,i, &(this->ptr_break_sc_data[j].dam_was_output));
					}
					this->eco_sys.soil_raster[i].clear_raster();
				}

				Sys_Common_Output::output_dam->rewind_userprefix();
				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch(Error msg){
				this->eco_sys.soil_raster[current_counter].clear_raster();
				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			try{
				//pop
				prefix << "POP> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for(int i=0; i< this->number_break_sc; i++){
					if(this->ptr_break_sc_data[i].must_calc==false){
						continue;
					}
					this->people_sys.delete_result_members_in_database(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
				}
				//calculation for each ecn raster
				for(int i=0; i< this->people_sys.get_number_raster(); i++){
					current_counter=i;
					this->people_sys.init_damage_rasters(i, &this->qsqldatabase, this->system_id);

					for(int j=0; j< this->number_break_sc; j++){
						if(this->ptr_break_sc_data[j].must_calc==false){
							continue;
						}
						cout << "Read in the hydraulic impact values for break scenario "<<this->ptr_break_sc_data[j].combi_name<<"..." << endl ;
						Sys_Common_Output::output_dam->output_txt(&cout);
						this->set_impact_values_hyd(this->ptr_break_sc_data[j].id_hyd_sc,this->ptr_break_sc_data[j].break_sc);
						this->people_sys.calculate_damages(this->impact_floodplain, this->number_floodplain_impact,i);
						Dam_Damage_System::check_stop_thread_flag();
						this->people_sys.output_result_member2database(&this->qsqldatabase, this->ptr_break_sc_data[j].id_hyd_sc,this->ptr_break_sc_data[j].break_sc,i, &(this->ptr_break_sc_data[j].dam_was_output));
					}
					this->people_sys.raster[i].clear_raster();
				}
				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch(Error msg){
				this->people_sys.raster[current_counter].clear_raster();

				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			try{
				//pys
				prefix << "PYS> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for(int i=0; i< this->number_break_sc; i++){
					if(this->ptr_break_sc_data[i].must_calc==false){
						continue;
					}
					this->pys_sys.delete_result_members_in_database(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
				}
				//calculation for each pys raster
				for(int i=0; i< this->pys_sys.get_number_raster(); i++){
					current_counter=i;
					this->pys_sys.init_damage_raster(i, &this->qsqldatabase, this->system_id);

					for(int j=0; j< this->number_break_sc; j++){
						if(this->ptr_break_sc_data[j].must_calc==false){
							continue;
						}
						cout << "Read in the hydraulic impact values for break scenario "<<this->ptr_break_sc_data[j].combi_name<<"..." << endl ;
						Sys_Common_Output::output_dam->output_txt(&cout);
						this->set_impact_values_hyd(this->ptr_break_sc_data[j].id_hyd_sc,this->ptr_break_sc_data[j].break_sc);
						this->pys_sys.calculate_damages(this->impact_floodplain, this->number_floodplain_impact,i);
						Dam_Damage_System::check_stop_thread_flag();
						this->pys_sys.output_result_member2database(&this->qsqldatabase, this->ptr_break_sc_data[j].id_hyd_sc,this->ptr_break_sc_data[j].break_sc,i, &(this->ptr_break_sc_data[j].dam_was_output));
					}
					this->pys_sys.raster[i].clear_raster();
				}
				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch(Error msg){
				this->pys_sys.raster[current_counter].clear_raster();
				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			try{
				//sc
				prefix << "SC> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for(int i=0; i< this->number_break_sc; i++){
					if(this->ptr_break_sc_data[i].must_calc==false){
						continue;
					}
					this->sc_sys.delete_result_members_in_database(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
				}

				for(int j=0; j< this->number_break_sc; j++){
					if(this->ptr_break_sc_data[j].must_calc==false){
						continue;
					}
					cout << "Read in the hydraulic impact values for break scenario "<<this->ptr_break_sc_data[j].combi_name<<"..." << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
					this->set_impact_values_hyd(this->ptr_break_sc_data[j].id_hyd_sc,this->ptr_break_sc_data[j].break_sc);
					this->sc_sys.calculate_damages(this->impact_floodplain, this->number_floodplain_impact);
					Dam_Damage_System::check_stop_thread_flag();
					this->sc_sys.output_result_member2database(&this->qsqldatabase,  this->ptr_break_sc_data[j].id_hyd_sc,this->ptr_break_sc_data[j].break_sc, &(this->ptr_break_sc_data[j].dam_was_output));
				}

				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch(Error msg){
				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
			}


			try {
				//sc
				prefix << "CI> ";
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for (int i = 0; i < this->number_break_sc; i++) {
					if (this->ptr_break_sc_data[i].must_calc == false) {
						continue;
					}
					this->ci_sys.delete_result_members_in_database(&this->qsqldatabase, this->system_id, this->ptr_break_sc_data[i].id_hyd_sc, this->ptr_break_sc_data[i].break_sc);
				}

				for (int j = 0; j < this->number_break_sc; j++) {
					if (this->ptr_break_sc_data[j].must_calc == false) {
						continue;
					}
					cout << "Read in the hydraulic impact values for break scenario " << this->ptr_break_sc_data[j].combi_name << "..." << endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					this->set_impact_values_hyd(this->ptr_break_sc_data[j].id_hyd_sc, this->ptr_break_sc_data[j].break_sc);
					this->ci_sys.calculate_damages(this->impact_floodplain, this->number_floodplain_impact);
					Dam_Damage_System::check_stop_thread_flag();
					this->ci_sys.output_result_member2database(&this->qsqldatabase, this->ptr_break_sc_data[j].id_hyd_sc, this->ptr_break_sc_data[j].break_sc, &(this->ptr_break_sc_data[j].dam_was_output));
				}

				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch (Error msg) {
				if (Dam_Damage_System::abort_thread_flag == true) {
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			//calculate the total results

			try{
				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix << "RES> ";
				Sys_Common_Output::output_dam->set_userprefix(&prefix);

				//output the results
				for(int i=0; i< this->number_break_sc; i++){
					if(this->ptr_break_sc_data[i].must_calc==false){
						continue;
					}
					cout << "Damage results of break scenario " << this->ptr_break_sc_data[i].combi_name<< endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					prefix <<this->ptr_break_sc_data[i].combi_name<<"> ";
					Sys_Common_Output::output_dam->set_userprefix(&prefix);
					//ecn
					this->ecn_sys.sum_total_results(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
					this->ecn_sys.calculate_damage_landuse_id(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
					this->ecn_sys.output_result_damage();
					//eco
					this->eco_sys.sum_total_results(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
					this->eco_sys.output_result_damage();
					//pop
					this->people_sys.sum_total_results(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
					this->people_sys.output_result_damage();
					//pys
					this->pys_sys.sum_total_results(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
					this->pys_sys.output_result_damage();
					//sc
					this->sc_sys.sum_total_results(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
					this->sc_sys.output_result_damage();
					//ci
					this->ci_sys.sum_total_results(&this->qsqldatabase, this->system_id, this->ptr_break_sc_data[i].id_hyd_sc, this->ptr_break_sc_data[i].break_sc);
					this->ci_sys.output_result_damage();

					//to database
					Dam_Damage_System::delete_data_in_erg_table(&this->qsqldatabase, this->system_id,this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
					this->output_result_members2database(&this->qsqldatabase, this->ptr_break_sc_data[i].id_hyd_sc,this->ptr_break_sc_data[i].break_sc);
					Sys_Common_Output::output_dam->rewind_userprefix();
				}
			}
			catch(Error msg){
				this->number_error++;
				if(Dam_Damage_System::abort_thread_flag==true){
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				msg.output_msg(4);
				Sys_Common_Output::output_dam->rewind_userprefix();
			}
		}
		catch(Error msg){
			msg.output_msg(4);
			Sys_Common_Output::output_dam->rewind_userprefix();
		}
	}
	else{
		cout << "No hydraulic results are available"<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_multi_break_sz();

	emit send_hyd_thread_enable(false);
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Calculate the instationary damage for the nobreak scenario
void Dam_Damage_System::calc_instat_damage_nobreak_sz(void) {
	ostringstream prefix;
	prefix << "CALC> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);
	emit send_hyd_thread_enable(true);
	this->read_damage_data_per_database();
	this->set_Dam_system_parameters_db();
	

	ostringstream cout;
	cout << "Calculate the instationary damages for selected nobreak scenarios..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);
	//QSqlQueryModel *model;
	//model=NULL;
	//model=new QSqlQueryModel;
	this->break_sz = "CA";

	if (Sys_Project::get_project_type() != _sys_project_type::proj_dam) {
		//check first for hydraulic results
		for (int i = 0; i < this->sz_bound_manager.get_number_sz(); i++) {
			if (Hyd_Hydraulic_System::check_hyd_instat_results_calculated(&this->qsqldatabase, this->system_id, this->sz_bound_manager.get_ptr_sz(i)->get_id(), "CA") == false) {
				Warning msg = this->set_warning(4);
				ostringstream info;
				info << "Scenario name  : " << this->sz_bound_manager.get_ptr_sz(i)->get_name() << endl;
				info << "Scenario id    : " << this->sz_bound_manager.get_ptr_sz(i)->get_id() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
				this->sz_bound_manager.get_ptr_sz(i)->set_is_selected(false);
			}
		}
	}
	//if(model!=NULL){
	//	delete model;
	//}

	if (this->sz_bound_manager.counter_number_selected_scenarios() != 0) {
		try {
			//try {
			//	//ecn
			//	ostringstream prefix;
			//	prefix << "ECN> ";
			//	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
			//	//delete the result data
			//	for (int i = 0; i < this->sz_bound_manager.get_number_sz(); i++) {
			//		if (this->sz_bound_manager.get_ptr_sz(i)->get_is_selected() == false) {
			//			continue;
			//		}
			//		this->ecn_sys.delete_result_members_in_database(&this->qsqldatabase, this->system_id, this->sz_bound_manager.get_ptr_sz(i)->get_id(), this->break_sz);
			//	}
			//	//calculation for each ecn raster
			//	for (int i = 0; i < this->ecn_sys.get_number_raster(); i++) {
			//		current_counter = i;
			//		this->ecn_sys.init_damage_rasters(i, &this->qsqldatabase, this->system_id);
			//		for (int j = 0; j < this->sz_bound_manager.get_number_sz(); j++) {
			//			if (this->sz_bound_manager.get_ptr_sz(j)->get_is_selected() == false) {
			//				continue;
			//			}
			//			cout << "Read in the hydraulic impact values for nobreak scenario " << this->sz_bound_manager.get_ptr_sz(j)->get_name() << "..." << endl;
			//			Sys_Common_Output::output_dam->output_txt(&cout);
			//			this->set_impact_values_hyd(this->sz_bound_manager.get_ptr_sz(j)->get_id(), this->break_sz);
			//			this->ecn_sys.calculate_damages(this->impact_floodplain, this->number_floodplain_impact, i);
			//			Dam_Damage_System::check_stop_thread_flag();
			//			this->ecn_sys.output_result_member2database(&this->qsqldatabase, this->sz_bound_manager.get_ptr_sz(j)->get_id(), this->break_sz, i, this->sz_bound_manager.get_ptr_sz(j)->get_ptr_dam_was_output());
			//		}
			//		this->ecn_sys.damage_raster[i].clear_raster();
			//	}
			//	Sys_Common_Output::output_dam->rewind_userprefix();
			//	prefix.str("");
			//}
			//catch (Error msg) {
			//	this->ecn_sys.damage_raster[current_counter].clear_raster();
			//	if (Dam_Damage_System::abort_thread_flag == true) {
			//		Sys_Common_Output::output_dam->rewind_userprefix();
			//		throw msg;
			//	}
			//	this->number_error++;
			//	msg.output_msg(4);
			//	prefix.str("");
			//	Sys_Common_Output::output_dam->rewind_userprefix();
			//}


			try {
				//sc
				prefix << "CI> ";
				//Input parameter

				bool max_timestep_reached = false;

				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for (int i = 0; i < this->sz_bound_manager.get_number_sz(); i++) {
					if (this->sz_bound_manager.get_ptr_sz(i)->get_is_selected() == false) {
						continue;
					}
					this->ci_sys.delete_instat_result_members_in_database(&this->qsqldatabase, this->system_id, this->sz_bound_manager.get_ptr_sz(i)->get_id(), this->break_sz);
				}
				//calculation for sc points for scenario
				for (int j = 0; j < this->sz_bound_manager.get_number_sz(); j++) {
					if (this->sz_bound_manager.get_ptr_sz(j)->get_is_selected() == false) {
						continue;
					}
					cout << "Read in the instationary hydraulic impact values for nobreak scenario " << this->sz_bound_manager.get_ptr_sz(j)->get_name() << "..." << endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					QStringList time_date;
					Hyd_Element_Floodplain::get_distinct_date_time_instat_results_elements_database(&time_date, &this->qsqldatabase, this->system_id, this->sz_bound_manager.get_ptr_sz(j)->get_id(), this->break_sz);
					cout << time_date.count()<< " number of distinct date-time string found in database for scenario " << this->sz_bound_manager.get_ptr_sz(j)->get_name() << " found"<< endl;
					Sys_Common_Output::output_dam->output_txt(&cout);


					Hyd_Param_Global buffer_para;
					
					try {
						buffer_para.globals_per_database(&this->qsqldatabase, false);
					}
					catch (Error msg) {
						throw msg;
					}


					double timestep = 0.0;
					double start_time_buff = buffer_para.get_startime();
					//timestep = Dam_Impact_Value_Floodplain::analyse_date_time(time_date)- start_time_buff;
					timestep = buffer_para.get_stepsize();
					double time = 0.0;

					for (int i = 0; i < time_date.count(); i++) {
						if (i == this->system_param.max_timesteps) {
							cout << "Maximum timesteps for calculation are reached ("<< this->system_param.max_timesteps <<")" << endl;
							Sys_Common_Output::output_dam->output_txt(&cout);
							max_timestep_reached = true;
							break;

						}

						time = time + timestep;
						cout << "Caluclate HYD-time: "<< time_date.at(i).toStdString()<< " as number " << i << " from " << time_date.count()<< " timesteps ("<<timestep<<")"<<endl;
						Sys_Common_Output::output_dam->output_txt(&cout);
						Dam_Damage_System::check_stop_thread_flag();
						this->set_instat_impact_values_hyd(this->sz_bound_manager.get_ptr_sz(j)->get_id(), this->break_sz, time_date.at(i).toStdString());
						Dam_Damage_System::check_stop_thread_flag();
						this->ci_sys.calculate_instat_damages(this->impact_floodplain, this->number_floodplain_impact, timestep, i);
						Dam_Damage_System::check_stop_thread_flag();
						this->ci_sys.output_instat_result_member2database(&this->qsqldatabase, this->sz_bound_manager.get_ptr_sz(j)->get_id(), this->break_sz, time_date.at(i).toStdString(), this->sz_bound_manager.get_ptr_sz(j)->get_ptr_dam_was_output());


					}

					if (max_timestep_reached == false && this->system_param.until_all_active==true) {
						int counter = time_date.count()-1;
						bool stop_flag = false;
						do {
							if (this->ci_sys.check_points_active_again() == true) {
								stop_flag = true;
							}
							string time_str;
							time = time + timestep;
							time_str=functions::convert_time2time_str_without(time+ start_time_buff);
							cout << "Calculate CI-time: " << time_str << " as timestep number " << counter << " until all CI-elements are active is reached" << endl;
							Sys_Common_Output::output_dam->output_txt(&cout);
							Dam_Damage_System::check_stop_thread_flag();
							this->set_instat_impact_values_hyd(this->sz_bound_manager.get_ptr_sz(j)->get_id(), this->break_sz, time_str);
							Dam_Damage_System::check_stop_thread_flag();
							this->ci_sys.calculate_instat_damages(this->impact_floodplain, this->number_floodplain_impact, timestep, counter);
							Dam_Damage_System::check_stop_thread_flag();
							if (stop_flag == true) {
								bool buff_out = false;
								this->ci_sys.output_instat_result_member2database(&this->qsqldatabase, this->sz_bound_manager.get_ptr_sz(j)->get_id(), this->break_sz, time_str, &buff_out);
							}
							else {
								this->ci_sys.output_instat_result_member2database(&this->qsqldatabase, this->sz_bound_manager.get_ptr_sz(j)->get_id(), this->break_sz, time_str, this->sz_bound_manager.get_ptr_sz(j)->get_ptr_dam_was_output());


							}



							counter++;
							if (counter == this->system_param.max_timesteps) {
								cout << "Maximum timesteps for calculation are reached (" << this->system_param.max_timesteps << ")" << endl;
								Sys_Common_Output::output_dam->output_txt(&cout);
								max_timestep_reached = true;
								break;
							}
						} while (stop_flag == false);
					}
				}

				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch (Error msg) {
				if (Dam_Damage_System::abort_thread_flag == true) {
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
			}
		}
		catch (Error msg) {
			msg.output_msg(4);
			Sys_Common_Output::output_dam->rewind_userprefix();
		}
	}
	else {
		cout << "No hydraulic results are available" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_multi_nobreak_hydsz();

	emit send_hyd_thread_enable(false);




}
//Calculate the instationary damage for the break scenario
void Dam_Damage_System::calc_instat_damage_break_sz(void) {
	ostringstream prefix;
	prefix << "CALC> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);
	emit send_hyd_thread_enable(true);
	this->read_damage_data_per_database();


	ostringstream cout;
	cout << "Calculate the instationary damages for generated break scenarios..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);

	//QSqlQueryModel *model;
	//model=NULL;
	//model=new QSqlQueryModel;
	//check first for hydraulic results
	for (int i = 0; i < this->number_break_sc; i++) {
		if (this->ptr_break_sc_data[i].must_calc == true) {
			if (Hyd_Hydraulic_System::check_hyd_results_calculated(&this->qsqldatabase, this->system_id, this->ptr_break_sc_data[i].id_hyd_sc, this->ptr_break_sc_data[i].break_sc) == false) {
				Warning msg = this->set_warning(3);
				ostringstream info;
				info << "Scenario name  : " << this->ptr_break_sc_data[i].combi_name << endl;
				info << "Scenario id    : " << this->ptr_break_sc_data[i].break_sc << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
				this->ptr_break_sc_data[i].must_calc = false;
			}
		}
	}
	//if(model!=NULL){
	//	delete model;
	//}
	//count the number of scenario for calculation
	this->number_break_sc2calc = 0;
	for (int i = 0; i < this->number_break_sc; i++) {
		if (this->ptr_break_sc_data[i].must_calc == true) {
			this->number_break_sc2calc++;
		}
	}

	if (this->number_break_sc2calc != 0) {
		try {
			//try {
			//	//ecn
			//	ostringstream prefix;
			//	prefix << "ECN> ";
			//	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
			//	//delete the result data
			//	for (int i = 0; i < this->number_break_sc; i++) {
			//		if (this->ptr_break_sc_data[i].must_calc == false) {
			//			continue;
			//		}
			//		this->ecn_sys.delete_result_members_in_database(&this->qsqldatabase, this->system_id, this->ptr_break_sc_data[i].id_hyd_sc, this->ptr_break_sc_data[i].break_sc);
			//	}
			//	//calculation for each ecn raster
			//	for (int i = 0; i < this->ecn_sys.get_number_raster(); i++) {
			//		current_counter = i;
			//		this->ecn_sys.init_damage_rasters(i, &this->qsqldatabase, this->system_id);
			//		for (int j = 0; j < this->number_break_sc; j++) {
			//			if (this->ptr_break_sc_data[j].must_calc == false) {
			//				continue;
			//			}
			//			cout << "Read in the hydraulic impact values for break scenario " << this->ptr_break_sc_data[j].combi_name << "..." << endl;
			//			Sys_Common_Output::output_dam->output_txt(&cout);
			//			this->set_impact_values_hyd(this->ptr_break_sc_data[j].id_hyd_sc, this->ptr_break_sc_data[j].break_sc);
			//			this->ecn_sys.calculate_damages(this->impact_floodplain, this->number_floodplain_impact, i);
			//			Dam_Damage_System::check_stop_thread_flag();
			//			this->ecn_sys.output_result_member2database(&this->qsqldatabase, this->ptr_break_sc_data[j].id_hyd_sc, this->ptr_break_sc_data[j].break_sc, i, &(this->ptr_break_sc_data[j].dam_was_output));
			//		}
			//		this->ecn_sys.damage_raster[i].clear_raster();
			//	}
			//	Sys_Common_Output::output_dam->rewind_userprefix();
			//	prefix.str("");
			//}
			//catch (Error msg) {
			//	this->ecn_sys.damage_raster[current_counter].clear_raster();
			//	if (Dam_Damage_System::abort_thread_flag == true) {
			//		Sys_Common_Output::output_dam->rewind_userprefix();
			//		throw msg;
			//	}
			//	this->number_error++;
			//	msg.output_msg(4);
			//	prefix.str("");
			//	Sys_Common_Output::output_dam->rewind_userprefix();
			//}




			try {
				//sc
				prefix << "CI> ";
				bool max_timestep_reached = false;
				Sys_Common_Output::output_dam->set_userprefix(prefix.str());
				//delete the result data
				for (int i = 0; i < this->number_break_sc; i++) {
					if (this->ptr_break_sc_data[i].must_calc == false) {
						continue;
					}
					this->ci_sys.delete_instat_result_members_in_database(&this->qsqldatabase, this->system_id, this->ptr_break_sc_data[i].id_hyd_sc, this->ptr_break_sc_data[i].break_sc);
				}

				for (int j = 0; j < this->number_break_sc; j++) {
					if (this->ptr_break_sc_data[j].must_calc == false) {
						continue;
					}
					cout << "Read in the instationary impact values for break scenario " << this->ptr_break_sc_data[j].combi_name << "..." << endl;
					Sys_Common_Output::output_dam->output_txt(&cout);

					QStringList time_date;
					Hyd_Element_Floodplain::get_distinct_date_time_instat_results_elements_database(&time_date, &this->qsqldatabase, this->system_id, this->ptr_break_sc_data[j].id_hyd_sc, this->ptr_break_sc_data[j].break_sc);
					cout << time_date.count() << " number of distinct date-time string found in database for break scenario " << this->ptr_break_sc_data[j].id_hyd_sc<< " " << this->ptr_break_sc_data[j].break_sc << " found" << endl;
					Sys_Common_Output::output_dam->output_txt(&cout);



					Hyd_Param_Global buffer_para;

					try {
						buffer_para.globals_per_database(&this->qsqldatabase, false);
					}
					catch (Error msg) {
						throw msg;
					}


					double timestep = 0.0;

					double start_time_buff = buffer_para.get_startime();
					//timestep = Dam_Impact_Value_Floodplain::analyse_date_time(time_date) - start_time_buff;
					timestep = buffer_para.get_stepsize();
					double time = 0.0;

					for (int i = 0; i < time_date.count(); i++) {
						if (i == this->system_param.max_timesteps) {
							cout << "Maximum timesteps for calculation are reached (" << this->system_param.max_timesteps << ")" << endl;
							Sys_Common_Output::output_dam->output_txt(&cout);
							max_timestep_reached = true;
							break;

						}

						time = time + timestep;
						cout << "Caluclate HYD-time: " << time_date.at(i).toStdString() << " as number " << i << " from " << time_date.count() << " timesteps (" << timestep << ")" << endl;
						Sys_Common_Output::output_dam->output_txt(&cout);
						Dam_Damage_System::check_stop_thread_flag();
						this->set_instat_impact_values_hyd(this->ptr_break_sc_data[j].id_hyd_sc, this->ptr_break_sc_data[j].break_sc, time_date.at(i).toStdString());
						Dam_Damage_System::check_stop_thread_flag();
						this->ci_sys.calculate_instat_damages(this->impact_floodplain, this->number_floodplain_impact, timestep, i);
						Dam_Damage_System::check_stop_thread_flag();
						this->ci_sys.output_instat_result_member2database(&this->qsqldatabase, this->ptr_break_sc_data[j].id_hyd_sc, this->ptr_break_sc_data[j].break_sc, time_date.at(i).toStdString(), &(this->ptr_break_sc_data[j].dam_was_output));

					
					}


					if (max_timestep_reached == false && this->system_param.until_all_active == true) {
						int counter = time_date.count() - 1;
						bool stop_flag = false;
						do {
							if (this->ci_sys.check_points_active_again() == true) {
								stop_flag = true;
							}
							string time_str;
							time = time + timestep;
							time_str = functions::convert_time2time_str_without(time + start_time_buff);
							cout << "Caluclate CI-time: " << time_str << " as timestep number " << counter << " until all CI-elements are active is reached" << endl;
							Sys_Common_Output::output_dam->output_txt(&cout);
							Dam_Damage_System::check_stop_thread_flag();
							this->set_instat_impact_values_hyd(this->ptr_break_sc_data[j].id_hyd_sc, this->ptr_break_sc_data[j].break_sc, time_str);
							Dam_Damage_System::check_stop_thread_flag();
							this->ci_sys.calculate_instat_damages(this->impact_floodplain, this->number_floodplain_impact, timestep, counter);
							Dam_Damage_System::check_stop_thread_flag();
							if (stop_flag == true) {
								bool buff_out = false;
								this->ci_sys.output_instat_result_member2database(&this->qsqldatabase, this->ptr_break_sc_data[j].id_hyd_sc, this->ptr_break_sc_data[j].break_sc, time_str, &buff_out);
							}
							else {
								this->ci_sys.output_instat_result_member2database(&this->qsqldatabase, this->ptr_break_sc_data[j].id_hyd_sc, this->ptr_break_sc_data[j].break_sc, time_str, &(this->ptr_break_sc_data[j].dam_was_output));


							}



							counter++;
							if (counter == this->system_param.max_timesteps) {
								cout << "Maximum timesteps for calculation are reached (" << this->system_param.max_timesteps << ")" << endl;
								Sys_Common_Output::output_dam->output_txt(&cout);
								max_timestep_reached = true;
								break;
							}
						} while (stop_flag == false);
					}

				}

				Sys_Common_Output::output_dam->rewind_userprefix();
				prefix.str("");
			}
			catch (Error msg) {
				if (Dam_Damage_System::abort_thread_flag == true) {
					Sys_Common_Output::output_dam->rewind_userprefix();
					throw msg;
				}
				this->number_error++;
				msg.output_msg(4);
				prefix.str("");
				Sys_Common_Output::output_dam->rewind_userprefix();
			}


		}
		catch (Error msg) {
			msg.output_msg(4);
			Sys_Common_Output::output_dam->rewind_userprefix();
		}
	}
	else {
		cout << "No hydraulic results are available" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_multi_break_sz();

	emit send_hyd_thread_enable(false);
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the statistic of the damage system
void Dam_Damage_System::output_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	cout << "Statistics of the DAM-system..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	//ecn
	this->ecn_sys.output_statistic();
	//eco
	this->eco_sys.output_statistic();
	//pop
	this->people_sys.output_statistic();
	//pys
	this->pys_sys.output_statistic();
	//sc
	this->sc_sys.output_statistic();
	//ci
	this->ci_sys.output_statistic();

	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the result members to database
void Dam_Damage_System::output_result_members2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz){
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Damage_System::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Damage_System::erg_table->maximum_int_of_column(Dam_Damage_System::erg_table->get_column_name(dam_label::glob_id),ptr_database)+1;

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Damage_System::erg_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(risk_label::sz_break_id) <<" , ";
	//results
	//ecn
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::immob_dam) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::mob_dam) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::immob_5_quant) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::immob_95_quant) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::mob_5_quant) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::mob_95_quant) <<" , ";
	//eco
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::btype_cost) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::soil_cost) <<" , ";
	//pop
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::pop_affected) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::pop_endangered) <<" , ";
	//pys
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::crit_age_50_59) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::crit_age_80) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::crit_female) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::crit_owner) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_age50_59) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_age_80) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_female) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_owner) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::sc_pub_build) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::sc_eco_build) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::sc_cult_build) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::sc_person_build) <<" , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_elect_pt) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_info_tec_pt) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_sup_pt) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_treat_pt) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_energy_pt) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_health_pt) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_social_pt) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_mat_pt) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_elect_p) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_info_tec_p) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_sup_p) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_treat_p) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_energy_p) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_health_p) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_social_p) << " , ";
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::ci_mat_p) << " ) ";




	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << bound_sz << " , " ;
	query_string << "'"<<break_sz<< "' , " ;
	//results
	//ecn
	query_string << this->ecn_sys.get_immob_results().mid_result << " , " ;
	query_string << this->ecn_sys.get_mob_results().mid_result << " , " ;
	query_string << this->ecn_sys.get_immob_results().quantile_5_result << " , " ;
	query_string << this->ecn_sys.get_immob_results().quantile_95_result << " , " ;
	query_string << this->ecn_sys.get_mob_results().quantile_5_result << " , " ;
	query_string << this->ecn_sys.get_mob_results().quantile_95_result << " , " ;
	//eco
	query_string << this->eco_sys.get_biotope_type_damages() << " , " ;
	query_string << this->eco_sys.get_soil_erosion_damages() << " , " ;
	//pop
	query_string << this->people_sys.get_affected_pop_results() << " , " ;
	query_string << this->people_sys.get_endangered_pop_results() << " , " ;
	//pys
	query_string << this->pys_sys.get_result_crit_age50_59().score_value << " , " ;
	query_string << this->pys_sys.get_result_crit_age80().score_value << " , " ;
	query_string << this->pys_sys.get_result_crit_female().score_value << " , " ;
	query_string << this->pys_sys.get_result_crit_owner().score_value << " , " ;
	query_string << this->pys_sys.get_result_crit_age50_59().score_pop_value << " , " ;
	query_string << this->pys_sys.get_result_crit_age80().score_pop_value << " , " ;
	query_string << this->pys_sys.get_result_crit_female().score_pop_value << " , " ;
	query_string << this->pys_sys.get_result_crit_owner().score_pop_value << " , " ;
	query_string << this->sc_sys.point_manager.get_score_pub_build() << " , " ;
	query_string << this->sc_sys.point_manager.get_score_eco_build() << " , " ;
	query_string << this->sc_sys.point_manager.get_score_cult_build() << " , " ;
	query_string << this->sc_sys.point_manager.get_score_person_build() << " , " ;
	query_string << this->ci_sys.get_result_from_list(1,true) << " , ";
	query_string << this->ci_sys.get_result_from_list(2, true) << " , ";
	query_string << this->ci_sys.get_result_from_list(3, true) << " , ";
	query_string << this->ci_sys.get_result_from_list(4, true) << " , ";
	query_string << this->ci_sys.get_result_from_list(5, true) << " , ";
	query_string << this->ci_sys.get_result_from_list(10, true)+ this->ci_sys.get_result_from_list(11, true) << " , ";
	query_string << this->ci_sys.get_result_from_list(14, true) + this->ci_sys.get_result_from_list(17, true) + this->ci_sys.get_result_from_list(18, true) + this->ci_sys.get_result_from_list(19, true) << " , ";
	query_string << this->ci_sys.get_result_from_list(12, true) + this->ci_sys.get_result_from_list(13, true) + this->ci_sys.get_result_from_list(15, true)+ this->ci_sys.get_result_from_list(16, true) << " , ";
	query_string << this->ci_sys.get_result_from_list(1, false) << " , ";
	query_string << this->ci_sys.get_result_from_list(2, false) << " , ";
	query_string << this->ci_sys.get_result_from_list(3, false) << " , ";
	query_string << this->ci_sys.get_result_from_list(4, false) << " , ";
	query_string << this->ci_sys.get_result_from_list(5, false) << " , ";
	query_string << this->ci_sys.get_result_from_list(10, false) + this->ci_sys.get_result_from_list(11, false) << " , ";
	query_string << this->ci_sys.get_result_from_list(14, false) + this->ci_sys.get_result_from_list(17, false) + this->ci_sys.get_result_from_list(18, false) + this->ci_sys.get_result_from_list(19, false) << " , ";
	query_string << this->ci_sys.get_result_from_list(12, false) + this->ci_sys.get_result_from_list(13, false) + this->ci_sys.get_result_from_list(15, false) + this->ci_sys.get_result_from_list(16, false) << " ) ";




	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if(model.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Dam_Damage_System::erg_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
}
//Create the database table for the results of the damage calculation (static)
void Dam_Damage_System::create_erg_table(QSqlDatabase *ptr_database){
		//Todo add CI
		if(Dam_Damage_System::erg_table==NULL){
			ostringstream cout;
			cout << "Create damage result database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_dam_erg;
			const int num_col=44;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=dam_label::glob_id;
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

			tab_col[3].name=label::applied_flag;
			tab_col[3].type=sys_label::tab_col_type_bool;
			tab_col[3].default_value="true";
			tab_col[3].key_flag=true;

			tab_col[4].name=hyd_label::sz_bound_id;
			tab_col[4].type=sys_label::tab_col_type_int;
			tab_col[4].unsigned_flag=true;
			tab_col[4].key_flag=true;

			tab_col[5].name=risk_label::sz_break_id;
			tab_col[5].type=sys_label::tab_col_type_string;
			tab_col[5].key_flag=true;

			tab_col[6].name=dam_label::immob_dam;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].default_value="0.0";
			tab_col[6].unsigned_flag=true;

			tab_col[7].name=dam_label::mob_dam;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].default_value="0.0";
			tab_col[7].unsigned_flag=true;

			tab_col[8].name=dam_label::immob_5_quant;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=dam_label::immob_95_quant;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";
			tab_col[9].unsigned_flag=true;

			tab_col[10].name=dam_label::mob_5_quant;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].default_value="0.0";
			tab_col[10].unsigned_flag=true;

			tab_col[11].name=dam_label::mob_95_quant;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].default_value="0.0";
			tab_col[11].unsigned_flag=true;

			tab_col[12].name=dam_label::btype_cost;
			tab_col[12].type=sys_label::tab_col_type_double;
			tab_col[12].default_value="0.0";
			tab_col[12].unsigned_flag=true;

			tab_col[13].name=dam_label::soil_cost;
			tab_col[13].type=sys_label::tab_col_type_double;
			tab_col[13].default_value="0.0";
			tab_col[13].unsigned_flag=true;

			tab_col[14].name=dam_label::pop_affected;
			tab_col[14].type=sys_label::tab_col_type_double;
			tab_col[14].default_value="0.0";
			tab_col[14].unsigned_flag=true;

			tab_col[15].name=dam_label::pop_endangered;
			tab_col[15].type=sys_label::tab_col_type_double;
			tab_col[15].default_value="0.0";
			tab_col[15].unsigned_flag=true;

			tab_col[16].name=dam_label::crit_age_50_59;
			tab_col[16].type=sys_label::tab_col_type_double;
			tab_col[16].default_value="0.0";
			tab_col[16].unsigned_flag=true;

			tab_col[17].name=dam_label::crit_age_80;
			tab_col[17].type=sys_label::tab_col_type_double;
			tab_col[17].default_value="0.0";
			tab_col[17].unsigned_flag=true;

			tab_col[18].name=dam_label::crit_female;
			tab_col[18].type=sys_label::tab_col_type_double;
			tab_col[18].default_value="0.0";
			tab_col[18].unsigned_flag=true;

			tab_col[19].name=dam_label::crit_owner;
			tab_col[19].type=sys_label::tab_col_type_double;
			tab_col[19].default_value="0.0";
			tab_col[19].unsigned_flag=true;

			tab_col[20].name=dam_label::denscrit_age50_59;
			tab_col[20].type=sys_label::tab_col_type_double;
			tab_col[20].default_value="0.0";
			tab_col[20].unsigned_flag=true;

			tab_col[21].name=dam_label::denscrit_age_80;
			tab_col[21].type=sys_label::tab_col_type_double;
			tab_col[21].default_value="0.0";
			tab_col[21].unsigned_flag=true;

			tab_col[22].name=dam_label::denscrit_female;
			tab_col[22].type=sys_label::tab_col_type_double;
			tab_col[22].default_value="0.0";
			tab_col[22].unsigned_flag=true;

			tab_col[23].name=dam_label::denscrit_owner;
			tab_col[23].type=sys_label::tab_col_type_double;
			tab_col[23].default_value="0.0";
			tab_col[23].unsigned_flag=true;

			tab_col[24].name=dam_label::sc_pub_build;
			tab_col[24].type=sys_label::tab_col_type_double;
			tab_col[24].default_value="0.0";
			tab_col[24].unsigned_flag=true;

			tab_col[25].name=dam_label::sc_eco_build;
			tab_col[25].type=sys_label::tab_col_type_double;
			tab_col[25].default_value="0.0";
			tab_col[25].unsigned_flag=true;

			tab_col[26].name=dam_label::sc_cult_build;
			tab_col[26].type=sys_label::tab_col_type_double;
			tab_col[26].default_value="0.0";
			tab_col[26].unsigned_flag=true;


			tab_col[27].name = dam_label::sc_person_build;
			tab_col[27].type = sys_label::tab_col_type_double;
			tab_col[27].default_value = "0.0";
			tab_col[27].unsigned_flag = true;



			tab_col[28].name = dam_label::ci_elect_pt;
			tab_col[28].type = sys_label::tab_col_type_double;
			tab_col[28].default_value = "0.0";
			tab_col[28].unsigned_flag = true;

			tab_col[29].name = dam_label::ci_info_tec_pt;
			tab_col[29].type = sys_label::tab_col_type_double;
			tab_col[29].default_value = "0.0";
			tab_col[29].unsigned_flag = true;

			tab_col[30].name = dam_label::ci_water_sup_pt;
			tab_col[30].type = sys_label::tab_col_type_double;
			tab_col[30].default_value = "0.0";
			tab_col[30].unsigned_flag = true;

			tab_col[31].name = dam_label::ci_water_treat_pt;
			tab_col[31].type = sys_label::tab_col_type_double;
			tab_col[31].default_value = "0.0";
			tab_col[31].unsigned_flag = true;

			tab_col[32].name = dam_label::ci_energy_pt;
			tab_col[32].type = sys_label::tab_col_type_double;
			tab_col[32].default_value = "0.0";
			tab_col[32].unsigned_flag = true;



			tab_col[33].name = dam_label::ci_health_pt;
			tab_col[33].type = sys_label::tab_col_type_double;
			tab_col[33].default_value = "0.0";
			tab_col[33].unsigned_flag = true;

			tab_col[34].name = dam_label::ci_social_pt;
			tab_col[34].type = sys_label::tab_col_type_double;
			tab_col[34].default_value = "0.0";
			tab_col[34].unsigned_flag = true;

			tab_col[35].name = dam_label::ci_mat_pt;
			tab_col[35].type = sys_label::tab_col_type_double;
			tab_col[35].default_value = "0.0";
			tab_col[35].unsigned_flag = true;



			tab_col[36].name = dam_label::ci_elect_p;
			tab_col[36].type = sys_label::tab_col_type_double;
			tab_col[36].default_value = "0.0";
			tab_col[36].unsigned_flag = true;

			tab_col[37].name = dam_label::ci_info_tec_p;
			tab_col[37].type = sys_label::tab_col_type_double;
			tab_col[37].default_value = "0.0";
			tab_col[37].unsigned_flag = true;

			tab_col[38].name = dam_label::ci_water_sup_p;
			tab_col[38].type = sys_label::tab_col_type_double;
			tab_col[38].default_value = "0.0";
			tab_col[38].unsigned_flag = true;

			tab_col[39].name = dam_label::ci_water_treat_p;
			tab_col[39].type = sys_label::tab_col_type_double;
			tab_col[39].default_value = "0.0";
			tab_col[39].unsigned_flag = true;

			tab_col[40].name = dam_label::ci_energy_p;
			tab_col[40].type = sys_label::tab_col_type_double;
			tab_col[40].default_value = "0.0";
			tab_col[40].unsigned_flag = true;



			tab_col[41].name = dam_label::ci_health_p;
			tab_col[41].type = sys_label::tab_col_type_double;
			tab_col[41].default_value = "0.0";
			tab_col[41].unsigned_flag = true;

			tab_col[42].name = dam_label::ci_social_p;
			tab_col[42].type = sys_label::tab_col_type_double;
			tab_col[42].default_value = "0.0";
			tab_col[42].unsigned_flag = true;

			tab_col[43].name = dam_label::ci_mat_p;
			tab_col[43].type = sys_label::tab_col_type_double;
			tab_col[43].default_value = "0.0";
			tab_col[43].unsigned_flag = true;





			try{
				Dam_Damage_System::erg_table= new Tables();
				if(Dam_Damage_System::erg_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Damage_System::create_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Dam_Damage_System::close_erg_table();
				throw msg;
			}

		Dam_Damage_System::close_erg_table();
	}
}
//Set the database table for the results of the damage calculation: it sets the table name and the name of the columns and allocate them (static)
void Dam_Damage_System::set_erg_table(QSqlDatabase *ptr_database, const bool not_close){
	if(Dam_Damage_System::erg_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_dam_erg;
		string tab_col[44];

		tab_col[0] =dam_label::glob_id;
		tab_col[1] =label::areastate_id;
		tab_col[2] =label::measure_id;
		tab_col[3] =hyd_label::sz_bound_id;
		tab_col[4] =risk_label::sz_break_id;
		tab_col[5] =dam_label::immob_dam;
		tab_col[6] =dam_label::mob_dam;
		tab_col[7] =dam_label::immob_5_quant;
		tab_col[8] =dam_label::immob_95_quant;
		tab_col[9] =dam_label::mob_5_quant;
		tab_col[10] =dam_label::mob_95_quant;
		tab_col[11] =dam_label::btype_cost;
		tab_col[12] =dam_label::soil_cost;
		tab_col[13] =dam_label::pop_affected;
		tab_col[14] =dam_label::pop_endangered;
		tab_col[15] =dam_label::crit_age_50_59;
		tab_col[16] =dam_label::crit_age_80;
		tab_col[17] =dam_label::crit_female;
		tab_col[18] =dam_label::crit_owner;
		tab_col[19] =dam_label::denscrit_age50_59;
		tab_col[20] =dam_label::denscrit_age_80;
		tab_col[21] =dam_label::denscrit_female;
		tab_col[22] =dam_label::denscrit_owner;
		tab_col[23]=label::applied_flag;
		tab_col[24]=dam_label::sc_pub_build;
		tab_col[25]=dam_label::sc_eco_build;
		tab_col[26]=dam_label::sc_cult_build;
		tab_col[27]=dam_label::sc_person_build;

		tab_col[28] = dam_label::ci_elect_pt;
		tab_col[29]= dam_label::ci_info_tec_pt;
		tab_col[30]= dam_label::ci_water_sup_pt;
		tab_col[31] = dam_label::ci_water_treat_pt;
		tab_col[32] = dam_label::ci_energy_pt;
		tab_col[33] = dam_label::ci_health_pt;
		tab_col[34] = dam_label::ci_social_pt;
		tab_col[35] = dam_label::ci_mat_pt;
		tab_col[36] = dam_label::ci_elect_p;
		tab_col[37] = dam_label::ci_info_tec_p;
		tab_col[38] = dam_label::ci_water_sup_p;
		tab_col[39] = dam_label::ci_water_treat_p;
		tab_col[40] = dam_label::ci_energy_p;
		tab_col[41] = dam_label::ci_health_p;
		tab_col[42] = dam_label::ci_social_p;
		tab_col[43] = dam_label::ci_mat_p;


		try{
			Dam_Damage_System::erg_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Damage_System::erg_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Damage_System::set_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			if (not_close == false) {
				Dam_Damage_System::close_erg_table();
			}
			throw msg;
		}

	}
}
//Delete all data in the database table for the results of the damage calculation (static)
void Dam_Damage_System::delete_data_in_erg_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Damage_System::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Damage_System::erg_table->delete_data_in_table(ptr_database);

	//appending tables
	Dam_Pys_Element::delete_data_in_erg_table(ptr_database);
	Dam_Sc_Point::delete_data_in_erg_table(ptr_database);
	Dam_Eco_Soil_Element::delete_data_in_erg_table(ptr_database);
	Dam_People_Element::delete_data_in_erg_table(ptr_database);
	Dam_Eco_Btype_Element::delete_data_in_erg_table(ptr_database);
	Dam_Ecn_Element::delete_data_in_erg_table(ptr_database);
	Dam_CI_Point::delete_data_in_erg_table(ptr_database);
	Dam_CI_Point::delete_data_in_instat_erg_table(ptr_database);
	Dam_CI_Polygon::delete_data_in_erg_table(ptr_database);
	Dam_CI_Polygon::delete_data_in_instat_erg_table(ptr_database);
}
//Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
void Dam_Damage_System::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id){
		//the table is set (the name and the column names) and allocated
	try{
		Dam_Damage_System::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Damage_System::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Damage_System::erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Damage_System::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Damage_System::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Damage_System::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-id (static)
void Dam_Damage_System::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int hyd_sc){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Damage_System::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Damage_System::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Damage_System::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Damage_System::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int hyd_sc)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Damage_System::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//delete in appending tables
	Dam_Ecn_Element::delete_data_in_erg_table(ptr_database, hyd_sc);
	Dam_Eco_Btype_Element::delete_data_in_erg_table(ptr_database, hyd_sc);
	Dam_Eco_Soil_Element::delete_data_in_erg_table(ptr_database, hyd_sc);
	Dam_People_Element::delete_data_in_erg_table(ptr_database, hyd_sc);
	Dam_Pys_Element::delete_data_in_erg_table(ptr_database, hyd_sc);
	Dam_Sc_Point::delete_data_in_erg_table(ptr_database,hyd_sc);
	Dam_CI_Point::delete_data_in_erg_table(ptr_database, hyd_sc);
	Dam_CI_Point::delete_data_in_instat_erg_table(ptr_database, hyd_sc);
	Dam_CI_Polygon::delete_data_in_erg_table(ptr_database, hyd_sc);
	Dam_CI_Polygon::delete_data_in_instat_erg_table(ptr_database, hyd_sc);
}
//Delete the data in the database table for the results of the damage calculation specified by the scenario-ids (static)
void Dam_Damage_System::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag){
		//the table is set (the name and the column names) and allocated
	try{
		Dam_Damage_System::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Damage_System::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Damage_System::erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Damage_System::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if(like_flag==false){
		query_string << Dam_Damage_System::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		query_string << Dam_Damage_System::erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Damage_System::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
void Dam_Damage_System::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Damage_System::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Damage_System::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Damage_System::erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Damage_System::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_Damage_System::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Damage_System::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Damage_System::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Damage_System::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the results of the damage calculation (static)
void Dam_Damage_System::close_erg_table(void){
	if(Dam_Damage_System::erg_table!=NULL){
		delete Dam_Damage_System::erg_table;
		Dam_Damage_System::erg_table=NULL;
	}
}
//Select the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
int Dam_Damage_System::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Damage_System::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT * FROM " << Dam_Damage_System::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Damage_System::erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Damage_System::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string <<  Dam_Damage_System::erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Damage_System::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Damage_System::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Damage_System::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"' ";
	Data_Base::database_request(query, query_string.str(), ptr_database);
	if(query->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Damage_System::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Damage_System::erg_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return query->rowCount();
}
//Select the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
int Dam_Damage_System::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const int glob_id){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Damage_System::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT * FROM " << Dam_Damage_System::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Damage_System::erg_table->get_column_name(dam_label::glob_id) << "= "<< glob_id;

	Data_Base::database_request(query, query_string.str(), ptr_database);
	if(query->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Damage_System::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const int glob_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Damage_System::erg_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return query->rowCount();
}
//Select the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static
int Dam_Damage_System::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const string like_risk_id){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Damage_System::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT * FROM " << Dam_Damage_System::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Damage_System::erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Damage_System::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string <<  Dam_Damage_System::erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Damage_System::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Damage_System::erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << like_risk_id<<"' ";
	Data_Base::database_request(query, query_string.str(), ptr_database);
	if(query->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Damage_System::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const string like_risk_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Damage_System::erg_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return query->rowCount();
}
//Create the database table for the system parameters (static)
void Dam_Damage_System::create_Dam_system_table(QSqlDatabase *ptr_database) {
	if (Dam_Damage_System::system_table == NULL) {
		ostringstream cout;
		cout << "Create Dam-system database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_dam_system;
		const int num_col = 3;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for (int i = 0; i < num_col; i++) {
			tab_col[i].key_flag = false;
			tab_col[i].unsigned_flag = false;
			tab_col[i].primary_key_flag = false;
		}

		tab_col[0].name = dam_label::glob_id;
		tab_col[0].type = sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag = true;
		tab_col[0].primary_key_flag = true;

		tab_col[1].name = dam_label::max_time_step;
		tab_col[1].type = sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag = true;

		tab_col[2].name = dam_label::all_active_flag;
		tab_col[2].type = sys_label::tab_col_type_bool;
		tab_col[2].default_value = "true";



		try {
			Dam_Damage_System::system_table = new Tables();
			if (Dam_Damage_System::system_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::dam) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_Damage_System::create_Dam_system_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_Damage_System::close_Dam_system_table();
			throw msg;
		}
		Dam_Damage_System::close_Dam_system_table();
		Dam_Damage_System::insert_row_dam_system(ptr_database);
		
	}


}
//Insert a row in the Dam-system table (static)
void Dam_Damage_System::insert_row_dam_system(QSqlDatabase *ptr_database) {
	try {
		Dam_Damage_System::set_Dam_system_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Damage_System::system_table->get_table_name();
	query_string << " ( ";
	query_string << Dam_Damage_System::system_table->get_column_name(dam_label::glob_id) << " , ";
	query_string << Dam_Damage_System::system_table->get_column_name(dam_label::max_time_step) << " , ";
	query_string << Dam_Damage_System::system_table->get_column_name(dam_label::all_active_flag) << " ) ";


	query_string << " VALUES ";

	query_string << " ( ";
	query_string << 1 << " , ";
	query_string << 1000 << " , ";
	query_string << "'" << functions::convert_boolean2string(true) << "')";


	string buffer;
	buffer = query_string.str();
	QSqlQuery query_buff(*ptr_database);
	Data_Base::database_request(&query_buff, buffer, ptr_database);


	Dam_Damage_System::close_Dam_system_table();

}
//Set the database table for the system parameters: it sets the table name and the name of the columns and allocate them (static)
void Dam_Damage_System::set_Dam_system_table(QSqlDatabase *ptr_database, const bool not_close) {
	if (Dam_Damage_System::system_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_dam_system;
		string tab_col[3];

		tab_col[0] = dam_label::glob_id;
		tab_col[1] = dam_label::max_time_step;
		tab_col[2] = dam_label::all_active_flag;
		

		try {
			Dam_Damage_System::system_table = new Tables(tab_id_name, tab_col, sizeof(tab_col) / sizeof(tab_col[0]));
			Dam_Damage_System::system_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("set_Dam_system_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			if (not_close == false) {
				Dam_Damage_System::close_Dam_system_table();
			}
			throw msg;
		}
	}


}
//Close and delete the database table for the Dam-system parameters (static)
void Dam_Damage_System::close_Dam_system_table(void) {
	if (Dam_Damage_System::system_table != NULL) {
		delete Dam_Damage_System::system_table;
		Dam_Damage_System::system_table = NULL;
	}
}
//Set system parameters by database table
void Dam_Damage_System::set_Dam_system_parameters_db(void) {
	QSqlQueryModel results;
	int number = 0;
	try {
		Dam_Damage_System::set_Dam_system_table(&this->qsqldatabase);
	}
	catch (Error msg) {
		throw msg;
	}

	ostringstream cout;
	cout << "Set the Dam-system parameters in database ..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);



	ostringstream test_filter;
	test_filter << "Select ";
	test_filter << Dam_Damage_System::system_table->get_column_name(dam_label::glob_id) << " , ";
	test_filter << Dam_Damage_System::system_table->get_column_name(dam_label::max_time_step) << " , ";
	test_filter << Dam_Damage_System::system_table->get_column_name(dam_label::all_active_flag) << "  ";

	test_filter << " from " << Dam_Damage_System::system_table->get_table_name();


	Data_Base::database_request(&results, test_filter.str(), &this->qsqldatabase);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_Damage_System::set_Dam_system_parameters_db(void)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Damage_System::system_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number = results.rowCount();


	if (number == 0) {
		this->system_param.max_timesteps = 1000;
		this->system_param.until_all_active = true;
	}
	else {
		this->system_param.max_timesteps = results.record(0).value((Dam_Damage_System::system_table->get_column_name(dam_label::max_time_step)).c_str()).toInt();
		this->system_param.until_all_active = results.record(0).value((Dam_Damage_System::system_table->get_column_name(dam_label::all_active_flag)).c_str()).toBool();


	}
}
//Check if all damage raster are connected to the hydraulic system (static)
bool Dam_Damage_System::check_all_raster_connected2hyd(QSqlDatabase *ptr_database, string *txt, _sys_system_id id){
	//reset txt
	*txt="";

	ostringstream buffer;

	if(Dam_Ecn_Raster::check_all_raster_connected2hyd(ptr_database)==false){
		buffer<<"Not all economical damage raster (ECN) are connected to the HYD-system."<<endl;
	}
	if(Dam_Eco_Btype_Raster::check_all_raster_connected2hyd(ptr_database)==false){
		buffer<<"Not all ecological biotope-type damage raster (ECO-BTYPE) are connected to the HYD-system."<<endl;
	}
	if(Dam_Eco_Soil_Raster::check_all_raster_connected2hyd(ptr_database)==false){
		buffer<<"Not all ecological soil-erosion damage raster (ECO-SOIL) are connected to the HYD-system."<<endl;
	}
	if(Dam_People_Raster::check_all_raster_connected2hyd(ptr_database)==false){
		buffer<<"Not all people2risk damage raster (POP) are connected to the HYD-system."<<endl;
	}
	if(Dam_Pys_Raster::check_all_raster_connected2hyd(ptr_database)==false){
		buffer<<"Not all psycho-social damage raster (PYS) are connected to the HYD-system."<<endl;
	}
	if(Dam_Sc_Point_Manager::check_connection2hydraulic(ptr_database, id)==false){
		buffer<<"Not all simple-counting damage points (SC) are connected to the HYD-system."<<endl;
	}
	if (Dam_CI_System::check_connection2hydraulic(ptr_database, id) == false) {
		buffer << "Not all CI-points (CI) are connected to the HYD-system." << endl;
	}

	*txt=buffer.str();
	if(txt->empty()==true){
		return true;
	}
	else{
		return false;
	}
}
//Reset the connection to the hydraulic system; (just the connected flag is set to false) (static)
void Dam_Damage_System::reset_raster_hyd_connection_flag(QSqlDatabase *ptr_database){
	Dam_Ecn_Raster::reset_hydraulic_connection_flag(ptr_database);
	Dam_Eco_Btype_Raster::reset_hydraulic_connection_flag(ptr_database);
	Dam_Eco_Soil_Raster::reset_hydraulic_connection_flag(ptr_database);
	Dam_People_Raster::reset_hydraulic_connection_flag(ptr_database);
	Dam_Pys_Raster::reset_hydraulic_connection_flag(ptr_database);
	Dam_Sc_Point_Manager::reset_hydraulic_connection_flag(ptr_database);
}
//Reset the connection to the hydraulic system in elements (static)
void Dam_Damage_System::reset_raster_hyd_connection_elements(QSqlDatabase *ptr_database, _dam_raster_types type){
	if(type==_dam_raster_types::all_raster || type==_dam_raster_types::ecn_total){
		Dam_Ecn_Raster::reset_hydraulic_connection_elements(ptr_database);
	}
	if(type==_dam_raster_types::all_raster ||  type==_dam_raster_types::eco_all ){	
		Dam_Eco_Btype_Raster::reset_hydraulic_connection_elements(ptr_database);
		Dam_Eco_Soil_Raster::reset_hydraulic_connection_elements(ptr_database);
	}
	if(type==_dam_raster_types::all_raster ||  type==_dam_raster_types::pop_total ){	
		Dam_People_Raster::reset_hydraulic_connection_elements(ptr_database);
	}
	if(type==_dam_raster_types::all_raster || type==_dam_raster_types::pys_pys_total ){	
		Dam_Pys_Raster::reset_hydraulic_connection_elements(ptr_database);
	}
	
}
//Switch the applied-flag for the damage results in the database table for a defined system state (static)
void Dam_Damage_System::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Damage_System::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Damage_System::erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Damage_System::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Damage_System::erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Damage_System::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Damage_System::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Damage_System::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Damage_System::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//appending elements
	Dam_Ecn_Element::switch_applied_flag_erg_table(ptr_database, id, flag);
	Dam_Eco_Btype_Element::switch_applied_flag_erg_table(ptr_database, id, flag);
	Dam_Eco_Soil_Element::switch_applied_flag_erg_table(ptr_database, id, flag);
	Dam_People_Element::switch_applied_flag_erg_table(ptr_database, id, flag);
	Dam_Pys_Element::switch_applied_flag_erg_table(ptr_database, id, flag);
	Dam_Sc_Point::switch_applied_flag_erg_table(ptr_database, id, flag);
	Dam_CI_Point::switch_applied_flag_erg_table(ptr_database, id, flag);
	Dam_CI_Polygon::switch_applied_flag_erg_table(ptr_database, id, flag);
}
//Switch the applied-flag for the damage results in the database table for a defined system state (static)
void Dam_Damage_System::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
//the table is set (the name and the column names) and allocated
	try{
		Dam_Damage_System::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Damage_System::erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Damage_System::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Damage_System::erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Damage_System::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Dam_Damage_System::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;
	query_string  << " AND ";
	query_string  << Dam_Damage_System::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Damage_System::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Damage_System::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//appending elements
	Dam_Ecn_Element::switch_applied_flag_erg_table(ptr_database, id, hyd_sc, flag);
	Dam_Eco_Btype_Element::switch_applied_flag_erg_table(ptr_database, id, hyd_sc,flag);
	Dam_Eco_Soil_Element::switch_applied_flag_erg_table(ptr_database, id, hyd_sc,flag);
	Dam_People_Element::switch_applied_flag_erg_table(ptr_database, id, hyd_sc,flag);
	Dam_Pys_Element::switch_applied_flag_erg_table(ptr_database, id, hyd_sc, flag);
	Dam_Sc_Point::switch_applied_flag_erg_table(ptr_database, id, hyd_sc, flag);
	Dam_CI_Point::switch_applied_flag_erg_table(ptr_database, id, hyd_sc, flag);
	Dam_CI_Polygon::switch_applied_flag_erg_table(ptr_database, id, hyd_sc, flag);
}
//Copy the results of a given system id to another one in database tables
void Dam_Damage_System::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Damage_System::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Dam_Damage_System::erg_table->maximum_int_of_column(Dam_Damage_System::erg_table->get_column_name(dam_label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Dam_Damage_System::erg_table->get_column_name(dam_label::glob_id);
	test_filter << " from " << Dam_Damage_System::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Damage_System::erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Dam_Damage_System::erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Damage_System::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Damage_System::erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	ostringstream cout;
	cout <<"Copy "<<model.rowCount()<<" total results of the damages to the new measure state..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
		test_filter.str("");
		test_filter << "INSERT INTO "<< Dam_Damage_System::erg_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(hyd_label::sz_bound_id)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(risk_label::sz_break_id)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::immob_dam)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::mob_dam)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::immob_5_quant)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::immob_95_quant)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::mob_5_quant)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::mob_95_quant)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::btype_cost)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::soil_cost)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::pop_affected)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::pop_endangered)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::crit_age_50_59)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::crit_age_80)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::crit_female)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::crit_owner)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_age50_59)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_age_80)  <<" , ";
		test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_female)  <<" , ";
        test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_owner)  <<" , ";

        test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::sc_pub_build)  <<" , ";
        test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::sc_pub_build)  <<" , ";
        test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::sc_cult_build)  <<" , ";
        test_filter <<  Dam_Damage_System::erg_table->get_column_name(dam_label::sc_person_build)  <<"  ";



		test_filter << " FROM " << Dam_Damage_System::erg_table->get_table_name() <<" ";
		test_filter << " WHERE " << Dam_Damage_System::erg_table->get_column_name(dam_label::glob_id) << " = ";
		test_filter << model.record(i).value(Dam_Damage_System::erg_table->get_column_name(dam_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Dam_Damage_System::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Dam_Damage_System::erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}

	try{
		Dam_Ecn_Element::copy_results(ptr_database, src,  dest);
		Dam_Eco_Btype_Element::copy_results(ptr_database, src,  dest);
		Dam_Eco_Soil_Element::copy_results(ptr_database, src,  dest);
		Dam_People_Element::copy_results(ptr_database, src,  dest);
		Dam_Pys_Element::copy_results(ptr_database, src,  dest);
		Dam_Sc_Point::copy_results(ptr_database, src,  dest);
		Dam_CI_Point::copy_results(ptr_database, src, dest);
		Dam_CI_Point::copy_instat_results(ptr_database, src, dest);
		Dam_CI_Polygon::copy_results(ptr_database, src, dest);
		Dam_CI_Polygon::copy_instat_results(ptr_database, src, dest);

		ostringstream cout;
		cout <<"Switch the applied-flags of the damage results..."<<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);

		Dam_Damage_System::switch_applied_flag_erg_table(ptr_database, src, false);
	}
	catch(Error msg){
		throw msg;
	}
}
//Select hydraulic boundary scenarios, which damage results have not yet calculated
int Dam_Damage_System::select_scenarios_without_dam_result(QSqlDatabase *ptr_database, const bool just_hydraulic){
	QSqlQueryModel model;
	int counter=0;

	//just for the hydraulic boundary scenarios
	if(just_hydraulic==true){
		this->sz_bound_manager.set_szenario_per_db(ptr_database);

		for(int i=0; i< this->sz_bound_manager.get_number_sz(); i++){
			if(Dam_Damage_System::select_data_in_erg_table(&model, ptr_database, this->system_id, this->sz_bound_manager.get_ptr_sz(i)->get_id(), this->break_sz )<1 &&
				Hyd_Hydraulic_System::check_hyd_results_calculated(ptr_database, this->system_id, this->sz_bound_manager.get_ptr_sz(i)->get_id(), this->break_sz )==true){
				//calculation is required
				this->sz_bound_manager.get_ptr_sz(i)->set_is_selected(true);
				counter++;
			}
			else{
				this->sz_bound_manager.get_ptr_sz(i)->set_is_selected(false);
			}
		}
	}
	//for break scenarios
	else{
		for(int i=0; i< this->number_break_sc; i++){
			if(Dam_Damage_System::select_data_in_erg_table(&model, ptr_database, this->system_id, this->ptr_break_sc_data[i].id_hyd_sc, this->ptr_break_sc_data[i].break_sc )<1){
				//calculation is required
				this->ptr_break_sc_data[i].must_calc=true;
				counter++;
			}
			else{
				this->ptr_break_sc_data[i].must_calc=false;
			}
		}
		this->number_break_sc2calc=counter;
	}
	return counter;
}
//Set the system-id (_sys_system_id) of the object
void Dam_Damage_System::set_systemid(const _sys_system_id actuel_id){
	_Sys_Common_System::set_systemid(actuel_id);
	this->sz_bound_manager.set_systemid(actuel_id);
}
//Set identifier string for the break scenario
void Dam_Damage_System::set_string_break_sc(const string break_sc){
	this->break_sz=break_sc;
}
//Set an array of break scenarios, which are calculated
void Dam_Damage_System::set_break_sc_array(const int number, _dam_break2nobreak_sc *ptr_break_sc){
	this->number_break_sc=number;
	this->ptr_break_sc_data=ptr_break_sc;
}
//Ask per dialog (HydGui_Boundary_Scenario_Dia), which of the availabe hydraulic boundary scenarios should be handled and return the number
int Dam_Damage_System::ask_boundary_scenarios_per_dialog(QSqlDatabase *ptr_database, QWidget *parent){
	ostringstream buff;
	if(this->thread_type==_dam_thread_type::dam_sys_calc_no_break_sz){
		buff <<"Select scenario(s) for nobreak damage calculation \nfrom the available hydraulic boundary scenario(s)" <<endl;
	}
	else if (this->thread_type == _dam_thread_type::dam_sys_instat_calc_no_break_sz) {
		buff << "Select scenario(s) for instationary nobreak damage calculation \nfrom the available hydraulic boundary scenario(s)" << endl;
	}
	else{
		buff <<"Select scenario(s) for unknown handling from the available hydraulic boundary scenario(s)" <<endl;
	}

	this->sz_bound_manager.set_szenario_per_db(ptr_database);
	Hyd_Boundary_Scenario_List my_list;
	for(int i=0; i< this->sz_bound_manager.get_number_sz(); i++){
		if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
			my_list.add_scenario2list(this->sz_bound_manager.get_ptr_sz(i));
		}
		else{
			if (this->thread_type == _dam_thread_type::dam_sys_calc_no_break_sz) {
				if (Hyd_Hydraulic_System::check_hyd_results_calculated(ptr_database, this->system_id, this->sz_bound_manager.get_ptr_sz(i)->get_id(), "CA") == true) {
					my_list.add_scenario2list(this->sz_bound_manager.get_ptr_sz(i));
				}

			}
			else if(this->thread_type == _dam_thread_type::dam_sys_instat_calc_no_break_sz) {
				if (Hyd_Hydraulic_System::check_hyd_instat_results_calculated(ptr_database, this->system_id, this->sz_bound_manager.get_ptr_sz(i)->get_id(), "CA") == true) {
					my_list.add_scenario2list(this->sz_bound_manager.get_ptr_sz(i));
				}
			}
		}
	}

	int number_selected=0;
	this->sz_bound_manager.ask_boundary_scenarios_per_dialog( buff.str(), parent, &my_list, &number_selected);
	return number_selected;
}
//Get the flag if the thread has started
bool Dam_Damage_System::get_thread_has_started(void){
	return this->thread_is_started;
}
//Input the polygons for raster interception per file
void Dam_Damage_System::input_polygons_per_file(const string file_name){
	bool found_flag=false;
	//file
	QFile poly_file;
	//count the lines in file
	int line_counter=0;

	//open file
	poly_file.setFileName(file_name.c_str());
	QIODevice::OpenMode my_flag;
	my_flag=my_flag|(QIODevice::ReadOnly);
	my_flag=my_flag|(QIODevice::Text);
	poly_file.open(my_flag);

	if(poly_file.isOpen()==false){
		ostringstream info;
		Error msg=this->set_error(3);
		info << "Filename is: " << file_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//find number of polygons in file
	string myline;
	QString qmyline;
	ostringstream cout;
	cout << "Input the polygon data for interception..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	do{
		qmyline=poly_file.readLine();
		myline=qmyline.toStdString();
		line_counter++;
		functions::clean_string(&myline);
		qmyline=myline.c_str();

		if(myline=="!BEGIN"){
			break;
		}
		else if(qmyline.contains(dam_label::number_polygons.c_str())==true){
			stringstream buff;
			buff<<myline;
			buff>> myline >> this->number_polygons;
			if(buff.fail()==true){
				Error msg=this->set_error(4);
				ostringstream info;
				info << "Searched key word " << dam_label::number_polygons << endl;
				info << "Wrong input sequenze " << buff.str() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}

			found_flag=true;
			break;
		}
	}
	while(poly_file.atEnd()==false);

	if(found_flag==false){
		//number of polygons not found
		Error msg=this->set_error(5);
		ostringstream info;
		msg.make_second_info(info.str());
		throw msg;
	}

	//allocate polygons
	try{
		this->allocate_polygons();
	}
	catch(Error msg){
		throw msg;
	}
	//close file
	poly_file.close();

	//input the polygons
	try{
		for(int i=0; i< this->number_polygons; i++){
			this->polygons[i].input_members(i, file_name);
		}
	}
	catch (Error msg) {
		throw msg;
	}
}
//Intercept the damage raster with the polygons for the measure "resettlement"
void Dam_Damage_System::intercept_raster_resettlement(QList <int> btype_replace){
	ostringstream cout;
	cout << "Intercept the damage raster of each damage type among each other..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	QList <int> buff_current;
	QList <int> buff_new;
	//set the ecn values to zero (no-info)
	this->intercept_ecn_raster(buff_current, buff_new);
	this->intercept_eco_btype_raster(buff_current, btype_replace);
	this->intercept_pop_category_raster(buff_current, buff_new);
	this->intercept_pys_category_raster(buff_current, buff_new);
	cout << "Interception is finished" << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
}
//Intercept the economical damage raster with the polygons for the measure "change ECN-damage function"
void Dam_Damage_System::intercept_ecn_raster(QList <int> current_id, QList <int> new_id){
	ostringstream cout;
	cout << "Intercept "<<this->ecn_sys.get_number_raster()<<" economical damage raster with "<< this->number_polygons;
	if(current_id.count()==0 && new_id.count()==0){
		cout << ", where the intercepted elements are set to no-info value"<<endl;
	}
	else{
		cout << ", where the intercepted elements are set to: "<<endl;
		cout << " no. "<< W(5)<<" ID_landuse_old --> ID_landuse_new "<<endl;
		for(int i=0; i<current_id.count(); i++){
			cout << " "<<i << W(10)<<current_id.at(i)<<W(10)<< new_id.at(i)<<endl;
		}
	}
	Sys_Common_Output::output_dam->output_txt(&cout);

	try{
		this->ecn_sys.read_damage_raster_per_database_general(&this->qsqldatabase);
		//intercept it and change landuse id
		this->ecn_sys.intercept_polygons2landuse_change(&this->qsqldatabase, this->system_id, this->number_polygons, this->polygons, current_id, new_id);
	}
	catch(Error msg){
		throw msg;
	}
	cout << "Interception of the economical damage raster with "<< this->number_polygons << " polygon(s) is finished!"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
}
//Intercept the ecological biotope type raster with the polygons for the measure "change ECO-biotope type"
void Dam_Damage_System::intercept_eco_btype_raster(QList <int> current_id, QList <int> new_id){
	ostringstream cout;
	cout << "Intercept "<<this->eco_sys.get_number_biotope_raster()<<" ecological biotope-type raster with "<< this->number_polygons;
	if(current_id.count()==0 && new_id.count()==0){
		cout << ", where the intercepted elements are set to no-info value"<<endl;
	}
	else{
		cout << ", where the intercepted elements are set to: "<<endl;
		cout << " no. "<< W(5)<<" ID_biotope-type_old --> ID_biotope-type_new "<<endl;
		for(int i=0; i<current_id.count(); i++){
			cout << " "<<i << W(10)<<current_id.at(i)<<W(10)<< new_id.at(i)<<endl;
		}
	}
	Sys_Common_Output::output_dam->output_txt(&cout);

	try{
		this->eco_sys.read_biotope_raster_per_database_general(&this->qsqldatabase);
		//intercept it and change biotope-type id
		this->eco_sys.intercept_polygons2biotope_type_change(&this->qsqldatabase, this->system_id, this->number_polygons, this->polygons, current_id, new_id);
	}
	catch(Error msg){
		throw msg;
	}
	cout << "Interception of the ecological biotope-type raster with "<< this->number_polygons << " polygon(s) is finished!"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
}
//Intercept the ecological soil type raster with the polygons for the measure "change ECO-soil type"
void Dam_Damage_System::intercept_eco_soil_raster(QList <int> current_id, QList <int> new_id){
	ostringstream cout;
	cout << "Intercept "<<this->eco_sys.get_number_soil_raster()<<" ecological soil-erosion raster with "<< this->number_polygons;
	if(current_id.count()==0 && new_id.count()==0){
		cout << ", where the intercepted elements are set to no-info value"<<endl;
	}
	else{
		cout << ", where the intercepted elements are set to: "<<endl;
		cout << " no. "<< W(5)<<" ID_soil-type_old --> ID_soil-type_new "<<endl;
		for(int i=0; i<current_id.count(); i++){
			cout << " "<<i << W(10)<<current_id.at(i)<<W(10)<< new_id.at(i)<<endl;
		}
	}
	Sys_Common_Output::output_dam->output_txt(&cout);

	try{
		this->eco_sys.read_soil_type_erosion_raster_per_database_general(&this->qsqldatabase);
		//intercept it and change soil-type (cost) id
		this->eco_sys.intercept_polygons2soil_type_change(&this->qsqldatabase, this->system_id, this->number_polygons, this->polygons, current_id, new_id);
	}
	catch(Error msg){
		throw msg;
	}
	cout << "Interception of the ecological soil-erosion raster with "<< this->number_polygons << " polygon(s) is finished!"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
}
//Intercept the people2risk category raster with the polygons for the measure "change POP category"
void Dam_Damage_System::intercept_pop_category_raster(QList <int> current_id, QList <int> new_id){
	ostringstream cout;
	cout << "Intercept "<<this->people_sys.get_number_raster()<<" peopel2risk raster with "<< this->number_polygons;
	if(current_id.count()==0 && new_id.count()==0){
		cout << ", where the intercepted elements are set to no-info value"<<endl;
	}
	else{
		cout << ", where the intercepted elements are set to: "<<endl;
		cout << " no. "<< W(5)<<" ID_category_old --> ID_category_new "<<endl;
		for(int i=0; i<current_id.count(); i++){
			cout << " "<<i << W(10)<<current_id.at(i)<<W(10)<< new_id.at(i)<<endl;
		}
	}
	Sys_Common_Output::output_dam->output_txt(&cout);

	try{
		this->people_sys.read_people2risk_raster_per_database_general(&this->qsqldatabase);
		//intercept it and change category
		this->people_sys.intercept_polygons2category_change(&this->qsqldatabase, this->system_id, this->number_polygons, this->polygons, current_id, new_id);
	}
	catch(Error msg){
		throw msg;
	}
	cout << "Interception of the people2risk raster with "<< this->number_polygons << " polygon(s) is finished!"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
}
//Intercept the psycho-social category raster with the polygons for the measure "change PYS category"
void Dam_Damage_System::intercept_pys_category_raster(QList <int> current_id, QList <int> new_id){
	ostringstream cout;
	cout << "Intercept "<<this->pys_sys.get_number_raster()<<" psycho-social raster with "<< this->number_polygons;
	if(current_id.count()==0 && new_id.count()==0){
		cout << ", where the intercepted elements are set to no-info value"<<endl;
	}
	else{
		cout << ", where the intercepted elements are set to: "<<endl;
		cout << " no. "<< W(5)<<" ID_category_old --> ID_category_new "<<endl;
		for(int i=0; i<current_id.count(); i++){
			cout << " "<<i << W(10)<<current_id.at(i)<<W(10)<< new_id.at(i)<<endl;
		}
	}
	Sys_Common_Output::output_dam->output_txt(&cout);

	try{
		this->pys_sys.read_damage_raster_per_database_general(&this->qsqldatabase);
		//intercept it and change psycho-social category
		this->pys_sys.intercept_polygons2category_change(&this->qsqldatabase, this->system_id, this->number_polygons, this->polygons, current_id, new_id);
	}
	catch(Error msg){
		throw msg;
	}
	cout << "Interception of the psycho-social raster with "<< this->number_polygons << " polygon(s) is finished!"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
}
//Ask for the file(s) of the hydraulic result raster(s) per dialog for multiple floodplains
bool Dam_Damage_System::ask_file_hyd_res_raster_multi_fp(const bool base_sc){
	this->hyd_base_sc_import=base_sc;

	if(this->hyd_base_sc_import==true){
		//ask number floodplain raster per dialog
		DamGui_Number_Raster_Dia my_dia;
		QIcon icon;
		icon.addFile(":hyd_icon");
		my_dia.change_window_title("Number of raster representing the floodplains",icon);
		my_dia.change_main_text("Set the number of hydraulic result raster as foodplains, \nwhich should be imported!");
		if(my_dia.start_dialog()==false){
			return false;
		}
		if(my_dia.get_number_raster()==0){
			return false;
		}
		this->number_hyd_imp_dia=my_dia.get_number_raster();
	}
	else{
		//check the number of rasters in the base scenario
		this->number_hyd_imp_dia=Hyd_Model_Floodplain::check_relevant_model_database(&this->qsqldatabase);
	}
	//ask the multiple raster
	this->allocate_hyd_res_import_dia_multi_fp();

	QIcon icon;
	icon.addFile(":dam_icon");

	for(int i=0; i<this->number_hyd_imp_dia; i++){
		this->multi_fp_hyd_raster_import_dia[i].set_number_file_browser(5,icon);
		stringstream buffer;
		if(this->hyd_base_sc_import==true){
			buffer << "Choose the file(s), where the HYD results of the base scenario"<<endl;
		}
		else{
			buffer << "Choose the file(s), where the additional HYD results "<<endl;
		}
		buffer << " ( floodplain " << i+1 << " of " << this->number_hyd_imp_dia << ") are stored."<< endl;
		buffer << "The files of the flow-velocity v, duration d and velocity of waterlevel rise dh/dt are optional."<< endl;
		buffer << "If they are not set, tha values are set to zero."<< endl;
		this->multi_fp_hyd_raster_import_dia[i].set_main_text_label(buffer.str());
		buffer.str("");
		buffer << "Choose HYD results file(s) for floodplain "<< i;
		this->multi_fp_hyd_raster_import_dia[i].set_window_title(buffer.str());
		buffer.str("");

		this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(0)->set_text_label("Raster file of the relative waterlevel h [m]");
		this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(0)->set_tooltip("Information of the maximum waterlevel h [m] in the floodplain elements measured above the geographic surface");

		this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(1)->set_text_label("Raster file of the geodetic height z [m] [optional by setting the base-scenario]");
		this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(1)->set_tooltip("Information of the geodetic height z of the floodplain elements [m], used to compute absolute waterlevel s [m]");
		if(this->hyd_base_sc_import==false){
			this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(1)->setEnabled(false);
		}

		this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(2)->set_text_label("Raster file of the maximum flow velocity vmax [m/s] [optional]");
		this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(2)->set_tooltip("Information of the maximum flow velocity vmax [m/s] of the floodplain elements, used in the People2risk or the ecological damage calculation");

		this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(3)->set_text_label("Raster file of duration of flooding d [s] [optional]");
		this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(3)->set_tooltip("Information of the flood duration d [s] of the floodplain elements, used in the ecological damage calculation");

		this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(4)->set_text_label("Raster file of speed of waterlevel rise dh/dt [m/s] [optional]");
		this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(4)->set_tooltip("Information of speed of waterlevel rise dh/dt [m/s] of the floodplain elements, used in the People2risk calculation");

		//start dialog
		if(this->multi_fp_hyd_raster_import_dia[i].start_dialog()==false){
			return false;
		}
	}

	if(this->number_hyd_imp_dia>0 && this->multi_fp_hyd_raster_import_dia[0].get_ptr_file_browser(0)->get_file_name()!=label::not_set){
		if(this->sz_bound_manager.ask_scenario_data_per_dialog(1,&(this->multi_fp_hyd_raster_import_dia[0].get_ptr_file_browser(0)->get_file_name()),&this->qsqldatabase, this->hyd_base_sc_import,NULL)==true){
			if(this->hyd_base_sc_import==true){
				this->sz_bound_manager.delete_data_in_table(&this->qsqldatabase);
			}
			return true;
		}
		else{
			return false;
		}
	}
	return true;
}
//___________
//private
//Read in the damage calculation data from database
void Dam_Damage_System::read_damage_data_per_database(void){
	ostringstream prefix;
	prefix << "INP>";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	try{
		//ecn
		this->ecn_sys.read_damage_system_per_database(&this->qsqldatabase);
		//eco
		this->eco_sys.read_system_per_database(&this->qsqldatabase);
		//pop
		this->people_sys.read_people2risk_system_per_database(&this->qsqldatabase);
		//pys
		this->pys_sys.read_system_per_database(&this->qsqldatabase);

		//sc
		this->sc_sys.read_system_per_database(&this->qsqldatabase, this->system_id);

		//ci
		this->ci_sys.read_system_per_database(&this->qsqldatabase, this->system_id);
		this->ci_sys.init_system();


	}
	catch(Error msg){
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Set the number of warnings before an action
void Dam_Damage_System::set_start_warnings_number(void){
	this->number_warning_begin=Warning::get_number_dam_warnings();
}
//Set the warning number, which occured during an action
void Dam_Damage_System::set_warning_number(void){
	this->number_warning=Warning::get_number_dam_warnings()-this->number_warning_begin;
}
//Get the warning number, which occurred during an action
int Dam_Damage_System::get_occured_warning(void){
	return this->number_warning;
}
//Output the error statistic of the damage system
void Dam_Damage_System::output_error_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	cout << "Error statistics of the DAM-system..." << endl ;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)                 :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)               :" << W(3) << this->get_occured_warning() <<endl;

	if(Dam_Damage_System::get_stop_thread_flag()==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_dam->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the error statistic and the needed time of the damage system
void Dam_Damage_System::output_error_time_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	cout << "Error statistics of the DAM-system..." << endl ;
	cout << " Time              :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)                 :" << W(3) << this->number_error +this->number_hyd_errors<<endl;
	cout << " Warning(s)               :" << W(3) << this->get_occured_warning() +this->number_hyd_warnings<<endl;
	if(this->number_hyd_errors!=0 || this->number_hyd_warnings!=0){
		cout << "DETAILED ERROR-/WARNING-NUMBER"<< endl;
		if(this->number_error>0){
			cout << " Error(s) DAM             :" << W(3) << this->number_error <<endl;
		}
		if(this->get_occured_warning()>0){
			cout << " Warning(s) DAM           :" << W(3) << this->get_occured_warning() <<endl;
		}
		if(this->number_hyd_errors>0){
			cout << " Error(s) HYD             :" << W(3) << this->number_hyd_errors <<endl;
		}
		if(this->number_hyd_warnings>0){
			cout << " Warning(s) HYD           :" << W(3) << this->number_hyd_warnings <<endl;
		}
	}

	if(Dam_Damage_System::get_stop_thread_flag()==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_dam->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the statistic of the damage calculation for selected nobreak-scenarios
void Dam_Damage_System::output_final_statistic_multi_nobreak_hydsz(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	cout << "Statistics of the multiple nobreak hydraulic boundary scenario(s)..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Number scenario(s):" << W(3) << this->sz_bound_manager.counter_number_selected_scenarios() <<endl;
	cout << " Time              :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)          :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)        :" << W(3) << this->number_warning <<endl;
	if(abort_thread_flag==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_dam->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the statistic of the damage calculation for generated nbreak-scenarios
void Dam_Damage_System::output_final_statistic_multi_break_sz(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	cout << "Statistics of the multiple break scenario(s)..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Number scenario(s):" << W(3) << this->number_break_sc2calc <<endl;
	cout << " Time              :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)          :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)        :" << W(3) << this->number_warning <<endl;
	if(abort_thread_flag==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_dam->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Check the damage system
void Dam_Damage_System::check_damage_system(void){
	this->set_start_warnings_number();
	//begin time recording
	time(&this->start_time);

	ostringstream cout;
	ostringstream prefix;
	cout <<"Check of damage system in database..."<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	//set prefix
	prefix << "CHECK_SYS> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);
	try{
		//ecn
		prefix << "ECN> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
		this->ecn_sys.check_system();
		Sys_Common_Output::output_dam->rewind_userprefix();
		Dam_Damage_System::check_stop_thread_flag();
		//eco
		prefix << "ECO> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
		this->eco_sys.check_system();
		Sys_Common_Output::output_dam->rewind_userprefix();
		Dam_Damage_System::check_stop_thread_flag();
		//pop
		prefix << "POP> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
		this->people_sys.check_system();
		Sys_Common_Output::output_dam->rewind_userprefix();
		Dam_Damage_System::check_stop_thread_flag();
		//pys
		prefix << "PYS> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
		this->pys_sys.check_system();
		Sys_Common_Output::output_dam->rewind_userprefix();
		//SC
		prefix << "SC> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
		this->sc_sys.check_system();
		Sys_Common_Output::output_dam->rewind_userprefix();
		//CI
		prefix << "CI> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
		this->ci_sys.check_system();
		Sys_Common_Output::output_dam->rewind_userprefix();
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		if(Dam_Damage_System::abort_thread_flag==false){
			this->number_error++;
		}
	}
	this->output_statistic();

	this->set_warning_number();
	//set the actual time
	time(&this->actual_time);
	this->output_error_time_statistic();

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Set the impact values of the floodplains from database
void Dam_Damage_System::set_impact_values_hyd(const int bound_sz, const string break_sz){
	if(this->impact_floodplain_set==false){
		QSqlTableModel results(0,this->qsqldatabase);
		try{
			this->number_floodplain_impact=Hyd_Model_Floodplain::select_relevant_model_database(&results, false);
		}
		catch(Error msg){
			throw msg;
		}
		if(this->number_floodplain_impact==0){
			Error msg=this->set_error(1);
			throw msg;
		}
		this->allocate_impact_floodplain();
		int number_x=0;
		int number_y=0;
		//set the floodplain index
		for(int i=0; i<this->number_floodplain_impact; i++){
			this->impact_floodplain[i].set_floodplain_index(results.record(i).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::genmod_id)).c_str()).toInt());
			number_x=results.record(i).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::nx)).c_str()).toInt();
			number_y=results.record(i).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::ny)).c_str()).toInt();
			this->impact_floodplain[i].set_number_element(number_x*number_y);
		}
	}
	ostringstream cout;
	//set the impact values for each floodplain
	for(int i=0; i< this->number_floodplain_impact; i++){
		cout << "Read in the hydraulic impact values for break scenario "<<break_sz<<" for floodplain "<< this->impact_floodplain[i].get_index_floodplain()<<"..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
		this->impact_floodplain[i].set_impact_values_database(bound_sz, break_sz, &this->qsqldatabase, this->system_id);
	}
}
///Set the instationary impact values of the floodplains from database
void Dam_Damage_System::set_instat_impact_values_hyd(const int bound_sz, const string break_sz, const string time_date) {
	if (this->impact_floodplain_set == false) {
		QSqlTableModel results(0, this->qsqldatabase);
		try {
			this->number_floodplain_impact = Hyd_Model_Floodplain::select_relevant_model_database(&results, false);
		}
		catch (Error msg) {
			throw msg;
		}
		if (this->number_floodplain_impact == 0) {
			Error msg = this->set_error(12);
			throw msg;
		}
		this->allocate_impact_floodplain();
		int number_x = 0;
		int number_y = 0;
		//set the floodplain index
		for (int i = 0; i < this->number_floodplain_impact; i++) {
			this->impact_floodplain[i].set_floodplain_index(results.record(i).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::genmod_id)).c_str()).toInt());
			number_x = results.record(i).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::nx)).c_str()).toInt();
			number_y = results.record(i).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::ny)).c_str()).toInt();
			this->impact_floodplain[i].set_number_element(number_x*number_y);
		}
	}
	//ostringstream cout;
	//set the impact values for each floodplain
	for (int i = 0; i < this->number_floodplain_impact; i++) {
		//cout << "Read in the hydraulic instationary impact values for break scenario " << break_sz << " for floodplain " << this->impact_floodplain[i].get_index_floodplain() << "..." << endl;
		//Sys_Common_Output::output_dam->output_txt(&cout);
		this->impact_floodplain[i].set_instationary_impact_values_database(bound_sz, break_sz, &this->qsqldatabase, this->system_id, time_date);
	}


}
//Allocate the impact values of the floodplains from the hydraulic system
void Dam_Damage_System::allocate_impact_floodplain(void){
	this->delete_impact_floodplain();
	try{
		this->impact_floodplain=new Dam_Impact_Value_Floodplain[this->number_floodplain_impact];
		this->impact_floodplain_set=true;
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the impact values of the floodplains from the hydraulic system
void Dam_Damage_System::delete_impact_floodplain(void){
	if(this->impact_floodplain!=NULL){
		delete []this->impact_floodplain;
		this->impact_floodplain=NULL;
		this->impact_floodplain_set=false;
	}
}
//Import multi hydraulic result raster from file to database for multiple floodplains
void Dam_Damage_System::import_multi_fp_hyd_res_raster_file2database(void){
	ostringstream prefix;

	//begin time recording
	time(&this->start_time);

	this->set_start_warnings_number();
	try{
		if(this->hyd_base_sc_import==true){
			Hyd_Hydraulic_System buffer_hyd_system;
			//set database
			buffer_hyd_system.set_ptr2database(&this->qsqldatabase,0);

			//delete all hydraulic infos
			buffer_hyd_system.delete_data_hyd_database_tables();
			//delete all damage results
			Dam_Damage_System::delete_data_in_erg_table(&this->qsqldatabase);
			this->reset_raster_hyd_connection_flag(&this->qsqldatabase);

			//set global params and matparams with dummy data sets
			buffer_hyd_system.global_parameters.set_number_floodplain_models(this->number_hyd_imp_dia);
			buffer_hyd_system.global_parameters.transfer_default2database_table(&this->qsqldatabase);

			buffer_hyd_system.material_params.transfer_dummy_params2database_table(&this->qsqldatabase);
		}

		//check the files
		string h_f=label::not_set;
		string z_f=label::not_set;
		string d_f=label::not_set;
		string v_f=label::not_set;
		string dh_dt_f=label::not_set;

		//floodplain buffers are allocated at checked
		for(int i=0; i<this->number_hyd_imp_dia; i++){
			//reset file name
			h_f=label::not_set;
			z_f=label::not_set;
			d_f=label::not_set;
			v_f=label::not_set;
			dh_dt_f=label::not_set;
			//set prefix for output
			prefix << "MULTI_HYD_"<<i+1<<"> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);
			ostringstream cout;
			cout <<"Check the data of the HYD results of floodplain "<<i+1<<"..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//set the file name
			h_f=this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(0)->get_file_name();
			z_f=this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(1)->get_file_name();
			v_f=this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(2)->get_file_name();
			d_f=this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(3)->get_file_name();
			dh_dt_f=this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(4)->get_file_name();

			this->check_fp_hyd_res_raster_file2database(i, h_f, z_f, v_f, d_f, dh_dt_f);
			cout <<"Data check of the HYD results of floodplain "<<i+1<<" is finished..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Sys_Common_Output::output_dam->rewind_userprefix();
		}

		//import the data
		Hyd_Boundary_Szenario buffer;
		//insert to database
		this->sz_bound_manager.insert_new_hyd_sz(&buffer, &this->qsqldatabase, 0);

		//floodplain buffers are allocated at transfer
		for(int i=0; i<this->number_hyd_imp_dia; i++){
			//reset file name
			h_f=label::not_set;
			z_f=label::not_set;
			d_f=label::not_set;
			v_f=label::not_set;
			dh_dt_f=label::not_set;
			//set prefix for output
			prefix << "MULTI_HYD_"<<i+1<<"> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);
			ostringstream cout;
			cout <<"Import the data of the HYD results of floodplain "<<i+1<<" to database..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//set the file name
			h_f=this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(0)->get_file_name();
			z_f=this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(1)->get_file_name();
			v_f=this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(2)->get_file_name();
			d_f=this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(3)->get_file_name();
			dh_dt_f=this->multi_fp_hyd_raster_import_dia[i].get_ptr_file_browser(4)->get_file_name();

			this->import_fp_hyd_res_raster_file2database(i, h_f, z_f, v_f, d_f, dh_dt_f, &buffer);
			cout <<"Data import of the HYD results of floodplain "<<i+1<<" to database is finished..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Sys_Common_Output::output_dam->rewind_userprefix();
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		if(Dam_Damage_System::get_stop_thread_flag()==false){
			this->number_error++;
		}
		msg.output_msg(4);
	}

	//set the actual time
	time(&this->actual_time);

	this->set_warning_number();
	this->number_hyd_warnings=Warning::get_number_hyd_warnings();
	this->number_hyd_errors=Error::get_number_hyd_errors();

	this->output_error_time_statistic();
}
//Import multi hydraulic result raster from file to database for one specified floodplain
void Dam_Damage_System::import_fp_hyd_res_raster_file2database(const int fp_index, string h_file, string z_file, string v_file, string d_file, string dh_dt_file, Hyd_Boundary_Szenario *sc){
	try{
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		Dam_Hyd_Result_Raster result_raster;
		result_raster.set_raster_number(fp_index);

		//check if at least the waterlevel raster is given
		if(h_file!=label::not_set){
			//read the result data
			result_raster.read_all_hydraulic_rasters_per_file(h_file, z_file, v_file, d_file, dh_dt_file);

			if(this->hyd_base_sc_import==false){
				//check them
				QSqlQueryModel result;
				Dam_Hyd_Result_Raster referenz_raster;
				//Select and count the number of relevant floodplain models in a database table with a given floodplain number
				Hyd_Model_Floodplain::select_relevant_model_database(&result, &this->qsqldatabase, fp_index);
				referenz_raster.set_geometrical_member_database(fp_index, &result);
				if(referenz_raster.compare_raster_infos(&result_raster)==false){
					Error msg=this->set_error(8);
					ostringstream info;
					info << "Raster id   : " << fp_index << endl;
					info << "Raster name : " << referenz_raster.get_raster_name() << endl;
					info << "File name   : " << h_file << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}

			this->check_stop_thread_flag();

			//output the members
			result_raster.output_member();
			this->check_stop_thread_flag();

			//transfer data to database
			result_raster.transfer_input_members2database(&this->qsqldatabase, fp_index, sc, this->hyd_base_sc_import);
			this->check_stop_thread_flag();

			//reset the raster
			result_raster.clear_raster();
		}else{
			Error msg=this->set_error(7);
			throw msg;
		}
	}
	catch(Error msg){
		Hyd_Boundary_Szenario_Management::delete_scenario_by_id(&this->qsqldatabase, sc->get_id(),this->system_id);
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Import multi hydraulic result raster from file to database for one specified floodplain
void Dam_Damage_System::check_fp_hyd_res_raster_file2database(const int fp_index, string h_file, string z_file, string v_file, string d_file, string dh_dt_file){
		try{
		//set prefix
		ostringstream prefix;
		prefix << "CHECK> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		Dam_Hyd_Result_Raster result_raster;
		result_raster.set_raster_number(fp_index);

		//check if at least the waterlevel raster is given
		if(h_file!=label::not_set){
			//read the result data
			result_raster.read_all_hydraulic_rasters_per_file(h_file, z_file, v_file, d_file, dh_dt_file);

			if(result_raster.get_number_elements()>=constant::max_elems){
				Error msg=this->set_error(11);
				ostringstream info;
				info << "Raster id         : " << fp_index << endl;
				info << "Raster name       : " << result_raster.get_raster_name() << endl;
				info << "File name         : " << h_file << endl;
				info << "Number elements   : " << result_raster.get_number_elements() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}

			if(this->hyd_base_sc_import==false){
				//check them
				QSqlQueryModel result;
				Dam_Hyd_Result_Raster referenz_raster;
				//Select and count the number of relevant floodplain models in a database table with a given floodplain number
				Hyd_Model_Floodplain::select_relevant_model_database(&result, &this->qsqldatabase, fp_index);
				referenz_raster.set_geometrical_member_database(fp_index, &result);
				if(referenz_raster.compare_raster_infos(&result_raster)==false){
					Error msg=this->set_error(10);
					ostringstream info;
					info << "Raster id   : " << fp_index << endl;
					info << "Raster name : " << referenz_raster.get_raster_name() << endl;
					info << "File name   : " << h_file << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}

			this->check_stop_thread_flag();

			//reset the raster
			result_raster.clear_raster();
		}else{
			Error msg=this->set_error(9);
			throw msg;
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Allocate the polygons for an interception
void Dam_Damage_System::allocate_polygons(void){
	this->delete_polygons();
	try{
		this->polygons=new Dam_Polygon[this->number_polygons];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the polygons for an interception
void Dam_Damage_System::delete_polygons(void){
	if(this->polygons!=NULL){
		delete []this->polygons;
		this->polygons=NULL;
	}
}
//Allocate the dialog for importing multiple hydraulic results for multiple floodplains
void Dam_Damage_System::allocate_hyd_res_import_dia_multi_fp(void){
	this->delete_hyd_res_import_dia_multi_fp();
	try{
		this->multi_fp_hyd_raster_import_dia=new Sys_Multi_Filechooser_Dia[this->number_hyd_imp_dia];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(6);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the dialog for importing multiple hydraulic results for multiple floodplains
void Dam_Damage_System::delete_hyd_res_import_dia_multi_fp(void){
	if(this->multi_fp_hyd_raster_import_dia!=NULL){
		delete []this->multi_fp_hyd_raster_import_dia;
		this->multi_fp_hyd_raster_import_dia=NULL;
	}
}
//Set the warning
Warning Dam_Damage_System::set_warning(const int warn_type){
	string place="Dam_Damage_System::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://the type of thread is not specified
			place.append("run(void)") ;
			reason="The thread type is unknown";
			reaction="No thread is launched";
			help= "Check the source code";
			type=5;
			break;
		case 1://results datas can not submitted
			place.append("output_result_members2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz)");
			reason="Can not submit the element results data of the damage calculation to the database";
			help="Check the database";
			type=2;
			break;
		case 2://no hydraulic results available
			place.append("calc_damage_nobreak_sz(void))");
			reason="No hydraulic results are availabe in database for the selected hydraulic boundary scenario(s)";
			help="Check the hydraulic results";
			reaction="The damages of the hydraulic boundary scenario will not be calculated";
			type=18;
			break;
		case 3://no hydraulic results available
			place.append("calc_damage_break_sz(void))");
			reason="No hydraulic results are availabe in database for the hydraulic break scenario(s)";
			help="Check the hydraulic results";
			reaction="The damages of the hydraulic boundary scenario will not be calculated";
			type=18;
			break;
		case 4://no hydraulic results available
			place.append("calc_instat_damage_nobreak_sz(void))");
			reason = "No hydraulic instationary results are availabe in database for the selected hydraulic boundary scenario(s)";
			help = "Check the hydraulic instationary results";
			reaction = "The damages of the hydraulic boundary scenario will not be calculated";
			type = 18;
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
//Set error(s)
Error Dam_Damage_System::set_error(const int err_type){
	string place="Dam_Damage_System::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_impact_floodplain(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://no hydraulic floodplain is found; this error is later converted into a warning
			place.append("set_impact_values_hyd(const int bound_sz, const string break_sz)");
			reason="No hydraulic floodplains are found. A damage caluclation is not possible";
			help="Check the hydraulic system";
			type=24;
			break;
		case 2://bad alloc
			place.append("allocate_polygons(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://polygon file is not open
			place.append("input_polygons_per_file(const string file_name)");
			reason="Can not open the file of intercepting polygons";
			help="Check the file";
			type=5;
			break;
		case 4://wrong input
			place.append("input_polygons_per_file(const string file_name))");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the polygon number in the polygon file for the intercepting polygons";
			type=1;
			break;
		case 5://keyword not found
			place.append("input_polygons_per_file(const string file_name))");
			reason="The number of polygons in file is not found";
			help="It has to be defined with !no_polygons ";
			type=1;
			break;
		case 6://bad alloc
			place.append("allocate_hyd_res_import_dia_multi_fp(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 7://waterlevel file is not defined
			place.append("import_fp_hyd_res_raster_file2database(const int fp_index, string h_file, string z_file, string v_file, string d_file, string dh_dt_file)");
			reason="The filename of result file of the relative waterlevel h is not defined";
			help="Check the name of the input file(s)";
			type=1;
			break;
		case 8://waterlevel file is not defined
			place.append("import_fp_hyd_res_raster_file2database(const int fp_index, string h_file, string z_file, string v_file, string d_file, string dh_dt_file)");
			reason="The given geometrical information of the hydraulic result raster are not identical to the reference raster";
			help="Check the geometrical information of the raster";
			type=1;
			break;
		case 9://waterlevel file is not defined
			place.append("check_fp_hyd_res_raster_file2database(const int fp_index, string h_file, string z_file, string v_file, string d_file, string dh_dt_file)");
			reason="The filename of result file of the relative waterlevel h is not defined";
			help="Check the name of the input file(s)";
			type=1;
			break;
		case 10://waterlevel file is not defined
			place.append("check_fp_hyd_res_raster_file2database(const int fp_index, string h_file, string z_file, string v_file, string d_file, string dh_dt_file)");
			reason="The given geometrical information of the hydraulic result raster are not identical to the reference raster";
			help="Check the geometrical information of the raster";
			type=1;
			break;
		case 11://to much elements
			place.append("check_fp_hyd_res_raster_file2database(const int fp_index, string h_file, string z_file, string v_file, string d_file, string dh_dt_file)");
			reason="There are to much elements in the raster";
			help="Split the raster or decrease the raster resolution";
			type=33;
			break;
		case 12://no hydraulic floodplain is found; this error is later converted into a warning
			place.append("set_instat_impact_values_hyd(const int bound_sz, const string break_sz)");
			reason = "No hydraulic floodplains are found. A damage caluclation is not possible";
			help = "Check the hydraulic system";
			type = 24;
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
