echo 'export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH' >> /etc/bash.bashrc
echo 'export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH' >>  /etc/bash.bashrc
source  /etc/bash.bashrc

g++ -o cli cli.cpp $(pkg-config --cflags --libs libczmq)
g++ -o svr svr.cpp $(pkg-config --cflags --libs libczmq)


./svr -a tcp://0.0.0.0:81 -s xxx6

[root@A003 dev]# cat test.sh 
#!/bin/bash

# 无限循环
while true; do
    # 打印当前时间
    #echo "$(date '+%Y-%m-%d %H:%M:%S')"
    ./cli -a tcp://10.0.20.188:81 -s "$(date '+%Y-%m-%d %H:%M:%S')"
    # 等待一秒钟
    sleep 1
done


