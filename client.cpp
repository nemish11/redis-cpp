#include "redis.h"
using namespace std;
#define int long long int

int32_t main()
{   
    #ifndef ONLINE_JUDGE
        freopen("input.txt", "r", stdin);
        //freopen("output.txt", "w", stdout);
    #endif
    Redis r;
    cout<< r.SET("hello", "hii")<<endl;

    cout<< r.EXPIRE("hello", 1)<<endl;

    for(int i=0;i<7;i++)
    {
        cout<<r.GET("hello")<<endl;
    }
    cout<<r.GET("hello")<<endl;

    cout<<r.SET("jjj", "34", "PX", 5478);
    cout<<r.SET("jjj", "34", "PX", 5667);
    r.SET("hhh","gjjg");
    cout<<r.GET("hhh")<<endl;
    cout<<r.EXPIRE("hhh",1)<<endl;
    cout<<r.GET("hhh")<<endl;
    //thread t1(&Redis::free_memory, &r);
    //t1.detach();
    //test GET
    cout<<r.GET("hello")<<endl;
    
    //test SET
    cout<<r.SET("hello", "hii")<<endl;

    cout<<r.GET("hello")<<endl;
    cout<<r.ZADD("hello", 3, "aaa")<<endl;
    cout<<r.ZADD("myset", 3, "aaa")<<endl;
    cout<<r.ZADD("myset", 2, "aab")<<endl;
    cout<<r.ZADD("myset", 3, "aac")<<endl;
    cout<<r.ZADD("myset", 2, "aad")<<endl;
    cout<<r.ZADD("myset1", 1, "aaa")<<endl;
    cout<<r.ZADD("myset1", 1, "aav")<<endl;
    cout<<endl;
    cout<<r.ZRANK("myset", "aaa")<<endl;
    cout<<r.ZRANK("myset", "aac")<<endl;
    cout<<r.ZRANK("myset1", "aaa")<<endl;
    cout<<r.ZRANK("myset1", "aav")<<endl;

    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            vector<string>ok = r.ZRANGE("myset",i,j);
            cout<<i<<" "<<j<<" : ";
            for(auto x:ok)
            {
                cout<<x<<" ";
            }
            cout<<endl;
        }
    }

    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            vector<string>ok = r.ZRANGE("myset1",i,j);
            cout<<i<<" "<<j<<" : ";
            for(auto x:ok)
            {
                cout<<x<<" ";
            }
            cout<<endl;
        }
    }

    cout<<r.ZADD("myset", 2, "aaa")<<endl;
    cout<<r.ZADD("myset", 3, "aab")<<endl;
    cout<<r.ZADD("myset", 4, "aac")<<endl;
    // r.ZADD("myke",1,"ff");
    // cout<< r.GET("myke")<<endl;
    cout<<endl;
    cout<<r.ZRANK("myset", "aaa")<<endl;
    cout<<r.ZRANK("myset", "aac")<<endl;
    cout<<r.ZRANK("myset1", "aaa")<<endl;
    cout<<r.ZRANK("myset1", "aav")<<endl;

    // for(int i=0;i<4;i++)
    // {
    //     for(int j=0;j<4;j++)
    //     {
    //         vector<pair<string,int>>ok = r.ZRANGE("myset",i,j,"WITHSCORES");
    //         cout<<i<<" "<<j<<" : ";
    //         for(auto x:ok)
    //         {
    //             cout<<x.first<<" "<<x.second<<" ";
    //         }
    //         cout<<endl;
    //     }
    // }

    // for(int i=0;i<4;i++)
    // {
    //     for(int j=0;j<4;j++)
    //     {
    //         vector<pair<string,int> >ok = r.ZRANGE("myset1",i,j,"WITHSCORES");
    //         cout<<i<<" "<<j<<" : ";
    //         for(auto x:ok)
    //         {
    //             cout<<x.first<<" "<<x.second<<" ";
    //         }
    //         cout<<endl;
    //     }
    // }

    // cout<<"before"<<endl;
    //this_thread::sleep_for(chrono::seconds(5));
    // cout<<"after closed"<<endl;
    return 0;
}

