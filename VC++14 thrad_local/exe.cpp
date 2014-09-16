#include <windows.h>
#include <stdio.h>
#include <assert.h>

constexpr WCHAR Dll[] = L"dll";

DWORD WINAPI start_address( void * )
{
	printf( "[% 8lx] " __FUNCSIG__ "\n", GetCurrentThreadId() );

	HMODULE dll;
	while( ( dll = GetModuleHandleW( Dll ) ) == nullptr )
		Sleep( 1 );

	auto p = ( void(*)() )GetProcAddress( dll, "f" );
	assert( p );
	p();

	Sleep( 1 );
	return 0;
}

int __cdecl main()
{
	printf( "[% 8x] " __FUNCSIG__ "\n", GetCurrentThreadId() );

	WaitForSingleObject( CreateThread( nullptr, 0, []( void* )->DWORD {
		constexpr int count = 2;
		HANDLE thread[count * 2];
		DWORD thread_id[ARRAYSIZE( thread )];

		for( int i = 0; i < count; ++i )
		{
			int j = i * 2;
			thread[j] = CreateThread( nullptr, 0, start_address, nullptr, 0, &thread_id[j] );
			printf( "[% 8lx] CreateThread [% 8lx]\n", GetCurrentThreadId(), thread_id[j] );
		}

		auto dll = LoadLibraryW( Dll );
		assert( dll );

		for( int i = 0; i < count; ++i )
		{
			int j = i * 2 + 1;
			thread[j] = CreateThread( nullptr, 0, start_address, nullptr, 0, &thread_id[j] );
			printf( "[% 8lx] CreateThread [% 8lx]\n", GetCurrentThreadId(), thread_id[j] );
		}

		auto p = ( void(*)() )GetProcAddress( dll, "f" );
		assert( p );
		p();

		WaitForMultipleObjects( ARRAYSIZE( thread ), thread, TRUE, INFINITE );
		return 0;
	}, nullptr, 0, nullptr ), INFINITE );
}