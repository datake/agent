#include <iostream>
#include <string>
#include <fstream>
#include<stdlib.h>
using namespace std;

//プロトタイプ宣言
int read_data();
struct Dataset{
int input_first,input_second,input_result;

};



int main (){
	read_data();

}
int read_data() {
	
	struct Dataset data[100];
	//ifstreamでファイル読み込み
	cout << "ifstream" << endl;

	ifstream ifs("data.txt");
	string str;

	if(ifs.fail()) {
		cerr << "File do not exist.\n";
		exit(0);
	}


//	int input_first=0, input_second=0, input_result=0;
	int i=0;
	while(getline(ifs, str)) {
		if(i==100) exit(0);
		data[i].input_first=0; data[i].input_second=0; data[i].input_result=0;
		sscanf(str.data(), "%d %d %d", &data[i].input_first, &data[i].input_second, &data[i].input_result);
		cout<<i<<endl;
		cout << "first = " << data[i].input_first << endl;
		cout << "second = " << data[i].input_second << endl;
		cout << "result = " <<  data[i].input_result << endl;
		i++;
	}	
	return 0;
}
