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
	// 连接到服务器
	conn, err := net.Dial("tcp", *addr)
	if err != nil {
		fmt.Println("Error connecting:", err.Error())
		os.Exit(1)
	}
	defer conn.Close()

	fmt.Println("Connected to TCP server. addr : ", *addr)

	// 从标准输入读取消息并发送给服务器
	reader := bufio.NewReader(os.Stdin)

	for {
		fmt.Print("Enter message: ")
		message, _ := reader.ReadString('\n')

		// 发送消息给服务器
		_, err := conn.Write([]byte(message))
		if err != nil {
			fmt.Println("Error writing to connection:", err.Error())
			return
		}

		// 接收服务器回显的消息
		reply, err := bufio.NewReader(conn).ReadString('\n')
		if err != nil {
			fmt.Println("Error reading from connection:", err.Error())
			return
		}

		fmt.Printf("Reply from server: %s", reply)
		fmt.Printf("Message from : %s\n", conn.RemoteAddr().String())

	}
}
