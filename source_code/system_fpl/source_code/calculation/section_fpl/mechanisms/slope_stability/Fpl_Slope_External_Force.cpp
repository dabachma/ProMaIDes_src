#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Slope_External_Force.h"


//Default constructor
Fpl_Slope_External_Force::Fpl_Slope_External_Force(void){

	this->lever_arm=NULL;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Slope_External_Force), _sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Slope_External_Force::~Fpl_Slope_External_Force(void){

	this->delete_lever_arm();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Slope_External_Force), _sys_system_modules::FPL_SYS);//count the memory

}
//___________
//public
//Set the orthogonal distance to the mid point of the slip cricle (lever arm) for each segment
void Fpl_Slope_External_Force::set_lever_arm(Geo_Point *mid_point, const bool landside){

	this->allocate_lever_arm();


	Geo_Straight_Line line_1;
	Geo_Straight_Line line_2;


	if(landside==false){
		//waterside => take the first point
		for(int i=0; i< this->number_segments; i++){

			if(this->my_segment[i].get_vertical_flag()==true){
				this->lever_arm[i]=this->my_segment[i].point1.vertical_distance(mid_point);
				if(this->lever_arm[i]!=0.0){
					if(this->my_segment[i].point1.check_above_of(mid_point)==true){
						this->lever_arm[i]=-1.0*this->lever_arm[i];
					}
				}
			}
			else if(this->my_segment[i].get_gradient()==0.0){
				this->lever_arm[i]=this->my_segment[i].point1.horizontal_distance(mid_point);
				if(this->lever_arm[i]!=0.0){
					if(this->my_segment[i].point1.check_right_of(mid_point)==true){
						this->lever_arm[i]=-1.0*this->lever_arm[i];
					}
				}
			}
			else if(this->my_segment[i].get_gradient()>0.0){
				line_1.set_coor_grad(mid_point->get_xcoordinate(), mid_point->get_ycoordinate(), this->my_segment[i].get_gradient());
				line_2.set_coor_grad(this->my_segment[i].point1.get_xcoordinate(), this->my_segment[i].point1.get_ycoordinate(), -1.0/this->my_segment[i].get_gradient());
				
				_geo_interception_point inter_po;
				line_1.calc_interception(&line_2, &inter_po);

				if(inter_po.interception_flag==false || inter_po.indefinite_flag==true){
					Warning msg=this->set_warning(0);
					msg.output_msg(1);
					//reaction
					this->lever_arm[i]=0.0;
				}
				else{
					this->lever_arm[i]=mid_point->distance(&inter_po.interception_point);
					if(this->lever_arm[i]!=0.0){
						if(inter_po.interception_point.check_right_of(mid_point)==true){
							this->lever_arm[i]=-1.0*this->lever_arm[i];
						}
					}
				}
			}
		}
	}
	else{
		//landside=> take the second point
		for(int i=0; i< this->number_segments; i++){

			if(this->my_segment[i].get_vertical_flag()==true){
				this->lever_arm[i]=this->my_segment[i].point2.vertical_distance(mid_point);
				if(this->my_segment[i].point2.check_above_of(mid_point)==true){
					this->lever_arm[i]=-1.0*this->lever_arm[i];
				}

			}
			else if(this->my_segment[i].get_gradient()==0.0){
				this->lever_arm[i]=this->my_segment[i].point2.horizontal_distance(mid_point);
				if(this->lever_arm[i]!=0.0){
					if(this->my_segment[i].point2.check_left_of(mid_point)==true){
						this->lever_arm[i]=-1.0*this->lever_arm[i];
					}
				}
			}
			else if(this->my_segment[i].get_gradient()<0.0){
				line_1.set_coor_grad(mid_point->get_xcoordinate(), mid_point->get_ycoordinate(), this->my_segment[i].get_gradient());
				line_2.set_coor_grad(this->my_segment[i].point2.get_xcoordinate(), this->my_segment[i].point2.get_ycoordinate(), -1.0/this->my_segment[i].get_gradient());
				
				_geo_interception_point inter_po;
				line_1.calc_interception(&line_2, &inter_po);

				if(inter_po.interception_flag==false || inter_po.indefinite_flag==true){
					Warning msg=this->set_warning(0);
					msg.output_msg(1);
					//reaction
					this->lever_arm[i]=0.0;
				}
				else{
					this->lever_arm[i]=mid_point->distance(&inter_po.interception_point);
					if(this->lever_arm[i]!=0.0){
						if(inter_po.interception_point.check_left_of(mid_point)==true){
							this->lever_arm[i]=-1.0*this->lever_arm[i];
						}
					}
				}
			}

		}
	}
}
//Calculate external water forces to the slip circle
double Fpl_Slope_External_Force::calc_external_water_forces(const double water_level, const double water_density){
	double moment=0.0;
	double hebel=0.0;
	double sum=0.0;
	double a=0.0;
	double b=0.0;

	for(int i=0; i< this->get_number_segments(); i++){

		a=water_level-this->get_segment(i)->point1.get_ycoordinate();
		b=water_level-this->get_segment(i)->point2.get_ycoordinate();

		if(a>0.0 && b>0.0){
			a=a*water_density*constant::Cgg;
			b=b*water_density*constant::Cgg;
			moment=this->get_segment(i)->get_distance()*0.5*(a+b);
			hebel=this->lever_arm[i]-this->get_segment(i)->get_distance()*(a+2.0*b)/(3.0*(a+b));

		}
		else if(a>0.0 && b<0.0){
			double len=0.0;
			len=pow(pow(a,2.0)+pow(a/this->get_segment(i)->get_gradient(),2.0),0.5);
			moment=len*a*water_density*constant::Cgg*0.5;
			hebel=this->lever_arm[i]-1.0/3.0*len;
		}
		else if(a<0.0 && b>0.0){
			double len=0.0;
			len=pow(pow(b,2.0)+pow(b/this->get_segment(i)->get_gradient(),2.0),0.5);
			moment=len*b*water_density*constant::Cgg*0.5;
			hebel=this->lever_arm[i]-1.0/3.0*len;
		}
		sum=sum+moment*hebel;
	}

	return sum;


}
//_________
//private
//Allocate the orthogonal distance to the mid point of the slip cricle (lever arm) for each segment
void Fpl_Slope_External_Force::allocate_lever_arm(void){
	this->delete_lever_arm();
	try{
		this->lever_arm=new double[this->number_segments];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//init
	for(int i=0; i< this->number_segments; i++){
		this->lever_arm[i]=0.0;
	}
}
//Delete the orthogonal distance to the mid point of the slip cricle (lever arm) for each segment
void Fpl_Slope_External_Force::delete_lever_arm(void){
	if(this->lever_arm!=NULL){
		delete []this->lever_arm;
		this->lever_arm=NULL;
	}
}
//Set warning(s)
Warning Fpl_Slope_External_Force::set_warning(const int warn_type){
	string place="Fpl_Slope_External_Force::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://slice width is greater than circle width
			place.append("set_lever_arm(Geo_Point *mid_point, const bool landside)");
			reason="No interception is found for the lever arm calculation";
			reaction="The lever arm is set to 0.0 for this segment";
			type=26;
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
//set the error
Error Fpl_Slope_External_Force::set_error(const int err_type){
		string place="Fpl_Slope_External_Force::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("allocate_lever_arm(void)");
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
