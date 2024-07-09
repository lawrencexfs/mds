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

vrrp_instance VI_1 {
    state BACKUP
    interface eth0
    virtual_router_id 51
    priority 90
    advert_int 1
    authentication {
        auth_type PASS
        auth_pass 1111
    }
    virtual_ipaddress {
        10.0.20.188
    }
}

virtual_server 10.0.20.188 80 {
    delay_loop 6
    lb_algo rr
    lb_kind DR
    persistence_timeout 50
    protocol TCP

    real_server 10.0.20.21 80 {
        weight 1
        TCP_CHECK {
            connect_timeout 3
            nb_get_retry 3
            delay_before_retry 3
        }
    }

    real_server 10.0.20.22 80 {
        weight 1
        TCP_CHECK {
            connect_timeout 3
            nb_get_retry 3
        }
    }
}

