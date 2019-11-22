#ifndef MLP_H
#define MLP_H
#include <systemc>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <array>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include "tlm_utils/tlm_quantumkeeper.h"
#include "Types.hpp"

using namespace std;
using namespace sc_core;
using namespace tlm;

class Mlp : public sc_module
{
   public:
      SC_HAS_PROCESS(Mlp);
      Mlp(sc_module_name name);
      
      tlm_utils::simple_target_socket<Mlp> s_mlp_t;
      
      sc_out<sc_logic> p_out;
      sc_port<sc_fifo_in_if<in_data_t>> p_fifo;

   protected:
      void b_transport(pl_t&, sc_time&);
      void classify();

      //Mlp VARIABLES
      vector <in_data_t> image_v;
      vector <in_data_t> res_v;
      in_data_t max_res;
      num_t cl_num;
      acc_t acc;
      sc_logic toggle;
      sc_logic start;


};

#endif
