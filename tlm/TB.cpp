#ifndef TB_C
#define TB_C
#include "TB.hpp"

#define LAYER1 0
#define LAYER2 SV_LEN*30 + 30
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
   din_t *image;
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
   cout<<BKG_BLUE<<BLACK<<"***** STARTING CLASSIFICATION *****"<<BKG_RST<<RST<<endl;
   cout<<BLUE<<"  -testing on "<<lines<<" images images-  \n"RST<<endl;
   
   for(int img=0; img<lines; img++)
   {
         //READ NEW IMAGE FROM DDR 
         //cout<<"READ NEW IMAGE FROM DDR"<<endl;
         addr = (IMG_START + img*SV_LEN);
         pl.set_address(addr);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_READ_COMMAND);
         s_tb_i1->b_transport(pl, offset);
         buf = pl.get_data_ptr();
         image = ((din_t*)buf);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif
         
         //WRITE NEW IMAGE TO RAM
         //cout<<"WRITE NEW IMAGE TO RAM"<<endl;
         pl.set_data_ptr((unsigned char *)image);
         pl.set_address(0x81000000);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_WRITE_COMMAND);
         s_tb_i0->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif
         
         //START PREP MODULE 
         //cout<<"DUMMY FOR PREP"<<endl;
         pl.set_address(0x82000000);
         pl.set_command(TLM_WRITE_COMMAND);
         s_tb_i0->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
         
         #ifdef QUANTUM
         qk.inc(sc_time(10, SC_NS));
         offset = qk.get_local_time();
         qk.set_and_sync(offset);
         #endif
         
         //************** WAIT FOR PREP TO FINISH PROCESSING IMAGE    
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

         //READ PREPROCESSED IMAGE FROM RAM
         //cout<<"READ PREPROCESSED IMAGE FROM RAM"<<endl;
         pl.set_address(0x81000000+SV_LEN);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_READ_COMMAND);
         s_tb_i0->b_transport(pl, offset);
         buf = pl.get_data_ptr();
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif
         
         //WRITE NEW IMAGE TO DDR 
         //cout<<"WRITE NEW IMAGE TO DDR"<<endl;
         addr = (IMG_START + img*SV_LEN);
         pl.set_address(addr);
         pl.set_data_ptr(buf);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_WRITE_COMMAND);
         s_tb_i1->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif

         //START MLP
         //cout<<"START MLP"<<endl;
         pl.set_address(0x80000000);
         pl.set_data_length(SV_LEN);
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
         pl.set_address(0x83000000 + IMG_START + img*SV_LEN);
         pl.set_data_length(SV_LEN);
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
               //if(neuron==0)cout<<"SEND APPROPRIATE WEIGHTS"<<endl;
               pl.set_address(0x83000000+layer_start_addr[layer]+neuron*neuron_array[layer-1]);
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
               //if(neuron==0)cout<<"SEND APPROPRIATE BIASES"<<endl;
               pl.set_address(0x83000000+layer_start_addr[layer]+neuron_array[layer]*neuron_array[layer-1]+neuron);
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
               #endif*/
            }
            
            /*do
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
            while( tmp_sig == SC_LOGIC_0);*/


            #ifdef QUANTUM
            qk.inc(sc_time(20, SC_NS));
            offset = qk.get_local_time();
            qk.set_and_sync(offset);
            #else
            offset += sc_time(20, SC_NS);
            #endif
            
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
         //cout<<"READ RESULTS FROM MLP"<<endl;
         pl.set_address(0x80000000);
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
               cout<<B_GREEN<<"CORRECT CLASSIFICATION"<<RST<<D_GREEN<<" :: classified number: "
                  <<num<<"["<<labels[img]<<"] :true_number"<<RST;
               cout<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<"  ("<<img<<")"<<RST<<endl;
            match++;
         }
         else
         {
               cout<<B_RED<<"     MISCLASSIFICATION"<<RST<<D_RED<<" :: classified number: "
                  <<num<<"["<<labels[img]<<"] :true_number"<<RST;
               cout<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<"  ("<<img<<")"<<RST<<endl;
         }
   }

   //REPORT FINAL RESULTS ON ALL IMAGES
   cout<<"Number of classifications : "<<lines<<D_MAGNETA<<"\nPercentage: "<<B_MAGNETA
    <<(float)match/lines*100<<"%\t"<<RST<<DIM<<"@"<<sc_time_stamp()<<"\t#"<<name()<<RST<<endl;
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
         cout<<BKG_RED<<"ERROR"<<BKG_RST<<RED<<" OPENING LABEL FILE"<<endl;
         cout<<RST<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
      }
   l_file.close();   
}
#endif
