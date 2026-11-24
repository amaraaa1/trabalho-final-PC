#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/*
    CALCULADORA DE EXPRESSOES MATEMATICAS
    Linguagem: C ANSI (C89/C90)

    Operacoes:
    +  Soma
    -  Subtracao
    *  Multiplicacao
    x  Multiplicacao
    /  Divisao
    %  Resto da divisao (inteiros)
    ^  Potenciacao
    !  Fatorial
    | | Valor absoluto
*/

/* Variaveis globais do interpretador */
const char *expressao;
int erro;
char mensagemErro[100];

/* Prototipos das funcoes */
double calcularExpressao(void);
double calcularTermo(void);
double calcularUnario(void);
double calcularPotencia(void);
double calcularPosfixo(void);
double calcularPrimario(void);

void ignorarEspacos(void);
void registrarErro(const char *mensagem);
double calcularFatorial(double numero);

/*
    FUNCAO: ignorarEspacos

    Ignora os espacos em branco existentes
    na expressao digitada pelo usuario.
*/
void ignorarEspacos(void)
{
    while (*expressao != '\0' &&
           isspace((unsigned char)*expressao))
    {
        expressao++;
    }
}

/*
    FUNCAO: registrarErro

    Registra o primeiro erro encontrado
    durante a interpretacao da expressao.
*/
void registrarErro(const char *mensagem)
{
    if (!erro)
    {
        erro = 1;
        strcpy(mensagemErro, mensagem);
    }
}

/*
    FUNCAO: calcularFatorial

    Calcula o fatorial de um numero inteiro
    nao negativo.

    Exemplo:
    5! = 120

    O limite de 170 evita estouro do double.
*/
double calcularFatorial(double numero)
{
    double resultado;
    int i;

    if (numero < 0 || floor(numero) != numero)
    {
        registrarErro("Fatorial exige um inteiro nao negativo.");
        return 0;
    }

    if (numero > 170)
    {
        registrarErro("Numero muito grande para calcular fatorial.");
        return 0;
    }

    resultado = 1;

    for (i = 1; i <= (int)numero; i++)
    {
        resultado = resultado * i;
    }

    return resultado;
}

/*
    FUNCAO: calcularPrimario

    Interpreta os elementos basicos da expressao:

    - Numeros reais;
    - Parenteses;
    - Colchetes;
    - Chaves;
    - Valor absoluto.

    Utiliza recursividade para resolver
    expressoes dentro dos delimitadores.
*/
double calcularPrimario(void)
{
    double valor;
    char fechamento;
    int barras;
    char *fim;

    ignorarEspacos();

    if (erro)
        return 0;

    if (*expressao == '\0')
    {
        registrarErro("Expressao incompleta.");
        return 0;
    }

    /* Verifica os delimitadores de agrupamento */
    if (*expressao == '(' ||
        *expressao == '[' ||
        *expressao == '{')
    {
        if (*expressao == '(')
            fechamento = ')';
        else if (*expressao == '[')
            fechamento = ']';
        else
            fechamento = '}';

        expressao++;

        valor = calcularExpressao();

        ignorarEspacos();

        if (*expressao != fechamento)
        {
            registrarErro("Delimitadores incorretos ou nao fechados.");
            return 0;
        }

        expressao++;

        return valor;
    }

    /* Verifica o valor absoluto */
    if (*expressao == '|')
    {
        barras = 1;
        expressao++;

        if (*expressao == '|')
        {
            barras = 2;
            expressao++;
        }

        valor = calcularExpressao();

        ignorarEspacos();

        if (*expressao != '|')
        {
            registrarErro("Valor absoluto nao foi fechado.");
            return 0;
        }

        expressao++;

        if (barras == 2)
        {
            if (*expressao != '|')
            {
                registrarErro("Valor absoluto nao foi fechado corretamente.");
                return 0;
            }

            expressao++;
        }

        return fabs(valor);
    }

    /* Verifica e converte numeros reais */
    if (isdigit((unsigned char)*expressao) ||
        *expressao == '.')
    {
        valor = strtod(expressao, &fim);

        if (fim == expressao)
        {
            registrarErro("Numero invalido.");
            return 0;
        }

        expressao = fim;

        return valor;
    }

    registrarErro("Elemento invalido na expressao.");

    return 0;
}

