#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Mech_Bursting_Din_Dike.h"

//Default constructor
Fpl_Mech_Bursting_Din_Dike::Fpl_Mech_Bursting_Din_Dike(void){

	this->fixed_leak_length=0.0;
	this->total_leak_length=0.0;
	this->failure=false;
	this->buff_stress=0.0;
	this->buff_res=0.0;
	this->impermeable_zone=NULL;

	this->variable_leak_length.set_unit(label::m);
	this->dens_imperm_layer.set_unit(label::kg_per_qm);
	this->resistance.set_unit(label::kn_per_sqm);
	this->stress.set_unit(label::kn_per_sqm);
	this->size_imperm_layer.set_unit(label::m);
	this->hydraulic_head_loss.set_unit(label::m_per_m);

	this->ptr_mat_zone=NULL;
	this->number_mat_zone=0;

	this->slices=NULL;
	this->number_slices=0;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Bursting_Din_Dike)-sizeof(Fpl_Random_Variables)*6,_sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Mech_Bursting_Din_Dike::~Fpl_Mech_Bursting_Din_Dike(void){

	this->delete_bursting_slices();

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mech_Bursting_Din_Dike)-sizeof(Fpl_Random_Variables)*6,_sys_system_modules::FPL_SYS);//count the memory
}
//_________
//public
//Set the random variables of the result members
void Fpl_Mech_Bursting_Din_Dike::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->resistance.set_input(id, section_id,fpl_label::res_burst_din, true, ptr_database, fpl_label::mech_bursting_din, fpl_label::sec_dike, 0, false);
		this->resistance.set_distribution_types(ptr_database);
		this->stress.set_input(id, section_id,fpl_label::stress_burst_din, true, ptr_database, fpl_label::mech_bursting_din, fpl_label::sec_dike, 0, false);
		this->stress.set_distribution_types(ptr_database);

	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per database table
