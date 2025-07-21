package main

import (
	"fmt"
	_ "net/http/pprof"
	"reflect"
)

func Inspect(v interface{}) {
	tt := reflect.TypeOf(v)  // 获取类型信息
	vv := reflect.ValueOf(v) // 获取值信息

	fmt.Println("Type:", tt.Name())
	fmt.Println("Kind:", tt.Kind()) // 基础类型（如 struct、int 等）

	// 遍历结构体字段
	if tt.Kind() == reflect.Struct {
		for i := 0; i < tt.NumField(); i++ {
			field := tt.Field(i)
			value := vv.Field(i)
			fmt.Printf("Field %s: type=%s, value=%v\n",
				field.Name, field.Type, value.Interface())
		}
	}
}

type User struct {
	ID       int    `db:"id"`
	UserName string `db:"username"`
}

type Person struct {
	Name     string `json:"name"`
	Age      int
	PickName string `json:"pname"`
	Sex      string `json:"sex"`

	// uu   User
	// 通过反射读取 struct tag，映射数据库字段
}

func main() {
	p := Person{Name: "Alice", Age: 30, PickName: "Al", Sex: "female"}
	Inspect(p) // 运行时分析 Person 类型
	Inspect(User{ID: 1, UserName: "Bob"})

}

//
