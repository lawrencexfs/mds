package api

import (
	"io/ioutil"
	"glog"

	"github.com/gin-gonic/gin"

	"mds/api/model"

)

const MinLength = 16
const BufLength = 1024

type PostFileReq struct {
	Len uint32
	// Key   [16]byte
	Buf []byte
}

func PostFile(c *gin.Context) {

	glog.V(2).Infoln("接收请求PostFile.")
	req := &PostFileReq{}
	req.Len = uint32(c.Request.ContentLength)

	// if err := c.Bind(req); err != nil { //从参数中获取失败一样则尝试从body中获取
	// 	glog.Errorln(err)
	// 	c.JSON(400, ReqArgsErr)
	// 	return
	// }
	buf, err := ioutil.ReadAll(c.Request.Body)
	if err != nil || len(buf) != int(req.Len) {
		glog.Errorln(err)
		c.JSON(400, ReqArgsErr)
		return
	}
	req.Buf = buf[:]

	// if req.Key == "" || req.Value == "" {
	if req.Len < MinLength {
		c.JSON(400, ReqArgsErr)
		return
	}
	// c.Request.ContentLength
	// s := arr[startIndex:endIndex]
	// 将arr中从下标startIndex到endIndex-1 下的元素创建为一个新的切片
	data := &model.FileInfo{
		Len:  req.Len,
		Id:   string(req.Buf[:MinLength]),
		Info: string(req.Buf[MinLength:]),
	}
	err = data.Create()
	if err != nil {
		c.JSON(400, FAILURE)
	}
	c.JSON(201, SUCCESS)
}

type FileResp struct {
	status Resp
	info   PostFileReq
}

func GetFile(c *gin.Context) {
	glog.V(2).Infoln("接收请求GetFile.")

	gfid := c.Param("gfid")
	data := &model.FileInfo{
		Id: gfid,
	}

	if err := data.Select(); err != nil {
		c.JSON(400, FAILURE)
		return
	}
	inforsp := PostFileReq{Len: data.Len}
	// copy(inforsp.Key, data.Id)
	// copy(inforsp.Value, data.Info)
	inforsp.Buf = make([]byte, MinLength+len(data.Info))
	// for i := 0; i < len(data.Id) && i < MinLength; i++ {
	// 	inforsp.Buf[i] = data.Id[i]
	// }
	// for i := 0; i < len(data.Info) && i < BufLength-MinLength; i++ {
	// 	inforsp.Buf[i+MinLength] = data.Info[i]
	// }
	copy(inforsp.Buf[:MinLength], data.Id)
	copy(inforsp.Buf[MinLength:], data.Info)
	c.Data(200, "private", (inforsp.Buf[:]))
}

func DelFile(c *gin.Context) {
	glog.V(2).Infoln("接收请求DelFile.")

	gfid := c.Param("gfid")
	data := &model.FileInfo{
		Id: gfid,
	}

	if err := data.Delete(); err != nil {
		c.JSON(400, FAILURE)
		return
	}
	c.JSON(200, SUCCESS)
}

func PutFile(c *gin.Context) {

	glog.V(2).Infoln("接收请求PutFile.")
	req := &PostFileReq{}
	req.Len = uint32(c.Request.ContentLength)

	buf, err := ioutil.ReadAll(c.Request.Body)
	if err != nil || len(buf) != int(req.Len) {
		glog.Errorln(err)
		c.JSON(400, ReqArgsErr)
		return
	}
	req.Buf = buf[:]

	// if req.Key == "" || req.Value == "" {
	if req.Len < MinLength {
		c.JSON(400, ReqArgsErr)
		return
	}

	data := &model.FileInfo{
		Len:  req.Len,
		Id:   string(req.Buf[:MinLength]),
		Info: string(req.Buf[MinLength:]),
	}
	err = data.Update()
	if err != nil {
		c.JSON(400, FAILURE)
	}
	c.JSON(201, SUCCESS)
}
