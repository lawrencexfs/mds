package db

import (
	"context"
	"log"
	"time"

	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
)

type Mongo struct {
	URL        string //数据库网络地址
	Database   string //要连接的数据库
	Collection string //要连接的集合
	client     *mongo.Client
	coll       *mongo.Collection
}

func (m *Mongo) Open() (*mongo.Client, *mongo.Collection) {
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()
	client, err := mongo.Connect(ctx, options.Client().ApplyURI(m.URL))
	if err != nil {
		log.Print(err)
	}
	// Check the connection
	err = client.Ping(ctx, nil)
	if err != nil {
		log.Print(err)
	}
	m.client = client

	collection := client.Database(m.Database).Collection(m.Collection)
	m.coll = collection

	return client, collection
}

func (m *Mongo) Close() {
	err := m.client.Disconnect(context.Background())
	if err != nil {
		log.Print(err)
	}
}

func (m *Mongo) Connect() *mongo.Collection {
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()
	client, err := mongo.Connect(ctx, options.Client().ApplyURI(m.URL))
	if err != nil {
		log.Print(err)
	}
	// Check the connection
	err = client.Ping(ctx, nil)
	if err != nil {
		log.Print(err)
	}

	collection := client.Database(m.Database).Collection(m.Collection)
	m.coll = collection
	return collection
}
