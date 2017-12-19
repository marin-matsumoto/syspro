#define m 2
#define n 2

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
void error(char *msg)
{
 perror(msg);
 exit(0);
}
int main(int argc, char *argv[]){
 int sockfd, portno, s;
 struct sockaddr_in serv_addr;
 struct hostent *server;
 char buffer[256];

 if (argc < 3) { //プログラムの引数が３より少なっかったら
   fprintf(stderr,"引数が足りません。usage %s hostname port\n", argv[0]);
   exit(0);
 }

 portno = atoi(argv[2]); //serverと同様に

  /* ソケットを生成 */
 sockfd = socket(AF_INET, SOCK_STREAM, 0); 

 if (sockfd < 0)
   error("ERROR opening socket");

   server = gethostbyname(argv[1]);

 if (server == NULL) {
   fprintf(stderr,"ERROR, no such host¥n");
   exit(0);
 }
 
   /*

   * client_addr構造体に、接続するサーバのアドレス・ポート番号を設定

   */

 //bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
   serv_addr.sin_port = htons(portno);
 if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
   error("ERROR connecting");
   
   bzero(buffer,256);
   
   printf("行列計算のプログラムを開始します。\n");

   /*---------------------行列の入力------------------------------*/
   
   for(int i=1;i<m+1;i++){
        for (int j = 1; j < n+1; ++j){
            printf("a[%d][%d] = ",i,j );
            fgets(buffer,255,stdin);
            s = write(sockfd,buffer,strlen(buffer));
           if (s < 0)
             error("ERROR writing to socket");
            bzero(buffer,256);
        }     
           
    }

 
   for(int i=1;i<m+1;i++){
        for (int j = 1; j < n+1; ++j){
            printf("b[%d][%d] = ",i,j );
            fgets(buffer,255,stdin);
            s = write(sockfd,buffer,strlen(buffer));
           if (s < 0)
             error("ERROR writing to socket");
            bzero(buffer,256);
        }     
           
    }

    
    /*-------------------------結果の出力------------------------------*/

    printf("計算結果\n");
    for (int i = 1; i < m+1; ++i){
        for (int j = 1; j < n+1; ++j){
            double buffer2;
            read(sockfd,&buffer2,sizeof(double));
            printf("c[%d][%d] = %f\n",i,j, buffer2);
        }
       
    }

    /* ソケットをクローズ */
    close(socket);
 return 0;
}
