PROJECT_NAME=paint

C_SOURCES=main.cpp

CC=g++

CC_FLAGS=-lglut	\
				 -lGL		\
				 -lGLU

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(C_SOURCES)
	$(CC) $(C_SOURCES) -o $(PROJECT_NAME) $(CC_FLAGS)

clean:
	$(RM) $(PROJECT_NAME)
