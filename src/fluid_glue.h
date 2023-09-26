#ifndef _FLUID_GLUE_H
#define _FLUID_GLUE_H

typedef struct FLUID_FILE {
	void *f;
} FLUID_file;

typedef FLUID_FILE* fluid_file;

fluid_file fluid_fopen(const char *p, const char *omode);
int 	fluid_fseek (fluid_file f, size_t offset, int origin);
int 	fluid_feof  (fluid_file f);
size_t 	fluid_ftell (fluid_file f);
void 	fluid_fclose(fluid_file f);
uint8_t *fluid_mmap (size_t pos, size_t size, fluid_file f);
size_t 	fluid_fread (void *ptr, size_t size, size_t count, fluid_file f);

void    *fluid_malloc(size_t size);
void    fluid_free(void *ptr);

int     fluid_printf(const char* format, ...);
int     fluid_vprintf(const char* format, va_list va);

#endif
