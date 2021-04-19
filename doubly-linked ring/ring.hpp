#ifndef RING_HPP
#define RING_HPP
#include <utility>

template <typename Data>
class Ring 
{
private:
    //element on which ring operates
    struct Node
    {
        Data data;
        Node* next;
        Node* previous;
        Node(Data d,Node* n = nullptr,Node* p = nullptr) : 
        data(d), next(n), previous(p){};
    };
public:
class Iterator
    {
    protected:
        //the ring to which iterator belongs
        //used in check if iterator belongs to ring, whose method is involved
        const Ring<Data>* ring;
        Node* current;
        //first node pointed by iterator
        //if iterator do a full cicrcle and points first element, it becomes end iterator
        Node* first;
        //private constructor, inaccessible for user
        //makes things faster inside methods
        Iterator(const Ring<Data>* r,Node* frs,Node* curr) : ring(r),current(curr), first(frs){};
        //check if iterator can be decremented or incremented
        //if not throws and exception
        void isValidToMove(bool forward);
        //check if iterator can be dereferenced
        //if not throws an exception
        void isValidToAcces();
    public:
        bool isEnd(){return ring != nullptr && current == nullptr;};
        bool isBegin(){return ring != nullptr && current == first;};
        bool isEmpty(){return first == nullptr;};

        Iterator() : ring(nullptr), current(nullptr), first(nullptr) {};
        Iterator(const Iterator& it){*this = it;};

        //Basic operations on iterator
        Iterator& operator++();
        Iterator operator++(int);
        Iterator operator+(int) const;

        Iterator& operator--();
        Iterator operator--(int);
        Iterator operator-(int) const;
        
        //accesing data
        Data& operator*();
        const Data& operator*()const;

        bool operator!=(const Iterator&) const;
        bool operator==(const Iterator&) const;
        Iterator& operator=(const Iterator&);

        friend class Ring;
    };
private:
    Node* any;
    int size;
public:
    int getSize()const {return size;};
    bool isEmpty() const {return size == 0;};


    Iterator begin();
    Iterator begin() const;
    Iterator end();
    Iterator end() const;
    Data& getFirst();
    const Data& getFirst() const;
    Data& getLast();
    const Data& getLast() const;
 

    //insert element before given iterator
    void insert(Iterator,Data);
    //if we push element as first it will become new any
    void pushFirst(Data data) {insert(begin(),data); any = any->previous;};
    void pushLast(Data data) {insert(end(),data);};
    void copy(const Ring<Data>&);

    //erase element pointed by iterator
    void erase(Iterator);
    void popFirst();
    void popLast();
    void clear();
    
    //check if element with given data belong to ring
    bool isInside(const Data&) const;
	

    Ring() : any(nullptr),size(0){};
    Ring(const Ring<Data>&);
    ~Ring();
    Ring<Data>& operator=(const Ring<Data>&);
};


//-----------------ITERATOR---------------
template <typename Data>
void Ring<Data>::Iterator::isValidToAcces()
{
    if(isEmpty())
    {
        throw std::logic_error("Empty iterator can't be dereferenced.");
    }
    if(isEnd())
    {
        throw std::logic_error("End iterator can't be dereferenced.");
    }
}

template <typename Data>
void Ring<Data>::Iterator::isValidToMove(bool forward)
{
    if(isEmpty())
    {
        throw std::logic_error("Empty iterator can't be moved.");
    }

    //moving to next element
    if(forward)
    {
        if(isEnd())
        {
            throw std::logic_error("End iterator can't be incremented.");
        }
    }
    //moving to previous element
    else
    {
        if(isBegin())
        {
            throw std::logic_error("Begin iterator can't be decremented.");
        }
    }
    
}

template <typename Data>
bool Ring<Data>::Iterator::operator==(const Iterator& it) const
{
    return (ring == it.ring && current == it.current && first == it.first);
}


template <typename Data>
bool Ring<Data>::Iterator::operator!=(const Iterator& it) const
{
    return (ring != it.ring || first != it.first || current != it.current);
}


