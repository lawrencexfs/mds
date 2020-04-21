package model

import (
	"go.mongodb.org/mongo-driver/mongo"
)

var (
	mgo     *mongo.Client
	mgocoll *mongo.Collection
	// pg  *gorm.DB
	// rdb *rdsdb.Client
)

// Init db client
func Init(mongodb *mongo.Client, coll *mongo.Collection) {
	mgo = mongodb
	mgocoll = coll
}
