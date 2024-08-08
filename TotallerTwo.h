#ifndef TOTALLERTWO_H
#define TOTALLERTWO_H

class TotallerTwo{
	public:
		//turnaround
		TotallerTwo();
		void inc_cbtt(int amt);
		
		int get_cbtt() const { return totalccbt; }

		void inc_cbwt(int amt);
		
		int get_cbwt() const { return totalccbt; }
		

		//turnaround divisors
		void inc_cbttn();
		
		int get_cbttn() const { return ccbtn; }

		

		//wait divisors
		
		void inc_cbwtn();
		
		int get_cbwtn() const { return ccbtn; }

		void inc_cpreemptn(int amt);
		
		int get_cpreemptn() const { return ccbtn; }
		
		
		void inc_ipreemptn(int amt);
		
		int get_ipreemptn() const { return ccbtn; }

		void inc_ccots(int amt);
		
		int get_ccots() const { return ccbtn; }

		void inc_icots(int amt);
		
		int get_icots() const { return ccbtn; }
		//these divide by total cpu bursts







		
	private:
		//to be changed in constructor
		int totalcbwt;
		int totalcbtt;
		//totals for what is to be divided
		int cbwtn;
		int cbttn;

		int cpreemptn;
		int ipreempth;

		int ccots;
		int icots;

		
};


#endif