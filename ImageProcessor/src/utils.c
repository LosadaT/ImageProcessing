//Francisco Losada Totaro - 10364673
//Pedro Henrique Lanfredi Moreiras - 10441998
#define SDL_DISABLE_OLD_NAMES
#include "utils.h"
#include "image.h"
#include <SDL3_image/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>

int image_save_png(Image *img, const char *filename) {
    if (!img || !img->grayscale_surface || !filename) {
        fprintf(stderr, "Erro: imagem ou filename inválido\n");
        return 0;
    }
    
    if (IMG_SavePNG(img->grayscale_surface, filename)) {
        return 1;
    } else {
        fprintf(stderr, "Erro ao salvar imagem: %s\n", SDL_GetError());
        return 0;
    }
}
