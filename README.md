# mds
meta data server

# 本地编译运行
go build -o demo.exe .\mds\mds_demo.go

# 编译跨平台程序
CGO_ENABLED=0 GOOS=linux GOARCH=arm GOARM=7 CC=arm-linux-gnueabi-gcc-4.7 go build -o ./qilin/mds mds.go

yum whatprovides iostat -y

exec: "arm-linux-gnueabi-gcc-4.7": executable file not found in $PATH

export GOARCH=arm

  SET CGO_ENABLED=0
  SET GOOS=linux
  SET GOARCH=amd64
go build -o demo.linux .\mds\mds_demo.go
.
# go env
set GO111MODULE=
set GOARCH=amd64
set GOBIN=
set GOCACHE=C:\Users\YY\AppData\Local\go-build
set GOENV=C:\Users\YY\AppData\Roaming\go\env

set GOHOSTARCH=amd64
set GOHOSTOS=windows

set GOOS=windows
set GOPATH=E:\gospace

set GOPROXY=https://mirrors.aliyun.com/goproxy/,direct
set GOROOT=E:\tools\go
set GOSUMDB=sum.golang.org
set GOTOOLDIR=E:\tools\go\pkg\tool\windows_amd64

set GCCGO=gccgo
set AR=ar
set CC=gcc
set CXX=g++
set CGO_ENABLED=1


# 教程
https://zhuanlan.zhihu.com/p/98215258

Windows 下编译 Mac 和 Linux 64位可执行程序
  SET CGO_ENABLED=0
  SET GOOS=darwin
  SET GOARCH=amd64
  go build main.go
  ​
  SET CGO_ENABLED=0
  SET GOOS=linux
  SET GOARCH=amd64
  go build main.go


(1)首先进入go/src 源码所在目录，执行如下命令创建目标平台所需的包和工具文件。

$ cd /usr/local/go/src
$ CGO_ENABLED=0 GOOS=linux GOARCH=amd64 ./make.bash
 

如果是 Windows 则修改 GOOS 即可。

$ CGO_ENABLED=0 GOOS=windows GOARCH=amd64 ./make.bat
 
(2) 现在可以编译 Linux 和 Windows 平台所需的执行文件了。

$ CGO_ENABLED=0 GOOS=linux GOARCH=amd64 go build
$ CGO_ENABLED=0 GOOS=windows GOARCH=amd64 go build