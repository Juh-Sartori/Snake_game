#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>

/** 
*---------------------------------------------- SUMARIO -------------------------------------------------
*-> DEFINER ____________________________________   32
*-> STRUCTS ____________________________________   78
*-> AUXILIARES _________________________________  183
*-> DISPLEY ____________________________________  203
*-> IMAGEM _____________________________________  266
*-> AUDIO ______________________________________  471
*-> POSICIONAMENTO DO CORPO ____________________  514
*-> INICIALIZACAO DOS OJETOS ___________________  541
*-> AJUESTE DA COBRA ___________________________  592
*-> ANIMACAO ___________________________________  620
*-> COLISAO ____________________________________  709
*-> EVENTOS DA COBRA __________________________  1134
*-> ATUALISACAO E EVENTOS DA COBRA _____________ 1190
*-> DESENHAR OBJETOS ___________________________ 1331
*-> DESENHAR TELAS _____________________________ 1466
*-> METODOS MAIN DO JOGO _______________________ 1594
*/
//------------------------------------------- DEFINER -------------------------------------------------

#define KEY_SEEN 1
#define KEY_RELEASED 2

#define DISP_SCALE 1 // muda o tamanho da cobra
#define TELA_LARGURA 1200 / DISP_SCALE
#define TELA_ALTURA 700 / DISP_SCALE

#define DISP_LARGURA (TELA_LARGURA * DISP_SCALE)
#define DISP_ALTURA (TELA_ALTURA * DISP_SCALE)

#define SHIP_SPEED 1 //velocidade da nave
#define COBRA_MAX_X (TELA_LARGURA - COBRA_ALTURA)
#define COBRA_MAX_Y (TELA_ALTURA - COBRA_ALTURA)

#define COBRA_VELO_SLOW 8
#define COBRA_VELO 4
#define COBRA_VELO_SPEED 2

#define NORMAL 0
#define SLOW 1
#define SPEED 2
#define TONTA 4
#define MORTA 4
#define TEMPO_STATUS 200;

#define FIM 0
#define INICIO 1
#define PAUSE 2
#define PLAY 3

#define COBRA_LARGURA 39
#define COBRA_ALTURA 63

#define CORPO_LARGURA 23
#define CORPO_ALTURA 45

#define RABO_LARGURA 25 //x
#define RABO_ALTURA 57  //y

#define RATO_LARGURA 25 //x
#define RATO_ALTURA 35  //y

#define ANDA (CORPO_ALTURA - 11)

//-------------------------------------- STRUCTS -----------------------------------

typedef struct SCORE
{
    int qtdCorpo;
    int qtdPreto;
    int qtdMarrom;
    int qtdDourado;
    int qtdVermelho;
    int qtdMorte;
    int trasformouEmRato;
    int vitorias;

} SCORE;

typedef struct CORPO
{
    long x, y;
    int angulo;
    int ajuste;

} CORPO;

typedef struct COBRA
{
    ALLEGRO_BITMAP *img[14]; //onde q esta a imagem
    long x, y;
    int angulo;
    int contaVelo;
    int tonta;
    int status; //velocidades
    int conta_status;
    int timer_imune;

    ALLEGRO_BITMAP *corpo_img;
    ALLEGRO_BITMAP *rabo_img;
    CORPO vetCorpo[10000];
    long qtdCorpo;

    SCORE score;

} COBRA;

typedef struct ALIMENTO
{
    ALLEGRO_BITMAP *img; //onde q esta a imagem
    int timer_hide;      //falando q esta ativo
    int timer_show;      //
    long x, y;
    int angulo;

} ALIMENTO;

typedef struct JOGO
{
    long frames;

    //ALIMENTOS //RATOS E QUEIJO
    COBRA jogadores[2];
    ALIMENTO marrom[2];
    ALIMENTO dourado;
    ALIMENTO preto;
    ALIMENTO vermelho;
    ALIMENTO queijo;

    ALLEGRO_BITMAP *rosa;

    //COBRAS
    COBRA cobras[4];
    int jogador1, jogador2;

    //ESTADO DA TELA
    int estado;

    //TELA
    ALLEGRO_DISPLAY *disp;
    ALLEGRO_BITMAP *buffer;

    //IMAGEM
    ALLEGRO_BITMAP *imagem;
    ALLEGRO_BITMAP *img_play;
    ALLEGRO_BITMAP *img_quadro;
    ALLEGRO_BITMAP *fundo;
    ALLEGRO_BITMAP *parede;
    //IMAGEM TECLADO
    ALLEGRO_BITMAP *wasd;
    ALLEGRO_BITMAP *direcional;

    //AUDIO
    ALLEGRO_SAMPLE *tema;
    ALLEGRO_SAMPLE *inicioFim;
    ALLEGRO_SAMPLE *speed;
    ALLEGRO_SAMPLE *slow;
    ALLEGRO_SAMPLE *ecolher;
    ALLEGRO_SAMPLE *comer;
    ALLEGRO_SAMPLE *viraRato;
    ALLEGRO_SAMPLE *come;

    //FONTE
    ALLEGRO_FONT *font_corpo;
    ALLEGRO_FONT *font_titulo;
    ALLEGRO_FONT *font_sub_titulo;
    ALLEGRO_FONT *font_numb;
} JOGO;

//------------------------------------------- AUXILIARES ---------------------------------------------

float radianos(int angulo)
{
    return angulo * acos(-1) / 180.0;
}

/**
 * Responsavel por inicializar tudo no allegro
 */
