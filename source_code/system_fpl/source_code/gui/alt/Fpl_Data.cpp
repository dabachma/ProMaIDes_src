//#include "Fpl_Data.h"
#include "Fpl_Headers_Precompiled.h"


//constructor
Fpl_Data::Fpl_Data(void){
	Memory_Count::self()->add_mem(sizeof(Fpl_Data));//count the memory
	this->my_browser=NULL;
}

//destructor
Fpl_Data::~Fpl_Data(void){
	if(this->my_browser!=NULL){
		delete this->my_browser;
		this->my_browser=NULL;
	}	
	Memory_Count::self()->minus_mem((sizeof(Fpl_Data)));//count the memory
}
//______________________
//public
//decide per console which action is desired
void Fpl_Data::action(void){
	
	int action_number=0;
	bool stop_flag=false;
	bool flag=false;

	try{
		do{
			Fpl_Data_Section my_data_section;
			//output requriered mem
			Memory_Count::self()->output_mem();
			//read in which action
			do{
				cout << endl;
				cout << "=======================================================================" <<endl;
				cout << "AreaState " << this->system_id.area_state << " Measure " << this->system_id.area_state << endl;
				cout << "=======================================================================" <<endl;
				cout << "FPL-DATA-MENU " << endl;
				cout << "Which action do you want to make ?" << endl;
				cout << " (0) Go back to the FPL-SYTEM-MENU " << endl;
				cout << " (1) Section Data " << endl;
				cout << " (2) Performance parameter " << endl;
				if(!(flag=(cin  >> action_number))){//check if the input is valif
					cout << "Wrong input! Do it again" << endl;
					cin.clear();//clear the error flags
					cin.ignore();//clear cin
				}
				else if(action_number < 0 || action_number>2){
					cout << "Wrong number! Do it again! " << endl;
				}
			}
			while(!flag || action_number<0 || action_number>2);

			//make the action
			switch(action_number){
				case 0:
					stop_flag=true;
					 break;
				case 1:
					my_data_section.set_database(this->ptr_database);
					my_data_section.set_systemid(this->system_id);
					my_data_section.action();
					 break;
				case 2:
					this->show_performance_table();
					break;
			}
		}
		while(!stop_flag);
	}catch(Error msg){
		throw msg;
	}
}
//show the performance table
QTableView* Fpl_Data::show_performance_table(void){
	try{
		this->my_browser = new Database_Viewer(this->ptr_outputwin);
		//set the table
		Mc_Sim::set_table(this->ptr_database->get_database());
		//set table name
		this->my_browser->set_tablename(Mc_Sim::get_mc_table_name());
		this->my_browser->table_to_model(this->ptr_database->get_database());
		
		if(this->ptr_outputwin==NULL){
			//make the view
			this->my_browser->model_to_view()->showMaximized();
			QApplication::exec();
			return 0;
		}
		else{
			return this->my_browser->model_to_view();
		}
	}
	catch(bad_alloc&){
		Error msg=set_error(1);
		throw msg;
		
	}
	catch(Error msg){
		throw msg;
	}
}
//________________
//private
//build the error msg
Error Fpl_Data::set_error(const int err_type){
		string place="Fpl_Data::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 1://bad alloc
			place.append("show...");
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
	msg.set_msg(place,reason,help,type,fatal);
	msg.make_second_info(info.str());

return msg;
}