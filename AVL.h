#include "libs.h"
using namespace std;

/*
Node class stores all the details. like, key, value,
left child, right child, height of node.
*/

class Node  
{  
    public: 
        long long int key;  //key of node
        Node *left;  //left child
        Node *right;  //right child
        long long int height;  //height of node in tree
        string value; // value at node
        //leftcount stores how many nodes have smaller or equal keys
        long long int leftcount, totalcount; 

        Node(){}

        Node(long long int key, string value)
        {
            this->key = key;  
            this->left = NULL;  
            this->right = NULL;  
            this->height = 1;
            this->leftcount = this->totalcount = 1;
            this->value = value;  
        }

        ~Node()
        {
            // delete left and right child
            delete this->left;
            delete this->right;
        }
};  
  
/*
AVLTree class provide insert, searchByRank, find values in given range.
Also, need to maintain balance structure of tree at any time.
*/

class AVLTree
{
    public:
        //root node 
        Node* root = NULL;

        //return height of node.
        long long int height(Node* node)
        {
            if(node == NULL) //if null return 0;
                return 0;
            return node->height;
        }

        //return how many node in it's subtree including current node as well.
        long long int node_count(Node* node)
        {
            if(node == NULL)
                return 0;
            return node->totalcount;
        }

        //return difference of height of left and right subtree
        long long int getBalance(Node* node)
        {
            if (node == NULL)  
                return 0;  
            return height(node->left) - height(node->right);
        }

        //perform right rotation on given node

        /*        T1, T2, T3 and T4 are subtrees.
                 z                                      y 
                / \                                   /   \
               y   T4      Right Rotate (z)          x      z
              / \          - - - - - - - - ->      /  \    /  \ 
             x   T3                               T1  T2  T3  T4
            / \
          T1   T2 */
        Node *rightRotate(Node *z)  
        {  
            Node *y = z->left;   
            Node *T3 = y->right; 
            // Perform rotation
            y->right = z;  
            z->left = T3;  
            // Updates heights  
            z->height = max(height(z->left), height(z->right)) + 1;  
            y->height = max(height(y->left), height(y->right)) + 1;  

            //updates leftcount and totalcount
            z->leftcount = node_count(z->left) + 1;
            z->totalcount = node_count(z->left) + node_count(z->right) + 1;
            y->totalcount = node_count(y->left) + node_count(y->right) + 1;
            y->leftcount = node_count(y->left) + 1;
            // Return new root 
            return y;  
        }  
          
        // A utility function to left  
        // rotate subtree rooted with x  
        // See the diagram given above.  
         /*     z                                y
             /  \                            /   \ 
            T1   y     Left Rotate(z)       z      x
                /  \   - - - - - - - ->    / \    / \
               T2   x                     T1  T2 T3  T4
                   / \
                 T3  T4  */
        Node *leftRotate(Node *z)  
        {  
            Node *y = z->right;  
            Node *T2 = y->left;  
            // Perform rotation  
            y->left = z;  
            z->right = T2;  
          
            // Updates heights  
            z->height = max(height(z->left), height(z->right)) + 1;  
            y->height = max(height(y->left), height(y->right)) + 1; 

            //updates counts
            z->totalcount = node_count(z->left) + node_count(z->right) + 1;
            z->leftcount = node_count(z->left) + 1;
            y->totalcount = node_count(y->left) + node_count(y->right) + 1;
            y->leftcount = node_count(y->left) + 1; 
            // Return new root  
            return y;  
        }


        /* Recursive function insert new key-value pair in tree and update details of nodes
        and returns the new root of the subtree.*/
        Node* insert(Node* node, long long int key, string value)  
        {  
            if (node == NULL)
                return new Node(key, value);
            
            //if key is smaller than node->key then insert left side
            if (key < node->key)
                node->left = insert(node->left, key, value);
            else if (key > node->key)  
                node->right = insert(node->right, key, value);
            else
            {
                //key is same as node->key then compare value
                if(value < node->value)
                    node->left = insert(node->left, key, value);
                else if (value > node->value)
                    node->right = insert(node->right, key, value);
            } 
          
            node->height = 1 + max(height(node->left), height(node->right)); 
            node->leftcount = node_count(node->left) + 1;
            node->totalcount = node_count(node->left) + node_count(node->right) + 1;

            long long int balance = getBalance(node);  
            
            //Left Left Case
            if (balance > 1 && key < node->left->key)  
                return rightRotate(node);  
          
            // Right Right Case  
            if (balance < -1 && key > node->right->key)  
                return leftRotate(node);  
          
            // Left Right Case  
            if (balance > 1 && key > node->left->key)  
            {  
                node->left = leftRotate(node->left);  
                return rightRotate(node);  
            }

            // Right Left Case  
            if (balance < -1 && key < node->right->key)  
            {  
                node->right = rightRotate(node->right);  
                return leftRotate(node);  
            }
            
            return node;  
        }

        /*
        This method returns rank of given value.
        */
        long long int findRank(Node *root, long long int score, string value, long long int rank=0)
        {
            if(root == NULL)
                return rank;
            
            if(score < root->key)
                return findRank(root->left, score, value, rank);

            if(score > root->key)
                return findRank(root->right, score, value, rank+root->leftcount);

            if(value < root->value)
                return findRank(root->left, score, value, rank);

            if(value > root->value)
                return findRank(root->right, score, value, rank+root->leftcount);
            
            return rank + root->leftcount;
        }

        /*
        Stores count number of element from left subtree of tree.
        */
        void takeElementFromLeftSubtree(Node *root, long long int count, vector<string>&values)
        {
            if(root!=NULL)
            {
                takeElementFromLeftSubtree(root->right, count, values);

                if((int)values.size() < count)
                    values.push_back(root->value);
                else
                    return;
                takeElementFromLeftSubtree(root->left, count, values);
            }
        }


