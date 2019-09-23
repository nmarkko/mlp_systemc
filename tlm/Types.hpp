#ifndef TYPES_H
#define TYPES_H
#include <systemc>
#include <array>
#define SV_LEN 784
#define QUANTUM

#define WIDTH   16
#define Q   SC_RND
#define O   SC_SAT_SYM

using namespace sc_dt;
using namespace tlm;
using namespace std;
//sc_dt
const array<int, 3> neuron_array = {784, 30, 10}; 
typedef sc_fixed_fast <WIDTH,2,Q,O> din_t;
typedef sc_fixed_fast <WIDTH,2,Q,O> bin_t;
typedef sc_fixed_fast <WIDTH+3,7,Q,O> deskew_t;
typedef sc_fixed_fast <WIDTH,2,Q,O> lin_t;
typedef sc_fixed_fast <WIDTH,6,Q,O> res_t;
typedef sc_fixed_fast <28,14,Q,O> acc_t;
typedef sc_fixed_fast <28,14,Q,O> p_t;
//typedef sc_fixed_fast <WIDTH+8,14,Q,O> acc_t;
//typedef sc_fixed_fast <WIDTH+12,14,Q,O> p_t;
/*typedef double din_t;
typedef double bin_t;
typedef double deskew_t;
typedef double lin_t;
typedef double res_t;
typedef double acc_t;
typedef double p_t;*/

typedef sc_uint <4> num_t;
//typedef int num_t;
//TLM
typedef tlm_base_protocol_types::tlm_payload_type pl_t;
typedef tlm_base_protocol_types::tlm_phase_type phase_t;

#endif
