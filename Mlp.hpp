#ifndef MLP_H
#define MLP_H
#include <systemc>
#include <deque>
#include <cstdlib>
#include <ctime>
#include "Neuron.hpp"
//#include "input_data.hpp"
//#include "Format.hpp"
//#include "Types.hpp"

using namespace std;
using namespace sc_core;

SC_MODULE(Mlp)
{
 public:
   SC_HAS_PROCESS(Mlp);
   Mlp(sc_module_name name, double** weights_1, double** weights_2, double* biases_1, double* biases_2, double* slika);
   void proc();
 protected:
   //sc_event ready;
   //sc_event *next;
   sc_event finish;
   /*double weights_1[30][784];
	double weights_2[10][30] ;
	double biases_1[30];
	double biases_2[10];*/
	double** weights_1;
	double** weights_2 ;
	double* biases_1;
	double* biases_2;

	double results_1 [30];
   double results_2 [10];
	double* slika;
   int recognized_number;

};

#endif