/***/
void iniciar(bool test, const char *description)
{
    if (test)
        return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

//------------------------------------------------------ DISPLEY ----------------------------------------------
/**
 * Responsavel por preparar o display pro jogo
 * Assim como o teclado
*/

/***/
void iniciar_display(JOGO *jogo)
{
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    jogo->disp = al_create_display(DISP_LARGURA, DISP_ALTURA);
    iniciar(jogo->disp, "display");

    jogo->buffer = al_create_bitmap(TELA_LARGURA, TELA_ALTURA);
    iniciar(jogo->buffer, "bitmap buffer");
}

void destruir_display(JOGO *jogo)
{
    al_destroy_bitmap(jogo->buffer);
    al_destroy_display(jogo->disp);
}

void preparar_display(JOGO *jogo)
{
    al_set_target_bitmap(jogo->buffer);
}

void desenhar_display(JOGO *jogo)
{
    al_set_target_backbuffer(jogo->disp);
    al_draw_scaled_bitmap(jogo->buffer, 0, 0, TELA_LARGURA, TELA_ALTURA, 0, 0, DISP_LARGURA, DISP_ALTURA, 0);

    al_flip_display();
}

unsigned char key[ALLEGRO_KEY_MAX];

void keyboard_init()
{
    memset(key, 0, sizeof(key));
}

void keyboard_update(ALLEGRO_EVENT *event)
{
    switch (event->type)
    {
    case ALLEGRO_EVENT_TIMER:
        for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
            key[i] &= KEY_SEEN;
        break;

    case ALLEGRO_EVENT_KEY_DOWN:
        key[event->keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
        break;
    case ALLEGRO_EVENT_KEY_UP:
        key[event->keyboard.keycode] &= KEY_RELEASED;
        break;
    }
}

//------------------------------------------ IMAGEM ---------------------------------------------
/**
 * Parte responsavel por encontrar e remover as imagens do jogo
*/

/***/
ALLEGRO_BITMAP *econtrar_imagem(JOGO *jogo, int x, int y, int w, int h)
{
    ALLEGRO_BITMAP *sprite = al_create_sub_bitmap(jogo->imagem, x, y, w, h);
    iniciar(sprite, "sprite grab");
    return sprite;
}

COBRA init_vermelha(JOGO *jogo)
{
    COBRA vermelha;
    int h[] = {10, 75};
    int i = 13;
    vermelha.img[i--] = econtrar_imagem(jogo, 14, h[1], COBRA_LARGURA, COBRA_ALTURA);  //13
    vermelha.img[i--] = econtrar_imagem(jogo, 55, h[1], COBRA_LARGURA, COBRA_ALTURA);  //12
    vermelha.img[i--] = econtrar_imagem(jogo, 97, h[1], COBRA_LARGURA, COBRA_ALTURA);  //11s
    vermelha.img[i--] = econtrar_imagem(jogo, 138, h[1], COBRA_LARGURA, COBRA_ALTURA); //10
    vermelha.img[i--] = econtrar_imagem(jogo, 179, h[1], COBRA_LARGURA, COBRA_ALTURA); //09
    vermelha.img[i--] = econtrar_imagem(jogo, 220, h[1], COBRA_LARGURA, COBRA_ALTURA); //08
    vermelha.img[i--] = econtrar_imagem(jogo, 262, h[1], COBRA_LARGURA, COBRA_ALTURA); //07
    vermelha.img[i--] = econtrar_imagem(jogo, 14, h[0], COBRA_LARGURA, COBRA_ALTURA);  //06
    vermelha.img[i--] = econtrar_imagem(jogo, 55, h[0], COBRA_LARGURA, COBRA_ALTURA);  //05
    vermelha.img[i--] = econtrar_imagem(jogo, 97, h[0], COBRA_LARGURA, COBRA_ALTURA);  //04
    vermelha.img[i--] = econtrar_imagem(jogo, 138, h[0], COBRA_LARGURA, COBRA_ALTURA); //03
    vermelha.img[i--] = econtrar_imagem(jogo, 179, h[0], COBRA_LARGURA, COBRA_ALTURA); //02
    vermelha.img[i--] = econtrar_imagem(jogo, 220, h[0], COBRA_LARGURA, COBRA_ALTURA); //01
    vermelha.img[i] = econtrar_imagem(jogo, 262, h[0], COBRA_LARGURA, COBRA_ALTURA);   //00
    vermelha.corpo_img = econtrar_imagem(jogo, 383, 359, CORPO_LARGURA, CORPO_ALTURA);
    vermelha.rabo_img = econtrar_imagem(jogo, 412, 358, RABO_LARGURA, RABO_ALTURA);
    return vermelha;
}

COBRA init_verde(JOGO *jogo)
{
    COBRA verde;
    int h[] = {143, 205};
    int i = 13;
    verde.img[i--] = econtrar_imagem(jogo, 14, h[1], COBRA_LARGURA, COBRA_ALTURA);  //13
    verde.img[i--] = econtrar_imagem(jogo, 55, h[1], COBRA_LARGURA, COBRA_ALTURA);  //12
    verde.img[i--] = econtrar_imagem(jogo, 97, h[1], COBRA_LARGURA, COBRA_ALTURA);  //11
    verde.img[i--] = econtrar_imagem(jogo, 138, h[1], COBRA_LARGURA, COBRA_ALTURA); //10
    verde.img[i--] = econtrar_imagem(jogo, 179, h[1], COBRA_LARGURA, COBRA_ALTURA); //09
    verde.img[i--] = econtrar_imagem(jogo, 220, h[1], COBRA_LARGURA, COBRA_ALTURA); //08
    verde.img[i--] = econtrar_imagem(jogo, 262, h[1], COBRA_LARGURA, COBRA_ALTURA); //07
    verde.img[i--] = econtrar_imagem(jogo, 14, h[0], COBRA_LARGURA, COBRA_ALTURA);  //06
    verde.img[i--] = econtrar_imagem(jogo, 55, h[0], COBRA_LARGURA, COBRA_ALTURA);  //05
    verde.img[i--] = econtrar_imagem(jogo, 97, h[0], COBRA_LARGURA, COBRA_ALTURA);  //04
    verde.img[i--] = econtrar_imagem(jogo, 138, h[0], COBRA_LARGURA, COBRA_ALTURA); //03
    verde.img[i--] = econtrar_imagem(jogo, 179, h[0], COBRA_LARGURA, COBRA_ALTURA); //02
    verde.img[i--] = econtrar_imagem(jogo, 220, h[0], COBRA_LARGURA, COBRA_ALTURA); //01
    verde.img[i] = econtrar_imagem(jogo, 262, h[0], COBRA_LARGURA, COBRA_ALTURA);   //00
    verde.corpo_img = econtrar_imagem(jogo, 443, 359, CORPO_LARGURA, CORPO_ALTURA);
    verde.rabo_img = econtrar_imagem(jogo, 473, 358, RABO_LARGURA, RABO_ALTURA);
    return verde;
}

COBRA init_preta(JOGO *jogo)
{
    COBRA preta;
    int h[] = {275, 342};
    int i = 13;
    preta.img[i--] = econtrar_imagem(jogo, 14, h[1], COBRA_LARGURA, COBRA_ALTURA);  //13
    preta.img[i--] = econtrar_imagem(jogo, 55, h[1], COBRA_LARGURA, COBRA_ALTURA);  //12
    preta.img[i--] = econtrar_imagem(jogo, 97, h[1], COBRA_LARGURA, COBRA_ALTURA);  //11
    preta.img[i--] = econtrar_imagem(jogo, 138, h[1], COBRA_LARGURA, COBRA_ALTURA); //10
    preta.img[i--] = econtrar_imagem(jogo, 179, h[1], COBRA_LARGURA, COBRA_ALTURA); //09
    preta.img[i--] = econtrar_imagem(jogo, 220, h[1], COBRA_LARGURA, COBRA_ALTURA); //08
    preta.img[i--] = econtrar_imagem(jogo, 262, h[1], COBRA_LARGURA, COBRA_ALTURA); //07
    preta.img[i--] = econtrar_imagem(jogo, 14, h[0], COBRA_LARGURA, COBRA_ALTURA);  //06
    preta.img[i--] = econtrar_imagem(jogo, 55, h[0], COBRA_LARGURA, COBRA_ALTURA);  //05
    preta.img[i--] = econtrar_imagem(jogo, 97, h[0], COBRA_LARGURA, COBRA_ALTURA);  //04
    preta.img[i--] = econtrar_imagem(jogo, 138, h[0], COBRA_LARGURA, COBRA_ALTURA); //03
    preta.img[i--] = econtrar_imagem(jogo, 179, h[0], COBRA_LARGURA, COBRA_ALTURA); //02
    preta.img[i--] = econtrar_imagem(jogo, 220, h[0], COBRA_LARGURA, COBRA_ALTURA); //01
    preta.img[i] = econtrar_imagem(jogo, 262, h[0], COBRA_LARGURA, COBRA_ALTURA);   //00
    preta.corpo_img = econtrar_imagem(jogo, 505, 359, CORPO_LARGURA, CORPO_ALTURA);
    preta.rabo_img = econtrar_imagem(jogo, 537, 361, RABO_LARGURA, RABO_ALTURA);
    return preta;
}

//Gerar
COBRA init_branca(JOGO *jogo)
{
    COBRA branca;
    int h[] = {406, 473};
    int i = 13;
    branca.img[i--] = econtrar_imagem(jogo, 14, h[1], COBRA_LARGURA, COBRA_ALTURA);  //13
    branca.img[i--] = econtrar_imagem(jogo, 55, h[1], COBRA_LARGURA, COBRA_ALTURA);  //12
    branca.img[i--] = econtrar_imagem(jogo, 97, h[1], COBRA_LARGURA, COBRA_ALTURA);  //11
    branca.img[i--] = econtrar_imagem(jogo, 138, h[1], COBRA_LARGURA, COBRA_ALTURA); //10
    branca.img[i--] = econtrar_imagem(jogo, 179, h[1], COBRA_LARGURA, COBRA_ALTURA); //09
    branca.img[i--] = econtrar_imagem(jogo, 220, h[1], COBRA_LARGURA, COBRA_ALTURA); //08
    branca.img[i--] = econtrar_imagem(jogo, 262, h[1], COBRA_LARGURA, COBRA_ALTURA); //07
    branca.img[i--] = econtrar_imagem(jogo, 14, h[0], COBRA_LARGURA, COBRA_ALTURA);  //06
    branca.img[i--] = econtrar_imagem(jogo, 55, h[0], COBRA_LARGURA, COBRA_ALTURA);  //05
    branca.img[i--] = econtrar_imagem(jogo, 97, h[0], COBRA_LARGURA, COBRA_ALTURA);  //04
    branca.img[i--] = econtrar_imagem(jogo, 138, h[0], COBRA_LARGURA, COBRA_ALTURA); //03
    branca.img[i--] = econtrar_imagem(jogo, 179, h[0], COBRA_LARGURA, COBRA_ALTURA); //02
    branca.img[i--] = econtrar_imagem(jogo, 220, h[0], COBRA_LARGURA, COBRA_ALTURA); //01
    branca.img[i] = econtrar_imagem(jogo, 262, h[0], COBRA_LARGURA, COBRA_ALTURA);   //00
    branca.corpo_img = econtrar_imagem(jogo, 324, 359, CORPO_LARGURA, CORPO_ALTURA);
    branca.rabo_img = econtrar_imagem(jogo, 355, 359, RABO_LARGURA, RABO_ALTURA);
    return branca;
}

ALIMENTO init_rato_dourado(JOGO *jogo)
{
    ALIMENTO rato;
    rato.img = econtrar_imagem(jogo, 420, 420, RATO_LARGURA, RATO_ALTURA);
    return rato;
}

ALIMENTO init_rato_marrom(JOGO *jogo)
{
    ALIMENTO rato;
    rato.img = econtrar_imagem(jogo, 452, 420, RATO_LARGURA, RATO_ALTURA);
    return rato;
}

ALIMENTO init_rato_preto(JOGO *jogo)
{
    ALIMENTO rato;
    rato.img = econtrar_imagem(jogo, 484, 420, RATO_LARGURA, RATO_ALTURA);
    return rato;
}

ALIMENTO init_rato_vermelho(JOGO *jogo)
{
    ALIMENTO rato;
    rato.img = econtrar_imagem(jogo, 516, 420, RATO_LARGURA, RATO_ALTURA);
    return rato;
}

ALIMENTO init_queijo(JOGO *jogo)
{
    ALIMENTO queijo;
    queijo.img = econtrar_imagem(jogo, 543, 430, RATO_LARGURA, RATO_LARGURA);
    return queijo;
}

//inicia imagem
void iniciar_imagem(JOGO *jogo)
{
    jogo->imagem = al_load_bitmap("Cobrinhas.png");
    iniciar(jogo->imagem, "imagemFundo");
}

void destruir_cobra(COBRA *cobra)
{
    for (size_t j = 0; j < 14; j++)
    {
        al_destroy_bitmap(cobra->img[j]);
    }
    al_destroy_bitmap(cobra->corpo_img);
    al_destroy_bitmap(cobra->rabo_img);
    free(cobra->img);
}

void destruir_imagem(JOGO *jogo)
{
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 14; j++)
        {
            al_destroy_bitmap(jogo->cobras[i].img[j]);
        }
        al_destroy_bitmap(jogo->cobras[i].corpo_img);
        al_destroy_bitmap(jogo->cobras[i].rabo_img);
        free(jogo->jogadores[i].img);
    }
    free(jogo->cobras);

    for (size_t i = 0; i < 2; i++)
    {
        for (size_t j = 0; j < 14; j++)
        {
            al_destroy_bitmap(jogo->jogadores[i].img[j]);
        }
        al_destroy_bitmap(jogo->jogadores[i].corpo_img);
        al_destroy_bitmap(jogo->jogadores[i].rabo_img);
        free(jogo->jogadores[i].img);
    }
    free(jogo->jogadores);
    //ALIMENTOS
    al_destroy_bitmap(jogo->dourado.img);
    al_destroy_bitmap(jogo->marrom[0].img);
    al_destroy_bitmap(jogo->marrom[1].img);
    free(jogo->marrom);
    al_destroy_bitmap(jogo->preto.img);
    al_destroy_bitmap(jogo->vermelho.img);
    al_destroy_bitmap(jogo->dourado.img);
    al_destroy_bitmap(jogo->queijo.img);
    //MUNDO
    al_destroy_bitmap(jogo->parede);
    al_destroy_bitmap(jogo->fundo);
    al_destroy_bitmap(jogo->img_play);
    al_destroy_bitmap(jogo->img_quadro);
    al_destroy_bitmap(jogo->imagem);
}

