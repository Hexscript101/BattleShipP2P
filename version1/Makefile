# Configurazione del compilatore e dei flag
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

# Nome del file eseguibile finale
TARGET = battleship

# Regola standard (eseguita digitando semplicemente "make")
all: $(TARGET)

# Compilazione dell'eseguibile finale unendo i file oggetto
$(TARGET): main.o func.o
	$(CC) $(CFLAGS) main.o func.o -o $(TARGET)

# Compilazione di main.o (dipende da main.c e functions.h)
main.o: main.c functions.h
	$(CC) $(CFLAGS) -c main.c

# Compilazione di func.o (dipende da func.c e functions.h)
func.o: func.c functions.h
	$(CC) $(CFLAGS) -c func.c

# Comando per ripulire la cartella (eseguito con "make clean")
clean:
	rm -f *.o $(TARGET)
