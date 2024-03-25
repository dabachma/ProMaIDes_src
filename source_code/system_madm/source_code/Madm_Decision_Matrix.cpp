#include "Madm_Headers_Precompiled.h"
//#include "Madm_Decision_Matrix.h"


//init static members
Tables *Madm_Decision_Matrix::table_matrix=NULL;

//Default constructor
Madm_Decision_Matrix::Madm_Decision_Matrix(void){

	this->number_crit=0;
	this->crit=NULL;
	this->number_alt=0;
	this->alt=NULL;
	this->matrix=NULL;
	this->norm_type=_madm_normalisation_type::norm_no;

	this->name=label::not_set;


	Sys_Memory_Count::self()->add_mem(sizeof(Madm_Decision_Matrix),_sys_system_modules::MADM_SYS);//count the memory
}
//Copy constructor
Madm_Decision_Matrix::Madm_Decision_Matrix(Madm_Decision_Matrix &copy){

	this->number_crit=copy.number_crit;
	this->number_alt=copy.number_alt;
	this->norm_type=copy.norm_type;
	this->name=copy.name;
    this->crit=NULL;
    this->alt=NULL;
    this->matrix=NULL;

	this->init_matrix(this->number_alt,this->number_crit);

	//copy alternatives
	for(int i=0; i< this->number_alt; i++){
		this->alt[i]=copy.alt[i];

	}
	//copy criteria
	for(int i=0; i< this->number_crit; i++){
		this->crit[i]=copy.crit[i];

	}
	//copy matrix
	for(int i=0; i< this->number_alt; i++){
		for(int j=0; j<this->number_crit; j++){
			this->matrix[i][j]=copy.matrix[i][j];
		}
	}
	Sys_Memory_Count::self()->add_mem(sizeof(Madm_Decision_Matrix),_sys_system_modules::MADM_SYS);//count the memory
}
//Default destructor
Madm_Decision_Matrix::~Madm_Decision_Matrix(void){

	this->delete_criteria();
	this->delete_alternatives();
	this->delete_matrix();

	Sys_Memory_Count::self()->minus_mem(sizeof(Madm_Decision_Matrix),_sys_system_modules::MADM_SYS);//count the memory
}
//_________
//public
//Set the database table for the decision matrix: it sets the table name and the name of the columns and allocate them (static)
void Madm_Decision_Matrix::set_table_matrix(QSqlDatabase *ptr_database, const bool not_close){
	if(Madm_Decision_Matrix::table_matrix==NULL){
		//make specific input for this class
		const string tab_id_name=madm_label::tab_matrix;
		string tab_col[23];
		tab_col[0]=label::glob_id;
		tab_col[1]=label::areastate_id;
		tab_col[2]=label::measure_id;
		tab_col[3]=risk_label::risk_type;
		tab_col[4]=madm_label::crit_risk_ecn;
		tab_col[5]=madm_label::crit_risk_eco;
		tab_col[6]=madm_label::crit_risk_pop_aff;
		tab_col[7]=madm_label::crit_risk_pys;
		tab_col[8]=madm_label::crit_risk_outflow;
		tab_col[9]=madm_label::crit_cost_ecn;
		tab_col[10]=madm_label::crit_risk_pop_dan;
		tab_col[11]=madm_label::crit_risk_sc_pub;
		tab_col[12]=madm_label::crit_risk_sc_eco;
		tab_col[13]=madm_label::crit_risk_sc_cult;
		tab_col[14]=madm_label::crit_risk_sc_person;
		tab_col[15] = madm_label::crit_risk_ci_elect_pt;
		tab_col[16] = madm_label::crit_risk_ci_info_tec_pt;
		tab_col[17] = madm_label::crit_risk_ci_water_sup_pt;
		tab_col[18] = madm_label::crit_risk_ci_water_treat_pt;
		tab_col[19] = madm_label::crit_risk_ci_energy_pt;
		tab_col[20] = madm_label::crit_risk_ci_health_pt;
		tab_col[21] = madm_label::crit_risk_ci_social_pt;
		tab_col[22] = madm_label::crit_risk_ci_mat_pt;


		
		//set the table
		try{
			Madm_Decision_Matrix::table_matrix= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Madm_Decision_Matrix::table_matrix->set_name(ptr_database, _sys_table_type::madm);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Madm_Decision_Matrix::set_table_matrix(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			if (not_close == false) {
				Madm_Decision_Matrix::close_table_matrix();
			}
			throw msg;
		}
	}
}
//Create the database table for the decision matrix (static)
void Madm_Decision_Matrix::create_table_matrix(QSqlDatabase *ptr_database){
	if(Madm_Decision_Matrix::table_matrix==NULL){
		ostringstream cout;
		cout << "Create database table of the decision matrix..." << endl ;
		Sys_Common_Output::output_madm->output_txt(&cout);
		//make specific input for this class
		const string tab_name=madm_label::tab_matrix;
		const int num_col=23;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}

		tab_col[0].name=label::glob_id;
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

		tab_col[3].name=risk_label::risk_type;
		tab_col[3].type=sys_label::tab_col_type_string;

		tab_col[4].name=madm_label::crit_risk_ecn;
		tab_col[4].type=sys_label::tab_col_type_double;

		tab_col[5].name=madm_label::crit_risk_eco;
		tab_col[5].type=sys_label::tab_col_type_double;

		tab_col[6].name=madm_label::crit_risk_pop_aff;
		tab_col[6].type=sys_label::tab_col_type_double;

		tab_col[7].name=madm_label::crit_risk_pop_dan;
		tab_col[7].type=sys_label::tab_col_type_double;

		tab_col[8].name=madm_label::crit_risk_pys;
		tab_col[8].type=sys_label::tab_col_type_double;

		tab_col[9].name=madm_label::crit_risk_sc_pub;
		tab_col[9].type=sys_label::tab_col_type_double;

		tab_col[10].name=madm_label::crit_risk_sc_eco;
		tab_col[10].type=sys_label::tab_col_type_double;

		tab_col[11].name=madm_label::crit_risk_sc_cult;
		tab_col[11].type=sys_label::tab_col_type_double;

		tab_col[12].name=madm_label::crit_risk_sc_person;
		tab_col[12].type=sys_label::tab_col_type_double;

		
		tab_col[13].name = madm_label::crit_risk_ci_elect_pt;
		tab_col[13].type = sys_label::tab_col_type_double;

		tab_col[14].name = madm_label::crit_risk_ci_info_tec_pt;
		tab_col[14].type = sys_label::tab_col_type_double;

		tab_col[15].name = madm_label::crit_risk_ci_water_sup_pt;
		tab_col[15].type = sys_label::tab_col_type_double;


		tab_col[16].name = madm_label::crit_risk_ci_water_treat_pt;
		tab_col[16].type = sys_label::tab_col_type_double;

		tab_col[17].name = madm_label::crit_risk_ci_energy_pt;
		tab_col[17].type = sys_label::tab_col_type_double;

		tab_col[18].name = madm_label::crit_risk_ci_health_pt;
		tab_col[18].type = sys_label::tab_col_type_double;

		tab_col[19].name = madm_label::crit_risk_ci_social_pt;
		tab_col[19].type = sys_label::tab_col_type_double;

		tab_col[20].name = madm_label::crit_risk_ci_mat_pt;
		tab_col[20].type = sys_label::tab_col_type_double;

		tab_col[21].name=madm_label::crit_risk_outflow;
		tab_col[21].type=sys_label::tab_col_type_double;

		tab_col[22].name=madm_label::crit_cost_ecn;
		tab_col[22].type=sys_label::tab_col_type_double;
		tab_col[22].unsigned_flag=true;

		try{
			Madm_Decision_Matrix::table_matrix= new Tables();
			if(Madm_Decision_Matrix::table_matrix->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::madm)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_madm->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Madm_Decision_Matrix::create_table_matrix(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Madm_Decision_Matrix::close_table_matrix();
			throw msg;
		}
		Madm_Decision_Matrix::close_table_matrix();
	}
}
//Delete all data in the database table for the decision matrix (static)
void Madm_Decision_Matrix::delete_data_in_table_matrix(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Madm_Decision_Matrix::set_table_matrix(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Madm_Decision_Matrix::table_matrix->delete_data_in_table(ptr_database);
}
//Delete data in the database table for the decision matrix for the given systemstate (static)
void Madm_Decision_Matrix::delete_data_in_table_matrix(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Madm_Decision_Matrix::set_table_matrix(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "DELETE FROM ";
	test_filter << Madm_Decision_Matrix::table_matrix->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Madm_Decision_Matrix::table_matrix->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND " ;
	test_filter << Madm_Decision_Matrix::table_matrix->get_column_name(label::measure_id) << " = "  << id.measure_nr;

	Data_Base::database_request(&model, test_filter.str(), ptr_database);
}
//Close and delete the database table for the decision matrix (static)
void Madm_Decision_Matrix::close_table_matrix(void){
	if(Madm_Decision_Matrix::table_matrix!=NULL){
		delete Madm_Decision_Matrix::table_matrix;
		Madm_Decision_Matrix::table_matrix=NULL;
	}
}
//Select the data in the database table for madm-matrix for the specified parameters (static)
int Madm_Decision_Matrix::select_data_in_table_matrix(QSqlQueryModel *model, QSqlDatabase *ptr_database, _risk_type type){
	try{
		Madm_Decision_Matrix::set_table_matrix(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "SELECT * FROM ";
	test_filter << Madm_Decision_Matrix::table_matrix->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Madm_Decision_Matrix::table_matrix->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type)<< "' ";
	test_filter << " ORDER BY " <<  Madm_Decision_Matrix::table_matrix->get_column_name(label::glob_id);

	Data_Base::database_request(model, test_filter.str(), ptr_database);

	//check the request
	if(model->lastError().isValid()){
		Error msg;
		msg.set_msg("Madm_Decision_Matrix::select_data_in_table_matrix(QSqlQueryModel *model, QSqlDatabase *ptr_database, _risk_type type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Madm_Decision_Matrix::table_matrix->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return model->rowCount();
}
//Read the matrix in per file
void Madm_Decision_Matrix::read_matrix_per_file(const string filename){

	ostringstream cout;
	try{
		cout << "Check the input-file of the MADM-matrix " << filename << "..."<<endl;
		Sys_Common_Output::output_madm->output_txt(&cout);
		this->check_input_file_analysis(filename);
	}
	catch(Error msg){
		throw msg;
	}


	int line_counter=0;
	QFile ifile;

	
	ostringstream prefix;
	prefix << "INP"<<"> ";
	Sys_Common_Output::output_madm->set_userprefix(&prefix);
	cout << "Input the MADM-matrix from file " << filename << "..."<<endl;
	Sys_Common_Output::output_madm->output_txt(&cout);
	//read in damage function
	try{
		//open file
		ifile.setFileName(filename.c_str());
		QIODevice::OpenMode my_flag;
		my_flag=my_flag|(QIODevice::ReadOnly);
		my_flag=my_flag|(QIODevice::Text);
		ifile.open(my_flag);
		if(ifile.isOpen()==false){
			Error msg=this->set_error(3);
			ostringstream info;
			info << "Filename: " << filename << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		Madm_System::check_stop_thread_flag();
		this->read_in_general_data_file(&ifile, &line_counter);
		//init the matrix
		this->init_matrix(this->number_alt, this->number_crit);

		Madm_System::check_stop_thread_flag();
		this->read_in_criteria_data_file(&ifile, &line_counter);
		Madm_System::check_stop_thread_flag();
		this->read_in_matrix_data_file(&ifile, &line_counter);

	}
	catch(Error msg){
		ifile.close();
		Sys_Common_Output::output_madm->rewind_userprefix();
		ostringstream info;
		info << "Error occurs near line: "<< line_counter << endl;
		info << "Filename              : "<< filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	cout << "Input of the MADM-matrix " << this->name<<" from file " << filename << " is finished"<<endl;
	Sys_Common_Output::output_madm->output_txt(&cout);
	ifile.close();
	Sys_Common_Output::output_madm->rewind_userprefix();

}
///Read the matrix in per database
void Madm_Decision_Matrix::read_matrix_per_database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type){

	try{
		Madm_Decision_Matrix::set_table_matrix(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	QSqlQueryModel model;
	for(int i=0; i<this->number_alt; i++){
		test_filter.str("");
		test_filter<< "SELECT * FROM ";
		test_filter << Madm_Decision_Matrix::table_matrix->get_table_name();
		test_filter << " WHERE " ;
		test_filter << Madm_Decision_Matrix::table_matrix->get_column_name(label::areastate_id) << " = "  << area_state;
		test_filter << " AND " ;
		test_filter << Madm_Decision_Matrix::table_matrix->get_column_name(label::measure_id) << " = "  << this->alt[i].index;
		test_filter << " AND " ;
		test_filter << Madm_Decision_Matrix::table_matrix->get_column_name(risk_label::risk_type) << " = '"  <<Risk_Break_Scenario::convert_risk_type2txt(type)<<"'";
		Data_Base::database_request(&model, test_filter.str(), ptr_database);

		if(model.lastError().isValid()){
			Error msg=this->set_error(20);
			ostringstream info;
			info << "Table Name      : " << Madm_Decision_Matrix::table_matrix->get_table_name() << endl;
			info << "Table error info: " << model.lastError().text().toStdString() << endl;
			info << "Measure id      : " <<this->alt[i].index<<endl;
			info << "Measure name    : " <<this->alt[i].name<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		if(model.rowCount()==0){
			Error msg=this->set_error(21);
			ostringstream info;
			info << "Table Name      : " << Madm_Decision_Matrix::table_matrix->get_table_name() << endl;
			info << "Table error info: " << model.lastError().text().toStdString() << endl;
			info << "Measure id      : " <<this->alt[i].index<<endl;
			info << "Measure name    : " <<this->alt[i].name<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		else{
			this->matrix[i][0]=model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ecn)).c_str()).toDouble();
			this->matrix[i][1]=model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_eco)).c_str()).toDouble();
			this->matrix[i][2]=model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_pop_aff)).c_str()).toDouble();
			this->matrix[i][3]=model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_pop_dan)).c_str()).toDouble();
			this->matrix[i][4]=model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_pys)).c_str()).toDouble();
			this->matrix[i][5]=model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_sc_pub)).c_str()).toDouble();
			this->matrix[i][6]=model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_sc_eco)).c_str()).toDouble();
			this->matrix[i][7]=model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_sc_cult)).c_str()).toDouble();
			this->matrix[i][8]=model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_sc_person)).c_str()).toDouble();

			this->matrix[i][9] = model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_elect_pt)).c_str()).toDouble();
			this->matrix[i][10] = model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_info_tec_pt)).c_str()).toDouble();

			this->matrix[i][11] = model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_water_sup_pt)).c_str()).toDouble();
			this->matrix[i][12] = model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_water_treat_pt)).c_str()).toDouble();


			this->matrix[i][13] = model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_energy_pt)).c_str()).toDouble();
			this->matrix[i][14] = model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_health_pt)).c_str()).toDouble();

			this->matrix[i][15] = model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_social_pt)).c_str()).toDouble();
			this->matrix[i][16] = model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_mat_pt)).c_str()).toDouble();

			this->matrix[i][17]=model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_outflow)).c_str()).toDouble();
			this->matrix[i][18]=model.record(0).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_cost_ecn)).c_str()).toDouble();

	

		}
	}
}
//Set values of the matrix table 
bool Madm_Decision_Matrix::set_matrix_values2database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int number_alt, _alt_general_info *alts){

	bool ok_flag=true;
	double base_ecn_risk=0.0;
	double base_eco_risk=0.0;
	double base_pop_aff_risk=0.0;
	double base_pop_dan_risk=0.0;
	double base_pys_risk=0.0;
	double base_outflow_risk=0.0;
	double base_sc_pub_risk=0.0;
	double base_sc_eco_risk=0.0;
	double base_sc_cult_risk=0.0;
	double base_sc_person_risk=0.0;

	double base_ci_elect_risk = 0.0;
	double base_ci_info_tec_risk = 0.0;
	double base_ci_water_sup_risk = 0.0;
	double base_ci_water_treat_risk = 0.0;
	double base_ci_energy_risk = 0.0;
	double base_ci_health_risk = 0.0;
	double base_ci_social_risk = 0.0;
	double base_ci_mat_risk = 0.0;

	double buff_ecn_risk=0.0;
	double buff_eco_risk=0.0;
	double buff_pop_aff_risk=0.0;
	double buff_pop_dan_risk=0.0;
	double buff_pys_risk=0.0;
	double buff_outflow_risk=0.0;
	double buff_ecn_cost=0.0;
	double buff_sc_pub_risk=0.0;
	double buff_sc_eco_risk=0.0;
	double buff_sc_cult_risk=0.0;
	double buff_sc_person_risk=0.0;

	double buff_ci_elect_risk = 0.0;
	double buff_ci_info_tec_risk = 0.0;
	double buff_ci_water_sup_risk = 0.0;
	double buff_ci_water_treat_risk = 0.0;
	double buff_ci_energy_risk = 0.0;
	double buff_ci_health_risk = 0.0;
	double buff_ci_social_risk = 0.0;
	double buff_ci_mat_risk = 0.0;

	_sys_system_id buff_id;
	buff_id.area_state=area_state;
	buff_id.measure_nr=0;

	//base state
	QSqlQueryModel risk_model;
	if(Risk_System::select_data_in_resulttable(&risk_model, ptr_database, buff_id, type)==0){
		ok_flag=false;
		return ok_flag;
	}
	else{
		base_ecn_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble()+
			risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble();	
		base_eco_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble()+
			risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble();	
		base_pop_aff_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
		base_pop_dan_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
		base_pys_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble()+
			risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble()+
			risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::denscrit_female)).c_str()).toDouble()+
			risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
		base_outflow_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(risk_label::max_outflow)).c_str()).toDouble();

		base_sc_pub_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_pub_build)).c_str()).toDouble();
		base_sc_eco_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_eco_build)).c_str()).toDouble();
		base_sc_cult_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_cult_build)).c_str()).toDouble();
		base_sc_person_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_person_build)).c_str()).toDouble();

		base_ci_elect_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_elect_pt)).c_str()).toDouble();
		base_ci_info_tec_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_info_tec_pt)).c_str()).toDouble();
		base_ci_water_sup_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_water_sup_pt)).c_str()).toDouble();
		base_ci_water_treat_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_water_treat_pt)).c_str()).toDouble();
		base_ci_energy_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_energy_pt)).c_str()).toDouble();

		base_ci_health_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_health_pt)).c_str()).toDouble();
		base_ci_social_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_social_pt)).c_str()).toDouble();
		base_ci_mat_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_mat_pt)).c_str()).toDouble();

	}

	ostringstream filter_header;

	filter_header << "INSERT INTO  " << Madm_Decision_Matrix::table_matrix->get_table_name();
	filter_header <<" ( ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(label::glob_id) <<" , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(label::areastate_id) <<" , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(label::measure_id) <<" , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(risk_label::risk_type) <<" , ";
	
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ecn) <<" , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_eco) <<" , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_pop_aff) <<" , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_pop_dan) <<" , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_pys) <<" , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_sc_pub) <<" , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_sc_eco) <<" , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_sc_cult) <<" , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_sc_person) <<" , ";

	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_elect_pt) << " , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_info_tec_pt) << " , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_water_sup_pt) << " , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_water_treat_pt) << " , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_energy_pt) << " , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_health_pt) << " , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_social_pt) << " , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ci_mat_pt) << " , ";

	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_outflow) <<" , ";
	filter_header << Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_cost_ecn) ;
	filter_header<<" ) ";
	
	ostringstream data;
	ostringstream total;
	int glob_id=0;
	QSqlQuery madm_model(*ptr_database);
	try{
		Madm_Decision_Matrix::set_table_matrix(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	glob_id=Madm_Decision_Matrix::table_matrix->maximum_int_of_column(Madm_Decision_Matrix::table_matrix->get_column_name(label::glob_id),ptr_database)+1;

	for(int i=0; i< number_alt; i++){
		data.str("");
		total.str("");
		buff_id.measure_nr=alts[i].id.measure_nr;
		if(Cost_Ecn_Cost_System::get_total_cost_table(ptr_database, buff_id, &buff_ecn_cost)==false){
			ok_flag=false;
			continue;
		}
		if(Risk_System::select_data_in_resulttable(&risk_model, ptr_database, buff_id, type)==0){
			ok_flag=false;
			continue;
		}
		else{
			buff_ecn_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::immob_dam)).c_str()).toDouble()+
				risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::mob_dam)).c_str()).toDouble();	
			buff_eco_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::btype_cost)).c_str()).toDouble()+
				risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::soil_cost)).c_str()).toDouble();	
			buff_pop_aff_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::pop_affected)).c_str()).toDouble();
			buff_pop_dan_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::pop_endangered)).c_str()).toDouble();
			buff_pys_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble()+
				risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble()+
				risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::denscrit_female)).c_str()).toDouble()+
				risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
			buff_outflow_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(risk_label::max_outflow)).c_str()).toDouble();

			buff_sc_pub_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_pub_build)).c_str()).toDouble();
			buff_sc_eco_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_eco_build)).c_str()).toDouble();
			buff_sc_cult_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_cult_build)).c_str()).toDouble();
			buff_sc_person_risk=risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::sc_person_build)).c_str()).toDouble();

			buff_ci_elect_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_elect_pt)).c_str()).toDouble();
			buff_ci_info_tec_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_info_tec_pt)).c_str()).toDouble();
			buff_ci_water_sup_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_water_sup_pt)).c_str()).toDouble();
			buff_ci_water_treat_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_water_treat_pt)).c_str()).toDouble();
			buff_ci_energy_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_energy_pt)).c_str()).toDouble();

			buff_ci_health_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_health_pt)).c_str()).toDouble();
			buff_ci_social_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_social_pt)).c_str()).toDouble();
			buff_ci_mat_risk = risk_model.record(0).value((Risk_System::table_results->get_column_name(dam_label::ci_mat_pt)).c_str()).toDouble();


		}


		data << " VALUES ( ";
		data << glob_id << " , " ;
		data << buff_id.area_state << " , " ;
		data << buff_id.measure_nr<< " , " ;
		data << "'"<< Risk_Break_Scenario::convert_risk_type2txt(type) << "' , " ;
		data << base_ecn_risk-buff_ecn_risk << " , " ;
		data << base_eco_risk-buff_eco_risk << " , " ;
		data << base_pop_aff_risk-buff_pop_aff_risk << " , " ;
		data << base_pop_dan_risk-buff_pop_dan_risk << " , " ;
		data << base_pys_risk-buff_pys_risk << " , " ;
		data << base_sc_pub_risk-buff_sc_pub_risk << " , " ;
		data << base_sc_eco_risk-buff_sc_eco_risk << " , " ;
		data << base_sc_cult_risk-buff_sc_cult_risk << " , " ;
		data << base_sc_person_risk-buff_sc_person_risk << " , " ;


		data << base_ci_elect_risk - buff_ci_elect_risk << " , ";
		data << base_ci_info_tec_risk - buff_ci_info_tec_risk << " , ";
		data << base_ci_water_sup_risk - buff_ci_water_sup_risk << " , ";
		data << base_ci_water_treat_risk - buff_ci_water_treat_risk << " , ";
		data << base_ci_energy_risk - buff_ci_energy_risk << " , ";
		data << base_ci_health_risk - buff_ci_health_risk << " , ";
		data << base_ci_social_risk - buff_ci_social_risk << " , ";
		data << base_ci_mat_risk - buff_ci_mat_risk << " , ";


		data << base_outflow_risk-buff_outflow_risk << " , " ;
		data << buff_ecn_cost << " ) " ;
		total << filter_header.str() << data.str();
		Data_Base::database_request(&madm_model, total.str(), ptr_database);


		if(madm_model.lastError().isValid()){
			Warning msg=this->set_warning(0);
			ostringstream info;
			info << "Table Name                : " << Madm_Decision_Matrix::table_matrix->get_table_name() << endl;
			info << "Table error info          : " << madm_model.lastError().text().toStdString() << endl;
			info << "Measure id                : " << buff_id.measure_nr<<endl;
			info << "Measure name              : " << alts[i].name<<endl;
			msg.make_second_info(info.str());
			msg.output_msg(3);	
		}

		glob_id++;
	}

	//last base state
	data.str("");
	total.str("");
	data << " VALUES ( ";
	data << glob_id << " , " ;
	data << buff_id.area_state << " , " ;
	data << 0<< " , " ;
	data << "'"<< Risk_Break_Scenario::convert_risk_type2txt(type) << "' , " ;
	data << 0.0 << " , " ;
	data << 0.0 << " , " ;
	data << 0.0 << " , " ;
	data << 0.0 << " , " ;
	data << 0.0 << " , " ;
	data << 0.0 << " , " ;
	data << 0.0 << " , " ;
	data << 0.0 << " , " ;
	data << 0.0 << " , " ;
	data << 0.0 << " , " ;
	data << 0.0 << " , ";
	data << 0.0 << " , ";
	data << 0.0 << " , ";
	data << 0.0 << " , ";
	data << 0.0 << " , ";
	data << 0.0 << " , ";
	data << 0.0 << " , ";
	data << 0.0 << " , ";
	data << 0.0 << " ) " ;
	total << filter_header.str() << data.str();
	Data_Base::database_request(&madm_model, total.str(), ptr_database);

	if(madm_model.lastError().isValid()){
			Warning msg=this->set_warning(0);
			ostringstream info;
			info << "Table Name                : " << Madm_Decision_Matrix::table_matrix->get_table_name() << endl;
			info << "Table error info          : " << madm_model.lastError().text().toStdString() << endl;
			info << "Measure id                : " << 0<<endl;
			info << "Measure name              : " << "Bas-state"<<endl;
			msg.make_second_info(info.str());
			msg.output_msg(3);	
	}

	return ok_flag;
}
//Clear the matrix
void Madm_Decision_Matrix::clear_matrix(void){
	this->delete_criteria();
	this->delete_alternatives();
	this->delete_matrix();
	this->number_crit=0;
	this->number_alt=0;
}
//Change the weights of the criteria
void Madm_Decision_Matrix::change_weights(const int index, const double weight){
	if(index<0 || index>=this->number_crit){
		return;
	}
	else{
		this->crit[index].weight=weight;
	}

}
//Initialize the matrix
void Madm_Decision_Matrix::init_matrix(const int number_alternatives, const int number_criteria){
	this->clear_matrix();

	this->number_crit=number_criteria;
	this->number_alt=number_alternatives;

	this->allocate_alternatives();
	this->allocate_criteria();
	this->allocate_matrix();

}
//Set a alternative specified by the given index
void Madm_Decision_Matrix::set_alternative(const _madm_alternative alternative, const int index){
	if(index<0 || index>=this->number_alt){
		return;
	}
	else{
		this->alt[index]=alternative;
	}
}
//Set a criteria specified by the given index
void Madm_Decision_Matrix::set_criteria(const _madm_criteria criteria, const int index){
	if(index<0 || index>=this->number_crit){
		return;
	}
	else{
		this->crit[index]=criteria;
	}
}
//Get number of alternatives
int Madm_Decision_Matrix::get_number_alternatives(void){
	return this->number_alt;
}
//Get the alternative specified by the given index
_madm_alternative Madm_Decision_Matrix::get_alternative(const int index){
	if(index<0 || index>=this->number_alt){
		_madm_alternative buff;
		this->init_alt(&buff);
		return buff;
	}
	else{
		return this->alt[index];
	}
}
//Get number of criteria
int Madm_Decision_Matrix::get_number_criteria(void){
	return this->number_crit;
}
//Get the criteria specified by the given index
_madm_criteria Madm_Decision_Matrix::get_criteria(const int index){
	if(index<0 || index>=this->number_crit){
		_madm_criteria buff;
		this->init_crit(&buff);
		return buff;
	}
	else{
		return this->crit[index];
	}
}
//Get a matrix value specified by the given indices
double Madm_Decision_Matrix::get_matrix_value(const int index_alt, const int index_crit){
	if(index_alt<0 || index_alt>=this->number_alt){
		return -99999.0;
	}
	else if(index_crit<0 || index_crit>=this->number_crit){
		return -99999.0;
	}
	else{
		return this->matrix[index_alt][index_crit];
	}
}
//Apply the weigthing of the matrix (use it after the normalisation!)
void Madm_Decision_Matrix::apply_weighting(void){
	this->crit_weight_normalisation();
	//apply the weights
	for (int i=0; i< this->number_crit; i++){
		for(int j=0; j< this->number_alt; j++){
			this->matrix[j][i]=this->crit[i].weight*this->matrix[j][i];
		}
	}
}
//Set the normalisation type and apply the normalsation
void Madm_Decision_Matrix::apply_matrix_normalisation(const _madm_normalisation_type type){
	this->norm_type=type;

	switch(type){
		case _madm_normalisation_type::norm_no:
			return;
		case _madm_normalisation_type::norm_standard:
			this->standard_norm_scheme();
			break;
		case _madm_normalisation_type::norm_range:
			this->range_norm_scheme();
			break;
		case _madm_normalisation_type::norm_vector:
			this->vector_scheme();
			break;
		default:
			this->norm_type=_madm_normalisation_type::norm_no;
			
	}
}
//Output matrix to display console
void Madm_Decision_Matrix::output_matrix(void){
	ostringstream prefix;
	ostringstream cout;
	prefix<<"OUT> ";
	Sys_Common_Output::output_madm->set_userprefix(&prefix);
	if(this->number_alt>0 && this->number_crit>0){
		cout <<"DECISION MATRIX"<<endl;
		cout <<" Number criteria     : " << this->number_crit<<endl;
		cout <<" Number alternatives : " << this->number_alt<<endl;
		cout <<" Normalisation type  : " << Madm_Decision_Matrix::convert_norm_scheme2txt(this->norm_type)<<endl;
		Sys_Common_Output::output_madm->output_txt(&cout,true);
		cout << "    CRIT        ";
		for(int j=0; j<this->number_crit; j++){
			cout <<" "<<this->crit[j].name <<W(13);
		}
		cout <<endl;
		Sys_Common_Output::output_madm->output_txt(&cout,true);
		cout << FORMAT_FIXED_REAL<< P(3)<< "    WEIGHTS        ";
		for(int j=0; j<this->number_crit; j++){
			cout <<" "<<this->crit[j].weight <<W(15);
		}
		cout <<endl;
		Sys_Common_Output::output_madm->output_txt(&cout,true);
		cout << "    MAX/MIN        ";
		for(int j=0; j<this->number_crit; j++){
			cout <<" "<<functions::convert_boolean2string(this->crit[j].max_flag) ;
			if(j<this->number_crit-1){
				cout <<W(15);	
			}
		}
		cout <<endl;
		Sys_Common_Output::output_madm->output_txt(&cout,true);
		
		for(int i=0; i< this->number_alt; i++){
			cout << "    "<<this->alt[i].name<<W(15);
			for(int j=0; j<this->number_crit; j++){
				cout <<" "<<matrix[i][j];
				if(j<this->number_crit-1){
					cout<<W(18);	
				}
			}
			cout <<endl;
			Sys_Common_Output::output_madm->output_txt(&cout,true);

		}
	}
	else{
		cout <<"No matrix is set!"<<endl;
	}


	Sys_Common_Output::output_madm->output_txt(&cout,true);
	Sys_Common_Output::output_madm->rewind_userprefix();

}
//Output matrix to file
void Madm_Decision_Matrix::output_matrix(ofstream *outfile){
	*outfile <<"DECISION MATRIX"<<endl;
	*outfile <<" Number criteria     : " << this->number_crit<<endl;
	*outfile <<" Number alternatives : " << this->number_alt<<endl;
	*outfile <<" Normalisation type  : " << Madm_Decision_Matrix::convert_norm_scheme2txt(this->norm_type)<<endl;
	*outfile << "    CRIT        ";
	for(int j=0; j<this->number_crit; j++){
		*outfile <<" "<<this->crit[j].name <<W(13);
	}
	*outfile <<endl;

	*outfile <<FORMAT_FIXED_REAL<< P(3)<< "    WEIGHTS        ";
	for(int j=0; j<this->number_crit; j++){
		*outfile <<" "<<this->crit[j].weight <<W(15);
	}
	*outfile <<endl;

	*outfile << "    MAX/MIN        ";
	for(int j=0; j<this->number_crit; j++){
		*outfile <<" " <<functions::convert_boolean2string(this->crit[j].max_flag);
		if(j<this->number_crit-1){
			*outfile <<W(15);	
		}
	}
	*outfile <<endl;

	
	for(int i=0; i< this->number_alt; i++){
		*outfile <<"    "<<this->alt[i].name<<W(15);
		for(int j=0; j<this->number_crit; j++){
			*outfile <<matrix[i][j];
			if(j<this->number_crit-1){
				*outfile <<W(18);	
			}
		}
		*outfile <<endl;
	}
	outfile->flush();
}
//Convert a string into a madm normalisation scheme (_madm_normalisation_type)
_madm_normalisation_type Madm_Decision_Matrix::convert_txt2norm_scheme(const string txt){
	_madm_normalisation_type type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lower_case(&buffer);

	if(buffer==madm_label::norm_standard){
		type=_madm_normalisation_type::norm_standard;
	}
	else if(buffer==madm_label::norm_range){
		type=_madm_normalisation_type::norm_range;
	}
	else if(buffer==madm_label::norm_vector){
		type=_madm_normalisation_type::norm_vector;
	}
	else if(buffer==madm_label::norm_no){
		type=_madm_normalisation_type::norm_no;
	}
	else{
		Error msg;
		msg.set_msg("_madm_normalisation_type Madm_Decision_Matrix::convert_txt2norm_scheme(const string txt)", "Can not convert this normalisation scheme", "Check the given normalisation scheme", 1, false);
		ostringstream info;
		info << "Try to convert normalisation scheme: " << txt << endl;
		info << "Possible normalisation schemes: "<< endl;
		info << " "<<madm_label::norm_standard  << endl;
		info << " "<<madm_label::norm_range << endl;
		info << " "<<madm_label::norm_vector  << endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	return type;
}
//Convert a madm approach (_madm_normalisation_type) into a string 
string Madm_Decision_Matrix::convert_norm_scheme2txt(const _madm_normalisation_type scheme){
	string buffer;
	switch(scheme){
		case _madm_normalisation_type::norm_standard:
			buffer=madm_label::norm_standard;
			break;
		case _madm_normalisation_type::norm_range:
			buffer=madm_label::norm_range;
			break;
		case _madm_normalisation_type::norm_vector:
			buffer=madm_label::norm_vector;
			break;
		case _madm_normalisation_type::norm_no:
			buffer=madm_label::norm_no;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;

}
//Get the name of the matrix
string Madm_Decision_Matrix::get_matrix_name(void){
	return this->name;
}
//copy operator
Madm_Decision_Matrix& Madm_Decision_Matrix::operator= (const Madm_Decision_Matrix& copy){

	this->delete_criteria();
	this->delete_alternatives();
	this->delete_matrix();

	this->number_crit=copy.number_crit;
	this->number_alt=copy.number_alt;
	this->norm_type=copy.norm_type;
	this->name=copy.name;

	this->init_matrix(this->number_alt,this->number_crit);

	//copy alternatives
	for(int i=0; i< this->number_alt; i++){
		this->alt[i]=copy.alt[i];

	}
	//copy criteria
	for(int i=0; i< this->number_crit; i++){
		this->crit[i]=copy.crit[i];

	}

	//copy matrix
	for(int i=0; i< this->number_alt; i++){
		for(int j=0; j<this->number_crit; j++){
			this->matrix[i][j]=copy.matrix[i][j];
		}
	}

	return *this;
}
//_________
//private
//Allocate the criteria
void Madm_Decision_Matrix::allocate_criteria(void){
	try{
		this->crit=new _madm_criteria[this->number_crit];
		Sys_Memory_Count::self()->add_mem(sizeof(_madm_criteria)*this->number_crit,_sys_system_modules::MADM_SYS);//count the memory
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init
	ostringstream buff;
	for(int i=0; i< this->number_crit; i++){
		this->crit[i].index=i;
		buff<<"crit_"<<i;
		this->crit[i].name=buff.str();
		this->crit[i].weight=1.0;
		this->crit[i].max_flag=true;
		buff.str("");
	}
}
//Delete the criteria
void Madm_Decision_Matrix::delete_criteria(void){
	if(this->crit!=NULL){
		delete []this->crit;
		Sys_Memory_Count::self()->minus_mem(sizeof(_madm_criteria)*this->number_crit,_sys_system_modules::MADM_SYS);//count the memory
		this->crit=NULL;
	}
}
//Allocate the alternatives
void Madm_Decision_Matrix::allocate_alternatives(void){
	try{
		this->alt=new _madm_alternative[this->number_alt];
		Sys_Memory_Count::self()->add_mem(sizeof(_madm_alternative)*this->number_alt,_sys_system_modules::MADM_SYS);//count the memory
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init
	ostringstream buff;
	for(int i=0; i< this->number_alt; i++){
		this->alt[i].index=i;
		buff<<"alt_"<<i;
		this->alt[i].name=buff.str();
		buff.str("");
	}
}
//Delete the alternatives
void Madm_Decision_Matrix::delete_alternatives(void){
	if(this->alt!=NULL){
		delete []this->alt;
		Sys_Memory_Count::self()->minus_mem(sizeof(_madm_alternative)*this->number_alt,_sys_system_modules::MADM_SYS);//count the memory
		this->alt=NULL;
	}
}
//Allocate the matrix
void Madm_Decision_Matrix::allocate_matrix(void){
	try{
		this->matrix=new double*[this->number_alt];

		for(int i=0;i<this->number_alt; i++){
			this->matrix[i]=NULL;
			this->matrix[i]=new double[this->number_crit];
		}
		Sys_Memory_Count::self()->add_mem(sizeof(double)*this->number_alt*this->number_crit,_sys_system_modules::MADM_SYS);//count the memory

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//init matrix
	for(int i=0; i< this->number_alt; i++){
		for(int j=0; j<this->number_crit; j++){
			this->matrix[i][j]=0.0;
		}
	}
}
//Delete the matrix
void Madm_Decision_Matrix::delete_matrix(void){
	if(this->matrix!=NULL){
		for(int i=0; i< this->number_alt; i++){
			if(this->matrix[i]!=NULL){
				delete []this->matrix[i];
			}
		}
		delete []this->matrix;
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->number_alt*this->number_crit,_sys_system_modules::MADM_SYS);//count the memory
		this->matrix=NULL;
	}
}
//Initialize the criteria structure
void Madm_Decision_Matrix::init_crit(_madm_criteria *init){
	init->index=-1;
	init->max_flag=true;
	init->weight=0.0;
	init->name=label::not_defined;
}
//Initialize the alternative structure
void Madm_Decision_Matrix::init_alt(_madm_alternative *init){
	init->name=label::not_defined;
	init->index=-1;
}
//Normalisation of the matrix after the standard normalisation scheme
void Madm_Decision_Matrix::standard_norm_scheme(void){
	double max=-1e20;

	//convert min-criteria values
	for(int i=0; i< this->number_crit; i++){
		if (this->crit[i].max_flag==false){
			for(int j=0; j< this->number_alt; j++){
				if(this->matrix[j][i]==0.0){
					this->matrix[j][i]=1e-20;
				}
				this->matrix[j][i]=1.0/this->matrix[j][i];
			}
		}
	}


	for (int i=0; i< this->number_crit; i++){
		max=this->matrix[0][i];
		for(int j=0; j< this->number_alt; j++){
			if(max<this->matrix[j][i]){
				max=this->matrix[j][i];	
			}
		}
		if(max==0.0){
			max=-1e20;
			for(int j=0; j< this->number_alt; j++){
				if(this->matrix[j][i]!=0.0 && max<this->matrix[j][i]){
					max=this->matrix[j][i];	
				}
			}
			
		}
		for(int j=0; j< this->number_alt; j++){
			this->matrix[j][i]= this->matrix[j][i]/max;
		}
	}
}
//Normalisation of the matrix after the range normalisation scheme
void Madm_Decision_Matrix::range_norm_scheme(void){
	double max=-1e20;
	double min=+1e20;
	for (int i=0; i< this->number_crit; i++){
		max=this->matrix[0][i];
		min=this->matrix[0][i];
		for(int j=0; j< this->number_alt; j++){
			if(max<this->matrix[j][i]){
				max=this->matrix[j][i];	
			}
			if(min>this->matrix[j][i]){
				min=this->matrix[j][i];	
			}
		}
		double delta=max-min;
		if(delta==0.0){
			delta=1e-20;
		}
		for(int j=0; j< this->number_alt; j++){
			if (this->crit[i].max_flag==false){
				this->matrix[j][i]= (max - this->matrix[j][i])/delta;
			}
			else if (this->crit[i].max_flag==true){
				this->matrix[j][i]= (this->matrix[j][i]-min)/delta;
			}
		}
	}
}
//Normalisation of the matrix after the vector normalisation scheme
void Madm_Decision_Matrix::vector_scheme(void){
	double denom=0.0;
	for (int i=0; i< this->number_crit; i++){
		for(int j=0; j< this->number_alt; j++){
			denom=denom+pow(this->matrix[j][i],2);
		}
		denom=pow(denom,0.5);
		if(denom==0.0){
			denom=1e-20;
		}
		for(int j=0; j< this->number_alt; j++){
			this->matrix[j][i]=this->matrix[j][i]/denom;
		}
		denom=0.0;
	}
}
//Normalisation of the criteria weights
void Madm_Decision_Matrix::crit_weight_normalisation(void){
	double sumweight=0.0;
	for (int i=0; i< this->number_crit; i++){
		sumweight=sumweight+this->crit[i].weight;
	}
	for(int i=0; i< this->number_crit; i++){
		this->crit[i].weight=this->crit[i].weight/sumweight;
	}
}
//Check the file for the madm-analysis if the !$BEGIN... always are close with !$END...
void Madm_Decision_Matrix::check_input_file_analysis(const string file_name){
	int line_counter=0;
	QFile ifile;
	//open file
	ifile.setFileName(file_name.c_str());
	QIODevice::OpenMode my_flag;
	my_flag=my_flag|(QIODevice::ReadOnly);
	my_flag=my_flag|(QIODevice::Text);
	ifile.open(my_flag);
	if(ifile.isOpen()==false){
		Error msg=this->set_error(3);
		ostringstream info;
		info << "Filename: " << file_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//check the file
	string myline;
	QString qmyline;
	QStringList list_begin;
	QString id_str;
	bool flag;
	try{
		do{
			qmyline=ifile.readLine();
			myline=qmyline.toStdString();
			line_counter++;
			functions::clean_string(&myline);
			id_str=myline.c_str();
			flag=functions::decide_begin_end_keyword(&id_str);
			if(flag==true && id_str.isEmpty()==false){
				list_begin.append(id_str);

			}
			else if(flag==false && id_str.isEmpty()==false){
				if(list_begin.last()==id_str){
					list_begin.removeLast();
				}
				else{
					Error msg=this->set_error(19);
					ostringstream info;
					info << "Error occurs near line: "<< line_counter << endl;
					info << "Filename              : "<< file_name << endl;
					info << "Key                   : "<< "!$END_"<<id_str.toStdString()<< endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
		while(ifile.atEnd()==false);
		
		if(list_begin.count()!=0){
			Error msg=this->set_error(18);
			ostringstream info;
			info << "Filename                 : "<< file_name << endl;
			info << "Number blocks not closed : "<< list_begin.count()<< endl;
			for(int i=0; i< list_begin.count(); i++){
				info << "Key                      : "<< "!$BEGIN_"<<list_begin.at(i).toStdString()<< endl;	
			}
			msg.make_second_info(info.str());
			throw msg;
		}

	}
	catch(Error msg){
		ifile.close();
		throw msg;
	}

	//close file
	ifile.close();
}
//Read in the general data block for the matrix per file
void Madm_Decision_Matrix::read_in_general_data_file(QFile *ifile, int *line_counter){
	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	int must_found_counter=0;


	//search for the begin
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==madm_label::begin_general){
			found_flag=true;
			break;
		}
	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(4);
		throw msg;

	}
	else{
		found_flag=false;
	}
	//search for the keywords
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==madm_label::end_general){
			found_flag=true;
			break;
		}
		else if(myline==madm_label::begin_criteria){
			break;
		}
		else if(myline==madm_label::begin_matrix){
			break;
		}
		else if(myline.empty()==false){
			this->find_keyvalues_general_data_file(myline, &must_found_counter);
		}

	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(5);
		throw msg;
	}

	if(must_found_counter!=3){
		Error msg=this->set_error(6);
		throw msg;
	}

}
//Read in the criteria data block for the matrix per file
void Madm_Decision_Matrix::read_in_criteria_data_file(QFile *ifile, int *line_counter){
	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	int must_found_counter=0;


	//search for the begin
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==madm_label::begin_criteria){
			found_flag=true;
			break;
		}
	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(7);
		throw msg;

	}
	else{
		found_flag=false;
	}
	//search for the keywords
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==madm_label::end_criteria){
			found_flag=true;
			break;
		}
		else if(myline==madm_label::begin_matrix){
			break;
		}
		else if(myline.empty()==false){
			this->find_keyvalues_criteria_data_file(myline, &must_found_counter);
		}

	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(8);
		throw msg;
	}

	if(must_found_counter!=3){
		Error msg=this->set_error(9);
		throw msg;
	}
}
//Read in the matrix data block for the matrix per file
void Madm_Decision_Matrix::read_in_matrix_data_file(QFile *ifile, int *line_counter){

	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	int must_found_counter=0;


	//search for the begin
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==madm_label::begin_matrix){
			found_flag=true;
			break;
		}
	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(10);
		throw msg;

	}
	else{
		found_flag=false;
	}
	//search for the keywords
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==madm_label::end_matrix){
			found_flag=true;
			break;
		}
		else if(myline.empty()==false){
			this->find_keyvalues_matrix_data_file(myline, &must_found_counter);
		}

	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(11);
		throw msg;
	}

	if(must_found_counter!=this->number_alt){
		Error msg=this->set_error(12);
		throw msg;
	}
}
//Find the key values for the general data block for an file-input
void Madm_Decision_Matrix::find_keyvalues_general_data_file(const string myline, int *must_found){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(madm_label::analysis_name );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::analysis_name.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->name;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	pos=myline.find(madm_label::no_crit );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::no_crit.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->number_crit;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	pos=myline.find(madm_label::no_alt );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::no_alt.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->number_alt;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found)++;
			return;
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(14);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Find the key values for the criteria data block for an file-input
void Madm_Decision_Matrix::find_keyvalues_criteria_data_file(const string myline, int *must_found){
	int pos=-1;
	string buffer;
	string column;
	int column_nr=0;
	bool wrong_input=false;

	pos=myline.find(madm_label::crit_name);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::crit_name.length());
		functions::clean_string(&buffer);
		if(functions::count_number_columns(buffer)!=this->number_crit){
			Error msg=this->set_error(13);
			ostringstream info;
			info <<"All names of the criteria have to be specified here!"<<endl;
			info <<"Number of criteria : " <<this->number_crit<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		for(int i=0; i< this->number_crit; i++){
			column_nr=i;
			column=functions::get_column(&buffer);
			functions::clean_string(&buffer);
			this->crit[i].name=column;
		}
		if(wrong_input==false){
			(*must_found)++;
			return;
		}
	}

	pos=myline.find(madm_label::crit_weight );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::crit_weight.length());
		functions::clean_string(&buffer);
		if(functions::count_number_columns(buffer)!=this->number_crit){
			Error msg=this->set_error(13);
			ostringstream info;
			info <<"All weights of the criteria have to be specified here!"<<endl;
			info <<"Number of criteria : " <<this->number_crit<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		for(int i=0; i< this->number_crit; i++){
			column_nr=i;
			stringstream buffer1;
			column=functions::get_column(&buffer);
			buffer1<<column;
			buffer1>>this->crit[i].weight;
			if(buffer1.fail()==true){
				wrong_input=true;
				break;
			}
		}
		if(wrong_input==false){
			(*must_found)++;
			return;
		}
	}

	pos=myline.find(madm_label::crit_max_min );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(madm_label::crit_max_min.length());
		functions::clean_string(&buffer);
		if(functions::count_number_columns(buffer)!=this->number_crit){
			Error msg=this->set_error(13);
			ostringstream info;
			info <<"All max/min-flags of the criteria have to be specified here!"<<endl;
			info <<"Number of criteria : " <<this->number_crit<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		for(int i=0; i< this->number_crit; i++){
			column_nr=i;
			stringstream buffer1;
			column=functions::get_column(&buffer);
			functions::clean_string(&column);
			try{
				this->crit[i].max_flag=functions::convert_string2boolean(column);
			}
			catch(Error msg){
				wrong_input=true;
				break;
			}
		}
		if(wrong_input==false){
			(*must_found)++;
			return;
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Column number         : " << column_nr << endl;
		info << "Wrong column          : " << column << endl;
		Error msg=this->set_error(15);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Find the key values for the matrix data block for an file-input
void Madm_Decision_Matrix::find_keyvalues_matrix_data_file(const string myline, int *must_found){
	string buffer=myline;
	stringstream buffer1;
	string column;
	int column_nr=0;
	bool wrong_input=false;

	functions::clean_string(&buffer);
	if(functions::count_number_columns(buffer)!=this->number_crit+1){
		Error msg=this->set_error(17);
		ostringstream info;
		info <<"All values of the criteria have to be specified here!"<<endl;
		info <<"Number of criteria : " <<this->number_crit<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//alternative name
	column_nr=1;
	column=functions::get_column(&buffer);
	functions::clean_string(&buffer);
	functions::clean_string(&column);
	this->alt[*must_found].name=column;


	if(wrong_input==false){
		for(int i=0; i< this->number_crit; i++){
			column_nr++;
			buffer1.str("");
			buffer1.clear();
			column=functions::get_column(&buffer);
			functions::clean_string(&column);
			buffer1<<column;
			buffer1>>this->matrix[*must_found][i];
			if(buffer1.fail()==true){
				wrong_input=true;
				break;
			}
		}
		if(wrong_input==false){
			(*must_found)++;
			return;
		}
	}


	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		info << "Column number         : " << column_nr << endl;
		Error msg=this->set_error(16);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set warning(s)
Warning Madm_Decision_Matrix::set_warning(const int warn_type){
	string place="Madm_Decision_Matrix::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("set_matrix_values2database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int number_alt, int *alts)");
			reason="Can not submit the values of the decision matrix to the database";
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
//Set error(s)
Error Madm_Decision_Matrix::set_error(const int err_type){
	string place="Madm_Decision_Matrix::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_matrix(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("allocate_criteria(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://bad alloc
			place.append("allocate_alternatives(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://can not open madm-matrix file
			place.append("read_matrix_per_file(const string filename)");
			reason="Can not open the file of the MADM-matrix";
			help="Check the file";
			type=5;
			break;
		case 4://do not find the begin of the general matrix information in file
			place.append("read_in_general_data_file(QFile *ifile, int *line_counter)");
			reason="Can not find the begin of the general matrix information in file";
			help="The general matrix information has to start with !$BEGIN_GENERAL";
			type=1;
			break;
		case 5://do not find the end of the general matrix information in file
			place.append("read_in_general_data_file(QFile *ifile, int *line_counter)");
			reason="Can not find the end of the general matrix information in file";
			help="The general matrix information has to end with !$END_GENERAL";
			type=1;
			break;
		case 6://do not find all required general matrix infos
			place.append("read_in_general_data_file(QFile *ifile, int *line_counter)");
			reason="Can not find all required general matrix information";
			help="Check if all general matrix information are set in the file";
			info << "Required information:"<<endl;
			info << " " << madm_label::analysis_name<< endl;
			info << " " << madm_label::no_alt<< endl;
			info << " " << madm_label::no_crit<< endl;
			type=1;
			break;
		case 7://do not find the begin of the criteria matrix information in file
			place.append("read_in_criteria_data_file(QFile *ifile, int *line_counter)");
			reason="Can not find the begin of the criteria matrix information in file";
			help="The criteria matrix information has to start with !$BEGIN_CRITERIA";
			type=1;
			break;
		case 8://do not find the end of the criteria matrix information in file
			place.append("read_in_criteria_data_file(QFile *ifile, int *line_counter)");
			reason="Can not find the end of the criteria matrix information in file";
			help="The criteria matrix information has to end with !$END_CRITERIA";
			type=1;
			break;
		case 9://do not find all required criteria matrix infos
			place.append("read_in_criteria_data_file(QFile *ifile, int *line_counter)");
			reason="Can not find all required criteria matrix information";
			help="Check if all criteria matrix information are set in the file";
			info << "Required information:"<<endl;
			info << " " << madm_label::crit_name<< endl;
			info << " " << madm_label::crit_max_min<< endl;
			info << " " << madm_label::crit_weight<< endl;
			type=1;
			break;
		case 10://do not find the begin of the matrix information in file
			place.append("read_in_matrix_data_file(QFile *ifile, int *line_counter)");
			reason="Can not find the begin of the matrix information in file";
			help="The matrix information has to start with !$BEGIN_MATRIX";
			type=1;
			break;
		case 11://do not find the end of the general matrix information in file
			place.append("read_in_matrix_data_file(QFile *ifile, int *line_counter)");
			reason="Can not find the end of the matrix information in file";
			help="The matrix information has to end with !$END_MATRIX";
			type=1;
			break;
		case 12://do not find all required matrix infos
			place.append("read_in_matrix_data_file(QFile *ifile, int *line_counter)");
			reason="Can not find all required matrix information. All alternative(s) have to be specified here!";
			help="Check if all matrix information are set in the file";
			info << "Number of alternative(s) :"<< this->number_alt<<endl;
			type=1;
			break;
		case 13://do not find all required criteria
			place.append("find_keyvalues_criteria_data_file(const string myline, int *must_found)");
			reason="Can not find all required criteria information.";
			help="Check if all criteria are set in the file";
			type=1;
			break;
		case 14://wrong input
			place.append("find_keyvalues_general_data_file(const string myline, int *must_found)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the general matrix information in file";
			type=1;
			break;
		case 15://wrong input
			place.append("find_keyvalues_criteria_data_file(const string myline, int *must_found)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the matrix criteria information in file";
			type=1;
			break;
		case 16://wrong input
			place.append("find_keyvalues_matrix_data_file(const string myline, int *must_found)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the matrix information in file";
			type=1;
			break;
		case 17://do not find all required criteria
			place.append("find_keyvalues_matrix_data_file(const string myline, int *must_found)");
			reason="Can not find all required criteria information.";
			help="Check if all criteria are set in the file";
			type=1;
			break;
		case 18://!$BEGIN is not correctly closed
			place.append("check_input_file_analysis(const string file_name)");
			reason="The !$BEGIN-key is not correctly closed with the corresponding !$END-key";
			help="Check the file and close the blocks correctly";
			type=5;
			break;
		case 19://!$END is not correctly opened
			place.append("check_input_file_analysis(const string file_name)");
			reason="The !$END-key is not correctly opened with the corresponding !$BEGIN-key";
			help="Check the file and start the blocks correctly";
			type=5;
			break;
		case 20://wrong database request
			place.append("read_matrix_per_database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type)");
			reason="Invalid database request";
			help="Check the database";
			type=2;
			break;
		case 21://no results found in database table
			place.append("read_matrix_per_database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type)");
			reason="No results found in database table for the specified measure. Can not set-up the decision matrix.";
			help="Check the database table";
			type=30;
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
