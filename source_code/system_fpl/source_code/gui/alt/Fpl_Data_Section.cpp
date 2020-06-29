//#include "Fpl_Data_Section.h"
#include "Fpl_Headers_Precompiled.h"

//constructor
Fpl_Data_Section::Fpl_Data_Section(void):nr_distclass_default(4),nr_distclass_table(5) {
	this->number_specif_sectiontype=NULL;
	this->brows_distclass=NULL;
	this->brows_distclass_default=NULL;
	this->brows_section=NULL;
	this->win_section=NULL;
	this->my_layout=NULL;
	Memory_Count::self()->add_mem(sizeof(Fpl_Data_Section));//count the memory
}
//destructor
Fpl_Data_Section::~Fpl_Data_Section(void){
	if(this->number_specif_sectiontype!=NULL){
		delete[] this->number_specif_sectiontype;
		this->number_specif_sectiontype=NULL;
	}
	if(this->my_layout!=NULL){
		delete this->my_layout;
		this->my_layout=NULL;
	}
	if(this->brows_distclass!=NULL){
		delete[] this->brows_distclass;
		this->brows_distclass=NULL;
	}
	if(this->brows_distclass_default!=NULL){
		delete[] this->brows_distclass_default;
		this->brows_distclass_default=NULL;
	}
	if(this->brows_section!=NULL){
		delete this->brows_section;
		this->brows_section=NULL;
	}
	if(this->win_section!=NULL){
		delete this->win_section;
		this->win_section=NULL;
	}

	Memory_Count::self()->minus_mem((sizeof(Fpl_Data_Section)));//count the memory
}


