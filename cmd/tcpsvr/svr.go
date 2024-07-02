package main

import (
	"bufio"
	"flag"
	"fmt"
	"net"
	"os"
)

var addr = flag.String("addr", ":8081", "The server listen addr")

func main() {
	flag.Parse()
	// 监听特定端口
	ln, err := net.Listen("tcp", *addr)
	if err != nil {
		fmt.Println("Error listening:", err.Error())
		os.Exit(1)
	}
	defer ln.Close()

	fmt.Println("TCP server listening on addr " + *addr)

	for {
		// 接受客户端连接
		conn, err := ln.Accept()
		if err != nil {
			fmt.Println("Error accepting connection:", err.Error())
			continue
		}

		// 处理客户端连接
		go handleConnection(conn)
	}
}

func handleConnection(conn net.Conn) {
	defer conn.Close()
	reader := bufio.NewReader(conn)

	for {
		const size = 1024
		buff := make([]byte, size)
		// 读取客户端发送的消息
		len, err := reader.Read(buff)
		if err != nil {
			fmt.Println("Error reading from connection:", err.Error())
			return
		}

		fmt.Printf("Message received: %s.len : %d \n", buff, len)
		fmt.Printf("Message from : %s \n", conn.RemoteAddr().String())

		reply := "ni hao <." + " to {} " + string(buff)
		// 回显消息给客户端
		_, err = conn.Write([]byte(reply))
		if err != nil {
			fmt.Println("Error writing to connection:", err.Error())
			return
		}
	}
}
