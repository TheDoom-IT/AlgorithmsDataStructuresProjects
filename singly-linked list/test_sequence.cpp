#include <catch2/catch_all.hpp>
#include "sequence.hpp"

TEST_CASE( "Sequence tests", "[sequence]" ) 
{
    //basic constructor
    Sequence<double,int> seq;
    CHECK(seq.isEmpty());

    //Adding elements with pushFirst pushLast
    seq.pushFirst(1,1);
    seq.pushLast(2,4);
    seq.pushLast(3,5);
    seq.pushFirst(4,4);
    //seq == {4,4 1,1  2,4  3,5}

    //Checking elements with getFirst, getLast, operator[]
    CHECK(seq.getSize() == 4);
    CHECK(seq[0].key == 4);
    CHECK(seq[1].key == 1);
    CHECK(seq[seq.getSize()-1].info == 5);
    CHECK(seq.getLast().key == 3);
    CHECK(seq.getFirst().info == 4);

    //copy constructor
    Sequence <double,int> seq2(seq);
    //comparing every element using iterators
    for(auto x = seq2.begin(), y = seq.begin(); x != seq2.end() && y!=seq.end(); x++, y++)
    {
        CHECK((*x).key == (*y).key);
        CHECK((*x).info == (*y).info);
    }

    //clear
    seq2.clear();
    CHECK(seq2.isEmpty());
    //getFirst and getLast should throw, if sequence is empty
    CHECK_THROWS(seq2.getFirst());
    CHECK_THROWS(seq2.getLast());
    CHECK_THROWS(seq2[0].key);

    //deleting elements
    //seq == {4,4 1,1  2,4  3,5}
    seq.popFirst();
    CHECK(seq.getSize() == 3);
    CHECK(seq.getFirst().key == 1);

    seq.popLast();
    CHECK(seq.getSize() == 2);
    CHECK(seq.getLast().info == 4);
    //seq = {1,1  2,4}

    //operator=
    seq2 = seq;
    CHECK(seq2[0].key == 1);
    CHECK(seq2[1].info == 4);

    //iterators
    seq.pushLast(3,3);
    seq.pushLast(4,7);
    seq.pushLast(5,8);
    //seq = {1,1  2,4  3,3  4,7  5,8}
    //insert at beginning
    seq.insert(seq.begin(),0 ,5);
    CHECK(seq.getSize() == 6);
    CHECK(seq.getFirst().key == 0);

    //insert at the end
    seq.insert(seq.end(), 2, 2);
    CHECK(seq.getSize() == 7);
    CHECK(seq.getLast().info == 2);

    //seq = {0,5  1,1  2,4  3,3  4,7  5,8  2,2}
    //insert in the middle
    {
        auto it = seq.begin();
        it++;
        it++;

        //x points 2,4
        CHECK((*it).key == 2);
        seq.insert(it,4,4);
        CHECK(seq[2].key == 4);
        CHECK(seq.getSize() == 8);
    }

    //erase
    //seq = {0,5  1,1  4,4  2,4  3,3  4,7  5,8  2,2}
    seq.erase(seq.begin());
    CHECK(seq.getSize() == 7);
    CHECK(seq.getFirst().key == 1);

    //seq.end() points after last element, so it should throw an exception
    CHECK_THROWS(seq.erase(seq.end()));

    //erase somewhere in the middle
    //seq = {1,1  4,4  2,4  3,3  4,7  5,8  2,2}
    {
        auto it = seq.begin();
        ++it;
        ++it;
        //it points 2,4
        CHECK((*it).key == 2);
        seq.erase(it);
        //it is invalide now and shouldn't be used

        CHECK(seq.getSize() == 6);
        CHECK(seq[2].key == 3);
    }

    //shuffle
    Sequence<int,int> source1;
    Sequence<int,int> source2;
    for(int x = 1; x <= 5; x++)
    {
        source1.pushLast(x,1);
        source2.pushLast(x*10,1);
    }

    Sequence <int,int> result;

    //sources are to small to fill the result
    result = shuffle(source1,0,1,source2,0,1,30);
    CHECK(result.getSize() == 10);
    //result = {1,10,2,20,3,30 ...}
    CHECK(result[1].key == 10);
    CHECK(result.getLast().key == 50);

    //first source runs out of elements first, and the rest elements are coppied from source2
    result = shuffle(source1,2,2,source2, 1, 1,30);
    //result = {3,4,20,5,30,40,50}
    CHECK(result.getSize() == 7);
    CHECK(result.getLast().key == 50);
    CHECK(result[2].key == 20);
    CHECK(result[3].key == 5);

    //second source runs out of elements, rest elements coppied from source1
    result = shuffle(source1,0,1,source2, 4, 5,30);
    //result = {1,50,2,3,4,5}
    CHECK(result.getSize() == 6);
    CHECK(result[1].key == 50);
    CHECK(result[4].key == 4);

    //all elemets can't be copied because of the limit
    result = shuffle(source1,0,2,source2, 1, 2,7);
    //result = {1,2,20,30,3,4,40}
    CHECK(result.getSize() == 7);
    CHECK(result.getLast().key == 40);
    CHECK(result[4].key == 3);
}


