#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "demo/glfw_opengl3/nuklear_glfw_gl3.h"
#include <gui/editor_ctx.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

static void error_callback(int e, const char *d) {
	printf("Error %d: %s\n", e, d);
}

static unsigned char hex_char_to_int(char c[2]) {
	unsigned char ret = 0;
	ret += (c[0] >= 'a' && c[0] <= 'f') ? (10 + c[0] - 'a') : c[0] - '0';
	ret *= 16;
	ret += (c[1] >= 'a' && c[1] <= 'f') ? (10 + c[1] - 'a') : c[1] - '0';
	return ret;
}

static struct nk_colorf
str_to_color(char *str) {
	// format '#rrggbbaa'
	struct nk_colorf col = {0};
	if (strlen(str) == 8) {
		char r[2], g[2], b[2], a[2];
		memcpy(r, str, 2);
		str += 2;
		memcpy(g, str, 2);
		str += 2;
		memcpy(b, str, 2);
		str += 2;
		memcpy(a, str, 2);
		str += 2;
		col.r = (float) hex_char_to_int(r) / 255;
		col.g = (float) hex_char_to_int(g) / 255;
		col.b = (float) hex_char_to_int(b) / 255;
		col.a = (float) hex_char_to_int(a) / 255;
		// assert reached the end
		assert(*str == 0);
	}
	return col;
}

int main(void) {
	/* Platform */
	struct nk_glfw glfw = {0};
	static GLFWwindow *win;
	int width = 0, height = 0;
	struct nk_context *ctx;
	struct nk_colorf bg;

	// save editor related config and data
	editor_ctx_t ed_ctx;

	// init ctx and load config
	editor_ctx_init(&ed_ctx);
	editor_ctx_set_color_ptr(&ed_ctx, &bg);

	// set color immediately when config is loaded
	section_t *cfg = config_get_section(&ed_ctx.config_file, "Configuration");
	if (!cfg) {
		// error loading config
		return -1;
	}

	pair_t *bgcol = section_get_pair(cfg, "bg_color");
	bg = str_to_color(bgcol->value);

	/* GLFW */
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		fprintf(stdout, "[GFLW] failed to init!\n");
		exit(1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Demo", NULL, NULL);
	glfwMakeContextCurrent(win);
	glfwGetWindowSize(win, &width, &height);

	/* OpenGL */
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glewExperimental = 1;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to setup GLEW\n");
		exit(1);
	}

	ctx = nk_glfw3_init(&glfw, win, NK_GLFW3_INSTALL_CALLBACKS);
	{
		struct nk_font_atlas *atlas;
		nk_glfw3_font_stash_begin(&glfw, &atlas);
		nk_glfw3_font_stash_end(&glfw);
	}

	while (!glfwWindowShouldClose(win)) {
		/* Input */
		glfwPollEvents();
		nk_glfw3_new_frame(&glfw);

		/* Config GUI */
		if (nk_begin(ctx, "Configuration", nk_rect(0, 0, 400, WINDOW_HEIGHT),
								 NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_TITLE)) {
			enum { EASY,
						 HARD };
			static int op = EASY;
			static int property = 20;

			// dir name
			nk_layout_row_static(ctx, 30, 100, 3);
			nk_label(ctx, "Directory:", NK_TEXT_LEFT);
			nk_edit_string(ctx, NK_EDIT_FIELD, ed_ctx.dname_buffer, &ed_ctx.dname_len, 256, NULL);
			if (nk_button_label(ctx, "Save Dir")) {
				// save directory field to config file
				fprintf(stdout, "Saving %s...\n", ed_ctx.dname_buffer);
			}

			// file name
			nk_layout_row_static(ctx, 30, 100, 3);
			nk_label(ctx, "Filename:", NK_TEXT_LEFT);
			nk_edit_string(ctx, NK_EDIT_FIELD, ed_ctx.fname_buffer, &ed_ctx.fname_len, 256, NULL);

			// open file button
			if (nk_button_label(ctx, "Open File")) {
				// load srt file
				editor_ctx_load(&ed_ctx);
				srt_print(&ed_ctx.srt_file, stdout);
			}

			nk_layout_row_dynamic(ctx, 30, 2);
			if (nk_option_label(ctx, "easy", op == EASY))
				op = EASY;
			if (nk_option_label(ctx, "hard", op == HARD))
				op = HARD;

			nk_layout_row_dynamic(ctx, 25, 1);
			nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

			nk_layout_row_dynamic(ctx, 20, 1);
			nk_label(ctx, "Background Color:", NK_TEXT_LEFT);
			nk_layout_row_dynamic(ctx, 25, 1);
			if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400))) {
				nk_layout_row_dynamic(ctx, 120, 1);
				bg = nk_color_picker(ctx, bg, NK_RGBA);
				nk_layout_row_dynamic(ctx, 25, 1);
				bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
				bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
				bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
				bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
				nk_combo_end(ctx);
			}

			nk_layout_row_static(ctx, 30, 200, 1);
			nk_label(ctx, "Configuration action buttons:", NK_TEXT_LEFT);
			nk_layout_row_static(ctx, 30, 100, 2);
			if (nk_button_label(ctx, "Save Config")) {
				// save directory field to config file
				fprintf(stdout, "Saving config...\n");
			}
			if (nk_button_label(ctx, "Open Config")) {
				// save directory field to config file
				fprintf(stdout, "Opening config...\n");
			}
		}
		nk_end(ctx);

		/* Subs GUI */
		if (nk_begin(ctx, "Subs", nk_rect(400, 0, WINDOW_WIDTH, 500),
								 NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_TITLE)) {
			if (ed_ctx.srt_loaded) {
				// file is loaded -> show subs
				int count = ed_ctx.srt_file.subs.count;
				char buf[100];
				int *selected = malloc(sizeof(int) * count);
				for (int i = 0; i < count; i++) {
					nk_layout_row_dynamic(ctx, 20, 3);
					nk_layout_row_static(ctx, 50, (WINDOW_WIDTH - 400) / 3, 3);
					sprintf(buf, "Line %d", i + 1);
					nk_selectable_label(ctx, buf, NK_TEXT_ALIGN_CENTERED, &selected[i]);
					if (nk_button_label(ctx, "Edit")) {
						// save directory field to config file
						fprintf(stdout, "Edit sub...\n");
					}
					if (nk_button_label(ctx, "Remove")) {
						// save directory field to config file
						fprintf(stdout, "Edit sub...\n");
					}
				}
				free(selected);
			} else {
				// show basic label
				nk_layout_row_dynamic(ctx, 20, 1);
				nk_label(ctx, "Waiting for subs...", NK_TEXT_LEFT);
			}
		}
		nk_end(ctx);

		/* Draw */
		glfwGetWindowSize(win, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(bg.r, bg.g, bg.b, bg.a);
		/* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
		nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
		glfwSwapBuffers(win);
	}
	editor_ctx_free(&ed_ctx);
	nk_glfw3_shutdown(&glfw);
	glfwTerminate();
	return 0;
}
