# チャット形式の行列計算
## クライアントから渡された行列の積をサーバが計算しクライアントに返す

```
my_app_client.c   # クライアント側
my_app_server.c   # サーバ側

```

##実行方法
```
必ずサーバ側から実行
$ ./my_app_server <ポート番号>
$ ./my_app_client <IPアドレス> <ポート番号>
例）
$ ./my_app_server 1111
$ ./my_app_client 127.0.0.1 1111
```