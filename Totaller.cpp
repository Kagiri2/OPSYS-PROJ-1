#include "totaller.h"

Totaller::Totaller(){
	totalccbt = 0;
	totalicbt = 0;
	totalcibt = 0;
    totaliibt = 0;
}

void Totaller::inc_ccbt(int amt){
	totalccbt += amt;
}
void Totaller::inc_icbt(int amt){
	totalicbt += amt;
}
void Totaller::inc_cibt(int amt){
	totalcibt += amt;
}
void Totaller::inc_iibt(int amt){
	totaliibt += amt;
}