OUTPUT = run
# INCLUDES = 
all: output

output:
	gcc -g main.c Node/node.c Configuration/configuration.c Parser/cli_parser.c Reactor/select.c Reactor/handler.c Protocol/message.c -o ${OUTPUT}