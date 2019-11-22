#include <systemc>
#include <string>
#include <deque>
#include "Mlp.hpp"
#include "MemCtrl.hpp"
#include "Checker.hpp"
#include "Types.hpp"


using namespace std;
using namespace sc_core;

int sc_main(int argc, char* argv[])
{
   Mlp *Perceptron;
   MemCtrl *Mem_Ctrl;
   Checker *Check;
   
   num_t number; 
   sc_event e_mlp_start;
   sc_event e_ready[30]; //parametrize 30
   sc_event e_done[30];
   sc_event e_fin;
   deque <din_t> data;
   deque <din_t> results;
   deque <din_t> weights;
   bin_t bias;
   din_t res;
   
   Perceptron = new Mlp("mlp", &e_mlp_start, e_ready, e_done, &e_fin, weights, bias, data, results, res, number);
   Mem_Ctrl = new MemCtrl("mem_ctrl", &e_mlp_start, e_ready, e_done, weights, bias, data);
   Check = new Checker("checker", &e_fin, number);

   sc_start(10, sc_core::SC_SEC);
   cout << "Simulation finished at " << sc_time_stamp() << std::endl;
   
   delete Perceptron;
   delete Mem_Ctrl;	
   delete Check;

   return 0;
}
