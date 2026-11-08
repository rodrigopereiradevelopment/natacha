#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <vector>

using namespace std;

int main() {
    int largura = 64, altura = 64;
    // A "tela" em branco (nossos vetores/tensores)
    vector<vector<int>> tela(altura, vector<int>(largura, 255)); 

    // Gerando a imagem proceduralmente
    // A IA usa "seed" para a aleatoriedade, igual aos nossos treinos!
    mt19937 gen(42); 
    uniform_int_distribution<> dis(10, 50);

    // Desenhando "formas" baseadas em vetores matemáticos
    for (int i = 0; i < 15; ++i) {
        int cx = dis(gen); // Centro X
        int cy = dis(gen); // Centro Y
        int r = dis(gen) / 2; // Raio

        // Equação do círculo: (x - cx)^2 + (y - cy)^2 = r^2
        for (int y = 0; y < altura; ++y) {
            for (int x = 0; x < largura; ++x) {
                if ((x - cx)*(x - cx) + (y - cy)*(y - cy) < r*r) {
                    tela[y][x] = 0; // Pinta de preto
                }
            }
        }
    }

    // Salvando em formato PPM (imagem real para você abrir no PC)
    ofstream out("desenho_natacha.ppm");
    out << "P3\n" << largura << " " << altura << "\n255\n";
    for (int y = 0; y < altura; ++y) {
        for (int x = 0; x < largura; ++x) {
            out << tela[y][x] << " " << tela[y][x] << " " << tela[y][x] << "\n";
        }
    }
    out.close();
    
    cout << "Imagem gerada! Abra o arquivo 'desenho_natacha.ppm' no PC." << endl;
    return 0;
}