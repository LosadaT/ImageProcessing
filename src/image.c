#define SDL_DISABLE_OLD_NAMES
#include "image.h"
#include <SDL3_image/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

Image* image_load(const char *filename) {
    Image *img = (Image *)malloc(sizeof(Image));
    if (!img) {
        fprintf(stderr, "Erro: falha ao alocar memória para a imagem\n");
        return NULL;
    }
    
    // Carregar a superfície usando SDL_image
    img->original_surface = IMG_Load(filename);
    if (!img->original_surface) {
        fprintf(stderr, "Erro ao carregar imagem: %s\n", SDL_GetError());
        free(img);
        return NULL;
    }
    
    img->width = img->original_surface->w;
    img->height = img->original_surface->h;
    img->is_grayscale = 0;
    img->grayscale_surface = NULL;
    img->pixels = NULL;
    
    // Converter para escala de cinza
    image_convert_to_grayscale(img);
    
    return img;
}

void image_convert_to_grayscale(Image *img) {
    if (!img || !img->original_surface) {
        return;
    }

    if (img->pixels) {
        free(img->pixels);
        img->pixels = NULL;
    }
    if (img->grayscale_surface) {
        SDL_DestroySurface(img->grayscale_surface);
        img->grayscale_surface = NULL;
    }
    
    // Alocar memória para os pixels em escala de cinza
    img->pixels = (uint8_t *)malloc(img->width * img->height);
    if (!img->pixels) {
        fprintf(stderr, "Erro: falha ao alocar memória para pixels\n");
        return;
    }
    
    SDL_Surface *src = img->original_surface;
    SDL_Surface *working = SDL_ConvertSurface(src, SDL_PIXELFORMAT_XRGB8888);
    if (!working) {
        fprintf(stderr, "Erro ao converter superfície: %s\n", SDL_GetError());
        free(img->pixels);
        img->pixels = NULL;
        return;
    }

    const SDL_PixelFormatDetails *fmt = SDL_GetPixelFormatDetails(working->format);
    SDL_Palette *palette = SDL_GetSurfacePalette(working);
    if (!fmt) {
        fprintf(stderr, "Erro ao obter detalhes do formato de pixel\n");
        SDL_DestroySurface(working);
        free(img->pixels);
        img->pixels = NULL;
        return;
    }

    // Iterar sobre os pixels e converter para escala de cinza
    uint8_t *pixels = (uint8_t *)working->pixels;
    int is_grayscale_check = 1;
    int pitch = working->pitch;
    
    // Para cada pixel, extrair RGB e converterpara cinza
    for (uint32_t y = 0; y < img->height; y++) {
        for (uint32_t x = 0; x < img->width; x++) {
            uint8_t r = 0, g = 0, b = 0;
            
            uint8_t *pixel_ptr = pixels + (y * pitch) + (x * 4);
            Uint32 pixel_value = *(Uint32 *)pixel_ptr;
            SDL_GetRGB(pixel_value, fmt, palette, &r, &g, &b);
            
            // Fórmula de conversão para escala de cinza
            // Y = 0.2125 * R + 0.7154 * G + 0.0721 * B
            uint8_t gray = (uint8_t)(0.2125f * r + 0.7154f * g + 0.0721f * b);
            
            img->pixels[y * img->width + x] = gray;
            
            // Verificar se todas as componentes são iguais (imagem já é cinza)
            if (r != g || r != b) {
                is_grayscale_check = 0;
            }
        }
    }
    
    img->is_grayscale = is_grayscale_check;

    SDL_DestroySurface(working);
    
    // Criar uma superfície SDL para a imagem em escala de cinza
    img->grayscale_surface = image_create_surface_from_grayscale(img);
}

int image_is_grayscale(Image *img) {
    if (!img) return 0;
    return img->is_grayscale;
}

uint8_t image_get_pixel(Image *img, uint32_t x, uint32_t y) {
    if (!img || !img->pixels || x >= img->width || y >= img->height) {
        return 0;
    }
    return img->pixels[y * img->width + x];
}

SDL_Surface* image_create_surface_from_grayscale(Image *img) {
    if (!img || !img->pixels) {
        return NULL;
    }

    SDL_Surface *surface = SDL_CreateSurface(img->width, img->height, SDL_PIXELFORMAT_XRGB8888);
    if (!surface) {
        fprintf(stderr, "Erro ao criar superfície: %s\n", SDL_GetError());
        return NULL;
    }

    uint8_t *dst = (uint8_t *)surface->pixels;
    for (uint32_t y = 0; y < img->height; y++) {
        Uint32 *row = (Uint32 *)(dst + y * surface->pitch);
        for (uint32_t x = 0; x < img->width; x++) {
            uint8_t gray = img->pixels[y * img->width + x];
            row[x] = SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, gray, gray, gray);
        }
    }

    return surface;
}

int image_refresh_grayscale_surface(Image *img) {
    if (!img || !img->pixels) {
        return 0;
    }

    SDL_Surface *new_surface = image_create_surface_from_grayscale(img);
    if (!new_surface) {
        return 0;
    }

    if (img->grayscale_surface) {
        SDL_DestroySurface(img->grayscale_surface);
    }
    img->grayscale_surface = new_surface;
    return 1;
}

void image_free(Image *img) {
    if (!img) return;
    
    if (img->pixels) {
        free(img->pixels);
    }
    if (img->original_surface) {
        SDL_DestroySurface(img->original_surface);
    }
    if (img->grayscale_surface) {
        SDL_DestroySurface(img->grayscale_surface);
    }
    free(img);
}
