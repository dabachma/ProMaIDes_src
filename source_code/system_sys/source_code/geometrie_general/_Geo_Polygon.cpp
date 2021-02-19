#include "Sys_Headers_Precompiled.h"
//#include "_Geo_Polygon.h"

//constructor
_Geo_Polygon::_Geo_Polygon(void){
	this->segments=NULL;
	this->number_segments=0;
	this->segment_alloc_here=false;
}
//destructor
_Geo_Polygon::~_Geo_Polygon(void){
	this->delete_segments();
}
//___________
//public
//Get the number of segments of the polygon
int _Geo_Polygon::get_number_segments(void){
	return this->number_segments;
}
//Get the number of points
int _Geo_Polygon::get_number_points(void){
	return this->number_segments;
}

//Calculate the area of the polygon
double _Geo_Polygon::calculate_area(void){
	double area=0.0;

	if(this->number_segments<3){
		return 0.0;
	}

	for(int i=0; i<this->number_segments-1; i++){
		area=area+(this->segments[i].point1.get_xcoordinate()-this->segments[i+1].point1.get_xcoordinate())*(this->segments[i].point1.get_ycoordinate()+this->segments[i+1].point1.get_ycoordinate());
	}
	area = area + (this->segments[this->number_segments-1].point1.get_xcoordinate()-this->segments[0].point1.get_xcoordinate())*(this->segments[this->number_segments-1].point1.get_ycoordinate()+this->segments[0].point1.get_ycoordinate());

	return abs(area/2.0);
}
//Calculate the perimeter of the polygon
double _Geo_Polygon::calculate_perimeter(void){
	double perimeter=0.0;
	for(int i=0; i< this->number_segments;i++){
		perimeter=perimeter+this->segments[i].get_distance();
	}

	return perimeter;
}
//Check if a point is inside the polygon
bool _Geo_Polygon::check_point_inside(Geo_Point *check_point){
	bool inside_flag=false;
	//first check if the point is at the boundary
	if(this->check_point_atboundary(check_point)==true){
		return inside_flag=false;	
	}
	//check the interception with the even and odd-rule for the interception of a ray and the polygons segment;
	Geo_Ray check_ray;
	_geo_interception_point intercept_point;
	intercept_point.interception_point.set_point_name(label::interception_point);
	int intercept_counter=0;
	bool no_point_inter=true;

	double angle=0;
	double angle_step=0.1;

	do{
		//reset it 
		no_point_inter=true;
		intercept_counter=0;
		check_ray.set_coordinates(check_point->get_xcoordinate(), check_point->get_ycoordinate(),check_point->get_xcoordinate()+cos(angle),check_point->get_ycoordinate()+sin(angle));
		//check it
		for(int i=0; i< this->number_segments;i++){
			check_ray.calc_interception(&(this->segments[i]),&intercept_point);
			if(intercept_point.interception_flag==true && intercept_point.indefinite_flag==false){
				//full interception
				if(intercept_point.interception_point!=this->segments[i].point1 && intercept_point.interception_point!=this->segments[i].point2){
					intercept_counter++;
				}
				else if(intercept_point.interception_point==this->segments[i].point1 || intercept_point.interception_point==this->segments[i].point2){
					no_point_inter=false;

					angle=angle+angle_step;
					if(angle>constant::Cpi*2.0){
						Error msg=this->set_error(3);
						throw msg;
					}
					break;
				}
			}
		}
	}
	while(no_point_inter==false);

	if(intercept_counter ==0){
		inside_flag=false;
	}
	else if(intercept_counter % 2 !=0){
		inside_flag=true;
	}
	else{
		inside_flag=false;
	}

	return inside_flag;

}
//Check if a point is inside or at boundary the polygon
bool _Geo_Polygon::check_point_inside_atboundary(Geo_Point *check_point){
	bool inside_flag=false;
	//first check if the point is at the boundary
	if(this->check_point_atboundary(check_point)==true){
		return inside_flag=true;	
	}
	//check the interception with the even and odd-rule for the interception of a ray and the polygons segment;
	Geo_Ray check_ray;
	_geo_interception_point intercept_point;
	intercept_point.interception_point.set_point_name(label::interception_point);
	int intercept_counter=0;
	bool no_point_inter=true;
	int ray_counter=0;
	double angle=0.0;
	double angle_step=0.1;

	do{
		//reset it 
		no_point_inter=true;
		intercept_counter=0;
		check_ray.set_coordinates(check_point->get_xcoordinate(), check_point->get_ycoordinate(),check_point->get_xcoordinate()+cos(angle),check_point->get_ycoordinate()+sin(angle));
		//check it
		for(int i=0; i< this->number_segments;i++){
			check_ray.calc_interception(&(this->segments[i]),&intercept_point);
			if(intercept_point.interception_flag==true && intercept_point.indefinite_flag==false){
				//full interception
				if(intercept_point.interception_point!=this->segments[i].point1 && intercept_point.interception_point!=this->segments[i].point2){
					intercept_counter++;
				}
				else if(intercept_point.interception_point==this->segments[i].point1 || intercept_point.interception_point==this->segments[i].point2){
					no_point_inter=false;
					ray_counter++;
					angle=angle_step*(double)ray_counter;
					if(angle>constant::Cpi*2.0){
						Error msg=this->set_error(3);
						throw msg;
					}
					break;
				}
			}
		}
	}
	while(no_point_inter==false);

	if(intercept_counter ==0){
		inside_flag=false;
	}
	else if(intercept_counter % 2 !=0){
		inside_flag=true;
	}
	else{
		inside_flag=false;
	}

	return inside_flag;

}
//Check if a point is at the polygon boundary polygon
bool _Geo_Polygon::check_point_atboundary(Geo_Point *check_point){
	bool at_flag=false;

	for(int i=0; i< this->number_segments;i++){
		at_flag=this->segments[i].calc_point_on_segment(check_point);
		if(at_flag==true){
			break;
		}
	}

	return at_flag;
}
//Check if a point is completly outside the polygon
bool _Geo_Polygon::check_point_outside(Geo_Point *check_point){
	if(this->check_point_atboundary(check_point)==true){
		return false;
	}
	if(this->check_point_inside(check_point)==true){
		return false;
	}

	return true;

}
//Check if a segment is at the boundary of the polygon
bool _Geo_Polygon::check_segment_at_boundary(Geo_Segment *check_segment){
	bool check_flag=false;
	_geo_interception_point buffer;
	buffer.interception_point.set_point_name(label::interception_point);

	for(int i=0; i< this->number_segments; i++){
		check_segment->calc_interception(&(this->segments[i]), &buffer);
		if(buffer.indefinite_flag==true){
			check_flag=true;
			break;
		}
	}

	return check_flag;
}
//Check if a segment is completly inside or at boundary
bool _Geo_Polygon::check_segment_is_inside(Geo_Segment *check_segment){
	bool check_flag=false;
	if(this->check_point_outside(&check_segment->point1)==true || this->check_point_outside(&check_segment->point2)==true){
		return check_flag=false;
	}
	Geo_Interception_Point_List buff;
	this->calculate_segment_interception(&buff, check_segment);
	if(buff.get_number_points()==0){
		check_flag=true;
	}
	else{
		for(int i=0; i<buff.get_number_points();i++){
			if(buff.get_point(i).line_interception.indefinite_flag!=true){
				check_flag=false;
				break;
			}
			else{
				check_flag=true;
			}

		}

	}


	return check_flag;
}
//Check if a segment is completly inside or at boundary
bool _Geo_Polygon::check_segment_is_inside(Geo_Raster_Segment *check_segment){
	bool check_flag=false;
	if(this->check_point_outside(check_segment->point1)==true || this->check_point_outside(check_segment->point2)==true){
		return check_flag=false;
	}
	Geo_Interception_Point_List buff;
	this->calculate_segment_interception(&buff, check_segment);
	if(buff.get_number_points()==0){
		check_flag=true;
	}
	else{
		for(int i=0; i<buff.get_number_points();i++){
			if(buff.get_point(i).line_interception.indefinite_flag!=true){
				check_flag=false;
				break;
			}
			else{
				check_flag=true;
			}

		}

	}


	return check_flag;

}
//Check if a polygon is completly inside, outside ot partly intercepted
_polygon_intercept _Geo_Polygon::check_polygon_interception(_Geo_Polygon *check_polygon){
	_polygon_intercept intercept_enum1=_polygon_intercept::complete_outside;
	_polygon_intercept intercept_enum2=_polygon_intercept::complete_outside;
	int inside_counter=0;
	int at_bound_counter=0;

	//check the given polygon
	for (int i=0; i<check_polygon->get_number_points();i++){
		if(this->check_point_atboundary(check_polygon->get_point(i))==true){
			at_bound_counter++;
		}
		else if(this->check_point_inside(check_polygon->get_point(i))==true){
			inside_counter++;
		}
	}

	//evaluation of the given polygon
	if(inside_counter==check_polygon->get_number_points()){
		intercept_enum1=_polygon_intercept::complete_inside;
	}
	else if(inside_counter+at_bound_counter==check_polygon->get_number_points()){
		intercept_enum1=_polygon_intercept::inside_with_boundary;
	}
	else if(inside_counter+at_bound_counter==0){
		intercept_enum1=_polygon_intercept::complete_outside;
	}
	else if(inside_counter==0 && at_bound_counter!=0){
		intercept_enum1=_polygon_intercept::outside_with_boundary;
		Geo_Interception_Point_List list;
		for(int i=0; i< check_polygon->get_number_segments(); i++){
			this->calculate_segment_interception(&list, check_polygon->get_segment(i));
		}
		for(int i=1; i< list.get_number_points();i++){
			if((list.get_point(i-1).index_is_intercept!=list.get_point(i).index_is_intercept) && 
				list.get_point(i-1).index_point_is_intercepted==-1 && list.get_point(i).index_point_is_intercepted==-1){
				intercept_enum1=_polygon_intercept::partly_intercepted;
			}
		}
	}
	else{
		intercept_enum1=_polygon_intercept::partly_intercepted;
	}

	//check this polygon
	at_bound_counter=0;
	inside_counter=0;
	for (int i=0; i<this->get_number_points();i++){
		if(check_polygon->check_point_atboundary(this->get_point(i))==true){
			at_bound_counter++;
		}
		else if(check_polygon->check_point_inside(this->get_point(i))==true){
			inside_counter++;
		}
	}
	//evaluation of the this polygon
	if(inside_counter==this->get_number_points()){
		intercept_enum2=_polygon_intercept::complete_inside;
	}
	else if(inside_counter+at_bound_counter==this->get_number_points()){
		intercept_enum2=_polygon_intercept::inside_with_boundary;
	}
	else if(inside_counter+at_bound_counter==0){
		intercept_enum2=_polygon_intercept::complete_outside;
	}
	else if(inside_counter==0 && at_bound_counter!=0){
		intercept_enum2=_polygon_intercept::outside_with_boundary;
		Geo_Interception_Point_List list;
		for(int i=0; i< this->get_number_segments(); i++){
			check_polygon->calculate_segment_interception(&list, this->get_segment(i));
		}
		for(int i=1; i< list.get_number_points();i++){
			if((list.get_point(i-1).index_is_intercept!=list.get_point(i).index_is_intercept) && 
				list.get_point(i-1).index_point_is_intercepted==-1 && list.get_point(i).index_point_is_intercepted==-1){
				intercept_enum1=_polygon_intercept::partly_intercepted;
			}
		}
	}
	else{
		intercept_enum2=_polygon_intercept::partly_intercepted;
	}

	//end evaluation
	//outside
	if(intercept_enum2==_polygon_intercept::complete_outside && intercept_enum1==_polygon_intercept::complete_outside){
		if(this->check_interception(check_polygon)==false){
			return _polygon_intercept::complete_outside;
		}
		else{
			return _polygon_intercept::partly_intercepted;
		}
	}
	else if(intercept_enum2==_polygon_intercept::outside_with_boundary && intercept_enum1==_polygon_intercept::outside_with_boundary ){
		if(this->check_interception(check_polygon)==false){
			return _polygon_intercept::outside_with_boundary;
		}
		else{
			return _polygon_intercept::partly_intercepted;
		}
	}
	else if(intercept_enum2==_polygon_intercept::complete_outside && intercept_enum1==_polygon_intercept::outside_with_boundary ){
		if(this->check_interception(check_polygon)==false){
			return _polygon_intercept::outside_with_boundary;
		}
		else{
			return _polygon_intercept::partly_intercepted;
		}
	}
	else if(intercept_enum2==_polygon_intercept::outside_with_boundary && intercept_enum1==_polygon_intercept::complete_outside ){
		if(this->check_interception(check_polygon)==false){
			return _polygon_intercept::outside_with_boundary;
		}
		else{
			return _polygon_intercept::partly_intercepted;
		}
	}
	//inside
	else if(intercept_enum2==_polygon_intercept::complete_outside && intercept_enum1==_polygon_intercept::complete_inside){
		double area_1=check_polygon->calculate_area();
		double area_inter=this->polygon_interception(check_polygon);
		if(abs(area_inter-area_1)>constant::area_epsilon){
			return _polygon_intercept::partly_intercepted;
		}
		
		return _polygon_intercept::complete_inside;
	}
	else if(intercept_enum2==_polygon_intercept::complete_outside && intercept_enum1==_polygon_intercept::inside_with_boundary){
		double area_1=check_polygon->calculate_area();
		double area_inter=this->polygon_interception(check_polygon);
		if(abs(area_inter-area_1)>constant::area_epsilon){
			return _polygon_intercept::partly_intercepted;
		}

		return _polygon_intercept::inside_with_boundary;
	}
	else if(intercept_enum2==_polygon_intercept::outside_with_boundary && intercept_enum1==_polygon_intercept::inside_with_boundary){
		double area_1=check_polygon->calculate_area();
		double area_inter=this->polygon_interception(check_polygon);
		if(abs(area_inter-area_1)>constant::area_epsilon){
			return _polygon_intercept::partly_intercepted;
		}

		return _polygon_intercept::inside_with_boundary;
	}
	else if(intercept_enum2==_polygon_intercept::outside_with_boundary && intercept_enum1==_polygon_intercept::complete_inside){
		double area_1=check_polygon->calculate_area();
		double area_inter=this->polygon_interception(check_polygon);
		if(abs(area_inter-area_1)>constant::area_epsilon){
			return _polygon_intercept::partly_intercepted;
		}

		return _polygon_intercept::inside_with_boundary;
	}
	else if(intercept_enum2==_polygon_intercept::inside_with_boundary && intercept_enum1==_polygon_intercept::inside_with_boundary){
		double area_1=this->calculate_area();
		double area_inter=this->polygon_interception(check_polygon);
		if(abs(area_inter-area_1)>constant::area_epsilon){
			return _polygon_intercept::partly_intercepted;
		}

		return _polygon_intercept::inside_with_boundary;
	}
	else {
		return _polygon_intercept::partly_intercepted;
	}

}
//calculate an interception with a Geo_Segment and fill the Geo_Interception_Point_List 
void _Geo_Polygon::calculate_segment_interception(Geo_Interception_Point_List *intercept_list, Geo_Segment *check_segment){

	//make the interception
	_geo_multisegment_interception buffer;
	buffer.line_interception.interception_point.set_point_name(label::interception_point);

	for(int i=0; i< this->number_segments;i++){
		check_segment->calc_interception(&(this->segments[i]), &(buffer.line_interception));

		if(buffer.line_interception.interception_flag==true){
			buffer.index_intercepts=0;
			buffer.index_is_intercept=i;
			//check if a point is hit
			if(buffer.line_interception.interception_point==this->segments[i].point1){
				buffer.index_point_is_intercepted=0;
			}
			else if(buffer.line_interception.interception_point==this->segments[i].point2){
				buffer.index_point_is_intercepted=1;
			}
			else{
				buffer.index_point_is_intercepted=-1;
			}
			try{
				buffer.distance=check_segment->get_distance_along_segment(&(buffer.line_interception.interception_point));
			}
			catch(Error msg){
				throw msg;
			}
			intercept_list->set_new_point(&buffer);
		}
	}

}
//Calculate the interception points with a Geo_Segment and fill the Geo_Interception_Point_List 
void _Geo_Polygon::calculate_segment_interception(Geo_Interception_Point_List *intercept_list, Geo_Raster_Segment *check_segment){
	//make the interception
	_geo_multisegment_interception buffer;
	buffer.line_interception.interception_point.set_point_name(label::interception_point);

	for(int i=0; i< this->number_segments;i++){
		check_segment->calc_interception(&(this->segments[i]), &(buffer.line_interception));

		if(buffer.line_interception.interception_flag==true){
			buffer.index_intercepts=0;
			buffer.index_is_intercept=i;
			//check if a point is hit
			if(buffer.line_interception.interception_point==(this->segments[i].point1)){
				buffer.index_point_is_intercepted=0;
			}
			else if(buffer.line_interception.interception_point==(this->segments[i].point2)){
				buffer.index_point_is_intercepted=1;
			}
			else{
				buffer.index_point_is_intercepted=-1;
			}
			try{
				buffer.distance=check_segment->get_distance_along_segment(&(buffer.line_interception.interception_point));
			}
			catch(Error msg){
				throw msg;
			}
			intercept_list->set_new_point(&buffer);
		}
	}
}
//calculate an interception with a Geo_Poly_Segment and fill the Geo_Interception_Point_List 
void _Geo_Polygon::calculate_polysegment_interception(Geo_Interception_Point_List *intercept_list, Geo_Polysegment *check_polysegment){

	//make the interception
	_geo_multisegment_interception buffer;
	buffer.line_interception.interception_point.set_point_name(label::interception_point);


	for(int j=0; j< check_polysegment->get_number_segments(); j++){
		for(int i=0; i< this->number_segments;i++){
			this->segments[i].calc_interception(&(check_polysegment->my_segment[j]), &(buffer.line_interception));
			if(buffer.line_interception.interception_flag==true){
				buffer.index_intercepts=j;
				buffer.index_is_intercept=i;
				//check if a point is hit
				if(buffer.line_interception.interception_point==this->segments[i].point1){
					buffer.index_point_is_intercepted=0;
				}
				else if(buffer.line_interception.interception_point==this->segments[i].point2){
					buffer.index_point_is_intercepted=1;
				}
				else{
					buffer.index_point_is_intercepted=-1;
				}

				try{
					buffer.distance=check_polysegment->get_distance_along_polysegment(&(buffer.line_interception.interception_point));
				}
				catch(Error msg){
					
					throw msg;
				}
				intercept_list->set_new_point(&buffer);
				
			}
		}
	}

}
//Check if a polysegement is partly inside a polygon 
bool _Geo_Polygon::check_polysegment_partly_inside(Geo_Polysegment *check_polysegment){
	
	//check the points of the polysegment
	for(int i=0; i<check_polysegment->get_number_segments(); i++){
		if(this->check_point_inside(&check_polysegment->get_segment(i)->point1)==true){
			return true;
		}
	}

	//last point
	if(this->check_point_inside(check_polysegment->get_last_point())==true){
		return true;
	}

	//check if the dikeline intercept the floodplain at one segment
	Geo_Interception_Point_List buff;
	this->calculate_polysegment_interception(&buff, check_polysegment);

	if (buff.get_number_points()>0){
		return true;
	}

	return false;
}
//Calculate the common area of two polygons with the Weiler-Atherton algorithm
double _Geo_Polygon::polygon_interception(_Geo_Polygon *interception){
	double area=0.0;

	Geo_Polygon_Interception_Point_List poly1;
	Geo_Polygon_Interception_Point_List poly2;
	

	Geo_Polygon_Interception_Point buffer;
	Geo_Interception_Point_List buffer_list;

	//make list of polygon 1
	for(int i=0; i<this->number_segments; i++){
		//set polygon points
		buffer.reset_flags();
		buffer.set_point(this->get_point(i));
		buffer.set_polygon_point_flag(true);
		if(interception->check_point_inside(this->get_point(i))==true){
			buffer.set_inside_flag(true);
		}
		else{
			buffer.set_inside_flag(false);
		}
		poly1.set_new_point(&buffer);

		//set interception points
		buffer_list.delete_list();
		interception->calculate_segment_interception(&buffer_list, &this->segments[i]);
		buffer_list.sort_distance();
		
		for(int j=0; j< buffer_list.get_number_points(); j++){
			if(buffer_list.get_point(j).line_interception.interception_flag==true){
				if(buffer_list.get_point(j).line_interception.indefinite_flag==false){
					buffer.reset_flags();
                    buffer.set_point((buffer_list.get_point(j).line_interception.interception_point));
					buffer.set_interception_point_flag(true);
					if(buffer_list.get_point(j).index_point_is_intercepted>=0){
						buffer.set_polygon_point_flag(true);
					}
					poly1.set_new_point(&buffer);
				}
			}
		}

	}
	//set direction of the interception points in the list 1
	poly1.set_interception_point_direction(interception);

	//transfer interception points to list
	//Geo_Polygon_Interception_Point_List buffer_list_2;
	//for(int i=0; i< poly1.get_number_points(); i++){
	//	if(poly1.get_ptr_point(i)->get_interception_point_flag()==true){
	//		buffer_list_2.set_new_point(poly1.get_ptr_point(i));
	//	}
	//}


	//make list of polygon 2
	for(int i=0; i<interception->get_number_segments(); i++){
		//set polygon points
		buffer.reset_flags();
		buffer.set_point(interception->get_point(i));
		buffer.set_polygon_point_flag(true);
		if(this->check_point_inside(interception->get_point(i))==true){
			buffer.set_inside_flag(true);
		}
		else{
			buffer.set_inside_flag(false);
		}
		poly2.set_new_point(&buffer);

		////check interception point
		//for(int j=0; j< buffer_list_2.get_number_points(); j++){
		//	if(interception->segments[i].calc_point_on_segment((buffer_list_2.get_ptr_point(j)))==true){
		//		buffer.reset_flags();
		//		buffer.set_point(buffer_list_2.get_ptr_point(j));
		//		buffer.set_interception_point_flag(true);
		//		poly2->set_new_point(&buffer);
		//	}
		//}

		//set interception points
		buffer_list.delete_list();
		this->calculate_segment_interception(&buffer_list, &interception->segments[i]);
		buffer_list.sort_distance();

		for(int j=0; j< buffer_list.get_number_points(); j++){
			if(buffer_list.get_point(j).line_interception.interception_flag==true){
				if(buffer_list.get_point(j).line_interception.indefinite_flag==false){
					buffer.reset_flags();
                    buffer.set_point(buffer_list.get_point(j).line_interception.interception_point);
					buffer.set_interception_point_flag(true);
					if(buffer_list.get_point(j).index_point_is_intercepted>=0){
						buffer.set_polygon_point_flag(true);
					}
					poly2.set_new_point(&buffer);
				}
			}
		}
	}

	//set direction of the interception points in the list 2
	poly2.set_interception_point_direction(this);

	//set the indices of the corresponding points in the other list
	poly1.set_corresponding_indices(&poly2);

	//output for development
	//ostringstream info;
	//
	//cout <<"poly1"<<endl;
	//poly1.output_members(&info);
	//cout << info.str()<<endl<<endl;
	//info.str("");
	//cout <<"poly2"<<endl;
	//poly2.output_members(&info);
	//cout << info.str()<<endl<<endl;
	//info.str("");
	


	//evaluation
	Geo_Point check_point;
	//special case one polygon consists just of polygon point of the other
	int number1=poly1.get_number_corresponding_points();
	int number2=poly2.get_number_corresponding_points();
	if(number1==this->get_number_points() && number1==poly1.get_number_points()){
		//polygon1 consists just of polygon points of polygon2
		this->get_point_inside(&check_point);
		if(interception->check_point_inside(&check_point)==true){
			area=this->calculate_area();	
		}
		else{
			area=0.0;
		}
		return area;
	}
	if(number2==interception->get_number_points()&& number2==poly2.get_number_points()){
		//polygon2 consists just of polygon points of polygon1
		interception->get_point_inside(&check_point);
		if(this->check_point_inside(&check_point)==true){
			area=interception->calculate_area();	
		}
		else{
			area=0.0;
		}
		return area;
	}

	//special case total outside or total inside
	number1=poly1.get_number_inside_points();
	number2=poly2.get_number_inside_points();
	if(poly1.get_number_points()==this->number_segments || poly2.get_number_points()== interception->get_number_segments()){

		if(number1==this->number_segments){
			this->get_point_inside(&check_point);
			if(interception->check_point_inside(&check_point)==true){
				area=this->calculate_area();	
			}
			else{
				area=0.0;
			}
			return area;
		}
		else if(number2==interception->get_number_segments()){
			interception->get_point_inside(&check_point);
			if(this->check_point_inside(&check_point)==true){
				area=interception->calculate_area();	
			}
			else{
				area=0.0;
			}
			return area;
		}
		else if(number1==0 && number2==0){
			//total outside
			return area=0.0;
		}
	}
	//normal case create the polygons from the two lists
	
	//new create the polygons
	Geo_Point_List polygon_list;
	polygon_list.set_double_point_allowed(true);
	//search for start index
	int counter1=-1;
	int counter2=0;
	int start_index=-1;
	bool changed_flag=false;
	bool break_total=false;
	bool break1=false;
	bool break2=false;

	Geo_Polygon_Interception_Point_List *list1=&poly1;
	Geo_Polygon_Interception_Point_List *list2=&poly2;

	//find start point in list 1
	for(int i=0; i< poly1.get_number_points(); i++){
		if(poly1.get_ptr_point(i)->get_interception_point_flag()==true &&	poly1.get_ptr_point(i)->get_inside_flag()==true){
			counter1=i;
			break;
		}
	}
	//no start is found, change the lists
	if(counter1<0){
		for(int i=0; i< poly2.get_number_points(); i++){
			if(poly2.get_ptr_point(i)->get_interception_point_flag()==true &&	poly2.get_ptr_point(i)->get_inside_flag()==true){
				counter1=i;
				break;
			}
		}
		list1=&poly2;
		list2=&poly1;
		if(counter1<0){
			return 0.0;
		}
	}

	start_index=counter1;
	bool new_start_found=false;

	do{
		int counter_sec=0;
		new_start_found=false;
		do{
			//poly1
			do{
				//check for total break; the new polygon is closed
				if(polygon_list.get_number_points()>0 && polygon_list.get_first_point()==*list1->get_ptr_point(counter1)){
					break_total=true;
					break;
				}
				//set the point to the polygon list
				polygon_list.set_new_point(list1->get_ptr_point(counter1));
				list1->get_ptr_point(counter1)->set_is_used_flag(true);
				counter1++;
				if(counter1>list1->get_number_points()-1){
					counter1=0;
				}
				
				//check for a changing to list 2
				if(list1->get_ptr_point(counter1)->get_interception_point_flag()==true && 
					list1->get_ptr_point(counter1)->get_inside_flag()==false){
						break1=true;
						counter2=list1->get_ptr_point(counter1)->get_index_corresponding_point();
						if(counter2<0){
							ostringstream cout;
							list1->output_members(&cout);
							list2->output_members(&cout);

							Sys_Common_Output::output_fpl->output_txt(&cout);
						}
						list1->get_ptr_point(counter1)->set_is_used_flag(true);
				}
			}while(break1==false);
			break1=false;

			//poly2
			if(break_total==false){
				do{
					changed_flag=true;
					//check for total break; the new polygon is closed
					if(polygon_list.get_number_points()>0 && polygon_list.get_first_point()==*list2->get_ptr_point(counter2)){
						break_total=true;
						break;
					}
					//set the point to the polygon list
					polygon_list.set_new_point(list2->get_ptr_point(counter2));
					list2->get_ptr_point(counter2)->set_is_used_flag(true);
					counter2++;
					if(counter2>list2->get_number_points()-1){
						counter2=0;
					}
					
					//check for a changing to list 1
					if(list2->get_ptr_point(counter2)->get_interception_point_flag()==true && 
						list2->get_ptr_point(counter2)->get_inside_flag()==false){
							break2=true;
							counter1=list2->get_ptr_point(counter2)->get_index_corresponding_point();
							list2->get_ptr_point(counter2)->set_is_used_flag(true);
					}
				}while(break2==false);
				break2=false;
			}
			counter_sec++;
			if(counter_sec==2000){
				break;
			}
		}while(break_total==false);



		break_total=false;
		//there was no change=> no common area
		if(changed_flag==false){
			area=area+0.0;
		}
		else{
			//output of the new polygon list for developement
			/*if(flag==false){
				cout <<" New polygon"<<endl;
				polygon_list.output_members(&info);
				cout << info.str()<<endl;
				info.str("");
			}*/

			area=area+polygon_list.calculate_area();
		}
		changed_flag=false;

		//look for a new start index

		for(int i=start_index+1; i< list1->get_number_points(); i++){
			if(list1->get_ptr_point(i)->get_interception_point_flag()==true &&
				list1->get_ptr_point(i)->get_inside_flag()==true && list1->get_ptr_point(i)->get_is_used_flag()==false){
				counter1=i;
				new_start_found=true;
				start_index=counter1;
				polygon_list.delete_list();
				break;	
			}
		}

	}
	while(new_start_found==true);


	return area;
}
//Calculate the common area of two polygons with the Weiler-Atherton algorithm
double _Geo_Polygon::polygon_interception(_Geo_Polygon *interception, Geo_Polygon_Interception_Point_List *poly1, Geo_Polygon_Interception_Point_List *poly2, Geo_Interception_Point_List *buffer_list, Geo_Point_List *polygon_list, Geo_Polygon_Interception_Point *buffer){
	double area=0.0;

	
	


	//make list of polygon 1
	for(int i=0; i<this->number_segments; i++){
		//set polygon points
		buffer->reset_flags();
		buffer->set_point(this->get_point(i));
		buffer->set_polygon_point_flag(true);
		if(interception->check_point_inside(this->get_point(i))==true){
			buffer->set_inside_flag(true);
		}
		else{
			buffer->set_inside_flag(false);
		}
		poly1->set_new_point(buffer);

		//set interception points
		buffer_list->reset_list();
		interception->calculate_segment_interception(buffer_list, &this->segments[i]);
		buffer_list->sort_distance();
		
		for(int j=0; j< buffer_list->get_number_points(); j++){
			if(buffer_list->get_point(j).line_interception.interception_flag==true){
				if(buffer_list->get_point(j).line_interception.indefinite_flag==false){
					buffer->reset_flags();
                    buffer->set_point((buffer_list->get_point(j).line_interception.interception_point));
					buffer->set_interception_point_flag(true);
					if(buffer_list->get_point(j).index_point_is_intercepted>=0){
						buffer->set_polygon_point_flag(true);
					}
					poly1->set_new_point(buffer);
				}
			}
		}

	}
	//set direction of the interception points in the list 1
	poly1->set_interception_point_direction(interception);

	//transfer interception points to list
	//Geo_Polygon_Interception_Point_List buffer_list_2;
	//for(int i=0; i< poly1->get_number_points(); i++){
	//	if(poly1->get_ptr_point(i)->get_interception_point_flag()==true){
	//		buffer_list_2.set_new_point(poly1->get_ptr_point(i));
	//	}
	//}


	//make list of polygon 2
	for(int i=0; i<interception->get_number_segments(); i++){
		//set polygon points
		buffer->reset_flags();
		buffer->set_point(interception->get_point(i));
		buffer->set_polygon_point_flag(true);
		if(this->check_point_inside(interception->get_point(i))==true){
			buffer->set_inside_flag(true);
		}
		else{
			buffer->set_inside_flag(false);
		}
		poly2->set_new_point(buffer);

		////check interception point
		//for(int j=0; j< buffer_list_2.get_number_points(); j++){
		//	if(interception->segments[i].calc_point_on_segment((buffer_list_2.get_ptr_point(j)))==true){
		//		buffer.reset_flags();
		//		buffer.set_point(buffer_list_2.get_ptr_point(j));
		//		buffer.set_interception_point_flag(true);
		//		poly2->set_new_point(&buffer);
		//	}
		//}

		//set interception points
		buffer_list->reset_list();
		this->calculate_segment_interception(buffer_list, &interception->segments[i]);
		buffer_list->sort_distance();

		for(int j=0; j< buffer_list->get_number_points(); j++){
			if(buffer_list->get_point(j).line_interception.interception_flag==true){
				if(buffer_list->get_point(j).line_interception.indefinite_flag==false){
					buffer->reset_flags();
                    buffer->set_point(buffer_list->get_point(j).line_interception.interception_point);
					buffer->set_interception_point_flag(true);
					if(buffer_list->get_point(j).index_point_is_intercepted>=0){
						buffer->set_polygon_point_flag(true);
					}
					poly2->set_new_point(buffer);
				}
			}
		}
	}

	//set direction of the interception points in the list 2
	poly2->set_interception_point_direction(this);

	//set the indices of the corresponding points in the other list
	poly1->set_corresponding_indices(poly2);

	//output for development
	//ostringstream info;
	//
	//cout <<"poly1"<<endl;
	//poly1->output_members(&info);
	//cout << info.str()<<endl<<endl;
	//info.str("");
	//cout <<"poly2"<<endl;
	//poly2->output_members(&info);
	//cout << info.str()<<endl<<endl;
	//info.str("");
	


	//evaluation
	//Geo_Point check_point;
	//special case one polygon consists just of polygon point of the other
	int number1=poly1->get_number_corresponding_points();
	int number2=poly2->get_number_corresponding_points();
	if(number1==this->get_number_points() && number1==poly1->get_number_points()){
		//polygon1 consists just of polygon points of polygon2
		this->get_point_inside(buffer);
		if(interception->check_point_inside(buffer)==true){
			area=this->calculate_area();	
		}
		else{
			area=0.0;
		}
		return area;
	}
	if(number2==interception->get_number_points()&& number2==poly2->get_number_points()){
		//polygon2 consists just of polygon points of polygon1
		interception->get_point_inside(buffer);
		if(this->check_point_inside(buffer)==true){
			area=interception->calculate_area();	
		}
		else{
			area=0.0;
		}
		return area;
	}

	//special case total outside or total inside
	number1=poly1->get_number_inside_points();
	number2=poly2->get_number_inside_points();
	if(poly1->get_number_points()==this->number_segments || poly2->get_number_points()== interception->get_number_segments()){

		if(number1==this->number_segments){
			this->get_point_inside(buffer);
			if(interception->check_point_inside(buffer)==true){
				area=this->calculate_area();	
			}
			else{
				area=0.0;
			}
			return area;
		}
		else if(number2==interception->get_number_segments()){
			interception->get_point_inside(buffer);
			if(this->check_point_inside(buffer)==true){
				area=interception->calculate_area();	
			}
			else{
				area=0.0;
			}
			return area;
		}
		else if(number1==0 && number2==0){
			//total outside
			return area=0.0;
		}
	}
	//normal case create the polygons from the two lists
	
	//new create the polygons

	polygon_list->set_double_point_allowed(true);
	//search for start index
	int counter1=-1;
	int counter2=0;
	int start_index=-1;
	bool changed_flag=false;
	bool break_total=false;
	bool break1=false;
	bool break2=false;

	Geo_Polygon_Interception_Point_List *list1=poly1;
	Geo_Polygon_Interception_Point_List *list2=poly2;

	//find start point in list 1
	for(int i=0; i< poly1->get_number_points(); i++){
		if(poly1->get_ptr_point(i)->get_interception_point_flag()==true &&	poly1->get_ptr_point(i)->get_inside_flag()==true){
			counter1=i;
			break;
		}
	}
	//no start is found, change the lists
	if(counter1<0){
		for(int i=0; i< poly2->get_number_points(); i++){
			if(poly2->get_ptr_point(i)->get_interception_point_flag()==true &&	poly2->get_ptr_point(i)->get_inside_flag()==true){
				counter1=i;
				break;
			}
		}
		list1=poly2;
		list2=poly1;
		if(counter1<0){
			return 0.0;
		}
	}

	start_index=counter1;
	bool new_start_found=false;

	do{
		int counter_sec=0;
		new_start_found=false;
		do{
			//poly1
			do{
				//check for total break; the new polygon is closed
				if(polygon_list->get_number_points()>0 && polygon_list->get_first_point()==*list1->get_ptr_point(counter1)){
					break_total=true;
					break;
				}
				//set the point to the polygon list
				polygon_list->set_new_point(list1->get_ptr_point(counter1));
				list1->get_ptr_point(counter1)->set_is_used_flag(true);
				counter1++;
				if(counter1>list1->get_number_points()-1){
					counter1=0;
				}
				
				//check for a changing to list 2
				if(list1->get_ptr_point(counter1)->get_interception_point_flag()==true && 
					list1->get_ptr_point(counter1)->get_inside_flag()==false){
						break1=true;
						counter2=list1->get_ptr_point(counter1)->get_index_corresponding_point();
						if(counter2<0){
							ostringstream cout;
							list1->output_members(&cout);
							list2->output_members(&cout);

							Sys_Common_Output::output_fpl->output_txt(&cout);
						}
						list1->get_ptr_point(counter1)->set_is_used_flag(true);
				}
			}while(break1==false);
			break1=false;

			//poly2
			if(break_total==false){
				do{
					changed_flag=true;
					//check for total break; the new polygon is closed
					if(polygon_list->get_number_points()>0 && polygon_list->get_first_point()==*list2->get_ptr_point(counter2)){
						break_total=true;
						break;
					}
					//set the point to the polygon list
					polygon_list->set_new_point(list2->get_ptr_point(counter2));
					list2->get_ptr_point(counter2)->set_is_used_flag(true);
					counter2++;
					if(counter2>list2->get_number_points()-1){
						counter2=0;
					}
					
					//check for a changing to list 1
					if(list2->get_ptr_point(counter2)->get_interception_point_flag()==true && 
						list2->get_ptr_point(counter2)->get_inside_flag()==false){
							break2=true;
							counter1=list2->get_ptr_point(counter2)->get_index_corresponding_point();
							list2->get_ptr_point(counter2)->set_is_used_flag(true);
					}
				}while(break2==false);
				break2=false;
			}
			counter_sec++;
			if(counter_sec==2000){
				break;
			}
		}while(break_total==false);



		break_total=false;
		//there was no change=> no common area
		if(changed_flag==false){
			area=area+0.0;
		}
		else{
			//output of the new polygon list for developement
			/*if(flag==false){
				cout <<" New polygon"<<endl;
				polygon_list.output_members(&info);
				cout << info.str()<<endl;
				info.str("");
			}*/

			area=area+polygon_list->calculate_area();
		}
		changed_flag=false;

		//look for a new start index

		for(int i=start_index+1; i< list1->get_number_points(); i++){
			if(list1->get_ptr_point(i)->get_interception_point_flag()==true &&
				list1->get_ptr_point(i)->get_inside_flag()==true && list1->get_ptr_point(i)->get_is_used_flag()==false){
				counter1=i;
				new_start_found=true;
				start_index=counter1;
				polygon_list->reset_list();
				break;	
			}
		}

	}
	while(new_start_found==true);


	return area;




}
//Calculate the length of segment, which is in a polygon
double _Geo_Polygon::segment_interception(Geo_Segment *seg, Geo_Interception_Point_List *list){

	double length=0.0;

	Geo_Polygon_Interception_Point_List poly;
	Geo_Polygon_Interception_Point buffer;
	list->delete_list();

	//set polygon points
	buffer.set_point(&seg->point1);
	buffer.set_polygon_point_flag(true);
	if(this->check_point_inside(&seg->point1)==true){
		buffer.set_inside_flag(true);
	}
	else{
		buffer.set_inside_flag(false);
	}
	poly.set_new_point(&buffer);
	this->calculate_segment_interception(list, seg);
	list->sort_distance();

	for(int j=0; j< list->get_number_points(); j++){
		if(list->get_point(j).line_interception.interception_flag==true){
			if(list->get_point(j).line_interception.indefinite_flag==false){
				buffer.reset_flags();
				buffer.set_point(list->get_point(j).line_interception.interception_point);
				buffer.set_interception_point_flag(true);
				poly.set_new_point(&buffer);
			}
		}
	}

	buffer.set_point(&seg->point2);
	buffer.set_polygon_point_flag(true);
	if(this->check_point_inside(&seg->point2)==true){
		buffer.set_inside_flag(true);
	}
	else{
		buffer.set_inside_flag(false);
	}
	poly.set_new_point(&buffer);

	if(poly.get_number_points()==2){
		if(poly.get_number_inside_points()==2){
			return seg->get_distance();
		}
		else{
			Geo_Point buff_point=seg->calc_mid_point();
			if(this->check_point_inside(&buff_point)==true || this->check_point_atboundary(&buff_point)==true){
				return seg->get_distance();
			}
			else{
				return length=0.0;
			}
		}
	}

	poly.set_interception_point_direction(this);

	/*if(poly.get_number_points()>=2 && poly.get_last_point().get_inside_flag()==true){
		ostringstream info;
		poly.output_members(&info);
		cout << info.str()<<endl;

		poly.get_ptr_point(poly.get_number_points()-2)->set_inside_flag(true);
	}*/


	int index=0;
	for(int i=0; i< poly.get_number_points(); i++){
		index=i;
		if(poly.get_ptr_point(i)->get_inside_flag()==true){
			if((i+1)<poly.get_number_points()){
				length=length+poly.get_ptr_point(i)->distance(poly.get_ptr_next_point(&index));
			}
			else{
				break;
			}
		}
	}

	return length;
}
//(static) Convert the enum _polygon_intercept into string
string _Geo_Polygon::convert_ploygon_intercept2txt(_polygon_intercept given_enum){
	string txt;

	switch(given_enum){
		case _polygon_intercept::complete_inside:
			txt=sys_label::complete_in;
			break;
		case _polygon_intercept::complete_outside:
			txt=sys_label::complete_out;
			break;
		case  _polygon_intercept::partly_intercepted:
			txt=sys_label::partly_intercept;
			break;
		case  _polygon_intercept::outside_with_boundary:
			txt=sys_label::out_with_boundary;
			break;
		case  _polygon_intercept::inside_with_boundary:
			txt=sys_label::in_with_boundary;
			break;
		default:
			txt=label::unknown_type;
			break;
	}

	return txt;

}
//Get a point wich is inside a polgon
bool _Geo_Polygon::get_point_inside(Geo_Point *point_inside){
	bool found_flag=false;
	//three point polygon
	if(this->number_segments==2){
		double x_center=0.0;
		double y_center=0.0;
		for(int i=0; i<3; i++){
			x_center=x_center+this->segments[i].point1.get_xcoordinate();
			y_center=y_center+this->segments[i].point1.get_ycoordinate();
		}
		x_center=x_center/3.0;
		y_center=y_center/3.0;
		point_inside->set_point_coordinate(x_center, y_center);
		return found_flag=true;
	}
	//more than three points
	else{
		Geo_Segment seg;
		int counter=1;
		for(int j=0; j< this->number_segments-1 ; j++){
			for(int i=counter; i<this->number_segments-1; i++){
				seg.set_points(&(this->segments[j].point1),&(this->segments[i].point2));
				*point_inside=seg.calc_mid_point();
				if(this->check_point_inside(point_inside)==true){
					return found_flag=true;
				}
			}
			counter++;
		}
		return found_flag;
	}	
}
//Get a string for inserting apolygon data to a sql-database
string _Geo_Polygon::get_polygon2sql_string(void){
	ostringstream buff;
	buff << FORMAT_FIXED_REAL << P(3);
	buff << " ST_GEOMFROMTEXT('POLYGON((";
	for(int i=0; i< this->get_number_points(); i++){
		buff<<this->get_point(i)->get_xcoordinate() << "  ";
		buff<<this->get_point(i)->get_ycoordinate()<< ", ";
	}
	//set the first point again
	buff<<this->get_point(0)->get_xcoordinate() << "  ";
	buff<<this->get_point(0)->get_ycoordinate()<< " ";
	buff << "))') ";


	return buff.str();
}
//Clear polygon, the segments of the polygon are deleted
void _Geo_Polygon::clear_polygon(void){

	this->delete_segments();
	this->number_segments=0;

}
//Output point members to display/console
void _Geo_Polygon::output_point_members(ostringstream *cout){
	for(int i=0; i< this->number_segments; i++){
		this->segments[i].output_point_members(cout);
	}
}
//________________
//protected
//Allocate the segments
void _Geo_Polygon::allocate_segments(void){
	this->delete_segments();
	try{
		this->segments = new Geo_Segment[this->number_segments];
	}
	catch(bad_alloc &){
		Error msg=this->set_error(1);
		throw msg;
	}
	this->segment_alloc_here=true;
}
//Delete the segments
void _Geo_Polygon::delete_segments(void){
	if(this->segment_alloc_here==true){
		if(this->segments!=NULL){
			delete [] this->segments;
			this->segments=NULL;
			this->segment_alloc_here=false;
		}
	}
	else{
		this->segments=NULL;
	}
}
//Check the points
void _Geo_Polygon::check_points(Geo_Point *points){
	for(int i=0; i< this->number_segments-1;i++){
		if(points[i]==points[i+1]){
			Error msg=this->set_error(2);
			ostringstream info;
			info<< "Pointnumbers: " << i << " to " << i+1 << endl;
			info<< "x           : " << FORMAT_FIXED_REAL << P(2) << points[i].get_xcoordinate() << endl;
			info<< "y           : " << points[i].get_ycoordinate() << endl;
			msg.make_second_info(info.str());
			throw msg;

		}
	}
	//check the last points
	if(points[this->number_segments-1]==points[0]){
			Error msg=this->set_error(2);
			ostringstream info;
			info<< "Last point to the first point" << endl;
			info<< "x           : " << FORMAT_FIXED_REAL << P(2) << points[0].get_xcoordinate() << endl;
			info<< "y           : " << points[0].get_ycoordinate() << endl;
			msg.make_second_info(info.str());
			throw msg;
	}

}
//_____________________________
//private
//Check an intersection with another polygon (just true or false)
bool _Geo_Polygon::check_interception(_Geo_Polygon *check_polygon){
	_geo_interception_point intercept_point;
	intercept_point.interception_point.set_point_name(label::interception_point);

	
	for (int i=0; i<check_polygon->get_number_segments();i++){
		for (int j=0; j<this->get_number_segments();j++){
			this->segments[j].calc_interception(&(check_polygon->segments[i]),&intercept_point);
			if(intercept_point.interception_flag==true){
				if(intercept_point.interception_point!=this->segments[j].point1 && intercept_point.interception_point!=this->segments[j].point2
					&& intercept_point.interception_point!=check_polygon->segments[i].point1 && intercept_point.interception_point!=check_polygon->segments[i].point2){
						return true;
				}
			}

		}
	}


	return false;
}
//set the error
Error _Geo_Polygon::set_error(const int err_type){
	string place="_Geo_Polygon::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 1://bad alloc
			place.append("allocate_segments(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://same points is not possible
			place.append("check_points(Geo_Point *points)");
			reason="Same sequentiell points are given for the polygon";
			help="Check the points for the polygon";
			type=11;
			break;
		case 3://do not find a ray which do not intersect a point of the polygon
			place.append("check_point_inside(Geo_Point *check_point)");
			reason="Do not find a ray which do not intersect a point";
			help="Check the polygon";
			type=11;
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
