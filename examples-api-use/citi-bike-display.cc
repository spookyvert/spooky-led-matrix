// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Displays Citi Bike availability with an ASCII bike and numbers.
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"
#include "graphics.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace rgb_matrix;

static const char *bike_ascii_art = 
"   __o\n"
" _`\\<,_\n"
"(*)/ (*)";

static int usage(const char *progname) {
  fprintf(stderr, "usage: %s [options]\n", progname);
  fprintf(stderr, "Displays Citi Bike availability with an ASCII bike and numbers.\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr,
          "\t-f <font-file>    : Use given font for numbers.\n"
          "\t-x <x-origin>     : X-Origin of displaying text (Default: 0)\n"
          "\t-y <y-origin>     : Y-Origin of displaying text (Default: 0)\n"
          "\n");
  rgb_matrix::PrintMatrixFlags(stderr);
  return 1;
}

int main(int argc, char *argv[]) {
  RGBMatrix::Options matrix_options;
  rgb_matrix::RuntimeOptions runtime_opt;
  if (!rgb_matrix::ParseOptionsFromFlags(&argc, &argv,
                                         &matrix_options, &runtime_opt)) {
    return usage(argv[0]);
  }

  const char *bdf_font_file = NULL;
  int x_orig = 0;
  int y_orig = 0;

  int opt;
  while ((opt = getopt(argc, argv, "x:y:f:")) != -1) {
    switch (opt) {
    case 'x': x_orig = atoi(optarg); break;
    case 'y': y_orig = atoi(optarg); break;
    case 'f': bdf_font_file = strdup(optarg); break;
    default:
      return usage(argv[0]);
    }
  }

  if (bdf_font_file == NULL) {
    fprintf(stderr, "Need to specify BDF font-file with -f\n");
    return usage(argv[0]);
  }

  rgb_matrix::Font font;
  if (!font.LoadFont(bdf_font_file)) {
    fprintf(stderr, "Couldn't load font '%s'\n", bdf_font_file);
    return 1;
  }

  RGBMatrix *canvas = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
  if (canvas == NULL)
    return 1;

  Color bike_color(0, 0, 255);  // Blue
  Color number_color(255, 255, 255);  // White

  const int bike_x = x_orig;
  int bike_y = y_orig;

  const int number_x = x_orig;
  int number_y = bike_y + 20;  // Adjust below the bike ASCII art.

  if (isatty(STDIN_FILENO)) {
    printf("Enter availability numbers. CTRL-D for exit.\n");
  }

  canvas->Clear();
  rgb_matrix::DrawText(canvas, font, bike_x, bike_y, bike_color, NULL, bike_ascii_art, 0);

  char line[1024];
  while (fgets(line, sizeof(line), stdin)) {
    const size_t last = strlen(line);
    if (last > 0) line[last - 1] = '\0';  // remove newline.

    canvas->Clear();
    rgb_matrix::DrawText(canvas, font, bike_x, bike_y, bike_color, NULL, bike_ascii_art, 0);
    rgb_matrix::DrawText(canvas, font, number_x, number_y, number_color, NULL, line, 0);
  }

  delete canvas;
  return 0;
}
