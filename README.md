# **_Branch-and-bound_ para o problema da mochila 0-1**

Programa em C/C++ com o algoritmo do _Branch-and-Bound_ para resolver o problema de mochila 0-1.

## Usando instâncias próprias

Para testar suas próprias instâncias crie um arquivo de texto que siga o seguinte formato:

- Na primeira linha, deve haver um número que representa o número de variáveis do problema, chamaremos esse número de _n_;     
- Na segunda linha, deve haver um número que representa o lado direito da restrição (valor b);
- Na terceira linha, deve haver _n_ números, representando os coeficientes da função objetivo;
- Na quarta linha, deve haver _n_ números, representando os coeficientes da restrição;
- Não pode haver espaços depois do ultimo número de cada linha, nem uma linha em branco no final do arquivo;

Após de terminar o .txt, chame-o de instancia0m.txt, onde _m_ é um número qualquer, por ex: 16, e mova-o para a pasta _instancias_, dentro da pasta do projeto.

No arquivo main.cpp altere a linha: ``entrada.open("instancias/entrada01.txt");`` para ``entrada.open("instancias/entrada0m.txt");`` (troque m pelo numero que você escolheu anteriormente) e compile.

## Conferindo o resultado

Se por algum motivo, você estiver em dúvida sobre o resultado do programa, transforme o arquivo de texto que você criou em um arquivo de formato `.lp`, e modifique-o de acordo com as instâncias exemplo (observe que temos vários arquivos `.lp` na pasta `instancias`, tente deixar no mesmo formato). Faça um backup do .txt se quiser utilizá-lo ainda.

Em seguida rode este arquivo `.lp` em um solver CPLEX ou em um servidor de otimização, como o https://neos-server.org/neos/solvers/milp:CPLEX/LP.html. Veja as instruções no site para proceder de forma correta.
