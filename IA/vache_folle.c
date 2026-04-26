/*
 * vache_folle.c — Test local d'un modèle GGUF via llama.cpp
 *
 * Compilation : voir le Makefile dans ce répertoire
 * Usage       : ./vache_folle [prompt]
 *               ./vache_folle "Qu'est-ce qu'une vache ?"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "llama.h"

/*
 * Chemin vers le modèle (relatif au répertoire d'exécution).
 * Pointe par défaut vers le modèle fine-tuné exporté depuis IA/finetune_fr.
 */
/*#define MODEL_PATH  "./finetune_fr/outputs/Falcon-H1-Tiny-fr-Q6_K.gguf"*/
#define MODEL_PATH  "./Falcon_FineTune.Q6_K.gguf"

/* Nombre maximum de tokens générés en réponse */
#define MAX_NEW_TOKENS  256

/* Taille de la fenêtre de contexte */
#define CTX_SIZE        2048

/* Formate un échange chat (system + user) en prompt compatible modèle instruct */
static char * build_chat_prompt(const struct llama_model * model, const char * user_prompt) {
    const char * tmpl = llama_model_chat_template(model, NULL);

    const struct llama_chat_message chat[] = {
        {
            .role = "system",
            .content = "Tu es VYA, une vache Tamagotchi. Tu reponds toujours en francais, en une a trois phrases courtes. Tu exprimes tes emotions, tu fais des sons de vache (Meuh !) et tu decris tes actions entre asterisques. Tu es affectueuse, expressive et attachante.",
        },
        {
            .role = "user",
            .content = user_prompt,
        },
    };

    int32_t needed = llama_chat_apply_template(tmpl, chat, 2, /*add_ass=*/true, NULL, 0);
    if (needed <= 0) {
        return NULL;
    }

    char * out = malloc((size_t)needed + 1);
    if (out == NULL) {
        return NULL;
    }

    int32_t written = llama_chat_apply_template(tmpl, chat, 2, /*add_ass=*/true, out, needed + 1);
    if (written <= 0) {
        free(out);
        return NULL;
    }

    out[written] = '\0';
    return out;
}

