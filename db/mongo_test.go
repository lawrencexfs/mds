package db

import (
	"fmt"
	"testing"
)

func TestMgo_Connect(t *testing.T) {
	var mgodb = &Mongo{
		URL:        "mongodb://192.168.2.243:27017",
		Database:   "mydb",
		Collection: "file",
	}
	// mgodb.Connect()
	collection := mgodb.Connect()
	fmt.Printf("%T\n", collection)
}
