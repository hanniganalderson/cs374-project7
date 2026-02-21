# Grayscaler

## Building

Command line:

* `make` to build. An executable called `grayscaler` will be produced.
* `make clean` to clean up all build products.

## Files

* `grayscaler.c`: Main program that converts a color PPM image to grayscale using multiple threads
* `ppm.c`: PPM image library
* `ppm.h`: Header for the PPM library

## Data

A `thread_data` struct is used to pass data to and from each thread. It holds a pointer to the image, the starting y coordinate, number of rows, and running sums of R, G, B values.

An array of `pthread_t` and an array of `thread_data` structs are used, one per thread.

## Functions

* `main()`: Parses args, calls helpers, writes output
  * `launch_threads()`: Sets up thread data and creates all threads
    * `convert_slice()`: Thread function that converts a slice to grayscale and tracks RGB sums
  * `wait_and_print()`: Joins threads and prints per-thread info and average RGB values

## Notes

* The image is sliced horizontally into n slices. The last thread handles any leftover rows.
* Each thread computes grayscale using the BT.601 luma formula.
