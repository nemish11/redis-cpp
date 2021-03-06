
# redis-cpp
A custom implementation for [Redis](https://redis.io/) using C++ and AVL Tree .
<h2> Table Of Content </h2>

1. [ Environment ](#enviro)
2. [ Description. ](#desc)
3. [ Usage tips. ](#usage)
4. [ Implementation Details. ](#implementation)
5. [ Testing Details. ](#testing)
6. [ Command Line Interface. ](#command)

<a name="enviro"></a>
## 1. Environment
Language: C++ 11  <br/>
OS: macOS Catalina 10.15.3 </br>
Tools: VS code (1.41.1) 

<a name="desc"></a>
## 2. Description

Implemented following Redis commands:

1) [GET](https://redis.io/commands/get): Get the value of `key`
2) [SET](https://redis.io/commands/set): Set `key` to hold the string `value`
3) [EXPIRE](https://redis.io/commands/expire): Set a timeout on `key`. After the timeout has expired, the key will automatically be deleted
4) [ZADD](https://redis.io/commands/zadd): Adds all the specified members with the specified scores to the sorted set stored at `key`
5) [ZRANK](https://redis.io/commands/zrank): Returns the rank of `member` in the sorted set stored at `key`, with the scores ordered from low to high
6) [ZRANGE](https://redis.io/commands/zrange): Returns the specified range of elements in the sorted set stored at `key`
7) [ZREVRANK](https://redis.io/commands/zrevrank): Returns the rank of `member` in the sorted set stored at `key`, with the scores ordered from high to low
8) [ZCARD](https://redis.io/commands/zcard): Returns the sorted set cardinality (number of elements) of the sorted set stored at `key`.
9) [ZSCORE](https://redis.io/commands/zscore): Returns the score of `member` in the sorted set at `key`


<a name="usage"></a>
## 3. Usage tips

Step 1: Clone Repo

    git clone https://github.com/nemish11/redis-cpp

Step 2: Compile and Run client.cpp file

    g++ --std=c++11 client.cpp
    ./a.out

Step 3: Change content of client.cpp and perform step 2


>Note: Threading is used to clear expired keys. In the case of segmentation fault, running the code twice will create the thread.


<a name="implementation"></a>
## 4. Implementation Details

To manage sorted set of keys AVL tree is implemented. <br/>
AVL support insert, delete, getRank all operations in `O(logN)`. </br>

In Case of segmentation fault, comment following code in `redis.h`:

    Redis() 
    {        
        threadStatus = true;  // set thread status
        t1 = thread(&Redis::freeExpiredKey, this); // start thread for cleaning expire key 
        t1.detach(); // run thread in background
    }
    ~Redis()
    { 
        threadStatus = false; // complete execution of thread
        t1.~thread();
    } 


<a name="testing"></a>
## 5. Testing Details

`test_redis.cpp` contains unit test cases for each command. To run test cases enter below commands: <br>
    
    g++ --std=c++11 test_redis.cpp
    ./a.out

testGET( ) method will test GET command. <br/>
same naming convension for other commands. <br/>
New Redis instance will be created per method.

<a name="command"></a>
## 6. Command Line Interface

To use CLI run following commands:
    
    g++ --std=c++11 command_line_interface.cpp
    ./a.out

Type HELP command to check usage of each command.