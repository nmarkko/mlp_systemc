
#ifndef TYPES_H
#define TYPES_H
#include <systemc>
#include <array>
#include <sstream>
#include <fstream>
#define NUM_INPUTS 784
#define QUANTUM

#define WIDTH   16
#define Q   SC_RND
#define O   SC_SAT_SYM
#define LAYER_NUM 3
using namespace sc_dt;
using namespace tlm;
using namespace std;

const array<int, 3> neuron_array = {784, 30, 10}; 
//typedef sc_fixed_fast<WIDTH,2,Q,O> in_data_t;
typedef sc_fixed_fast<WIDTH,3,Q,O> in_data_t; //min 3 integer bits necessary for correct calculations
typedef sc_fixed_fast<28,14,Q,O> acc_t;
typedef sc_fixed_fast<28,14,Q,O> p_t;
typedef sc_uint <16> num_t;

//TLM
typedef tlm_base_protocol_types::tlm_payload_type pl_t;
typedef tlm_base_protocol_types::tlm_phase_type phase_t;

#endif



