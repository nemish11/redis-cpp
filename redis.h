#include "value.h"
using namespace std;


/*
Redis provides GET, SET, EXPIRE, ZADD, ZRANGE, ZRANK, etc operations.
*/
class Redis
{
    private:
        map<string, Value*>cache; // stores key-value pairs
        thread t1; // thread used to clean expired key
        bool threadStatus; // thread termination flag

        /*
        This method check whether given key is expired or not.
        if given key is expired then remove from cache.
        return 1 if given key is expired
        otherwise 0
        */
        bool isTimeOut(string key)
        {
            if(cache.find(key) == cache.end())  // if not present in cache
                return 1;
            
            Value *value = cache[key]; 
            time_t current_time = time(0);  // current time in second
            
            // delete and return 1 if key expired
            if(value->GetExpireTime() < current_time)
            {
                if(cache[key]->GetType() == NORMAL_VALUE)
                    delete (NormalValue*)cache[key];
                else
                    delete (SetValue*)cache[key];

                cache.erase(key);
                return 1;
            }
            
            return 0;
        }
        

        /*
        This method delete existing key from cache if type of key is SetValue.
        */
        void deleteExistingSetValueKey(string key)
        {
            isTimeOut(key); // first check about key is expired or not
            if(cache.find(key) != cache.end() && cache[key]->GetType() != NORMAL_VALUE)
                delete (SetValue*)cache[key];
        }


        /*
        This method perform cleaning at every 10 seconds.
        Thread will run at every 10 seconds to clean expired keys.
        */
        void freeExpiredKey()
        {
            while(threadStatus)
            {
                for(auto key: cache)
                {
                    isTimeOut(key.first);
                }
                this_thread::sleep_for(chrono::seconds(10));
            }
        }

    public:
        /*
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
        }*/

