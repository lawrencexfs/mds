package main

import (
	"context"
	"flag"
	"log"
	"mds/api"
	"mds/chat"
	"net/http"
	_ "net/http/pprof"
	"os"
	"os/signal"
	"syscall"

	"github.com/gorilla/mux"
)

/*
#cgo CFLAGS: -I./cmd/cgo-example-goexe
#cgo LDFLAGS: -L./cmd/cgo-example-goexe -lcalc
#include "calc.h"
*/
import "C"

var (
	confFile = flag.String("conf", "conf.yml", "The configure file")
	pprof    = flag.String("pprof", "127.0.0.1:6060", "[localhost:6060]start debug page.")
	version  = "over"
)

func main() {

	flag.Parse()
	// defer log.Flush()
	log.Println("Server start ...")

	if *pprof != "" {
		go func() {
			log.Println(http.ListenAndServe(*pprof, nil))
		}()
	}

	// 创建聊天室服务
	chatService := chat.NewChatService()

	// 创建默认房间
	chatService.CreateRoom("IUNIT", "默认房间")

	// 创建路由器
	r := mux.NewRouter()

	// 注册路由
	r.HandleFunc("/", chat.HandleHome)
	r.HandleFunc("/ws", func(w http.ResponseWriter, r *http.Request) {
		chat.HandleWebSocket(chatService, w, r)
	})

	// 启动服务器
	port := "8082"
	log.Printf("服务器启动在端口 %s", port)

	go func() {
		log.Println(http.ListenAndServe(":"+port, r))
	}()

	api.Init(*confFile, version)

	ch := make(chan os.Signal)
	signal.Notify(ch, os.Interrupt, syscall.SIGTERM)

	ctx, cancel := context.WithCancel(context.Background())
	api.StartContext(ctx)

	<-ch
	//TODO 清理资源
	log.Println("收到 ctrl+c 命令....")
	log.Println("Server stop ...")
	cancel()
	api.GracefulStop()

}
