package main

import (
	"context"
	"flag"
	"net/http"
	_ "net/http/pprof"
	"os"
	"os/signal"
	"syscall"
	"mds/api"
	"glog"

)

var (
	confFile    = flag.String("conf", "conf.yml", "The configure file")
	showVersion = flag.Bool("version", false, "show build version.")
	pprof       = flag.String("pprof", "", "[localhost:6060]start debug page.")
	buildstamp  = "UNKOWN"
	githash     = "UNKOWN"
	version     = "UNKOWN"
)

func main() {

	flag.Parse()
	defer glog.Flush()

	if *showVersion {
		println(`Delivery version :`, version)
		println(`Git Commit Hash :`, githash)
		println(`UTC Build Time :`, buildstamp)
		os.Exit(0)
	}

	glog.Infoln("当前Alarm版本: ", version)
	glog.Infoln(`Git Commit Hash :`, githash)
	glog.Infoln(`UTC Build Time :`, buildstamp)

	if *pprof != "" {
		go func() {
			glog.Infoln(http.ListenAndServe(*pprof, nil))
		}()
	}

	// local := [...]uint8{80, 91, 80, 91, 0, 91, 80, 91, 80, 91, 80, 91, 80, 91, 90}
	// var str string = string(local[:])
	// fmt.Print("len: ", len(str), " ... ", str, "\n")
	// for i := 0; i < len(str); i++ {
	// 	fmt.Printf("%d .", str[i])
	// }
	// return

	api.Init(*confFile, version)

	ch := make(chan os.Signal)
	signal.Notify(ch, os.Interrupt, syscall.SIGTERM)

	ctx, cancel := context.WithCancel(context.Background())
	api.StartContext(ctx)

	<-ch
	//TODO 清理资源
	glog.Infoln("收到 ctrl+c 命令....")
	cancel()
	api.GracefulStop()

}
