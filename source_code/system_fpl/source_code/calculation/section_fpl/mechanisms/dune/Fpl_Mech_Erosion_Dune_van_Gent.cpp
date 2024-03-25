#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Mech_Erosion_Dune_van_Gent.h"

//Default constructor
Fpl_Mech_Erosion_Dune_van_Gent::Fpl_Mech_Erosion_Dune_van_Gent(void):delta_x(0.1){
	this->resistance.set_unit(label::squaremeter);
	this->stress.set_unit(label::squaremeter);

	this->critical_crest_width.set_unit(label::m);
	this->mean_diameter.set_unit(label::m);
	this->roundness_fac.set_unit(label::no_unit);
	this->csf.set_unit(label::no_unit);
	this->grain_density.set_unit(label::kg_per_qm);
	this->settling_velocity.set_unit(label::m_per_sec);

	this->buff_stress=0.0;
	this->buff_res=0.0;
	this->failure=0;

	this->ptr_area_out=NULL;
	this->ptr_area_cubature=NULL;
	this->ptr_out_poly=NULL;
	this->ptr_land_crest=NULL;
	this->ptr_water_crest=NULL;
	this->ptr_foreland=NULL;
	this->ptr_waterside=NULL;
	this->ptr_crest=NULL;

	this->number_new_points=0;

	this->no_point_inside=0;
	this->no_foreland_short=0;

	this->init_matrix();

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Erosion_Dune_van_Gent)-sizeof(Fpl_Random_Variables)*3-sizeof(Geo_Polygon_Interception_Point_List)*2-sizeof(Geo_Point_List)-sizeof(Geo_Interception_Point_List),_sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Mech_Erosion_Dune_van_Gent::~Fpl_Mech_Erosion_Dune_van_Gent(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mech_Erosion_Dune_van_Gent)-sizeof(Fpl_Random_Variables)*3-sizeof(Geo_Polygon_Interception_Point_List)*2-sizeof(Geo_Point_List)-sizeof(Geo_Interception_Point_List),_sys_system_modules::FPL_SYS);//count the memory
}
//_________
//public
//Set the random variables of the result members
void Fpl_Mech_Erosion_Dune_van_Gent::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->resistance.set_input(id, section_id,fpl_label::res_waterside_erosion, true, ptr_database, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune, 0, false);
		this->resistance.set_distribution_types(ptr_database);

		this->stress.set_input(id, section_id,fpl_label::stress_waterside_erosion, true, ptr_database, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune, 0, false);
		this->stress.set_distribution_types(ptr_database);
	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per database table
void Fpl_Mech_Erosion_Dune_van_Gent::set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->critical_crest_width.set_input(id, section_id,fpl_label::crit_crest_width, false, ptr_database, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune, 0);
		this->critical_crest_width.set_distribution_types(ptr_database);

		this->mean_diameter.set_input(id, section_id,fpl_label::dia_mean, false, ptr_database, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune, 0);
		this->mean_diameter.set_distribution_types(ptr_database);

		this->roundness_fac.set_input(id, section_id,fpl_label::roundness_fac, false, ptr_database, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune, 0);
		this->roundness_fac.set_distribution_types(ptr_database);

		this->csf.set_input(id, section_id,fpl_label::shape_fac, false, ptr_database, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune, 0);
		this->csf.set_distribution_types(ptr_database);

		this->grain_density.set_input(id, section_id,fpl_label::grain_density, false, ptr_database, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune, 0);
		this->grain_density.set_distribution_types(ptr_database);

		this->settling_velocity.set_input(id, section_id,fpl_label::settling_v, true, ptr_database, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune, 0);
		this->settling_velocity.set_distribution_types(ptr_database);
	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per file
void Fpl_Mech_Erosion_Dune_van_Gent::set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start){
	ostringstream prefix;
	prefix << "WATERSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Input variable members per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	try{
		//variables
		this->critical_crest_width.set_input(ifile, line_counter,fpl_label::crit_crest_width, false, fpl_label::mech_erosion_waterside_gent);
		this->critical_crest_width.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->mean_diameter.set_input(ifile, line_counter,fpl_label::dia_mean, false, fpl_label::mech_erosion_waterside_gent);
		this->mean_diameter.check_positiv_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->roundness_fac.set_input(ifile, line_counter,fpl_label::roundness_fac, false, fpl_label::mech_erosion_waterside_gent);
		this->roundness_fac.check_positiv_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->csf.set_input(ifile, line_counter,fpl_label::shape_fac, false, fpl_label::mech_erosion_waterside_gent);
		this->csf.check_positiv_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->grain_density.set_input(ifile, line_counter,fpl_label::grain_density, false, fpl_label::mech_erosion_waterside_gent);
		this->grain_density.check_positiv_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->settling_velocity.set_input(ifile, line_counter,fpl_label::settling_v, true, fpl_label::mech_erosion_waterside_gent);
		this->settling_velocity.check_positiv_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		//results
		this->resistance.set_input(ifile, line_counter,fpl_label::res_waterside_erosion, true, fpl_label::mech_erosion_waterside_gent);
		this->resistance.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->stress.set_input(ifile, line_counter,fpl_label::stress_waterside_erosion, true, fpl_label::mech_erosion_waterside_gent);
		this->stress.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;
	}
	catch(Error msg){
		Sys_Common_Output::output_fpl->rewind_userprefix();
		throw msg;
	}

	cout << "Input of variable members per file is finsihed" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Transfer the bursting-mechanism (DIN 19712) data to database, e.g. geometry, random variables etc.
