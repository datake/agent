#include <iostream>
#include <string>
#include <fstream>
#include<stdlib.h>
#include "QuadProg++.hh"
#include <cmath>
#include  <math.h>
#define DATA_NUM 100
#define BLOCK_NUM 3
#define sigma 10.0
#define FILENAME "./data/sample_circle.dat"
using namespace std;

//構造体宣言

struct Dataset{
  double input_first,input_second,y;

};

//プロトタイプ宣言
double GaussianKernel(double x_i_0,double x_i_1,double x_j_0,double x_j_1);
//int read_data(Dataset*  data);
//２点間の距離を出力する関数。Gaussカーネルで利用
double norm(double x_i_0,double x_i_1,double x_j_0,double x_j_1);
double PolinomicalKernel(double x_i_0,double x_i_1,double x_j_0,double x_j_1);
int read_data(Dataset*  data);
void print_alpha(double alpha[MATRIX_DIM],int *alpha_max_number);
//int print_theta()
void print_theta(string argv1,int alpha_max_number,Dataset* data, double weight[2],double Kernel);
void  f(string argv1,double weight[2],int x0,int x1,double theta);
void MakeFold();

//クラス宣言
class fold{
public:
  double x_0[DATA_NUM/BLOCK_NUM];
  double x_1[DATA_NUM/BLOCK_NUM];
  double y[DATA_NUM/BLOCK_NUM];
  //メソッド
  void set_x_0(int array_num,double n){x_0[array_num]=n;}
  void set_x_1(int array_num,double n){x_1[array_num]=n;}
  void set_y(int array_num,double n){y[array_num]=n;}

  double get_x_0 (int array_num){return x_0[array_num];}
  double get_x_1  (int array_num){return x_1[array_num];}
  double get_y  (int array_num){return y[array_num];}
};






