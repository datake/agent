#include <iostream>
#include <string>
#include <fstream>
#include<stdlib.h>
#include "QuadProg++.hh"
//#include "SVM.h"
#include <cmath>
#include  <math.h>


#define DATA_NUM 100
//#define BLOCK_NUM 10
//#define sigma 10.0
#define FILENAME "./data/sample_linear.dat"
using namespace std;

//クラス宣言

class Dataset{
public:

  double x_0;
  double x_1;
  double y;

};

//プロトタイプ宣言
double GaussianKernel(double x_i_0,double x_i_1,double x_j_0,double x_j_1);
//int read_data(Dataset*  data);
//２点間の距離を出力する関数。Gaussカーネルで利用
double norm(double x_i_0,double x_i_1,double x_j_0,double x_j_1);
double PolinomicalKernel(double x_i_0,double x_i_1,double x_j_0,double x_j_1);
int read_data(Dataset*  data);
void set_max_alpha(double alpha[MATRIX_DIM],int *alpha_max_number);
void set_theta(string argv1,int alpha_max_number,Dataset* data, double weight[2],double Kernel);
double  f(string argv1,double weight[2],int x0,int x1,double theta);

void MakeFold();
//交差検定を行う関数
double cross_validation(Dataset* data,string argv1);

//SVMという関数を独立させる.その関数は引数としてカーネルの種類と訓練データをとり、出力としてf、またはweightとthetaを出力させる。
//double SVM(Dataset* data,double Kernel);
double return_Kernel_ij(Dataset* data,string argv1,int i,int j);
//void set_G(double *G[MATRIX_DIM][MATRIX_DIM],Dataset* data,double Kernel);

//正なら１を、負ならー１を返す関数
int sign(double something);
int sigma=1;
int BLOCK_NUM;
//クラス宣言

class fold{
public:
  double x_0[DATA_NUM/*/BLOCK_NUM*/];
  double x_1[DATA_NUM/*/BLOCK_NUM*/];
  double y[DATA_NUM/*/BLOCK_NUM*/];
  //メソッド
  void set_x_0(int array_num,double n){x_0[array_num]=n;}
  void set_x_1(int array_num,double n){x_1[array_num]=n;}
  void set_y(int array_num,double n){y[array_num]=n;}

  double get_x_0 (int array_num){return x_0[array_num];}
  double get_x_1  (int array_num){return x_1[array_num];}
  double get_y  (int array_num){return y[array_num];}
};



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


























int main (int argc, char *const argv[]) {
  string argv1;
  
  Dataset data[DATA_NUM];//クラスのインスタンス作成

  //指定したファイルの内容読み込んで、dataに書き込む。
  read_data(data);
  /* for(int i=0;i<100;i++){
     cout<<data[i].x_0<<endl;
     }*/

  if(argv[1]){
    argv1=argv[1];
  }else{
    argv1="No Kernel";
  }
  
  /*
  //Kernelはi,jに応じて、100*100こある数値。そのひとつひとつの数値に応じてG[i][j]を設定できる
  // SVM SVM(argv1,data);
  cross_validation(data,argv1);
  */
  sigma = 1;
  for (BLOCK_NUM = 1; BLOCK_NUM <21; BLOCK_NUM++){
    cout<<BLOCK_NUM<<" ";
  //Kernelはi,jに応じて、100*100こある数値。そのひとつひとつの数値に応じてG[i][j]を設定できる
  // SVM SVM(argv1,data);
  cross_validation(data,argv1);


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
    data[i].x_0=0; data[i].x_1=0; data[i].y=0;

    sscanf(str.data(), "%lf %lf %lf", &data[i].x_0, &data[i].x_1, &data[i].y);
    
    i++;
  }



  return 0;
}
void set_max_alpha(double alpha[MATRIX_DIM],int *alpha_max_number){
  int i=0;
  //出力
  //cout<<"alpha"<<endl;

  for ( i = 0; i <DATA_NUM*(BLOCK_NUM-1)/BLOCK_NUM; i++){
    //  for ( i = 0; i <DATA_NUM; i++){

    // cout<<"alpha[i]:"<<alpha[i]<<"alpha[alpha_max_number]:"<<alpha[alpha_max_number]<<endl;
    if(alpha[i]*10>alpha[*alpha_max_number]*10){
      (*alpha_max_number)=i;
      // cout<<"alpha_max_number"<<*alpha_max_number<<endl;
    }
    // printf("%d\t%f\n", i, alpha[i]);//alphaは浮動小数点型で出力

  }


}

