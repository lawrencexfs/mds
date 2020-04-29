package api

import (
	"glog"
	"log"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"

	"mds/api/model"
)

//Resp 返回基本状态定义
type Resp struct {
	Code    int32  `json:"code"`
	Message string `json:"message"`
	Detail  string `json:"detail,omitempty"`
}

//返回状态码
var (
	SUCCESS    = Resp{Code: 0, Message: "成功", Detail: "Welcome use mds server"}
	SUCCESS_V  = Resp{Code: 1, Detail: "Welcome visitor here"}
	FAILURE    = Resp{Code: 10001, Message: "失败", Detail: "内部调用失败"}
	ReqArgsErr = Resp{Code: 9001, Message: "请求参数错误"}
)

//SetupRouter return master api router
func SetupRouter() *gin.Engine {
	gin.SetMode(gin.ReleaseMode)

	router := gin.Default()
	// router := gin.New()
	// router.Use(gin.Logger())
	// router.Use(gin.Recovery())
	log.SetFlags(log.LstdFlags) // gin will disable log flags

	router.GET("/", HelloRespV)
	router.GET("api/file", HelloResp)
	router.GET("api/file/:gfid", GetFile)
	router.POST("api/file", PostFile)
	router.PUT("api/file", PutFile)
	router.DELETE("api/file/:gfid", DelFile)
	router.NoRoute(HelloRespV)
	// router.Run()
	return router

}

//通用回复
func HelloRespV(c *gin.Context) {
	glog.V(2).Infoln("接收请求.")
	c.JSON(200, SUCCESS_V)
}

//
func HelloResp(c *gin.Context) {
	glog.V(2).Infoln("接收请求.")
	sinfo := "....test len 16 + X"
	data := &model.FileInfo{
		Id:   uuid.New().String(),
		Info: sinfo,
		Len:  MinLength + uint32(len(sinfo)),
	}
	data.Create()
	rp := SUCCESS
	rp.Detail = data.Id
	c.JSON(201, rp)
}

//curl  -X "POST" -H "Content-Type: application/json; charset=utf-8;"  'availapi.monitor:80/api/flie'  -d '{"alert_id":"a1a19105e7aefbbb76fba5b1746efbc9","time":"2019-10-09T15:35:00Z","window":5}'
