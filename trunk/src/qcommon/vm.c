/*
===========================================================================

Return to Castle Wolfenstein multiplayer GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Return to Castle Wolfenstein multiplayer GPL Source Code (RTCW MP Source Code).  

RTCW MP Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RTCW MP Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RTCW MP Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the RTCW MP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the RTCW MP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

// vm.c -- shared library

/*


intermix code and data
symbol table

a dll has one imported function: VM_SystemCall
and one exported function: Perform


*/

#include "vm_local.h"

// fretn - osx
#include <stdarg.h>

vm_t    *currentVM = NULL; // bk001212
vm_t    *lastVM    = NULL; // bk001212
int vm_debugLevel;

#define MAX_VM      3
vm_t vmTable[MAX_VM];

void VM_Debug( int level ) {
	vm_debugLevel = level;
}

/*
==============
VM_Init
==============
*/
void VM_Init( void ) {
	Com_Memset( vmTable, 0, sizeof( vmTable ) );
}

/*
============
VM_DllSyscall

Dlls will call this directly

 rcg010206 The horror; the horror.

  The syscall mechanism relies on stack manipulation to get it's args.
   This is likely due to C's inability to pass "..." parameters to
   a function in one clean chunk. On PowerPC Linux, these parameters
   are not necessarily passed on the stack, so while (&arg[0] == arg)
   is true, (&arg[1] == 2nd function parameter) is not necessarily
   accurate, as arg's value might have been stored to the stack or
   other piece of scratch memory to give it a valid address, but the
   next parameter might still be sitting in a register.

  Quake's syscall system also assumes that the stack grows downward,
   and that any needed types can be squeezed, safely, into a signed int.

  This hack below copies all needed values for an argument to a
   array in memory, so that Quake can get the correct values. This can
   also be used on systems where the stack grows upwards, as the
   presumably standard and safe stdargs.h macros are used.

  As for having enough space in a signed int for your datatypes, well,
   it might be better to wait for DOOM 3 before you start porting.  :)

  The original code, while probably still inherently dangerous, seems
   to work well enough for the platforms it already works on. Rather
   than add the performance hit for those platforms, the original code
   is still in use there.

  For speed, we just grab 15 arguments, and don't worry about exactly
   how many the syscall actually needs; the extra is thrown away.

============
*/
int QDECL VM_DllSyscall( int arg, ... ) {
#if ( ( defined __linux__ ) && ( defined __powerpc__ ) ) //|| (defined MACOS_X)
	// rcg010206 - see commentary above
	int args[16];
	int i;
	va_list ap;

	args[0] = arg;

	va_start( ap, arg );
	for ( i = 1; i < sizeof( args ) / sizeof( args[i] ); i++ )
		args[i] = va_arg( ap, int );
	va_end( ap );

	return currentVM->systemCall( args );
#else // original id code
	return currentVM->systemCall( &arg );
#endif
}

/*
=================
VM_Restart

Reload the data, but leave everything else in place
This allows a server to do a map_restart without changing memory allocation
=================
*/
vm_t *VM_Restart( vm_t *vm ) {

	// DLL's can't be restarted in place
	if ( vm->dllHandle ) {
		char name[MAX_QPATH];
		int ( *systemCall )( int *parms );

		systemCall = vm->systemCall;
		Q_strncpyz( name, vm->name, sizeof( name ) );

		VM_Free( vm );

		vm = VM_Create( name, systemCall, VMI_NATIVE );
		return vm;
	}

	return NULL;
}

/*
================
VM_Create

If image ends in .qvm it will be interpreted, otherwise
it will attempt to load as a system dll
================
*/

#define STACK_SIZE  0x20000

vm_t *VM_Create( const char *module, int ( *systemCalls )(int *),
				 vmInterpret_t interpret ) {
	vm_t        *vm;
	int i;

	if ( !module || !module[0] || !systemCalls ) {
		Com_Error( ERR_FATAL, "VM_Create: bad parms" );
	}

	// see if we already have the VM
	for ( i = 0 ; i < MAX_VM ; i++ ) {
		if ( !Q_stricmp( vmTable[i].name, module ) ) {
			vm = &vmTable[i];
			return vm;
		}
	}

	// find a free vm
	for ( i = 0 ; i < MAX_VM ; i++ ) {
		if ( !vmTable[i].name[0] ) {
			break;
		}
	}

	if ( i == MAX_VM ) {
		Com_Error( ERR_FATAL, "VM_Create: no free vm_t" );
	}

	vm = &vmTable[i];

	Q_strncpyz( vm->name, module, sizeof( vm->name ) );
	vm->systemCall = systemCalls;

	if ( interpret == VMI_NATIVE ) {
		// try to load as a system dll
		//vm->dllHandle = Sys_LoadDll( module, vm->fqpath, &vm->entryPoint, VM_DllSyscall );
                vm->dllHandle = Sys_LoadDll( module, &vm->entryPoint, VM_DllSyscall );
		if ( vm->dllHandle ) {
			return vm;
		}

		Com_Printf( "Failed to load dll.\n" );
                return NULL;

	}
        else
        {
                return NULL;
        }

	return NULL;
}

