#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

template <typename Key, typename Info>
class Sequence;

template <typename Key, typename Info>
Sequence<Key, Info> shuffle (const Sequence<Key, Info> & source1, int startIndex1, int length1,
                             const Sequence<Key, Info> & source2, int startIndex2, int length2,
                             int limit);


template <typename Key, typename Info>
struct Node
{
    Key key;
    Info info;

    //constructors to make creating new nodes easier and faster
    Node(Key k, Info i);
    Node(Node<Key, Info>&);

private:
    //Node objects can be modified by the user via access methods in Sequence,
    //We can't give user the acces to next, that's why it is private.
    Node* next;
    //Sequence class need to have access to Node* next field.
    friend class Sequence<Key, Info>;
};


template <typename Key, typename Info>
class Sequence
{
private:
    //pointer to the first element
    Node<Key,Info>* head;
    //pointer to the last element
    Node<Key, Info>* tail;
    int size;
    //Copies given number of elements(length) from source.
    //Return true if succed, false if the source don't have enough elements or limit has been reached
    bool copyFromSequence(const Sequence<Key, Info>& source,int startIndex, int length, int limit);
public:
    class Iterator
    {
    private:
        Node<Key,Info>* currentNode;
    public:
        Iterator(){currentNode = NULL;};
        Iterator(Node<Key, Info>*& node) {currentNode = node;};

        //Basic operations on iterator
        Iterator& operator++();
        Iterator operator++(int);
        Node<Key, Info>& operator*();
        bool operator!=(const Iterator&);

        //Some methods in sequence need to acces currentNode
        friend class Sequence;
    };
    //add elements
    void pushFirst(Key,Info);
    void pushLast(Key,Info);
    //insert an element before the element pointed by iterator
    void insert(Iterator,Key,Info);

    //delete elements
    void popFirst();
    void popLast();
    //delete element pointed by iterator
    void erase(Iterator);

    //accesing elements
    //Const and non-const methods were needed. Some methods takes const Sequence
    //as a parameter, so we need const methods to acces its elements
    Node<Key, Info>& getFirst();
    const Node<Key, Info>& getFirst() const;
    Node<Key, Info>& getLast();
    const Node<Key, Info>& getLast() const;
    Node<Key,Info>& operator[](int);
    const Node<Key,Info>& operator[](int)const;
    Iterator begin();
    Iterator end();

    //erase all the elements from the sequence
    void clear();
    //erase current sequence and make a deep copy of given sequence
    void copy(const Sequence<Key,Info>&);


    bool isEmpty() const{return size == 0;}
    int getSize() const {return size;};

    Sequence();
    Sequence(const Sequence<Key,Info>&);
    ~Sequence();

    Sequence<Key,Info>& operator=(const Sequence<Key,Info>&);

    
    friend Sequence<Key, Info> shuffle<> (const Sequence<Key, Info> & source1, int startIndex1, int length1,
                                        const Sequence<Key, Info> & source2, int startIndex2, int length2,
                                        int limit);


};


//------------------------------NODE------------------------------
template <typename Key, typename Info>
Node<Key,Info>::Node(Key k, Info i)
{
    key = k;
    info = i;
    next = NULL;
}

template <typename Key, typename Info>
Node<Key,Info>::Node(Node<Key,Info>& node)
{
    key = node.key;
    info = node.info;
    next = NULL;
}

//---------------------ITERATOR---------------------
template <typename Key, typename Info>
Node<Key, Info>& Sequence<Key,Info>::Iterator::operator*()
{
    if(currentNode == NULL)
        throw std::out_of_range("Iterator can't be dereferenced. It is null iterator or points end of the sequence.");
    
    return *currentNode;
}

template <typename Key, typename Info>
typename Sequence <Key,Info>::Iterator& Sequence<Key,Info>::Iterator::operator++()
{
    if(currentNode == NULL)
        throw std::out_of_range("Iterator can't be incremented. It is null iterator or points end of the sequence.");
    
    currentNode = currentNode->next;
    return *this;
}

template <typename Key, typename Info>
typename Sequence<Key,Info>::Iterator Sequence<Key,Info>::Iterator::operator++(int)
{
    if(currentNode == NULL)
        throw std::out_of_range("Iterator can't be incremented. It is null iterator or points end of the sequence.");
    
    Iterator result = *this;
    currentNode = currentNode->next;
    return result;
}

