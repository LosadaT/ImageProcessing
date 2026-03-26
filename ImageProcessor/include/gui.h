#ifndef GUI_H
#define GUI_H

#include <SDL3/SDL.h>
#include "image.h"
#include "histogram.h"

typedef struct {
    int x, y;
    int width, height;
    int is_hovered;
    int is_pressed;
} Button;

typedef struct {
    SDL_Window *main_window;
    SDL_Renderer *main_renderer;
    SDL_Window *histogram_window;
    SDL_Renderer *histogram_renderer;
    
    SDL_Texture *image_texture;
    
    Button equalize_button;
    
    int is_equalized;
    int is_running;
} GUI;

// Função para inicializar a GUI
GUI* gui_init(Image *img);

// Função para renderizar a janela principal
void gui_render_main_window(GUI *gui, Image *img);

// Função para renderizar a janela secundária
void gui_render_histogram_window(GUI *gui, Image *img, Histogram *hist);

// Função para renderizar o botão
void gui_render_button(SDL_Renderer *renderer, Button *button, int is_equalized);

// Função para processar eventos
void gui_handle_events(GUI *gui, Image *img, Histogram **hist);

// Função para liberar memória da GUI
void gui_free(GUI *gui);

// Função auxiliar para desenhar o histograma
void gui_draw_histogram(SDL_Renderer *renderer, Histogram *hist, int x, int y, int width, int height);

// Função para detectar clique no botão
void gui_check_button_click(Button *button, int mouse_x, int mouse_y);

#endif // GUI_H