/*
==============
VM_Free
==============
*/
void VM_Free( vm_t *vm ) {

	if ( vm->dllHandle ) {
		Sys_UnloadDll( vm->dllHandle );
		Com_Memset( vm, 0, sizeof( *vm ) );
	}
#if 0   // now automatically freed by hunk
	if ( vm->codeBase ) {
		Z_Free( vm->codeBase );
	}
	if ( vm->dataBase ) {
		Z_Free( vm->dataBase );
	}
	if ( vm->instructionPointers ) {
		Z_Free( vm->instructionPointers );
	}
#endif
	Com_Memset( vm, 0, sizeof( *vm ) );

	currentVM = NULL;
	lastVM = NULL;
}

void VM_Clear( void ) {
	int i;
	for ( i = 0; i < MAX_VM; i++ ) {
		if ( vmTable[i].dllHandle ) {
			Sys_UnloadDll( vmTable[i].dllHandle );
		}
		Com_Memset( &vmTable[i], 0, sizeof( vm_t ) );
	}
	currentVM = NULL;
	lastVM = NULL;
}

void *VM_ArgPtr( int intValue ) {
	if ( !intValue ) {
		return NULL;
	}
	// bk001220 - currentVM is missing on reconnect
	if ( currentVM == NULL ) {
		return NULL;
	}

	if ( currentVM->entryPoint ) {
		return ( void * )( currentVM->dataBase + intValue );
	} else {
		return ( void * )( currentVM->dataBase + ( intValue & currentVM->dataMask ) );
	}
}

void *VM_ExplicitArgPtr( vm_t *vm, int intValue ) {
	if ( !intValue ) {
		return NULL;
	}

	// bk010124 - currentVM is missing on reconnect here as well?
	if ( currentVM == NULL ) {
		return NULL;
	}

	//
	if ( vm->entryPoint ) {
		return ( void * )( vm->dataBase + intValue );
	} else {
		return ( void * )( vm->dataBase + ( intValue & vm->dataMask ) );
	}
}


/*
==============
VM_Call


Upon a system call, the stack will look like:

sp+32	parm1
sp+28	parm0
sp+24	return value
sp+20	return address
sp+16	local1
sp+14	local0
sp+12	arg1
sp+8	arg0
sp+4	return stack
sp		return address

An interpreted function will immediately execute
an OP_ENTER instruction, which will subtract space for
locals from sp
==============
*/
#define MAX_STACK   256
#define STACK_MASK  ( MAX_STACK - 1 )

int QDECL VM_Call( vm_t *vm, int callnum, ... ) {
	vm_t    *oldVM;
        int     r;
	//rcg010207 see dissertation at top of VM_DllSyscall() in this file.
#if ( ( defined __linux__ ) && ( defined __powerpc__ ) ) || ( defined MACOS_X )
	int i;
	int args[16];
	va_list ap;
#endif

	// DHM - Nerve
#ifdef UPDATE_SERVER
	return 0;
#endif

	if ( !vm ) {
		Com_Error( ERR_FATAL, "VM_Call with NULL vm" );
	}

	oldVM = currentVM;
	currentVM = vm;
	lastVM = vm;

	if ( vm_debugLevel ) {
		Com_Printf( "VM_Call( %i )\n", callnum );
	}

	// if we have a dll loaded, call it directly
	if ( vm->entryPoint ) {
		//rcg010207 -  see dissertation at top of VM_DllSyscall() in this file.
#if ( ( defined __linux__ ) && ( defined __powerpc__ ) ) || ( defined MACOS_X )
		va_start( ap, callnum );
		for ( i = 0; i < sizeof( args ) / sizeof( args[i] ); i++ )
			args[i] = va_arg( ap, int );
		va_end( ap );

		r = vm->entryPoint( callnum,  args[0],  args[1],  args[2], args[3],
							args[4],  args[5],  args[6], args[7],
							args[8],  args[9], args[10], args[11],
							args[12], args[13], args[14], args[15] );
#else // PPC above, original id code below
		r = vm->entryPoint( ( &callnum )[0], ( &callnum )[1], ( &callnum )[2], ( &callnum )[3],
							( &callnum )[4], ( &callnum )[5], ( &callnum )[6], ( &callnum )[7],
							( &callnum )[8],  ( &callnum )[9],  ( &callnum )[10],  ( &callnum )[11],  ( &callnum )[12] );
#endif
	}

	if ( oldVM != NULL ) { // bk001220 - assert(currentVM!=NULL) for oldVM==NULL
		currentVM = oldVM;
	}
	return r;
}

//=================================================================
