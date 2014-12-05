#include <iostream>
#include <sstream>
#include <string>
#include "QuadProg++.hh"
#define DATA_NUM 5

//入力された値を読み込む
struct Input_data{
double input_first ;
double input_second;
double input_result;
};
int main (int argc, char *const argv[]) {
	double G[MATRIX_DIM][MATRIX_DIM], g0[MATRIX_DIM], 
		CE[MATRIX_DIM][MATRIX_DIM], ce0[MATRIX_DIM], 
		CI[MATRIX_DIM][MATRIX_DIM], ci0[MATRIX_DIM], 
		x[MATRIX_DIM];
	int n, m, p;	
	int result;	
	double sum = 0.0;
	char ch;
	int i=0,j;
	
	//入力した値をとる
	//DATA_NUM分のデータセットをうけとる
	struct Input_data input_data[DATA_NUM] ;
	
	//コマンドラインで-aを指定
/*
	 GetOpt getopt (argc, argv, "dcs:");
	  int option_char;
	 while ((option_char = getopt ()) != EOF)
    	switch (option_char)
	      {  
		 case 'a': cout<<"aaaa" break;
		 case 'c': compile_flag = 1; break;
		 case 's': size_in_bytes = atoi (getopt.optarg); break;
		 case '?': fprintf (stderr, 
				    "usage: %s [dcs<size>]\n", argv[0]);
	      }
*/
/*
	 while((result=getopt(argc,argv,"a:")!=-1){
    		switch(result){
		
			case 'a':
			cout <<result;
			
			for (int i = 0; i < DATA_NUM; i++){		
				 cout <<input_data[i].input_first   << "," 
				 << input_data[i].input_second   << "," 
				 << input_data[i].input_result  << endl;
				}
			break;
		}
	}*/
  

		while (std::cin >> input_data[i].input_first >> input_data[i].input_second >> input_data[i].input_result ){
           		 i++;
        	}
		for (int j = 0; j < i; j++){		
				 std::out <<input_data[j].input_first   << "," 
				 << input_data[j].input_second   << "," 
				 << input_data[j].input_result  << std::endl;
				}


//まだG変更してない
  n = DATA_NUM;
  {
	
		std::istringstream is("4, -2,"
													"-2, 4 ");

		for (int i = 0; i < n; i++)	
			for (int j = 0; j < n; j++)
				is >> G[i][j] >> ch;
	}
	
  {//g0の要素は全て-1
//"-1.0 -1.0 .."という文字列の読み込み(sscanfにあたる)
		std::istringstream is("-1.0, -1.0, -1.0, -1.0, -1.0 ");

		for (int i = 0; i < n; i++)
//文字列を展開
			is >> g0[i] >> ch;
	}
  
  m = 1;
	{//CEは(y1 y2 .. yn)
		std::istringstream is("y[0]" "y[1] ");
		for (int i = 0; i < n; i++)
			for (int j = 0; j < m; j++)
				is >> CE[i][j] >> ch;
	} 
  
	{//ce0は0
		std::istringstream is("0.0 ");
		
		for (int j = 0; j < m; j++)
			is >> ce0[j] >> ch;
  }
	
	p = 3;
  {
/*CIは
(1 0 0 0 0 1)
(0 1 0 0 0 1)
(0 0 1 0 0 1)
(0 0 0 1 0 1)
(0 0 0 0 1 1)
*/
		std::istringstream is("1.0, 0.0, 0.0,0.0,0.0,1.0 " 
		"0.0, 1.0, 0.0,0.0,0.0,1.0 " "0.0, 0.0, 1.0,0.0,0.0,1.0 "
		 "0.0, 0.0, 0.0,1.0,0.0,1.0 "); 
		for (int i = 0; i < n; i++)
			for (int j = 0; j < p; j++)
				is >> CI[i][j] >> ch;
	}
  
  {
//ci0は(0 0 0 0 0 0)
		std::istringstream is("0.0, 0.0, 0.0,0.0,0.0,0.0 ");

		for (int j = 0; j < p; j++)
			is >> ci0[j] >> ch;
	}

  std::cout << "f: " << solve_quadprog(G, g0, n, CE, ce0, m, CI, ci0, p, x) << std::endl;
	std::cout << "x: ";
  for (int i = 0; i < n; i++)
    std::cout << x[i] << ' ';
	std::cout << std::endl;	

	/* FOR DOUBLE CHECKING COST since in the solve_quadprog routine the matrix G is modified */
	
	{
    std::istringstream is("4, -2,"
													"-2, 4 ");
	
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				is >> G[i][j] >> ch;
	}
	
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			sum += x[i] * G[i][j] * x[j];
	sum *= 0.5;
	
	std::cout << "Double checking cost: ";
	for (int i = 0; i < n; i++)
		sum += g0[i] * x[i];
	std::cout << sum << std::endl;
}

