#include "redis.h"
using namespace std;


void testGET()
{
    Redis r;

    string value = r.GET("key1");
    assert(value == "nil");

    string isSet = r.SET("key1", "value1");
    assert(isSet == "OK");

    value = r.GET("key1");
    assert(value == "value1");

    cout<<"GET test cases passed"<<endl;
}


void testSET()
{
    Redis r;

    string value = r.GET("key1");
    assert(value == "nil");

    string isSet = r.SET("key1", "value1");
    assert(isSet == "OK");

    value = r.GET("key1");
    assert(value == "value1");

    isSet = r.SET("key1", "value2");
    assert(isSet == "OK");

    value = r.GET("key1");
    assert(value == "value2");

    cout<<"SET test cases passed"<<endl;
}


void testSETOptions()
{
    Redis r;

    string value = r.GET("key1");
    assert(value == "nil");

    /*
    TEST EX option of SET
    SET KEY VALUE EX EXPIRE_TIME(in second)
    */
    string isSet = r.SET("key1", "value1", "EX", 2);
    assert(isSet == "OK");

    value = r.GET("key1");
    assert(value == "value1");

    this_thread :: sleep_for( chrono :: seconds(3)); // sleep thread for 2 second

    value = r.GET("key1");
    assert(value == "nil");


    /*
    TEST PX option of SET
    SET KEY VALUE PX EXPIRE_TIME(in millisecond)
    */
    isSet = r.SET("key1", "value1", "PX", 2000);
    assert(isSet == "OK");

    value = r.GET("key1");
    assert(value == "value1");

    this_thread :: sleep_for( chrono :: seconds(3)); // sleep thread for 2 second

    value = r.GET("key1");
    assert(value == "nil");

    cout<<"SET with OPTIONS test cases passed"<<endl;
}


void testEXPIRE()
{
    Redis r;

    string value = r.GET("key1");
    assert(value == "nil");

    string isSet = r.SET("key1", "value1");
    assert(isSet == "OK");

    value = r.GET("key1");
    assert(value == "value1");

    string isSetExpirationTime = r.EXPIRE("key1", 3); // set expiration time of key1 to 3 second
    assert(isSetExpirationTime == "1");

    value = r.GET("key1");
    assert(value == "value1");

    this_thread :: sleep_for( chrono :: seconds(1)); // sleep thread for 1 second
    
    value = r.GET("key1");
    assert(value == "value1");  // value should be "value1"
    
    this_thread :: sleep_for( chrono :: seconds(1)); // sleep thread for 1 second
    
    value = r.GET("key1");
    assert(value == "value1"); // value should be "value1"
    
    this_thread :: sleep_for( chrono :: seconds(2));
    
    value = r.GET("key1");
    assert(value == "nil"); // value should be "nil"

    cout<<"EXPIRE test cases passed"<<endl;
}

void testZADD()
{
    Redis r;

    string isInserted = r.ZADD("key1", 1, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 1, "aaa");
    assert(isInserted == "0");
    
    isInserted = r.ZADD("key1", 1, "aab");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 2, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 2, "aad");
    assert(isInserted == "1");

    isInserted = r.ZADD("key2", 1, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key2", 2, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key2", 1, "aab");
    assert(isInserted == "1");

    cout<<"ZADD test cases passed"<<endl;
}


void testZRANK()
{
    Redis r;

    string isInserted = r.ZADD("key1", 1, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 1, "aab");
    assert(isInserted == "1");
    
    isInserted = r.ZADD("key1", 1, "aac");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 2, "aad");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 2, "aae");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 3, "aaf");
    assert(isInserted == "1");

    string rank = r.ZRANK("key1", "aaa");
    assert(rank == "0");
    rank = r.ZRANK("key1", "aab");
    assert(rank == "1");
    rank = r.ZRANK("key1", "aac");
    assert(rank == "2");
    rank = r.ZRANK("key1", "aad");
    assert(rank == "3");
    rank = r.ZRANK("key1", "aae");
    assert(rank == "4");
    rank = r.ZRANK("key1", "aaf");
    assert(rank == "5");
    rank = r.ZRANK("key1", "random");
    assert(rank == "-1");

    isInserted = r.ZADD("key2", 1, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key2", 2, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key2", 1, "aab");
    assert(isInserted == "1");

    rank = r.ZRANK("key2", "aaa");
    assert(rank == "1");
    rank = r.ZRANK("key2", "aab");
    assert(rank == "0");

    cout<<"ZRANK test cases passed"<<endl;
}


