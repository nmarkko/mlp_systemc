#ifndef NEURON_H
#define NEURON_H
#include <systemc>
#include <deque>
#include <cstdlib>
#include <ctime>
//#include "Format.hpp"
//#include "Types.hpp"

using namespace std;
using namespace sc_core;

SC_MODULE(Neuron)
{
 public:
   SC_HAS_PROCESS(Neuron);
   Neuron(sc_module_name name, sc_event *ready, double* input, int input_size, deque<double> weights, double bias, double *result);
   void proc();
   void bye_felicia();
   //acc_t get_acc();
 protected:
   sc_event *ready;
   //sc_event *next;
   sc_event finish;
   int input_size;
   double* input; //was not working properly when input was of type deque<double>
   deque <double> weights;
   double bias;
   double *result;



};

#endif

