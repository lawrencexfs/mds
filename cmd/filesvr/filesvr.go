
package main
  
import (
    "flag"
    "fmt"
    "net/http"
)

var addr = flag.String("addr", ":8031", "The server listen addr")
var dir = flag.String("dir", "/", "The server listen addr")

func main() {
    flag.Parse()
    http.Handle("/", http.FileServer(http.Dir(*dir)))
    err := http.ListenAndServe(*addr, nil)
    if err != nil {
        fmt.Println(err)
    }
}
