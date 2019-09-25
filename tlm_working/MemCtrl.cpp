#ifndef MEMCTRL_C
#define MEMCTRL_C
#include "MemCtrl.hpp"

MemCtrl::MemCtrl(sc_module_name name): sc_module(name),
                                       buffer(NULL)
{
   
   s_mc_t0.register_b_transport(this, &MemCtrl::b_transport);
   s_mc_t1.register_b_transport(this, &MemCtrl::b_transport);
   SC_THREAD(memory_init);
   
   cout<<name<<" constructed"<<endl;
   
	//num_weights_l1 + num_biases_l1 + num_weights_l2 + num_biases_l2 + 10*image_length
   mem.reserve(784*30+30+30*10+10+10*784);

}

void MemCtrl::memory_init()
{
   vector<in_data_t> test_image;
   test_image.reserve(784);
   uint64 addr;

   file_extract();
   return;
}
void MemCtrl::b_transport(pl_t& pl, sc_time& offset)
{
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned char *buf = pl.get_data_ptr();
   unsigned int len   = pl.get_data_length();

   switch(cmd)
      {
      case TLM_WRITE_COMMAND:
         for(int i=0; i<len; i++)
            mem[adr+i]=((in_data_t*)buf)[i];
         pl.set_response_status(TLM_OK_RESPONSE);
         break;
         
      case TLM_READ_COMMAND:
         buf = (unsigned char*)&mem[adr];
         pl.set_data_ptr(buf);
         pl.set_response_status(TLM_OK_RESPONSE);
         break;
      default:
         pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
      }

   offset += sc_time(10, SC_NS);
}


void MemCtrl::file_extract()
{
   
   double weights_line;
   string biases_line;
   string str;
   int lines;
   int j, k=0;
   int len [2] = {784, 30};
   int sum = 0;
   cout<<"\nExtracting data from files:"<<endl;
   
   for(int i=0; i<2; i++)
      {
			j=0;
         //extracting weights of i-th layer
         str = "../saved_data/weights_";
         str = str + to_string(i+1);
         str = str+".txt";
         lines = num_of_lines(str);
         num_of_neurons[i] = lines/2;
         //sum += lines;
         cout<<"-layer no."<< i+1 <<": "<< num_of_neurons[i] <<" neurons"<<endl;
         ifstream weights_file(str);
         if(weights_file.is_open())
            while(!weights_file.eof())
               {
						
						weights_file >> weights_line;
						mem.push_back(weights_line);
               }
         else
            cout<<"ERROR OPENING WEIGHTS_FILE number: "<<i<<endl;
         weights_file.close();
			

         /*j = 0;
         //extracting biases
         str = "../saved_data/biases_";
         str = str + to_string(i+1);
         str = str+".txt";
         ifstream biases_file(str);

         if(biases_file.is_open() )
            while(j != lines)
               {
                  //extracting biases
                  getline(biases_file,biases_line);   
                  mem.push_back(stod(biases_line));
                  j++;
               }
         else
            cout<<RED<<"ERROR OPENING BIASES_FILE number: "<<i<<RST<<endl;
        
         biases_file.close();*/
      }
   images_extraction();
   
}

int MemCtrl::num_of_lines(string str)
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

/*int MemCtrl::sum_of_sv(int to_element)
{
   if(to_element == -1)
      return 0;
   int sum = 0;
   for(int i=0;i<=to_element;i++ )
      sum += sv_array[i];
   
   return sum;
}*/


void MemCtrl::images_extraction()
{
   int k =0;
   string  y_line;
   int lines;
   
   lines = num_of_lines("../saved_data/y.txt");
   ifstream y_file("../saved_data/y.txt");
   
   
   if(y_file.is_open())
      {
         for(int i=0; i<784*lines;i++)
            {
               if(k == 784-1 )
                  {
                     getline(y_file, y_line, '\n');
                     k = 0;
                  }
               else
                  {
                     getline(y_file, y_line, ' ');
                     k++;
                  }
               mem.push_back(stod(y_line));
            }
      }
   else
      {
         cout<<RED<<"ERROR OPENING Y FILE"<<endl;
         cout<<RED<<"         @"<<sc_time_stamp()<<"   #"<<name()<<endl;
      }
   y_file.close();   
}


#endif

         
