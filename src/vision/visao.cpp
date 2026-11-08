#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;

// Filtro de Sobel (Detecção de bordas)
const int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
const int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

int main() {
    // 1. Criamos uma "imagem" de teste (uma matriz 5x5 em tons de cinza)
    // 0 = Preto, 255 = Branco
    vector<vector<int>> img = {
        {0,   0,   0,   0,   0},
        {0, 255, 255, 255,   0},
        {0, 255,   0, 255,   0},
        {0, 255, 255, 255,   0},
        {0,   0,   0,   0,   0}
    };

    int altura = img.size();
    int largura = img[0].size();
    vector<vector<int>> resultado(altura, vector<int>(largura, 0));

    // 2. A Matemática da Visão: Convolução
    for (int y = 1; y < altura - 1; ++y) {
        for (int x = 1; x < largura - 1; ++x) {
            int sumX = 0, sumY = 0;
            
            // Varre a matriz 3x3 ao redor do pixel
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    sumX += img[y + i][x + j] * Gx[i + 1][j + 1];
                    sumY += img[y + i][x + j] * Gy[i + 1][j + 1];
                }
            }
            
            // Calcula a magnitude do gradiente (a "força" da borda)
            int magnitude = sqrt(sumX * sumX + sumY * sumY);
            resultado[y][x] = min(255, magnitude);
        }
    }

    // 3. Imprimindo o resultado (as bordas detectadas)
    cout << "--- Imagem Original ---" << endl;
    for (auto& linha : img) { for (int p : linha) cout << (p > 128 ? "##" : ".."); cout << endl; }

    cout << "\n--- Bordas Detectadas (Visão) ---" << endl;
    for (auto& linha : resultado) { for (int p : linha) cout << (p > 128 ? "##" : ".."); cout << endl; }

    return 0;
}