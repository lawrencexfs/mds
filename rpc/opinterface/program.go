package opinterface

import (
	"log"

	"mds/rpc/dict"
	"mds/rpc/sunrpc"
)

const (
	brickOpsProgNum     = 4867634 // GD_BRICK_PROGRAM
	brickOpsProgVersion = 2       // GD_BRICK_VERSION

	mdsOpsProgNum     = 1238463 // GD_PROGRAM
	mdsOpsProgVersion = 2       // GD_VERSION

)

// TypeBrickOp is a brick operation
//go:generate stringer -type=TypeBrickOp
type TypeBrickOp uint32
type TypeCallOp uint32

// RPC procedures
const (
	OpBrickNull         TypeBrickOp = iota // GLUSTERD_BRICK_NULL
	OpBrickTerminate                       // GLUSTERD_BRICK_TERMINATE
	OpBrickXlatorInfo                      // GLUSTERD_BRICK_XLATOR_INFO
	OpBrickXlatorOp                        // GLUSTERD_BRICK_XLATOR_OP
	OpBrickStatus                          // GLUSTERD_BRICK_STATUS
	OpBrickOp                              // GLUSTERD_BRICK_OP
	OpBrickXlatorDefrag                    // GLUSTERD_BRICK_XLATOR_DEFRAG
	OpNodeProfile                          // GLUSTERD_NODE_PROFILE
	OpNodeStatus                           // GLUSTERD_NODE_STATUS
	OpVolumeBarrierOp                      // GLUSTERD_VOLUME_BARRIER_OP
	OpBrickBarrier                         // GLUSTERD_BRICK_BARRIER
	OpNodeBitrot                           // GLUSTERD_NODE_BITROT
	OpBrickAttach                          // GLUSTERD_BRICK_ATTACH
	OpDumpMetrics                          // GLUSTERD_DUMP_METRICS
	OpMaxValue                             // GLUSTERD_BRICK_MAXVALUE
)

const (
	OpCallNull TypeCallOp = iota // nil
	OpCallbb              = 27   // call
	OpCalldd              = 28
	OpMaxCall             = 29
)

// GfCommonReq is a generic RPC response type
type GfCommonReq struct {
	Bname   string
	Xflags  uint
	Pargfid [16]byte
	Xdata   []byte
}

// GfCommonRsp is a generic RPC response type
type GfCommonRsp struct {
	ID      uint
	OpRet   int
	OpErrno int
	Xdata   []byte
}

type Volume struct {
	OpRet int
}

func (vol *Volume) OpCallbb(in GfCommonReq, reply *GfCommonRsp) error {
	// reply.Xdata = []byte(string("default...local.rpc.OpCallbb...+ .") + string(in.Input))
	log.Print("OpCallbb ... req ... ", in)
	log.Print("OpCallbb ... req ... ", in.Bname)
	log.Print("OpCallbb ... req ... ", in.Xflags)
	log.Print("OpCallbb ... req ... ", in.Pargfid)
	log.Print("OpCallbb ... req ... ", in.Xdata)

	input, err := dict.Unserialize(in.Xdata)
	if err != nil {
		log.Print("dict.Unserialize ... Input fail... ")
		// return err
	} else {
		log.Print("OpCallbb ... Input len... ", len(in.Xdata))
		log.Print("OpCallbb ... Input map... ", input)
	}

	input["server"] = "local.mds"
	input["author"] = "yang"
	reply.Xdata, err = dict.Serialize(input)
	if err != nil {
		log.Print("dict.Serialize ... reply fail... ")
		return err
	}
	log.Print("OpCallbb ... Xdata map... ", input)
	log.Print("OpCallbb ... reply ... ", (reply))
	log.Print("OpCallbb ... Xdata ... ", (reply.Xdata))
	return nil
}

func (vol *Volume) OpCalldd(args GfBrickOpReq, reply *GfBrickOpRsp) error {
	reply.Output = []byte(args.Name + string("...local.rpc.OpCalldd"))
	return nil
}

// GfBrickOpReq is the request sent to the brick process
type GfBrickOpReq struct {
	Name  string
	Op    int
	Input []byte
}

// GfBrickOpRsp is the response sent by brick to a BrickOpReq request
type GfBrickOpRsp struct {
	OpRet    int
	OpErrno  int
	Output   []byte
	OpErrstr string
}

func init() {
	var p sunrpc.Procedure
	// for i := 0; i < int(OpMaxValue); i++ {
	// 	p.ID = sunrpc.ProcedureID{
	// 		ProgramNumber:   brickOpsProgNum,
	// 		ProgramVersion:  brickOpsProgVersion,
	// 		ProcedureNumber: uint32(i),
	// 	}
	// 	p.Name = "Brick." + TypeBrickOp(i).String()
	// 	_ = sunrpc.RegisterProcedure(p, false)
	// }

	for ind := OpCallbb; ind < int(OpMaxCall); ind++ {
		p.ID = sunrpc.ProcedureID{
			ProgramNumber:   mdsOpsProgNum,
			ProgramVersion:  mdsOpsProgVersion,
			ProcedureNumber: uint32(ind),
		}
		p.Name = "Volume." + TypeCallOp(ind).String()
		_ = sunrpc.RegisterProcedure(p, false)
	}

}
