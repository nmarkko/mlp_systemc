#ifndef NEURON_H
#define NEURON_H
#include <systemc>
#include <deque>
#include <cstdlib>
#include <ctime>
#include "Format.hpp"
#include "Types.hpp"

using namespace std;
using namespace sc_core;

SC_MODULE(Neuron)
{
 public:
   SC_HAS_PROCESS(Neuron);
   Neuron(sc_module_name name,
           sc_event *e_ready, sc_event *e_done, 
           deque<din_t> &weights, bin_t& bias, deque<din_t> &data, res_t &res);
   void proc();
   //acc_t get_acc();
 protected:
   sc_event *e_ready;
   sc_event *e_done;
   //sc_event *e_fin;
   deque <din_t> &weights;
   bin_t &bias;
   deque <din_t> &data;
   res_t &res;

   //deque <din_t> y;
   acc_t acc;


};

#endif