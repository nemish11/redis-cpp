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

    return 0;
}

