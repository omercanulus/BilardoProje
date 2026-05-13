#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

// top yapisi renkli
typedef struct
{
    float x, y;
    float vx, vy;
    int radius;
    SDL_Color color;
} Ball;

// ici dolu cember cizmek icin
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

    // coklu top dizisi
    Ball balls[16];
    int ballRadius = 15;

    // beyaz top olusturma
    balls[0] = (Ball){200.0f, 300.0f, 0.0f, 0.0f, ballRadius, {255, 248, 220, 255}};

    // diger toplari ucgen dizme
    int index = 1;
    float startX = 550.0f;
    float startY = 300.0f;

    // toplar arasi bosluk
    float rowSpacing = ballRadius * 1.732f;
    float colSpacing = ballRadius * 2.05f;

    for (int row = 0; row < 5; row++)
    {
        float currentX = startX + (row * rowSpacing);
        float currentStartY = startY - (row * ballRadius);

        for (int col = 0; col <= row; col++)
        {
            float currentY = currentStartY + (col * colSpacing);

            // kirmizi toplar
            balls[index] = (Ball){currentX, currentY, 0.0f, 0.0f, ballRadius, {200, 90, 90, 255}};
            index++;
        }
    }

    bool isRunning = true;
    SDL_Event event;

    // fare surukleme kontrolu degiskenleri
    bool isDragging = false;
    int startMouseX = 0, startMouseY = 0;

    while (isRunning)
    {
        // olaylar
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
            // fare ile vurusa baslama
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    isDragging = true;
                    startMouseX = event.button.x;
                    startMouseY = event.button.y;
                }
            }
            // vurus bittiginde
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (event.button.button == SDL_BUTTON_LEFT && isDragging)
                {
                    isDragging = false;
                    int endMouseX = event.button.x;
                    int endMouseY = event.button.y;

                    // beyaz topa vurus
                    // vurusun tersine gonder ve gucu yumusat
                    balls[0].vx = (startMouseX - endMouseX) * 0.05f;
                    balls[0].vy = (startMouseY - endMouseY) * 0.05f;
                }
            }
        }

        // fizik hareketleri 16 top icin
        for (int i = 0; i < 16; i++)
        {
            // hizi pozisyona ekler top hareket eder
            balls[i].x += balls[i].vx;
            balls[i].y += balls[i].vy;

            // surtunme
            balls[i].vx *= 0.99f;
            balls[i].vy *= 0.99f;

            // topun durmasi
            if (balls[i].vx > -0.02f && balls[i].vx < 0.02f)
                balls[i].vx = 0.0f;
            if (balls[i].vy > -0.02f && balls[i].vy < 0.02f)
                balls[i].vy = 0.0f;

            // ekran sinirlari
            if (balls[i].x - balls[i].radius < 0)
            {
                balls[i].x = balls[i].radius;
                balls[i].vx = -balls[i].vx;
            }
            else if (balls[i].x + balls[i].radius > 800)
            {
                balls[i].x = 800 - balls[i].radius;
                balls[i].vx = -balls[i].vx;
            }

            // ust ve alt bantlar
            if (balls[i].y - balls[i].radius < 0)
            {
                balls[i].y = balls[i].radius;
                balls[i].vy = -balls[i].vy;
            }
            else if (balls[i].y + balls[i].radius > 600)
            {
                balls[i].y = 600 - balls[i].radius;
                balls[i].vy = -balls[i].vy;
            }
        }

        // cizim
        SDL_SetRenderDrawColor(renderer, 85, 140, 110, 255);
        SDL_RenderClear(renderer);

        // vurus gosterimi
        if (isDragging)
        {
            int currentMouseX, currentMouseY;
            SDL_GetMouseState(&currentMouseX, &currentMouseY);
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 150);
            SDL_RenderDrawLine(renderer, (int)balls[0].x, (int)balls[0].y, currentMouseX, currentMouseY);
        }

        // toplari ciz
        for (int i = 0; i < 16; i++)
        {
            SDL_SetRenderDrawColor(renderer, balls[i].color.r, balls[i].color.g, balls[i].color.b, balls[i].color.a);
            DrawFilledCircle(renderer, (int)balls[i].x, (int)balls[i].y, balls[i].radius);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}