#include <catch2/catch_all.hpp>
#include "ring.hpp"

void createRing(Ring<int>& ring, int size)
{
    for(int x = 0; x < size; ++x)
    {
        ring.insert(ring.end(),x + 1);
    }
}

TEST_CASE("Inserting elements using iterators and operators of iterator") 
{
    
    Ring<int> test;
    CHECK(test.isEmpty());

    //insert into empty ring
    test.insert(test.begin(),1);
    //inserting at the end
    test.insert(test.end(),3);
    
    //inserting in the middle
    Ring<int>::Iterator it = test.begin();
    it++;
    test.insert(it,2);
    CHECK(test.getSize() == 3);
    
    
    //checking if test is {1,2,3} using iterators and + - operators
    //operator+
    for(int x = 0; x < 3; ++x)
    {
        CHECK(*(test.begin() + x) == x + 1);
    }
    
    //operator++(int)
    it = test.begin();
    CHECK(*(it++) == 1);
    
    CHECK(*it == 2);
    
    //operator++()
    CHECK(*(++it) == 3);

    //operator-
    for(int x = 1; x <= 3; x++)
    {
        CHECK(*(test.end()-x) == 4 - x);
    }

    //operator--(int)
    it = test.end();
    //end iterator can't be dereferenced
    CHECK_THROWS(*(it--));
    CHECK(*it == 3);
    //operator--()
    CHECK(*(--it) == 2);
}

TEST_CASE("Deleting elements using iterators")
{
    Ring<int> test;
    createRing(test, 10);
    CHECK(test.getSize() == 10);

    //first element
    test.erase(test.begin());
    //last element
    test.erase(test.end() - 1);
    for(int x = 0; x < test.getSize(); x++)
    {
        CHECK(*(test.begin() + x) == x + 2);
    }

    //clear
    test.clear();
    CHECK_THROWS(*test.begin());
    CHECK(test.getSize() == 0);

    createRing(test,5);
    //somewhere in the middle
    test.erase(test.begin() + 2);
    //element with value 3 should be erased
    CHECK(*(test.begin() + 1) == 2);
    CHECK(*(test.begin() + 2) == 4);
}

TEST_CASE("Copying rings")
{
	Ring <int> ring1;
	createRing(ring1,5);
	
	//copy constructor
	Ring<int> ring2(ring1);
    
	CHECK(ring2.getSize() == ring1.getSize());
	
	for(int x = 0; x < ring1.getSize(); ++x)
	{
		CHECK(*(ring2.begin() + x) == x + 1);
	}
	
	ring2.clear();
	
	//operator=
	createRing(ring2,3);
	ring1 = ring2;
	CHECK(ring1.getSize() == ring2.getSize());
	for(int x = 0; x < ring1.getSize(); ++x)
	{
		CHECK(*(ring1.begin() + x) == *(ring2.begin() + x));
	}
}

TEST_CASE("Other methods")
{
	Ring<int> ring;
	createRing(ring,5);
	
	//find
	for(int x = 0; x < ring.getSize(); ++x)
	{
		CHECK(ring.isInside(x + 1) == true);
	}
	
	CHECK(ring.isInside(6) == false);
	CHECK(ring.isInside(-100) == false);

    CHECK(ring.getFirst() == 1);
    CHECK(ring.getLast() == 5);

    ring.popFirst();
    ring.popLast();
    CHECK(ring.getSize() == 3);
    CHECK(ring.getFirst() == 2);
    CHECK(ring.getLast() == 4);

    ring.pushFirst(10);
    ring.pushLast(20);
    CHECK(ring.getSize() == 5);
    CHECK(ring.getFirst() == 10);
    CHECK(ring.getLast() == 20);
}

TEST_CASE("Split")
{
    Ring<int> ring1;
    createRing(ring1,10);

    //negative index
    CHECK_THROWS(split(ring1,-1,15,false,3,true,1,true));
    
    //length 0
    auto result = split(ring1,0,0,false,3,true,1,false);
    CHECK(result.first.getSize() == 0);
    CHECK(result.second.getSize() == 0);

    result = split(ring1,0,15,true,3,true,1,true);
    //1 {1,2,3,5,6,7,9,10,1,3,4,5}
    //2 {4,8,2}
    CHECK(result.first.getSize() == 12);
    CHECK(result.second.getSize() == 3);

    //1 == 1
    CHECK(*(result.first.begin()) == *(result.first.begin() + 8));
    //4 == 4
    CHECK(*(result.second.begin()) == *(result.first.end() - 2));

    //similar to case in taks description
    result = split(ring1, 2, 16, true, 3, true, 2, false);

    //1 {3,4,5,8,9,10,3,4,5,8}
    //2 {6,7,6,2,1,7}
    //3==3
    CHECK(*(result.first.begin()) == *(result.first.end()-4));
    //6==6
    CHECK(*(result.second.begin()) == *(result.second.begin() + 2));
    CHECK(*(result.first.end()-1) == 8);
    CHECK(*(result.second.end()-1) == 7);
}












