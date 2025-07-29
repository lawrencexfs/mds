package main

import (
	"github.com/sirupsen/logrus"
)

func main() {
	logrus.SetLevel(logrus.DebugLevel) // 设置日志级别
	logrus.Debug("调试日志")
	logrus.Info("信息日志")
	logrus.Warn("警告日志")
	logrus.Error("错误日志")
}
