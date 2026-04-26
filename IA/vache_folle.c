#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // pour usleep

#include "llama.h"

#define MODEL_PATH  "./Falcon_FineTune.Q6_K.gguf"
#define MAX_NEW_TOKENS  256
#define CTX_SIZE        2048

#define MAX_TEXT   4096
#define BUBBLE_W   40

void clear_screen() { printf("\033[H\033[J"); fflush(stdout); }
void hide_cursor()  { printf("\033[?25l");    fflush(stdout); }
void show_cursor()  { printf("\033[?25h");    fflush(stdout); }

static char * build_chat_prompt(const struct llama_model * model, const char * user_prompt) {
    const char * tmpl = llama_model_chat_template(model, NULL);

    const struct llama_chat_message chat[] = {
        {
            .role = "system",
            .content = "Tu es VYA, une vache Tamagotchi. Tu reponds toujours en francais, en une a trois phrases courtes. Tu fais des sons de vache (Meuh !) et tu decris tes actions entre asterisques.",
        },
        {
            .role = "user",
            .content = user_prompt,
        },
    };

    int32_t needed = llama_chat_apply_template(tmpl, chat, 2, true, NULL, 0);
    if (needed <= 0) return NULL;

    char * out = malloc((size_t)needed + 1);
    if (!out) return NULL;

    int32_t written = llama_chat_apply_template(tmpl, chat, 2, true, out, needed + 1);
    if (written <= 0) {
        free(out);
        return NULL;
    }

    out[written] = '\0';
    return out;
}

void affiche_reading_cow(const char *swallowed, char current) {  // On reutilise les fonctions de reading_cow
    int len = strlen(swallowed);
    int bub_len = len < BUBBLE_W ? len : BUBBLE_W;
    if (bub_len < 5) bub_len = 5;

    printf(" ");
    for (int i = 0; i < bub_len + 2; i++) printf("_");
    printf("\n");

    if (len == 0) {
        printf("< %-*s >\n", bub_len, "");
    } else {
        const char *display = (len > bub_len) ? swallowed + len - bub_len : swallowed;
        printf("< %-*s >\n", bub_len, display);
    }

    printf(" ");
    for (int i = 0; i < bub_len + 2; i++) printf("-");
    printf("\n");

    printf("        \\  ^__^\n");

    if (current == '\0')
        printf("         \\ (^^)\\_______\n");
    else
        printf("         \\ (oo)\\_______\n");

    printf("           (__)\\       )\\/\\\n");

    if (current != '\0' && current != '\n')
        printf("            %c  ||----w |\n", current);
    else
        printf("               ||----w |\n");

    printf("               ||     ||\n");

    fflush(stdout);
}

int main(int argc, char *argv[])
{
    system("chcp 65001"); //pour l'utf-8

    const char *user_prompt = (argc > 1) ? argv[1] : "Tu as faim ?";

    /* Désactive tous les logs de llama.cpp */
    llama_log_set(NULL, NULL);

    /* Initialisation */
    llama_backend_init();

    struct llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = 0;

    printf("Chargement du modèle : %s\n", MODEL_PATH);
    printf("Veuillez relancer le pogramme si il s'arrete ici (c'est normal.) \n");

    struct llama_model *model = llama_model_load_from_file(MODEL_PATH, model_params);
    if (!model) {
        fprintf(stderr, "Erreur chargement modèle\n");
        return 1;
    }

    const struct llama_vocab *vocab = llama_model_get_vocab(model);

    struct llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = CTX_SIZE;
    ctx_params.n_batch = 512;

    struct llama_context *ctx = llama_init_from_model(model, ctx_params);
    if (!ctx) {
        fprintf(stderr, "Erreur contexte\n");
        return 1;
    }

    char *prompt = build_chat_prompt(model, user_prompt);
    if (!prompt) return 1;

    int n_prompt = -llama_tokenize(vocab, prompt, strlen(prompt), NULL, 0, true, true);

    llama_token *prompt_tokens = malloc(n_prompt * sizeof(llama_token));
    llama_tokenize(vocab, prompt, strlen(prompt), prompt_tokens, n_prompt, true, true);

    struct llama_sampler *sampler = llama_sampler_chain_init(
        llama_sampler_chain_default_params());

    llama_sampler_chain_add(sampler, llama_sampler_init_top_k(40));
    llama_sampler_chain_add(sampler, llama_sampler_init_top_p(0.9f, 1));
    llama_sampler_chain_add(sampler, llama_sampler_init_temp(0.7f));
    llama_sampler_chain_add(sampler, llama_sampler_init_dist(42));

    struct llama_batch batch = llama_batch_get_one(prompt_tokens, n_prompt);
    llama_decode(ctx, batch);

    printf("\nPrompt : %s\n", user_prompt);

    char piece[256];

    // buffer global (corrigé)
    char sw[MAX_TEXT] = "";
    int sw_len = 0;

    for (int i = 0; i < MAX_NEW_TOKENS; i++) {

        llama_token new_token = llama_sampler_sample(sampler, ctx, -1);

        if (llama_vocab_is_eog(vocab, new_token))
            break;

        int n_piece = llama_token_to_piece(
            vocab, new_token,
            piece, sizeof(piece) - 1,
            0, false
        );

        if (n_piece > 0) {
            piece[n_piece] = '\0';

            for (int j = 0; j < n_piece; j++) {

                if (sw_len < MAX_TEXT - 1) {
                    sw[sw_len++] = piece[j];
                    sw[sw_len] = '\0';
                }

                clear_screen();
                affiche_reading_cow(sw, piece[j]);
                usleep(50000);
            }
        }

        llama_sampler_accept(sampler, new_token);

        struct llama_batch next = llama_batch_get_one(&new_token, 1);
        if (llama_decode(ctx, next) != 0)
            break;
    }
    printf("\n");

    free(prompt);
    free(prompt_tokens);
    llama_sampler_free(sampler);
    llama_free(ctx);
    llama_model_free(model);
    llama_backend_free();

    return 0;
}
