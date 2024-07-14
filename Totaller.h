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
		void inc_ccbtn();
		void inc_icbtn();
		void inc_cibtn();
		void inc_iibtn();
		int get_ccbtn() const{ return ccbtn};
		int get_icbtn() const {return icbtn};
		int get_cibtn() const {return cibtn};
		int get_iibtn() const { return iibtn}; 




	private:
		//to be changed in constructor
		int totalccbt;
		int totalicbt;
		int totalcibt;
		int totaliibt;
		//totals for what is to be divided
		int ccbtn;
		int icbtn;
		int cibtn;
		int iibtn;


}


#endif