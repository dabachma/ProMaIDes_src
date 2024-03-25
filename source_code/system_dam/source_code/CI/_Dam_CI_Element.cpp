#include "Dam_Headers_Precompiled.h"
#include "_Dam_CI_Element.h"


int _Dam_CI_Element::counter_circ_end=0;
int _Dam_CI_Element::counter_lin_end = 0;

//Default constructor
_Dam_CI_Element::_Dam_CI_Element(void):block_elems(50){

	//Boundary value (waterlevel), when the CI element fails
	this->boundary_value = -9999;

	//Index of the floodplain, to which the CI element is connected
	this->index_fp=-1;
	//Index of the floodplain element, to which the CI element is connected
	this->index_fp_elem=-1;
	//Flag if CI element is connected to the hydraulic
	this->is_connected= false;
	//Global index of CI element in the database
	this->global_index = -1;
	//Incomíngs CI elements
	this->incomings=NULL;
	this->no_incoming = 0;
	//Outgoings CI elements
	this->outgoing=NULL;
	this->no_outgoing = 0;

	this->sector_id = -999;
	this->sector_name = label::not_set;
	this->active_flag = true;
	this->was_affected = false;
	//Failure type
	this->failure_type=dam_label::no_failure;
	this->failure_type_enum = _dam_ci_failure_type::no_failure;
	//Failure duration
	this->failure_duration=0.0;
	this->final_flag = false;
	this->recovery_time = 0.0;

	this->orig_recovery_time = 0.0;
	this->orig_activation_time = 0.0;


	this->activation_time=0.0;
	this->regular_flag =true;
	this->ptr_point = NULL;
	this->is_point_id = 0;
	this->stat_value = 0.0;
	this->stat_buff = 0.0;
	this->last_instat_required = false;



	
	this->current_id = 0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(_Dam_CI_Element)+ 2*sizeof(QList<QList<int>>), _sys_system_modules::DAM_SYS);
}
//Copy constructor
_Dam_CI_Element::_Dam_CI_Element(const _Dam_CI_Element& object) :block_elems(50) {
	//Boundary value (waterlevel), when the CI element fails
	this->boundary_value = object.boundary_value;
	this->final_flag = object.final_flag;

	//Index of the floodplain, to which the CI element is connected
	this->index_fp = object.index_fp;
	//Index of the floodplain element, to which the CI element is connected
	this->index_fp_elem = object.index_fp_elem;
	//Flag if CI element is connected to the hydraulic
	this->is_connected = object.is_connected;
	//Global index of CI element in the database
	this->global_index = object.global_index;
	//Incomings CI elements
	this->no_incoming = object.no_incoming;
	this->allocate_incomings();
	for (int i = 0; i < this->no_incoming; i++) {
		this->incomings[i] = object.incomings[i];

	}
	this->list_sec_incoming = object.list_sec_incoming;
	//Outgoings CI elements
	this->no_outgoing = object.no_outgoing;
	this->allocate_outgoing();
	for (int i = 0; i < this->no_outgoing; i++) {
		this->outgoing[i] = object.outgoing[i];

	}


	this->sector_id = object.sector_id;
	this->sector_name = object.sector_name;
	this->active_flag = object.active_flag;
	this->was_affected = object.was_affected;
	//Failure type
	this->failure_type = object.failure_type;
	this->failure_type_enum = object.failure_type_enum;
	//Failure duration
	this->failure_duration = object.failure_duration;
	this->recovery_time = object.recovery_time;
	this->activation_time = object.activation_time;
	this->regular_flag = object.regular_flag;
	this->ptr_point = object.ptr_point;
	this->orig_recovery_time = 0.0;
	this->orig_activation_time = object.orig_activation_time;
	this->orig_recovery_time = object.orig_recovery_time;
	this->is_point_id = object.is_point_id;
	this->stat_value = object.stat_value;
	this->stat_buff = object.stat_buff;
	this->last_instat_required = object.last_instat_required;




}
//Default destructor
_Dam_CI_Element::~_Dam_CI_Element(void){
	this->delete_incomings();
	this->delete_outgoing();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(_Dam_CI_Element) +   2*sizeof(QList<QList<int>>), _sys_system_modules::DAM_SYS);

}
//__________
//public
//Get element active flag
bool _Dam_CI_Element::get_element_active(void) {
	return this->active_flag;
}
//Set element active flag
void _Dam_CI_Element::set_element_active(const bool active) {
	this->active_flag = active;
	if (active == false) {
		this->was_affected = true;
	}

}
//Get the sector id
int _Dam_CI_Element::get_sector_id(void) {
	return this->sector_id;
}
//Get the sector name
string _Dam_CI_Element::get_sector_name(void) {
	return this->sector_name;

}
//Get the failure type enum
_dam_ci_failure_type _Dam_CI_Element::get_failure_type(void) {
	return this->failure_type_enum;

}
//Set the failure type enum
void _Dam_CI_Element::set_failure_type(const _dam_ci_failure_type type) {
	this->failure_type_enum = type;
	this->failure_type = _Dam_CI_Element::convert_failuretype2txt(this->failure_type_enum);

}
//Set the active flag
void _Dam_CI_Element::set_active_flag(const bool flag) {
	this->active_flag = flag;
}
///Set the activation time and flag
void _Dam_CI_Element::set_active_time_flag(const bool flag, const double time) {
	if (this->regular_flag == false) {
		if (flag == true) {
			this->activation_time = this->activation_time - time / (constant::day_second);
			if (this->activation_time <= 0.0000) {
				this->active_flag = flag;
			}
		}
		else {
			this->activation_time = this->activation_time + time / (constant::day_second);
			this->active_flag = flag;
		}

	}

}
//Set the was-affected flag
void _Dam_CI_Element::set_was_affected_flag(const bool flag) {
	this->was_affected = flag;
}
//Set the index of the connected FP-model
void _Dam_CI_Element::set_index_floodplain(const int index) {
	this->index_fp = index;
}
//Set the index of the connected FP-element
void _Dam_CI_Element::set_index_floodplain_element(const int index) {
	this->index_fp_elem = index;
}
//Get the index of the connected FP-model
int _Dam_CI_Element::get_index_floodplain(void) {
	return this->index_fp;

}
//Get the index of the connected FP-element
int _Dam_CI_Element::get_index_floodplain_element(void) {
	return this->index_fp_elem;

}
///Get connected flag
bool _Dam_CI_Element::get_connected_flag(void) {
	return this->is_connected;

}
//Decide according to the sector-id, if the element gets an endflag (static)
bool _Dam_CI_Element::sector_id2endflag(const int sec_id) {
	if (sec_id > 0 && sec_id <10) {

		return false;
	}
	else {
		return true;
	}

}
//Reset result values
void _Dam_CI_Element::reset_result_values(void) {
	this->active_flag = true;
	this->was_affected = false;
	//Failure type
	this->set_failure_type(_dam_ci_failure_type::no_failure);
	//Failure duration
	this->failure_duration = 0.0;
	this->activation_time=this->orig_activation_time;
	this->recovery_time=this->orig_recovery_time;

	if (this->regular_flag == false) {
		this->active_flag = false;
	}
}
//Add pointer of incoming CI-elements to list
void _Dam_CI_Element::add_incomings(_Dam_CI_Element* add) {
	if (this->no_incoming != 0 && this->no_incoming % this->block_elems != 0) {


		this->incomings[this->no_incoming] = add;
		this->no_incoming++;


	}
	else {
		//allocate a buffer
		_Dam_CI_Element **buffer = NULL;

		try {
			int full_blocks = (int)((this->no_incoming + 1) / this->block_elems);
			if (((this->no_incoming + 1) - full_blocks * this->block_elems) > 0) {
				full_blocks++;
			}
			buffer = new _Dam_CI_Element*[full_blocks*this->block_elems];
		}
		catch (bad_alloc &) {
			Error msg = this->set_error(0);
			throw msg;
		}
		//copy it
		for (int i = 0; i < this->no_incoming; i++) {
			buffer[i] = this->incomings[i];
		}
		//delete
		this->delete_incomings();
		//count it
		this->no_incoming++;

		this->incomings = buffer;


		//set the new one
		this->incomings[this->no_incoming - 1] = add;

	}


}
//Add pointer of outgoing CI-elements to list
void _Dam_CI_Element::add_outgoing(_Dam_CI_Element* add) {
	if (this->no_outgoing != 0 && this->no_outgoing % this->block_elems != 0) {


		this->outgoing[this->no_outgoing] = add;
		this->no_outgoing++;


	}
	else {
		//allocate a buffer
		_Dam_CI_Element **buffer = NULL;

		try {
			int full_blocks = (int)((this->no_outgoing + 1) / this->block_elems);
			if (((this->no_outgoing + 1) - full_blocks * this->block_elems) > 0) {
				full_blocks++;
			}
			buffer = new _Dam_CI_Element*[full_blocks*this->block_elems];
		}
		catch (bad_alloc &) {
			Error msg = this->set_error(1);
			throw msg;
		}
		//copy it
		for (int i = 0; i < this->no_outgoing; i++) {
			buffer[i] = this->outgoing[i];
		}
		//delete
		this->delete_outgoing();
		//count it
		this->no_outgoing++;

		this->outgoing = buffer;


		//set the new one
		this->outgoing[this->no_outgoing - 1] = add;

	}

}
//Get the number of outgoing CI-elements
int _Dam_CI_Element::get_number_outgoing(void) {
	return this->no_outgoing;

}
//Get the number of incoming CI-elements
int _Dam_CI_Element::get_number_incoming(void) {
	return this->no_incoming;

}
//Get the pointer to the outgoing CI-elements
_Dam_CI_Element **_Dam_CI_Element::get_outgoing_elements(void) {
	return this->outgoing;

}
///Init the sectors list of incoming and outgoing CI-elements
void _Dam_CI_Element::init_sec_list(void) {

	if (this->no_outgoing == 0) {
		this->final_flag = true;
	}

	//init incoming list
	bool found = false;
	bool first_found = false;
	for (int i = 0; i < this->no_incoming; i++) {
		if (this->incomings[i]->get_regular_flag() == true) {


			if (first_found == false) {
				QList<int> buff;
				buff.append(this->incomings[i]->get_sector_id());
				buff.append(i);
				this->list_sec_incoming.append(buff);
				first_found = true;
			}
			else {
				found = false;
				for (int j = 0; j < this->list_sec_incoming.count(); j++) {
					if (this->incomings[i]->get_sector_id() == this->list_sec_incoming[j].at(0)) {
						//add to existing one
						found = true;
						this->list_sec_incoming[j].append(i);
						break;
					}
				}
				if (found == false) {
					//make a new one
					QList<int> buff;
					buff.append(this->incomings[i]->get_sector_id());
					buff.append(i);
					this->list_sec_incoming.append(buff);
				}
			}
		}
	}

	//init emergency list
	found = false;
	first_found = false;
	for (int i = 0; i < this->no_incoming; i++) {
		if (this->incomings[i]->get_regular_flag() == false) {


			if (first_found == false) {
				QList<int> buff;
				buff.append(this->incomings[i]->get_sector_id());
				buff.append(i);
				this->list_sec_emergency.append(buff);
				first_found = true;
			}
			else {
				found = false;
				for (int j = 0; j < this->list_sec_incoming.count(); j++) {
					if (this->incomings[i]->get_sector_id() == this->list_sec_emergency[j].at(0)) {
						//add to existing one
						found = true;
						this->list_sec_emergency[j].append(i);
						break;
					}
				}
				if (found == false) {
					//make a new one
					QList<int> buff;
					buff.append(this->incomings[i]->get_sector_id());
					buff.append(i);
					this->list_sec_emergency.append(buff);
				}
			}
		}
	}
}
//Get final level flag
bool _Dam_CI_Element::get_end_level_flag(void) {
	return this->final_flag;

}
//Get failure duratcion
double _Dam_CI_Element::get_failure_duration(void) {

	return this->failure_duration;
}
///Get recovery time
double _Dam_CI_Element::get_recovery_time(void) {
	return this->recovery_time;
}
//Get activation time
double _Dam_CI_Element::get_activation_time(void) {

	return this->activation_time;
}
//Get regular flag
bool _Dam_CI_Element::get_regular_flag(void) {

	return this->regular_flag;
}
//Calculate indirect damages; here the loop is in!
void _Dam_CI_Element::calculate_indirect_damages_loop(void) {


	QList<_Dam_CI_Element *> my_list;
	ostringstream cout;
	my_list.append(this);
	this->calc_indirect_damages(my_list.last());



	do {
		Dam_Damage_System::check_stop_thread_flag();
		if (my_list.last()->current_id < my_list.last()->get_number_outgoing()) {
			my_list.last()->current_id++;

			_Dam_CI_Element *before1 = my_list.last();
			if (my_list.contains(my_list.last()->outgoing[my_list.last()->current_id - 1]) == false) {
				
				
				my_list.append(my_list.last()->outgoing[my_list.last()->current_id - 1]);
				this->calc_indirect_damages(my_list.last());

			}
			else {
				my_list.last()->current_id = 0;
				my_list.removeLast();
				continue;
			}
		}
		else {
			if (my_list.count() > 1) {
				my_list.last()->current_id = 0;

				my_list.removeLast();
			}
			else {

				if (this->current_id == this->get_number_outgoing()) {
					this->current_id = 0;
					break;
				}

			}



		}


	} while (1 == 1);

}
//Calculate indirect damages instationary; here the loop is in!
void _Dam_CI_Element::calculate_indirect_damages_instationary_loop(const double time) {


	QList<_Dam_CI_Element *> my_list;
	ostringstream cout;
	my_list.append(this);
	this->calc_indirect_damages_instationary(my_list.last(), time);



	do {
		Dam_Damage_System::check_stop_thread_flag();
		if (my_list.last()->current_id < my_list.last()->get_number_outgoing()) {
			my_list.last()->current_id++;

			_Dam_CI_Element *before1 = my_list.last();
			if (my_list.contains(my_list.last()->outgoing[my_list.last()->current_id - 1]) == false) {


				my_list.append(my_list.last()->outgoing[my_list.last()->current_id - 1]);
				this->calc_indirect_damages_instationary(my_list.last(), time);

			}
			else {
				my_list.last()->current_id = 0;
				my_list.removeLast();
				continue;
			}
		}
		else {
			if (my_list.count() > 1) {
				my_list.last()->current_id = 0;

				my_list.removeLast();
			}
			else {

				if (this->current_id == this->get_number_outgoing()) {
					this->current_id = 0;
					break;
				}

			}



		}


	} while (1 == 1);

}
///Convert string to failure type (_dam_ci_failure_type) (static)
_dam_ci_failure_type _Dam_CI_Element::convert_txt2failuretype(const string txt) {
	_dam_ci_failure_type buff;

	if (txt == dam_label::no_failure) {
		buff = _dam_ci_failure_type::no_failure;
	}
	else if (txt == dam_label::direct_failure) {
		buff = _dam_ci_failure_type::direct;
	}
	else if (txt == dam_label::sectoral_failure) {
		buff = _dam_ci_failure_type::sectoral;
	}
	else if (txt == dam_label::transsectoral_failure) {
		buff = _dam_ci_failure_type::transsectoral;
	}
	else if (txt == dam_label::direct_active_failure) {
		buff = _dam_ci_failure_type::direct_activ;
	}

	else {

		buff = _dam_ci_failure_type::undefined_failure;
	}
	
	return buff;

}
//Convert category (_dam_sc_category) to string (static)
string _Dam_CI_Element::convert_sector_id2txt(const _dam_ci_sector sec) {
	string buffer;
	switch (sec) {

	case _dam_ci_sector::electricity:
		buffer = dam_label::ci_elect;
		break;
	case _dam_ci_sector::information_technology:
		buffer = dam_label::ci_info_tec;
		break;
	case _dam_ci_sector::water_supply:
		buffer = dam_label::ci_water_sup;
		break;
	case _dam_ci_sector::water_treatment:
		buffer = dam_label::ci_water_treat;
		break;
	case _dam_ci_sector::energy:
		buffer = dam_label::ci_energy;
		break;
	case _dam_ci_sector::emergency_service:
		buffer = dam_label::ci_emerg;
		break;
	case _dam_ci_sector::health:
		buffer = dam_label::ci_health;
		break;
	case _dam_ci_sector::logistic_goods:
		buffer = dam_label::ci_logistic_goods;
		break;
	case _dam_ci_sector::logistic_personal:
		buffer = dam_label::ci_logistic_person;
		break;
	case _dam_ci_sector::goverment:
		buffer = dam_label::ci_goverment;
		break;
	case _dam_ci_sector::haz_material:
		buffer = dam_label::ci_haz_mat;
		break;
	case _dam_ci_sector::industry:
		buffer = dam_label::ci_industry;
		break;
	case _dam_ci_sector::cultur:
		buffer = dam_label::ci_cultur;
		break;
	case _dam_ci_sector::education:
		buffer = dam_label::ci_education;
		break;
	case _dam_ci_sector::jail:
		buffer = dam_label::ci_jail;
		break;

	default:
		buffer = label::not_defined;
	}
	return buffer;
}
//Transfer the sector id in the enum _dam_ci_sector (static)
_dam_ci_sector _Dam_CI_Element::convert_id2enum(const int id) {
	_dam_ci_sector buffer;
	switch (id) {
	case 1:
		buffer = _dam_ci_sector::electricity;
		break;
	case 2:
		buffer = _dam_ci_sector::information_technology;
		break;
	case 3:
		buffer = _dam_ci_sector::water_supply;
		break;
	case 4:
		buffer = _dam_ci_sector::water_treatment;
		break;
	case 5:
		buffer = _dam_ci_sector::energy;
		break;


	case 10:
		buffer = _dam_ci_sector::emergency_service;
		break;
	case 11:
		buffer = _dam_ci_sector::health;
		break;
	case 12:
		buffer = _dam_ci_sector::logistic_goods;
		break;
	case 13:
		buffer = _dam_ci_sector::logistic_personal;
		break;
	case 14:
		buffer = _dam_ci_sector::goverment;
		break;
	case 15:
		buffer = _dam_ci_sector::haz_material;
		break;
	case 16:
		buffer = _dam_ci_sector::industry;
		break;
	case 17:
		buffer = _dam_ci_sector::cultur;
		break;
	case 18:
		buffer = _dam_ci_sector::education;
		break;
	case 19:
		buffer = _dam_ci_sector::jail;
		break;

	default:
		buffer = _dam_ci_sector::undefined;
	}

	return buffer;
}
///Check the connection of the CI-elements
void _Dam_CI_Element::check_connections(void) {

	if (this->final_flag == true) {
		if (this->no_outgoing > 0) {
			for (int i = 0; i < this->no_outgoing; i++) {
				if (this->outgoing[i]->get_is_point_id() ==0) {
					Error msg = this->set_error(4);
					ostringstream info;
					info << "Sector id          : " << this->sector_id << endl;
					info << "Sector name        : " << this->sector_name << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}

	}
	
	if (this->final_flag == true) {
		if (this->no_outgoing > 0) {
			for (int i = 0; i < this->no_outgoing; i++) {
				if (this->outgoing[i]->get_sector_id() != this->sector_id) {
					Error msg = this->set_error(8);
					ostringstream info;
					info << "Sector id          : " << this->sector_id << endl;
					info << "Sector name        : " << this->sector_name << endl;
					info << "Sector id outgoing : " << this->outgoing[i]->get_sector_id() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
			this->final_flag = false;
		}

	}
	if (this->regular_flag == false) {
		if (this->no_incoming > 0) {
			Error msg = this->set_error(5);
			ostringstream info;
			info << "Sector id    : " << this->sector_id << endl;
			info << "Sector name  : " << this->sector_name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		for (int i = 0; i < this->no_outgoing; i++) {
			if (this->outgoing[i]->get_end_level_flag() == false) {
				Error msg = this->set_error(6);
				ostringstream info;
				info << "Sector id    : " << this->sector_id << endl;
				info << "Sector name  : " << this->sector_name << endl;
				info << "Name connected not final: " << this->outgoing[i]->get_ptr_point()->get_point_name() << endl;
				//info << " i " << i << " flag " << functions::convert_boolean2string(this->outgoing[i]->get_end_level_flag()) << endl;
				msg.make_second_info(info.str());
				throw msg;

			}
		}

	}
	if (this->final_flag==true && this->sector_id <= 5){
		for (int i = 0; i < this->no_incoming; i++) {
			if (this->incomings[i]->get_sector_id() != this->sector_id) {
				Error msg = this->set_error(7);
				ostringstream info;
				info << "Sector id             : " << this->sector_id << endl;
				info << "Sector name           : " << this->sector_name << endl;
				info << "Name		           :  " << this->ptr_point->get_point_name() << endl;
				info << "Sector id incoming    :" << this->incomings[i]->get_sector_id() << endl;
				info << "Sector name incoming  : " << this->incomings[i]->get_sector_name() << endl;
				msg.make_second_info(info.str());
				throw msg;

			}
		}


	}


}
//Get the pointer to the Geo_Point
Geo_Point *_Dam_CI_Element::get_ptr_point(void) {
	return this->ptr_point;

}
//Get the int if the element is a point (=0) or a polygon (=1)
int _Dam_CI_Element::get_is_point_id(void) {
	return this->is_point_id;
}
//Get the statistic value of the CI-element
double _Dam_CI_Element::get_stat_value(void) {
	return this->stat_value;
}
//Set the statistic value of the CI-element
void _Dam_CI_Element::set_stat_value(const double stat) {
	this->stat_value = stat;
}
///Get the statistic buffer value (sum_up) of the CI-element
double _Dam_CI_Element::get_stat_buffer_value(void) {
	return this->stat_buff;

}
//Set the statistic buffer value (sum_up)  of the CI-element
void _Dam_CI_Element::set_stat_buffer_value(const double stat) {
	this->stat_buff = stat;
}
//Get the flag if a last instationary calcuation step is required
bool _Dam_CI_Element::get_last_instat_required(void) {
	return this->last_instat_required;

}
//Set the flag if a last instationary calcuation step is required
void _Dam_CI_Element::set_last_instat_required(const bool flag) {
	this->last_instat_required = flag;

}
//Get number of outgoing final flag
double _Dam_CI_Element::get_number_outgoing_final(const int sec_id, const int point_id) {
	if (this->final_flag == true) {
		return 0.0;
	}
	double number = 0.0;
	int redundant = 0;
	for (int i = 0; i < this->no_outgoing; i++) {
		if (this->outgoing[i]->final_flag == true) {
			//number = number + 1;
			redundant=this->outgoing[i]->get_number_incoming_same_sec(sec_id, point_id)+1;
			
			number = number + 1/ (double)redundant;
		}

	}
	for (int i = 0; i < this->no_outgoing; i++) {
		number = number + this->outgoing[i]->get_number_outgoing_final(sec_id, point_id);
	}
	return number;
}
//Get number of incoming same sector id
int _Dam_CI_Element::get_number_incoming_same_sec(const int sec_id, const int point_id) {
	int number = -1;
	bool found = false;
	if (this->no_incoming == 0 ){
		number = 0;
		return number;
	}
	
	for (int i = 0; i < this->no_incoming; i++) {
		if (this->incomings[i]->get_sector_id() == sec_id) {
			number = number + 1;
			found = true;
		}
	}

	if (number == -1) {
		number = 0;
	}
	
	for (int i = 0; i < this->no_incoming; i++) {
		if (this->incomings[i]->ptr_point->get_number() != point_id) {
			if (found == false) {
				
				number = number + this->incomings[i]->get_number_incoming_same_sec(sec_id, point_id);
			}
			else {
				if (this->incomings[i]->get_sector_id() == sec_id){
					number = number + this->incomings[i]->get_number_incoming_same_sec(sec_id, point_id);

				}
			}
		}
	}

	return number;

}
//Calculate the cascade vulnerability value (CV)
void _Dam_CI_Element::calc_cv_value(void) {
	if (this->no_incoming == 0) {
		return;
	}
	
	QList<_Dam_CI_Element *> my_list;
	ostringstream cout;
	my_list.append(this);
	
	this->stat_buff = 1.0;
	this->cp_value_list.append(1.0);
	int buff_counter_start = 0;
	double sum=0.0;

	do {
		Dam_Damage_System::check_stop_thread_flag();
		if (my_list.last()->current_id < my_list.last()->get_number_incoming()) {
			my_list.last()->current_id++;
			if (my_list.count() == 1) {
				buff_counter_start++;
			}
			_Dam_CI_Element *before1 = my_list.last();
			if (my_list.contains(my_list.last()->incomings[my_list.last()->current_id - 1]) == false) {
				my_list.append(my_list.last()->incomings[my_list.last()->current_id - 1]);
				int buff_number = 0;
				//list of sectors
				QList<int> sec_ids;
				//make list of sector ids
				for (int i = 0; i < before1->no_incoming; i++) {
					if (sec_ids.contains(before1->incomings[i]->get_sector_id()) == false) {

						sec_ids.append(before1->incomings[i]->get_sector_id());
					}
				}
				//set the values per sector
				for (int i = 0; i < sec_ids.count(); i++) {
					buff_number = 0;
					//count numbers in sector
					for (int j = 0; j < before1->no_incoming; j++) {
						if (sec_ids.at(i) == before1->incomings[j]->get_sector_id()) {
							buff_number++;
						}
					}
					for (int j = 0; j < before1->no_incoming; j++) {
						if (sec_ids.at(i) == before1->incomings[j]->get_sector_id() && sec_ids.at(i) == my_list.last()->get_sector_id()) {

								my_list.last()->cp_value_list.append(before1->cp_value_list.last() / buff_number);
								
								break;
							
						}
					}
				}
			}
			else {

				
				_Dam_CI_Element::counter_circ_end++;
				if (_Dam_CI_Element::counter_circ_end % 10000 == 0) {
					//cout << _Dam_CI_Element::counter_circ_end + _Dam_CI_Element::counter_lin_end << " total end; " << _Dam_CI_Element::counter_circ_end << " circular end; list has currently " << my_list.count() << " members" << endl;
					//Sys_Common_Output::output_dam->output_txt(&cout,true);
				}

				sum = 0.0;
				_Dam_CI_Element *buff_next = NULL;
				//mark all the pointers in the line backward to the beginning
				for (int i = my_list.count() - 1; i >= 0; i--) {
					sum = sum + my_list.at(i)->cp_value_list.last();
					my_list.at(i)->cv_akkumulated.append(sum);
					if (i < my_list.count() - 1) {
						my_list.at(i)->cv_akkumulated_next.append(buff_next);
					}
					buff_next = my_list.at(i);

				}


				my_list.last()->current_id = 0;
				if (my_list.count() > 1) {
					my_list.removeLast();
				}
				else {

					sum = 0.0;
				}
				continue;
			}
		}
		else {

			if (my_list.last()->get_number_incoming() == 0) {
				_Dam_CI_Element::counter_lin_end++;
				if (_Dam_CI_Element::counter_lin_end % 10000 == 0) {
					//cout << _Dam_CI_Element::counter_circ_end + _Dam_CI_Element::counter_lin_end<< " total end; "<< _Dam_CI_Element::counter_lin_end << " linear end; list has currently " << my_list.count() << " members" << endl;
					//Sys_Common_Output::output_dam->output_txt(&cout,true);
				}
				sum = 0.0;
				_Dam_CI_Element *buff_next = NULL;
				//mark all the pointers in the line backward to the beginning
				for (int i = my_list.count() - 1; i >= 0; i--) {
					sum = sum + my_list.at(i)->cp_value_list.last();
					my_list.at(i)->cv_akkumulated.append(sum);
					if (i < my_list.count() - 1){
						my_list.at(i)->cv_akkumulated_next.append(buff_next);
					}
					buff_next = my_list.at(i);

				}

			}

			if (my_list.count() > 1){
				my_list.last()->current_id = 0;
				my_list.removeLast();
			}
			else {
				//reset sum
				sum = 0.0;

				if (this->current_id == this->get_number_incoming()) {
					this->current_id = 0;
					break;
				}
			}	
		}
	} while (1==1);
	//cout <<" DEBUG: No. starting elements " << buff_counter_start << endl;
	//Sys_Common_Output::output_dam->output_txt(&cout, true);
}
//Add up the CV-value
void _Dam_CI_Element::add_up_cv(void) {
	
	if (this->no_incoming == 0) {
		return;
	}
	double sum_buff = 0.0;
	ostringstream cout;
	_Dam_CI_Element *buff_elem = NULL;
	


	buff_elem = this;
	sum_buff = 0.0;
	int no_before = -1;
	QList< _Dam_CI_Element*> elem_list_check;
	elem_list_check.append(buff_elem);

	do {
		Dam_Damage_System::check_stop_thread_flag();
		buff_elem=_Dam_CI_Element::get_sum_value_element(buff_elem->cv_akkumulated, &sum_buff, buff_elem->cv_akkumulated_next, &elem_list_check);
		if (buff_elem == NULL) {

			break;
		}



	} while (1 == 1);
	


	this->set_stat_value(sum_buff);

}
//Sum and reset CP-value
void _Dam_CI_Element::sum_reset_cp_value(void) {
	if (this->no_incoming == 0) {
		return;
	}

	QList<_Dam_CI_Element *> my_list;
	ostringstream cout;
	my_list.append(this);



	do {
		Dam_Damage_System::check_stop_thread_flag();
		if (my_list.last()->current_id < my_list.last()->get_number_incoming()) {
			my_list.last()->current_id++;
			
			_Dam_CI_Element *before1 = my_list.last();
			if (my_list.contains(my_list.last()->incomings[my_list.last()->current_id - 1]) == false) {
				my_list.append(my_list.last()->incomings[my_list.last()->current_id - 1]);

				for (int i = 0; i < before1->no_incoming; i++) {

					before1->incomings[i]->stat_value = before1->incomings[i]->stat_value + before1->incomings[i]->stat_buff;
					before1->incomings[i]->stat_buff = 0.0;
				}
			}
			else {
				my_list.last()->current_id = 0;
				my_list.removeLast();
				continue;
			}
		}
		else {
			if (my_list.count() > 1) {
				my_list.last()->current_id = 0;

				my_list.removeLast();
			}
			else {

				if (this->current_id == this->get_number_incoming()) {
					this->current_id = 0;
					break;
				}

			}



		}


	} while (1 == 1);

}
//Get sum value and a pointer to the next CI_Element back from list (static)
_Dam_CI_Element* _Dam_CI_Element::get_sum_value_element(QList<double> sums, double *sum, QList<_Dam_CI_Element*> elem, QList<_Dam_CI_Element*> *check_in_list) {

	if (elem.count() == 0) {
		return NULL;
	}


	
	Dam_Damage_System::check_stop_thread_flag();
	//find min in sectors
	QList<int> buff_sec_list;
	for (int i = 0; i < elem.count(); i++) {
		if (buff_sec_list.contains(elem.at(i)->get_sector_id()) == false) {
			buff_sec_list.append(elem.at(i)->get_sector_id());
		}
	}
	QList<double> min_sec_list;
	QList<int> min_id_list;
	double buff_min = 0.0;
	int buff_min_id = 0;
	//go through sector_ search for minimum per sector
	for (int i = 0; i < buff_sec_list.count(); i++) {
		buff_min = 99999999999.9;
		for (int j = 0; j < elem.count(); j++) {
			if (buff_sec_list.at(i) == elem.at(j)->get_sector_id()) {
				if (buff_min > sums.at(j)) {
					buff_min = sums.at(j);
					buff_min_id = j;
				}
			}
		}
		min_sec_list.append(buff_min);
		min_id_list.append(buff_min_id);
	}
	//maximum over all sectors
	double buff_max = 0.0;
	int buff_max_id = 0;
	for (int i = 0; i < buff_sec_list.count(); i++) {
		
		if (buff_max < min_sec_list.at(i)) {
			buff_max = min_sec_list.at(i);
			buff_max_id = min_id_list.at(i);
		}
		

	}
	_Dam_CI_Element* buff_elem =elem.at(buff_max_id);
	if (check_in_list->contains(buff_elem) == true) {
		return NULL;
	}
	else {
		check_in_list->append(buff_elem);
	}



	double sum_buff = 0.0;
	for (int i = 0; i < elem.at(buff_max_id)->cp_value_list.count(); i++) {
		sum_buff = sum_buff + elem.at(buff_max_id)->cp_value_list.at(i);

	}
	if (sum_buff > 1) {
		sum_buff = 1;
	}

	*sum = *sum + sum_buff;
	
	return elem.at(buff_max_id);
}
//Copy operator
_Dam_CI_Element& _Dam_CI_Element::operator=(const _Dam_CI_Element& object) {
	//Boundary value (waterlevel), when the CI element fails
	this->boundary_value = object.boundary_value;
	this->final_flag = object.final_flag;

	//Index of the floodplain, to which the CI element is connected
	this->index_fp = object.index_fp;
	//Index of the floodplain element, to which the CI element is connected
	this->index_fp_elem = object.index_fp_elem;
	//Flag if CI element is connected to the hydraulic
	this->is_connected = object.is_connected;
	//Global index of CI element in the database
	this->global_index = object.global_index;

	//Incomings CI elements
	this->no_incoming = object.no_incoming;
	this->allocate_incomings();
	for (int i = 0; i < this->no_incoming; i++) {
		this->incomings[i] = object.incomings[i];

	}
	this->list_sec_incoming = object.list_sec_incoming;
	//Outgoings CI elements
	this->no_outgoing = object.no_outgoing;
	this->allocate_outgoing();
	for (int i = 0; i < this->no_outgoing; i++) {
		this->outgoing[i] = object.outgoing[i];

	}
	

	this->sector_id = object.sector_id;
	this->sector_name = object.sector_name;
	this->active_flag = object.active_flag;
	this->was_affected = object.was_affected;
	//Failure type
	this->failure_type = object.failure_type;
	this->failure_type_enum = object.failure_type_enum;
	//Failure duration  
	this->failure_duration = object.failure_duration;
	this->recovery_time = object.recovery_time;
	this->activation_time = object.activation_time;
	this->regular_flag = object.regular_flag;
	this->ptr_point = object.ptr_point;
	this->orig_activation_time = object.orig_activation_time;
	this->orig_recovery_time = object.orig_recovery_time;
	this->is_point_id = object.is_point_id;
	this->stat_value = object.stat_value;
	this->stat_buff = object.stat_buff; 
	this->last_instat_required = object.last_instat_required;
	

	return *this;
}
//_________
//protected
//Convert failure type (_dam_ci_failure_type) to string (static)
string _Dam_CI_Element::convert_failuretype2txt(const _dam_ci_failure_type type) {
	string buffer;
	switch (type) {

	case _dam_ci_failure_type::no_failure:
		buffer = dam_label::no_failure;
		break;
	case _dam_ci_failure_type::direct:
		buffer = dam_label::direct_failure;
		break;
	case _dam_ci_failure_type::sectoral:
		buffer = dam_label::sectoral_failure;
		break;
	case _dam_ci_failure_type::transsectoral:
		buffer = dam_label::transsectoral_failure;
		break;
	case _dam_ci_failure_type::direct_activ:
		buffer = dam_label::direct_active_failure;
		break;
	
	default:
		buffer = label::not_defined;
	}
	return buffer;

}
//Transfer the failure type in the enum _dam_ci_failure_type
_dam_ci_failure_type _Dam_CI_Element::convert_failuretype2enum(const int id) {
	_dam_ci_failure_type buffer;
	switch (id) {
	case 0:
		buffer = _dam_ci_failure_type::no_failure;
		break;
	case 1:
		buffer = _dam_ci_failure_type::direct;
		break;
	case 2:
		buffer = _dam_ci_failure_type::sectoral;
		break;
	case 3:
		buffer = _dam_ci_failure_type::transsectoral;
		break;
	case 4:
		buffer = _dam_ci_failure_type::direct_activ;
		break;
	

	default:
		buffer = _dam_ci_failure_type::undefined_failure;
	}

	return buffer;

}
//____________
//private
//Calculate indirect damages; here the method is in!
void _Dam_CI_Element::calc_indirect_damages(_Dam_CI_Element *current) {

	double buff_time = 0.0;
	QList<double> buff_list;
	QList<int> list_sec_failure;
	//if (this->failure_type_enum == _dam_ci_failure_type::no_failure) {
	if (current->failure_type_enum != _dam_ci_failure_type::direct) {


		for (int i = 0; i < current->list_sec_incoming.count(); i++) {
			buff_time = 0.0;
			for (int j = 1; j < current->list_sec_incoming[i].count(); j++) {
				if (current->incomings[current->list_sec_incoming[i].at(j)]->get_failure_type() == _dam_ci_failure_type::no_failure) {
					break;
				}
				else {
					if (j == 1) {
						buff_time = current->incomings[current->list_sec_incoming[i].at(j)]->get_failure_duration();
					}
					else {
						buff_time = min(buff_time, current->incomings[current->list_sec_incoming[i].at(j)]->get_failure_duration());
					}
				}
				if (j == current->list_sec_incoming[i].count() - 1) {
					buff_list.append(buff_time);
					current->was_affected = true;
					current->active_flag = false;
					if (current->sector_id == current->list_sec_incoming[i].at(0)) {
						current->set_failure_type(_dam_ci_failure_type::sectoral);
					}
					else {
						current->set_failure_type(_dam_ci_failure_type::transsectoral);
					}
					list_sec_failure.append(current->list_sec_incoming[i].at(0));
				}
			}
		}

		//check emergency structures
		for (int i = 0; i < current->list_sec_emergency.count(); i++) {

			for (int l = 0; l < list_sec_failure.count(); l++) {
				if (current->list_sec_emergency[i].at(0) == list_sec_failure.at(l)) {
					for (int j = 1; j < current->list_sec_emergency[i].count(); j++) {
						if (current->incomings[current->list_sec_emergency[i].at(j)]->get_failure_type() == _dam_ci_failure_type::no_failure) {
							if (current->incomings[current->list_sec_emergency[i].at(j)]->get_activation_time() < buff_list.at(l)) {
								current->incomings[current->list_sec_emergency[i].at(j)]->set_active_flag(true);
								current->incomings[current->list_sec_emergency[i].at(j)]->set_was_affected_flag(true);
								buff_list.replace(l, min(buff_list.at(l), current->incomings[current->list_sec_emergency[i].at(j)]->get_activation_time()));
								if (j > 2) {
									current->incomings[current->list_sec_emergency[i].at(j - 1)]->set_active_flag(false);
									current->incomings[current->list_sec_emergency[i].at(j - 1)]->set_was_affected_flag(false);
								}
							}
						}
						else {
							if (current->incomings[current->list_sec_emergency[i].at(j)]->get_activation_time() < buff_list.at(l)) {
								current->incomings[current->list_sec_emergency[i].at(j)]->set_active_flag(true);
								current->incomings[current->list_sec_emergency[i].at(j)]->set_was_affected_flag(true);
								current->incomings[current->list_sec_emergency[i].at(j)]->set_failure_type(_dam_ci_failure_type::direct_activ);
								buff_list.replace(l, min(buff_list.at(l), current->incomings[current->list_sec_emergency[i].at(j)]->get_recovery_time()));
								if (j > 2) {
									current->incomings[current->list_sec_emergency[i].at(j - 1)]->set_active_flag(false);
									current->incomings[current->list_sec_emergency[i].at(j - 1)]->set_was_affected_flag(false);
									current->incomings[current->list_sec_emergency[i].at(j - 1)]->set_failure_type(_dam_ci_failure_type::direct);
								}
							}
						}
					}

				}
			}
		}



		//set the duration of failure
		for (int i = 0; i < buff_list.count(); i++) {
			current->failure_duration = max(current->failure_duration, buff_list.at(i));

		}


	}
	//set the duration of failure for the direct affected CI-elements
	else if (current->failure_type_enum == _dam_ci_failure_type::direct) {
		buff_list.append(current->recovery_time);
		for (int i = 0; i < current->list_sec_incoming.count(); i++) {
			buff_time = 0.0;
			for (int j = 1; j < current->list_sec_incoming[i].count(); j++) {
				if (current->incomings[current->list_sec_incoming[i].at(j)]->get_failure_type() == _dam_ci_failure_type::no_failure) {
					break;
				}
				else {
					if (j == 1) {
						buff_time = current->incomings[current->list_sec_incoming[i].at(j)]->get_failure_duration();
					}
					else {
						buff_time = min(buff_time, current->incomings[current->list_sec_incoming[i].at(j)]->get_failure_duration());
					}
				}
				if (j == current->list_sec_incoming[i].count() - 1) {
					buff_list.append(buff_time);
				}
			}
		}
		//set the duration of failure
		for (int i = 0; i < buff_list.count(); i++) {
			current->failure_duration = max(current->failure_duration, buff_list.at(i));

		}


	}

}
//Calculate indirect damages instationary; here the method is in!
void _Dam_CI_Element::calc_indirect_damages_instationary(_Dam_CI_Element *current, const double time) {

	double buff_time = 0.0;
	QList<double> buff_list;
	QList<int> list_sec_failure;
	if (current->failure_type_enum != _dam_ci_failure_type::direct) {
		current->failure_duration = 0.0;


		for (int i = 0; i < current->list_sec_incoming.count(); i++) {
			buff_time = 0.0;
			for (int j = 1; j < current->list_sec_incoming[i].count(); j++) {
				if (current->incomings[current->list_sec_incoming[i].at(j)]->get_failure_type() == _dam_ci_failure_type::no_failure) {
					break;
				}
				else {
					if (j == 1) {
						buff_time = current->incomings[current->list_sec_incoming[i].at(j)]->get_failure_duration();
					}
					else {
						buff_time = min(buff_time, current->incomings[current->list_sec_incoming[i].at(j)]->get_failure_duration());
					}
				}
				if (j == current->list_sec_incoming[i].count() - 1) {
					buff_list.append(buff_time);
					current->was_affected = true;
					current->active_flag = false;
					if (current->sector_id == current->list_sec_incoming[i].at(0)) {
						current->set_failure_type(_dam_ci_failure_type::sectoral);
						current->set_active_flag(false);
					}
					else {
						current->set_failure_type(_dam_ci_failure_type::transsectoral);
						current->set_active_flag(false);
					}
					list_sec_failure.append(current->list_sec_incoming[i].at(0));
				}
			}
		}

		//check emergency structures
		for (int i = 0; i < current->list_sec_emergency.count(); i++) {
			for (int j = 1; j < current->list_sec_emergency[i].count(); j++) {
				current->incomings[current->list_sec_emergency[i].at(j)]->set_active_flag(false);
			}

			for (int l = 0; l < list_sec_failure.count(); l++) {
				if (current->list_sec_emergency[i].at(0) == list_sec_failure.at(l)) {
					for (int j = 1; j < current->list_sec_emergency[i].count(); j++) {
						if (current->incomings[current->list_sec_emergency[i].at(j)]->get_failure_type() == _dam_ci_failure_type::no_failure) {
							if (current->incomings[current->list_sec_emergency[i].at(j)]->get_activation_time() < buff_list.at(l)) {
								current->incomings[current->list_sec_emergency[i].at(j)]->set_active_time_flag(true, time);
								current->incomings[current->list_sec_emergency[i].at(j)]->set_was_affected_flag(true);
								buff_list.replace(l, min(buff_list.at(l), current->incomings[current->list_sec_emergency[i].at(j)]->get_activation_time()));
								if (j > 2) {
									current->incomings[current->list_sec_emergency[i].at(j - 1)]->set_active_time_flag(false, time);
									current->incomings[current->list_sec_emergency[i].at(j - 1)]->set_was_affected_flag(false);
								}
							}
						}
						else {
							if (current->incomings[current->list_sec_emergency[i].at(j)]->get_activation_time() < buff_list.at(l)) {
								current->incomings[current->list_sec_emergency[i].at(j)]->set_active_time_flag(true, time);;
								current->incomings[current->list_sec_emergency[i].at(j)]->set_was_affected_flag(true);
								current->incomings[current->list_sec_emergency[i].at(j)]->set_failure_type(_dam_ci_failure_type::direct_activ);
								buff_list.replace(l, min(buff_list.at(l), current->incomings[current->list_sec_emergency[i].at(j)]->get_recovery_time()));
								if (j > 2) {
									current->incomings[current->list_sec_emergency[i].at(j - 1)]->set_active_time_flag(false, time);;
									current->incomings[current->list_sec_emergency[i].at(j - 1)]->set_was_affected_flag(false);
									current->incomings[current->list_sec_emergency[i].at(j - 1)]->set_failure_type(_dam_ci_failure_type::direct);
								}
							}
						}
					}

				}
			}
		}



		//set the duration of failure
		for (int i = 0; i < buff_list.count(); i++) {
			current->failure_duration = max(current->failure_duration, buff_list.at(i));

		}
		if (current->failure_duration <= 0.0 + constant::sec_epsilon) {
			current->failure_duration = 0.0;
			current->set_failure_type(_dam_ci_failure_type::no_failure);
			current->set_active_flag(true);

		}


	}
	//set the duration of failure for the direct affected CI-elements
	else if (current->failure_type_enum == _dam_ci_failure_type::direct) {
		buff_list.append(current->recovery_time);
		current->failure_duration = 0.0;
		for (int i = 0; i < current->list_sec_incoming.count(); i++) {
			buff_time = 0.0;
			for (int j = 1; j < current->list_sec_incoming[i].count(); j++) {
				if (current->incomings[current->list_sec_incoming[i].at(j)]->get_failure_type() == _dam_ci_failure_type::no_failure) {
					break;
				}
				else {
					if (j == 1) {
						buff_time = current->incomings[current->list_sec_incoming[i].at(j)]->get_failure_duration();
					}
					else {
						buff_time = min(buff_time, current->incomings[current->list_sec_incoming[i].at(j)]->get_failure_duration());
					}
				}
				if (j == current->list_sec_incoming[i].count() - 1) {
					buff_list.append(buff_time);
				}
			}
		}
		//set the duration of failure
		for (int i = 0; i < buff_list.count(); i++) {
			current->failure_duration = max(current->failure_duration, buff_list.at(i));

		}


	}

}
//Allocate the incoming CI-elements
void _Dam_CI_Element::allocate_incomings(void) {
	try {
		this->incomings = new _Dam_CI_Element*[this->no_incoming];
	}
	catch (bad_alloc &t) {
		Error msg = this->set_error(2);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the incoming CI-elements
void _Dam_CI_Element::delete_incomings(void) {
	if (this->incomings != NULL) {
		delete[]this->incomings;
		this->incomings = NULL;
	}

}
//Allocate the outgoing CI-elements
void _Dam_CI_Element::allocate_outgoing(void) {
	try {
		this->outgoing = new _Dam_CI_Element*[this->no_outgoing];
	}
	catch (bad_alloc &t) {
		Error msg = this->set_error(3);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the outgoing CI-elements
void _Dam_CI_Element::delete_outgoing(void) {
	if (this->outgoing != NULL) {
		delete[]this->outgoing;
		this->outgoing = NULL;
	}

}
//Set error(s)
Error _Dam_CI_Element::set_error(const int err_type) {
	string place = "_Dam_CI_Element::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;

	switch (err_type) {
	case 0://bad alloc
		place.append("add_incomings(_Dam_CI_Element* add))");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 1://bad alloc
		place.append("add_outgoing(_Dam_CI_Element* add)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 2://bad alloc
		place.append("allocate_incomings(void");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 3://bad alloc
		place.append("allocate_outgoing(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 4://final has outgoings
		place.append("check_connections(void)");
		reason = "Final CI-elments has outgoing connections to another point element; this is not possible! Just polygon elements of the the section id are allowed.";
		help = "Check the connections";
		type = 34;
		break;
	case 5://incomings by emergence
		place.append("check_connections(void)");
		reason = "An emergency CI-point has incomings; this is not possible!";
		help = "Check the connections";
		type = 34;
		break;
	case 6://incomings by emergence
		place.append("check_connections(void)");
		reason = "An emergency CI-point has an outgoing CI-element which is not a final user; this is not possible!";
		help = "Check the connections";
		type = 34;
		break;
	case 7://
		place.append("check_connections(void)");
		reason = "An CI-polygon with sector id 1 - 4 is enduser; just incommings from the same sector are allowed";
		help = "Check the connections";
		type = 34;
		break;
	case 8://final has outgoings
		place.append("check_connections(void)");
		reason = "Final CI-elments has outgoing connections to another polygon element with different section id; this is not possible!";
		help = "Check the connections";
		type = 34;
		break;
	default:
		place.append("set_error(const int err_type)");
		reason = "Unknown flag!";
		help = "Check the flags";
		type = 6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}


