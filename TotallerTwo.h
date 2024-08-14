#ifndef TOTALLERTWO_H
#define TOTALLERTWO_H

class TotallerTwo{
	public:
		
		TotallerTwo();

		//cpu utilization total
		void inc_cut(int amt);
		
		int get_cut() const { return cut; }


		//turnaround
		void inc_cbtt(int amt);
		
		int get_cbtt() const { return totalcbtt; }

		void inc_ibtt(int amt);
		
		int get_ibtt() const { return totalibtt; }

		//wait

		void inc_cbwt(int amt);
		
		int get_cbwt() const { return totalcbwt; }

		void inc_ibwt(int amt);
		
		int get_ibwt() const { return totalibwt; }
		
		

		//preemptions

		void inc_cpreemptn(int amt);
		
		int get_cpreemptn() const { return cpreemptn; }
		
		void inc_ipreemptn(int amt);
		
		int get_ipreemptn() const { return ipreemptn; }
		

		//context switches

		void inc_cbcs();
		
		int get_cbcs() const { return totalcbcs; }

		void inc_ibcs();
		
		int get_ibcs() const { return totalibcs; }

		//these divide by total cpu bursts

		//one time slice

		void inc_ccots(amt);
		
		int get_ccots() const { return ccots; }

		void inc_icots(amt);
		
		int get_icots() const { return icots; }







		
	private:

		//cpu utilization time
		int cut;

		//wait and turnaround to be divided by burst number
		int totalcbwt;
		int totalibwt;
		int totalcbtt;
		int totalibtt;

		//context switches
		int totalcbcs;
		int totalibcs;
		//totals for what is to be divided are just divided by num cpu and io bound processes

		int cpreemptn;
		int ipreempth;

		//divide by num bursts
		int ccots;
		int icots;

		
};


#endif