//------------------------------------------------- AUDIO ------------------------------------------------------
/**
 * Gerencia todo e qual audio do jogo
*/

/***/
void iniciar_audio(JOGO *jogo) //marcar aqui
{
    iniciar(al_install_audio(), "audio");
    iniciar(al_init_acodec_addon(), "audio acodec");
    iniciar(al_reserve_samples(3), "audio sample");
    jogo->tema = al_load_sample("Thema.wav");
    iniciar(jogo->tema, "tema");
    jogo->speed = al_load_sample("speed.wav");
    iniciar(jogo->speed, "speed");
    jogo->slow = al_load_sample("slow.wav");
    iniciar(jogo->slow, "slow");
    jogo->inicioFim = al_load_sample("Abertura.wav");
    iniciar(jogo->inicioFim, "Abertura");
    jogo->ecolher = al_load_sample("encolher.wav");
    iniciar(jogo->ecolher, "ecolher");
    jogo->comer = al_load_sample("comer.wav");
    iniciar(jogo->comer, "comer");
    jogo->viraRato = al_load_sample("virarRato.wav");
    iniciar(jogo->viraRato, "virarRato");
}

void tocar_audio(ALLEGRO_SAMPLE *audio, float volume, ALLEGRO_PLAYMODE play_mode)
{
    al_play_sample(audio, volume, 0.0, 1.0, play_mode, NULL);
}

void destruir_audio(JOGO *jogo)
{
    al_destroy_sample(jogo->tema);
    al_destroy_sample(jogo->speed);
    al_destroy_sample(jogo->slow);
    al_destroy_sample(jogo->inicioFim);
    al_destroy_sample(jogo->ecolher);
    al_destroy_sample(jogo->comer);
    al_destroy_sample(jogo->viraRato);
}

//----------------------------------------- POSICIONAMENTO DO CORPO ---------------------------------------
/**
 * Posiciona o corpo atras do outro e seguindo a cabeça, de acordo com seu angulo.
*/

/***/
void posicionar1Corpo(COBRA *cobra)
{
    CORPO *corpo = &cobra->vetCorpo[0];
    corpo->x = cobra->x - (CORPO_ALTURA - 11) * cos(radianos(corpo->angulo));
    corpo->y = cobra->y + (CORPO_ALTURA - 11) * sin(radianos(corpo->angulo));
}

void posicionarCorpo(COBRA *cobra, int posicao)
{
    CORPO *corpo = &cobra->vetCorpo[posicao];
    CORPO *ant = &cobra->vetCorpo[posicao - 1];
    corpo->x = ant->x - (CORPO_ALTURA - 11) * cos(radianos(corpo->angulo));
    corpo->y = ant->y + (CORPO_ALTURA - 11) * sin(radianos(corpo->angulo));
}

void posicionarRabo(COBRA *cobra)
{
    cobra->vetCorpo[cobra->qtdCorpo].x = cobra->vetCorpo[cobra->qtdCorpo - 1].x;
    cobra->vetCorpo[cobra->qtdCorpo].y = cobra->vetCorpo[cobra->qtdCorpo - 1].y;
}

//-------------------------------------------- INICIALIZACAO DOS OBJETOS ------------------------------------------
/**
 * Todo e qualquer inicialização de objetos do jogo.
 */

/***/
void iniciar_jogadores(COBRA *jogador, int x, int y, int angulo) // marcar aqui
{
    jogador->contaVelo = 0;
    jogador->angulo = angulo;
    jogador->timer_imune = 0;
    jogador->status = NORMAL;
    jogador->x = x;
    jogador->y = y;
    jogador->qtdCorpo = 1;
    jogador->vetCorpo[0].angulo = angulo;
    jogador->vetCorpo[jogador->qtdCorpo].angulo = angulo;
    jogador->conta_status = TEMPO_STATUS;
    jogador->score.qtdCorpo = 0;
    jogador->score.qtdMarrom = 0;
    jogador->score.qtdDourado = 0;
    jogador->score.qtdMorte = 0;
    jogador->score.qtdPreto = 0;
    jogador->score.qtdVermelho = 0;
    jogador->score.trasformouEmRato = 0;
    posicionar1Corpo(jogador);
    posicionarRabo(jogador);
}

void randon_alimento(ALIMENTO *alimento, int min_show, int max_show, int min_hide, int max_hide)
{
    alimento->x = (2 + rand() % (TELA_LARGURA / ANDA - 3)) * ANDA;
    alimento->y = (2 + rand() % (TELA_ALTURA / ANDA - 3)) * ANDA;
    alimento->angulo = 90 * (rand() % 4);
    alimento->timer_show = min_show + rand() % max_show;
    alimento->timer_hide = min_hide + rand() % max_show;
}

void iniciar_alimentos(JOGO *jogo)
{
    srand(time(NULL));
    randon_alimento(&jogo->dourado, 100, 100, 100, 200);
    randon_alimento(&jogo->marrom[0], 0, 100, 0, 0);
    randon_alimento(&jogo->marrom[1], 0, 100, 0, 0);
    randon_alimento(&jogo->preto, 100, 100, 100, 200);
    randon_alimento(&jogo->vermelho, 100, 100, 100, 200);
    randon_alimento(&jogo->queijo, 100, 100, 100, 200);
    jogo->marrom[0].timer_hide = 1;
    jogo->marrom[1].timer_hide = 1;
}

//------------------------------------------- AJUSTES DA COBRA ------------------------------------------------
/**
* Justa o crescimento da cobra, administrando o vetor de corpo, pois se simplimente dominiu o vetor,
* ocorre um bug pois a poisição fica desatializada, os metodos atualizam a posição dos corpos removidos
* ou adicionado.
*/

/**/
void crescer_corpo(COBRA *cobra)
{

    if (cobra->qtdCorpo < 9999)
    {
        cobra->vetCorpo[cobra->qtdCorpo + 1] = cobra->vetCorpo[cobra->qtdCorpo]; //Muda o rabo
        cobra->vetCorpo[cobra->qtdCorpo] = cobra->vetCorpo[cobra->qtdCorpo - 1]; //Adciona um corpo
        cobra->qtdCorpo++;
    }
}
void encolher_corpo(COBRA *cobra)
{

    if (cobra->qtdCorpo < 9999)
    {
        cobra->vetCorpo[cobra->qtdCorpo - 1] = cobra->vetCorpo[cobra->qtdCorpo - 2]; //Muda o rabo
        cobra->qtdCorpo--;
    }
}

//----------------------------------------- ANIMACAO ------------------------------------------

int dist_boca_rato(COBRA *jogador, ALIMENTO *rato)
{
    if (rato->timer_show > 0 || rato->timer_hide == 0)
    {
        return 7;
    }
    if (rato->x > jogador->x && jogador->angulo == 180 ||
        rato->x < jogador->x && jogador->angulo == 0 ||
        rato->y < jogador->y && jogador->angulo == 270 ||
        rato->y > jogador->y && jogador->angulo == 90)
    {
        return 7;
    }
    int dist_x = abs(jogador->x - rato->x) / (float)ANDA;
    int dist_y = abs(jogador->y - rato->y) / (float)ANDA;
    if (dist_y > 7 || dist_x > 7)
    {
        return 7;
    }
    return (dist_x < dist_y) ? dist_x : dist_y;
}

int dist_boca_cobra(COBRA *jogador, COBRA *alvo)
{

    if (alvo->x > jogador->x && jogador->angulo == 180 ||
        alvo->x < jogador->x && jogador->angulo == 0 ||
        alvo->y < jogador->y && jogador->angulo == 270 ||
        alvo->y > jogador->y && jogador->angulo == 90)
    {
        return 7;
    }
    int dist_x = abs(jogador->x - alvo->x) / (float)ANDA;
    int dist_y = abs(jogador->y - alvo->y) / (float)ANDA;
    if (dist_y > 7 || dist_x > 7)
    {
        return 7;
    }
    dist_x = abs(jogador->x - alvo->x) / (float)ANDA;
    dist_y = abs(jogador->y - alvo->y) / (float)ANDA;
    return (dist_x < dist_y) ? dist_x : dist_y;
}
int dist_boca_corpo(COBRA *jogador, CORPO *alvo)
{

    if (alvo->x > jogador->x && jogador->angulo == 180 ||
        alvo->x < jogador->x && jogador->angulo == 0 ||
        alvo->y < jogador->y && jogador->angulo == 270 ||
        alvo->y > jogador->y && jogador->angulo == 90)
    {
        return 7;
    }
    int dist_x = abs(jogador->x - alvo->x) / (float)ANDA;
    int dist_y = abs(jogador->y - alvo->y) / (float)ANDA;
    if (dist_y > 7 || dist_x > 7)
    {
        return 7;
    }
    dist_x = abs(jogador->x - alvo->x) / (float)ANDA;
    dist_y = abs(jogador->y - alvo->y) / (float)ANDA;
    return (dist_x < dist_y) ? dist_x : dist_y;
}

