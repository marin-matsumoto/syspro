#define m 2
#define n 2


#include <stdlib.h>
#include <pthread.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(char *msg)
{
 perror(msg);
 exit(1);
}
int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int s;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    }
        //bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = atoi(argv[1]); //受け取ったポート番号をint型にする
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        error("ERROR on binding");
    }

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
        error("ERROR on accept");
    }

    int i=0;
    for(i=0;i<256;i++){
        buffer[i]=0;
    }
 //bzero(buffer,256);
    int g_n[8];
    for(i=0;i<8;i++){
        s = read(newsockfd,buffer,sizeof(int));
        g_n[i] = atoi(buffer);
        printf("%d\n",g_n[i]);
        if (s < 0) error("ERROR reading from socket");

    }
    gyoretsu(g_n[0],g_n[1],g_n[2],g_n[3],g_n[4],g_n[5],g_n[6],g_n[7], newsockfd); //行列計算をさせる


    //printf("Here is the message: %s\n",buffer);//clientから受け取ったメッセージを表示

    //n = write(newsockfd,"I got your message",18);

    //if (n < 0) error("ERROR writing to socket");

    close(newsockfd);
    close(sockfd);
    return 0;
}


/*---------------------------------------行列計算-----------------------------------------*/
double **a, **b, **c;
//適切な形に書き換える
void* mulmat(void* arg) {
    int id = *((int*)arg);//ポインター型から中身をみる
    int j = id%m;//列
    int i = (id-j)/m;//行
    c[i][j] = 0;

    for (int k = 0; k < n; k++) {
      c[i][j] += a[i][k] * b[k][j];
    }
    pthread_exit(NULL);
}

int gyoretsu(int a1,int a2,int a3,int a4,int b1,int b2,int b3,int b4, int newsockfd) {
    //サーバのプログラムとクライアントのプログラムはすでに繋がっているのでファイルディスクリプタを指定すればmain関数でなくても
    //クライアントと接続できる
  int id[m*m];
  for(int i=0;i<m*m;i++){
    id[i]=i;
  }
    pthread_t t[m*m]; //スレッドを準備するための箱
    void* result; //結果
    //abcのメモリを確保すること
    a = malloc(m*sizeof(double*));
    b = malloc(n*sizeof(double*));
    c = malloc(m*sizeof(double*));

    for(int i=0;i<m;i++){
      a[i] = malloc(n*sizeof(double));
    }

    for(int i=0;i<n;i++){
      b[i] = malloc(m*sizeof(double));
    }

    for(int i=0;i<m;i++){
      c[i] = malloc(m*sizeof(double));
    }

    a[0][0] = a1; a[0][1] = a2;
    a[1][0] = a3; a[1][1] = a4;

    b[0][0] = b1; b[0][1] = b2;
    b[1][0] = b3; b[1][1] = b4;

    //2. thread

    for (int i = 0; i < m; i++) {
      for (int j = 0; j < m; j++) {
        int temp = pthread_create(&(t[i*m+j]),NULL,mulmat,&id[i*m+j]);
        if(temp != 0){ //
            printf("error\n");
          }
      }
    }


   //3. wait
   for (int i = 0; i < m*m; i++) {

       pthread_join(t[i],&result); //funcの処理待ちこれがないとmatrix[i]に何も入っていないまま

   }

   //4. check
   for (int i = 0; i < m; i++) {
     for (int j = 0; j < m; j++) {
       printf("c[%d][%d]=%f\n",i+1,j+1,c[i][j]);
       write(newsockfd,&(c[i][j]),sizeof(double));
     }
   }


  //5. free 順番が重要
   for(int i=0;i<m;i++){
     free(a[i]);
   }

   for(int i=0;i<n;i++){
     free(b[i]);
   }

   for(int i=0;i<m;i++){
     free(c[i]);
   }
   free (a);
   free (b);
   free (c);

    return 0;
}
