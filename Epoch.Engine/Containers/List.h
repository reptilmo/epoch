#pragma once

#include "../Types.h"
#include "../Defines.h"

#include "../Memory/Memory.h"

namespace Epoch {

    /**
     * A simple list which holds many types of data in dynamic memory. Automatically resizes as needed.
     */
    template<class T>
    class EPOCH_EXPORT List {
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
         * Creates a new list with the specified capacity and adds the given data.
         *
         * @param capacity The capacity to initialize this list with.
         * @param data The data to be set in this object.
         */
        List( const U32 capacity, const T* data );

        /**
         * Creates a new list from the provided list.
         */
        List( const List<T>& other );

        /**
         * Default destructor.
         */
        ~List();

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
         * Removes the item at the given index. Does not shrink capacity.
         *
         * @param index The index to remove from.
         */
        void RemoveAt( const U32 index );

        /**
         * Resizes this list to the given size (number of elements).
         *
         * @param size The size to resize to.
         */
        void Resize( const U32 size );

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
        FORCEINLINE T* Data() { return _items; }

        /**
         * Obtains a constant pointer to the internal data contained within.
         * Used for accessing the data directly in read-only mode.
         */
        FORCEINLINE const T* Data() const { return _items; }

        /**
         * Obtains a reference to the data at the provided index.
         *
         * @param index The index to retrieve the data from.
         *
         * @returns A reference to the data at the provided index.
         */
        FORCEINLINE T& Get( const U32 index ) {
            ASSERT( index >= 0 && index < _size );
            return _items[index];
        }

        /**
         * Obtains a reference to the data at the provided index.
         *
         * @param index The index to retrieve the data from.
         *
         * @returns A reference to the data at the provided index.
         */
        FORCEINLINE const T& Get( const U32 index ) const {
            ASSERT( index >= 0 && index < _size );
            return _items[index];
        }

        /**
         * Obtains a copy to the data at the provided index.
         *
         * @param index The index to retrieve the data from.
         *
         * @returns A copy of the data at the provided index.
         */
        FORCEINLINE T& operator[]( const U32 index ) { return Get( index ); }

        /**
         * Obtains a const copy to the data at the provided index.
         *
         * @param index The index to retrieve the data from.
         *
         * @returns A const copy of the data at the provided index.
         */
        FORCEINLINE const T& operator[]( const U32 index ) const { return Get( index ); }

        /**
         * Returns the number of elements currently in this list.
         */
        FORCEINLINE const U32 Size() const { return _size; }

        /**
         * Returns the current capacity of this list.
         */
        FORCEINLINE const U32 Capacity() const { return _capacity; }

    private:
        void ensureAllocated( U32 count, const bool keepData );

    private:
        T* _items = nullptr;
        U32 _size = 0;
        U32 _capacity = 0;
    };

    template<class T>
    FORCEINLINE List<T>::List() {
        _capacity = 0;
        _size = 0;
    }

    template<class T>
    FORCEINLINE List<T>::List( const U32 capacity ) {
        ensureAllocated( capacity, false );
        _size = capacity;
    }

    template<class T>
    FORCEINLINE List<T>::List( const U32 capacity, T* data ) {
        ensureAllocated( capacity, false );
        TMemory::Memcpy( _items, data, sizeof( T ) * capacity );
        _size = capacity;
    }

    template<class T>
    FORCEINLINE List<T>::List( const U32 capacity, const T* data ) {
        ensureAllocated( capacity, false );
        TMemory::Memcpy( _items, data, sizeof( T ) * capacity );
        _size = capacity;
    }

    template<class T>
    FORCEINLINE List<T>::List( const List<T>& other ) {
        ensureAllocated( other._capacity, false );
        TMemory::Memcpy( _items, other._items, (U64)other._size * sizeof( T ) );
        _size = other._size;
    }

    template<class T>
    FORCEINLINE List<T>::~List() {
        if( _items ) {
            TMemory::Free( _items );
            _items = nullptr;
        }
        _capacity = 0;
        _size = 0;
    }

    template<class T>
    FORCEINLINE void List<T>::Add( T item ) {
        ensureAllocated( _size + (U32)1, true );
        _items[_size] = item;
        ++_size;
    }

    template<class T>
    FORCEINLINE void List<T>::InsertAt( T item, const U32 index ) {
        ensureAllocated( _size + (U32)1, true );

        // Push out entries after the index.
        for( U32 i = _size - 1; i >= index; --i ) {
            _items[i + 1] = _items[i];
        }
        _items[index] = item;
        ++_size;
    }

    template<class T>
    FORCEINLINE I32 List<T>::Remove( T item ) {
        I32 index = IndexOf( item );
        if( index != -1 ) {
            RemoveAt( index );
        }
        return index;
    }

    template<class T>
    FORCEINLINE void List<T>::RemoveAt( const U32 index ) {

        // Boot out early if the index is out of range.
        if( index >= _size ) {
            return;
        }

        // Pull in entries after the index.
        for( U32 i = index; i < _size; ++i ) {
            _items[i] = _items[i + 1];
        }

        --_size;
    }

    template<class T>
    FORCEINLINE void List<T>::Resize( const U32 size ) {
        ensureAllocated( size, true );
        _size = size;
    }

    template<class T>
    FORCEINLINE void List<T>::Clear( const bool shrink ) {
        _size = 0;
        if( shrink ) {
            Shrink();
        }
    }

    template<class T>
    FORCEINLINE const I32 List<T>::IndexOf( T item ) const {
        for( U32 i = 0; i < _size; ++i ) {
            if( _items[i] == item ) {
                return i;
            }
        }

        return -1;
    }

    template<class T>
    FORCEINLINE const I32 List<T>::LastIndexOf( T item ) const {
        for( U32 i = _size - 1; i >= 0; --i ) {
            if( _items[i] == item ) {
                return i;
            }
        }

        return -1;
    }

    template<class T>
    FORCEINLINE void List<T>::Shrink() {
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
    FORCEINLINE void List<T>::ensureAllocated( U32 count, const bool keepData ) {

        if( count <= _capacity ) {
            return;
        }

        // Set up a new array
        T* temp = static_cast<T*>( TMemory::Allocate( sizeof( T ) * count ) );
        TMemory::MemZero( temp, sizeof( T ) * count );

        if( _items ) {
            if( keepData ) {
                TMemory::Memcpy( temp, _items, sizeof( T ) * _size );
            }
            TMemory::Free( _items );
        }

        _items = temp;
        _capacity = count;
    }
}
