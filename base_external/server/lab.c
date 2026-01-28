#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAILLE_BUFFER 10 // Taille du buffer fixe (comme BUFFER_SIZE)

int main() {
    char recv_buffer[TAILLE_BUFFER] = {0}; // Buffer fixe pour les morceaux
    char *packet_buffer = NULL;            // Buffer dynamique pour accumuler
    size_t packet_len = 0;                 // Taille du packet buffer

    // Simuler la réception de 3 morceaux du client
    char *morceaux[] = {"Salut le ", "monde! Comment ", "ça va ?\nA bientôt"};
    int nb_morceaux = 3;

    for (int i=0; i<nb_morceaux; i++) {
        // Copier le morceau dans recv_buffer (simule read())
        strcpy(recv_buffer, morceaux[i]);
        ssize_t bytes_lus = strlen(recv_buffer);
        printf("\n Morceau %d reçu : '%s' (taille %zd)\n", i+1, recv_buffer, bytes_lus);

        // Chercher '\n' dans le morceau
        char *newline = strchr(recv_buffer, '\n');
        printf("newline %s", newline);
        if (newline != NULL) {
            // === PAQUET COMPLET ===
            size_t len_paquet = newline - recv_buffer + 1;

            // Agrandir le buffer et ajouter la partie jusqu'à '\n'
            packet_buffer = realloc(packet_buffer, packet_len + len_paquet);
            memcpy(packet_buffer + packet_len, recv_buffer, len_paquet);
            packet_len += len_paquet;

            // Traiter le paquet (afficher ici)
            printf(" Paquet complet reçu : ");
            fwrite(packet_buffer, 1, packet_len, stdout);

            // Réinitialiser le packet buffer
            free(packet_buffer);
            packet_buffer = NULL;
            packet_len = 0;

            // Gérer le reste après '\n'
            size_t reste = bytes_lus - len_paquet;
            if (reste > 0) {
                packet_buffer = malloc(reste);
                memcpy(packet_buffer, newline + 1, reste);
                packet_len = reste;
                printf(" Reste stocké : '%s' (taille %zu)\n", packet_buffer, packet_len);
            }
        } else {
            // === PAQUET INCOMPLET ===
            packet_buffer = realloc(packet_buffer, packet_len + bytes_lus);
            memcpy(packet_buffer + packet_len, recv_buffer, bytes_lus);
            packet_len += bytes_lus;
            printf(" Paquet incomplet : buffer stocké (taille %zu)\n", packet_len);
        }
    }

    // Nettoyage final
    if (packet_buffer != NULL) {
        free(packet_buffer);
    }

    return 0;
}
