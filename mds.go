package main

import (
	"context"
	"flag"
	"fmt"
	"glog"
	"mds/api"
	"net/http"
	_ "net/http/pprof"
	"os"
	"os/signal"
	"syscall"
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
	defer glog.Flush()
	glog.Infoln("Server start ...")
	fmt.Println("Server start ...")

	if *pprof != "" {
		go func() {
			glog.Infoln(http.ListenAndServe(*pprof, nil))
		}()
	}

	api.Init(*confFile, version)

	ch := make(chan os.Signal)
	signal.Notify(ch, os.Interrupt, syscall.SIGTERM)

	ctx, cancel := context.WithCancel(context.Background())
	api.StartContext(ctx)

	<-ch
	//TODO 清理资源
	glog.Infoln("收到 ctrl+c 命令....")
	fmt.Println("Server stop ...")
	cancel()
	api.GracefulStop()

}
