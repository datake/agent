#include <iostream>
#include <stdlib.h>
#include "QuadProg++.hh"
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include  <iomanip>
#define sigma 7
#define CLIENT_NUM 2
#define GOODS_NUM 3
#define DATA_NUM 100
using namespace std;


struct Dataset{
  double input_first,input_second,y;
};
int main (int argc, char *const argv[]) {
  double G[MATRIX_DIM][MATRIX_DIM], g0[MATRIX_DIM], 
  CE[MATRIX_DIM][MATRIX_DIM], ce0[MATRIX_DIM], 
  CI[MATRIX_DIM][MATRIX_DIM], ci0[MATRIX_DIM], 
  alpha[MATRIX_DIM];
  double weight[2],theta;
  int n,m,alpha_max_number=1;	
  int i=0;
  double Kernel;
  n = DATA_NUM;
 // string argv1='G';

  struct Dataset data[DATA_NUM];

    //ファイル読み込み
  for(int temp_client =0;temp_client<CLIENT_NUM;temp_client++){
    for(int temp_goods=0;temp_goods<GOODS_NUM;temp_goods++){
      std::stringstream ss;
      ss << "server1.2/output_goods" << temp_goods+1 <<"_client" <<temp_client+1<<".dat";
         std::string result_outputfile =ss.str();//.str() でstring型の値を得る
         cout<<"LOADED FILE:"<<result_outputfile<<endl;
         std::ifstream ifs(result_outputfile.c_str(),std::ios::app);
         string str="";
         if(ifs.fail()) {
          cout<<"File do not exist.\n"<<endl;
          cerr << "File do not exist.\n";
          exit(0);
          
        }
        i=0;
        double weight[2]={};
        double theta=0;
        double alpha[MATRIX_DIM]={};
        alpha_max_number=1;

        while(getline(ifs, str)) {
          if(i==DATA_NUM){
            cout<<"getline break"<<endl;
            break;
          }
          data[i].input_first=0; data[i].input_second=0; data[i].y=0;
          sscanf(str.data(), "%lf %lf", &data[i].input_first,  &data[i].y);
          //printf("%lf %lf %lf", data[i].input_first, data[i].input_second, data[i].y);
          cout<<"data[i].input_first:"<<data[i].input_first<< ",data[i].input_second:"<< data[i].input_second<<",data[i].y:"<<data[i].y<<endl;
          i++;
        }

        {
          for (int i = 0; i < n; i++){	
           for (int j = 0; j < n; j++){
             Kernel=(data[i].input_first*data[j].input_first+data[i].input_second*data[j].input_second);
             G[i][j] =data[i].y*data[j].y*Kernel;
             //cout<<"G[i][j]"<<G[i][j]<<" ";
             if(i==j) G[i][j]+=1.0e-9;
           }

         }		

       }


  {//g0の要素は全て-1

    for (int i = 0; i < n; i++){
      g0[i] =(double) -1;
      //	cout<<"g0["<<i<<"]="<<g0[i] <<endl;
    }
  }
  m = 1;
  {//CEは(y1 y2 .. yn)
    for (int i = 0; i < n; i++){
      for(int j=0;j<n;j++){
       CE[i][0] =data[i].y;
	// 	cout<<"CE["<<i<<"]["<<j<<"]="<<CE[i][j]<<endl;
     }
   }

 } 


  {//ce0は0
    for (int i = 0; i < n; i++){
      ce0[i]=(double)0;
    }
  }


  {

    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
       if(i==j){
         CI[i][j]=(double)1;
	  //cout<<"CI["<<i<<"]["<<j<<"]=" << CI[i][j] <<endl;
       }else{
         CI[i][j]=(double)0;
	  //cout<<"CI["<<i<<"]["<<j<<"]=" << CI[i][j] <<endl;
       }


     }

     {
    //ci0は(0 0 0 0 0 )
      for (int j = 0; j < n; j++){
        ci0[j] =(double)0;
      //cout<<"ci0["<< j<<"]="<<ci0[i] <<endl;
      }
    }
  //solve_quadprogでとく


    try{
    //例外が発生する可能性のあるコード
     solve_quadprog(G, g0,100,  CE, ce0,1, CI, ci0,100, alpha);
   } catch (const std::exception& ex) {
    std::cerr << "solve_quadprog_failed" << ex.what() << std::endl;
    throw;
  }

  //出力
  cout<<"alpha"<<endl;
  for ( i = 0; i <n; i++){
   // cout<<"alpha["<<i<<"]:"<<alpha[i]<<endl;
    if(alpha[i]>alpha[alpha_max_number]){
      alpha_max_number=i;
    }
    //printf("%d\t%f\n", i, alpha[i]);//alphaは浮動小数点型で出力

  }
  //重みweight出力

  for(i=0;i<n;i++){
    weight[0]+=alpha[i]*data[i].y*data[i].input_first;
    weight[1]+=alpha[i]*data[i].y*data[i].input_second;
  }
  cout<<"weight[0]="<<weight[0]<<endl;
  cout<<"weight[1]="<<weight[1]<<endl;

  //しきい値theta出力
  //本来はどのalpha[k]のときでもthetaは同じになるはずである。ここではk=5をとる


  //(w,x)=thetaとなるときを考える(カーネルトリックなし)
  //カーネルトリックなしのときの境界の式（一次式）
  //cout<<weight[0]<<"x+"<<weight[1]<<"y="<<theta<<endl;

  //	cout<<argv1<<endl;


  //alpha_max_numberはalphaが最大の番号
  cout<<"alpha_max_number"<<alpha_max_number<<endl;
   //Kernel=(data[i].input_first*data[j].input_first+data[i].input_second*data[j].input_second);



   Kernel=(weight[0]*data[alpha_max_number].input_first+weight[1]*data[alpha_max_number].input_second);

   theta=Kernel-data[alpha_max_number].y;
  cout<<"theta="<<theta<<endl;


}
}
}

