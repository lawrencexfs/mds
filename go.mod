module mds

go 1.23

toolchain go1.23.4

require (
	// github.com/PuerkitoBio/goquery v1.6.1
	github.com/gin-gonic/gin v1.6.2
	github.com/go-zeromq/zmq4 v0.17.0
	github.com/google/uuid v1.6.0
	github.com/prashanthpai/sunrpc v0.0.0-20170623061256-3eefdc316eca
	github.com/rasky/go-xdr v0.0.0-20170124162913-1a41d1a06c93
	glog v0.0.0-00010101000000-000000000000
	go.mongodb.org/mongo-driver v1.3.2
	gopkg.in/yaml.v2 v2.2.8
)

require (
	github.com/PuerkitoBio/goquery v1.10.0
	github.com/alexflint/go-arg v1.5.1
	github.com/fullstorydev/grpcui v1.4.3
	github.com/go-telegram-bot-api/telegram-bot-api/v5 v5.5.1
	github.com/google/gopacket v1.1.19
	github.com/gorilla/mux v1.8.1
	github.com/gorilla/websocket v1.5.3
	github.com/sirupsen/logrus v1.4.2
	golang.org/x/text v0.22.0
)

require (
	github.com/alexflint/go-scalar v1.2.0 // indirect
	github.com/andybalholm/cascadia v1.3.2 // indirect
	github.com/bufbuild/protocompile v0.14.1 // indirect
	github.com/cncf/xds/go v0.0.0-20231109132714-523115ebc101 // indirect
	github.com/envoyproxy/go-control-plane v0.11.1 // indirect
	github.com/envoyproxy/protoc-gen-validate v1.0.2 // indirect
	github.com/fullstorydev/grpcurl v1.9.3 // indirect
	github.com/gin-contrib/sse v0.1.0 // indirect
	github.com/go-playground/locales v0.13.0 // indirect
	github.com/go-playground/universal-translator v0.17.0 // indirect
	github.com/go-playground/validator/v10 v10.2.0 // indirect
	github.com/go-stack/stack v1.8.0 // indirect
	github.com/go-zeromq/goczmq/v4 v4.2.2 // indirect
	github.com/golang/protobuf v1.5.4 // indirect
	github.com/golang/snappy v0.0.1 // indirect
	github.com/jhump/protoreflect v1.17.0 // indirect
	github.com/json-iterator/go v1.1.9 // indirect
	github.com/klauspost/compress v1.9.5 // indirect
	github.com/konsorten/go-windows-terminal-sequences v1.0.2 // indirect
	github.com/leodido/go-urn v1.2.0 // indirect
	github.com/mattn/go-isatty v0.0.12 // indirect
	github.com/modern-go/concurrent v0.0.0-20180228061459-e0a39a4cb421 // indirect
	github.com/modern-go/reflect2 v0.0.0-20180701023420-4b7aa43c6742 // indirect
	github.com/pkg/errors v0.8.1 // indirect
	github.com/ugorji/go/codec v1.1.7 // indirect
	github.com/xdg/scram v0.0.0-20180814205039-7eeb5667e42c // indirect
	github.com/xdg/stringprep v0.0.0-20180714160509-73f8eece6fdc // indirect
	golang.org/x/crypto v0.33.0 // indirect
	golang.org/x/net v0.35.0 // indirect
	golang.org/x/sync v0.11.0 // indirect
	golang.org/x/sys v0.30.0 // indirect
	google.golang.org/genproto/googleapis/rpc v0.0.0-20231106174013-bbf56f31fb17 // indirect
	google.golang.org/grpc v1.61.0 // indirect
	google.golang.org/protobuf v1.36.5 // indirect
)

// replace glog => /home/gopath/src/glog
replace glog => github.com/golang/glog v0.0.0-20160126235308-23def4e6c14b
