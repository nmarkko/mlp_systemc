#include <systemc>
#include <string>
#include <deque>
#include "Mlp.hpp"


using namespace std;
using namespace sc_core;

int sc_main(int argc, char* argv[])
{
   Mlp *m;
  /* MemCtrl *Mem_Ctrl;
   Checker *Check;
   
   num_t number; 
   int sv_num = 500;
   const int sv_len = 784;
   sc_event e_ready[10];
   sc_event e_next[10];
   sc_event e_fin;
   deque <din_t> data;
   lin_t lambda;
   bin_t bias;
   res_t res;*/
	double weights_1[30][784];
	double weights_2[10][30] ;
	double biases_1[30];
	double biases_2[10];
	double slika[784];

	int bias = 4;
	int result;

	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 784; j++) {
			weights_1[i][j] = 0.0;
		}
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 30; j++) {
			weights_2[i][j] = 0.0;
		}
	}

	for (int i = 0; i < 30; i++) {
			biases_1[i] = 0.0;
	}

	for (int i = 0; i < 10; i++) {
			biases_2[i] = 0.0;
	}

	for (int i = 0; i < 784; i++) {
			slika[i] = 1.0;
	}
   
	m = new Mlp("MLP", weights_1, weights_2, biases_1, biases_2, slika);
   //Mem_Ctrl = new MemCtrl("mem_ctrl",sv_num, sv_len, e_ready, e_next, lambda, bias, data);
   //Check = new Checker("checker", &e_fin, res, number);

   sc_start(20, sc_core::SC_SEC);
   cout << "Simulation finished at " << sc_time_stamp() << std::endl;
   
   delete m;
   //delete Mem_Ctrl;	
   //delete Check;

   return 0;
}
