//#include "Fpl_Wind_Fetch.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
Fpl_Wind_Fetch::Fpl_Wind_Fetch(void){
	this->fetch_number=0;
	this->fetches=NULL;
	this->bathymetrie=NULL;
	this->bathy_zones_number=0;
	Sys_Memory_Count::self()->add_mem((sizeof(Fpl_Wind_Fetch)), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Wind_Fetch::~Fpl_Wind_Fetch(void){
	//delete the fetches
	this->delete_fetches();
	this->delete_bathymetry();
	Sys_Memory_Count::self()->minus_mem((sizeof(Fpl_Wind_Fetch)), _sys_system_modules::FPL_SYS);//count the memory
}
//________________
//public
//Allocate and set the fetches from database table
int Fpl_Wind_Fetch::new_set_global_fetches(QSqlDatabase *ptr_database, const _sys_system_id id, const double section_direction, const int section_id){
	//mysql query with the query model
	QSqlQueryModel model;

	try{
		Fpl_Section_Points::select_different_zone_points(&model, id, section_id, fpl_label::fetches, ptr_database, fpl_label::region_surround, fpl_label::sec_all, false);
	}
	catch(Error msg){
		throw msg;
	}

	//give the numbers of fetches
	this->fetch_number=model.rowCount();
	//check
	if(this->fetch_number==0){
		return this->fetch_number;
		//Error msg=this->set_error(2);
		//ostringstream info;
		//info<< "Tabel name    : "<<Fpl_Section_Points::point_table->get_table_name()<<endl;
		//msg.make_second_info(info.str());
		//throw msg;
	}
	//allocate the fetches and a found flag
	this->allocate_fetches();

	
	//read the data in 
	for(int i=0;i< this->fetch_number; i++){
		this->fetches[i].length = model.record(i).value((Fpl_Section_Points::point_table->get_column_name(fpl_label::point_x_coor)).c_str()).toDouble();
		this->fetches[i].angle_to_north = model.record(i).value((Fpl_Section_Points::point_table->get_column_name(fpl_label::point_y_coor)).c_str()).toDouble();
		this->fetches[i].zone_number = model.record(i).value((Fpl_Section_Points::point_table->get_column_name(fpl_label::zone_id)).c_str()).toInt();
		this->fetches[i].dry_flag=false;
		this->fetches[i].ptr_random_bathy=NULL;
	}

	//check the logic of the fetches
	try{
		this->check_global_fetches(section_direction);
	}
	catch(Error msg){
		throw msg;
	}
	return this->fetch_number;
}
//Allocate and set the fetches from file
void Fpl_Wind_Fetch::new_set_global_fetches(QFile *ifile, int *line_counter,  const double section_direction){
	//read in
	string myline;
	QString qmyline;
	bool found_flag=false;

	//search for the begin-keywords
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::begin_wind_fetches){
			found_flag=true;
			break;
		}
		else if(myline==fpl_label::end_wind_fetches){
			break;
		}
		else if(myline==fpl_label::end_mech_wind_wave){
			break;
		}
	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(8);
		throw msg;
	}
	else{
		found_flag=false;
	}


	//find fetch number
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::end_wind_fetches){
			break;
		}
		else if(myline==fpl_label::end_mech_wind_wave){
			break;
		}
		if(myline.empty()==false){
			this->find_key_values_file(myline);
		}
		if(this->fetch_number>0){
			break;
		}
	}
	while(ifile->atEnd()==false);

	if(this->fetch_number<=0){
		Error msg=this->set_error(10);
		throw msg;
	}
	else{
		this->allocate_fetches();
	}
		
	//read in fetches
	int counter_point=0;
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::end_wind_fetches){
			break;
		}
		else if(myline==fpl_label::end_mech_wind_wave){
			break;
		}
		else if(myline.empty()==false){
			stringstream point_buff;
			point_buff << myline;

			point_buff >> this->fetches[counter_point].length
				>>  this->fetches[counter_point].angle_to_north
				>> this->fetches[counter_point].zone_number;
			this->fetches[counter_point].dry_flag=false;
			this->fetches[counter_point].ptr_random_bathy=NULL;
			
			if(point_buff.fail()==true){
				Error msg=this->set_error(15);
				ostringstream info;
				info << "Read line : " << myline << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			else{
				counter_point++;
			}
		}
	}
	while(ifile->atEnd()==false && counter_point<this->fetch_number);

	if(counter_point!=this->fetch_number){
		Error msg=this->set_error(13);
		ostringstream info;
		info << "Number found    : " << counter_point << endl;
		info << "Number required : " << this->fetch_number << endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	//check the logic of the fetches
	try{
		this->check_global_fetches(section_direction);
	}
	catch(Error msg){
		throw msg;
	}


}
//Allocate and set the random varaiables of the bathymetrie from database table
void Fpl_Wind_Fetch::new_set_bathymetry(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	

	QSqlQueryModel results;
	this->bathy_zones_number=Fpl_Random_Variables::select_variables_database(&results, id, section_id, fpl_label::bathymetrie, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all);


	this->allocate_bathymetry();

	_fpl_boundaries buff_bound;
	//set the bathymetry
	for(int i=0; i< this->bathy_zones_number; i++){
		this->bathymetrie[i].set_dist_type_position(results.record(i).value((Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id)).c_str()).toInt());
		this->bathymetrie[i].set_default_name(results.record(i).value((Fpl_Random_Variables::table->get_column_name(fpl_label::default_name)).c_str()).toString().toStdString());
		buff_bound.min=results.record(i).value((Fpl_Random_Variables::table->get_column_name(fpl_label::min_boundary)).c_str()).toDouble();
		buff_bound.max=results.record(i).value((Fpl_Random_Variables::table->get_column_name(fpl_label::max_boundary)).c_str()).toDouble();
		this->bathymetrie[i].set_boundaries(buff_bound);
		this->bathymetrie[i].set_distribution_type(Fpl_Distribution_Class_Mean::convert_txt2dist_type(results.record(i).value((Fpl_Random_Variables::table->get_column_name(fpl_label::distribution_type)).c_str()).toString().toStdString()));
		this->bathymetrie[i].set_zone_id(results.record(i).value((Fpl_Random_Variables::table->get_column_name(fpl_label::zone_id)).c_str()).toInt());
		this->bathymetrie[i].set_calculated_flag(false);
		this->bathymetrie[i].set_distribution_types(ptr_database);
	}
}
//Allocate and set the random varaiables of the bathymetrie from file
void Fpl_Wind_Fetch::new_set_bathymetry(QFile *ifile, int *line_counter){
	//read in
	string myline;
	QString qmyline;
	bool found_flag=false;

	//search for the begin-keywords
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::begin_bathymetry){
			found_flag=true;
			break;
		}
		else if(myline==fpl_label::end_bathmetry){
			break;
		}
		else if(myline==fpl_label::end_mech_wind_wave){
			break;
		}
	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(9);
		throw msg;
	}
	else{
		found_flag=false;
	}

	//find bathy number
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::end_bathmetry){
			break;
		}
		else if(myline==fpl_label::end_mech_wind_wave){
			break;
		}
		if(myline.empty()==false){
			this->find_key_values_file(myline);
		}
		if(this->bathy_zones_number>0){
			break;
		}
	}
	while(ifile->atEnd()==false);

	int counter=0;
	if(this->bathy_zones_number<=0){
		Error msg=this->set_error(12);
		throw msg;
	}
	else{
		this->allocate_bathymetry();

		do{
			this->bathymetrie[counter].set_input(ifile, line_counter, fpl_label::bathymetrie, false, fpl_label::end_bathmetry, true); 
			counter++;
			if(ifile->atEnd()==true){
				break;
			}

		}
		while(counter < this->bathy_zones_number);
	}

	//check the number of zone
	if(counter!=this->bathy_zones_number){
		Error msg=this->set_error(14);
		ostringstream info;
		info <<"Number of bathymetry zones found   : " << counter << endl;
		info <<"Number of bathymetry zone required : " << this->bathy_zones_number << endl;
		msg.make_second_info(info.str());
		throw msg;

	}



}
//Connect bathymetrie and fetches
void Fpl_Wind_Fetch::connect_fetches2bathy(void){

	bool found=false;
	for(int i=0; i< this->fetch_number; i++){
		found=false;
		for(int j=0; j<this->bathy_zones_number; j++){
			if(this->fetches[i].zone_number==this->bathymetrie[j].get_zone_id()){
				found=true;
				this->fetches[i].ptr_random_bathy=&this->bathymetrie[j];
				break;
			}
		}
		if(found==false){
			Error msg=this->set_error(6);
			ostringstream info;
			info << "Number global fetch     : "<< i << endl; 
			info << "Number bathymetrie zone : "<<this->fetches[i].zone_number << endl;
			msg.make_second_info(info.str());
			throw msg;

		}
	}
}
//Transfer the wind-wave fetch data to database, e.g. geometry, random variables etc.
void Fpl_Wind_Fetch::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){

	//fetches
	Fpl_Section_Points buffer;
	for(int i=0; i< this->fetch_number; i++){
		buffer.set_point_coordinate(this->fetches[i].length, this->fetches[i].angle_to_north);
		buffer.set_zone_number(this->fetches[i].zone_number);
		buffer.set_point_name(fpl_label::fetches);
		buffer.transfer_point2database(id, section_id, ptr_database, fpl_label::region_surround, fpl_label::sec_all); 
	}
	//bathymetry
	for(int i=0; i< this->bathy_zones_number; i++){
		this->bathymetrie[i].transfer_rand2database(ptr_database, id, section_id, fpl_label::mech_wind_wave, fpl_label::sec_all); 
	}
}
//Output members to display/console
void Fpl_Wind_Fetch::output_members(void){
	ostringstream cout;
	ostringstream prefix;
	prefix << "FETCH> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);

	if(this->fetches!=NULL){
		//output the fetch geometrie

		cout << " no." << W(10) << "length" <<label::m << W(15)<< "angle"<<label::degree<< W(15)<< "bathy_zone" << W(15)<<"dry/wet"<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
		for(int i=0; i< this->fetch_number; i++){
			cout <<i <<W(10);
			cout <<this->fetches[i].length << W(15);
			cout << this->fetches[i].angle_to_north << W(15);
			cout << this->fetches[i].zone_number << W(15);
			cout << functions::convert_boolean2string(this->fetches[i].dry_flag);
			Sys_Common_Output::output_fpl->output_txt(&cout,true);
		}
		//output the bathymetrie
		prefix << "BATHY> ";
		Sys_Common_Output::output_fpl->set_userprefix(&prefix);
		cout << "Number of bathymetry zone(s) "<<this->bathy_zones_number<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
		for(int i=0;i<this->bathy_zones_number;i++){
			this->bathymetrie[i].output_member();
		}
		Sys_Common_Output::output_fpl->rewind_userprefix();
		
	}
	else{
		cout << "No fetches are set" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
	}
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Get the number of the fetches
int Fpl_Wind_Fetch::get_fetch_number(void){
	return this->fetch_number;
}
//Get the angle related to the north direction
double Fpl_Wind_Fetch::get_angle(const int zone_index){
	return this->fetches[zone_index].angle_to_north;
}
//Get the fetch length
double Fpl_Wind_Fetch::get_length(const int zone_index){
	return this->fetches[zone_index].length;
}
//Get if the fetch is dry
bool Fpl_Wind_Fetch::get_dry_flag(const int zone_index){
	return this->fetches[zone_index].dry_flag;
}
//Get index of the corresponding bathymetrie zone
int Fpl_Wind_Fetch::get_zone(const int zone_index){
	return this->fetches[zone_index].zone_number;
}
//Get random generated variables of the bathymetrie of each fetch zone
double Fpl_Wind_Fetch::calculate_batymetrie(const bool random_calculation, const int zone_index){
	double buffer;	
	buffer=this->fetches[zone_index].ptr_random_bathy->calculate_variables(random_calculation).end_result;
	return buffer;
	//here just bathymetrie but it can be add some other random variables per zone like vegetation (add to structure _fpl_wind_fetch; create new structure as return_value (_zone_random_var))
}
//Reset the random generated flags of the random variables
void Fpl_Wind_Fetch::reset_random_flag(void){
	for(int i=0; i<this->bathy_zones_number;i++){
		this->bathymetrie[i].reset_random_generated();
	}
}
//Check the statistic of the random variables
void Fpl_Wind_Fetch::check_statistic(void){
	for(int i=0; i<this->bathy_zones_number;i++){
		this->bathymetrie[i].check_statistic();
	}
}
//Reset the statistic important values
void Fpl_Wind_Fetch::reset_statistics(void){
	for(int i=0; i<this->bathy_zones_number;i++){
		this->bathymetrie[i].reset_statistics();
	}
}
//Output the statistics of the random variables to display/console
void Fpl_Wind_Fetch::output_statistic(void){
	ostringstream cout;
	ostringstream prefix;
	prefix << "BATHY> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	//output the bathymetrie
	cout << "Number of bathymetry zone(s) "<<this->bathy_zones_number<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	for(int i=0;i<this->bathy_zones_number;i++){
		this->bathymetrie[i].output_statistic();	
	}
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Convert a global angle (0° is north) to a local one; local 0° is the reference angle [static]
double Fpl_Wind_Fetch::global_angle_to_local(const double global_angle, const double reference_angle){
	double local_angle=0.0;
	if(reference_angle<=global_angle){
		local_angle=global_angle-reference_angle;
	}
	else{
		local_angle=(360.0-reference_angle)+global_angle;
	}
	return local_angle;
}
//_________________
//private
//Check the global fetches
void Fpl_Wind_Fetch::check_global_fetches(const double section_direction){
	//end of the water surface (bank)
	double anticlockwise_bank_global=_Geo_Geometrie::angle_to_360(section_direction-90.0);
	double clockwise_bank_global=_Geo_Geometrie::angle_to_360(section_direction+90.0);
	double check_angle=0.0;
	bool flag_found_one=false;
	//loop over the set global fetches
	for(int i=0; i< this->fetch_number; i++){
		check_angle=Fpl_Wind_Fetch::global_angle_to_local(_Geo_Geometrie::angle_to_360(this->fetches[i].angle_to_north), anticlockwise_bank_global);
		if(check_angle<=180.0 && check_angle>=0.0){//one fetch is found
			flag_found_one=true;
		}
		else{//global fetch is on the land side
			//reaction
			this->fetches[i].dry_flag=true;
			Warning msg=this->set_warning(0);
			ostringstream info;
			info << "Section direction (global) " << section_direction << endl;
			info << "Anticlockwise bank end (global) " << anticlockwise_bank_global << " Clockwise bank end (global) " << clockwise_bank_global << endl;
			info << "Fetchnr. " << i << " Global angle " << this->fetches[i].angle_to_north<<endl;; 
			msg.make_second_info(info.str());
			msg.output_msg(1);
		}
	}
	//no global fetch on the water side is found
	if(flag_found_one==false){
		Error msg=this->set_error(3);
		ostringstream info;
		info << "Section direction (global) " << section_direction << endl;
		info << "Anticlockwise bank end (global) " << anticlockwise_bank_global << " Clockwise bank end (global) " << clockwise_bank_global << endl;
		for(int i=0; i< this->fetch_number; i++){
			info << "Fetchnr. " << i << " Global angle " << this->fetches[i].angle_to_north <<endl; 
		}
		msg.make_second_info(info.str());
		throw msg;
	}
}
///Allocate the fetches
void Fpl_Wind_Fetch::allocate_fetches(void){
	try{
		this->fetches=new _fpl_wind_fetch[this->fetch_number];
		//count the memory
		Sys_Memory_Count::self()->add_mem(this->fetch_number*sizeof(_fpl_wind_fetch), _sys_system_modules::FPL_SYS);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
///Delete the fetches
void Fpl_Wind_Fetch::delete_fetches(void){
	if(this->fetches!=NULL){
		//count the memory
		Sys_Memory_Count::self()->minus_mem(this->fetch_number*sizeof(_fpl_wind_fetch), _sys_system_modules::FPL_SYS);
		delete []this->fetches;
		this->fetches=NULL;
	}
}
///Allocate the bathymetry
void Fpl_Wind_Fetch::allocate_bathymetry(void){
	try{
		//alloc the bathymetrie
		this->bathymetrie=new Fpl_Random_Variables[this->bathy_zones_number];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(5);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for(int i=0; i<this->bathy_zones_number; i++){
		this->bathymetrie[i].set_unit(label::m);
	}
}
///Delete the bathymetry
void Fpl_Wind_Fetch::delete_bathymetry(void){
	if(this->bathymetrie!=NULL){
		delete []this->bathymetrie;
		this->bathymetrie=NULL;
	}
}
//Find the key-values for the file input of the fetches and bathymetry
void Fpl_Wind_Fetch::find_key_values_file(const string myline){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(fpl_label::key_no_fetch);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_no_fetch.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->fetch_number;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	pos=myline.find(fpl_label::key_no_bathy);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_no_bathy.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->bathy_zones_number;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(11);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set error(s)
Error Fpl_Wind_Fetch::set_error(const int err_type){
		string place="Fpl_Wind_Fetch::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://table not found (fatal error)
			place.append("new_set_global_fetches(QSqlDatabase *ptr_database, const _section_id id, const double section_direction)");
			reason="Table not found";
			help="Check the table name";
			type=2;
			fatal=true;//fatal error;
			break;
		case 1://bad_alloc
			place.append("allocate_fetches(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://no results found (non fatal error)
			place.append("new_set_global_fetches(QSqlDatabase *ptr_database, const _section_id id, const double section_direction)");
			reason="No results found in table";
			help="Check the table and the fetches for this section";
			type=2;
			break;
		case 3://no fetches found in the watersurface
			place.append("check_global_fetches(const double section_direction)");
			reason="No global fetches are found in the given watersurface";
			help="Check the global fetches and the section direction";
			type=10;
			break;
		case 5://bad_alloc
			place.append("allocate_bathymetry(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 6://the fetches are not correctly ordered
			place.append("connect_fetches2bathy(void)");
			reason="The global fetch can not connected to the bathymetry";
			help="Check the global fetches and the random variables of the bathymetrie";
			type=11;
			break;
		case 8://do not find the begin of the fetch information in file
			place.append("new_set_global_fetches(QFile *ifile, int *line_counter, const double section_direction)");
			reason="Can not find the begin of the fetch information in file";
			help="The section information has to start with !$BEGIN_FETCHES";
			type=1;
			break;
		case 9://do not find the begin of the gbathymetry information in file
			place.append("new_set_bathymetry(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start)");
			reason="Can not find the begin of the bathymetrie information in file";
			help="The section information has to start with !$BEGIN_BATHY";
			type=1;
			break;
		case 10://do not find the begin of the general section information in file
			place.append("new_set_global_fetches(QFile *ifile, int *line_counter, const double section_direction)");
			reason="Can not find the number of fetches in file";
			help="The number of fetches has to defined before the fetch information with !no_fetch";
			type=1;
			break;
		case 11://wrong input
			place.append("find_key_values_file(const string myline)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the meachanism wind-wave generation in file";
			type=1;
			break;
		case 12://do not find the number of bathymetry zone
			place.append("new_set_bathymetry(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start)");
			reason="Can not find the number of bathymetrie zones in file";
			help="The number of bathymetrie zones has to defined before the bathymetrie information with !no_bathy";
			type=1;
			break;
		case 13://not all fetches found
			place.append("new_set_global_fetches(QFile *ifile, int *line_counter, const double section_direction)");
			reason="Not all required fetches are found";
			help="Check the fetches of the wind wave generation mechanism in file";
			type=1;
			break;
		case 14://not all bathymetrie zones found
			place.append("new_set_bathymetry(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start)");
			reason="Not all required bathymetrie zones are found";
			help="Check the bathymetrie zones of the wind wave generation mechanism in file";
			type=1;
			break;
		case 15://wrong input
			place.append("new_set_global_fetches(QFile *ifile, int *line_counter, const double section_direction)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the fetches of the wind wave generation mechanism in file";
			type=1;
			break;
		case 16://wrong input
			place.append("new_set_bathymetry(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the bathymetrie zones of the wind wave generation mechanism in file";
			type=1;
			break;
		default:
			place.append("check_global_fetches(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
//Set warning(s)
Warning Fpl_Wind_Fetch::set_warning(const int warn_type){
		string place="Fpl_Wind_Fetch::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no unique result found
			place.append("check_global_fetches(const double section_direction)") ;
			reason="Global fetch is on the land side (dry)";
			reaction="The global fetch is set to dry; it is not further used";
			help= "Check the global fetches and the section direction";
			type=9;	
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
