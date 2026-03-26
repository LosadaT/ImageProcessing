//Francisco Losada Totaro - 10364673
//Pedro Henrique Lanfredi Moreiras - 10441998
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <stdint.h>
#include "image.h"

typedef struct {
    uint32_t frequency[256];  // Frequência de cada intensidade (0-255)
    double mean;               // Média de intensidade
    double stddev;             // Desvio padrão
    char brightness[32];       // "clara", "média", "escura"
    char contrast[32];         // "alto", "médio", "baixo"
} Histogram;

// Função para calcular o histograma
Histogram* histogram_calculate(Image *img);

// Função para classificar brilho (clara/média/escura)
void histogram_classify_brightness(Histogram *hist);

// Função para classificar contraste (alto/médio/baixo)
void histogram_classify_contrast(Histogram *hist);

// Função para equalizar o histograma
void histogram_equalize(Image *img);

// Função para reverter para a imagem original
void histogram_revert(Image *img);

// Função para liberar memória do histograma
void histogram_free(Histogram *hist);

#endif // HISTOGRAM_H
