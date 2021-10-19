#20210930 WebServer开发流程

@(网络编程实现)

## 知识点汇总

- [ ] 状态机解析HTTP请求
- [x] 线程池
- [ ] 支持长连接，定时回调处理超时连接
- [x] Reactor编程模型
- [x] 日志
- [ ] Webbench压力测试
- [ ] 数据库
- [ ] B/S模型

## 1、 线程池

基于muduo的思路：每个新到的连接（`connection`)分配一个线程，用于读请求、处理请求和应答。

## 2、 http类
这个类分配在heap上，指向该类的指针由`TcpConnection::context_`维护。http状态机的思路部分和https://mp.weixin.qq.com/s/BfnNl-3jc_x5WPrWEJGdzQ一致