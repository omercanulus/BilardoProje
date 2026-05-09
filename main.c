#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

// 1. Topun özelliklerini tutacak veri yapısı
typedef struct
{
    float x, y;   // Merkez koordinatları
    float vx, vy; // Hız (x ve y ekseninde)
    int radius;   // Yarıçap
} Ball;

// 2. İçi dolu çember çizme algoritması (Midpoint Circle Algorithm)
void DrawFilledCircle(SDL_Renderer *renderer, int x0, int y0, int radius)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        SDL_RenderDrawLine(renderer, x0 - x, y0 - y, x0 + x, y0 - y);
        SDL_RenderDrawLine(renderer, x0 - y, y0 - x, x0 + y, y0 - x);
        SDL_RenderDrawLine(renderer, x0 - x, y0 + y, x0 + x, y0 + y);
        SDL_RenderDrawLine(renderer, x0 - y, y0 + x, x0 + y, y0 + x);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

int main(int argc, char *argv[])
{
    // Derleyicinin kullanılmayan değişken uyarılarını susturmak için:
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL Baslatilamadi! Hata: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Bilardo Oyunu",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          800, 600,
                                          SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        printf("Pencere olusturulamadi. Hata: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("Cizici olusturulamadi! Hata: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Beyaz topu masanın sol tarafına yerleştiriyoruz
    Ball cueBall = {200.0f, 300.0f, 0.0f, 0.0f, 15};

    bool isRunning = true;
    SDL_Event event;

    while (isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }

        // 1. Arka planı temizle (Yeşil Çuha)
        SDL_SetRenderDrawColor(renderer, 85, 140, 110, 255);
        SDL_RenderClear(renderer);

        // 2. Çizim Rengini Ayarla (Soft Cornsilk Tonu)
        SDL_SetRenderDrawColor(renderer, 255, 248, 220, 255);

        // 3. Topu Ekrana Çiz
        DrawFilledCircle(renderer, (int)cueBall.x, (int)cueBall.y, cueBall.radius);

        // 4. Ekrana Yansıt
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}