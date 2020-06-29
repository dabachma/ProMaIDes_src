//#include "Fpl_System.h"
#include "Fpl_Headers_Precompiled.h"

//constructor
Fpl_System::Fpl_System(void){

	Memory_Count::self()->add_mem(sizeof(Fpl_System), _system_modules::FPL_SYS);//count the memory
}
//destructor
Fpl_System::~Fpl_System(void){

	Memory_Count::self()->minus_mem(sizeof(Fpl_System), _system_modules::FPL_SYS);//count the memory
}
//_____________
//public
//Decide, which action of the fpl-system should be performed; it is just needed for console applications
void Fpl_System::fpl_action(void){
	int action_number=0;
	bool stop_flag=false;
	bool flag=false;
	bool database_flag=false;
	try{
		if(this->ptr_database!=NULL){
			if(this->ptr_database->check_con_status()==true){
				database_flag=true;
			}
		}
			
		if(database_flag==true){
			do{
				//output requriered mem
				Memory_Count::self()->output_mem();
				//read in which action
				do{
					cout << endl;
					cout << "=======================================================================" <<endl;
					cout << "AreaState " << this->system_id.area_state << " Measure " << this->system_id.area_state << endl;
					cout << "=======================================================================" <<endl;
					cout << "FPL-SYSTEM-MENU " << endl;
					cout << " Which Fpl-System action do you want to make ?" << endl;
					cout << "  (0) Go back to the MAIN-MENU " << endl;
					cout << "  (1) Inputdata " << endl;
					cout << "  (2) Calculation " << endl;
					cout << "  (3) Results " << endl;
					cout << "  (4) Common " << endl;
					if(!(flag=(cin  >> action_number))){//check if the input is valif
							cout << "Wrong input! Do it again" << endl;
							cin.clear();//clear the error flags
							cin.ignore();//clear cin
					}
					else if(action_number < 0 || action_number>4){
						cout << "Wrong number! Do it again! " << endl;
					}
				}
				while(!flag || action_number<0 || action_number>4);

				//make the action
				switch(action_number){
					case 0:
						stop_flag=true;
						break;
					case 1:
						this->make_input_data();
						break;
					case 2:
						this->make_calculation();
						break;
					case 3:
						this->make_result_data();
						break;
					case 4:
						this->action_common();
						break;
				}
			}
			while(!stop_flag);
		}
		else{
			do{
				
				//output requriered mem
				Memory_Count::self()->output_mem();
				//read in which action
				do{
					cout << endl;
					cout << "=======================================================================" <<endl;
					cout << "FPL-SYSTEM-MENU " << endl;
					cout << " Which Fpl-System action do you want to make ?" << endl;
					cout << " The database connection is not set; there are just 3 possible action" << endl;
					cout << "  (0) Go back to the MAIN-MENU " << endl;
					cout << "  (1) Test the random generator and the transformation " << endl;
					cout << "  (2) Set the output status " << endl;
					cout << "  (3) Set the logfile name " << endl;
					if(!(flag=(cin  >> action_number))){//check if the input is valif
							cout << "Wrong input! Do it again" << endl;
							cin.clear();//clear the error flags
							cin.ignore();//clear cin
					}
					else if(action_number < 0 || action_number>3){
						cout << "Wrong number! Do it again! " << endl;
					}
				}
				while(!flag || action_number<0 || action_number>3);

				//make the action
				switch(action_number){
					case 0:
						stop_flag=true;
						break;
					case 1:
						this->test_random();
						break;
					case 2:
						this->set_output_flag();
						break;
					case 3:
						this->set_logfile_name();
						break;
				}
			}
			while(!stop_flag);
		}
	}catch(Error msg){
		msg.output_msg(1);
	}
}
//Set and check all database tables of the fpl-system (static)
void Fpl_System::set_check_table(void){	
	ostringstream cout;
	cout << "Set and check all needed Fpl-tables ..." << endl;
	Common_Output::output_system->output_txt(cout.str(),false);
	cout.str("");
	try{
		Frc_Sim::set_output_table(this->ptr_database->get_database());
		Frc_Sim::output_table->output_tab_col();
		Mc_Sim::set_table(this->ptr_database->get_database());
		Mc_Sim::table->output_tab_col();
		Fpl_Section::set_table(this->ptr_database->get_database());
		Fpl_Section::table->output_tab_col();
		Random_Variables::set_table(this->ptr_database->get_database());
		Random_Variables::table->output_tab_col();
		Distribution_Class_Mean::set_default_table(this->ptr_database->get_database());
		Distribution_Class_Mean::default_table->output_tab_col();
		Distribution_Class_Mean::set_table(this->ptr_database->get_database());
		Distribution_Class_Mean::table->output_tab_col();
		Distribution_Class_Triangle::set_table(this->ptr_database->get_database());
		Distribution_Class_Triangle::table->output_tab_col();
		Distribution_Class_Triangle::set_default_table(this->ptr_database->get_database());
		Distribution_Class_Triangle::default_table->output_tab_col();
		Distribution_Class_Triangle::default_table->output_tab_col();
		Distribution_Class_Discret::set_table(this->ptr_database->get_database());
		Distribution_Class_Discret::table->output_tab_col();
		Distribution_Class_Discret::set_default_table(this->ptr_database->get_database());
		Distribution_Class_Discret::default_table->output_tab_col();
		Distribution_Class_Mean_Mean::set_table(this->ptr_database->get_database());
		Distribution_Class_Mean_Mean::table->output_tab_col();
		Distribution_Class_Mean_Mean::set_default_table(this->ptr_database->get_database());
		Distribution_Class_Mean_Mean::default_table->output_tab_col();
		Distribution_Class_Discret_Mean::set_table(this->ptr_database->get_database());
		Distribution_Class_Discret_Mean::table->output_tab_col();
		_Fpl_Section_Type::set_table_mechanism(this->ptr_database->get_database());
		_Fpl_Section_Type::table_mechanism->output_tab_col();
		_Geometrie::set_table_geometrie(this->ptr_database->get_database());
		_Geometrie::table_geometrie->output_tab_col();
	}
	catch(Error msg){
		throw msg;
	}
	cout <<"Set and check for all needed FPL-tables ok" << endl;
	Common_Output::output_system->output_txt(cout.str(),false);
}
//Close all database tables of the fpl-system (static)
void Fpl_System::close_tables(void){
	Frc_Sim::close_table();
	Mc_Sim::close_table();
	Fpl_Section::close_table();
	Random_Variables::close_table();
	_Fpl_Section_Type::close_table();
	_Geometrie::close_table();
}
//___________
//private
//show the input data
void Fpl_System::make_input_data(void){
	Fpl_Data data;
	data.set_database(this->ptr_database);
	data.set_systemid(this->system_id);
	data.action();
}
//show the calculation
void Fpl_System::make_calculation(void){
	Fpl_Calculation calculation;
	calculation.set_database(this->ptr_database);
	calculation.set_systemid(this->system_id);

	//calculation.action();
}
//show the results data
void Fpl_System::make_result_data(void){
	Fpl_Data_Results results;
	results.set_database(this->ptr_database);
	results.set_systemid(this->system_id);
	
	results.action();
}
//common action decision
void Fpl_System::action_common(void){
	int action_number=0;
	bool stop_flag=false;
	bool flag=false;
	do{
		//output requriered mem
		Memory_Count::self()->output_mem();
		//read in which action
		do{
			cout << endl;
			cout << "=======================================================================" <<endl;
			cout << "FPL-SYSTEM-MENU-COMMON " << endl;
			cout << " Which common Fpl-System action do you want to make ?" << endl;
			cout << "  (0) Go back to the FPL-SYSTEM-MENU " << endl;
			cout << "  (1) Set and check all tables " << endl;
			cout << "  (2) Set the output status " << endl;
			cout << "  (3) Set the logfile name " << endl;
			if(!(flag=(cin  >> action_number))){//check if the input is valif
					cout << "Wrong input! Do it again" << endl;
					cin.clear();//clear the error flags
					cin.ignore();//clear cin
			}
			else if(action_number < 0 || action_number>3){
				cout << "Wrong number! Do it again! " << endl;
			}
		}
		while(!flag || action_number<0 || action_number>3);

		//make the action
		switch(action_number){
			case 0:
				stop_flag=true;
				break;
			case 1:
				this->set_check_table();
				break;
			case 2:
				this->set_output_flag();
				 break;
			case 3:
				this->set_logfile_name();
				break;
		}
	}
	while(!stop_flag);

}
//make just a test of the mc_sim
void Fpl_System::test_random(void){
	Fpl_Calculation calculation;
	//calculation.mc_test_random();
}

//set the output flags
void Fpl_System::set_output_flag(void){
	//for fpl
	Common_Output::output_fpl->set_detailflag_console();
	
}
//set the logfile name
void Fpl_System::set_logfile_name(void){
	string buffer1;
	cout << "=======================================================================" <<endl;
	cout << "Give a new logfile name for the system output" << endl;
	cin >> buffer1;
	Common_Output::output_fpl->open_new_file(buffer1);
}