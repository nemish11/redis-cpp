#include "AVL.h"
using namespace std;
#define MAX_EXPIRE_TIME 1e14

/*
NormalValue means string and
SetValue means Sorted Set
*/
enum TYPE {NORMAL_VALUE, SET_VALUE}; 


/*
Value can be Normal or Set value.
*/
class Value
{
    private:
        long long int expire_time;  //stores expire time in second.
        TYPE type; //type of value. (String or Sorted Set)

    public:
        Value()
        {
            expire_time = MAX_EXPIRE_TIME;
        }

        long long int GetExpireTime()
        {
            return expire_time;
        }

        void SetExpireTime(long long int expire_time)
        {
            this->expire_time = expire_time;
        }

        TYPE GetType()
        {
            return type;
        }

        void SetType(TYPE type)
        {
            this->type = type;
        }
};


/*
Value is String
*/
class NormalValue : public Value
{
    private:
        string value;
    public:
        NormalValue()
        {
            this->SetType(NORMAL_VALUE);
        }

        NormalValue(string value, long long int expire_time=MAX_EXPIRE_TIME) : NormalValue()
        {
            this->value = value;
            this->SetExpireTime(expire_time);
        }

        string getValue()
        {
            return value;
        }

        void setValue(string value)
        {
            this->value = value;
        }
};


/*
Value is Set.
*/
class SetValue : public Value
{
    private:
        AVLTree tree;  //tree is instance.
        map<string, long long int> value_to_score; //stores score of value
    public:
        SetValue()
        {
            this->SetType(SET_VALUE);
        }

        SetValue(AVLTree tree, long long int expire_time=MAX_EXPIRE_TIME) : SetValue()
        {
            this->tree = tree;
            this->SetExpireTime(expire_time);
        }

        AVLTree getTreeInstance()
        {
            return tree;
        }

        void setTreeInstance(AVLTree tree)
        {
            this->tree = tree;
        }

        void setTreeInstanceRoot(Node* root)
        {
            this->tree.root = root;
        }

        void setValueToScore(string value, long long int score)
        {
            this->value_to_score[value] = score;
        }

        bool checkIsValueExist(string value)
        {
            if(this->value_to_score.find(value) != this->value_to_score.end())
                return true;
            return false;
        }

        long long int getScoreFromValue(string value)
        {
            return this->value_to_score[value];
        }

        ~SetValue()
        {
            delete tree.root;
            value_to_score.clear();
        }
};
