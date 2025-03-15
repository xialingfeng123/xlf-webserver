#  xlf-webserver: 基于 C++ 的高性能 HTTP 服务器  
>  轻量、高效、多线程，支持 MySQL 认证的 HTTP 服务器

##  项目介绍  
`xlf-webserver` 是一个基于 **C++11** 及 **Linux epoll** 的高性能 HTTP 服务器，采用**单 Reactor + 多线程**的架构，支持**学生信息管理**，包括 **用户登录、个性签名修改** 等功能，并集成了 **MySQL 进行数据存储**,支持大量用户并发访问

 **支持功能**：
-  **基于 非阻塞Socket + Epoll ET + 线程池** 处理高并发请求  
-  **完整的 HTTP 解析**，支持 GET 和 POST 请求  
-  **访问 MySQL 数据库**  实现web端用户的登录，查询和修改数据的功能
-  **支持 HTML 页面渲染**，提供前端交互界面  
-  **压力测试 QPS 高达 24,000**，可支持 **4000+ 并发连接**  

---

##  项目结构
```plaintext
├── WebServer.h/.cpp        # HTTP 服务器核心
├── ThreadPool.h/.cpp       # 线程池
├── HttpParser.h/.cpp       # HTTP 解析
├── MySQLConnPool.h/.cpp    # MySQL 连接池
├── StudentDB.h/.cpp        # 业务逻辑（学生信息管理）
├── main.cpp                # 入口文件
├── index.html              # 前端页面（用户登录 & 修改个性签名）
├── Makefile                # 编译脚本
└── README.md               # 说明文档（本文件）
```

---

##  快速运行

###  1. 安装依赖
#### 1.1 安装 g++ 和 CMake
```bash
sudo apt update
sudo apt install g++ make cmake -y
```

####  1.2 安装 MySQL
```bash
sudo apt install mysql-server -y
sudo systemctl start mysql
```

####  1.3 创建数据库
```sql
CREATE DATABASE student_db;
USE student_db;
CREATE TABLE students (
    student_id VARCHAR(20) PRIMARY KEY,
    password VARCHAR(50) NOT NULL,
    signature TEXT DEFAULT ''
);
```

---

###  2. 编译 & 运行

####  2.1 编译项目
```bash
make
```

#### 2.2 运行服务器
```bash
./webserver
```
---

##  性能测试
使用 `webbench` 进行压力测试：
```bash
webbench -c 4000 -t 10 http://127.0.0.1:8080/index.html
```
**测试结果（4000 并发）：**  
![1](https://github.com/user-attachments/assets/3f7d7040-b68c-4383-bf6c-87e97d5d48a4)

服务器性能优秀，可稳定支持 **4000+ 并发**，QPS达到 **24000+**。

---

## 关键优化
###  1. 使用 epoll ET（边缘触发）
- 采用 `epoll ET` 结合 **非阻塞 I/O**，减少不必要的系统调用，提高吞吐量  
- 解决了 `epoll LT` 下 **EPOLLIN 反复触发的 bug**  

###  2. 线程池优化
- 采用 **固定大小线程池**，减少频繁创建/销毁线程的开销  
- **任务队列** 避免过载，防止服务器崩溃  

###  3. 数据库连接池
- 复用 MySQL 连接，避免每次请求都 `mysql_connect()`  
- 减少 **数据库连接的开销**，提升查询效率  

---

##  相关链接
- **GitHub 仓库**：[xlf-webserver](https://github.com/xialingfeng123/xlf-webserver)
- **作者**：[xialingfeng123](https://github.com/xialingfeng123)

---
