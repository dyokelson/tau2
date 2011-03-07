#include <stdio.h>
#include <TAU.h>
#include <stdlib.h>
#include <map>
using namespace std;


#ifndef TAU_WINDOWS
#define TAU_LLONG long
#else /* for Windows */
#define TAU_LLONG long long
#endif /* TAU_WINDOWS */

typedef int cuda_result;
typedef int cuda_device;
typedef unsigned TAU_LLONG cuda_deviceptr;
typedef unsigned int cuda_deviceptrx;
typedef void* cuda_array;
typedef void* cuda_context;
typedef void* cuda_module;
typedef void* cuda_function;
typedef void* cuda_stream;




//TAU_GLOBAL_TIMER(pgi_acc_region_timer, "pgi accelerator region", "", TAU_DEFAULT);

static map<cuda_function,string> functionMap;
static char* TauPgiFile; 
static char* TauPgiFunc; 
#define TAU_PGI_ACC_NAME_LEN 4096

extern "C" void __pgi_cu_init_p(char* file, char* func, long lineno);
extern "C" void __pgi_cu_init(char* file, char* func, long lineno) {
  //TAU_GLOBAL_TIMER_START(pgi_acc_region_timer);
  TauPgiFile = file;
  TauPgiFunc = func;
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_init %s [{%s}{%ld}]", TauPgiFunc, TauPgiFile, lineno);
  
  TAU_START(sourceinfo);
  __pgi_cu_init_p(file, func, lineno);
  TAU_STOP(sourceinfo);
}


// extern "C" void __pgi_cu_close_p(void);
// extern "C" void __pgi_cu_close(void) {
//   TAU_GLOBAL_TIMER_START(pgi_acc_region_timer);
//   char sourceinfo[TAU_PGI_ACC_NAME_LEN];
//   sprintf(sourceinfo, "__pgi_cu_close %s [{%s}]", TauPgiFunc, TauPgiFile);
  
//   TAU_START(sourceinfo);
//   __pgi_cu_close_p();
//   TAU_STOP(sourceinfo);
// }



extern "C" void __pgi_cu_sync_p(long lineno);
extern "C" void __pgi_cu_sync(long lineno) {
  //TAU_GLOBAL_TIMER_START(pgi_acc_region_timer);
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_sync %s [{%s}{%ld}]", TauPgiFunc, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  __pgi_cu_sync_p(lineno);
  TAU_STOP(sourceinfo);
}

extern "C" void __pgi_cu_fini_p();
extern "C" void __pgi_cu_fini() {
  TAU_PROFILE("__pgi_cu_fini","",TAU_DEFAULT);
  __pgi_cu_fini_p();
}

extern "C" void __pgi_cu_module_p(void *image, cuda_module *module, long lineno);
extern "C" void __pgi_cu_module(void *image, cuda_module *module, long lineno) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_module %s [{%s}{%ld}]", TauPgiFunc, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  __pgi_cu_module_p(image, module, lineno);
  TAU_STOP(sourceinfo);
}

typedef struct __pgi_cuda_module{
        unsigned int capability;/* major*CAPM + minor, so 2003 is 2.3 */
        unsigned int code;      /* code type, cubin, or elf, or ptx */
        size_t length;          /* length in bytes of the code module */
        char* image;            /* the actual code */
    }__pgi_cuda_module;

extern "C" void __pgi_cu_module3_p( __pgi_cuda_module* modlist, long lineno );
extern "C" void __pgi_cu_module3( __pgi_cuda_module* modlist, long lineno ) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_module %s [{%s}{%ld}]", TauPgiFunc, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  __pgi_cu_module3_p(modlist, lineno);
  TAU_STOP(sourceinfo);
}


extern "C" cuda_function __pgi_cu_module_function_p(char *name, long lineno);
extern "C" cuda_function __pgi_cu_module_function(char *name, long lineno) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_module_function %s name=%s [{%s}{%ld}]", TauPgiFunc, name, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  cuda_function func = __pgi_cu_module_function_p(name, lineno);
  functionMap[func] = name;
  TAU_STOP(sourceinfo);
  return func;
}

