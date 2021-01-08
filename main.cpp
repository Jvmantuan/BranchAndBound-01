#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <new>
#include "main.hpp"
#include <chrono>
#include <math.h>
#include <omp.h>
using namespace std;

int main() {

    cout << "*************************************************\n\n";
    cout << "Branch-And-Bound para o Problema da Mochila 0-1\n\n";
    cout << "*************************************************\n\n";
    ifstream entrada;
    string linha, c;

    entrada.open("instancias/entrada01.txt"); // Abre o arquivo

    // Se der erro ao abrir o arquivo, envia uma mensagem de erro
    if (!entrada.is_open()) {
        cerr << "Erro ao abrir o arquivo!";
        return false;
    }

    getline(entrada, linha); // Le o numero de variaveis
    int num_variaveis = to_int(linha); //Converte o valor lido para inteiro

    getline(entrada, linha); // Le o valor do lado direito da restricao
    int b = to_int(linha);

    double* restricao = new(nothrow) double[num_variaveis]; // vetor das restricoes
    double* obj = new(nothrow) double[num_variaveis]; // vetor da funcao objetivo

    // Se der erro de alocacao, retorna false
    if (restricao == nullptr) {
        cerr << "Erro de alocacao" << endl;
        return false;
    }

    // Le a linha da f.o e passa para um vetor
    getline(entrada, linha);
    int j = 0; c.clear();
    for (int i = 0; i < linha.length(); i++) {
        obj[j] = 0;
        if (linha[i] != ' ')
            c += linha[i];
        else {
            obj[j] = to_int(c);
            c.clear();
            j++;
        }
    }
    obj[j] = to_int(c); // Passa o ultimo valor do arquivo para a matriz

    // Passa os coeficientes da restricao
    getline(entrada, linha);
    j = 0; c.clear();
    for (int i = 0; i < linha.length(); i++) {
        restricao[j] = 0;
        if (linha[i] != ' ')
            c += linha[i];
        else {
            restricao[j] = to_int(c);
            c.clear();
            j++;
        }
    }
    restricao[j] = to_int(c); // Passa o ultimo valor do arquivo para a matriz

    double* vetor_razao = razao(obj, restricao, num_variaveis);

    // Inicializa a pilha
    p.inicializar();

    // Estamos usando -1 para dizer que as variaveis do nosso problema não tem um valor fixado
    double* var = new(nothrow) double[num_variaveis];
    for (int i = 0; i < num_variaveis; i++)
        var[i] = -1;

    // Empilha a relaxacao linear e adiciona aos nos abertos
    sol relaxacao_linear = relaxacao(obj, restricao, num_variaveis, b, var);
    p.empilhar(relaxacao_linear);
    relatoriof.nos_abertos++;	

    cout << "Numero de variaveis do problema: " << num_variaveis << "\n\n";

    auto start = chrono::steady_clock::now();
    
    #pragma omp parallel
    {
        BranchBound(relaxacao_linear, obj, restricao, num_variaveis, b, var);
    }

    auto end = chrono::steady_clock::now();

    p.terminar();

    // Se houver solução inteira, mostra os resultados, se nao, avisa que não há solução inteira
    if (solint.z != 0) {

        cout << "Numero de nos avaliados: " << relatoriof.nos_avaliados << "\n";

        cout << "Tempo decorrido: "
            << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1000000000.0 << " segundos\n";

        cout << "Gap: " << relatoriof.gap << "%\n";
        printar_solucao(solint, num_variaveis);
    }
    else {
        cout << "Numero de nos avaliados: " << relatoriof.nos_avaliados << "\n";

        cout << "Tempo decorrido: "
            << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1000000000.0 << " segundos\n";
		
		if (relatoriof.nos_avaliados == MaxNos){
            cout << "Programa encerrado devido ao limite de nos.\n";
			cout << "\nNao foi possivel encontrar uma solucao inteira.\n";
        }
		else
			cout << "\nNao foi possivel encontrar uma solucao inteira.\n";
    }

    delete[] restricao;
    delete[] vetor_razao;
    delete[] obj;
    delete[] relaxacao_linear.var;
    delete[] solint.var;
}
