//#include "Geo_Segment.h"
#include "Sys_Headers_Precompiled.h"

//Default constructor
Geo_Segment::Geo_Segment(void){
	this->horizontal_dist=0.0;
	this->vertical_dist=0.0;
	this->distance=0.0;
	//common
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Segment)-(sizeof(Geo_Straight_Line)), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//Copy constructor
Geo_Segment::Geo_Segment(const Geo_Segment& object){
	Geo_Straight_Line::operator =(object);
	this->distance=object.distance;
	this->vertical_dist=object.vertical_dist;
	this->horizontal_dist=object.horizontal_dist;
	//common
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Segment)-(sizeof(Geo_Straight_Line)), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//Constructor where the two points are given (start- and end-point)
Geo_Segment::Geo_Segment(Geo_Point *point1, Geo_Point *point2){
	this->point1=*point1;
	this->point2=*point2;
	this->vertical_flag=false;
	//set the gradient
	this->calc_gradient();
	//set the y_interception
	this->calc_y_interception();
	//set the name of the first point
	this->name=*(this->point1.get_ptr_point_name());

	//make the claculation
	this->calc_horizontal_dist();
	this->calc_vertical_dist();
	this->calc_distance();
	this->calc_gradient();
	this->calc_y_interception();

	//common
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Segment)-(sizeof(Geo_Straight_Line)), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//Default destructor
Geo_Segment::~Geo_Segment(void){
	//common
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Segment)-(sizeof(Geo_Straight_Line)), _sys_system_modules::GEOSYS_SYS);//count the memory
}

//_______________
//public
//Set the segment with two given points (start- and end-point)
void Geo_Segment::set_points(Geo_Point *point1, Geo_Point *point2){
	this->point1=*point1;
	this->point2=*point2;
	//make the claculation
	this->calc_horizontal_dist();
	this->calc_vertical_dist();
	this->calc_distance();
	this->calc_gradient();
	this->calc_y_interception();
	//set the name of the first point
	this->name=*(this->point2.get_ptr_point_name());

}
//Set the segment with the given coordinates of the start- and end-point
void Geo_Segment::set_coordinates(const double x1, const double y1, const double x2, const double y2){
	Geo_Straight_Line::set_coordinates(x1, y1, x2, y2);
	//make the claculation
	this->calc_horizontal_dist();
	this->calc_vertical_dist();
	this->calc_distance();
	

}
//Set the segement with a Geo_Raster_Segement
void Geo_Segment::set_segment(Geo_Raster_Segment *rast_seg){
	this->set_points((rast_seg->point1), (rast_seg->point2));
}
//Get the horizontal distance of the segment
double Geo_Segment::get_horizontal_dist(void){
	return this->horizontal_dist;
}
//Get the vertical distance of the segment
double Geo_Segment::get_vertical_dist(void){
	return this->vertical_dist;
}
//Get the total distance of the segment
double Geo_Segment::get_distance(void){
	return this->distance;
}

