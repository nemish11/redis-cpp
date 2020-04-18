#include "bits/stdc++.h"
#include "avl_tree_class.h"
using namespace std;
#define PB(x) push_back(x)
#define MP(x,y) make_pair(x,y)
#define F first
#define S second
#define boost ios_base::sync_with_stdio(false),cin.tie(NULL);
//#include <ext/pb_ds/assoc_container.hpp> // Common file 
//#include <ext/pb_ds/tree_policy.hpp>  
//using namespace __gnu_pbds; 
//stringstream ss; string temp; ss<<str(original string) ss>>temp;
class Value
{
    private:
        string value;
        time_t creation_time;
        int expire_time;  // store in millisecond
    public:
        Value(){}

        Value(string value)
        {
            this->value = value;
            time(&creation_time);
            expire_time = INT_MAX;
        }

        time_t get_creation_time()
        {
            return creation_time;
        }

        int get_expire_time()
        {
            return expire_time;
        }

        string get_value()
        {
            return value;
        }
};

class Redis
{
    private:
        map<string, Value>mp;
        map<string, AVLTree>tree_instance;
        map<string, int> value_to_score;

        bool isTimeOut(Value value)
        {
            time_t current_time;
            time(&current_time);
            double diff_seconds = difftime(current_time, value.get_creation_time());
            int milliseconds = diff_seconds*1000;
            if(value.get_expire_time() < milliseconds)
                return 1;
            return 0;
        }

    public:

        string GET(string key)
        {
            if(mp.find(key) != mp.end())
            {
                if(!isTimeOut(mp[key]))
                {
                    return mp[key].get_value();
                }
            }

            if(tree_instance.find(key) != tree_instance.end())
            {
                try
                {
                    throw "(error) WRONGTYPE Operation against a key holding the wrong kind of value";
                }
                catch(const char* msg)
                {
                    return msg;
                }
            }

            return "nil";
        }

        string SET(string key, string val)
        {
            Value value(val);
            mp[key] = value;
            return "OK";
        }

        int SET(string key, string value, string options, int expire_time)
        {
            return 1;
        }

        int EXPIRE(string key, int time)
        {
            return 1;
        }

        int ZADD(string key, int score, string value)
        {
            if(mp.find(key) != mp.end())
            {
                //raise exception
                return 0;
            }

            tree_instance[key].root = tree_instance[key].insert(tree_instance[key].root, score, value);
            value_to_score[value] = score;
            return 1;
        }

        int ZRANK(string key, string value)
        {
            int score = value_to_score[value];
            int rank = tree_instance[key].findRank(tree_instance[key].root, score, value);
            return rank;
        }

        vector<string> ZRANGE(string key, int l, int r)
        {
            // not key found then return
            return tree_instance[key].retrieveFromRange(tree_instance[key].root, l, r);
        }
};

int32_t main()
{	
	boost
    srand(time(0));
	#ifndef ONLINE_JUDGE
        freopen("input.txt", "r", stdin);
        freopen("output.txt", "w", stdout);
    #endif
    Redis r;
    // //test GET
    // cout<<r.GET("hello")<<endl;
    
    // //test SET
    // r.SET("hello", "hii");
    // cout<<r.GET("hello")<<endl;

    // r.ZADD("myset", 3, "aaa");
    // r.ZADD("myset", 2, "aab");
    // r.ZADD("myset", 3, "aac");
    // r.ZADD("myset", 2, "aad");
    // r.ZADD("myset1", 1, "aaa");
    // r.ZADD("myset1", 1, "aav");

    // cout<<r.ZRANK("myset", "aab")<<endl;
    // cout<<r.ZRANK("myset", "aac")<<endl;
    // cout<<r.ZRANK("myset1", "aaa")<<endl;
    // cout<<r.ZRANK("myset1", "aav")<<endl;

    // for(int i=1;i<=4;i++)
    // {
    //     for(int j=1;j<=4;j++)
    //     {
    //         vector<string>ok = r.ZRANGE("myset",i,j);
    //         cout<<i<<" "<<j<<" : ";
    //         for(auto x:ok)
    //         {
    //             cout<<x<<" ";
    //         }
    //         cout<<endl;
    //     }
    // }

    // for(int i=1;i<=4;i++)
    // {
    //     for(int j=1;j<=4;j++)
    //     {
    //         vector<string>ok = r.ZRANGE("myset1",i,j);
    //         cout<<i<<" "<<j<<" : ";
    //         for(auto x:ok)
    //         {
    //             cout<<x<<" ";
    //         }
    //         cout<<endl;
    //     }
    // }
    r.ZADD("myke",1,"ff");
    cout<< r.GET("myke")<<endl;
    return 0;
}

