all:	folders server client

server:	bin/Controller

client:	bin/Runner

folders: 
	@mkdir -p src bin temp

bin/Controller: folders
	gcc -Wall -g Controller.c -o bin/Controller

bin/Runner: folders
	gcc -Wall -g Runner.c -o bin/Runner

clean:
	rm -f temp/* bin/{Controller,Runner}	

run: 
	./bin/Controller &
	./bin/Runner