//Get the distance along a segment from the beginning to a given point
double Geo_Segment::get_distance_along_segment(Geo_Point * check_point){

	double length=-1.0;
	if(this->calc_point_on_segment(check_point)==true){
		//calculate the distance
		length=this->point1.distance(check_point);
	}
	return length;
}
//Refresh the distances 
void Geo_Segment::refresh_distances(void){
	//make the claculation
	this->calc_horizontal_dist();
	this->calc_vertical_dist();
	this->calc_distance();
	this->calc_gradient();
	this->calc_y_interception();
}
//Calculate if a given interception point is on the segment
bool Geo_Segment::calc_interception_point_on_segment(Geo_Point *check_point){
	bool online_flag=false;
	if(*check_point==this->point1 || *check_point==this->point2){
		return true;
	}
	//check if interception point is on the segment
	if(this->vertical_flag==false){//not vertical
		 if(this->point1.get_xcoordinate()<this->point2.get_xcoordinate()){//find min x of the to segment points
			if(check_point->get_xcoordinate()>=this->point1.get_xcoordinate() && check_point->get_xcoordinate()<=this->point2.get_xcoordinate()){
				online_flag=true;//check point x is between the to x coordinates
			}
			else{
				online_flag=false;
			}
		}
		else{
			if(check_point->get_xcoordinate()<=this->point1.get_xcoordinate() && check_point->get_xcoordinate()>=this->point2.get_xcoordinate()){
				online_flag=true;//check point x is between the to x coordinates (other direction)
			}
			else{
				online_flag=false;
			}
		}
	}
	else{//vertical
		if(this->point1.get_ycoordinate()<this->point2.get_ycoordinate()){
			if(check_point->get_ycoordinate()>=this->point1.get_ycoordinate() && check_point->get_ycoordinate()<=this->point2.get_ycoordinate()){
				online_flag=true;
			}
			else{
				online_flag=false;
			}
		}
		else{
			if(check_point->get_ycoordinate()<=this->point1.get_ycoordinate() && check_point->get_ycoordinate()>=this->point2.get_ycoordinate()){
				online_flag=true;
			}
			else{
				online_flag=false;
			}
		}
	}

	return online_flag;
}
//Calculate if a given point is on the segment
bool Geo_Segment::calc_point_on_segment(Geo_Point *check_point){
	bool online_flag=false;

	if(this->calc_point_on_line(check_point)==false){
		return false;
	}
	online_flag=this->calc_interception_point_on_segment(check_point);

	return online_flag;
}
//Calculate and retrieve an interception point with a given straight line
void Geo_Segment::calc_interception(Geo_Straight_Line *interception_line, _geo_interception_point *my_interception){
	my_interception->name_1=this->name;
	my_interception->name_2=*(interception_line->get_ptr_name());
	my_interception->indefinite_flag=false;
	my_interception->interception_flag=false;
	
	//exception: check vertical line
	if(this->vertical_flag==true && interception_line->get_vertical_flag()==false){//check if it is vertic
		double x_value=this->point1.get_xcoordinate();
		double y_value=interception_line->get_gradient()*x_value+interception_line->get_y_interception();
		my_interception->interception_flag=true;
		my_interception->indefinite_flag=false;
		my_interception->interception_point.set_point_coordinate(x_value, y_value);
		//check if interception point is on the segment
		if(this->calc_interception_point_on_segment(&my_interception->interception_point)==true){//is on segment
			return;
		}
		else{//not on segment
			my_interception->interception_flag=false;
			my_interception->indefinite_flag=false;
			return;
		}
	}
	else if(this->vertical_flag==false && interception_line->get_vertical_flag()==true){
		double x_value=interception_line->point1.get_xcoordinate();
		double y_value=this->gradient*x_value+this->y_interception;
		my_interception->interception_flag=true;
		my_interception->indefinite_flag=false;
		my_interception->interception_point.set_point_coordinate(x_value, y_value);
		//check if interception point is on the segment and on line
		if(this->calc_interception_point_on_segment(&my_interception->interception_point)==true){//is on segment
			return;
		}
		else{//not on segment
			my_interception->interception_flag=false;
			my_interception->indefinite_flag=false;
			return;
		}
	}
	else if(this->vertical_flag==true && interception_line->get_vertical_flag()==true){
		my_interception->interception_flag=false;
		if(this->point1.get_xcoordinate()==interception_line->point1.get_xcoordinate()){
			my_interception->indefinite_flag=true;	
		}
		else{
			my_interception->indefinite_flag=false;
		}
		return;
	}

	//calculate the interception point (no vertical segment)
	double delta_m=this->get_gradient()-interception_line->get_gradient();
	double delta_b=interception_line->get_y_interception()-this->get_y_interception();
	if(abs(delta_m)<=constant::diff_grad_epsilon){//no one point interception
		if(abs(delta_b)<=constant::meter_epsilon){//indefinite interception points
			my_interception->indefinite_flag=true;
			my_interception->interception_flag=false;
		}
		else{
			my_interception->indefinite_flag=false;
			my_interception->interception_flag=false;
		}
		return;
	}
	else{
		my_interception->indefinite_flag=false;
		my_interception->interception_flag=true;
		double x_value=delta_b/delta_m;
		double y_value=this->get_gradient()*x_value+this->get_y_interception();
		my_interception->interception_point.set_point_coordinate(x_value, y_value);
		//check if interception point is on the segment
		if(this->calc_interception_point_on_segment(&my_interception->interception_point)==true){//is on segment
			return;
		}
		else{//not on segment
			my_interception->interception_flag=false;
			my_interception->indefinite_flag=false;
			return;
		}
	}
}
//Calculate and retrieve an interception point with a given segment
void Geo_Segment::calc_interception(Geo_Segment *interception_segment,  _geo_interception_point *my_interception){
	my_interception->name_1=this->name;
	my_interception->name_2=*(interception_segment->get_ptr_name());
	my_interception->indefinite_flag=false;
	my_interception->interception_flag=false;
	
	//exception: check vertical line
	if(this->vertical_flag==true && interception_segment->get_vertical_flag()==false){//check if it is vertical
		double x_value=this->point1.get_xcoordinate();
		double y_value=interception_segment->get_gradient()*x_value+interception_segment->get_y_interception();
		my_interception->interception_flag=true;
		my_interception->indefinite_flag=false;
		my_interception->interception_point.set_point_coordinate(x_value, y_value);
		//check if interception point is on the segment
		if(this->calc_interception_point_on_segment(&my_interception->interception_point)==true && interception_segment->calc_interception_point_on_segment(&my_interception->interception_point)==true){//is on segment both segments
			return;
		}
		else{//not on segment
			my_interception->interception_flag=false;
			my_interception->indefinite_flag=false;
			return;
		}
	}
	else if(this->vertical_flag==false && interception_segment->get_vertical_flag()==true){
		double x_value=interception_segment->point1.get_xcoordinate();
		double y_value=this->gradient*x_value+this->y_interception;
		my_interception->interception_flag=true;
		my_interception->indefinite_flag=false;
		my_interception->interception_point.set_point_coordinate(x_value, y_value);
		//check if interception point is on the segment
		if(this->calc_interception_point_on_segment(&my_interception->interception_point)==true&& interception_segment->calc_interception_point_on_segment(&my_interception->interception_point)==true){//is on both segments
			return;
		}
		else{//not on segment
			my_interception->interception_flag=false;
			my_interception->indefinite_flag=false;
			return;
		}
	}
	else if(this->vertical_flag==true && interception_segment->get_vertical_flag()==true){
		my_interception->interception_flag=false;
		if(this->calc_interception_point_on_segment(&interception_segment->point1)==true || this->calc_interception_point_on_segment(&interception_segment->point2)==true 
				|| interception_segment->calc_interception_point_on_segment(&this->point1)==true || interception_segment->calc_interception_point_on_segment(&this->point2)==true){
			if((this->point1==interception_segment->point1 && this->calc_point_on_segment(&interception_segment->point2)==false) 
				|| (this->point1==interception_segment->point2 && this->calc_point_on_segment(&interception_segment->point1)==false)){
				my_interception->indefinite_flag=false;
				my_interception->interception_flag=true;
				my_interception->interception_point=this->point1;
			}
			else if((this->point2==interception_segment->point1 && this->calc_point_on_segment(&interception_segment->point2)==false)
				|| (this->point2==interception_segment->point2 && this->calc_point_on_segment(&interception_segment->point1)==false)){
				my_interception->indefinite_flag=false;
				my_interception->interception_flag=true;
				my_interception->interception_point=this->point2;
			}
			else{
				my_interception->indefinite_flag=true;
				my_interception->interception_flag=false;
			}
		}
		else{
			my_interception->indefinite_flag=false;
			my_interception->interception_flag=false;
		}
		return;
	}

	//calculate the interception point (no vertical segment)
	double delta_m=this->get_gradient()-interception_segment->get_gradient();
	double delta_b=interception_segment->get_y_interception()-this->get_y_interception();
	if(abs(delta_m)<=constant::diff_grad_epsilon){//no one point interception
		if(abs(delta_b)<=constant::meter_epsilon){//indefinite interception points
			if(this->calc_interception_point_on_segment(&interception_segment->point1)==true || this->calc_interception_point_on_segment(&interception_segment->point2)==true 
				|| interception_segment->calc_interception_point_on_segment(&this->point1)==true || interception_segment->calc_interception_point_on_segment(&this->point2)==true){
				if((this->point1==interception_segment->point1 && this->calc_interception_point_on_segment(&interception_segment->point2)==false) 
					|| (this->point1==interception_segment->point2 && this->calc_interception_point_on_segment(&interception_segment->point1)==false)){
					my_interception->indefinite_flag=false;
					my_interception->interception_flag=true;
					my_interception->interception_point=this->point1;
				}
				else if((this->point2==interception_segment->point1 && this->calc_interception_point_on_segment(&interception_segment->point2)==false)
					|| (this->point2==interception_segment->point2 && this->calc_interception_point_on_segment(&interception_segment->point1)==false)){
					my_interception->indefinite_flag=false;
					my_interception->interception_flag=true;
					my_interception->interception_point=this->point2;
				}
				else{
					my_interception->indefinite_flag=true;
					my_interception->interception_flag=false;
				}
			}
			else{
				my_interception->indefinite_flag=false;
				my_interception->interception_flag=false;
			}
		}
		else{
			my_interception->indefinite_flag=false;
			my_interception->interception_flag=false;
		}
		return;
	}
	else{
		my_interception->indefinite_flag=false;
		my_interception->interception_flag=true;
		double x_value=delta_b/delta_m;
		double y_value=this->get_gradient()*x_value+this->get_y_interception();
		my_interception->interception_point.set_point_coordinate(x_value, y_value);
		//check if interception point is on the segment
		if(this->calc_interception_point_on_segment(&my_interception->interception_point)==true && interception_segment->calc_interception_point_on_segment(&my_interception->interception_point)==true){//is on both segment
			return;
		}
		else{//not on segment
			my_interception->interception_flag=false;
			my_interception->indefinite_flag=false;
			return;
		}
	}
}
//Get the mid point of the segment
Geo_Point Geo_Segment::calc_mid_point(void){
	Geo_Point mid_point;
	double x_mid=0.0;
	double y_mid=0.0;
	x_mid=(this->point1.get_xcoordinate()+this->point2.get_xcoordinate())/2.0;
	y_mid=(this->point1.get_ycoordinate()+this->point2.get_ycoordinate())/2.0;
	mid_point.set_point(x_mid,y_mid,label::elem_mid);
	return mid_point;
}
//Calculate the interception with a square-root function (c1-(x-c3)*c2)^0.5+c4
void Geo_Segment::calc_interception_square_root(Geo_Point_List *list, const double c1, const double c2, const double c3, const double c4){


	Geo_Point_List buffer;
	Geo_Straight_Line::calc_interception_square_root(&buffer, c1, c2, c3, c4);
	list->delete_list();

	for(int i=0; i< buffer.get_number_points(); i++){
		if(this->calc_interception_point_on_segment(buffer.get_ptr_point(i))==true){
			list->set_new_point(buffer.get_ptr_point(i));
		}
	}
}
//Move the segment for a given distance in x-direction (positiv distance is to the right)
void Geo_Segment::move_to_x_direction(const double distance){
	Geo_Straight_Line::move_to_x_direction(distance);
	this->refresh_distances();
}
//Move the segment for a given distance in y-direction (positiv distance is upwards)
void Geo_Segment::move_to_y_direction(const double distance){
	Geo_Straight_Line::move_to_y_direction(distance);
	this->refresh_distances();

}
//Get a list of additional points on the segement specified by a given number
void Geo_Segment::get_additional_points_on_segment(Geo_Point_List *list, const int number){
	if(number<=0){
		return;
	}
	
	double dist=0.0;
	double dist_tot=0.0;
	dist=this->distance/(double)(number+1);
	Geo_Point insert;

	if(this->get_vertical_flag()==false){
		double c2=(pow(this->get_gradient(),2.0)+1.0);
		double c1=(this->get_gradient()*(this->get_y_interception()-this->point1.get_ycoordinate())-this->point1.get_xcoordinate())/c2;
		double c3=(pow(this->point1.get_xcoordinate(),2.0)+pow(this->get_y_interception()-this->point1.get_ycoordinate(),2.0))/c2;
		double x_new=0.0;
		double y_new=0.0;

		double dir=1.0;
		if(this->point1.check_left_of(&this->point2)==false){
			dir=-1.0;
		}

		for(int i=0;i<number;i++){
			dist_tot=dist*(i+1);
			double test2=pow(dist_tot,2.0)/c2-c3+pow(c1,2.0);
			if(test2>=0.0-constant::meter_epsilon && test2<0.0){
				test2=0.0;
			}
			else if(test2<0.0-constant::meter_epsilon){
				continue;


			}
			x_new=-c1+dir*(pow(test2,0.5));
			y_new=this->get_gradient()*x_new+this->get_y_interception();
			insert.set_point_coordinate(x_new, y_new);
			list->set_new_point(&insert);

		/*	double check_dist=this->point1.distance(&insert);
			bool check_on=false;
			check_on=this->calc_point_on_segment(&insert);
*/

		}
	}
	//vertical
	else{
		double dir=1.0;
		double x_new=0.0;
		double y_new=0.0;
		if(this->point1.check_below_of(&this->point2)==false){
			dir=-1.0;
		}
		for(int i=0;i<number;i++){
			dist_tot=dist*(i+1);
			x_new=this->point1.get_xcoordinate();
			y_new=this->point1.get_ycoordinate()+dir*dist_tot;
			insert.set_point_coordinate(x_new, y_new);
			list->set_new_point(&insert);

			//double check_dist=this->point1.distance(&insert);
			//bool check_on=this->calc_point_on_segment(&insert);

		}
	}

}
//Get a list of additional points on the segement specified by a given distance
void Geo_Segment::get_additional_points_on_segment(Geo_Point_List *list, const double interval){
	if(distance<=0){
		return;
	}
	
	double dist=0.0;
	double dist_tot=0.0;
	dist=interval;
	int number=0;
	number=(int)(this->distance/interval);
	if(number<=0){
		return;
	}

	Geo_Point insert;

	if(this->get_vertical_flag()==false){
		double c2=(pow(this->get_gradient(),2.0)+1.0);
		double c1=(this->get_gradient()*(this->get_y_interception()-this->point1.get_ycoordinate())-this->point1.get_xcoordinate())/c2;
		double c3=(pow(this->point1.get_xcoordinate(),2.0)+pow(this->get_y_interception()-this->point1.get_ycoordinate(),2.0))/c2;
		double x_new=0.0;
		double y_new=0.0;

		double dir=1.0;
		if(this->point1.check_left_of(&this->point2)==false){
			dir=-1.0;
		}

		for(int i=0;i<number;i++){
			dist_tot=dist*(i+1);
			double test2=pow(dist_tot,2.0)/c2-c3+pow(c1,2.0);
			if(test2>=0.0-constant::meter_epsilon && test2<0.0){
				test2=0.0;
			}
			else if(test2<0.0-constant::meter_epsilon){
				continue;


			}
			x_new=-c1+dir*(pow(test2,0.5));
			y_new=this->get_gradient()*x_new+this->get_y_interception();
			
			insert.set_point_coordinate(x_new, y_new);
			list->set_new_point(&insert);

		/*	double check_dist=this->point1.distance(&insert);
			bool check_on=false;
			check_on=this->calc_point_on_segment(&insert);
*/

		}
	}
	//vertical
	else{
		double dir=1.0;
		double x_new=0.0;
		double y_new=0.0;
		if(this->point1.check_below_of(&this->point2)==false){
			dir=-1.0;
		}
		for(int i=0;i<number;i++){
			dist_tot=dist*(i+1);
			x_new=this->point1.get_xcoordinate();
			y_new=this->point1.get_ycoordinate()+dir*dist_tot;
			insert.set_point_coordinate(x_new, y_new);
			list->set_new_point(&insert);

			//double check_dist=this->point1.distance(&insert);
			//bool check_on=this->calc_point_on_segment(&insert);

		}
	}




}
//Switch the points (point1->point2 v.v.)
void Geo_Segment::switch_points(void){
	Geo_Point buffer=this->point1;
	this->set_points(&this->point2,&buffer);
}
//Output members to display/console
void Geo_Segment::output_members(ostringstream *cout){
	*cout <<" GEO_SEGMENT "<<this->point1.get_point_name()<<endl;
	if(this->vertical_flag==false){
		*cout <<"  Gradient            : " << this->gradient << label::no_unit<<endl;
	}
	else{
		*cout <<"  vertical segment" << endl;
	}
	*cout <<"  Horizontal distance : " << this->horizontal_dist << label::m << endl;
	*cout <<"  Vertical distance   : " << this->vertical_dist << label::m << endl;
	*cout <<"  Distance total      : " << this->distance << label::m << endl;
}
//Output point members to display/console
void Geo_Segment::output_point_members(ostringstream *cout){
	this->point1.output_coor_members(cout);
	this->point2.output_coor_members(cout);
}
//Copy operator
Geo_Segment& Geo_Segment::operator=(const Geo_Segment& object){
	Geo_Straight_Line::operator =(object);
	this->distance=object.distance;
	this->vertical_dist=object.vertical_dist;
	this->horizontal_dist=object.horizontal_dist;
	return *this;
}
//Compare operator ==
bool Geo_Segment::operator==(const Geo_Segment& object){
	bool flag=false;
	//if both points are equal -> segments are equal
	if(this->point1==object.point1  && this->point2==object.point2){
		flag=true;
	}
	if(this->point1==object.point2  && this->point2==object.point1){
		flag=true;
	}
	return flag;
}
//Compare operator !=
bool Geo_Segment::operator!=(const Geo_Segment& object){
	bool flag=false;
	if((*this==object)!=true){
		flag=true;
	}
	return flag;
}
//______________________
//protected
//Calculate the horizontal distance of the segment
void Geo_Segment::calc_horizontal_dist(void){
	this->horizontal_dist=abs(this->point1.horizontal_distance(&this->point2));
}
//Calculate the vertical distance of the segment
void Geo_Segment::calc_vertical_dist(void){
	this->vertical_dist=abs(this->point1.vertical_distance(&this->point2));
}
//Calculate the total distance of the segment
void Geo_Segment::calc_distance(void){
	this->distance=abs(this->point1.distance(&this->point2));
}
