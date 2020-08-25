#include "srt.h"
#include <argparse.h>
#include <stdio.h>
static const char *const usage[] = {
    "srted [options] [[--] args]",
    "srted [options]",
    NULL,
};

#define PERM_READ (1 << 0)
#define PERM_WRITE (1 << 1)
#define PERM_EXEC (1 << 2)

int main(int argc, const char **argv) {
  int force = 0;
  int test = 0;
  long long num = 0;
  int hours, minutes, seconds;
  const char *in_file = NULL, *out_file = NULL;
  struct argparse_option options[] = {
      OPT_HELP(),
      OPT_GROUP("Basic options"),
      OPT_STRING('i', "input", &in_file, "input file"),
      OPT_STRING('o', "output", &out_file, "output file"),
      OPT_BOOLEAN('r', "hours", &hours, "hours to add/subtract"),
      OPT_BOOLEAN('m', "minutes", &minutes, "minutes to add/subtract"),
      OPT_BOOLEAN('s', "seconds", &seconds, "seconds to add/subtract"),
      OPT_INTEGER('n', "number", &num,
                  "number of given time to change (by default in miliseconds)"),
      OPT_END(),
  };

  struct argparse argparse;
  argparse_init(&argparse, options, usage, 0);
  argparse_describe(
      &argparse,
      "\nA .srt file editor and a program for manipulation of srt files.",
      "\nManipulating time of whole subtitles and specific subtitles.");
  argc = argparse_parse(&argparse, argc, argv);
  srt_t file;
  srt_init(&file);

  if (in_file != NULL) {
    srt_load_from_file(&file, in_file);
    // srt_print(&file, stdout);
  }
  if (num != 0) {
    if (hours) {
      srt_add_hours(&file, num);
    } else if (minutes) {
      srt_add_minutes(&file, num);
    } else if (seconds) {
      srt_add_seconds(&file, num);
    } else {
      srt_add_miliseconds(&file, num);
    }
  }

  if (out_file) {
    srt_write_to_file(&file, out_file);
  } else {
    // if no output file given => print to standard output
    srt_print(&file, stdout);
  }
  srt_free(&file);
  return 0;
}