void set_theta(string argv1,int alpha_max_number,Dataset* data, double weight[2],double Kernel){

  //alpha_max_numberはalphaが最大の番号
  //cout<<"alpha_max_number:"<<alpha_max_number<<endl;
  if(argv1=="P"){

    Kernel=PolinomicalKernel(weight[0],weight[1],data[alpha_max_number].x_0,data[alpha_max_number].x_1);


  }else if(argv1=="G"){

    Kernel=GaussianKernel(weight[0],weight[1],data[alpha_max_number].x_0,data[alpha_max_number].x_1);

  }else{

    //内積でカーネルトリックなし
    Kernel=(weight[0]*data[alpha_max_number].x_0+weight[1]*data[alpha_max_number].x_1);


  }

  double theta=Kernel-data[alpha_max_number].y;
  // cout<<"theta="<<theta<<endl;



}


double  f(string argv1,double weight[2],int x0,int x1,double theta){
  
  double Kernel;
  if(argv1=="P"){   
    Kernel=PolinomicalKernel(weight[0],weight[1],x0,x1);


  }else if(argv1=="G"){

    Kernel=GaussianKernel(weight[0],weight[1],x0,x1);

  }else{

    //内積でカーネルトリックなし
    Kernel=(weight[0]*x0+weight[1]*x1);


  }
  /*

    if(Kernel*100<theta*100){
    printf("%d\t%d\n",x0,x1);
    }*/
  // cout<<"f:"<<Kernel-theta <<endl;

  return Kernel-theta; 



}