int animacao(JOGO *jogo, int id_jogador)
{
    int dist = dist_boca_rato(&jogo->jogadores[id_jogador], &jogo->marrom[0]);
    int aux = dist_boca_rato(&jogo->jogadores[id_jogador], &jogo->marrom[1]);
    dist = (dist < aux) ? dist : aux;
    aux = dist_boca_rato(&jogo->jogadores[id_jogador], &jogo->dourado);
    dist = (dist < aux) ? dist : aux;
    aux = dist_boca_rato(&jogo->jogadores[id_jogador], &jogo->vermelho);
    dist = (dist < aux) ? dist : aux;
    aux = dist_boca_rato(&jogo->jogadores[id_jogador], &jogo->preto);
    dist = (dist < aux) ? dist : aux;
    if (jogo->jogadores[id_jogador].qtdCorpo > jogo->jogadores[(id_jogador + 1) % 2].qtdCorpo || jogo->jogadores[(id_jogador + 1) % 2].status == MORTA)
    {
        aux = dist_boca_cobra(&jogo->jogadores[id_jogador], &jogo->jogadores[(id_jogador + 1) % 2]);
        dist = (dist < aux) ? dist : aux;
        for (size_t i = 0; i < jogo->jogadores[(id_jogador + 1) % 2].qtdCorpo; i++)
        {
            aux = dist_boca_corpo(&jogo->jogadores[id_jogador], &jogo->jogadores[(id_jogador + 1) % 2].vetCorpo[i]);
            dist = (dist < aux) ? dist : aux;
        }
    }
    return (6 - dist < 0) ? 0 : 6 - dist;
}

//---------------------------------------------- COLISAO ---------------------------------------
/**
 * Este topico trata apenas das colisõesdo jogo
 */

int *ajustar_pontos_cobra(COBRA *cobra, int ajust_x, int ajust_y)
{
    float seno = sin(radianos(cobra->angulo));
    float cosceno = cos(radianos(cobra->angulo));
    int *pontos = calloc(4, sizeof(int));
    pontos[0] = cobra->x - (11 + ajust_y) * cosceno - (COBRA_LARGURA / 2 + ajust_x) * seno;
    pontos[1] = cobra->y - (COBRA_LARGURA / 2 + ajust_x) * cosceno + (11 + ajust_y) * seno;
    pontos[2] = cobra->x + (COBRA_ALTURA - 11 - ajust_y) * cosceno + (COBRA_LARGURA / 2 + ajust_x) * seno;
    pontos[3] = cobra->y + (COBRA_LARGURA / 2 + ajust_x) * cosceno - (COBRA_ALTURA - 11 - ajust_y) * seno;
    return pontos;
}

int *ajustar_pontos_rato(ALIMENTO *alimento)
{
    float seno = sin(radianos(alimento->angulo));
    float cosceno = cos(radianos(alimento->angulo));
    int *pontos = calloc(4, sizeof(int));
    pontos[0] = alimento->x - RATO_LARGURA / 2 * seno;
    pontos[1] = alimento->y - RATO_LARGURA / 2 * cosceno;
    pontos[2] = alimento->x + RATO_ALTURA * cosceno + RATO_LARGURA / 2 * seno;
    pontos[3] = alimento->y + RATO_LARGURA / 2 * cosceno + RATO_ALTURA * seno;
    return pontos;
}

int *ajustar_pontos_quijo(ALIMENTO *alimento)
{
    int *pontos = calloc(4, sizeof(int));
    float seno = sin(radianos(alimento->angulo));
    float cosceno = cos(radianos(alimento->angulo));
    pontos[0] = alimento->x - RATO_LARGURA / 2 * seno;
    pontos[1] = alimento->y - RATO_LARGURA / 2 * cosceno;
    pontos[2] = alimento->x + RATO_LARGURA * cosceno + RATO_LARGURA / 2 * seno;
    pontos[3] = alimento->y + RATO_LARGURA / 2 * cosceno + RATO_LARGURA * seno;
    return pontos;
}

int *ajustar_pontos_corpo(CORPO *corpo)
{
    int *pontos = calloc(4, sizeof(int));
    float seno = sin(radianos(corpo->angulo));
    float cosceno = cos(radianos(corpo->angulo));
    pontos[0] = corpo->x + CORPO_ALTURA * cosceno - (CORPO_LARGURA / 2) * seno;
    pontos[1] = corpo->y + (CORPO_LARGURA / 2) * cosceno + 11 * seno;
    pontos[2] = corpo->x - 11 * cosceno + (CORPO_LARGURA / 2 + 2) * seno;
    pontos[3] = corpo->y - (CORPO_LARGURA / 2) * cosceno - CORPO_ALTURA * seno;
    return pontos;
}

int *ajustar_pontos_rabo(CORPO *corpo)
{
    int *pontos = calloc(4, sizeof(int));
    float seno = sin(radianos(corpo->angulo));
    float cosceno = cos(radianos(corpo->angulo));
    pontos[0] = corpo->x - RABO_ALTURA * cosceno - RABO_LARGURA / 2 * seno;
    pontos[1] = corpo->y + RABO_LARGURA / 2 * cosceno;
    pontos[2] = corpo->x + RABO_LARGURA / 2 * seno;
    pontos[3] = corpo->y - RABO_LARGURA / 2 * cosceno + RABO_ALTURA * seno;
    return pontos;
}

int *ajustar_pontos_rosa(COBRA *cobra)
{
    float seno = sin(radianos(cobra->angulo));
    float cosceno = cos(radianos(cobra->angulo));
    int *pontos = calloc(4, sizeof(int));
    pontos[0] = cobra->x - (RATO_LARGURA / 2 - 10) * seno - 10 * cosceno;
    pontos[1] = cobra->y - (RATO_LARGURA / 2 - 10) * cosceno + 10 * seno;
    pontos[2] = cobra->x + (RATO_ALTURA - 10) * cosceno + (RATO_LARGURA / 2 + 10) * seno;
    pontos[3] = cobra->y + (RATO_LARGURA / 2 + 10) * cosceno - (RATO_ALTURA - 10) * seno;
    return pontos;
}

/**
 * Recebe dois conjuntos de pontos e analiza de a distancia de cada pontos dos conjuntos esta a uma distencia menor
 * q a largura em x e a altura em y do segundo conjunto, caso seja ele confirma a colisão.
 */
int colisao_pontual(int *p1, int *p2)
{
    for (size_t i = 0; i < 2; i++)
    {

        int dxi[2];
        int dxf[2];
        int largura = abs(p2[0] - p2[2]);
        dxi[0] = abs(p1[0] - p2[0]);
        dxf[0] = abs(p1[0] - p2[2]);
        dxi[1] = abs(p1[2] - p2[0]);
        dxf[1] = abs(p1[2] - p2[2]);

        int altura = abs(p2[1] - p2[3]);
        int dyi[2];
        int dyf[2];
        dyi[0] = abs(p1[1] - p2[1]);
        dyf[0] = abs(p1[1] - p2[3]);
        dyi[1] = abs(p1[3] - p2[1]);
        dyf[1] = abs(p1[3] - p2[3]);

        for (size_t i = 0; i < 2; i++)
        {
            for (size_t j = 0; j < 2; j++)
            {
                if (largura > dxi[i] && altura > dyi[j] && largura > dxf[i] && altura > dyf[j])
                {
                    return 1;
                }
            }
        }
        //Troca os pontos para analize
        int *aux = p1;
        p1 = p2;
        p2 = aux;
    }
    return 0;
}

/**
 * É o unico metodo que não usa o metodo de colisão pontual, pois ele precisa apenas olhar o tamanho da tela
 * subtraido de um ajuste, que se refere ao tamanho da cobra, ou rato rosa.
 */
void colide_cobra_parede(JOGO *jogo, COBRA *cobra)
{
    int ajuste = (cobra->status != MORTA) ? 11 : 28;
    if (cobra->x < COBRA_ALTURA - ajuste)
    {
        if (cobra->status != MORTA)
        {
            cobra->score.qtdMorte++;
            tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
        }
        cobra->x = COBRA_ALTURA - ajuste;
        cobra->status = MORTA;
        cobra->qtdCorpo = 1;
    }
    if (cobra->y < COBRA_ALTURA - ajuste)
    {
        if (cobra->status != MORTA)
        {
            cobra->score.qtdMorte++;
            tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
        }
        cobra->y = COBRA_ALTURA - ajuste;
        cobra->status = MORTA;
        cobra->qtdCorpo = 1;
    }
    if (cobra->x > COBRA_MAX_X + ajuste)
    {
        if (cobra->status != MORTA)
        {
            cobra->score.qtdMorte++;
            tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
        }
        cobra->x = COBRA_MAX_X + ajuste;
        cobra->status = MORTA;
        cobra->qtdCorpo = 1;
    }
    if (cobra->y > COBRA_MAX_Y + ajuste)
    {
        if (cobra->status != MORTA)
        {
            cobra->score.qtdMorte++;
            tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
        }
        cobra->y = COBRA_MAX_Y + ajuste;
        cobra->status = MORTA;
        cobra->qtdCorpo = 1;
    }
}

void colide_cobra_rato_dourado(JOGO *jogo, COBRA *cobra, ALIMENTO *dourado)
{
    if (colisao_pontual(ajustar_pontos_cobra(cobra, 0, 0), ajustar_pontos_rato(dourado)))
    {

        cobra->status = SPEED;
        dourado->timer_show = 0;
        dourado->timer_hide = 0;
        cobra->score.qtdDourado++;
        tocar_audio(jogo->speed, 0.3, ALLEGRO_PLAYMODE_ONCE);
    }
}
void colide_cobra_rato_marrom(JOGO *jogo, COBRA *cobra, ALIMENTO *marrom)
{
    if (colisao_pontual(ajustar_pontos_cobra(cobra, 0, 0), ajustar_pontos_rato(marrom)))
    {
        marrom->timer_show = 0;
        marrom->timer_hide = 0;
        crescer_corpo(cobra);
        cobra->score.qtdMarrom++;
        tocar_audio(jogo->comer, 0.3, ALLEGRO_PLAYMODE_ONCE);
    }
}

