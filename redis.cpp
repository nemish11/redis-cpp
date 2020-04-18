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

enum TYPE {NORMAL_VALUE, SET_VALUE};

class Value
{
    private:
        int expire_time;
        TYPE type;
    public:
        Value()
        {
            expire_time = LONG_MAX;
        }
        int get_expire_time()
        {
            return expire_time;
        }

        void set_expire_time(int expire_time)
        {
            this->expire_time = expire_time;
        }

        TYPE get_type()
        {
            return type;
        }

        void set_type(TYPE type)
        {
            this->type = type;
        }
};

class NormalValue : public Value
{
    private:
        string value;
    public:
        NormalValue()
        {
            this->set_type(NORMAL_VALUE);
        }

        NormalValue(string value, int expire_time=LONG_MAX) : NormalValue()
        {
            this->value = value;
            this->set_expire_time(expire_time);
        }

        string get_value()
        {
            return value;
        }

        void set_value(string value)
        {
            this->value = value;
        }
};

class SetValue : public Value
{
    private:
        AVLTree tree;
    public:
        SetValue()
        {
            this->set_type(SET_VALUE);
        }

        SetValue(AVLTree tree, int expire_time=LONG_MAX) : SetValue()
        {
            this->tree = tree;
            this->set_expire_time(expire_time);
        }

        AVLTree get_tree_instance()
        {
            return tree;
        }

        void set_tree_instance(AVLTree tree)
        {
            this->tree = tree;
        }

        void set_tree_instance_root(Node* root)
        {
            this->tree.root = root;
        }

        string get_name()
        {
            return "ggg";
        }
};



class Redis
{
    private:
        map<string, Value*>mp;
        map<string, int> value_to_score;

        bool isTimeOut(Value *value)
        {
            time_t current_time = time(0);
            if(value->get_expire_time() < current_time)
                return 1;
            return 0;
        }

    public:

        string GET(string key)
        {
            if(mp.find(key) != mp.end())
            {
                if(isTimeOut(mp[key]))
                {
                    return "nil";
                }

                if(mp[key]->get_type() == NORMAL_VALUE)
                {
                    return ((NormalValue*)mp[key])->get_value();
                }
                else
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

            }

            return "nil";
        }

        string SET(string key, string val)
        {
            NormalValue value(val);
            mp[key] = &value;
            return "OK";
        }

        string SET(string key, string val, string option, int expire_time=LONG_MAX)
        {
            if(option == "EX")
            {
                NormalValue value(val, expire_time*1000);
                mp[key] = &value;
                return "OK";
            }
            else if(option == "PX")
            {
                NormalValue value(val, expire_time);
                mp[key] = &value;
                return "OK";
            }
            else if(option == "NX")
            {
                if(mp.find(key) != mp.end())
                {
                    if(mp[key]->get_type() == NORMAL_VALUE)
                    {
                        return "nil";
                    }
                }
                NormalValue value(val);
                mp[key] = &value;
                return "OK";
            }
            else if(option == "XX")
            {
                if(mp.find(key) != mp.end())
                {
                    if(mp[key]->get_type() == NORMAL_VALUE)
                    {
                        NormalValue value(val);
                        mp[key] = &value;
                        return "OK";
                    }
                }
                return "nil";
            }
            else if(option == "KEEPTTL")
            {
                if(mp.find(key) != mp.end())
                {
                    NormalValue value(val, mp[key]->get_expire_time());
                    mp[key] = &value;
                }
                else
                {
                    NormalValue value(val);
                    mp[key] = &value;
                }
                return "ok";
            }
            else
            {
                try
                {
                    throw "(error) ERR Syntax error";
                }
                catch(const char* msg)
                {
                    return msg;
                }
            }
        }

        int EXPIRE(string key, int time)
        {
            if(mp.find(key) != mp.end())
            {
                mp[key]->set_expire_time(time*1000);
                return 1;
            }
            return 0; //key not found
        }

        int ZADD(string key, int score, string value)
        {
            if(mp.find(key) != mp.end() && mp[key]->get_type() == NORMAL_VALUE)
            {
                try
                {
                    throw "(error) WRONGTYPE Operation against a key holding the wrong kind of value";
                }
                catch(const char* msg)
                {
                    cout<<msg<<endl;
                    return 0;
                }
            }

            SetValue* obj;
            if(mp.find(key) != mp.end())
            {
                obj = (SetValue*)mp[key];
            }
            else
            {
                AVLTree tree;
                obj = new SetValue(tree);
                mp[key] = obj;
            }

            if(value_to_score.find(key+'_'+value) != value_to_score.end())
            {
                int old_score = value_to_score[key+'_'+value];
                Node *node = obj->get_tree_instance().deleteNode(obj->get_tree_instance().root, old_score, value);
                obj->set_tree_instance_root(node);
            }

            Node *node = obj->get_tree_instance().insert(obj->get_tree_instance().root, score, value);
            obj->set_tree_instance_root(node);
            value_to_score[key+"_"+value] = score;
            return 1;
        }

