#ifndef Image_preprocessor_C
#define Image_preprocessor_C
#include "Image_preprocessor.hpp"

//gaussian filter kernel 5x5, standard deviation 1
static const in_data_t filter[5][5] = {{0.0037,0.0146 ,0.0256 , 0.0146 ,0.0037 },
{0.0146 ,0.0586 ,0.0952 ,0.0586 ,0.0146 },
{0.0256 ,0.0952 ,0.1502 ,0.0952 ,0.0256 },
{0.0146 ,0.0586 ,0.0952 ,0.0586 ,0.0146 },
{0.0037,0.0146 ,0.0256 , 0.0146 ,0.0037 }};


Image_preprocessor::Image_preprocessor(sc_module_name name):sc_module(name)
{
   s_prep_t.register_b_transport(this, &Image_preprocessor::b_transport);
   cout<<name<<" constructed"<<endl;
   SC_THREAD(proc);
   toggle = SC_LOGIC_0;
   start = SC_LOGIC_0;
   image.reserve(NUM_INPUTS);
   
}

void Image_preprocessor::b_transport(pl_t& pl, sc_time& offset)
{
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned char *buf = pl.get_data_ptr();
   
   if(cmd==TLM_WRITE_COMMAND && adr==0x81000000)
   {
      start=SC_LOGIC_1; 
      pl.set_response_status(TLM_OK_RESPONSE);
   }
   else 
   {
      pl.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
   } 

}

void Image_preprocessor::proc()
{
   pl_t pl;
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned char *buf = pl.get_data_ptr();
   sc_time offset=SC_ZERO_TIME;
   #ifdef QUANTUM
   tlm_utils::tlm_quantumkeeper qk;
   qk.reset();
   #endif
   while(1)
   {
      //cout<<"ImagePreprocessor: WAITING FOR START"<<endl;
      while(start==SC_LOGIC_0)
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

      image.clear();
      //cout<<"ImagePreprocessor: READ IMAGE FROM BRAM"<<endl;
      //READ IMAGE FROM BRAM 
      pl.set_command(TLM_READ_COMMAND);
      pl.set_address(0x80000000);
      pl.set_data_length(NUM_INPUTS);
      s_prep_i -> b_transport(pl, offset);
      assert(pl.get_response_status() == TLM_OK_RESPONSE);

      #ifdef QUANTUM
      qk.set_and_sync(offset);
      #endif

      //cout<<"PREP: Image_preprocessING"<<endl;
      //Image_preprocess IT
      buf = pl.get_data_ptr();
      for(int i=0; i<NUM_INPUTS; i++)
         image.push_back(((in_data_t*)buf)[i]);
      image = Image_preprocessing(image);
      
      #ifdef QUANTUM
      qk.inc(sc_time(10, SC_NS));
      offset = qk.get_local_time();
      qk.set_and_sync(offset);
      #else
      offset += sc_time(10, SC_NS);
      #endif
      
      //cout<<"ImagePreprocessor: WRITE TO BRAM"<<endl;
      //WRITE NEW IMAGE TO BRAM
      pl.set_data_ptr((unsigned char *)&image[0]);
      pl.set_command(TLM_WRITE_COMMAND);
      pl.set_address(0x80000000+NUM_INPUTS);
      pl.set_data_length(NUM_INPUTS);

      s_prep_i -> b_transport(pl, offset);
      assert(pl.get_response_status() == TLM_OK_RESPONSE);

      //cout<<"ImagePreprocessor: SET TOGGLE"<<endl;
      toggle = SC_LOGIC_1; 
      p_out->write(toggle);//finished, send interrupt
      
      #ifdef QUANTUM
      qk.set_and_sync(offset);
      #endif
      
      //cout<<"ImagePreprocessor: RESET TOGGLE"<<endl;
      toggle = SC_LOGIC_0; 
      p_out->write(toggle);//finished, send interrupt

   }
}

vector<in_data_t> Image_preprocessor:: Image_preprocessing(vector <in_data_t> image)
{
   
   vector <acc_t> new_image;
   vector <in_data_t> final_image;
   new_image.reserve(576); //576=24*24
   final_image.reserve(784);
   new_image.clear();
   final_image.clear();
	for (int x=0; x<28; x++)
      for(int y = 0; y<28; y++)
         {
            new_image.push_back(0.0);
            final_image.push_back(0.0);
         }

    int height = 28;
    int width = 28;
    int filterHeight = 5;
    int filterWidth = 5;
    int newImageHeight = height-filterHeight+1;
    int newImageWidth = width-filterWidth+1;
    int i,j,h,w;

        for (i=0 ; i<newImageHeight ; i++) {
            for (j=0 ; j<newImageWidth ; j++) {
                for (h=i ; h<i+filterHeight ; h++) {
                    for (w=j ; w<j+filterWidth ; w++) {
                        new_image[i*newImageHeight+j] += filter[h-i][w-j]*image[h*height+w];
                    }
                }
            }
        }
        
        for(int x=0; x<height; x++) {
                for(int y=0; y<height; y++){
                	if( (x<2 || x>25) && (y<2 || y>25))
                		final_image[x*height+y]=0.0;
                	else
                		final_image[x*height+y]=new_image[(x-2)*newImageHeight+y-2];
                }
       }
   return final_image;
}

#endif
