#ifndef _VP_HPP_
#define _VP_HPP_

#include <systemc>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include "IntCon.hpp"
#include "Mlp.hpp"
#include "Types.hpp"
#include "Image_preprocessor.hpp"
#include "MemCtrl.hpp"
#include "RAM.hpp"
#include "DMA.hpp"
using namespace sc_core;
using namespace std;

class VP : sc_module
{
      public:
      VP(sc_module_name);
      tlm_utils::simple_target_socket<VP> s_vp_t0;
      tlm_utils::simple_initiator_socket<VP> s_vp_i1;

      sc_out<sc_logic> p_out0;
      sc_out<sc_logic> p_out1;

      protected:
      tlm_utils::simple_initiator_socket<VP> s_vp_i0;
      tlm_utils::simple_target_socket<VP> s_vp_t1;
      IntCon ic;
      Mlp mlp;
      Image_preprocessor prep;
      RAM ram;
      DMA dma; 

      sc_fifo<din_t> fifo;

      typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
      void b_transport0(pl_t&, sc_core::sc_time&);
      void b_transport1(pl_t&, sc_core::sc_time&);
};

#endif
