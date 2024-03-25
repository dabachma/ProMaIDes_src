#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Floodplain_Raster.h"

//constructor
Hyd_Floodplain_Raster::Hyd_Floodplain_Raster(void){
	
	this->number_polygons=0;
	this->raster_points=NULL;
	this->number_points=0;
	this->raster_segments=NULL;
	this->number_segments=0;
	this->geo_info.angle=0.0;
	this->geo_info.number_x=0;
	this->geo_info.number_y=0;
	this->geo_info.origin_global_x=0.0;
	this->geo_info.origin_global_y=0.0;
	this->geo_info.width_x=0.0;
	this->geo_info.width_y=0.0;
	this->floodplain_pointer=NULL;
	this->geometrical_bound.set_elementtype_inside(_hyd_elem_type::OTHER_FP_ELEM);
	this->index_fp_model=-1;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Floodplain_Raster)-sizeof(Hyd_Floodplain_Polygon), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Floodplain_Raster::~Hyd_Floodplain_Raster(void){
	this->delete_raster_points();
	this->delete_raster_segments();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Floodplain_Raster)-sizeof(Hyd_Floodplain_Polygon), _sys_system_modules::HYD_SYS);
}
//_____________
//public
//set the geometric information
void Hyd_Floodplain_Raster::set_geometric_info(const _hyd_floodplain_geo_info geometric_info){
	this->geo_info=geometric_info;
}
//Set the geometric information per database
void Hyd_Floodplain_Raster::set_geometric_info(QSqlTableModel *results, const int index){
	this->geo_info.number_x=results->record(index).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::nx)).c_str()).toInt();
	this->geo_info.number_y=results->record(index).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::ny)).c_str()).toInt();
	this->geo_info.width_x=results->record(index).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::elemwidth_x)).c_str()).toDouble();
	this->geo_info.width_y=results->record(index).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::elemwidth_y)).c_str()).toDouble();
	this->geo_info.origin_global_x=results->record(index).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::lowleftx)).c_str()).toDouble();
	this->geo_info.origin_global_y=results->record(index).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::lowlefty)).c_str()).toDouble();
	this->geo_info.angle=results->record(index).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::angle)).c_str()).toDouble();

	this->make_boundary2polygon();

}
//Get the geometric information
_hyd_floodplain_geo_info Hyd_Floodplain_Raster::get_geometric_info(void){
	return this->geo_info;
}
//Generate the model geometrical boundary as a polygon
void Hyd_Floodplain_Raster::make_boundary2polygon(void){
		Geo_Point buffer[4];
		//set counter clockwise
		//first point
		buffer[0].set_point_coordinate(0.0, 0.0);
		buffer[0]=buffer[0].transform_coordinate_system(this->geo_info.angle, this->geo_info.origin_global_x, this->geo_info.origin_global_y);
		//second point
		buffer[1].set_point_coordinate(this->geo_info.width_x*(double)this->geo_info.number_x,0.0);
		buffer[1]=buffer[1].transform_coordinate_system(this->geo_info.angle, this->geo_info.origin_global_x, this->geo_info.origin_global_y);
		//third point
		buffer[2].set_point_coordinate(this->geo_info.width_x*(double)this->geo_info.number_x,this->geo_info.width_y*(double)this->geo_info.number_y);
		buffer[2]=buffer[2].transform_coordinate_system(this->geo_info.angle, this->geo_info.origin_global_x, this->geo_info.origin_global_y);
		//fourth point
		buffer[3].set_point_coordinate(0.0,this->geo_info.width_y*(double)this->geo_info.number_y);
		buffer[3]=buffer[3].transform_coordinate_system(this->geo_info.angle, this->geo_info.origin_global_x, this->geo_info.origin_global_y);

		//set it to the polygon
		try{
			this->geometrical_bound.set_number_points(4);
			this->geometrical_bound.set_points(buffer);
		}
		catch(Error msg){
			ostringstream info;
			info<< "Try to set-up the geometrical floodplain raster boundary" << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
}
//Set a geometrical raster polygon with a given element index without any pointer to the floodplain element
void Hyd_Floodplain_Raster::set_geometrical_raster_polygon(const int element_index){
	int nx_counter=(int)(element_index/this->geo_info.number_x);
	//calculate the index
	int index_y=element_index+(2*this->geo_info.number_x+1)*(nx_counter+1)-nx_counter*this->geo_info.number_x;
	int index_x=index_y-this->geo_info.number_x;
	int index_minus_y=index_y-(2*this->geo_info.number_x+1);
	int index_minus_x=index_x-1;

	//set the polygon
	try{
		this->raster_elem.set_polygon_segments(&(this->raster_segments[index_y]), &(this->raster_segments[index_x]), &(this->raster_segments[index_minus_y]), &(this->raster_segments[index_minus_x]));
	}
	catch(Error msg){
		throw msg;
	}

}
//output the geometrical boundary of the floodplain model
void Hyd_Floodplain_Raster::output_geometrical_boundary(void){
	//output the elements
	ostringstream prefix1;
	prefix1 << "GEO_BOUND> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix1.str());
	ostringstream cout;
	cout <<"Geometrical boundary of the floodplain model (global)" << endl;
	cout <<"No. " << W(8) << "x" << label::m<<W(8)<< "y" << label::m << endl;
	for(int i=0; i<4; i++){
		cout << i << W(14) << P(2) << FORMAT_FIXED_REAL<< this->geometrical_bound.get_point(i)->get_xcoordinate();
		cout << W(14) << P(2) << FORMAT_FIXED_REAL<< this->geometrical_bound.get_point(i)->get_ycoordinate() << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output the geometrical boundary to tecplot
void Hyd_Floodplain_Raster::output_geometrical_boundary2tecplot(ofstream *output_file){	
	for(int i=0; i< this->geometrical_bound.get_number_segments(); i++){
		*output_file << "#Boundary " << i << endl;
		*output_file <<"GEOMETRY  X=" <<0.0 <<", Y="<<0.0<<", T=LINE, C=RED, LT=0.25, CS=GRID, ZN=1" << endl;
		*output_file <<"1" << endl;
		*output_file <<"2" << endl;
		*output_file << "#coordinates x  y" << endl;
		*output_file << this->geometrical_bound.get_segment(i)->point1.get_xcoordinate() << "  " << this->geometrical_bound.get_segment(i)->point1.get_ycoordinate() << endl;
		*output_file << this->geometrical_bound.get_segment(i)->point2.get_xcoordinate() << "  "<< this->geometrical_bound.get_segment(i)->point2.get_ycoordinate() << endl;
		*output_file << endl;
	}
}
//output members
void Hyd_Floodplain_Raster::output_members(void){
	
	//output the raster
	ostringstream prefix1;
	prefix1.str("");
	prefix1 << "RAST> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix1.str());
	ostringstream cout;

	if(this->number_points>0){
		//output raster points
		cout << " Output the points of the raster (" << this->number_points <<")..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
		this->raster_points[0].output_header(&cout);
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
		for (int i=0; i<this->number_points; i++){
			this->raster_points[i].output_members(&cout);
			Sys_Common_Output::output_hyd->output_txt(&cout,true);
		}
		
	}
	if(this->number_segments>0){
		//output the segments
		cout <<"Output the segments of the raster (" << this->number_segments<<")..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
		
		this->raster_segments[0].output_header(&cout);
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
		for(int i=0; i<this->number_segments; i++){
			this->raster_segments[i].output_members(&cout);
			Sys_Common_Output::output_hyd->output_txt(&cout,true);
		}
		
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();


}
//Set the index of the floodplain model
void Hyd_Floodplain_Raster::set_index_fp_model(const int index){
	this->index_fp_model=index;
}
//Set a pointer to the floodplain elemnts
void Hyd_Floodplain_Raster::set_floodplain_elem_pointer(Hyd_Element_Floodplain *floodplain_pointer){
	this->floodplain_pointer=floodplain_pointer;	
};
//Generate and allocate the raster points
void Hyd_Floodplain_Raster::generate_alloc_points_segments(void){
	this->delete_raster_points();
	//set number of points
	this->number_points=(this->geo_info.number_x+1)*(this->geo_info.number_y+1);
		//set number of elements
	this->number_polygons=this->geo_info.number_x*this->geo_info.number_y;
	
	this->allocate_raster_points();

	
	this->delete_raster_segments();
	//set the segments
	this->number_segments=(this->geo_info.number_x+ this->geo_info.number_y)+(this->geo_info.number_y*2*this->geo_info.number_x);
	
	this->allocate_raster_segments();


	//generate the points
	int counter=0;
	Geo_Point buffer;
	for(int i=0; i<this->geo_info.number_y+1;i++){
		for(int j=0; j< this->geo_info.number_x+1;j++){
			this->raster_points[counter].set_point_coordinate((double)j*this->geo_info.width_x, (double)i*this->geo_info.width_y);
			buffer=this->raster_points[counter].transform_coordinate_system(this->geo_info.angle,this->geo_info.origin_global_x, this->geo_info.origin_global_y);
			this->raster_points[counter].set_point(&buffer);
			this->raster_points[counter].set_point_name(label::raster_point);
			this->raster_points[counter].set_point_index(counter);
			counter++;
		}
	}

	//generate the segments
	counter=0;
	int counter_x=0;
	int counter_y=0;
	bool x_set=true;
	bool y_set=true;
	int index_1=0;
	int index_2=0;
	
	for(int j=0; j< this->number_segments ;j++){
		this->raster_segments[j].set_segment_index(j);
		if(counter_y==counter_x){
			index_1=counter-counter_y*this->geo_info.number_x;
			index_2=index_1+1;
			this->raster_segments[j].set_firstpoint_index(index_1);
			this->raster_segments[j].set_secondpoint_index(index_2);
			this->raster_segments[j].set_points(&(this->raster_points[index_1]),&(this->raster_points[index_2]));
			
			//set the segment type
			//YFLOW
			if(counter_x!=0 && j < this->number_segments-this->geo_info.number_x){
				this->raster_segments[j].set_segmenttype(_hyd_floodplain_raster_segment_type::YFLOW);
				if(this->floodplain_pointer!=NULL){
					this->raster_segments[j].set_element_ptr(&(this->floodplain_pointer[j-(counter_x*(this->geo_info.number_x+1)+this->geo_info.number_x)]));
				}
			}
			//border
			else{ 
				this->raster_segments[j].set_segmenttype(_hyd_floodplain_raster_segment_type::BORDER);
			}
		
		}
		else{
			index_1=counter-counter_x*this->geo_info.number_x;
			index_2=index_1+this->geo_info.number_x+1;
			this->raster_segments[j].set_firstpoint_index(index_1);
			this->raster_segments[j].set_secondpoint_index(index_2);
			this->raster_segments[j].set_points(&(this->raster_points[index_1]),&(this->raster_points[index_2]));
		
			//set the segment type
			//border
			if((j==counter_x*this->geo_info.number_x+ counter_y*(this->geo_info.number_x+1))
				|| (j==counter_x*this->geo_info.number_x+this->geo_info.number_x+counter_y*(this->geo_info.number_x+1))){
				this->raster_segments[j].set_segmenttype(_hyd_floodplain_raster_segment_type::BORDER);
			}
			//XFLOW
			else{ 
				this->raster_segments[j].set_segmenttype(_hyd_floodplain_raster_segment_type::XFLOW);
				if(this->floodplain_pointer!=NULL){
					this->raster_segments[j].set_element_ptr(&(this->floodplain_pointer[j-counter_x*(this->geo_info.number_x+1)]));
				}
			}
		
		}
		if(counter==(this->geo_info.number_x-1)+counter_x*this->geo_info.number_x+counter_y*(this->geo_info.number_x+1)&& y_set==true){
			counter_x++;
			x_set=true;
			y_set=false;
		}
		if(counter==(this->geo_info.number_x)+counter_x*this->geo_info.number_x+counter_y*(this->geo_info.number_x+1) && x_set==true){
			counter_y++;
			x_set=false;
			y_set=true;
		}
		counter++;
	}
}
//Delete the raster segments
void Hyd_Floodplain_Raster::delete_raster_segments(void){
	if(this->raster_segments!=NULL){
		delete []this->raster_segments;
		this->raster_segments=NULL;
	}
}
//Delete the raster points
void Hyd_Floodplain_Raster::delete_raster_points(void){
	if(this->raster_points!=NULL){
		delete []this->raster_points;
		this->raster_points=NULL;
	}
}
//Delete raster points and raster segments
void Hyd_Floodplain_Raster::delete_points_segments(void){
	this->delete_raster_points();
	this->delete_raster_segments();
}
//Get the pointer to all raster points
Hyd_Floodplainraster_Point* Hyd_Floodplain_Raster::get_ptr_raster_points(void){
	return this->raster_points;
}
//Get the pointer to all rastersegments
Hyd_Floodplainraster_Segment* Hyd_Floodplain_Raster::get_ptr_raster_segments(void){
	return this->raster_segments;
}
//Get the number of raster points
int Hyd_Floodplain_Raster::get_number_raster_points(void){
	return this->number_points;
}
//Clone the raster points
void Hyd_Floodplain_Raster::clone_raster_points(Hyd_Floodplain_Raster *raster){
	this->delete_raster_points();
	this->number_points=raster->number_points;
	this->allocate_raster_points();

	for(int i=0; i< this->number_points; i++){
		this->raster_points[i]=raster->raster_points[i];

	}

}
//Get a raster point with a given index
Hyd_Floodplainraster_Point * Hyd_Floodplain_Raster::get_raster_point(const int index){
	if(index >=0 && index < this->number_points){
		return &(this->raster_points[index]);
	}
	else{
		return NULL;
	}
}
//Set a given type to Hyd_Element_Floodplain (e.g. noflow) if a raster polygon (Hyd_Floodplain_Raster_Polygon) is inside a given polygon 
void Hyd_Floodplain_Raster::set_rasterpolygon_inside(Hyd_Floodplain_Polygon *polygon){
	for(int i=0; i< this->number_polygons; i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		//set the polygon
		this->set_raster_polygon(i);
		//check if the raster polygon is inside the given polygon
		this->raster_elem.is_inside_polygon(polygon);

		
	}
}
//Set a given type to Hyd_Element_Floodplain (e.g. noflow) if a raster polygon (Hyd_Floodplain_Raster_Polygon) intercepts a given polygon 
void Hyd_Floodplain_Raster::set_rasterpolygon_intercepted(Hyd_Floodplain_Polygon *polygon,  _hyd_elem_type type){
	for(int i=0; i< this->number_polygons; i++){
		//set the polygon
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->set_raster_polygon(i);
		//check if the raster polygon is inside the given polygon
		this->raster_elem.is_intercepted_polygon(polygon, type);
	}
}
//Intercept a Hyd_Floodplainpolysegment with the raster (e.g. dikeline, riverline)
void Hyd_Floodplain_Raster::interception_polysegments2raster(Hyd_Floodplain_Polysegment *line){
	try{
		for(int i=0; i< this->number_polygons; i++){
			//set the polygon
			this->set_raster_polygon(i);
			//make the interception
			this->raster_elem.calculate_polysegment_interception(&line->intercept_list, line);
		}
		//sort the points in list after the distance to the beginning of the polysegment
		line->intercept_list.sort_distance();
		//output for development
		//line->intercept_list.output_members();
		//make the list of the assigned raster points
		this->assign_points_value2list(line);
		//output for development
		//this->raster_point_list.output_members();
		//set the points to the floodplain elements
		this->assign_listvalues2elements(line);
		
		//delete the list
		this->raster_point_list.delete_list();
		line->intercept_list.delete_list();
	}
	catch(Error msg){
		throw msg;
	}
}
//Assign the relevant elements of coupling to other models to a Hyd_Coupling_Point_FP2CO_List (floodplain to coast model)
void Hyd_Floodplain_Raster::assign_elements2couplingpointlist(Hyd_Coupling_Point_FP2CO_List *list){
	try{
		//first delete the interception list of the defining polysegment
		list->get_defining_polysegment()->intercept_list.delete_list();
		//fill it with the interception points
		for(int i=0; i< this->number_polygons; i++){
			//set the polygon
			this->set_raster_polygon(i);
			//make the interception
			this->raster_elem.calculate_polysegment_interception(&(list->get_defining_polysegment()->intercept_list), list->get_defining_polysegment());
		}
		//sort the interception points in list after the distance to the beginning of the polysegment
		list->get_defining_polysegment()->intercept_list.sort_distance();

		//output for development
		//list->get_defining_polysegment()->intercept_list.output_members();

		//transfer the interception points to coupling points
		int elem_index=-1;
		Hyd_Coupling_Point_FP2CO buffer;
		for(int i=1; i<list->get_defining_polysegment()->intercept_list.get_number_points(); i++){
			elem_index=this->convert_interception_points2elemindex(list->get_defining_polysegment()->intercept_list.get_point(i-1), list->get_defining_polysegment()->intercept_list.get_point(i), list->get_defining_polysegment()->get_line_type());
			//set the fist point of the list
			if(i==1){
				buffer.set_point((list->get_defining_polysegment()->intercept_list.get_point(0).line_interception.interception_point));
				buffer.set_indices(list->get_defining_polysegment()->intercept_list.get_point(0).index_intercepts, elem_index);
				buffer.set_floodplain_index(this->index_fp_model);
				if(elem_index>=0){
					buffer.set_pointers(&this->floodplain_pointer[elem_index]);
				}
				else{
					buffer.set_pointers(NULL);
				}
				buffer.set_point_name(hyd_label::coupling_point_FP2CO);
				list->set_new_couplingpoint(&buffer);	
			}
			//set rest of the points into the coupling point list
			buffer.set_point((list->get_defining_polysegment()->intercept_list.get_point(i).line_interception.interception_point));
			buffer.set_indices(list->get_defining_polysegment()->intercept_list.get_point(i).index_intercepts, elem_index);
			buffer.set_floodplain_index(this->index_fp_model);
			if(elem_index>=0){
				buffer.set_pointers(&this->floodplain_pointer[elem_index]);
			}
			else{
				buffer.set_pointers(NULL);
			}
			buffer.set_point_name(hyd_label::coupling_point_FP2CO);
			list->set_new_couplingpoint(&buffer);
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Assign the relevant elements of coupling to other models to a Hyd_Coupling_Point_RV2FP_List (river to floodplain model)
void Hyd_Floodplain_Raster::assign_elements2couplingpointlist(Hyd_Coupling_Point_RV2FP_List *list){
	try{
		//first delete the interception list of the defining polysegment
		list->get_defining_polysegment()->intercept_list.delete_list();
		//fill it with the interception points
		for(int i=0; i< this->number_polygons; i++){
			//set the polygon
			this->set_raster_polygon(i);
			//make the interception
			this->raster_elem.calculate_polysegment_interception(&(list->get_defining_polysegment()->intercept_list), list->get_defining_polysegment());
		}
		//sort the interception points in list after the distance to the beginning of the polysegment
		list->get_defining_polysegment()->intercept_list.sort_distance();

		//output for development
		//list->get_defining_polysegment()->intercept_list.output_members();

		//transfer the interception points to coupling points
		int elem_index=-1;
		Hyd_Coupling_Point_RV2FP buffer;

		//just one point found (boundary element)
		if(list->get_defining_polysegment()->intercept_list.get_number_points()==1){
			elem_index=this->convert_interception_points2elemindex(list->get_defining_polysegment()->intercept_list.get_point(0));
			buffer.set_first_found_elem_index(elem_index);
			if(elem_index>=0){
				if(this->floodplain_pointer[elem_index].get_elem_type()==_hyd_elem_type::RIVER_ELEM){
					this->search_convenient_coupling_element(&elem_index, *(list->get_defining_polysegment()->get_first_point()), list->get_defining_polysegment()->intercept_list.get_point(0).line_interception.interception_point, list->get_leftriver_bank_line());
				}	
			}
			buffer.set_point((list->get_defining_polysegment()->intercept_list.get_point(0).line_interception.interception_point));
			buffer.set_indices(list->get_defining_polysegment()->intercept_list.get_point(0).index_intercepts, elem_index);
			buffer.set_floodplain_index(this->index_fp_model);
			if(elem_index>=0){
				buffer.set_pointer_floodplain_element(&this->floodplain_pointer[elem_index]);
			}
			else{
				buffer.set_pointer_floodplain_element(NULL);
			}
			buffer.set_point_name(hyd_label::coupling_point_RV2FP);
			list->set_new_couplingpoint(&buffer);
		}

		//more than one point
		for(int i=1; i<list->get_defining_polysegment()->intercept_list.get_number_points(); i++){
			elem_index=this->convert_interception_points2elemindex(list->get_defining_polysegment()->intercept_list.get_point(i-1), list->get_defining_polysegment()->intercept_list.get_point(i), list->get_defining_polysegment()->get_line_type());
			buffer.set_first_found_elem_index(elem_index);
			if(elem_index>=0){
				if(this->floodplain_pointer[elem_index].get_elem_type()==_hyd_elem_type::RIVER_ELEM || this->floodplain_pointer[elem_index].get_elem_type()==_hyd_elem_type::DIKELINE_ELEM){  
					//|| this->floodplain_pointer[elem_index].get_elem_type()==_hyd_elem_type::OTHER_FP_ELEM){
					this->search_convenient_coupling_element(&elem_index, list->get_defining_polysegment()->intercept_list.get_point(i-1).line_interception.interception_point, list->get_defining_polysegment()->intercept_list.get_point(i).line_interception.interception_point, list->get_leftriver_bank_line());
				}	
			}
			//set the fist point of the list
			if(i==1){
				buffer.set_point((list->get_defining_polysegment()->intercept_list.get_point(0).line_interception.interception_point));
				buffer.set_indices(list->get_defining_polysegment()->intercept_list.get_point(0).index_intercepts, elem_index);
				buffer.set_floodplain_index(this->index_fp_model);
				if(elem_index>=0){
					buffer.set_pointer_floodplain_element(&this->floodplain_pointer[elem_index]);
				}
				else{
					buffer.set_pointer_floodplain_element(NULL);
				}
				buffer.set_point_name(hyd_label::coupling_point_RV2FP);
				list->set_new_couplingpoint(&buffer);	
			}
			//set rest of the points into the coupling point list
			buffer.set_point((list->get_defining_polysegment()->intercept_list.get_point(i).line_interception.interception_point));
			buffer.set_indices(list->get_defining_polysegment()->intercept_list.get_point(i).index_intercepts, elem_index);
			buffer.set_floodplain_index(this->index_fp_model);
			if(elem_index>=0){
				buffer.set_pointer_floodplain_element(&this->floodplain_pointer[elem_index]);
			}
			else{
				buffer.set_pointer_floodplain_element(NULL);
			}
			buffer.set_point_name(hyd_label::coupling_point_RV2FP);
			list->set_new_couplingpoint(&buffer);
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Assign the relevant elements of coupling to other models to a Hyd_Coupling_Point_RP2FP_List (floodplain to floodplain model)
void Hyd_Floodplain_Raster::assign_elements2couplingpointlist(Hyd_Coupling_Point_FP2FP_List *list, const bool first_fp_flag){
	try{
		//first delete the interception list of the defining polysegment
		list->get_defining_polysegment()->intercept_list.delete_list();
		//for debug
		//list->get_defining_polysegment()->output_members();
		//fill it with the interception points
		for(int i=0; i< this->number_polygons; i++){
			//set the polygon
			this->set_raster_polygon(i);
			//make the interception
			this->raster_elem.calculate_polysegment_interception(&(list->get_defining_polysegment()->intercept_list), list->get_defining_polysegment());
		}
		//sort the interception points in list after the distance to the beginning of the polysegment
		list->get_defining_polysegment()->intercept_list.sort_distance();


		//transfer the interception points to coupling points
		int elem_index=-1;
		Hyd_Coupling_Point_FP2FP buffer;
		Geo_Point buff1;
		Geo_Point buff2;
		for(int i=1; i<list->get_defining_polysegment()->intercept_list.get_number_points(); i++){
			buff1=(list->get_defining_polysegment()->intercept_list.get_point(i-1).line_interception.interception_point);
			buff2=(list->get_defining_polysegment()->intercept_list.get_point(i).line_interception.interception_point);
			elem_index=this->convert_interception_points2elemindex(list->get_defining_polysegment()->intercept_list.get_point(i-1),list->get_defining_polysegment()->intercept_list.get_point(i), list->get_defining_polysegment()->get_line_type());
			//set the fist point of the list
			if(i==1){
				buffer.set_point(buff1);
				//set the index	
				if(first_fp_flag==true){
					buffer.set_first_fpelem_index(elem_index);
					if(elem_index>=0){		
						buffer.set_first_fpelem_pointer(&this->floodplain_pointer[elem_index]);
					}
					else{
						buffer.set_first_fpelem_pointer(NULL);
					}
					//calculate the direction
					double direction_xaxis=0.0;
					direction_xaxis=buff1.direction_xaxsis(&buff2)+90.0;
					//transform it to the local raster
					direction_xaxis=direction_xaxis+this->geo_info.angle;
					direction_xaxis=_Geo_Geometrie::angle_to_360(direction_xaxis);
					//convert into direction of neighbouring elements
					_hyd_all_element_direction direction;
					direction=this->convert_angle2direction(direction_xaxis);
					if(direction ==_hyd_all_element_direction::X_DIRECT || direction ==_hyd_all_element_direction::MINUS_X_DIRECT){
						buffer.set_direction_coupling(true);
					}
					else if(direction ==_hyd_all_element_direction::Y_DIRECT || direction ==_hyd_all_element_direction::MINUS_Y_DIRECT){
						buffer.set_direction_coupling(false);
					}
				}
				else{
					buffer.set_second_fpelem_index(elem_index);
					if(elem_index>=0){
						buffer.set_second_fpelem_pointer(&this->floodplain_pointer[elem_index]);
					}
					else{
						buffer.set_second_fpelem_pointer(NULL);
					}
				}
				buffer.set_point_name(hyd_label::coupling_point_RV2FP);
				list->set_new_couplingpoint(&buffer);	
			}

			//set the rest of points
			buffer.set_point(buff2);
			//set the index	
			if(first_fp_flag==true){
				buffer.set_first_fpelem_index(elem_index);
				if(elem_index>=0){
					buffer.set_first_fpelem_pointer(&this->floodplain_pointer[elem_index]);
				}
				else{
					buffer.set_first_fpelem_pointer(NULL);
				}
				//calculate the direction
				double direction_xaxis=0.0;
				direction_xaxis=buff1.direction_xaxsis(&buff2)+90.0;
				//transform it to the local raster
				direction_xaxis=direction_xaxis+this->geo_info.angle;
				direction_xaxis=_Geo_Geometrie::angle_to_360(direction_xaxis);
				//convert into direction of neighbouring elements
				_hyd_all_element_direction direction;
				direction=this->convert_angle2direction(direction_xaxis);
				if(direction ==_hyd_all_element_direction::X_DIRECT || direction ==_hyd_all_element_direction::MINUS_X_DIRECT){
					buffer.set_direction_coupling(true);
				}
				else if(direction ==_hyd_all_element_direction::Y_DIRECT || direction ==_hyd_all_element_direction::MINUS_Y_DIRECT){
					buffer.set_direction_coupling(false);
				}
			}
			else{
				buffer.set_second_fpelem_index(elem_index);
				if(elem_index>=0){
					buffer.set_second_fpelem_pointer(&this->floodplain_pointer[elem_index]);
				}
				else{
					buffer.set_second_fpelem_pointer(NULL);
				}
			}
			buffer.set_point_name(hyd_label::coupling_point_RV2FP);
			list->set_new_couplingpoint(&buffer);
		}

		//if the floodplain model is inside the other one
		if(list->get_defining_polysegment()->get_is_closed()==true){
			buff1=(list->get_defining_polysegment()->intercept_list.get_last_point().line_interception.interception_point);
			buff2=(list->get_defining_polysegment()->intercept_list.get_first_point().line_interception.interception_point);
			elem_index=this->convert_interception_points2elemindex(list->get_defining_polysegment()->intercept_list.get_last_point(),list->get_defining_polysegment()->intercept_list.get_first_point(), list->get_defining_polysegment()->get_line_type());
			//set the rest of points
			buffer.set_point(buff2);
			//set the index	
			if(first_fp_flag==true){
				buffer.set_first_fpelem_index(elem_index);
				if(elem_index>=0){
					buffer.set_first_fpelem_pointer(&this->floodplain_pointer[elem_index]);
				}
				else{
					buffer.set_first_fpelem_pointer(NULL);
				}
				//calculate the direction
				double direction_xaxis=0.0;
				direction_xaxis=buff1.direction_xaxsis(&buff2)+90.0;
				//transform it to the local raster
				direction_xaxis=direction_xaxis+this->geo_info.angle;
				direction_xaxis=_Geo_Geometrie::angle_to_360(direction_xaxis);
				//convert into direction of neighbouring elements
				_hyd_all_element_direction direction;
				direction=this->convert_angle2four_direction(direction_xaxis);
				if(direction ==_hyd_all_element_direction::X_DIRECT || direction ==_hyd_all_element_direction::MINUS_X_DIRECT){
					buffer.set_direction_coupling(true);
				}
				else if(direction ==_hyd_all_element_direction::Y_DIRECT || direction ==_hyd_all_element_direction::MINUS_Y_DIRECT){
					buffer.set_direction_coupling(false);
				}
			}
			else{
				buffer.set_second_fpelem_index(elem_index);
				if(elem_index>=0){
					buffer.set_second_fpelem_pointer(&this->floodplain_pointer[elem_index]);
				}
				else{
					buffer.set_second_fpelem_pointer(NULL);
				}
			}
			buffer.set_point_name(hyd_label::coupling_point_RV2FP);
			list->set_new_couplingpoint(&buffer);
		}//end is_closed

	}
	catch(Error msg){
		throw msg;
	}
}
//Find the index of a raster element where a given point is inside; if it is at boundary nothing is found
int Hyd_Floodplain_Raster::find_elem_index_by_point(Geo_Point *point){
	int index=-1;

	for(int i=0; i< this->number_polygons;i++){
		this->set_raster_polygon(i);
		if(this->raster_elem.check_point_inside(point)==true){
			index=i;
			break;
		}	
	}
	return index;
}
//Find the index of a raster element where a given point is inside or at boundary
int Hyd_Floodplain_Raster::find_elem_index_by_point_withboundary(Geo_Point *point){
	int index=-1;

	for(int i=0; i< this->number_polygons;i++){
		this->set_raster_polygon(i);
		if(this->raster_elem.check_point_inside(point)==true || this->raster_elem.check_point_atboundary(point)==true ){
			index=i;
			break;
		}	
	}
	return index;
}
//Search for a convenient coupling element at neighbouring elements in case of an river element type (RV2FP couplings)
void Hyd_Floodplain_Raster::search_convenient_coupling_element(int *elem_index, Geo_Point point1, Geo_Point point2, const bool left_flag){
	//direction orthogonal to the segment to the x-axis
	double direction_xaxis=0.0;
	int elem_index_buff_0=-1;
	double distance_0=-1.0;
	int elem_index_buff_1=-1;
	double distance_1=-1.0;
	int elem_index_buff_2=-1;
	double distance_2=-1.0;
	int elem_index_buff=-1;
	double min_dist=-1.0;


	//calculate the angle
	direction_xaxis=point1.direction_xaxsis(&point2);
	if(left_flag==true){
		direction_xaxis=direction_xaxis+90.0;
	}
	else{
		direction_xaxis=direction_xaxis-90.0;
	}
	//transform it to the local raster
	direction_xaxis=direction_xaxis+this->geo_info.angle;
	direction_xaxis=_Geo_Geometrie::angle_to_360(direction_xaxis);
	//convert into direction of neighbouring elements
	_hyd_all_element_direction direction;


	direction=this->convert_angle2direction(direction_xaxis);
	//find the first element
	elem_index_buff_0=this->convert_2neighbouring_elemindex(direction, *elem_index);
	//look for neighbouring possible elemenst
	
	elem_index_buff_1=this->convert_2neighbouring_elemindex(_Hyd_Element_Floodplain_Type::get_next_element_direction_clockwise(direction), *elem_index);
	elem_index_buff_2=this->convert_2neighbouring_elemindex(_Hyd_Element_Floodplain_Type::get_next_element_direction_counterclockwise(direction), *elem_index);

	//calculate the distances
	if(elem_index_buff_0>=0){
		distance_0=this->calculate_distance_midpoints(*elem_index, elem_index_buff_0);
	}
	if(elem_index_buff_1>=0){
		distance_1=this->calculate_distance_midpoints(*elem_index, elem_index_buff_1);
	}
	if(elem_index_buff_2>=0){
		distance_2=this->calculate_distance_midpoints(*elem_index, elem_index_buff_2);
	}
	//find the minimal distance
	if(distance_0>=0.0){
		elem_index_buff=elem_index_buff_0;
		min_dist=distance_0;
	}
	if(distance_1>=0.0){
		if( min_dist>=0.0){
			if(distance_1 < min_dist ){
				elem_index_buff=elem_index_buff_1;
				min_dist=distance_1;
			}
		}
		else{
			elem_index_buff=elem_index_buff_1;
			min_dist=distance_1;
		}
	}
	if(distance_2>=0.0){
		if(min_dist>=0.0){
			if(distance_2 < min_dist){
				elem_index_buff=elem_index_buff_2;
				min_dist=distance_2;
			}
		}
		else{
			elem_index_buff=elem_index_buff_2;
			min_dist=distance_2;
		}
	}


	*elem_index=elem_index_buff;
}
//Set raster segments to noflow, if there are inside or at boundary of a given polygon
void Hyd_Floodplain_Raster::set_raster_segments2noflow(Hyd_Floodplain_Polygon *polygon){
	
	for(int i=0; i< this->number_segments; i++){
		if(polygon->check_segment_is_inside(&this->raster_segments[i])==true){
			this->raster_segments[i].assign_value_flag2floodplain_element(0.0,constant::poleni_const, true);
		}

	}
}
//Get the complete raster polygons
void Hyd_Floodplain_Raster::get_complete_raster_polygons(Hyd_Floodplainraster_Polygon **polygons){

	try{
		(*polygons)=new Hyd_Floodplainraster_Polygon[this->number_polygons];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	int nx_counter=0;
	int index_y=0;
	int index_x=0;
	int index_minus_y=0;
	int index_minus_x=0;


	//set them
	for(int i=0; i<this->number_polygons; i++){
		nx_counter=(int)(i/this->geo_info.number_x);
		//calculate the index
		index_y=i+(2*this->geo_info.number_x+1)*(nx_counter+1)-nx_counter*this->geo_info.number_x;
		index_x=index_y-this->geo_info.number_x;
		index_minus_y=index_y-(2*this->geo_info.number_x+1);
		index_minus_x=index_x-1;

		//set the polygon
		try{
			(*polygons)[i].set_polygon_segments(&(this->raster_segments[index_y]), &(this->raster_segments[index_x]), &(this->raster_segments[index_minus_y]), &(this->raster_segments[index_minus_x]));
		}
		catch(Error msg){
			throw msg;
		}

	}

}
//_________________
//protected
//Convert an element index and a given neighbourig direction into an neighbouring element index
int Hyd_Floodplain_Raster::convert_2neighbouring_elemindex(_hyd_all_element_direction direction, const int elem_index){
	int new_elemindex=-1;

	if(elem_index<0){
		return new_elemindex;
	}
	//calculate the row
	int row=(int)(elem_index/this->geo_info.number_x);
	//calculate the column
	int column=elem_index-row*this->geo_info.number_x;
	//sort out when the element is on a boundary
	//element is in the rightest column
	if(column==this->geo_info.number_x-1 && (direction==_hyd_all_element_direction::X_DIRECT || direction==_hyd_all_element_direction::XY_DIRECT || direction==_hyd_all_element_direction::MINUS_Y_X_DIRECT)){
		//there is no appropriate element
		return new_elemindex;
	}
	//element is in the last row
	if(row==this->geo_info.number_y-1 && (direction== _hyd_all_element_direction::Y_DIRECT || direction== _hyd_all_element_direction::XY_DIRECT || direction== _hyd_all_element_direction::YMINUS_X_DIRECT)){
		//there is no appropriate element
		return new_elemindex;
	}
	//element is in the leftest column
	if(column==0 && (direction== _hyd_all_element_direction::MINUS_X_DIRECT || direction== _hyd_all_element_direction::MINUS_X_MINUS_Y_DIRECT || direction== _hyd_all_element_direction::YMINUS_X_DIRECT)){
		//there is no appropriate element
		return new_elemindex;
	}
	//element is in the first row
	if(row==0 && (direction== _hyd_all_element_direction::MINUS_Y_DIRECT || direction== _hyd_all_element_direction::MINUS_X_MINUS_Y_DIRECT || direction== _hyd_all_element_direction::MINUS_Y_X_DIRECT)){
		//there is no appropriate element
		return new_elemindex;
	}

	//the element is not on a boundary
	if(direction== _hyd_all_element_direction::X_DIRECT){
		new_elemindex=elem_index+1;
	}
	else if(direction== _hyd_all_element_direction::XY_DIRECT){
		new_elemindex=elem_index+1+this->geo_info.number_x;
	}
	else if(direction== _hyd_all_element_direction::Y_DIRECT){
		new_elemindex=elem_index+this->geo_info.number_x;
	}
	else if(direction== _hyd_all_element_direction::YMINUS_X_DIRECT){
		new_elemindex=elem_index+this->geo_info.number_x-1;
	}
	else if(direction== _hyd_all_element_direction::MINUS_X_DIRECT){
		new_elemindex=elem_index-1;
	}
	else if(direction== _hyd_all_element_direction::MINUS_X_MINUS_Y_DIRECT){
		new_elemindex=elem_index-1-this->geo_info.number_x;
	}
	else if(direction== _hyd_all_element_direction::MINUS_Y_DIRECT){
		new_elemindex=elem_index-this->geo_info.number_x;
	}
	else if(direction== _hyd_all_element_direction::MINUS_Y_X_DIRECT){
		new_elemindex=elem_index-this->geo_info.number_x+1;
	}

	//check if the element type is convenient
	if(this->floodplain_pointer!=NULL && new_elemindex>=0){
		if(this->floodplain_pointer[new_elemindex].get_elem_type()==_hyd_elem_type::STANDARD_ELEM || this->floodplain_pointer[new_elemindex].get_elem_type()==_hyd_elem_type::DIKELINE_ELEM){
			return new_elemindex;
		}
		else{
			return new_elemindex=-1;
		}
	}
	else{
		return new_elemindex=-1;
	}

	return new_elemindex;
}
//________________
//private
//Allocate the raster points
void Hyd_Floodplain_Raster::allocate_raster_points(void){
	//allocate points
	try{
		this->raster_points=new Hyd_Floodplainraster_Point[this->number_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Allocate the raster segments
void Hyd_Floodplain_Raster::allocate_raster_segments(void){
	//allocate segments
	try{
		this->raster_segments=new Hyd_Floodplainraster_Segment[this->number_segments];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
}
//Set a geometrical raster polygon with a given element index and connect the pointer to the floodplain element
void Hyd_Floodplain_Raster::set_raster_polygon(const int element_index){
	int nx_counter=(int)(element_index/this->geo_info.number_x);
	//calculate the index
	int index_y=element_index+(2*this->geo_info.number_x+1)*(nx_counter+1)-nx_counter*this->geo_info.number_x;
	int index_x=index_y-this->geo_info.number_x;
	int index_minus_y=index_y-(2*this->geo_info.number_x+1);
	int index_minus_x=index_x-1;

	//set the polygon
	try{
		//this->raster_elem.set_polygon_segments(&(this->raster_segments[index_y]), &(this->raster_segments[index_x]), &(this->raster_segments[index_minus_y]), &(this->raster_segments[index_minus_x]));
		//use counter clockwise
		this->raster_elem.set_polygon_segments(&(this->raster_segments[index_x]), &(this->raster_segments[index_y]), &(this->raster_segments[index_minus_x]), &(this->raster_segments[index_minus_y]));

	}
	catch(Error msg){
		throw msg;
	}
	//set the pointer to the floodplain element
	this->raster_elem.set_ptr2floodplain_element(&(this->floodplain_pointer[element_index]));
}
//Assign values (heights or noflow flags) and the corresponding index of a point to raster_point_list
void Hyd_Floodplain_Raster::assign_points_value2list(Hyd_Floodplain_Polysegment *line){
	//for assigning the element type of intercepted elements
	int elem_index;

	//loop over all points in the list; check always a pair of points which are always in one polygon; there are no duplicate points in the list
	for(int j=0; j<line->intercept_list.get_number_points()-1; j++){
		//two interception points intercept the segments of the raster
		if(line->intercept_list.get_point(j).index_point_is_intercepted==-1 &&
			line->intercept_list.get_point(j+1).index_point_is_intercepted==-1){
				//assign the value to the point
				this->assign_points_value2list_segments(line->intercept_list.get_point(j), line->intercept_list.get_point(j+1), line);
				//assign an _hyd_elem_type to the intersected element
				elem_index=this->segments_index2elem_index(line->intercept_list.get_point(j).index_is_intercept, line->intercept_list.get_point(j+1).index_is_intercept, line->get_line_type());
				this->set_elem_type_per_elemindex(elem_index, line->get_element_type());
		}
		//two interception points are on two rasterpoints 
		else if(line->intercept_list.get_point(j).index_point_is_intercepted!=-1 &&
			line->intercept_list.get_point(j+1).index_point_is_intercepted!=-1){
				this->assign_points_value2list_twopoints(line->intercept_list.get_point(j), line->intercept_list.get_point(j+1), line);
				//assign an _hyd_elem_type to the intersected element
				elem_index=this->points_index2elem_index(line->intercept_list.get_point(j).index_point_is_intercepted,line->intercept_list.get_point(j+1).index_point_is_intercepted, line->get_line_type());
				this->set_elem_type_per_elemindex(elem_index, line->get_element_type());
		}
		//first interception point is on a pointraster; the second interception point intercepts a rastersegment
		else if(line->intercept_list.get_point(j).index_point_is_intercepted!=-1 &&
			line->intercept_list.get_point(j+1).index_point_is_intercepted==-1){
				this->assign_points_value2list_point_segment(line->intercept_list.get_point(j), line->intercept_list.get_point(j+1), line);
				//assign an _hyd_elem_type to the intersected element
				elem_index=this->points_segment_index2elem_index(line->intercept_list.get_point(j+1).index_is_intercept,line->intercept_list.get_point(j).index_point_is_intercepted, line->get_line_type());
				this->set_elem_type_per_elemindex(elem_index, line->get_element_type());

		}
		//first interception point intercepts a rastersegment; the second interception is on a rasterpoint
		else if(line->intercept_list.get_point(j).index_point_is_intercepted==-1 &&
			line->intercept_list.get_point(j+1).index_point_is_intercepted!=-1){
				this->assign_points_value2list_segment_point(line->intercept_list.get_point(j), line->intercept_list.get_point(j+1), line);
				//assign an _hyd_elem_type to the intersected element
				elem_index=this->points_segment_index2elem_index(line->intercept_list.get_point(j).index_is_intercept,line->intercept_list.get_point(j+1).index_point_is_intercepted, line->get_line_type());
				this->set_elem_type_per_elemindex(elem_index, line->get_element_type());

		}
			
	}

	//set the last point if a point is hit
	if (this->raster_point_list.get_number_points()>1){
		//raster poinbt is hit
		if(line->intercept_list.get_last_point().index_point_is_intercepted!=-1){
			//this buffer is filled and given to the list
			_hyd_value2pointindex buffer;
			buffer.index=line->intercept_list.get_last_point().index_point_is_intercepted;
			buffer.flag=line->get_segments_no_overflow(line->intercept_list.get_last_point().index_intercepts);
			buffer.value_h=line->get_segments_height(line->intercept_list.get_last_point().index_intercepts);
			buffer.value_pol=line->get_segments_poleni(line->intercept_list.get_last_point().index_intercepts);
			this->raster_point_list.set_new_point(&buffer);
		}
	}
}
//Assign a value from a line to raster points (the two interception are on a point)
bool Hyd_Floodplain_Raster::assign_points_value2list_twopoints( _geo_multisegment_interception point_1, _geo_multisegment_interception point_2, Hyd_Floodplain_Polysegment *line){
	//found flag
	bool found_flag=false;
	//index of the intercepting line segments (dikeline)
	int index_line[2];
	index_line[0]=point_1.index_intercepts;
	index_line[1]=point_2.index_intercepts;
	//this buffer is filled and given to the list
	_hyd_value2pointindex buffer;

	//points are not diagonal
	if(abs(point_1.index_point_is_intercepted-point_2.index_point_is_intercepted)==1 || 
		abs(point_1.index_point_is_intercepted-point_2.index_point_is_intercepted)==this->geo_info.number_x+1){
		//set the values first point
		buffer.flag=line->get_segments_no_overflow(index_line[0]);
		buffer.value_h=line->get_segments_height(index_line[0]);
		buffer.value_pol=line->get_segments_poleni(index_line[0]);
		buffer.index=point_1.index_point_is_intercepted;
		this->raster_point_list.set_new_point(&buffer);
		//set the values second point
		buffer.flag=line->get_segments_no_overflow(index_line[1]);
		buffer.value_h=line->get_segments_height(index_line[1]);
		buffer.value_pol=line->get_segments_poleni(index_line[1]);
		buffer.index=point_2.index_point_is_intercepted;
		this->raster_point_list.set_new_point(&buffer);
		return found_flag=true;
		
	}
	//points are the same
	else if(point_1.index_point_is_intercepted==point_2.index_point_is_intercepted){
		//nothing happen
		return found_flag=true;
	}
	//points are diagonal
	else{
		//set the value of the first one
		buffer.flag=line->get_segments_no_overflow(index_line[0]);
		buffer.value_h=line->get_segments_height(index_line[0]);
		buffer.value_pol=line->get_segments_poleni(index_line[0]);
		buffer.index=point_1.index_point_is_intercepted;
		this->raster_point_list.set_new_point(&buffer);

		//set the corner point
		if(abs(point_1.index_point_is_intercepted-point_2.index_point_is_intercepted)==this->geo_info.number_x){
			if(point_1.index_point_is_intercepted>point_2.index_point_is_intercepted){
				buffer.index=point_1.index_point_is_intercepted+1;
			}
			else{
				buffer.index=point_2.index_point_is_intercepted+1;
			}
			this->raster_point_list.set_new_point(&buffer);
		}
		else if(abs(point_1.index_point_is_intercepted-point_2.index_point_is_intercepted)==this->geo_info.number_x+2){
			if(point_1.index_point_is_intercepted>point_2.index_point_is_intercepted){
				buffer.index=point_1.index_point_is_intercepted-1;
			}
			else{
				buffer.index=point_2.index_point_is_intercepted-1;
			}
			this->raster_point_list.set_new_point(&buffer);
		}

		//set the last point
		buffer.flag=line->get_segments_no_overflow(index_line[1]);
		buffer.value_h=line->get_segments_height(index_line[1]);
		buffer.value_pol=line->get_segments_poleni(index_line[1]);
		buffer.index=point_2.index_point_is_intercepted;
		this->raster_point_list.set_new_point(&buffer);
		return found_flag=true;


	}
	return found_flag=false;
}
//Assign a value from a line to raster points (the two interception point are clear on the raster segments
bool Hyd_Floodplain_Raster::assign_points_value2list_segments( _geo_multisegment_interception point_1, _geo_multisegment_interception point_2, Hyd_Floodplain_Polysegment *line){
	//found flag
	bool found_flag=false;
	//index of the intercepted raster segments
	int index_rast_seg[2];
	//first segment
	index_rast_seg[0]=point_1.index_is_intercept;
	//second segment
	index_rast_seg[1]=point_2.index_is_intercept;
	//index of the intercepting line segments (dikeline)
	int index_line[2];
	index_line[0]=point_1.index_intercepts;
	index_line[1]=point_2.index_intercepts;
	//this buffer is filled and given to the list
	_hyd_value2pointindex buffer;
	
	//same segment
	if(abs(index_rast_seg[0]-index_rast_seg[1])==0){
		//nothing happen
		return found_flag=true;

	}
	//corner
	else if(abs(index_rast_seg[0]-index_rast_seg[1])==this->geo_info.number_x || abs(index_rast_seg[0]-index_rast_seg[1])==this->geo_info.number_x+1){
		if(this->raster_segments[index_rast_seg[0]].get_firstpoint_index()==this->raster_segments[index_rast_seg[1]].get_secondpoint_index()||this->raster_segments[index_rast_seg[0]].get_firstpoint_index()==this->raster_segments[index_rast_seg[1]].get_firstpoint_index() ){
			buffer.index=this->raster_segments[index_rast_seg[0]].get_firstpoint_index();
		}
		else{
			buffer.index=this->raster_segments[index_rast_seg[0]].get_secondpoint_index();
		}
		//set the values
		buffer.flag=line->get_segments_no_overflow(index_line[0]);
		buffer.value_h=0.5*(line->get_segments_height(index_line[0])+line->get_segments_height(index_line[1]));
		buffer.value_pol=line->get_segments_poleni(index_line[0]);
		this->raster_point_list.set_new_point(&buffer);
		return found_flag=true;

	}
	//opposite segments
	else if(abs(index_rast_seg[0]-index_rast_seg[1])==1 || abs(index_rast_seg[0]-index_rast_seg[1])==2*this->geo_info.number_x+1 ){
		//calculate the distance
		//distances to the points of the raster segments
		double distance[4];
		int min_dist_index=-1;
		//first segment
		distance[0]=point_1.line_interception.interception_point.distance((this->raster_segments[index_rast_seg[0]].point1));
		distance[1]=point_1.line_interception.interception_point.distance((this->raster_segments[index_rast_seg[0]].point2));
		//second segment
		distance[2]=point_2.line_interception.interception_point.distance((this->raster_segments[index_rast_seg[1]].point1));
		distance[3]=point_2.line_interception.interception_point.distance((this->raster_segments[index_rast_seg[1]].point2));
		//assign min distance index 
		if(distance[0]+distance[2]>=distance[1]+distance[3]){
			min_dist_index=1;
		}
		else{
			min_dist_index=0;
		}
		//set the values
		if(min_dist_index==0){
			//set data first point
			buffer.flag=line->get_segments_no_overflow(index_line[0]);
			buffer.value_h=line->get_segments_height(index_line[0]);
			buffer.value_pol=line->get_segments_poleni(index_line[0]);
			buffer.index=this->raster_segments[index_rast_seg[0]].get_firstpoint_index();
			this->raster_point_list.set_new_point(&buffer);
			//set data second point
			buffer.flag=line->get_segments_no_overflow(index_line[1]);
			buffer.value_h=line->get_segments_height(index_line[1]);
			buffer.value_pol=line->get_segments_poleni(index_line[1]);
			buffer.index=this->raster_segments[index_rast_seg[1]].get_firstpoint_index();
			this->raster_point_list.set_new_point(&buffer);
			return found_flag=true;

		}
		else{
			//set data first point
			buffer.flag=line->get_segments_no_overflow(index_line[0]);
			buffer.value_h=line->get_segments_height(index_line[0]);
			buffer.value_pol=line->get_segments_poleni(index_line[0]);
			buffer.index=this->raster_segments[index_rast_seg[0]].get_secondpoint_index();
			this->raster_point_list.set_new_point(&buffer);

			//set data second point
			buffer.flag=line->get_segments_no_overflow(index_line[1]);
			buffer.value_h=line->get_segments_height(index_line[1]);
			buffer.value_pol=line->get_segments_poleni(index_line[1]);
			buffer.index=this->raster_segments[index_rast_seg[1]].get_secondpoint_index();
			this->raster_point_list.set_new_point(&buffer);
			return found_flag=true;

		}
	}
	return found_flag=false;
}
//Assign a value from a line to raster points (first point is hit, second is on segment) 
bool Hyd_Floodplain_Raster::assign_points_value2list_point_segment( _geo_multisegment_interception point_1, _geo_multisegment_interception point_2, Hyd_Floodplain_Polysegment *line){

	//found flag
	bool found_flag=false;
	//index of the intercepted raster segment (second point is just on segment
	int index_rast_2=point_2.index_is_intercept;
	//index of the intercepting line segments (dikeline)
	int index_line[2];
	index_line[0]=point_1.index_intercepts;
	index_line[1]=point_2.index_intercepts;
	//this buffer is filled and given to the list
	_hyd_value2pointindex buffer;

	//set the hitted point
	buffer.index=point_1.index_point_is_intercepted;		
	buffer.flag=line->get_segments_no_overflow(index_line[0]);
	buffer.value_h=line->get_segments_height(index_line[0]);
	buffer.value_pol=line->get_segments_poleni(index_line[0]);
	this->raster_point_list.set_new_point(&buffer);

	//they are on one segement
	if(point_1.index_point_is_intercepted==this->raster_segments[index_rast_2].get_firstpoint_index() ||
		point_1.index_point_is_intercepted==this->raster_segments[index_rast_2].get_secondpoint_index()){

			return found_flag=true;
	}
	//set the nearest point to the hitted point of the second segment
	else if(abs(point_1.index_point_is_intercepted-this->raster_segments[index_rast_2].get_firstpoint_index())==1 ||
		abs(point_1.index_point_is_intercepted-this->raster_segments[index_rast_2].get_firstpoint_index())==this->geo_info.number_x+1){
			buffer.index=this->raster_segments[index_rast_2].get_firstpoint_index();
			buffer.flag=line->get_segments_no_overflow(index_line[1]);
			buffer.value_h=line->get_segments_height(index_line[1]);
			buffer.value_pol=line->get_segments_poleni(index_line[1]);
			this->raster_point_list.set_new_point(&buffer);
	}
	else if(abs(point_1.index_point_is_intercepted-this->raster_segments[index_rast_2].get_secondpoint_index())==1 ||
		abs(point_1.index_point_is_intercepted-this->raster_segments[index_rast_2].get_secondpoint_index())==this->geo_info.number_x+1){
			buffer.index=this->raster_segments[index_rast_2].get_secondpoint_index();
			buffer.flag=line->get_segments_no_overflow(index_line[1]);
			buffer.value_h=line->get_segments_height(index_line[1]);
			buffer.value_pol=line->get_segments_poleni(index_line[1]);
			this->raster_point_list.set_new_point(&buffer);
	}
	return found_flag=false;
}
//Assign a value from a line to raster points (second point is hit, first is on segment) 
bool Hyd_Floodplain_Raster::assign_points_value2list_segment_point( _geo_multisegment_interception point_1, _geo_multisegment_interception point_2, Hyd_Floodplain_Polysegment *line){
	//found flag
	bool found_flag=false;
	//index of the intercepted raster segment (first point is just on segment)
	int index_rast_1=point_1.index_is_intercept;
	//index of the intercepting line segments (dikeline)
	int index_line=point_1.index_intercepts;
	//this buffer is filled and given to the list
	_hyd_value2pointindex buffer;

	//they are on one segement
	if(point_2.index_point_is_intercepted==this->raster_segments[index_rast_1].get_firstpoint_index() ||
		point_2.index_point_is_intercepted==this->raster_segments[index_rast_1].get_secondpoint_index()){
			//nothing happen in the next loop it is set
			return found_flag=true;
	}
	//set the nearest point to the hitted point of the second segment
	else if(abs(point_2.index_point_is_intercepted-this->raster_segments[index_rast_1].get_firstpoint_index())==1 ||
		abs(point_2.index_point_is_intercepted-this->raster_segments[index_rast_1].get_firstpoint_index())==this->geo_info.number_x+1){
			buffer.index=this->raster_segments[index_rast_1].get_firstpoint_index();

			buffer.flag=line->get_segments_no_overflow(index_line);
			buffer.value_h=line->get_segments_height(index_line);
			buffer.value_pol=line->get_segments_poleni(index_line);
			this->raster_point_list.set_new_point(&buffer);
	}
	else if(abs(point_2.index_point_is_intercepted-this->raster_segments[index_rast_1].get_secondpoint_index())==1 ||
		abs(point_2.index_point_is_intercepted-this->raster_segments[index_rast_1].get_secondpoint_index())==this->geo_info.number_x+1){
			buffer.index=this->raster_segments[index_rast_1].get_secondpoint_index();
			buffer.flag=line->get_segments_no_overflow(index_line);
			buffer.value_h=line->get_segments_height(index_line);
			buffer.value_pol=line->get_segments_poleni(index_line);
			this->raster_point_list.set_new_point(&buffer);
	}


	return found_flag=false;
}
//Assign the points with values in the list to the raster segments
void Hyd_Floodplain_Raster::assign_listvalues2elements(Hyd_Floodplain_Polysegment *line){
	_hyd_value2pointindex point1;
	_hyd_value2pointindex point2;
	int seg_index=-1;
	//loop over all points in the list
	for(int i=0; i< this->raster_point_list.get_number_points()-1 ; i++){
		point1=this->raster_point_list.get_point(i);
		point2=this->raster_point_list.get_point(i+1);
		//take just pairs of different points
		if(point1.index!=point2.index){
			seg_index=this->point_index2segment_index(point1.index, point2.index);

			if(seg_index!=-1 && seg_index < this->number_segments){
				this->raster_segments[seg_index].assign_value_flag2floodplain_element(0.5*(point1.value_h+point2.value_h),point1.value_pol,point1.flag);
			}
			else{
				//not needed
				//Error msg=this->set_error(1);
				//throw msg;
			}
		}
	}
	//if closed_flag is et the last and the first point are connected too
	if(this->raster_point_list.get_number_points()>2 && line->get_is_closed()==true){
		point1=this->raster_point_list.get_point(this->raster_point_list.get_number_points()-1);
		point2=this->raster_point_list.get_point(0);
		//take just pairs of different points
		if(point1.index!=point2.index){
			seg_index=this->point_index2segment_index(point1.index, point2.index);

			if(seg_index!=-1 && seg_index < this->number_segments){
				this->raster_segments[seg_index].assign_value_flag2floodplain_element(0.5*(point1.value_h+point2.value_h), point1.value_pol,point1.flag);
			}
			else{
				Error msg=this->set_error(1);
				throw msg;
			}
		}
	}
}
//Convert two interception points of a rasterpolygon into the element index
int Hyd_Floodplain_Raster::convert_interception_points2elemindex(_geo_multisegment_interception point_1 , _geo_multisegment_interception point_2, const _hyd_floodplain_polysegment_type type){
	int elem_index=-1;
	//two interception points intercept the segments of the raster
	if(point_1.index_point_is_intercepted==-1 && point_2.index_point_is_intercepted==-1){
		elem_index=this->segments_index2elem_index(point_1.index_is_intercept, point_2.index_is_intercept, type);
	}
	//two interception points are on two rasterpoints 
	else if(point_1.index_point_is_intercepted!=-1 && point_2.index_point_is_intercepted!=-1){
		elem_index=this->points_index2elem_index(point_1.index_point_is_intercepted,point_2.index_point_is_intercepted, type);
	}
	//first interception point is on a pointraster; the second interception point intercepts a rastersegment
	else if(point_1.index_point_is_intercepted!=-1 && point_2.index_point_is_intercepted==-1){
		elem_index=this->points_segment_index2elem_index(point_2.index_is_intercept,point_1.index_point_is_intercepted, type);

	}
	//first interception point intercepts a rastersegment; the second interception is on a rasterpoint
	else if(point_1.index_point_is_intercepted==-1 && point_2.index_point_is_intercepted!=-1){
		elem_index=this->points_segment_index2elem_index(point_1.index_is_intercept,point_2.index_point_is_intercepted, type);
	}
	return elem_index;
}
//Convert two interception points of a rasterpolygon into the element index; just for one point=> the intercepting line starts in a boundary element 
int Hyd_Floodplain_Raster::convert_interception_points2elemindex(_geo_multisegment_interception point_1){
	int elem_index=-1;
	//the interception is on a raster point
	if(point_1.index_point_is_intercepted!=-1){
		//corner down right
		if(point_1.index_point_is_intercepted==this->geo_info.number_x){
			elem_index=point_1.index_point_is_intercepted-1;
		}
		else if(point_1.index_point_is_intercepted==this->number_points-1){
			elem_index=(point_1.index_point_is_intercepted-1)-(this->geo_info.number_x+this->geo_info.number_y);
		}
		//first row
		else if(point_1.index_point_is_intercepted<this->geo_info.number_x){
			elem_index=point_1.index_point_is_intercepted;
		}
		//last row
		else if(point_1.index_point_is_intercepted<this->number_points-1 && point_1.index_point_is_intercepted>=(this->number_points-1)-this->geo_info.number_x){
			elem_index=point_1.index_point_is_intercepted-(this->geo_info.number_x+this->geo_info.number_y);
		}
		//first column
		else if(point_1.index_point_is_intercepted % (this->geo_info.number_x+1)==0){
			elem_index=point_1.index_point_is_intercepted-(point_1.index_point_is_intercepted/(this->geo_info.number_x+1))-this->geo_info.number_x;
		}
		//last column
		else if(point_1.index_point_is_intercepted+1 % (this->geo_info.number_x+1)==0){
			elem_index=point_1.index_point_is_intercepted-((point_1.index_point_is_intercepted+1)/(this->geo_info.number_x+1))-this->geo_info.number_x;
		}

	}
	//the interception is on a raster segment
	else{
		int row=(int)(point_1.index_is_intercept/(this->geo_info.number_x*2+1));
		//first row
		if(point_1.index_is_intercept<this->geo_info.number_x){
			elem_index=point_1.index_is_intercept;
		}
		//last row
		else if(point_1.index_is_intercept<this->number_segments && point_1.index_is_intercept>this->number_segments-1-this->geo_info.number_x){
			elem_index=point_1.index_is_intercept-row*(this->geo_info.number_x+1)-this->geo_info.number_x;
		}
		//first column
		else if((point_1.index_is_intercept-(row+1)*this->geo_info.number_x-row*(this->geo_info.number_x+1))==0){
			elem_index=point_1.index_is_intercept-this->geo_info.number_x-row*(this->geo_info.number_x+1);
		}
		//last column
		else if((point_1.index_is_intercept-(row+2)*this->geo_info.number_x-(row)*(this->geo_info.number_x+1))==0){
			elem_index=point_1.index_is_intercept-this->geo_info.number_x-row*(this->geo_info.number_x+1)-1;
		}

	}
	return elem_index;
}
//Find the index of a segment, when two point indices are given
int Hyd_Floodplain_Raster::point_index2segment_index(const int point_index1, const int point_index2){
	int seg_index=-1;
	int x_row1=int(point_index1/(this->geo_info.number_x+1));
	int x_row2=int(point_index2/(this->geo_info.number_x+1));

	//in the same row
	if(x_row1==x_row2){
		if(point_index1>point_index2){
			seg_index=point_index2+this->geo_info.number_x*x_row1;
		}
		else{
			seg_index=point_index1+this->geo_info.number_x*x_row1;
		}
	}
	//one row above
	else if(abs(x_row1-x_row2)==1){
		if(point_index1>point_index2){
			seg_index=point_index2+this->geo_info.number_x*(x_row2+1);
		}
		else{
			seg_index=point_index1+this->geo_info.number_x*(x_row1+1);
		}

	}



	return seg_index;
}
//Find the indices of two points, when a segment index is given
void Hyd_Floodplain_Raster::segment_index2point_index(const int seg_index, int *point_index1, int *point_index2){
	
	if(seg_index>=0 && seg_index<this->number_segments){
		*point_index1=this->raster_segments[seg_index].get_firstpoint_index();
		*point_index2=this->raster_segments[seg_index].get_secondpoint_index();
	}
	else{
		*point_index1=-1;
		*point_index2=-1;
	}

}
//Find element index, when two segments indices are given
int Hyd_Floodplain_Raster::segments_index2elem_index(const int seg_index_1 , const int seg_index_2, const _hyd_floodplain_polysegment_type type){
	int elem_index=-1;
	//segment row
	int row_1=(int)(seg_index_1/(this->geo_info.number_x*2+1));
	int row_2=(int)(seg_index_2/(this->geo_info.number_x*2+1));

	//Same segment
	if(seg_index_1==seg_index_2){
		int point_1=-1;
		int point_2=-1;
		this->segment_index2point_index(seg_index_1, &point_1, &point_2);
		elem_index=this->points_index2elem_index(point_1, point_2, type);

	}
	// opposite direction ||
	else if(abs(seg_index_1-seg_index_2)==1){
		if(seg_index_1>seg_index_2){
			elem_index=seg_index_2-this->geo_info.number_x-row_2*(this->geo_info.number_x+1);
		}
		else{

			elem_index=seg_index_1-this->geo_info.number_x-row_1*(this->geo_info.number_x+1);
		}
	}
	//opposite direction =
	else if(abs(seg_index_1-seg_index_2)==(2*this->geo_info.number_x+1)){
		if(seg_index_1>seg_index_2){
			elem_index=seg_index_2-row_2*(this->geo_info.number_x+1);
		}
		else{

			elem_index=seg_index_1-row_1*(this->geo_info.number_x+1);
		}
	}
	//corner  |-  _|
	else if(abs(seg_index_1-seg_index_2)==this->geo_info.number_x+1){
		// _|corner	
		if(row_2==row_1){
			if(seg_index_1>seg_index_2){
				elem_index=seg_index_2-row_2*(this->geo_info.number_x+1);
			}
			else{
				elem_index=seg_index_1-row_1*(this->geo_info.number_x+1);
			}
		}// |-corner 
		else{
			if(seg_index_1>seg_index_2){
				elem_index=seg_index_2-this->geo_info.number_x-row_2*(this->geo_info.number_x+1);
			}
			else{
				elem_index=seg_index_1-this->geo_info.number_x-row_1*(this->geo_info.number_x+1);
			}
		}
	}
	//corner |_  -|
	else if(abs(seg_index_1-seg_index_2)==this->geo_info.number_x){
		//|_corner
		if(row_2==row_1){
			if(seg_index_1>seg_index_2){
				elem_index=seg_index_2-row_2*(this->geo_info.number_x+1);
			}
			else{
				elem_index=seg_index_1-row_1*(this->geo_info.number_x+1);
			}
		}
		// -|corner
		else{
			if(seg_index_1>seg_index_2){
				elem_index=seg_index_2-(row_2+1)*(this->geo_info.number_x+1);
			}
			else{
				elem_index=seg_index_1-(row_1+1)*(this->geo_info.number_x+1);
			}
		}
	}

	return elem_index;

}
//Find element index, when two point indices are given; there will be only a result, by diagonal points
int Hyd_Floodplain_Raster::points_index2elem_index(const int point_index_1 , const int point_index_2){
	int elem_index=-1;
	//point row
	int row_1=(int)(point_index_1/(this->geo_info.number_x+1));
	int row_2=(int)(point_index_2/(this->geo_info.number_x+1));

	//  /diagonal
	if(abs(point_index_1-point_index_2)==this->geo_info.number_x+2){
		if(point_index_1>point_index_2){
			elem_index=point_index_2-row_2;
		}
		else{
			elem_index=point_index_1-row_1;
		}

	}
	//  \diagonal
	else if(abs(point_index_1-point_index_2)==this->geo_info.number_x){
		if(point_index_1>point_index_2){
			elem_index=point_index_2-row_2-1;
		}
		else{
			elem_index=point_index_1-row_1-1;
		}
	}

	return elem_index;

}
//Find element index, when two point indices are given; if the points are on the same segment the element index is decided with the linetype
int Hyd_Floodplain_Raster::points_index2elem_index(const int point_index_1 , const int point_index_2, const _hyd_floodplain_polysegment_type type){
	int elem_index=-1;
	//point row
	int row_1=(int)(point_index_1/(this->geo_info.number_x+1));
	int row_2=(int)(point_index_2/(this->geo_info.number_x+1));

	//  /diagonal
	if(abs(point_index_1-point_index_2)==this->geo_info.number_x+2){
		if(point_index_1>point_index_2){
			elem_index=point_index_2-row_2;
		}
		else{
			elem_index=point_index_1-row_1;
		}

	}
	//  \diagonal
	else if(abs(point_index_1-point_index_2)==this->geo_info.number_x){
		if(point_index_1>point_index_2){
			elem_index=point_index_2-row_2-1;
		}
		else{
			elem_index=point_index_1-row_1-1;
		}
	}
	//-horizontal
	else if(abs(point_index_1-point_index_2)==1){
		int elem_index_2=-1;
		//first row=> just one element possibile
		if(row_1==0){
			//up
			if(point_index_1>point_index_2){
				elem_index=point_index_2-row_2;
			}
			else{
				elem_index=point_index_1-row_1;
			}
			elem_index_2=-1;

		}
		//last row=> just one element possibile
		else if(row_1==this->geo_info.number_y){
			//down
			if(point_index_1>point_index_2){
				elem_index=point_index_2-row_2-this->geo_info.number_x;
			}
			else{
				elem_index=point_index_1-row_1-this->geo_info.number_x;
			}
			elem_index_2=-1;

		}
		else{
			//first possible element up
			if(point_index_1>point_index_2){
				elem_index=point_index_2-row_2;
			}
			else{
				elem_index=point_index_1-row_1;
			}
			//second possible element down
			if(point_index_1>point_index_2){
				elem_index_2=point_index_2-row_2-this->geo_info.number_x;
			}
			else{
				elem_index_2=point_index_1-row_1-this->geo_info.number_x;
			}
		}
		elem_index=this->decide_elemindex_per_type(elem_index, elem_index_2, type);

	}
	//| vertical
	else if(abs(point_index_1-point_index_2)==this->geo_info.number_x+1){
		int elem_index_2=-1;
		//first row=> just one element possibile
		if((point_index_1 % (this->geo_info.number_x+1))==0){
			//right
			if(point_index_1>point_index_2){
				elem_index=point_index_2-row_2;
			}
			else{
				elem_index=point_index_1-row_1;
			}
			elem_index_2=-1;

		}
		//last row=> just one element possibile
		else if(((point_index_1+1) % (this->geo_info.number_x+1))==0){
			//left
			if(point_index_1>point_index_2){
				elem_index=point_index_2-row_2-1;
			}
			else{
				elem_index=point_index_1-row_1-1;
			}
			elem_index_2=-1;

		}
		else{
			//first right possible element
			if(point_index_1>point_index_2){
				elem_index=point_index_2-row_2;
			}
			else{
				elem_index=point_index_1-row_1;
			}
			//left possible element
			if(point_index_1>point_index_2){
				elem_index_2=point_index_2-row_2-1;
			}
			else{
				elem_index_2=point_index_1-row_1-1;
			}
		}
		elem_index=this->decide_elemindex_per_type(elem_index, elem_index_2, type);

	}

	return elem_index;

}
//Find element index, when one segments index and one point index are given
int Hyd_Floodplain_Raster::points_segment_index2elem_index(const int seg_index , const int point_index, const _hyd_floodplain_polysegment_type type){
	int elem_index=-1;
	
	//it is not possible to find an unique element index
	if(point_index==this->raster_segments[seg_index].get_firstpoint_index()){
		elem_index=this->points_index2elem_index(point_index,this->raster_segments[seg_index].get_secondpoint_index(), type);
	}	
	else if( point_index==this->raster_segments[seg_index].get_secondpoint_index()){
		elem_index=this->points_index2elem_index(point_index,this->raster_segments[seg_index].get_firstpoint_index(), type);
	}
	else{
		//find the diagonal point
		if(abs(point_index-this->raster_segments[seg_index].get_firstpoint_index())==this->geo_info.number_x ||
			abs(point_index-this->raster_segments[seg_index].get_firstpoint_index())==(this->geo_info.number_x+2) ){
				elem_index=this->points_index2elem_index(point_index,this->raster_segments[seg_index].get_firstpoint_index(), type); 
		}
		else if(abs(point_index-this->raster_segments[seg_index].get_secondpoint_index())==this->geo_info.number_x ||
			abs(point_index-this->raster_segments[seg_index].get_secondpoint_index())==(this->geo_info.number_x+2) ){
				elem_index=this->points_index2elem_index(point_index,this->raster_segments[seg_index].get_secondpoint_index(), type);
		}

	}

	return elem_index;

}
//Set an element type (_hyd_elem_type) to the element with the given element index
void Hyd_Floodplain_Raster::set_elem_type_per_elemindex(const int elem_index, _hyd_elem_type type){
	if(elem_index>=0 && elem_index < this->number_polygons){
		this->floodplain_pointer[elem_index].set_element_type(type);
	}
}
//Decide the element index by a given _hyd_floodplain_polysegment_type and two element indices
int Hyd_Floodplain_Raster::decide_elemindex_per_type(const int index_1, const int index_2, const _hyd_floodplain_polysegment_type type){
	int elem_index=-1;
	if(type==_hyd_floodplain_polysegment_type::COASTLINE || type==_hyd_floodplain_polysegment_type::RIVERBANKLINE || type==_hyd_floodplain_polysegment_type::FP_BOUNDARYLINE){
		if(index_1==-1 && index_2!=-1){
			if(this->floodplain_pointer[index_2].get_elem_type() ==_hyd_elem_type::STANDARD_ELEM || this->floodplain_pointer[index_2].get_elem_type()==_hyd_elem_type::DIKELINE_ELEM){
				elem_index=index_2;
				return elem_index;
			}
		}
		else if(index_2==-1 && index_1!=-1){
			if(this->floodplain_pointer[index_1].get_elem_type() ==_hyd_elem_type::STANDARD_ELEM || this->floodplain_pointer[index_1].get_elem_type() ==_hyd_elem_type::DIKELINE_ELEM){
				elem_index=index_1;
				return elem_index;
			}
		}
		else{
			if(this->floodplain_pointer[index_1].get_elem_type() ==_hyd_elem_type::STANDARD_ELEM || this->floodplain_pointer[index_1].get_elem_type() ==_hyd_elem_type::DIKELINE_ELEM){
				elem_index=index_1;
				return elem_index;
			}
			else if(this->floodplain_pointer[index_2].get_elem_type() ==_hyd_elem_type::STANDARD_ELEM || this->floodplain_pointer[index_2].get_elem_type() ==_hyd_elem_type::DIKELINE_ELEM){
				elem_index=index_2;
				return elem_index;
			}
		}
	}
	return elem_index;
}
//Convert an angle to a correspondig direction of a neighbouring raster element (just four direction)
_hyd_all_element_direction Hyd_Floodplain_Raster::convert_angle2four_direction(const double angle){
	_hyd_all_element_direction direction;
	if(angle >=0.0 && angle <= 45.0){
		direction=_hyd_all_element_direction::X_DIRECT;
	}
	else if(angle > 45.0 && angle <=135.0){
		direction=_hyd_all_element_direction::Y_DIRECT;
	}
	else if(angle > 135.0 && angle <=225.0){
		direction=_hyd_all_element_direction::MINUS_X_DIRECT;
	}
	else if(angle > 225.0 && angle <=315.0){
		direction=_hyd_all_element_direction::MINUS_Y_DIRECT;
	}
	else if(angle > 315.0 && angle <=360.0){
		direction=_hyd_all_element_direction::X_DIRECT;
	}

	return direction;
}
//Convert an angle to a correspondig direction of a neighbouring raster element (eight directions)
_hyd_all_element_direction Hyd_Floodplain_Raster::convert_angle2direction(const double angle){
	_hyd_all_element_direction direction;
	if(angle >=0.0 && angle <= 22.5){
		direction=_hyd_all_element_direction::X_DIRECT;
	}
	else if(angle > 22.5 && angle <=67.5){
		direction=_hyd_all_element_direction::XY_DIRECT;
	}
	else if(angle > 67.5 && angle <=112.5){
		direction=_hyd_all_element_direction::Y_DIRECT;
	}
	else if(angle > 112.5 && angle <=157.5){
		direction=_hyd_all_element_direction::YMINUS_X_DIRECT;
	}
	else if(angle > 157.5 && angle <=202.5){
		direction=_hyd_all_element_direction::MINUS_X_DIRECT;
	}
	else if(angle > 202.5 && angle <=247.5){
		direction=_hyd_all_element_direction::MINUS_X_MINUS_Y_DIRECT;
	}
	else if(angle > 247.5 && angle <=292.5){
		direction=_hyd_all_element_direction::MINUS_Y_DIRECT;
	}
	else if(angle > 292.5 && angle <=337.5){
		direction=_hyd_all_element_direction::MINUS_Y_X_DIRECT;
	}
	else if(angle > 337.5 && angle <360.0){
		direction=_hyd_all_element_direction::X_DIRECT;
	}
	return direction;
}
//Convert an angle to a second possible direction of a neighbouring raster element
_hyd_all_element_direction Hyd_Floodplain_Raster::convert_angle2seconddirection(const double angle){
	_hyd_all_element_direction direction;
	if(angle >=0.0 && angle <= 22.5){
		direction=_hyd_all_element_direction::XY_DIRECT;
	}
	else if(angle > 22.5 && angle <=45.0){
		direction=_hyd_all_element_direction::X_DIRECT;
	}
	else if(angle > 45.0 && angle <=67.5){
		direction=_hyd_all_element_direction::Y_DIRECT;
	}
	else if(angle > 67.5 && angle <=90.0){
		direction=_hyd_all_element_direction::XY_DIRECT;
	}
	else if(angle > 90.0 && angle <=112.5){
		direction=_hyd_all_element_direction::YMINUS_X_DIRECT;
	}
	else if(angle > 112.5 && angle <=135.0){
		direction=_hyd_all_element_direction::Y_DIRECT;
	}
	else if(angle > 135. && angle <=157.5){
		direction=_hyd_all_element_direction::MINUS_X_DIRECT;
	}
	else if(angle > 157.5 && angle <=180.0){
		direction=_hyd_all_element_direction::YMINUS_X_DIRECT;
	}
	else if(angle > 180.0 && angle <=202.5){
		direction=_hyd_all_element_direction::MINUS_X_MINUS_Y_DIRECT;
	}
	else if(angle > 202.5 && angle <=225.0){
		direction=_hyd_all_element_direction::MINUS_X_DIRECT;
	}
	else if(angle > 225.0 && angle <=247.5){
		direction=_hyd_all_element_direction::MINUS_Y_DIRECT;
	}
	else if(angle > 247.5 && angle <=270.0){
		direction=_hyd_all_element_direction::MINUS_X_MINUS_Y_DIRECT;
	}
	else if(angle > 270.0 && angle <=292.5){
		direction=_hyd_all_element_direction::MINUS_Y_X_DIRECT;
	}
	else if(angle > 292.5 && angle <=315.0){
		direction=_hyd_all_element_direction::MINUS_Y_DIRECT;
	}
	else if(angle > 315.0 && angle <=337.5){
		direction=_hyd_all_element_direction::X_DIRECT;
	}
	else if(angle > 337.5 && angle <360.0){
		direction=_hyd_all_element_direction::MINUS_Y_X_DIRECT;
	}
	return direction;
}
//Calculate the distances between two given elment mid point
double Hyd_Floodplain_Raster::calculate_distance_midpoints(const int index_1, const int index_2){
	double distance=-1.0;

	if(index_1<0 || index_2 <0 || index_1>=this->number_polygons || index_2>=this->number_polygons){
		return distance;
	}
	distance=this->floodplain_pointer[index_1].get_mid_point()->distance(this->floodplain_pointer[index_2].get_mid_point());

	return distance;
}
//set the error
Error Hyd_Floodplain_Raster::set_error(const int err_type){
	string place="Hyd_Floodplain_Raster::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;
	switch (err_type){
		case 0://bad alloc
			place.append("allocate_raster_points/segments(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://no segment index is found to the two points
			place.append("assign_listvalues2segments(Hyd_Floodplain_Polysegment *dikeline)");
			reason="Method is not used in the right way";
			help="Check the code";
			type=6;
			break;
		case 2://bad alloc
			place.append("get_complete_raster_polygons(Hyd_Floodplainraster_Polygon **polygons)");
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
