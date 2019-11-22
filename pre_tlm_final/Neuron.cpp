#include "Neuron.hpp"
#define R_CHECK_OVERFLOW if(res.overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN NEURON"<<RST
#define A_CHECK_OVERFLOW if(acc.overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN NEURON"<<RST


Neuron::Neuron(sc_module_name name,
           sc_event *e_ready, sc_event *e_done, 
           deque<din_t> &weights, bin_t& bias, deque<din_t> &data, res_t &res):
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
   int k;
   //bin_t b;	TODO: don't need this, delete it
   while(true)
   {
      wait(*e_ready); //wait till data is ready
	  acc=0.0;
      deque<din_t>::iterator it_w = weights.begin();
	  deque<din_t>::iterator it_d = data.begin();

		while (it_w != weights.end())
		{
			//std::cout << ' ' << *it++;
			//acc +=  data[it_d].to_double() * weights[it_w].to_double();
			acc +=  (*it_d).to_double() * (*it_w).to_double();
			it_w ++;
			it_d ++;
		}	
      acc+=bias.to_double();
      A_CHECK_OVERFLOW<<"acc+=b"<<endl;
	  if(acc<0) acc*=0.001; //leakyReLu
	  
      res=acc;
      R_CHECK_OVERFLOW<<"res=acc"<<endl;
	  
      e_done->notify(SC_ZERO_TIME); //notify that calculations are done
   }
   return;	
}

/* acc_t Neuron::get_acc()
{
   return acc;
} */