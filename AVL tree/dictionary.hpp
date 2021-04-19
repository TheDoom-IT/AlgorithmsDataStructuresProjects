#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP
#include <queue>
#include <string>
#include <stack>

template <typename Key, typename Info>
class Dictionary 
{
private:
    //element on which dictionary operates
    struct Node
    {
        Key key;
        Info info;

        Node* parent;
        Node* right;
        Node* left;
        
        //balance factor
        int bfactor;
        //height of the tree or subtree with this node as a root
        int height;
        Node(Key k,Info i,Node* p = nullptr, Node* r = nullptr, Node* l = nullptr,int bf = 0, int h = 1)
        : key(k), info(i), parent(p), right(r),left(l),bfactor(bf), height(h) {};
    };
public:
    class Iterator
    {
    protected:
        Node* curr;
        //holds dictionary to which iterator belongs
        //protects from using iterator in improper dictionary
        const Dictionary<Key,Info>* dictionary;

        //this pair will be returned by operator->
        std::pair<const Key&,Info&>* pair;
        //makes information stored in pair consistent with current node
        void updatePair();

        //private contructor
        //makes things faster in some functions
        //user can't know about curr and dictionary
        Iterator(Node* node, const Dictionary<Key,Info>* dic) : curr(node), dictionary(dic), pair(nullptr){};

        //checks if iterator can move in given direction
        //if not throws an exception
        enum class Move{LEFT,RIGHT,PARENT,FORWARD,BACKWARD};
        void isValidToMove(Move) const;
        //checks if node can be accesed
        //if not throws an exception
        void isValidToAcces()const;
    public:
        //accesing data
        //key is const, because user can't change a key
        std::pair<const Key&,Info&>& operator*();
        const std::pair<const Key&,Info&>& operator*()const;
        std::pair<const Key&,Info&>* operator->();
        const std::pair<const Key&,Info&>* operator->()const;

        //Iterator allows moving between nodes in two ways
        //Go/get functions moves iterator between neighbouring nodes
        //while ++ -- moves iterator in order from the smallest key value up to the highest

        //moves iterator to node's child or parent
        Iterator& goRight();
        Iterator& goLeft();
        Iterator& goParent();

        //return iterator to node's child or parent
        Iterator getRight()const;
        Iterator getLeft() const;
        Iterator getParent()const;

        //moves iterator to the next(greater) element
        //end iterator(curr == nullptr) can be reached this way
        Iterator& operator++();
        Iterator operator++(int);

        //moves iterator to the previous(smaller) element
        Iterator& operator--();
        Iterator operator--(int);

        Iterator() : curr(nullptr), dictionary(nullptr), pair(nullptr){};
        Iterator(const Iterator& it) : Iterator() {*this = it;};
        ~Iterator() {delete pair;};

        //gives information if move in this direciton is possible
        //go/get functions won't reach end iterator
        //that's why it there is no left child we can't do goLeft/getLeft etc.
        bool isRightPossible() const {return ( curr != nullptr && curr->right != nullptr); };
        bool isLeftPossible() const {return (curr != nullptr && curr->left != nullptr);};
        bool isParentPossible() const {return (curr != nullptr && curr->parent != nullptr);};
        
        bool isEmpty()const {return dictionary == nullptr;};
        bool isEnd()const {return curr == nullptr;};
        bool isLeaf()const {return (curr != nullptr && curr->right == nullptr && curr->left == nullptr);};

        bool operator!=(const Iterator&) const;
        bool operator==(const Iterator&) const;
        Iterator& operator=(const Iterator&);

        //gives dictionary acces to Node* curr
        friend class Dictionary;
    };

private:
    Node* root;
    //number of elements in the dictionary
    int size;

    //change of height or bfactor of some node may cause also change of its parent height etc.
    //this function updates given element and every parent up to root
    //rotates element if it is needed
    void updateNodesAndRotate(Iterator);
    //update given element's height and bfactor
    void updateNode(Iterator);

    //rotations used to mantain dictionary as AVL tree
    void rotateLeft(Iterator);
    void rotateRight(Iterator);
    //also double right rotation
    void rotateLeftRight(Iterator);
    //also double left rotation
    void rotateRightLeft(Iterator);

