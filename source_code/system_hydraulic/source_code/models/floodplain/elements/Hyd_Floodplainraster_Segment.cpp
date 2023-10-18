#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Floodplainraster_Segment.h"

//constructor
Hyd_Floodplainraster_Segment::Hyd_Floodplainraster_Segment(void){
		
	this->segment_index=-1;
	this->index_point_1=-1;
	this->index_point_2=-1;
	this->type=_hyd_floodplain_raster_segment_type::BORDER;
	this->ptr_floodplain_elem=NULL;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Floodplainraster_Segment)-sizeof(Geo_Segment), _sys_system_modules::HYD_SYS);


}
//destructor
Hyd_Floodplainraster_Segment::~Hyd_Floodplainraster_Segment(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Floodplainraster_Segment)-sizeof(Geo_Segment), _sys_system_modules::HYD_SYS);

}
//____________
//public
//Set the type of the segment
void Hyd_Floodplainraster_Segment::set_segmenttype(_hyd_floodplain_raster_segment_type type){
	this->type=type;
}
//Set a pointer to Hyd_Element_Floodplain 
void Hyd_Floodplainraster_Segment::set_element_ptr(Hyd_Element_Floodplain *element){
	this->ptr_floodplain_elem=element;
}
//Set the index in the raster of the segment
void Hyd_Floodplainraster_Segment::set_segment_index(const int index){
	this->segment_index=index;
}
//Get the index in the raster of the segment
int Hyd_Floodplainraster_Segment::get_segment_index(void){
	return this->segment_index;
}
//Set the index in the raster of the first point
void Hyd_Floodplainraster_Segment::set_firstpoint_index(const int index){
	this->index_point_1=index;
}
//Get the index in the raster of the first point
int Hyd_Floodplainraster_Segment::get_firstpoint_index(void){
	return this->index_point_1;
}
//Set the index in the raster of the second point
void Hyd_Floodplainraster_Segment::set_secondpoint_index(const int index){
	this->index_point_2=index;
}
//Get the index in the raster of the second point
int Hyd_Floodplainraster_Segment::get_secondpoint_index(void){
	return this->index_point_2;
}
//Output the header for the members
void Hyd_Floodplainraster_Segment::output_header(ostringstream *cout){

	*cout <<W(8) << "Index "<< W(8) << "Index 1" << W(8) << "Index 2" << W(8) << "type" <<  W(12) << "Elem.No." << endl;

}
//Output members
void Hyd_Floodplainraster_Segment::output_members(ostringstream *cout){

	*cout <<W(8)<< this->segment_index << W(8) << this->index_point_1 << W(8) << this->index_point_2;
	*cout << W(10) << this->convert_segment_type2txt(this->type);
	if(this->ptr_floodplain_elem!=NULL){
		*cout << W(10) << this->ptr_floodplain_elem->get_elem_number();
	}
	*cout<< endl;
}
//Assign a border height and a border flag to the Hyd_Element_Flodplain pointer
void Hyd_Floodplainraster_Segment::assign_value_flag2floodplain_element(const double value_h, const double value_pol, const bool flag){
	if(this->ptr_floodplain_elem!=NULL){
		if(this->type==_hyd_floodplain_raster_segment_type::BORDER){
			return; 
		}
		else if(this->type==_hyd_floodplain_raster_segment_type::XFLOW){
			if(this->ptr_floodplain_elem->get_flow_data().no_flow_x_flag==true){
				return;
			}
			this->ptr_floodplain_elem->set_x_noflow_flag(flag);
			this->ptr_floodplain_elem->set_x_border_z(value_h, value_pol);
		}
		else if(this->type==_hyd_floodplain_raster_segment_type::YFLOW){
			if(this->ptr_floodplain_elem->get_flow_data().no_flow_y_flag==true){
				return;
			}
			this->ptr_floodplain_elem->set_y_noflow_flag(flag);
			this->ptr_floodplain_elem->set_y_border_z(value_h, value_pol);
		}
	}
}
//Switch the points
void Hyd_Floodplainraster_Segment::switch_points(void){
	Geo_Point *buffer=this->point1;
	this->set_points(this->point2,buffer);

	int buffer_int=this->index_point_1;
	this->index_point_1=this->index_point_2;
	this->index_point_2=buffer_int;
}
//copy operator
Hyd_Floodplainraster_Segment& Hyd_Floodplainraster_Segment::operator=(const Hyd_Floodplainraster_Segment& object){
	Geo_Raster_Segment::operator =(object);
	this->segment_index=object.segment_index;
	this->index_point_1=object.index_point_1;
	this->index_point_2=object.index_point_2;
	return *this;
}
//_________
//private
//Conversion function for output (Enumerator _hyd_floodplain_raster_segment_type to text)
string Hyd_Floodplainraster_Segment::convert_segment_type2txt(_hyd_floodplain_raster_segment_type type){
	string txt;	
	switch(type){
		case _hyd_floodplain_raster_segment_type::XFLOW:
			txt=hyd_label::rast_seg_flow_x;
			break;
		case _hyd_floodplain_raster_segment_type::YFLOW:
			txt=hyd_label::rast_seg_flow_y;
			break;
		case _hyd_floodplain_raster_segment_type::BORDER:
			txt=hyd_label::rast_seg_border;
			break;
		default:
			txt==label::unknown_type;
	}
	return txt;
}