void Fpl_Mech_Bursting_Din_Dike::set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, Fpl_Dike_Var_Materialzone *zone){
	try{
		this->variable_leak_length.set_input(id, section_id,fpl_label::var_leak_length, false, ptr_database, fpl_label::mech_bursting_din, fpl_label::sec_dike, 0, false);
		this->variable_leak_length.set_distribution_types(ptr_database);
		this->hydraulic_head_loss.set_input(id, section_id,fpl_label::hyd_head_loss, false, ptr_database, fpl_label::mech_bursting_din, fpl_label::sec_dike, 0, false);
		this->hydraulic_head_loss.set_distribution_types(ptr_database);
		

		if(zone==NULL){
			try{
				this->dens_imperm_layer.set_input(id, section_id,fpl_label::dens_imperm_lay, false, ptr_database, fpl_label::mech_bursting_din, fpl_label::sec_dike, -1, false);
				this->dens_imperm_layer.set_distribution_types(ptr_database);
				this->size_imperm_layer.set_input(id, section_id,fpl_label::size_imperm_lay, false, ptr_database, fpl_label::mech_bursting_din, fpl_label::sec_dike, 0, false);
				this->size_imperm_layer.set_distribution_types(ptr_database);
			}
			catch(Error msg){
				ostringstream info;
				info<<" No impermeable zone is found!"<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		else{
			this->impermeable_zone=zone;
			ostringstream cout;
			cout <<"The impermeable layer is connected to material variable zone "<<zone->get_zone_id()  << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout,false);
			this->size_imperm_layer.set_input(id, section_id,fpl_label::size_imperm_lay, true, ptr_database, fpl_label::mech_bursting_din, fpl_label::sec_dike, 0, false);
			this->size_imperm_layer.set_distribution_types(ptr_database);
		}
		
		
		

	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per file
void Fpl_Mech_Bursting_Din_Dike::set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start, Fpl_Dike_Var_Materialzone *zone){
	ostringstream prefix;
	prefix << "BURST_DIN> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Input variable members per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	
	try{
		this->variable_leak_length.set_input(ifile, line_counter,fpl_label::var_leak_length,false, fpl_label::end_mech_burst_din);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->hydraulic_head_loss.set_input(ifile, line_counter,fpl_label::hyd_head_loss, false, fpl_label::end_mech_burst_din);
		this->hydraulic_head_loss.check_given_boundaries(1.0,0.0);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;



		if(zone==NULL){
			try{
				this->size_imperm_layer.set_input(ifile, line_counter,fpl_label::size_imperm_lay, false, fpl_label::end_mech_burst_din);
				this->size_imperm_layer.check_positiv_variable();
				ifile->seek(pos_file);
				*line_counter=line_counter_start;

				this->dens_imperm_layer.set_input(ifile, line_counter,fpl_label::dens_imperm_lay, false, fpl_label::end_mech_burst_din);
				this->dens_imperm_layer.check_given_boundaries(5000.0,800.0);
				ifile->seek(pos_file);
				*line_counter=line_counter_start;
			}
			catch(Error msg){
				ostringstream info;
				info<<" No impermeable zone is found!"<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		else{
			this->size_imperm_layer.set_input(ifile, line_counter,fpl_label::size_imperm_lay, true, fpl_label::end_mech_burst_din);
			this->size_imperm_layer.check_positiv_zero_variable();
			ifile->seek(pos_file);
			*line_counter=line_counter_start;

		}

		//results
		this->resistance.set_input(ifile, line_counter,fpl_label::res_burst_din, true, fpl_label::end_mech_burst_din);
		this->resistance.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->stress.set_input(ifile, line_counter,fpl_label::stress_burst_din, true, fpl_label::end_mech_burst_din);
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
void Fpl_Mech_Bursting_Din_Dike::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database, Fpl_Dike_Var_Materialzone *zone){
	ostringstream prefix;
	ostringstream cout;
	prefix << "BURST_DIN> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	cout << "Transfer variable members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->variable_leak_length.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_bursting_din, fpl_label::sec_dike);
	this->hydraulic_head_loss.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_bursting_din, fpl_label::sec_dike);
	this->size_imperm_layer.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_bursting_din, fpl_label::sec_dike);
	if(zone==NULL){
		this->dens_imperm_layer.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_bursting_din, fpl_label::sec_dike);
	}
	//result members
	cout << "Transfer result members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->resistance.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_bursting_din, fpl_label::sec_dike);
	this->stress.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_bursting_din, fpl_label::sec_dike);
	
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output members of the mechanism to display/console
void Fpl_Mech_Bursting_Din_Dike::output_members(void){
	ostringstream prefix;
	prefix << "BURST_DIN> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism bursting after DIN19712..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	cout << " Fixed leakage length : "<< this->fixed_leak_length<< label::m << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->variable_leak_length.output_member();
	this->hydraulic_head_loss.output_member();
	this->size_imperm_layer.output_member();
	if(this->impermeable_zone==NULL){
		this->dens_imperm_layer.output_member();
	}
	else{
		cout << "The density of the impermeable layer is found in material variable zone " << this->impermeable_zone->get_zone_id() << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
	}
	cout << "Result members mechanism bursting after DIN19712..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_member();
	this->stress.output_member();

	Sys_Common_Output::output_fpl->rewind_userprefix();

}
//Check the statistic of the random variables of the mechanism
void Fpl_Mech_Bursting_Din_Dike::check_statistic(void){
	this->variable_leak_length.check_statistic();
	this->hydraulic_head_loss.check_statistic();
	this->size_imperm_layer.check_statistic();
	if(this->impermeable_zone==NULL){
		this->dens_imperm_layer.check_statistic();
	}
	this->resistance.check_statistic();
	this->stress.check_statistic();
}
//Reset the statistic important values
void Fpl_Mech_Bursting_Din_Dike::reset_statistics(void){
	this->variable_leak_length.reset_statistics();
	this->hydraulic_head_loss.reset_statistics();
	this->size_imperm_layer.reset_statistics();
	if(this->impermeable_zone==NULL){
		this->dens_imperm_layer.reset_statistics();
	}
	this->resistance.reset_statistics();
	this->stress.reset_statistics();

	this->reset_counter_prob();
}
//Output the statistics of the random variables of the mechanism to display/console
void Fpl_Mech_Bursting_Din_Dike::output_statistic(void){
	ostringstream prefix;
	prefix << "BURST_DIN> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism bursting after DIN19712..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	cout << " Fixed leakage length : "<< this->fixed_leak_length<< label::m << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->variable_leak_length.output_statistic();
	this->hydraulic_head_loss.output_statistic();
	this->size_imperm_layer.output_statistic();
	if(this->impermeable_zone==NULL){
		this->dens_imperm_layer.output_statistic();
	}
	else{
		cout << "The density of the impermeable layer is found in material variable zone " << this->impermeable_zone->get_zone_id() << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
	}
	cout << "Result members mechanism bursting after DIN19712..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_statistic();
	this->stress.output_statistic();

	Sys_Common_Output::output_fpl->rewind_userprefix();

}
//Output the results of a deterministic calculation to display/console
void Fpl_Mech_Bursting_Din_Dike::output_determ_results(void){
	ostringstream prefix;
	prefix << "BURST_DIN> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Deterministic results mechanism bursting after DIN19712..."<< endl;
	cout << " Total leakage length      : "<< this->total_leak_length << label::m << endl;
	cout << " Fixed leakage length      : "<< this->fixed_leak_length << label::m << endl;
	cout << " Variable leakage length   : "<< this->variable_leak_length.get_result().end_result << label::m << endl;
	cout << " Hydraulic head loss       : "<< this->hydraulic_head_loss.get_result().end_result << label::m_per_m << endl;
	cout << " Size impermeable layer    : "<< this->size_imperm_layer.get_result().end_result << label::m << endl;
	if(this->impermeable_zone==NULL){
		cout << " Density material impermeable layer  : "<< this->dens_imperm_layer.get_result().end_result << label::kg_per_qm << endl;
	}
	else{
		cout << " Density material impermeable layer  : "<< "see material variable zone "<< this->impermeable_zone->get_zone_id() << endl;

	}
	cout << "Result members mechanism bursting after DIN19712..." << endl;
	cout << " Resistance                : "<< this->resistance.get_result().end_result<<label::kn_per_sqm<<endl;
	cout << " Stress                    : "<< this->stress.get_result().end_result<<label::kn_per_sqm<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	Sys_Common_Output::output_fpl->rewind_userprefix();

}
//Calculate the mechanism; main input parameter is the waterlevel before the dike section
bool Fpl_Mech_Bursting_Din_Dike::calculate_mechanism(const bool random_calculation, const double waterlevel, const double water_density, Fpl_Seepage_Line_Point_List *seepage_list){
	this->failure=false;
	//reset the random flags
	this->reset_random_flag();
	this->buff_stress=0.0;
	this->buff_res=0.0;

	if(waterlevel<=0.0){
		this->failure=false;
		return this->failure;
	}

	if(this->impermeable_zone==NULL){
		return this->calculate_mecha_without_slice(random_calculation, waterlevel, water_density);
	}
	else{
		return this->calculate_mecha_with_slice(random_calculation, waterlevel, water_density, seepage_list);
	}
}
//Set the fixed leakage length
void Fpl_Mech_Bursting_Din_Dike::set_fixed_leak_length(const double length){
	this->fixed_leak_length=length;
}
//Get total leakage length
double Fpl_Mech_Bursting_Din_Dike::get_total_leak_length(void){
	return this->total_leak_length;
}
//Set the pointer to the impermeable layer material variable zone
void Fpl_Mech_Bursting_Din_Dike::set_ptr_impermeable_layer(Fpl_Dike_Var_Materialzone *zone){
	this->impermeable_zone=zone;
}
//Set the pointer and the numbner of geometrical material zone(s) of the section
void Fpl_Mech_Bursting_Din_Dike::set_ptr_no_material_zone(const int number, Fpl_Dike_Geo_Materialzone *ptr_mat_zone){
	this->number_mat_zone=number;
	this->ptr_mat_zone=ptr_mat_zone;
}
//Calculate geometrical values and sclice für the bursting calculation in case of available material zone(s)
double Fpl_Mech_Bursting_Din_Dike::calculate_geometrical_values(Fpl_Section_Points *ptr_base_land, Fpl_Section_Points *ptr_base_water, Fpl_Section_Points *ptr_crest_land, Fpl_Dike_Geo_Hinterland *ptr_hinterland, Fpl_Seepage_Line_Point_List *seepage_list, Geo_Polysegment *outer_polysegment){

	double min_x=0.0;
	bool found=false;
	//search for the start of the imperemeable layer
	for(int i=0; i< this->number_mat_zone; i++){
		if(this->ptr_mat_zone[i].get_zone_type()==_fpl_zone_type::imperm_layer){
			min_x=this->ptr_mat_zone[i].get_min_x_coordinate();
			found=true;
			break;
		}
	}

	if(found==false){
		//is convertet later into a warning
		Error msg=this->set_error(4);
		throw msg;

	}

	//the impermeable layer begins left of the water side base point
	if(ptr_base_water->get_xcoordinate()>min_x){
		min_x=ptr_base_water->get_xcoordinate();
	}

	//the impermeable layer is right of the landside base point; no bursting mechanism is taken into account
	if(min_x>ptr_base_land->get_xcoordinate()){
		//warning
		Error msg=this->set_error(2);
		throw msg;
	}

	//set all 0.5 m a 10 slices from the land side base point to the right and 100 to the left
	this->number_slices=20;
	this->allocate_bursting_slices();

	//allocate a buffer
	Fpl_Dike_Geo_Materialzone_Connector *connect_buff=NULL;
	try{
		connect_buff=new Fpl_Dike_Geo_Materialzone_Connector[this->number_mat_zone]; 
	}
	catch(bad_alloc&t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//
	double x_rightest=0.0;
	if(ptr_hinterland->get_number_segments()>0){
		x_rightest=ptr_hinterland->get_segment(ptr_hinterland->get_number_segments()-1)->point2.get_xcoordinate();
	}
	else{
		x_rightest=ptr_base_land->get_xcoordinate();
	}


	int counter_slice=0;

	for(int i=0; i< this->number_slices/2; i++){
		if(ptr_base_land->get_xcoordinate()+0.5*(i)<x_rightest){
			this->slices[counter_slice].set_fixed_leak_length(ptr_base_land->get_xcoordinate()-min_x+0.5*(i));
			this->slices[counter_slice].init_zone_connector(ptr_base_land->get_xcoordinate()+0.5*(i), this->number_mat_zone, this->ptr_mat_zone, connect_buff);
			this->slices[counter_slice].add_seepage_line_point(seepage_list, outer_polysegment);
			counter_slice++;
		}
	}
	for(int i=0; i< this->number_slices/2; i++){
		if(ptr_crest_land->get_xcoordinate()<ptr_base_land->get_xcoordinate()-0.5*(i+1)){
			this->slices[counter_slice].set_fixed_leak_length(ptr_base_land->get_xcoordinate()-min_x-0.5*(i+1));
			this->slices[counter_slice].init_zone_connector(ptr_base_land->get_xcoordinate()-0.5*(i+1), this->number_mat_zone, this->ptr_mat_zone, connect_buff);
			this->slices[counter_slice].add_seepage_line_point(seepage_list, outer_polysegment);
			counter_slice++;
		}
	}

	//delete the connect buffer
	if(connect_buff!=NULL){
		delete []connect_buff;
		connect_buff=NULL;
	}

	//check the slices
	for(int i=0; i< counter_slice; i++){
		if(this->slices[i].get_impermeable_zone_intercepted()==false){
			//is convertet later into a warning
			Error msg=this->set_error(3);
			throw msg;
		}
	}

	return min_x;
}
//__________
//private
//Reset the random generated flags of the random variables
void Fpl_Mech_Bursting_Din_Dike::reset_random_flag(void){
	this->variable_leak_length.reset_random_generated();
	this->hydraulic_head_loss.reset_random_generated();
	this->size_imperm_layer.reset_random_generated();
	if(this->impermeable_zone==NULL){
		this->dens_imperm_layer.reset_random_generated();
	}
	this->resistance.reset_random_generated();
	this->stress.reset_random_generated();
}
//Allocate the slices of bursting
void Fpl_Mech_Bursting_Din_Dike::allocate_bursting_slices(void){
	this->delete_bursting_slices();
	try{
		this->slices=new Fpl_Bursting_Slice[this->number_slices];
	}
	catch(bad_alloc&t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Delete the slices of bursting
void Fpl_Mech_Bursting_Din_Dike::delete_bursting_slices(void){
	if(this->slices!=NULL){
		delete []this->slices;
		this->slices=NULL;
	}

}
//Calculate the mechanism with slices
bool Fpl_Mech_Bursting_Din_Dike::calculate_mecha_with_slice(const bool random_calculation, const double waterlevel, const double water_density, Fpl_Seepage_Line_Point_List *seepage_list){

	double buff_density=0.0;
	buff_density=this->impermeable_zone->get_wet_density();
	double buff_head_loss=0.0;
	buff_head_loss=this->hydraulic_head_loss.calculate_variables(random_calculation).end_result;

	this->variable_leak_length.calculate_variables(random_calculation);

	double min_stress=0.0;
	double min_res=0.0;
	double min_delta=0.0;
	double min_leak_length=0.0;
	double min_fix_leak_length=0.0;

	//check every slice
	for(int i=0; i< this->number_slices; i++){
		if(this->slices[i].get_impermeable_zone_intercepted()==true){
			//resistance
			this->size_imperm_layer.set_new_middle(this->slices[i].get_size_impermeable_layer());
			this->size_imperm_layer.reset_random_generated();
			this->buff_res=buff_density*this->size_imperm_layer.calculate_variables(random_calculation).end_result;
			if(this->buff_res<0.0){
				this->buff_res=0.0;
			}
			this->buff_res=this->buff_res+this->slices[i].get_extra_load(seepage_list, water_density);
			this->buff_res=this->buff_res*constant::Cgg/1000.0;


			//stress
			this->fixed_leak_length=this->slices[i].get_fixed_leak_length();
			//cout << " i " << i << " le " << this->fixed_leak_length << " siz " <<this->slices[i].get_size_impermeable_layer()<<" rsize " << this->size_imperm_layer.calculate_variables(random_calculation).end_result << " extra " <<this->slices[i].get_extra_load() << " h "<< waterlevel<< endl;
			this->total_leak_length=this->slices[i].get_fixed_leak_length()+this->variable_leak_length.get_result().end_result;
			this->buff_stress=(waterlevel-this->total_leak_length*this->hydraulic_head_loss.calculate_variables(random_calculation).end_result)*water_density;
			this->buff_stress=this->buff_stress*constant::Cgg/1000.0;
			if(i==0){
				min_stress=this->buff_stress;
				min_res=this->buff_res;
				min_delta=this->buff_res-this->buff_stress;
				min_leak_length=this->total_leak_length;
				min_fix_leak_length=this->fixed_leak_length;
			}
			else{
				if(min_delta>this->buff_res-this->buff_stress){
					min_delta=this->buff_res-this->buff_stress;
					min_res=this->buff_res;
					min_stress=this->buff_stress;
					min_leak_length=this->total_leak_length;
					min_fix_leak_length=this->fixed_leak_length;
				}
			}
			//cout << " stress " << this->buff_stress<< " res " << this->buff_res <<endl;
		}
	}

	//set the results variables
	this->resistance.set_new_middle(min_res);
	this->resistance.calculate_variables(random_calculation);
	this->stress.set_new_middle(min_stress);
	this->stress.calculate_variables(random_calculation);
	this->total_leak_length=min_leak_length;
	this->fixed_leak_length=min_fix_leak_length;

	if(this->stress.get_result().end_result<=0.0){
		this->failure=false;
		return this->failure;
	}
	if(this->resistance.get_result().end_result<=0.0){
		this->failure=true;
		return this->failure;
	}


	//check for failure
	if(this->stress.get_result().end_result>=this->resistance.get_result().end_result){
		//failure
		this->failure=true;
	}
	else{
		this->failure=false;
	}

	return this->failure;
}
//Calculate the mechanism without slices
bool Fpl_Mech_Bursting_Din_Dike::calculate_mecha_without_slice(const bool random_calculation, const double waterlevel, const double water_density){



	double buff_density=0.0;
	buff_density=this->dens_imperm_layer.calculate_variables(random_calculation).end_result;

	//resistance
	this->buff_res=buff_density*this->size_imperm_layer.calculate_variables(random_calculation).end_result;
	this->buff_res=this->buff_res*constant::Cgg/1000.0;
	
	//stress
	this->variable_leak_length.calculate_variables(random_calculation);
	this->total_leak_length=this->fixed_leak_length+this->variable_leak_length.get_result().end_result;
	if(this->total_leak_length<=0.0){
		this->total_leak_length=constant::meter_epsilon;
	}
	this->buff_stress=waterlevel-this->total_leak_length*this->hydraulic_head_loss.calculate_variables(random_calculation).end_result;
	if(this->buff_stress<0.0){
		this->buff_stress=0.0;
	}
	this->buff_stress=(this->buff_stress+this->size_imperm_layer.get_result().end_result)*water_density;
	this->buff_stress=this->buff_stress*constant::Cgg/1000.0;

	//set the results variables
	this->resistance.set_new_middle(this->buff_res);
	this->resistance.calculate_variables(random_calculation);
	this->stress.set_new_middle(this->buff_stress);
	this->stress.calculate_variables(random_calculation);

	if(this->stress.get_result().end_result<=0.0){
		this->failure=false;
		return this->failure;
	}
	if(this->resistance.get_result().end_result<=0.0){
		this->failure=true;
		return this->failure;
	}


	//check for failure
	if(this->stress.get_result().end_result>=this->resistance.get_result().end_result){
		//failure
		this->failure=true;
	}
	else{
		this->failure=false;
	}

	return this->failure;

}
//Set warning(s)
Warning Fpl_Mech_Bursting_Din_Dike::set_warning(const int warn_type){
	string place="Fpl_Mech_Bursting_Din_Dike::";
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
		case 1://no mechanism is set
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
Error Fpl_Mech_Bursting_Din_Dike::set_error(const int err_type){
	string place="Fpl_Mech_Bursting_Din_Dike::";
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
		case 1://bad alloc
			place.append("calculate_geometrical_values(Fpl_Section_Points *ptr_base_land, Fpl_Section_Points *ptr_base_water, Fpl_Section_Points *ptr_crest_land)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://no perm layer is available
			place.append("calculate_geometrical_values(Fpl_Section_Points *ptr_base_land, Fpl_Section_Points *ptr_base_water, Fpl_Section_Points *ptr_crest_land)");
			reason="The impermeable layer begins right of the land side base point";
			help="Check the impermeable layer of the geometrical material zones";
			type=12;
			break;
		case 3://the impermeable zone is not continuous 
			place.append("calculate_geometrical_values(Fpl_Section_Points *ptr_base_land, Fpl_Section_Points *ptr_base_water, Fpl_Section_Points *ptr_crest_land)");
			reason="The impermeable layer is not continuous";
			help="Check the impermeable layer of the geometrical material zones";
			type=12;
			break;
		case 4://the impermeable zone is not defined 
			place.append("calculate_geometrical_values(Fpl_Section_Points *ptr_base_land, Fpl_Section_Points *ptr_base_water, Fpl_Section_Points *ptr_crest_land)");
			reason="The impermeable layer is not defined";
			help="Check the impermeable layer of the geometrical material zones";
			type=12;
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