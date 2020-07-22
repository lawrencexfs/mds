package main

import (
	"flag"
	"log"
	"mds/rpc/opinterface"
	"mds/rpc/sunrpc"
	"net"
	"net/http"
	_ "net/http/pprof"
	"net/rpc"
	"os"
	"os/signal"
	"syscall"

	lorpc "github.com/prashanthpai/sunrpc"
)

const (
	port = 9061
)

var addr = flag.String("addr", "0.0.0.0:9061", "The rpc server addr")
var pprof = flag.String("pprof", "0.0.0.0:6062", "[localhost:6060]start debug page.")

func listenTCP() (net.Listener, string) {
	lis, err := net.Listen("tcp", *addr) // any available address
	if err != nil {
		log.Fatalf("net.Listen tcp : %v", err)
	}
	return lis, lis.Addr().String()
}

func main() {
	ch := make(chan os.Signal)
	signal.Notify(ch, os.Interrupt, syscall.SIGTERM)

	flag.Parse()

	if *pprof != "" {
		go func() {
			log.Print(http.ListenAndServe(*pprof, nil))
		}()
	}
	log.Print("net.Dial addr: ", *addr)

	// var l net.Listener
	lis, SAddr := listenTCP()
	log.Print("NewServer test RPC server listening on ", SAddr)

	programNumber := uint32(1238463)
	programVersion := uint32(2)

	// _ = lorpc.RegisterProcedure(lorpc.Procedure{
	// 	ID:   lorpc.ProcedureID{programNumber, programVersion, uint32(27)},
	// 	Name: "Arith.Add"}, true)
	// _ = lorpc.RegisterProcedure(lorpc.Procedure{
	// 	ID:   lorpc.ProcedureID{programNumber, programVersion, uint32(5)},
	// 	Name: "Arith.Multiply"}, true)

	lorpc.DumpProcedureRegistry()

	// Tell portmapper about it
	_, err := lorpc.PmapUnset(programNumber, programVersion)
	if err != nil {
		log.Fatal("sunrpc.PmapUnset() failed: ", err)
	}
	_, err = lorpc.PmapSet(programNumber, programVersion, lorpc.IPProtoTCP, uint32(port))
	if err != nil {
		log.Fatal("sunrpc.PmapSet() failed: ", err)
	}

	// newServer := rpc.NewServer()
	// // newServer.Register(new(Brick))
	// newServer.RegisterName("Brick.OpBrickTerminate", new(Brick))
	// go newServer.Accept(lis)
	go func() {
		for {
			conn, ierr := lis.Accept()
			if ierr != nil {
				log.Print("rpc.Serve: accept:", err.Error())
				return
			}
			server := rpc.NewServer()
			// server.Register(new(Brick))
			server.Register(new(opinterface.Volume))
			// server.RegisterName("Brick.OpBrickTerminate", new(Brick))
			// server.RegisterName("Brick.OpTest", new(Brick))

			// for _, p := range lorpc.NewprogramsList {
			// 	// if v, ok := p.(lorpc.Conn); ok {
			// 	// 	v.SetConn(conn)
			// 	// }
			// 	// if err := server.Register(p); err != nil {
			// 	// 	panic(fmt.Sprintf("rpc.Register failed: %s", err.Error()))
			// 	// }
			// }

			session := sunrpc.NewServerCodec(conn, nil)

			// sessionCh := make(chan struct{})
			go func() {
				defer session.Close()
				server.ServeCodec(session)
			}()
		}

	}()

	<-ch
	// log.Print("rpc.Call ... rsp: ", rsp)
	log.Print("CTRL + c 控制命令....")
}