/*
    FUNCAO: calcularPosfixo

    Realiza o calculo do fatorial (!).

    Exemplos:
    5! = 120
    3! = 6
*/
double calcularPosfixo(void)
{
    double valor;

    valor = calcularPrimario();

    ignorarEspacos();

    while (*expressao == '!' && !erro)
    {
        expressao++;

        valor = calcularFatorial(valor);

        ignorarEspacos();
    }

    return valor;
}

/*
    FUNCAO: calcularPotencia

    Realiza a potenciacao utilizando o operador ^.

    Exemplo:
    2^3 = 8

    A potencia e associativa a direita:
    2^3^2 = 512.
*/
double calcularPotencia(void)
{
    double base;
    double expoente;
    double resultado;

    base = calcularPosfixo();

    ignorarEspacos();

    if (*expressao == '^' && !erro)
    {
        expressao++;

        expoente = calcularUnario();

        resultado = pow(base, expoente);

        /* Verifica resultado matematico invalido */
        if (resultado != resultado)
        {
            registrarErro("Erro ao calcular a potencia.");
            return 0;
        }

        return resultado;
    }

    return base;
}

/*
    FUNCAO: calcularUnario

    Trata os sinais positivo e negativo.

    Exemplos:
    -5
    +10
    -(2+3)
*/
double calcularUnario(void)
{
    ignorarEspacos();

    if (*expressao == '+')
    {
        expressao++;
        return calcularUnario();
    }

    if (*expressao == '-')
    {
        expressao++;
        return -calcularUnario();
    }

    return calcularPotencia();
}

/*
    FUNCAO: calcularTermo

    Realiza multiplicacao, divisao e modulo.

    O modulo (%) aceita somente numeros inteiros.

    Exemplos:
    10*2 = 20
    10/2 = 5
    10%3 = 1
*/

/*
    FUNCAO: calcularTermo

    Realiza as operacoes de multiplicacao,
    divisao e modulo.

    Tambem reconhece multiplicacao implicita,
    quando nao existe o operador * entre os elementos.

    Exemplos:
    10*2 = 20
    10/2 = 5
    10%3 = 1
    5(2+3) = 25
    5{2+3} = 25
    2[3+4] = 14
    (2+3)(4+5) = 45
*/
double calcularTermo(void)
{
    double valor;
    double proximo;
    char operador;

    valor = calcularUnario();

    ignorarEspacos();

    while (!erro)
    {
        ignorarEspacos();

        if (*expressao == '*' ||
            *expressao == '/' ||
            *expressao == '%' ||
            *expressao == 'x' ||
            *expressao == 'X')
        {
            operador = *expressao;
            expressao++;
        }

        /*Verifica se existe multiplicacao implicita.*/
        
        else if (*expressao == '(' ||
         		*expressao == '[' ||
         		*expressao == '{' ||
         		*expressao == '|' ||
        isdigit((unsigned char)*expressao) ||
        *expressao == '.')
		{
    	operador = '*';
		}

        /*
           Se nao encontrar nenhum operador,
           encerra o calculo do termo.
        */
        else
        {
            break;
        }

        /* Calcula o proximo elemento */
        proximo = calcularUnario();

        if (erro)
            return 0;

        /* Realiza a operacao identificada */
        if (operador == '*' ||
            operador == 'x' ||
            operador == 'X')
        {
            valor = valor * proximo;
        }
        else if (operador == '/')
        {
            if (proximo == 0)
            {
                registrarErro("Nao e permitido dividir por zero.");
                return 0;
            }

            valor = valor / proximo;
        }
        else if (operador == '%')
        {
            if (floor(valor) != valor ||
                floor(proximo) != proximo)
            {
                registrarErro("Modulo exige numeros inteiros.");
                return 0;
            }

            if (proximo == 0)
            {
                registrarErro("Modulo por zero nao e permitido.");
                return 0;
            }

            valor = fmod(valor, proximo);
        }
    }

    return valor;
}


