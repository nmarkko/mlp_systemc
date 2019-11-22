#ifndef MEMCTRL_H
#define MEMCTRL_H
#include <iostream>
#include <systemc>
#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <array>
#include <algorithm>
#include "Types.hpp"
using namespace std;
using namespace sc_core;

SC_MODULE(MemCtrl)
{
 public:
   SC_HAS_PROCESS(MemCtrl);
   MemCtrl(sc_module_name name,
		 sc_event *e_mlp_start, sc_event *e_ready, sc_event *e_done, deque<din_t>& weights, bin_t& bias, deque<din_t>& data);
   int num_of_lines(string str);
   int num_of_test_img();
 protected:
	sc_event *e_mlp_start;
   sc_event *e_ready;
   sc_event *e_done;
   deque<din_t>& weights;
   bin_t &bias;
   deque <din_t> &data;
   
   deque<din_t> images;
   deque<din_t> mem;


   //METHODS
   void grab_from_mem();
   void file_extract();
   

};

#endif