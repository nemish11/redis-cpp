#include "redis.h"
using namespace std;
#define BASE 10


class ErrorMessage 
{
        static ErrorMessage* errMsgObj;
        ErrorMessage() 
        {
            wrongUseOfCmd = "Wrong Use of Command, Please refer Help";
            wrongExpiryTime = "Expiry Time must be a positive number";
            noCommandFound = "No Command found";
        }

    public:
        string wrongUseOfCmd, wrongExpiryTime, noCommandFound;

        static ErrorMessage* getInstance() 
        {
            if(!errMsgObj)
                errMsgObj = new ErrorMessage();
            return errMsgObj;
        }
};


Redis redis;
ErrorMessage* ErrorMessage:: errMsgObj = NULL; 
ErrorMessage* errMsg = errMsg->getInstance();


long long int  validateNumber(string numberString) 
{
    try
    {
        long long int val = stoll(numberString);
    }
    catch(const std::invalid_argument e)
    {
        return 0;
    }
    catch(const std::out_of_range e)
    {
        return 0;
    }
    return 1;
}

vector<string> splitBySpace(string cmdline) 
{
    vector<string> command;
    string cmdopt;
    istringstream strStream(cmdline);

    while (getline(strStream, cmdopt, ' '))
        command.push_back(cmdopt);

    return command;
}

void printHelp()
{
    cout<<"1) GET"<<endl;
    cout<<"GET keyname"<<endl;
    cout<<endl;
    
    cout<<"2) SET"<<endl;
    cout<<"SET keyname valuename [option] expiration"<<endl;
    cout<<"(option can be: (EX, PX, NX, XX, KEEPTTL) )"<<endl;
    cout<<endl;
    
    cout<<"3) EXPIRE"<<endl;
    cout<<"EXPIRE keyname expire_time"<<endl;
    cout<<"(please provide expire_time in second"<<endl;
    cout<<endl;

    cout<<"4) ZADD"<<endl;
    cout<<"ZADD keyname [option] score value"<<endl;
    cout<<"(score only supports integer)"<<endl;
    cout<<"(option can be: (XX, NX, CH, INCR))"<<endl;
    cout<<endl;

    cout<<"5) ZRANK"<<endl;
    cout<<"ZRANK keyname value"<<endl;
    cout<<endl;

    cout<<"6) ZRANGE"<<endl;
    cout<<"ZRANGE keyname left right"<<endl;
    cout<<"ZRANGE keyname left right WITHSCORES"<<endl;
    cout<<endl;
    cout<<"Please enter HELP to see help"<<endl;
    cout<<"......start testing......"<<endl;
} 

void parseGET(vector<string> &cmd) 
{
    if(cmd.size() == 2) 
        cout<< redis.GET(cmd[1]);
    else
        cout<< "GET: " << errMsg->wrongUseOfCmd;
}


void parseSET(vector<string> &cmd) 
{
    if(cmd.size() == 3) 
    {
        cout<< redis.SET(cmd[1], cmd[2]);
    } 
    else if(cmd.size() == 5 && (cmd[3] == "EX" || cmd[3] == "PX")) 
    {
        long long expiration = validateNumber(cmd[4]);
        if(expiration == 0) 
        {
            cout<< errMsg->wrongExpiryTime;
        } 
        else 
        {
            expiration = stoll(cmd[4]);
            cout<< redis.SET(cmd[1], cmd[2], cmd[3], expiration);
        }
    } 
    else if(cmd.size() == 4 && (cmd[3] == "NX" || cmd[3] == "XX" || cmd[3] == "KEEPTTL"))
    {
        cout<< redis.SET(cmd[1], cmd[2], cmd[3]);
    } 
    else 
    {
        cout<< "SET: "<< errMsg->wrongUseOfCmd;
    }
}


