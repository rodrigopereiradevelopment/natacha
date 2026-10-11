#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>

struct ImpactoItem {
    std::string arquivo;
    size_t nivel_profundidade;
    std::vector<std::string> caminho;
};

class AnalisadorImpacto {
private:
    std::unordered_map<std::string, std::vector<std::string>> grafo;

public:
    void adicionarDependente(std::string_view base, std::string_view dependente) {
        grafo[std::string(base)].push_back(std::string(dependente));
    }

    void rastrearEstrago(std::string_view arquivo_alterado) const {
        std::string inicio(arquivo_alterado);
        std::unordered_set<std::string> visitados;
        std::queue<std::pair<std::string, std::vector<std::string>>> fila;
        std::vector<ImpactoItem> relatorio;

        visitados.insert(inicio);
        fila.push({inicio, {inicio}});

        while (!fila.empty()) {
            auto [atual, caminho] = fila.front();
            fila.pop();

            auto it = grafo.find(atual);
            if (it == grafo.end()) continue;

            for (const auto& dependente : it->second) {
                if (visitados.count(dependente)) continue;
                visitados.insert(dependente);
                std::vector<std::string> novo_caminho = caminho;
                novo_caminho.push_back(dependente);
                relatorio.push_back({
                    dependente,
                    novo_caminho.size() - 1,
                    novo_caminho
                });
                fila.push({dependente, novo_caminho});
            }
        }

        std::sort(relatorio.begin(), relatorio.end(),
            [](const ImpactoItem& a, const ImpactoItem& b) {
                return a.nivel_profundidade < b.nivel_profundidade;
            });

        std::cout << "\n--- [Natacha Agente: Analise de Impacto] ---\n";
        if (relatorio.empty()) {
            std::cout << "Alteracao segura. Nada depende de: " << arquivo_alterado << "\n";
            return;
        }

        std::cout << "ATENCAO: mexer em '" << arquivo_alterado
                  << "' afeta " << relatorio.size() << " arquivo(s):\n\n";

        for (const auto& item : relatorio) {
            std::cout << "  [Nivel " << item.nivel_profundidade << "] " << item.arquivo << "\n";
            std::cout << "    Caminho: ";
            for (size_t i = 0; i < item.caminho.size(); ++i) {
                std::cout << item.caminho[i];
                if (i + 1 < item.caminho.size()) std::cout << " -> ";
            }
            std::cout << "\n";
        }
    }
};

int main() {
    AnalisadorImpacto natacha;

    natacha.adicionarDependente("banco.ts", "mesas.service.ts");
    natacha.adicionarDependente("banco.ts", "comandas.service.ts");
    natacha.adicionarDependente("mesas.service.ts", "mapa-mesas.component.ts");
    natacha.adicionarDependente("mapa-mesas.component.ts", "banco.ts");
    natacha.adicionarDependente("auth.guard.ts", "main.ts");

    natacha.rastrearEstrago("banco.ts");
    natacha.rastrearEstrago("auth.guard.ts");

    return 0;
}
