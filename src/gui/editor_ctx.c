#include <gui/editor_ctx.h>
#include <string.h>

int editor_ctx_init(editor_ctx_t *ed) {
	memset(ed->fname_buffer, 0, sizeof(ed->fname_buffer));
	memset(ed->dname_buffer, 0, sizeof(ed->dname_buffer));
	ed->dname_len = ed->fname_len = 0;
	srt_init(&ed->srt_file);
	ed->srt_loaded = 0;

	// try and load config if exists, else create a new one
	config_create(&ed->config_file, "examples/gui_config");

	// load path
	section_t *cfg = config_get_section(&ed->config_file, "Configuration");
	if (cfg) {
		// set all options
		pair_t *dir = section_get_pair(cfg, "subs_dir");
		if (dir) {
			ed->dname_len = strlen(dir->value);
			memcpy(ed->dname_buffer, dir->value, ed->dname_len);
			ed->dname_buffer[ed->dname_len] = 0;
		}
	}
}

void editor_ctx_set_color_ptr(editor_ctx_t *ed, struct nk_colorf *col) {
	ed->color = col;
}

int editor_ctx_srt_is_loaded(editor_ctx_t *ed) {
	return ed->srt_loaded;
}

int editor_ctx_load(editor_ctx_t *ed) {
	// check dir last character -> '/'
	char fpath[512];
	int ret = 0;

	// make sure the string is closed => nuklear for some reason sometimes doesn't close at last character
	ed->dname_buffer[ed->dname_len] = 0;
	ed->fname_buffer[ed->fname_len] = 0;

	if (ed->dname_buffer[ed->dname_len - 1] == '/') {
		ed->dname_buffer[ed->dname_len - 1] = 0;
		ed->dname_len--;
	}
	sprintf(fpath, "%s/%s", ed->dname_buffer, ed->fname_buffer);
	fprintf(stdout, "full path: %s\n", fpath);
	ret = srt_load_from_file(&ed->srt_file, fpath);
	if (ret == 0) {
		ed->srt_loaded = 1;
	}
	return ret;
}

void editor_ctx_free(editor_ctx_t *ed) {
	srt_free(&ed->srt_file);
	config_delete(&ed->config_file);
}