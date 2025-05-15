#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void compute_freqs(size_t *freqs, const char* message, size_t length)
{
  for (size_t i = 0; i < length; i++) {
    freqs[(unsigned char)message[i]] += 1;
  }
}

void compute_probabilities(double* probs, size_t* freqs, size_t length)
{
  for (size_t i = 0; i < length; i++) {
    probs[i] = (double)freqs[i] / (double)length;
  }
}

double calculate_entropy(const char* message, size_t length)
{
    size_t freqs[256] = {0};
    compute_freqs(freqs, message, length);

    double H = 0.0;
    for (size_t i = 0; i < 256; i++) {
      if (freqs[i] > 0) {
        double p = (double)freqs[i] / (double)length;
        H += p * log2(p);
      }
    }

    return -H;
}

double calculate_entropy_s(const char* message)
{
    return calculate_entropy(message, strlen(message));
}

double max_entropy(size_t length)
{
    double H = 0.0;
    for (size_t i = 0; i < 256; i++) {
      double p = 1.0 / (double)length;
      H += p * log2(p);
    }
    return -H;
}

double compute_E(const char* message, size_t length)
{
    size_t freqs[256] = {0};
    compute_freqs(freqs, message, length);

    double E = 0.0;
    for (size_t i = 0; i < 256; i++) {
	E += (double)i * ((double)freqs[i] / (double)length);
    }
    
    return E;
}

double generic_compute_E(const double* X, const double* PX, size_t length)
{
    double E = 0.0;
    for (size_t i = 0; i < length; i++) {
	E += X[i] * PX[i];
    }
    return E;
}

double generic_compute_H(const double* probabilities, size_t length)
{
    double* surprises = (double*)malloc(length * sizeof(double));
    for (size_t i = 0; i < length; i++) {
	surprises[i] = log2(1.0 / probabilities[i]);
    }
    double H = generic_compute_E(surprises, probabilities, length);
    free(surprises);
    return H;
}

#define ALL_COUNT 256

int main(void)
{
    double H, E;
    char all[ALL_COUNT] = {0};
    for (size_t i = 0; i < ALL_COUNT; i++) {
      all[i] = i % ALL_COUNT;
    }
    H = calculate_entropy(all, ALL_COUNT);
    printf("H = %f\n", H);
    H = max_entropy(ALL_COUNT);
    printf("H = %f\n", H);

    const char* text = "Hello, world! Im here what do you want?";
    E = compute_E(text, strlen(text));
    printf("E = %f\n", E);

    size_t freqs[256] = {0};
    compute_freqs(freqs, all, ALL_COUNT);
    double probs[256] = {0};
    compute_probabilities(probs, freqs, 256);
    H = generic_compute_H(probs, 256);
    printf("H = %f\n", H);
    
    printf("-------------------------\n");
    {
	char text[512] = {0};
	for (size_t i = 0; i < 512; i++) {
	    text[i] = i;
	}
	H = calculate_entropy(text, 512);
	printf("H = %f\n", H);
    }
    return 0;
}

// TODO: Entropy of the words (take code from huffman coding) for spam filtering?

