// main.go
package main

/*
#cgo CFLAGS: -I.
#cgo LDFLAGS: -L. -lcalc
#include "calc.h"
*/
import "C"
import "fmt"

func main() {
	result := C.add(911, 199)
	fmt.Println("Result from C++:", int(result))
}

// # Linux/macOS
// g++ -c -fPIC calc.cpp -o calc.o
// g++ -shared -o libcalc.so calc.o
// go build -o xx xx.go

// # Windows
// g++ -c calc.cpp -o calc.o
// g++ -shared -o libcalc.dll calc.o
// ==  g++ -shared -o libcalc.dll calc.cpp
// go build -o xx.exe xx.go

// # Linux/macOS
// GO111MODULE=off go build -o xx xx.go

// # Windows (PowerShell)
// $env:GO111MODULE="off"
// go build -o xx.exe xx.go
