#pragma once

#include "../Types.h"
#include "../Defines.h"

namespace Epoch {

    /**
     * Represents a single node to be used within a LinkedList. Containes a pointer
     * to the next node in the list. When deleted, this node
     * Automatically deletes Next if it exists.
     */
    template<class T>
    struct LinkedListNode {

        /**
         * The value contained in this node.
         */
        T Value;

        /**
         * A pointer to the next node in the owning list.
         */
        LinkedListNode* Next = nullptr;

        /**
         * Destroys this node. Also destroys the the Next node if it exists.
         */
        ~LinkedListNode() {
            if( Next != nullptr ) {
                delete Next;
                Next = nullptr;
            }
        }
    };

    /**
     * A linked list container, where each node contains a pointer to the next.
     * Random access is not allowed; therefore elements must be accessed sequentially.
     */
    template<class T>
    class LinkedList {
    public:

        /**
         * Creates a new LinkedList.
         */
        LinkedList();

        /**
         * Destroys this LinkedList.
         */
        ~LinkedList();

        /**
         * Retrieves the head node of this list.
         *
         * @returns A pointer to the head node of this list.
         */
        LinkedListNode<T>* Peek();

        /**
         * Pushes a new node to the front of this list with the provided value.
         *
         * @param value The value to be pushed to this list.
         *
         * @returns A pointer to the newly-created list node.
         */
        LinkedListNode<T>* Push( T value );

        /**
         * Appends a new node to the back of this list with the provided value.
         *
         * @param value The value to be appended to this list.
         *
         * @returns A pointer to the newly-created list node.
         */
        LinkedListNode<T>* Append( T value );

        /**
         * Inserts a new node at the specified index of this list with the provided value.
         * Pushes the node currently at this index out.
         * Note that an assertion will occur if the end of the list is reached without having inserted the new node.
         *
         * @param value The value to be inserted to this list.
         * @param index The index of the node to insert after.
         *
         * @returns A pointer to the newly-created list node.
         */
        LinkedListNode<T>* InsertAt( T value, U64 index );

        /**
         * Removes the first node containing the provided value from this list.
         *
         * @param value The value to be removed from this list.
         *
         * @returns True if an element was removed; otherwise false.
         */
        const bool RemoveByValue( T value );

        /**
         * Removes node at the given index from this list.
         * Note that an assertion will occur if the index exceeds the size of this list. Only use this
         * if you know what you are doing.
         *
         * @param value The index of the node to be removed from this list.
         */
        void RemoveAt( U64 index );

    private:
        LinkedListNode<T>* _head = nullptr;
    };

    template<class T>
    LinkedList<T>::LinkedList() {

    }

    template<class T>
    LinkedList<T>::~LinkedList() {
        if( _head != nullptr ) {
            delete _head;
            _head = nullptr;
        }
    }

    template<class T>
    LinkedListNode<T>* LinkedList<T>::Peek() {
        return _head;
    }

    template<class T>
    LinkedListNode<T>* LinkedList<T>::Push( T value ) {
        LinkedListNode<T>* node = new LinkedListNode<T>();
        node->Value = value;
        if( _head == nullptr ) {
            _head = node;
        } else {
            node->Next = _head;
            _head = node;
        }
        return node;
    }

    template<class T>
    LinkedListNode<T>* LinkedList<T>::Append( T value ) {
        LinkedListNode<T>* node = new LinkedListNode<T>();
        node->Value = value;
        if( _head == nullptr ) {
            _head = node;
        } else {
            LinkedListNode<T>* p;
            p = _head;
            while( p->Next != nullptr ) {
                p = p->Next;
            }
            p->Next = node;
        }
        return node;
    }

    template<class T>
    LinkedListNode<T>* LinkedList<T>::InsertAt( T value, U64 index ) {
        LinkedListNode<T>* p = _head;
        LinkedListNode<T>* prev = nullptr;
        for( U64 i = 0; i <= index; ++i ) {
            if( i == index ) {
                LinkedListNode<T>* node = new LinkedListNode<T>();
                node->Value = value;

                // Insert head with no entries.
                if( p == nullptr ) {
                    _head = node;
                } else {

                    // Has entries, but inserting at head
                    if( i == 0 ) {
                        _head = node;
                        node->Next = p;
                    } else {
                        // Has entries, not inserting at head
                        LinkedListNode<T>* next = prev->Next;
                        prev->Next = node;
                        node->Next = next;
                    }
                }
                return node;
            }

            prev = p;
            p = p->Next;
        }

        ASSERT_MSG( false, "LinkedListNode::InsertAt Attempted to insert an an index which is outside the bounds of this list." );
        return nullptr;
    }

    template<class T>
    const bool LinkedList<T>::RemoveByValue( T value ) {
        LinkedListNode<T>* p, * prev;
        p = _head;
        prev = nullptr;
        while( p->Next != nullptr ) {
            if( p->Value == value ) {

                if( prev == nullptr ) {
                    // Value contained in head. Delete head and reassign.
                    _head = p->Next;
                    p->Next = nullptr;
                    delete p;
                    return true;
                } else {
                    prev->Next = p->Next;
                    p->Next = nullptr;
                    delete p;
                    return true;
                }
            }
            prev = p;
            p = p->Next;
        }
        return false;
    }

    template<class T>
    void LinkedList<T>::RemoveAt( U64 index ) {
        LinkedListNode<T>* p = _head;
        LinkedListNode<T>* prev = nullptr;
        for( U64 i = 0; i <= index; ++i ) {
            if( i == index ) {
                LinkedListNode<T>* temp = p->Next;
                p->Next = nullptr;
                delete p;
                if( prev ) {
                    prev->Next = temp;
                }
                return;
            }
            prev = p;
            p = p->Next;
        }

        ASSERT_MSG( false, "LinkedListNode::RemoveAt Attempted to remove an an index which is outside the bounds of this list." );
    }
}