void Fpl_Mech_Erosion_Dune_van_Gent::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	ostringstream prefix;
	ostringstream cout;
	prefix << "WATERSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	//variable members
	cout << "Transfer variable members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->critical_crest_width.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune);
	this->mean_diameter.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune);
	this->roundness_fac.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune);
	this->csf.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune);
	this->grain_density.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune);
	this->settling_velocity.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune);

	//result members
	cout << "Transfer result members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->resistance.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune);
	this->stress.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_waterside_gent, fpl_label::sec_dune);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output members of the mechanism to display/console
void Fpl_Mech_Erosion_Dune_van_Gent::output_members(void){
	ostringstream prefix;
	prefix << "WATERSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism erosion of the waterside slope..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->critical_crest_width.output_member();
	this->mean_diameter.output_member();
	this->roundness_fac.output_member();
	this->csf.output_member();
	this->grain_density.output_member();
	this->settling_velocity.output_member();

	cout << "Result members mechanism erosion of the waterside slope..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_member();
	this->stress.output_member();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Check the statistic of the random variables of the mechanism
void Fpl_Mech_Erosion_Dune_van_Gent::check_statistic(void){
	this->critical_crest_width.check_statistic();
	this->mean_diameter.check_statistic();
	this->roundness_fac.check_statistic();
	this->csf.check_statistic();
	this->grain_density.check_statistic();
	this->settling_velocity.check_statistic();

	this->resistance.check_statistic();
	this->stress.check_statistic();
}
//Reset the statistic important values
void Fpl_Mech_Erosion_Dune_van_Gent::reset_statistics(void){
	this->critical_crest_width.reset_statistics();
	this->mean_diameter.reset_statistics();
	this->roundness_fac.reset_statistics();
	this->csf.reset_statistics();
	this->grain_density.reset_statistics();
	this->settling_velocity.reset_statistics();

	this->resistance.reset_statistics();
	this->stress.reset_statistics();

	this->reset_counter_single_prob();
	this->reset_counter_tree_prob();
}
//Output the statistics of the random variables of the mechanism to display/console
void Fpl_Mech_Erosion_Dune_van_Gent::output_statistic(void){
	ostringstream prefix;
	prefix << "WATERSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "General"<<endl;
	cout << " End point inside old profile       : " << this->no_point_inside<< endl;
	cout << " Foreland to short for new profile  : " << this->no_foreland_short<< endl;

	cout << "Variable members mechanism erosion of the waterside slope..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->critical_crest_width.output_statistic();
	this->mean_diameter.output_statistic();
	this->roundness_fac.output_statistic();
	this->csf.output_statistic();
	this->grain_density.output_statistic();
	this->settling_velocity.output_statistic();

	cout << "Result members mechanism erosion of the waterside slope..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_statistic();
	this->stress.output_statistic();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the results of a deterministic calculation to display/console
void Fpl_Mech_Erosion_Dune_van_Gent::output_determ_results(void){
	ostringstream prefix;
	prefix << "WATERSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "General"<<endl;
	if(this->wave2zero==false){
		cout << " End point inside old profile       : " << this->no_point_inside<< endl;
		cout << " Foreland to short for new profile  : " << this->no_foreland_short<< endl;
		cout << "Deterministic results mechanism erosion of the waterside slope..."<< endl;
		cout << " Critical crest width           : "<< this->critical_crest_width.get_result().end_result<<label::m << endl;
		cout << " Existing crest width           : "<< this->ptr_land_crest->get_xcoordinate()-this->ptr_water_crest->get_xcoordinate()<<label::m<<endl;
		cout << " Mean particle diameter         : "<< this->mean_diameter.get_result().end_result<<label::m << endl;
		cout << " Roundness factor of particle   : "<< this->roundness_fac.get_result().end_result<<label::no_unit << endl;
		cout << " Shape factor of particle (CSF) : "<< this->csf.get_result().end_result<<label::no_unit << endl;
		cout << " Grain density                  : "<< this->grain_density.get_result().end_result<<label::kg_per_qm << endl;
		cout << " Settling velocity of particle  : "<< this->settling_velocity.get_result().end_result<<label::m_per_sec << endl;

		Sys_Common_Output::output_fpl->output_txt(&cout,false);
		cout << "Result members mechanism erosion of the waterside slope..." << endl;
		cout << " Resistance                    : "<< this->resistance.get_result().end_result<<label::squaremeter<<endl;
		cout << " Stress                        : "<< this->stress.get_result().end_result<<label::squaremeter<<endl;
	}
	else{
		cout << " Wave height is reduced to zero due to the wave direction!" << endl;
	}
	
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the new profile shape to tecplot
void Fpl_Mech_Erosion_Dune_van_Gent::output_results2tecplot(const string new_profile){
	//open the file
	ofstream output;
	output.open(new_profile.c_str());
	if(output.is_open()==false){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Filename : " << new_profile << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	output << P(4) << FORMAT_FIXED_REAL;

	//output the file header
	output << "TITLE = "<< "\" New profile of dune in file " << new_profile << " ";
	output  << " \"" << endl;
	output << "VARIABLES = " <<endl;
	output << "\" x " << label::m << "\" "<<endl;
	output << "\" z " << label::m << "\" "<< endl;
	output <<endl<<endl<<endl;


	//search raster
	output << endl << "ZONE T= "<< "\" New dune profile after wave attack ";
	output << "\" "<< " I = " << this->new_shape_poly.get_number_points()-2 << endl;
	

	//output points
	double x_coor=0.0;
	double y_coor=0.0;


	for(int i=1; i< this->new_shape_poly.get_number_points()-1; i++){
		x_coor=this->new_shape_poly.get_point(i)->get_xcoordinate();
		y_coor=this->new_shape_poly.get_point(i)->get_ycoordinate();
		output << W(12) << x_coor << W(12) << y_coor <<  endl;
		output.flush();
	}
	
	output << endl << "ZONE T= "<< "\" Waterlevel ";
	output << "\" "<< " I = " << 2 << endl;
	x_coor=this->new_shape_poly.get_point(1)->get_xcoordinate();
	y_coor=this->one2one_line.point1.get_ycoordinate();
	output << W(12) << x_coor << W(12) << y_coor <<  endl;
	x_coor=this->one2one_line.point1.get_xcoordinate();
	y_coor=this->one2one_line.point1.get_ycoordinate();
	output << W(12) << x_coor << W(12) << y_coor <<  endl;


	//close the file
	output.close();
}
//Output the new profile shape to paraview
void Fpl_Mech_Erosion_Dune_van_Gent::output_results2paraview(const string new_profile) {
	//open the file
	ofstream output;
	output.open(new_profile.c_str());
	if (output.is_open() == false) {
		Error msg = this->set_error(3);
		ostringstream info;
		info << "Filename : " << new_profile << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	output << P(4) << FORMAT_FIXED_REAL;
	//header
	output <<"x,";
	int count_col_tot = 1;
	output << "z_new_profile,";
	count_col_tot++;
	output << "waterlevel";
	count_col_tot++;

	output << endl;
	output.flush();
	int counter_col_before = 0;
	int counter_col_after = count_col_tot - 1;

	//output points
	double x_coor = 0.0;
	double y_coor = 0.0;

	counter_col_after--;
	for (int i = 1; i < this->new_shape_poly.get_number_points() - 1; i++) {
		x_coor = this->new_shape_poly.get_point(i)->get_xcoordinate();
		y_coor = this->new_shape_poly.get_point(i)->get_ycoordinate();
		output << x_coor << ",";
		functions::add_seperator_csv("NAN,", &output, counter_col_before);
		output << y_coor;
		functions::add_seperator_csv(",NAN", &output, counter_col_after);
		output << endl;

		output.flush();
	}
	counter_col_before++;

	counter_col_after--;
	//output waterlevel
	x_coor = this->new_shape_poly.get_point(1)->get_xcoordinate();
	y_coor = this->one2one_line.point1.get_ycoordinate();
	output << x_coor << ",";
	functions::add_seperator_csv("NAN,", &output, counter_col_before);
	output << y_coor;
	functions::add_seperator_csv(",NAN", &output, counter_col_after);
	output << endl;

	x_coor = this->one2one_line.point1.get_xcoordinate();
	y_coor = this->one2one_line.point1.get_ycoordinate();
	output << x_coor << ",";
	functions::add_seperator_csv("NAN,", &output, counter_col_before);
	output << y_coor;
	functions::add_seperator_csv("NAN,", &output, counter_col_after);
	output << endl;
	counter_col_before++;


	//close the file
	output.close();

}
//Output the new profile shape to excel
void Fpl_Mech_Erosion_Dune_van_Gent::output_results2excel(const string new_profile) {
	//open the file
	ofstream output;
	output.open(new_profile.c_str());
	if (output.is_open() == false) {
		Error msg = this->set_error(4);
		ostringstream info;
		info << "Filename : " << new_profile << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	output << P(4) << FORMAT_FIXED_REAL;
	//header
	output << "x;";
	int count_col_tot = 1;
	output << "z_new_profile;";
	count_col_tot++;
	output << "waterlevel";
	count_col_tot++;

	output << endl;
	output.flush();
	int counter_col_before = 0;
	int counter_col_after = count_col_tot - 1;

	//output points
	double x_coor = 0.0;
	double y_coor = 0.0;

	counter_col_after--;
	for (int i = 1; i < this->new_shape_poly.get_number_points() - 1; i++) {
		x_coor = this->new_shape_poly.get_point(i)->get_xcoordinate();
		y_coor = this->new_shape_poly.get_point(i)->get_ycoordinate();
		output << x_coor << ";";
		functions::add_seperator_csv(";", &output, counter_col_before);
		output << y_coor;
		functions::add_seperator_csv(";", &output, counter_col_after);
		output << endl;

		output.flush();
	}
	counter_col_before++;

	counter_col_after--;
	//output waterlevel
	x_coor = this->new_shape_poly.get_point(1)->get_xcoordinate();
	y_coor = this->one2one_line.point1.get_ycoordinate();
	output << x_coor << ";";
	functions::add_seperator_csv(";", &output, counter_col_before);
	output << y_coor;
	functions::add_seperator_csv(";", &output, counter_col_after);
	output << endl;

	x_coor = this->one2one_line.point1.get_xcoordinate();
	y_coor = this->one2one_line.point1.get_ycoordinate();
	output << x_coor << ";";
	functions::add_seperator_csv(";", &output, counter_col_before);
	output << y_coor;
	functions::add_seperator_csv(";", &output, counter_col_after);
	output << endl;
	counter_col_before++;


	//close the file
	output.close();
}
//Calculate the mechanism
bool Fpl_Mech_Erosion_Dune_van_Gent::calculate_mechanism(const bool random_calculation, const double waterlevel, double wave_height, double wave_period, const double water_density, const double section_direction, const double wave_direction){
	this->failure=0;
	this->reset_random_flag();

	this->buff_stress=0.0;
	this->buff_res=0.0;

	this->calculate_wavedirection_reduction(wave_direction,section_direction, &wave_height, &wave_period);
	if(waterlevel<=0.0 || wave_period<=0.0 || wave_height<=0.0){
		this->failure=0;
		return this->failure;
	}
	double buff_waterlevel=waterlevel;
	if(waterlevel>=this->ptr_water_crest->get_ycoordinate()){
		buff_waterlevel=this->ptr_water_crest->get_ycoordinate()-constant::meter_epsilon*2.0;
	}

	//calculate the settling velocity of the particle
	this->calculate_settling_velocity(random_calculation, water_density);

	//critical crest width
	this->critical_crest_width.calculate_variables(random_calculation);
	//check crest width
	if((this->ptr_land_crest->get_xcoordinate()-this->critical_crest_width.get_result().end_result)<=this->ptr_water_crest->get_xcoordinate()){
		this->failure=1;

		//set the stress and resistance
		this->resistance.set_new_middle(0.0);
		this->resistance.calculate_variables(random_calculation);
		this->stress.set_new_middle(*this->ptr_area_cubature);
		this->stress.calculate_variables(random_calculation);

		return this->failure;
	}
	if(this->critical_crest_width.get_result().end_result<=0.0){
		this->failure=0;

		//set the stress and resistance
		this->resistance.set_new_middle(*this->ptr_area_cubature);
		this->resistance.calculate_variables(random_calculation);
		this->stress.set_new_middle(0.0);
		this->stress.calculate_variables(random_calculation);


		return this->failure;
	}

	

	//set the geometry
	this->waterlevel_line.set_coordinates(-0.0, buff_waterlevel);
	this->vertical_line.set_coordinates(this->ptr_land_crest->get_xcoordinate()-this->critical_crest_width.get_result().end_result);

	this->calculate_new_dune_profile(random_calculation, wave_height, wave_period);

	//compare stress and resistance
	if(this->resistance.get_result().end_result>=this->stress.get_result().end_result){
		this->failure=0;
	}
	else{
		this->failure=1;
	}

	return this->failure;
}
//Set the pointer to the area of the outer dune polygon and the pointer to the outer polygon itself
void Fpl_Mech_Erosion_Dune_van_Gent::set_ptr_area_poly_outer(double *area, Geo_Simple_Polygon *poly){
	this->ptr_area_out=area;
	this->ptr_out_poly=poly;

}
//Set the pointer to the crest points
void Fpl_Mech_Erosion_Dune_van_Gent::set_ptr_crest_points(Geo_Point *land_crest, Geo_Point *water_crest){
	this->ptr_land_crest=land_crest;
	this->ptr_water_crest=water_crest;
}
//Set pointer to the foreland, the waterside cubature and the crest cubature
void Fpl_Mech_Erosion_Dune_van_Gent::set_ptr_cubature(Fpl_Dike_Geo_Foreland *foreland, Fpl_Cub_Dike_Waterside *waterside, Fpl_Cub_Dike_Crest *crest, double *area){
	this->ptr_foreland=foreland;
	this->ptr_waterside=waterside;
	this->ptr_crest=crest;
	this->ptr_area_cubature=area;
}
//Initialize the polygon for the new shape of the dune after wave attack (use it after the geometric pointers are set)
void Fpl_Mech_Erosion_Dune_van_Gent::init_new_shape_poly(void){
	Geo_Point_List list_buff;
	bool crest_end_found=false;
	int counter=0;

	this->number_new_points=this->ptr_foreland->get_number_segments()+this->ptr_waterside->get_number_segments();
	this->number_new_points=this->number_new_points+(int)(this->ptr_water_crest->get_ycoordinate()/this->delta_x);
	//set a boundary
	if(this->number_new_points>20000){
		this->number_new_points=20000;
	}

	for(int i=0; i<this->ptr_out_poly->get_number_points(); i++){
		//check for land crest points
		if(*(this->ptr_out_poly->get_point(i))==*this->ptr_land_crest){
			crest_end_found=true;
			//set new points
			double diff=this->ptr_land_crest->get_xcoordinate()-list_buff.get_last_point().get_xcoordinate();

			diff=(diff/(double)(this->number_new_points+1));
			Geo_Point buff_point;
			for(int j=0; j< this->number_new_points; j++){
				buff_point.set_point_coordinate(list_buff.get_last_point().get_xcoordinate()+diff,list_buff.get_last_point().get_ycoordinate());
				list_buff.set_new_point(&buff_point);
				counter++;
			}
		}
		//set the first to points of the given polygon
		if(i<=1 && crest_end_found==false){
			list_buff.set_new_point(this->ptr_out_poly->get_point(i));
			counter++;
		}
		//set points of the given polygon
		else if(crest_end_found == true){
			list_buff.set_new_point(this->ptr_out_poly->get_point(i));
			counter++;
		}
	}
	this->new_shape_poly.set_number_points(list_buff.get_number_points());
	this->new_shape_poly.set_points(&list_buff);
}
//__________
//private
//Reset the random generated flags of the random variables
void Fpl_Mech_Erosion_Dune_van_Gent::reset_random_flag(void){
	this->critical_crest_width.reset_random_generated();
	this->mean_diameter.reset_random_generated();
	this->roundness_fac.reset_random_generated();
	this->csf.reset_random_generated();
	this->grain_density.reset_random_generated();
	this->settling_velocity.reset_random_generated();

	this->resistance.reset_random_generated();
	this->stress.reset_random_generated();
}
//Calculate the settling veleocity of the sand material
void Fpl_Mech_Erosion_Dune_van_Gent::calculate_settling_velocity(const bool random_calculation, const double water_density){
	double a_value=0.0;
	double b_value=0.0;
	double dn=0.0;
	double s_star=0.0;
	double s=0.0;
	double v=0.0;

	//calculate a_ and b_value
	this->csf.calculate_variables(random_calculation);
	this->roundness_fac.calculate_variables(random_calculation);
	a_value=this->interpolate_matrix_value(this->csf.get_result().end_result, this->roundness_fac.get_result().end_result,this->a_matrix);
	b_value=this->interpolate_matrix_value(this->csf.get_result().end_result, this->roundness_fac.get_result().end_result,this->b_matrix);

	//calculate dn
	this->mean_diameter.calculate_variables(random_calculation);
	dn=this->mean_diameter.get_result().end_result/0.9;

	//calculate s
	this->grain_density.calculate_variables(random_calculation);
	s=this->grain_density.get_result().end_result/water_density;


	//calculate s_star
	if(s>=1.0){
		s_star=dn*pow(constant::Cgg*dn*(s-1.0),0.5)/(4.0*constant::Cvisco_water);
	}
	else{
		v=0.0;
	}

	

	//calculate velocity
	if(s>=1.0 && s_star>0.0){
		v=(1.0/(a_value+b_value/s_star))*pow((s-1.0)*constant::Cgg*dn,0.5);
		
	}
	else{
		v=0.0;
	}

	//make random of v
	this->settling_velocity.set_new_middle(v);
	this->settling_velocity.calculate_variables(random_calculation);
}
//Initiate the A-matrix and B-matrix
void Fpl_Mech_Erosion_Dune_van_Gent::init_matrix(void){
	//column values (roundness)
	this->matrix_col[0]=2.0;
	this->matrix_col[1]=3.5;
	this->matrix_col[3]=6.0;
	//row values (csf)
	this->matrix_row[0]=0.4;
	this->matrix_row[1]=0.5;
	this->matrix_row[2]=0.6;
	this->matrix_row[3]=0.7;
	this->matrix_row[4]=0.8;
	this->matrix_row[5]=0.9;

	//matrix values A-value
	this->a_matrix[0][0]=1.197;
	this->a_matrix[1][0]=1.129;
	this->a_matrix[2][0]=1.02;
	this->a_matrix[0][1]=1.11;
	this->a_matrix[1][1]=1.05;
	this->a_matrix[2][1]=0.96;
	this->a_matrix[0][2]=1.045;
	this->a_matrix[1][2]=1.0;
	this->a_matrix[2][2]=0.922;
	this->a_matrix[0][3]=0.995;
	this->a_matrix[1][3]=0.954;
	this->a_matrix[2][3]=0.89;
	this->a_matrix[0][4]=0.945;
	this->a_matrix[1][4]=0.918;
	this->a_matrix[2][4]=0.848;
	this->a_matrix[0][5]=0.9;
	this->a_matrix[1][5]=0.87;
	this->a_matrix[2][5]=0.829;

	//matrix values B-value
	this->b_matrix[0][0]=6.25;
	this->b_matrix[1][0]=6.0;
	this->b_matrix[2][0]=5.58;
	this->b_matrix[0][1]=5.75;
	this->b_matrix[1][1]=5.55;
	this->b_matrix[2][1]=5.25;
	this->b_matrix[0][2]=5.45;
	this->b_matrix[1][2]=5.3;
	this->b_matrix[2][2]=5.05;
	this->b_matrix[0][3]=5.211;
	this->b_matrix[1][3]=5.121;
	this->b_matrix[2][3]=4.974;
	this->b_matrix[0][4]=5.0;
	this->b_matrix[1][4]=4.95;
	this->b_matrix[2][4]=4.89;
	this->b_matrix[0][5]=4.78;
	this->b_matrix[1][5]=4.78;
	this->b_matrix[2][5]=4.78;
}
//Interpolate value
double Fpl_Mech_Erosion_Dune_van_Gent::interpolate_matrix_value(const double csf, const double roundness, double mat[3][6]){
	double value_up=0.0;
	double value_down=0.0;
	double value=0.0;
	int index_col_up=3;
	int index_row_up=6;
	int index_col_down=2;
	int index_row_down=5;

	//find index
	for(int i=0; i<3; i++){
		if(roundness<=this->matrix_col[i]){
			index_col_up=i;
			index_col_down=i-1;
			break;
		}
	}
	for(int i=0; i<6; i++){
		if(csf<=this->matrix_row[i]){
			index_row_up=i;
			index_row_down=i-1;
			break;
		}
	}

	//interpolate
	if(index_row_up>0 && index_row_up<6){
		//interpolate
		if(index_col_up>0 && index_col_up<3){
			value_down=mat[index_col_down][index_row_down]+((mat[index_col_down][index_row_up]-mat[index_col_down][index_row_down])/(this->matrix_row[index_row_up]-this->matrix_row[index_row_down]))*(csf-this->matrix_row[index_row_down]);
			value_up=mat[index_col_up][index_row_down]+((mat[index_col_up][index_row_up]-mat[index_col_up][index_row_down])/(this->matrix_row[index_row_up]-this->matrix_row[index_row_down]))*(csf-this->matrix_row[index_row_down]);
			value=value_down+((value_up-value_down)/(this->matrix_col[index_col_up]-this->matrix_col[index_col_down]))*(roundness-this->matrix_col[index_col_down]);
		}
		else if(index_col_up==3){
			value_down=mat[index_col_down][index_row_down]+((mat[index_col_down][index_row_up]-mat[index_col_down][index_row_down])/(this->matrix_row[index_row_up]-this->matrix_row[index_row_down]))*(csf-this->matrix_row[index_row_down]);
			value=value_down;
		}
		else if(index_col_up==0){
			value_up=mat[index_col_up][index_row_down]+((mat[index_col_up][index_row_up]-mat[index_col_up][index_row_down])/(this->matrix_row[index_row_up]-this->matrix_row[index_row_down]))*(csf-this->matrix_row[index_row_down]);
			value=value_up;
		}
	}
	//extrapolate
	else if(index_row_up==6){
		if(index_col_up>0 && index_col_up<3){
			value_down=mat[index_col_down][index_row_down];
			value_up=mat[index_col_up][index_row_down];
			value=value_down+((value_up-value_down)/(this->matrix_col[index_col_up]-this->matrix_col[index_col_down]))*(roundness-this->matrix_col[index_col_down]);
		}
		else if(index_col_up==3){
			value=mat[index_col_down][index_row_down];
		}
		else if(index_col_up==0){
			value=mat[index_col_up][index_row_down];
		}
	}
	//extrapolate
	else if(index_row_up==0){
		if(index_col_up>0 && index_col_up<3){
			value_down=mat[index_col_down][index_row_up];
			value_up=mat[index_col_up][index_row_up];
			value=value_down+((value_up-value_down)/(this->matrix_col[index_col_up]-this->matrix_col[index_col_down]))*(roundness-this->matrix_col[index_col_down]);
		}
		else if(index_col_up==3){
			value=mat[index_col_down][index_row_up];
		}
		else if(index_col_up==0){
			value=mat[index_col_up][index_row_up];;
		}
	}

	return value;
}
//Calculate new dune profile
void Fpl_Mech_Erosion_Dune_van_Gent::calculate_new_dune_profile(const bool random_calculation,  const double wave_height, const double wave_period){
	double outside_area=0.0;
	double inside_area=0.0;
	double buff_y=0.0;
	double buff_x=0.0;
	bool point_inside=false;

	_geo_interception_point buff_int;
	//interception with crest
	this->ptr_crest->get_segment()->calc_interception(&this->vertical_line, &buff_int);
	if(buff_int.interception_flag==false){
		//Error no interception found
		Error msg=this->set_error(1);
		stringstream info;
		info << "No interception found between crest and vertical line" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//set second point (start) to 1:1 line
	this->one2one_line.set_coordinates(buff_int.interception_point.get_xcoordinate()-1.0, buff_int.interception_point.get_ycoordinate()-1.0, buff_int.interception_point.get_xcoordinate(), buff_int.interception_point.get_ycoordinate());
	
	//interception one2one with waterlevel line
	this->one2one_line.calc_interception(&this->waterlevel_line, &buff_int);
	if(buff_int.interception_flag==false){
		//Error no interception found
		Error msg=this->set_error(1);
		stringstream info;
		info << "No interception found between 1 : 1 line and waterlevel" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//set first point q to one2one line
	this->one2one_line.point1.set_point_coordinate(buff_int.interception_point.get_xcoordinate(), buff_int.interception_point.get_ycoordinate());

	//calculate the r point via the parabel formula of van Gent
	buff_x=250.0*pow((wave_height/7.6),1.28)*pow((0.0268/this->settling_velocity.get_result().end_result),0.56);
	buff_x=this->one2one_line.point1.get_xcoordinate()-buff_x;

	buff_y=(wave_height/7.6)*(0.4714*pow((250.0*pow((10.9/wave_period),0.45)+18.0),0.5)-2.0);
	buff_y=this->one2one_line.point1.get_ycoordinate()-buff_y;

	//set second point r of 1:12.5 line
	this->one2twelve_line.set_coordinates(buff_x-125000.0, buff_y-10000.0, buff_x, buff_y);

	if(this->ptr_out_poly->check_point_inside(&this->one2twelve_line.point2)==true){
		point_inside=true;
		this->no_point_inside++;
		if(this->one2twelve_line.point2.get_xcoordinate()<0.0 && this->one2twelve_line.point2.get_ycoordinate()<0.0){
			//calculation of interception point
			double c1=18.0*pow(wave_height*0.4714/7.6,2.0);
			double c4=2.0*wave_height/7.6+this->one2one_line.point1.get_ycoordinate();
			double c3=this->one2one_line.point1.get_xcoordinate();
			double c2=pow(wave_height*0.4714/7.6,2.0)*(pow((7.6/wave_height),1.28)*pow((10.9/wave_period),0.45)*pow((this->settling_velocity.get_result().end_result/0.0268),0.56));
			double test=-pow(c1-(this->one2one_line.point1.get_xcoordinate()-c3)*c2,0.5)+c4;
			test=-pow(c1-(-45.73-c3)*c2,0.5)+c4;
			
			Geo_Point_List buffer;
			for(int i=this->ptr_foreland->get_number_segments()-1; i>=0; i--){
				this->ptr_foreland->get_segment(i)->calc_interception_square_root(&buffer,c1,c2,c3,c4);
				if(buffer.get_number_points()>0){
					break;
				}

			}
			
			if(buffer.get_number_points()<=0){			
				//new calculation of r-point
				buff_y=(this->one2one_line.point1.get_ycoordinate());
				buff_x=(pow((2.12134*(2.0+(7.6/wave_height)*buff_y)),2.0)-18.0)*(1.0/(pow((7.6/wave_height),1.28)*pow((10.9/wave_period),0.45)*pow((this->settling_velocity.get_result().end_result/0.0268),0.56)));
				buff_x=this->one2one_line.point1.get_xcoordinate()-buff_x;
				buff_y=this->one2one_line.point1.get_ycoordinate()-buff_y;
				this->one2twelve_line.set_coordinates(buff_x-125000.0, buff_y-10000.0, buff_x, buff_y);
			}
			else{
				buff_y=(this->one2one_line.point1.get_ycoordinate())-buffer.get_first_point().get_ycoordinate()+0.01;
				buff_x=(pow((2.12134*(2.0+(7.6/wave_height)*buff_y)),2.0)-18.0)*(1.0/(pow((7.6/wave_height),1.28)*pow((10.9/wave_period),0.45)*pow((this->settling_velocity.get_result().end_result/0.0268),0.56)));
				buff_x=this->one2one_line.point1.get_xcoordinate()-buff_x;
				buff_y=this->one2one_line.point1.get_ycoordinate()-buff_y;
				this->one2twelve_line.set_coordinates(buff_x-125000.0, buff_y-10000.0, buff_x, buff_y);


			}
		}
	}


	bool intercept_found=false;
	int id_foreland_buff=-1;
	//check interception 1:12.5 with foreland
	for(int i=0; i< this->ptr_foreland->get_number_segments(); i++){
		this->ptr_foreland->get_segment(i)->calc_interception(&this->one2twelve_line, &buff_int);
		if(buff_int.interception_flag==true){
			intercept_found=true;
			//set the point
			id_foreland_buff=i;
			this->one2twelve_line.point1.set_point_coordinate(buff_int.interception_point.get_xcoordinate(), buff_int.interception_point.get_ycoordinate());
			break;
		}
	}
	int id_waterside_buff=-1;
	//check interception with waterside
	if(intercept_found==false){
		for(int i=0; i< this->ptr_waterside->get_number_segments(); i++){
			this->ptr_waterside->get_segment(i)->calc_interception(&this->one2twelve_line, &buff_int);
			if(buff_int.interception_flag==true){
				intercept_found=true;
				//set the point
				id_waterside_buff=i;
				this->one2twelve_line.point1.set_point_coordinate(buff_int.interception_point.get_xcoordinate(), buff_int.interception_point.get_ycoordinate());
				break;
			}
		}
	}
	//cut at the end
	if(intercept_found==false){
		if(point_inside==false){
			////change gradient to 1:1 
			//this->one2twelve_line.point1.set_point_coordinate(this->one2twelve_line.point2.get_xcoordinate()-10000.0, this->one2twelve_line.point2.get_ycoordinate()-10000.0);
			//this->one2twelve_line.refresh_distances();
			////check again the interception
			//for(int i=0; i< this->ptr_foreland->get_number_segments(); i++){
			//	this->ptr_foreland->get_segment(i)->calc_interception(&this->one2twelve_line, &buff_int);
			//	if(buff_int.interception_flag==true){
			//		intercept_found=true;
			//		//set the point
			//		id_foreland_buff=i;
			//		this->one2twelve_line.point1.set_point_coordinate(buff_int.interception_point.get_xcoordinate(), buff_int.interception_point.get_ycoordinate());
			//		break;
			//	}
			//}

			//if(intercept_found==false){
			//	//Error no interception found
			//	Error msg=this->set_error(1);
			//	stringstream info;
			//	info << "No interception found between 1 : 12.5 (now 1:1) line and foreland" << endl;
			//	msg.make_second_info(info.str());
			//	throw msg;
			//}
			double diff=0.0;
			diff=this->one2one_line.point1.get_xcoordinate()-this->ptr_foreland->get_ptr_first_point()->get_xcoordinate();
			diff=(wave_height/7.6)*0.4714*pow((pow((7.6/wave_height),1.28)*pow((10.9/wave_period),0.45)*pow((this->settling_velocity.get_result().end_result/0.0268),0.56)*diff+18.0),0.5)-2.0;
			diff=this->one2one_line.point1.get_ycoordinate()-diff;
			this->one2twelve_line.set_coordinates(this->ptr_foreland->get_ptr_first_point()->get_xcoordinate(), this->ptr_foreland->get_ptr_first_point()->get_ycoordinate()+constant::meter_epsilon*2.0, this->ptr_foreland->get_ptr_first_point()->get_xcoordinate(), diff);
	
			this->no_foreland_short++;
			if(diff<this->ptr_foreland->get_ptr_first_point()->get_ycoordinate()){
				this->one2twelve_line.set_coordinates(this->ptr_foreland->get_ptr_first_point()->get_xcoordinate()+constant::meter_epsilon*2.0, this->ptr_foreland->get_ptr_first_point()->get_ycoordinate()-constant::meter_epsilon*2.0, this->ptr_foreland->get_ptr_first_point()->get_xcoordinate()+constant::meter_epsilon*2.0, diff);
			}
		}
		else{
			if(this->one2twelve_line.point2.get_xcoordinate()>=0.0 && this->one2twelve_line.point2.get_ycoordinate()>=0.0){
				this->one2twelve_line.set_coordinates(0.000, 0.000, this->one2twelve_line.point2.get_xcoordinate(), this->one2twelve_line.point2.get_ycoordinate());
				id_foreland_buff=this->ptr_foreland->get_number_segments()-1;
			}
			else if(this->one2twelve_line.point2.get_xcoordinate()>0.0 && this->one2twelve_line.point2.get_ycoordinate()<0.0){
				this->one2twelve_line.set_coordinates(0.000, 0.000, this->one2twelve_line.point2.get_xcoordinate(), this->one2twelve_line.point2.get_ycoordinate());
				id_foreland_buff=this->ptr_foreland->get_number_segments()-1;
			}
			else if(this->one2twelve_line.point2.get_xcoordinate()<0.0 && this->one2twelve_line.point2.get_ycoordinate()<0.0){
				this->one2twelve_line.set_coordinates(this->ptr_foreland->get_ptr_first_point()->get_xcoordinate()+constant::meter_epsilon*2.0, this->ptr_foreland->get_ptr_first_point()->get_ycoordinate(), this->one2twelve_line.point2.get_xcoordinate(), this->one2twelve_line.point2.get_ycoordinate());
			}



		}
	}

	//set new shape to polygon: first the not change points
	int counter=0;
	if(id_foreland_buff>0){
		//just part of the foreland
		for(int i=1; i<this->ptr_foreland->get_number_segments(); i++){
			if(i<=id_foreland_buff){
				this->new_shape_poly.change_point_coordinates(counter+2,this->ptr_foreland->get_segment(i)->point1.get_xcoordinate(),this->ptr_foreland->get_segment(i)->point1.get_ycoordinate());
				counter++;
			}
			else{
				break;
			}
		}
	}
	else if(id_waterside_buff>0){
		//complete foreland
		for(int i=1; i<this->ptr_foreland->get_number_segments(); i++){
			this->new_shape_poly.change_point_coordinates(counter+2,this->ptr_foreland->get_segment(i)->point1.get_xcoordinate(),this->ptr_foreland->get_segment(i)->point1.get_ycoordinate());
			counter++;
		}
		//part of the waterside
		for(int i=0; i<this->ptr_waterside->get_number_segments(); i++){
			if(i<=id_waterside_buff){
				this->new_shape_poly.change_point_coordinates(counter+2,this->ptr_waterside->get_segment(i)->point1.get_xcoordinate(),this->ptr_waterside->get_segment(i)->point1.get_ycoordinate());
				counter++;
			}
			else{
				break;
			}
		}
	}

	//set the first points of the 1:12.5 line
	this->new_shape_poly.change_point_coordinates(counter+2,this->one2twelve_line.point1.get_xcoordinate(),this->one2twelve_line.point1.get_ycoordinate());
	counter++;
	this->new_shape_poly.change_point_coordinates(counter+2,this->one2twelve_line.point2.get_xcoordinate(),this->one2twelve_line.point2.get_ycoordinate());
	counter++;

	//caluclate point difference
	double diff=0.0;
	double step=0.0;
	diff=this->one2one_line.point1.get_xcoordinate()-this->one2twelve_line.point2.get_xcoordinate();
	step=diff/(double)(this->number_new_points-(counter+2-1));
	double x_buff=0.0;
	double y_buff=0.0;
	double counter_do=1.0;

	//set new shape to polygon for the new parabel shaped cubatur after van Gent 2008
	do{
		//calculate local coordinate
		x_buff=diff-step*counter_do;
		y_buff=(wave_height/7.6)*(0.4714*pow((pow((7.6/wave_height),1.28)*pow((10.9/wave_period),0.45)*pow((this->settling_velocity.get_result().end_result/0.0268),0.56)*x_buff+18.0),0.5)-2.0);
		//transfer to global coordinate
		x_buff=this->one2one_line.point1.get_xcoordinate()-x_buff;
		y_buff=this->one2one_line.point1.get_ycoordinate()-y_buff;
		//set them to the polygon
		this->new_shape_poly.change_point_coordinates(counter+2,x_buff, y_buff);
		counter++;
		counter_do=counter_do+1.0;
	}while(counter<this->number_new_points-2);

	//set the first points of the 1:12.5 line
	this->new_shape_poly.change_point_coordinates(counter+2,this->one2one_line.point1.get_xcoordinate(),this->one2one_line.point1.get_ycoordinate());
	counter++;
	this->new_shape_poly.change_point_coordinates(counter+2,this->one2one_line.point2.get_xcoordinate(),this->one2one_line.point2.get_ycoordinate());
	counter++;

	try{
		if(random_calculation==false){
		//first for code checking
			this->new_shape_poly.check_simple_polygon();
		}
	}
	catch(Error msg){
		ostringstream info;
		info<<"Set new dune profile shape"<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	double common_a=0.0;
	//calculate the area outside and inside
	this->poly1.reset_list();
	this->poly2.reset_list();
	this->point_list.reset_list();
	this->inter_point_list.reset_list();

	common_a=this->ptr_out_poly->polygon_interception(&this->new_shape_poly, &this->poly1, &this->poly2, &this->inter_point_list, &this->point_list, &this->inter_point);
	inside_area=*this->ptr_area_out-common_a;
	outside_area=this->new_shape_poly.calculate_area();
	outside_area=outside_area-common_a;

	//set the stress and resistance
	this->resistance.set_new_middle(inside_area);
	this->resistance.calculate_variables(random_calculation);
	this->stress.set_new_middle(outside_area);
	this->stress.calculate_variables(random_calculation);

	

}
//Set warning(s)
Warning Fpl_Mech_Erosion_Dune_van_Gent::set_warning(const int warn_type){
		string place="Fpl_Mech_Erosion_Dune_van_Gent::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no mechanism is set
			place.append("set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, Fpl_Dike_Var_Materialzone *zone, const int number)");
			reason="No material variable zone is found for the density of the impermeable layer";
			help="Check the random variable and the material variable zone";
			reaction="No connection can be established between the material variable zone and the random variable; the results can be wrong";
			type=20;
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
Error Fpl_Mech_Erosion_Dune_van_Gent::set_error(const int err_type){
	string place="Fpl_Mech_Erosion_Dune_van_Gent::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_bursting_slices(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://no interception found
			place.append("calculate_new_dune_profile(const bool random_calculation,  const double wave_height, const double wave_period)");
			reason="No interception is found";
			help="Check the geometry of the dune";
			type=11;
			break;
		case 2://could not open the tecplot file
			place.append("output_results2tecplot(const string new_profile)");
			reason="Could not open the file for the tecplot output (new dune profile results) of the FPL-section";
			help="Check the file";
			type=5;
			break;
		case 3://could not open the tecplot file
			place.append("output_results2paraview(const string new_profile)");
			reason = "Could not open the file for the paraview output (new dune profile results) of the FPL-section";
			help = "Check the file";
			type = 5;
			break;
		case 4://could not open the tecplot file
			place.append("output_results2excel(const string new_profile)");
			reason = "Could not open the file for the excel output (new dune profile results) of the FPL-section";
			help = "Check the file";
			type = 5;
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