int main (int argc, char *const argv[]) {
  double G[MATRIX_DIM][MATRIX_DIM], g0[MATRIX_DIM], 
    CE[MATRIX_DIM][MATRIX_DIM], ce0[MATRIX_DIM], 
    CI[MATRIX_DIM][MATRIX_DIM], ci0[MATRIX_DIM], 
    alpha[MATRIX_DIM];
  double weight[2],theta;
 

  int n,m,alpha_max_number=1,x0,x1;	
  int i=0;
  double Kernel;
  n = DATA_NUM;
  string argv1;

  struct Dataset data[DATA_NUM];
 
  read_data(data);
  






/*  const int data_element_count=sizeof data/sizeof data[0];
  //cout<<"sizeof(data)"<<sizeof data<<"sizeof data[0]"<<sizeof data[0]<<endl;*/
//cout<<"dataの要素数:"<<DATA_NUM<<endl;
//cout<<BLOCK_NUM<<"こずつに分解します"<<endl;
  //交差検定は10回学習させる
  /*
    データを１０ブロックに分割して１０このでーたセットを作成
    test１つとtrain９つに分割
  */

  //クラスのインスタンス作成

  //  fold  fold1,fold2,fold3,fold4,fold5,fold6,fold7,fold8, fold9,fold10;
  
  //BLOCK_NUMこのブロックにランダムに分割する
  fold fold1;
  fold fold_obj[BLOCK_NUM]; 
  for ( i = 0; i <DATA_NUM; i++){
 
    //  fold fold_obj[i].x_0=;
  }  
  /*
  
  //セット
  fold1.set_x_0(0,1);
  fold1.set_x_1(0,-1);
  fold1.set_y(0,1);

  //ゲット
  cout<<fold1.get_x_0(0)<<endl;
  cout<<fold1.get_x_1(0)<<endl;
  cout<<fold1.get_y(0)<<endl;
  
  */

  /*
  for ( i = 0; i <DATA_NUM; i++){
    fold1.set_x_0(i,data[i].input_first);
    fold1.set_x_1(i,data[i].input_second);
    fold1.set_y(i,data[i].y);
    cout<<fold1.get_x_0(i)<<","<<fold1.get_x_1(i)<<","<<fold1.get_y(i)<<endl;


    }*/
  //fold_obj[0]からfold_obj[9]の１０つのブロックに分割
  //それぞれのブロックには１０つずつ値が入る
  /*
  fold_obj[0]にはdata[X|Xは１０の倍数]
  fold_obj[1]にはdata[X|Xは１０の倍数+1]
  fold_obj[2]にはdata[X|Xは１０の倍数+2]
    ...
  fold_obj[9]にはdata[X|Xは１０の倍数+9]
  */

  //たとえばfold_obj[0]からfold_obj[2]の３つのブロックにわけるには
  //それぞれのブロックには33か34こずつ値が入る

  //方法１
  //fold_obj[0]にdata[0]からdata[33]まで
  //fold_obj[1]にdata[34]からdata[66]まで
  //fold_obj[2]にdata[67]からdata[100]まで
  

  //方法２←こちらを採用
  //fold_obj[0]にはdata[X|Xは３の倍数]
  //fold_obj[1]にはdata[X|Xは３の倍数+1]
  //fold_obj[2]にはdata[X|Xは３の倍数+2]
  

  
 for ( i = 0; i <DATA_NUM; i++){
    for(int j=0;j<BLOCK_NUM ;j++){//i%jでj=0にならないように注意
      //iをjでわった剰余(i%BLOCK_NUM)のグループ(fold_obj[(i%BLOCK_NUM)])に分類される
      // cout <<"i:"<<i<<",j:"<<j<<",i%j:"<<i%j<<endl;


      //あるiにたいして、セットするのは１回のみ
      if(j==(i%BLOCK_NUM)){

	fold_obj[i%BLOCK_NUM].set_x_0(floor(i/BLOCK_NUM),data[i].input_first);
	fold_obj[i%BLOCK_NUM].set_x_1(floor(i/BLOCK_NUM),data[i].input_second);
	fold_obj[i%BLOCK_NUM].set_y(floor(i/BLOCK_NUM),data[i].y);
	//cout <<"i:"<<i<<",j:"<<j<<",fold_obj["<<j<<"]に、fold_obj["<<j<<"].x_0["<<floor(i/BLOCK_NUM)<<"]:"<<fold_obj[j].get_x_0(floor(i/BLOCK_NUM))<<",fold_obj["<<j<<"].x_1["<<floor(i/BLOCK_NUM)<<"]:"<<fold_obj[j].get_x_1(floor(i/BLOCK_NUM))<<",fold_obj["<<j<<"].y["<<floor(i/BLOCK_NUM)<<"]:"<<fold_obj[j].get_y(floor(i/BLOCK_NUM))<<"が代入された"<<endl;
      }
    }

  }







  {
    {
     
      for (int i = 0; i < n; i++){	
	//0のクラス出力
	if(data[i].y==-1){
	

	}
	for (int j = 0; j < n; j++){

	  //C/C++ではargv[]はchar型
				
	  if(argv[1]){
	    argv1=argv[1];
	  }else{
	    argv1="No Kernel";
	  }
	
	  if(argv1=="P"){
	   
	    Kernel=PolinomicalKernel(data[i].input_first,data[i].input_second,data[j].input_first,data[j].input_second);


	  }else if(argv1=="G"){
	  
	    Kernel=GaussianKernel(data[i].input_first,data[i].input_second,data[j].input_first,data[j].input_second);

	  }else{
	   
	    //内積でカーネルトリックなし
	    Kernel=(data[i].input_first*data[j].input_first+data[i].input_second*data[j].input_second);


	  }


	  G[i][j] =data[i].y*data[j].y*Kernel;

	  if(i==j) G[i][j]+=1.0e-7;



	

	}
	{/*
	 //シグモイドカーネル
	 Kernel=(double)tanh((1.0+data[i].input_first*data[j].input_first+data[i].input_second*data[j].input_second),2.0);
	 G[i][j] =(double)data[i].y*data[j].y*Kernel;
	 if(i==j) G[i][j]+=1.0e-7;*/
	}
      }		

    }
  }

  {//g0の要素は全て-1

    for (int i = 0; i < n; i++){
      g0[i] =(double) -1;
    
    }
  }
  m = 1;
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
  //print_alpha(alpha,& alpha_max_number);
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

  if(argv[1]){
    argv1=argv[1];
  }else{
    argv1="No Kernel";
  }


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
double norm(double x_i_0,double x_i_1,double x_j_0,double x_j_1){
  return pow(x_i_0 -x_j_0,2.0)+ pow(x_i_1 -x_j_1,2.0);

}
double PolinomicalKernel(double x_i_0,double x_i_1,double x_j_0,double x_j_1){
  return pow((1.0+x_i_0*x_j_0+x_i_1*x_j_1),2.0);

}

double GaussianKernel(double x_i_0,double x_i_1,double x_j_0,double x_j_1){
  return exp(-norm(x_i_0,x_i_1,x_j_0,x_j_1)/2.0/sigma/sigma);


}

int read_data(Dataset* data) {
  int i=0;
  ifstream ifs(FILENAME);
  string str;
  if(ifs.fail()) {
    cerr << "File do not exist.\n";
    exit(0);
  }

  while(getline(ifs, str)) {
    if(i==DATA_NUM) break;
    data[i].input_first=0; data[i].input_second=0; data[i].y=0;

    sscanf(str.data(), "%lf %lf %lf", &data[i].input_first, &data[i].input_second, &data[i].y);
  
    i++;
  }



  return 0;
}
void print_alpha(double alpha[MATRIX_DIM],int *alpha_max_number){
  int i=0;
  //出力
  cout<<"alpha"<<endl;
  for ( i = 0; i <DATA_NUM; i++){
    // cout<<"alpha[i]:"<<alpha[i]<<"alpha[alpha_max_number]:"<<alpha[alpha_max_number]<<endl;
    if(alpha[i]*10>alpha[*alpha_max_number]*10){
      (*alpha_max_number)=i;
      // cout<<"alpha_max_number"<<*alpha_max_number<<endl;
    }
    printf("%d\t%f\n", i, alpha[i]);//alphaは浮動小数点型で出力

  }


}

void print_theta(string argv1,int alpha_max_number,Dataset* data, double weight[2],double Kernel){
 



  //alpha_max_numberはalphaが最大の番号
   cout<<"alpha_max_number:"<<alpha_max_number<<endl;
  if(argv1=="P"){
   
    Kernel=PolinomicalKernel(weight[0],weight[1],data[alpha_max_number].input_first,data[alpha_max_number].input_second);


  }else if(argv1=="G"){
   
    Kernel=GaussianKernel(weight[0],weight[1],data[alpha_max_number].input_first,data[alpha_max_number].input_second);

  }else{
  
    //内積でカーネルトリックなし
    Kernel=(weight[0]*data[alpha_max_number].input_first+weight[1]*data[alpha_max_number].input_second);


  }
 
  double theta=Kernel-data[alpha_max_number].y;
  cout<<"theta="<<theta<<endl;



}
void  f(string argv1,double weight[2],int x0,int x1,double theta){

  double Kernel;
 if(argv1=="P"){   
    Kernel=PolinomicalKernel(weight[0],weight[1],x0,x1);


  }else if(argv1=="G"){
   
   Kernel=GaussianKernel(weight[0],weight[1],x0,x1);

  }else{
  
    //内積でカーネルトリックなし
    Kernel=(weight[0]*x0+weight[1]*x1);


  }

 if(Kernel*100<theta*100){
   printf("%d\t%d\n",x0,x1);
  

  }

}




