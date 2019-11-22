#ifndef MEMCTRL_C
#define MEMCTRL_C
#include "MemCtrl.hpp"

MemCtrl::MemCtrl(sc_module_name name,
		 sc_event *e_mlp_start, sc_event *e_ready, sc_event *e_done, deque<din_t>& weights,
		 bin_t& bias, deque<din_t>& data):sc_module(name),
							e_mlp_start(e_mlp_start),
                                          e_ready(e_ready),
                                          e_done(e_done),
                                          weights(weights),
                                          bias(bias),
                                          data(data)                                                          
{

   SC_THREAD(grab_from_mem);
   cout<<"Memory controller constructed"<<endl;
}

void MemCtrl::grab_from_mem()
{
   int image_num = 0;
   int sv_count=0;
   int num_of_img;
   int j = 0,k = 0;
   //deque <din_t> y_deq;
   din_t y;
   file_extract();
   num_of_img = num_of_lines("../saved_data/y.txt");
   
   while(image_num<num_of_img)
   {

		wait(*e_mlp_start);
/*		data.clear();
		y_deq.clear();
		for(int i=0; i<NUM_INPUTS; i++)
		{
			y = images[j];
			y_deq.push_back(y);
			j++;
		}
		data=y_deq;
*/		  
		  
		data.clear();
		for(int i=0; i<NUM_INPUTS; i++)
		{
			y = images[j];
			data.push_back(y);
			j++;
		}
		
		int k=0; //for each new image start sending neural parameters
		for(int layer=0; layer<2; layer++)
		{
			for(int neuron=0; neuron< neuron_array[layer+1]; neuron++)
				{
					weights.clear();
					for(int syn=0; syn < neuron_array[layer]; syn++)
					{
						weights.push_back(mem[k]);
						k++; //1 more parameter storaged
					}
					bias = mem[k++]; //1 more parameter storaged
					e_ready[neuron].notify(SC_ZERO_TIME); //neuron parameters are storaged & ready
					wait(e_done[neuron]); //wait for neuron to finish calculations
				}
		}
		
		image_num ++;
    }

}

void MemCtrl::file_extract()
{
   string y_line; //image pixel
   string str;
   int lines;
   lines=num_of_lines("../saved_data/y.txt");
   cout<<"num of images: "<<lines<<endl;
   ifstream y_file("../saved_data/y.txt");
   mem.clear();
   
   int k=0;
   
   //extracting test images
   if(y_file.is_open())
      for(int i=0; i < NUM_INPUTS * lines; i++)
      {
         if(k == NUM_INPUTS-1 )
         {
            getline(y_file, y_line, '\n');
            k = 0;
         }
         else
         {
            getline(y_file, y_line, ' ');
            k++;
         }
         images.push_back(stod(y_line));
      }
   else
      cout<<"ERROR OPENING Y_FILE"<<endl;
   
   y_file.close();
   
   string weights_line;
   string biases_line;
   string str_w;
   string str_b;
   cout<<"\nExtracting data from files:"<<endl;
   
   for(int i=0; i<2; i++)
      {
		 str_w = "../saved_data/weights_";
         str_w = str_w + to_string(i+1);
         str_w = str_w+".txt";
         ifstream weights_file(str_w);
         
         str_b = "../saved_data/biases_";
         str_b = str_b + to_string(i+1);
         str_b = str_b+".txt";
         ifstream biases_file(str_b);
         
         cout<<"-layer no."<< i+1 <<": "<< neuron_array[i+1] <<" neurons"<<endl;
         
	  if(weights_file.is_open() && biases_file.is_open())
	      {
		for(int p=0; p< neuron_array[i+1]; p++)
		    {

		    		for(int r=0; r < neuron_array[i]; r++)
		    		{
		       			if(r == neuron_array[i] - 1)
					     getline(weights_file, weights_line, '\n');
				        else
					     getline(weights_file, weights_line, ' ');
		      			mem.push_back(stod(weights_line));
				}			
		      		getline(biases_file, biases_line, '\n');
		       		mem.push_back(stod(biases_line));
		    }
	      }
	     
	   else
	      {
		 cout<<"ERROR OPENING WEIGHTS_FILE OR BIASES_FILE"<<endl;
		 cout<<"         @"<<sc_time_stamp()<<"   #"<<name()<<endl;
	      }
	   weights_file.close();
	   biases_file.close();		
      }
   
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
#endif