    //elements from the bottom of the tree are on the top of the stack
    //used in functions where processing starts from the bottom of the tree
    std::stack<Iterator> createStack();

public:
    Dictionary() : root(nullptr), size(0){};
    Dictionary(const Dictionary<Key,Info>& toCopy) : Dictionary() {copy(toCopy);};
    ~Dictionary() {clear();};

    //add node
    //return true if succeed
    //false if there is an element with such a key
    bool addNode(Key,Info);

    //delete node
    //true if succeed
    //false if there is no element with such a key
    bool deleteNode(Key);
    bool deleteNode(Iterator);

    //return iterator to the element with given key
    //return end iterator if there is no element with such a key
    Iterator find(Key)const;

    //iterator to the root
    Iterator top() const;
    //iterator to the smallest element
    Iterator begin() const;
    //end iterator
    Iterator end() const;

    void clear();
    
    int getSize()const {return size;};
    int getHeight() {return root == nullptr ? 0 : root->height;};

    //check if dictionary is AVL tree
    //created especially for testing
    bool isAVL();

    void copy(const Dictionary<Key,Info>&);
    Dictionary<Key,Info>& operator=(const Dictionary<Key,Info>&);
};


//----------------------------------------------------ITERATOR-----------------------------------------------------
template <typename Key, typename Info>
void Dictionary<Key,Info>::Iterator::isValidToAcces()const
{
    if(isEnd())
    {
        throw std::logic_error("End iterator can't be dereferenced.");
    }

    if(isEmpty())
    {
        throw std::logic_error("Empty iterator can't be dereferenced.");
    }
}

template <typename Key, typename Info>
void Dictionary<Key,Info>::Iterator::isValidToMove(Move direction)const
{
    if(isEmpty())
    {
        throw std::logic_error("Empty iterator can't be moved.");
    }

    if(direction == Move::RIGHT)
    {
        if(!isRightPossible())
        {
            throw std::logic_error("Iterator can't be moved right.");
        }
    }
    else if(direction == Move::LEFT)
    {
        if(!Iterator::isLeftPossible())
        {
            throw std::logic_error("Iterator can't be moved left.");
        }
    }
    else if(direction == Move::PARENT)
    {
        if(!Iterator::isParentPossible())
        {
            throw std::logic_error("Iterator can't be moved to the parent.");
        }
    }
    else if(direction == Move::FORWARD)
    {
        if(Iterator::isEnd())
        {
            throw std::logic_error("End iterator can't be incremented.");
        }
    }
    //BACKWARD
    else
    {
        if(*this == this->dictionary->begin())
        {
            throw std::logic_error("Begin iterator can't be decremented.");
        }
    }
}       

template <typename Key, typename Info>
std::pair<const Key&,Info&>& Dictionary<Key,Info>::Iterator::operator*()
{
    isValidToAcces();
    //pair is not updated if iterator moves
    //so it need to be updated during accesing
    updatePair();
    return *pair;
}

template <typename Key, typename Info>
const std::pair<const Key&,Info&>& Dictionary<Key,Info>::Iterator::operator*()const
{
    isValidToAcces();
    updatePair();
    return *pair;
}

template <typename Key, typename Info>
std::pair<const Key&,Info&>* Dictionary<Key,Info>::Iterator::operator->()
{
    isValidToAcces();
    updatePair();
    return pair;
}

template <typename Key, typename Info>
const std::pair<const Key&,Info&>* Dictionary<Key,Info>::Iterator::operator->()const
{
    isValidToAcces();
    updatePair();
    return pair;
}

