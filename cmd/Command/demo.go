package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

func executeCommandWithStreamingOutput(command string, args ...string) error {
	// 创建命令
	cmd := exec.Command(command, args...)

	// 获取标准输出和错误输出的管道
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		return fmt.Errorf("获取标准输出管道失败: %v", err)
	}

	stderr, err := cmd.StderrPipe()
	if err != nil {
		return fmt.Errorf("获取错误输出管道失败: %v", err)
	}

	// 启动命令
	if err := cmd.Start(); err != nil {
		return fmt.Errorf("启动命令失败: %v", err)
	}

	// 异步读取标准输出
	go func() {
		scanner := bufio.NewScanner(stdout)
		for scanner.Scan() {
			fmt.Printf("[stdout] %s\n", scanner.Text())
		}
		if err := scanner.Err(); err != nil {
			log.Printf("读取标准输出错误: %v", err)
		}
	}()

	// 异步读取错误输出
	go func() {
		scanner := bufio.NewScanner(stderr)
		for scanner.Scan() {
			fmt.Printf("[stderr] %s\n", scanner.Text())
		}
		if err := scanner.Err(); err != nil {
			log.Printf("读取错误输出错误: %v", err)
		}
	}()

	// 等待命令执行完毕
	if err := cmd.Wait(); err != nil {
		return fmt.Errorf("命令执行失败: %v", err)
	}

	return nil
}

// 设置UTF-8编码
// # PowerShell中执行此命令，设置当前会话为UTF-8
// chcp 65001
func main() {
	// 示例：执行一个长时间运行的命令并实时获取输出
	err := executeCommandWithStreamingOutput("E:\\space\\mds\\cmd\\Command\\iunit-server.exe ", "-n", "5", "10.6.25.9")
	if err != nil {
		log.Fatalf("执行命令出错: %v", err)
	}
	fmt.Println("命令执行完毕")
	processXPath()
}

// 处理用户输入的路径，支持跨平台
func processUserPath(inputPath string) (string, error) {
	// 1. 标准化路径分隔符：将所有反斜杠(\)替换为正斜杠(/)
	// 这一步确保无论用户输入的是Windows风格还是Unix风格路径，都能统一处理
	normalizedPath := strings.ReplaceAll(inputPath, "\\", "/")
	fmt.Printf("rrrr替换处理结果: %s\n", normalizedPath)

	// 2. 使用filepath.Clean清理路径，处理相对路径、.和..等
	cleanedPath := filepath.Clean(normalizedPath)

	// 3. 如果是绝对路径，直接返回清理后的路径
	if filepath.IsAbs(cleanedPath) {
		return cleanedPath, nil
	}

	// 4. 如果是相对路径，与当前工作目录拼接
	wd, err := os.Getwd()
	if err != nil {
		return "", fmt.Errorf("获取当前工作目录失败: %v", err)
	}

	// 使用filepath.Join确保路径分隔符合当前平台
	absolutePath := filepath.Join(wd, cleanedPath)
	return absolutePath, nil
}

func processXPath() {
	// 示例1：Windows风格路径
	windowsPath := `E:\xpro\demo`
	processedPath, err := processUserPath(windowsPath)
	if err != nil {
		fmt.Printf("处理路径失败: %v\n", err)
		return
	}
	fmt.Printf("Windows风格路径处理结果: %s\n", processedPath)

	// 示例2：Unix风格路径
	unixPath := "/home/user/demo"
	processedPath, err = processUserPath(unixPath)
	if err != nil {
		fmt.Printf("处理路径失败: %v\n", err)
		return
	}
	fmt.Printf("Unix风格路径处理结果: %s\n", processedPath)

	// 示例3：相对路径
	relativePath := "data/files"
	processedPath, err = processUserPath(relativePath)
	if err != nil {
		fmt.Printf("处理路径失败: %v\n", err)
		return
	}
	fmt.Printf("相对路径处理结果: %s\n", processedPath)
}
