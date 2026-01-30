#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#define PORT 9001
#define BUFFER_SIZE 1024
#define ALLOWED_IP "127.0.0.1" // Sécurité : seulement localhost

// Variable globale pour arrêter le daemon gracieusement
int running = 1;

// Gestion du signal SIGTERM (arrêt gracieux)
void handle_sigterm(int sig) {
    running = 0;
    printf("\nService température : arrêt gracieux demandé\n");
}

// Lit la température calibrée depuis le fichier (version robuste)
char* get_calibrated_temp() {
    static char buffer[BUFFER_SIZE];
    FILE *f = fopen("/var/tmp/temp_data", "r");
    
    // Gestion d'erreur si le fichier n'existe pas
    if (f == NULL) {
        snprintf(buffer, BUFFER_SIZE, "Erreur : fichier /var/tmp/temp_data absent (code erreur : %d)", errno);
        return buffer;
    }
    
    // Lire la valeur (éviter les dépassements de buffer)
    if (fgets(buffer, BUFFER_SIZE - 1, f) == NULL) {
        strcpy(buffer, "Erreur : fichier temp_data vide");
    }
    fclose(f);
    
    // Supprimer le retour à la ligne (si présent)
    buffer[strcspn(buffer, "\n")] = 0;
    return buffer;
}

int main() {
    int server_fd = -1, new_socket = -1;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    //char buffer[BUFFER_SIZE] = {0};
    
    // Enregistrer le gestionnaire de signal (pour arrêt gracieux)
    signal(SIGTERM, handle_sigterm);
    
    // 1. Créer socket (gestion d'erreur robuste)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        fprintf(stderr, "Erreur création socket : %d\n", errno);
        return EXIT_FAILURE;
    }
    
    // 2. Configurer socket (réutiliser port + adresse)
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        fprintf(stderr, "Erreur setsockopt : %d\n", errno);
        close(server_fd);
        return EXIT_FAILURE;
    }
    
    // 3. Configurer l'adresse du serveur
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // 4. Lier socket au port 9001
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        fprintf(stderr, "Erreur bind (port %d) : %d\n", PORT, errno);
        close(server_fd);
        return EXIT_FAILURE;
    }
    
    // 5. Écouter les connexions (max 3 clients en attente)
    if (listen(server_fd, 3) < 0) {
        fprintf(stderr, "Erreur listen : %d\n", errno);
        close(server_fd);
        return EXIT_FAILURE;
    }
    
    printf("Service température démarré sur port %d (seulement localhost)\n", PORT);
    
    // 6. Boucle daemon (avec gestion d'arrêt gracieux)
    while (running) {
        // Attendre une connexion (timeout de 1s pour vérifier `running`)
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        struct timeval timeout = {1, 0}; // 1s
        
        int activity = select(server_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (activity < 0 && errno != EINTR) {
            fprintf(stderr, "Erreur select : %d\n", errno);
            break;
        }
        
        // Si pas de connexion (timeout), continuer la boucle
        if (activity == 0) continue;
        
        // Accepter la connexion
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (new_socket < 0) {
            fprintf(stderr, "Erreur accept : %d\n", errno);
            continue;
        }
        
        // 7. Vérifier l'IP du client (sécurité)
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &address.sin_addr, client_ip, INET_ADDRSTRLEN);
        if (strcmp(client_ip, ALLOWED_IP) != 0) {
            send(new_socket, "Accès refusé : IP non autorisée\n", 30, 0);
            close(new_socket);
            continue;
        }
        
        // 8. Envoyer la température calibrée
        char *temp = get_calibrated_temp();
        send(new_socket, temp, strlen(temp), 0);
        
        // 9. Fermer la connexion
        close(new_socket);
        new_socket = -1;
    }
    
    // 10. Nettoyage (arrêt gracieux)
    if (server_fd != -1) close(server_fd);
    printf("Service température arrêté\n");
    return EXIT_SUCCESS;
}
