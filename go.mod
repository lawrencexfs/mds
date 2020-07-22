module mds

go 1.12

require (
	github.com/gin-gonic/gin v1.6.2
	github.com/google/uuid v1.1.1
	github.com/prashanthpai/sunrpc v0.0.0-20170623061256-3eefdc316eca
	github.com/rasky/go-xdr v0.0.0-20170124162913-1a41d1a06c93
	glog v0.0.0-00010101000000-000000000000
	go.mongodb.org/mongo-driver v1.3.2
	gopkg.in/yaml.v2 v2.2.8
)

// replace glog => /home/gopath/src/glog
replace glog => github.com/golang/glog v0.0.0-20160126235308-23def4e6c14b