//_______________
//public
//decide per console which action is desired
void Fpl_Data_Section::action(void){
	int action_number=0;
	bool stop_flag=false;
	bool flag=false;

	try{
		do{
			//output requriered mem
			Memory_Count::self()->output_mem();
			//read in which action
			do{
				cout << endl;
				cout << "=======================================================================" <<endl;
				cout << "AreaState " << this->system_id.area_state << " Measure " << this->system_id.area_state << endl;
				cout << "=======================================================================" <<endl;
				cout << "FPL-DATA-SECTION-MENU " << endl;
				cout << "Which action do you want to make ?" << endl;
				cout << " (0) Go back to the FPL-DATA-MENU " << endl;
				cout << " (1) Show one(!) section " << endl;
				cout << " (2) Show the section table " << endl;
				cout << " (3) Show statistics of section " << endl;
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
					int sec_num;
					sec_num=this->ask_section("all");
					this->show_one_section(sec_num);
					 break;
				case 2:
					this->show_section_table();
					break;
				case 3:
					this->statistics_of_section();
					 break;
			}
		}
		while(!stop_flag);
	}catch(Error msg){
		throw msg;
	}
}
//show the section table from database
QTableView* Fpl_Data_Section::show_section_table(void){
	try{
		//define the table to show
		brows_section=new Database_Viewer(this->ptr_outputwin);
		//init the table
		Fpl_Section::set_table(this->ptr_database->get_database());
		//set the table name for the browser
		brows_section->set_tablename(Fpl_Section::table->table_name.name.c_str());
		//set the filter
		ostringstream filter;
		filter <<  Fpl_Section::table->get_column_name("AREA_STATE")<< " = " << this->system_id.area_state;
		filter << " and " << Fpl_Section::table->get_column_name("MEASURE")<< " = " << this->system_id.measure_nr;
		brows_section->set_where_filter(filter.str());
		//sort filter
		filter.str("");
		filter<< Fpl_Section::table->get_column_name("SECTION_NUMBER") ;
		brows_section->set_sort_filter(filter.str());

		brows_section->table_to_model(this->ptr_database->get_database());
		
		//execute the view
		if(this->ptr_outputwin==NULL){
			this->brows_section->model_to_view()->show();
			QApplication::exec();
			return 0;
		}
		else {
			return this->brows_section->model_to_view();
		}

		
	}
	catch(Error msg){
		throw msg;
	}
	
}
//show one section from database
QWidget* Fpl_Data_Section::show_one_section(const int actuel_secnum){
	try{
		//make the parentwindow
		this->win_section=new QWidget(this->ptr_outputwin);
		//make the layout
		this->my_layout=new QBoxLayout(QBoxLayout::TopToBottom, this->win_section);
		ostringstream title;
		title << "Data from Section Nr. " << actuel_secnum ;
		win_section->setWindowTitle(title.str().c_str());
		//define section
		brows_section=new Database_Viewer(this->win_section);
		//init the section table
		Fpl_Section::set_table(this->ptr_database->get_database());
		//set the table name for the browser
		brows_section->set_tablename(Fpl_Section::table->table_name.name);
		//set the filter
		ostringstream filter;
		filter << Fpl_Section::table->get_column_name("SECTION_NUMBER")<< " = " << actuel_secnum;
		filter << " and " << Fpl_Section::table->get_column_name("AREA_STATE")<< " = " << this->system_id.area_state;
		filter << " and " << Fpl_Section::table->get_column_name("MEASURE")<< " = " << this->system_id.measure_nr;
		brows_section->set_where_filter(filter.str());

		brows_section->table_to_model(this->ptr_database->get_database());
		//show the table
		this->my_layout->addWidget((brows_section->model_to_view()));

		
		//variables and their distribution classes
		this->brows_distclass=new Database_Viewer[this->nr_distclass_table];
		this->brows_distclass_default=new Database_Viewer[this->nr_distclass_default];
		//init the table of random variables
		Random_Variables::set_table(this->ptr_database->get_database());
		//for the given distribution class
		for(int i=0 ; i<this->nr_distclass_table; i++){
			this->set_distributionclass_view(i, actuel_secnum);
		}
		//for the default distribution class
		for(int i=0 ; i<this->nr_distclass_default; i++){
			this->set_distributionclass_default_view(i, actuel_secnum);
		}
		this->win_section->setLayout(this->my_layout);
		//this->win_section->show();
		//execute the view
		if(this->ptr_outputwin==NULL){
			this->win_section->show();
			QApplication::exec();
			return 0;
		}
		else {
			return this->win_section;
		}
	}
	catch(bad_alloc&){
		Error msg=this->set_error(1);
		throw msg;
	
	}
	catch(Error msg){
		throw msg;
	}
}
//statistics of the section data
void Fpl_Data_Section::statistics_of_section(void){
	try{
		ostringstream cout;
		cout <<"Statistics of the Fpl-Sections:" << endl;
		cout << " AreaState " << this->system_id.area_state << " Measure " << this->system_id.area_state << endl;
		cout << " There are " << this->calc_quant_spec_section("all") << " Sections total" << endl;
		cout << " There are " << this->calc_quant_spec_section("calc") << " Sections to calculate" << endl;
		cout << " There are " << this->calc_quant_spec_section("highground")<< " Sections with a Highground type" << endl;
		cout << " There are " << this->calc_quant_spec_section("set_by_hand") << " Sections, which are set by hand " << endl;
		cout << " There are " << this->calc_quant_spec_section("dike")<< " Sections with a dike type" << endl;
		Common_Output::output_fpl->output_txt(cout.str(),false);

	}
	catch(Error msg){
		throw msg;
	}
}
//ask which section
int Fpl_Data_Section::ask_section(const string section_specifier){
	int quantity=0;
	//how many section are there
	try{
		quantity=this->calc_quant_spec_section(section_specifier);
	}
	catch(Error msg){
		throw msg;
	}

	bool flag=false;
	bool sec_flag=false;
	//which section_number
	int sec_num=0;
	do{
		cout << endl;
		cout << "=======================================================================" <<endl;
		cout << "There are "<<  quantity << " Sections" <<  endl;
		for(int i=0; i<quantity; i++){
			cout << "Sectionnr. " << this->number_specif_sectiontype[i] << endl;	
		}
		cout << " Give the section number " << endl;
		cout << " (0) Go back " << endl;
		if(!(flag=(cin  >> sec_num))){//check if the input is valif
				cout << "Wrong input! Do it again" << endl;
				cin.clear();//clear the error flags
				cin.ignore();//clear cin
		}
		//test if the given section is available
		if(sec_num==0){
			sec_flag=true;
		}
		else{
			for(int i=0; i < quantity;i++){
				if(sec_num==this->number_specif_sectiontype[i]){
					sec_flag=true;
				}
			}
		}
		if(sec_flag==false && flag!=false){
			cout << "Sectionnumber is not available! Do it again! " << endl;
		}
	}
	while((!flag || !sec_flag));

	return sec_num;
}