template <typename Key, typename Info>
void Dictionary<Key,Info>::Iterator::updatePair()
{
    if(pair == nullptr)
    {
        pair = new std::pair<const Key&,Info&>(curr->key, curr->info);
    }
    else
    {
        //key is const, so it can't be changed
        //that's why pair need to be created again
        delete pair;
        pair = new std::pair<const Key&,Info&>(curr->key, curr->info);
    }
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator& Dictionary<Key,Info>::Iterator::goRight()
{
    isValidToMove(Move::RIGHT);

    curr = curr->right;
    return *this;
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator Dictionary<Key,Info>::Iterator::getRight()const
{
    isValidToMove(Move::RIGHT);

    Iterator result = *this;
    result.goRight();
    return result;
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator& Dictionary<Key,Info>::Iterator::goLeft()
{
    isValidToMove(Move::LEFT);

    curr = curr->left;
    return *this;
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator Dictionary<Key,Info>::Iterator::getLeft()const
{
    isValidToMove(Move::LEFT);

    Iterator result = *this;
    result.goLeft();
    return result;
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator& Dictionary<Key,Info>::Iterator::goParent()
{
   isValidToMove(Move::PARENT);

    curr = curr->parent;
    return *this;
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator Dictionary<Key,Info>::Iterator::getParent()const
{
    isValidToMove(Move::PARENT);

    Iterator result = *this;
    result.goParent();
    return result;
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator& Dictionary<Key,Info>::Iterator::operator++()
{
    isValidToMove(Move::FORWARD);
    
    //higher element is in right subtree
    if(isRightPossible())
    {
        Iterator it = *this;
        it.goRight();
        //smallest element in this subtree is higher than current key
        while(it.isLeftPossible())
        {
            it.goLeft();
        }

        *this = it;
    }
    //higher node is somewhere above
    else
    {
        Iterator it = *this;
        while(true)
        {
            //looking for parent with higher key
            if(it.isParentPossible())
            {
                it.goParent();

                //found higher element
                if(it->first > this->curr->key)
                {   
                  *this = it;
                  break;
                }
            }
            else
            {
                //there is no parent with higher key and there is no node on the right
                //it is the highest element in the tree
                //iterator becomes end iterator
                curr = nullptr;
                break;
            }
        }   
    }

    return *this;
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator Dictionary<Key,Info>::Iterator::operator++(int)
{
    Iterator temp = *this;
    ++(*this);
    return temp;
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator& Dictionary<Key,Info>::Iterator::operator--()
{
    //check if not begin
    isValidToMove(Move::BACKWARD);

    //end iterator, so we need to move to the highest element
    if(*this == this->dictionary->end())
    {
        Iterator it = this->dictionary->top();
        while(it.isRightPossible())
        {
            it.goRight();
        }
        *this = it;
    }
    //smaller element is somewhere in the left subtree
    else if(isLeftPossible())
    {
        Iterator it = *this;
        it.goLeft();
        //highest element in left subtree is smaller than current key
        while(it.isRightPossible())
        {
            it.goRight();
        }
        *this = it;
    }
    else
    {
        Iterator it = *this;
        //It is different case than operator++ and checking(isParentPossible) is not required
        //If isValidToMove did not fail it means that iterator is not begin(smaller element exist)
        //If there is no left node, one of the parent need to be smaller
        while(true)
        {
            it.goParent();
            if(it->first < this->curr->key)
            {
                *this = it;
                 break;
            }
        }
    }
    return *this;
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator Dictionary<Key,Info>::Iterator::operator--(int)
{
    Iterator temp = *this;
    --(*this);
    return temp;
}

template <typename Key, typename Info>
bool Dictionary<Key,Info>::Iterator::operator!=(const Iterator& it) const
{
    return !(*this == it);
}

template <typename Key, typename Info>
bool Dictionary<Key,Info>::Iterator::operator==(const Iterator& it) const
{
    return (curr == it.curr && dictionary == it.dictionary);
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator& Dictionary<Key,Info>::Iterator::operator=(const Iterator& it)
{
    curr = it.curr;
    dictionary = it.dictionary;
    return *this;
}

//--------------------------------------------DICTIONARY------------------------------------------
template <typename Key, typename Info>
Dictionary<Key,Info>& Dictionary<Key,Info>::operator=(const Dictionary<Key,Info>& toCopy)
{
    copy(toCopy);

    return *this;
}

template <typename Key, typename Info>
void Dictionary<Key,Info>::copy(const Dictionary<Key,Info>& toCopy)
{
    //self copy
    if(this == &toCopy)
    {
        return;
    }

    clear();

    //nothing to copy
    if(toCopy.getSize() == 0)
    {
        return;
    }

    //if we add level by level tree will be all the time balanced
    //there will not be any rotations
    //dictionaries will be equal

    std::queue<Iterator> queue;
    queue.push(toCopy.top());

    while(queue.size() > 0)
    {
        Iterator it = queue.front();
        queue.pop();

        addNode(it->first,it->second);

        if(it.isRightPossible())
        {
            queue.push(it.getRight());
        }
        
        if(it.isLeftPossible())
        {
            queue.push(it.getLeft());
        }
    }
}

template <typename Key, typename Info>
bool Dictionary<Key,Info>::addNode(Key k, Info i)
{
    if(size == 0)
    {
        Node* toAdd = new Node(k,i);
        root = toAdd;
        ++size;
    }
    else
    {
        //find place for node
        Iterator it = top();
        while(true)
        {
            //go right
            if(k > it->first)
            {
                if(it.isRightPossible())
                {
                    it.goRight();
                }
                //there is no option to go right -> we found a proper place
                else
                {
                    Node* toAdd = new Node(k,i, it.curr);
                    it.curr->right = toAdd;
                    ++size;

                    //there is no element on the left and element is instered to the right
                    //as a result of that the height of parent changed
                    //we need to update bfactor and height of every parent and did rotations if necessary
                    if(!it.isLeftPossible())
                    {
                        updateNodesAndRotate(it);
                    }
                    //if there is element on the left and element on the right is inserted
                    //the height of the parent did not change, neither did height of other parents
                    //bfactor only changed from -1 to 0
                    else
                    {
                        it.curr->bfactor = 0;
                    }
                    break;
                }
            }
            else if(k < it->first)
            {
                if(it.isLeftPossible())
                {
                    it.goLeft();
                }
                //there is no option to go left -> we found proper place
                else
                {
                    Node* toAdd = new Node(k,i,it.curr);
                    it.curr->left = toAdd;
                    ++size;

                    if(!it.isRightPossible())
                    {
                        updateNodesAndRotate(it);
                    }
                    else
                    {
                        it.curr->bfactor = 0;
                    }
                    break;
                }
                
            }
            //k == curr->first
            else
            {
                //element already exist in the tree
                return false;
            }
        }
    }

    return true;
}

template <typename Key, typename Info>
bool Dictionary<Key,Info>::deleteNode(Key k)
{
    Iterator it = find(k);
    //if element wasn't found
    if(it == end())
    {
        return false;
    }

    return deleteNode(it);
}

template <typename Key, typename Info>
bool Dictionary<Key,Info>::deleteNode(Iterator it)
{
    if(it.dictionary != this)
    {
        throw std::invalid_argument("Iterator of other dictionary can't be used.");
    }

    if(it.isEnd())
    {
        throw std::invalid_argument("End iterator can't be used.");
    }

    if(it.isLeaf())
    {
        if(it.curr != root)
        {
            //break connection from parent to the deleted node
            if(it.curr->parent->left == it.curr)
            {
                it.curr->parent->left = nullptr;
            }
            else
            {
                it.curr->parent->right = nullptr;
            }
            //height of the parent did change
            //we need to update parents on the higher level
            updateNodesAndRotate(Iterator(it.curr->parent,this));
            delete it.curr;
        }
        //leaf is root -> 1 element in the dictionary
        else
        {
            delete root;
        }
        
    }
    else if(it.curr->left != nullptr && it.curr->right != nullptr)
    {
        //look for the node with empty right in left subtree
        Iterator toSwap = it;
        toSwap.goLeft();
        while(toSwap.isRightPossible())
        {
            toSwap.goRight();
        }

        //found node is copied into current one
        it.curr->key = toSwap.curr->key;
        it.curr->info = toSwap.curr->info;

        //toSwap is the first on the left from iterator it
        if(toSwap.curr->parent == it.curr)
        {
            //breaking connection between parent and toSwap
            //parent takes child of toSwap
            it.curr->left = toSwap.curr->left;
            if(it.curr->left != nullptr)
            {
                it.curr->left->parent = it.curr;
            }
        }
        //toSwap is depper than on the left from iterator it
        else
        {
            //breaking connection between parent and toSwap
            toSwap.curr->parent->right = toSwap.curr->left;
            if(toSwap.curr->left != nullptr)
            {
                toSwap.curr->left->parent = toSwap.curr->parent;
            }
        }
        
        //height of the parent of toSwap might have changed
        //some rotation might be needed also
        updateNodesAndRotate(Iterator(toSwap.curr->parent,this));
        delete toSwap.curr;
    }
    else if(it.curr->left == nullptr)
    {
        //if left subtree is height 0, right is at most height 1
        //the node on the right can't have child

        //copying data from right node
        it.curr->key = it.curr->right->key;
        it.curr->info = it.curr->right->info;
        
        Node* temp = it.curr->right;
        it.curr->right = nullptr;

        delete temp;
        //height of iterator it changed
        updateNodesAndRotate(it);
    }
    //right == nullptr
    else
    {
        //one node on the left
        it.curr->key = it.curr->left->key;
        it.curr->info = it.curr->left->info;

        Node* temp = it.curr->left;
        it.curr->left = nullptr;

        delete temp;
        //height of iterator it changed
        updateNodesAndRotate(it);
    }

    --size;
    if(size == 0)
    {
        root = nullptr;
    }

    return true;
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator Dictionary<Key,Info>::find(Key k)const
{
    if(size == 0)
    {
        return end();
    }

    Iterator it = this->top();
    while(it->first != k)
    {
        if(k > it->first)
        {
            if(it.isRightPossible())
            {
                it.goRight();
            }
            //if we can't go right there is no such an element in a dictionary
            else
            {
                return this->end();
            }
        }else if(k < it->first)
        {
            if(it.isLeftPossible())
            {
                it.goLeft();
            }
            //if we can't go element there is no such a key inside a dictionary
            else
            {
                return this->end();
            }
            
        }
    }

    return it;
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator Dictionary<Key,Info>::top() const
{
    return Iterator(root,this);
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator Dictionary<Key,Info>::begin() const
{
    Iterator it = top();
    //begin is the smallest element in the tree - the leftmost element
    while(it.isLeftPossible())
    {
        it.goLeft();
    }

    return it;
}

template <typename Key, typename Info>
typename Dictionary<Key,Info>::Iterator Dictionary<Key,Info>::end() const
{
    return Iterator(nullptr,this);
}


template <typename Key, typename Info>
void Dictionary<Key,Info>::updateNodesAndRotate(Dictionary<Key,Info>::Iterator it)
{
    while(true)
    {
        updateNode(it);

        //check if rotation is needed
        if(it.curr->bfactor == -2)
        {
            if(it.curr->left->bfactor == -1)
            {
                rotateRight(it);
            }
            else
            //it.curr->left->height == 1
            {
                rotateLeftRight(it);
            }
        }
        else if(it.curr->bfactor == 2)
        {
            if(it.curr->right->bfactor == 1)
            {
                rotateLeft(it);
            }
            else
            //it.curr->right->bfactor == -1
            {
                rotateRightLeft(it);
            }
        }

        //if node's height changes parent's height may also change
        //if height of parent changes the height of parent of parent may change etc.
        //so we need to check every parent up to root
        if(!it.isParentPossible())
        {
            break;
        }
        it.goParent();
    }
}

template <typename Key, typename Info>
void Dictionary<Key,Info>::updateNode(Dictionary<Key,Info>::Iterator it)
{
    int left = it.isLeftPossible() ? it.curr->left->height : 0;
    int right = it.isRightPossible() ? it.curr->right->height : 0;

    it.curr->height = std::max(left,right) + 1;
    it.curr->bfactor = right - left;
}


template <typename Key, typename Info>
void Dictionary<Key,Info>::clear()
{
    //the fastest way to delete nodes will be from the bottom
    //it will avoid unecessery rotations done in deleteNode

    if(size > 0)
    {
       std::stack<Iterator> stack = createStack();

        Iterator it;
        while(stack.size() > 0)
        {
            it = stack.top();
            stack.pop();
            deleteNode(it);
        }
    }

}

template <typename Key, typename Info>
void Dictionary<Key,Info>::rotateRight(Iterator it)
{  
   
 //         n1           n2
 //        /            /  \       .
 //      n2     ->     n3   n1
 //     /  \               /
 //   n3          

    //three nodes only changes
    //n1, n2 and right child of n2
    Node* n1 = it.curr;
    Node* n2 = it.curr->left;
    Node* temp = n2->right;

    //n1 becomes child of n2
    n2->parent = n1->parent;
    n1->parent = n2;
    n2->right = n1;
   
    //n1 takes right child of n2
    n1->left = temp;
    if(temp != nullptr)
    {
       temp->parent = n1;
    }

    //create connection from n2's parent to n2
    if(n2->parent != nullptr)
    {
        if(n2->parent->right == n1)
        {
            n2->parent->right = n2;
        }
        else
        {
            n2->parent->left = n2;
        }
        
    }

    //n1 was at a top, it might be a root
    //if yes, we need to change root to n2
    if(n1 == root)
    {
        root = n2;
    }

    //n1 and n2 children changes
    //so their bfactor and height may change
    //to proper calculate height of n2, we need to know height of its children(n1, n3)
    //that's why updating starts from n1
    updateNode(Iterator(n1,this));
    updateNode(Iterator(n2,this));
}

template <typename Key, typename Info>
void Dictionary<Key,Info>::rotateLeft(Iterator it)
{
//     n1               n2
 //     \              /  \      .
 //     n2      ->    n1   n3
 //    /  \             \       .
 //       n3

    //n1 n2 and left child of n2
    Node* n1 = it.curr;
    Node* n2 = it.curr->right;
    Node* temp = n2->left;

    //n1 becomes left child of n2
    n2->parent = n1->parent;
    n1->parent = n2;
    n2->left = n1;
    
    //n1 takes left child of n2
    n1->right = temp;
    if(temp != nullptr)
    {
        temp->parent = n1;
    }

    //connection from n2's parent to n2
    if(n2->parent != nullptr)
    {
        if(n2->parent->right == n1)
        {
            n2->parent->right = n2;
        }
        else
        {
            n2->parent->left = n2;
        }
        
    }

    //check if change of root is needed
    if(n1 == root)
    {
        root = n2;
    }

    //n1 and n2 children changes -> height of them need to be updated
    updateNode(Iterator(n1,this));
    updateNode(Iterator(n2, this));
}

template <typename Key, typename Info>
void Dictionary<Key,Info>::rotateRightLeft(Iterator it)
{
    //also known as double left rotation
    rotateRight(it.getRight());
    rotateLeft(it);
}

template <typename Key, typename Info>
void Dictionary<Key,Info>::rotateLeftRight(Iterator it)
{
    //also known as double right rotation
    rotateLeft(it.getLeft());
    rotateRight(it);
}

template<typename Key, typename Info>
bool Dictionary<Key,Info>::isAVL()
{
    if(root == nullptr)
    {
        return true;
    }

    std::stack<Iterator> stack = createStack();
    
    Iterator it;

    int bfactor;
    int height;

    while(stack.size() > 0)
    {
        it = stack.top();
        stack.pop();

        //saves data currently stored in the tree
        bfactor = it.curr->bfactor;
        height = it.curr->height;

        //calculates new bfactor and height of node
        updateNode(it);

        //if bfactor or height is different after recalculation than before
        //values in a tree were wrongly calculated during addNode or deleteNode
        if(it.curr->bfactor != bfactor || it.curr->height != height)
        {
            return false;
        }

        //absoulute value of bfactor can't be greater than 1
        //if it is greater, it means that it is not AVL tree
        if(it.curr->bfactor < -1 || it.curr->bfactor > 1)
        {
            return false;
        }
    }
    return true;
}


template <typename Key, typename Info>
std::stack<typename Dictionary<Key,Info>::Iterator> Dictionary<Key,Info>::createStack()
{
    //if we proceed through queue, we will acess nodes level by level from top
    //if add elements of queue to stack
    //we will be able to acces nodes level by level but from the bottom of the tree
    std::queue<Iterator> queue;
    std::stack<Iterator> stack;
    if(size > 0)
    {
        queue.push(top());
        Iterator it;
        while(queue.size() > 0)
        {
            it = queue.front();
            queue.pop();
            if(it.isLeftPossible())
            {
                queue.push(it.getLeft());
            }
            if(it.isRightPossible())
            {
                queue.push(it.getRight());
            }

            //every element poped from queue is pushed onto the stack
            stack.push(it);
        }
    }

    return stack;
}

#endif