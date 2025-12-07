#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// Fonction qui exécute l'action demandée sur le PID, il fontionne de manière indépendante pour l'instant mais le principe est celui ci
void action_processus(int pid, int choix) {
    switch (choix) {
        case 1: // MISE EN PAUSE
            // SIGSTOP force le processus à geler immédiatement
            if (kill(pid, SIGSTOP) == 0)
                printf("[OK] PID %d mis en pause (SIGSTOP).\n", pid);
            else
                perror("Erreur");
            break;

        case 2: // REPRISE
            // SIGCONT demande au processus de continuer s'il était stoppé
            if (kill(pid, SIGCONT) == 0)
                printf("[OK] PID %d a repris (SIGCONT).\n", pid);
            else
                perror("Erreur");
            break;

        case 3: // ARRÊT
            // SIGTERM demande gentiment au processus de s'arrêter
            if (kill(pid, SIGTERM) == 0)
                printf("[OK] Signal d'arrêt envoyé au PID %d (SIGTERM).\n", pid);
            else
                perror("Erreur");
            break;

        case 4: // REDÉMARRAGE (Reload)
            // SIGHUP est le standard pour demander à un processus de se recharger/redémarrer
            if (kill(pid, SIGHUP) == 0)
                printf("[OK] Signal de redémarrage/reload envoyé au PID %d (SIGHUP).\n", pid);
            else
                perror("Erreur");
            break;

        default:
            printf("Choix non reconnu.\n");
    }
}

int main(int argc, char *argv[]) {
    // 1. On récupère le PID en argument
    if (argc != 2) {
        printf("Usage: %s <PID>\n", argv[0]);
        return 1;
    }
    int target_pid = atoi(argv[1]);

    printf("--- Contrôleur de processus pour PID: %d ---\n", target_pid);
    printf("1 : Mise en pause\n");
    printf("2 : Reprise\n");
    printf("3 : Arrêt\n");
    printf("4 : Redémarrage (SIGHUP)\n");
    printf("0 : Quitter le programme\n");
    printf("--------------------------------------------\n");

    int choix = 0;

    // 2. Boucle pour lire l'entrée utilisateur
    while (1) {
        printf("\nEntrez votre action : ");
        if (scanf("%d", &choix) != 1) break; // Sortie si erreur de saisie

        if (choix == 0) break; // Quitter

        // 3. Exécution de l'action
        action_processus(target_pid, choix);
    }

    return 0;
}
