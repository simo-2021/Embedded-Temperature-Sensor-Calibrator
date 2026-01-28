// helloworld_daemon.c - Programme Hello World en daemon (avec fichier .h)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>

// Inclure notre bibliothèque daemon
#include "daemon_utils.h"

// Flag pour arrêter le daemon proprement
volatile sig_atomic_t stop_daemon = 0;

// Gestionnaire de signal (pour SIGINT/SIGTERM)
void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        stop_daemon = 1;
        syslog(LOG_INFO, "Daemon : Signal d'arrêt reçu, fermeture...");
    }
}

int main(int argc, char *argv[]) {
    int is_daemon = 0;

    // Vérifier l'option -d (mode daemon)
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        is_daemon = 1;
    }

    if (is_daemon) {
        // Utiliser la fonction du fichier .h pour devenir daemon
        if (become_daemon() != 0) {
            fprintf(stderr, "Erreur : impossible de démarrer le daemon\n");
            return EXIT_FAILURE;
        }

        // Initialiser le syslog (logs du daemon)
        openlog("helloworld_daemon", LOG_PID | LOG_CONS, LOG_USER);
        syslog(LOG_INFO, "Daemon démarré avec succès");

        // Configurer les signaux pour arrêter le daemon
        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);

        // Boucle principale du daemon (toutes les 5 secondes)
        while (!stop_daemon) {
            syslog(LOG_INFO, "Hello World (daemon mode)");
            sleep(5);
        }

        // Fermer le syslog
        syslog(LOG_INFO, "Daemon arrêté");
        closelog();
    } else {
        // Mode interactif (console)
        printf("Hello World (interactif mode)\n");
        printf("Pour lancer en daemon : %s -d\n", argv[0]);
        printf("Arrêter le daemon : kill $(pgrep helloworld_daemon)\n");
    }

    return EXIT_SUCCESS;
}
