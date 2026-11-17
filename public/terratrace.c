/* ====================================================================
 * TerraTrace-C - Modulo de borda para cadastro e triagem de lotes de
 * terras raras. Projeto Integrado Multidisciplinar (UNIP / ADS).
 *
 * Compilar: gcc -Wall -Wextra -pedantic -std=c99 -o terratrace terratrace.c
 * Executar: ./terratrace
 *
 * ATENCAO: os dados do modo demonstracao sao FICTICIOS.
 * ==================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_LOTES 500
#define TAM_TEXTO 40
#define TAM_LINHA 128
#define MAX_ELEM 20
#define ARQ_DADOS "lotes.dat"
#define ARQ_JSON "lotes.json"
#define ARQ_CSV "lotes.csv"
#define NUM(v) (sizeof (v) / sizeof (v)[0])
typedef struct {
    int id;
    char origem[TAM_TEXTO]; /* mina ou projeto de origem */
    char uf[3]; /* sigla da unidade federativa */
    char elemento[4]; /* simbolo do elemento principal */
    double massa_t; /* massa do concentrado (toneladas) */
    double teor_treo; /* teor de oxidos de TR totais (%) */
    int rastreado; /* 1 = possui certificado de origem */
    int licenca_ok; /* 1 = licenca ambiental regular */
    char destino[TAM_TEXTO]; /* comprador ou etapa seguinte */
} Lote;
typedef struct {
    Lote itens[MAX_LOTES];
    int n;
} Repositorio;
/* Listas de valores permitidos */
static const char *UFS[] = {
    "AC", "AL", "AP", "AM", "BA", "CE", "DF", "ES", "GO", "MA", "MT", "MS", "MG", "PA",
    "PB", "PR", "PE", "PI", "RJ", "RN", "RS", "RO", "RR", "SC", "SP", "SE", "TO"};
static const char *ELEMENTOS[] = { /* 15 lantanideos + Sc e Y */
    "La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm",
    "Yb", "Lu", "Sc", "Y"};
static const char *PESADOS[] = { /* de Gd a Lu, mais Y */
    "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Y"};
