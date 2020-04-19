#include "AVL.h"
using namespace std;
#define int long long int


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
        int expire_time;  //stores expire time in second.
        TYPE type; //type of value. (String or Sorted Set)

    public:
        Value()
        {
            expire_time = LONG_MAX;
        }

        int GetExpireTime()
        {
            return expire_time;
        }

        void SetExpireTime(int expire_time)
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

        NormalValue(string value, int expire_time=LONG_MAX) : NormalValue()
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
    public:
        SetValue()
        {
            this->SetType(SET_VALUE);
        }

        SetValue(AVLTree tree, int expire_time=LONG_MAX) : SetValue()
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

        void setTreeInstance_root(Node* root)
        {
            this->tree.root = root;
        }

        ~SetValue()
        {
            delete tree.root;
        }
};
