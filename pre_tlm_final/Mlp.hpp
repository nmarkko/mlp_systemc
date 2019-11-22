#ifndef MLP_H
#define MLP_H
#include <systemc>
#include <cstdlib>
#include <ctime>
#include "Neuron.hpp"
#include "Types.hpp"

using namespace std;
using namespace sc_core;

SC_MODULE(Mlp)
{
   public:
      SC_HAS_PROCESS(Mlp);
     Mlp(sc_module_name name,
           sc_event *e_mlp_start, sc_event *e_ready, sc_event *e_done, sc_event *e_fin, 
            deque<din_t> &weights, bin_t &bias, deque<din_t> &data, deque <res_t> &results, res_t &res, num_t &number);
      void classify();
	protected:
		sc_event *e_mlp_start;
		sc_event *e_ready;
		sc_event *e_done;
		sc_event *e_fin;
		deque<din_t> &weights;
		bin_t &bias;
		deque <din_t> &data;
		deque <res_t> &results;
		res_t &res;
		num_t &number;

		//deque <din_t> y;
		//acc_t acc;
		Neuron *neurons[30]; //parametrize 30
};

#endif