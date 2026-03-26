#ifndef IMAGE_H
#define IMAGE_H

#include <SDL3/SDL.h>
#include <stdint.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t *pixels;      // Grayscale pixels
    SDL_Surface *original_surface;
    SDL_Surface *grayscale_surface;
    int is_grayscale;
} Image;

// Função para carregar uma imagem
Image* image_load(const char *filename);

// Função para converter para escala de cinza
void image_convert_to_grayscale(Image *img);

// Função para verificar se é escala de cinza
int image_is_grayscale(Image *img);

// Função para liberar memória da imagem
void image_free(Image *img);

// Função para obter o pixel em escala de cinza
uint8_t image_get_pixel(Image *img, uint32_t x, uint32_t y);

// Função para criar uma superfície SDL a partir dos pixels em escala de cinza
SDL_Surface* image_create_surface_from_grayscale(Image *img);

// Reconstrói a superfície grayscale a partir de img->pixels
int image_refresh_grayscale_surface(Image *img);

#endif // IMAGE_H