void colide_cobra_rato_preto(JOGO *jogo, COBRA *cobra, ALIMENTO *preto)
{
    if (colisao_pontual(ajustar_pontos_cobra(cobra, 0, 0), ajustar_pontos_rato(preto)))
    {
        preto->timer_show = 0;
        preto->timer_hide = 0;
        cobra->score.qtdPreto++;
        if (cobra->qtdCorpo > 1)
        {
            encolher_corpo(cobra);
            tocar_audio(jogo->ecolher, 0.7, ALLEGRO_PLAYMODE_ONCE);
        }
        else
        {
            cobra->status = MORTA;
            cobra->score.qtdMorte++;
            tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
        }
    }
}

void colide_cobra_rato_vermelho(JOGO *jogo, COBRA *cobra, ALIMENTO *vermelho)
{

    if (colisao_pontual(ajustar_pontos_cobra(cobra, 0, 0), ajustar_pontos_rato(vermelho)))
    {
        vermelho->timer_show = 0;
        vermelho->timer_hide = 0;
        cobra->status = SLOW;
        cobra->score.qtdVermelho++;
        tocar_audio(jogo->slow, 0.5, ALLEGRO_PLAYMODE_ONCE);
    }
}

void colide_cobra_queijo(JOGO *jogo, COBRA *cobra, ALIMENTO *queijo)
{
    if (colisao_pontual(ajustar_pontos_cobra(cobra, 0, 0), ajustar_pontos_quijo(queijo)))
    {
        queijo->timer_show = 0;
        queijo->timer_hide = 0;
        cobra->status = NORMAL;
        tocar_audio(jogo->comer, 0.3, ALLEGRO_PLAYMODE_ONCE);
    }
}

void colide_cobra_corpo(JOGO *jogo, COBRA *cobra)
{
    int i;
    for (i = 2; i < cobra->qtdCorpo; i++)
    {
        if (colisao_pontual(ajustar_pontos_cobra(cobra, -10, 0), ajustar_pontos_corpo(&cobra->vetCorpo[i])))
        {
            cobra->status = MORTA;
            cobra->qtdCorpo = 1;
            cobra->score.qtdMorte++;
            tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
        }
    }

    if (colisao_pontual(ajustar_pontos_cobra(cobra, -10, 0), ajustar_pontos_rabo(&cobra->vetCorpo[cobra->qtdCorpo])))
    {
        cobra->status = MORTA;
        cobra->qtdCorpo = 1;
        cobra->score.qtdMorte++;
        tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
    }
}

void colide_cobra_cobra(COBRA *cobra1, COBRA *cobra2, JOGO *jogo)
{
    if (colisao_pontual(ajustar_pontos_cobra(cobra1, 0, 0), ajustar_pontos_cobra(cobra2, 0, 0)))
    {
        if (cobra1->qtdCorpo > cobra2->qtdCorpo)
        {
            cobra2->status = MORTA;
            cobra2->qtdCorpo = 1;
            cobra2->timer_imune = 100;
            cobra2->score.qtdMorte++;
            cobra1->score.trasformouEmRato++;
            tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
        }
        else if (cobra1->qtdCorpo < cobra2->qtdCorpo)
        {
            cobra1->status = MORTA;
            cobra1->qtdCorpo = 1;
            cobra1->score.qtdMorte++;
            cobra2->score.trasformouEmRato++;
            tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
        }
        else
        {
            cobra1->status = cobra2->status = MORTA;
            cobra1->qtdCorpo = cobra2->qtdCorpo = 1;
            cobra2->score.qtdMorte++;
            tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
        }
    }
    int i = 0;
    for (i = 0; i < cobra2->qtdCorpo; i++)
    {
        if (colisao_pontual(ajustar_pontos_cobra(cobra1, 0, 0), ajustar_pontos_corpo(&cobra2->vetCorpo[i])))
        {
            if (cobra1->qtdCorpo > cobra2->qtdCorpo)
            {
                cobra2->status = MORTA;
                cobra2->qtdCorpo = 1;
                cobra2->timer_imune = 100;
                cobra2->score.qtdMorte++;
                cobra1->score.trasformouEmRato++;
                tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
            }
            else
            {
                cobra1->status = MORTA;
                cobra1->qtdCorpo = 1;
                cobra2->timer_imune = 100;
                cobra1->score.qtdMorte++;
                tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
            }
        }
    }
    if (colisao_pontual(ajustar_pontos_cobra(cobra1, 0, 0), ajustar_pontos_rabo(&cobra2->vetCorpo[i])))
    {
        if (cobra1->qtdCorpo > cobra2->qtdCorpo)
        {
            cobra2->status = MORTA;
            cobra2->qtdCorpo = 1;
            cobra2->timer_imune = 100;
            cobra2->score.qtdMorte++;
            cobra1->score.trasformouEmRato++;
            tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
        }
        else
        {
            cobra1->status = MORTA;
            cobra1->qtdCorpo = 1;
            cobra1->timer_imune = 100;
            cobra1->score.qtdMorte++;
            tocar_audio(jogo->viraRato, 0.3, ALLEGRO_PLAYMODE_ONCE);
        }
    }
}

void colide_cobra_rosa(COBRA *cobra1, COBRA *cobra2, JOGO *jogo)
{
    if (colisao_pontual(ajustar_pontos_cobra(cobra1, 0, 0), ajustar_pontos_rosa(cobra2)))
    {
        if (cobra2->timer_imune <= 0)
        {
            if (jogo->estado != FIM)
                cobra1->score.vitorias++;
            jogo->estado = FIM;
            al_stop_samples();
            tocar_audio(jogo->inicioFim, 0.3, ALLEGRO_PLAYMODE_LOOP);
        }
    }
    int i = 0;
    if (abs(cobra1->angulo - cobra2->angulo) != 180)
    {
        for (i = 0; i < cobra1->qtdCorpo; i++)
        {
            if (colisao_pontual(ajustar_pontos_corpo(&cobra1->vetCorpo[i]), ajustar_pontos_rosa(cobra2)))
            {
                cobra2->x = cobra2->x - ANDA * cos(radianos(cobra2->angulo));
                cobra2->y = cobra2->y + ANDA * sin(radianos(cobra2->angulo));
            }
        }
        if (colisao_pontual(ajustar_pontos_rabo(&cobra1->vetCorpo[i]), ajustar_pontos_rosa(cobra2)))
        {
            cobra2->x = cobra2->x - ANDA * cos(radianos(cobra2->angulo));
            cobra2->y = cobra2->y + ANDA * sin(radianos(cobra2->angulo));
        }
    }
}

/**
 * Meotodo principal da colisão, ele é quem é repsonsavel por chama a colisão do jogador com
 * qualquer item do jogo.
 */
void colide(JOGO *jogo)
{
    for (size_t i = 0; i < 2; i++)
    {

        colide_cobra_parede(jogo, &jogo->jogadores[i]);
        if (jogo->jogadores[i].status != MORTA)
        {
            //MARROM
            for (size_t j = 0; j < 2; j++)
            {
                if (jogo->marrom[j].timer_show == 0)
                {
                    colide_cobra_rato_marrom(jogo, &jogo->jogadores[i], &jogo->marrom[j]);
                }
            }

            //DOURADO
            if (jogo->dourado.timer_hide > 0 && jogo->dourado.timer_show == 0)
            {
                colide_cobra_rato_dourado(jogo, &jogo->jogadores[i], &jogo->dourado);
            }
            //PRETO
            if (jogo->preto.timer_hide > 0 && jogo->preto.timer_show == 0)
            {
                colide_cobra_rato_preto(jogo, &jogo->jogadores[i], &jogo->preto);
            }
            //VRMELHA
            if (jogo->vermelho.timer_hide > 0 && jogo->vermelho.timer_show == 0)
            {
                colide_cobra_rato_vermelho(jogo, &jogo->jogadores[i], &jogo->vermelho);
            }
            colide_cobra_corpo(jogo, &jogo->jogadores[i]);
            if (jogo->jogadores[(i + 1) % 2].status == MORTA)
                colide_cobra_rosa(&jogo->jogadores[i], &jogo->jogadores[(i + 1) % 2], jogo);
            else
                colide_cobra_cobra(&jogo->jogadores[i], &jogo->jogadores[(i + 1) % 2], jogo);
        }
        else
        {
            //QUEIJO
            if (jogo->queijo.timer_hide > 0 && jogo->queijo.timer_show == 0)
            {
                colide_cobra_queijo(jogo, &jogo->jogadores[i], &jogo->queijo);
            }
        }
    }
}

//---------------------------------- EVENTOS DA COBRA -------------------------------------------
/**
 * Adimistra o evento de teclado durante o jogo.
 */

/**/
void evento_jogador1(COBRA *cobra)
{
    if (key[ALLEGRO_KEY_LEFT])
        if (cobra->vetCorpo[0].angulo != 0)
        {
            cobra->angulo = 180;
        }

    if (key[ALLEGRO_KEY_RIGHT])
        if (cobra->vetCorpo[0].angulo != 180)
        {
            cobra->angulo = 0;
        }
    if (key[ALLEGRO_KEY_UP])
        if (cobra->vetCorpo[0].angulo != 270)
        {
            cobra->angulo = 90;
        }
    if (key[ALLEGRO_KEY_DOWN])
        if (cobra->vetCorpo[0].angulo != 90)
        {
            cobra->angulo = 270;
        }
}

void evento_jogador2(COBRA *cobra)
{
    if (key[ALLEGRO_KEY_A])
        if (cobra->vetCorpo[0].angulo != 0)
        {
            cobra->angulo = 180;
        }

    if (key[ALLEGRO_KEY_D])
        if (cobra->vetCorpo[0].angulo != 180)
        {
            cobra->angulo = 0;
        }
    if (key[ALLEGRO_KEY_W])
        if (cobra->vetCorpo[0].angulo != 270)
        {
            cobra->angulo = 90;
        }
    if (key[ALLEGRO_KEY_S])
        if (cobra->vetCorpo[0].angulo != 90)
        {
            cobra->angulo = 270;
        }
}

