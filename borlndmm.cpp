//---------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#pragma hdrstop
#pragma argsused
//---------------------------------------------------------------------------
#if !defined(_DEBUG) && defined(POISON_HEAP_USER_DATA)
#error "Only poison heap user data in debug build"
#endif

#if defined(POISON_HEAP_USER_DATA)
// As we dont link in the default runtime library, we must provide our own memset.
void * __memset( void * ptr, unsigned char value, size_t num )
{
	unsigned char * p = (unsigned char *)ptr;

	while( num-- )
		*p++ = value;

	return ptr;
}
#endif

#if defined(_WIN64)
typedef __int64 NativeInt;
#else
typedef int NativeInt;
#endif

// The native Windows heap all allocations will be serviced from. We dont use
// the default process heap to seperate borlndmm allocations from all other
// native heap allocations.
HANDLE heap = NULL;
//---------------------------------------------------------------------------
namespace Borlndmm
{
	void * __fastcall __declspec(dllexport) SysGetMem( NativeInt sz )
	{
		void * p = NULL;

		do
		{
			if( sz == 0 )
				break;

			if( !heap )
			{
				heap = ::HeapCreate( 0, 0, 0 );
				if( !heap )
					break;
			}

			p = ::HeapAlloc( heap, 0, sz );

			#if defined(POISON_HEAP_USER_DATA)
			if( !p )
				break;

			__memset( p, 0xA1, sz );
			#endif

		} while( 0 );

		return p;
	};

	void * __fastcall __declspec(dllexport) SysAllocMem( NativeInt sz )
	{
		void * p = NULL;

		do
		{
			if( sz == 0 )
				break;

			if( !heap )
			{
				heap = ::HeapCreate( 0, 0, 0 );
				if( !heap )
					break;
			}

			p = ::HeapAlloc( heap, HEAP_ZERO_MEMORY, sz );

		} while( 0 );

		return p;
	};

	int __fastcall __declspec(dllexport) SysFreeMem( void * p )
	{
		int status = -1;

		do
		{
			if( !p || !heap )
				break;

			#if defined(POISON_HEAP_USER_DATA)
			SIZE_T sz = ::HeapSize( heap, 0, p );

			if( sz != (SIZE_T)-1 )
				__memset( p, 0xF1, sz );
			#endif

			if( !::HeapFree( heap, 0, p ) )
				break;

			status = 0;

		} while( 0 );

		return status;
	};

	void * __fastcall __declspec(dllexport) SysReallocMem( void * p, NativeInt sz )
	{
		if( !p || !heap || sz == 0 )
			return NULL;

		return ::HeapReAlloc( heap, 0, p, sz );
	};

	bool __fastcall __declspec(dllexport) SysUnregisterExpectedMemoryLeak( int * p )
	{
		return true;
	};

	bool __fastcall __declspec(dllexport) SysRegisterExpectedMemoryLeak( int * p )
	{
		return true;
	};

	void __fastcall __declspec(dllexport) HeapRelease( void )
	{

	};

	void __fastcall __declspec(dllexport) HeapAddRef( void )
	{

	};
}
//---------------------------------------------------------------------------
extern "C"
{
	int __stdcall __declspec(dllexport) GetAllocMemSize( void * p )
	{
		if( !p || !heap )
			return 0;

		return ::HeapSize( heap, 0, p );
	}

	int __stdcall __declspec(dllexport) GetAllocMemCount( void )
	{
		return 0;
	}

	int __stdcall __declspec(dllexport) GetHeapStatus( void )
	{
		return 0;
	}

	int __stdcall __declspec(dllexport) DumpBlocks( void )
	{
		return 0;
	}

	void * __stdcall __declspec(dllexport) ReallocMemory( void * p, NativeInt sz )
	{
		return Borlndmm::SysReallocMem( p, sz );
	}

	int __stdcall __declspec(dllexport) FreeMemory( void * p )
	{
		return Borlndmm::SysFreeMem( p );
	}

	void * __stdcall __declspec(dllexport) GetMemory( NativeInt sz )
	{
		return Borlndmm::SysGetMem( sz );
	}
}
//---------------------------------------------------------------------------

