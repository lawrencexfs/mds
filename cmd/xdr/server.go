package main

import (
	"bytes"
	"fmt"
	"mds/rpc/opinterface"
	"net"

	xdr "github.com/rasky/go-xdr/xdr2"
)

func process(conn net.Conn) {
	defer conn.Close() // 关闭连接
	for {
		// reader := bufio.NewReader(conn)
		// record := bytes.NewBuffer([]byte{})
		// record.Bytes()
		// recordReader := bytes.NewReader(record)

		// Unmarshall RPC message
		// var call sunrpc.RPCMsg
		// _, err = xdr.Unmarshal(c.recordReader, &call)

		// var buf [4096]byte
		buf := make([]byte, 4096)
		n, err := conn.Read(buf[:])
		if err != nil {
			fmt.Println("read from client failed, err:", err)
			break
		}

		recvStr := string(buf[:n])
		fmt.Println("read from client len ：", n)
		fmt.Println("read from client data ：", recvStr)

		conn.Write([]byte(recvStr)) // 回写发送数据

		if n == 0 {
			break
		}

		rReader := bytes.NewReader(buf[:n])
		var call opinterface.GfCommonReq
		if _, err := xdr.Unmarshal(rReader, &call); err != nil {
			fmt.Println("xdr.Unmarshal call failed, err:", err)
			break
		}
		fmt.Println("xdr.Unmarshal req:", call)

	}
}

func main() {

	listen, err := net.Listen("tcp", "0.0.0.0:9611")
	if err != nil {
		fmt.Println("listen failed, err:", err)
		return
	}
	for {
		conn, err := listen.Accept() // 建立连接
		if err != nil {
			fmt.Println("Accept failed, err:", err)
			continue
		}
		go process(conn) // 启动一个goroutine处理连接
	}

}
