#pragma once

#include "../Types.h"
#include "../Memory/Memory.h"

namespace Epoch {

    /**
     * A simple list which holds many types of data in dynamic memory. Automatically resizes as needed.
     */
    template<class T>
    class List {
    public:

        /**
         * Creates a new list with the specified capacity.
         */
        List();

        /**
         * Creates a new list with the specified capacity.
         *
         * @param capacity The capacity to initialize this list with.
         */
        List( const U32 capacity );

        /**
         * Creates a new list with the specified capacity and adds the given data.
         *
         * @param capacity The capacity to initialize this list with.
         * @param data The data to be set in this object.
         */
        List( const U32 capacity, T* data );

        /**
         * Adds the given item to this list.
         *
         * @param item The item to be added.
         */
        void Add( T item );

        /**
         * Inserts the given item at the provided index.
         *
         * @param item The item to be inserted.
         * @param index The index to insert at.
         */
        void InsertAt( T item, const U32 index );

        /**
         * Removes the first occurrance of the given item.
         *
         * @param item The item to search for.
         *
         * @returns The index of the item if found and removed; otherwise -1 if not found.
         */
        I32 Remove( T item );

        /**
         * Removes the item the given index. Does not shrink capacity.
         *
         * @param index The index to remove from.
         *
         * @returns The removed item.
         */
        T RemoveAt( const U32 index );

        void Resize( const U32 capacity );

        /**
         * Clears the items from this list. Does not automatically shrink.
         *
         * @param shrink Indicates if a shrink operation should be performed on clear. Default: false.
         */
        void Clear( const bool shrink = false );

        /**
         * Shrinks the capacity of this list to the number of items currently contained.
         * Note that this does involve a copy operation, so it is not recommended except scenarios where
         * large amounts of memory should be freed.
         */
        void Shrink();

        /**
         * Returns the index of the given item.
         *
         * @param item The item to search for.
         *
         * @returns The index of the item if found; otherwise -1 if not found.
         */
        const I32 IndexOf( T item ) const;

        /**
         * Returns the last index of the given item.
         * 
         * @param item The item to search for.
         * 
         * @returns The index of the item if found; otherwise -1 if not found.
         */
        const I32 LastIndexOf( T item ) const;

        /**
         * Obtains a pointer to the internal data contained within.
         * Used for filling from a buffer, for example.
         */
        T* Data() { return _items; }

        /**
         * Obtains a reference to the data at the provided index.
         *
         * @param index The index to retrieve the data from.
         *
         * @returns A reference to the data at the provided index.
         */
        T& Get( const U32 index ) { return _items[index]; }

        /**
         * Obtains a reference to the data at the provided index.
         *
         * @param index The index to retrieve the data from.
         *
         * @returns A reference to the data at the provided index.
         */
        T& operator[]( const U32 index ) { return _items[index]; }

        /**
         * Returns the number of elements currently in this list.
         */
        const U32 Size() const { return _size; }

        /**
         * Returns the current capacity of this list.
         */
        const U32 Capacity() const { return _capacity; }

    private:
        void ensureAllocated( U32 count );

    private:
        T* _items;
        U32 _size;
        U32 _capacity;
    };

    template<class T>
    List<T>::List() {
        _capacity = 0;
        _size = 0;
    }

    template<class T>
    List<T>::List( const U32 capacity ) {
        _capacity = capacity;
        _size = capacity;
        ensureAllocated( capacity );
    }

    template<class T>
    List<T>::List( const U32 capacity, T* data ) {
        _capacity = capacity;
        _size = capacity;
        ensureAllocated( capacity );
        TMemory::Memcpy( _items, data, sizeof( T ) * capacity );
    }

    template<class T>
    void List<T>::Add( T item ) {
        ensureAllocated( _size + (U32)1 );
        _items[_size] = item;
        ++_size;
    }

    template<class T>
    void List<T>::InsertAt( T item, const U32 index ) {
        ensureAllocated( _size + (U32)1 );

        // Push out entries after the index.
        for( U32 i = _size - 1; i >= index; --i ) {
            _items[i + 1] = _items[i];
        }
        _items[index] = item;
        ++_size;
    }

    template<class T>
    I32 List<T>::Remove( T item ) {
        I32 index = IndexOf( item );
        if( index != -1 ) {
            RemoveAt( index );
        }
        return index;
    }

    template<class T>
    T List<T>::RemoveAt( const U32 index ) {
        T item = _items[index];

        // Pull in entries after the index.
        for( U32 i = index; i < _size; ++i ) {
            _items[i] = _items[i + 1];
        }

        --_size;
        return item;
    }

    template<class T>
    void List<T>::Resize( const U32 capacity ) {
        ensureAllocated( capacity );
        _size = capacity;
    }

    template<class T>
    void List<T>::Clear( const bool shrink ) {
        _size = 0;
        if( shrink ) {
            Shrink();
        }
    }

    template<class T>
    const I32 List<T>::IndexOf( T item ) const {
        for( U32 i = 0; i < _size; ++i ) {
            if( _items[i] == item ) {
                return i;
            }
        }

        return -1;
    }

    template<class T>
    const I32 List<T>::LastIndexOf( T item ) const {
        for( U32 i = _size - 1; i >= 0; --i ) {
            if( _items[i] == item ) {
                return i;
            }
        }

        return -1;
    }

    template<class T>
    void List<T>::Shrink() {
        if( _capacity > _size ) {
            T* temp = static_cast<T*>( TMemory::Allocate( sizeof( T ) * _size ) );

            if( _items ) {
                TMemory::Memcpy( temp, _items, sizeof( T ) * _size );
                TMemory::Free( _items );
            }

            _items = temp;
        }
    }

    template<class T>
    void List<T>::ensureAllocated( U32 count ) {

        if( count <= _capacity ) {
            return;
        }

        // Set up a new array
        T* temp = static_cast<T*>( TMemory::Allocate( sizeof( T ) * count ) );

        // If data is already present, copy it over.
        if( _items ) {
            TMemory::Memcpy( temp, _items, sizeof( T ) * _size );
            TMemory::Free( _items );
        }

        _items = temp;
        _capacity = count;
    }
}
