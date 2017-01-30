all: server.C client.C

server: server.C
	g++ server.C -o "echos"

client: client.C
	g++ client.C -o "echo"

clean:
	rm "echos" "echo"