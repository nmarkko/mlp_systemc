#define X 30
#define Y 784

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

int main() {
	fstream input_data;
	input_data.open("y_new.txt", fstream::out);
	double a;

	ifstream f("y.txt");	

	for (int i = 0; i < 10000; i++) {
		for (int j = 0; j < 784; j++) {
			f >> a;
			//cout << A[i][j];
			input_data << a;
			if(j!=784-1) input_data <<" ";
			else input_data <<"\n";
	
		}
	}
	f.close();
	input_data.close();
	
}
