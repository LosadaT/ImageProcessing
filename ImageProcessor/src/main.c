#define SDL_DISABLE_OLD_NAMES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "histogram.h"
#include "gui.h"

int main(int argc, char *argv[]) {
    // Verificar argumentos da linha de comando
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <caminho_da_imagem>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s imagem.png\n", argv[0]);
        return 1;
    }
    
    const char *image_path = argv[1];
    
    // Carregar a imagem
    printf("Carregando imagem: %s\n", image_path);
    Image *img = image_load(image_path);
    
    if (!img) {
        fprintf(stderr, "Erro fatal: não foi possível carregar a imagem\n");
        return 1;
    }
    
    printf("Imagem carregada com sucesso: %ux%u pixels\n", img->width, img->height);
    printf("Tipo de imagem: %s\n", image_is_grayscale(img) ? "Escala de cinza" : "Colorida");
    
    // Calcular histograma
    Histogram *hist = histogram_calculate(img);
    
    if (!hist) {
        fprintf(stderr, "Erro ao calcular histograma\n");
        image_free(img);
        return 1;
    }
    
    printf("Informações do histograma:\n");
    printf("  Média de intensidade: %.2f\n", hist->mean);
    printf("  Classificação de brilho: %s\n", hist->brightness);
    printf("  Desvio padrão: %.2f\n", hist->stddev);
    printf("  Classificação de contraste: %s\n", hist->contrast);
    
    // Inicializar GUI
    printf("Inicializando interface gráfica...\n");
    GUI *gui = gui_init(img);
    
    if (!gui) {
        fprintf(stderr, "Erro ao inicializar GUI\n");
        histogram_free(hist);
        image_free(img);
        return 1;
    }
    
    printf("Aplicação iniciada com sucesso!\n");
    printf("Controles:\n");
    printf("  - Clique no botão para equalizar/reverter o histograma\n");
    printf("  - Pressione 'S' para salvar a imagem como output_image.png\n");
    printf("  - Feche a janela para encerrar\n");
    
    // Loop de eventos principal
    while (gui->is_running) {
        gui_handle_events(gui, img, &hist);
        gui_render_main_window(gui, img);
        gui_render_histogram_window(gui, img, hist);
        
        //SDL_Delay(16); 
    }
    
    // Liberar recursos
    printf("Encerrando aplicação...\n");
    gui_free(gui);
    histogram_free(hist);
    image_free(img);
    
    printf("Aplicação encerrada com sucesso!\n");
    return 0;
}