int main(int argc, char *argv[])
{
    const char *user_prompt = (argc > 1) ? argv[1] : "Tu as faim ?";

    /* ------------------------------------------------------------------ */
    /* 1. Initialisation du backend llama.cpp                              */
    /* ------------------------------------------------------------------ */
    llama_backend_init();

    /* ------------------------------------------------------------------ */
    /* 2. Chargement du modèle                                             */
    /* ------------------------------------------------------------------ */
    struct llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = 0;   /* CPU uniquement, pas de GPU requis */

    printf("Chargement du modèle : %s\n", MODEL_PATH);
    fflush(stdout);

    struct llama_model *model = llama_model_load_from_file(MODEL_PATH, model_params);
    if (model == NULL) {
        fprintf(stderr, "Erreur : impossible de charger le modèle '%s'\n"
                        "Vérifiez que le fichier .gguf est présent dans le "
                        "répertoire IA/.\n", MODEL_PATH);
        llama_backend_free();
        return 1;
    }

    /* Vocabulaire associé au modèle (nécessaire pour les fonctions vocab_*) */
    const struct llama_vocab *vocab = llama_model_get_vocab(model);

    /* ------------------------------------------------------------------ */
    /* 3. Création du contexte d'inférence                                 */
    /* ------------------------------------------------------------------ */
    struct llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx   = CTX_SIZE;
    ctx_params.n_batch = 512;

    struct llama_context *ctx = llama_init_from_model(model, ctx_params);
    if (ctx == NULL) {
        fprintf(stderr, "Erreur : impossible de créer le contexte llama\n");
        llama_model_free(model);
        llama_backend_free();
        return 1;
    }

    /* ------------------------------------------------------------------ */
    /* 4. Construction d'un prompt chat puis tokenisation                  */
    /* ------------------------------------------------------------------ */
    char * prompt = build_chat_prompt(model, user_prompt);
    if (prompt == NULL) {
        fprintf(stderr, "Erreur : impossible de formater le prompt chat\n");
        llama_free(ctx);
        llama_model_free(model);
        llama_backend_free();
        return 1;
    }

    /* Premier appel avec buf=NULL pour obtenir le nombre de tokens */
    int n_prompt = -llama_tokenize(vocab, prompt, (int)strlen(prompt),
                                   NULL, 0, /*add_special=*/true, /*parse_special=*/true);
    if (n_prompt <= 0) {
        fprintf(stderr, "Erreur de tokenisation\n");
        free(prompt);
        llama_free(ctx);
        llama_model_free(model);
        llama_backend_free();
        return 1;
    }

    llama_token *prompt_tokens = malloc((size_t)n_prompt * sizeof(llama_token));
    if (prompt_tokens == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        free(prompt);
        llama_free(ctx);
        llama_model_free(model);
        llama_backend_free();
        return 1;
    }

    llama_tokenize(vocab, prompt, (int)strlen(prompt),
                   prompt_tokens, n_prompt, /*add_special=*/true, /*parse_special=*/true);

    /* ------------------------------------------------------------------ */
    /* 5. Création du sampler (échantillonneur pour les gens qui comprennent pas l'anglais)*/
    /* ------------------------------------------------------------------ */
    struct llama_sampler *sampler = llama_sampler_chain_init(
        llama_sampler_chain_default_params());

    /* Sampler plus robuste qu'un simple dist() pour éviter une sortie vide */
    llama_sampler_chain_add(sampler, llama_sampler_init_top_k(40));
    llama_sampler_chain_add(sampler, llama_sampler_init_top_p(0.90f, 1));
    llama_sampler_chain_add(sampler, llama_sampler_init_min_p(0.05f, 1));
    llama_sampler_chain_add(sampler, llama_sampler_init_penalties(64, 1.10f, 0.0f, 0.0f));
    llama_sampler_chain_add(sampler, llama_sampler_init_temp(0.7f));
    llama_sampler_chain_add(sampler, llama_sampler_init_dist(/*seed=*/42));

    /* ------------------------------------------------------------------ */
    /* 6. Décodage du prompt (prefill)                                     */
    /* ------------------------------------------------------------------ */
    struct llama_batch batch = llama_batch_get_one(prompt_tokens, n_prompt);

    if (llama_decode(ctx, batch) != 0) {
        fprintf(stderr, "Erreur lors du décodage du prompt\n");
        free(prompt);
        free(prompt_tokens);
        llama_sampler_free(sampler);
        llama_free(ctx);
        llama_model_free(model);
        llama_backend_free();
        return 1;
    }

    /* ------------------------------------------------------------------ */
    /* 7. Génération token par token (En meme temps c'est le seul moyen)  */
    /* ------------------------------------------------------------------ */
    printf("\nPrompt   : %s\n", user_prompt);
    printf("Réponse  : ");
    fflush(stdout);

    char piece[256];

    for (int i = 0; i < MAX_NEW_TOKENS; i++) {
        llama_token new_token = llama_sampler_sample(sampler, ctx, /*idx=*/-1);

        /* Arrêt sur token de fin de génération */
        if (llama_vocab_is_eog(vocab, new_token))
            break;

        /* Conversion du token en texte */
        int n_piece = llama_token_to_piece(vocab, new_token,
                                           piece, (int)sizeof(piece) - 1,
                                           /*lstrip=*/0, /*special=*/false);
        if (n_piece > 0) {
            piece[n_piece] = '\0';
            printf("%s", piece);
            fflush(stdout);
        }

        /* Informer le sampler du token accepté (sinon il est capricieux)*/
        llama_sampler_accept(sampler, new_token);

        /* Décoder le nouveau token pour le prochain pas */
        struct llama_batch next_batch = llama_batch_get_one(&new_token, 1);
        if (llama_decode(ctx, next_batch) != 0) {
            fprintf(stderr, "\nErreur lors de la génération\n");
            break;
        }
    }

    printf("\n");

    /* ------------------------------------------------------------------ */
    /* 8. Libération des ressources (Eh on fait du C ou pas?)             */
    /* ------------------------------------------------------------------ */
    free(prompt);
    free(prompt_tokens);
    llama_sampler_free(sampler);
    llama_free(ctx);
    llama_model_free(model);
    llama_backend_free();

    return 0;
}
