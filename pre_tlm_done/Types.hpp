#ifndef TYPES_H
#define TYPES_H
#include <array>
#include <systemc>
#include <sstream>
#include <fstream>
#define W   16
#define Q   SC_RND
#define O   SC_SAT_SYM
#define NUM_INPUTS 784

using namespace sc_dt;
using namespace std;

const array<int, 3> neuron_array = {784, 30, 10}; 
typedef sc_fixed_fast <W,4,Q,O> din_t;
typedef sc_fixed_fast <W,4,Q,O> bin_t;
//typedef sc_fixed_fast <28,14,Q,O> res_t;
typedef sc_fixed_fast <28,14,Q,O> acc_t;
typedef sc_uint <4> num_t;


#endif
