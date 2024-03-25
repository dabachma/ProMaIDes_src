#include "Dam_Headers_Precompiled.h"
//#include "Dam_Impact_Values.h"

//Default constructor
Dam_Impact_Values::Dam_Impact_Values(void){
	this->impact_dsdt=0.0;
	this->impact_duration=0.0;
	this->impact_h=0.0;
	this->impact_v_tot=0.0;
	this->impact_vh=0.0;
	this->watervolume=0.0;
	this->impact_first_t=-1.0;
	this->fp_elem_id=-1;
	this->was_wet_flag=false;
	this->date_time = "";
	

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Impact_Values), _sys_system_modules::DAM_SYS);
}

//Default destructor
Dam_Impact_Values::~Dam_Impact_Values(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Impact_Values), _sys_system_modules::DAM_SYS);
}

//_______________
//public
//Get the impact value of waterlevel [m]
double Dam_Impact_Values::get_impact_h(void){
	return this->impact_h;
}
//Get the impact value of total flow velocity [m/s]
double Dam_Impact_Values::get_impact_v_tot(void){
	return this->impact_v_tot;
}
//Get the impact value of the wet period [s]
double Dam_Impact_Values::get_impact_duration(void){
	return this->impact_duration;
}
//Get the impact value of the product of waterlevel times flow velocity [m²/s]
double Dam_Impact_Values::get_impact_vh(void){
	return this->impact_vh;
}
//Get the impact value of the waterlevel rise [m/min]
double Dam_Impact_Values::get_impact_dsdt(void){
	return this->impact_dsdt;
}
//Get the watervolume [m³]
double Dam_Impact_Values::get_watervolume(void){
	return this->watervolume;
}
//Get the impact value of the first arrival time of water [s]
double Dam_Impact_Values::get_impact_first_t(void){
	return this->impact_first_t;
}
//Get the watervolume [m³]
int Dam_Impact_Values::get_fp_elem_id(void){
	return this->fp_elem_id;
}
//Get the flag if the hydraulic element was wet
bool Dam_Impact_Values::get_was_wet_flag(void){
	return this->was_wet_flag;
}
//Get the date-time string in instationary case
string Dam_Impact_Values::get_date_time_str(void) {
	return this->date_time;

}
//Read out the member data from a given QSqlTableModel
void Dam_Impact_Values::readout_data_from_database_model(QSqlQueryModel *model, const int model_index){
																		
	try{
		this->fp_elem_id=model->record(model_index).value((Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id)).c_str()).toInt();
		this->impact_h=model->record(model_index).value((Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_h_max)).c_str()).toDouble();
		this->impact_v_tot=model->record(model_index).value((Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vtot_max)).c_str()).toDouble();
		this->impact_dsdt=model->record(model_index).value((Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dsdt_max)).c_str()).toDouble();
		this->impact_duration=model->record(model_index).value((Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dur_wet)).c_str()).toDouble();
		this->impact_first_t=model->record(model_index).value((Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_t_first)).c_str()).toDouble();
		this->impact_vh=model->record(model_index).value((Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_hv_max)).c_str()).toDouble();
		this->watervolume=model->record(model_index).value((Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_end_vol)).c_str()).toDouble();
		if(this->impact_h>constant::meter_epsilon){
			this->was_wet_flag=true;
		}
		else{
			this->was_wet_flag=false;
		}
	}
	catch(Error msg){
		ostringstream info;
		throw msg;
	}
}
//Read out the member data from a given QSqlTableModel
void Dam_Impact_Values::readout_instat_data_from_database_model(QSqlQueryModel *model, const int model_index) {
	try {
		this->fp_elem_id = model->record(model_index).value((Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_id)).c_str()).toInt();
		this->impact_h = model->record(model_index).value((Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_h_max)).c_str()).toDouble();
		this->impact_v_tot = model->record(model_index).value((Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_vtot_max)).c_str()).toDouble();
		this->impact_dsdt = model->record(model_index).value((Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_dsdt_max)).c_str()).toDouble();
		this->date_time = model->record(model_index).value((Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::data_time)).c_str()).toString().toStdString();
		this->impact_vh = model->record(model_index).value((Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_hv_max)).c_str()).toDouble();
		if (this->impact_h > constant::meter_epsilon) {
			this->was_wet_flag = true;
		}
		else {
			this->was_wet_flag = false;
		}
	}
	catch (Error msg) {
		ostringstream info;
		throw msg;
	}

}
//Convert the enumerator (_dam_impact_type) to a text (static)
string Dam_Impact_Values::convert_dam_impact_type2txt(const _dam_impact_type value){
	string buff;

	switch(value){
		case(_dam_impact_type::WATER_H):
			buff=dam_label::impact_h;
			break;
		case(_dam_impact_type::WATER_V):
			buff=dam_label::impact_v;
			break;
		case(_dam_impact_type::WATER_D):
			buff=dam_label::impact_d;
			break;
		case(_dam_impact_type::WATER_VH):
			buff=dam_label::impact_vh;
			break;
		case(_dam_impact_type::WET_DRY):
			buff=dam_label::impact_wet_dry;
			break;
		default:
			buff=label::unknown_type;
	}

	return buff;
}
//Convert a text to the enumerator (_dam_impact_type) 
_dam_impact_type Dam_Impact_Values::convert_txt2dam_impact_type(const string txt){
	_dam_impact_type buff;

	if(txt==dam_label::impact_h){
		buff=_dam_impact_type::WATER_H;
	}
	else if(txt==dam_label::impact_v){
		buff=_dam_impact_type::WATER_V;
	}
	else if(txt==dam_label::impact_d){
		buff=_dam_impact_type::WATER_D;
	}
	else if(txt==dam_label::impact_vh){
		buff=_dam_impact_type::WATER_VH;
	}
	else if(txt==dam_label::impact_wet_dry){
		buff=_dam_impact_type::WET_DRY;
	}
	else{
		Error msg;
		msg.set_msg("Dam_Impact_Values::convert_dam_impact_type2txt(const string txt)","Can not convert the given text to an _dam_impact_type", "Check the given text", 3, false);
		ostringstream info;
		info<< "Given text :" << txt << endl;
		info<< "Possible type : " << endl;
		info << " " << dam_label::impact_h << endl;
		info << " " << dam_label::impact_v << endl;
		info << " " << dam_label::impact_d << endl;
		info << " " << dam_label::impact_vh << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return buff;

}