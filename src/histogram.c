#define SDL_DISABLE_OLD_NAMES
#include "histogram.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

Histogram* histogram_calculate(Image *img) {
    if (!img || !img->pixels) {
        return NULL;
    }
    
    Histogram *hist = (Histogram *)malloc(sizeof(Histogram));
    if (!hist) {
        fprintf(stderr, "Erro: falha ao alocar memória para o histograma\n");
        return NULL;
    }
    
    // Inicializar frequências
    memset(hist->frequency, 0, sizeof(hist->frequency));
    
    // Calcular frequências
    uint32_t total_pixels = img->width * img->height;
    for (uint32_t i = 0; i < total_pixels; i++) {
        uint8_t pixel = img->pixels[i];
        hist->frequency[pixel]++;
    }
    
    // Calcular média (mean)
    double sum = 0.0;
    for (int i = 0; i < 256; i++) {
        sum += (double)i * hist->frequency[i];
    }
    hist->mean = sum / total_pixels;
    
    // Calcular desvio padrão (standard deviation)
    double variance = 0.0;
    for (int i = 0; i < 256; i++) {
        double diff = i - hist->mean;
        variance += diff * diff * hist->frequency[i];
    }
    variance /= total_pixels;
    hist->stddev = sqrt(variance);
    
    // Classificar brilho e contraste
    histogram_classify_brightness(hist);
    histogram_classify_contrast(hist);
    
    return hist;
}

void histogram_classify_brightness(Histogram *hist) {
    if (!hist) return;
    
    // Classificar como "clara", "média" ou "escura"
    // Intervalo: 0-255
    // Clara: 170-255
    // Média: 85-170
    // Escura: 0-85
    
    if (hist->mean >= 170.0) {
        strcpy(hist->brightness, "clara");
    } else if (hist->mean >= 85.0) {
        strcpy(hist->brightness, "média");
    } else {
        strcpy(hist->brightness, "escura");
    }
}

void histogram_classify_contrast(Histogram *hist) {
    if (!hist) return;
    
    // Classificar contraste como "alto", "médio" ou "baixo"
    // Baixo: 0-30
    // Médio: 30-80
    // Alto: 80+
    
    if (hist->stddev >= 80.0) {
        strcpy(hist->contrast, "alto");
    } else if (hist->stddev >= 30.0) {
        strcpy(hist->contrast, "médio");
    } else {
        strcpy(hist->contrast, "baixo");
    }
}

void histogram_equalize(Image *img) {
    if (!img || !img->pixels) {
        return;
    }
    
    uint32_t total_pixels = img->width * img->height;
    
    // Calcular histograma
    uint32_t frequency[256];
    memset(frequency, 0, sizeof(frequency));
    
    for (uint32_t i = 0; i < total_pixels; i++) {
        frequency[img->pixels[i]]++;
    }
    
    // Calcular CDF (Cumulative Distribution Function)
    uint32_t cdf[256];
    cdf[0] = frequency[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i-1] + frequency[i];
    }
    
    // Normalizar CDF e criar map de equalização
    uint8_t equalization_map[256];
    uint8_t cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] != 0) {
            cdf_min = cdf[i];
            break;
        }
    }
    
    if (total_pixels == cdf_min) {
        for (int i = 0; i < 256; i++) {
            equalization_map[i] = (uint8_t)i;
        }
    } else {
        for (int i = 0; i < 256; i++) {
            uint32_t cdf_value = cdf[i];
            uint8_t equalized = (uint8_t)(((cdf_value - cdf_min) * 255) / (total_pixels - cdf_min));
            equalization_map[i] = equalized;
        }
    }
    
    // Aplicar mapa de equalização
    for (uint32_t i = 0; i < total_pixels; i++) {
        img->pixels[i] = equalization_map[img->pixels[i]];
    }

    image_refresh_grayscale_surface(img);
}

void histogram_revert(Image *img) {
    if (!img || !img->original_surface) {
        return;
    }
    
    // Reconverter a imagem original para escala de cinza
    image_convert_to_grayscale(img);
}

void histogram_free(Histogram *hist) {
    if (hist) {
        free(hist);
    }
}
