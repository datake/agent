#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/uio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "QuadProg++.hh"
#define BUF_LEN 1024                      // バッファのサイズ 
#define EVAL_VALUE_FILE "./kadai4_data/2client-2goods-2.dat"
#define LOG_FILE "./kadai4_data/output_goods1_client1.dat"
//for SVM
#define sigma 7
#define CLIENT_NUM 2
#define GOODS_NUM 3
#define DATA_NUM 100
#define MATRIX_DIM 100
using namespace std;



struct Dataset{
  double input_first,input_second,y;
};

int SVM();
int client_id;
int is_firstday=0;
double SVM_expected_price[GOODS_NUM-1];


int main(int argc, char *argv[]){
  int i=0,j=0,k=0,l=0;
  int loop_count=0;
  int s;  /* ソケットのためのファイルディスクリプタ */
  int goods_num,client_num;
  struct hostent *servhost;            // ホスト名と IP アドレスを扱うための構造体 
  struct sockaddr_in server;           // ソケットを扱うための構造体 
  struct servent *service;             // サービス (http など) を扱うための構造体 
  char buf[BUF_LEN];
  int read_size;
  char *host;
  //char host[BUF_LEN] = "127.0.0.1";    // 接続するホスト名 
  char path[BUF_LEN] = "/";              // 要求するパス 
  unsigned short port = 5000;            // 接続するポート番号 
  string string_myname="kadai4_client_weak";//クライアント名
  string str;
  std::vector<int> eval_val_vector;//評価値が順番に2^n-1こ入っている。
  std::vector<int>  goods_price_vector;
  std::vector<int>  client_log_vector;


  // ファイル出力ストリームの初期化
  std::ofstream ofs("./kadai4_goods_client_log/goods_price_log.dat",std::ios::app);
  std::ofstream ofs2("./kadai4_goods_client_log/client_log.dat",std::ios::app);



  // 引数が不足している場合 
  if (argc < 3) {
    printf("usage: client [hostname] [port]\n");
    exit(1);
  }

  // コマンドライン引数からサーバ名を取得
  if (argc > 1) {
    host = (char *)argv[1];
  }

  if (argc > 2) {
    port = atoi(argv[2]);
  }

  // ホストの情報(IPアドレスなど)を取得 
  servhost = gethostbyname(host);
  if ( servhost == NULL ){
    fprintf(stderr, "[%s] から IP アドレスへの変換に失敗しました。\n", host);
    return 0;
  }

  bzero(&server, sizeof(server));            // 構造体をゼロクリア 

  server.sin_family = AF_INET;

  // IPアドレスを示す構造体をコピー 
  bcopy(servhost->h_addr, &server.sin_addr, servhost->h_length);

  server.sin_port = htons(port);

  // ソケット生成 
  if ( ( s = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ){
    fprintf(stderr, "ソケットの生成に失敗しました。\n");
    return 1;
  }
  // サーバに接続 
  if ( connect(s, (struct sockaddr *)&server, sizeof(server)) == -1 ){
    fprintf(stderr, "connect に失敗しました。\n");
    return 1;
  }

  // 名前の送信 
   // "PLEASE INPUT YOUR NAME"
  read_size = read(s, buf, BUF_LEN);//sにつながっているファイルからBUF_LENバイト読み込みbuf格納
  if ( read_size > 0 ){
    write(1, buf, read_size);
  }
  write(s, string_myname.c_str(), string_myname.length());
  write(s, "\n", 1);


  //エージェントの評価値の読み取り
  //評価値データのあるファイルから一行読み込む
  ifstream ifs(EVAL_VALUE_FILE);

  if(ifs.fail()) {
    cerr << "File EVAL_VALUE_FILE"<<EVAL_VALUE_FILE<<"do not exist.\n";
    exit(0);
  }

  //一日目かどうかを判別するためのログファイル
  ifstream ifs_log_file(LOG_FILE);
  if(ifs_log_file.fail()) {
    is_firstday=1;
    cout << "File LOG_FILE"<<LOG_FILE<<"do not exist.\nThis is a first day";
  }

  //自分の評価値をeval_val_vectorに保存
  while(getline(ifs,str)) {//strに評価値データが入る
    cout<<"EVAL_VALUE_FILE:"<<str.data()<<endl;//EVAL_VALUE_FILE:10:80 01:40 11:110

    //http://mementoo.info/archives/611#2-2-1
    string tmp,tmp_eval_val_str;
    std::istringstream stream(str);
    while(getline(stream,tmp,' ')){
      cout<<"tmp:"<< tmp << std::endl;
      //tmp:10:80 tmp:01:40のように一行を空白ごとに分割したのがtmp

      int colon_loc=tmp.find(":",0);//colon_loc:コロンの位置   
      tmp_eval_val_str=tmp.substr(colon_loc+1,3); //コロンの次の数字（商品の値段）
      cout<<"tmp_eval_val:"<<tmp_eval_val_str<<endl;
      eval_val_vector.push_back(atoi(tmp_eval_val_str.c_str()));//商品の値段データの追加
      tmp=tmp.substr(colon_loc+1);//コロン以降の文字列
      //cout<<"tmp_buffer:"<<tmp_buffer<<endl;
    }
  }

  //a1:takeda a2:hajime a3:take 
  //自分のidとクライアント名を覚える
  read_size = read(s, buf, BUF_LEN);
  if (read_size > 0)
    write(1, buf, read_size);
  buf[read_size] = '\0';//文字列の最後という印


  // "Your ID: x", x はクライアントID
  read_size = read(s, buf, BUF_LEN);
  if ( read_size> 0)
    write(1, buf, read_size);
  buf[read_size] = '\0';
  sscanf(buf, "%*[^0-9]%d", &client_id);//任意の文字を数字が始まる直前までよみとばして、数字のclient_idをよみとる
  printf("got your client_id:%d\n",client_id);
  //sscanf(const char *str入力元の文字列, const char *format書式指定文字列,格納する引数)
  //0-9までの%d(10進数int型)
  //^は"YOUR ID:"に相当し、捨てる

  //商品数とクライアント数を認識
  // "n,m", n は商品数 m はクライアント合計数
  read_size = read(s, buf, BUF_LEN);
  if (read_size > 0)
    write(1, buf, read_size);
  buf[read_size] = '\0';
  sscanf(buf, "%d%*[^0-9]%d", &goods_num, &client_num);//%*[^0-9]は数字が始まるまで読み飛ばす
  printf("got goods_num:%d,client_num:%d\n",goods_num,client_num);


  puts("最初の入札情報を自動で入力");
  //それぞれの商品について入札するかどうかを決定。各商品に対して入札する場合は1，しない場合は0
  //標準入力を使わずにサーバーに情報を送信
  for (i=0;i<goods_num;i++){
    buf[i] = '1';
  }
  buf[i] = '\n';
  buf[i+1] = '\0';
  printf("最初の入札完了");  
  write(s, buf, strlen(buf));//ソケットにつながるsにbufをstrlenバイト書き込む
  read_size = read(s, buf, BUF_LEN);
  if ( read_size > 0 ){
    write(1, buf, read_size);
  }

  // 送受信ループ 
  while (1) {
    char str[256];
    //それぞれの商品に対して入札するかどうか
    //自分の予算-相手の予算＝儲け>0なら入札する
    //SVM_expected_price[goods_num]=相手の予算
    //eval_val_vector=自分の予算

    //初日の場合、自分の予算より低い価格であれば入札する
    if(is_firstday==1){
       for (j=0;j<goods_num;j++){
          if(loop_count==0){//1回目のwhileループではgoods_price_vectorが存在しない
              buf[j] = '1';
          }else{
            cout<<"評価値"<<eval_val_vector[j]<<",(loop_count-1)*goods_num+j:"<<(loop_count-1)*goods_num+j<<",値段:"<<goods_price_vector[(loop_count-1)*goods_num+j]<<endl;
            if(eval_val_vector[j]>goods_price_vector[(loop_count-1)*goods_num+j]){
               buf[j] = '1';
            }else{
               buf[j] = '0';
            }
          }
      }
    //初日でない場合、自分の予算-相手の推定予算>0なら入札する。
    }else if(is_firstday==0){
      for (j=0;j<goods_num;j++){
        if(loop_count==0){//1回目のwhileループではgoods_price_vectorが存在しない
              buf[j] = '1';
        }else{
          //このクライアントはSVMつかわない
          cout<<"評価値"<<eval_val_vector[j]<<",(loop_count-1)*goods_num+j:"<<(loop_count-1)*goods_num+j<<",値段:"<<goods_price_vector[(loop_count-1)*goods_num+j]<<endl;
          if(eval_val_vector[j]>goods_price_vector[(loop_count-1)*goods_num+j]){
             buf[j] = '1';
          }else{
             buf[j] = '0';
          }
          /*if((eval_val_vector[j]-SVM_expected_price[j])>0){
            buf[j] = '1';
          }else{
             buf[j] = '0';
          }*/
        }
      }
    }
    

    buf[j] = '\n';
    buf[j+1] = '\0';
    write(s, buf, strlen(buf));

    std::string tmp_buffer,tmp_buffer2,tmp_goods_price,tmp_client_log,tmp_client_goods_log;
    int goods_price_int;

  //サーバーからの情報をファイルに出力する。
    read_size = read(s, buf, BUF_LEN);
    if ( read_size > 0 ){
      write(1, buf, read_size);
      buf[read_size] = '\0';
      cout<<"buf:"<<buf<<endl;
      //buffにはg1:3 g2:1 g3:1 g4:2 g5:3 g6:1 g7:1 g8:2 g9:3 g10:1 
      // if (buf==("end")) break;
      if (std::string(buf).find("end") != std::string::npos)
         break;
      tmp_buffer=buf;
      for (k=0;k<goods_num;k++){
        int colon_loc=tmp_buffer.find(":",0);//colon_loc:コロンの位置 	
        //find()関数:現在の文字列のインデックス 番目の文字から検索を開始し、最初に文字列 が現れた場所を返す
        tmp_goods_price=tmp_buffer.substr(colon_loc+1,2); //コロンの次2桁の数字（商品の値段）
        // cout<<"tmp_goods_price商品の値段g["<<i<<"]:"<<tmp_goods_price<<endl;
        goods_price_vector.push_back(atoi(tmp_goods_price.c_str()));//商品の値段データの追加
        tmp_buffer=tmp_buffer.substr(colon_loc+1);//コロン以降の文字列
        //cout<<"tmp_buffer:"<<tmp_buffer<<endl;

        //ファイルに商品の値段データを書き込み
        ofs << atoi(tmp_goods_price.c_str());
      }
    }
    ofs<<endl;

    //サーバーからの情報を読み取り
    read_size = read(s, buf, BUF_LEN);

    if ( read_size > 0 ){
      write(1, buf, read_size);
      buf[read_size] = '\0';
      cout<<"buf:"<<buf<<endl;
      //bufの文字列がendの時、bufを読み進めるとエラーが発生する
      if (std::string(buf).find("end") != std::string::npos)
         break;
      //buffにはg1:1 g2:1 g3:1 g4:1 g5:1 g6:1 g7:1 g8:1 g9:1 g10:1 a1:1101110111 a2:1110111011
      //a1:???,a2:???を認識

      tmp_buffer2=buf;
      //bufの前半のgの情報はけずる。
      int a_loc=tmp_buffer2.find("a",0);//aの位置 
      tmp_buffer2=tmp_buffer2.substr(a_loc);//a以降の文字列
      //tmp_bufferは a1:1101110111 a2:1110111011　..
      for (i=0;i<client_num;i++){
        int colon_loc2=tmp_buffer2.find(":",0);//コロンの位置         	
        tmp_client_log=tmp_buffer2.substr(colon_loc2+1,goods_num); //コロンの次の数字(goods_num桁)はそれぞれのクライアントの入札データ

        for (int j=0;j<goods_num;j++){//一人のクライアントのそれぞれの商品に対する入札をよむ
          tmp_client_goods_log=tmp_client_log.substr(j,1);//a[i]のクライアントのj番目の商品に対する入札データ (0 or 1)
          client_log_vector.push_back(atoi(tmp_client_goods_log.c_str()));//クライアント入札データの追加
          //ofs2ファイルにクライアントiの商品jの入札データを書き込み
          ofs2 << atoi(tmp_client_goods_log.c_str());
        }
        tmp_buffer2=tmp_buffer2.substr(colon_loc2+1);//コロン以降の文字列
      }
    }
    ofs2<<endl;
    //printf("ここで次の入力待ち&SVM run");
    //このクライアントはSVMつかわない
    /*if(is_firstday==0){
      SVM();
    }*/
    loop_count++;
    cout<<"loop_count:"<<loop_count<<endl;

  }//end while  

  //(g1_price,g2_price,1_-1) g1とg2の値段の組あわせがあり、a1がg1に対して入札を行うかを判断したデータ
  //出力先ファイルの作成(SVMで読み込む)
  for(int temp_loop =0;temp_loop<loop_count;temp_loop++){
    for(int temp_client =0;temp_client<client_num;temp_client++){
      for(int temp_goods=0;temp_goods<goods_num;temp_goods++){
        std::stringstream ss;
        ss << "./kadai4_goods_client_log/output_goods" << temp_goods+1 <<"_client" <<temp_client+1<<".dat";
        cout << "output" << temp_goods+1 <<"_" <<temp_client+1<<".dat"<<endl;
        std::string result_outputfile =ss.str();//.str() でstring型の値を得る
        std::ofstream ofs4(result_outputfile.c_str(),std::ios::app);
        //商品tempの値段  １or-1
        if(client_log_vector[temp_goods+temp_client*goods_num+client_num*goods_num*temp_loop]){//入札するが１のとき:client_log_vector は、0、1で判別ではなく、1,-1で判別
          ofs4 <<goods_price_vector[temp_loop*goods_num+temp_goods] <<" 1"<<endl;
        }else{//入札しない＝０のとき
          ofs4 <<goods_price_vector[temp_loop*goods_num+temp_goods] <<" -1"<<endl;
        }
        cout<<temp_loop*goods_num+temp_goods<<":"<<goods_price_vector[temp_loop*goods_num+temp_goods] <<"&"<<temp_goods+temp_client*goods_num+client_num*goods_num*temp_loop<<":"<<client_log_vector[temp_goods+temp_client*goods_num+client_num*goods_num*temp_loop]<<endl;
      }
    }

  }
  close(s);
  return 0;
}


//SVMのコンストラクタの定義
//SVM::SVM(){

int SVM(){

  int m=0;
  int alpha_max_number=1; 
  int i=0;
  int n = DATA_NUM;

  double G[MATRIX_DIM][MATRIX_DIM], g0[MATRIX_DIM], 
  CE[MATRIX_DIM][MATRIX_DIM], ce0[MATRIX_DIM], 
  CI[MATRIX_DIM][MATRIX_DIM], ci0[MATRIX_DIM];

  double Kernel;
  struct Dataset data[DATA_NUM];
  double weight[2]={};
  double theta=0;
  double alpha[MATRIX_DIM]={};
  


//ファイル読み込み
for(int temp_client =0;temp_client<CLIENT_NUM;temp_client++){
  //自分については評価しない
  if(temp_client== client_id){
    break;
  }


  for(int temp_goods=0;temp_goods<GOODS_NUM;temp_goods++){
    std::stringstream ss;
    ss << "./kadai4_goods_client_log/output_goods" << temp_goods+1 <<"_client" <<temp_client+1<<".dat";
    std::string result_outputfile =ss.str();//.str() でstring型の値を得る
    cout<<"LOADED FILE:"<<result_outputfile<<endl;
    std::ifstream ifs(result_outputfile.c_str(),std::ios::app);
    string str="";
    if(ifs.fail()) {
      cout<<"File output_goods_client_.dat do not exist.\n"<<endl;
      cerr << "File do not exist.\n";
      exit(0);
    }
 
    while(getline(ifs, str)) {
      if(i==DATA_NUM){
        cout<<"getline break"<<endl;
        break;
      }
      data[i].input_first=0; data[i].input_second=0; data[i].y=0;
      sscanf(str.data(), "%lf %lf", &data[i].input_first,  &data[i].y);
    //printf("%lf %lf %lf", data[i].input_first, data[i].input_second, data[i].y);
    //cout<<"data[i].input_first:"<<data[i].input_first<< ",data[i].input_second:"<< data[i].input_second<<",data[i].y:"<<data[i].y<<endl;
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
    //  cout<<"g0["<<i<<"]="<<g0[i] <<endl;
      }
    }
    m = 1;
    {//CEは(y1 y2 .. yn)
      for (int i = 0; i < n; i++){
        for(int j=0;j<n;j++){
          CE[i][0] =data[i].y;
    //  cout<<"CE["<<i<<"]["<<j<<"]="<<CE[i][j]<<endl;
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
    try{
      //例外が発生する可能性のあるコード
      solve_quadprog(G, g0,100,  CE, ce0,1, CI, ci0,100, alpha);
    } catch (const std::exception& ex) {
      std::cerr << "solve_quadprog_failed" << ex.what() << std::endl;
      throw;
    }

    //出力
    //cout<<"alpha"<<endl;
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
   // cout<<"weight[1]="<<weight[1]<<endl;

    //alpha_max_numberはalphaが最大の番号
    //cout<<"alpha_max_number"<<alpha_max_number<<endl;
    //カーネルは内積
    Kernel=(weight[0]*data[alpha_max_number].input_first+weight[1]*data[alpha_max_number].input_second);

    theta=Kernel-data[alpha_max_number].y;
    cout<<"theta="<<theta<<endl;

    //SVM_expected_priceは、商品がtemp_goodsに対する相手の予算を予想した値
    SVM_expected_price[temp_goods]=theta/weight[0];
    cout<<"SVM_expected_price:["<<temp_goods<<"]:"<<SVM_expected_price[temp_goods]<<endl;

    }//finish loop of one goods
  }//finish loop of one client
}