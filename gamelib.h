#include "utils.h"

void imposta_gioco();

void gioca();

void termina_gioco();

#define MIN_PLAYER 1
#define MAX_PLAYER 4
#define nicknameLength 12
#define numberOfZonaSegrete 15

enum classe_giocatore
{
    barbaro,
    nano,
    elfo,
    mago
};

struct Giocatore
{
    char nome_giocatore[12];
    enum classe_giocatore classe;
    struct Zona_segrete *posizione;
    unsigned char p_vita;
    unsigned char dadi_attacco;
    unsigned char dadi_difesa;
    unsigned char mente;
    unsigned char potere_speciale;
    unsigned char punti_vita_max;
};

typedef struct Giocatore Giocatore;

enum tipo_zona
{
    corridoio,
    scala,
    scala_banchetto,
    magazzino,
    giardino,
    posto_guardia,
    prigione,
    cucina,
    armeria,
    tempio
};

enum tipo_tesoro
{
    nessun_tesoro,
    veleno,
    guarigione,
    doppia_guarigione
};

enum tipo_porta
{
    nessuna_porta,
    porta_normale,
    porta_da_scassinare
};

struct Zona_segrete
{
    struct Zona_segrete* zona_successiva;
    struct Zona_segrete* zona_precedente;
    enum tipo_zona tipoZona;
    enum tipo_tesoro tipoTesoro;
    enum tipo_porta tipoPorta;
};

struct Abitante_segrete {
    char nome[12];
    unsigned char dadi_attacco;
    unsigned char dadi_difesa;
    unsigned char punti_vita;
    unsigned char punti_vita_max;
};

typedef struct Abitante_segrete Abitante_segrete;
typedef struct Zona_segrete Zona_segrete;