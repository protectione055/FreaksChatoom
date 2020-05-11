# FreaksChatoom
a multi-user chatroom

基于epoll做的垃圾聊天室，只有命令行界面，所以非常搓。以后学QT的时候再整整容吧。

**运行环境**
Linux环境下运行，编译时加上-lpthread。
server: gcc server.c chatroom.c -o server -lpthread
client: gcc client.c chatroom.c -o client -lptrhead
