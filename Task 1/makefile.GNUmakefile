COMPILER = g++
SRCMODULES = Set.cpp
OBJMODULES = $(SRCMODULES:.c=.o)
NAME = sets

%.o: %.cpp %.h
	$(COMPILER) $< -o $@

$(NAME): main.cpp $(OBJMODULES)
	$(COMPILER) $^ -o $@

run: $(NAME)
	./$(NAME)

clean:
	rm -f *.o $(NAME)