        /*
        Stores count number of element from right subtree of tree.
        */
        void takeElementFromRightSubtree(Node *root, long long int count, vector<string>&values)
        {
            if(root!=NULL)
            {
                takeElementFromRightSubtree(root->left, count, values);

                if((int)values.size() < count)
                    values.push_back(root->value);
                else
                    return;
                takeElementFromRightSubtree(root->right, count, values);
            }
        }

        /*
        Return all values which has rank between left and right
        */
        vector<string> retrieveByRange(Node *root, long long int left, long long int right)
        {
            if(left > right || root == NULL || left<=0 || right<=0)
                return vector<string>();

            if(root->leftcount < left)
                return retrieveByRange(root->right, left-root->leftcount, right-root->leftcount);

            if(root->leftcount - 1 >= right)
                return retrieveByRange(root->left, left, right);

            vector<string>left_val;
            
            takeElementFromLeftSubtree(root->left, root->leftcount - left, left_val);
            reverse(left_val.begin(), left_val.end()); //need to reverse to maintain correct order
            
            //push current node value in left_val
            left_val.push_back(root->value);

            vector<string>right_val;
            takeElementFromRightSubtree(root->right, right - root->leftcount, right_val);
            
            for(auto value: right_val)
                left_val.push_back(value);

            return left_val;
        }


        /*
        Stores count number of element from left subtree of tree.
        */
        void takeElementFromLeftSubtreeWithscores(Node *root, long long int count, vector<pair<string,int>>&values)
        {
            if(root!=NULL)
            {
                takeElementFromLeftSubtreeWithscores(root->right, count, values);

                if((int)values.size() < count)
                    values.push_back(make_pair(root->value, root->key));
                else
                    return;

                takeElementFromLeftSubtreeWithscores(root->left, count, values);
            }
        }


        /*
        Stores count number of element from right subtree of tree.
        */
        void takeElementFromRightSubtreeWithscores(Node *root, long long int count, vector<pair<string,int>>&values)
        {
            if(root!=NULL)
            {
                takeElementFromRightSubtreeWithscores(root->left, count, values);

                if((int)values.size() < count)
                    values.push_back(make_pair(root->value, root->key));
                else
                    return;
                takeElementFromRightSubtreeWithscores(root->right, count, values);
            }
        }


        /*
        Return all values with score which has rank between left and right
        */
        vector< pair<string,int> > retrieveByRangeWithScore(Node *root, long long int left, long long int right)
        {
            if(left > right || root == NULL || left<=0 || right<=0)
                return vector<pair<string,int>>();

            if(root->leftcount < left)
                return retrieveByRangeWithScore(root->right, left-root->leftcount, right-root->leftcount);

            if(root->leftcount - 1 >= right)
                return retrieveByRangeWithScore(root->left, left, right);
            
            
            vector<pair<string,int>>left_val;
            
            takeElementFromLeftSubtreeWithscores(root->left, root->leftcount - left, left_val);
            reverse(left_val.begin(), left_val.end());
            
            left_val.push_back(make_pair(root->value, root->key));

            vector<pair<string,int>>right_val;
            takeElementFromRightSubtreeWithscores(root->right, right - root->leftcount, right_val);
            
            for(auto value: right_val)
                left_val.push_back(value);

            return left_val;
        }

        /*
        Return minValue node
        */
        Node* minValueNode(Node* node)  
        {  
            Node* current = node;  
            while (current->left != NULL)  
                current = current->left;  
            return current;  
        }  


        /*
        Delete node with has given key and value. After deletion updates count and heights
        of remaning nodes. 
        */
        Node* deleteNode(Node* root, long long int key, string value)  
        {  
            if (root == NULL)  
                return root;  

            if (key < root->key) 
                root->left = deleteNode(root->left, key, value); 
            else if(key > root->key) 
                root->right = deleteNode(root->right, key, value);
            else
            {  
                if(value < root->value)
                    root->left = deleteNode(root->left, key, value);
                else if(value > root->value)
                    root->right = deleteNode(root->right, key, value);
                else
                {
                    if((root->left == NULL) || (root->right == NULL))  
                    {  
                        Node *temp = root->left ? root->left : root->right;  
                        if (temp == NULL)  
                        {  
                            temp = root;  
                            root = NULL;  
                        }  
                        else  
                            *root = *temp;
                        free(temp);  
                    }  
                    else
                    {  
                        Node* temp = minValueNode(root->right);  
                        root->key = temp->key;    
                        root->value = temp->value;
                        root->right = deleteNode(root->right, temp->key, temp->value);  
                    }  
                }
            }  
           
            if (root == NULL)  
                return root;  
          
            //updates height
            root->height = 1 + max(height(root->left), height(root->right)); 

            //updates counts
            root->leftcount = node_count(root->left) + 1;
            root->totalcount = node_count(root->left) + node_count(root->right) + 1;

            long long int balance = getBalance(root);  
           
            if (balance > 1 && getBalance(root->left) >= 0)  
                return rightRotate(root);  
          
            if (balance > 1 && getBalance(root->left) < 0)  
            {  
                root->left = leftRotate(root->left);  
                return rightRotate(root);  
            }  
          
            if (balance < -1 && getBalance(root->right) <= 0)  
                return leftRotate(root);  
          
            if (balance < -1 && getBalance(root->right) > 0)  
            {  
                root->right = rightRotate(root->right);  
                return leftRotate(root);  
            }  

            return root;  
        }

        int getTotalNodesInTree(Node *root)
        {
            if(root == NULL)
                return 0;
            return root->totalcount;
        }

};



