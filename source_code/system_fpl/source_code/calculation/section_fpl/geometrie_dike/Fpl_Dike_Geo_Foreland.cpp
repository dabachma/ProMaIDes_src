#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Dike_Geo_Foreland.h"

//Default constructor
Fpl_Dike_Geo_Foreland::Fpl_Dike_Geo_Foreland(void){

	this->number_points=0;
	this->total_length=0.0;
	this->total_horizontal=0.0;
	this->total_vertical=0.0;

	//segments
	this->ptr_segments=NULL;
	this->number_segments=0;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Dike_Geo_Foreland), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Dike_Geo_Foreland::~Fpl_Dike_Geo_Foreland(void){

	this->delete_segments();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Dike_Geo_Foreland), _sys_system_modules::FPL_SYS);//count the memory
}
//__________
//public
//Set the geometrie of the the foreland geometry per database table
void Fpl_Dike_Geo_Foreland::set_segments(Fpl_Section_Points *base_point, QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	//output
	ostringstream cout;
	cout <<"Set the foreland geometry of the dike... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);

	QSqlQueryModel results;

	Fpl_Section_Points::select_different_zone_points(&results, id, section_id, fpl_label::foreland , ptr_database, fpl_label::region_surround, fpl_label::sec_dike);

	this->number_points=results.rowCount()+1;
	this->number_segments=this->number_points-1;
	if(this->number_segments==0){
		this->number_points=0;
		return;
	}

	try{
		this->allocate_segments();
	}
	catch(Error msg){
		throw msg;
	}

	//set the segments
	try{
		if(this->number_segments==1){
			//just one segment
			Fpl_Section_Points next(fpl_label::foreland);
			next.set_point_per_database(&results,0);
			//begin with the first, end with the base point; no points between
			this->ptr_segments[0].set_points(&next, base_point);
		}
		else{
			//more than one segment
			Fpl_Section_Points next(fpl_label::foreland);
			Fpl_Section_Points before(fpl_label::foreland);
			for(int i=0; i< this->number_segments+1; i++){
				if(i==0){//just read in po
					next.set_point_per_database(&results,i);
					before=next;
				}
				else if(i==this->number_segments){//end with the base point
					this->ptr_segments[i-1].set_points(&before,base_point);
				}
				else{
					next.set_point_per_database(&results,i);
					this->ptr_segments[i-1].set_points(&before,&next);
					before=next;
				}
			}
		}
	}
	catch(Error msg){
		throw msg;
	}
	this->calc_total_length();
}
//Set the geometrie of the foreland geometry per file
void Fpl_Dike_Geo_Foreland::set_segments(Fpl_Section_Points *base_point,  QFile *ifile, int *line_counter){
	//read in
	string myline;
	QString qmyline;
	

	//find number of points
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::end_foreland){
			break;
		}
		if(myline.empty()==false){
			this->find_key_values_file(myline);
		}
		if(this->number_points>0){
			break;
		}
	}
	while(ifile->atEnd()==false);


	this->number_points=this->number_points+1;
	this->number_segments=this->number_points-1;
	
	if(this->number_segments==0){
		this->number_points=0;
		return;
	}

	try{
		this->allocate_segments();
	}
	catch(Error msg){
		throw msg;
	}


	//set the segments
	int counter_point=0;
	int counter_segment=0;
	try{

		//more than one segment
		Fpl_Section_Points next(fpl_label::foreland);
		Fpl_Section_Points before(fpl_label::foreland);
		do{

			qmyline=ifile->readLine();
			myline=qmyline.toStdString();
			(*line_counter)++;
			functions::clean_string(&myline);
			if(myline==fpl_label::end_foreland){
				break;
			}
			else if(myline.empty()==false){
				double buff_x=0.0;
				double buff_y=0.0;
				stringstream point_buff;
				point_buff << myline;
				point_buff >> buff_x >> buff_y;

				
				if(point_buff.fail()==true){
					Error msg=this->set_error(15);
					ostringstream info;
					info << "Read line : " << myline << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
				next.set_point_coordinate(buff_x, buff_y);



				if(counter_point==0 && this->number_segments==1){
					this->ptr_segments[counter_point].set_points(&next, base_point);
					before=next;
				}
				else if(counter_point==0 && this->number_segments>1){
					before=next;
				}
				else{

					this->ptr_segments[counter_segment].set_points(&before,&next);
					before=next;
					counter_segment++;
					
				}
				counter_point++;
			}

		}
		while(ifile->atEnd()==false && counter_segment<this->number_segments);
		if(this->number_segments>1){
			this->ptr_segments[this->number_segments-1].set_points(&before, base_point);
		}

		if(counter_point!=this->number_points-1){
			Error msg=this->set_error(3);
			ostringstream info;
			info<<"Number points found in file : "<<counter_point<<endl;
			info<<"Number points required      : "<<this->number_points-1<<endl;
			msg.make_second_info(info.str());
			throw msg;
		
		}


		
		//check the cubature
		this->check_cubature();
	}
	catch(Error msg){
		throw msg;
	}
	this->calc_total_length();
}
//Transfer the geomtrical data of the foreland geometry to database
void Fpl_Dike_Geo_Foreland::transfer_geometry2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){

	Fpl_Section_Points buffer;
	for(int i=0; i<this->number_segments; i++){
		buffer.set_point(this->ptr_segments[i].point1);
		buffer.set_zone_number(0);
		buffer.set_point_name(fpl_label::foreland);
		buffer.transfer_point2database(id, section_id, ptr_database,  fpl_label::region_surround, fpl_label::sec_dike);
	}
}
//Output the geometrie of the foreland geometry to display/console
void Fpl_Dike_Geo_Foreland::output_geometrie(void){
	ostringstream cout;
	cout<<"FORELAND GEOMETRY"<< endl;
	cout <<" Number points (segments)      : " << this->number_points <<" ("<<this->number_segments<<")"<< endl;
	cout <<" Horizontal distance           : " << this->total_horizontal << label::m <<endl;
	cout <<" Vertical distance             : " << this->total_vertical << label::m << endl;
	cout <<" Distance                      : " << this->total_length << label::m << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	cout<<"SEGMENTS:"<< endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	//output the segments
	for(int i=0;i<this->number_segments;i++){
		this->ptr_segments[i].output_members(&cout);
	}
}
//Get a pointer to the first point
Geo_Point* Fpl_Dike_Geo_Foreland::get_ptr_first_point(void){
 
	if(this->number_segments>0){
		return &this->get_segment(0)->point1;
	}
	else{
		return NULL;
	}
}
//Get the number of segments
int Fpl_Dike_Geo_Foreland::get_number_segments(void){
	return this->number_segments;
}
//Get a pointer to a segments specified with an index
Fpl_Dike_Segment *Fpl_Dike_Geo_Foreland::get_segment(const int index){
	if(index<0 || index>this->number_segments-1){
		return NULL;
	}
	else{
		return &this->ptr_segments[index];		
	}
}
//Get the total horizontal length
double Fpl_Dike_Geo_Foreland::get_tot_hori_length(void){
	return this->total_horizontal;
}
//Get the total vertical length
double Fpl_Dike_Geo_Foreland::get_tot_vert_length(void){
	return this->total_vertical;
}
//_________
//private
//Allocate the segments of the foreland geometry
void Fpl_Dike_Geo_Foreland::allocate_segments(void){
	try{
		this->ptr_segments=new Fpl_Dike_Segment[this->number_segments];
	}
	catch(bad_alloc &t){
		Error msg=set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the segments of the foreland geometry
void Fpl_Dike_Geo_Foreland::delete_segments(void){
	if(this->ptr_segments!=NULL){
		delete []this->ptr_segments;
		this->ptr_segments=NULL;
	}
}
//Calculate the total length as well as the vertical and horizontal distance 
void Fpl_Dike_Geo_Foreland::calc_total_length(void){
	this->total_length=0.0;
	this->total_horizontal=0.0;
	this->total_vertical=0.0;

	for(int i=0;i<this->number_segments;i++){
		//set the points to a segment
		this->total_length=this->total_length+this->ptr_segments[i].get_distance();
		this->total_horizontal=this->total_horizontal+this->ptr_segments[i].get_horizontal_dist();
		this->total_vertical=this->total_vertical+this->ptr_segments[i].get_vertical_dist();
	}
}
//Check the cubature segments
void Fpl_Dike_Geo_Foreland::check_cubature(void){
	
}
//Find the key-values for the file input of the geometrical points
void Fpl_Dike_Geo_Foreland::find_key_values_file(const string myline){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(fpl_label::key_no_points);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_no_points.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->number_points;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(2);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set error(s)
Error Fpl_Dike_Geo_Foreland::set_error(const int err_type){
		string place="Fpl_Dike_Geo_Foreland::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("allocate_segments(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://wrong input
			place.append("find_key_values_file(const string myline)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the foreland geometry in file";
			type=1;
			break;
		case 3://not all points found in file
			place.append("set_segments(Fpl_Section_Points *base_point,  QFile *ifile, int *line_counter)");
			reason="Not all points are found in file";
			help="Check the information of the foreland geometry in file";
			type=1;
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
//Set warning(s)
Warning Fpl_Dike_Geo_Foreland::set_warning(const int warn_type){
	string place="Fpl_Dike_Geo_Foreland::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://do not find any interception point
			//place.append("calculate_intercept_...cubature...(Geo_Straight_Line *ptr_relv_waterlevel)");
			//reason="Do not find any interception point with the waterside cubature";
			//help="Check the waterside cubature and the relevant line";
			//reaction="The whole cubature is taken";
			//msg.set_during_calculation_flag(false);
			//type=9;
			//break;
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