extern "C" cuda_function __pgi_cu_module_function2_p( char* name, long lineno, 
	char* argname, long argsize, char* varname, long varsize ); 
extern "C" cuda_function __pgi_cu_module_function2( char* name, long lineno, 
	char* argname, long argsize, char* varname, long varsize ) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_module_function2 %s name=%s, argname=%s, argsize=%ld, varname=%s, varsize=%ld [{%s}{%ld}]", 
	TauPgiFunc, name, argname, argsize, varname, varsize, 
	TauPgiFile, lineno);

  TAU_START(sourceinfo);
  cuda_function func = __pgi_cu_module_function2_p(name, lineno, argname, argsize, varname, varsize);
  functionMap[func] = name;
  TAU_STOP(sourceinfo);
  return func;
}

extern "C" cuda_function __pgi_cu_module_file_p(char *imagefile, cuda_module* module, long lineno);
extern "C" cuda_function __pgi_cu_module_file(char *imagefile, cuda_module* module, long lineno) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_module_file %s [{%s}{%ld}]", TauPgiFunc, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  cuda_function func = __pgi_cu_module_file_p(imagefile, module, lineno);
  functionMap[func] = imagefile;
  TAU_STOP(sourceinfo);
  return func;
}



extern "C" void __pgi_cu_module_unload_p();
extern "C" void __pgi_cu_module_unload() {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_module_unload %s [{%s}]", TauPgiFunc, TauPgiFile);
  TAU_START(sourceinfo);
  __pgi_cu_module_unload_p();
  TAU_STOP(sourceinfo);
  TAU_GLOBAL_TIMER_STOP();
}


extern "C" cuda_deviceptr __pgi_cu_alloc_p(size_t size, long lineno, char *name);
extern "C" cuda_deviceptr __pgi_cu_alloc(size_t size, long lineno, char *name) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_alloc %s [{%s}{%ld}]", TauPgiFunc, TauPgiFile, lineno);
  cuda_deviceptr ret;
  TAU_START(sourceinfo);
  ret = __pgi_cu_alloc_p(size, lineno, name);
  TAU_STOP(sourceinfo);
  return ret;
}




extern "C" void __pgi_cu_upload_p(cuda_deviceptr devptr, void* hostptr, size_t size, long lineno, char *name );
extern "C" void __pgi_cu_upload(cuda_deviceptr devptr, void* hostptr, size_t size, long lineno, char *name ) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_upload %s [{%s}{%ld}]", TauPgiFunc, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  __pgi_cu_upload_p(devptr, hostptr, size, lineno, name );
  TAU_STOP(sourceinfo);
}


extern "C" void __pgi_cu_upload1_p(cuda_deviceptr devptr, void* hostptr,
				   size_t devx, size_t hostx,
				   size_t size, size_t hoststride, size_t elementsize, 
				   long lineno, char *name);
extern "C" void __pgi_cu_upload1(cuda_deviceptr devptr, void* hostptr,
				 size_t devx, size_t hostx,
				 size_t size, size_t hoststride, size_t elementsize, 
                                 long lineno, char *name) {
  
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_upload1 %s var=%s [{%s}{%ld}]", TauPgiFunc, name, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  __pgi_cu_upload1_p(devptr, hostptr,
		     devx, hostx,
		     size, hoststride, elementsize, lineno, name);
  TAU_STOP(sourceinfo);
}

extern "C" void __pgi_cu_upload2_p(cuda_deviceptr devptr, void* hostptr,
				   size_t devx, size_t devy, size_t hostx, size_t hosty,
				   size_t size1, size_t size2, size_t devstride2,
				   size_t hoststride1, size_t hoststride2, size_t elementsize, 
				   long lineno, char *name);
