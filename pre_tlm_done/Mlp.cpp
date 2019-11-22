#include "Mlp.hpp"
Mlp::Mlp(sc_module_name name,
           sc_event *e_mlp_start, sc_event *e_ready, sc_event *e_done, sc_event *e_fin, 
           deque<din_t> &weights, bin_t &bias, deque<din_t> &data, deque <din_t> &results, din_t &res, num_t &number): sc_module(name),
									e_mlp_start(e_mlp_start),
                                                          e_ready(e_ready),
                                                          e_done(e_done),
                                                          e_fin(e_fin),
                                                          weights(weights),
                                                          bias(bias),
                                                          data(data),
									results(results),
                                                          res(res),
                                                          number(number)
                                                          
{
   cout<<"Mlp constructed"<<endl;
   for(int i=0; i<30; i++) 
   {
      string str("Neuron_no_");
      string num=to_string(i+1);
      str=str+num;
      neurons[i]=new Neuron(str.c_str(), &e_ready[i], &e_done[i], weights, bias, data, res);
   }
   SC_THREAD(classify);
}

void Mlp::classify()
{
   din_t max;
   din_t output;
   while(true)
   {  
      e_mlp_start->notify(SC_ZERO_TIME);
	results.clear();
      for(int layer=0; layer<2; layer++) 
      {
		 if(layer!=0)
		 {
			 data.clear();
			 for(int index=0; index<neuron_array[layer]; index++) {
				output=results[index];
				data.push_back(output);
			 }
			 results.clear();
		 }

		for(int neuron=0; neuron< neuron_array[layer+1]; neuron++)
		{			
				wait(e_done[neuron]);
				results.push_back(res);
		}
	
      }
	  
      max=results[0];
      number=0;
      for(int i=1; i<10; i++)
      {
        if(max<results[i])
        {
           max=results[i];
           number=i;
        } 
      }

	e_fin->notify(SC_ZERO_TIME);
   }
   return;	
}
