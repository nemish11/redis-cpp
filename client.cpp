#include "redis.h"
using namespace std;


int32_t main()
{   
    #ifndef ONLINE_JUDGE
        freopen("input.txt", "r", stdin);
        //freopen("output.txt", "w", stdout);
    #endif
    
    Redis redis;

    cout<< redis.GET("key") <<endl;

    cout<< redis.SET("key", "value") <<endl;

    cout<< redis.GET("key") <<endl;

    cout<< redis.ZADD("key1",1, "aaa") <<endl;

    cout<< redis.ZADD("key1","CH", 2, "aab")<<endl;

    cout<< redis.ZSCORE("key1", "aaa")<<endl;
    return 0;
}

