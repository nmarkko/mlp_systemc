#include "Neuron.hpp"
#define R_CHECK_OVERFLOW if(res.overflow_flag()) cout<<"WARNING OVERFLOW DETECTED IN NEURON"
#define A_CHECK_OVERFLOW if(acc.overflow_flag()) cout<<"WARNING OVERFLOW DETECTED IN NEURON"


Neuron::Neuron(sc_module_name name, sc_event *e_ready, sc_event *e_done, 
           deque<din_t> &weights, bin_t& bias, deque<din_t> &data, din_t &res):
           								    sc_module(name),
                                                          e_ready(e_ready),
                                                          e_done(e_done),
                                                          weights(weights),
                                                          bias(bias),
                                                          data(data),
                                                          res(res)
{
   cout<<name<<" constructed"<<endl;
   SC_THREAD(proc);
}

void Neuron::proc()
{
   while(true)
   {
      wait(*e_ready); //wait till data is ready
	acc=0.0;
      deque<din_t>::iterator it_w = weights.begin();
	deque<din_t>::iterator it_d = data.begin();

		while (it_w != weights.end())
		{
			
			acc +=  (*it_d).to_double() * (*it_w).to_double();
			it_w ++;
			it_d ++;
		}
			
      acc+=bias.to_double();
      A_CHECK_OVERFLOW<<"acc+=b"<<endl;
	if(acc<0) acc*=0.001; //leakyReLu
	wait(1, SC_NS);
      res=acc;
      R_CHECK_OVERFLOW<<"res=acc"<<endl;
	  
      e_done->notify(SC_ZERO_TIME); //notify that calculations are done
   }
   return;	
}
