#ifndef TOTALLER_H
#define TOTALLER_H

class Totaller{
	public:
		Totaller();
		void inc_ccbt(int amt);
		void inc_icbt(int amt);
		void inc_cibt(int amt);
		void inc_iibt(int amt);
		int get_ccbt() const{ return totalccbt};
		int get_icbt() const {return totalicbt};
		int get_cibt() const {return totalcibt};
		int get_iibt() const { return totaliibt}; 



	private:
		//to be changed in constructor
		int totalccbt;
		int totalicbt;
		int totalcibt;
		int totaliibt;
		//totals for what is to be divided
		//are in main


}


#endif