/**
 * @file   tiledb_array_write_whole.cc
 *
 * @section LICENSE
 *
 * The MIT License
 * 
 * @copyright Copyright (c) 2016 MIT and Intel Corporation
 * @copyright Copyright (c) 2019 Omics Data Automation, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * @section DESCRIPTION
 *
 * Example to write dense array to hold 300x300 pixel image of
 *    3x3 color palette by 100x100 panels
 */

#include "examples.h"

int *build_image(size_t num_comps, size_t width, size_t height)
{
   size_t panel_bytes = (num_comps * width * height + 3) * sizeof(int);
   size_t buffer_size = 9 * panel_bytes;
   int *image_buffer = (int *)malloc(buffer_size);
   int *l_data = image_buffer;

   int R[10], G[10], B[10];
//       Black,        Red,          Orange
   R[0] =   0;   R[1] = 201;   R[2] = 234;
   G[0] =   0;   G[1] =  23;   G[2] =  85;
   B[0] =   0;   B[1] =  30;   B[2] =   6;

//       Pink,         White,        Yellow
   R[3] = 233;   R[4] = 255;   R[5] = 255;
   G[3] =  82;   G[4] = 255;   G[5] = 234;
   B[3] = 149;   B[4] = 255;   B[5] =   0;

//       Purple,       Blue,         Green
   R[6] = 101;   R[7] =  12;   R[8] =   0;
   G[6] =  49;   G[7] =   2;   G[8] =  85;
   B[6] = 142;   B[7] = 196;   B[8] =  46;

//       Grey (unused)
   R[9] = 130;
   G[9] = 130;
   B[9] = 130;

   for (int panel = 0; panel < 9; ++panel) {

      // Insert "header" info into image buffer
      *l_data = num_comps; ++l_data;
      *l_data = width    ; ++l_data;
      *l_data = height   ; ++l_data;
   
      for (size_t i = 0; i < width*height; ++i) {
         *l_data = R[panel]; ++l_data;
      }
      for (size_t j = 0; j < width*height; ++j) {
         *l_data = G[panel]; ++l_data;
      }
      for (size_t k = 0; k < width*height; ++k) {
         *l_data = B[panel]; ++l_data;
      }
   }

   return image_buffer;
}

int main(int argc, char *argv[]) {
  // Initialize context with home dir if specified in command line, else
  // initialize with the default configuration parameters
  TileDB_CTX* tiledb_ctx;
  if (argc > 1) {
    TileDB_Config tiledb_config;
    tiledb_config.home_ = argv[1];
    CHECK_RC(tiledb_ctx_init(&tiledb_ctx, &tiledb_config));
  } else {
    CHECK_RC(tiledb_ctx_init(&tiledb_ctx, NULL));
  }

  // Initialize array
  TileDB_Array* tiledb_array;
  CHECK_RC(tiledb_array_init(
      tiledb_ctx,                                // Context 
      &tiledb_array,                             // Array object
      "my_workspace/image_arrays/panelimage",    // Array name
      TILEDB_ARRAY_WRITE,                        // Mode
      NULL,                                      // Entire domain
      NULL,                                      // All attributes
      0));                                        // Number of attributes

  // Prepare cell buffer
  size_t num_comps = 3;
  size_t width  = 100;  // per panel
  size_t height = 100;  // per panel
  size_t num_panels = 9;
  size_t num_panel_elements = num_comps * width * height + 3;
  size_t image_bytes = num_panels * (num_panel_elements * sizeof(int));

  int* buffer_image = build_image(num_comps, width, height);

  const void* buffers[] = { buffer_image };
  size_t buffer_sizes[] = 
  { 
      image_bytes        // sizeof( buffer_image )  
  };

  // Write to array
  CHECK_RC(tiledb_array_write(tiledb_array, buffers, buffer_sizes));

  // Finalize array
  CHECK_RC(tiledb_array_finalize(tiledb_array));

  // Finalize context
  CHECK_RC(tiledb_ctx_finalize(tiledb_ctx));

  return 0;
}
