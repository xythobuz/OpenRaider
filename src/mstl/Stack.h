/*===============================================================
 * Project: Freyja
 * Author : Terry 'Mongoose' Hendrix II
 * Website: http://www.westga.edu/~stu7440/
 * Email  : stu7440@westga.edu
 * Object : Stack
 * Comment: 
 *
 *-- History -----------------------------------------------
 *
 * 2001-05-12:
 * Mongoose - Created
 ==============================================================*/

#ifndef __MONGOOSE_FREYJA_STACK_H
#define __MONGOOSE_FREYJA_STACK_H

#include <stdio.h>
#include <stdlib.h>

 
template <class T> class StackNode
{
 public:

  StackNode(T data)
  {
    _data  = data;
    _prev = NULL;
  }


  ~StackNode()
  {
  }

  void Data(T data)
  {
    _data = data;
  }

  T Data()
  {
    return _data;
  }

  StackNode<T> *Prev()
  {
    return _prev;
  }

  void Prev(StackNode<T> *prev)
  {
    _prev = prev;
  }

 private:

  StackNode<T> *_prev;

  T _data;
};



template <class T> class Stack
{
public:

   Stack()
   {
     _num_items = 0;
     _top = NULL;
   }
   /*-----------------------------------------
    * Created  : 990831 by Terry Hendrix II
    * Modified : NA
    * Inputs   : None
    * Returns  : Nothing
    * Pre      : None
    * Post     : Constructs an Stack
    -----------------------------------------*/

   ~Stack()
   {
     while (_top) //(!Empty())
       pop();
   }
   /*-----------------------------------------
    * Created  : 990831 by Terry Hendrix II
    * Modified : NA
    * Inputs   : None
    * Returns  : Nothing
    * Pre      : None
    * Post     : Deconstructs an Stack
    -----------------------------------------*/

   T peek()
   {
     if (_top)
     {
       return _top->Data();
     }
     else
     {
       fprintf(stderr, "Stack<T>::Peek> NULL _top!\n");     
       return (T)0;
     }
   }
   /*-----------------------------------------
    * Created  : 990831 by Terry Hendrix II
    * Modified : NA
    * Inputs   : None
    * Returns  : value of top stack element
    * Pre      : stack isn't empty
    * Post     : Returns top of stack's value
    -----------------------------------------*/

   T pop()
   {
     StackNode<T> *rm;
     T data;


     if (_top)
     {
       rm = _top;

       data = _top->Data();
       _top = _top->Prev();

       delete rm;
       _num_items--;

       return data;
     }
     else
     {
       fprintf(stderr, "Stack<T>::Pop> NULL _top!\n");
       return (T)0;
     }
   }
   /*-----------------------------------------
    * Created  : 990831 by Terry Hendrix II
    * Modified : NA
    * Inputs   : None
    * Returns  : value of top stack element
    * Pre      : stack isn't empty
    * Post     : Removes top of stack and 
    *            returns it's value
    -----------------------------------------*/

   void push(T data)
   {
     StackNode<T> *node;


     node = new StackNode<T>(data);
     node->Prev(_top);

     _top = node;
     _num_items++;
   }
   /*-----------------------------------------
    * Created  : 990831 by Terry Hendrix II
    * Modified : NA
    * Inputs   : Value to push on top of stack
    * Returns  : Nothing
    * Pre      : Stack isn't full
    * Post     : Stack has new top, with value i
    -----------------------------------------*/

   bool empty()
   {
     return (_num_items && _top);
   }
   /*-----------------------------------------
    * Created  : 990831 by Terry Hendrix II
    * Modified : NA
    * Inputs   : None
    * Returns  : true if stack is empty, else
    *            returns false
    * Pre      : None
    * Post     : No side effect
    -----------------------------------------*/

 private:

  unsigned int _num_items;

  StackNode<T> *_top;
};

#endif




