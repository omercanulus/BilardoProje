#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// top yapisi
typedef struct
{
    float x, y;
    float vx, vy;
    int radius;
    SDL_Color color;
    bool active;
} Ball;

// delik yapisi
typedef struct
{
    int x, y;
    int radius;
} Pocket;

// ici dolu cember cizimi
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

    // delikler
    Pocket pockets[6] = {
        {25, 25, 25}, {400, 20, 25}, {775, 25, 25}, {25, 575, 25}, {400, 580, 25}, {775, 575, 25}};

    Ball balls[16];
    int ballRadius = 15;

    // beyaz top
    balls[0] = (Ball){200.0f, 300.0f, 0.0f, 0.0f, ballRadius, {255, 248, 220, 255}, true};

    // renkli toplari dizme
    int index = 1;
    float startX = 550.0f;
    float startY = 300.0f;
    float rowSpacing = ballRadius * 1.732f;
    float colSpacing = ballRadius * 2.05f;

    for (int row = 0; row < 5; row++)
    {
        float currentX = startX + (row * rowSpacing);
        float currentStartY = startY - (row * ballRadius);

        for (int col = 0; col <= row; col++)
        {
            float currentY = currentStartY + (col * colSpacing);
            balls[index] = (Ball){currentX, currentY, 0.0f, 0.0f, ballRadius, {200, 90, 90, 255}, true};
            index++;
        }
    }

    bool isRunning = true;
    SDL_Event event;

    bool isDragging = false;
    int startMouseX = 0, startMouseY = 0;

    while (isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    // sadece beyaz top aktifse vurusa izin ver
                    if (balls[0].active)
                    {
                        isDragging = true;
                        startMouseX = event.button.x;
                        startMouseY = event.button.y;
                    }
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (event.button.button == SDL_BUTTON_LEFT && isDragging)
                {
                    isDragging = false;
                    int endMouseX = event.button.x;
                    int endMouseY = event.button.y;

                    balls[0].vx = (startMouseX - endMouseX) * 0.05f;
                    balls[0].vy = (startMouseY - endMouseY) * 0.05f;
                }
            }
        }

        // fizik
        for (int i = 0; i < 16; i++)
        {
            if (!balls[i].active)
                continue;

            balls[i].x += balls[i].vx;
            balls[i].y += balls[i].vy;
            balls[i].vx *= 0.99f;
            balls[i].vy *= 0.99f;

            if (balls[i].vx > -0.02f && balls[i].vx < 0.02f)
                balls[i].vx = 0.0f;
            if (balls[i].vy > -0.02f && balls[i].vy < 0.02f)
                balls[i].vy = 0.0f;

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

        // carpisma
        for (int i = 0; i < 16; i++)
        {
            if (!balls[i].active)
                continue;

            for (int j = i + 1; j < 16; j++)
            {
                if (!balls[j].active)
                    continue;

                float dx = balls[j].x - balls[i].x;
                float dy = balls[j].y - balls[i].y;
                float distance = sqrt(dx * dx + dy * dy);
                float minDistance = balls[i].radius + balls[j].radius;

                if (distance < minDistance)
                {
                    if (distance == 0.0f)
                    {
                        dx = 1.0f;
                        dy = 0.0f;
                        distance = 1.0f;
                    }

                    float overlap = minDistance - distance;
                    float nx = dx / distance;
                    float ny = dy / distance;

                    balls[i].x -= nx * (overlap / 2.0f);
                    balls[i].y -= ny * (overlap / 2.0f);
                    balls[j].x += nx * (overlap / 2.0f);
                    balls[j].y += ny * (overlap / 2.0f);

                    float tx = -ny;
                    float ty = nx;

                    float dpNorm1 = balls[i].vx * nx + balls[i].vy * ny;
                    float dpTang1 = balls[i].vx * tx + balls[i].vy * ty;
                    float dpNorm2 = balls[j].vx * nx + balls[j].vy * ny;
                    float dpTang2 = balls[j].vx * tx + balls[j].vy * ty;

                    balls[i].vx = (tx * dpTang1) + (nx * dpNorm2);
                    balls[i].vy = (ty * dpTang1) + (ny * dpNorm2);
                    balls[j].vx = (tx * dpTang2) + (nx * dpNorm1);
                    balls[j].vy = (ty * dpTang2) + (ny * dpNorm1);
                }
            }
        }

        // delik kontrol
        for (int i = 0; i < 16; i++)
        {
            if (!balls[i].active)
                continue;

            for (int p = 0; p < 6; p++)
            {
                float dx = balls[i].x - pockets[p].x;
                float dy = balls[i].y - pockets[p].y;
                float dist = sqrt(dx * dx + dy * dy);

                if (dist < pockets[p].radius)
                {
                    if (i == 0)
                    {
                        balls[0].x = 200.0f;
                        balls[0].y = 300.0f;
                        balls[0].vx = 0.0f;
                        balls[0].vy = 0.0f;
                    }
                    else
                    {
                        balls[i].active = false;
                    }
                }
            }
        }

        // cizim motoru
        SDL_SetRenderDrawColor(renderer, 85, 140, 110, 255);
        SDL_RenderClear(renderer);

        // delikler
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        for (int p = 0; p < 6; p++)
        {
            DrawFilledCircle(renderer, pockets[p].x, pockets[p].y, pockets[p].radius);
        }

        // dinamik istaka cizimi (trigonometri ile)
        if (isDragging)
        {
            int mx, my;
            SDL_GetMouseState(&mx, &my);

            float dx = mx - balls[0].x;
            float dy = my - balls[0].y;
            float angle = atan2(dy, dx);
            float pullDist = sqrt(dx * dx + dy * dy); // fareyi ne kadar cektik

            // istaka uzunlugu ve toptan uzakligi (geriye cekme hissi)
            float cueLength = 120.0f;
            float offset = balls[0].radius + 10.0f + (pullDist * 0.3f);

            // istakanin ucu ve arkasi
            float startX = balls[0].x + cos(angle) * offset;
            float startY = balls[0].y + sin(angle) * offset;
            float endX = balls[0].x + cos(angle) * (offset + cueLength);
            float endY = balls[0].y + sin(angle) * (offset + cueLength);

            // ahsap rengi (peru color)
            SDL_SetRenderDrawColor(renderer, 205, 133, 63, 255);

            // kalinlik vermek icin yan yana 5 cizgi ciziyoruz
            for (int w = -2; w <= 2; w++)
            {
                int px1 = startX + cos(angle + M_PI / 2) * w;
                int py1 = startY + sin(angle + M_PI / 2) * w;
                int px2 = endX + cos(angle + M_PI / 2) * w;
                int py2 = endY + sin(angle + M_PI / 2) * w;
                SDL_RenderDrawLine(renderer, px1, py1, px2, py2);
            }
        }

        // toplar
        for (int i = 0; i < 16; i++)
        {
            if (balls[i].active)
            {
                SDL_SetRenderDrawColor(renderer, balls[i].color.r, balls[i].color.g, balls[i].color.b, balls[i].color.a);
                DrawFilledCircle(renderer, (int)balls[i].x, (int)balls[i].y, balls[i].radius);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}