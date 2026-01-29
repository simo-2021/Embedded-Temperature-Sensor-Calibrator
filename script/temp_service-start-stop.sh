#!/bin/sh
# Script de démarrage/arrêt du service température (calibré)

# Chemins
SENSOR_BIN="/usr/bin/temp_sensor"
SERVER_BIN="/usr/bin/temp_server"
DATA_FILE="/var/tmp/temp_data"

case "$1" in
    start)
        echo "Démarrage capteur + service température..."
        # Lancer capteur (génère valeur calibrée)
        $SENSOR_BIN
        # Lancer service TCP en daemon
        $SERVER_BIN &
        ;;
    stop)
        echo "Arrêt service température..."
        # Arrêter le service (gracieux)
        if ps aux | grep temp_server | grep -v grep > /dev/null; then
            PID=$(ps aux | grep temp_server | grep -v grep | awk '{print $2}')
            kill "$PID"
            # Nettoyer fichier de données (sécurité)
            rm -f "$DATA_FILE"
        fi
        ;;
    restart)
        $0 stop
        sleep 1
        $0 start
        ;;
    *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
        ;;
esac

exit 0
