/* URetro - a thing for the Wii U */
/* https://github.com/QuarkTheAwesome/URetro */

#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "dynamic_libs/fs_functions.h"

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

void exception_disassembly_helper(char *fmt, int addr,int opcode, char* s)
{
		char* *store = (char**)0x1ab5d140;
		char *buffer = (char *)store[0];
		store[0] += __os_snprintf(buffer,512,fmt, addr,opcode,s);
}
unsigned char exception_handler(void* contextIn) {

		//Temporary hacky fix, please ignore me.
		unsigned int coreinit_handle;
		OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
		void (*DisassemblePPCRange)(void *start, void *end, void *printf_func, int *find_symbol_func, int flags);
		OSDynLoad_FindExport(coreinit_handle, 0, "DisassemblePPCRange", &DisassemblePPCRange);  

		int* context = (int*)contextIn;

		char buf2[512];
		int* store = (int*)0x1AB5D140;
		store[0] = (int)buf2;

		DisassemblePPCRange((void*)context[38]-0x5, (void*)context[38]+0x30, (void*)exception_disassembly_helper, 0, 0);

		int ancientSP = 0xDEADCAFE;
		int* crashSP = (int*)context[3];

		ancientSP = *(crashSP - 0x16); //where we're storing the old SP

		char buf[1024];
		__os_snprintf(buf, 1024, "r0:%08X  SP:%08X  r2:%08X  r3:%08X r9:%08X\n r10:%08X r11:%08X r30:%08X r31:%08X\nlr:%08X slr:%08X  PC:%08X\n%s", context[2], context[3], context[4], context[5], context[11], context[12], context[13], context[32], context[33], context[35], ancientSP, context[38], buf2);

		OSFatal(buf);

		return 0;
}
void InstallExceptionHandler() {
		OSSetExceptionCallback(2, &exception_handler);
		OSSetExceptionCallback(3, &exception_handler);
		OSSetExceptionCallback(6, &exception_handler);
}

#endif //__EXCEPTION_H__