template <typename Data>
typename Ring<Data>::Iterator& Ring<Data>::Iterator::operator++()//prefix
{
    isValidToMove(true);
    current = current->next;
    //we did a full circle, hence iterator will become end iterator
    if(current == first)
    {
        //current is NULL for end iterator
        current = nullptr;
    }
    return *this;
}

template <typename Data>
typename Ring<Data>::Iterator Ring<Data>::Iterator::operator++(int)//postfix
{
    //do not need to check iterator
    //prefix ++ operator will do it
    Iterator result = *this;
    ++(*this);
    return result;
}

template <typename Data>
typename Ring<Data>::Iterator Ring<Data>::Iterator::operator+(int times) const
{
    //do not need to check iterator
    //prefix ++ operator will do it

    Iterator result(*this);
    for(;times > 0; --times)
    {
        ++result;
    }
    return result;
}

template <typename Data>
typename Ring<Data>::Iterator& Ring<Data>::Iterator::operator--()//prefix
{
    isValidToMove(false);
    //end iterator can be decremented too
    if(isEnd())
    {
        current = first->previous;
    }
    else
    {
        current = current->previous;
    }
    return *this;
}

template <typename Data>
typename Ring<Data>::Iterator Ring<Data>::Iterator::operator--(int)//postfix
{
    Iterator result = *this;
    --(*this);
    return result;
}

template <typename Data>
typename Ring<Data>::Iterator Ring<Data>::Iterator::operator-(int times) const
{
    Iterator result = *this;
    for(;times > 0; --times)
    {
        --result;
    }
    return result;
}

template <typename Data>
Data& Ring<Data>::Iterator::operator*()
{
    isValidToAcces();
    return current->data;
}

template <typename Data>
const Data& Ring<Data>::Iterator::operator*() const
{
    isValidToAcces();
    return current->data;
}

template <typename Data>
typename Ring<Data>::Iterator& Ring<Data>::Iterator::operator=(const Iterator& it)
{
    first = it.first;
    current = it.current;
    ring = it.ring;
    return *this;
}
//---------------RING------------------

template <typename Data>
Ring<Data>::Ring(const Ring<Data>& toCopy) : any(nullptr), size(0)
{
    copy(toCopy);
}

template <typename Data>
Ring<Data>::~Ring()
{
    clear();
}

template <typename Data>
Ring<Data>& Ring<Data>::operator=(const Ring<Data>& toCopy)
{
    //self-copy check inside copy function
    copy(toCopy);
    return *this;
}

template <typename Data>
typename Ring<Data>::Iterator Ring<Data>::begin()
{
    return Iterator(this,any,any);
}

template <typename Data>
typename Ring<Data>::Iterator Ring<Data>::begin() const
{
    return Iterator(this,any,any);
}

template <typename Data>
typename Ring<Data>::Iterator Ring<Data>::end()
{
    return Iterator(this,any,nullptr);
}

template <typename Data>
typename Ring<Data>::Iterator Ring<Data>::end() const
{
    return Iterator(this,any,nullptr);
}

template <typename Data>
Data& Ring<Data>::getFirst()
{
    if(isEmpty())
    {
        throw std::logic_error("Ring is empty, there is no element to get.");
    }
    return *begin();
}

template <typename Data>
const Data& Ring<Data>::getFirst() const
{
    if(isEmpty())
    {
        throw std::logic_error("Ring is empty, there is no element to get.");
    }
    return *begin();
}

template <typename Data>
Data& Ring<Data>::getLast()
{
    if(isEmpty())
    {
        throw std::logic_error("Ring is empty, there is no element to get.");
    }
    return *(end()-1);
}

template <typename Data>
const Data& Ring<Data>::getLast() const
{
    if(isEmpty())
    {
        throw std::logic_error("Ring is empty, there is no element to get.");
    }
    return *(end()-1);
}

template <typename Data>
void Ring<Data>::copy(const Ring<Data>& toCopy)
{
    if(this == &toCopy)
    {
        return;
    }
    clear();

    for(auto x = toCopy.begin(); x != toCopy.end(); x++)
    {
        insert(this->end(),(*x));
    }
}