void testZRAGE()
{
    Redis r;

    string isInserted = r.ZADD("key1", 1, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 1, "aab");
    assert(isInserted == "1");
    
    isInserted = r.ZADD("key1", 1, "aac");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 2, "aad");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 2, "aae");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 3, "aaf");
    assert(isInserted == "1");

    vector<string> values = r.ZRANGE("key1", 0, -1);
    vector<string> expected{"aaa", "aab", "aac", "aad", "aae", "aaf"};
    assert(values == expected);

    values = r.ZRANGE("key1", 0, 50000);
    assert(values == expected);

    for(long long int i=0;i<6;i++)
    {
        for(long long int j=0;j<6;j++)
        {
            vector<string> values_ = r.ZRANGE("key1", i, j);
            if(i > j)
            {
                assert(values_.size() == 0);
            }
            else
            {
                vector<string> expected_(expected.begin()+i, expected.begin()+j+1);
                assert(values_ == expected_);   
            }
        }
    }

    isInserted = r.ZADD("key2", 1, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key2", 2, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key2", 1, "aab");
    assert(isInserted == "1");

    values = r.ZRANGE("key2", 0, 1);
    expected = {"aab", "aaa"};
    assert(values == expected);

    values = r.ZRANGE("key2", -1, -1);
    expected = {"aaa"};
    assert(values == expected);

    values = r.ZRANGE("key2", -2, -2);
    expected = {"aab"};
    assert(values == expected);

    values = r.ZRANGE("key2", -1, -2);
    expected = {};
    assert(values == expected);
    
    values = r.ZRANGE("key2", -2, -1);
    expected = {"aab", "aaa"};
    assert(values == expected);

    cout<<"ZRANGE test cases passed"<<endl;
}


void testAllInOne()
{
    Redis r;

    string isSet = r.SET("key1", "value1");
    assert(isSet == "OK");

    string value = r.GET("key1");
    assert(value == "value1");

    // if key already inserted by SET then it's not allowed to insert same key in ZADD
    string isInsertedByZADD = r.ZADD("key1", 1, "aaa"); 
    assert(isInsertedByZADD == "(error) WRONGTYPE Operation against a key holding the wrong kind of value");

    r.EXPIRE("key1", 2);
    this_thread :: sleep_for( chrono :: seconds(3)); // sleep thread for 3 second

    // key should be inserted because "key1" will expire
    isInsertedByZADD = r.ZADD("key1", 1, "aaa"); 
    assert(isInsertedByZADD == "1");

    isInsertedByZADD = r.ZADD("key2", 1, "aaa"); 
    assert(isInsertedByZADD == "1");

    value = r.GET("key2");
    assert(value == "(error) WRONGTYPE Operation against a key holding the wrong kind of value");

    cout<<"integration of multiple commande passed"<<endl;
}


void testZCARD()
{
    Redis r;

    string isInserted = r.ZADD("key1", 1, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 1, "aaa");
    assert(isInserted == "0");
    
    isInserted = r.ZADD("key1", 1, "aab");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 2, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 2, "aad");
    assert(isInserted == "1");

    isInserted = r.ZADD("key2", 1, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key2", 2, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key2", 1, "aab");
    assert(isInserted == "1");

    string count = r.ZCARD("key1");
    assert(count == "3");

    count = r.ZCARD("key2");
    assert(count == "2");

    cout<<"ZCARD test cases passed"<<endl;
}


void testZREVRANK()
{
    Redis r;

    string isInserted = r.ZADD("key1", 1, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 1, "aab");
    assert(isInserted == "1");
    
    isInserted = r.ZADD("key1", 1, "aac");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 2, "aad");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 2, "aae");
    assert(isInserted == "1");

    isInserted = r.ZADD("key1", 3, "aaf");
    assert(isInserted == "1");

    string rank = r.ZREVRANK("key1", "aaa");
    assert(rank == "5");
    rank = r.ZREVRANK("key1", "aab");
    assert(rank == "4");
    rank = r.ZREVRANK("key1", "aac");
    assert(rank == "3");
    rank = r.ZREVRANK("key1", "aad");
    assert(rank == "2");
    rank = r.ZREVRANK("key1", "aae");
    assert(rank == "1");
    rank = r.ZREVRANK("key1", "aaf");
    assert(rank == "0");
    rank = r.ZREVRANK("key1", "random");
    assert(rank == "-1");

    isInserted = r.ZADD("key2", 1, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key2", 2, "aaa");
    assert(isInserted == "1");

    isInserted = r.ZADD("key2", 1, "aab");
    assert(isInserted == "1");

    rank = r.ZREVRANK("key2", "aaa");
    assert(rank == "0");
    rank = r.ZREVRANK("key2", "aab");
    assert(rank == "1");

    cout<<"ZREVRANK test case passed"<<endl;    

}


int32_t main()
{   
    #ifndef ONLINE_JUDGE
        freopen("input.txt", "r", stdin);
        //freopen("output.txt", "w", stdout);
    #endif

    testGET();

    testSET();

    testSETOptions();

    testEXPIRE();

    testZADD();

    testZRANK();

    testZRAGE();

    testZCARD();

    testZREVRANK();

    testAllInOne();

    return 0;
}

