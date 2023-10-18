#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Cub_Dike_Landside.h"


//Default constructor
Fpl_Cub_Dike_Landside::Fpl_Cub_Dike_Landside(void){

	//points
	this->ptr_base_point=NULL;
	this->ptr_crest_point=NULL;
	this->number_points=0;


	this->total_length=0.0;
	this->total_horizontal=0.0;
	this->total_vertical=0.0;

	//segments
	this->ptr_segments=NULL;
	this->number_segments=0;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Cub_Dike_Landside), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Cub_Dike_Landside::~Fpl_Cub_Dike_Landside(void){

	this->delete_segments();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Cub_Dike_Landside), _sys_system_modules::FPL_SYS);//count the memory
}
//_________
//public
//Set the geometrie of the landside cubature per database table
void Fpl_Cub_Dike_Landside::set_segments(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point, QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	//output
	ostringstream cout;
	cout <<"Set the landside cubature of the dike... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	this->set_base_crest_point(base_point, crest_point);

	QSqlQueryModel results;

	Fpl_Section_Points::select_different_zone_points(&results, id, section_id, fpl_label::cub_landside , ptr_database, fpl_label::region_cubature, fpl_label::sec_dike);

	this->number_points=results.rowCount()+2;
	this->number_segments=this->number_points-1;

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
			//begin with the crest point land, end with the base point; no points between
			this->ptr_segments[0].set_points(this->ptr_crest_point, this->ptr_base_point);
		}
		else{
			//more than one segment
			Fpl_Section_Points landside_next(fpl_label::cub_landside);
			Fpl_Section_Points landside_before(fpl_label::cub_landside);
			for(int i=0; i< this->number_segments; i++){
				if(i==0){//begin with the crest point land then the landside number 0 
					landside_next.set_point_per_database(&results,i);
					this->ptr_segments[i].set_points(this->ptr_crest_point,&landside_next);
					landside_before=landside_next;
				}
				else if(i==this->number_segments-1){//end with the base point
					this->ptr_segments[i].set_points(&landside_before,this->ptr_base_point);
				}
				else{
					landside_next.set_point_per_database(&results,i);
					this->ptr_segments[i].set_points(&landside_before,&landside_next);
					landside_before=landside_next;
				}
			}
		}
	}
	catch(Error msg){
		throw msg;
	}
	this->calc_total_length();
}
//Set the geometrie of the landside cubature per file
void Fpl_Cub_Dike_Landside::set_segments(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point, QFile *ifile, int *line_counter){
	//read in
	string myline;
	QString qmyline;
	
	this->set_base_crest_point(base_point, crest_point);

	//find number of points
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		 if(myline==fpl_label::end_landside_cub){
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


	this->number_points=this->number_points+2;
	this->number_segments=this->number_points-1;
	try{
		this->allocate_segments();
	}
	catch(Error msg){
		throw msg;
	}


	//set the segments
	int counter_point=0;
	try{
		if(this->number_segments==1){
			//just one segment
			//begin with the base point water end with the crest point; no points between
			this->ptr_segments[0].set_points(this->ptr_crest_point,this->ptr_base_point);
		}
		else{
			//more than one segment
			Fpl_Section_Points next(fpl_label::cub_landside);
			Fpl_Section_Points before(fpl_label::cub_landside);
			do{

				qmyline=ifile->readLine();
				myline=qmyline.toStdString();
				(*line_counter)++;
				functions::clean_string(&myline);
				if(myline==fpl_label::end_landside_cub){
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



					if(counter_point==0){
						this->ptr_segments[counter_point].set_points(this->ptr_crest_point,&next);
						before=next;
					}
					else{

						this->ptr_segments[counter_point].set_points(&before,&next);
						before=next;
					}
					counter_point++;
				}

			}
			while(ifile->atEnd()==false && counter_point<this->number_points-2);
			this->ptr_segments[this->number_segments-1].set_points(&before,this->ptr_base_point);

		}
		if(counter_point!=this->number_points-2){
			Error msg=this->set_error(4);
			ostringstream info;
			info<<"Number points found in file : "<<counter_point<<endl;
			info<<"Number points required      : "<<this->number_points-2<<endl;
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
//Transfer the geomtrical data of the landside dike cubature to database
void Fpl_Cub_Dike_Landside::transfer_geometry2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){

	Fpl_Section_Points buffer;
	for(int i=0; i<this->number_segments-1; i++){
		buffer.set_point(this->ptr_segments[i].point2);
		buffer.set_zone_number(0);
		buffer.set_point_name(fpl_label::cub_landside);
		buffer.transfer_point2database(id, section_id, ptr_database,  fpl_label::region_cubature, fpl_label::sec_dike);
	}

}
//Output the geometrie of the landside to display/console
void Fpl_Cub_Dike_Landside::output_geometrie(void){
	ostringstream cout;
	cout<<"LANDSIDE CUBATURE GEOMETRY"<< endl;
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
//Get a pointer to the last point (:= base landside)
Geo_Point *Fpl_Cub_Dike_Landside::get_ptr_last_point(void){
	return &this->ptr_segments[this->number_segments-1].point2;
}
//Get a pointer to the last point (:= crest landside)
Geo_Point *Fpl_Cub_Dike_Landside::get_ptr_first_point(void){
	return &this->ptr_segments[0].point1;
}
//Get the total length
double Fpl_Cub_Dike_Landside::get_total_length(void){
	return this->total_length;
}
//Get the number of segments
int Fpl_Cub_Dike_Landside::get_number_segments(void){
	return this->number_segments;
}
//Get a pointer to a segments specified with an index
Fpl_Cub_Dike_Segment_Landside *Fpl_Cub_Dike_Landside::get_segment(const int index){
	if(index<0 || index>this->number_segments-1){
		return NULL;
	}
	else{
		return &this->ptr_segments[index];		
	}
}
//Calculate the middle of the gradient at the landside cubature
double Fpl_Cub_Dike_Landside::calculate_mid_gradient(void){
	double dist_horizontal=0.0;
	double dist_vertical=0.0;
	double mid_gradient=0.0;

	//loop over the real relevant segments
	for(int i=0; i<this->number_segments;i++){
		//sum it up over the segments
		dist_horizontal=dist_horizontal+this->ptr_segments[i].get_horizontal_dist();
		dist_vertical=dist_vertical+this->ptr_segments[i].get_vertical_dist();
	}
	//calculate mid gradient
	if(dist_horizontal!=0){
		mid_gradient=dist_vertical/dist_horizontal;
	}
	else{//quasi vertical (normaly not possible; see check_cubature)
		mid_gradient=10e10;
	}
	return mid_gradient;
}
//_________
//private
//Allocate the segments of the landside cubature
void Fpl_Cub_Dike_Landside::allocate_segments(void){
	try{
		this->ptr_segments=new Fpl_Cub_Dike_Segment_Landside[this->number_segments];
	}
	catch(bad_alloc &t){
		Error msg=set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the segments of the landside cubature
void Fpl_Cub_Dike_Landside::delete_segments(void){
	if(this->ptr_segments!=NULL){
		delete []this->ptr_segments;
		this->ptr_segments=NULL;
	}
}
//Calculate the total length as well as the vertical and horizontal distance 
void Fpl_Cub_Dike_Landside::calc_total_length(void){
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
//Set the pointer to the base point and the crest point
void Fpl_Cub_Dike_Landside::set_base_crest_point(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point){
	this->ptr_base_point=base_point;
	this->ptr_crest_point=crest_point;
}
//Check the cubature segments
void Fpl_Cub_Dike_Landside::check_cubature(void){
	for(int i=0; i< this->number_segments; i++){
			if(this->ptr_segments[i].get_gradient()>0.0){//gradient greater than 0
				Error msg=this->set_error(2);
				ostringstream info;
				info << "Segmentnumber " << i << " with a gradient of " << this->ptr_segments[i].get_gradient() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}

	}
}
//Find the key-values for the file input of the geometrical points
void Fpl_Cub_Dike_Landside::find_key_values_file(const string myline){
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
		Error msg=this->set_error(3);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set error(s)
Error Fpl_Cub_Dike_Landside::set_error(const int err_type){
		string place="Fpl_Cub_Dike_Landside::";
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
		case 2://gradient is greater than zero
			place.append("check_cubature(void)");
			reason="The gradient of the segments is greater 0; just a negative or a zero-gradient is possible";
			help="Check the landside cubature";
			type=11;
			break;
		case 3://wrong input
			place.append("find_key_values_file(const string myline)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the landside cubature in file";
			type=1;
			break;
		case 4://not all points found in file
			place.append("set_segments(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point, QFile *ifile, int *line_counter)");
			reason="Not all points are found in file";
			help="Check the information of the landside cubature geometry in file";
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
Warning Fpl_Cub_Dike_Landside::set_warning(const int warn_type){
	string place="Fpl_Cub_Dike_Landside::";
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