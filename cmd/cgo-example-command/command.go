package main

import (
	"C"
	"bytes"
	"fmt"
	"os/exec"
)
import (
	"bufio"
	"log"
	"os"
	"strings"
)

// Command 接口定义
type Command struct {
	Path string   // 可执行文件的全路径
	Args []string // 参数列表
}

// Run 执行命令并返回标准输出、错误输出和错误
func (c *Command) Run() (stdout, stderr string, err error) {
	cmd := exec.Command(c.Path, c.Args...)
	var out, errOut bytes.Buffer
	cmd.Stdout = &out
	cmd.Stderr = &errOut
	err = cmd.Run()
	stdout = out.String()
	stderr = errOut.String()
	return
}

func (c *Command) RunWithOut() (stdout, stderr string, errOut error) {
	// 创建命令
	cmd := exec.Command(c.Path, c.Args...)
	// 获取标准输出和错误输出的管道
	outp, err := cmd.StdoutPipe()
	if err != nil {
		errOut = fmt.Errorf("获取标准输出管道失败: %v", err)
		return
	}

	errp, err := cmd.StderrPipe()
	if err != nil {
		errOut = fmt.Errorf("获取错误输出管道失败: %v", err)
		return
	}

	// 启动命令
	if err := cmd.Start(); err != nil {
		errOut = fmt.Errorf("启动命令失败: %v", err)
		return
	}

	// 异步读取标准输出
	go func() {
		scanner := bufio.NewScanner(outp)
		for scanner.Scan() {
			fmt.Printf("[Command stdout] %s\n", scanner.Text())
			stdout += scanner.Text() + "\n"
		}
		if err := scanner.Err(); err != nil {
			log.Printf("读取标准输出错误: %v", err)
		}
	}()

	// 异步读取错误输出
	go func() {
		scanner := bufio.NewScanner(errp)
		for scanner.Scan() {
			fmt.Printf("[Command stderr] %s\n", scanner.Text())
			stderr += scanner.Text() + "\n"
		}
		if err := scanner.Err(); err != nil {
			log.Printf("读取错误输出错误: %v", err)
		}
	}()

	// 等待命令执行完毕
	if err := cmd.Wait(); err != nil {
		errOut = fmt.Errorf("命令执行失败: %v", err)
		return
	}

	return
}

//export RunCommand
func RunCommand(path *C.char, args *C.char) *C.char {
	cPath := C.GoString(path)
	cArgs := strings.Split(C.GoString(args), " ")
	cmd := Command{Path: cPath, Args: cArgs}
	stdout, stderr, err := cmd.Run()
	result := fmt.Sprintf("stdout: %s\nstderr: %s\nerr: %v", stdout, stderr, err)
	return C.CString(result)
}

//export RunCommandWithOut
func RunCommandWithOut(path *C.char, args *C.char, pstdout, pstderr, perrOut **C.char) {
	cPath := C.GoString(path)
	cArgs := strings.Split(C.GoString(args), " ")
	cmd := Command{Path: cPath, Args: cArgs}
	stdout, stderr, err := cmd.RunWithOut()

	// 将 Go 字符串转换为 C 字符串
	if stdout != "" {
		*pstdout = C.CString(stdout)
	}
	if stderr != "" {
		*pstderr = C.CString(stderr)
	}
	if err != nil {
		*perrOut = C.CString(err.Error())
		fmt.Fprintf(os.Stderr, "[Run Error] %v\n", err)
	}

	// result := fmt.Sprintf("stdout: %s\nstderr: %s\nerr: %v", stdout, stderr, err)
	return
}

func main() {}

// go build -buildmode=c-archive -o command.a command.go
// gcc -shared -o command.dll command.a -Wl,--out-implib,command.lib

// go build -buildmode=c-shared  -o command.dll  command.go
// g++ -o demo.exe demo.cpp command.lib
// g++ -o demo.exe demo.cpp -L. -lcommand
