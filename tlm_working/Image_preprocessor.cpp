#ifndef Image_preprocessor_C
#define Image_preprocessor_C
#include "Image_preprocessor.hpp"

Image_preprocessor::Image_preprocessor(sc_module_name name):sc_module(name)
{
   s_prep_t.register_b_transport(this, &Image_preprocessor::b_transport);
   cout<<name<<" constructed"<<endl;
   SC_THREAD(proc);
   toggle = SC_LOGIC_0;
   start = SC_LOGIC_0;
   image.reserve(SV_LEN);
   
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
      //cout<<"DSKW: WAITING FOR START"<<endl;
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
      //cout<<"DSKW: READ IMAGE FROM BRAM"<<endl;
      //READ IMAGE FROM BRAM 
      pl.set_command(TLM_READ_COMMAND);
      pl.set_address(0x80000000);
      pl.set_data_length(SV_LEN);
      s_prep_i -> b_transport(pl, offset);
      assert(pl.get_response_status() == TLM_OK_RESPONSE);

      #ifdef QUANTUM
      qk.set_and_sync(offset);
      #endif

      //cout<<"PREP: Image_preprocessING"<<endl;
      //Image_preprocess IT
      buf = pl.get_data_ptr();
      for(int i=0; i<SV_LEN; i++)
         image.push_back(((in_data_t*)buf)[i]);
      image = Image_preprocessing(image);
      
      #ifdef QUANTUM
      qk.inc(sc_time(10, SC_NS));
      offset = qk.get_local_time();
      qk.set_and_sync(offset);
      #else
      offset += sc_time(10, SC_NS);
      #endif
      
      //cout<<"DSKW: WRITE TO BRAM"<<endl;
      //WRITE NEW IMAGE TO BRAM
      pl.set_data_ptr((unsigned char *)&image[0]);
      pl.set_command(TLM_WRITE_COMMAND);
      pl.set_address(0x80000000+SV_LEN);
      pl.set_data_length(SV_LEN);

      s_prep_i -> b_transport(pl, offset);
      assert(pl.get_response_status() == TLM_OK_RESPONSE);

      //cout<<"DSKW: SET TOGGLE"<<endl;
      toggle = SC_LOGIC_1; 
      p_out->write(toggle);//finished, send interrupt
      
      #ifdef QUANTUM
      qk.set_and_sync(offset);
      #endif
      
      //cout<<"DSKW: RESET TOGGLE"<<endl;
      toggle = SC_LOGIC_0; 
      p_out->write(toggle);//finished, send interrupt

   }
}





vector<in_data_t> Image_preprocessor:: Image_preprocessing(vector <in_data_t> image)
{
   
   vector <in_data_t> new_image;
   new_image.reserve(784);
	p_t x_center, y_center;

   calc_center(image, x_center, y_center);
   //p_t x_diff = 10 - x_center;
	//p_t y_diff = 10 - y_center;
	//TRIAL VERSION - SUBJECT TO CHANGE
	p_t x_diff = 14 - x_center;
	p_t y_diff = 14 - y_center;
   
   
   for (int x=0; x<28; x++)
      for(int y = 0; y<28; y++)
         {
            new_image.push_back(0.0);
         }
	
	p_t x_new_coord=0, y_new_coord=0;

   /*for (int x=0; x<20; x++)
      for(int y = 0; y<20; y++)
         {
				x_new_coord = x+4+x_diff;
				y_new_coord = y+4+y_diff;

            if( (x_new_coord >= 0) && (x_new_coord < 28) )
					if( (y_new_coord >= 0) && (y_new_coord < 28) )
						new_image(28*x_new_coord + y_new_coord) = image(28*x+y);

         }*/

		//TRIAL VERSION - SUBJECT TO CHANGE
		for (int x=0; x<28; x++)
      for(int y = 0; y<28; y++)
         {
						new_image[28*x + y] = image[28*x+y];

         }
   
   //return new_image;
	return image;
}


void Image_preprocessor::calc_center(vector<in_data_t> image, p_t& x_CenterOfMass, p_t& y_CenterOfMass)
{
   double sum_intensity = 0;
   double sum_x, sum_y;
   
   for (int x=0; x<28; x++)
      for(int y = 0; y<28; y++)
         {
            sum_intensity+=image[28*x+y];
            sum_x+=x*image[28*x+y];
            sum_y+=y*image[28*x+y];
         }
  //adding 0.5 causes it to do proper rounding.
  x_CenterOfMass = (p_t)(0.5+sum_x/sum_intensity);
  y_CenterOfMass = (p_t)(0.5+sum_y/sum_intensity); 
   
   
}


#endif
