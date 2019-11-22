#include "Mlp.hpp"
Mlp::Mlp(sc_module_name name,
           sc_event *e_mlp_start, sc_event *e_ready, sc_event *e_done, sc_event *e_fin, 
           deque<din_t> &weights, bin_t &bias, deque<din_t> &data, deque <res_t> &results, res_t &res, num_t &number):
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
   //results.reserve(30);
   for(int i=0; i<30; i++) //TODO: parametrize 30
   {
      string str("Neuron_no_");
      string num=to_string(i);
      str=str+num;
      neurons[i]=new Neuron(str.c_str(), &e_ready[i], &e_done[i], weights, bias, data, res);
   }
   SC_THREAD(classify);
}

void Mlp::classify()
{
   int k;
   res_t max;
   double output;
   while(true)
   {  
      e_mlp_start->notify(SC_ZERO_TIME);
	  results.clear();
      k=0;
      for(int layer=0; layer<2; layer++) //parametrize 2
      {
         
         //cout<<"-layer no."<< i+1 <<": "<< neuron_array[i+1] <<" neurons"<<endl;
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
				//e_...[neuron].notify(SC_ZERO_TIME);
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