extern "C" void __pgi_cu_upload2(cuda_deviceptr devptr, void* hostptr,
				 size_t devx, size_t devy, size_t hostx, size_t hosty,
				 size_t size1, size_t size2, size_t devstride2,
				 size_t hoststride1, size_t hoststride2, size_t elementsize,
				 long lineno, char *name) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_upload2 %s var=%s [{%s}{%ld}]", TauPgiFunc, name, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  __pgi_cu_upload2_p(devptr, hostptr,
		     devx, devy, hostx, hosty,
		     size1, size2, devstride2,
		     hoststride1, hoststride2, elementsize, lineno, name);
  TAU_STOP(sourceinfo);
}

extern "C" void __pgi_cu_upload3_p(cuda_deviceptr devptr, void* hostptr,
				   size_t devx, size_t devy, size_t devz,
				   size_t hostx, size_t hosty, size_t hostz,
				   size_t size1, size_t size2, size_t size3,
				   size_t devstride2, size_t devstride3,
				   size_t hoststride1, size_t hoststride2, size_t hoststride3,
				   size_t elementsize, long lineno, char *name );

extern "C" void __pgi_cu_upload3(cuda_deviceptr devptr, void* hostptr,
				 size_t devx, size_t devy, size_t devz,
				 size_t hostx, size_t hosty, size_t hostz,
				 size_t size1, size_t size2, size_t size3,
				 size_t devstride2, size_t devstride3,
				 size_t hoststride1, size_t hoststride2, size_t hoststride3,
				 size_t elementsize, long lineno, char *name ) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_upload3 %s var=%s [{%s}{%ld}]", TauPgiFunc, name, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  __pgi_cu_upload3_p(devptr, hostptr,
		     devx, devy, devz,
		     hostx, hosty, hostz,
		     size1, size2, size3,
		     devstride2, devstride3,
		     hoststride1, hoststride2, hoststride3,
		     elementsize, lineno, name);
  TAU_STOP(sourceinfo);
}


extern "C" void __pgi_cu_paramset_p(cuda_function func, void* ptr, unsigned long bytes, unsigned long sharedbytes );
extern "C" void __pgi_cu_paramset(cuda_function func, void* ptr, unsigned long bytes, unsigned long sharedbytes ) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_paramset %s [{%s}]", TauPgiFunc, TauPgiFile);
  TAU_START(sourceinfo);
  __pgi_cu_paramset_p(func, ptr, bytes, sharedbytes );
  TAU_STOP(sourceinfo);
}

extern "C" void __pgi_cu_launch_p(cuda_function func, int gridx, int gridy, int gridz, int blockx, int blocky, int blockz, long lineno );
extern "C" void __pgi_cu_launch(cuda_function func, int gridx, int gridy, int gridz, int blockx, int blocky, int blockz, long lineno ) {
  
  string name = functionMap[func];
  char routine[TAU_PGI_ACC_NAME_LEN];
  sprintf (routine, "__pgi_cu_launch %s (%s,gx=%d,gy=%d,gz=%d,bx=%d,by=%d,bz=%d) [{%s}{%ld}]",
    TauPgiFunc, name.c_str(), gridx, gridy, gridz, blockx, blocky, blockz,
    TauPgiFile, lineno);
 
  TAU_START(routine);
  __pgi_cu_launch_p(func, gridx, gridy, gridz, blockx, blocky, blockz, lineno);
  TAU_STOP(routine);
}


extern "C" void __pgi_cu_launch2_p(cuda_function func, int gridx, int gridy, int gridz, int blockx, int blocky, int blockz, int flag, long lineno );
extern "C" void __pgi_cu_launch2(cuda_function func, int gridx, int gridy, int gridz, int blockx, int blocky, int blockz, int flag, long lineno ) {

  string name = functionMap[func];
  char routine[TAU_PGI_ACC_NAME_LEN];
  sprintf (routine, "__pgi_cu_launch %s (%s,gx=%d,gy=%d,gz=%d,bx=%d,by=%d,bz=%d,flag=%d) [{%s}{%ld}]",
    TauPgiFunc, name.c_str(), gridx, gridy, gridz, blockx, blocky, blockz, flag,
    TauPgiFile, lineno);

  TAU_START(routine);
  __pgi_cu_launch2_p(func, gridx, gridy, gridz, blockx, blocky, blockz, flag, lineno);
  TAU_STOP(routine);
}



