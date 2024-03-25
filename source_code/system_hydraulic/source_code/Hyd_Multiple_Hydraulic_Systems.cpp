#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Multiple_Hydraulic_Systems.h"

//init static members
bool Hyd_Multiple_Hydraulic_Systems::abort_thread_flag=false;


//constructor
Hyd_Multiple_Hydraulic_Systems::Hyd_Multiple_Hydraulic_Systems(void){

	this->number_systems=0;
	this->file_names=NULL;
	this->counter_warnings=0;
	this->counter_error=0;
	this->type=_hyd_thread_type::hyd_thread_unknown;
	this->threads=NULL;
	this->output2file_required=true;

	this->required_threads=1;

	this->thread_is_started=false;
	this->gis_raster=NULL;
	this->prom_raster=NULL;
	this->raster_import_dia=NULL;
	this->profil_import_dia=NULL;
	this->profil_dgm_conversion=NULL;


	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Multiple_Hydraulic_Systems)-sizeof(Hyd_Boundary_Szenario_Management), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Multiple_Hydraulic_Systems::~Hyd_Multiple_Hydraulic_Systems(void){
	this->delete_file_names();
	this->delete_hyd_threads();
	this->delete_raster_class();
	this->delete_raster_path_dia();
	this->delete_profile_path_dia();
	this->delete_profile_conversion();
	this->set_stop_thread_flag(false);
	//close database
	if(this->qsqldatabase.isOpen()){
		this->qsqldatabase.close();
	}
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Multiple_Hydraulic_Systems)-sizeof(Hyd_Boundary_Szenario_Management), _sys_system_modules::HYD_SYS);
}
//_____________
//public
//Ask the number of hydraulic systems and the names of the global files per console
void Hyd_Multiple_Hydraulic_Systems::ask_system_number_file_console(void){
	int number=0;
	bool flag=false;
	do{
		cout << "Give in the number of calculation : " << endl;
		cin >> number;
		if((flag=(cin.fail()))){//check if the input is valif
				cout << " Wrong input! Do it again" << endl;
				cin.clear();//clear the error flags
				cin.ignore();//clear cin
		}
		else if(number <  0){
			cout << " Wrong number! Do it again! " << endl;
		}
	}
	while(flag || number < 0);

	//store it to the system
	this->number_systems=number;
	try{

		this->allocate_file_names();
		for(int i=0; i< this->number_systems; i++){
			cout<<"Calculation " << i << endl;
			this->file_names[i]=Hyd_Hydraulic_System::ask_global_file_name();
		}

	}
	catch(Error msg){
		throw msg;
	}
}
//Set the number of files for calculation and the global filenames per dialog (gui)
bool Hyd_Multiple_Hydraulic_Systems::set_system_number_file_gui(void){
	Sys_Multipathsettings_Dia dialog;
	ostringstream text;
	QIcon icon;
	icon.addFile(":hyd_icon");

	switch(this->type){
		case (_hyd_thread_type::hyd_calculation):
			text << "Choose the global system file(s) (.ilm), which " << endl;
			text << "should be calculated"<< endl;
			dialog.set_txt2headerlabel(text.str(),icon);
			dialog.set_file_filters(tr("Global hydraulic data files (*.ilm)"));
			break;
		case (_hyd_thread_type::hyd_check_system):
			text << "Choose the global system file(s) (.ilm), which " << endl;
			text << "should be checked"<< endl;
			dialog.set_txt2headerlabel(text.str(),icon);
			dialog.set_file_filters(tr("Global hydraulic data files (*.ilm)"));
			break;
		case (_hyd_thread_type::hyd_add_sz):
			text << "Choose the global system file(s) (.ilm), which " << endl;
			text << "should be added to database as boundary scenarios."<< endl;
			text << "The hydraulic system have to be equal to the base system"<< endl;
			text << "in database. The scenario id(s) are incremented by 1"<< endl;
			dialog.set_txt2headerlabel(text.str(),icon);
			dialog.set_file_filters(tr("Global hydraulic data files (*.ilm)"));
			break;
		default:
			text << "" << endl;
			dialog.set_txt2headerlabel(text.str(),icon);
	}
	HydGui_Bound_Scenario_Data_Dia buffer;
	if(this->type==_hyd_thread_type::hyd_add_sz){
		buffer.set_dialog(0,NULL, &this->qsqldatabase, false);
		dialog.set_widget2existing_scenario_layout(buffer.get_ptr_scenario_data_widget());
	}

	dialog.adjustSize();

	bool flag=false;
	flag=dialog.asked_path_filenames();

	if(flag==true){
		//read out the number of files
		this->number_systems=dialog.getFilesnumber();
		if(this->number_systems==0){
			flag=false;
			return flag;
		}
		try{
			this->allocate_file_names();
		}
		catch(Error msg){
			throw msg;
		}

		for(int i=0; i<this->number_systems; i++){
			this->file_names[i]=dialog.get_filename(i);
		}
		if(this->type==_hyd_thread_type::hyd_add_sz){
			try{
				if(this->sz_bound_manager.ask_scenario_data_per_dialog(this->number_systems, this->file_names, &this->qsqldatabase, false)==false){
					this->number_systems=0;
					this->delete_file_names();
				}
			}
			catch(Error msg){
				this->number_systems=0;
				this->delete_file_names();
				msg.output_msg(2);
			}
		}
	}
	return flag;
}
//Set one scenario for calculation directly
bool Hyd_Multiple_Hydraulic_Systems::set_system_number_file_direct(QStringList list_id, QList<int> *new_id) {
	bool flag = true;
	
	//todo new list (member in Main_Wid!)

	if (this->type == _hyd_thread_type::hyd_add_sz) {
		//read out the number of files
		this->number_systems = 1;
		try {
			this->allocate_file_names();
		}
		catch (Error msg) {
			throw msg;
		}

		//first has the path in
		this->file_names[0] = list_id.at(0).toStdString();
		Hyd_Boundary_Scenario_List my_list;
		Hyd_Boundary_Szenario my_sc;
		my_sc.set_name(list_id.at(1).toStdString());
		int id_buff = this->sz_bound_manager.get_new_hyd_sz_id(&this->qsqldatabase);
		new_id->append(id_buff);
		my_sc.set_members(id_buff, list_id.at(2).toDouble(), list_id.at(3).toDouble(), list_id.at(1).toStdString());

		my_list.add_scenario2list(&my_sc);

	
		try {
			this->sz_bound_manager.set_new_boundary_scenario_directly(&my_list);
	
		}
		catch (Error msg) {
			this->number_systems = 0;
			this->delete_file_names();
			msg.output_msg(2);
		}
	}

	return flag;

}
//Ask per dialog (HydGui_Boundary_Scenario_Dia), which of the availabe hydraulic boundary scenarios should be handled and return the number
int Hyd_Multiple_Hydraulic_Systems::ask_boundary_scenarios_per_dialog(QSqlDatabase *ptr_database, QWidget *parent) {


	ostringstream buff;
	if (this->type == _hyd_thread_type::hyd_calculation) {
		buff << "Select scenario(s) for calculation from the available hydraulic boundary scenario(s)" << endl;
	}
	else if (this->type == _hyd_thread_type::hyd_temp_calculation) {
		buff << "Select scenario(s) for temperature calculation from the available \nhydraulic boundary scenario(s) with instat results" << endl;
	}
	else if (this->type == _hyd_thread_type::hyd_del_sz) {
		buff << "Select scenario(s) for deleting from the available hydraulic boundary scenario(s)" << endl;
	}
	else {
		buff << "Select scenario(s) for unknown handling from the available hydraulic boundary scenario(s)" << endl;
	}

	if (this->type != _hyd_thread_type::hyd_temp_calculation){
		if (this->sz_bound_manager.ask_boundary_scenarios_per_dialog(ptr_database, buff.str(), parent) == 0) {
			return 0;
		}
		else {
			this->number_systems = this->sz_bound_manager.counter_number_selected_scenarios();
			return this->number_systems;
		}
	}
	else {

		this->sz_bound_manager.set_szenario_per_db(ptr_database);
		Hyd_Boundary_Scenario_List my_list;
		for (int i = 0; i < this->sz_bound_manager.get_number_sz(); i++) {

			if (Hyd_Hydraulic_System::check_hyd_results_calculated(ptr_database, this->system_id, this->sz_bound_manager.get_ptr_sz(i)->get_id(), "CA") == true) {
				my_list.add_scenario2list(this->sz_bound_manager.get_ptr_sz(i));
			}

		}

		int number_selected = 0;
		this->sz_bound_manager.ask_boundary_scenarios_per_dialog(buff.str(), parent, &my_list, &number_selected);
		this->number_systems = number_selected;
		return number_selected;



	}
}
//Set per list, which of the availabe hydraulic boundary scenarios should be handled and return the number
int Hyd_Multiple_Hydraulic_Systems::set_boundary_scenarios_per_list(QSqlDatabase *ptr_database, QList<int> list_id) {

	if (this->sz_bound_manager.set_boundary_scenarios_per_list(ptr_database, list_id) == 0) {
		return 0;
	}
	else {
		this->number_systems = this->sz_bound_manager.counter_number_selected_scenarios();
		return this->number_systems;
	}

}
//Ask the file for raster conversion per dialog
bool Hyd_Multiple_Hydraulic_Systems::ask_file_raster_conversion_dialog(void){

	//allocate
	this->allocate_raster_path_dia();
	QIcon icon;
	icon.addFile(":hyd_icon");
	this->raster_import_dia->set_number_file_browser(2,icon);

	stringstream buffer;
	buffer << "Choose the file(s), where the hydraulic raster for conversion are stored."<<endl;
	this->raster_import_dia->set_main_text_label(buffer.str());
	buffer.str("");
	this->raster_import_dia->set_window_title("Choose HYD-raster file(s)");
	this->raster_import_dia->get_ptr_file_browser(0)->set_text_label("GIS raster for conversion");
	this->raster_import_dia->get_ptr_file_browser(0)->set_tooltip("Here the data of the GIS-raster are stored. This raster is converted.");

	this->raster_import_dia->get_ptr_file_browser(1)->set_text_label("ProMaIDes HYD-raster");
	this->raster_import_dia->get_ptr_file_browser(1)->set_tooltip("Here the data of the resulting ProMaIDes HYD-raster are stored.");

	return this->raster_import_dia->start_dialog();
}
//Ask the file for profile conversion per dialog
bool Hyd_Multiple_Hydraulic_Systems::ask_file_profile_conversion_dialog(void){
	//allocate
	this->allocate_profile_dia();
	QIcon icon;
	icon.addFile(":hyd_icon");
	this->profil_import_dia->set_number_file_browser(3,icon);

	stringstream buffer;
	buffer << "Choose the file(s), where the data of profile to river DGM-W conversion are stored."<<endl;
	this->profil_import_dia->set_main_text_label(buffer.str());
	buffer.str("");
	this->profil_import_dia->set_window_title("Choose profile points to convert to DEM-river...");
	this->profil_import_dia->get_ptr_file_browser(0)->set_text_label("Profile points for conversion");
	this->profil_import_dia->get_ptr_file_browser(0)->set_tooltip("Here the data (x, y, z) of the profile points is stored. This points are interpolated to the streamlines.");

	this->profil_import_dia->get_ptr_file_browser(1)->set_text_label("Streamline for the in flow direction right river bank");
	this->profil_import_dia->get_ptr_file_browser(1)->set_tooltip("Here the data (x,y) for the in flow direction right river bank is stored.");

	this->profil_import_dia->get_ptr_file_browser(2)->set_text_label("Streamline for the in flow direction left river bank");
	this->profil_import_dia->get_ptr_file_browser(2)->set_tooltip("Here the data (x,y) for the in flow direction left river bank is stored.");


	return this->profil_import_dia->start_dialog();

}
//Set the number of hydraulic systems and the names of the global files direct
void Hyd_Multiple_Hydraulic_Systems::set_system_number_file(const int number, char *file[]){
	//store it to the system
	this->number_systems=number;
	try{
		this->allocate_file_names();
		for(int i=0; i< this->number_systems; i++){
			this->file_names[i]=file[i+1];
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the number of hydraulic systems and the names of the global files direct
void Hyd_Multiple_Hydraulic_Systems::set_system_number_file(QStringList *list){
	//store it to the system
	this->number_systems=list->count();
	try{
		this->allocate_file_names();
		for(int i=0; i< this->number_systems; i++){
			this->file_names[i]=list->at(i).toStdString();
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the file name of one hydraulic systems 
void Hyd_Multiple_Hydraulic_Systems::set_one_system_filename(string file){
	//store it to the system
	this->number_systems=1;
	try{
		this->allocate_file_names();
		for(int i=0; i< this->number_systems; i++){
			this->file_names[i]=file;
		}
		if(this->type==_hyd_thread_type::hyd_data_import){
			if(this->sz_bound_manager.ask_scenario_data_per_dialog(this->number_systems, this->file_names, &this->qsqldatabase, true)==false){
				this->number_systems=0;
				this->delete_file_names();
			}
		}
	}
	catch(Error msg){
		this->number_systems=0;
		this->delete_file_names();
		msg.output_msg(2);
	}
}
//Set a pointer to the database
void Hyd_Multiple_Hydraulic_Systems::set_ptr2database(QSqlDatabase *ptr_database){
	this->qsqldatabase=QSqlDatabase::cloneDatabase(*ptr_database, sys_label::str_hyd.c_str());
	this->qsqldatabase.open();
}
//Set the number of hydraulic systems
void Hyd_Multiple_Hydraulic_Systems::set_number_systems(const int number){
	this->number_systems=number;
}
//Set thread type; use it before starting a thread
void Hyd_Multiple_Hydraulic_Systems::set_thread_type(_hyd_thread_type type){
	this->type=type;
}
//Get the thread type for specifiying the task of the thread
_hyd_thread_type Hyd_Multiple_Hydraulic_Systems::get_thread_type(void){
	return this->type;
}
//Make multiple hydraulic calculations
void Hyd_Multiple_Hydraulic_Systems::run(void){
	this->thread_is_started=true;
	//file calculation
	if(this->type==_hyd_thread_type::hyd_calculation){
		//perfile
		if(this->qsqldatabase.isOpen()==false){
			if(this->number_systems>0){
				this->set_required_threads();
				this->set_required_output2file(true);
				this->calculate_file_systems();
			}
		}
		//per database
		else{
			if(this->number_systems>0){
				this->set_required_threads();
				this->calculate_hydraulic_system_database();
			}
		}
	}
	//temperature calculation
	else if (this->type == _hyd_thread_type::hyd_temp_calculation) {
		if (this->number_systems > 0) {
			this->set_required_threads();
			this->calculate_hydraulic_temp_system_database();
		}

	}	
	//file import of the base hydraulic system
	else if(this->type==_hyd_thread_type::hyd_data_import){
		if(this->number_systems>0){
			this->set_number_required_threads(1);
			this->set_required_threads();
			this->set_required_output2file(false);
			this->import_basesystem_file2database();
		}
	}
	//adding additional boundary szenarios
	else if(this->type==_hyd_thread_type::hyd_add_sz){
		if(this->number_systems>0){
			this->set_number_required_threads(1);
			this->set_required_threads();
			this->set_required_output2file(false);
			this->add_boundary_szenarios_file2database();
		}

	}
	//tables creating
	else if(this->type==_hyd_thread_type::hyd_create_tab){
			this->set_number_required_threads(1);
			this->set_required_threads();
			this->set_required_output2file(false);
			this->create_database_tables();
	}
	//tables checking
	else if(this->type==_hyd_thread_type::hyd_check_tab){
			this->set_number_required_threads(1);
			this->set_required_threads();
			this->set_required_output2file(false);
			this->check_database_tables();
	}
	//hydraulic system checking
	else if(this->type==_hyd_thread_type::hyd_check_system){
		//per file
		if(this->qsqldatabase.isOpen()==false){
			if(this->number_systems>0){
				this->set_number_required_threads(1);
				this->set_required_threads();
				this->check_hydraulic_system_perfile();
			}

		}
		//per database
		else{
			this->set_number_required_threads(1);
			this->set_required_threads();
			this->set_required_output2file(false);
			this->check_hydraulic_system_database();
		}
	}
	//delete selected scenarios
	else if(this->type==_hyd_thread_type::hyd_del_sz){
		this->set_number_required_threads(1);
		this->set_required_threads();
		this->set_required_output2file(false);
		this->delete_selected_scenarios();
	}
	//convert Gis to Prom-raster
	else if(this->type==_hyd_thread_type::hyd_convert_Gis2Prom){
		this->set_number_required_threads(1);
		this->set_required_threads();
		this->set_required_output2file(false);
		this->convert_Gis2Promraster();
	}
	else if(this->type==_hyd_thread_type::hyd_convert_profil2dgmw){
		this->set_number_required_threads(1);
		this->set_required_threads();
		this->set_required_output2file(false);
		this->convert_profil2dgmw();
	}
	else if(this->type==_hyd_thread_type::hyd_thread_unknown){
		//warning
		Warning msg=this->set_warning(1);
		msg.output_msg(2);
	}
	this->thread_is_started=false;
}
//Set the stop thread flag (static)
void Hyd_Multiple_Hydraulic_Systems::set_stop_thread_flag(const bool flag){
	Hyd_Multiple_Hydraulic_Systems::abort_thread_flag=flag;
}
//Check the stop thread flag (static)
void Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag(void){

	if(abort_thread_flag==true){
		Error msg;
		msg.set_msg(_sys_system_modules::HYD_SYS);
		throw msg;
	}
}
//Get the stop thread flag
bool Hyd_Multiple_Hydraulic_Systems::get_stop_thread_flag(void){
	return Hyd_Multiple_Hydraulic_Systems::abort_thread_flag;
}
//Set the system-id (_sys_system_id) of the object
void Hyd_Multiple_Hydraulic_Systems::set_systemid(const _sys_system_id actuel_id){
	_Sys_Common_System::set_systemid(actuel_id);
	this->sz_bound_manager.set_systemid(actuel_id);
}
//Get the number of errors
int Hyd_Multiple_Hydraulic_Systems::get_error_number(void){
	return this->counter_error;
}
//Set number of required threads
void Hyd_Multiple_Hydraulic_Systems::set_number_required_threads(const int number){
	this->required_threads=number;	
}
//Get the number of threads
int Hyd_Multiple_Hydraulic_Systems::get_number_threads(void){
	return this->required_threads;
}
//Get a pointer to the thread specified by the index
Hyd_Hydraulic_System * Hyd_Multiple_Hydraulic_Systems::get_ptr_hyd_threads(const int index){
	if(index<0 || index>=this->required_threads){
		return NULL;
	}
	return &this->threads[index];
}
//Set the flag if a file output is required in case of a database based hydraulic calculation
void Hyd_Multiple_Hydraulic_Systems::set_required_output2file(const bool flag){
	this->output2file_required=flag;
}
//Get the flag if the thread has started
bool Hyd_Multiple_Hydraulic_Systems::get_thread_has_started(void){
	return this->thread_is_started;
}
//Get if the hydraulic threads are running
bool Hyd_Multiple_Hydraulic_Systems::get_hydraulic_thread_running(int *number){
	int counter=0;
	bool runs=false;
	if(this->threads!=NULL){
		if(this->required_threads==1){
			counter=1;
		}
		for(int i=0; i< this->required_threads; i++){
			if(this->threads[i].isRunning()==true){
				counter++;
				runs=true;
			}
		}

	}
	*number=counter;
	return runs;
}
//Delete hydraulic boundary scenarios specified by their id
void Hyd_Multiple_Hydraulic_Systems::delete_hydraulic_boundary_scenarios(const int id){
	if(Sys_Project::get_project_type()!=_sys_project_type::proj_dam){
		_Hyd_River_Profile::delete_boundary_cond_by_scenario(&this->qsqldatabase,id);
		_Hyd_River_Profile::delete_results_by_scenario(&this->qsqldatabase,id);
		_Hyd_River_Profile::delete_instat_results_by_scenario(&this->qsqldatabase, id);
		Hyd_Element_Floodplain::delete_boundary_cond_by_scenario(&this->qsqldatabase,id);
		Hyd_Instationary_Boundary::delete_boundary_curve_by_scenario(&this->qsqldatabase,id);
	}
	Hyd_Element_Floodplain::delete_results_by_scenario(&this->qsqldatabase,id); 
	Hyd_Boundary_Szenario_Management::delete_scenario_by_id(&this->qsqldatabase,id, this->system_id);
}
//_____________
//public slots
//Manage the output of the hydraulic classes
void Hyd_Multiple_Hydraulic_Systems::recieve_output_required(int thread){
	if(this->threads[thread].get_output_is_required()==false){
		return;
	}
	//check if as thread output is running
	for(int i=0; i< this->required_threads; i++){
		if(this->threads[i].get_output_is_running()==true){
			return;
		}
	}
	this->threads[thread].set_output_is_allowed();
}
//_____________
//private
//Set the required threads and initialize them
void Hyd_Multiple_Hydraulic_Systems::set_required_threads(void){
	//check if multithreading is possible
	if(this->number_systems==1){
		this->required_threads=1;
	}
	else if(this->number_systems>1){
		if(this->number_systems<this->required_threads){
			this->required_threads=this->number_systems;
		}
		if(this->required_threads > constant::max_threads){
			//warning
			Warning msg=this->set_warning(2);
			ostringstream info;
			info <<"Required number : " << this->required_threads << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
			//reaction
			this->required_threads=constant::max_threads;
		}

		int ideal_no=-1;
		ideal_no=this->idealThreadCount();
		if(ideal_no<=0){
			ideal_no=1;
		}
		if(ideal_no>1){
			ideal_no=ideal_no-1;
		}
		if(this->required_threads> ideal_no){
			//warning
			Warning msg=this->set_warning(0);
			ostringstream info;
			info <<"Required number : " << this->required_threads << endl;
			info <<"Ideal number    : " << ideal_no<<endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);

		}
	}
}
//Catch the number of CPU and GPU working, which is emitted from each Hyd_Hydraulic_System
void Hyd_Multiple_Hydraulic_Systems::catch_multi_statusbar_hyd_solver_update(unsigned int cpu_count, unsigned int gpu_count) {
	emit statusbar_main_hyd_solver_update(cpu_count, gpu_count);
}
//Allocate the required hydraulic threads
void Hyd_Multiple_Hydraulic_Systems::allocate_hyd_threads(void){
	this->delete_hyd_threads();
	try{
		this->threads=new Hyd_Hydraulic_System[this->required_threads];
		ostringstream buffer;
		buffer << this->required_threads;
		QObject::connect(this->threads, SIGNAL(statusbar_Multi_hyd_solver_update(unsigned int, unsigned int)), this, SLOT(catch_multi_statusbar_hyd_solver_update(unsigned int, unsigned int)));
		emit emit_number_threads(buffer.str().c_str());
		emit emit_threads_allocated();
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init the threads
	if(this->required_threads>1){
		Hyd_Hydraulic_System::qt_thread_applied=true;
	}
	else{
		Hyd_Hydraulic_System::qt_thread_applied=false;
	}
	for(int i=0; i< this->required_threads; i++){
		this->threads[i].set_systemid(this->get_sys_system_id());
		this->threads[i].set_thread_number(i);
		if(this->qsqldatabase.isOpen()==true){
			this->threads[i].set_file_output_flag(this->output2file_required);
			this->threads[i].set_ptr2database(&this->qsqldatabase,i);
		}
		if (this->type == _hyd_thread_type::hyd_temp_calculation) {
			this->threads[i].set_temp_calc_apply(true);

		}
		QObject::connect(&this->threads[i], SIGNAL(output_required(int)), this, SLOT(recieve_output_required(int )));

	}
}
//Delete the hydraulic threads
void Hyd_Multiple_Hydraulic_Systems::delete_hyd_threads(void){
	
	if(this->threads!=NULL){
		delete []this->threads;
		this->threads=NULL;
	}
	if(this->qsqldatabase.isOpen()==true){
		//delete database
		for(int i=0; i< this->required_threads; i++){
			ostringstream buffer;
			buffer<<sys_label::str_hyd<<"_"<<i;
			if(QSqlDatabase::contains(buffer.str().c_str())==true){
				if(QSqlDatabase::database(buffer.str().c_str(),false).isOpen()==true){
					QSqlDatabase::database(buffer.str().c_str(),false).close();	
				}	
				QSqlDatabase::removeDatabase(buffer.str().c_str());
			}
		}
	}
	Hyd_Hydraulic_System::qt_thread_applied=false;
}
//Set hydraulic system(s) of the threads the first time per file
void Hyd_Multiple_Hydraulic_Systems::set_hyd_system_new_file(int *counter_sys){
	ostringstream cout;
	ostringstream prefix;
	for(int i=0; i<this->required_threads; i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->threads[i].total_reset();
		do{
			if(*counter_sys==this->number_systems){
				break;
			}
			this->threads[i].set_system_number(*counter_sys);
									
			cout <<"Preprocessing of hydraulic system " << *counter_sys << " thread no. " << i<<"..."<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			prefix << "SYS_"<<*counter_sys<<"> ";
			Sys_Common_Output::output_hyd->set_userprefix(&prefix);
			//preprocessing 
			try{
				this->threads[i].set_system_per_file(this->file_names[*counter_sys]);
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->threads[i].output_glob_models();
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->threads[i].init_models();
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->threads[i].set_folder_name_file();
				this->threads[i].output_setted_members();
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->threads[i].init_solver();
				this->threads[i].set_preproc_flag(true);
			}
			catch(Error msg){
				if(msg.get_user_aborted_exception()==false){
					ostringstream info;
					info <<"Number hydraulic system : "<< *counter_sys<< endl;
					msg.make_second_info(info.str());
					this->counter_error++;
					this->threads[i].increase_error_number();
					msg.output_msg(2);
					this->threads[i].output_final_model_statistics();
					this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
					this->threads[i].total_reset();
				}
				else{
					Sys_Common_Output::output_hyd->rewind_userprefix();
					throw msg;
				}

			}
			Sys_Common_Output::output_hyd->rewind_userprefix();
			(*counter_sys)++;
		}
		while(*counter_sys<this->number_systems && this->threads[i].get_preproc_flag()==false);
	}
}
//Decide for new loading a hydraulic system or just reseting and new loading the boundary conditions from database
void Hyd_Multiple_Hydraulic_Systems::decide_new_reset_db(int *counter_sys){
	for(int i=0; i<this->required_threads; i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		if(this->threads[i].get_preproc_flag()==false){
			if (this->type == _hyd_thread_type::hyd_temp_calculation) {

				this->set_hyd_temp_system_new_db(counter_sys, i);
			}
			else{
				this->set_hyd_system_new_db(counter_sys, i);
			}
		}
		else{
			if (this->type == _hyd_thread_type::hyd_temp_calculation) {
				this->reset_hyd_temp_system_boundaries_db(counter_sys, i);

			}
			else {
				this->reset_hyd_system_boundaries_db(counter_sys, i);
			}
		}
	}
}
//Set hydraulic system(s) of the threads the first time per database
void Hyd_Multiple_Hydraulic_Systems::set_hyd_system_new_db(int *counter_sys, const int i){
	ostringstream cout;
	ostringstream prefix;
	
	do{
		if(*counter_sys==this->sz_bound_manager.get_number_sz()){
			break;
		}
		if(this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_is_selected()==true){
			//new setting of the system
			if(i==0){
				this->threads[i].set_system_number(this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_id());
				ostringstream buffer;
				buffer << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_id()<<"-"<<this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name();
				this->threads[i].set_identifier_string(buffer.str());
				

				//set hydraulic boundary scenario id
				this->threads[i].set_new_hyd_bound_sz_id(*this->sz_bound_manager.get_ptr_sz(*counter_sys));
				cout <<"Preprocessing of hydraulic system " << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name() << " thread no. " << i<<"..."<< endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				prefix << this->threads[i].get_identifier_prefix();
				Sys_Common_Output::output_hyd->set_userprefix(&prefix);
				try{
					
					//preprocessing
					this->threads[i].set_system_per_database();
					buffer << ".CA";
					this->threads[i].set_folder_name(buffer.str(), true);
					Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
					this->threads[i].output_glob_models();
					Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
					this->threads[i].init_models();
					Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
					this->threads[i].output_setted_members();
					Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
					this->threads[i].init_solver();
					this->threads[i].set_preproc_flag(true);
				}
				catch(Error msg){
					if(msg.get_user_aborted_exception()==false){
						ostringstream info;
						info <<"Number hydraulic system : "<< *counter_sys<< endl;
						info <<"Name hydraulic system   : "<< this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name()<< endl;
						msg.make_second_info(info.str());
						this->counter_error++;
						this->threads[i].increase_error_number();
						msg.output_msg(2);
						this->threads[i].output_final_model_statistics();
						this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
						this->threads[i].total_reset();
					}
					else{
						Sys_Common_Output::output_hyd->rewind_userprefix();
						throw msg;
					}

				}
				Sys_Common_Output::output_hyd->rewind_userprefix();
			}
			//just cloning
			else{
				try{
					this->threads[i].clone_system(&this->threads[0]);
				}
				catch(Error msg){
					if(msg.get_user_aborted_exception()==false){
						ostringstream info;
						info <<"Thread number : "<< i << endl;
						msg.make_second_info(info.str());
						this->counter_error++;
						this->threads[i].increase_error_number();
						msg.output_msg(2);
						this->threads[i].output_final_model_statistics();
						this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
						this->threads[i].total_reset();
					}
					else{
						throw msg;
					}
				}
				this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
				//set boundary
				this->threads[i].reset_system();
				this->threads[i].set_system_number(this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_id());
				ostringstream buffer;
				buffer << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_id()<<"-"<<this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name();
				this->threads[i].set_identifier_string(buffer.str());
				buffer<<".CA";
				this->threads[i].set_folder_name(buffer.str(), true);
				//set hydraulic boundary scenario id
				this->threads[i].set_new_hyd_bound_sz_id(*this->sz_bound_manager.get_ptr_sz(*counter_sys));
				cout <<"Preprocessing of hydraulic boundary condition for system " << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name() << " thread no. " << i<<"..."<< endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				prefix << this->threads[i].get_identifier_prefix();
				Sys_Common_Output::output_hyd->set_userprefix(&prefix);
				try{
					//preprocessing
					this->threads[i].clear_boundary_conditions();
					Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
					this->threads[i].set_new_boundary_condition();
					Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
					this->threads[i].set_preproc_flag(true);
				}
				catch(Error msg){
					if(msg.get_user_aborted_exception()==false){
						ostringstream info;
						info <<"Number hydraulic system : "<< *counter_sys<< endl;
						info <<"Name hydraulic system   : "<< this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name()<< endl;
						msg.make_second_info(info.str());
						this->counter_error++;
						this->threads[i].increase_error_number();
						msg.output_msg(2);
						this->threads[i].output_final_model_statistics();
						this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
						this->threads[i].reset_system();
					}
					else{
						Sys_Common_Output::output_hyd->rewind_userprefix();
						throw msg;
					}
				}
				Sys_Common_Output::output_hyd->rewind_userprefix();
			}
			
		}
		(*counter_sys)++;
	}
	while(*counter_sys<this->sz_bound_manager.get_number_sz() && this->threads[i].get_preproc_flag()==false);
}
//Reset hydraulic system(s) boundary condition and set them new per databse
void Hyd_Multiple_Hydraulic_Systems::reset_hyd_system_boundaries_db(int *counter_sys, const int i){
	ostringstream cout;
	ostringstream prefix;
	this->threads[i].reset_system();
	do{
		if(*counter_sys==this->sz_bound_manager.get_number_sz()){
			break;
		}
		if(this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_is_selected()==true){
			this->threads[i].set_system_number(this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_id());
			ostringstream buffer;
			buffer << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_id()<<"-"<<this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name();
			this->threads[i].set_identifier_string(buffer.str());
			buffer<<".CA";
			this->threads[i].set_folder_name(buffer.str(), true);
			//set hydraulic boundary scenario id
			this->threads[i].set_new_hyd_bound_sz_id(*this->sz_bound_manager.get_ptr_sz(*counter_sys));
			cout <<"Preprocessing of hydraulic boundary condition for system " << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name() << " thread no. " << i<<"..."<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			prefix << this->threads[i].get_identifier_prefix();
			Sys_Common_Output::output_hyd->set_userprefix(&prefix);
			try{
				//preprocessing
				this->threads[i].clear_boundary_conditions();
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->threads[i].set_new_boundary_condition();
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->threads[i].set_preproc_flag(true);
			}
			catch(Error msg){
				if(msg.get_user_aborted_exception()==false){
					ostringstream info;
					info <<"Number hydraulic system : "<< *counter_sys<< endl;
					info <<"Name hydraulic system   : "<< this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name()<< endl;
					msg.make_second_info(info.str());
					this->counter_error++;
					this->threads[i].increase_error_number();
					msg.output_msg(2);
					this->threads[i].output_final_model_statistics();
					this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
					this->threads[i].reset_system();
				}
				else{
					Sys_Common_Output::output_hyd->rewind_userprefix();
					throw msg;
				}

			}
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
		(*counter_sys)++;
	}
	while(*counter_sys<this->sz_bound_manager.get_number_sz() && this->threads[i].get_preproc_flag()==false);

}
//Set hydraulic temperature system(s) of the threads the first time per database for calculation
void Hyd_Multiple_Hydraulic_Systems::set_hyd_temp_system_new_db(int *counter_sys, const int i) {
		ostringstream cout;
	ostringstream prefix;
	
	do{
		if(*counter_sys==this->sz_bound_manager.get_number_sz()){
			break;
		}
		if(this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_is_selected()==true){
			//new setting of the system
			if(i==0){
				this->threads[i].set_system_number(this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_id());
				ostringstream buffer;
				buffer << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_id()<<"-"<<this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name();
				this->threads[i].set_identifier_string(buffer.str());
				

				//set hydraulic boundary scenario id
				this->threads[i].set_new_hyd_bound_sz_id(*this->sz_bound_manager.get_ptr_sz(*counter_sys));
				cout <<"Preprocessing of temperature system " << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name() << " thread no. " << i<<"..."<< endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				prefix << this->threads[i].get_identifier_prefix();
				Sys_Common_Output::output_hyd->set_userprefix(&prefix);
				try{
					
					//preprocessing
					this->threads[i].set_temp_system_per_database();
					buffer << ".CA";
					this->threads[i].set_folder_name(buffer.str(), true);
					Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
					this->threads[i].output_glob_models();
					Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
					this->threads[i].init_temp_models();
					Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
					this->threads[i].output_setted_temp_members();
					Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
					this->threads[i].init_temp_solver();
					this->threads[i].set_preproc_flag(true);
				}
				catch(Error msg){
					if(msg.get_user_aborted_exception()==false){
						ostringstream info;
						info <<"Number hydraulic system : "<< *counter_sys<< endl;
						info <<"Name hydraulic system   : "<< this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name()<< endl;
						msg.make_second_info(info.str());
						this->counter_error++;
						this->threads[i].increase_error_number();
						msg.output_msg(2);
						this->threads[i].output_final_model_statistics();
						this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
						this->threads[i].total_reset();
					}
					else{
						Sys_Common_Output::output_hyd->rewind_userprefix();
						throw msg;
					}

				}
				Sys_Common_Output::output_hyd->rewind_userprefix();
			}
			//just cloning
			else{
				try{
					this->threads[i].clone_system(&this->threads[0]);
				}
				catch(Error msg){
					if(msg.get_user_aborted_exception()==false){
						ostringstream info;
						info <<"Thread number : "<< i << endl;
						msg.make_second_info(info.str());
						this->counter_error++;
						this->threads[i].increase_error_number();
						msg.output_msg(2);
						this->threads[i].output_final_model_statistics();
						this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
						this->threads[i].total_reset();
					}
					else{
						throw msg;
					}
				}
				this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
				//set boundary
				this->threads[i].reset_system();
				this->threads[i].set_system_number(this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_id());
				ostringstream buffer;
				buffer << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_id()<<"-"<<this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name();
				this->threads[i].set_identifier_string(buffer.str());
				buffer<<".CA";
				this->threads[i].set_folder_name(buffer.str(), true);
				//set hydraulic boundary scenario id
				this->threads[i].set_new_hyd_bound_sz_id(*this->sz_bound_manager.get_ptr_sz(*counter_sys));
				cout <<"Preprocessing of hydraulic boundary condition for system " << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name() << " thread no. " << i<<"..."<< endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				prefix << this->threads[i].get_identifier_prefix();
				Sys_Common_Output::output_hyd->set_userprefix(&prefix);
				try{
					//preprocessing
					this->threads[i].clear_boundary_conditions();
					Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
					this->threads[i].set_new_boundary_condition();
					Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
					this->threads[i].set_preproc_flag(true);
				}
				catch(Error msg){
					if(msg.get_user_aborted_exception()==false){
						ostringstream info;
						info <<"Number hydraulic system : "<< *counter_sys<< endl;
						info <<"Name hydraulic system   : "<< this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name()<< endl;
						msg.make_second_info(info.str());
						this->counter_error++;
						this->threads[i].increase_error_number();
						msg.output_msg(2);
						this->threads[i].output_final_model_statistics();
						this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
						this->threads[i].reset_system();
					}
					else{
						Sys_Common_Output::output_hyd->rewind_userprefix();
						throw msg;
					}
				}
				Sys_Common_Output::output_hyd->rewind_userprefix();
			}
			
		}
		(*counter_sys)++;
	}
	while(*counter_sys<this->sz_bound_manager.get_number_sz() && this->threads[i].get_preproc_flag()==false);






}
//Reset hydraulic temperature system(s) boundary condition and set them new per databse for calculation
void Hyd_Multiple_Hydraulic_Systems::reset_hyd_temp_system_boundaries_db(int *counter_sys, const int i) {

	ostringstream cout;
	ostringstream prefix;
	this->threads[i].reset_system();
	do {
		if (*counter_sys == this->sz_bound_manager.get_number_sz()) {
			break;
		}
		if (this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_is_selected() == true) {
			this->threads[i].set_system_number(this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_id());
			ostringstream buffer;
			buffer << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_id() << "-" << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name();
			this->threads[i].set_identifier_string(buffer.str());
			buffer << ".CA";
			this->threads[i].set_folder_name(buffer.str(), true);
			//set hydraulic boundary scenario id
			this->threads[i].set_new_hyd_bound_sz_id(*this->sz_bound_manager.get_ptr_sz(*counter_sys));
			cout << "Preprocessing of temperature boundary condition for system " << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name() << " thread no. " << i << "..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			prefix << this->threads[i].get_identifier_prefix();
			Sys_Common_Output::output_hyd->set_userprefix(&prefix);
			try {
				//preprocessing
				this->threads[i].clear_boundary_conditions();
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->threads[i].set_new_boundary_condition();
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->threads[i].set_preproc_flag(true);
			}
			catch (Error msg) {
				if (msg.get_user_aborted_exception() == false) {
					ostringstream info;
					info << "Number hydraulic system : " << *counter_sys << endl;
					info << "Name hydraulic system   : " << this->sz_bound_manager.get_ptr_sz(*counter_sys)->get_name() << endl;
					msg.make_second_info(info.str());
					this->counter_error++;
					this->threads[i].increase_error_number();
					msg.output_msg(2);
					this->threads[i].output_final_model_statistics();
					this->counter_warnings = this->counter_warnings + this->threads[i].get_number_warnings();
					this->threads[i].reset_system();
				}
				else {
					Sys_Common_Output::output_hyd->rewind_userprefix();
					throw msg;
				}

			}
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
		(*counter_sys)++;
	} while (*counter_sys < this->sz_bound_manager.get_number_sz() && this->threads[i].get_preproc_flag() == false);



}
//Calculate the hydraulic system
void Hyd_Multiple_Hydraulic_Systems::calculate_hyd_system(void){
	for(int i=0; i<this->required_threads; i++){
		if(threads[i].get_preproc_flag()==true){
			this->threads[i].start();
		}
	}
	this->wait_loop();

	//count errors
	for(int i=0; i<this->required_threads; i++){
		if(threads[i].get_preproc_flag()==true){
			this->counter_error=this->counter_error+this->threads[i].get_number_errors();
		}
	}
}
//Calculate the hydraulic temperature system
void Hyd_Multiple_Hydraulic_Systems::calculate_hyd_temp_system(void) {
	for (int i = 0; i < this->required_threads; i++) {
		
		if (threads[i].get_preproc_flag() == true) {
			this->threads[i].start();
		}
	}
	this->wait_loop();

	//count errors
	for (int i = 0; i < this->required_threads; i++) {
		if (threads[i].get_preproc_flag() == true) {
			this->counter_error = this->counter_error + this->threads[i].get_number_errors();
		}
	}

}
//Perform the postprocessing of the hydraulic system(s)
void Hyd_Multiple_Hydraulic_Systems::make_postprocessing_hyd_system(void){
	if(Hyd_Multiple_Hydraulic_Systems::abort_thread_flag==false){
		for(int i=0; i<this->required_threads; i++){
			if(threads[i].get_preproc_flag()==true){
				try{
					ostringstream cout;
					cout <<"Postprocessing of hydraulicsystem " << this->threads[i].get_identifier_prefix(false)<<"..."<< endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
					this->threads[i].output_final_model_statistics();
					this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
					Sys_Common_Output::output_hyd->insert_separator(4);
				}
				catch(Error msg){
					ostringstream info;
					info <<"Number hydraulic system : "<< this->threads[i].get_identifier_prefix(false)<< endl;
					msg.make_second_info(info.str());
					this->counter_error++;
					this->threads[i].increase_error_number();
					msg.output_msg(2);
				}
			}
		}
	}
}
//Perform the postprocessing of the hydraulic temperature system(s) calculation 
void Hyd_Multiple_Hydraulic_Systems::make_postprocessing_hyd_temp_system(void) {
	if (Hyd_Multiple_Hydraulic_Systems::abort_thread_flag == false) {
		for (int i = 0; i < this->required_threads; i++) {
			if (threads[i].get_preproc_flag() == true) {
				try {
					ostringstream cout;
					cout << "Postprocessing of hydraulic temperature system " << this->threads[i].get_identifier_prefix(false) << "..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
					this->threads[i].output_final_model_statistics();
					this->counter_warnings = this->counter_warnings + this->threads[i].get_number_warnings();
					Sys_Common_Output::output_hyd->insert_separator(4);
				}
				catch (Error msg) {
					ostringstream info;
					info << "Number hydraulic temperature system : " << this->threads[i].get_identifier_prefix(false) << endl;
					msg.make_second_info(info.str());
					this->counter_error++;
					this->threads[i].increase_error_number();
					msg.output_msg(2);
				}
			}
		}
	}

}
//Waiting loop for the threads
void Hyd_Multiple_Hydraulic_Systems::wait_loop(void){
	bool all_finished=true;
	int counter=0;
	ostringstream buffer;
	//waiting loop for this thread
	do{
		all_finished=true;
		counter=0;
		buffer.str("");
		for(int i=0; i<this->required_threads; i++){
			if(this->threads[i].isRunning()==true){
				all_finished=false;
				counter++;
			}
		}
		buffer << counter;
		emit emit_number_threads(buffer.str().c_str());
		if(all_finished==false){
			this->sleep(2);
		}
	}
	while(all_finished==false);
}
//Allocate the file names
void Hyd_Multiple_Hydraulic_Systems::allocate_file_names(void){
	try{
		this->file_names=new string[this->number_systems];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the file names
void Hyd_Multiple_Hydraulic_Systems::delete_file_names(void){
	if(this->file_names!=NULL){
		delete []this->file_names;
		this->file_names=NULL;
	}
}
//Calculate one/multiple hydraulic system(s) from files
void Hyd_Multiple_Hydraulic_Systems::calculate_file_systems(void){
	int counter_sys=0;
	//begin time recording	
	time(&this->start_time);

	try{
		this->allocate_hyd_threads();
		do{
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->set_hyd_system_new_file(&counter_sys);
			this->calculate_hyd_system();
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->make_postprocessing_hyd_system();
		}
		while(counter_sys<this->number_systems);
	}
	catch(Error msg){
		if(msg.get_user_aborted_exception()==false){
			this->counter_error++;
			msg.output_msg(2);
		}
		else{
			for(int i=0; i<this->required_threads; i++){
				this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
			}
		}

	}
	this->delete_hyd_threads();
	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic();
}
//Import the base hydraulic system from a file to the database (hydraulic boundary szenario 1)
void Hyd_Multiple_Hydraulic_Systems::import_basesystem_file2database(void){


	try{
		//check first the base scenario
		ostringstream cout;
		cout <<"Check the hydfraulic system in file(s) before the data import"<< endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		this->check_hydraulic_system_perfile();
		if(this->get_error_number()>0){
			this->delete_hyd_threads();
			ostringstream cout;
			cout <<"System check was not successfull. No import is done. Check the hydraulic system in file(s) before import."<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			return;

		}
		else{
			this->delete_hyd_threads();
		}

		//begin time recording	
		time(&this->start_time);
		//now transfer it to database
		//allocate
		this->allocate_hyd_threads();
		this->threads[0].check_hyd_database_tables();

		//boundary szenario
		this->sz_bound_manager.delete_table_create_base_sz(&this->qsqldatabase);
		this->sz_bound_manager.set_szenario_per_db(&this->qsqldatabase);
		this->threads[0].hyd_sz.set_members(this->sz_bound_manager.get_ptr_sz(0));
		//this->threads[0].set_folder_name("", false);
		this->threads[0].set_folder_name_file();
		
		//set numbers
		this->threads[0].set_thread_number(0);
		this->threads[0].set_system_number(0);
		cout <<"Import the data of the base hydraulic system to the database ..."<< endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());


		
		//make the import
		this->threads[0].import_basesystem_file2db(this->file_names[0]);

	}
	catch(Error msg){

		if(msg.get_user_aborted_exception()==false){
			this->threads[0].increase_error_number();
		}
		msg.output_msg(2);
		this->delete_hydraulic_boundary_scenarios(1);
		this->threads[0].delete_data_hyd_database_tables();
	}
	ostringstream cout;
	cout <<"Data-Import of the base hydraulic system is finished"<< endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	this->counter_warnings=this->counter_warnings+this->threads[0].get_number_warnings();
	this->counter_error=this->counter_error+this->threads[0].get_number_errors();
	//begin time recording	
	time(&this->actual_time);
	this->output_final_statistic_basesystem_import();
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Add additional boundary szenarios from a file to the database 
void Hyd_Multiple_Hydraulic_Systems::add_boundary_szenarios_file2database(void){

	//set prefix for output
	ostringstream prefix;
	//set prefix
	prefix << "ADD> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	prefix.str("");
	prefix.clear();

	int sz_id=0;
	//begin time recording	
	time(&this->start_time);
	Hyd_Hydraulic_System base_system;
	base_system.set_system_number(0);
	base_system.set_ptr2database(this->qsqldatabase,0);
	//check the tables
	try{
		base_system.check_hyd_database_tables();
	}
	catch(Error msg){
		msg.output_msg(2);
		this->counter_error++;
		Sys_Common_Output::output_hyd->rewind_userprefix();
		return;
	}

	_sys_system_id base;
	base.area_state=this->system_id.area_state;
	base.measure_nr=0;
	this->sz_bound_manager.set_systemid(base);
	//set the database
	this->sz_bound_manager.set_existing_szenario_per_db(&this->qsqldatabase);
	//check if there is an base system
	if(this->sz_bound_manager.get_number_sz()==0){
		Error msg=this->set_error(2);
		msg.output_msg(2);
		this->counter_error++;
		Sys_Common_Output::output_hyd->rewind_userprefix();
		return;
	}

	//set the base system
	try{
		ostringstream cout;
		cout <<"Set the base hydraulic system per database..."<< endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		base_system.hyd_sz.set_members(this->sz_bound_manager.get_ptr_sz(0));
		base_system.set_system_per_database();

	}
	catch(Error msg){
		ostringstream info;
		info << "The hydraulic base system is not properly stored in the database" << endl;
		info << "Check it or import a new proper hydraulic base system" << endl;
		msg.set_fatal_flag(true);
		msg.make_second_info(info.str());
		msg.output_msg(2);
		this->counter_error++;
		Sys_Common_Output::output_hyd->rewind_userprefix();
		return;
	}

	this->sz_bound_manager.set_systemid(this->system_id);
	//start adding
	this->allocate_hyd_threads();
	for(int i=0; i< this->number_systems; i++){
		try{
			this->threads[0].total_reset();
			this->threads[0].set_system_number(i+1);
			ostringstream cout;
			cout <<"Adding hydraulic boundary scenario (just boundary conditions)" << i+1<<" to database with scenario id " <<sz_id<<"..."<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			sz_id=this->sz_bound_manager.get_new_hyd_sz_id(&this->qsqldatabase);
			//set prefix
			prefix << "BSZ_"<<sz_id<<"> ";
			Sys_Common_Output::output_hyd->set_userprefix(&prefix);
			//adding
			try{
				//set the system to add per file
				this->threads[0].set_system_per_file(this->file_names[i]);
				//compare it to the base hydraulic system already stored in the database
				base_system.compare_hydraulic_system(&this->threads[0]);
				//transfer the global element id of the elements for faster database import
				base_system.transfer_glob_elem_id(&this->threads[0]);
				//set a new hydraulic boundary scenario
				Hyd_Boundary_Szenario new_sz;
				this->sz_bound_manager.insert_new_hyd_sz(&new_sz,&this->qsqldatabase, i);
				this->sz_bound_manager.set_szenario_per_db(&this->qsqldatabase);
				this->threads[0].set_new_hyd_bound_sz_id(new_sz);
				//transfer the hydraulic boundary informations to the database
				this->threads[0].import_new_hydraulic_boundary_sz2database();
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

			}
			catch(Error msg){
				//rewind the prefix
				Sys_Common_Output::output_hyd->rewind_userprefix();
				throw msg;
				
			}
			//rewind the prefix
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
		catch(Error msg){
			this->delete_hydraulic_boundary_scenarios(sz_id);
			if(msg.get_user_aborted_exception()==false){
				ostringstream info;
				info <<"Number hydraulic boundary scenario : "<< i<< endl;
				info <<"Id hydraulic boundary scenario     : "<< sz_id<< endl;
				info <<"This hydraulic boundary scenario is deleted from database"<< endl;
				msg.make_second_info(info.str());
				this->counter_error++;
				this->threads[0].increase_error_number();
				msg.output_msg(2);
			}
			else{
				msg.output_msg(2);
				break;
			}
		}
		this->counter_warnings=this->counter_warnings+this->threads[0].get_number_warnings();
	}

	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_add_boundsz();
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Create the required database tables for an hydraulic project
void Hyd_Multiple_Hydraulic_Systems::create_database_tables(void){

	this->counter_warnings=Warning::get_number_sys_warnings();

	ostringstream cout;
	cout << "Create HYD database tables..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	try{
		//allocate
		this->allocate_hyd_threads();
		//set numbers
		this->threads[0].set_system_number(0);

		//create the tables rekursive
		this->threads[0].create_hyd_database_tables();

	}
	catch(Error msg){
		msg.output_msg(2);
		this->counter_error++;
	}
	this->counter_warnings=Warning::get_number_sys_warnings()-this->counter_warnings;
	cout << "Creation of the HYD database tables is finished" << endl ;
	this->output_final_statistic_create();
	Sys_Common_Output::output_hyd->output_txt(&cout);
}
//Check the database for the required tables for an hydraulic project
void Hyd_Multiple_Hydraulic_Systems::check_database_tables(void){
	//set prefix for output
	this->counter_warnings=Warning::get_number_sys_warnings();

	ostringstream cout;
	cout << "Check HYD database tables..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
	try{
		//allocate
		this->allocate_hyd_threads();
		//set numbers
		this->threads[0].set_system_number(0);

		//create the tables
		this->threads[0].check_hyd_database_tables();

	}
	catch(Error msg){
		//switch to non-fatal, because is handled later as fatal error
		this->counter_error++;
		msg.set_fatal_flag(false);
		ostringstream info;
		info << "Check if the HYD tables are created!"<< endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
		this->output_final_statistic_check();
		
	}
	this->counter_warnings=Warning::get_number_sys_warnings()-this->counter_warnings;
	cout << "Check of the HYD database tables is finished" << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
	
}
//Check a hydraulic system, read in from a file, for the data, the geometrical interceptions, the couplings etc.; here the complete pre-processing is performed
void Hyd_Multiple_Hydraulic_Systems::check_hydraulic_system_perfile(void){
	//set prefix for output
	ostringstream prefix;
	
	//begin time recording	
	time(&this->start_time);

	this->allocate_hyd_threads();
	for(int i=0; i< this->number_systems; i++){

		try{
			//allocate
			this->threads[0].total_reset();
			//set numbers
			this->threads[0].set_system_number(i);
			ostringstream cout;
			cout <<"Check of hydraulic system " << i<<"..."<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//set prefix
			prefix << "CHECK_SYS_"<<i<<"> ";
			Sys_Common_Output::output_hyd->set_userprefix(&prefix);
			//preprocessing
			this->threads[0].set_system_per_file(this->file_names[i]);
			this->threads[0].output_glob_models();
			this->threads[0].init_models();
			this->threads[0].set_folder_name_file();
			this->threads[0].output_setted_members();
			this->threads[0].init_solver();
			//rewind the prefix
			Sys_Common_Output::output_hyd->rewind_userprefix();

		}
		catch(Error msg){
			if(msg.get_user_aborted_exception()==false){
				ostringstream info;
				info <<"Number hydraulic system : "<< i<< endl;
				msg.make_second_info(info.str());
				this->counter_error++;
				this->threads[0].increase_error_number();
				msg.output_msg(2);
			}
			else{
				msg.output_msg(2);
				break;
			}
		}
		this->counter_warnings=this->counter_warnings+this->threads[0].get_number_warnings();

	}

	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_systemcheck();
}
//Check a hydraulic system, read in from a database, for the data, the geometrical interceptions, the couplings etc.; here the complete pre-processing is performed
void Hyd_Multiple_Hydraulic_Systems::check_hydraulic_system_database(void){
	//set prefix for output
	ostringstream prefix;
	this->number_systems=1;


	//set the base system for checking
	this->sz_bound_manager.set_szenario_per_db(&this->qsqldatabase);
	if(this->sz_bound_manager.get_number_sz()==0){
		ostringstream cout;
		cout <<"No hydraulic system for checking is found"<< endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		return;
	}
	
	//begin time recording	
	time(&this->start_time);
	this->allocate_hyd_threads();
	try{
		//check the database tables
		this->threads[0].check_hyd_database_tables();
		//set numbers
		this->threads[0].set_system_number(0);
		//hydraulic boundary scenario id
		this->threads[0].set_new_hyd_bound_sz_id(*this->sz_bound_manager.get_ptr_sz(0));
		ostringstream cout;
		cout <<"Check of hydraulic system in database..."<< endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//set prefix
		prefix << "CHECK_SYS> ";
		Sys_Common_Output::output_hyd->set_userprefix(&prefix);
		//preprocessing=checking
		this->threads[0].set_system_per_database();
		this->threads[0].output_glob_models();
		this->threads[0].init_models();
		this->threads[0].output_setted_members();
		this->threads[0].init_solver();
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();

	}
	catch(Error msg){
		if(msg.get_user_aborted_exception()==false){
			this->counter_error++;
			this->threads[0].increase_error_number();
			msg.output_msg(2);
		}
		else{
			msg.output_msg(2);
		}
	}
	this->counter_warnings=this->counter_warnings+this->threads[0].get_number_warnings();

	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_systemcheck();
}
//Calculate one/multiple hydraulic system, read in from a database
void Hyd_Multiple_Hydraulic_Systems::calculate_hydraulic_system_database(void){
	int counter_sys=0;
	ostringstream cout;
	//begin time recording	
	time(&this->start_time);

	try{
		this->allocate_hyd_threads();
		do{
			if(counter_sys>0){
				cout.str("");
				cout <<counter_sys << " of " << this->sz_bound_manager.get_number_sz();
				emit_number_performed_calculation(cout.str().c_str());
			}
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->decide_new_reset_db(&counter_sys);
			this->calculate_hyd_system();
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->make_postprocessing_hyd_system();
			

		}
		while(counter_sys<this->sz_bound_manager.get_number_sz());
	}
	catch(Error msg){
		if(msg.get_user_aborted_exception()==false){
			this->counter_error++;
			msg.output_msg(2);
		}
		else{
			for(int i=0; i<this->required_threads; i++){
				this->counter_warnings=this->counter_warnings+this->threads[i].get_number_warnings();
			}
		}

	}

	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_multi_hydsz();

}
//Calculate one/multiple hydraulic temperature system, read in from a database
void Hyd_Multiple_Hydraulic_Systems::calculate_hydraulic_temp_system_database(void) {

	int counter_sys = 0;
	ostringstream cout;
	//begin time recording	
	time(&this->start_time);

	try {
		this->allocate_hyd_threads();
		do {
			if (counter_sys > 0) {
				cout.str("");
				cout << counter_sys << " of " << this->sz_bound_manager.get_number_sz();
				emit_number_performed_calculation(cout.str().c_str());
			}
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->decide_new_reset_db(&counter_sys);
			this->calculate_hyd_temp_system();
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->make_postprocessing_hyd_temp_system();


		} while (counter_sys < this->sz_bound_manager.get_number_sz());
	}
	catch (Error msg) {
		if (msg.get_user_aborted_exception() == false) {
			this->counter_error++;
			msg.output_msg(2);
		}
		else {
			for (int i = 0; i < this->required_threads; i++) {
				this->counter_warnings = this->counter_warnings + this->threads[i].get_number_warnings();
			}
		}

	}

	//set the actual time
	time(&this->actual_time);
	this->output_final_statistic_multi_hydsz();

}
//Delete selected boundary scenarios from the database table
void Hyd_Multiple_Hydraulic_Systems::delete_selected_scenarios(void){
	try{
		ostringstream cout;
		cout <<"Delete selected hydraulic boundary scenarios..."<< endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		int counter=0;
		for(int i=0; i< this->sz_bound_manager.get_number_sz(); i++){
			if(this->sz_bound_manager.get_ptr_sz(i)->get_is_selected()==true){
				cout <<"Delete hydraulic boundary scenario "<<this->sz_bound_manager.get_ptr_sz(i)->get_name()<<"..."<< endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				this->delete_hydraulic_boundary_scenarios(this->sz_bound_manager.get_ptr_sz(i)->get_id());
				counter++;
			}
		}

		//if all scenarios are delete, delete also the geometric data of the hydraulic system
		if(counter==this->sz_bound_manager.get_number_sz()){
			//allocate
			this->set_number_required_threads(1);
			this->allocate_hyd_threads();
			//delete all infos in database table
			this->threads[0].delete_data_hyd_database_tables();
		}
	}
	catch(Error msg){
		msg.output_msg(2);
	}

}
//Convert a given Gis-raster into a Prom-raster
void Hyd_Multiple_Hydraulic_Systems::convert_Gis2Promraster(void){
	this->counter_warnings=Warning::get_number_sys_warnings();
	//begin time recording	
	time(&this->start_time);

	ostringstream cout;
	cout << "Convert Gis-raster to ProMaIDes HYD-raster..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	try{

		this->allocate_raster_class();
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->gis_raster->input_gis_raster(this->raster_import_dia->get_file_path(0).c_str());
		Sys_Common_Output::output_hyd->output_txt(&cout);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->prom_raster->input_prom_raster(this->raster_import_dia->get_file_path(1).c_str());
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		if(this->prom_raster->intercept_rasters(this->gis_raster)==true){;
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->prom_raster->output_prom_raster2file();
		}




	}
	catch(Error msg){
		msg.output_msg(2);
		this->counter_error++;
	}
	this->counter_warnings=Warning::get_number_sys_warnings()-this->counter_warnings;
	cout << "Conversion of Gis-raster to ProMaIDes HYD-raster is finished!" << endl ;
	
	//set the actual time
	time(&this->actual_time);

	this->output_final_statistic_raster_convert();
	Sys_Common_Output::output_hyd->output_txt(&cout);

}
//Convert a profile data to a river DGM-W
void Hyd_Multiple_Hydraulic_Systems::convert_profil2dgmw(void){

	//begin time recording	
	time(&this->start_time);

	ostringstream cout;
	cout << "Convert profile data to river DTM-W..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	try{

		this->allocate_profile_conversion();
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->profil_dgm_conversion->set_dia_ptr(this->profil_import_dia);
		this->profil_dgm_conversion->start_interpolation();
		

	}
	catch(Error msg){
		msg.output_msg(2);
		this->counter_error++;
	}
	this->counter_warnings=Warning::get_number_sys_warnings()-this->counter_warnings;
	cout << "Conversion of profile data to river DTM-W is finished!" << endl ;
	
	//set the actual time
	time(&this->actual_time);

	this->output_final_statistic_raster_convert();
	Sys_Common_Output::output_hyd->output_txt(&cout);
	





}
//Output the statistic of the create hydraulic tables
void Hyd_Multiple_Hydraulic_Systems::output_final_statistic_create(void){
	ostringstream prefix;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Statistics of the hydraulic tables creation in the database..." << endl ;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)        :" << W(3) << this->counter_error <<endl;
	cout << " Warning(s)      :" << W(3) << this->counter_warnings <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the statistic of the create hydraulic tables
void Hyd_Multiple_Hydraulic_Systems::output_final_statistic_check(void){
	ostringstream prefix;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Statistics of the hydraulic tables check in the database..." << endl ;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)        :" << W(3) << this->counter_error <<endl;
	cout << " Warning(s)      :" << W(3) << this->counter_warnings <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the statistic of the raster conversion
void Hyd_Multiple_Hydraulic_Systems::output_final_statistic_raster_convert(void){
	ostringstream prefix;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Statistics of conversion..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Time            :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)        :" << W(3) << this->counter_error <<endl;
	cout << " Warning(s)      :" << W(3) << this->counter_warnings <<endl;
	if(abort_thread_flag==true){
		cout << " User has aborted the data-ímport " << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the statistic of the hydraulic system check-action
void Hyd_Multiple_Hydraulic_Systems::output_final_statistic_systemcheck(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "Statistics of the Hydraulic system(s) checks..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Number systems  :" << W(3) << this->number_systems <<endl;
	cout << " Time            :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)        :" << W(3) << this->counter_error <<endl;
	cout << " Warning(s)      :" << W(3) << this->counter_warnings <<endl;
	if(abort_thread_flag==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output the statistic of the data import of the hydraulic base system
void Hyd_Multiple_Hydraulic_Systems::output_final_statistic_basesystem_import(void){
	ostringstream prefix;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Statistics of the hydraulic data import of the hydraulic base system from file to database..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Global Filename :" << W(3) << this->file_names[0] <<endl;
	cout << " Time            :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)        :" << W(3) << this->counter_error <<endl;
	cout << " Warning(s)      :" << W(3) << this->counter_warnings <<endl;
	if(abort_thread_flag==true){
		cout << " User has aborted the data-ímport " << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the statistic of adding additional hydraulic boundary szenarios
void Hyd_Multiple_Hydraulic_Systems::output_final_statistic_add_boundsz(void){
	ostringstream prefix;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Statistics of adding additional hydraulic boundary scenarios..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Number boundary scenario(s) :" << W(3) << this->number_systems <<endl;
	cout << " Time                        :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)                    :" << W(3) << this->counter_error <<endl;
	cout << " Warning(s)                  :" << W(3) << this->counter_warnings <<endl;
	if(abort_thread_flag==true){
		cout << " User has aborted the data-ímport " << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the statistic of the multiple calculation
void Hyd_Multiple_Hydraulic_Systems::output_final_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "Statistics of the multiple Hydraulic simulation(s)..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Number systems  :" << W(3) << this->number_systems <<endl;
	cout << " Time            :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)        :" << W(3) << this->counter_error <<endl;
	cout << " Warning(s)      :" << W(3) << this->counter_warnings <<endl;
	if(abort_thread_flag==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the statistic of the multiple hydraulic boundary scenario calculation
void Hyd_Multiple_Hydraulic_Systems::output_final_statistic_multi_hydsz(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "Statistics of the multiple Hydraulic boundary scenario(s)..." << endl ;
	cout << "GENERAL"<< endl;
	cout << " Number scenario(s):" << W(3) << this->sz_bound_manager.counter_number_selected_scenarios() <<endl;
	cout << " Time              :" << "  " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output <<endl;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)          :" << W(3) << this->counter_error <<endl;
	cout << " Warning(s)        :" << W(3) << this->counter_warnings <<endl;
	if(abort_thread_flag==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Allocate the raster classes for conversion
void Hyd_Multiple_Hydraulic_Systems::allocate_raster_class(void){
	this->delete_raster_class();
	try{
		this->gis_raster=new Hyd_Gis2Promraster_Converter;
		this->prom_raster=new Hyd_Gis2Promraster_Converter;
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the raster classes for conversion
void Hyd_Multiple_Hydraulic_Systems::delete_raster_class(void){
	if(this->gis_raster!=NULL){
		delete this->gis_raster;
		this->gis_raster=NULL;
	}
	if(this->prom_raster!=NULL){
		delete this->prom_raster;
		this->prom_raster=NULL;
	}
}
//Allocate the dialog of the raster path for a  raster import
void Hyd_Multiple_Hydraulic_Systems::allocate_raster_path_dia(void){
	this->delete_raster_path_dia();
	try{
		this->raster_import_dia=new Sys_Multi_Filechooser_Dia;
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(4);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the dialog of the raster path for a raster import
void Hyd_Multiple_Hydraulic_Systems::delete_raster_path_dia(void){
	if(this->raster_import_dia!=NULL){
		delete this->raster_import_dia;
		this->raster_import_dia=NULL;
	}
}
//Allocate the dialog of the profile and streamline data
void Hyd_Multiple_Hydraulic_Systems::allocate_profile_dia(void){
		this->delete_profile_path_dia();
	try{
		this->profil_import_dia=new HydGui_Profil2Dgm_Converter_Dia;
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(5);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the dialog of the profile and streamline data
void Hyd_Multiple_Hydraulic_Systems::delete_profile_path_dia(void){
	if(this->profil_import_dia!=NULL){
		delete this->profil_import_dia;
		this->profil_import_dia=NULL;
	}
}
//Allocate the conversion class of profile and streamlines to DGM-W
void Hyd_Multiple_Hydraulic_Systems::allocate_profile_conversion(void){
	
	this->delete_profile_conversion();
	try{
		this->profil_dgm_conversion=new Hyd_Profil2Dgm_Converter;
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(6);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the conversion class of profile and streamlines to DGM-W
void Hyd_Multiple_Hydraulic_Systems::delete_profile_conversion(void){
	if(this->profil_dgm_conversion!=NULL){
		delete this->profil_dgm_conversion;
		this->profil_dgm_conversion=NULL;
	}
}
//set the error
Error Hyd_Multiple_Hydraulic_Systems::set_error(const int err_type){
	string place="Hyd_Multiple_Hydraulic_Systems::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;
	switch (err_type){
		case 0://bad alloc
			place.append("allocate_hydraulic_systems(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("allocate_file_names(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://no base system is set
			place.append("add_boundary_szenarios_file2database(void)");
			reason="There must be a base hydraulic system before adding new boundary scenarios";
			help="Add first a base hydraulic system";
			type=20;
			fatal=true;
			break;	
		case 3://bad alloc
			place.append("allocate_raster_class(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			fatal=true;
			break;
		case 4://bad alloc
			place.append("allocate_raster_path_dia(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 5://bad alloc
			place.append("allocate_profile_dia(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 6://bad alloc
			place.append("allocate_profile_conversion(void)");
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
//Set the warning
Warning Hyd_Multiple_Hydraulic_Systems::set_warning(const int warn_type){
	string place="Hyd_Multiple_Hydraulic_Systems::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://a multithreading is not possible 
			place.append("set_required_threads(const int number)") ;
			reason="The required number of threads exceeds the iideal number of threads (=number of cores-1, if number of cores>1) ";
			reaction="The ideal thread number is applied";
			help= "Check the existing cores of your workstation";
			type=14;	
			break;
		case 1://the type of thread is not specified 
			place.append("run(void)") ;
			reason="The thread type is unknown";
			reaction="No thread is launched";
			help= "Check the source code";
			type=5;	
			break;
		case 2://a multithreading is not possible 
			place.append("set_required_threads(const int number)") ;
			reason="The required number of threads exceeds the maximum number of threads";
			reaction="Required number of threads is set to the maximum number of threads";
			help= "Check the required number of threads";
			info << "Maximum number of threads : " <<constant::max_threads <<endl;
			type=14;	
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