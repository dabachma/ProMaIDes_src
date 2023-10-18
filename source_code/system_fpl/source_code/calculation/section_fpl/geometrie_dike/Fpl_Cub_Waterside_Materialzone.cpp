#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Cub_Waterside_Materialzone.h"


//Default constructor
Fpl_Cub_Waterside_Materialzone::Fpl_Cub_Waterside_Materialzone(void){


	this->zone_id=-1;

	this->runup_requiered=false;
	this->wave_impact_required=false;
	this->erosion_required=false;
	this->grass_cover=false;

	this->roughness.set_unit(label::no_unit);
	this->cover_thickness_variables.set_unit(label::m);
	

	this->rand_arti_revet=NULL;
	this->rand_arti_revet_imp=NULL;
	this->rand_arti_revet_ero=NULL;
	this->rand_grass=NULL;
	this->rand_grass_cover_imp=NULL;
	this->rand_grass_cover_ero=NULL;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Cub_Waterside_Materialzone)-2*sizeof(Fpl_Random_Variables), _sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Cub_Waterside_Materialzone::~Fpl_Cub_Waterside_Materialzone(void){

	this->delete_zone_material_erosion();
	this->delete_zone_material_wave_impact();

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Cub_Waterside_Materialzone)-2*sizeof(Fpl_Random_Variables), _sys_system_modules::FPL_SYS);//count the memory

}
//_________
//public
//Set the variable of the material variable zone via database
void Fpl_Cub_Waterside_Materialzone::set_input(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database, const int zone_id, const bool runup, const bool wave_impact, const bool erosion){
	try{
		this->zone_id=zone_id;
		this->runup_requiered=runup;
		this->wave_impact_required=wave_impact;
		this->erosion_required=erosion; 


		if(this->runup_requiered==true){
			this->roughness.set_input(id, section_id, fpl_label::gamma_roughness, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, zone_id, false);
			this->roughness.set_distribution_types(ptr_database);

		}
		if(this->erosion_required==true || this->wave_impact_required==true){
			this->cover_thickness_variables.set_input(id, section_id, fpl_label::cover_thickness, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, zone_id, false);
			this->cover_thickness_variables.set_distribution_types(ptr_database);

			if(this->erosion_required==true && this->wave_impact_required==true){
		
				QSqlQueryModel results;
				int no=0;
				no=Fpl_Random_Variables::select_variables_database(&results, id, section_id, fpl_label::crack_thickness, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id);
				if(no>0){
					this->grass_cover=true;
				}
				else{
					this->grass_cover=false;
				}

				this->allocate_zone_material_wave_impact();
				this->allocate_zone_material_erosion();

				if(this->grass_cover==false){
					//general
					this->rand_arti_revet->bulk_density.set_input(id, section_id,fpl_label::bulk_density, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet->bulk_density.set_distribution_types(ptr_database);
					this->rand_arti_revet->porosity.set_input(id, section_id,fpl_label::porosity_cover, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet->porosity.set_distribution_types(ptr_database);	
					//wave impact
					this->rand_arti_revet_imp->exponent_x_variables.set_input(id, section_id,fpl_label::exponent_x, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet_imp->exponent_x_variables.set_distribution_types(ptr_database);
					this->rand_arti_revet_imp->stability_f_variables.set_input(id, section_id,fpl_label::stability_f, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet_imp->stability_f_variables.set_distribution_types(ptr_database);
					//erosion
					this->rand_arti_revet_ero->friction_angle.set_input(id, section_id,fpl_label::friction_angle_cover, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet_ero->friction_angle.set_distribution_types(ptr_database);
					this->rand_arti_revet_ero->stability_total_revetment.set_input(id, section_id,fpl_label::tot_stab_cover, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet_ero->stability_total_revetment.set_distribution_types(ptr_database);
					this->rand_arti_revet_ero->stability_under_layer.set_input(id, section_id,fpl_label::under_layer_quality, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet_ero->stability_under_layer.set_distribution_types(ptr_database);
					this->rand_arti_revet_ero->thickness_under_layer.set_input(id, section_id,fpl_label::under_layer_thickness, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet_ero->thickness_under_layer.set_distribution_types(ptr_database);
				}
				else{
					//general
					this->rand_grass->crack_thickness_variables.set_input(id, section_id,fpl_label::crack_thickness, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass->crack_thickness_variables.set_distribution_types(ptr_database);
					this->rand_grass->root_zone_thickness_variables.set_input(id, section_id,fpl_label::root_zone_thickness, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass->root_zone_thickness_variables.set_distribution_types(ptr_database);
					//wave impact
					this->rand_grass_cover_imp->root_cohesion_variables.set_input(id, section_id,fpl_label::cohesion_root, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass_cover_imp->root_cohesion_variables.set_distribution_types(ptr_database);
					this->rand_grass_cover_imp->undrained_cohesion_variables.set_input(id, section_id,fpl_label::cohesion_undrained, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass_cover_imp->undrained_cohesion_variables.set_distribution_types(ptr_database);
					//erosion
					this->rand_grass_cover_ero->grass_quality.set_input(id, section_id,fpl_label::quality_grass, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass_cover_ero->grass_quality.set_distribution_types(ptr_database);
					this->rand_grass_cover_ero->stability_under_layer.set_input(id, section_id,fpl_label::under_layer_quality, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass_cover_ero->stability_under_layer.set_distribution_types(ptr_database);

				}
				
			}
			else if(this->erosion_required==true && this->wave_impact_required==false){
				QSqlQueryModel results;
				int no=0;
				no=Fpl_Random_Variables::select_variables_database(&results, id, section_id, fpl_label::crack_thickness, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id);
				if(no>0){
					this->grass_cover=true;
				}
				else{
					this->grass_cover=false;
				}
				this->allocate_zone_material_erosion();

				if(this->grass_cover==false){
					//general
					this->rand_arti_revet->bulk_density.set_input(id, section_id,fpl_label::bulk_density, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet->bulk_density.set_distribution_types(ptr_database);
					this->rand_arti_revet->porosity.set_input(id, section_id,fpl_label::porosity_cover, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet->porosity.set_distribution_types(ptr_database);
					//erosion
					this->rand_arti_revet_ero->friction_angle.set_input(id, section_id,fpl_label::friction_angle_cover, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet_ero->friction_angle.set_distribution_types(ptr_database);
					this->rand_arti_revet_ero->stability_total_revetment.set_input(id, section_id,fpl_label::tot_stab_cover, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet_ero->stability_total_revetment.set_distribution_types(ptr_database);
					this->rand_arti_revet_ero->stability_under_layer.set_input(id, section_id,fpl_label::under_layer_quality, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet_ero->stability_under_layer.set_distribution_types(ptr_database);
					this->rand_arti_revet_ero->thickness_under_layer.set_input(id, section_id,fpl_label::under_layer_thickness, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet_ero->thickness_under_layer.set_distribution_types(ptr_database);
				}
				else{
					//general
					this->rand_grass->crack_thickness_variables.set_input(id, section_id,fpl_label::crack_thickness, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass->crack_thickness_variables.set_distribution_types(ptr_database);
					this->rand_grass->root_zone_thickness_variables.set_input(id, section_id,fpl_label::root_zone_thickness, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass->root_zone_thickness_variables.set_distribution_types(ptr_database);
					//erosion
					this->rand_grass_cover_ero->grass_quality.set_input(id, section_id,fpl_label::quality_grass, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass_cover_ero->grass_quality.set_distribution_types(ptr_database);
					this->rand_grass_cover_ero->stability_under_layer.set_input(id, section_id,fpl_label::under_layer_quality, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass_cover_ero->stability_under_layer.set_distribution_types(ptr_database);
				}

			}
			else if(this->erosion_required==false && this->wave_impact_required==true){
				QSqlQueryModel results;
				int no=0;
				no=Fpl_Random_Variables::select_variables_database(&results, id, section_id, fpl_label::crack_thickness, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id);
				if(no>0){
					this->grass_cover=true;
				}
				else{
					this->grass_cover=false;
				}
				this->allocate_zone_material_wave_impact();

				if(this->grass_cover==false){
					//general
					this->rand_arti_revet->bulk_density.set_input(id, section_id,fpl_label::bulk_density, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet->bulk_density.set_distribution_types(ptr_database);
					this->rand_arti_revet->porosity.set_input(id, section_id,fpl_label::porosity_cover, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet->porosity.set_distribution_types(ptr_database);
					//wave impact
					this->rand_arti_revet_imp->exponent_x_variables.set_input(id, section_id,fpl_label::exponent_x, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet_imp->exponent_x_variables.set_distribution_types(ptr_database);
					this->rand_arti_revet_imp->stability_f_variables.set_input(id, section_id,fpl_label::stability_f, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_arti_revet_imp->stability_f_variables.set_distribution_types(ptr_database);
				
				}
				else{
					//general
					this->rand_grass->crack_thickness_variables.set_input(id, section_id,fpl_label::crack_thickness, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass->crack_thickness_variables.set_distribution_types(ptr_database);
					this->rand_grass->root_zone_thickness_variables.set_input(id, section_id,fpl_label::root_zone_thickness, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass->root_zone_thickness_variables.set_distribution_types(ptr_database);
					//wave impact
					this->rand_grass_cover_imp->root_cohesion_variables.set_input(id, section_id,fpl_label::cohesion_root, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass_cover_imp->root_cohesion_variables.set_distribution_types(ptr_database);
					this->rand_grass_cover_imp->undrained_cohesion_variables.set_input(id, section_id,fpl_label::cohesion_undrained, false, ptr_database, fpl_label::material_zone_w, fpl_label::sec_dike, this->zone_id, false);
					this->rand_grass_cover_imp->undrained_cohesion_variables.set_distribution_types(ptr_database);

				}
			}
		}
			
	}catch(Error msg){
		throw msg;
	}

}
//Transfer the slope stability-mechanism random variables data to database
void Fpl_Cub_Waterside_Materialzone::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	
	if(this->runup_requiered==true){
		this->roughness.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
	}
	if(this->erosion_required==true || this->wave_impact_required==true){
		this->cover_thickness_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);

		if(this->erosion_required==true && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_arti_revet->porosity.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);	
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_arti_revet_imp->stability_f_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				//erosion
				this->rand_arti_revet_ero->friction_angle.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_arti_revet_ero->stability_total_revetment.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_arti_revet_ero->stability_under_layer.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_arti_revet_ero->thickness_under_layer.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_grass->root_zone_thickness_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_grass_cover_imp->undrained_cohesion_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				//erosion
				this->rand_grass_cover_ero->grass_quality.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_grass_cover_ero->stability_under_layer.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
			}
		}
		else if(this->erosion_required==true && this->wave_impact_required==false){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_arti_revet->porosity.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				//erosion
				this->rand_arti_revet_ero->friction_angle.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_arti_revet_ero->stability_total_revetment.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_arti_revet_ero->stability_under_layer.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_arti_revet_ero->thickness_under_layer.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_grass->root_zone_thickness_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				//erosion
				this->rand_grass_cover_ero->grass_quality.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_grass_cover_ero->stability_under_layer.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
			}
		}
		else if(this->erosion_required==false && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_arti_revet->porosity.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_arti_revet_imp->stability_f_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_grass->root_zone_thickness_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);
				this->rand_grass_cover_imp->undrained_cohesion_variables.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone_w, fpl_label::sec_dike);

			}
		}
	}
}
//Read the material variable zone per file
void Fpl_Cub_Waterside_Materialzone::read_matzone_per_file(QFile *ifile, int *line_counter, const bool runup, const bool wave_impact, const bool erosion){

	qint64 pos_file=0;
	int store_line=0;
	int must_found_counter=0;

	this->erosion_required=erosion;
	this->wave_impact_required=wave_impact;
	this->runup_requiered=runup;

	

	//find the start of the info block
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_material_zone_info, fpl_label::end_material_zone_info, true);
		store_line=*line_counter;
	}
	catch(Error msg){
		throw msg;
	}

	//read in
	string myline;
	QString qmyline;
	//read in the zone infos
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::end_material_zone_info){
			break;
		}
		if(myline.empty()==false){
			this->find_key_values_file(myline, &must_found_counter);
		}
		if(this->erosion_required==true || this->wave_impact_required==true){
			if(must_found_counter==2){
				break;
			}
		}
		else{
			if(must_found_counter==1){
				break;
			}
		}
	}
	while(ifile->atEnd()==false);

	if(this->erosion_required==true || this->wave_impact_required==true){
		if(must_found_counter!=2){
			Error msg=this->set_error(4);
			throw msg;
		}
	}
	else{
		if(must_found_counter!=1){
			Error msg=this->set_error(4);
			throw msg;
		}
	}

	//read in random variables
	try{
		if(this->runup_requiered==true){
			this->roughness.set_input(ifile, line_counter, fpl_label::gamma_roughness, false,fpl_label::end_material_zone_info);
			this->roughness.check_given_boundaries(2.0,0.0);
			ifile->seek(pos_file);
			*line_counter=store_line;
		}

		if(this->erosion_required==true || this->wave_impact_required==true){
			this->cover_thickness_variables.set_input(ifile, line_counter, fpl_label::cover_thickness, false,fpl_label::end_material_zone_info);
			this->cover_thickness_variables.check_positiv_zero_variable();
			ifile->seek(pos_file);
			*line_counter=store_line;


			if(this->erosion_required==true && this->wave_impact_required==true){
				this->allocate_zone_material_erosion();
				this->allocate_zone_material_wave_impact();

				if(this->grass_cover==false){
					//general
					this->rand_arti_revet->bulk_density.set_input(ifile, line_counter, fpl_label::bulk_density, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet->bulk_density.check_given_boundaries(10000.0,500.0);
					ifile->seek(pos_file);
					*line_counter=store_line;	
					this->rand_arti_revet->porosity.set_input(ifile, line_counter, fpl_label::porosity_cover, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet->porosity.check_given_boundaries(1.0,0.0);
					ifile->seek(pos_file);
					*line_counter=store_line;					
					//wave impact
					this->rand_arti_revet_imp->exponent_x_variables.set_input(ifile, line_counter, fpl_label::exponent_x, false,fpl_label::end_material_zone_info);
					ifile->seek(pos_file);
					*line_counter=store_line;	
					this->rand_arti_revet_imp->stability_f_variables.set_input(ifile, line_counter, fpl_label::stability_f, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet_imp->stability_f_variables.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;	
					//erosion
					this->rand_arti_revet_ero->friction_angle.set_input(ifile, line_counter, fpl_label::friction_angle_cover, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet_ero->friction_angle.check_given_boundaries(120.0,0.0);
					ifile->seek(pos_file);
					*line_counter=store_line;	
					this->rand_arti_revet_ero->stability_total_revetment.set_input(ifile, line_counter, fpl_label::tot_stab_cover, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet_ero->stability_total_revetment.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;	
					this->rand_arti_revet_ero->stability_under_layer.set_input(ifile, line_counter, fpl_label::under_layer_quality, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet_ero->stability_under_layer.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
					this->rand_arti_revet_ero->thickness_under_layer.set_input(ifile, line_counter, fpl_label::under_layer_thickness, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet_ero->thickness_under_layer.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
				}
				else{
					//general
					this->rand_grass->crack_thickness_variables.set_input(ifile, line_counter, fpl_label::crack_thickness, false,fpl_label::end_material_zone_info);
					this->rand_grass->crack_thickness_variables.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;					
					this->rand_grass->root_zone_thickness_variables.set_input(ifile, line_counter, fpl_label::root_zone_thickness, false,fpl_label::end_material_zone_info);
					this->rand_grass->root_zone_thickness_variables.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
					//wave impact
					this->rand_grass_cover_imp->root_cohesion_variables.set_input(ifile, line_counter, fpl_label::cohesion_root, false,fpl_label::end_material_zone_info);
					this->rand_grass_cover_imp->root_cohesion_variables.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
					this->rand_grass_cover_imp->undrained_cohesion_variables.set_input(ifile, line_counter, fpl_label::cohesion_undrained, false,fpl_label::end_material_zone_info);
					this->rand_grass_cover_imp->undrained_cohesion_variables.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
					//erosion
					this->rand_grass_cover_ero->grass_quality.set_input(ifile, line_counter, fpl_label::quality_grass, false,fpl_label::end_material_zone_info);
					this->rand_grass_cover_ero->grass_quality.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
					this->rand_grass_cover_ero->stability_under_layer.set_input(ifile, line_counter, fpl_label::under_layer_quality, false,fpl_label::end_material_zone_info);
					this->rand_grass_cover_ero->stability_under_layer.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
				}
			}
			else if(this->erosion_required==true && this->wave_impact_required==false){
				this->allocate_zone_material_erosion();
				
				if(this->grass_cover==false){
					//general
					this->rand_arti_revet->bulk_density.set_input(ifile, line_counter, fpl_label::bulk_density, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet->bulk_density.check_given_boundaries(10000.0,500.0);;
					ifile->seek(pos_file);
					*line_counter=store_line;	
					this->rand_arti_revet->porosity.set_input(ifile, line_counter, fpl_label::porosity_cover, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet->porosity.check_given_boundaries(1.0,0.0);
					ifile->seek(pos_file);
					*line_counter=store_line;	
					//erosion
					this->rand_arti_revet_ero->friction_angle.set_input(ifile, line_counter, fpl_label::friction_angle_cover, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet_ero->friction_angle.check_given_boundaries(120.0,0.0);
					ifile->seek(pos_file);
					*line_counter=store_line;	
					this->rand_arti_revet_ero->stability_total_revetment.set_input(ifile, line_counter, fpl_label::tot_stab_cover, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet_ero->stability_total_revetment.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;	
					this->rand_arti_revet_ero->stability_under_layer.set_input(ifile, line_counter, fpl_label::under_layer_quality, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet_ero->stability_under_layer.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
					this->rand_arti_revet_ero->thickness_under_layer.set_input(ifile, line_counter, fpl_label::under_layer_thickness, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet_ero->thickness_under_layer.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;				}
				else{
					//general
					this->rand_grass->crack_thickness_variables.set_input(ifile, line_counter, fpl_label::crack_thickness, false,fpl_label::end_material_zone_info);
					this->rand_grass->crack_thickness_variables.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;					
					this->rand_grass->root_zone_thickness_variables.set_input(ifile, line_counter, fpl_label::root_zone_thickness, false,fpl_label::end_material_zone_info);
					this->rand_grass->root_zone_thickness_variables.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
					//erosion
					this->rand_grass_cover_ero->grass_quality.set_input(ifile, line_counter, fpl_label::quality_grass, false,fpl_label::end_material_zone_info);
					this->rand_grass_cover_ero->grass_quality.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
					this->rand_grass_cover_ero->stability_under_layer.set_input(ifile, line_counter, fpl_label::under_layer_quality, false,fpl_label::end_material_zone_info);
					this->rand_grass_cover_ero->stability_under_layer.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;			}
			}
			else if(this->erosion_required==false && this->wave_impact_required==true){
			
				this->allocate_zone_material_wave_impact();

				if(this->grass_cover==false){
					//general
					this->rand_arti_revet->bulk_density.set_input(ifile, line_counter, fpl_label::bulk_density, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet->bulk_density.check_given_boundaries(10000.0,500.0);;
					ifile->seek(pos_file);
					*line_counter=store_line;	
					this->rand_arti_revet->porosity.set_input(ifile, line_counter, fpl_label::porosity_cover, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet->porosity.check_given_boundaries(1.0,0.0);
					ifile->seek(pos_file);
					*line_counter=store_line;
					//wave impact
					this->rand_arti_revet_imp->exponent_x_variables.set_input(ifile, line_counter, fpl_label::exponent_x, false,fpl_label::end_material_zone_info);
					ifile->seek(pos_file);
					*line_counter=store_line;	
					this->rand_arti_revet_imp->stability_f_variables.set_input(ifile, line_counter, fpl_label::stability_f, false,fpl_label::end_material_zone_info);
					this->rand_arti_revet_imp->stability_f_variables.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;					
				}
				else{
					//general
					this->rand_grass->crack_thickness_variables.set_input(ifile, line_counter, fpl_label::crack_thickness, false,fpl_label::end_material_zone_info);
					this->rand_grass->crack_thickness_variables.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;					
					this->rand_grass->root_zone_thickness_variables.set_input(ifile, line_counter, fpl_label::root_zone_thickness, false,fpl_label::end_material_zone_info);
					this->rand_grass->root_zone_thickness_variables.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
					//wave impact
					this->rand_grass_cover_imp->root_cohesion_variables.set_input(ifile, line_counter, fpl_label::cohesion_root, false,fpl_label::end_material_zone_info);
					this->rand_grass_cover_imp->root_cohesion_variables.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
					this->rand_grass_cover_imp->undrained_cohesion_variables.set_input(ifile, line_counter, fpl_label::cohesion_undrained, false,fpl_label::end_material_zone_info);
					this->rand_grass_cover_imp->undrained_cohesion_variables.check_positiv_zero_variable();
					ifile->seek(pos_file);
					*line_counter=store_line;
				}
			}
		}
	}
	catch(Error msg){
		ostringstream info;
		info<<"Material variable zone id: " << this->zone_id <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//set the zone id
	if(this->runup_requiered==true){
		this->roughness.set_zone_id(this->zone_id);
	}
	if(this->erosion_required==true || this->wave_impact_required==true){
		this->cover_thickness_variables.set_zone_id(this->zone_id);

		if(this->erosion_required==true && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.set_zone_id(this->zone_id);
				this->rand_arti_revet->porosity.set_zone_id(this->zone_id);	
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.set_zone_id(this->zone_id);
				this->rand_arti_revet_imp->stability_f_variables.set_zone_id(this->zone_id);
				//erosion
				this->rand_arti_revet_ero->friction_angle.set_zone_id(this->zone_id);
				this->rand_arti_revet_ero->stability_total_revetment.set_zone_id(this->zone_id);
				this->rand_arti_revet_ero->stability_under_layer.set_zone_id(this->zone_id);
				this->rand_arti_revet_ero->thickness_under_layer.set_zone_id(this->zone_id);
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.set_zone_id(this->zone_id);
				this->rand_grass->root_zone_thickness_variables.set_zone_id(this->zone_id);
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.set_zone_id(this->zone_id);
				this->rand_grass_cover_imp->undrained_cohesion_variables.set_zone_id(this->zone_id);
				//erosion
				this->rand_grass_cover_ero->grass_quality.set_zone_id(this->zone_id);
				this->rand_grass_cover_ero->stability_under_layer.set_zone_id(this->zone_id);
			}
		}
		else if(this->erosion_required==true && this->wave_impact_required==false){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.set_zone_id(this->zone_id);
				this->rand_arti_revet->porosity.set_zone_id(this->zone_id);
				//erosion
				this->rand_arti_revet_ero->friction_angle.set_zone_id(this->zone_id);
				this->rand_arti_revet_ero->stability_total_revetment.set_zone_id(this->zone_id);
				this->rand_arti_revet_ero->stability_under_layer.set_zone_id(this->zone_id);
				this->rand_arti_revet_ero->thickness_under_layer.set_zone_id(this->zone_id);
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.set_zone_id(this->zone_id);
				this->rand_grass->root_zone_thickness_variables.set_zone_id(this->zone_id);
				//erosion
				this->rand_grass_cover_ero->grass_quality.set_zone_id(this->zone_id);
				this->rand_grass_cover_ero->stability_under_layer.set_zone_id(this->zone_id);
			}
		}
		else if(this->erosion_required==false && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.set_zone_id(this->zone_id);
				this->rand_arti_revet->porosity.set_zone_id(this->zone_id);
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.set_zone_id(this->zone_id);
				this->rand_arti_revet_imp->stability_f_variables.set_zone_id(this->zone_id);
				
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.set_zone_id(this->zone_id);
				this->rand_grass->root_zone_thickness_variables.set_zone_id(this->zone_id);
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.set_zone_id(this->zone_id);
				this->rand_grass_cover_imp->undrained_cohesion_variables.set_zone_id(this->zone_id);

			}
		}
	}


	//search for the end
	bool found_flag=false;
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==fpl_label::end_material_zone_info){
			found_flag=true;
			break;
		}
	}
	while(ifile->atEnd()==false);


	if(found_flag==false ){
		Error msg=this->set_error(8);
		ostringstream info;
		info << "End keyword   : " << fpl_label::end_material_zone_info<<endl;
		msg.make_second_info(info.str());
 		throw msg;

	}

}
//Get the zone identifier
int Fpl_Cub_Waterside_Materialzone::get_zone_id(void){
	return this->zone_id;
}
//Reset the statistics of the random variables of the zone variables to display/console
void Fpl_Cub_Waterside_Materialzone::reset_statistic(void){
	
	if(this->runup_requiered==true){
		this->roughness.reset_statistics();
	}
	if(this->erosion_required==true || this->wave_impact_required==true){
		this->cover_thickness_variables.reset_statistics();

		if(this->erosion_required==true && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.reset_statistics();
				this->rand_arti_revet->porosity.reset_statistics();	
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.reset_statistics();
				this->rand_arti_revet_imp->stability_f_variables.reset_statistics();
				//erosion
				this->rand_arti_revet_ero->friction_angle.reset_statistics();
				this->rand_arti_revet_ero->stability_total_revetment.reset_statistics();
				this->rand_arti_revet_ero->stability_under_layer.reset_statistics();
				this->rand_arti_revet_ero->thickness_under_layer.reset_statistics();
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.reset_statistics();
				this->rand_grass->root_zone_thickness_variables.reset_statistics();
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.reset_statistics();
				this->rand_grass_cover_imp->undrained_cohesion_variables.reset_statistics();
				//erosion
				this->rand_grass_cover_ero->grass_quality.reset_statistics();
				this->rand_grass_cover_ero->stability_under_layer.reset_statistics();
			}
		}
		else if(this->erosion_required==true && this->wave_impact_required==false){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.reset_statistics();
				this->rand_arti_revet->porosity.reset_statistics();
				//erosion
				this->rand_arti_revet_ero->friction_angle.reset_statistics();
				this->rand_arti_revet_ero->stability_total_revetment.reset_statistics();
				this->rand_arti_revet_ero->stability_under_layer.reset_statistics();
				this->rand_arti_revet_ero->thickness_under_layer.reset_statistics();
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.reset_statistics();
				this->rand_grass->root_zone_thickness_variables.reset_statistics();
				//erosion
				this->rand_grass_cover_ero->grass_quality.reset_statistics();
				this->rand_grass_cover_ero->stability_under_layer.reset_statistics();
			}
		}
		else if(this->erosion_required==false && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.reset_statistics();
				this->rand_arti_revet->porosity.reset_statistics();
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.reset_statistics();
				this->rand_arti_revet_imp->stability_f_variables.reset_statistics();
				
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.reset_statistics();
				this->rand_grass->root_zone_thickness_variables.reset_statistics();
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.reset_statistics();
				this->rand_grass_cover_imp->undrained_cohesion_variables.reset_statistics();

			}
		}
	}
}
//Check the statistics of the random variables of the zone variables to display/console
void Fpl_Cub_Waterside_Materialzone::check_statistic(void){
	if(this->runup_requiered==true){
		this->roughness.check_statistic();
	}

	if(this->erosion_required==true || this->wave_impact_required==true){
		this->cover_thickness_variables.check_statistic();

		if(this->erosion_required==true && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.check_statistic();
				this->rand_arti_revet->porosity.check_statistic();	
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.check_statistic();
				this->rand_arti_revet_imp->stability_f_variables.check_statistic();
				//erosion
				this->rand_arti_revet_ero->friction_angle.check_statistic();
				this->rand_arti_revet_ero->stability_total_revetment.check_statistic();
				this->rand_arti_revet_ero->stability_under_layer.check_statistic();
				this->rand_arti_revet_ero->thickness_under_layer.check_statistic();
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.check_statistic();
				this->rand_grass->root_zone_thickness_variables.check_statistic();
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.check_statistic();
				this->rand_grass_cover_imp->undrained_cohesion_variables.check_statistic();
				//erosion
				this->rand_grass_cover_ero->grass_quality.check_statistic();
				this->rand_grass_cover_ero->stability_under_layer.check_statistic();
			}
		}
		else if(this->erosion_required==true && this->wave_impact_required==false){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.check_statistic();
				this->rand_arti_revet->porosity.check_statistic();
				//erosion
				this->rand_arti_revet_ero->friction_angle.check_statistic();
				this->rand_arti_revet_ero->stability_total_revetment.check_statistic();
				this->rand_arti_revet_ero->stability_under_layer.check_statistic();
				this->rand_arti_revet_ero->thickness_under_layer.check_statistic();
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.check_statistic();
				this->rand_grass->root_zone_thickness_variables.check_statistic();
				//erosion
				this->rand_grass_cover_ero->grass_quality.check_statistic();
				this->rand_grass_cover_ero->stability_under_layer.check_statistic();
			}
		}
		else if(this->erosion_required==false && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.check_statistic();
				this->rand_arti_revet->porosity.check_statistic();
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.check_statistic();
				this->rand_arti_revet_imp->stability_f_variables.check_statistic();
				
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.check_statistic();
				this->rand_grass->root_zone_thickness_variables.check_statistic();
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.check_statistic();
				this->rand_grass_cover_imp->undrained_cohesion_variables.check_statistic();

			}
		}
	}
}
//Output the statistics of the random variables of the zone variables to display/console
void Fpl_Cub_Waterside_Materialzone::output_statistic(void){
	ostringstream prefix;
	prefix << "W_ZONE_"<<this->zone_id<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	if(this->runup_requiered==true){
		this->roughness.output_statistic();
	}

	if(this->erosion_required==true || this->wave_impact_required==true){
		this->cover_thickness_variables.output_statistic();

		if(this->erosion_required==true && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.output_statistic();
				this->rand_arti_revet->porosity.output_statistic();	
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.output_statistic();
				this->rand_arti_revet_imp->stability_f_variables.output_statistic();
				//erosion
				this->rand_arti_revet_ero->friction_angle.output_statistic();
				this->rand_arti_revet_ero->stability_total_revetment.output_statistic();
				this->rand_arti_revet_ero->stability_under_layer.output_statistic();
				this->rand_arti_revet_ero->thickness_under_layer.output_statistic();
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.output_statistic();
				this->rand_grass->root_zone_thickness_variables.output_statistic();
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.output_statistic();
				this->rand_grass_cover_imp->undrained_cohesion_variables.output_statistic();
				//erosion
				this->rand_grass_cover_ero->grass_quality.output_statistic();
				this->rand_grass_cover_ero->stability_under_layer.output_statistic();
			}
		}
		else if(this->erosion_required==true && this->wave_impact_required==false){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.output_statistic();
				this->rand_arti_revet->porosity.output_statistic();
				//erosion
				this->rand_arti_revet_ero->friction_angle.output_statistic();
				this->rand_arti_revet_ero->stability_total_revetment.output_statistic();
				this->rand_arti_revet_ero->stability_under_layer.output_statistic();
				this->rand_arti_revet_ero->thickness_under_layer.output_statistic();
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.output_statistic();
				this->rand_grass->root_zone_thickness_variables.output_statistic();
				//erosion
				this->rand_grass_cover_ero->grass_quality.output_statistic();
				this->rand_grass_cover_ero->stability_under_layer.output_statistic();
			}
		}
		else if(this->erosion_required==false && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.output_statistic();
				this->rand_arti_revet->porosity.output_statistic();
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.output_statistic();
				this->rand_arti_revet_imp->stability_f_variables.output_statistic();
				
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.output_statistic();
				this->rand_grass->root_zone_thickness_variables.output_statistic();
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.output_statistic();
				this->rand_grass_cover_imp->undrained_cohesion_variables.output_statistic();

			}
		}
	}
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the results of a deterministic calculation to display/console
void Fpl_Cub_Waterside_Materialzone::output_determ_results(void){
	ostringstream prefix;
	prefix << "W_ZONE_"<<this->zone_id<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	ostringstream cout;
	cout << "DETERMINISTIC"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout, false);
	if(this->runup_requiered==true){
		this->roughness.output_deterministic();
	}

	if(this->erosion_required==true || this->wave_impact_required==true){
		this->cover_thickness_variables.output_deterministic();

		if(this->erosion_required==true && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.output_deterministic();
				this->rand_arti_revet->porosity.output_deterministic();	
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.output_deterministic();
				this->rand_arti_revet_imp->stability_f_variables.output_deterministic();
				//erosion
				this->rand_arti_revet_ero->friction_angle.output_deterministic();
				this->rand_arti_revet_ero->stability_total_revetment.output_deterministic();
				this->rand_arti_revet_ero->stability_under_layer.output_deterministic();
				this->rand_arti_revet_ero->thickness_under_layer.output_deterministic();
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.output_deterministic();
				this->rand_grass->root_zone_thickness_variables.output_deterministic();
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.output_deterministic();
				this->rand_grass_cover_imp->undrained_cohesion_variables.output_deterministic();
				//erosion
				this->rand_grass_cover_ero->grass_quality.output_deterministic();
				this->rand_grass_cover_ero->stability_under_layer.output_deterministic();
			}
		}
		else if(this->erosion_required==true && this->wave_impact_required==false){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.output_deterministic();
				this->rand_arti_revet->porosity.output_deterministic();
				//erosion
				this->rand_arti_revet_ero->friction_angle.output_deterministic();
				this->rand_arti_revet_ero->stability_total_revetment.output_deterministic();
				this->rand_arti_revet_ero->stability_under_layer.output_deterministic();
				this->rand_arti_revet_ero->thickness_under_layer.output_deterministic();
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.output_deterministic();
				this->rand_grass->root_zone_thickness_variables.output_deterministic();
				//erosion
				this->rand_grass_cover_ero->grass_quality.output_deterministic();
				this->rand_grass_cover_ero->stability_under_layer.output_deterministic();
			}
		}
		else if(this->erosion_required==false && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.output_deterministic();
				this->rand_arti_revet->porosity.output_deterministic();
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.output_deterministic();
				this->rand_arti_revet_imp->stability_f_variables.output_deterministic();
				
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.output_deterministic();
				this->rand_grass->root_zone_thickness_variables.output_deterministic();
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.output_deterministic();
				this->rand_grass_cover_imp->undrained_cohesion_variables.output_deterministic();

			}
		}
	}
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Reset the random generated flag
void Fpl_Cub_Waterside_Materialzone::reset_random_flag(void){
	if(this->runup_requiered==true){
		this->roughness.reset_random_generated();
	}

	if(this->erosion_required==true || this->wave_impact_required==true){
		this->cover_thickness_variables.reset_random_generated();

		if(this->erosion_required==true && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.reset_random_generated();
				this->rand_arti_revet->porosity.reset_random_generated();	
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.reset_random_generated();
				this->rand_arti_revet_imp->stability_f_variables.reset_random_generated();
				//erosion
				this->rand_arti_revet_ero->friction_angle.reset_random_generated();
				this->rand_arti_revet_ero->stability_total_revetment.reset_random_generated();
				this->rand_arti_revet_ero->stability_under_layer.reset_random_generated();
				this->rand_arti_revet_ero->thickness_under_layer.reset_random_generated();
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.reset_random_generated();
				this->rand_grass->root_zone_thickness_variables.reset_random_generated();
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.reset_random_generated();
				this->rand_grass_cover_imp->undrained_cohesion_variables.reset_random_generated();
				//erosion
				this->rand_grass_cover_ero->grass_quality.reset_random_generated();
				this->rand_grass_cover_ero->stability_under_layer.reset_random_generated();
			}
		}
		else if(this->erosion_required==true && this->wave_impact_required==false){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.reset_random_generated();
				this->rand_arti_revet->porosity.reset_random_generated();
				//erosion
				this->rand_arti_revet_ero->friction_angle.reset_random_generated();
				this->rand_arti_revet_ero->stability_total_revetment.reset_random_generated();
				this->rand_arti_revet_ero->stability_under_layer.reset_random_generated();
				this->rand_arti_revet_ero->thickness_under_layer.reset_random_generated();
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.reset_random_generated();
				this->rand_grass->root_zone_thickness_variables.reset_random_generated();
				//erosion
				this->rand_grass_cover_ero->grass_quality.reset_random_generated();
				this->rand_grass_cover_ero->stability_under_layer.reset_random_generated();
			}
		}
		else if(this->erosion_required==false && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.reset_random_generated();
				this->rand_arti_revet->porosity.reset_random_generated();
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.reset_random_generated();
				this->rand_arti_revet_imp->stability_f_variables.reset_random_generated();
				
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.reset_random_generated();
				this->rand_grass->root_zone_thickness_variables.reset_random_generated();
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.reset_random_generated();
				this->rand_grass_cover_imp->undrained_cohesion_variables.reset_random_generated();

			}
		}
	}
}
//Reset the random generated flag
void Fpl_Cub_Waterside_Materialzone::generate_random_variables(const bool random_calculation){
	if(this->runup_requiered==true){
		this->roughness.calculate_variables(random_calculation);
	}

	if(this->erosion_required==true || this->wave_impact_required==true){
		this->cover_thickness_variables.calculate_variables(random_calculation);

		if(this->erosion_required==true && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.calculate_variables(random_calculation);
				this->rand_arti_revet->porosity.calculate_variables(random_calculation);	
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.calculate_variables(random_calculation);
				this->rand_arti_revet_imp->stability_f_variables.calculate_variables(random_calculation);
				//erosion
				this->rand_arti_revet_ero->friction_angle.calculate_variables(random_calculation);
				this->rand_arti_revet_ero->stability_total_revetment.calculate_variables(random_calculation);
				this->rand_arti_revet_ero->stability_under_layer.calculate_variables(random_calculation);
				this->rand_arti_revet_ero->thickness_under_layer.calculate_variables(random_calculation);
			}
			else{
				//general
				//set boundary to the cover thickness
				this->rand_grass->crack_thickness_variables.set_max_boundary(this->cover_thickness_variables.get_result().end_result);

				this->rand_grass->crack_thickness_variables.calculate_variables(random_calculation);
				this->rand_grass->root_zone_thickness_variables.calculate_variables(random_calculation);
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.calculate_variables(random_calculation);
				this->rand_grass_cover_imp->undrained_cohesion_variables.calculate_variables(random_calculation);
				//erosion
				this->rand_grass_cover_ero->grass_quality.calculate_variables(random_calculation);
				this->rand_grass_cover_ero->stability_under_layer.calculate_variables(random_calculation);
			}
		}
		else if(this->erosion_required==true && this->wave_impact_required==false){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.calculate_variables(random_calculation);
				this->rand_arti_revet->porosity.calculate_variables(random_calculation);
				//erosion
				this->rand_arti_revet_ero->friction_angle.calculate_variables(random_calculation);
				this->rand_arti_revet_ero->stability_total_revetment.calculate_variables(random_calculation);
				this->rand_arti_revet_ero->stability_under_layer.calculate_variables(random_calculation);
				this->rand_arti_revet_ero->thickness_under_layer.calculate_variables(random_calculation);
			}
			else{
				//general
				//set boundary to the cover thickness
				this->rand_grass->crack_thickness_variables.set_max_boundary(this->cover_thickness_variables.get_result().end_result);

				this->rand_grass->crack_thickness_variables.calculate_variables(random_calculation);
				this->rand_grass->root_zone_thickness_variables.calculate_variables(random_calculation);
				//erosion
				this->rand_grass_cover_ero->grass_quality.calculate_variables(random_calculation);
				this->rand_grass_cover_ero->stability_under_layer.calculate_variables(random_calculation);
			}
		}
		else if(this->erosion_required==false && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.calculate_variables(random_calculation);
				this->rand_arti_revet->porosity.calculate_variables(random_calculation);
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.calculate_variables(random_calculation);
				this->rand_arti_revet_imp->stability_f_variables.calculate_variables(random_calculation);
				
			}
			else{
				//general
				//set boundary to the cover thickness
				this->rand_grass->crack_thickness_variables.set_max_boundary(this->cover_thickness_variables.get_result().end_result);

				this->rand_grass->crack_thickness_variables.calculate_variables(random_calculation);
				this->rand_grass->root_zone_thickness_variables.calculate_variables(random_calculation);
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.calculate_variables(random_calculation);
				this->rand_grass_cover_imp->undrained_cohesion_variables.calculate_variables(random_calculation);

			}
		}
	}
}
//Output the material variable zone to display/console
void Fpl_Cub_Waterside_Materialzone::output_member(void){

	ostringstream prefix;
	prefix << "W_ZONE_"<<this->zone_id<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);

	if(this->runup_requiered==true){
		this->roughness.output_member();
	}

		if(this->erosion_required==true || this->wave_impact_required==true){
		this->cover_thickness_variables.output_member();

		if(this->erosion_required==true && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.output_member();
				this->rand_arti_revet->porosity.output_member();	
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.output_member();
				this->rand_arti_revet_imp->stability_f_variables.output_member();
				//erosion
				this->rand_arti_revet_ero->friction_angle.output_member();
				this->rand_arti_revet_ero->stability_total_revetment.output_member();
				this->rand_arti_revet_ero->stability_under_layer.output_member();
				this->rand_arti_revet_ero->thickness_under_layer.output_member();
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.output_member();
				this->rand_grass->root_zone_thickness_variables.output_member();
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.output_member();
				this->rand_grass_cover_imp->undrained_cohesion_variables.output_member();
				//erosion
				this->rand_grass_cover_ero->grass_quality.output_member();
				this->rand_grass_cover_ero->stability_under_layer.output_member();
			}
		}
		else if(this->erosion_required==true && this->wave_impact_required==false){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.output_member();
				this->rand_arti_revet->porosity.output_member();
				//erosion
				this->rand_arti_revet_ero->friction_angle.output_member();
				this->rand_arti_revet_ero->stability_total_revetment.output_member();
				this->rand_arti_revet_ero->stability_under_layer.output_member();
				this->rand_arti_revet_ero->thickness_under_layer.output_member();
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.output_member();
				this->rand_grass->root_zone_thickness_variables.output_member();
				//erosion
				this->rand_grass_cover_ero->grass_quality.output_member();
				this->rand_grass_cover_ero->stability_under_layer.output_member();
			}
		}
		else if(this->erosion_required==false && this->wave_impact_required==true){
			if(this->grass_cover==false){
				//general
				this->rand_arti_revet->bulk_density.output_member();
				this->rand_arti_revet->porosity.output_member();
				//wave impact
				this->rand_arti_revet_imp->exponent_x_variables.output_member();
				this->rand_arti_revet_imp->stability_f_variables.output_member();
				
			}
			else{
				//general
				this->rand_grass->crack_thickness_variables.output_member();
				this->rand_grass->root_zone_thickness_variables.output_member();
				//wave impact
				this->rand_grass_cover_imp->root_cohesion_variables.output_member();
				this->rand_grass_cover_imp->undrained_cohesion_variables.output_member();

			}
		}
	}

	Sys_Common_Output::output_fpl->rewind_userprefix();

}
//Get the grass cover flag
bool Fpl_Cub_Waterside_Materialzone::get_grass_cover_flag(void){
	return this->grass_cover;
}
//_________
//private
//Allocate the random variables of the wave impact
void Fpl_Cub_Waterside_Materialzone::allocate_zone_material_wave_impact(void){
	try{
		if(this->grass_cover==false){
			this->rand_arti_revet_imp=new _fpl_rand_artificial_revetment_wave_impact;
			Sys_Memory_Count::self()->add_mem(sizeof(_fpl_rand_artificial_revetment_wave_impact), _sys_system_modules::FPL_SYS);//count the memory
			//init artificial revetment wave impact
			this->rand_arti_revet_imp->stability_f_variables.set_unit(label::no_unit);
			this->rand_arti_revet_imp->exponent_x_variables.set_unit(label::no_unit);


			if(this->rand_arti_revet==NULL){
				this->rand_arti_revet=new _fpl_rand_artificial_revetment;
				Sys_Memory_Count::self()->add_mem(sizeof(_fpl_rand_artificial_revetment), _sys_system_modules::FPL_SYS);//count the memory
				//init artificial revetment general
				this->rand_arti_revet->bulk_density.set_unit(label::kg_per_qm);
				this->rand_arti_revet->porosity.set_unit(label::no_unit);
			}
			
		}
		else{
			this->rand_grass_cover_imp=new _fpl_rand_grass_cover_wave_impact;
			Sys_Memory_Count::self()->add_mem(sizeof(_fpl_rand_grass_cover_wave_impact), _sys_system_modules::FPL_SYS);//count the memory
			//init grass cover wave impact
			this->rand_grass_cover_imp->root_cohesion_variables.set_unit(label::n_per_sqm);
			this->rand_grass_cover_imp->undrained_cohesion_variables.set_unit(label::n_per_sqm);

			if(this->rand_grass==NULL){
				this->rand_grass=new _fpl_rand_grass_cover;
				Sys_Memory_Count::self()->add_mem(sizeof(_fpl_rand_grass_cover), _sys_system_modules::FPL_SYS);//count the memory
				//init grass cover
				this->rand_grass->root_zone_thickness_variables.set_unit(label::m);
				this->rand_grass->crack_thickness_variables.set_unit(label::m);

			}
		
		}
	}
	catch(bad_alloc &t){
		Error msg=set_error(10);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the random variables of the wave impact
void Fpl_Cub_Waterside_Materialzone::delete_zone_material_wave_impact(void){
	if(this->rand_arti_revet_imp!=NULL){
		delete this->rand_arti_revet_imp;
		this->rand_arti_revet_imp=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_rand_artificial_revetment_wave_impact), _sys_system_modules::FPL_SYS);//count the memory
	}
	if(this->rand_grass_cover_imp!=NULL){
		delete this->rand_grass_cover_imp;
		this->rand_grass_cover_imp=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_rand_grass_cover_wave_impact), _sys_system_modules::FPL_SYS);//count the memory
	}
	if(this->rand_arti_revet!=NULL){
		delete this->rand_arti_revet;
		this->rand_arti_revet=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_rand_artificial_revetment), _sys_system_modules::FPL_SYS);//count the memory
	}
	if(this->rand_grass!=NULL){
		delete this->rand_grass;
		this->rand_grass=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_rand_grass_cover), _sys_system_modules::FPL_SYS);//count the memory
	}
}
//Allocate the random variables of the erosion
void Fpl_Cub_Waterside_Materialzone::allocate_zone_material_erosion(void){
	try{
		if(this->grass_cover==false){
			this->rand_arti_revet_ero=new _fpl_rand_artificial_revetment_erosion;
			Sys_Memory_Count::self()->add_mem(sizeof(_fpl_rand_artificial_revetment_erosion), _sys_system_modules::FPL_SYS);//count the memory
			//init artificial revetment erosion
			this->rand_arti_revet_ero->stability_total_revetment.set_unit(label::no_unit);
			this->rand_arti_revet_ero->friction_angle.set_unit(label::degree);
			this->rand_arti_revet_ero->stability_under_layer.set_unit(label::one_per_sec_m);
			this->rand_arti_revet_ero->thickness_under_layer.set_unit(label::m);

			if(this->rand_arti_revet==NULL){
				this->rand_arti_revet=new _fpl_rand_artificial_revetment;
				Sys_Memory_Count::self()->add_mem(sizeof(_fpl_rand_artificial_revetment), _sys_system_modules::FPL_SYS);//count the memory
				//init artificial revetment general
				this->rand_arti_revet->bulk_density.set_unit(label::kg_per_qm);
				this->rand_arti_revet->porosity.set_unit(label::no_unit);
			}
			
			
		}
		else{
			this->rand_grass_cover_ero=new _fpl_rand_grass_cover_erosion;
			Sys_Memory_Count::self()->add_mem(sizeof(_fpl_rand_grass_cover_erosion), _sys_system_modules::FPL_SYS);//count the memory
			//init grass cover erosion
			this->rand_grass_cover_ero->grass_quality.set_unit(label::one_per_sec_m);
			this->rand_grass_cover_ero->stability_under_layer.set_unit(label::one_per_sec_m);

			if(this->rand_grass==NULL){
				this->rand_grass=new _fpl_rand_grass_cover;
				Sys_Memory_Count::self()->add_mem(sizeof(_fpl_rand_grass_cover), _sys_system_modules::FPL_SYS);//count the memory
				//init grass cover
				this->rand_grass->root_zone_thickness_variables.set_unit(label::m);
				this->rand_grass->crack_thickness_variables.set_unit(label::m);
			}

		}
	}
	catch(bad_alloc &t){
		Error msg=set_error(11);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
}
//Delete the random variables of the erosion
void Fpl_Cub_Waterside_Materialzone::delete_zone_material_erosion(void){
	if(this->rand_arti_revet_ero!=NULL){
		delete this->rand_arti_revet_ero;
		this->rand_arti_revet_ero=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_rand_artificial_revetment_erosion), _sys_system_modules::FPL_SYS);//count the memory
	}
	if(this->rand_grass_cover_ero!=NULL){
		delete this->rand_grass_cover_ero;
		this->rand_grass_cover_ero=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_rand_grass_cover_erosion), _sys_system_modules::FPL_SYS);//count the memory
	}
	if(this->rand_arti_revet!=NULL){
		delete this->rand_arti_revet;
		this->rand_arti_revet=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_rand_artificial_revetment), _sys_system_modules::FPL_SYS);//count the memory
	}
	if(this->rand_grass!=NULL){
		delete this->rand_grass;
		this->rand_grass=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_rand_grass_cover), _sys_system_modules::FPL_SYS);//count the memory
	}
}
//Find the start of an input block of the fpl-section in file
qint64 Fpl_Cub_Waterside_Materialzone::find_start_block_file(QFile *ifile, int *line_counter, const string begin, const string end, const bool must_found){
	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	qint64 pos_file=0;

	//search for the begin
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==begin){
			found_flag=true;
			pos_file=ifile->pos();
			break;
		}
		if(myline==end){
			break;
		}
	}
	while(ifile->atEnd()==false);


	if(found_flag==false && must_found==true){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Start keyword : " << begin << endl;
		info << "End keyword   : " << end<<endl;
		msg.make_second_info(info.str());
 		throw msg;

	}
	else{
		found_flag=false;
	}

	return pos_file;

}
//Find the key-values for the file input of the material zone data
void Fpl_Cub_Waterside_Materialzone::find_key_values_file(const string myline, int *must_found_counter){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;



	//zone id
	pos=myline.find(fpl_label::key_zone_id);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_zone_id.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->zone_id;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else if(this->zone_id<=0){
			Error msg=this->set_error(9);
			ostringstream info;
			info<<"Found material variable zone id : "<< this->zone_id<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(1);
		msg.make_second_info(info.str());
		throw msg;
	}

	//zone type
	if(this->erosion_required==true || this->wave_impact_required==true){
		pos=myline.find(fpl_label::key_zone_grass);
		if(pos>=0 && wrong_input==false){
			buffer=myline.substr(fpl_label::key_zone_grass.length());
			functions::clean_string(&buffer);
			buffer1 << buffer;
			this->grass_cover=functions::convert_string2boolean(buffer1.str());
			if(buffer1.fail()==true){
				wrong_input=true;
			}
			else{
				(*must_found_counter)++;
				return;
			}
		}
		if(wrong_input==true){
			ostringstream info;
			info << "Wrong input sequenze  : " << buffer1.str() << endl;
			Error msg=this->set_error(1);
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//set the error
Error Fpl_Cub_Waterside_Materialzone::set_error(const int err_type){
		string place="Fpl_Cub_Waterside_Materialzone::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://do not find the begin of the material zone information in file
			place.append("find_start_block_file(QFile *ifile, int *line_counter, const string begin, const string end, const bool must_found)");
			reason="Can not find the begin of the data block in file";
			help="The material variable zone block data has to start with the keys !$BEGIN_...";
			type=1;
			break;
		case 1://wrong input
			place.append("find_key_values_file(const string myline, int *must_found_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the waterside material zones in file";
			type=1;
			break;
		case 4://do not find all required section infos
			place.append("read_matzone_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find all required waterside material zone information";
			help="Check if all waterside material variable zone information are set in the file";
			info << "Required information:"<<endl;
			info << " " << fpl_label::key_zone_id<< endl;
			info << " " << fpl_label::key_zone_grass<<" (for wave impact or waterside erosion)"<<endl;
			type=1;
			break;
		case 8://do not find the begin of the material zone information in file
			place.append("read_matzone_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find the end of the data block in file";
			help="The waterside material variable zone block data has to end with the keys !$END_...";
			type=1;
			break;
		case 9://zone id must be greater than zero
			place.append("find_key_values_file(const string myline, int *must_found_counter)");
			reason="The zone id has to be greater than 0";
			help="Check the zone id of the waterside material variable zone(s) in file";
			type=1;
			break;
		case 10://bad_alloc
			place.append("allocate_zone_material_wave_impact(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 11://bad_alloc
			place.append("allocate_zone_material_erosion(void)");
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
