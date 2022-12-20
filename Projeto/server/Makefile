
# compilador a utilizar
CC= g++
# flags para compilar
CFLAGS= -g -Wall -Wextra
# gerar variáveis com listas ficheiros de cada tipo para usar nas várias regras
SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp, %.o, $(SRCS))
HEADERS=$(wildcard *.h)
PDFS=$(wildcard *.pdf)

# nome do executável a produzir, compilar com "make"
TARGET= gs

all: $(TARGET)

$(TARGET): $(OBJS) Makefile
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

# listar dependências. para simplificar, cada ficheiro .o depende do correspondente .c, de todos os ficheiros .h e da Makefile
%.o: %.cpp $(HEADERS) Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

# para forçar recompilação, dar o comando "make clean" e depois "make" ou o comando "make clean ; make"
clean:
	rm -f *.o

rm:
	rm SCORES/*; rm -rf GAMES/*;
