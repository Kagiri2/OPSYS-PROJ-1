#include "Totaller.h"

Totaller::Totaller(){
	this->totalccbt = 0;
	this->totalicbt = 0;
	this->totalcibt = 0;
    this->totaliibt = 0;
	this->ccbtn = 0;
	this->icbtn = 0;
	this->cibtn = 0;
	this->iibtn = 0;
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

void Totaller::inc_ccbtn(){
	ccbtn ++;
}
void Totaller::inc_icbtn(){
	icbtn ++;
}
void Totaller::inc_cibtn(){
	cibtn ++;
}
void Totaller::inc_iibtn(){
	iibtn ++;
}
