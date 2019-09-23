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
	input_data.open("input_data.hpp", fstream::out);
	double A[X][Y];
	double B[30];

	ifstream f("weights_1.txt");	
	input_data << "weights_1[30][784] = {";

	for (int i = 0; i < X; i++) {
	input_data << "{ ";
		for (int j = 0; j < Y; j++) {
			f >> A[i][j];
			cout << A[i][j];
			input_data << A[i][j];
			if(j!=Y-1) input_data <<", ";	
	
		}
		input_data << "}";
		if(i!=X-1) input_data <<", ";
		cout << endl;
	}
	input_data << "}" << endl;
	f.close();

//------------------------------------------
	f.open("weights_2.txt");	
	input_data << "weights_2[10][30] = {";

	for (int i = 0; i < 10; i++) {
	input_data << "{ ";
		for (int j = 0; j < 30; j++) {
			f >> A[i][j];
			cout << A[i][j];
			input_data << A[i][j];
			if(j!=30-1) input_data <<", ";	
	
		}
		input_data << "}";
		if(i!=10-1) input_data <<", ";
		cout << endl;
	}
	input_data << "}" << endl;
	f.close();
//-------------------------------------

	f.open("biases_1.txt");	
	input_data << "biases_1[30] = {";

	for (int i = 0; i < 30; i++) {
			f >> B[i];
			cout << B[i];
			input_data << B[i];
			if(i!=30-1) input_data <<", ";	
	}
	input_data << "}" << endl;
	f.close();
//-------------------------------------
	f.open("biases_2.txt");	
	input_data << "biases_2[10] = {";

	for (int i = 0; i < 10; i++) {
			f >> B[i];
			cout << B[i];
			input_data << B[i];
			if(i!=10-1) input_data <<", ";	
	}
	input_data << "}" << endl;
	f.close();



	input_data.close();
	
}
