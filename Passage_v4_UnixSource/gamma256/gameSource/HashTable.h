template <class Type> 
class HashTable {
    
    public:
        
        HashTable( int inSize );
        
        ~HashTable();
        
        Type lookup( int inKeyA, int inKeyB, char *outFound );
        
        void insert( int inKeyA, int inKeyB, Type inItem );
        
        // flush all entries from table
        void clear();
        
        
    private:
        int mSize;
        
        Type *mTable;
        char *mSetFlags;

        int *mKeysA;
        int *mKeysB;
        
        int mHitCount;
        int mMissCount;

        int computeHash( int inKeyA, int inKeyB );
        
    };



// Wow... never new that template function implementations must be in the
// same file as the declaration


template <class Type> 
HashTable<Type>::HashTable( int inSize )
        : mSize( inSize ),
          mTable( new Type[ inSize ] ),
          mSetFlags( new char[ inSize ] ),
          mKeysA( new int[ inSize ] ),
          mKeysB( new int[ inSize ] ),
          mHitCount( 0 ),
          mMissCount( 0 ) {
    

    clear();
        
    }



#include <stdio.h>

        
template <class Type> 
HashTable<Type>::~HashTable() {
    delete [] mTable;
    delete [] mSetFlags;
    delete [] mKeysA;
    delete [] mKeysB;

    printf( "%d hits, %d misses, %f hit ratio\n", 
            mHitCount, mMissCount, 
            mHitCount / (double)( mHitCount + mMissCount ) );
    }



template <class Type> 
inline int HashTable<Type>::computeHash( int inKeyA, int inKeyB ) {
    
    int hashKey = ( inKeyA * 734727 + inKeyB * 263474 ) % mSize;
    if( hashKey < 0 ) {
        hashKey += mSize;
        }
    return hashKey;
    }


        
template <class Type> 
Type HashTable<Type>::lookup( int inKeyA, int inKeyB, char *outFound ) {
    
    int hashKey = computeHash( inKeyA, inKeyB );
    
    if( mSetFlags[ hashKey ]
        &&
        mKeysA[ hashKey ] == inKeyA
        &&
        mKeysB[ hashKey ] == inKeyB ) {
    
        // hit
        mHitCount ++;
        
        *outFound = true;
        }
    else {
        // miss
        mMissCount ++;
        
        *outFound = false;
        }
    
    return mTable[ hashKey ];
    }



template <class Type> 
void HashTable<Type>::insert( int inKeyA, int inKeyB, Type inItem ) {
    
    int hashKey = computeHash( inKeyA, inKeyB );

    mSetFlags[ hashKey ] = true;
    mKeysA[ hashKey ] = inKeyA;
    mKeysB[ hashKey ] = inKeyB;
    mTable[ hashKey ] = inItem;
    }



template <class Type> 
void HashTable<Type>::clear() {
    
    for( int i=0; i<mSize; i++ ) {
        mSetFlags[i] = false;
        }
    }
