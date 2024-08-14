#include "TotallerTwo.h"

TotallerTwo::TotallerTwo(){

	this->cut = 0;
	this->totalcbwt = 0;
	this->totalibwt = 0;
	this->totalcbtt = 0;
	this->totalibtt = 0;

	this->totalcbcs = 0;
	this->totalibcs = 0;

	this->cpreemptn = 0;
	this->ipreemptn = 0;
	this->ccots = 0;
	this-> icots = 0;
}

	//cpu utilization total
	void TotallerTwo:: inc_cut(int amt){
		cut += amt;
	}
	

	//turnaround
	void TotallerTwo:: inc_cbtt(int amt){
		totalcbtt += amt;
	}
	

	void TotallerTwo:: inc_ibtt(int amt){
		totalibtt += amt;
	}

	//wait

	void TotallerTwo:: inc_cbwt(int amt){
		totalcbwt += amt;

	}
	
	void TotallerTwo:: inc_ibwt(int amt){
		totalibwt += amt;

	}
	
	
	

	//preemptions

	void TotallerTwo:: inc_cpreemptn(int amt){
		cpreemptn += amt;
	}
	
	
	void TotallerTwo:: inc_ipreemptn(int amt){
		ipreemptn += amt;

	}
	

	//context switches

	void TotallerTwo:: inc_cbcs(){
		totalcbcs++;
	}
	

	void TotallerTwo:: inc_ibcs(){
		totalicbs++;

	}

	//these divide by total cpu bursts

	//one time slice

	void TotallerTwo:: inc_ccots(amt){
		ccots += amt;
	}
	

	void TotallerTwo:: inc_icots(amt){
		icots += amt;

	}