//---------------------------------- ATUALIZACAO E DINAMICA DA COBRA -------------------------------------------
/**
 * Responsavel por administra o que esta acontessendo com a cobra durante o jogo, ele quem adm todos movimentos.
 */

/**
 * Troca a direção da cobra se for possivel.
*/
void muda_angulo(COBRA *cobra)
{
    int i;
    if (cobra->vetCorpo[cobra->qtdCorpo].angulo != cobra->vetCorpo[cobra->qtdCorpo - 1].angulo)
    {
        cobra->vetCorpo[cobra->qtdCorpo].angulo = cobra->vetCorpo[cobra->qtdCorpo - 1].angulo;
    }

    for (i = cobra->qtdCorpo - 1; i > 0; i--)
    {
        if (cobra->vetCorpo[i].angulo != cobra->vetCorpo[i - 1].angulo)
        {
            cobra->vetCorpo[i].angulo = cobra->vetCorpo[i - 1].angulo;
        }
    }

    if (cobra->vetCorpo[0].angulo != cobra->angulo)
    {
        cobra->vetCorpo[0].angulo = cobra->angulo;
    }
}

/**
 * Faz o corpo acompanhar a cabeça.
*/
void atualizacao_corpo(COBRA *cobra)
{
    int i;
    posicionar1Corpo(cobra);

    for (i = 1; i < cobra->qtdCorpo; i++)
    {
        posicionarCorpo(cobra, i);
    }
    posicionarRabo(cobra);
}

void atualizacao_cabeca(COBRA *cobra)
{

    if (cobra->conta_status == 0)
    {
        cobra->conta_status = TEMPO_STATUS;
        cobra->status = (cobra->status == MORTA) ? MORTA : NORMAL;
    }
    else
    {
        cobra->conta_status--;
    }
    cobra->x += ANDA * cos(radianos(cobra->angulo));
    cobra->y -= ANDA * sin(radianos(cobra->angulo));
}

/**
 * Dinamica rotacional do corpo, dividi(cabeca, corpo e rabo) tbm
 * Colocar todos os srtuctures para 90 de todas as funções
*/
void atualizacao_jogadores(COBRA *jogador)
{
    int velo;
    switch (jogador->status)
    {
    case SPEED:
        velo = COBRA_VELO_SPEED;
        break;
    case SLOW:
        velo = COBRA_VELO_SLOW;
        break;
    default:
        velo = COBRA_VELO;
        break;
    }

    if (jogador->contaVelo >= velo) //Correto
    {
        atualizacao_cabeca(jogador);
        muda_angulo(jogador);
        atualizacao_corpo(jogador);
        jogador->contaVelo = 0;
    }
    else
    {
        jogador->contaVelo++;
    }
    if (jogador->qtdCorpo > jogador->score.qtdCorpo)
    {
        jogador->score.qtdCorpo = jogador->qtdCorpo;
    }
    if (jogador->timer_imune > 0)
    {
        jogador->timer_imune--;
    }
}

void atualizacao_alimento(ALIMENTO *alimento, int min_show, int max_show, int min_hide, int max_hide)
{
    if (alimento->timer_show > 0)
        alimento->timer_show--;
    else if (alimento->timer_hide > 0)
        alimento->timer_hide--;
    else
    {
        randon_alimento(alimento, min_show, max_show, min_hide, max_hide);
    }
}

void atualizacao_alimentos(JOGO *jogo)
{
    atualizacao_alimento(&jogo->dourado, 100, 100, 100, 200);
    atualizacao_alimento(&jogo->marrom[0], 0, 100, 0, 0);
    atualizacao_alimento(&jogo->marrom[1], 0, 100, 0, 0);
    atualizacao_alimento(&jogo->preto, 100, 100, 100, 200);
    atualizacao_alimento(&jogo->vermelho, 100, 100, 100, 200);
    atualizacao_alimento(&jogo->queijo, 100, 100, 100, 200);
    jogo->marrom[0].timer_hide = 2;
    jogo->marrom[1].timer_hide = 2;
}

/**
 * Metodo principal da atualização, é responsavel por chamar os metodos desta alem de outras partes como
 * COLISAO
 * EVENTOS
*/
void atualizar_jogo(JOGO *jogo)
{
    atualizacao_jogadores(&jogo->jogadores[0]);
    atualizacao_jogadores(&jogo->jogadores[1]);
    atualizacao_alimentos(jogo);
    colide(jogo);
    evento_jogador1(&jogo->jogadores[0]);
    evento_jogador2(&jogo->jogadores[1]);
}

//--------------------------------------------- DESENHAR OBJETOS ------------------------------------------------
/**
 * Resposaveis por desenhar os objetos do jogo
*/

/***/
void desenhar_fundo(JOGO *jogo)
{
    int i, j;

    for (i = 0; i < (TELA_LARGURA / 93) + 1; i++)
    {
        for (j = 0; j < (TELA_ALTURA / 92) + 1; j++)
        {
            al_draw_bitmap(jogo->fundo, 93 * i, 92 * j, 0);
        }
    }
}

void desenhar_parede(JOGO *jogo)
{
    int i, j;

    for (i = 0; i < (TELA_LARGURA / 275) + 1; i++)
    {
        for (j = 0; j < 2; j++)
        {
            al_draw_rotated_bitmap(jogo->parede, 275 * (1 - j), 16 * (1 - j), 275 * i, (TELA_ALTURA - 16) * j, radianos(180) * (1 - j), 0);
        }
    }
    for (i = 0; i < (TELA_ALTURA / 275) + 1; i++)
    {
        for (j = 0; j < 2; j++)
        {
            al_draw_rotated_bitmap(jogo->parede, 275 * j, 16 * (1 - j), (TELA_LARGURA - 16) * j, 275 * i, radianos(90 + (180 * (j))), 0);
        }
    }
}

void desenhar_rabo(COBRA *cobra)
{

    float PI = acos(-1);
    float angulo;
    int i;
    CORPO rabo = cobra->vetCorpo[cobra->qtdCorpo];
    //int *pontos = ajustar_pontos_rabo(&cobra->vetCorpo[cobra->qtdCorpo]);
    //al_draw_filled_rectangle(pontos[0], pontos[1], pontos[2], pontos[3], al_map_rgba_f(0, 0, 0.5, 0.5));
    if (cobra->status != MORTA)
    {
        al_draw_rotated_bitmap(cobra->rabo_img, RABO_LARGURA / 2, 11, rabo.x, rabo.y, -radianos(270 + rabo.angulo), 0);
    }
}

void desenhar_corpo(COBRA *cobra)
{

    float PI = acos(-1);
    float angulo;
    int i;

    for (i = cobra->qtdCorpo - 1; i >= 0; i--)
    {
        CORPO corpo = cobra->vetCorpo[i];
        if (cobra->status != MORTA)
        {
            al_draw_rotated_bitmap(cobra->corpo_img, CORPO_LARGURA / 2, CORPO_ALTURA - 11, corpo.x, corpo.y, -radianos(270 + corpo.angulo), 0); //corpo
        }
        //int *pontos = ajustar_pontos_corpo(&cobra->vetCorpo[i]);
        //al_draw_filled_rectangle(pontos[0], pontos[1], pontos[2], pontos[3], al_map_rgba_f(0, 0, 0.5, 0.5));
    }
}

void desenhar_cabeca(JOGO *jogo, COBRA *cobra, int id)
{
    if (cobra->status != MORTA)
    {
        al_draw_rotated_bitmap(cobra->img[id], COBRA_LARGURA / 2 + 2, 11, cobra->x, cobra->y, -radianos(90 + cobra->angulo), 0); //aq desenha a cabeca*/
        //int *pontos = ajustar_pontos_cobra(cobra, -15, 0);
        //al_draw_filled_rectangle(pontos[0], pontos[1], pontos[2], pontos[3], al_map_rgba_f(0, 0, 0.5, 0.5));
    }
    else
    {
        al_draw_rotated_bitmap(jogo->rosa, COBRA_LARGURA / 2 + 2, 11, cobra->x, cobra->y, -radianos(90 + cobra->angulo), 0);
        //int *pontos = ajustar_pontos_rosa(cobra);
        //al_draw_filled_rectangle(pontos[0], pontos[1], pontos[2], pontos[3], al_map_rgba_f(0, 0, 0.5, 0.5));
    }
}

void desenhar_cobra(JOGO *jogo, COBRA *cobra, int id)
{
    for (size_t i = 0; i < 2; i++)
    {
        desenhar_rabo(cobra);
        desenhar_corpo(cobra);
        desenhar_cabeca(jogo, cobra, id);
    }
}

void desenhar_rato(ALIMENTO *alimento)
{
    //int *pontos = ajustar_pontos_rato(alimento);
    //al_draw_filled_rectangle(pontos[0], pontos[1], pontos[2], pontos[3], al_map_rgba_f(0, 0, 0.5, 0.9));
    if (alimento->timer_hide > 0 && alimento->timer_show == 0)
        al_draw_rotated_bitmap(alimento->img, RATO_LARGURA / 2, 0, alimento->x, alimento->y, -radianos(90 - alimento->angulo), 0);
}
void desenhar_queijo(ALIMENTO *alimento, JOGO *jogo)
{
    //int *pontos = ajustar_pontos_quijo(alimento);
    //al_draw_filled_rectangle(pontos[0], pontos[1], pontos[2], pontos[3], al_map_rgba_f(0.5, 0.5, 0, 0.5));
    if (alimento->timer_hide > 0 && alimento->timer_show == 0 && (jogo->jogadores[0].status == MORTA || jogo->jogadores[1].status == MORTA))
        al_draw_rotated_bitmap(alimento->img, RATO_LARGURA / 2, 0, alimento->x, alimento->y, -radianos(90 - alimento->angulo), 0);
}