extern "C" void __pgi_cu_download_p(cuda_deviceptr devptr, void* hostptr, size_t size, long lineno);
extern "C" void __pgi_cu_download(cuda_deviceptr devptr, void* hostptr, size_t size, long lineno) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_download %s [{%s}{%ld}]", TauPgiFunc, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  __pgi_cu_download_p(devptr, hostptr, size, lineno );
  TAU_STOP(sourceinfo);
}


extern "C" void __pgi_cu_download1_p(cuda_deviceptr devptr, void* hostptr,
				   size_t devx, size_t hostx,
				   size_t size, size_t hoststride, size_t elementsize, 
				   long lineno, char *name);
extern "C" void __pgi_cu_download1(cuda_deviceptr devptr, void* hostptr,
				 size_t devx, size_t hostx,
				 size_t size, size_t hoststride, size_t elementsize, 
				 long lineno, char *name) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_download1 %s var=%s [{%s}{%ld}]", TauPgiFunc, name, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  __pgi_cu_download1_p(devptr, hostptr,
		     devx, hostx,
		     size, hoststride, elementsize, lineno, name);
  TAU_STOP(sourceinfo);
}

extern "C" void __pgi_cu_download2_p(cuda_deviceptr devptr, void* hostptr,
				    size_t devx, size_t devy, size_t hostx, size_t hosty,
				    size_t size1, size_t size2, size_t devstride2,
				    size_t hoststride1, size_t hoststride2, size_t elementsize,
				    long lineno, char *name);
extern "C" void __pgi_cu_download2(cuda_deviceptr devptr, void* hostptr,
				  size_t devx, size_t devy, size_t hostx, size_t hosty,
				  size_t size1, size_t size2, size_t devstride2,
				  size_t hoststride1, size_t hoststride2, size_t elementsize, 
				  long lineno, char *name) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_download2 %s var=%s [{%s}{%ld}]", TauPgiFunc, name, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  __pgi_cu_download2_p(devptr, hostptr,
		     devx, devy, hostx, hosty,
		     size1, size2, devstride2,
		     hoststride1, hoststride2, elementsize, lineno, name);
  TAU_STOP(sourceinfo);
}

extern "C" void __pgi_cu_download3_p(cuda_deviceptr devptr, void* hostptr,
			      size_t devx, size_t devy, size_t devz,
			      size_t hostx, size_t hosty, size_t hostz,
			      size_t size1, size_t size2, size_t size3,
			      size_t devstride2, size_t devstride3,
			      size_t hoststride1, size_t hoststride2, size_t hoststride3,
			      size_t elementsize, long lineno, char *name);

extern "C" void __pgi_cu_download3(cuda_deviceptr devptr, void* hostptr,
				  size_t devx, size_t devy, size_t devz,
				  size_t hostx, size_t hosty, size_t hostz,
				  size_t size1, size_t size2, size_t size3,
				  size_t devstride2, size_t devstride3,
				  size_t hoststride1, size_t hoststride2, size_t hoststride3,
				  size_t elementsize, long lineno, char *name) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_download3 %s var=%s [{%s}{%ld}]", TauPgiFunc, name, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  __pgi_cu_download3_p(devptr, hostptr,
		     devx, devy, devz,
		     hostx, hosty, hostz,
		     size1, size2, size3,
		     devstride2, devstride3,
		     hoststride1, hoststride2, hoststride3,
		     elementsize, lineno, name);
  TAU_STOP(sourceinfo);
}

typedef struct __pgi_nv_data{
        size_t devx, devstride, hostx, hoststride, size;
    }__pgi_nv_data;

typedef struct __pgi_nv_xdata{
        TAU_LLONG devx, devstride, hostx, hoststride, size, extent;
    }__pgi_nv_xdata; 

extern "C" void __pgi_cu_downloadx_p( cuda_deviceptr devptr, void* hostptr, int dims,
        __pgi_nv_xdata* desc, long elementsize, long lineno, char* name );

