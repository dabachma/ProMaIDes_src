//#include "Fpl_Local_Wind_Fetch.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
Fpl_Local_Wind_Fetch::Fpl_Local_Wind_Fetch(void):number_of_fetches(12){
	this->wind_direction=0.0;
	this->anticlockwise_fetchend_global=0.0;
	this->clockwise_fetchend_global=0.0;
	//this->number_of_fetches=12;
	this->anticlockwise_bank_local=0.0;
	this->clockwise_bank_local=0.0;
	this->fetch_param=NULL;
	this->number_rel_fetches=0;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Local_Wind_Fetch), _sys_system_modules::FPL_SYS);//count the memory
}
//Copy constructor
Fpl_Local_Wind_Fetch::Fpl_Local_Wind_Fetch(const Fpl_Local_Wind_Fetch& object):number_of_fetches(12){
	this->anticlockwise_fetchend_global=object.anticlockwise_fetchend_global;
	this->clockwise_fetchend_global=object.clockwise_fetchend_global;
	this->wind_direction=object.wind_direction;
	this->anticlockwise_bank_local=object.anticlockwise_bank_local;
	this->clockwise_bank_local=object.clockwise_bank_local;
	this->number_rel_fetches=object.number_rel_fetches;
	//allocate the fetch_param
	if(object.fetch_param!=NULL){
		this->fetch_param=new _fpl_local_wind_fetch_param[this->number_of_fetches];
		Sys_Memory_Count::self()->add_mem(sizeof(_fpl_local_wind_fetch_param)*this->number_of_fetches, _sys_system_modules::FPL_SYS);
		for(int i=0; i< this->number_of_fetches; i++){
			this->fetch_param[i].a_angle=object.fetch_param[i].a_angle;
			this->fetch_param[i].index_variable=object.fetch_param[i].index_variable;
			this->fetch_param[i].ortho_length=object.fetch_param[i].ortho_length;
			this->fetch_param[i].dry_flag=object.fetch_param[i].dry_flag;
		}
	}
	else{//or set it to NULL
		this->fetch_param=object.fetch_param; 
	}
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Local_Wind_Fetch), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Local_Wind_Fetch::~Fpl_Local_Wind_Fetch(void){
	if(this->fetch_param!=NULL){
		delete []this->fetch_param;
		this->fetch_param=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_local_wind_fetch_param)*this->number_of_fetches, _sys_system_modules::FPL_SYS);//count the memory
	}
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Local_Wind_Fetch), _sys_system_modules::FPL_SYS);//count the memory
}
//__________
//public
//Get the wind direction as identifier
double Fpl_Local_Wind_Fetch::give_wind_direction(void){
	return this->wind_direction;
}
//Get the number of fetches in the set
int Fpl_Local_Wind_Fetch::give_number_fetches(void){
	return this->number_of_fetches;
}
//Get the number of relevant fetches in the set
int Fpl_Local_Wind_Fetch::get_number_relevant_fetches(void){
	return this->number_rel_fetches;
}
//Set a new local fetch set
void Fpl_Local_Wind_Fetch::set_fetches(const double wind_direction, const double section_direction, Fpl_Wind_Fetch *global_fetches){
	this->wind_direction=wind_direction;
	//end of the considered total fetch area
	this->anticlockwise_fetchend_global=_Geo_Geometrie::angle_to_360(this->wind_direction-90.0);
	this->clockwise_fetchend_global=_Geo_Geometrie::angle_to_360(this->wind_direction+90.0);
	//end of the water surface (bank)
	this->anticlockwise_bank_local=Fpl_Wind_Fetch::global_angle_to_local(_Geo_Geometrie::angle_to_360(section_direction-90.0),this->anticlockwise_fetchend_global);
	this->clockwise_bank_local=Fpl_Wind_Fetch::global_angle_to_local(_Geo_Geometrie::angle_to_360(section_direction+90.0),this->anticlockwise_fetchend_global);
	
	//set the fetch param
	try{
		this->set_fetch_param(global_fetches);
	}
	catch(Error msg){
		throw msg;
	}
	
}
//Output the members to display/console
void Fpl_Local_Wind_Fetch::output_members(void){
	ostringstream cout;
	cout << "LOCAL FETCH SET " << this->wind_direction << label::degree<< endl;
	cout << " Global a.c.w. angle of the fetchend : " << this->anticlockwise_fetchend_global << label::degree<< endl;
	cout << " Global c.w. angle of the fetchend   : " << this->clockwise_fetchend_global << label::degree<< endl;
	cout << " Local a.c.w. angle of the bank      : " << this->anticlockwise_bank_local << label::degree<< endl;
	cout << " Local c.w. angle of the bank        : " << this->clockwise_bank_local << label::degree<< endl;
	cout << " Number of local fetch sectors       : " << this->number_of_fetches << endl;
	cout << " Number of relevant fetch sectors    : " << this->number_rel_fetches << endl;
	//output for development
	//cout << " Fetches: "<< endl;
	//cout << " No. " << W(10)<< "spectral_factor"<< W(15) << "index_global_fetch" <<W(15) <<"orthogonal_lenght"<< label::m<<endl;
	//
	//for(int i=0; i< this->number_of_fetches;i++){
	//	cout << i <<W(10)<< this->fetch_param[i].a_angle <<W(15);
	//	if(this->fetch_param[i].dry_flag==true){
	//		cout<< "dry" ;
	//	}
	//	else{
	//		cout << this->fetch_param[i].index_variable ;
	//	}
	//	cout << W(15) << this->fetch_param[i].ortho_length <<  endl;
	//}
	Sys_Common_Output::output_fpl->output_txt(&cout,true);

}
//Copy operator (required for the dynamic re-allocation of the local fetches)
Fpl_Local_Wind_Fetch& Fpl_Local_Wind_Fetch::operator=(const Fpl_Local_Wind_Fetch& object){
	this->anticlockwise_fetchend_global=object.anticlockwise_fetchend_global;
	this->clockwise_fetchend_global=object.clockwise_fetchend_global;
	this->wind_direction=object.wind_direction;
	this->anticlockwise_bank_local=object.anticlockwise_bank_local;
	this->clockwise_bank_local=object.clockwise_bank_local;
	this->number_rel_fetches=object.number_rel_fetches;
	//allocate the fetch_param
	if(object.fetch_param!=NULL){
		this->fetch_param=new _fpl_local_wind_fetch_param[this->number_of_fetches];
		Sys_Memory_Count::self()->add_mem(sizeof(_fpl_local_wind_fetch_param)*this->number_of_fetches, _sys_system_modules::FPL_SYS);
		for(int i=0; i< this->number_of_fetches; i++){
			this->fetch_param[i].a_angle=object.fetch_param[i].a_angle;
			this->fetch_param[i].index_variable=object.fetch_param[i].index_variable;
			this->fetch_param[i].ortho_length=object.fetch_param[i].ortho_length;
			this->fetch_param[i].dry_flag=object.fetch_param[i].dry_flag;
		}
	}
	else{//or set it to NULL
		this->fetch_param=object.fetch_param; 
	}
	return *this;
}
//___________
//private
//Set the fetches of one set from the given global (user-defined) fecthes
void Fpl_Local_Wind_Fetch::set_fetch_param(Fpl_Wind_Fetch *global_fetches){
	//allocate the fetch_param structure
	try{
		this->fetch_param=new _fpl_local_wind_fetch_param[this->number_of_fetches];
	}
	catch(bad_alloc&){
		Error msg=this->set_error(0);
		throw msg;
	}
	Sys_Memory_Count::self()->add_mem(sizeof(_fpl_local_wind_fetch_param)*this->number_of_fetches, _sys_system_modules::FPL_SYS);//count the memory

	//init this structure
	for(int i=0; i< this->number_of_fetches;i++){
		this->fetch_param[i].a_angle=0.0;
		this->fetch_param[i].index_variable=-1;
		this->fetch_param[i].ortho_length=0.0;
		this->fetch_param[i].dry_flag=true;
	}

	//step of angle
	double step_angle=180.0/this->number_of_fetches;
	double a_begin=0.0;
	double a_end=0.0;
	double angle_begin=0.0;
	double angle_end=0.0;
	double angle_mid=0.0;

	double min_angle=361.0;//not higher is possible than 360 °
	bool found_flag=false;
	double local_angle=0.0;

	//loop over the fetches with the same angle (180/number_of_fetches);
	for(int i=0; i< this->number_of_fetches;i++){
		//calculate a_angle
		angle_begin=double(i)*step_angle;
		angle_end=double(i+1)*step_angle;
		angle_mid=(angle_begin+angle_end)/2.0;

		a_begin=(angle_begin/180.0)-sin(2.0*_Geo_Geometrie::grad_to_rad(angle_begin))/(2.0*constant::Cpi);
		a_end=(angle_end/180.0)-sin(2.0*_Geo_Geometrie::grad_to_rad(angle_end))/(2.0*constant::Cpi);
		//clac a_param
		this->fetch_param[i].a_angle=a_end-a_begin;

		//test if it is dry
		if (this->anticlockwise_bank_local <= 180.0 && angle_mid < this->anticlockwise_bank_local){
			this->fetch_param[i].ortho_length=0.0; //dry
			this->fetch_param[i].index_variable=-1;
			this->fetch_param[i].dry_flag=true;
		}
		else if(this->clockwise_bank_local <= 180.0 && angle_mid >this->clockwise_bank_local){
			this->fetch_param[i].ortho_length=0.0;//dry
			this->fetch_param[i].index_variable=-1;
			this->fetch_param[i].dry_flag=true;
		}
		else{//isn't dry; search for the given global fetches
			min_angle=361.0;//not higher is possible than 360 °
			found_flag=false;
			
			//loop over global fetches 
			for(int n=0; n<global_fetches->get_fetch_number(); n++){
				local_angle=0.0;
				//calc the local angle of the global fetches
				local_angle=Fpl_Wind_Fetch::global_angle_to_local(_Geo_Geometrie::angle_to_360(global_fetches->get_angle(n)),this->anticlockwise_fetchend_global);
				//find the nearest global fetch sector for this local fetch
				if(local_angle-angle_mid<min_angle && local_angle-angle_mid>=0.0){
					min_angle=local_angle-angle_mid;
					this->fetch_param[i].ortho_length=global_fetches->get_length(n)*sin(_Geo_Geometrie::grad_to_rad(angle_mid));
					this->fetch_param[i].index_variable=n;
					this->fetch_param[i].dry_flag=false;
					this->number_rel_fetches++;
					found_flag=true;
				}
				
			}//end loop global fetches
			if(found_flag==false){//no global fetches is found, min_angle is not changed..local fetch zone it is set to zero
				this->fetch_param[i].ortho_length=0.0;//dry
				this->fetch_param[i].index_variable=-1;
				this->fetch_param[i].dry_flag=true;
				//set a warning
				Warning msg=this->set_warning(0);
				ostringstream info;
				info<<" Wind direction (global)         : " << this->wind_direction << label::degree<< endl;
				info<<" Local end-angle of local fetch  : " << angle_end << label::degree << endl;
				msg.make_second_info(info.str());
				msg.output_msg(1);
			}
		}
		//development
		//cout << "Wind " << this->wind_direction << " angle_begin " << angle_begin << " angle_end " << angle_end << " zone index " << this->fetch_param[i].index_variable << " ortho lenght " <<  this->fetch_param[i].ortho_length << endl;

	}//end loop local fetches
}
//Set the error(s)
Error Fpl_Local_Wind_Fetch::set_error(const int err_type){
		string place="Fpl_Local_Wind_Fetch::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("set_fetch_param(Fpl_Wind_Fetch *global_fetches)");
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
Warning Fpl_Local_Wind_Fetch::set_warning(const int warn_type){
		string place="Fpl_Local_Wind_Fetch::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no unique result found
			place.append("set_fetch_param(Fpl_Wind_Fetch *global_fetches)") ;
			reason="No last global fetch in clockwise direction is found for the local fetch";
			reaction="The length is set to 0";
			help= "Check global fetches and the direction of the section";
			type=9;	
			msg.set_during_calculation_flag(false);//just output to the exception output
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