void desenhar_play(ALLEGRO_BITMAP *play)
{
    al_draw_bitmap(play, 0, 0, 0);
}

/**
 * Metodo main dos desenhos
*/
void desenhar_jogo(JOGO *jogo)
{
    for (size_t i = 0; i < 2; i++)
        desenhar_cobra(jogo, &jogo->jogadores[i], animacao(jogo, i));
    desenhar_rato(&jogo->dourado);
    desenhar_rato(&jogo->marrom[0]);
    desenhar_rato(&jogo->marrom[1]);
    desenhar_rato(&jogo->preto);
    desenhar_rato(&jogo->vermelho);
    desenhar_queijo(&jogo->queijo, jogo);
}

//-------------------------------------------- DESENHAR TELAS --------------------------------------------
/**
 * Desenha as telas do jogo
*/

/***/
void desenhar_inicio(JOGO *jogo)
{

    al_draw_text(jogo->font_titulo, al_map_rgb(100, 221, 23), TELA_LARGURA / 2, 20, ALLEGRO_ALIGN_CENTER, "SNAKE WAR");

    //QUADRO DE INSTRUCOES
    int esc_x = 4, esc_y = 5;
    int x = 20, y = 150;
    al_draw_scaled_bitmap(jogo->img_quadro, 0, 0, al_get_bitmap_width(jogo->img_quadro), al_get_bitmap_height(jogo->img_quadro), x, y, 900 * esc_x, 105 * esc_y, 0); //(900,105)
    al_draw_text(jogo->font_sub_titulo, al_map_rgb(0, 0, 0), x + 50, y + 50, 0, "Instruções");

    int ps_sum = 40, i = 0, ps = 210 + 50, ps_x = 80 - 10;
    //int txt_sum = 40, txt_i = 0, txt = 210;
    al_draw_rotated_bitmap(jogo->marrom->img, RATO_LARGURA / 2, 0, ps_x, ps + ps_sum * i, radianos(0), 0);
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), ps_x + 15, ps + 15 + ps_sum * i++, 0, "Cresce a cobra");
    al_draw_rotated_bitmap(jogo->preto.img, RATO_LARGURA / 2, 0, ps_x, ps + ps_sum * i, radianos(0), 0);
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), ps_x + 15, ps + 15 + ps_sum * i++, 0, "Encolhe a cobra em um corpo e mata se houver apenas um corpo");
    al_draw_rotated_bitmap(jogo->vermelho.img, RATO_LARGURA / 2, 0, ps_x, ps + ps_sum * i, radianos(0), 0);
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), ps_x + 15, ps + 15 + ps_sum * i++, 0, "Deixa mais lento");
    al_draw_rotated_bitmap(jogo->dourado.img, RATO_LARGURA / 2, 0, ps_x, ps + ps_sum * i, radianos(0), 0);
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), ps_x + 15, ps + 15 + ps_sum * i++, 0, "Deixa mais rapido");
    al_draw_rotated_bitmap(jogo->rosa, RATO_LARGURA / 2, 0, ps_x, ps + ps_sum * i, radianos(0), 0);
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), ps_x + 15, ps + 15 + ps_sum * i++, 0, "Quando a cobra morre, recebe mais uma chance com o rato rosa");
    al_draw_rotated_bitmap(jogo->queijo.img, RATO_LARGURA / 2, 0, ps_x, ps + ps_sum * i + 10, radianos(0), 0);
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), ps_x + 15, ps + 15 + ps_sum * i++, 0, "Se o rato rosa comer o queijo torna-se cobra novamente");
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), ps_x - 5, ps + 15 + ps_sum * i, 0, "Uma cobra vira rato se tocar na outra maior");
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), ps_x - 5, ps + 30 + ps_sum * i++, 0, "Uma cobra morre se bater na cabeça da outra maior");
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), ps_x - 5, ps + 30 + ps_sum * i++, 0, "(SPACE) Inicia, pausa e retorna o jogo");
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), ps_x - 5, ps + 30 + ps_sum * i++, 0, "(ESC) Sair do jogo");

    //QUADRO JOGADOR 1
    static int id = 0;
    id = (id + 1) % 14;
    int esc2_x = 2, esc2_y = 2;
    int x2 = 800, y2 = 160;
    int xj2 = 967, yj2 = 260;
    al_draw_scaled_bitmap(jogo->img_quadro, 0, 0, al_get_bitmap_width(jogo->img_quadro), al_get_bitmap_height(jogo->img_quadro), x2, y2, 900 * esc2_x, 105 * esc2_y, 0); //(900,105)
    al_draw_text(jogo->font_sub_titulo, al_map_rgb(0, 0, 0), 860, 175, 0, "JOGADOR I");
    iniciar_jogadores(&jogo->cobras[jogo->jogador1], xj2, yj2, 0);
    desenhar_cobra(jogo, &jogo->cobras[jogo->jogador1], id);
    al_draw_bitmap(jogo->direcional, 825, 305, 0);
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), 890, 305, 0, "Controle a cobra no jogo");
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), 890, 325, 0, "Escolha a cobra");

    //QUADRO JOGADOR 2
    int x3 = 800, y3 = 425;                                                                                                                                              //caixa
    int xj3 = 967, yj3 = 525;                                                                                                                                            //cobra
    al_draw_scaled_bitmap(jogo->img_quadro, 0, 0, al_get_bitmap_width(jogo->img_quadro), al_get_bitmap_height(jogo->img_quadro), x3, y3, 900 * esc2_x, 105 * esc2_y, 0); //(900,105)
    al_draw_text(jogo->font_sub_titulo, al_map_rgb(0, 0, 0), 860, 445, 0, "JOGADOR II");
    iniciar_jogadores(&jogo->cobras[jogo->jogador2], xj3, yj3, 0);
    desenhar_cobra(jogo, &jogo->cobras[jogo->jogador2], id);
    al_draw_bitmap(jogo->wasd, 825, 570, 0);
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), 890, 570, 0, "Controle a cobra no jogo");
    al_draw_text(jogo->font_corpo, al_map_rgb(0, 0, 0), 890, 590, 0, "Escolha a cobra");
}

void desenhar_pause(JOGO *jogo)
{
    //al_draw_text(jogo->font_titulo, al_map_rgb(100, 221, 23), TELA_LARGURA / 2, TELA_ALTURA / 2 - 50, ALLEGRO_ALIGN_CENTER, "ESPASSO");
    al_draw_bitmap(jogo->img_play, TELA_LARGURA / 2 - 50, TELA_ALTURA / 2 - 50, 0);
}

void desenhar_fim(JOGO *jogo)
{
    float esc_x = 3, esc_y = 4;
    int x = 50, y = 150;
    al_draw_scaled_bitmap(jogo->img_quadro, 0, 0, al_get_bitmap_width(jogo->img_quadro), al_get_bitmap_height(jogo->img_quadro), x, y, 900 * esc_x, 105 * esc_y, 0); //(900,105)
    char *txt;
    ALLEGRO_COLOR cor;
    if (jogo->jogadores[0].status != MORTA)
    {
        txt = "Vencedor";
        cor = al_map_rgb(27, 33, 10);
    }
    else
    {
        txt = "Derrotado";
        cor = al_map_rgb(255, 0, 0);
    }
    al_draw_text(jogo->font_sub_titulo, al_map_rgb(0, 0, 0), x + 50, y + 40, 0, "Jogador I");
    al_draw_text(jogo->font_sub_titulo, cor, x + 275, y + 40, 0, txt);
    al_draw_bitmap(jogo->direcional, x + 50, y + 280, 0);
    int dist = 1;
    SCORE sc = jogo->jogadores[0].score;
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120, 0, "Maior corpo adiquirido ------------------------- %03d", sc.qtdCorpo);
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120 + 20 * dist++, 0, "Ratos pretos comidos -------------------------- %03d", sc.qtdPreto);
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120 + 20 * dist++, 0, "Ratos marrons comidos ------------------------ %03d", sc.qtdMarrom);
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120 + 20 * dist++, 0, "Ratos dourados comidos ----------------------- %03d", sc.qtdDourado);
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120 + 20 * dist++, 0, "Ratos vermelhos comidos ---------------------- %03d", sc.qtdVermelho);
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120 + 20 * dist++, 0, "Quantidades de vezes que morreu -------------- %03d", sc.qtdMorte);
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120 + 20 * dist++, 0, "Trasformou adversario em rato rosa ----------- %03d", sc.trasformouEmRato);
    al_draw_text(jogo->font_sub_titulo, al_map_rgb(0, 0, 0), x + 50, y + 340, 0, "Vitorias:");
    al_draw_textf(jogo->font_numb, al_map_rgb(0, 0, 0), x + 200 + 5, y + 345, 0, "%d", sc.vitorias);

    x += 600;
    al_draw_scaled_bitmap(jogo->img_quadro, 0, 0, al_get_bitmap_width(jogo->img_quadro), al_get_bitmap_height(jogo->img_quadro), x, y, 900 * esc_x, 105 * esc_y, 0); //(900,105)
    if (jogo->jogadores[1].status != MORTA)                                                                                                                          //inverte a comrparação
    {
        txt = "Vencedor";
        cor = al_map_rgb(27, 33, 10);
    }
    else
    {
        txt = "Derrotado";
        cor = al_map_rgb(255, 0, 0);
    }
    al_draw_text(jogo->font_sub_titulo, al_map_rgb(0, 0, 0), x + 50, y + 40, 0, "Jogador II");
    al_draw_text(jogo->font_sub_titulo, cor, x + 275, y + 40, 0, txt);
    dist = 1;
    sc = jogo->jogadores[1].score;
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120, 0, "Maior corpo adiquirido ------------------------- %03d", sc.qtdCorpo);
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120 + 20 * dist++, 0, "Ratos pretos comidos -------------------------- %03d", sc.qtdPreto);
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120 + 20 * dist++, 0, "Ratos marrons comidos ------------------------ %03d", sc.qtdMarrom);
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120 + 20 * dist++, 0, "Ratos dourados comidos ----------------------- %03d", sc.qtdDourado);
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120 + 20 * dist++, 0, "Ratos vermelhos comidos ---------------------- %03d", sc.qtdVermelho);
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120 + 20 * dist++, 0, "Quantidades de vezes que morreu -------------- %03d", sc.qtdMorte);
    al_draw_textf(jogo->font_corpo, al_map_rgb(0, 0, 0), x + 50, y + 120 + 20 * dist++, 0, "Trasformou adversario em rato rosa ------------ %03d", sc.trasformouEmRato);
    al_draw_text(jogo->font_sub_titulo, al_map_rgb(0, 0, 0), x + 50, y + 340, 0, "Vitorias:");
    al_draw_bitmap(jogo->wasd, x + 50, y + 280, 0);
    al_draw_textf(jogo->font_numb, al_map_rgb(0, 0, 0), x + 200 + 5, y + 345, 0, "%d", sc.vitorias);
}

