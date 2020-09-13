#pragma once

#include <srt.h>
#include <cconf.h>

typedef struct editor_ctx editor_ctx_t;

// define empty structs because nuklear...
struct nkcolorf;

struct editor_ctx {
	// file name and dir name
	// TODO: add option to save dir name
	char fname_buffer[256];
	char dname_buffer[256];

	int fname_len;
	int dname_len;

	// editor color
	// save at the end
	struct nk_colorf *color;

	// srt file to load
	srt_t srt_file;
	// marks if a file has been loaded or not
	int srt_loaded;

	// config file used
	config_t config_file;
};

/** Initializes ctx and sets all fields to zero
    @param ed ctx to init
*/
int editor_ctx_init(editor_ctx_t *ed);

/** Check if editor_ctx loaded some file
    @param ed ctx
    @return 1 if the file is loaded and 0 if not
*/
int editor_ctx_srt_is_loaded(editor_ctx_t *ed);

/** Set editor background color pointer
    @param ed ctx for which a color should be set
    @param col color to set
*/
void editor_ctx_set_color_ptr(editor_ctx_t *ed, struct nk_colorf *col);

/** Load srt_file with given file and dir path
    @param ed ctx which should load the file
    @return error if any occurs; unable to read file, wrong paths etc.
*/
int editor_ctx_load(editor_ctx_t *ed);

/** Free all ctx data
    @param ed ctx to free
*/
void editor_ctx_free(editor_ctx_t *ed);