        /*
        GET-> first check if key present in cache,
                    if type of value is string then return value
                    otherwise raise exception.
              if key not exist then return "nil" object.
        */
        string GET(string key)
        {
            if(cache.find(key) != cache.end()) // check existance of key
            {
                if(isTimeOut(key))
                    return "nil";  // key timeout
                
                if(cache[key]->GetType() == NORMAL_VALUE)
                {
                    return ((NormalValue*)cache[key])->getValue(); // return value of key
                }
                else
                {
                    // type of value of key is not string then raise exceptions
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

            return "nil";  // key not present in cache
        }


        /*
        set key and value. if key already exist then overwrite value. 
        set default expiration time MAX_EXPIRE_TIME
        NOTE: if key previously added by ZADD then it will be deleted and new key-value pair inserted
        */
        string SET(string key, string val, long long int expire_time=MAX_EXPIRE_TIME)
        {
            deleteExistingSetValueKey(key); //delete already existing key added by ZADD or SET
            time_t current_time = time(0);
            cache[key] = new NormalValue(val, current_time + expire_time);
            return "OK";
        }


        /*
        set key and value based on given option and expire_time. 
        */
        string SET(string key, string val, string option, long long int expire_time=MAX_EXPIRE_TIME)
        {
            isTimeOut(key); // remove key if already expired

            time_t current_time = time(0); // current time in second
            if(option == "EX")
            {
                // if given key already added by ZADD then remove first and insert new pair.
                deleteExistingSetValueKey(key);

                cache[key] = new NormalValue(val, current_time + expire_time);
                return "OK";
            }
            else if(option == "PX")
            {
                deleteExistingSetValueKey(key);

                cache[key] = new NormalValue(val, current_time + llround(expire_time/1000.0));
                return "OK";
            }
            else if(option == "NX")
            {
                if(cache.find(key) != cache.end())
                {
                    if(cache[key]->GetType() == NORMAL_VALUE)
                        return "nil";
                }
                deleteExistingSetValueKey(key);

                cache[key] = new NormalValue(val);
                return "OK";
            }
            else if(option == "XX")
            {
                if(cache.find(key) != cache.end())
                {
                    if(cache[key]->GetType() == NORMAL_VALUE)
                    {
                        deleteExistingSetValueKey(key);

                        cache[key] = new NormalValue(val);
                        return "OK";
                    }
                }
                return "nil";
            }
            else if(option == "KEEPTTL")
            {
                deleteExistingSetValueKey(key);

                if(cache.find(key) != cache.end())
                    cache[key] = new NormalValue(val, cache[key]->GetExpireTime());
                else
                    cache[key] = new NormalValue(val);
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

        /*
        Set new expire time if key exist in cache.
        */
        string EXPIRE(string key, long long int expire_time)
        {
            isTimeOut(key);
            
            if(cache.find(key) != cache.end())
            {
                time_t current_time = time(0);
                cache[key]->SetExpireTime(current_time + expire_time);
                return "1";
            }

            return "0"; //key not found
        }


        /*
        ZADD score and value with given key to AVL tree.
        */
        string ZADD(string key, long long int score, string value)
        {
            isTimeOut(key);

            if(cache.find(key) != cache.end() && cache[key]->GetType() != SET_VALUE)
            {
                // if key already set by SET command then throw error
                try
                {
                    throw "(error) WRONGTYPE Operation against a key holding the wrong kind of value";
                }
                catch(const char* msg)
                {
                    return msg;
                }
            }

            SetValue* obj;
            if(cache.find(key) != cache.end())
            {
                // if already tree instance exist then use it
                obj = (SetValue*)cache[key];
            }
            else
            {
                // create new instance of SetValue
                AVLTree tree;
                obj = new SetValue(tree);
                cache[key] = obj;
            }

            if(obj->checkIsValueExist(value))
            {
                // if same value already exist then first remove it and update score
                long long int old_score = obj->getScoreFromValue(value);
                
                if(old_score == score)
                    return "0";
                
                Node *node = obj->getTreeInstance().deleteNode(obj->getTreeInstance().root, old_score, value);
                obj->setTreeInstanceRoot(node);
            }

            Node *node = obj->getTreeInstance().insert(obj->getTreeInstance().root, score, value);
            obj->setTreeInstanceRoot(node);
            obj->setValueToScore(value, score);
            
            return "1";
        }


        string ZADD(string key, string option, long long int score, string value)
        {
            isTimeOut(key);

            if(cache.find(key) != cache.end() && cache[key]->GetType() != SET_VALUE)
            {
                // if key already set by SET command then throw error
                try
                {
                    throw "(error) WRONGTYPE Operation against a key holding the wrong kind of value";
                }
                catch(const char* msg)
                {
                    return msg;
                }
            }

            SetValue* obj;
            if(cache.find(key) != cache.end())
            {
                // if already tree instance exist then use it
                obj = (SetValue*)cache[key];
            }
            else
            {
                // create new instance of SetValue
                AVLTree tree;
                obj = new SetValue(tree);
                cache[key] = obj;
            }

            long long int modified_count  = 0;

            if(option == "XX")
            {
                if(obj->checkIsValueExist(value))
                {
                    // if same value already exist then first remove it and update score
                    long long int old_score = obj->getScoreFromValue(value);
                    
                    if(old_score == score)
                        return "0";
                    
                    Node *node = obj->getTreeInstance().deleteNode(obj->getTreeInstance().root, old_score, value);
                    obj->setTreeInstanceRoot(node);
                }
                else
                {
                    return "0";
                }
            }
            else if(option == "NX")
            {
                if(obj->checkIsValueExist(value))
                {
                    return "0";
                }
            }
            else if(option == "CH")
            {
                if(obj->checkIsValueExist(value))
                {
                    // if same value already exist then first remove it and update score
                    long long int old_score = obj->getScoreFromValue(value);
                    
                    if(old_score == score)
                        return "0";
                    
                    Node *node = obj->getTreeInstance().deleteNode(obj->getTreeInstance().root, old_score, value);
                    obj->setTreeInstanceRoot(node);
                    modified_count++;
                }
            }
            else if(option =="INCR")
            {
                if(obj->checkIsValueExist(value))
                {
                    // if same value already exist then first remove it and update score
                    long long int old_score = obj->getScoreFromValue(value);
                    score = score + old_score;
                    if(old_score == score)
                        return "0";
                    
                    Node *node = obj->getTreeInstance().deleteNode(obj->getTreeInstance().root, old_score, value);
                    obj->setTreeInstanceRoot(node);
                }
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

                return "0";
            }

            Node *node = obj->getTreeInstance().insert(obj->getTreeInstance().root, score, value);
            obj->setTreeInstanceRoot(node);
            obj->setValueToScore(value, score);
            
            return to_string(1 + modified_count);
        }

        
        string ZRANK(string key, string value)
        {
            if(cache.find(key) == cache.end() || isTimeOut(key))
            {
                return "-1"; //key not found
            }

            SetValue* obj = (SetValue*)cache[key];

            if(!obj->checkIsValueExist(value))
            {
                return "-1"; //value not found in current tree
            }

            long long int score = obj->getScoreFromValue(value);
            long long int rank = obj->getTreeInstance().findRank(obj->getTreeInstance().root, score, value);
            
            return to_string(rank - 1);
        }


        string ZREVRANK(string key, string value)
        {
            string rank = ZRANK(key, value);
            
            if(rank == "-1")
                return rank;
            
            SetValue* obj = (SetValue*)cache[key];

            long long int totalnodes = obj->getTreeInstance().getTotalNodesInTree(obj->getTreeInstance().root);
            return to_string(totalnodes - stoll(rank) - 1);
        }


        vector<string> ZRANGE(string key, long long int l, long long int r)
        {
            if(cache.find(key) == cache.end() || isTimeOut(key) || cache[key]->GetType()!=SET_VALUE)
            {
                return vector<string>();
            }
            
            SetValue* obj = (SetValue*)cache[key];
            long long int total_values = obj->getTreeInstance().root->totalcount;
            
            if(l<0)
                l = l + total_values;
            
            if(r<0)
                r = r + total_values;
            
            return obj->getTreeInstance().retrieveByRange(obj->getTreeInstance().root, l+1, r+1);
        }


        vector<pair<string,long long int> > ZRANGE(string key, long long int l, long long int r, string option)
        {
            if(cache.find(key) == cache.end() || isTimeOut(key) || cache[key]->GetType()!=SET_VALUE)
            {
                return vector<pair<string,long long int> >();
            }
            if(option == "WITHSCORES")
            {
                SetValue* obj = (SetValue*)cache[key];
                long long int total_values = obj->getTreeInstance().root->totalcount;
                if(l<0)
                    l = l + total_values;
                if(r<0)
                    r = r + total_values;
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
                    vector<pair<string,long long int> >error;
                    error.push_back(make_pair(msg,0));
                    return error;
                }
            }
        }


        string ZCARD(string key)
        {
            isTimeOut(key);

            if(cache.find(key) != cache.end())
            {
                if(cache[key]->GetType() != SET_VALUE)
                {
                    try
                    {
                        throw "(error) WRONGTYPE Operation against a key holding the wrong kind of value";
                    }
                    catch(const char* msg)
                    {
                        return msg;
                    }
                    return "0";
                }

                SetValue* obj= (SetValue*)cache[key];
                return to_string(obj->getTreeInstance().getTotalNodesInTree(obj->getTreeInstance().root));
            }
            
            return "0";
        }


        string ZSCORE(string key, string value)
        {
            isTimeOut(key);

            if(cache.find(key) != cache.end())
            {
                if(cache[key]->GetType() != SET_VALUE)
                {
                    try
                    {
                        throw "(error) WRONGTYPE Operation against a key holding the wrong kind of value";
                    }
                    catch(const char* msg)
                    {
                        return msg;
                    }
                    return "-1";
                }

                SetValue* obj= (SetValue*)cache[key];
                return to_string(obj->getScoreFromValue(value));
            }
            return "-1"; // if key not found
        }

        string TTL(string key)
        {
            isTimeOut(key);

            if(cache.find(key) != cache.end())
            {
                long long int expire = cache[key]->GetExpireTime();
                return to_string(expire-time(0));
            }
            else
            {
                return "-2";
            }
        }
};