//-------------------------------------------- METODOS MAIN DO JOGO --------------------------------------------

int main()
{
    JOGO jogo;
    jogo.frames = 0;
    jogo.estado = INICIO;

    iniciar(al_init(), "allegro");
    iniciar(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER *tempo = al_create_timer(1.0 / 40.0); //So ta testes
    iniciar(tempo, "timer");

    ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
    iniciar(fila_eventos, "fila");

    iniciar_display(&jogo);

    iniciar(al_init_image_addon(), "image");

    iniciar_imagem(&jogo);

    iniciar(al_init_primitives_addon(), "primitives");

    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(jogo.disp));
    al_register_event_source(fila_eventos, al_get_timer_event_source(tempo));

    keyboard_init();

    iniciar_audio(&jogo);
    al_init_font_addon();
    al_init_ttf_addon();

    bool done = false;
    bool spc_press = false;
    bool spc_releaze = false;
    bool right_press = false;
    bool right_releaze = false;
    bool left_press = false;
    bool left_releaze = false;
    bool a_press = false;
    bool a_releaze = false;
    bool d_press = false;
    bool d_releaze = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(tempo);

    jogo.font_titulo = al_load_font("C.A. Gatintas.ttf", 75, 0);
    jogo.font_sub_titulo = al_load_font("C.A. Gatintas.ttf", 35, 0);
    jogo.font_corpo = al_load_font("roboto.ttf", 16, 0);
    jogo.font_numb = al_load_font("roboto.ttf", 35, 0);

    jogo.marrom[0] = init_rato_marrom(&jogo);
    jogo.marrom[1] = init_rato_marrom(&jogo);
    jogo.preto = init_rato_preto(&jogo);
    jogo.vermelho = init_rato_vermelho(&jogo);
    jogo.dourado = init_rato_dourado(&jogo);
    jogo.queijo = init_queijo(&jogo);
    jogo.rosa = econtrar_imagem(&jogo, 389, 420, RATO_LARGURA, RATO_ALTURA);

    jogo.cobras[0] = init_branca(&jogo);
    jogo.cobras[1] = init_verde(&jogo);
    jogo.cobras[2] = init_vermelha(&jogo);
    jogo.cobras[3] = init_preta(&jogo);

    jogo.img_quadro = econtrar_imagem(&jogo, 410, 9, 900, 95);
    jogo.parede = econtrar_imagem(&jogo, 306, 478, 275, 16);
    jogo.fundo = econtrar_imagem(&jogo, 311, 10, 93, 92);
    jogo.img_play = econtrar_imagem(&jogo, 300, 110, 115, 150);

    jogo.direcional = econtrar_imagem(&jogo, 420, 110, 55, 105);
    jogo.wasd = econtrar_imagem(&jogo, 480, 110, 60, 95);

    iniciar_alimentos(&jogo);
    tocar_audio(jogo.inicioFim, 0.3, ALLEGRO_PLAYMODE_LOOP);
    for (size_t i = 0; i < 2; i++)
    {
        jogo.jogadores[i].score.vitorias = 0;
    }
    jogo.jogador1 = 1;
    jogo.jogador2 = 2;
    while (1)
    {
        al_wait_for_event(fila_eventos, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_TIMER:
            if (jogo.estado == PLAY)
                atualizar_jogo(&jogo);
            if (key[ALLEGRO_KEY_SPACE])
            {
                spc_releaze = false;
                spc_press = true;
            }
            else
            {
                spc_releaze = true;
            }
            if (spc_releaze && spc_press)
            {
                spc_press = false;
                int xj, yj;
                switch (jogo.estado)
                {
                case INICIO:
                    xj = (TELA_LARGURA / 2) - (COBRA_LARGURA / 2);
                    yj = (TELA_ALTURA / 2) - COBRA_ALTURA;
                    iniciar_jogadores(&jogo.jogadores[0], xj, yj, 0);
                    yj = (TELA_ALTURA / 2) + COBRA_ALTURA;
                    iniciar_jogadores(&jogo.jogadores[1], xj, yj, 180);
                    jogo.cobras[jogo.jogador1].score.vitorias = jogo.jogadores[0].score.vitorias;
                    jogo.cobras[jogo.jogador2].score.vitorias = jogo.jogadores[1].score.vitorias;
                    jogo.jogadores[0] = jogo.cobras[jogo.jogador1];
                    jogo.jogadores[1] = jogo.cobras[jogo.jogador2];
                    xj = (TELA_LARGURA / 2) - (COBRA_LARGURA / 2);
                    yj = (TELA_ALTURA / 2) - COBRA_ALTURA;
                    iniciar_jogadores(&jogo.jogadores[0], xj, yj, 0);
                    yj = (TELA_ALTURA / 2) + COBRA_ALTURA;
                    iniciar_jogadores(&jogo.jogadores[1], xj, yj, 180);
                    al_stop_samples();
                    tocar_audio(jogo.tema, 0.3, ALLEGRO_PLAYMODE_LOOP);
                    jogo.estado = PLAY;
                    break;
                case PLAY:
                    jogo.estado = PAUSE;
                    break;
                case PAUSE:
                    jogo.estado = PLAY;
                    break;
                case FIM:
                    jogo.estado = INICIO;
                    break;
                }
            }
            // Reconhece o evento apenas quando soltar a tecla
            if (key[ALLEGRO_KEY_RIGHT])
            {
                right_releaze = false;
                right_press = true;
            }
            else
            {
                right_releaze = true;
            }
            if (right_releaze && right_press)
            {
                right_press = false;
                jogo.jogador1 = (jogo.jogador1 + 1) % 4;
                if (jogo.jogador1 == jogo.jogador2)
                    jogo.jogador1 = (jogo.jogador1 + 1) % 4;
            }
            //////////
            if (key[ALLEGRO_KEY_LEFT])
            {
                left_releaze = false;
                left_press = true;
            }
            else
            {
                left_releaze = true;
            }
            if (left_releaze && left_press)
            {
                left_press = false;
                jogo.jogador1 = (jogo.jogador1 - 1 < 0) ? 3 : jogo.jogador1 - 1;
                if (jogo.jogador1 == jogo.jogador2)
                    jogo.jogador1 = (jogo.jogador1 - 1 < 0) ? 3 : jogo.jogador1 - 1;
            }
            if (key[ALLEGRO_KEY_A])
            {
                a_releaze = false;
                a_press = true;
            }
            else
            {
                a_releaze = true;
            }
            if (a_releaze && a_press)
            {
                a_press = false;
                jogo.jogador2 = (jogo.jogador2 + 1) % 4;
                if (jogo.jogador2 == jogo.jogador1)
                    jogo.jogador2 = (jogo.jogador2 + 1) % 4;
            }
            if (key[ALLEGRO_KEY_D])
            {
                d_releaze = false;
                d_press = true;
            }
            else
            {
                d_releaze = true;
            }
            if (d_releaze && d_press)
            {
                d_press = false;
                jogo.jogador2 = (jogo.jogador2 - 1 < 0) ? 3 : jogo.jogador2 - 1;
                if (jogo.jogador2 == jogo.jogador1)
                    jogo.jogador2 = (jogo.jogador2 - 1 < 0) ? 3 : jogo.jogador2 - 1;
            }
            if (key[ALLEGRO_KEY_ESCAPE])
            {
                done = true;
            }
            redraw = true;
            jogo.frames++;
            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        if (done)
            break;

        keyboard_update(&event);

        if (redraw && al_is_event_queue_empty(fila_eventos))
        {
            preparar_display(&jogo);
            al_clear_to_color(al_map_rgb(0, 0, 0));
            desenhar_fundo(&jogo);
            desenhar_parede(&jogo);
            switch (jogo.estado)
            {
            case INICIO:
                desenhar_inicio(&jogo);
                break;
            case PLAY:
                desenhar_jogo(&jogo);
                break;
            case PAUSE:
                desenhar_pause(&jogo);
                break;
            case FIM:
                desenhar_fim(&jogo);
            default:
                break;
            }
        }
        desenhar_display(&jogo);
    }

    for (size_t i = 0; i < 2; i++)
    {
        free(jogo.jogadores[i].vetCorpo);
    }
    destruir_imagem(&jogo);

    //disp_deinit();
    al_destroy_font(jogo.font_titulo);
    al_destroy_font(jogo.font_corpo);
    al_destroy_font(jogo.font_sub_titulo);
    al_destroy_font(jogo.font_numb);
    destruir_display(&jogo);
    al_destroy_timer(tempo);

    al_destroy_event_queue(fila_eventos);
    destruir_audio(&jogo);

    return 0;
}