#include "Mlp.hpp"
#include <string>



Mlp::Mlp(sc_module_name name, double weights_1_new[][784], double weights_2_new[][30], double biases_1_new[], double biases_2_new[], double* slika): slika(slika)
{
   cout<<name<<"Mlp constucted"<<endl;


      for (int i = 0; i<30; i++) {
         biases_1[i]=biases_1_new[i];
      }
      for (int i = 0; i<10; i++) {
            biases_2[i]=biases_2_new[i];
		}

      for (int i = 0; i<30; i++) {
			for(int j=0; j<784;j++) {
            weights_1[i][j]=weights_1_new[i][j];
			}
		}
       for (int i = 0; i<10; i++) {
			for(int j=0; j<30;j++) {
            weights_2[i][j]=weights_2_new[i][j];
			}

		}



		deque <double> tezina_lanaca_1;
		deque <double> tezina_lanaca_2;

	 for (int i = 0; i<30; i++) {
			for(int j=0; j<784;j++) {
				tezina_lanaca_1.push_back(weights_1[i][j]);
			}
         string str1("Neuron_0_");
         string num1=to_string(i);
         str1=str1+num1;
			layer1[i] = new Neuron(str1.c_str(), &ready1, slika, 784, tezina_lanaca_1, biases_1[i], &results_1[i]);
			tezina_lanaca_1.clear();
		}


      for (int i = 0; i<10; i++) {
			for(int j=0; j<30;j++) {
				tezina_lanaca_2.push_back(weights_2[i][j]);
			}
			string str2("Neuron_1_");
         string num2=to_string(i);
         str2=str2+num2;
			layer2[i] = new Neuron(str2.c_str(), &ready2, results_1, 30, tezina_lanaca_2, biases_2[i], &results_2[i]);
			tezina_lanaca_2.clear();
		}



      //---------------------
	//deque <double> hidden_results;

//      for (int i = 0; i<30; i++) {
//         hidden_results.push_back(results_1[i]);
//      }



   SC_THREAD(proc);
   SC_THREAD(show_output);
   dont_initialize();
	sensitive << finish;
}

void Mlp::proc()
{

   while(true)
   {
   //let the hidden layer start the computation
   ready1.notify();
   wait(5,SC_SEC);
   //let the output layer start the computation
   ready2.notify();
	finish.notify(SC_ZERO_TIME);
	wait(15, SC_SEC);
   }
   return;
}

void Mlp::show_output()
{
   std::cout << "Izlazi su " ;
   for(int i=0; i<10; i++) {
         std::cout << results_2[i]<< " " ;
   }
   std::cout<<std::endl;
   //print out index of a max element of array results_2
   //that is the recognized digit
   double max=results_2[0];
   int index_max=0;
   for(int i=1; i<10; i++) {
      if (results_2[i]>max) {
         max=results_2[i];
         index_max=i;
      }
   }
   std::cout << "Prepoznat je broj " << index_max << std::endl;
}

