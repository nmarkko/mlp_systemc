#ifndef Image_preprocessor_H
#define Image_preprocessor_H
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include "tlm_utils/tlm_quantumkeeper.h"
#include "Types.hpp"
#include <string>
#include <array>
#include <vector>
using namespace std;
using namespace sc_core;
using namespace tlm;

class Image_preprocessor : public sc_module
{
   public:
      SC_HAS_PROCESS(Image_preprocessor);
      Image_preprocessor(sc_module_name name);

      tlm_utils::simple_initiator_socket<Image_preprocessor> s_prep_i;
      tlm_utils::simple_target_socket<Image_preprocessor> s_prep_t;

      sc_out<sc_logic> p_out;

   protected:

      void b_transport(pl_t&, sc_time&);
      void proc();

      vector<in_data_t> Image_preprocessing(vector <in_data_t> image);
      vector<in_data_t> image;
      sc_logic toggle;
      sc_logic start;
};
#endif
