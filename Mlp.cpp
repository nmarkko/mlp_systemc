#include "Mlp.hpp"



Mlp::Mlp(sc_module_name name, double** weights_1, double** weights_2, double* biases_1, double* biases_2, double* slika): weights_1(weights_1), weights_2(weights_2), biases_1(biases_1), biases_2(biases_2), slika(slika)
{
   cout<<name<<"Mlp constucted"<<endl;
   SC_THREAD(proc);
}

void Mlp::proc()
{
	deque <double> tezina_lanaca_1;
	deque <double> tezina_lanaca_2;
	Neuron *layer1;
	Neuron *layer2;

   while(true)
   {
		for (int i = 0; i<30; i++) {
			for(int j=0; j<784;j++) {
				tezina_lanaca_1.push_back(weights_1(i,j));
			}
			*(layer1+i) =new Neuron("fuzzy0", slika, tezina_lanaca_1, biases_1[i], results_1[i]);
		}
		
		for (int i = 0; i<10; i++) {
			for(int j=0; j<30;j++) {
				tezina_lanaca_2.push_back(weights_2(i,j));
			}
			*(layer2+i) =new Neuron("fuzzy1", results_1[j], tezina_lanaca_2, biases_2[i], results_2[i]);
		}
	finish.notify(SC_ZERO_TIME);
	delete layer1;
delete layer2;
	wait(20, SC_SEC);
   }
   return;	
}

/*double Neuron::get_res()
{
   return res;
}*/

void Neuron::bye_felicia() {
	cout << "Izlaz mlp-a je " << endl;
	for (int i = 0; i<10; i++) {
			 cout << results_2[i] << endl;
		}
}