template <typename Key, typename Info>
bool Sequence<Key,Info>::Iterator::operator!=(const Iterator& it)
{
    return(currentNode != it.currentNode); 
}

//------------------SEQUENCE------------------
template <typename Key, typename Info>
Sequence<Key,Info>::Sequence()
{
    head = tail = NULL;
    size = 0;
}

template <typename Key, typename Info>
Sequence<Key,Info>::Sequence(const Sequence<Key,Info>& toCopy)
{
    head = tail = NULL;
    size = 0;
    copy(toCopy);
}

template <typename Key, typename Info>
Sequence<Key,Info>::~Sequence()
{
    clear();
}

template <typename Key, typename Info>
void Sequence<Key,Info>::pushFirst(Key k,Info i)
{
    Node<Key,Info>* toAdd = new Node(k,i);
    if(size == 0)
    {
        head = toAdd;
        tail = toAdd;
    }
    else
    {
        toAdd->next = head;
        head = toAdd;
    }
    size++;
}

template <typename Key, typename Info>
void Sequence<Key,Info>::pushLast(Key k,Info i)
{
    Node<Key,Info>* toAdd = new Node(k,i);
    if(size == 0)
    {
        head = toAdd;
        tail = toAdd;
    }
    else
    {
        tail->next = toAdd;
        tail = toAdd;
    }
    size++;
}

template <typename Key, typename Info>
void Sequence<Key,Info>::insert(Iterator position,Key k,Info i)
{
    //Iterator points end of sequence
    if(position.currentNode == end().currentNode)
        pushLast(k,i);
    else if(position.currentNode == head)
        pushFirst(k,i);
    else
    {
       //find the position where to insert
       Node<Key,Info>* temp = head;
       while(temp->next != position.currentNode)
       {
            temp = temp->next;
            //If user use iterator from other sequence, the position won't be found.
            if(temp == NULL)
                throw std::invalid_argument("Iterator belongs to other sequence.");
       }

        //Element is inserter between the temp and position.currentNode
        Node<Key,Info>* toInsert = new Node(k,i);
        temp->next = toInsert;
        toInsert->next = position.currentNode;
        ++size;
    }
}

template <typename Key, typename Info>
void Sequence<Key,Info>::popFirst()
{
    //check if there is an element to delete
    if(size == 0)
        return;

    //delete this element
    Node<Key,Info>* temp = head;
    head = head->next;
    delete temp;
    size--;

    //if sequence is empty, the tail need to be NULL
    //head was set to NULL previously head = head->next
    if(size == 0)
        tail = NULL;
}

template <typename Key, typename Info>
void Sequence<Key,Info>::popLast()
{
    //if sequence is empty, there is nothing to delete
    if(size == 0)
        return;

    //one element case
    //head == tail
    //there is no element preceding tail
    if(size == 1)
    {
        delete tail;
        head = tail = NULL;
        size = 0;
        return;
    }

    //looking for element preceding tail
    Node<Key,Info>* temp = head;
    while(temp->next != tail)
        temp = temp->next;
    
    delete tail;
    tail = temp;
    tail->next = NULL;
    size--;
}

template <typename Key, typename Info>
void Sequence<Key,Info>::erase(Iterator position)
{
    //Iterator points end of sequence
    if(position.currentNode == end().currentNode)
        throw std::invalid_argument("Iterator points end of sequence. There is nothing to erase.");
    else if(position.currentNode == tail)
    {
        popLast();
    }
    else if(position.currentNode == head)
    {
        popFirst();
    }
    else
    {
        //find the position where to delete
        Node<Key,Info>* temp = head;
        while(temp->next != position.currentNode)
        {
          temp = temp->next;
          //If user use iterator from other sequence, the position won't be found.
          if(temp == NULL)
          throw std::invalid_argument("Iterator belongs to other sequence.");
        }
        temp->next = temp->next->next;
        delete position.currentNode;
        --size;
    }
    
}

template <typename Key, typename Info>
Node<Key,Info>& Sequence<Key,Info>::getFirst()
{
    if(size == 0)
        throw std::out_of_range("The sequence is empty. There is no first element.");
    
    return *head;
}

