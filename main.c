#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Para usar strings
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

// Um pixel Pixel (24 bits)
typedef struct
{
    unsigned char r, g, b;
} Pixel;

// Uma imagem Pixel
typedef struct
{
    int width, height;
    int channels;
    Pixel *pixels;
} Img;

// As 2 imagens
Img in, out;

// Protótipos
void load(char *name, Img *pic);
void draw_line(int width, int height, Pixel img[][width], int x0, int y0, int x1, int y1, Pixel color, int thickness);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("forensics [origem]\n");
        exit(1);
    }

    // Carrega a imagem original
    load(argv[1], &in);

    // Exibe as dimensões na tela, para conferência
    printf("Origem   : %s %d x %d\n", argv[1], in.width, in.height);

    printf("Processando...\n");

    // 1. Prepare the output image as a direct copy of the input
    int tam = in.width * in.height;
    out = in;
    out.pixels = malloc(tam * sizeof(Pixel));
    
    // Copy all original pixels to 'out' immediately
    memcpy(out.pixels, in.pixels, tam * sizeof(Pixel));

    // 2. Re-map the 2D matrix pointers
    // Note: Use [in.width] for both dimensions to match your draw_line signature
    Pixel (*pin)[in.width] = (Pixel(*)[in.width]) in.pixels;
    Pixel (*pout)[in.width] = (Pixel(*)[in.width]) out.pixels;

    // 3. Define detection parameters
    Pixel red = {255, 0, 0};
    int blockSize = 8;
    int step = 4;         
    int threshold = 10; 
    int minVariance = 4000; // Increased to help filter background
    int minDist = 60;      // Ensure the clone is far enough away

    //carros: int threshold = 50; int minVariance = 3500;
    //futebol: int threshold = 32; int minVariance = 1200;
    //spinebuster: int threshold = 10; int minVariance = 4000;

    printf("Analyzing high-detail regions for clones...\n");

    for (int y1 = 0; y1 <= in.height - blockSize; y1 += step) {
        for (int x1 = 0; x1 <= in.width - blockSize; x1 += step) {
            
            // --- STEP A: Calculate Variance for Block 1 ---
            // If the block is too uniform (like concrete), skip it.
            long sum = 0, sqSum = 0;
            for(int i=0; i<blockSize; i++) {
                for(int j=0; j<blockSize; j++) {
                    int gray = (pin[y1+i][x1+j].r + pin[y1+i][x1+j].g + pin[y1+i][x1+j].b) / 3;
                    sum += gray;
                    sqSum += gray * gray;
                }
            }
            double mean = (double)sum / (blockSize * blockSize);
            double variance = ((double)sqSum / (blockSize * blockSize)) - (mean * mean);

            if (variance < minVariance) continue; // Skip "simple" textures

            // --- STEP B: Compare with other high-detail blocks ---
            for (int y2 = 0; y2 <= in.height - blockSize; y2 += step) {
                for (int x2 = 0; x2 <= in.width - blockSize; x2 += step) {
                    
                    // Skip if the blocks are the same or too close (Natural similarity)
                    int distance = abs(x1 - x2) + abs(y1 - y2);
                    if (distance < minDist) continue;

                    long diff = 0;
                    int match = 1;

                    for (int i = 0; i < blockSize && match; i++) {
                        for (int j = 0; j < blockSize && match; j++) {
                            Pixel p1 = pin[y1 + i][x1 + j];
                            Pixel p2 = pin[y2 + i][x2 + j];
                            diff += abs(p1.r - p2.r) + abs(p1.g - p2.g) + abs(p1.b - p2.b);
                            if (diff > (threshold * blockSize * blockSize)) match = 0;
                        }
                    }

                    if (match) {
                        draw_line(out.width, out.height, pout, x1+(blockSize/2), y1+(blockSize/2), x2+(blockSize/2), y2+(blockSize/2), red, 1);
                    }
                }
            }
        }
    }

    // NÃO ALTERAR A PARTIR DAQUI!

    // Grava a imagem como JPEG para registro
    stbi_write_jpg("saida.jpg", out.width, out.height, 3, pout, 90);

    free(in.pixels);
    free(out.pixels);
}

void load(char *name, Img *pic)
{
    pic->pixels = (Pixel *)stbi_load(name, &pic->width, &pic->height, &pic->channels, 0);
    if (!pic->pixels)
    {
        printf("STB loading error\n");
        exit(1);
    }
    printf("Load: %d x %d x %d\n", pic->width, pic->height, pic->channels);
    // Exibe os 16 primeiros pixels (teste)
    for (int i = 0; i < 16; i++)
    {
        printf("[%02X %02X %02X] ", pic->pixels[i].r, pic->pixels[i].g, pic->pixels[i].b);
    }
    printf("\n");
}

// Algoritmo de Bresenham para desenhar uma linha em uma matriz de pixels
void draw_line(int width, int height, Pixel img[][width], int x0, int y0, int x1, int y1, Pixel color, int thickness) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    int half = thickness / 2;
    while (1) {
        // Draw a square of size thickness x thickness centered at (x0, y0)
        for (int i = -half; i <= half; i++) {
            for (int j = -half; j <= half; j++) {
                int xi = x0 + i, yj = y0 + j;
                if (xi >= 0 && xi < width && yj >= 0 && yj < height)
                    img[yj][xi] = color;
            }
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)  {
            err += dx;
            y0 += sy;
        }
    }
}
