#include "Checker.hpp"
Checker::Checker(sc_module_name name,  sc_event *e_fin,  num_t &number): sc_module(name),
											e_fin(e_fin),
                                             				number(number)
{
   cout<<"Checker constucted"<<endl;
   SC_THREAD(verify);
}

void Checker::verify()
{
   int num=0;
   int true_number;
   int num_of_cl=0;
   int correct_cl=0;
   double percentage;
   int num_of_images=num_of_lines("../saved_data/labels.txt");
   
   string l_line;
   ifstream l_file("../saved_data/labels.txt");
   if(l_file.is_open())
      while(num_of_images)
      {
            wait(*e_fin);
            getline(l_file, l_line);
            true_number = stoi(l_line);
            if(true_number == number.to_int())
            {
               correct_cl++;
               
               cout<<"CORRECT CLASSIFICATION :: classified number "<<number.to_int()<<" : "<<true_number<<" true_number";
               cout<<"         @"<<sc_time_stamp()<<"   #"<<name()<<endl;
            }
            else
            {              
                cout<<"     MISCLASSIFICATION :: classified number "<<number.to_int()<<" : "<<true_number<<" true_number";
                cout<<"   X     @"<<sc_time_stamp()<<"   #"<<name()<<endl;
            }
            
            num_of_images--;
            num_of_cl++; 
      }
   else
      cout<<"ERROR OPENING RES/LABEL FILE"<<endl;

   percentage=(double)correct_cl/(double)num_of_cl;
   cout<<"number of classifications : "<<num_of_cl<<"\tpercentage: "<<100*percentage<<"%\t"<<"@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
   l_file.close();
   return;	
}

int Checker::num_of_lines(string str)
{
   int count = 0;
   string line;
   ifstream label_file(str);
   if(label_file.is_open())

   {
      while(getline(label_file,line))
         count++;
      label_file.close();
   }
   else
   {

      cout<<"ERROR OPENING FILE"<<endl;
      cout<<"         @"<<sc_time_stamp()<<"   #"<<name()<<endl;
   }
   return count;
}
