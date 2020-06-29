//#include "Sys_Memory_Count.h"
#include "Sys_Headers_Precompiled.h"

//init the static members
long long int Sys_Memory_Count::mem_count=0;
long long int Sys_Memory_Count::mem_count_sys=0;
long long int Sys_Memory_Count::mem_count_hyd=0;
long long int Sys_Memory_Count::mem_count_fpl=0;
long long int Sys_Memory_Count::mem_count_madm=0;
long long int Sys_Memory_Count::mem_count_dam=0;
long long int Sys_Memory_Count::mem_count_risk=0;
long long int Sys_Memory_Count::mem_count_alt=0;
long long int Sys_Memory_Count::mem_count_cost=0;
long long int Sys_Memory_Count::mem_count_geosys=0;

Sys_Memory_Count *Sys_Memory_Count::ptr_class=NULL;


//destructor
Sys_Memory_Count::~Sys_Memory_Count(void){
	if (ptr_class!=NULL){
		delete ptr_class;
		ptr_class=NULL;
	}
}
//_______________________
//public
//(static) Main method of the singleton pattern; it instantiate the class and returns a static pointer to the single object
Sys_Memory_Count* Sys_Memory_Count::self(void){
if (ptr_class==NULL){
		ptr_class = new Sys_Memory_Count();
	}
	return ptr_class;
}
//Add the memory to the total memory and to the given memory counter of the moduls
void Sys_Memory_Count::add_mem(const long long int mem){

	this->my_locker.lock();
	mem_count=mem_count+mem;

	this->delta_mem=this->delta_mem+mem;

	if(this->delta_mem>constant::kbyte_size*300){
		this->memory_txt=functions::convert_byte2string(mem_count).c_str();
		//dividing of the memory
		emit send_memorytxt(this->memory_txt, this->generate_tooltip().c_str());
		this->delta_mem=0;
	}
	this->my_locker.unlock();
}
//Add the memory to the total memory and to the given memory counter of the moduls
void Sys_Memory_Count::add_mem(const long long int mem, const _sys_system_modules modul){
	

	switch(modul){
		case _sys_system_modules::SYS_SYS:
			mem_count_sys=mem_count_sys+mem;
			break;
		case _sys_system_modules::FPL_SYS:
			mem_count_fpl=mem_count_fpl+mem;
			break;
		case _sys_system_modules::HYD_SYS:
			mem_count_hyd=mem_count_hyd+mem;
			break;
		case _sys_system_modules::MADM_SYS:
			mem_count_madm=mem_count_madm+mem;
			break;
		case _sys_system_modules::DAM_SYS:
			mem_count_dam=mem_count_dam+mem;
			break;
		case _sys_system_modules::RISK_SYS:
			mem_count_risk=mem_count_risk+mem;
			break;
		case _sys_system_modules::COST_SYS:
			mem_count_cost=mem_count_cost+mem;
			break;
		case _sys_system_modules::ALT_SYS:
			mem_count_alt=mem_count_alt+mem;
			break;
		case _sys_system_modules::GEOSYS_SYS:
			Sys_Memory_Count::mem_count_geosys=Sys_Memory_Count::mem_count_geosys+mem;
			break;
		default:
			break;
	}
	Sys_Memory_Count::add_mem(mem);

}
//Subtract the memory of the total memory and of the given memory counter of the moduls
void Sys_Memory_Count::minus_mem(const long long int mem){

	this->my_locker.lock();
	mem_count=mem_count-mem;
	
	//cout << "  -   " << mem<< endl;
	
	this->delta_mem=this->delta_mem+mem;
	if(abs(this->delta_mem)>constant::kbyte_size*300){
		this->memory_txt=functions::convert_byte2string(mem_count).c_str();
		//dividing of the memory
		emit send_memorytxt(this->memory_txt, this->generate_tooltip().c_str());
		this->delta_mem=0;
	}
	this->my_locker.unlock();
}
//Subtract the memory of the total memory and of the given memory counter of the moduls
void Sys_Memory_Count::minus_mem(const long long int mem, const _sys_system_modules modul){
	

	switch(modul){
		case _sys_system_modules::SYS_SYS:
			mem_count_sys=mem_count_sys-mem;
			break;
		case _sys_system_modules::FPL_SYS:
			mem_count_fpl=mem_count_fpl-mem;
			break;
		case _sys_system_modules::HYD_SYS:
			mem_count_hyd=mem_count_hyd-mem;
			break;
		case _sys_system_modules::MADM_SYS:
			mem_count_madm=mem_count_madm-mem;
			break;
		case _sys_system_modules::DAM_SYS:
			mem_count_dam=mem_count_dam-mem;
			break;
		case _sys_system_modules::RISK_SYS:
			mem_count_risk=mem_count_risk-mem;
			break;
		case _sys_system_modules::COST_SYS:
			mem_count_cost=mem_count_cost-mem;
			break;
		case _sys_system_modules::ALT_SYS:
			mem_count_alt=mem_count_alt-mem;
			break;
		case _sys_system_modules::GEOSYS_SYS:
			Sys_Memory_Count::mem_count_geosys=Sys_Memory_Count::mem_count_geosys-mem;
			break;
		default:
			break;

	}
	Sys_Memory_Count::minus_mem(mem);
}
//Set a pointer to the output widget
void Sys_Memory_Count::set_output_widget(QWidget *ptr_output){
	this->ptr_output=ptr_output;
	this->memory_txt=functions::convert_byte2string(mem_count).c_str();
	//dividing of the memory
	emit send_memorytxt(this->memory_txt, this->generate_tooltip().c_str());
}
//Output the momory to the console; just used in case of an application without a gui
void Sys_Memory_Count::output_mem(void){
	if(this->ptr_output==NULL){
		cout<<"--------"<< endl;
		this->memory_txt=functions::convert_byte2string(mem_count).c_str();
		cout<<"Actual memory requeries: "<<  memory_txt.toStdString() << endl;
		cout<<"________________________"<<endl;
		cout<<"SYS memory requeries    : "<<  functions::convert_byte2string(mem_count_sys).c_str() <<endl;
		cout<<"GEOSYS memory requeries : "<<  functions::convert_byte2string(mem_count_geosys).c_str() <<endl;
		cout<<"FPL memory requeries    : "<<  functions::convert_byte2string(mem_count_fpl).c_str() <<endl;
		cout<<"HYD memory requeries    : "<<  functions::convert_byte2string(mem_count_hyd).c_str() <<endl;
		cout<<"DAM memory requeries    : "<<  functions::convert_byte2string(mem_count_dam).c_str() <<endl;
		cout<<"RISK memory requeries   : "<<  functions::convert_byte2string(mem_count_risk).c_str() <<endl;
		cout<<"ALT memory requeries    : "<<  functions::convert_byte2string(mem_count_alt).c_str() <<endl;
		cout<<"COST memory requeries   : "<<  functions::convert_byte2string(mem_count_cost).c_str() <<endl;
		cout<<"MADM memory requeries   : "<<  functions::convert_byte2string(mem_count_madm).c_str() <<endl;
	}
}
//Make a widget for warning
void Sys_Memory_Count::make_warning_end_widget(void){
	if(Sys_Memory_Count::mem_count_alt!=0 || Sys_Memory_Count::mem_count_cost!=0||
		Sys_Memory_Count::mem_count_dam!=0 || Sys_Memory_Count::mem_count_fpl!=0 ||
		Sys_Memory_Count::mem_count_hyd!=0 || Sys_Memory_Count::mem_count_madm!=0 ||
		Sys_Memory_Count::mem_count_risk!=0 || Sys_Memory_Count::mem_count_sys!=0 || Sys_Memory_Count::mem_count_sys!=0){
		
		ostringstream cout;
		cout<<"SYS memory requeries    : "<<  functions::convert_byte2string(mem_count_sys).c_str() <<endl;
		cout<<"GEOSYS memory requeries : "<<  functions::convert_byte2string(mem_count_geosys).c_str() <<endl;
		cout<<"FPL memory requeries    : "<<  functions::convert_byte2string(mem_count_fpl).c_str() <<endl;
		cout<<"HYD memory requeries    : "<<  functions::convert_byte2string(mem_count_hyd).c_str() <<endl;
		cout<<"DAM memory requeries    : "<<  functions::convert_byte2string(mem_count_dam).c_str() <<endl;
		cout<<"RISK memory requeries   : "<<  functions::convert_byte2string(mem_count_risk).c_str() <<endl;
		cout<<"ALT memory requeries    : "<<  functions::convert_byte2string(mem_count_alt).c_str() <<endl;
		cout<<"COST memory requeries   : "<<  functions::convert_byte2string(mem_count_cost).c_str() <<endl;
		cout<<"MADM memory requeries   : "<<  functions::convert_byte2string(mem_count_madm).c_str() <<endl;

		
		QMessageBox::warning(NULL, "Memory counter warning", cout.str().c_str());

	}
}
//______________________
//private
//Private constructor for the singleton pattern
Sys_Memory_Count::Sys_Memory_Count(void){
	this->ptr_output=NULL;
	this->delta_mem=0;
}
//Generate a string for the tooltip
string Sys_Memory_Count::generate_tooltip(void){
	string buffer;

	ostringstream buff;
	if(mem_count_sys!=0){
		buff << sys_label::str_sys << "  " <<functions::convert_byte2string(mem_count_sys).c_str();
	}
	if(mem_count_geosys!=0){
		buff << sys_label::str_geosys << "  " <<functions::convert_byte2string(mem_count_geosys).c_str();
	}
	if(mem_count_fpl!=0){
		buff << sys_label::str_fpl << "  " <<functions::convert_byte2string(mem_count_fpl).c_str();
	}
	if(mem_count_hyd!=0){
		buff << sys_label::str_hyd << "  " <<functions::convert_byte2string(mem_count_hyd).c_str();
	}
	if(mem_count_dam!=0){
		buff << sys_label::str_dam << "  " <<functions::convert_byte2string(mem_count_dam).c_str();
	}
	if(mem_count_risk!=0){
		buff << sys_label::str_risk<< " " <<functions::convert_byte2string(mem_count_risk).c_str();
	}
	if(mem_count_alt!=0){
		buff << sys_label::str_alt << "  " <<functions::convert_byte2string(mem_count_alt).c_str();
	}
	if(mem_count_cost!=0){
		buff << sys_label::str_cost << " " <<functions::convert_byte2string(mem_count_cost).c_str();
	}
	if(mem_count_madm!=0){
		buff << sys_label::str_madm << " " <<functions::convert_byte2string(mem_count_madm).c_str();
	}


	buffer=buff.str();
	int pos=0;
	pos=buffer.rfind("\n");

	if(pos>0){
		buffer=buffer.erase(pos);
	}


	return buffer;
}
