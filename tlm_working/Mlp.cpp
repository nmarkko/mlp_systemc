#ifndef MLP_C
#define MLP_C
#include "Mlp.hpp"

Mlp::Mlp(sc_module_name name): sc_module(name)
{
   s_mlp_t.register_b_transport(this, &Mlp::b_transport);
   cout<<name<<" constucted"<<endl;
   SC_THREAD(classify);
   
   image_v.reserve(784);
   res_v.reserve(30);
	output_v.reserve(10);
   toggle = SC_LOGIC_0;
   start = SC_LOGIC_0;
}

void Mlp::b_transport(pl_t& pl, sc_time& offset)
{
	
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned char *buf = pl.get_data_ptr();
   unsigned int len   = pl.get_data_length();

   switch(cmd)
   {
   case TLM_WRITE_COMMAND://Begin classification
         
      image_v.clear();
      pl.set_response_status( TLM_OK_RESPONSE );
      start=SC_LOGIC_1;
      offset+=sc_time(10, SC_NS);
      break;

   case TLM_READ_COMMAND://Read classified number 
         
      buf=(unsigned char *)&cl_num;
      pl.set_data_ptr        ( buf );
      pl.set_response_status( TLM_OK_RESPONSE );
      offset+=sc_time(5, SC_NS);
      break;

   default:
      pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
   }

}

void Mlp::classify ()
{
   in_data_t fifo_tmp;
   sc_time offset=SC_ZERO_TIME;
   #ifdef QUANTUM
   tlm_utils::tlm_quantumkeeper qk;
   qk.reset();
   #endif

   while(1)
   {
      while(start==SC_LOGIC_0)//wait for start reg
      {
			#ifdef QUANTUM
			qk.inc(sc_time(10, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#else
			offset += sc_time(10, SC_NS);
			#endif
         
      }
      start=SC_LOGIC_0;
      toggle =  SC_LOGIC_1; 
      p_out->write(toggle);// wait
	  
	 
      for( int p=0; p<SV_LEN; p++)
      {
         //send interrupt and then waste time until there is something in fifo
         //waiting for test image in fifo
         while(!p_fifo->nb_read(fifo_tmp))
         {
            #ifdef QUANTUM
            qk.inc(sc_time(10, SC_NS));
            offset = qk.get_local_time();
            qk.set_and_sync(offset);
            #else
            offset += sc_time(10, SC_NS);
            #endif
         }
         image_v[p]= fifo_tmp;
         toggle =  SC_LOGIC_0; 
         p_out->write(toggle);
      }
		
      
      res_v.clear();
	  
	  for(unsigned int layer = 1; layer < LAYER_NUM; layer++) {
		//calculating outputs of neurons in a single layer
		  for(unsigned int neuron=0; neuron < neuron_array[layer]; neuron++)
		  {
			 acc=0;
			 //calculating output of a single neuron
			 for( int i=0; i < neuron_array[layer-1]; i++)
			 {
				toggle =  SC_LOGIC_1; 
				p_out->write(toggle);
			   //send interrupt and then waste time until there is something in fifo
			   //waiting for weights in fifo
			   while(!p_fifo->nb_read(fifo_tmp))
			   {
				  #ifdef QUANTUM
				  qk.inc(sc_time(10, SC_NS));
				  offset = qk.get_local_time();
				  qk.set_and_sync(offset);
				  #else
				  offset += sc_time(10, SC_NS);
				  #endif
			   }
				acc+=image_v[i]*fifo_tmp;
				toggle =  SC_LOGIC_0; 
				p_out->write(toggle);

			 }
			 toggle =  SC_LOGIC_1; 
			 p_out->write(toggle);// wait
			 //send interrupt and then waste time until there is something in fifo
			 //waiting for bias in fifo
			 while(!p_fifo->nb_read(fifo_tmp))
			 {
				#ifdef QUANTUM
				qk.inc(sc_time(10, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#else
				offset += sc_time(10, SC_NS);
				#endif
			 }
			 toggle =  SC_LOGIC_0; 
			 p_out->write(toggle);
			 acc+=fifo_tmp;

			//leakyReLu
			if(acc<0)
				acc*=0.001;
				
			res_v.push_back (acc);
			 
		  }
		  //up until the last layer
		  //output of the current layer is input for the next
		  if(layer != LAYER_NUM-1) {
			  image_v.clear();
			  image_v = res_v;
			  res_v.clear();
			  }
	  }
	  
      //find most convincing result of 10 output neurons
      max_res=res_v[0];
      cl_num=0;
      for(int i=1; i<10; i++)
      {
         if(max_res<res_v[i])
         {
            max_res=res_v[i];
            cl_num=(num_t)i;
         }
      }
		//std::cout<<"MLP result calculated. "<<cl_num<<std::endl;
      //classification of test image finished, send interrupt to test bench
      toggle =  SC_LOGIC_1; 
      p_out->write(toggle);// wait
      #ifdef QUANTUM
      qk.inc(sc_time(20, SC_NS));
      offset = qk.get_local_time();
      qk.set_and_sync(offset);
      #endif
      toggle =  SC_LOGIC_0; 
      p_out->write(toggle);
   }
}
#endif
