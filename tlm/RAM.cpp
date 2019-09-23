#ifndef RAM_C
#define RAM_C
#include "RAM.hpp"

RAM::RAM(sc_module_name name):sc_module(name)
{
   s_ram_t0.register_b_transport(this, &RAM::b_transport);
   s_ram_t1.register_b_transport(this, &RAM::b_transport);
   cout<<name<<" constructed"<<endl;
   
   mem.reserve(2*SV_LEN); // SAMO NAMESTITI NORMALNO
}

void RAM::b_transport(pl_t& pl, sc_time& offset)
{
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned int len   = pl.get_data_length();
   unsigned char *buf = pl.get_data_ptr();
   unsigned int start = adr-0x81000000;
   if(start+len>2*SV_LEN)
   {
      cout<<RED<<"OUT OF RAM ADRESS LIMITS"<<RST<<endl;
      pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
      return;
   }
    
   switch (cmd)
   {
      case TLM_WRITE_COMMAND:
         //cout<<"RAM: WRITING"<<endl;
         for(int i=0; i<len; i++)
            mem[start+i]=(((din_t*)buf)[i]);

         pl.set_response_status( TLM_OK_RESPONSE );
         offset += sc_time(10, SC_NS);
         break;

      case TLM_READ_COMMAND:
         //cout<<"RAM: READING"<<endl;
         pl.set_data_ptr((unsigned char*)&mem[start]);
         pl.set_response_status( TLM_OK_RESPONSE );
         offset += sc_time(10, SC_NS);
         break;

      default:
         pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
   }
}


#endif
