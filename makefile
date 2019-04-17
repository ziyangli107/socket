all: aws1 serverA1 serverB1 serverC1 client1 monitor1

aws1: aws.c
	gcc -o aws aws.c
serverA1: servera.c
	gcc -o servera servera.c
serverB1: serverb.c
	gcc -o serverb serverb.c
serverC1: serverc.c
	gcc -o serverc serverc.c -lm
monitor1: monitor.c
	gcc -o monitor monitor.c
client1: client.c
	gcc -o client client.c 


.PHONY:serverA serverB serverC monitor aws clean

serverA:
	./servera
serverB:
	./serverb
serverC:
	./serverc
aws:
	./aws
monitor:
	./monitor
clean:
	rm -f aws servera serverb serverc monitor client
