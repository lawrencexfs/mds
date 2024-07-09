package main

import (
	"context"
	"flag"
	"log"
	"os"
	"os/signal"
	"sync"
	"syscall"
	"time"

	"github.com/go-zeromq/zmq4"
)

// GOOS=windows  GOARCH=amd64 go build -o zmqcli.exe
// go build -o cli
// ./cli --num=1000 --print

var addr = flag.String("addr", "10.0.20.2:9011", "The server addr")
var num = flag.Int("num", 10, "test num")
var bprint = flag.Bool("print", false, "print message")

var svrAddr string

func main() {
	flag.Parse()
	ch := make(chan os.Signal)
	signal.Notify(ch, os.Interrupt, syscall.SIGTERM)

	ctx, cancel := context.WithCancel(context.Background())

	var wg sync.WaitGroup

	start := time.Now()
	svrAddr = "tcp://" + *addr
	for i := 0; i < *num; i++ {
		wg.Add(1)
		go Handle(ctx, &wg)
	}

	wg.Wait()
	now := time.Now()
	// 计算从开始时间到现在的时间差
	duration := now.Sub(start)
	log.Println("批量任务执行的时间:", duration)

	<-ch
	cancel()
	//TODO 清理资源
	log.Println("收到退出命令....over")
	// 主程序等待一段时间，以确保协程有时间响应取消信号
	time.Sleep(300 * time.Millisecond)
}

func Handle(ctx context.Context, wg *sync.WaitGroup) {
	req := zmq4.NewReq(nil)
	defer req.Close()

	err := req.Dial(svrAddr)
	if err != nil {
		log.Fatalf("could not connect: %v", err)
	}

	msg := zmq4.NewMsgString("Hello")
	if err := req.Send(msg); err != nil {
		log.Fatalf("could not send message: %v", err)
	}

	reply, err := req.Recv()
	if err != nil {
		log.Fatalf("could not receive reply: %v", err)
	}
	if *bprint {
		log.Println("Received reply:", string(reply.Bytes()))
	}
	// log.Println("Received reply:", string(reply.Bytes()))
	// log.Println(".")
	wg.Done()

	// for {
	// 	select {
	// 	case <-ctx.Done():
	// 		log.Println("任务退出")
	// 		return
	// 	default:
	// 		log.Println("等待中")
	// 		time.Sleep(200 * time.Millisecond)
	// 	}
	// }

}
