CPP_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.c=.o)))
LD_FLAGS :=
CC_FLAGS := -I include/

pbproxy: $(OBJ_FILES)
	gcc -o $@ $^ $(LD_FLAGS)

obj/%.o: src/%.c
	gcc $(CC_FLAGS) -c -o $@ $<

clean :
	\rm -fr obj/*
	\rm -fr pbproxy
	\rm -fr *~
	\rm -fr src/*~
	\rm -fr include/*~