template <typename Key, typename Info>
const Node<Key,Info>& Sequence<Key,Info>::getFirst() const
{
    if(size == 0)
        throw std::out_of_range("The sequence is empty. There is no first element.");
    
    return *head;
}

template <typename Key, typename Info>
Node<Key,Info>& Sequence<Key,Info>::getLast()
{
    if(size == 0)
        throw std::out_of_range("The sequence is empty. There is no last element.");
    
    return *tail;
}

template <typename Key, typename Info>
const Node<Key,Info>& Sequence<Key,Info>::getLast() const
{
    if(size == 0)
        throw std::out_of_range("The sequence is empty. There is no last element.");
    
    return *tail;
}

template <typename Key, typename Info>
Node<Key,Info>& Sequence<Key,Info>::operator[](int index)
{
    if(size == 0 || index < 0 || index >= size)
        throw std::out_of_range("The sequence is empty or index is out of range.");

    //look for element
    Node<Key, Info>* temp = head;
    for(int x = 0; x < index; x++)
        temp = temp->next;

    return *temp;
}

template <typename Key, typename Info>
const Node<Key,Info>& Sequence<Key,Info>::operator[](int index) const
{
    if(size == 0 || index < 0 || index >= size)
        throw std::out_of_range("The sequence is empty or index is out of range.");

    Node<Key, Info>* temp = head;
    for(int x = 0; x < index; x++)
        temp = temp->next;

    return *temp;
}

template <typename Key, typename Info>
typename Sequence<Key, Info>::Iterator Sequence<Key,Info>::begin()
{
    return Iterator(head);
}

template <typename Key, typename Info>
typename Sequence<Key, Info>::Iterator Sequence<Key,Info>::end()
{
    //end iterator points to NULL value
    //basic constructor creates such an iterator
    return Iterator();
}


template <typename Key, typename Info>
Sequence<Key,Info>& Sequence<Key,Info>::operator=(const Sequence<Key,Info>& toCopy)
{
    //self-copy protection is inside copy method
    copy(toCopy);
    return *this;
}

template <typename Key, typename Info>
void Sequence<Key,Info>::clear()
{
    Node<Key,Info>* temp;
    while(head != NULL)
    {
        temp = head;
        head = head->next;
        delete temp;
    }
    tail = NULL;

    size = 0;
}


template <typename Key, typename Info>
void Sequence<Key,Info>::copy(const Sequence<Key,Info>& toCopy)
{
    if(this == &toCopy)
        return;

    clear();

    Node<Key,Info>* temp = toCopy.head;
    while(temp != NULL)
    {
        pushLast(temp->key,temp->info);
        temp = temp->next;
    }
}

template <typename Key, typename Info>
bool Sequence<Key,Info>::copyFromSequence(const Sequence<Key, Info> & source, int startIndex,int length, int limit)
{
    for(int x = 0; x < length; x++)
    {
        //limit has been reached
        if(limit == this->size)
            return false;
        if(source.size > startIndex)
        {
            pushLast(source[startIndex].key,source[startIndex].info);
            ++startIndex;
        }
        else
            //the source does not have enough elements to insert given amount of elements
            return false;
    }
    //elements were added succesfully
    return true;
}

template <typename Key, typename Info>
Sequence<Key, Info> shuffle (const Sequence<Key, Info> & source1, int startIndex1, int length1,
                             const Sequence<Key, Info> & source2, int startIndex2, int length2,
                             int limit)
{
    Sequence<Key, Info> result;

    while(true)
    {   
        //End of first source has been reached or result is full now
        if(!result.copyFromSequence(source1,startIndex1,length1,limit))
        {
            //If source1 has reached end, rest of the elements will be copied from source 2
            //If limit has been reached, the method will do nothing
            result.copyFromSequence(source2, startIndex2, limit - result.size, limit);
            break;
        }
        else
        //Elements were succesfully added, so move the startIndex
        startIndex1+=length1;

        if(!result.copyFromSequence(source2,startIndex2,length2,limit))
        {
            result.copyFromSequence(source1, startIndex1, limit - result.size, limit);
            break;
        }
        else
        startIndex2+=length2;
    }

    return result;
}

#endif