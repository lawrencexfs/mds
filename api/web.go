package api

import (
	"context"
	"net/http"
	"mds/db"
	"glog"

	"mds/api/conf"
	"mds/api/model"
)

var (
	srv *http.Server
)

//Init 初始化WEB api
func Init(path, version string) {

	conf.LoadConfOrDie(path, version)
	config := conf.Get()
	router := SetupRouter()
	srv = &http.Server{Addr: config.APIAddr, Handler: router}

	// config.MongoAddr
	var mgodb = &db.Mongo{
		URL:        config.MongoAddr.URL,        //"mongodb://192.168.2.243:27017",
		Database:   config.MongoAddr.Database,   //"mydb",
		Collection: config.MongoAddr.Collection, //"local",
	}
	client, coll := mgodb.Open()
	model.Init(client, coll)

}

//StartContext 启动master 节点
func StartContext(ctx context.Context) {

	go func() {
		// service connections
		if err := srv.ListenAndServe(); err != nil {
			glog.Warningln("HTTP server exit: ", err)
		}
	}()
}

//GracefulStop exit
func GracefulStop() {
	srv.Shutdown(nil)
	glog.V(2).Infoln("HTTP Server exited.")
}
