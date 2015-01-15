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

#define BUF_LEN 1024                      // バッファのサイズ 

int main(int argc, char *argv[]){
  int s;  /* ソケットのためのファイルディスクリプタ */
  int client_id;
  size_t goods_num,client_num;
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
  //標準入力を使わずにサーバーに名前(randomな１０桁の数字)を送信
  int i=0;
  /* for (i=0;i<10;i++){
    buf=rand() %10000;
    printf("buf[%d]=%d",i,buf[i]);
  }
  buf[i] = '\n';
  printf("buf[%d]=%d",i,buf[i]);
  buf[i+1] = '\0';
  printf("buf[%d]=%d",i+1,buf[i+1]);
  printf("最初の自動入札（全てに1=入札）");  
  */
  // buf=rand() %10000;
  char str;
  sprintf(str, "%d", rand() %10000);

   fgets(buf, BUF_LEN, str);
  //  fgets(buf, BUF_LEN, stdin);
  printf("名前送信された。全てのクライアントが入力し終わる同時にyour id:とクライアント数,商品数表示された\n");
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
  sscanf(buf, "%*[^0-9]%d", &client_id);
  printf("got your client_id:%d\n",client_id);
  //sscanfは要はscanfの入力がキーボードからではなく文字列strになる
  //sscanf(const char *str入力元の文字列, const char *format書式指定文字列,格納する引数)
  //0-9までの%d(10進数int型)
  //^は"YOUR ID:"に相当し、捨てる

  // "n,m", n は商品数 m はクライアント合計数
  if ((read_size = read(s, buf, BUF_LEN)) > 0)
    write(1, buf, read_size);
  buf[read_size] = '\0';
  sscanf(buf, "%zu%*[^0-9]%zu", &goods_num, &client_num);
  printf("got goods_num:%zu,client_num:%zu\n",goods_num,client_num);
  



  puts("最初の入札情報を自動で入力（全て１）");
  
  //それぞれの商品について入札するかどうか。各商品に対して入札する場合は1，しない場合は0
  
  //標準入力でサーバーに情報を送信
  //fgets(buf, BUF_LEN, stdin);

  //標準入力を使わずにサーバーに情報を送信
 
  for (i=0;i<goods_num;i++){
    buf[i] = '1';
    printf("buf[%d]=%d",i,buf[i]);
  }
  buf[i] = '\n';
  printf("buf[%d]=%d",i,buf[i]);
  buf[i+1] = '\0';
  printf("buf[%d]=%d",i+1,buf[i+1]);
  printf("最初の自動入札（全てに1=入札）");
  
  write(s, buf, strlen(buf));//ソケットにつながるsにbufをstrlenバイト書き込む
  //  printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyy");
   
  read_size = read(s, buf, BUF_LEN);
  if ( read_size > 0 ){
    write(1, buf, 
	  read_size);
  }
  //printf("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");

  printf("最初の入札として全てに１が入力された。");
  /* 送受信ループ */
  /* 標準入力をそのまま送信、入札結果と価格を受信して表示 */
  while (1) {
    printf("press enter?");
    //ここで標準入力ではなく代入する。
    char str[256];
    fgets(str, 256, stdin);
    printf("str:%s",str);
    //  if(){
      int i=0;
      for (i=0;i<goods_num;i++){


   
	if(rand()%2==0){
	  buf[i] = '0';
	  printf("rand mod 2==0");
	}else if(rand()%2==1){
	  buf[i] = '1';
	  printf("rand mod 1==0");
	}else{
	  printf("oops:%d:",rand()%2);
	}
	printf("buf[%d]=%d",i,buf[i]);
      }
      buf[i] = '\n';
      printf("buf[%d]=%d",i,buf[i]);
      buf[i+1] = '\0';
      printf("buf[%d]=%d",i+1,buf[i+1]);
      //  }

    write(s, buf, strlen(buf));
    printf("2222222222222222222222222222222222");
    read_size = read(s, buf, BUF_LEN);
    if ( read_size > 0 ){
      write(1, buf, read_size);
    }
    printf("33333333333333333333333333333333");
    read_size = read(s, buf, BUF_LEN);
    printf("444444444444444444444444444444");
    if ( read_size > 0 ){
      write(1, buf, read_size);
      printf("ここで次の入力待ち");
    }
  }

  close(s);

  return 0;
}