double cross_validation(Dataset* data,string argv1){


  /*
    データをDATA_NUMブロックに分割してDATA_NUMこのデータセットを作成
    test１つとtrainDATA_NUM-1つに分割
  */

  fold fold_obj[BLOCK_NUM]; 
  Dataset tmp_trainset[DATA_NUM/BLOCK_NUM];
  fold fold_test[BLOCK_NUM+1]; 
  fold fold_train[BLOCK_NUM+1]; 
  int 	size_per_block=floor(DATA_NUM/BLOCK_NUM);


  //dataをfold_objに代入
  for ( int i = 0; i <DATA_NUM; i++){
    for(int j=0;j<BLOCK_NUM ;j++){
      if(j==(i%BLOCK_NUM)){

	fold_obj[i%BLOCK_NUM].set_x_0(floor(i/BLOCK_NUM),data[i].x_0);
	fold_obj[i%BLOCK_NUM].set_x_1(floor(i/BLOCK_NUM),data[i].x_1);
	fold_obj[i%BLOCK_NUM].set_y(floor(i/BLOCK_NUM),data[i].y);
	//	cout <<"i:"<<i<<",j:"<<j<<",fold_obj["<<j<<"]に、fold_obj["<<j<<"].x_0["<<floor(i/BLOCK_NUM)<<"]:"<<fold_obj[j].get_x_0(floor(i/BLOCK_NUM))<<",fold_obj["<<j<<"].x_1["<<floor(i/BLOCK_NUM)<<"]:"<<fold_obj[j].get_x_1(floor(i/BLOCK_NUM))<<",fold_obj["<<j<<"].y["<<floor(i/BLOCK_NUM)<<"]:"<<fold_obj[j].get_y(floor(i/BLOCK_NUM))<<"が代入された"<<endl;
      }
    }
  }


  //グループjをテスト、それ以外のグループを訓練として、一回分割したグループの再編を行う
 

  //j番目のグループをテストグループとしてループ
  for (int j = 0; j < BLOCK_NUM; j++) {

    //テストグループへの代入
    for (int i = 0; i < DATA_NUM/BLOCK_NUM; i++) {

      fold_test[j].x_0[i]=fold_obj[j].x_0[i];
      fold_test[j].x_1[i]=fold_obj[j].x_1[i];
      fold_test[j].y[i]=fold_obj[j].y[i];
      // cout<<"fold_test["<<j<<"].x_0["<<i <<"]\t"<<fold_test[j].x_0[i]<<"\tx_1:"<<fold_test[j].x_1[i]<<"\ty:"<< fold_test[j].y[i]<<endl;

    }//end for i
  
    //訓練グループへのデータ代入
   
    for (int tmp = 0;tmp < BLOCK_NUM;tmp++) {
      for (int i = 0; i <floor(DATA_NUM/BLOCK_NUM); i++){
	if(tmp<j){
	  fold_train[j].x_0[i+tmp*size_per_block]=fold_obj[j].x_0[i];
	  fold_train[j].x_1[i+tmp*size_per_block]=fold_obj[j].x_1[i];
	  fold_train[j].y[i+tmp*size_per_block]=fold_obj[j].y[i];
	  //  cout<<"i:"<<i<<"fold_train["<<j<<"].x_0["<<i+tmp*size_per_block <<"]\t"<<fold_train[j].x_0[i+tmp*size_per_block]<<"\tx_1:"<<fold_train[j].x_1[i+tmp*size_per_block]<<"\ty:"<< fold_train[j].y[i+tmp*size_per_block]<<endl;
	}else if(tmp>j){
	  fold_train[j].x_0[i+(tmp-1)*size_per_block]=fold_obj[j].x_0[i];
	  fold_train[j].x_1[i+(tmp-1)*size_per_block]=fold_obj[j].x_1[i];
	  fold_train[j].y[i+(tmp-1)*size_per_block]=fold_obj[j].y[i];
	  // cout<<"i:"<<i<<"fold_train["<<j<<"].x_0["<<i+(tmp-1)*size_per_block <<"]\t"<<fold_train[j].x_0[i+(tmp-1)*size_per_block]<<"\tx_1:"<<fold_train[j].x_1[i+(tmp-1)*size_per_block]<<"\ty:"<< fold_train[j].y[i+(tmp-1)*size_per_block]<<endl;
	    

	  //	  }else if(tmp==j){
	  //cout<<"j=tmp="<<tmp<<j<<"i:"<<i<<endl;
	}
      }
    }
  }
  //}
	  
   
  double sum_theta=0;
  double sum_weight[2]={};
 
  int pass =0;

  //thetaとweightを足していく。

  //fold_train[count_of_test]に対してSVMを実行して識別機(j=0,1,..BLOCK_NUM)
  //fold_train[count_of_test].x_0[0]からfold_train[count_of_test].x_0[79]に対して1回SVM実行
  //SVMは合計BLOCK_NUM回実行
  //tmp_trainset[0].x_0からtmp_trainset[79].x_0だと考える
  //ex. 80=100*(5-1)/5 
  int count_of_test=0, count_of_train=0;
  for (count_of_test = 0; count_of_test < BLOCK_NUM; count_of_test++) {
    for (count_of_train = 0; count_of_train <floor(DATA_NUM*(BLOCK_NUM-1)/BLOCK_NUM); count_of_train++) {
      tmp_trainset[count_of_train].x_0=fold_train[count_of_test].x_0[count_of_train];
      tmp_trainset[count_of_train].x_1=fold_train[count_of_test].x_1[count_of_train];
      tmp_trainset[count_of_train].y=fold_train[count_of_test].y[count_of_train];
      // cout<<"SVM!  count_of_train:" <<count_of_train<<"count_of_test:"<<count_of_test<<endl;
    }//ここで１回分のSVM実行するデータ挿入完了。これからSVM実行

    
    SVM SVM(argv1,tmp_trainset);
    // cout<<"j:"<<j<<"weight and theta:"<<SVM.weight[0]<<SVM.weight[1]<<SVM.theta<<endl;
    sum_theta += SVM.theta;
    sum_weight[0] += SVM.weight[0];
    sum_weight[1] += SVM.weight[1];
 

 
    //  cout<<"theta:"<<sum_theta/BLOCK_NUM<<"weight"<< sum_weight[0]/BLOCK_NUM<< sum_weight[1]/BLOCK_NUM<<endl;

    //識別機は次のfであらわされ、x0,x1にテストデータの値をいれる。
    
    //正しいかどうかで正答率を判断
    int test=sign(f( argv1, sum_weight,  fold_test[count_of_test].x_0[count_of_train],  fold_test[count_of_test].x_1[count_of_train], sum_theta));

    if( test== sign(*fold_test[count_of_test].y)){//訓練データで作成した識別機にテストデータx0,x1を通した結果がyとただしければ
      //  cout<<"成功test:"<<test<<"\tf:"<<f( argv1, sum_weight,  fold_test[count_of_test].x_0[count_of_train],  fold_test[count_of_test].x_1[count_of_train], sum_theta)<<endl;
      pass++;
    }else{
      // cout<<"test失敗/"<<count_of_test<<"回目"<<endl;
    }//end if
    // cout<<"test:"<<test<<"\tf:"<<f( argv1, sum_weight,  fold_test[j].x_0[i],  fold_test[j].x_1[i], sum_theta)<<endl;
    
  
  }
  // cout<<"pass:"<<pass<<"/"<<count_of_test<<"回目"<<endl;
  cout<<pass/(double)BLOCK_NUM<<endl;
  return 0;
}


























