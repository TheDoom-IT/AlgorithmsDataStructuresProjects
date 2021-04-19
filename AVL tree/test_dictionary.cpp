#include <catch2/catch_all.hpp>
#include "dictionary.hpp"

void createDictionary(Dictionary<char,int>& test, std::string str = "dbfaceg")
{
    for(std::size_t x = 0; x < str.size(); ++x)
    {
        test.addNode(str[x],1);
    }
    /*the tree should look like that:
                d
            b       f
          a   c    e  g
    */
}

TEST_CASE("Iterators and adding without any rotations") 
{
    Dictionary<char,int> test;
    createDictionary(test);

    /*the tree should look like that:
                d
            b       f
          a   c    e  g
    */

    CHECK(test.isAVL());
    CHECK(test.getSize() == 7);
    CHECK(test.getHeight() == 3);
    //operator ->
    //top()
    Dictionary<char,int>::Iterator it(test.top());
    CHECK(it->first == 'd');
    CHECK(it->second == 1);

    //getLeft getRight
    CHECK(it.getLeft()->first == 'b');
    CHECK(it.getRight()->first == 'f');

    //goLeft
    it.goLeft();
    CHECK(it->first == 'b');
    CHECK(it.getLeft()->first == 'a');
    CHECK(it.getRight()->first == 'c');

    //goParent
    it.goParent();
    CHECK(it->first == 'd');
    CHECK(it->second == 1);

    //goRight
    it.goRight();
    CHECK(it->first == 'f');
    CHECK(it.getLeft()->first == 'e');
    CHECK(it.getRight()->first == 'g');

    //getParent
    CHECK(it.getParent()->first == 'd');

    //operator* ++ --
    it = test.end();
    
    CHECK_THROWS(++it);
    CHECK_THROWS(it++);

    CHECK((*(--it)).first == 'g');
    CHECK((*(it--)).first == 'g');
    CHECK((*it).first == 'f');
    CHECK((*it).second == 1);

    it = test.begin();
    CHECK_THROWS(--it);
    CHECK_THROWS(it--);

    it = test.top();
    CHECK((*it).first == 'd');
    CHECK((*(++it)).first == 'e');
    CHECK((*(it++)).first == 'e');
    CHECK((*it).first == 'f');

    //changing info
    it = test.top();
    it->second = 10;
    CHECK((*test.top()).second == 10);

    (*it).second = 5;
    CHECK(it->second == 5);
    it->second = 1;

    //begin end
    int index = 0;
    std::string str = "abcdefg";
    //should move from the smallest to the highest element
    for(auto x = test.begin(); x != test.end(); ++x)
    {
        CHECK((*x).first == str[index]);
        CHECK((*x).second == 1);
        ++index;
    }

    //operator == !=
    it = test.begin();
    CHECK(it == test.begin());
    CHECK(it != test.top());

    //isLeaf
    it = test.begin();
    CHECK(it->first == 'a');
    CHECK(it.isLeaf());

    //is****Possible
    it = test.begin();
    CHECK(it.isRightPossible() == false);
    CHECK(it.isLeftPossible() == false);
    CHECK(it.isParentPossible() == true);

    it = test.top();
    CHECK(it.isRightPossible() == true);
    CHECK(it.isLeftPossible() == true);
    CHECK(it.isParentPossible() == false);

    //isEnd isEmpty
    Dictionary<char,int>::Iterator it2;
    CHECK(it2.isEmpty());
    it2 = test.end();
    CHECK(it2.isEnd());
}

TEST_CASE("Usage of find")
{
    Dictionary<char,int> test;
    createDictionary(test);

    Dictionary<char,int>::Iterator it(test.find('a'));

    CHECK(it.isEnd() == false);
    CHECK(it->first == 'a');

    it = test.find('d');
    CHECK(it.isEnd() == false);
    CHECK(it->first == 'd');
    CHECK(it->second == 1);

    it = test.find('1');
    CHECK(it.isEnd());

    CHECK(test.find('w').isEnd());
}

TEST_CASE("Removing leaf without rotations")
{
    Dictionary<char,int> test;
    createDictionary(test);

    /*the tree should look like that:
                d
            b       f
          a   c    e  g
    */

    //remove leaf
    auto it = test.begin();
    it.goParent();
    it.goRight();
    //it points c
    CHECK(test.deleteNode(it));

    /*
                d
            b       f
          a        e  g
    */

    CHECK(test.getSize() == 6);
    //check if conditions of AVL are passed
    CHECK(test.isAVL());
    it = test.top();
    it.goLeft();
    CHECK(it->first == 'b');
    CHECK(it.isRightPossible() == false);
    CHECK(it.getLeft()->first == 'a');

    //find
    it = test.find('c');
    CHECK(it.isEnd());
}

