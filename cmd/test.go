package main

import (
	"flag"
	"fmt"
	"net/http"
	_ "net/http/pprof"
)

var (
	confFile = flag.String("conf", "conf.yml", "The configure file read")
	pprof    = flag.String("pprof", "", "[localhost:6060]start debug page.")
)

func main() {

	flag.Parse()./t	
	if *pprof != "" {
		go func() {
			http.ListenAndServe(*pprof, nil)
		}()
	}
	local := [...]uint8{80, 91, 80, 91, 0, 91, 80, 91, 80, 91, 80, 91, 80, 91, 90}
	var str string = string(local[:])
	fmt.Print("len: ", len(str), " ... ", str, "\n")
	for i := 0; i < len(str); i++ {
		fmt.Printf("%d .", str[i])
	}
	return
}
