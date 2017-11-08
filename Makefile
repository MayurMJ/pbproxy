CPP_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.c=.o)))
#CPP_CLI_FILES := $(wildcard cli/*.c)
#OBJ_CLI_FILES := $(addprefix obj_cli/,$(notdir $(CPP_CLI_FILES:.c=.o)))
LD_FLAGS := -lm -lcrypto -lssl
CC_FLAGS := -I include/


all: pbproxy

#pbproxy_cli: $(OBJ_CLI_FILES)
#	gcc -o $@ $^ $(LD_FLAGS)

#obj_cli/%.o: cli/%.c
#	gcc $(CC_FLAGS) -c -o $@ $<

pbproxy: $(OBJ_FILES)
	gcc -o $@ $^ $(LD_FLAGS)

obj/%.o: src/%.c
	gcc $(CC_FLAGS) -c -o $@ $<

clean :
	\rm -fr obj/* obj_cli/*
	\rm -fr pbproxy pbproxy_cli
	\rm -fr *~
	\rm -fr src/*~ cli/*~
	\rm -fr include/*~
