#include <SDL.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    // SDL Video alt sistemini baslat
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL Baslatilamadi! Hata: %s\n", SDL_GetError());
        return -1;
    }

    // Ekranda ortalanmis bir pencere olustur
    SDL_Window *window = SDL_CreateWindow("2D Bilardo Simulasyonu",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          800, 600,
                                          SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        printf("Pencere olusturulamadi! Hata: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    printf("Motor basariyla calisti. Pencere 3 saniye acik kalacak...\n");

    // Her seyin calistigini gormek icin pencereyi 3 saniye bekletmek icin.
    SDL_Delay(3000);

    // Bellekleri temizleyen ve motoru kapatan kisim.
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}