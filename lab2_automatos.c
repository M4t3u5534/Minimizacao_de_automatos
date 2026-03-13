#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

struct grupo {
	char efetivo;
	char atual;
};

struct resposta {
	int cont_t;
	char **transicoes;
	int cont_f;
	char *finais;
};

struct caminhos {
	char estado;
	struct grupo a;
	struct grupo b;
	bool final;
};

struct resposta minimiza_AFD(char **transicoes, int quant_transicoes, char *finais, int quant_finais){
	struct caminhos *tabela = NULL;
	int quant_caminhos = 0;

	struct caminhos **grupos = NULL;
	int *colunas = NULL;

	int indice;

	bool mudou = true;
	
	bool primeira = true;

	while (mudou) {
		mudou = false;
		if (primeira) {
			for (int i = 0; i < quant_transicoes; i++) {
				indice = -1;
				for (int j = 0; j < quant_caminhos; j++) {
					if (transicoes[i][0] == tabela[j].estado) {
						indice = j;
						break;
					}
				}
				if (indice == -1) {
					tabela = realloc(tabela, ++quant_caminhos * sizeof(struct caminhos));
					tabela[quant_caminhos - 1].estado = transicoes[i][0];
				}
				if (i % 2 == 0) tabela[quant_caminhos - 1].a.efetivo = transicoes[i][1];
				else tabela[quant_caminhos - 1].b.efetivo = transicoes[i][1];
			}

			for (int i = 0; i < quant_caminhos; i++) {
                        	for (int j = 0; j < quant_finais; j++) {
                                	if (tabela[i].estado == finais[j]) tabela[i].final = true;
					else tabela[i].final = false;
                                }
                        }

			for (int i = 0; i < quant_caminhos; i++){
				int j = 0;
                        	while (j < quant_finais) {
                        		if (tabela[i].a.efetivo == finais[j]) {
                                		tabela[i].a.atual = finais[0];
                                	        break;
                                	}
                                	j++;
                        	}
				if (j == quant_finais) {
					for (int k = 0; k < quant_caminhos; k++) {
						if (tabela[k].final == false) {
							tabela[i].a.atual = tabela[k].estado;
							break;
						}
					}
				}

				j = 0;
                                while (j < quant_finais) {
                                        if (tabela[i].b.efetivo == finais[j]) {
                                                tabela[i].b.atual = finais[0];
                                                break;
                                        }
                                        j++;
                                }
                                if (j == quant_finais) {
                                        for (int k = 0; k < quant_caminhos; k++) {
                                                if (tabela[k].final == false) {
                                                        tabela[i].b.atual = tabela[k].estado;
                                                        break;
                                                }
                                        }
                                }
			}

			grupos = malloc(quant_caminhos * sizeof(struct caminhos*));
			colunas = malloc(quant_caminhos * sizeof(int));
			for (int i = 0; i < quant_caminhos; i++) {
				colunas[i] = 0;
				grupos[i] = malloc(quant_caminhos * sizeof(struct caminhos));
			}

			for (int i = 0; i < quant_caminhos; i++) {
				if (tabela[i].final) grupos[0][colunas[0]++] = tabela[i];
				else grupos[1][colunas[1]++] = tabela[i];
			}
			mudou = true;
			primeira = false;
		} else {
			bool mudou_local;
			for (int l = 0; l < quant_caminhos; l++) {
				mudou_local = false;
				int coluna_nova = -1;
				if (colunas[l] > 1) {
					for (int i = colunas[l] - 1; i > 0; i--) {
						if (grupos[l][i].a.atual != grupos[l][0].a.atual || grupos[l][i].b.atual != grupos[l][0].b.atual) {
							mudou = true;
							mudou_local = true;
							for (int j = 0; j < quant_caminhos; j++) {
								if (colunas[j] == 0 || coluna_nova != -1) {
									if (coluna_nova == -1) coluna_nova = j;
									else j = coluna_nova;
									grupos[j][colunas[j]++] = grupos[l][i];
									grupos[l][i] = (struct caminhos){0};
									colunas[l]--;
									break;
								}
							}
						}
					}
				}
				if (mudou) {
					for (int i = 0; i < quant_caminhos; i++){
                        			for (int j = 0; j < colunas[i]; j++) {
							for (int k = 0; k < colunas[coluna_nova]; k++) {
								if (grupos[i][j].a.efetivo == grupos[coluna_nova][k].estado) grupos[i][j].a.atual = grupos[coluna_nova][0].estado;
								if (grupos[i][j].b.efetivo == grupos[coluna_nova][k].estado) grupos[i][j].b.atual = grupos[coluna_nova][0].estado;
							}
						}		
					}
				}
			}	
		}
	}
	struct resposta devolucao = {0, NULL, 0, NULL};
	int estados = 0;
	int fim = 0;
	
