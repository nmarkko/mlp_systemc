#include "Checker.hpp"
Checker::Checker(sc_module_name name,  sc_event *e_fin,  num_t &number): 
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
            //wait(1,SC_NS);
            getline(l_file, l_line);
            true_number = stoi(l_line);
            if(true_number == number.to_int())
            {
               correct_cl++;
               cout<<B_GREEN<<"CORRECT CLASSIFICATION"<<RST<<D_GREEN<<" :: classified number: "
                  <<number.to_int()<<"["<<true_number<<"] :true_number"<<RST;
               cout<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
            }
            else
            {
               cout<<B_RED<<"     MISCLASSIFICATION"<<RST<<D_RED<<" :: classified number: "
                  <<number.to_int()<<"["<<true_number<<"] :true_number"<<RST;
               cout<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
            }
            
            num_of_images--;
            num_of_cl++; 
      }
   else
      cout<<BKG_RED<<BLACK<<"ERROR"<<BKG_RST<<D_RED<<" OPENING RES/LABEL FILE"<<RST<<endl;

   percentage=(double)correct_cl/(double)num_of_cl;
   cout<<"number of classifications : "<<num_of_cl<<D_MAGNETA<<"\tpercentage: "<<B_MAGNETA<<100*percentage<<"%\t"<<RST<<DIM<<"@"<<sc_time_stamp()<<"\t#"<<name()<<RST<<endl;
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

      cout<<BKG_RED<<"ERROR"<<BKG_RST<<RED<<" OPENING FILE"<<endl;
      cout<<RST<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
   }
   return count;
}
