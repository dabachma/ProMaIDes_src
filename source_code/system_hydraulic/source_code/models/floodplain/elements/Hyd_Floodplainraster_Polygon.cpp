#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Floodplainraster_Polygon.h"

//constructor
Hyd_Floodplainraster_Polygon::Hyd_Floodplainraster_Polygon(void){
	this->ptr2floodplain_elem=NULL;
	this->segments=new Hyd_Floodplainraster_Segment[4];
	this->allocate_segments();
	this->is_ouside=false;
}
//destructor
Hyd_Floodplainraster_Polygon::~Hyd_Floodplainraster_Polygon(void){
	if(this->segments!=NULL){
		delete [] this->segments;
		this->segments=NULL;
	}
}
//___________
//public	
//Set the pointer to the floodplain element
void Hyd_Floodplainraster_Polygon::set_ptr2floodplain_element(Hyd_Element_Floodplain *ptr2elem){
	this->ptr2floodplain_elem=ptr2elem;
}
//Set the polygon's segments
void Hyd_Floodplainraster_Polygon::set_polygon_segments(Hyd_Floodplainraster_Segment *segment_y, Hyd_Floodplainraster_Segment *segment_x, Hyd_Floodplainraster_Segment *segment_minus_y, Hyd_Floodplainraster_Segment *segment_minus_x){
	this->segments[0]=(*segment_y);
	this->segments[1]=(*segment_x);
	this->segments[2]=(*segment_minus_y);
	this->segments[3]=(*segment_minus_x);
	
	//set the Geo_Segments
	Geo_Raster_Polygon::set_polygon_segments(segment_y, segment_x, segment_minus_y, segment_minus_x);
	this->sort_points();
	
}
//Set the midpoint of the polygon
void Hyd_Floodplainraster_Polygon::set_mid_point(Geo_Point *mid){
	double x_buff;
	double y_buff;
	x_buff=this->segments[2].point2->get_xcoordinate()+this->segments[2].get_distance()/2.0;
	y_buff=this->segments[2].point2->get_ycoordinate()+this->segments[0].get_distance()/2.0;
	mid->set_point_coordinate(x_buff, y_buff);

}
//Check if this raster polygon is inside the given Hyd_Floodplain_Polygon and set element type of the given polygon
void Hyd_Floodplainraster_Polygon::is_inside_polygon(Hyd_Floodplain_Polygon *polygon){
//    _polygon_intercept buff;
//    buff=polygon->check_polygon_interception(this);
//    if(buff==_polygon_intercept::inside_with_boundary || buff ==_polygon_intercept::complete_inside){
//            this->ptr2floodplain_elem->set_element_type(polygon->get_elementtype_inside());
//        }



//    Geo_Point buffer;
//    this->set_mid_point(&buffer);
    bool inside=true;

//    if(polygon->check_point_inside(&buffer)==true){
      for(int i=0;i<4;i++){
          if(polygon->check_point_inside(this->get_point(i))==false){
             inside=false;
             break;
          }

      }
      if(inside==true){
         this->ptr2floodplain_elem->set_element_type(polygon->get_elementtype_inside());
       }
    //}
}
//Check if this raster polygon is intercepted by the given Hyd_Floodplain_Polygon and set the given element type 
void Hyd_Floodplainraster_Polygon::is_intercepted_polygon(Hyd_Floodplain_Polygon *polygon, _hyd_elem_type type){
	if(polygon->check_polygon_interception(this)==_polygon_intercept::partly_intercepted){
		this->ptr2floodplain_elem->set_element_type(type);
	}
}
//Calculate the interception points with a Geo_Poly_Segment and fill the Geo_Interception_Point_List 
void Hyd_Floodplainraster_Polygon::calculate_polysegment_interception(Geo_Interception_Point_List *intercept_list, Geo_Polysegment *check_polysegment){
	int number=intercept_list->get_number_points();
	_Geo_Polygon::calculate_polysegment_interception(intercept_list, check_polysegment);
	
	for (int i=intercept_list->get_number_points(); i> number; i--){
		//the first point of the segment is hit; transfer it to the raster point index
		if(intercept_list->get_point(i-1).index_point_is_intercepted==0){
			intercept_list->change_point_index_is_intercepted(i-1,this->segments[intercept_list->get_point(i-1).index_is_intercept].get_firstpoint_index());
		}
		//the second point of the segment is hit; transfer it to the raster point index
		else if(intercept_list->get_point(i-1).index_point_is_intercepted==1){
			intercept_list->change_point_index_is_intercepted(i-1,this->segments[intercept_list->get_point(i-1).index_is_intercept].get_secondpoint_index());

		}
		//transfer the segment index to the raster segment index
		intercept_list->change_index_is_intercepted(i-1,this->segments[intercept_list->get_point(i-1).index_is_intercept].get_segment_index());
	
	}
}

//output members
void Hyd_Floodplainraster_Polygon::output_members(ostringstream *cout){
	for(int i=0; i< this->number_segments; i++){
		*cout << this->ptr2floodplain_elem->get_elem_number() << W(5) << i << W(12) << P(2)<< FORMAT_FIXED_REAL<< this->segments[i].point1->get_xcoordinate();
		*cout << W(12) << P(2)<< FORMAT_FIXED_REAL << this->segments[i].point1->get_ycoordinate()<<endl;
	}
}
//output the header (just once)
void Hyd_Floodplainraster_Polygon::output_header(ostringstream *cout){
	*cout << "No." << W(5) << "P.No" << W(8) << "x" << label::m << W(8) << "y" << label::m << endl; 
}
//Get is-outside flag
bool Hyd_Floodplainraster_Polygon::get_is_outside_flag(void){
	return this->is_ouside;
}
//Set is-outside flag
void Hyd_Floodplainraster_Polygon::set_is_outside_flag(const bool flag){
	this->is_ouside=flag;
}
//_________
//private
//Sort the points
void Hyd_Floodplainraster_Polygon::sort_points(void){
	for(int i=1; i<this->number_segments;i++){
		if(this->segments[i-1].point2!=this->segments[i].point1){
			this->segments[i].switch_points();
		}
	}
}
