# Star_Tool
Star个人C++跨平台工具仓库，基于C++ 20 std，若版本低于此版本可能会造成部分代码报错问题，需要将部分库和头文件剔除
简介文档链接：
https://blog.csdn.net/qq_29322325/article/details/130715160?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22130715160%22%2C%22source%22%3A%22qq_29322325%22%7D

目前bug（待修补）：
（1）C++20以下无锁queue容器抛出错误问题

优化目标：
（1）完善和支持更多tcp、udp不同平台间flag标志位和不同读取模式的支持

更新日志：
日期                    更新内容
2023年5月21日            修复了C++20以下无锁queue容器抛出错误问题，完善了udp、tcp平台代码优化和功能的完善，总结了Linux和Windows运用的差异性并进行代码缩减，优化代码长度
2022年5月22日            为无锁容器和有锁容器queue和stack添加了原位构造方法emplace，修复和完善了无锁容器queue的内存泄漏问题，增强了其代码的线程安全性。

