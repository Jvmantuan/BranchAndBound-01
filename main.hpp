#pragma once
#include <iostream>
#include <new>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <omp.h>

using namespace std;

const int MaxNos = 0;

typedef struct sol {
	double* var;
	double z = 0;
	int sol_inteira = 0;
	double peso = 0;
};

struct Pilha {
	sol* v;
	int ult, tam;

	// Inicializa a pilha com tamanho 1
	bool inicializar() {
		v = new(nothrow) sol[1];
		if (v == nullptr) return true;
		ult = -1;
		tam = 1;
		return false;
	}

	void terminar() {
		delete[] v;
	}

	// Retorna true se a pilha estiver vazia
	bool vazia() {
		return ult == -1;
	}

	// Aumenta/diminui o tamanho da pilha
	bool redimensionar(int novo_tam) {
		sol* w = new(nothrow) sol[novo_tam];

		if (w == nullptr) return true;

		for (int i = 0; i <= ult; i++)
			w[i] = v[i];

		delete[] v;

		v = w;
		tam = novo_tam;
		return false;
	}

	bool empilhar(sol e) {
		if (ult == tam - 1)
			if (redimensionar(tam * 2))
				return true;

		++ult;
		v[ult] = e;
		return false;
	}

	sol topo() {
		return v[ult];
	}

	/*
		Verifica se a solucao passada e viavel e retorna a melhor solução da pilha (que representa os nos abertos)
		Utilizada para atualizar o gap
	*/ 
	double melhor_limite(sol solucao, int b) {
		double maior = 0;

		for (int i = 0; i <= ult; i++) {
			if (v[i].peso > b)
				continue;
			if (v[i].z > maior) {
				maior = v[i].z;
				continue;
			}
		}
		return maior;
	}

	// Caso apos desempilhar um elemento, a pilha ficar com um tamanho 2 vezes maior do que o numero de elementos, 
	// diminuimos o tamanho da pilha pela metade.
	bool desempilhar() {
		--ult;
		if (!vazia() && 4 * (ult + 1) <= tam)
			if (redimensionar(tam / 2))
				return true;

		return false;
	}
};

// Struct que controla o relatorio
typedef struct relatorio {
	int gapzero = 0;
	int nos_avaliados = 0;
	int nos_abertos = 0;
	double gap = 0;
	int total_nos = 0;
};

// Infelizmente, devido à recursão, tivemos que criar algumas variaveis globais 
// para não ficarmos criando elas sempre que chamarmos o B&B
relatorio relatoriof;
sol solint;
Pilha p;

// Funcao que converte de string para int
int to_int(string str) {
	stringstream ss(str);
	int x = 0;
	ss >> x;
	return x;
}

// Funcao que retorna um vetor com a razao dos valores de dois vetores
double* razao(double* fo, double* restr, int tam) {
	double* v_razao = new(nothrow) double[tam];

	for (int i = 0; i < tam; i++) {
		v_razao[i] = fo[i] / restr[i];
	}

	return v_razao;
}

// Funcao que retorna o maior valor de um vetor
double maior_valor(double* v, int tam) {
	double max = v[0];

	for (int i = 0; i < tam; i++)
		if (v[i] > max) max = v[i];

	return max;
}

// Funcao que resolve relaxacao
sol relaxacao(double* fo, double* restr, int tam, double b, double* var_fixadas) {

	sol sol_relaxado;
	sol_relaxado.var = new(nothrow) double[tam]; // Vetor solucao

	double* v_prioridade = razao(fo, restr, tam); // Calcula a prioridade de cada variavel

	double max = maior_valor(v_prioridade, tam); // Obtem a variavel com maior prioridade
	double peso = 0;

	for (int i = 0; i < tam; i++)
		sol_relaxado.var[i] = 0;

	// Caso haja alguma variavel fixada, muda o valor da variavel da solucao para o valor determinado e zera a sua prioridade
	for (int i = 0; i < tam; i++) {

		if (var_fixadas[i] == 1) {
			sol_relaxado.var[i] = 1;
			peso += restr[i] * sol_relaxado.var[i];

			v_prioridade[i] = 0;
			max = maior_valor(v_prioridade, tam);
		}
		if (var_fixadas[i] == 0) {
			sol_relaxado.var[i] = 0;
			
			v_prioridade[i] = 0;
			max = maior_valor(v_prioridade, tam);
		}
	}

	// Calcula o peso, e o valor de cada varivel de forma a satisfazer o valor de b
	#pragma omp parallel
	{
		for (int j = 0; j < tam; j++) {
			for (int i = 0; i < tam; i++) {
				if (max == 0)
					break;

				if (v_prioridade[i] == max && peso + restr[i] <= b) {
					sol_relaxado.var[i] = 1;
					peso += restr[i] * sol_relaxado.var[i];

					// Zero a prioridade da variavel xi, pois ela ja teve um valor atribuido
					v_prioridade[i] = 0;
					max = maior_valor(v_prioridade, tam);
				}
				else if (v_prioridade[i] == max && peso + restr[i] > b) {
					sol_relaxado.var[i] = abs((b - peso)) / restr[i];

					peso += restr[i] * sol_relaxado.var[i];

					v_prioridade[i] = 0;
					max = maior_valor(v_prioridade, tam);
				}
			}
		}
	}

	// Peso acumulado da solucao
	sol_relaxado.peso = peso;

	//Calcula o valor de z
	for (int i = 0; i < tam; i++) {
		sol_relaxado.z += sol_relaxado.var[i] * fo[i];
	}

	delete[] v_prioridade;
	return sol_relaxado;
}