static int salvar(const Repositorio *r);
static void maiusculas(char *s)
{
    for (; *s; s++)
        *s = (char)toupper((unsigned char)*s);
}
static int igual_sem_caixa(const char *a, const char *b)
{
    for (; *a && *b; a++, b++)
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b))
            return 0;
    return *a == *b;
}
static int so_espacos(const char *p)
{
    for (; *p; p++)
        if (!isspace((unsigned char)*p))
            return 0;
    return 1;
}
static void aparar(char *s)
{
    char *ini = s, *fim;
    while (isspace((unsigned char)*ini)) ini++;
    memmove(s, ini, strlen(ini) + 1);
    fim = s + strlen(s);
    while (fim > s && isspace((unsigned char)fim[-1])) *--fim = '\0';
}
static int na_lista(const char *s, const char *lista[], size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
        if (igual_sem_caixa(s, lista[i]))
            return 1;
    return 0;
}
/* Retorna 1 = linha lida, 0 = fim da entrada, -1 = linha longa demais */
static int ler_linha(const char *rotulo, char *buf, size_t tam)
{
    int c;
    printf("%s", rotulo);
    if (fgets(buf, (int)tam, stdin) == NULL)
        return 0;
    if (strchr(buf, '\n') == NULL && !feof(stdin)) {
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        return -1;
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}
/* Inteiro valido: nenhum caractere alem de espacos apos o numero */
static int parse_inteiro(const char *s, long *v)
{
    char *fim;
    *v = strtol(s, &fim, 10);
    return fim != s && so_espacos(fim);
}
static int ler_texto(const char *rotulo, char *dest, size_t tam)
{
    char buf[TAM_LINHA];
    int r;
    for (;;) {
        r = ler_linha(rotulo, buf, sizeof buf);
        if (r == 0) return 0;
        if (r < 0) { printf(" Texto longo demais.\n"); continue; }
        aparar(buf);
        if (buf[0] == '\0') { printf(" Campo obrigatorio: nao pode ficar vazio.\n"); continue; }
        if (strlen(buf) >= tam) { printf(" Maximo de %d caracteres.\n", (int)tam - 1); continue; }
        strcpy(dest, buf);
        return 1;
    }
}
static int ler_inteiro(const char *rotulo, long min, long max, int *out)
{
    char buf[TAM_LINHA];
    long v;
    int r;
    for (;;) {
        r = ler_linha(rotulo, buf, sizeof buf);
        if (r == 0) return 0;
        if (r > 0 && parse_inteiro(buf, &v) && v >= min && v <= max) {
            *out = (int)v;
            return 1;
        }
        printf(" Valor invalido (inteiro entre %ld e %ld).\n", min, max);
    }
}
static int ler_numero(const char *rotulo, double min, double max, double *out)
{
    char buf[TAM_LINHA], *fim;
    double v;
    int r;
    for (;;) {
        r = ler_linha(rotulo, buf, sizeof buf);
        if (r == 0) return 0;
        if (r > 0) {
            v = strtod(buf, &fim);
            if (fim != buf && so_espacos(fim) && v >= min && v <= max) {
                *out = v;
                return 1;
            }
        }
        printf(" Valor invalido (numero entre %.10g e %.10g, sem texto extra).\n", min,
            max);
    }
}
static int ler_binario(const char *rotulo, int *out)
{
    char buf[TAM_LINHA];
    int r;
    for (;;) {
        r = ler_linha(rotulo, buf, sizeof buf);
        if (r == 0) return 0;
        if (r > 0) {
            aparar(buf);
            if (strcmp(buf, "0") == 0 || strcmp(buf, "1") == 0) {
                *out = buf[0] - '0';
                return 1;
            }
        }
        printf(" Resposta invalida: digite somente 0 ou 1.\n");
    }
}
static int ler_uf(char *dest)
{
    char buf[TAM_LINHA];
    int r;
    for (;;) {
        r = ler_linha("UF (sigla): ", buf, sizeof buf);
        if (r == 0) return 0;
        if (r > 0) {
            aparar(buf);
            if (na_lista(buf, UFS, NUM(UFS))) {
                maiusculas(buf);
                strcpy(dest, buf);
                return 1;
            }
        }
        printf(" UF invalida: use uma sigla brasileira (ex.: GO, MG, BA).\n");
    }
}
static int ler_elemento(char *dest)
{
    char buf[TAM_LINHA];
    size_t i;
    int r;
    for (;;) {
        r = ler_linha("Elemento principal (simbolo): ", buf, sizeof buf);
        if (r == 0) return 0;
        if (r > 0) {
            aparar(buf);
            for (i = 0; i < NUM(ELEMENTOS); i++)
                if (igual_sem_caixa(buf, ELEMENTOS[i])) {
                strcpy(dest, ELEMENTOS[i]); /* forma canonica */
                return 1;
            }
        }
        printf(" Elemento invalido: informe o simbolo de um dos 17 ETR (ex.: Nd, Dy, Gd).\n");
    }
}
static int ler_opcao(void)
{
    char buf[TAM_LINHA];
    long v;
    int r = ler_linha("\nOpcao: ", buf, sizeof buf);
    if (r == 0) return 0; /* fim da entrada: encerra */
    if (r < 0 || !parse_inteiro(buf, &v) || v < 0 || v > 8) return -1;
    return (int)v;
}
static int elemento_pesado(const char *el)
{
    return na_lista(el, PESADOS, NUM(PESADOS));
}
/* Indice de Prioridade de Auditoria (0 a 100) - heuristica didatica */
static double calcular_ipa(const Lote *l)
{
    double ipa = 0.0;
    if (!l->rastreado) ipa += 40.0;
    if (!l->licenca_ok) ipa += 30.0;
    if (elemento_pesado(l->elemento)) ipa += 20.0;
    if (l->teor_treo < 30.0) ipa += 10.0;
    return ipa;
}
static const char *classe_ipa(double ipa)
{
    if (ipa >= 70.0) return "ALTA";
    if (ipa >= 40.0) return "MEDIA";
    return "BAIXA";
}
static int proximo_id(const Repositorio *r)
{
    int i, maior = 0;
    for (i = 0; i < r->n; i++)
        if (r->itens[i].id > maior)
            maior = r->itens[i].id;
    return maior + 1;
}
static int carregar(Repositorio *r)
{
    int i;
    FILE *f = fopen(ARQ_DADOS, "rb");
    if (f == NULL) {
        r->n = 0;
        return 0;
    }
    r->n = (int)fread(r->itens, sizeof(Lote), MAX_LOTES, f);
    fclose(f);
    for (i = 0; i < r->n; i++) { /* garante textos terminados */
        r->itens[i].origem[TAM_TEXTO - 1] = '\0';
        r->itens[i].destino[TAM_TEXTO - 1] = '\0';
        r->itens[i].uf[2] = '\0';
        r->itens[i].elemento[3] = '\0';
    }
    return r->n;
}
static int salvar(const Repositorio *r)
{
    size_t gravados;
    FILE *f = fopen(ARQ_DADOS, "wb");
    if (f == NULL)
        return 0;
    gravados = fwrite(r->itens, sizeof(Lote), (size_t)r->n, f);
    if (fclose(f) != 0)
        return 0;
    return gravados == (size_t)r->n;
}
static void salvar_ou_avisar(const Repositorio *r)
{
    if (!salvar(r))
        printf("AVISO: falha ao salvar %s; as alteracoes existem so em memoria.\n",
            ARQ_DADOS);
}
static void cadastrar(Repositorio *r)
{
    Lote l;
    if (r->n >= MAX_LOTES) {
        printf("Repositorio cheio.\n");
        return;
    }
    memset(&l, 0, sizeof l);
    l.id = proximo_id(r);
    if (!ler_texto("Origem (mina/projeto): ", l.origem, sizeof l.origem) ||
        !ler_uf(l.uf) ||
        !ler_elemento(l.elemento) ||
        !ler_numero("Massa do concentrado (t): ", 0.001, 1000000.0, &l.massa_t) ||
        !ler_numero("Teor de TREO (%): ", 0.0, 100.0, &l.teor_treo) ||
        !ler_binario("Certificado de origem? (1=sim, 0=nao): ", &l.rastreado) ||
        !ler_binario("Licenca ambiental regular? (1=sim, 0=nao): ", &l.licenca_ok) ||
        !ler_texto("Destino: ", l.destino, sizeof l.destino)) {
        printf("Entrada encerrada: lote nao cadastrado.\n");
        return;
    }
    r->itens[r->n++] = l;
    printf("Lote %d cadastrado. IPA = %.0f (%s)\n", l.id,
        calcular_ipa(&l), classe_ipa(calcular_ipa(&l)));
    salvar_ou_avisar(r);
}
static void remover(Repositorio *r)
{
    int i, id;
    if (!ler_inteiro("ID do lote a remover: ", 1, 1000000, &id))
        return;
    for (i = 0; i < r->n; i++) {
        if (r->itens[i].id == id) {
            memmove(&r->itens[i], &r->itens[i + 1],
                sizeof(Lote) * (size_t)(r->n - i - 1));
            r->n--;
            printf("Lote %d removido.\n", id);
            salvar_ou_avisar(r);
            return;
        }
    }
    printf("ID nao encontrado.\n");
}
static void imprimir_cabecalho(void)
{
    printf("\n%-3s %-22s %-2s %-3s %9s %7s %-4s %-4s %5s %-5s\n",
        "ID", "ORIGEM", "UF", "EL", "MASSA(t)", "TREO(%)",
        "RAST", "LIC", "IPA", "CLASSE");
}
static void imprimir_lote(const Lote *l)
{
    double ipa = calcular_ipa(l);
    printf("%-3d %-22.22s %-2s %-3s %9.2f %7.1f %-4s %-4s %5.0f %-5s\n",
        l->id, l->origem, l->uf, l->elemento, l->massa_t, l->teor_treo,
        l->rastreado ? "SIM" : "NAO", l->licenca_ok ? "SIM" : "NAO",
        ipa, classe_ipa(ipa));
}
static void listar(const Lote *v, int n)
{
    int i;
    if (n == 0) {
        printf("Nenhum lote para exibir.\n");
        return;
    }
    imprimir_cabecalho();
    for (i = 0; i < n; i++)
        imprimir_lote(&v[i]);
}
static void buscar(const Repositorio *r)
{
    char chave[TAM_TEXTO];
    int i, modo, achou = 0;
    if (!ler_inteiro("Buscar por (1=UF, 2=Elemento): ", 1, 2, &modo) ||
        !ler_texto("Valor: ", chave, sizeof chave))
    return;
    for (i = 0; i < r->n; i++) {
        const char *campo = (modo == 1) ? r->itens[i].uf : r->itens[i].elemento;
        if (igual_sem_caixa(campo, chave)) {
            if (!achou)
                imprimir_cabecalho();
            imprimir_lote(&r->itens[i]);
            achou++;
        }
    }
    if (!achou)
        printf("Nenhum lote encontrado.\n");
}
static int cmp_massa_desc(const void *a, const void *b)
{
    double x = ((const Lote *)a)->massa_t, y = ((const Lote *)b)->massa_t;
    return (x < y) - (x > y);
}
static int cmp_ipa_desc(const void *a, const void *b)
{
    double x = calcular_ipa((const Lote *)a), y = calcular_ipa((const Lote *)b);
    return (x < y) - (x > y);
}
static void ordenar_e_listar(const Repositorio *r)
{
    static Lote copia[MAX_LOTES];
    int modo;
    if (r->n == 0) {
        printf("Nenhum lote para ordenar.\n");
        return;
    }
    if (!ler_inteiro("Ordenar por (1=massa, 2=IPA), decrescente: ", 1, 2, &modo))
        return;
    memcpy(copia, r->itens, sizeof(Lote) * (size_t)r->n);
    qsort(copia, (size_t)r->n, sizeof(Lote),
        modo == 1 ? cmp_massa_desc : cmp_ipa_desc);
    listar(copia, r->n);
}
static void estatisticas(const Repositorio *r)
{
    char nomes[MAX_ELEM][4];
    double massas[MAX_ELEM];
    double massa_total = 0.0, soma_teor = 0.0, massa_pesada = 0.0;
    int i, k, ne = 0, rast = 0, lic = 0, alta = 0;
    for (i = 0; i < r->n; i++) {
        const Lote *l = &r->itens[i];
        massa_total += l->massa_t;
        soma_teor += l->massa_t * l->teor_treo;
        if (elemento_pesado(l->elemento)) massa_pesada += l->massa_t;
        if (l->rastreado) rast++;
        if (l->licenca_ok) lic++;
        if (calcular_ipa(l) >= 70.0) alta++;
        for (k = 0; k < ne && !igual_sem_caixa(nomes[k], l->elemento); k++)
            ;
        if (k == ne && ne < MAX_ELEM) {
            strcpy(nomes[ne], l->elemento);
            massas[ne++] = 0.0;
        }
        if (k < MAX_ELEM)
            massas[k] += l->massa_t;
    }
    if (r->n == 0 || massa_total <= 0.0) {
        printf("Sem dados.\n");
        return;
    }
    printf("\n=== ESTATISTICAS ===\n");
    printf("Lotes cadastrados ........ %d\n", r->n);
    printf("Massa total .............. %.2f t\n", massa_total);
    printf("Teor medio ponderado ..... %.2f %% TREO\n", soma_teor / massa_total);
    printf("Participacao dos lotes com elemento principal pesado na massa total: %.1f %%\n",
        100.0 * massa_pesada / massa_total);
    printf("Lotes com rastreabilidade %.1f %%\n", 100.0 * rast / r->n);
    printf("Lotes com licenca regular %.1f %%\n", 100.0 * lic / r->n);
    printf("Lotes com prioridade ALTA %d\n", alta);
    printf("Massa por elemento principal:\n");
    for (k = 0; k < ne; k++)
        printf(" %-3s %9.2f t (%.1f %%)\n", nomes[k], massas[k],
            100.0 * massas[k] / massa_total);
}
static void csv_texto(FILE *f, const char *s)
{
    fputc('"', f); for (; *s; s++) {
        if (*s == '"') fputc('"', f); /* aspas internas duplicadas */
        fputc(*s, f);
    }
    fputc('"', f); }
static void json_texto(FILE *f, const char *s)
{
    fputc('"', f); for (; *s; s++) {
        unsigned char c = (unsigned char)*s;
        switch (c) {
            case '"': fputs("\\\"", f); break;
            case '\\': fputs("\\\\", f); break;
            case '\b': fputs("\\b", f); break;
            case '\f': fputs("\\f", f); break;
            case '\n': fputs("\\n", f); break;
            case '\r': fputs("\\r", f); break;
            case '\t': fputs("\\t", f); break;
            default:
            if (c < 0x20) fprintf(f, "\\u%04x", c); /* demais controles */
            else fputc(c, f);
        }
    }
    fputc('"', f); }
static int exportar(const Repositorio *r)
{
    FILE *fj = fopen(ARQ_JSON, "w");
    FILE *fc = fopen(ARQ_CSV, "w");
    int i, ok = 1;
    if (fj == NULL || fc == NULL) {
        if (fj) fclose(fj);
        if (fc) fclose(fc);
        return 0;
    }
    fprintf(fc, "id,origem,uf,elemento,massa_t,teor_treo,rastreado,"
        "licenca_ok,destino,ipa,classe\n");
    fprintf(fj, "[\n");
    for (i = 0; i < r->n; i++) {
        const Lote *l = &r->itens[i];
        double ipa = calcular_ipa(l);
        fprintf(fj, " {\"_id\": %d, \"origem\": ", l->id);
        json_texto(fj, l->origem);
        fputs(", \"uf\": ", fj); json_texto(fj, l->uf);
        fputs(", \"elemento\": ", fj); json_texto(fj, l->elemento);
        fprintf(fj, ", \"grupo\": \"%s\", \"massa_t\": %.2f, \"teor_treo\": %.1f, "
            "\"rastreado\": %s, \"licenca_ok\": %s, \"destino\": ",
            elemento_pesado(l->elemento) ? "pesado" : "leve",
            l->massa_t, l->teor_treo,
            l->rastreado ? "true" : "false", l->licenca_ok ? "true" : "false");
        json_texto(fj, l->destino);
        fprintf(fj, ", \"ipa\": %.0f, \"classe\": \"%s\"}%s\n", ipa, classe_ipa(ipa),
            (i < r->n - 1) ? "," : "");
        fprintf(fc, "%d,", l->id);
        csv_texto(fc, l->origem); fputc(',', fc);
        csv_texto(fc, l->uf); fputc(',', fc);
        csv_texto(fc, l->elemento); fputc(',', fc);
        fprintf(fc, "%.2f,%.1f,%d,%d,", l->massa_t, l->teor_treo,
            l->rastreado, l->licenca_ok);
        csv_texto(fc, l->destino);
        fprintf(fc, ",%.0f,%s\n", ipa, classe_ipa(ipa));
    }
    fprintf(fj, "]\n");
    if (fclose(fj) != 0) ok = 0;
    if (fclose(fc) != 0) ok = 0;
    return ok;
}
static void carregar_demo(Repositorio *r)
{
    static const Lote demo[] = {
        {1, "Projeto Alfa (demo)", "GO", "Nd", 42.0, 48.5, 1, 1, "Refinaria Sul"},
        {2, "Projeto Beta (demo)", "MG", "Dy", 3.2, 35.0, 1, 0, "Laboratorio Norte"},
        {3, "Projeto Gama (demo)", "BA", "Pr", 18.7, 27.0, 0, 1, "Fabrica de Imas"},
        {4, "Projeto Delta (demo)", "GO", "Tb", 1.1, 22.0, 0, 0, "Exportacao"},
        {5, "Projeto Epsilon (demo)","MG", "Ce", 75.0, 55.0, 1, 1, "Polimento Ind."},
        {6, "Projeto Zeta (demo)", "AM", "Y", 5.4, 31.0, 1, 1, "Fosforos Ltda"},
        {7, "Projeto Eta (demo)", "GO", "La", 20.0, 41.0, 0, 1, "Baterias SA"}
    };
    if (r->n != 0) {
        printf("Demonstracao so pode ser carregada com o repositorio vazio.\n");
        return;
    }
    r->n = (int)NUM(demo);
    memcpy(r->itens, demo, sizeof demo);
    printf("%d lotes ficticios carregados.\n", r->n);
    salvar_ou_avisar(r);
}
static void menu(void)
{
    printf("\n===== TerraTrace-C =====\n"
        "1 - Cadastrar lote\n"
        "2 - Listar lotes\n"
        "3 - Buscar (UF ou elemento)\n"
        "4 - Ordenar (massa ou IPA)\n"
        "5 - Estatisticas\n"
        "6 - Exportar JSON e CSV\n"
        "7 - Carregar dados de demonstracao\n"
        "8 - Remover lote\n"
        "0 - Salvar e sair\n");
}
int main(void)
{
    static Repositorio repo;
    int op;
    carregar(&repo);
    printf("TerraTrace-C: %d lote(s) carregado(s) de %s.\n", repo.n, ARQ_DADOS);
    do {
        menu();
        op = ler_opcao();
        switch (op) {
            case 1: cadastrar(&repo); break;
            case 2: listar(repo.itens, repo.n); break;
            case 3: buscar(&repo); break;
            case 4: ordenar_e_listar(&repo); break;
            case 5: estatisticas(&repo); break;
            case 6: printf(exportar(&repo) ? "Arquivos %s e %s gerados.\n"
                : "Falha na exportacao (%s, %s).\n",
                ARQ_JSON, ARQ_CSV); break;
            case 7: carregar_demo(&repo); break;
            case 8: remover(&repo); break;
            case 0: break;
            default: printf("Opcao invalida.\n");
        }
    } while (op != 0);
    if (!salvar(&repo))
        printf("Erro ao salvar %s.\n", ARQ_DADOS);
    return 0;
}
