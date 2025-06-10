// add.go
package main

/*
#cgo CFLAGS: -I.
*/
import "C"
import "fmt"

//export Add
func Add(a, b C.int) C.int {
	// log_message(C.CString("Calculating sum...")) // 调用C++函数
	fmt.Println("Calculating sum...")
	return a + b
}

func main() {} // 必须保留空main

// # Linux/macOS
// go build -buildmode=c-shared -o libadd.so add.go

// # Windows
// go build -buildmode=c-shared -o libadd.dll add.go

// #cgo CFLAGS: -I.
// #cgo LDFLAGS: -L. -lyourlib
// extern void log_message(const char* msg); // 声明C++函数供Go调用