// Funcao para printar a solucao
void printar_solucao(sol solucao, int tam) {
	cout << "Valor objetivo = " << solucao.z << endl;
	cout << "Variaveis: " << endl;
	for (int i = 0; i < tam; i++)
		if (solucao.var[i] == 1)
			cout << "x" << to_string(i + 1) << " = " << solucao.var[i] << "\n";
	cout << endl;
	cout << "Todas as outras variaveis possuem valor 0.\n";

	//cout << endl;
}

int c = 0;
// Funcao de Branch & Bound
sol BranchBound(sol solucao, double* fo, double* restr, int tam, double b, double* var_fixadas) {
	sol solucao1;
	sol solucao2;
	double soma = 0;
	int i, poda = 0;
	double* aux = new(nothrow) double[tam];

	// Para o programa quando MaxNos for atingido
	if(MaxNos > 0)
		if (relatoriof.nos_avaliados > MaxNos - 1) 
			poda = 5;

	if (poda == 5) {
		if (relatoriof.nos_abertos != 0) {
			p.desempilhar();
			return solint;
		}
	}

	relatoriof.nos_avaliados++;

	//Fixa a variavel xi em 1 quando xi for nao-inteira
	for (i = 0; i < tam; i++) {
		if (solucao.var[i] - floor(solucao.var[i]) != 0) {
			var_fixadas[i] = 1;
			break;
		}
	}

	 //Inviabilidade	
	if (solucao.peso > b) {
		poda = 1;
	}

	//Otimalidade
	else if (i == tam) {
		if (solucao.z > solint.z) {
			solint = solucao;
			poda = 2;
		}
		//Bound
		else {
			poda = 3;
		}
	}

	else {
		if (solint.z >= solucao.z) {
			poda = 4;
		}
	}

	// Printa o relatório
	if (c == 0) {
		cout << setw(9) << "Nos\n";
		cout << "Avaliados   Abertos   Melhor Inteiro   Melhor Limite        Gap" << "\n";
		c++;
	}

	// Calcula o melhor limite
	// Caso ele seja menor do que a melhor solucao conhecida, o substituimos pela melhor solucao conhecida
	double melhor_limite = p.melhor_limite(solucao, b);

	if (melhor_limite < solint.z) {
		p.desempilhar();
		p.empilhar(solint);
		melhor_limite = p.melhor_limite(solucao, b);
	}

	// Calcula o gap
	relatoriof.gap = abs(melhor_limite - solint.z) / solint.z * 100;

	// Aumenta sempre que a solucao atual for inteira, para mostrar no relatório
	if (relatoriof.gap == 0 && relatoriof.nos_avaliados != 1) {
		solint.sol_inteira++;
	}

	// Mostra uma linha no relatório
	if (relatoriof.nos_avaliados % 100 == 0 || solint.sol_inteira == 1 || poda == 2) {
		if (solint.sol_inteira <= 1) {
			if (poda == 2 || solint.sol_inteira == 1) {
				cout << "*";
				cout << setw(8) << relatoriof.nos_avaliados;
			}
			else {
				cout << setw(9) << relatoriof.nos_avaliados;
			}
			cout << setw(10) << relatoriof.nos_abertos;
			cout << setw(17) << solint.z;
			cout << setw(16) << melhor_limite;
			cout << setw(13) << relatoriof.gap << "%"
				<< "\n";
		}
	}

	if (relatoriof.gap == 0 && relatoriof.nos_avaliados != 1) {
		solint.sol_inteira++;
	}

	// Se houver alguma poda na solucao atual ou se for uma solucao fraca, desempilha essa solucao
	if (poda == 1 || poda == 2 || poda == 3 || poda == 4 || melhor_limite == solint.z) {
		relatoriof.nos_abertos--;
		p.desempilhar();
		return solucao;
	}

	// Correcao de um bug onde as variaveis não se mantinham fixadas até o final do Branch-and-Bound
	for (int k = 0; k < tam; k++) {
		aux[k] = var_fixadas[k];
		if ((solucao.var[k] == 0 || solucao.var[k] == 1) && var_fixadas[k] == 1)
			continue;
		else
			solucao.var[k] = -1;
	}

	// Ramifica a solucao atual e calcula a sua relaxacao, em seguida, empilha essas solucoes
	solucao.var[i] = 0;
	solucao1 = relaxacao(fo, restr, tam, b, solucao.var);
	
	solucao.var[i] = 1;
	solucao2 = relaxacao(fo, restr, tam, b, solucao.var);

	relatoriof.nos_abertos--;
	p.desempilhar();

	p.empilhar(solucao2);
	p.empilhar(solucao1);

	relatoriof.nos_abertos++;
	relatoriof.nos_abertos++;
	
	// Resolve o Branch-and-Bound para a variável com o valor de z mais promissor
	if (solucao1.z >= solucao2.z){
		BranchBound(solucao1, fo, restr, tam, b, var_fixadas);
		BranchBound(solucao2, fo, restr, tam, b, aux);
	}else {
		BranchBound(solucao2, fo, restr, tam, b, var_fixadas);
		BranchBound(solucao1, fo, restr, tam, b, aux);
	} 
	
	delete[] aux;
	return solint;
}
