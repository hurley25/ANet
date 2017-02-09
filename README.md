# ANet
基于Redis网络模型的简易网络库，网络模块代码取自Redis源码。


### Redis网络模型介绍

Redis网络模型是一个使用**IO多路复用**、**单线程**、**非阻塞**的模型。这个模型的优点在于单线程不用考虑加锁，如果在单核环境上可以将效率发挥到最大。


### 如何启动一个服务器

- 通过`aeCreateEventLoop`创建核心`aeEventLoop`
- 通过`anetTcpServer`完成`socket() bind() listen()`
- 通过`aeCreateFileEvent`给`fd`注册相应的事件
- `aeMain`循环检测每个`fd`是否有事件发生，如果有就交给相应的读/写处理程序。



### 附：各个文件介绍


|文件|作用|
|---|---|
|ae.c ae.h ae_epoll.c ae_select.c|Redis事件处理器的实现（Redis源码）|
|anet.c  anet.h|Redis网络库的实现（Redis源码）|
|buffer.c  buffer.h|自行实现的buffer|
|protocol.c  protocol.h|自行定义协议|
|define.h|一些常量，比如listen的backlog大小|
|server.c  server_test.c|自定义的服务端和客户端程序|
