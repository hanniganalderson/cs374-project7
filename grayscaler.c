#include "ppm.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct thread_data {
  struct ppm *img;
  int start_y;
  int rows;
  long sum_r;
  long sum_g;
  long sum_b;
};

void *convert_slice(void *arg) {
  struct thread_data *td = (struct thread_data *)arg;

  td->sum_r = 0;
  td->sum_g = 0;
  td->sum_b = 0;

  for (int y = td->start_y; y < td->start_y + td->rows; y++) {
    for (int x = 0; x < td->img->width; x++) {
      int p = ppm_get_pixel(td->img, x, y);
      int r = PPM_PIXEL_R(p);
      int g = PPM_PIXEL_G(p);
      int b = PPM_PIXEL_B(p);

      td->sum_r += r;
      td->sum_g += g;
      td->sum_b += b;

      int gray = (299 * r + 587 * g + 114 * b) / 1000;
      ppm_set_pixel(td->img, x, y, PPM_PIXEL(gray, gray, gray));
    }
  }

  return NULL;
}

void launch_threads(pthread_t *threads, struct thread_data *td, struct ppm *img,
                    int n) {
  int base_h = img->height / n;
  int extra = img->height % n;

  for (int i = 0; i < n; i++) {
    td[i].img = img;
    td[i].start_y = i * base_h;
    td[i].rows = base_h;
    if (i == n - 1)
      td[i].rows += extra;

    pthread_create(&threads[i], NULL, convert_slice, &td[i]);
  }
}

void wait_and_print(pthread_t *threads, struct thread_data *td, int n,
                    int total_pixels) {
  long total_r = 0, total_g = 0, total_b = 0;

  for (int i = 0; i < n; i++)
    pthread_join(threads[i], NULL);

  for (int i = 0; i < n; i++) {
    printf("Thread %d: %d %d\n", i, td[i].start_y, td[i].rows);
    total_r += td[i].sum_r;
    total_g += td[i].sum_g;
    total_b += td[i].sum_b;
  }

  printf("Average R: %ld\n", total_r / total_pixels);
  printf("Average G: %ld\n", total_g / total_pixels);
  printf("Average B: %ld\n", total_b / total_pixels);
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "usage: grayscaler <threads> <infile> <outfile>\n");
    return 1;
  }

  int n = atoi(argv[1]);
  struct ppm *img = ppm_read(argv[2]);

  pthread_t threads[n];
  struct thread_data td[n];

  launch_threads(threads, td, img, n);
  wait_and_print(threads, td, n, img->width * img->height);

  ppm_write(img, argv[3]);

  return 0;
}
