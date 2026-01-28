# Makefile for aesdsocket
# 07JAN2026 - Arnaud SImo


# Name of the Exe
TARGET = aesdsocket

# Sources
SRC = aesdsocket.c

# Ne PAS définir CC : Buildroot le fournit
# Ne PAS définir CROSS_COMPILE

# Ajouter nos flags sans écraser ceux de Buildroot
CFLAGS += -Wall -Werror 

# Règle par défaut
all: $(TARGET)

# Construction du binaire
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

# Nettoyage
clean:
	rm -f $(TARGET) *.o

distclean: clean
