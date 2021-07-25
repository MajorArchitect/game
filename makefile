IDIR =src
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=src/obj
LDIR =../lib

LIBS=-lglfw3 -lGL -lm -ldl -lXinerama -lXrandr -lXi -lXcursor -lX11 -lXxf86vm \
-lpthread src/glad.c

_DEPS = globals.h entity.h matvec.h shader.h physics.h world.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o entity.o matvec.o shader.o physics.o world.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

bin/game: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