double return_Kernel_ij(Dataset* data,string argv1,int i,int j){


 
  double Kernel;


  if(argv1=="P"){

    Kernel=PolinomicalKernel(data[i].x_0,data[i].x_1,data[j].x_0,data[j].x_1);


  }else if(argv1=="G"){

    Kernel=GaussianKernel(data[i].x_0,data[i].x_1,data[j].x_0,data[j].x_1);

  }else{

    //内積でカーネルトリックなし
    Kernel=(data[i].x_0*data[j].x_0+data[i].x_1*data[j].x_1);


  }



     
  {/*
   //シグモイドカーネル
   Kernel=(double)tanh((1.0+data[i].x_0*data[j].x_0+data[i].x_1*data[j].x_1),2.0);
   G[i][j] =(double)data[i].y*data[j].y*Kernel;
   if(i==j) G[i][j]+=1.0e-7;*/
  }
 		


  return Kernel;



}





























//SVMのコンストラクタの定義
SVM::SVM(string argv1 ,Dataset* data){

  
  int i=0;
  int j=0;
  alpha_max_number=1;
  n = DATA_NUM*(BLOCK_NUM-1)/BLOCK_NUM;
  //n=DATA_NUM;n変更した





 
  //  cout<<Kernel<<endl;
  for (int i=0; i < n; i++){	
    for (int j=0 ; j <n; j++){

      G[i][j] =data[i].y*data[j].y*return_Kernel_ij(data,argv1,i,j);
     
      if(i==j) {
	G[i][j]+=1;//1.0e-2;
	//cout<<"微小値"<<i<<","<<j<<","<<G[i][j]<<endl;
      }
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
    //CIは
    // (1 0 0 0 0 )
    /// (0 1 0 0 0 )
    //(0 0 1 0 0 )
    //(0 0 0 1 0 )
    //(0 0 0 0 1 )
    

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

    solve_quadprog(G, g0,n,  CE, ce0,1, CI, ci0,100, alpha);
  } catch (const std::exception& ex) {
    std::cerr << "solve_quadprog_failed" << ex.what() << std::endl;
    throw;
  }


  //max_alphaをセットする関数。消したらだめ！
   set_max_alpha(alpha,&alpha_max_number);
  //重みweight出力  

  for(i=0;i<n;i++){
    weight[0]+=alpha[i]*data[i].y*data[i].x_0;
    weight[1]+=alpha[i]*data[i].y*data[i].x_1;
  }
  // cout<<"weight[0]="<<weight[0]<<endl;
  //cout<<"weight[1]="<<weight[1]<<endl;


  //(w,x)=thetaとなるときを考える(カーネルトリックなし)
  //カーネルトリックなしのときの境界の式（一次式）
  //cout<<weight[0]<<"x+"<<weight[1]<<"y="<<theta<<endl;



  //thetaを出力する
  set_theta(argv1,alpha_max_number,data,weight,Kernel);

  // cout<<"f"<<endl;
  for (int x0 = 0; x0 < 50; x0 ++){
    for (int x1 = 0; x1 < 50; x1 ++){
      //識別f
      // このだしかたちがうf(argv1,weight,x0,x1,theta);
    }
  }

}


int sign(double something){
  if(something*100>0){

    return 1;
  }else if(something*100<0){
    return -1;

  }


}