TEST_CASE("Removing node with two children without rotations")
{
    Dictionary<char,int> test;
    createDictionary(test,"dbfaeg");

    /*
                d
            b       f
          a        e  g
    */

    test.deleteNode('f');
    /*
                d
            b       e
          a           g
    */

    CHECK(test.getSize() == 5);
    CHECK(test.find('f').isEnd());
    CHECK(test.isAVL());

    auto it = test.top();
    it.goRight();
    CHECK(it->first == 'e');
    CHECK(it.isRightPossible() == true);
    CHECK(it.goRight()->first == 'g');
    CHECK(it.isLeftPossible() == false);
}

TEST_CASE("Removing node with right children without rotations")
{
    Dictionary<char,int> test;
    createDictionary(test,"dbfag");
    /*
                d
            b       e
          a           g
    */
    
    auto it = --test.end();
    it.goParent();
    CHECK(test.deleteNode(it));
    /*
                d
            b       g
          a           
    */

   CHECK(test.getSize() == 4);
   CHECK(test.isAVL());

   it = test.top();
   it.goRight();
   CHECK(it->first == 'g');
   CHECK(it.isRightPossible() == false);
   CHECK(it.isLeftPossible() == false);
}

TEST_CASE("Removing node with left children")
{

    Dictionary<char,int> test;
    createDictionary(test,"dbga");
    /*
                d
            b       g
          a           
    */

    //remove element with children on left
    CHECK(test.deleteNode('b'));
    /*
                d
            a       g         
    */

   CHECK(test.getSize() == 3);
   CHECK(test.isAVL());

   auto it = test.top();
   it.goLeft();
   CHECK(it->first == 'a');
   CHECK(it.isRightPossible() == false);
   CHECK(it.isLeftPossible() == false);
}

TEST_CASE("Copy and clear function of dictionary")
{
    Dictionary<char,int> test;
    createDictionary(test);

    Dictionary<char,int> test2;
    test2.copy(test);
    CHECK(test2.getSize() == test.getSize());

     /*test2 should look like that:
                d
            b       f
          a   c    e  g
    */
   
    auto it = test2.top();
    CHECK(it->first == 'd');
    CHECK(it.getRight()->first == 'f');
    CHECK(it.getLeft()->first == 'b');
    it.goLeft();
    CHECK(it.getRight()->first == 'c');
    CHECK(it.getLeft()->first == 'a');
    it.goParent().goRight();
    CHECK(it.getRight()->first == 'g');
    CHECK(it.getLeft()->first == 'e');

    //should be different iterators, belonging to different dictionaries
    CHECK(test.top() != test2.top());

    //check if deep copy was done
    test.addNode('j',1);
    CHECK(test.getSize() != test2.getSize());
    CHECK(test2.find('j').isEnd());

    //copying empty dictionary
    Dictionary<char,int> test3;
    test2.copy(test3);
    CHECK(test2.getSize() == 0);

    //clear
    test.clear();
    CHECK(test.getSize() == 0);
    CHECK(test.top().isEnd());
    CHECK(test.begin().isEnd());
    

    //clearing empty dictionary
    test.clear();
    CHECK(test.getSize() == 0);
}

TEST_CASE("Copy contructor and operator= of dictionary")
{
    Dictionary<char,int> test;
    createDictionary(test);

    Dictionary<char,int> test2(test);
    CHECK(test2.getSize() == test.getSize());
    
    Dictionary<char,int> test3;
    //empty dictionary to copy
    Dictionary<char,int> test4(test3);

    CHECK(test4.getSize() == 0);

    test3 = test2;
    CHECK(test3.getSize() == test2.getSize());
    CHECK(test3.top()->first == 'd');

    //empty dictionary
    test3 = test4;
    CHECK(test3.getSize() == 0);
}

TEST_CASE("Adding and removing with rotations")
{
    Dictionary<char,int> test;
    //adding such a long string will cause a lot of different rotations
    std::string str = "qw1ert3yui6opas5dfg4h7jkl8zx2cv9bnm";

    //adding elements
    //after every add it checks if tree is proper AVL tree
    for(std::size_t x = 0; x < str.size(); ++x)
    {
        test.addNode(str[x],1);
        CHECK(test.isAVL());
    }
    
    CHECK(test.getSize() == (int)str.size());

    //checks if every element is inside
    for(std::size_t x = 0; x < str.size(); ++x)
    {
        CHECK(test.find(str[x]).isEnd() == false);
        CHECK(test.find(str[x])->first == str[x]);
    }

    //the same characters but with totally different order
    str = "plok9m1ijn3uhby8gvt2fcr5dx4e7szw6aq";

    for(std::size_t x = 0; x < str.size(); ++x)
    {
        test.deleteNode(str[x]);
        CHECK(test.isAVL() == true);
    }
    CHECK(test.getSize() == 0);
}