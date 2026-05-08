#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
    // SDL Video alt sistemini baslatmak icin.
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL Baslatilamadi! Hata: %s\n", SDL_GetError());
        return -1;
    }

    // Ekranda bir pencere olusturmak icin
    SDL_Window *window = SDL_CreateWindow(" Bilardo Oyunu",
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

    // Pencereye cizim yapabilmek icin bir çizici olusturmak gerekiyor
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("Cizici olusturulamadi! Hata: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // oyun dongusu icin
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

        // arka plan icin
        SDL_SetRenderDrawColor(renderer, 85, 140, 110, 255);

        SDL_RenderClear(renderer);

        // cizimleri ekrana yansıtmak icin
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}