	for (int i = 0; i < quant_caminhos; i++) {
		if (colunas[i] > 0) {
			estados++;
			if (grupos[i][0].final) fim++;
		}
	}
	devolucao.transicoes = malloc(2 * estados * sizeof(char*));
	devolucao.finais = malloc(fim * sizeof(char));
	
	for (int i = 0; i < 2 * estados; i++){
		devolucao.transicoes[i] = malloc(2 * sizeof(char));
	}

	int j = 0;
	for (int i = 0; i < estados; i++){
		devolucao.transicoes[j][0] = grupos[i][0].estado;
		devolucao.transicoes[j][1] = grupos[i][0].a.atual;
		j++;

		devolucao.transicoes[j][0] = grupos[i][0].estado;
                devolucao.transicoes[j][1] = grupos[i][0].b.atual;
		j++;
	}
	
	int quant = 0;
	for (int i = 0; i < quant_caminhos && quant < fim; i++) {
                if (grupos[i][0].final) {
			devolucao.finais[quant] = grupos[i][0].estado;
			quant++;
		}
        }

	devolucao.cont_t = 2 * estados;
	devolucao.cont_f = fim;

	return devolucao;
}

int main(){
        FILE *file, *file2;
        char buffer[255];

        file = fopen("exemplo.txt", "r");

        if (file == NULL) {
                printf("Erro ao abrir o arquivo!\n");
                return 1;
        }

	int tam_alfabeto = 1;
        int letras = 0;
        char *alfabeto = NULL;

        int tam_estados = 1;
        int quant_estados = 0;
        char *estados = NULL;

        char inicio;

        int tam_finais = 1;
        int quant_finais = 0;
        char *finais = NULL;

        int quant_transicoes = 0;
        char **transicoes = NULL;

	while (fgets(buffer, 255, file) != NULL) {
		if (buffer[0] == 'A') {
                        int i = 2;
                        while (buffer[i] != '\0' && buffer[i] != '\n') {
                                if (!isspace(buffer[i])) {
                                        alfabeto = realloc(alfabeto, ++tam_alfabeto * sizeof(char));
                                        alfabeto[letras] = buffer[i];
                                        letras++;
                                        i++;
                                } else i++;
                        }
                } else if (buffer[0] == 'Q') {
                        int i = 3;
                        while (buffer[i] != '\0' && buffer[i] != '\n') {
                                if (isdigit(buffer[i])) {
                                        estados = realloc(estados, ++tam_estados * sizeof(char));
                                        estados[quant_estados] = buffer[i];
                                        quant_estados++;
                                        i++;
                                } else i++;
                        }
                } else if (buffer[0] == 'q') {
                        for (int i = 0; buffer[3] != estados[i] && i <= quant_estados; i++)
                        if (i == quant_estados) {
                                printf("Estado inicial invalido!\n");
                                return 0;
                        }
                        inicio = buffer[3];
                } else if (buffer[0] == 'F') {
                        int i = 3;
                        while (buffer[i] != '\0' && buffer[i] != '\n') {
                                if (isdigit(buffer[i])) {
                                        for (int j = 0; buffer[i] != estados[j] && j <= quant_estados; j++)
                                        if (j == quant_estados) {
                                                printf("Estado final invalido!\n");
                                                return 0;
                                        }
                                        finais = realloc(finais, ++tam_finais * sizeof(char));
                                        finais[quant_finais] = buffer[i];
                                        quant_finais++;
                                        i++;
                                } else i++;
                        }
                } else if (buffer[0] == 'T') {
                        if (quant_transicoes == 0) {
                                transicoes = realloc(transicoes, (letras * quant_estados) * sizeof(char *));
                                for (int i = 0; i < letras * quant_estados; i++) transicoes[i] = malloc(2 * sizeof(char));
                        }
                        bool valido = true;
                        for (int i = 0; buffer[5] != alfabeto[i] && i < letras; i++)
                        if (i == letras) valido = false;

                        for (int j = 0; buffer[3] != estados[j] && j < quant_estados; j++)
                        if (j == quant_estados) valido = false;

			for (int k = 0; buffer[8] != estados[k] && k < quant_estados; k++)
                        if (k == quant_estados) valido = false;

                        if (!valido) {
                                printf("Transicao invalida!\n");
                                return 0;
                        }
                        transicoes[quant_transicoes][0] = buffer[3];
                        transicoes[quant_transicoes][1] = buffer[8];
                        quant_transicoes++;
                }
	}
	struct resposta resposta = minimiza_AFD(transicoes, quant_transicoes, finais, quant_finais);

        char novos_nomes[256];
	for (int i = 0; i < 256; i++) novos_nomes[i] = -1;
	int mapeados = 0;

	for (int i = 0; i < resposta.cont_t; i+=2) {
		unsigned char estado_original = (unsigned char)resposta.transicoes[i][0];
		if (novos_nomes[estado_original] == -1) novos_nomes[estado_original] = mapeados++;
	}

	file2 = fopen("resposta.txt", "w");

        if (file2 == NULL) {
        	printf("Erro ao criar arquivo!\n");
                return 1;
        }

        fprintf(file2, "# Automato Finito Determinístico Minimizado\n\n");

        printf("escrevendo o alfabeto!\n");

        fprintf(file2, "# Alfabeto:\n");
        fprintf(file2, "A ");
        for (int i = 0; i < letras; i++) fprintf(file2, "%c ", alfabeto[i]);
        fprintf(file2, "\n\n");

        printf("escrevendo os estados!\n");

        fprintf(file2, "# Estados:\n");
        fprintf(file2, "Q ");
        for (int i = 0; i < resposta.cont_t; i+=2) fprintf(file2, "s%d ", novos_nomes[(unsigned char)resposta.transicoes[i][0]]);
        fprintf(file2, "\n\n");
        fprintf(file2, "# Estado Inicial:\n");
        fprintf(file2, "q s%d\n\n", novos_nomes[(unsigned char)inicio]);
        fprintf(file2, "# Estados Finais:\n");
        fprintf(file2, "F ");
        for (int i = 0; i < resposta.cont_f; i++) fprintf(file2, "s%d ", novos_nomes[(unsigned char)resposta.finais[i]]);
        	fprintf(file2, "\n\n");

        printf("escrevendo as transicoes!\n");

        fprintf(file2, "# Transições\n");
        for (int i = 0; i < resposta.cont_t; i+=2) {
		int de = novos_nomes[(unsigned char)resposta.transicoes[i][0]];
    		int le_a = novos_nomes[(unsigned char)resposta.transicoes[i][1]];
    		int le_b = novos_nomes[(unsigned char)resposta.transicoes[i+1][1]];

        	fprintf(file2, "T s%d %c s%d\n", de, alfabeto[0], le_a);
        	fprintf(file2, "T s%d %c s%d\n", de, alfabeto[1], le_b);
        }

        printf("resposta concluida!\n");

        fclose(file);
	fclose(file2);
	return 0;
}