extern "C" void __pgi_cu_downloadx( cuda_deviceptr devptr, void* hostptr, int dims,
        __pgi_nv_xdata* desc, long elementsize, long lineno, char* name ) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_downloadx %s var=%s, dims=%d, desc.devx=%ld, desc.devstride=%ld, desc.hoststride=%ld, desc.size=%ld, desc.extent=%ld, elementsize=%ld [{%s}{%ld}]", 
	TauPgiFunc, name, dims, desc->devx, desc->devstride, 
        desc->hoststride, desc->size, desc->extent, elementsize, TauPgiFile, lineno);

  TAU_START(sourceinfo);

  __pgi_cu_downloadx_p(devptr, hostptr, dims, desc, elementsize, lineno, name);
  TAU_STOP(sourceinfo);
}


extern "C" void __pgi_cu_downloadc_p(char* name, void* hostptr, size_t size, long lineno, size_t offset );
extern "C" void __pgi_cu_downloadc( char* name, void* hostptr, size_t size, long lineno, size_t offset ) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];

  sprintf(sourceinfo, "__pgi_cu_downloadc %s var=%s, size=%ld, offset=%ld [{%s}{%ld}]", 
	TauPgiFunc, name, size, offset, TauPgiFile, lineno);

  TAU_START(sourceinfo);
  __pgi_cu_downloadc_p(name, hostptr, size, lineno, offset);
  TAU_STOP(sourceinfo);
}


extern "C" void __pgi_cu_uploadn_p( cuda_deviceptr devptr, void* hostptr, 
	int dims, __pgi_nv_data* desc, size_t elementsize, long lineno, 
	char* name );

extern "C" void __pgi_cu_uploadn( cuda_deviceptr devptr, void* hostptr, 
	int dims, __pgi_nv_data* desc, size_t elementsize, long lineno, 
	char* name ) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_uploadn %s var=%s, dims=%d, desc.devx=%ld, desc.devstride=%ld, desc.hoststride=%ld, desc.size=%ld, elementsize=%ld [{%s}{%ld}]",
        TauPgiFunc, name, dims, desc->devx, desc->devstride,
        desc->hoststride, desc->size, elementsize, TauPgiFile, lineno);

  TAU_START(sourceinfo);
  __pgi_cu_uploadn_p( devptr, hostptr, dims, desc, elementsize, lineno, name);
  TAU_STOP(sourceinfo);

}

extern "C" void __pgi_cu_uploadp_p( cuda_deviceptr devptr, void* hostptr, 
	int dims, __pgi_nv_data* desc, size_t elementsize, long lineno, 
	char* name );

extern "C" void __pgi_cu_uploadp( cuda_deviceptr devptr, void* hostptr, 
	int dims, __pgi_nv_data* desc, size_t elementsize, long lineno, 
	char* name ) {

  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_uploadp %s var=%s, dims=%d, desc.devx=%ld, desc.devstride=%ld, desc.hoststride=%ld, desc.size=%ld, elementsize=%ld [{%s}{%ld}]",
        TauPgiFunc, name, dims, desc->devx, desc->devstride,
        desc->hoststride, desc->size, elementsize, TauPgiFile, lineno);

  TAU_START(sourceinfo);
  __pgi_cu_uploadp_p( devptr, hostptr, dims, desc, elementsize, lineno, name);
  TAU_STOP(sourceinfo);

}

extern "C" void __pgi_cu_uploadx_p( cuda_deviceptr devptr, void* hostptr, 
	int dims, __pgi_nv_xdata* desc, long elementsize, long lineno, 
	char* name );

extern "C" void __pgi_cu_uploadx( cuda_deviceptr devptr, void* hostptr, 
	int dims, __pgi_nv_xdata* desc, long elementsize, long lineno, 
	char* name ) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_uploadx %s var=%s, dims=%d, desc.devx=%ld, desc.devstride=%ld, desc.hoststride=%ld, desc.size=%ld, elementsize=%ld [{%s}{%ld}]",
        TauPgiFunc, name, dims, desc->devx, desc->devstride,
        desc->hoststride, desc->size, elementsize, TauPgiFile, lineno);

  TAU_START(sourceinfo);
  __pgi_cu_uploadx_p( devptr, hostptr, dims, desc, elementsize, lineno, name);
  TAU_STOP(sourceinfo);

}


