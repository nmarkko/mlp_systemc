#ifndef RAM_H
#define RAM_H
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include "Format.hpp"
#include "Types.hpp"
#include <string>
#include <array>
#include <vector>
using namespace std;
using namespace sc_core;
using namespace tlm;

class RAM : public sc_module
{
   public:
      RAM(sc_module_name name);
      tlm_utils::simple_target_socket<RAM> s_ram_t0;
      tlm_utils::simple_target_socket<RAM> s_ram_t1;
   protected:
      void b_transport(pl_t&, sc_time&);
      vector<in_data_t> mem;
};
#endif