/*
    FUNCAO: calcularExpressao

    Realiza as operacoes de soma e subtracao.

    Tambem chama as outras funcoes para garantir
    que a prioridade matematica seja respeitada.

    Exemplos:
    10+5 = 15
    10-5 = 5
    2+3*4 = 14
*/
double calcularExpressao(void)
{
    double valor;
    double proximo;
    char operador;

    valor = calcularTermo();

    ignorarEspacos();

    while ((*expressao == '+' ||
            *expressao == '-') && !erro)
    {
        operador = *expressao;
        expressao++;

        proximo = calcularTermo();

        if (erro)
            return 0;

        if (operador == '+')
        {
            valor = valor + proximo;
        }
        else
        {
            valor = valor - proximo;
        }

        ignorarEspacos();
    }

    return valor;
}

/*
    FUNCAO PRINCIPAL: main

    Responsavel pela interacao com o usuario.

    - Recebe a expressao matematica;
    - Inicializa as variaveis;
    - Chama a funcao de calculo;
    - Mostra o resultado ou o erro;
    - Pergunta se o usuario deseja continuar.
*/
int main(void)
{
    char entrada[512];
    char resposta;
    double resultado;

    printf("========================================\n");
    printf("       CALCULADORA DE EXPRESSOES\n");
    printf("========================================\n");

    printf("\nOperadores disponiveis:\n");
    printf("+  Soma\n");
    printf("-  Subtracao\n");
    printf("*  Multiplicacao\n");
    printf("x  Multiplicacao\n");
    printf("/  Divisao\n");
    printf("%%  Modulo (inteiros)\n");
    printf("^  Potenciacao\n");
    printf("!  Fatorial\n");
    printf("| | Valor absoluto\n");

    printf("\nDelimitadores: (), [] e {}\n");

    do
    {
        printf("\nDigite uma expressao matematica:\n");

        if (fgets(entrada, sizeof(entrada), stdin) == NULL)
        {
            printf("\nErro na leitura da expressao.\n");
            return 1;
        }

        /* Inicializa o interpretador */
        expressao = entrada;
        erro = 0;
        mensagemErro[0] = '\0';

        /* Calcula a expressao */
        resultado = calcularExpressao();

        ignorarEspacos();

        /* Verifica se sobraram caracteres */
        if (!erro && *expressao != '\0')
        {
            registrarErro("Caracteres extras ou operador invalido.");
        }

        /* Mostra o resultado ou a mensagem de erro */
        if (erro)
        {
            printf("\nERRO: %s\n", mensagemErro);
        }
        else
        {
            printf("\nResultado: %.10g\n", resultado);
        }

 
/* Pergunta se o usuario deseja continuar */
while (1)
{
    char entradaResposta[100];
    int i;

    printf("\nDeseja realizar outro calculo? (S/N ou SIM/NAO): ");

    if (fgets(entradaResposta, sizeof(entradaResposta), stdin) == NULL)
    {
        return 0;
    }

    /* Converte a resposta para letras minusculas */
    for (i = 0; entradaResposta[i] != '\0'; i++)
    {
        if (entradaResposta[i] == '\n')
        {
            entradaResposta[i] = '\0';
            break;
        }

        entradaResposta[i] =
            (char)tolower((unsigned char)entradaResposta[i]);
    }

    /* Aceita S ou SIM */
    if (strcmp(entradaResposta, "s") == 0 ||
        strcmp(entradaResposta, "sim") == 0)
    {
        resposta = 'S';
        break;
    }

    /* Aceita N ou NAO */
    else if (strcmp(entradaResposta, "n") == 0 ||
             strcmp(entradaResposta, "nao") == 0)
    {
        resposta = 'N';
        break;
    }

    /* Qualquer outra resposta e invalida */
    else
    {
        printf("\nAlternativa invalida! Tente novamente.\n");
    }
}

} while (resposta == 'S');


    printf("\nCalculadora encerrada. Obrigado!\n");

    return 0;
}