template <typename Data>
void Ring<Data>::insert(Iterator place, Data data)
{
    if(place.ring != this)
    {
        throw std::invalid_argument("Other's ring iterator can't be used.");
    }

    if(size == 0)
    {
        any = new Node(data);
        any->next = any->previous = any;
        ++size;
    }
    else if(place == end())
    {
        //placing before end is equivalent to placing before begin
        insert(begin(),data);
    }
    else
    //somewhere in the middle
    {
        Node* toInsert = new Node(data,place.current,place.current->previous);
        place.current->previous->next = toInsert;
        place.current->previous = toInsert;
        ++size;
    }
}


template <typename Data>
void Ring<Data>::erase(Iterator place)
{
    if(place.ring != this)
    {
        throw std::invalid_argument("Other's ring iterator can't be used.");
    }
    else if(place == end())
    {
        throw std::invalid_argument("End iterator can't be used.");
    }

    if(size == 1)
    {
        delete any;
        any = any->previous = any->next = nullptr;
    }
    else
    {
        //any will be deleted, we need to change it
        if(place == begin())
            any = any->next;

        place.current->previous->next = place.current->next;
        place.current->next->previous = place.current->previous;
        delete place.current;
    }
    --size;

}

template <typename Data>
void Ring<Data>::popFirst()
{
    if(isEmpty())
    {
        throw std::logic_error("Ring is empty, element can't be removed.");
    }
    erase(begin());
}

template <typename Data>
void Ring<Data>::popLast()
{
    if(isEmpty())
    {
        throw std::logic_error("Ring is empty, element can't be removed.");
    }
    erase(end() - 1);
}

template <typename Data>
void Ring<Data>::clear()
{
    while(getSize() != 0)
    {
        erase(begin());
    }
}

template <typename Data>
bool Ring<Data>::isInside(const Data& data) const
{
	for(auto x = begin(); x != end(); ++x)
	{
		if(*x == data)
		{
			return true;
		}
	}
	
	return false;
}

//function used in split
//iterator can be infinitely moved without reaching end
template <typename Data>
void moveRingIterator(typename Ring<Data>::Iterator& it,const Ring<Data>& ring, bool direction)
{
    //move to next element
    if(direction)
    {
        //end iterator and last element iterator will change into begin
        if(it == ring.end() || it == ring.end() - 1)
        {
            it = ring.begin();
        }
        else
        {
            ++it;
        }
    }
    //move to previous element
    else
    {
        //first element will change into last
        if(it == ring.begin())
        {
            it = ring.end() - 1;
        }
        else
        {
            --it;
        }
    }
}				

//function used in split
//insert element in proper position in order to keep given direction
template <typename Data>
void insertInDirection(Ring<Data>& ring, const Data& data, bool direction)
{
	if(direction)
	{
		ring.insert(ring.end(),data);
	}
	else
	{
		if(ring.isEmpty())
		{
			ring.insert(ring.begin(),data);
		}
		else
		{
			ring.insert(ring.begin()+1,data);
		}
	}
}


template <typename Data>
std::pair<Ring<Data> , Ring<Data>> split(const Ring<Data> &source,int startIndex, int length, 
										 bool direction,int step1, bool direction1, int step2, bool direction2)
{
    if(startIndex < 0)
    {
        throw std::invalid_argument("Start index can't be negative.");
    }
    if(step1 < 0 || step2 < 0)
    {
        throw std::invalid_argument("Step can't be negative number.");
    }
	std::pair<Ring<Data>, Ring<Data>> result;
	if(source.isEmpty())
	{
		//two empty rings
		return result;
	}
	else
	{
		//if startIndex > size
		startIndex = startIndex % source.getSize();
		typename Ring<Data>::Iterator it = source.begin() + startIndex;
		
		while(length > 0)
		{
            for(int x = 0; x < step1 + step2 && length > 0; ++x)
            {
                if(x < step1)
                {
                    insertInDirection(result.first,*it,direction1);
                }
                else
                {
                    insertInDirection(result.second,*it,direction2);
                }

                --length;
                moveRingIterator<Data>(it, source, direction);
            }
		}
	}
	
	return result;
}

#endif