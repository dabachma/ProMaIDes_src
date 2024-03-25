#include "Dam_Headers_Precompiled.h"
//#include "_Dam_2value_Function.h"

//Default constructor
_Dam_2value_Function::_Dam_2value_Function(void){


	this->number_points=0;
	this->function_points=NULL;
	this->stepwise_flag=false;
}
///Default destructor
_Dam_2value_Function::~_Dam_2value_Function(void){
	this->delete_points();
}
//________
//public
//Get the y-value by a x-value
double _Dam_2value_Function::get_y_value(const double x_value){
	if(this->stepwise_flag==true){
		return this->get_discret_y_value(x_value);
	}
	else{
		return this->get_conti_x_value(x_value);
	}
}
//Output the members of the function to display/console
void _Dam_2value_Function::output_member(void){
	ostringstream cout;

	if(this->number_points >0){
		for(int i=0; i< this->number_points; i++){
			cout << W(3)<< i << W(8) << P(2)<< FORMAT_FIXED_REAL<< this->function_points[i].x_value;
			cout << W(14) << this->function_points[i].y_value << endl;		
			Sys_Common_Output::output_dam->output_txt(&cout,true);
		}
	}
	else{
		cout << " No points are set ! " << endl;
		Sys_Common_Output::output_dam->output_txt(&cout,true);
	}
}
//Set the mebers to a plot widget
void _Dam_2value_Function::set_members2plot_wid(Sys_Plot_Wid *widget){

	widget->set_curve_symbols(0, 5, QwtSymbol::Diamond); 
	int no_po=0;
	double *x=NULL;
	double *y=NULL;
	//count points
	if(this->number_points>1){
		if(this->stepwise_flag==true){
			no_po=this->number_points+(this->number_points-1);
		}
		else{
			no_po=this->number_points;
		}
	}
	else{
		no_po=2;
	}

	//allocate 
	try{
		x=new double[no_po]; 
		y=new double[no_po]; 
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(4);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	if(this->number_points>1){
		//set points
		if(this->stepwise_flag==true){
			int counter=0;
			for(int i=0; i<this->number_points;i++){
				x[counter]=this->function_points[i].x_value;
				y[counter]=this->function_points[i].y_value;
				counter++;
				if(i<this->number_points-1){
					x[counter]=this->function_points[i+1].x_value;
					y[counter]=this->function_points[i].y_value;
					counter++;
				}
			}
		}
		else{
			for(int i=0; i<this->number_points;i++){
				x[i]=this->function_points[i].x_value;
				y[i]=this->function_points[i].y_value;
			}
		}
	}
	else{
		if(this->number_points==1){
			x[0]=this->function_points[0].x_value;
			y[0]=this->function_points[0].y_value;
			x[1]=this->function_points[0].x_value+5.0;
			y[1]=this->function_points[0].y_value;
		}
		else{
			x[0]=0.0;
			y[0]=0.0;
			x[1]=0.0+5.0;
			y[1]=0.0;
		}
	}

	//set them to the function
	widget->set_curve_data(0, no_po, x,y);


	//delete 
	if(x!=NULL){
		delete []x;
		x=NULL;
	}
	if(y!=NULL){
		delete []y;
		y=NULL;
	}
}
//___________
//private
//Get a discret y-value by a given x-value (step-function)
double _Dam_2value_Function::get_discret_y_value(const double x_value){
		double result=0.0;

	//is below the first value
	if(x_value <= this->function_points[0].x_value){
		result=this->function_points[0].y_value;
		return result;
	}
	//is above the last value
	if(x_value >= this->function_points[this->number_points-1].x_value){
		result=this->function_points[this->number_points-1].y_value;
		return result;
	}


	int end=0;
	int nodes=0;

	const int fMax[2] = { this->number_points/50, this->number_points/10 };// determination of stepwide for approximate search (two stepwide or one stepwide depending of number of nodes)
	if(fMax[1]<=1){//use just the approaximate search by /10
		end=1;
	}
	else{//use both
		end=2;
	}

	// Search approximate 
	for (int j = end-1; j >= 0; j--){
		if(fMax[j]==0){
			break;
		}
		while(x_value >= this->function_points[nodes].x_value ){
			nodes=nodes+fMax[j];
			if(nodes>=this->number_points-1){
				break;
			}
		}
		nodes=nodes-fMax[j];
	}

	//search exact
	while(x_value >= this->function_points[nodes].x_value){
			nodes=nodes+1;
	}

	//stepwise function scheme
	//y-value
	result=this->function_points[nodes-1].y_value;

	return result;

}
//Get a continuous yvalue by a given x-value (linear interpüolation)
double _Dam_2value_Function::get_conti_x_value(const double x_value){
	double result=0.0;

	//is below the first value
	if(x_value <= this->function_points[0].x_value){
		result=this->function_points[0].y_value;
		return result;
	}
	//is above the last value
	if(x_value >= this->function_points[this->number_points-1].x_value){
		result=this->function_points[this->number_points-1].y_value;
		return result;
	}


	int end=0;
	int nodes=0;

	const int fMax[2] = { this->number_points/50, this->number_points/10 };// determination of stepwide for approximate search (two stepwide or one stepwide depending of number of nodes)
	if(fMax[1]<=1){//use just the approaximate search by /10
		end=1;
	}
	else{//use both
		end=2;
	}

	// Search approximate 
	for (int j = end-1; j >= 0; j--){
		if(fMax[j]==0){
			break;
		}
		while(x_value >= this->function_points[nodes].x_value ){
			nodes=nodes+fMax[j];
			if(nodes>=this->number_points-1){
				break;
			}
		}
		nodes=nodes-fMax[j];
	}

	//search exact
	while(x_value >= this->function_points[nodes].x_value){
			nodes=nodes+1;
	}

	//linear interploation scheme
	//y-value
	result=this->function_points[nodes-1].y_value+ (this->function_points[nodes].y_value-this->function_points[nodes-1].y_value)
		/ (this->function_points[nodes].x_value -this->function_points[nodes-1].x_value)*(x_value-this->function_points[nodes-1].x_value);

	return result;

}
//Allocate the points of the functions 
void _Dam_2value_Function::allocate_points(void){
	try{
		this->function_points=new _dam_function_point[this->number_points];
		//count the memory
		Sys_Memory_Count::self()->add_mem(sizeof(_dam_function_point)*this->number_points, _sys_system_modules::DAM_SYS);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for(int i=0; i< this->number_points; i++){
		this->function_points[i].x_value=0.0;
		this->function_points[i].y_value=0.0;
	}

}
//Delete the points of the functions
void _Dam_2value_Function::delete_points(void){
	if(this->function_points!=NULL){
		delete []this->function_points;
		this->function_points=NULL;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(_dam_function_point)*this->number_points, _sys_system_modules::DAM_SYS);
	}

}
//Check the read in line if there is an !END or !BEGIN
void _Dam_2value_Function::check_line(string line, const int line_counter){
	if(line=="!BEGIN" || line=="!END"){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Error occurs near line: "<< line_counter << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check the points of the function
void _Dam_2value_Function::check_function(void){

	for(int i=0; i< this->number_points;i++){
		if(this->function_points[i].x_value<0.0){
			Error msg=this->set_error(2);
			ostringstream info;
			info <<"point-no. : "<<i <<endl;
			info <<"x-value   : "<<this->function_points[i].x_value <<endl;
			info <<"y-value  : "<<this->function_points[i].y_value <<endl;
			throw msg;

		}
		if(this->function_points[i].y_value<0.0){
			Error msg=this->set_error(3);
			ostringstream info;
			info <<"point-no. : "<<i <<endl;
			info <<"x-value   : "<<this->function_points[i].x_value <<endl;
			info <<"y-value  : "<<this->function_points[i].y_value <<endl;
			throw msg;
		
		}
	}
}
//Set error(s)
Error _Dam_2value_Function::set_error(const int err_type){
		string place="_Dam_2value_Function::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_points(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://end/begin is found before finishing the function input
			place.append("check_line(string line)");
			reason="A !END/!BEGIN is found before finishing the input of the function";
			help="Check the function in file";
			type=1;
			break;
		case 2://the x-value of the functzion < 0.0
			place.append("check_function(void)");
			reason="The x-value of the function is < 0.0";
			help="Check the function";
			type=3;
			break;
		case 3://the y-value of the functzion < 0.0
			place.append("check_function(void)");
			reason="The y-value of the function is < 0.0";
			help="Check the function";
			type=3;
			break;
		case 4://bad alloc
			place.append("set_members2plot_wid(Sys_Plot_Wid *widget)");
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