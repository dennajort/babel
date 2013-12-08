NAME            =       testAudio

CXXFLAGS        =      	-Iinclude -g3

LDFLAGS         =	-lportaudio

CXX             =       g++

RM              =       rm -fv

SRC             =       src/main.cpp		\
			src/BabelPortAudio.cpp

OBJ             =       $(SRC:.cpp=.o)

all:	$(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re:     fclean all

.PHONY: clean fclean distclean
