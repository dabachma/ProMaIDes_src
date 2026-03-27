//#include "Sys_Memory_Count.h"
#include "Sys_Headers_Precompiled.h"


//constructor
Sys_Median_Calculator::Sys_Median_Calculator(void){
	this->n = 0.0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Median_Calculator), _sys_system_modules::SYS_SYS);

}

//destructor
Sys_Median_Calculator::~Sys_Median_Calculator(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Median_Calculator), _sys_system_modules::SYS_SYS);

}
//_______________________
//public
//Add a new data value for an update 
void Sys_Median_Calculator::add_value(double x){
    this->n++;
    // Initialisierungsphase: Die ersten 5 Werte sammeln und sortieren
    if (this->n <= 5) {
        q[this->n] = x;
        if (this->n == 5) {
            std::sort(this->q + 1, this->q + 6);
            for (int i = 1; i <= 5; ++i) this->n_pos[i] = i;
            this->n_desired[1] = 1;
            this->n_desired[2] = 2; // 25% Quantil
            this->n_desired[3] = 3; // Median (50%)
            this->n_desired[4] = 4; // 75% Quantil
            this->n_desired[5] = 5;
        }
        return;
    }

    // 1. Finde das passende Intervall und schiebe Marker
    int k;
    if (x < this->q[1]) { this->q[1] = x; k = 1; }
    else if (x < this->q[2]) k = 1;
    else if (x < this->q[3]) k = 2;
    else if (x < this->q[4]) k = 3;
    else if (x < this->q[5]) k = 4;
    else { this->q[5] = x; k = 4; }

    for (int i = k + 1; i <= 5; ++i) this->n_pos[i]++;

    // 2. Gewünschte Positionen aktualisieren
    this->n_desired[2] = 1.0 + (this->n - 1) * 0.25;
    this->n_desired[3] = 1.0 + (this->n - 1) * 0.50;
    this->n_desired[4] = 1.0 + (this->n - 1) * 0.75;
    this->n_desired[5] = this->n;

    // 3. Marker anpassen
    this->update_markers();
	
}
//Get the median value
double Sys_Median_Calculator::get_median(void){
	
    return (this->n < 5) ? 0.0 : this->q[3]; // q[3] median
	
}
///Get the 25 percentile
double Sys_Median_Calculator::get_perc25(void) {

    return (this->n < 5) ? 0.0 : this->q[2]; // q[2] perc25
}


//______________________
//private
//Update the markers
void Sys_Median_Calculator::update_markers(void){
    for (int i = 2; i <= 4; ++i) {
        double d = this->n_desired[i] - this->n_pos[i];
        if ((d >= 1.0 && this->n_pos[i + 1] - this->n_pos[i] > 1.0) || (d <= -1.0 && this->n_pos[i - 1] - this->n_pos[i] < -1.0)) {
            int sign = (d > 0) ? 1 : -1;

            // Parabolische Formel zur Wertanpassung
            double q_new = this->q[i] + (sign / (this->n_pos[i + 1] - this->n_pos[i - 1])) *
                ((this->n_pos[i] - this->n_pos[i - 1] + sign) * (this->q[i + 1] - this->q[i]) / (this->n_pos[i + 1] - this->n_pos[i]) +
                    (this->n_pos[i + 1] - this->n_pos[i] - sign) * (this->q[i] - this->q[i - 1]) / (this->n_pos[i] - this->n_pos[i - 1]));

            // Sicherstellen, dass die Marker sortiert bleiben
            if (this->q[i - 1] < q_new && q_new < q[i + 1]) {
                this->q[i] = q_new;
            }
            else {
                // Lineare Anpassung als Fallback
                this->q[i] = this->q[i] + sign * (this->q[i + sign] - this->q[i]) / (this->n_pos[i + sign] - this->n_pos[i]);
            }
            this->n_pos[i] += sign;
        }
    }
	
}

