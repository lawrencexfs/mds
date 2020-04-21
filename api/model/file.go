package model

import (
	"context"
	"glog"

	// "gopkg.in/mgo.v2/bson"
	"go.mongodb.org/mongo-driver/bson"

)

//DisposeStatus 告警处置状态
type DisposeStatus int32

//定义处置状态
const (
	DisposeStatusNil  DisposeStatus = 0 //未配置对应的处置
	DisposeStatusDone DisposeStatus = 1 //完成
)

type FileInfo struct {
	Len  uint32 `json:"len"`
	Id   string `json:"id"`
	Info string `json:"info"`
	// Type string `json:"type"`
	// Ctx  context.Context `json:"-"`
}

//Select
func (lo *FileInfo) Select() error {
	glog.Infoln("FileInfo Select: ", lo.Id)
	filter := bson.D{{"id", lo.Id}}
	// var result FileInfo
	err := mgocoll.FindOne(context.Background(), &filter).Decode(lo)
	if err != nil {
		glog.Error("FileInfo err: ", err)
	}
	glog.Infoln("FileInfo Find Result: ", lo)

	return err
}

//Create
func (lo *FileInfo) Create() error {
	glog.Infoln("FileInfo Create: ", lo)

	Result, err := mgocoll.InsertOne(context.Background(), lo)
	if err != nil {
		glog.Error("FileInfo: ", err)
	}
	glog.Infoln("FileInfo Create Result: ", Result.InsertedID)

	return err
}

//Delete
func (lo *FileInfo) Delete() error {
	glog.Infoln("FileInfo Delete: ", lo.Id)
	filter := bson.D{{"id", lo.Id}}

	Result, err := mgocoll.DeleteOne(context.Background(), &filter)
	if err != nil {
		glog.Error("FileInfo: ", err)
	}
	glog.Infoln("FileInfo Delete Result: ", Result)

	return err
}

//Update
func (lo *FileInfo) Update() error {
	glog.Infoln("FileInfo Update: ", lo)

	filter := bson.D{{"id", lo.Id}}
	update := bson.D{
		{"$set", bson.D{
			{"info", lo.Info},
		}},
	}

	Result, err := mgocoll.UpdateOne(context.Background(), &filter, &update)
	if err != nil {
		glog.Error("FileInfo: ", err)
	}
	glog.Infoln("FileInfo Update Result: ", Result)

	return err
}
