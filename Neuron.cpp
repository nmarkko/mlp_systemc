#include "Neuron.hpp"



Neuron::Neuron(sc_module_name name, deque<double> input, deque<double> weights, double bias, int result): 
input(input), weights(weights), bias(bias), result(result)
{
   cout<<name<<" constucted"<<endl;
   SC_THREAD(proc);
	SC_METHOD(bye_felicia);
	dont_initialize();
	sensitive << finish;
}

void Neuron::proc()
{
	double acc=0.0;
   while(true)
   {
	wait(5, SC_SEC);
	//next->notify(SC_ZERO_TIME);
	//wait(*ready);
	for (int i=0; i!=input.size(); i++) {
		acc += weights[i]*input[i];
	 }
	acc += bias;
	
	if (acc<0)
		acc *= 0.001;
	else
		acc *= 1;

	result=acc;
	
	finish.notify(SC_ZERO_TIME);
	wait(15, SC_SEC);
	
      /* y.clear();
      for(int i=0;i<sv_len;i++)
         y.push_back(data[i]);

      while(k)
         {
            e_next->notify(SC_ZERO_TIME);
            wait(*e_ready);
            p=1.0;
            for(int i=0; i<sv_len; i++)
               p+=y[i].to_double()*data[i].to_double();


            p*=0.1;
            P_CHECK_OVERFLOW<<" mult 0.1"<<endl;

            p=p*p*p;
            P_CHECK_OVERFLOW<<" cube "<<endl;


            p=p*lambda.to_double();
            P_CHECK_OVERFLOW<<" lambda*p"<<endl;


            wait(1,SC_NS);
            
            //cout<<"\tcurrent acc="<<acc<<"\tp="<<p<<"\tnew acc="<<(acc+p);
            //cout<<"\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
            

            acc+=p.to_double();
            A_CHECK_OVERFLOW<<" acc+=p"<<endl;
            k--;
         }
      acc+=b.to_double();
      A_CHECK_OVERFLOW<<"acc+=b"<<endl;
      res=acc;

      R_CHECK_OVERFLOW<<"res=acc"<<endl;
      e_fin->notify(SC_ZERO_TIME);*/
   }
   return;	
}

/*double Neuron::get_res()
{
   return res;
}*/

void Neuron::bye_felicia() {
	cout << "Izlaz neurona je " << result << endl;
}

