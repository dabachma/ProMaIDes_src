//#include "Hyd_Tables.h"
#include "Hyd_Headers_Precompiled.h"

//constructor
Hyd_Tables::Hyd_Tables(void){
	this->number_values=0;
	this->depending_value=NULL;
	this->independent_value=NULL;
	this->old_node=0;
	this->table_type_name=label::not_set;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Tables), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Tables::~Hyd_Tables(void){
	this->delete_table();

	//count the memory

	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Tables), _sys_system_modules::HYD_SYS);
}
//______________________
//public
///Set the table name => the table type
void Hyd_Tables::set_table_type(const string type_name){
	this->table_type_name=type_name;
}
//Allocate the number of points required for the table 
void Hyd_Tables::allocate_dependent_values(const int number){
	this->delete_table();
	this->number_values=number;
	try{
		this->allocate_table();
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the x-value of table; it must be allocated outside this class with number_values
void Hyd_Tables::set_independent_values(double *independent_value){
	this->independent_value=independent_value;
}
//Set the x-value of table; it is the depending value of an other table
void Hyd_Tables::set_independent_values( Hyd_Tables *independent_table){
	this->independent_value=independent_table->depending_value;

	if(this->number_values!=independent_table->number_values){
		Error msg=this->set_error(4);
		throw msg;
	}
}
//Get the number of values stroed in the table
int Hyd_Tables::get_number_values(void){
	return this->number_values;
}
//Set the y-value of the table
void Hyd_Tables::set_values(const int counter, const double dependingvalue){
	this->depending_value[counter]=dependingvalue;
}
//Get the table values for a given index
_hyd_table_values Hyd_Tables::get_values(const int counter){
	_hyd_table_values result;
	result.independentvalue=this->independent_value[counter];
	result.dependingvalue=this->depending_value[counter];

	return result;
}
//Get the interpolated values
double Hyd_Tables::get_interpolated_values(const double value2interpolation){
	
	//check if value to interploated is smaller than min value
	if(value2interpolation < this->independent_value[0]){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Tabletype : " << this->table_type_name << endl;
		info<<" Value to interpolate " << value2interpolation << " Minimum value in table " << this->independent_value[0] << endl;
		info<<" Profile gets dry " << endl;
		msg.make_second_info(info.str());
		throw msg;
		return 0;
	}
	if(value2interpolation == this->independent_value[0]){
		return this->depending_value[0];
	}
	//value to extrapolate is greater than max value in table
	if(value2interpolation>this->independent_value[this->number_values-1]){
		return this->extrapolate_value(value2interpolation);
	}
	//value to interploated is greater than the old value
	if(value2interpolation>=this->independent_value[this->old_node]){
		return this->interpolateupwards_value(value2interpolation);
	}
	//value to interploated is greater than the old value
	if(value2interpolation<this->independent_value[this->old_node]){
		return this->interpolatedownwards_value(value2interpolation);
	}

	//interpolation problem
	Error msg=this->set_error(2);
	ostringstream info;
	info << "Tabletype : " << this->table_type_name << endl;
	info<<" Value to interpolate " << value2interpolation <<endl;
	msg.make_second_info(info.str());
	throw msg;
	return 0;	
}
//Output the table and their values to display/console
void Hyd_Tables::output_table(ostringstream *cout){

	Sys_Common_Output::output_hyd->set_userprefix("TAB> ");

	*cout << "Table type:" << this->table_type_name <<endl;
	*cout << "Number of values: "<< this->number_values<<endl;
	*cout<< "No.      x-value    "  <<     "     y-value "<<endl;
	for (int i=0; i<this->number_values; i++){
		*cout << i <<"  "<<this->independent_value[i]<<" "<<this->depending_value[i]<<endl;
	}
	Sys_Common_Output::output_hyd->output_txt(cout, true);
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Clone table
void Hyd_Tables::clone_tables(Hyd_Tables *table){
	this->delete_table();
	this->number_values=table->number_values;
	this->table_type_name=table->table_type_name;
	this->allocate_table();
	for(int i=0; i<this->number_values; i++){
		this->depending_value[i]=table->depending_value[i];;
	}

}
//_________________
//private
//Allocate the table values with the given number of values
void Hyd_Tables::allocate_table(void){

	try{
		this->depending_value=new double[this->number_values];
		//count the memory
		Sys_Memory_Count::self()->add_mem(sizeof(double)*this->number_values, _sys_system_modules::HYD_SYS);
	}
	catch(bad_alloc&){
		Error msg=this->set_error(0);
		throw msg;
	}

	
	//init the table
	for(int i=0; i< this->number_values; i++){
		this->depending_value[i]=0.0;
	}

}
//Delete the table values 
void Hyd_Tables::delete_table(void){
	if(this->depending_value!=NULL){
		delete [] this->depending_value;
		this->depending_value=NULL;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->number_values, _sys_system_modules::HYD_SYS);

	}
}
//Extrapolate the given value, if it is outside the table values
double Hyd_Tables::extrapolate_value(const double value2interpolation){
	//set the the last node as old node
	this->old_node=this->number_values-1;
	//make extrapolation
	double m=0.0;
	double b=0.0;
	if(this->number_values>2){
		m=(this->depending_value[this->number_values-1]-this->depending_value[this->number_values-3])/(this->independent_value[this->number_values-1]-this->independent_value[this->number_values-3]);

		b=this->depending_value[this->number_values-3]-m*this->independent_value[this->number_values-3];
	}
	else{
		m=(this->depending_value[this->number_values-1]-this->depending_value[this->number_values-2])/(this->independent_value[this->number_values-1]-this->independent_value[this->number_values-2]);
		b=this->depending_value[this->number_values-2]-m*this->independent_value[this->number_values-2];
	
	}
	return m*value2interpolation+b;

}
//Interpolate the given value upwards from the old interpolation interval
double Hyd_Tables::interpolateupwards_value(const double value2interpolation){
	double interpolated_result=0.0;
	int end=0;
	int nodes=this->old_node;

	const int fMax[2] = { this->number_values/50, this->number_values/10 };// determination of stepwide for approximate search (two stepwide or one stepwide depending of number of nodes)
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
		while(value2interpolation >= this->independent_value[nodes] ){
			nodes=nodes+fMax[j];
			if(nodes>=this->number_values-1){
				break;
			}
		}
		nodes=nodes-fMax[j];
	}

	//search exact
	while(value2interpolation >= this->independent_value[nodes]){
			nodes=nodes+1;
	}
	//now interpolation
	//linear interploation scheme
	interpolated_result=this->depending_value[nodes-1]+ (this->depending_value[nodes]-this->depending_value[nodes-1])
				/ (this->independent_value[nodes] -this->independent_value[nodes-1])*(value2interpolation-this->independent_value[nodes-1]);
	//set the old node
	this->old_node=nodes-1;
	
	return interpolated_result;
}
//Interpolate the given value downwards from the old interpolation interval
double Hyd_Tables::interpolatedownwards_value(const double value2interpolation){
	double interpolated_result=0.0;
	int end=0;
	int nodes=this->old_node;

	const int fMax[2] = { this->number_values/50, this->number_values/10 };// determination of stepwide for approximate search (two stepwide or one stepwide depending of number of nodes)
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
		while(value2interpolation <= this->independent_value[nodes]){
			nodes=nodes-fMax[j];
			if(nodes<=0){
				break;
			}	
		}
		nodes=nodes+fMax[j];
	}

	//search exact
	while(value2interpolation <= this->independent_value[nodes]){
			nodes=nodes-1;
	}
	//now interpolation
	//linear interploation scheme
	interpolated_result=this->depending_value[nodes]+ (this->depending_value[nodes+1]-this->depending_value[nodes])
				/ (this->independent_value[nodes+1] -this->independent_value[nodes])*(value2interpolation-this->independent_value[nodes]);
	//set the old node
	this->old_node=nodes;
	
	return interpolated_result;

}
//Set the error(s)
Error Hyd_Tables::set_error(const int err_type){
		string place="Hyd_Tables::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_table(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://value is not in the range of the table
			place.append("get_interpolated_values(const double value2interpolation)");
			reason="Value to interpolate is smaller than the minimum value; Can not interpolate";
			help="Check the tables of the river system";
			type=13;
			break;
		case 2://problem with the interpolation
			place.append("get_interpolated_values(const double value2interpolation)");
			reason="Interpolation problem";
			help="The value to interpolate is a unknown type";
			type=13;
			break;
		case 4://the number of values in dependent table and the independent table are not the same
			place.append("set_independent_values(Hyd_Tables *independent_table)");
			reason="The number of values in the dependent table and the independent table are not the same";
			help="Check the tables";
			type=13;
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