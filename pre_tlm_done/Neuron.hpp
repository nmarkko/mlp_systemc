#ifndef NEURON_H
#define NEURON_H
#include <systemc>
#include <deque>
#include <cstdlib>
#include <ctime>
#include "Types.hpp"

using namespace std;
using namespace sc_core;

SC_MODULE(Neuron)
{
 public:
   SC_HAS_PROCESS(Neuron);
   Neuron(sc_module_name name,
           sc_event *e_ready, sc_event *e_done, 
           deque<din_t> &weights, bin_t& bias, deque<din_t> &data, din_t &res);
   void proc();
 protected:
   sc_event *e_ready;
   sc_event *e_done;
   deque <din_t> &weights;
   bin_t &bias;
   deque <din_t> &data;
   din_t &res;
   acc_t acc;

};

#endif