extern "C" void __pgi_cu_free_p(cuda_deviceptr ptr, long lineno, char* name);
extern "C" void __pgi_cu_free(cuda_deviceptr ptr, long lineno, char* name) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_free %s [{%s}]", TauPgiFunc, TauPgiFile);
  TAU_START(sourceinfo);
  __pgi_cu_free_p(ptr, lineno, name);
  TAU_STOP(sourceinfo);
}




/*  These routines are supposed to be part of 8.0-6, but they're not there! */

/*
extern "C" void __pgi_cu_uploadc_p(char *name, void* hostptr, size_t size, long lineno);
extern "C" void __pgi_cu_uploadc(char *name, void* hostptr, size_t size, long lineno) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_uploadc %s [{%s}{%ld}]", TauPgiFunc, TauPgiFile, lineno);
  TAU_START(sourceinfo);
  __pgi_cu_uploadc_p(name, hostptr, size, lineno);
  TAU_STOP(sourceinfo);
}


typedef struct __pgi_nv_data {
  size_t devx, devstride, hostx, hoststride, size;
} __pgi_nv_data;



extern "C" void __pgi_cu_uploadn_p(cuda_deviceptr devptr, void* hostptr, int dims, 
				 __pgi_nv_data* desc, size_t elementsize, long lineno, char* name);
extern "C" void __pgi_cu_uploadn(cuda_deviceptr devptr, void* hostptr, int dims, 
				 __pgi_nv_data* desc, size_t elementsize, long lineno, char* name) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_uploadn %s [{%s}]", TauPgiFunc, TauPgiFile);
  TAU_START(sourceinfo);
  __pgi_cu_uploadn_p(devptr, hostptr, dims, desc, elementsize, lineno, name);
  TAU_STOP(sourceinfo);
}


extern "C" void __pgi_cu_uploadp_p(cuda_deviceptr devptr, void* hostptr, int dims, 
				 __pgi_nv_data* desc, size_t elementsize, long lineno, char* name);
extern "C" void __pgi_cu_uploadp(cuda_deviceptr devptr, void* hostptr, int dims, 
				 __pgi_nv_data* desc, size_t elementsize, long lineno, char* name) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_uploadp %s [{%s}]", TauPgiFunc, TauPgiFile);
  TAU_START(sourceinfo);
  __pgi_cu_uploadp_p(devptr, hostptr, dims, desc, elementsize, lineno, name);
  TAU_STOP(sourceinfo);
}


extern "C" void __pgi_cu_downloadn_p(cuda_deviceptr devptr, void* hostptr, int dims, 
				 __pgi_nv_data* desc, size_t elementsize, long lineno, char* name);
extern "C" void __pgi_cu_downloadn(cuda_deviceptr devptr, void* hostptr, int dims, 
				 __pgi_nv_data* desc, size_t elementsize, long lineno, char* name) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_downloadn %s [{%s}]", TauPgiFunc, TauPgiFile);
  TAU_START(sourceinfo);
  __pgi_cu_downloadn_p(devptr, hostptr, dims, desc, elementsize, lineno, name);
  TAU_STOP(sourceinfo);
}


extern "C" void __pgi_cu_downloadp_p(cuda_deviceptr devptr, void* hostptr, int dims, 
				 __pgi_nv_data* desc, size_t elementsize, long lineno, char* name);
extern "C" void __pgi_cu_downloadp(cuda_deviceptr devptr, void* hostptr, int dims, 
				 __pgi_nv_data* desc, size_t elementsize, long lineno, char* name) {
  char sourceinfo[TAU_PGI_ACC_NAME_LEN];
  sprintf(sourceinfo, "__pgi_cu_downloadp %s [{%s}]", TauPgiFunc, TauPgiFile);
  TAU_START(sourceinfo);
  __pgi_cu_downloadp_p(devptr, hostptr, dims, desc, elementsize, lineno, name);
  TAU_STOP(sourceinfo);
}

*/
