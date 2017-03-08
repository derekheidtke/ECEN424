all: server.C client.C
	g++ server.C -o "echos"
	g++ client.C -o "echo"

server: server.C
	g++ server.C -o "echos"

client: client.C
	g++ client.C -o "echo"

clean:
	rm "echos" "echo" "myecho"