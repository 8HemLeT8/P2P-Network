OUTPUT = run
# INCLUDES = 
all: output

output:
	gcc main.c Node/node.c Configuration/configuration.c Parser/parser.c Reactor/select.c Reactor/handler.c Protocol/message.c -o ${OUTPUT}