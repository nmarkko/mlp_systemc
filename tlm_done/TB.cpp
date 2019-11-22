#ifndef TB_C
#define TB_C
#include "TB.hpp"

#define LAYER1 0
#define LAYER2 NUM_INPUTS*30 + 30
#define IMG_START LAYER2 + (30*10 + 10)


const array<int, 2> layer_start_addr = {LAYER1, LAYER2};

SC_HAS_PROCESS(TB);

TB::TB(sc_module_name name) : sc_module(name)
{
   cout<<name<<" constructed"<<endl;
   SC_THREAD(verify);
   match=0;
   p_port0.bind(sig0);
   p_port1.bind(sig1);
   sig0=SC_LOGIC_0;
   sig1=SC_LOGIC_0;
}


void TB::verify()
{
   pl_t pl;
   unsigned char *buf;
   in_data_t *image;
	in_data_t *helper;
   unsigned int addr = 0;
   num_t num;

   sc_time offset=SC_ZERO_TIME;
   #ifdef QUANTUM
   tlm_utils::tlm_quantumkeeper qk;
   qk.reset();
   #endif

   lines = num_of_lines("../saved_data/y.txt");
   labels_extraction();

   #ifdef QUANTUM
   qk.inc(sc_time(10, SC_NS));
   offset = qk.get_local_time();
   qk.set_and_sync(offset);
   #else
   offset += sc_time(10, SC_NS);
   #endif
   

   cout<<endl;
   cout<<"***** STARTING CLASSIFICATION *****"<<endl;
   cout<<"  -testing on "<<lines<<" images images-  \n"<<endl;
   
   for(int img=0; img<lines; img++)
   {
         //READ NEW IMAGE FROM DDR 
         //cout<<"READ NEW IMAGE FROM DDR"<<endl;
         addr = (IMG_START + img*NUM_INPUTS);
         pl.set_address(addr);
         pl.set_data_length(NUM_INPUTS);
         pl.set_command(TLM_READ_COMMAND);
         s_tb_i1->b_transport(pl, offset);
         buf = pl.get_data_ptr();
         image = ((in_data_t*)buf);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif
         
         //WRITE NEW IMAGE TO BRAM
         //cout<<"WRITE NEW IMAGE TO BRAM"<<endl;
         pl.set_data_ptr((unsigned char *)image);
         pl.set_address(0x80000000);
         pl.set_data_length(NUM_INPUTS);
         pl.set_command(TLM_WRITE_COMMAND);
         s_tb_i0->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif
         
         //START PREP MODULE 
         //cout<<"DUMMY FOR PREP"<<endl;
         pl.set_address(0x81000000);
         pl.set_command(TLM_WRITE_COMMAND);
         s_tb_i0->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
         
         #ifdef QUANTUM
         qk.inc(sc_time(10, SC_NS));
         offset = qk.get_local_time();
         qk.set_and_sync(offset);
         #endif
         
         //WAIT FOR PREP TO FINISH PROCESSING IMAGE    
         //cout<<"WAITING FOR INTERRUPT"<<endl;
         do
         {
            #ifdef QUANTUM
            qk.inc(sc_time(10, SC_NS));
            offset = qk.get_local_time();
            qk.set_and_sync(offset);
            #endif
            
         tmp_sig=sig0.read();
         }
         while( tmp_sig == SC_LOGIC_0);

         //READ PREPROCESSED IMAGE FROM BRAM
         //cout<<"READ PREPROCESSED IMAGE FROM BRAM"<<endl;
         pl.set_address(0x80000000+NUM_INPUTS);
         pl.set_data_length(NUM_INPUTS);
         pl.set_command(TLM_READ_COMMAND);
         s_tb_i0->b_transport(pl, offset);
         buf = pl.get_data_ptr();
			
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
			

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif
         
         //WRITE NEW IMAGE TO DDR 
         //cout<<"WRITE NEW IMAGE TO DDR"<<endl;
         addr = (IMG_START + img*NUM_INPUTS);
         pl.set_address(addr);
         pl.set_data_ptr(buf);
         pl.set_data_length(NUM_INPUTS);
         pl.set_command(TLM_WRITE_COMMAND);
         s_tb_i1->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif

         //START MLP
         //cout<<"START MLP"<<endl;
         pl.set_address(0x82000000);
         pl.set_data_length(NUM_INPUTS);
         pl.set_command(TLM_WRITE_COMMAND);
         s_tb_i0->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
              
         #ifdef QUANTUM
         qk.inc(sc_time(10, SC_NS));
         offset = qk.get_local_time();
         qk.set_and_sync(offset);
         #endif

         //SEND PREPROCESSED IMAGE - TROUGH DMA TO MLP
         //cout<<"SEND PREPROCESSED IMAGE TO MLP TROUGH DMA"<<endl;
         pl.set_address(0x83000000 + IMG_START + img*NUM_INPUTS);
         pl.set_data_length(NUM_INPUTS);
         pl.set_command(TLM_WRITE_COMMAND);
         s_tb_i0->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
         
         #ifdef QUANTUM
         qk.inc(sc_time(10, SC_NS));
         offset = qk.get_local_time();
         qk.set_and_sync(offset);
         #endif

         for (int layer=1; layer<=2; layer++)
         {
            for (int neuron=0; neuron<neuron_array[layer]; neuron++)
            {
               do
               {
                  #ifdef QUANTUM
                  qk.inc(sc_time(10, SC_NS));
                  offset = qk.get_local_time();
                  qk.set_and_sync(offset);
                  #else
                  offset += sc_time(10, SC_NS);
                  #endif
                  
                  tmp_sig=sig1.read();
               }
               while( tmp_sig == SC_LOGIC_0);

               //SEND APPROPRIATE WEIGHTS - TROUGH DMA TO MLP
              // if(neuron==0)cout<<"SEND APPROPRIATE WEIGHTS"<<endl;
	

               pl.set_address(0x83000000+layer_start_addr[layer-1]+neuron*(neuron_array[layer-1]+1));
               pl.set_data_length(neuron_array[layer-1]);
               pl.set_command(TLM_WRITE_COMMAND);
               s_tb_i0->b_transport(pl, offset);
               assert(pl.get_response_status() == TLM_OK_RESPONSE);
					
					
               #ifdef QUANTUM
               qk.inc(sc_time(10, SC_NS));
               offset = qk.get_local_time();
               qk.set_and_sync(offset);
               #else
               offset += sc_time(20, SC_NS);
               #endif
               
               do
               {
                  #ifdef QUANTUM
                  qk.inc(sc_time(5, SC_NS));
                  offset = qk.get_local_time();
                  qk.set_and_sync(offset);
                  #else
                  offset += sc_time(1, SC_NS);
                  #endif
                  
                  tmp_sig=sig1.read();
               }
               while( tmp_sig == SC_LOGIC_0);

               //SEND APPROPRIATE BIASES - TROUGH DMA TO MLP
              // if(neuron==0)cout<<"SEND APPROPRIATE BIASES"<<endl;
					
               pl.set_address(0x83000000+layer_start_addr[layer-1]+(neuron+1)*(neuron_array[layer-1]+1)-1);
               pl.set_data_length(1);
               pl.set_command(TLM_WRITE_COMMAND);
               s_tb_i0->b_transport(pl, offset);
               assert(pl.get_response_status() == TLM_OK_RESPONSE);

               #ifdef QUANTUM
               qk.inc(sc_time(20, SC_NS));
               offset = qk.get_local_time();
               qk.set_and_sync(offset);
               #else
               offset += sc_time(20, SC_NS);
               #endif
            }
            
         }
         //********* WAIT FOR MLP TO FINISH CLASSIFYING THIS IMAGE 
         do
         {
            #ifdef QUANTUM
            qk.inc(sc_time(10, SC_NS));
            offset = qk.get_local_time();
            qk.set_and_sync(offset);
            #else
            offset += sc_time(10, SC_NS);
            #endif
            
            tmp_sig=sig1.read();
         }
         while( tmp_sig == SC_LOGIC_0);

         //READ RESULTS FROM MLP
        // cout<<"READ RESULTS FROM MLP"<<endl;
         pl.set_address(0x82000000);
         pl.set_data_length(1);
         pl.set_command(TLM_READ_COMMAND);
         s_tb_i0->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
         num= ((num_t*)pl.get_data_ptr())[0];
         
         #ifdef QUANTUM
         qk.inc(sc_time(10, SC_NS));
         offset = qk.get_local_time();
         qk.set_and_sync(offset);
         #endif
         
         //REPORT CLASSIFICATION OF CURRENT IMG
         if(labels[img] == num)
         {
               cout<<"CORRECT CLASSIFICATION :: classified number "<<num<<" : "<<labels[img]<<" true_number";
               cout<<"         @"<<sc_time_stamp()<<"   #"<<name()<<"  ("<<img<<")"<<endl;
	       match++;
         }
         else
         {
               cout<<"     MISCLASSIFICATION :: classified number " <<num<<" : "<<labels[img]<<" true_number";
               cout<<"   X     @"<<sc_time_stamp()<<"   #"<<name()<<"  ("<<img<<")"<<endl;
         }
   }

   //REPORT FINAL RESULTS ON ALL IMAGES
   cout<<"Number of classifications : "<<lines<<endl;
   cout<<"Percentage: "<<(float)match/lines*100<<"%\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
   return;

}





int TB::num_of_lines(string str)
{
   int count = 0;
   string line;
   ifstream str_file(str);
   if(str_file.is_open())

   {
      while(getline(str_file,line))
         count++;
      str_file.close();
   }
   else
      cout<<"error opening str file in method num of lines"<<endl;
   return count;

}

void TB::labels_extraction()
{
   string  l_line;
   int lines;
   labels.clear();
   ifstream l_file("../saved_data/labels.txt");
   lines = num_of_lines("../saved_data/labels.txt");
      if(l_file.is_open())
      {
         for(int i=0; i<lines;i++)
         {
            getline(l_file,l_line,'\n');
            labels.push_back(stoi(l_line));
         }
      }
      else
      {
         cout<<"ERROR"<<" OPENING LABEL FILE"<<endl;
         cout<<"         @"<<sc_time_stamp()<<"   #"<<name()<<endl;
      }
   l_file.close();   
}
#endif
