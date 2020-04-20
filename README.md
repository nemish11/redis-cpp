# redis-cpp
<h2> Table Of Content </h2>

1. [ Environment ](#enviro)
2. [ Description. ](#desc)
3. [ Usage tips. ](#usage)
4. [ Implementation Details. ](#implementation)

<a name="enviro"></a>
## 1. Environment
Language: C++11  <br/>
OS: macOS Catalina 10.15.3 </br>
Tools: VS code (1.41.1) 

<a name="desc"></a>
## 2. Description

Implemented following Redis commands:

1) GET(https://redis.io/commands/get)
2) SET(https://redis.io/commands/set)
3) EXPIRE(https://redis.io/commands/expire)
4) ZADD(https://redis.io/commands/zadd)
5) ZRANK(https://redis.io/commands/zrank)
6) ZRANGE(https://redis.io/commands/zrange)

<a name="usage"></a>
## 3. Usage tips

step 1: Clone Repo. <br/>
step 2: Compile and Run client.cpp file. (g++ --std=c++11 client.cpp) <br/>
step 3: Change content of client.cpp and perform step 2. <br/>


<a name="implementation"></a>
## 4. Implementation Details

To manage sorted set of keys AVL tree is implemented. <br/>
AVL support insert, delete, getRank all operations in O(logN). </br>

In Case of segmentation fault, comment following code in "redis.h" <br/>
<br/>
Redis() <br/>
{        
    threadStatus = true;  // set thread status <br/>
    t1 = thread(&Redis::freeExpiredKey, this); // start thread for cleaning expire key <br/>
    t1.detach(); // run thread in background <br/>
} <br/>
<br/>
~Redis() <br/>
{ <br/>
    threadStatus = false; // complete execution of thread <br/>
    t1.~thread(); <br/>
} <br/>