package main

import (
	"flag"
	"io"
	"log"
	"mds/rpc/dict"
	"mds/rpc/opinterface"
	"mds/rpc/sunrpc"
	"net"
	"net/http"
	_ "net/http/pprof"
	"net/rpc"
)

var addr = flag.String("addr", "192.168.2.243:9061", "The rpc server addr")
var pprof = flag.String("pprof", "0.0.0.0:6060", "[localhost:6060]start debug page.")
var method = flag.String("method", "Volume.OpCallbb", "The rpc server method")

func main() {
	flag.Parse()

	if *pprof != "" {
		go func() {
			log.Print(http.ListenAndServe(*pprof, nil))
		}()
	}
	log.Print("net.Dial addr: ", *addr)
	// Dial("tcp", "golang.org:http")
	conn, err := net.Dial("tcp", *addr)
	if err != nil {
		log.Print("net.Dial err: ", err)
		return
	}
	log.Print("net.Dial ok: ", conn)

	var notifyClose = make(chan io.ReadWriteCloser, 10)
	client := rpc.NewClientWithCodec(sunrpc.NewClientCodec(conn, notifyClose))
	log.Print("NewClientWithCodec pass... ")

	req := &opinterface.GfCommonReq{
		// Xflags:  0x1f1f1f1f,
		// Pargfid: [16]byte{a, a, a, a, a},
	}
	req.Bname = string("i am cli .call rpc svr")
	req.Xflags = 0x3f
	req.Pargfid = [16]byte{96, 96, 96, 96, 96, 96}
	// var rsp brick.GfBrickOpRsp
	var rsp opinterface.GfCommonRsp
	err = client.Call(*method, req, &rsp)

	// var rsp lorpc.GfGetVolumeInfoResp
	// err = client.Call("GfHandshake.ServerGetVolumeInfo", req, &rsp)
	if err != nil {
		log.Print("rpc.Call err: ", err)
	}
	log.Print("rpc.Call method... ", *method, "  rsp:", rsp)
	log.Print("rpc.Call ... rsp: ", string(rsp.Xdata))

	input, err := dict.Unserialize(rsp.Xdata)
	if err != nil {
		log.Print("dict.Unserialize ... rsp.Xdata fail... ")
		// return err
	} else {
		log.Print("Call ... rsp.Xdata len... ", len(rsp.Xdata))
		log.Print("Call ... rsp.Xdata map... ", input)
	}

}
