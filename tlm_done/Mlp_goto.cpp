#ifndef MLP_C
#define MLP_C
#include "Mlp.hpp"

Mlp::Mlp(sc_module_name name): sc_module(name)
{
   s_mlp_t.register_b_transport(this, &Mlp::b_transport);
   cout<<name<<" constructed"<<endl;
   SC_THREAD(classify);
   
   image_v.reserve(NUM_INPUTS);
   res_v.reserve(30); //layer output
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
idle: if(start != SC_LOGIC_0) goto start_state;//wait for start reg
		#ifdef QUANTUM
		qk.inc(sc_time(10, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#else
		offset += sc_time(10, SC_NS);
		#endif
	goto idle;
	  start_state:
      start=SC_LOGIC_0;
      toggle =  SC_LOGIC_1; 
      p_out->write(toggle);// wait
	  
	 

	  int p=0;
	  load_image:
         //send interrupt and then waste time until there is something in fifo
         //waiting for test image in fifo
			wait_pixel: if(p_fifo->nb_read(fifo_tmp)) goto load_pixel;
            #ifdef QUANTUM
            qk.inc(sc_time(10, SC_NS));
            offset = qk.get_local_time();
            qk.set_and_sync(offset);
            #else
            offset += sc_time(10, SC_NS);
            #endif
			goto wait_pixel;
			load_pixel:
	  image_v[p]= fifo_tmp;
	  toggle =  SC_LOGIC_0; 
	  p_out->write(toggle);
	  p++;
	  if(p<NUM_INPUTS) goto load_image;
	  else goto cont_3;
	  cont_3:
   
		
      
      res_v.clear();
	  //calculating outputs of each layer
	int layer=1;
	layer_state:
		unsigned int neuron = 0;
		neuron_state:
				acc=0;
				int i=0;
				synapse_state:	
				toggle =  SC_LOGIC_1; 
				p_out->write(toggle);
				   //send interrupt and then waste time until there is something in fifo
				   //waiting for weights in fifo
					wait_weight:   	if(p_fifo->nb_read(fifo_tmp)) goto load_weight;			  
							#ifdef QUANTUM
						  qk.inc(sc_time(10, SC_NS));
						  offset = qk.get_local_time();
						  qk.set_and_sync(offset);
						  #else
						  offset += sc_time(10, SC_NS);
						  #endif
					goto wait_weight;
				load_weight:	
				acc+=image_v[i]*fifo_tmp;
				toggle =  SC_LOGIC_0; 
				p_out->write(toggle);
				i++;
				if(i < neuron_array[layer-1]) goto synapse_state;
				//else ;
				
				toggle =  SC_LOGIC_1; 
				p_out->write(toggle);// wait
				//send interrupt and then waste time until there is something in fifo
				//waiting for bias in fifo
				wait_bias:  if(p_fifo->nb_read(fifo_tmp)) goto load_bias; 				  
						#ifdef QUANTUM
					  qk.inc(sc_time(10, SC_NS));
					  offset = qk.get_local_time();
					  qk.set_and_sync(offset);
					  #else
					  offset += sc_time(10, SC_NS);
					  #endif
				
				goto wait_bias;
				load_bias:
				toggle =  SC_LOGIC_0; 
				p_out->write(toggle);
				acc+=fifo_tmp;

				if(acc<0)
					acc*=0.001;
				
				res_v.push_back (acc);
			 
		neuron++;
		if(neuron < neuron_array[layer]) goto neuron_state;
		else goto cont;
		cont:
		  //up until the last layer, clear inputs of current layer &
		  //outputs of current layer become inputs for next layer
		  if(layer != LAYER_NUM-1) {
			  image_v.clear();
			  image_v = res_v;
			  res_v.clear();
			  }
		layer++;
		if(layer < LAYER_NUM) goto layer_state;
		else goto cont_2;
	  cont_2:
	  
      //find most convincing result of 10 output neurons
      max_res=res_v[0];
      cl_num=0;
     
      int j=1;
 find_res:
         if(max_res<res_v[j])
         {
            max_res=res_v[j];
            cl_num=(num_t)j;
         }
	  j++;
	  if(j<10) goto find_res;
		//else ;
		
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
