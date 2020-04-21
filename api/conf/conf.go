package conf

import (
	"io/ioutil"
	"glog"

	yaml "gopkg.in/yaml.v2"
)

var (
	config *Config
)

type MongoConf struct {
	URL        string `yaml:"addr"`
	Database   string `yaml:"dbname"`
	Collection string `yaml:"collection"`
}

//Config Return conf.yml mapper
type Config struct {
	Version string `yaml:"version"`
	APIAddr string `yaml:"api_addr"`

	MongoAddr *MongoConf `yaml:"mongo_addr"`
}

// LoadConfOrDie Loading config from local file
func LoadConfOrDie(confFile, version string) {

	config = &Config{}
	yamlFile, err := ioutil.ReadFile(confFile)
	if err != nil {
		glog.Fatalf("Open Configure file failed, %v ", err)
	}
	err = yaml.Unmarshal(yamlFile, config)
	if err != nil {
		glog.Fatalf("Unmarshal, %v ", err)
	}

}

//Get return config
func Get() *Config {
	conf := config
	return conf
}
