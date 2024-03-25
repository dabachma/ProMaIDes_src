#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Profil2Dgm_Converter.h"

//Default constructor
Hyd_Profil2Dgm_Converter::Hyd_Profil2Dgm_Converter(void){

	this->dia=NULL;
	this->profiles=NULL;
	this->add_streamline=NULL;
	this->density_stream=0.2;

	offset_x=-2300000;
	offset_y=-5600000;
	
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Profil2Dgm_Converter), _sys_system_modules::HYD_SYS);
}
///Default destructor
Hyd_Profil2Dgm_Converter::~Hyd_Profil2Dgm_Converter(void){

	this->no_prof=0;
	this->no_add_stream=0;
	this->dia=NULL;
	this->delete_profiles();
	this->delete_additional_streamlines();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Profil2Dgm_Converter), _sys_system_modules::HYD_SYS);
}
//_____________
//public
//Set the dialog pointer with the information about files, number of profiles and number of streamlines
void Hyd_Profil2Dgm_Converter::set_dia_ptr(HydGui_Profil2Dgm_Converter_Dia *ptr){
	this->dia=ptr;

	this->no_prof=this->dia->get_number_profiles_in_file();
	this->no_add_stream=this->dia->get_number_additional_streamlines();
	this->density_stream=this->dia->get_dens_streamline();
	offset_x=this->dia->get_offset_x();
	offset_y=this->dia->get_offset_y();

	this->file_prof=this->dia->get_file_path(0);
	this->file_stream_r=this->dia->get_file_path(1);
	this->file_stream_l=this->dia->get_file_path(2);

}
//Start the interpolation
void Hyd_Profil2Dgm_Converter::start_interpolation(void){

	ostringstream cout;
	cout << "Start the copnversion process..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	try{
		this->allocate_profiles();
		this->input_data_file();
		this->allocate_additional_stream();
		this->set_additional_stream_line_points();
		this->intersect_stream_line_profile();
		this->output_points_to_file();
	}
	catch(Error msg){
		throw msg;
	}
	



}
//_____________
//private
//Input the data from file
void Hyd_Profil2Dgm_Converter::input_data_file(void){
	try{

		ostringstream cout;
		cout << "Input "<< this->no_prof<<" of profiles from file "<< this->file_prof<<"..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->no_prof; i++){
			this->profiles[i].input_members(i, this->file_prof, this->offset_x, this->offset_y);
		}
		cout << "Input left streamline from file "<< this->file_stream_l<<"..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		this->stream_l.input_members(0, this->file_stream_l, this->offset_x, this->offset_y);
		cout << "Input right streamline from file "<< this->file_stream_r<<"..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		this->stream_r.input_members(0, this->file_stream_r, this->offset_x, this->offset_y);


		//check points
		if(this->stream_l.get_number_segments()!=this->stream_r.get_number_segments()){
			Error msg=this->set_error(1);
			ostringstream info;
			info<< "Number of points of left bank streamline " << this->stream_l.get_number_segments()+1 << endl;
			info<< "Number of points of right bank streamline " << this->stream_r.get_number_segments()+1 << endl;
			
			msg.make_second_info(info.str());
			throw msg;

		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Set additional stream line points on the segment between left and right streamline
void Hyd_Profil2Dgm_Converter::set_additional_stream_line_points(void){
	ostringstream cout;
	cout << "Set additional streamline points..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	Geo_Segment buffer;
	Geo_Point_List list;

	for(int i=0; i< this->stream_l.get_number_of_points(); i++){
		list.delete_list();
		buffer.set_points(this->stream_l.get_ptr_dike_line_points(i), this->stream_r.get_ptr_dike_line_points(i));
		buffer.get_additional_points_on_segment(&list, this->no_add_stream);

		if(list.get_number_points()!=this->no_add_stream){
			Error msg=this->set_error(3);
			ostringstream info;
			info<< "Number of additional points found " << list.get_number_points() << endl;
			info<< "Number of additional streamline given " << this->no_add_stream << endl;
			
			msg.make_second_info(info.str());
			throw msg;
		}

		//points to line
		for(int j=0; j< list.get_number_points();j++){
			this->add_streamline[j].get_ptr_dike_line_points(i)->set_point(list.get_ptr_point(j));

		}

	}
	ostringstream buff_stream;
	for(int i=0; i<this->no_add_stream; i++){
		buff_stream<<"Add_stream_"<<i;
		this->add_streamline[i].set_name(buff_stream.str());
		buff_stream.str("");
		this->add_streamline[i].set_dikelinepoints_to_segment();
		this->add_streamline[i].set_distances2begin();
	}

}
//Intersect streamlines with profiles
void Hyd_Profil2Dgm_Converter::intersect_stream_line_profile(void){
	ostringstream cout;
	cout << "Intersect streamlines with profiles..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	Geo_Interception_Point_List list;
	Hyd_Floodplain_Dikeline_Point_List list_point;
	Hyd_Floodplain_Dikeline_Point new_point;
	Geo_Segment *buff_seg=NULL;
	Hyd_Floodplain_Dikeline_Point *point1;
	Hyd_Floodplain_Dikeline_Point *point2;
	double h_inter=0.0;
	double tot_dist=0.0;
	double dist1=0.0;
	double dist2=0.0;
	double int_h=0.0;
	int index_last_found = 0;
	
	//left stream line
	for(int i=0;i<this->no_prof; i++){
		list.delete_list();
		for(int j=0; j<this->stream_l.get_number_segments(); j++){
			list.delete_list();
			this->profiles[i].calculate_segment_interception(&list, this->stream_l.get_segment(j));
			if(list.get_number_points()>0){
				for (int l = index_last_found; l < j + 1; l++) {
					new_point.set_abs_height(-9999.0);
					new_point.set_point(&(this->stream_l.get_segment(l)->point1));
					list_point.set_new_point(&new_point);
					index_last_found = j;
				}
			}

			for(int k=0; k<list.get_number_points();k++){
				point1=this->profiles[i].get_ptr_dike_line_points(list.get_point(k).index_is_intercept);
				point2=this->profiles[i].get_ptr_dike_line_points(list.get_point(k).index_is_intercept+1);
				tot_dist=this->profiles[i].get_segment(list.get_point(k).index_is_intercept)->get_distance();
				dist1=point1->distance(&(list.get_point(k).line_interception.interception_point));
				dist2=point2->distance(&(list.get_point(k).line_interception.interception_point));
				int_h=(1.0-dist1/tot_dist)*point1->get_abs_height()+(1.0-dist2/tot_dist)*point2->get_abs_height();
				new_point.set_abs_height(int_h);
				new_point.set_point(&(list.get_point(k).line_interception.interception_point));
				list_point.set_new_point(&new_point);
			}
		}
	}
	
	this->stream_l.set_dike_line_by_list(&list_point);
	this->stream_l.fill_marked_points_heights(-9999.0);
	this->stream_l.output_members();
	list_point.delete_list();
	index_last_found = 0;

	
	//right streamline
	for(int i=0;i<this->no_prof; i++){
		list.delete_list();
		for(int j=0; j<this->stream_r.get_number_segments(); j++){
			list.delete_list();
			this->profiles[i].calculate_segment_interception(&list, this->stream_r.get_segment(j));
			if(list.get_number_points()>0){
				for (int l = index_last_found; l < j + 1; l++) {
					new_point.set_abs_height(-9999.0);
					new_point.set_point(&(this->stream_r.get_segment(l)->point1));
					list_point.set_new_point(&new_point);
					index_last_found = j;
				}
			}

			for(int k=0; k<list.get_number_points();k++){
				point1=this->profiles[i].get_ptr_dike_line_points(list.get_point(k).index_is_intercept);
				point2=this->profiles[i].get_ptr_dike_line_points(list.get_point(k).index_is_intercept+1);
				tot_dist=this->profiles[i].get_segment(list.get_point(k).index_is_intercept)->get_distance();
				dist1=point1->distance(&(list.get_point(k).line_interception.interception_point));
				dist2=point2->distance(&(list.get_point(k).line_interception.interception_point));
				int_h=(1.0-dist1/tot_dist)*point1->get_abs_height()+(1.0-dist2/tot_dist)*point2->get_abs_height();
				new_point.set_abs_height(int_h);
				new_point.set_point(&(list.get_point(k).line_interception.interception_point));
				list_point.set_new_point(&new_point);
			}
		}
	}
	
	this->stream_r.set_dike_line_by_list(&list_point);
	this->stream_r.fill_marked_points_heights(-9999.0);
	this->stream_r.output_members();
	list_point.delete_list();
	index_last_found = 0;


	//additional streamline+
	for(int l=0; l<this->no_add_stream; l++){
		for(int i=0;i<this->no_prof; i++){
			list.delete_list();
			for(int j=0; j<this->add_streamline[l].get_number_segments(); j++){
				list.delete_list();
				this->profiles[i].calculate_segment_interception(&list, this->add_streamline[l].get_segment(j));
				if(list.get_number_points()>0){
					for (int z = index_last_found; z < j + 1; z++) {
						new_point.set_abs_height(-9999.0);
						new_point.set_point(&(this->add_streamline[l].get_segment(z)->point1));
						list_point.set_new_point(&new_point);
						index_last_found = j;
					}
				}

				for(int k=0; k<list.get_number_points();k++){
					point1=this->profiles[i].get_ptr_dike_line_points(list.get_point(k).index_is_intercept);
					point2=this->profiles[i].get_ptr_dike_line_points(list.get_point(k).index_is_intercept+1);
					tot_dist=this->profiles[i].get_segment(list.get_point(k).index_is_intercept)->get_distance();
					dist1=point1->distance(&(list.get_point(k).line_interception.interception_point));
					dist2=point2->distance(&(list.get_point(k).line_interception.interception_point));
					int_h=(1.0-dist1/tot_dist)*point1->get_abs_height()+(1.0-dist2/tot_dist)*point2->get_abs_height();
					new_point.set_abs_height(int_h);
					new_point.set_point(&(list.get_point(k).line_interception.interception_point));
					list_point.set_new_point(&new_point);
				}
			}
		}
		
		this->add_streamline[l].set_dike_line_by_list(&list_point);
		this->add_streamline[l].fill_marked_points_heights(-9999.0);
		this->add_streamline[l].output_members();
		list_point.delete_list();
		index_last_found = 0;


	}

}
//Output points to file
void Hyd_Profil2Dgm_Converter::output_points_to_file(void){
	string file_name;
	ostringstream buff_f_name;
	buff_f_name<<functions::get_file_path(this->file_prof)<<"/interpolate_"<<functions::get_file_name(this->file_prof);
	file_name=buff_f_name.str();

	Geo_Point_List list;
	Geo_Point *buff_p;
	ofstream file;
	//open the file
	file.open(file_name.c_str());
	if(file.is_open()==false){
		Error msg=this->set_error(4);
		ostringstream info;
		info << "Filename " << file_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	ostringstream cout;
	cout << "Output DGM-W to file "<<file_name<<"..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);

    file<<"x,  "<<"y,   "<<"z   "<<endl;

	double tot_dist=0.0;
	double dist=0.0;
	double dist2=0.0;
	double int_h=0.0;

	//left stream line
	for(int i=0; i< this->stream_l.get_number_of_points(); i++){


        file<<P(8) << FORMAT_FIXED_REAL<<this->stream_l.get_ptr_dike_line_points(i)->get_xcoordinate()-this->offset_x<<",   "<<this->stream_l.get_ptr_dike_line_points(i)->get_ycoordinate()-this->offset_y<<",    "<<this->stream_l.get_ptr_dike_line_points(i)->get_abs_height()<<endl;
		if(i<this->stream_l.get_number_of_points()-1){
			list.delete_list();

			this->stream_l.get_segment(i)->get_additional_points_on_segment(&list, this->density_stream);
			tot_dist=this->stream_l.get_segment(i)->get_distance();
			for(int j=0; j<list.get_number_points(); j++){
				buff_p=list.get_ptr_point(j);
				dist=this->stream_l.get_segment(i)->get_distance_along_segment(buff_p);
				dist2=tot_dist-dist;
				int_h=(1.0-dist/tot_dist)*this->stream_l.get_ptr_dike_line_points(i)->get_abs_height()+(1.0-dist2/tot_dist)*this->stream_l.get_ptr_dike_line_points(i+1)->get_abs_height();
                file<<P(8) << FORMAT_FIXED_REAL<<buff_p->get_xcoordinate()-this->offset_x<<",   "<<buff_p->get_ycoordinate()-this->offset_y<<",    "<<int_h<<endl;
			}
		}
	
	}
	//right stream line
	for(int i=0; i< this->stream_r.get_number_of_points(); i++){

        file<<P(8) << FORMAT_FIXED_REAL<<this->stream_r.get_ptr_dike_line_points(i)->get_xcoordinate()-this->offset_x<<",   "<<this->stream_r.get_ptr_dike_line_points(i)->get_ycoordinate()-this->offset_y<<",    "<<this->stream_r.get_ptr_dike_line_points(i)->get_abs_height()<<endl;
		if(i<this->stream_r.get_number_of_points()-1){
			list.delete_list();
			this->stream_r.get_segment(i)->get_additional_points_on_segment(&list, this->density_stream);
			tot_dist=this->stream_r.get_segment(i)->get_distance();
			for(int j=0; j<list.get_number_points(); j++){
				buff_p=list.get_ptr_point(j);
				dist=this->stream_r.get_segment(i)->get_distance_along_segment(buff_p);
				dist2=tot_dist-dist;
				int_h=(1.0-dist/tot_dist)*this->stream_r.get_ptr_dike_line_points(i)->get_abs_height()+(1.0-dist2/tot_dist)*this->stream_r.get_ptr_dike_line_points(i+1)->get_abs_height();
                file<<P(8) << FORMAT_FIXED_REAL<<buff_p->get_xcoordinate()-this->offset_x<<",   "<<buff_p->get_ycoordinate()-this->offset_y<<",    "<<int_h<<endl;
			}
		}
	
	}

	//additionoal streamline
for(int l=0; l<this->no_add_stream;l++){
	
	for(int i=0; i< this->add_streamline[l].get_number_of_points(); i++){
        file<<P(8) << FORMAT_FIXED_REAL<<this->add_streamline[l].get_ptr_dike_line_points(i)->get_xcoordinate()-this->offset_x<<",   "<<this->add_streamline[l].get_ptr_dike_line_points(i)->get_ycoordinate()-this->offset_y<<",    "<<this->add_streamline[l].get_ptr_dike_line_points(i)->get_abs_height()<<endl;
		if(i<this->add_streamline[l].get_number_of_points()-1){
			list.delete_list();
			this->add_streamline[l].get_segment(i)->get_additional_points_on_segment(&list, this->density_stream);
			tot_dist=this->add_streamline[l].get_segment(i)->get_distance();
			for(int j=0; j<list.get_number_points(); j++){
				buff_p=list.get_ptr_point(j);
				dist=this->add_streamline[l].get_segment(i)->get_distance_along_segment(buff_p);
				dist2=tot_dist-dist;
				int_h=(1.0-dist/tot_dist)*this->add_streamline[l].get_ptr_dike_line_points(i)->get_abs_height()+(1.0-dist2/tot_dist)*this->add_streamline[l].get_ptr_dike_line_points(i+1)->get_abs_height();
                file<<P(8) << FORMAT_FIXED_REAL<<buff_p->get_xcoordinate()-this->offset_x<<",   "<<buff_p->get_ycoordinate()-this->offset_y<<",    "<<int_h<<endl;
			}
		}
	
	}

}


	file.close();

}
//Allocate the polysegments for the profiles
void Hyd_Profil2Dgm_Converter::allocate_profiles(void){
	
	this->delete_profiles();
	try{
		this->profiles=new Hyd_Floodplain_Polysegment[this->no_prof];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the polysegments for the profiles
void Hyd_Profil2Dgm_Converter::delete_profiles(void){
	if(this->profiles!=NULL){
		delete []this->profiles;
		this->profiles=NULL;
	}
}
//Allocate additional streamlines
void Hyd_Profil2Dgm_Converter::allocate_additional_stream(void){
	this->delete_additional_streamlines();
	try{
		this->add_streamline=new Hyd_Floodplain_Polysegment[this->no_add_stream];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	for(int i=0; i< this->no_add_stream; i++){
		this->add_streamline[i].set_number_of_points(this->stream_l.get_number_of_points());
	}
}
//Delete additional streamlines
void Hyd_Profil2Dgm_Converter::delete_additional_streamlines(void){
	if(this->add_streamline!=NULL){
		delete []this->add_streamline;
		this->add_streamline=NULL;
	}
}
//set the error
Error Hyd_Profil2Dgm_Converter::set_error(const int err_type){
	string place="Hyd_Profil2Dgm_Converter::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;
	switch (err_type){
		case 0://bad alloc
			place.append("allocate_profiles(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://not the same number of points
			place.append("input_data_file(void)");
			reason="The number of points in the given streamline has to be equal.";
			help="Check the left and right bank streamline";
			type=11;
			break;
		case 2://bad alloc
			place.append("allocate_additional_stream(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://not all points found
			place.append("set_additional_stream_line_points(void)");
			reason="Not all additional streamline points are found ";
			help="Check the streamlines";
			type=10;
			break;
		case 4://could not open the tecplot file
			place.append("output_points_to_file(void)");
			reason="Could not open the file for output";
			help="Check the file";
			type=5;
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
//Set the warning
Warning Hyd_Profil2Dgm_Converter::set_warning(const int warn_type){
	string place="Hyd_Profil2Dgm_Converter::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://a multithreading is not possible 
			place.append("set_additional_stream_line_points(void)") ;
			reason="Not all additional streamline points are found ";
			reaction="No reaction";
			help= "Check the streamlines";
			type=9;	
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
