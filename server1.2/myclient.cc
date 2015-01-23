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
#include    <iomanip>
using namespace std;

#define BUF_LEN 1024                      // バッファのサイズ 

int main(int argc, char *argv[]){
  int s;  /* ソケットのためのファイルディスクリプタ */
  int client_id;
  int goods_num,client_num;
  struct hostent *servhost;            // ホスト名と IP アドレスを扱うための構造体 
  struct sockaddr_in server;           // ソケットを扱うための構造体 
  struct servent *service;             // サービス (http など) を扱うための構造体 
  char buf[BUF_LEN];
  int read_size;

  char *host;
  //char host[BUF_LEN] = "127.0.0.1";    // 接続するホスト名 
  char path[BUF_LEN] = "/";            // 要求するパス 
  unsigned short port = 5000;             // 接続するポート番号 

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
  read_size = read(s, buf, BUF_LEN);//sにつながっているファイルからBUF_LENバイト読み込みbuf格納
  if ( read_size > 0 ){
    write(1, buf, read_size);
  }

  printf("ここでサーバーからPLEASE INPUT YOUR NAMEと表示される.名前を入力してエンター");
  
  int i=0,j=0,k=0,l=0;
  
  fgets(buf, BUF_LEN, stdin);
  printf("名前送信された。全てのクライアントが入力し終わる同時にyour id:とクライアント数,商品数表示される\n");
  write(s, buf, strlen(buf));





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
  //sscanfは要はscanfの入力がキーボードからではなく文字列strになる
  //sscanf(const char *str入力元の文字列, const char *format書式指定文字列,格納する引数)
  //0-9までの%d(10進数int型)
  //^は"YOUR ID:"に相当し、捨てる

  // "n,m", n は商品数 m はクライアント合計数
  read_size = read(s, buf, BUF_LEN);
  if (read_size > 0)
    write(1, buf, read_size);
  buf[read_size] = '\0';
  sscanf(buf, "%d%*[^0-9]%d", &goods_num, &client_num);//%*[^0-9]は数字が始まるまで読み飛ばす
  printf("got goods_num:%d,client_num:%d\n",goods_num,client_num);
  



  puts("最初の入札情報を自動で入力（全て１）");
  
  //それぞれの商品について入札するかどうか。各商品に対して入札する場合は1，しない場合は0
  
  //標準入力でサーバーに情報を送信
  //fgets(buf, BUF_LEN, stdin);

  //標準入力を使わずにサーバーに情報を送信

  for (i=0;i<goods_num;i++){
    buf[i] = '1';
    //printf("buf[%d]=%d",i,buf[i]);
  }
  buf[i] = '\n';
 // printf("buf[%d]=%d",i,buf[i]);
  buf[i+1] = '\0';
 // printf("buf[%d]=%d",i+1,buf[i+1]);
  printf("最初の自動入札（全てに1=入札）");
  
  write(s, buf, strlen(buf));//ソケットにつながるsにbufをstrlenバイト書き込む

  read_size = read(s, buf, BUF_LEN);
  if ( read_size > 0 ){
    write(1, buf, 
     read_size);
  }

  printf("最初の入札として全てに１が入力された。");
  // 送受信ループ 
  // 標準入力をそのまま送信、入札結果と価格を受信して表示 
  // ファイル出力ストリームの初期化
  std::ofstream ofs("goods_price_log.dat",std::ios::app);
  std::ofstream ofs2("client_log.dat",std::ios::app);
  std::ofstream ofs3("g1_g2_tender.dat",std::ios::app);
  std::vector  <int>  goods_price_vector;
  std::vector <int>  client_log_vector;
  int loop_count=0;

  

  while (1) {
    //printf("0から４まですきな数字を入力");
    //入札情報を登録
    char str[256];
    //fgets(str, 256, stdin);
    //printf("入力されたものは:%s\n",str);

   // int version = atoi(str);
    //printf( "version=%d" , version );
    
    for (j=0;j<goods_num;j++){
     buf[j] = '1';
     
    //printf("buf[%d]=%d",i,buf[i]);
   }
   buf[j] = '\n';
  //printf("buf[%d]=%d",i,buf[i]);
   buf[j+1] = '\0';
  //printf("buf[%d]=%d",i+1,buf[i+1]);


   write(s, buf, strlen(buf));


    //char key_g[256];
    //char goods_price[256];
  // std::vector  <int>  goods_price_vector;
   //std::vector <int>  client_log_vector;

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
    if (std::string(buf).find("end") != std::string::npos) break;
    tmp_buffer=buf;
    for (k=0;k<goods_num;k++){
	//	cout<<"tmp_buffer:"<<tmp_buffer<<endl;
	int colon_loc=tmp_buffer.find(":",0);//コロンの位置 
	//cout <<"colon_loc:"<<colon_loc<<endl;
	
	tmp_goods_price=tmp_buffer.substr(colon_loc+1,1); //コロンの次の数字（商品の値段）
	//cout<<"tmp_goods_price商品の値段g["<<i<<"]:"<<tmp_goods_price<<endl;
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
 // if (buf=="end") break;
  if (std::string(buf).find("end") != std::string::npos) break;
      //buffにはg1:1 g2:1 g3:1 g4:1 g5:1 g6:1 g7:1 g8:1 g9:1 g10:1 a1:1101110111 a2:1110111011
      //a1:???,a2:???を認識

  tmp_buffer2=buf;
      //bufの前半のgの情報はけずる。
      int a_loc=tmp_buffer2.find("a",0);//aの位置 
      tmp_buffer2=tmp_buffer2.substr(a_loc);//a以降の文字列
      //tmp_bufferは a1:1101110111 a2:1110111011　..
      for (i=0;i<client_num;i++){

             //  cout<<"tmp_buffer:"<<tmp_buffer<<endl;
        	int colon_loc2=tmp_buffer2.find(":",0);//コロンの位置 
        	//cout <<"colon_loc:"<<colon_loc<<endl;
        	tmp_client_log=tmp_buffer2.substr(colon_loc2+1,goods_num); //コロンの次の数字(goods_num桁)はそれぞれのクライアントの入札データ
        	//cout<<"tmp_client_log:a["<<i<<"]:"<<tmp_client_log<<endl;//ex tmp_client_log:a[0]:1101110111

        	
        	for (int j=0;j<goods_num;j++){//一人のクライアントのそれぞれの商品に対する入札をよむ
        	  tmp_client_goods_log=tmp_client_log.substr(j,1);//a[i]のクライアントのj番目の商品に対する入札データ (0 or 1)
        	  client_log_vector.push_back(atoi(tmp_client_goods_log.c_str()));//クライアント入札データの追加
        	  //ofs2ファイルにクライアントiの商品jの入札データを書き込み
        	  //毎回毎回改行されてしまっている
        	  //最初の１回の入札情報がとれてない。
        	  ofs2 << atoi(tmp_client_goods_log.c_str());
        	}
        	 tmp_buffer2=tmp_buffer2.substr(colon_loc2+1);//コロン以降の文字列

         }
       }
       ofs2<<endl;
       printf("ここで次の入力待ち");
       loop_count++;
     }//end while

//(g1_price,g2_price,1_-1)g1とg2の値段の組あわせがあり、a1がg1に対して入札を行うかを判断したデータ
     for (int r= 0; r < goods_num;r++) {
     //   for (int s = 0; s < goods_num;s++) {

      //ofs3 << "price:"<<goods_price_vector[r]<< " ";
      //cout << "price:"<<goods_price_vector[r]<< " ";

     }

     for (int r= 0; r < client_num;r++) {
     //ofs3 <<"client" <<client_log_vector[r]<< " ";
     //cout<<"client" <<client_log_vector[r]<< " ";
     }

           //出力先ファイルの作成(SVMで読み込む)
     for(int temp_loop =0;temp_loop<loop_count;temp_loop++){
        for(int temp_client =0;temp_client<client_num;temp_client++){
            for(int temp_goods=0;temp_goods<goods_num;temp_goods++){
            std::stringstream ss;
            ss << "output_goods" << temp_goods+1 <<"_client" <<temp_client+1<<".dat";
            cout << "output" << temp_goods+1 <<"_" <<temp_client+1<<".dat"<<endl;

              std::string result_outputfile =ss.str();//.str() でstring型の値を得る
              std::ofstream ofs4(result_outputfile.c_str(),std::ios::app);
              //商品tempの値段  １or-1
              ofs4 <<goods_price_vector[temp_loop*goods_num+temp_goods] <<" "<<client_log_vector[temp_goods+temp_client*goods_num+client_num*goods_num*temp_loop]<<endl;

              cout<<temp_loop*goods_num+temp_goods<<":"<<goods_price_vector[temp_loop*goods_num+temp_goods] <<"&"<<temp_goods+temp_client*goods_num+client_num*goods_num*temp_loop<<":"<<client_log_vector[temp_goods+temp_client*goods_num+client_num*goods_num*temp_loop]<<endl;
          }
        }
      }

      cout<<"test"<<endl;
      for (int test_int=0;test_int<10;test_int++){

        //cout<<goods_price_vector[test_int]<<"&"<<client_log_vector[test_int]<<endl;

      }

      // ofs4 << std::endl;


      ofs3 << std::endl;









      close(s);

      return 0;
    }