void parseEXPIRE(vector<string> &cmd)
{
    if(cmd.size() == 3) 
    {
        long long expiry = validateNumber(cmd[2]);
        if(expiry == 1) 
        {
            cout<< redis.EXPIRE(cmd[1], stoll(cmd[2]));
        } 
        else 
        {
            cout << "EXPIRE: Expire time must be in number" << "\n";
        }
    } 
    else 
    {
        cout<< "EXPIRE: "<< errMsg->wrongUseOfCmd;
    }
}


void parseZADD(vector<string> &cmd) 
{
    if(cmd.size() == 4) 
    {
        long long int score = validateNumber(cmd[2]);
        
        if(score == 0) 
        {
            cout << "ZADD: Wrong score value" << "\n";
        } 
        else 
        {
            cout<< redis.ZADD(cmd[1], stoll(cmd[2]), cmd[3]);
        }
    } 
    else if(cmd.size() == 5) 
    {
        long long int score = validateNumber(cmd[3]);
        
        if(score == 0) 
        {
            cout << "ZADD: Wrong score value" << "\n";
        } 
        else 
        {
            cout<< redis.ZADD(cmd[1], cmd[2], stoll(cmd[3]), cmd[4]);
        }

    } 
    else 
    {
        cout<< "ZADD: "<< errMsg->wrongUseOfCmd;
    }
}


void parseZRANK(vector<string> &cmd) 
{
    if(cmd.size() == 3) 
    {
        cout<< redis.ZRANK(cmd[1], cmd[2]);
    } 
    else 
    {
        cout<< "ZRANK: "<< errMsg->wrongUseOfCmd;
    }
}


void parseZRANGE(vector<string> &cmd) {
    if(cmd.size() == 4) 
    {
        long long int left = validateNumber(cmd[2]);
        long long int right = validateNumber(cmd[3]);

        if(left == 0|| right == 0)
        {
            cout<<"Left and Right must be Numbers"<<endl;
            return;
        }
        vector<string> values = redis.ZRANGE(cmd[1], stoll(cmd[2]), stoll(cmd[3]));
        cout<<"[ ";
        for(auto value: values)
        {
            cout<<value<<" ";
        }
        cout<<"]";
    } 
    else if(cmd.size() == 5 && cmd[4] == "WITHSCORES") 
    {
        long long int left = validateNumber(cmd[2]);
        long long int right = validateNumber(cmd[3]);

        if(left == 0|| right == 0)
        {
            cout<<"Left and Right must be Numbers"<<endl;
            return;
        }

        vector<pair<string, long long int> > values = redis.ZRANGE(cmd[1], stoll(cmd[2]), stoll(cmd[3]), cmd[4]);
        cout<<"[ ";
        for(auto value: values)
        {
            cout<<"{"<<value.first<<","<<value.second<<"} ";
        }
        cout<<"]";

    } 
    else 
    {
        cout<< "ZRANGE: "<< errMsg->wrongUseOfCmd;
    }
}


int main()
{
    printHelp();
    while(1)
    {
        cout << "\n\nredis> ";
        string commandline;
        getline(cin, commandline);

        vector<string> command = splitBySpace(commandline);
        string commandType = command[0];
        transform(commandType.begin(), commandType.end(), commandType.begin(), ::toupper);

        if(commandType == "HELP") 
        {
            printHelp();
        } 
        else if(commandType == "GET")
        {
            parseGET(command);
        }
        else if(commandType == "SET")
        {
            parseSET(command);
        }
        else if(commandType == "EXPIRE")
        {
            parseEXPIRE(command);
        }
        else if(commandType == "ZADD")
        {
            parseZADD(command);
        }
        else if(commandType == "ZRANK")
        {
            parseZRANK(command);
        }
        else if(commandType == "ZRANGE")
        {
            parseZRANGE(command);
        }
        else if(commandType == "EXIT")
        {
            if(command.size() == 1) 
            {
                cout << "Good Bye !\n\n";
                break;
            } 
            else 
            {
                cout<< "EXIT: "<< errMsg->wrongUseOfCmd;
            }
        }
        else
        {
            cout<< commandType <<": "<< errMsg->noCommandFound;
        }
    }
    return 0;
}