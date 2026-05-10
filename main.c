#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

// Top yapisi
typedef struct
{
    float x, y;
    float vx, vy;
    int radius;
} Ball;

// Ici dolu cember cizmek icin
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
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Top olusturma
    Ball cueBall = {200.0f, 300.0f, 0.0f, 0.0f, 15};

    bool isRunning = true;
    SDL_Event event;

    // Fare sürükleme kontrolu degiskenleri
    bool isDragging = false;
    int startMouseX = 0, startMouseY = 0;

    while (isRunning)
    {
        // Olaylar
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
            // Fare ile vurusa baslama
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    isDragging = true;
                    startMouseX = event.button.x;
                    startMouseY = event.button.y;
                }
            }
            // Vurus bittiginde
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (event.button.button == SDL_BUTTON_LEFT && isDragging)
                {
                    isDragging = false;
                    int endMouseX = event.button.x;
                    int endMouseY = event.button.y;

                    // Vurusun tersine gonder
                    // gucu yumusatmak icin 0.5 ile caro
                    cueBall.vx = (startMouseX - endMouseX) * 0.05f;
                    cueBall.vy = (startMouseY - endMouseY) * 0.05f;
                }
            }
        }

        // Fizik hareketleri

        // Hizi pozisyona ekler top hareket eder
        cueBall.x += cueBall.vx;
        cueBall.y += cueBall.vy;

        // Surtunme
        cueBall.vx *= 0.99f;
        cueBall.vy *= 0.99f;

        // Topun durmasi
        if (cueBall.vx > -0.02f && cueBall.vx < 0.02f)
            cueBall.vx = 0.0f;
        if (cueBall.vy > -0.02f && cueBall.vy < 0.02f)
            cueBall.vy = 0.0f;

        // Ekran Sinirlari
        if (cueBall.x - cueBall.radius < 0)
        {
            cueBall.x = cueBall.radius;
            cueBall.vx = -cueBall.vx;
        }
        else if (cueBall.x + cueBall.radius > 800)
        {
            cueBall.x = 800 - cueBall.radius;
            cueBall.vx = -cueBall.vx;
        }

        // Ust ve Alt bantlar
        if (cueBall.y - cueBall.radius < 0)
        {
            cueBall.y = cueBall.radius;
            cueBall.vy = -cueBall.vy;
        }
        else if (cueBall.y + cueBall.radius > 600)
        {
            cueBall.y = 600 - cueBall.radius;
            cueBall.vy = -cueBall.vy;
        }

        // Cizim
        SDL_SetRenderDrawColor(renderer, 85, 140, 110, 255);
        SDL_RenderClear(renderer);

        // Vurus gosterimi
        if (isDragging)
        {
            int currentMouseX, currentMouseY;
            SDL_GetMouseState(&currentMouseX, &currentMouseY);

            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 150);
            SDL_RenderDrawLine(renderer, (int)cueBall.x, (int)cueBall.y, currentMouseX, currentMouseY);
        }

        // Topu ciz
        SDL_SetRenderDrawColor(renderer, 255, 248, 220, 255);
        DrawFilledCircle(renderer, (int)cueBall.x, (int)cueBall.y, cueBall.radius);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}