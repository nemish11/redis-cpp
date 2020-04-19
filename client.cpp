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

    this_thread :: sleep_for( chrono :: milliseconds(2600)); // sleep thread for 2 second

    value = r.GET("key1");
    assert(value == "nil");

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

    int isSetExpirationTime = r.EXPIRE("key1", 3); // set expiration time of key1 to 3 second
    assert(isSetExpirationTime == 1);

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

}

void testZADD()
{
    Redis r;

    int isInserted = r.ZADD("key1", 1, "aaa");
    assert(isInserted == 1);

    isInserted = r.ZADD("key1", 1, "aaa");
    assert(isInserted == 0);
    
    isInserted = r.ZADD("key1", 1, "aab");
    assert(isInserted == 1);

    isInserted = r.ZADD("key1", 2, "aaa");
    assert(isInserted == 1);

    isInserted = r.ZADD("key1", 2, "aad");
    assert(isInserted == 1);

    isInserted = r.ZADD("key2", 1, "aaa");
    assert(isInserted == 1);

    isInserted = r.ZADD("key2", 2, "aaa");
    assert(isInserted == 1);

    isInserted = r.ZADD("key2", 1, "aab");
    assert(isInserted == 1);
}


void testZRANK()
{
    Redis r;

    int isInserted = r.ZADD("key1", 1, "aaa");
    assert(isInserted == 1);

    isInserted = r.ZADD("key1", 1, "aab");
    assert(isInserted == 1);
    
    isInserted = r.ZADD("key1", 1, "aac");
    assert(isInserted == 1);

    isInserted = r.ZADD("key1", 2, "aad");
    assert(isInserted == 1);

    isInserted = r.ZADD("key1", 2, "aae");
    assert(isInserted == 1);

    isInserted = r.ZADD("key1", 3, "aaf");
    assert(isInserted == 1);

    int rank = r.ZRANK("key1", "aaa");
    assert(rank == 0);
    rank = r.ZRANK("key1", "aab");
    assert(rank == 1);
    rank = r.ZRANK("key1", "aac");
    assert(rank == 2);
    rank = r.ZRANK("key1", "aad");
    assert(rank == 3);
    rank = r.ZRANK("key1", "aae");
    assert(rank == 4);
    rank = r.ZRANK("key1", "aaf");
    assert(rank == 5);
    rank = r.ZRANK("key1", "random");
    assert(rank == -1);

    isInserted = r.ZADD("key2", 1, "aaa");
    assert(isInserted == 1);

    isInserted = r.ZADD("key2", 2, "aaa");
    assert(isInserted == 1);

    isInserted = r.ZADD("key2", 1, "aab");
    assert(isInserted == 1);

    rank = r.ZRANK("key2", "aaa");
    assert(rank == 1);
    rank = r.ZRANK("key2", "aab");
    assert(rank == 0);
}


void testZRAGE()
{
    Redis r;

    int isInserted = r.ZADD("key1", 1, "aaa");
    assert(isInserted == 1);

    isInserted = r.ZADD("key1", 1, "aab");
    assert(isInserted == 1);
    
    isInserted = r.ZADD("key1", 1, "aac");
    assert(isInserted == 1);

    isInserted = r.ZADD("key1", 2, "aad");
    assert(isInserted == 1);

    isInserted = r.ZADD("key1", 2, "aae");
    assert(isInserted == 1);

    isInserted = r.ZADD("key1", 3, "aaf");
    assert(isInserted == 1);

    vector<string> values = r.ZRANGE("key1", 0, -1);
    vector<string> expected{"aaa", "aab", "aac", "aad", "aae", "aaf"};
    assert(values == expected);

    values = r.ZRANGE("key1", 0, 50000);
    assert(values == expected);

    for(int i=0;i<6;i++)
    {
        for(int j=0;j<6;j++)
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
    assert(isInserted == 1);

    isInserted = r.ZADD("key2", 2, "aaa");
    assert(isInserted == 1);

    isInserted = r.ZADD("key2", 1, "aab");
    assert(isInserted == 1);

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
}


void testAllInOne()
{
    Redis r;

    string isSet = r.SET("key1", "value1");
    assert(isSet == "OK");

    string value = r.GET("key1");
    assert(value == "value1");

    // if key already inserted by SET then it's not allowed to insert same key in ZADD
    int isInsertedByZADD = r.ZADD("key1", 1, "aaa"); 
    assert(isInsertedByZADD == 0);

    r.EXPIRE("key1", 2);
    this_thread :: sleep_for( chrono :: seconds(3)); // sleep thread for 3 second

    // key should be inserted because "key1" will expire
    isInsertedByZADD = r.ZADD("key1", 1, "aaa"); 
    assert(isInsertedByZADD == 1);
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

    testAllInOne();
    return 0;
}

