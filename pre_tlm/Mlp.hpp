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
   Mlp(sc_module_name name, double weights_1[][784], double weights_2[][30], double biases_1[], double biases_2[], double* slika);
   void proc();
   void show_output();
 protected:
   //sc_event ready;
   //sc_event *next;
   sc_event finish;
   sc_event ready1, ready2;

	
	//double* slika;
	double* slika;
	int recognized_number;

	double weights_1[30][784];
	double weights_2[10][30] ;
	double biases_1[30];
	double biases_2[10];
	//unnecessary initialization
	double results_1[30] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,8.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	double results_2[10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	Neuron *layer1[30];
	Neuron *layer2[10];

	deque <double> hidden_results;



};

#endif