//numbers of specific section type to calc
int Fpl_Data_Section::calc_quant_spec_section(const string section_specifier){
	int quantity=0;
	//mysql query with the table_model
	QSqlTableModel model(0,*this->ptr_database->get_database());
	//the table is set the name and the column names
	try{
		Fpl_Section::set_table(this->ptr_database->get_database());
	}
	catch(Error msg){
		throw msg;
	}
	//give the complet table of control parameters FPL
	model.setTable((Fpl_Section::table->table_name.name).c_str());
	//string for the query
	ostringstream filter;
	filter << Fpl_Section::table->get_column_name("AREA_STATE") << "=" << this->system_id.area_state;
	filter << " and " << Fpl_Section::table->get_column_name("MEASURE") << "=" <<  this->system_id.measure_nr;
	if(section_specifier=="all"){
		//no further filter
	}
	else if(section_specifier=="calc"){
		filter << " and " << Fpl_Section::table->get_column_name("SECTION_TYPE_NAME") <<"!= " << "'set_by_hand'";
	}
	else{
		filter << " and " << Fpl_Section::table->get_column_name("SECTION_TYPE_NAME") <<"= '" << section_specifier  << "'";
	}
	model.setFilter(filter.str().c_str());
    //set the query; select all in table
	Data_Base::database_request(&model);
	//check the query
	if(model.lastError().isValid()){//table is not found
		Error msg=set_error(0);
		stringstream info;
		info << "Table Name: " << Fpl_Section::table->table_name.name << endl;
		info << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	else{
		quantity=model.rowCount();
	}
	//read in the numbers of sections which are to calc
	try{
		this->number_specif_sectiontype= new int[quantity];
	}
	catch(bad_alloc&){
		Error msg=set_error(1);
		throw msg;
	}
	for(int i=0; i< quantity; i++){
		this->number_specif_sectiontype[i]=model.record(i).value((Fpl_Section::table->get_column_name("SECTION_NUMBER")).c_str()).toInt();
	}
	return quantity;
}

//______________________________
//private
//the tables from the distribution_class are set and show
void Fpl_Data_Section::set_distributionclass_view(const int number, const int actuel_secnum){
		//the different filter
		ostringstream filter1;
		ostringstream filter2;
		ostringstream filter3;
		ostringstream filter4;
		//set the table for the browser
		this->brows_distclass[number].set_tablename(Random_Variables::table->table_name.name);
		//this filters are always the same
		//set up column filter
		filter1 << Random_Variables::table->get_column_name("NAME")<< " , ";
		filter1 << Random_Variables::table->get_column_name("DEFAULT_NAME")<< " , ";
		filter1 << Random_Variables::table->get_column_name("DISTRIBUTION_CLASS_NAME")<< " , ";
		filter1 << Random_Variables::table->get_column_name("CALCULATED_FLAG") <<",";
		//set the where filter
		filter2 << Random_Variables::table->get_column_name("SECTION_NUMBER")<< " = " << actuel_secnum;
		filter2 << " and " << Random_Variables::table->get_column_name("AREA_STATE")<< " = " << this->system_id.area_state;
		filter2 << " and " << Random_Variables::table->get_column_name("MEASURE")<< " = " << this->system_id.measure_nr;
		filter2 << " and " << Random_Variables::table->get_column_name("DEFAULT_NAME") << " = " <<"'given'";
		//set the order filter
		filter3 << Random_Variables::table->get_column_name("NAME");
	
		//set join filter
		filter4 << Random_Variables::table->table_name.name<<"."<<Random_Variables::table->get_column_name("DISTRIBUTION_NUMBER");
		//this is per distribution class different
		switch (number){
			case 0:
				//table init
				Distribution_Class_Mean::set_table(this->ptr_database->get_database());
				//complete filter 
				filter1 << Distribution_Class_Mean::table->table_name.name<<".*";
				filter2 << " and " << Random_Variables::table->get_column_name("DISTRIBUTION_CLASS_NAME") << " = " <<"'mean'";
				filter4 << " = " <<Distribution_Class_Mean::table->table_name.name<<"." <<Distribution_Class_Mean::table->get_column_name("DISTRIBUTION_NUMBER");
				this->brows_distclass[number].set_join_filter(filter4.str(),Distribution_Class_Mean::table->table_name.name) ;
				break;
			case 1:
				//table init
				Distribution_Class_Triangle::set_table(this->ptr_database->get_database());
				//complete filter 
				filter1 << Distribution_Class_Triangle::table->table_name.name<<".*";
				filter2 << " and " << Random_Variables::table->get_column_name("DISTRIBUTION_CLASS_NAME") << " = " <<"'triangle'";
				filter4 << " = " <<Distribution_Class_Triangle::table->table_name.name<<"." <<Distribution_Class_Triangle::table->get_column_name("DISTRIBUTION_NUMBER");
				this->brows_distclass[number].set_join_filter(filter4.str(),Distribution_Class_Triangle::table->table_name.name) ;
				break;
			case 2:
				//table init
				Distribution_Class_Discret::set_table(this->ptr_database->get_database());
				//complete filter 
				filter1 << Distribution_Class_Discret::table->table_name.name<<".*";
				filter2 << " and " << Random_Variables::table->get_column_name("DISTRIBUTION_CLASS_NAME") << " = " <<"'discret'";
				filter4 << " = " <<Distribution_Class_Discret::table->table_name.name<<"." <<Distribution_Class_Discret::table->get_column_name("DISTRIBUTION_NUMBER");
				this->brows_distclass[number].set_join_filter(filter4.str(),Distribution_Class_Discret::table->table_name.name) ;
				break;
			case 3:
				//table init
				Distribution_Class_Mean_Mean::set_table(this->ptr_database->get_database());
				//complete filter 
				filter1 << Distribution_Class_Mean_Mean::table->table_name.name<<".*";
				filter2 << " and " << Random_Variables::table->get_column_name("DISTRIBUTION_CLASS_NAME") << " = " <<"'mean_mean'";
				filter4 << " = " <<Distribution_Class_Mean_Mean::table->table_name.name<<"." <<Distribution_Class_Mean_Mean::table->get_column_name("DISTRIBUTION_NUMBER");
				this->brows_distclass[number].set_join_filter(filter4.str(),Distribution_Class_Mean_Mean::table->table_name.name);
				break;
			case 4:
				//table init
				Distribution_Class_Discret_Mean::set_table(this->ptr_database->get_database());
				//complete filter 
				filter1 << Distribution_Class_Discret_Mean::table->table_name.name<<".*";
				filter2 << " and " << Random_Variables::table->get_column_name("DISTRIBUTION_CLASS_NAME") << " = " <<"'discret_mean'";
				filter4 << " = " <<Distribution_Class_Discret_Mean::table->table_name.name<<"." <<Distribution_Class_Discret_Mean::table->get_column_name("DISTRIBUTION_NUMBER");
				this->brows_distclass[number].set_join_filter(filter4.str(),Distribution_Class_Discret_Mean::table->table_name.name);
				break;
		}
		//set the filters to the browser
		this->brows_distclass[number].set_column_filter(filter1.str());
		this->brows_distclass[number].set_where_filter(filter2.str());
		this->brows_distclass[number].set_sort_filter(filter3.str());
		//show just if there is an result
		if(this->brows_distclass[number].table_to_model(this->ptr_database->get_database())->rowCount()!=0){
			this->my_layout->addWidget((this->brows_distclass[number].model_to_view()));
		}

}
//the tables from the distribution_class (default values) are set and show
void Fpl_Data_Section::set_distributionclass_default_view(const int number, const int actuel_secnum){
	//the different filter
		ostringstream filter1;
		ostringstream filter2;
		ostringstream filter3;
		ostringstream filter4;
		ostringstream filter4_0;
		ostringstream filter4_1;
		//set the table for the browser
		this->brows_distclass_default[number].set_tablename(Random_Variables::table->table_name.name);
		//this filters are always the same
		//set up column filter
		/*filter1 << Random_Variables::table->table_name.name<<"."<<Random_Variables::table->get_column_name("NAME")<< " , ";
		filter1 << Random_Variables::table->get_column_name("DEFAULT_NAME")<< " , ";*/
		filter1 << Random_Variables::table->get_column_name("DISTRIBUTION_CLASS_NAME")<< " , ";
		filter1 << Random_Variables::table->get_column_name("CALCULATED_FLAG") <<",";
		//set the where filter
		filter2 << Random_Variables::table->get_column_name("SECTION_NUMBER")<< " = " << actuel_secnum;
		filter2 << " and " << Random_Variables::table->get_column_name("AREA_STATE")<< " = " << this->system_id.area_state;
		filter2 << " and " << Random_Variables::table->get_column_name("MEASURE")<< " = " << this->system_id.measure_nr;
		filter2 << " and " << Random_Variables::table->get_column_name("DEFAULT_NAME") << " != " <<"'given'";
		//set the order filter
		filter3 << Random_Variables::table->table_name.name<<"."<<Random_Variables::table->get_column_name("NAME");
	
		//set join filter
		filter4_0 << Random_Variables::table->table_name.name<<"."<<Random_Variables::table->get_column_name("NAME");
		//set second join filter
		filter4_1 << " and " << Random_Variables::table->table_name.name<<"."<<Random_Variables::table->get_column_name("DEFAULT_NAME");
		//this is per distribution class different
		switch (number){
			case 0:
				//table init
				Distribution_Class_Mean::set_default_table(this->ptr_database->get_database());
				//complete filter 
				filter1 << Distribution_Class_Mean::default_table->table_name.name<<".*";
				filter2 << " and " << Random_Variables::table->get_column_name("DISTRIBUTION_CLASS_NAME") << " = " <<"'mean'";
				filter4_0 << " = " <<Distribution_Class_Mean::default_table->table_name.name<<"." <<Distribution_Class_Mean::default_table->get_column_name("NAME");
				filter4_1 << " = " <<Distribution_Class_Mean::default_table->table_name.name<<"." <<Distribution_Class_Mean::default_table->get_column_name("DEFAULT_NAME");
				filter4<< filter4_0.str() << filter4_1.str();
				this->brows_distclass_default[number].set_join_filter(filter4.str(),Distribution_Class_Mean::default_table->table_name.name) ;
				break;
			case 1:
				//table init
				Distribution_Class_Triangle::set_default_table(this->ptr_database->get_database());
				//complete filter 
				filter1 << Distribution_Class_Triangle::default_table->table_name.name<<".*";
				filter2 << " and " << Random_Variables::table->get_column_name("DISTRIBUTION_CLASS_NAME") << " = " <<"'triangle'";
				filter4_0 << " = " <<Distribution_Class_Triangle::default_table->table_name.name<<"." <<Distribution_Class_Triangle::default_table->get_column_name("NAME");
				filter4_1 << " = " <<Distribution_Class_Triangle::default_table->table_name.name<<"." <<Distribution_Class_Triangle::default_table->get_column_name("DEFAULT_NAME");
				filter4<< filter4_0.str() << filter4_1.str();
				this->brows_distclass_default[number].set_join_filter(filter4.str(),Distribution_Class_Triangle::default_table->table_name.name) ;
				break;
			case 2:
				//table init
				Distribution_Class_Discret::set_default_table(this->ptr_database->get_database());
				//complete filter 
				filter1 << Distribution_Class_Discret::default_table->table_name.name<<".*";
				filter2 << " and " << Random_Variables::table->get_column_name("DISTRIBUTION_CLASS_NAME") << " = " <<"'discret'";
				filter4_0 << " = " <<Distribution_Class_Discret::default_table->table_name.name<<"." <<Distribution_Class_Discret::default_table->get_column_name("NAME");
				filter4_1 << " = " <<Distribution_Class_Discret::default_table->table_name.name<<"." <<Distribution_Class_Discret::default_table->get_column_name("DEFAULT_NAME");
				filter4<< filter4_0.str() << filter4_1.str();
				this->brows_distclass_default[number].set_join_filter(filter4.str(),Distribution_Class_Discret::default_table->table_name.name) ;
				break;
			case 3:
				//table init
				Distribution_Class_Mean_Mean::set_default_table(this->ptr_database->get_database());
				//complete filter 
				filter1 << Distribution_Class_Mean_Mean::default_table->table_name.name<<".*";
				filter2 << " and " << Random_Variables::table->get_column_name("DISTRIBUTION_CLASS_NAME") << " = " <<"'mean_mean'";
				filter4_0 << " = " <<Distribution_Class_Mean_Mean::default_table->table_name.name<<"." <<Distribution_Class_Mean_Mean::default_table->get_column_name("NAME");
				filter4_1 << " = " <<Distribution_Class_Mean_Mean::default_table->table_name.name<<"." <<Distribution_Class_Mean_Mean::default_table->get_column_name("DEFAULT_NAME");
				filter4<< filter4_0.str() << filter4_1.str();
				this->brows_distclass_default[number].set_join_filter(filter4.str(),Distribution_Class_Mean_Mean::default_table->table_name.name);
				break;
			//case 4:
			//	//table init
			//	Distribution_Class_Discret_Mean::set_default_table(this->ptr_database->get_database());
			//	//complete filter 
			//	filter1 << Distribution_Class_Discret_Mean::default_table->table_name.name<<".*";
			//	filter2 << " and " << Random_Variables::table->get_column_name("DISTRIBUTION_CLASS_NAME") << " = " <<"'discret_mean'";
			//	filter4_0 << " = " <<Distribution_Class_Discret_Mean::default_table->table_name.name<<"." <<Distribution_Class_Discret_Mean::default_table->get_column_name("NAME");
			//	filter4_1 << " = " <<Distribution_Class_Discret_Mean::default_table->table_name.name<<"." <<Distribution_Class_Discret_Mean::default_table->get_column_name("DEFAULT_NAME");
			//	filter4<< filter4_0.str() << filter4_1.str();
			//	this->brows_distclass_default[number].set_join_filter(filter4.str(),Distribution_Class_Discret_Mean::default_table->table_name.name);
			//	break;
		}
		//set the filters to the browser

		this->brows_distclass_default[number].set_column_filter(filter1.str());
		this->brows_distclass_default[number].set_where_filter(filter2.str());
		this->brows_distclass_default[number].set_sort_filter(filter3.str());
		//show just if there is an result
		if(this->brows_distclass_default[number].table_to_model(this->ptr_database->get_database())->rowCount()!=0){
			this->my_layout->addWidget((this->brows_distclass_default[number].model_to_view()));
		}
}

//set the error
Error Fpl_Data_Section::set_error(const int err_type){
		string place="Fpl_Data_Section::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://table not found (fatal error)
			place.append("calc_quant_section_....(void)");
			reason="Table not found";
			help="Check the table name";
			type=2;
			fatal=true;//fatal error;
			break;
		case 1://bad alloc
			place.append("calc_quant_...section(void)/void show_one_section(void)");
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