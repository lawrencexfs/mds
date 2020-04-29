package main

import (
	"flag"
	"fmt"
	"net"
	"net/http"
	_ "net/http/pprof"
)

var (
	confFile = flag.String("conf", "conf.yml", "The configure file read")
	pprof    = flag.String("pprof", "", "[localhost:6060]start debug page.")
	addr     = flag.String("addr", "127.0.0.1:22", "peer addr")
)

func main() {

	flag.Parse()
	if *pprof != "" {
		go func() {
			http.ListenAndServe(*pprof, nil)
		}()
	}
	buf := make([]byte, 100, 1000)
	sess, err := net.Dial("tcp", *addr)
	if err != nil {
		fmt.Printf(err.Error())
		return
	}
	lens, err := sess.Write(buf)
	if err != nil {
		fmt.Printf(err.Error())
		return
	}
	fmt.Println("write len: ", lens)
	lens, err = sess.Read(buf)
	if err != nil {
		fmt.Printf(err.Error())
		return
	}
	fmt.Println("read len: ", lens)
	fmt.Println("read buf: ", buf[:lens])
	fmt.Println("read buf string: ", string(buf[:lens]))

	return

	local := [...]uint8{80, 91, 80, 91, 0, 91, 80, 91, 80, 91, 80, 91, 80, 91, 90}
	var str string = string(local[:])
	fmt.Print("len: ", len(str), " ... ", str, "\n")
	for i := 0; i < len(str); i++ {
		fmt.Printf("%d .", str[i])
	}
	return
}