        int ZRANK(string key, string value)
        {
            int score = value_to_score[key+"_"+value];
            SetValue* obj = (SetValue*)mp[key];
            //cout<<key<<" "<<value<<" "<<score<<endl;
            int rank = obj->get_tree_instance().findRank(obj->get_tree_instance().root, score, value);
            return rank-1;
        }

        vector<string> ZRANGE(string key, int l, int r)
        {
            SetValue* obj = (SetValue*)mp[key];
            int total_values = obj->get_tree_instance().root->totalcount;
            if(l<0)
            {
                l = l + total_values;
            }
            if(r<0)
            {
                r = r + total_values;
            }
            return obj->get_tree_instance().retrieveFromRange(obj->get_tree_instance().root, l+1, r+1);
        }
};

// int32_t main()
// {	
// 	boost
//     srand(time(0));
// 	#ifndef ONLINE_JUDGE
//         freopen("input.txt", "r", stdin);
//         freopen("output.txt", "w", stdout);
//     #endif
//     Redis r;
//     //test GET
//     cout<<r.GET("hello")<<endl;
    
//     //test SET
//     r.SET("hello", "hii");
//     cout<<r.GET("hello")<<endl;
//     cout<<r.ZADD("myset", 3, "aaa")<<endl;
//     cout<<r.ZADD("myset", 2, "aab")<<endl;
//     cout<<r.ZADD("myset", 3, "aac")<<endl;
//     cout<<r.ZADD("myset", 2, "aad")<<endl;
//     cout<<r.ZADD("myset1", 1, "aaa")<<endl;
//     cout<<r.ZADD("myset1", 1, "aav")<<endl;
//     cout<<endl;
//     cout<<r.ZRANK("myset", "aaa")<<endl;
//     cout<<r.ZRANK("myset", "aac")<<endl;
//     cout<<r.ZRANK("myset1", "aaa")<<endl;
//     cout<<r.ZRANK("myset1", "aav")<<endl;

//     for(int i=0;i<4;i++)
//     {
//         for(int j=0;j<4;j++)
//         {
//             vector<string>ok = r.ZRANGE("myset",i,j);
//             cout<<i<<" "<<j<<" : ";
//             for(auto x:ok)
//             {
//                 cout<<x<<" ";
//             }
//             cout<<endl;
//         }
//     }

//     for(int i=0;i<4;i++)
//     {
//         for(int j=0;j<4;j++)
//         {
//             vector<string>ok = r.ZRANGE("myset1",i,j);
//             cout<<i<<" "<<j<<" : ";
//             for(auto x:ok)
//             {
//                 cout<<x<<" ";
//             }
//             cout<<endl;
//         }
//     }

//     cout<<r.ZADD("myset", 2, "aaa")<<endl;
//     cout<<r.ZADD("myset", 3, "aab")<<endl;
//     cout<<r.ZADD("myset", 2, "aac")<<endl;
//     // r.ZADD("myke",1,"ff");
//     // cout<< r.GET("myke")<<endl;
//     cout<<endl;
//     cout<<r.ZRANK("myset", "aaa")<<endl;
//     cout<<r.ZRANK("myset", "aac")<<endl;
//     cout<<r.ZRANK("myset1", "aaa")<<endl;
//     cout<<r.ZRANK("myset1", "aav")<<endl;

//     for(int i=0;i<4;i++)
//     {
//         for(int j=0;j<4;j++)
//         {
//             vector<string>ok = r.ZRANGE("myset",i,j);
//             cout<<i<<" "<<j<<" : ";
//             for(auto x:ok)
//             {
//                 cout<<x<<" ";
//             }
//             cout<<endl;
//         }
//     }

//     for(int i=0;i<4;i++)
//     {
//         for(int j=0;j<4;j++)
//         {
//             vector<string>ok = r.ZRANGE("myset1",i,j);
//             cout<<i<<" "<<j<<" : ";
//             for(auto x:ok)
//             {
//                 cout<<x<<" ";
//             }
//             cout<<endl;
//         }
//     }
//     return 0;
// }

