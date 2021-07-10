OUTPUT = run

all: output

output:
	gcc main.c Node/node.c Configuration/configuration.c parser.c Reactor/select.c -o ${OUTPUT}