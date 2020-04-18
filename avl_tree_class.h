#include "bits/stdc++.h"
using namespace std;
#define int long long int

class Node  
{  
    public: 
        int key;  
        Node *left;  
        Node *right;  
        int height;  
        string value;
        int leftcount, totalcount;

        Node()
        {

        }

        Node(int key, string value)
        {
            this->key = key;  
            this->left = NULL;  
            this->right = NULL;  
            this->height = 1;
            this->leftcount = this->totalcount = 1;
            this->value = value;  
        }
};  
  

class AVLTree
{

    public:
        Node* root = NULL;

        int height(Node* node)
        {
            if(node == NULL)
                return 0;
            return node->height;
        }

        int node_count(Node* node)
        {
            if(node == NULL)
                return 0;
            return node->totalcount;
        }

        int getBalance(Node* node)
        {
            if (node == NULL)  
                return 0;  
            return height(node->left) - height(node->right);
        }

        Node *rightRotate(Node *z)  
        {  
            Node *y = z->left;   
            Node *T3 = y->right; 
          
            // Perform rotation  
            y->right = z;  
            z->left = T3;  
          
            // Update heights  
            z->height = max(height(z->left), height(z->right)) + 1;  
            y->height = max(height(y->left), height(y->right)) + 1;  

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
        Node *leftRotate(Node *z)  
        {  
            Node *y = z->right;  
            Node *T2 = y->left;  
          
            // Perform rotation  
            y->left = z;  
            z->right = T2;  
          
            // Update heights  
            z->height = max(height(z->left), height(z->right)) + 1;  
            y->height = max(height(y->left), height(y->right)) + 1; 

            z->totalcount = node_count(z->left) + node_count(z->right) + 1;
            z->leftcount = node_count(z->left) + 1;

            y->totalcount = node_count(y->left) + node_count(y->right) + 1;
            y->leftcount = node_count(y->left) + 1; 
          
            // Return new root  
            return y;  
        }

        Node* insert(Node* node, int key, string value)  
        {  
            if (node == NULL)
            {  
                Node* tmpnode = new Node(key, value);
                return  tmpnode;  
            }
            
            if (key < node->key)
            {  
                node->left = insert(node->left, key, value);
            }  
            else if (key > node->key)  
            {
                node->right = insert(node->right, key, value);
            }
            else
            {
                if(value < node->value)
                {
                    node->left = insert(node->left, key, value);
                }
                else if (value > node->value)
                {
                    node->right = insert(node->right, key, value);
                }
                else
                {

                }
            } 
          
            node->height = 1 + max(height(node->left), height(node->right)); 
            node->leftcount = node_count(node->left) + 1;
            node->totalcount = node_count(node->left) + node_count(node->right) + 1;

            int balance = getBalance(node);  
            
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

        int findRank(Node *root, int score, string value, int rank=0)
        {
            if(root == NULL)
            {
                return rank;
            }
            
            if(score < root->key)
            {
                return findRank(root->left, score, value, rank);
            }

            if(score > root->key)
            {
                return findRank(root->right, score, value, rank+root->leftcount);
            }

            if(value < root->value)
            {
                return findRank(root->left, score, value, rank);
            }

            if(value > root->value)
            {
                return findRank(root->right, score, value, rank+root->leftcount);
            }
            
            return rank + root->leftcount;
        }

        void take_element_from_left(Node *root, int count, vector<string>&values)
        {
            if(root!=NULL)
            {
                take_element_from_left(root->right, count, values);

                if((int)values.size() < count)
                {
                    values.push_back(root->value);
                }
                else
                {
                    return;
                }
                take_element_from_left(root->left, count, values);
            }
        }

        void take_element_from_right(Node *root, int count, vector<string>&values)
        {
            if(root!=NULL)
            {
                take_element_from_right(root->left, count, values);

                if((int)values.size() < count)
                {
                    values.push_back(root->value);
                }
                else
                {
                    return;
                }
                take_element_from_right(root->right, count, values);
            }
        }

        vector<string> retrieveFromRange(Node *root, int left, int right)
        {
            if(left > right || root == NULL || left<=0 || right<=0)
            {
                return vector<string>();
            }

            if(root->leftcount < left)
            {
                vector<string>ans = retrieveFromRange(root->right, left-root->leftcount, right-root->leftcount);
                return ans;
            }

            if(root->leftcount - 1 >= right)
            {
                vector<string>ans = retrieveFromRange(root->left, left, right);
                return ans;
            }

            vector<string>left_val;
            
            take_element_from_left(root->left, root->leftcount - left, left_val);
            reverse(left_val.begin(), left_val.end());
            
            left_val.push_back(root->value);

            vector<string>right_val;
            take_element_from_right(root->right, right - root->leftcount, right_val);
            
            for(auto value: right_val)
            {
                left_val.push_back(value);
            }

            return left_val;
        }

        Node* minValueNode(Node* node)  
        {  
            Node* current = node;  
          
            while (current->left != NULL)  
                current = current->left;  
          
            return current;  
        }  

        Node* deleteNode(Node* root, int key, string value)  
        {  
            if (root == NULL)  
                return root;  

            if (key < root->key) 
            { 
                root->left = deleteNode(root->left, key, value); 
            }
            else if(key > root->key) 
            { 
                root->right = deleteNode(root->right, key, value); 
            } 
            else
            {  
                if(value < root->value)
                {
                    root->left = deleteNode(root->left, key, value);
                }
                else if(value > root->value)
                {
                    root->right = deleteNode(root->right, key, value);
                }
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
                        {
                            *root = *temp;
                        } 
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
          
            root->height = 1 + max(height(root->left), height(root->right));  
            int balance = getBalance(root);  
           
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

};

// void preOrder(Node *root)  
// {  
//     if(root != NULL)  
//     {  
//         cout << root->key << " "<<root->leftcount<<" "<<root->totalcount<<" ";
//         for(auto ok: root->values)
//             cout<<ok<<" ";
//         cout<<endl;
//         preOrder(root->left);  
//         preOrder(root->right);  
//     }  
// }  


// int32_t main()
// {	
// 	#ifndef ONLINE_JUDGE
//         freopen("input.txt", "r", stdin);
//         freopen("output.txt", "w", stdout);
//     #endif
    
//     AVLTree tree; 

//     tree.root = tree.insert(tree.root, 1, "aaa"); 
//     tree.root = tree.insert(tree.root, 2, "aau");
//     tree.root = tree.insert(tree.root, 2, "abc");  
//     tree.root = tree.insert(tree.root, 1, "agh");  
//     tree.root = tree.insert(tree.root, 3, "ffg");  
//     tree.root = tree.insert(tree.root, 3, "fff");  
      
    /* The constructed AVL Tree would be  
            30  
            / \  
            20 40  
            / \ \  
            10 25 50  
    */
//     cout << "Preorder traversal of the "
//             "constructed AVL tree is \n";  
//     preOrder(tree.root);
//     for(int i=1;i<=7;i++)
//         cout<<tree.searchByrank(tree.root, i)<<endl;
//     cout<<endl;
//     cout<<endl;
//     vector<string>val = tree.retrieveFromRange(tree.root, 2, 3);
//     for(auto x: val)
//     {
//         cout<<x<<" ";
//     }
//     cout<<endl;
//     return 0;
// }




