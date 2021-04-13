package main

import (
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	_ "net/http/pprof"
	"os"

	// "strconv"

	"github.com/PuerkitoBio/goquery"
)

var url = flag.String("url", "http://192.168.2.243:8031/lolo", "The server url")
var pprof = flag.String("pprof", "0.0.0.0:6060", "[localhost:6060]start debug page.")
var prepath = flag.String("prepath", "./data/", "[localhost:6060]start debug page.")

func main() {
	flag.Parse()
	if *pprof != "" {
		go func() {
			log.Print(http.ListenAndServe(*pprof, nil))
		}()
	}
	fmt.Println("Game Begin")
	CreateDir(*prepath)
	ScanURL(*url, *prepath)

}

//创建文件夹
func CreateDir(path string) {
	_, err := os.Stat(path)
	if err != nil {
		fmt.Println("Path not exist : ", path)
		if os.IsNotExist(err) {
			err := os.Mkdir(path, os.ModePerm)
			if err != nil {
				fmt.Printf("os.Mkdir fail ![%v]\n", err)
			}
		}
	}
	return
}

func DownReFile(urls, path string) {
	var netClient = &http.Client{}
	resp, err := netClient.Get(urls)
	defer resp.Body.Close()
	if err != nil {
		fmt.Println("http.Get(urls) error", err)
		return
	} else {
		fmt.Println("req url : ", urls)
		// fmt.Println("rsp.Header : ", resp.Header)
		// fmt.Println("rsp.type : ", resp.Header.Get("Accept-Ranges"))
	}
	if resp.Header.Get("Accept-Ranges") == "bytes" {
		fmt.Println("file : ", path)
		Download(urls, path)
	} else {
		fmt.Println("path: ", path)
		CreateDir(path)
		ScanURL(urls, path)
	}
	return

}

func ScanURL(urls, path string) {
	var netClient = &http.Client{}
	resp, err := netClient.Get(urls)
	if err != nil {
		fmt.Println("http get error", err)
		return
	}
	doc, err := goquery.NewDocumentFromReader(resp.Body)
	if err != nil {
		return
	}

	defer resp.Body.Close()
	doc.Find("a").Each(func(i int, selection *goquery.Selection) {
		_href, _err := selection.Attr("href")
		if _err { //如果存在
			DownReFile(urls+"/"+_href, path+_href)
		}
	})
}

func Download(urlone, filename string) {

	// pbuff := make([]byte, 10*1024*1024)
	// var ntotal int64 = 0
	// var nlen int = 0

	var netClient = &http.Client{}
	rsp, err := netClient.Get(urlone)
	if err != nil {
		fmt.Println(err)
	} else {
		fmt.Println("rsp.Header : ", rsp.Header.Get("Content-Length"))
	}

	defer rsp.Body.Close()

	tmp, err := ioutil.ReadAll(rsp.Body)
	if err != nil {
		panic(err)
	}

	//保存到本地
	ioutil.WriteFile(filename, tmp, 0644)
	fmt.Println("Download ok ... filename: ", filename)

	// fi, err := os.Create(filename)
	// if err != nil {
	// 	panic(err)
	// }
	// io.Copy(fi, rsp.Body)
}
