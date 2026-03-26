#define SDL_DISABLE_OLD_NAMES
#include "gui.h"
#include "image.h"
#include "histogram.h"
#include "utils.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define HISTOGRAM_WINDOW_WIDTH 400
#define HISTOGRAM_WINDOW_HEIGHT 350
#define BUTTON_WIDTH 150
#define BUTTON_HEIGHT 50
#define BUTTON_MARGIN 20

static TTF_Font *g_font = NULL;

static TTF_Font *gui_open_default_font(void) {
    const char *font_candidates[] = {
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf",
        "/opt/homebrew/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    };
    size_t count = sizeof(font_candidates) / sizeof(font_candidates[0]);
    for (size_t i = 0; i < count; i++) {
        TTF_Font *font = TTF_OpenFont(font_candidates[i], 16.0f);
        if (font) {
            return font;
        }
    }
    return NULL;
}

static void gui_draw_text(SDL_Renderer *renderer, const char *text, int x, int y, SDL_Color color) {
    if (!renderer || !text || !g_font) {
        return;
    }

    SDL_Surface *text_surface = TTF_RenderText_Blended(g_font, text, strlen(text), color);
    if (!text_surface) {
        return;
    }

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (!text_texture) {
        SDL_DestroySurface(text_surface);
        return;
    }

    SDL_FRect dst = {
        (float)x,
        (float)y,
        (float)text_surface->w,
        (float)text_surface->h
    };

    SDL_RenderTexture(renderer, text_texture, NULL, &dst);
    SDL_DestroyTexture(text_texture);
    SDL_DestroySurface(text_surface);
}

GUI* gui_init(Image *img) {
    if (!img) {
        fprintf(stderr, "Erro: imagem inválida\n");
        return NULL;
    }
    
    // Inicializar SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Inicializar SDL_ttf
    if (!TTF_Init()) {
        fprintf(stderr, "Erro ao inicializar SDL_ttf: %s\n", SDL_GetError());
        SDL_Quit();
        return NULL;
    }

    g_font = gui_open_default_font();
    if (!g_font) {
        fprintf(stderr, "Aviso: não foi possível carregar fonte padrão, texto não será exibido.\n");
    }
    
    GUI *gui = (GUI *)malloc(sizeof(GUI));
    if (!gui) {
        printf("Erro ao alocar memória para GUI\n");
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }
    
    // Criar janela principal
    gui->main_window = SDL_CreateWindow(
        "Processador de Imagem",
        img->width,
        img->height,
        0  // SDL3 não usa SDL_WINDOW_SHOWN, usa 0 para flags padrão
    );
    
    if (!gui->main_window) {
        fprintf(stderr, "Erro ao criar janela principal: %s\n", SDL_GetError());
        free(gui);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }
    
    // Centralizar janela principal
    SDL_DisplayID display = SDL_GetPrimaryDisplay();
    if (display != 0) {
        const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(display);
        if (mode) {
            int x = (mode->w - img->width) / 2;
            int y = (mode->h - img->height) / 2;
            SDL_SetWindowPosition(gui->main_window, x, y);
        }
    }
    
    // Criar renderer para janela principal
    gui->main_renderer = SDL_CreateRenderer(gui->main_window, NULL);
    if (!gui->main_renderer) {
        fprintf(stderr, "Erro ao criar renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(gui->main_window);
        free(gui);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }
    
    // Criar janela secundária para o histograma
    gui->histogram_window = SDL_CreateWindow(
        "Histograma",
        HISTOGRAM_WINDOW_WIDTH,
        HISTOGRAM_WINDOW_HEIGHT,
        0  // SDL3 não usa SDL_WINDOW_SHOWN
    );
    
    if (!gui->histogram_window) {
        fprintf(stderr, "Erro ao criar janela de histograma: %s\n", SDL_GetError());
        SDL_DestroyRenderer(gui->main_renderer);
        SDL_DestroyWindow(gui->main_window);
        free(gui);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }
    
    // Posicionar histograma ao lado da janela principal
    int main_x, main_y;
    SDL_GetWindowPosition(gui->main_window, &main_x, &main_y);
    SDL_SetWindowPosition(gui->histogram_window, main_x + img->width + 10, main_y);
    
    // Criar renderer para janela de histograma
    gui->histogram_renderer = SDL_CreateRenderer(gui->histogram_window, NULL);
    if (!gui->histogram_renderer) {
        fprintf(stderr, "Erro ao criar renderer do histograma: %s\n", SDL_GetError());
        SDL_DestroyWindow(gui->histogram_window);
        SDL_DestroyRenderer(gui->main_renderer);
        SDL_DestroyWindow(gui->main_window);
        free(gui);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }
    
    // Criar textura para a imagem
    gui->image_texture = SDL_CreateTextureFromSurface(gui->main_renderer, img->grayscale_surface);
    if (!gui->image_texture) {
        fprintf(stderr, "Erro ao criar textura: %s\n", SDL_GetError());
        SDL_DestroyRenderer(gui->histogram_renderer);
        SDL_DestroyWindow(gui->histogram_window);
        SDL_DestroyRenderer(gui->main_renderer);
        SDL_DestroyWindow(gui->main_window);
        free(gui);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }
    
    // Inicializar botão de equalização
    gui->equalize_button.x = BUTTON_MARGIN;
    gui->equalize_button.y = HISTOGRAM_WINDOW_HEIGHT - BUTTON_HEIGHT - BUTTON_MARGIN;
    gui->equalize_button.width = BUTTON_WIDTH;
    gui->equalize_button.height = BUTTON_HEIGHT;
    gui->equalize_button.is_hovered = 0;
    gui->equalize_button.is_pressed = 0;
    
    gui->is_equalized = 0;
    gui->is_running = 1;
    
    return gui;
}

void gui_render_main_window(GUI *gui, Image *img) {
    if (!gui || !img) {
        return;
    }
    
    // Limpar a janela
    SDL_SetRenderDrawColor(gui->main_renderer, 50, 50, 50, 255);
    SDL_RenderClear(gui->main_renderer);
    
    // Renderizar textura da imagem
    if (gui->image_texture) {
        SDL_RenderTexture(gui->main_renderer, gui->image_texture, NULL, NULL);
    }
    
    // Atualizar display
    SDL_RenderPresent(gui->main_renderer);
}

void gui_draw_histogram(SDL_Renderer *renderer, Histogram *hist, int x, int y, int width, int height) {
    if (!renderer || !hist) {
        return;
    }
    
    // Encontrar o valor máximo de frequência para normalização
    uint32_t max_freq = 0;
    for (int i = 0; i < 256; i++) {
        if (hist->frequency[i] > max_freq) {
            max_freq = hist->frequency[i];
        }
    }
    
    if (max_freq == 0) return;
    
    // Desenhar as barras do histograma
    int bar_width = width / 256;
    if (bar_width < 1) bar_width = 1;
    
    SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
    
    for (int i = 0; i < 256; i++) {
        if (hist->frequency[i] > 0) {
            int bar_height = (int)((hist->frequency[i] * height) / max_freq);
            SDL_FRect rect = {
                x + i * bar_width,
                y + height - bar_height,
                bar_width,
                bar_height
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void gui_render_button(SDL_Renderer *renderer, Button *button, int is_equalized) {
    if (!renderer || !button) {
        return;
    }
    
    // Determinar cor do botão baseado no estado
    SDL_Color color;
    if (button->is_pressed) {
        color = (SDL_Color){0, 50, 150, 255};  // Azul escuro
    } else if (button->is_hovered) {
        color = (SDL_Color){100, 200, 255, 255};  // Azul claro
    } else {
        color = (SDL_Color){0, 100, 200, 255};  // Azul neutro
    }
    
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_FRect button_rect = {
        button->x,
        button->y,
        button->width,
        button->height
    };
    SDL_RenderFillRect(renderer, &button_rect);
    
    // Desenhar borda do botão
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderRect(renderer, &button_rect);

    if (g_font) {
        SDL_Color text_color = {255, 255, 255, 255};
        const char *label = is_equalized ? "Ver original" : "Equalizar";
        int text_x = button->x + 12;
        int text_y = button->y + 15;
        gui_draw_text(renderer, label, text_x, text_y, text_color);
    }
}

void gui_render_histogram_window(GUI *gui, Image *img, Histogram *hist) {
    if (!gui || !img || !hist) {
        return;
    }
    
    // Limpar a janela
    SDL_SetRenderDrawColor(gui->histogram_renderer, 50, 50, 50, 255);
    SDL_RenderClear(gui->histogram_renderer);
    
    // Desenhar histograma
    gui_draw_histogram(gui->histogram_renderer, hist, 10, 10, 380, 200);
    
    if (g_font) {
        SDL_Color text_color = {235, 235, 235, 255};
        char line1[96];
        char line2[96];
        char line3[96];
        char line4[96];

        snprintf(line1, sizeof(line1), "Brilho: %s", hist->brightness);
        snprintf(line2, sizeof(line2), "Contraste: %s", hist->contrast);
        snprintf(line3, sizeof(line3), "Media: %.2f", hist->mean);
        snprintf(line4, sizeof(line4), "Desvio padrao: %.2f", hist->stddev);

        gui_draw_text(gui->histogram_renderer, line1, 12, 220, text_color);
        gui_draw_text(gui->histogram_renderer, line2, 12, 242, text_color);
        gui_draw_text(gui->histogram_renderer, line3, 220, 220, text_color);
        gui_draw_text(gui->histogram_renderer, line4, 220, 242, text_color);
    }
    
    gui_render_button(gui->histogram_renderer, &gui->equalize_button, gui->is_equalized);
    
    // Atualizar display
    SDL_RenderPresent(gui->histogram_renderer);
}

void gui_check_button_click(Button *button, int mouse_x, int mouse_y) {
    if (!button) return;
    
    if (mouse_x >= button->x && mouse_x <= button->x + button->width &&
        mouse_y >= button->y && mouse_y <= button->y + button->height) {
        button->is_pressed = 1;
    }
}

void gui_handle_events(GUI *gui, Image *img, Histogram **hist) {
    if (!gui) {
        return;
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                gui->is_running = 0;
                break;
            
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                if (event.window.windowID == SDL_GetWindowID(gui->main_window) ||
                    event.window.windowID == SDL_GetWindowID(gui->histogram_window)) {
                    gui->is_running = 0;
                }
                break;
            
            case SDL_EVENT_MOUSE_MOTION: {
                if (event.motion.windowID != SDL_GetWindowID(gui->histogram_window)) {
                    break;
                }

                float mouse_x = event.motion.x;
                float mouse_y = event.motion.y;
                
                gui->equalize_button.is_hovered = 0;
                if (mouse_x >= gui->equalize_button.x && 
                    mouse_x <= gui->equalize_button.x + gui->equalize_button.width &&
                    mouse_y >= gui->equalize_button.y && 
                    mouse_y <= gui->equalize_button.y + gui->equalize_button.height) {
                    gui->equalize_button.is_hovered = 1;
                }
                break;
            }
            
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (event.button.windowID != SDL_GetWindowID(gui->histogram_window)) {
                        break;
                    }

                    float mouse_x = event.button.x;
                    float mouse_y = event.button.y;
                    
                    gui_check_button_click(&gui->equalize_button, mouse_x, mouse_y);
                }
                break;
            }
            
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (event.button.windowID != SDL_GetWindowID(gui->histogram_window)) {
                        gui->equalize_button.is_pressed = 0;
                        break;
                    }

                    float mouse_x = event.button.x;
                    float mouse_y = event.button.y;
                    int inside_button =
                        mouse_x >= gui->equalize_button.x &&
                        mouse_x <= gui->equalize_button.x + gui->equalize_button.width &&
                        mouse_y >= gui->equalize_button.y &&
                        mouse_y <= gui->equalize_button.y + gui->equalize_button.height;

                    if (gui->equalize_button.is_pressed && inside_button) {
                        gui->equalize_button.is_pressed = 0;
                        
                        if (!gui->is_equalized) {
                            histogram_equalize(img);
                            gui->is_equalized = 1;
                        } else {
                            histogram_revert(img);
                            gui->is_equalized = 0;
                        }
                        
                        // Recriar textura com imagem atualizada
                        if (gui->image_texture) {
                            SDL_DestroyTexture(gui->image_texture);
                        }
                        gui->image_texture = SDL_CreateTextureFromSurface(gui->main_renderer, img->grayscale_surface);
                        
                        // Recalcular histograma
                        if (*hist) {
                            histogram_free(*hist);
                        }
                        *hist = histogram_calculate(img);
                    } else {
                        gui->equalize_button.is_pressed = 0;
                    }
                }
                break;
            }
            
            case SDL_EVENT_KEY_DOWN: {
                if (event.key.key == SDLK_S) {
                    image_save_png(img, "output_image.png");
                    printf("Imagem salva como output_image.png\n");
                }
                break;
            }
        }
    }
}

void gui_free(GUI *gui) {
    if (!gui) return;
    
    if (gui->image_texture) {
        SDL_DestroyTexture(gui->image_texture);
    }
    if (gui->histogram_renderer) {
        SDL_DestroyRenderer(gui->histogram_renderer);
    }
    if (gui->histogram_window) {
        SDL_DestroyWindow(gui->histogram_window);
    }
    if (gui->main_renderer) {
        SDL_DestroyRenderer(gui->main_renderer);
    }
    if (gui->main_window) {
        SDL_DestroyWindow(gui->main_window);
    }
    
    if (g_font) {
        TTF_CloseFont(g_font);
        g_font = NULL;
    }

    TTF_Quit();
    SDL_Quit();
    free(gui);
}
