#ifndef SVM_H
#define SVM_H

#include <iostream>
#include <string>
#include <fstream>
#include<stdlib.h>
#include <cmath>
#include  <math.h>
#include "QuadProg++.hh"
//#include "SVM.cc"

using namespace std;


class SVM{
public:
  //データメンバ
  double G[MATRIX_DIM][MATRIX_DIM], g0[MATRIX_DIM], 
    CE[MATRIX_DIM][MATRIX_DIM], ce0[MATRIX_DIM], 
    CI[MATRIX_DIM][MATRIX_DIM], ci0[MATRIX_DIM], 
    alpha[MATRIX_DIM];
  double weight[2],theta;


  int n,m,alpha_max_number,x0,x1;	
  int i,j;
  double Kernel;
  string argv1;
  //struct Dataset data[DATA_NUM];

  //メンバ関数

  SVM(string argv1,Dataset* data);    //コンストラクタ
  //メソッド
  double get_G (int array_num0,int array_num1){return G[array_num0][array_num1];}

  double get_alpha  (int array_num){return alpha[array_num];}
  double get_weight  (int array_num){return weight[array_num];}

  double get_theta()  {return theta;} 
  void set_G(int array_num0,int array_num1,double n){G[array_num0][array_num1]=n;}

};



















//SVMのコンストラクタの定義
SVM::SVM(string argv1 ,Dataset* data){

  
  int i=0;
  int j=0;
  alpha_max_number=1;
  n = DATA_NUM;






  Kernel=return_Kernel(data, argv1);
  cout<<Kernel<<endl;
  for (int i=0; i < DATA_NUM; i++){	
    for (int j=0 ; j <DATA_NUM; j++){

      G[i][j] =data[i].y*data[j].y*Kernel;
     
      if(i==j) G[i][j]+=1.0e-7;
    }
    // cout<<data[i].y<<endl;
  }


 
  {//g0の要素は全て-1

    for (int i = 0; i < n; i++){
      g0[i] =(double) -1;

    }
  }
  //m = 1;
  {//CEは(y1 y2 .. yn)
    for (int i = 0; i < n; i++){
      for(int j=0;j<n;j++){

	CE[i][0] =data[i].y;

      }
    }



  } 


  {//ce0は0
    for (int i = 0; i < n; i++){

      ce0[i]=(double)0;
    }
  }

  {
    /*CIは
      (1 0 0 0 0 )
      (0 1 0 0 0 )
      (0 0 1 0 0 )
      (0 0 0 1 0 )
      (0 0 0 0 1 )
    */

    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
	if(i==j){
	  CI[i][j]=(double)1;

	}else{
	  CI[i][j]=(double)0;
	}
  }

  {
    //ci0は(0 0 0 0 0 )
    for (int j = 0; j < n; j++){
      ci0[j] =(double)0;
    }
  }



  try{
    //例外が発生する可能性のあるコード

    solve_quadprog(G, g0,100,  CE, ce0,1, CI, ci0,100, alpha);
  } catch (const std::exception& ex) {
    std::cerr << "solve_quadprog_failed" << ex.what() << std::endl;
    throw;
  }


  //alphaを出力
    print_alpha(alpha,&alpha_max_number);
  //重みweight出力  

  for(i=0;i<n;i++){
    weight[0]+=alpha[i]*data[i].y*data[i].input_first;
    weight[1]+=alpha[i]*data[i].y*data[i].input_second;
  }
  cout<<"weight[0]="<<weight[0]<<endl;
  cout<<"weight[1]="<<weight[1]<<endl;


  //(w,x)=thetaとなるときを考える(カーネルトリックなし)
  //カーネルトリックなしのときの境界の式（一次式）
  //cout<<weight[0]<<"x+"<<weight[1]<<"y="<<theta<<endl;


  /*
    if(argv[1]){
    argv1=argv[1];
    }else{
    argv1="No Kernel";
    }
  */

  //thetaを出力する
  print_theta(argv1,alpha_max_number,data,weight,Kernel);

  cout<<"f"<<endl;
  for (int x0 = 0; x0 < 50; x0 ++){
    for (int x1 = 0; x1 < 50; x1 ++){
      //識別f
      // f(argv1,weight,x0,x1,theta);
    }
  }

}














#endif
