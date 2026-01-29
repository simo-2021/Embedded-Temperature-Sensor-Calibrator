#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9001
#define BUFFER_SIZE 1024
#define ALLOWED_IP "127.0.0.1" // Sécurité : seulement localhost

// Lit la température calibrée depuis le fichier
char* get_calibrated_temp() {
    static char buffer[BUFFER_SIZE];
    FILE *f = fopen("/var/tmp/temp_data", "r");
    if (f == NULL) {
        strcpy(buffer, "Erreur : pas de données");
        return buffer;
    }
    fgets(buffer, BUFFER_SIZE, f);
    fclose(f);
    return buffer;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    
    // Créer socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Configurer socket (réutiliser port, etc.)
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Lier socket au port 9001
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Écouter connexions (mode daemon)
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Service température démarré sur port %d (seulement localhost)\n", PORT);
    
    // Boucle infinie (daemon)
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }
        
        // Sécurité : vérifier l'IP du client
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &address.sin_addr, client_ip, INET_ADDRSTRLEN);
        if (strcmp(client_ip, ALLOWED_IP) != 0) {
            send(new_socket, "Accès refusé : IP non autorisée\n", 30, 0);
            close(new_socket);
            continue;
        }
        
        // Envoyer la température calibrée
        char *temp = get_calibrated_temp();
        send(new_socket, temp, strlen(temp), 0);
        close(new_socket);
    }
    
    return 0;
}
