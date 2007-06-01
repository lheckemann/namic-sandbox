OBJS = QdecContrast.o \
	QdecDataTable.o \
	QdecFactor.o \
	QdecGlmDesign.o \
	QdecGlmFit.o \
	QdecGlmFitResults.o \
	QdecProject.o \
	QdecSubject.o \
	QdecUtilities.o \
	QdecCmd.o
CC = g++ -g
CFLAGS = -g
CXXFLAGS = -g

all: qdecCmd

qdecCmd: $(OBJS)
	$(CC) -o qdecCmd $(OBJS)

clean:
	rm -f $(OBJS) qdecCmd
