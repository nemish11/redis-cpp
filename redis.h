#include "value.h"
using namespace std;
#define int long long int


class Redis
{
    private:
        map<string, Value*>cache;
        map<string, int> value_to_score;

        bool isTimeOut(string key)
        {
            if(cache.find(key) == cache.end())
            {
                return 1;
            }
            
            Value *value = cache[key];
            time_t current_time = time(0);
            
            if(value->GetExpireTime() < current_time)
            {
                if(cache[key]->GetType() == NORMAL_VALUE)
                {
                    delete (NormalValue*)cache[key];
                }
                else
                {
                    delete (SetValue*)cache[key];
                }
                cache.erase(key);
                return 1;
            }
            
            return 0;
        }

        void delete_node_from_SetValue(string key)
        {
            isTimeOut(key);
            if(cache.find(key) != cache.end() && cache[key]->GetType() != NORMAL_VALUE)
            {
                delete (SetValue*)cache[key];
            }
        }

        void free_memory()
        {
            A:
            for(auto key: cache)
            {
                isTimeOut(key.first);
            }
            this_thread::sleep_for(chrono::seconds(10));
            goto A;
        }

    public:

        Redis()
        {
            thread t1(&Redis::free_memory, this);
            t1.detach();
        }

        string GET(string key)
        {
            if(cache.find(key) != cache.end())
            {
                if(isTimeOut(key))
                {
                    return "nil";
                }   
                
                if(cache[key]->GetType() == NORMAL_VALUE)
                {
                    return ((NormalValue*)cache[key])->getValue();
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
            delete_node_from_SetValue(key);
            cache[key] = new NormalValue(val);
            return "OK";
        }

        string SET(string key, string val, string option, int expire_time=LONG_MAX)
        {
            time_t current_time = time(0);
            if(option == "EX")
            {
                delete_node_from_SetValue(key);

                cache[key] = new NormalValue(val, current_time + expire_time);
                return "OK";
            }
            else if(option == "PX")
            {
                delete_node_from_SetValue(key);

                cache[key] = new NormalValue(val, current_time + llround(expire_time/1000.0));
                cout<<cache[key]->GetExpireTime()<<" "<<current_time<<endl;
                return "OK";
            }
            else if(option == "NX")
            {
                if(cache.find(key) != cache.end())
                {
                    if(cache[key]->GetType() == NORMAL_VALUE)
                    {
                        return "nil";
                    }
                }
                delete_node_from_SetValue(key);

                cache[key] = new NormalValue(val);
                return "OK";
            }
            else if(option == "XX")
            {
                if(cache.find(key) != cache.end())
                {
                    if(cache[key]->GetType() == NORMAL_VALUE)
                    {
                        delete_node_from_SetValue(key);

                        cache[key] = new NormalValue(val);
                        return "OK";
                    }
                }
                return "nil";
            }
            else if(option == "KEEPTTL")
            {
                delete_node_from_SetValue(key);

                if(cache.find(key) != cache.end())
                {
                    cache[key] = new NormalValue(val, cache[key]->GetExpireTime());
                }
                else
                {
                    cache[key] = new NormalValue(val);
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

        int EXPIRE(string key, int expire_time)
        {
            if(cache.find(key) != cache.end())
            {
                time_t current_time = time(0);
                cache[key]->SetExpireTime(current_time + expire_time);
                return 1;
            }
            return 0; //key not found
        }

        int ZADD(string key, int score, string value)
        {
            isTimeOut(key);

            if(cache.find(key) != cache.end() && cache[key]->GetType() != SET_VALUE)
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
            if(cache.find(key) != cache.end())
            {
                obj = (SetValue*)cache[key];
            }
            else
            {
                AVLTree tree;
                obj = new SetValue(tree);
                cache[key] = obj;
            }

            if(value_to_score.find(key+"_"+value) != value_to_score.end())
            {
                int old_score = value_to_score[key+"_"+value];
                Node *node = obj->getTreeInstance().deleteNode(obj->getTreeInstance().root, old_score, value);
                obj->setTreeInstance_root(node);
            }

            Node *node = obj->getTreeInstance().insert(obj->getTreeInstance().root, score, value);
            obj->setTreeInstance_root(node);
            value_to_score[key+"_"+value] = score;
            return 1;
        }

        int ZRANK(string key, string value)
        {
            if(cache.find(key) == cache.end() || isTimeOut(key) || value_to_score.find(key+"_"+value)==value_to_score.end())
            {
                //cout<<"nil"<<endl;
                return -1;
            }
            int score = value_to_score[key+"_"+value];
            SetValue* obj = (SetValue*)cache[key];
            int rank = obj->getTreeInstance().findRank(obj->getTreeInstance().root, score, value);
            return rank-1;
        }

        vector<string> ZRANGE(string key, int l, int r)
        {
            if(cache.find(key) == cache.end() || isTimeOut(key) || cache[key]->GetType()!=SET_VALUE)
            {
                return vector<string>();
            }
            SetValue* obj = (SetValue*)cache[key];
            int total_values = obj->getTreeInstance().root->totalcount;
            if(l<0)
            {
                l = l + total_values;
            }
            if(r<0)
            {
                r = r + total_values;
            }
            return obj->getTreeInstance().retrieveByRange(obj->getTreeInstance().root, l+1, r+1);
        }

        vector<pair<string,int> > ZRANGE(string key, int l, int r, string option)
        {
            if(cache.find(key) == cache.end() || isTimeOut(key) || cache[key]->GetType()!=SET_VALUE)
            {
                return vector<pair<string,int> >();
            }
            if(option == "WITHSCORES")
            {
                SetValue* obj = (SetValue*)cache[key];
                int total_values = obj->getTreeInstance().root->totalcount;
                if(l<0)
                {
                    l = l + total_values;
                }
                if(r<0)
                {
                    r = r + total_values;
                }
                return obj->getTreeInstance().retrieveByRangeWithScore(obj->getTreeInstance().root, l+1, r+1);
            }
            else
            {
                try
                {
                    throw "(error) ERR Syntax error";
                }
                catch(const char* msg)
                {
                    vector<pair<string,int> >error;
                    error.push_back(make_pair(msg,0));
                    return error;
                }
            }
        }

        ~Redis()
        {
            
        }
};