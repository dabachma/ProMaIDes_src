#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Param_Material.h"

//init static members
Tables *Hyd_Param_Material::mat_param_table=NULL;

//constructor
Hyd_Param_Material::Hyd_Param_Material(void):default_manning(1.0/35.0), default_poleni(0.65){
	this->filename=label::not_set;
	this->material_params=NULL;
	this->no_values=0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Param_Material), _sys_system_modules::HYD_SYS);
}
//Copy constructor
Hyd_Param_Material::Hyd_Param_Material(Hyd_Param_Material &par):default_manning(1.0/35.0), default_poleni(0.65){
	this->filename=par.filename;
	this->no_values=par.no_values;

	this->alloc_mat_array();
	for(int i=0; i< this->no_values; i++){
		this->material_params[i]=par.material_params[i];
	}

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Param_Material), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Param_Material::~Hyd_Param_Material(void){
	if(this->material_params!=NULL){
		delete [] this->material_params;
		this->material_params=NULL;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(_hyd_mat_params)*this->no_values, _sys_system_modules::HYD_SYS);

	}
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Param_Material), _sys_system_modules::HYD_SYS);
}
//__________________________________
//public
//Set the filename
void Hyd_Param_Material::set_filename(const string filename){
	this->filename=filename;
}
//Get the material value corresponding to material type number
double Hyd_Param_Material::get_mat_value(const int mat_number, _hyd_mat_coefficient_types type){
	double mat_value=0.0;
	if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
		//no hydraulik sim is used in this project type; just a dummy raster is created
		return this->default_manning;
	}
	bool found_flag=false;
	for(int i=0; i<this->no_values; i++){
		if(this->material_params[i].type_number==mat_number){
			if(this->material_params[i].type==type){
				mat_value=this->material_params[i].value;
				found_flag=true;
				break;
			}
			else{
				if(type==_hyd_mat_coefficient_types::manning_coefficient){
					Warning msg;
					msg=this->set_warning(0);
					stringstream info;
					info << "Material type of material number "<< mat_number <<" : " << this->convert_coefficienttype2txt(this->material_params[i].type) << endl;
					info <<"Material type asked for : " << this->convert_coefficienttype2txt(type) << endl;
					mat_value=this->default_manning;
					info << "Default value          : "<< mat_value <<" "<<label::n_unit <<endl;
					msg.make_second_info(info.str());
					msg.output_msg(2);
					found_flag=true;
					break;
				}else if(type==_hyd_mat_coefficient_types::poleni_coefficient){
					Warning msg;
					msg=this->set_warning(0);
					stringstream info;
					info << "Material type of material number "<< mat_number <<" in file: " << this->convert_coefficienttype2txt(this->material_params[i].type) << endl;
					info <<"Material type asked for : " << this->convert_coefficienttype2txt(type) << endl;
					mat_value=this->default_poleni;
					info << "Default value          : "<< mat_value <<" "<<label::no_unit <<endl;
					msg.make_second_info(info.str());
					msg.output_msg(2);
					found_flag=true;
					break;
				}
			}
		}
	}
	if(found_flag==false){
		if(type==_hyd_mat_coefficient_types::manning_coefficient){
			Warning msg;
			msg=this->set_warning(1);
			stringstream info;
			info << "Material value of material number "<< mat_number <<" not found " << endl;
			mat_value=this->default_manning;
			info << "Default value : "<< mat_value <<" "<<label::n_unit <<endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}else if(type==_hyd_mat_coefficient_types::poleni_coefficient){
			Warning msg;
			msg=this->set_warning(1);
			stringstream info;
			info << "Material value of material number "<< mat_number <<" not found " << endl;
			mat_value=this->default_poleni;
			info << "Default value : "<< mat_value <<" "<<label::no_unit <<endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}

	if(mat_value<=0.0){
		if(type==_hyd_mat_coefficient_types::manning_coefficient){
			Warning msg;
			msg=this->set_warning(2);
			stringstream info;
			info << "Material value of material number "<< mat_number <<" is "<< mat_value << endl;
			mat_value=this->default_manning;
			info << "Default value : "<< mat_value <<" "<<label::n_unit <<endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}else if(type==_hyd_mat_coefficient_types::poleni_coefficient){
			Warning msg;
			msg=this->set_warning(2);
			stringstream info;
			info << "Material value of material number "<< mat_number <<" is " << mat_value << endl;
			mat_value=this->default_poleni;
			info << "Default value : "<< mat_value <<" "<<label::no_unit <<endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	
	}
	return mat_value;
}
//Read the material values from file
void Hyd_Param_Material::read_values_per_file(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "MAT> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	//output
	ostringstream cout;
	cout << "Read the material parameters from file" << this->filename << " ..."<< endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	ifstream ifile;
	//open file
	ifile.open(this->filename.c_str(), ios_base::in);
	//throw error
	if(ifile.is_open()==false){
		Error msg=this->set_error(1);
		throw msg;
	}
	//variables
	string myline;
	stringstream my_stream;
	/*int found_index=-1;*/
	string type_buff;
	bool found_flag=false;
	do{
		getline(ifile, myline,'\n');
		if(myline.empty()!=true){	
			functions::clean_string(&myline);
			if(myline.empty()!=true){
				my_stream << myline;
				my_stream >> this->no_values;
				if(my_stream.fail()==true){
					ostringstream info;
					info << "Wrong input sequenze " << my_stream.str() << endl;
					Error msg=this->set_error(2);
					msg.make_second_info(info.str());
					ifile.close();
					throw msg;
				}
				else{
					my_stream.clear();
					my_stream.str("");
					found_flag=true;
				}
			}
		}
		
	}while( found_flag!=true && ifile.eof()!=true);
	//first alloc the structures
	try{
		this->alloc_mat_array();
	}
	catch(Error msg){
		throw msg;
	}


	//now read in the values
	int found_counter=0;
	do{
		getline(ifile, myline,'\n');
		if(myline.empty()!=true){	
			functions::clean_string(&myline);

			if(myline.empty()!=true){
				my_stream << myline;
				my_stream >> this->material_params[found_counter].type_number >> this->material_params[found_counter].value >> type_buff;
				if(my_stream.fail()==true){
					ostringstream info;
					info << "Wrong input sequenze " << my_stream.str() << endl;
					Error msg=this->set_error(4);
					msg.make_second_info(info.str());
					ifile.close();
					throw msg;
				}
				else{
					try{ 
						this->material_params[found_counter].type=this->convert_txt2coefficienttype(type_buff);
					}
					catch(Error msg){
						ifile.close();
						throw msg;
					}
					my_stream.clear();
					my_stream.str("");
					found_counter++;
				}
			}
		}
	}
	while(found_counter < this->no_values && ifile.eof()!=true);

	if(found_counter < this->no_values){
		Error msg=this->set_error(6);
		ostringstream info;
		info << "Number of types found " << found_counter << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Create the database table for the material data of the hydraulic system (static)
void Hyd_Param_Material::create_table(QSqlDatabase *ptr_database){
	if(Hyd_Param_Material::mat_param_table==NULL){
			ostringstream cout;
			cout << "Create hydraulic material data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_mat_param;
			const int num_col=4;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=hyd_label::matparam_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=hyd_label::matparam_val;
			tab_col[1].type=sys_label::tab_col_type_double;
			tab_col[1].unsigned_flag=true;
			tab_col[1].default_value="0.0";

			tab_col[2].name=hyd_label::matparam_typ;
			tab_col[2].type=sys_label::tab_col_type_string;

			tab_col[3].name=label::description;
			tab_col[3].type=sys_label::tab_col_type_string;

			try{
				Hyd_Param_Material::mat_param_table= new Tables();
				if(Hyd_Param_Material::mat_param_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){;
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Param_Material::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Hyd_Param_Material::close_table();;
				throw msg;
			}

		Hyd_Param_Material::close_table();
	}

}
//Set the database table for the material parameters: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Param_Material::set_table(QSqlDatabase *ptr_database){
	if(Hyd_Param_Material::mat_param_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_mat_param;
		string tab_id_col[3];

		tab_id_col[0]=hyd_label::matparam_id;
		tab_id_col[1]=hyd_label::matparam_val;
		tab_id_col[2]=hyd_label::matparam_typ;

		try{
			Hyd_Param_Material::mat_param_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Param_Material::mat_param_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Param_Material::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Param_Material::close_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the material parameters (static)
void Hyd_Param_Material::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Param_Material::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Hyd_Param_Material::mat_param_table->delete_data_in_table(ptr_database);

}
//Close and delete the database table for the material parameters (static)
void Hyd_Param_Material::close_table(void){
	if(Hyd_Param_Material::mat_param_table!=NULL){
		delete Hyd_Param_Material::mat_param_table;
		Hyd_Param_Material::mat_param_table=NULL;
	}
}
//Transfer the material parameters of a file into a database table
void Hyd_Param_Material::transfer_materialparams_file2database_table(QSqlDatabase *ptr_database, const string mat_file){

	//output
	ostringstream cout;
	cout << "Transfer the material parameters from a file to a database table..."<< endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	try{
		//read in the globals per file
		this->set_filename(mat_file);
		this->read_values_per_file();

		//set it tot the database table
		this->material_params2database(ptr_database);
	}
	catch(Error msg){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
		
	}
}
//Transfer just one dummy material parameter to database 
void Hyd_Param_Material::transfer_dummy_params2database_table(QSqlDatabase *ptr_database){
	try{
		this->no_values=1;
		this->alloc_mat_array();
		this->material_params[0].type_number=1;
		this->material_params[0].value=0.0001;
		this->material_params2database(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the material parameters per database
void Hyd_Param_Material::matparams_per_database(QSqlDatabase *ptr_database, const bool with_output){
	//set prefix for output
	ostringstream prefix;
	prefix << "MAT> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	//output
	if(with_output==true){
		ostringstream cout;
		cout << "Read the material parameters from database ..."<< endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Param_Material::set_table(ptr_database);
	}
	catch(Error msg){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}

	///give the table name to the query model
	model.setTable((Hyd_Param_Material::mat_param_table->get_table_name()).c_str());

    //set the query
	Data_Base::database_request(&model);
	//check the query
	if(model.lastError().isValid()){
		//error
		Error msg=set_error(7);
		ostringstream info;
		info << "Table Name      : " << Hyd_Param_Material::mat_param_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	if(model.rowCount()==0){
		Error msg=set_error(8);
		ostringstream info;
		info << "Table Name      : " << Hyd_Param_Material::mat_param_table->get_table_name() << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	else{
		//number of values
		this->no_values=model.rowCount();
		//alloc the structures
		try{
			this->alloc_mat_array();
		}
		catch(Error msg){
			throw msg;
		}

	}
	//read out the results
	for(int i=0; i<this->no_values; i++){
		this->material_params[i].value=model.record(i).value((Hyd_Param_Material::mat_param_table->get_column_name(hyd_label::matparam_val)).c_str()).toDouble();
		this->material_params[i].type_number=model.record(i).value((Hyd_Param_Material::mat_param_table->get_column_name(hyd_label::matparam_id)).c_str()).toDouble();	
		this->material_params[i].type=this->convert_txt2coefficienttype((model.record(i).value((Hyd_Param_Material::mat_param_table->get_column_name(hyd_label::matparam_typ)).c_str()).toString().toStdString()));
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//output the members
void Hyd_Param_Material::output_members(void){

	ostringstream cout;

	ostringstream prefix;
	prefix << "MAT> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());


	cout << "Number of material types " << this->no_values << endl;
	cout << "   No. " << "  Type "  << "  Value " << endl;
	for(int i =0; i< this->no_values; i++){
		cout << W(5) << this->material_params[i].type_number << W(8) ;
		if(this->material_params[i].type==_hyd_mat_coefficient_types::poleni_coefficient){
			cout << hyd_label::POL << W(10) << P(3)<< FORMAT_FIXED_REAL << this->material_params[i].value << label::no_unit << endl;
		
		}
		else if(this->material_params[i].type==_hyd_mat_coefficient_types::manning_coefficient){
			cout << hyd_label::MAN << W(10) << P(3)<< FORMAT_FIXED_REAL <<this->material_params[i].value << label::n_unit << endl;
		}

	}
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Total reset of the global parameters
void Hyd_Param_Material::total_reset(void){
	if(this->material_params!=NULL){
		delete [] this->material_params;
		this->material_params=NULL;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(_hyd_mat_params)*this->no_values, _sys_system_modules::HYD_SYS);
	}
	this->filename=label::not_set;
	this->no_values=0;
}
//copy operator
Hyd_Param_Material& Hyd_Param_Material::operator= (const Hyd_Param_Material& par){
	this->filename=par.filename;
	this->no_values=par.no_values;

	this->alloc_mat_array();
	for(int i=0; i< this->no_values; i++){
		this->material_params[i]=par.material_params[i];
	}
	return *this;
}
//________________________________
//private
//Allocate _hyd_mat_params structures for num_vales:
void Hyd_Param_Material::alloc_mat_array(void){
	try{
		this->material_params = new _hyd_mat_params[this->no_values];
	}
	catch(bad_alloc&){
		Error msg;
		msg=this->set_error(3);
		throw msg;
	}
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(_hyd_mat_params)*this->no_values, _sys_system_modules::HYD_SYS);
	//init the array
	for(int i=0; i< this->no_values; i++){
		this->material_params[i].type_number=-1;
		this->material_params[i].value=0.0;
		this->material_params[i].type=_hyd_mat_coefficient_types::manning_coefficient;
	}
}
//Convert text to enum mat_coefficient_types
_hyd_mat_coefficient_types Hyd_Param_Material::convert_txt2coefficienttype(const string txt){

	if(txt==hyd_label::POL){
		return _hyd_mat_coefficient_types::poleni_coefficient;
	}
	else if(txt==hyd_label::MAN){
		return _hyd_mat_coefficient_types::manning_coefficient;
	}
	else{
		Error msg=this->set_error(5);
		ostringstream info;
		info << "Given type text " << txt << endl;
		msg.make_second_info(info.str());
		throw msg;
		
	}

}
//Convert text to enum mat_coefficient_types
string Hyd_Param_Material::convert_coefficienttype2txt(_hyd_mat_coefficient_types type){
	string txt;

	switch(type){
		case _hyd_mat_coefficient_types::manning_coefficient :
			txt=hyd_label::MAN;
			break;
		case _hyd_mat_coefficient_types::poleni_coefficient :
			txt=hyd_label::POL;
			break;
		default:
			txt=label::unknown_type;
			break;
	}

	return txt;
}
//Transfer the material parameters to a database table
void Hyd_Param_Material::material_params2database(QSqlDatabase *ptr_database){
	//set prefix for output
	ostringstream prefix;
	prefix << "MAT> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	//output
	ostringstream cout;
	cout << "Transfer the material parameters to the database table..."<< endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Param_Material::set_table(ptr_database);
	}
	catch(Error msg){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}

	///give the table name to the query model
	model.setTable((Hyd_Param_Material::mat_param_table->get_table_name()).c_str());


	//insert data
	for(int i=0; i<this->no_values; i++){
		//insert row
		model.insertRows(0, 1);
		//set data
		model.setData(model.index(0,model.record().indexOf((Hyd_Param_Material::mat_param_table->get_column_name(hyd_label::matparam_id)).c_str())),this->material_params[i].type_number);
		model.setData(model.index(0,model.record().indexOf((Hyd_Param_Material::mat_param_table->get_column_name(hyd_label::matparam_val)).c_str())),this->material_params[i].value);
		model.setData(model.index(0,model.record().indexOf((Hyd_Param_Material::mat_param_table->get_column_name(hyd_label::matparam_typ)).c_str())),this->convert_coefficienttype2txt(this->material_params[i].type).c_str());
	
		//submit it to the datbase
		Data_Base::database_submit(&model);
		if(model.lastError().isValid()){
			Warning msg=set_warning(3);
			ostringstream info;
			info << "Table Name                : " << Hyd_Param_Material::mat_param_table->get_table_name() << endl;
			info << "Table error info          : " << model.lastError().text().toStdString() << endl;
			info << "Material parameter ID     : " << this->material_params[i].type_number << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);	
		}
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();


}
//Set the warning
Warning Hyd_Param_Material::set_warning(const int warn_type){
	string place="Hyd_Param_Material::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://Type of material value does not match
			place.append("get_mat_value(const int mat_number, mat_coefficient_types type)") ;
			reason="Type of material value does not match";
			reaction="Default value is used";
			help= "Check the material type or number";
			type=3;	
			break;
		case 1://material value not found
			place.append("get_mat_value(const int mat_number, mat_coefficient_types type)") ;
			reason="Material value not found";
			reaction="Default value is used";
			help= "Check the material type number or the file";
			type=3;	
			break;
		case 2://material value is <=0.0
			place.append("get_mat_value(const int mat_number, mat_coefficient_types type)") ;
			reason="Material value is <=0.0; it is not possible";
			reaction="Default value is used";
			help= "Check the material type number or the file";
			type=3;	
			break;
		case 3://input datas can not submitted
			place.append("material_params2database(QSqlDatabase *ptr_database)");
			reason="Can not submit one of the read in material input data to the database";
			help="Check the database";
			type=2;
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
//Set the error
Error Hyd_Param_Material::set_error(const int err_type){
	string place="Hyd_Param_Material::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://Unknown Mattype
			place.append("get_mat_value(const int mat_number, mat_coefficient_types type)");
			reason="Unknown material type";
			help="Check the material file";
			type=3;
			fatal=false;
			break;
		case 1://File nor open
			place.append("read_values_per_file(void)");
			reason="Can not open file";
			help="Check the material file";
			info << "Filename " << this->filename << endl;
			type=5;
			fatal=false;
			break;
		case 2://wrong input
			place.append("read_values_per_file(void)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the number of material types in file";
			info << "Filename " << this->filename << endl;
			type=1;
			break;
		case 3://bad alloc
			place.append("alloc_mat_array(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 4://wrong input
			place.append("read_values_per_file(void)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the values in file";
			info << "Filename " << this->filename << endl;
			type=1;
			break;
		case 5://wrong input
			place.append("convert_txt2coefficienttype(const sting txt)");
			reason="Unknown coefficient type text";
			help="Check the coeffiecient type in file";
			info << "Filename " << this->filename << endl;
			type=1;
			break;
		case 6://not all mattypes found
			place.append("read_values_per_file(void)");
			reason="Not all material types are found in file";
			help="Check the number of material types in file";
			info << "Filename " << this->filename << endl;
			info << "Number of types to be found " << this->no_values << endl;
			type=1;
			break;
		case 7://problems with database table
			place.append("matparams_per_database(QSqlDatabase *ptr_database)/input_globals2database_table(QSqlDatabase *ptr_database)") ;
			reason="Table for the material parameters is not found";
			help="Check the table name";
			type=2;	
			break;
		case 8://no matching parameter sets are found in the table
			place.append("matparams_per_database(QSqlDatabase *ptr_database)") ;
			reason="There are no records found for the material parameters";
			help="Check the material parameters in the database";
			type=2;	
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
