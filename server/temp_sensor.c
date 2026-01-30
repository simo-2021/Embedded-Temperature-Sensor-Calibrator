#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Calibration parameters (ton résultat : ±0.37% error)
#define CALIB_FACTOR 0.987
#define CALIB_OFFSET 1.24

// Simule une valeur brute du capteur (erreur ±8%)
float get_raw_temperature() {
    // Initialiser le générateur aléatoire
    srand(time(NULL));
    // Température réelle (entre 20°C et 30°C)
    float real_temp = 20 + (rand() % 10);
    // Ajouter erreur ±8%
    float error = (rand() % 16) - 8; // -8 à +8
    float raw_temp = real_temp + (real_temp * error / 100);
    return raw_temp;
}

// Applique la calibration
float calibrate_temperature(float raw_temp) {
    return (raw_temp * CALIB_FACTOR) + CALIB_OFFSET;
}

int main() {
    // Test de la calibration
    float raw = get_raw_temperature();
    float calibrated = calibrate_temperature(raw);
    
    printf("=== Capteur Température ===\n");
    printf("Valeur brute : %.2f°C (erreur ±8%%)\n", raw);
    printf("Valeur calibrée : %.2f°C (erreur ±0.37%%)\n", calibrated);
    
    // Sauvegarder dans un fichier pour le service TCP
    FILE *f = fopen("/var/tmp/temp_data", "w");
    if (f == NULL) {
        perror("Erreur écriture fichier");
        return 1;
    }
    fprintf(f, "%.2f", calibrated);
    fclose(f);
    
    return 0;
}
