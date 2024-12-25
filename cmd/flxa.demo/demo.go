package main

import (
	"bytes"
	"crypto/md5"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"net/url"
	"os"
	"runtime"
	"strconv"
	"strings"
	"time"

	"github.com/alexflint/go-arg"
	"golang.org/x/text/encoding/simplifiedchinese"
	"golang.org/x/text/transform"
)

const (
	// 登录
	LOGIN = "/iUnit/api/v1/user/login"
	// 工程列表
	PROJECT_LIST = "/iUnit/api/v1/project/list"
	// 工程详情
	PROJECT_DETAIL = "/iUnit/api/v1/project/detail"
	// 平台模板详情
	PLATFROM_TEMPLATE_DETAIL = "/iUnit/api/v1/platform-template/detail"
	// 获取当前工程编译器配置
	GET_CURRENT_COMPILER = "/iUnit/api/v1/toolchain/getCurrentCompiler"
	// 新增平台模板
	PLATFROM_TEMPLATE_SAVE = "/iUnit/api/v1/platform-template/save"
	// 创建或更新工程
	ADD_OR_UPDATE_PROJECT = "/iUnit/api/v1/project/addOrUpdateProject"
	// 创建或更新编译器
	ADD_OR_UPDATE_COMPILER = "/iUnit/api/v1/toolchain/addOrUpdateCompiler"
	// 创建工程
	ADD_PROJECT_FROM_REPOSITORY = "/iUnit/api/v1/project/addProjectFromRepository"
	// 获取工程目录树
	GET_PROJECT_DIR_TREE = "/iUnit/api/v1/project/dirTree/"
	// 更新工程
	UPDATE_PROJECT = "/iUnit/api/v1/project/update"
	// 获取工程目标
	GET_PROJECT_TARGET = "/iUnit/api/v1/project/getTarget"
	// 更新工程目标
	UPDATE_PROJECT_TARGET = "/iUnit/api/v1/project/updateTarget"
	// 忽略文件
	IGNORE_FILE = "/iUnit/api/v1/project/compile/ignore-files/add"
	// 解析工程
	ANALYZE_PROJECT = "/iUnit/api/v1/project/analyzed"
	// 执行工程
	OPERATE_PROJECT = "/iUnit/api/v1/function/operate"
	// 工程概要
	PROJECT_STASTIC = "/iUnit/api/v1/project/getProjectStatic"
	// 增加报告
	ADD_REPORT = "/iUnit/api/v1/report/add"
	// 报告列表
	LIST_REPORT = "/iUnit/api/v1/report/list"
)

// 登录成功响应
type loginRes struct {
	Code int
	Msg  string
	Data struct {
		Id       int
		Username string
		Nickname string
		Token    string
	}
}

// 工程列表
type projectListRes struct {
	Code int
	Msg  string
	Data []projectVO
}

// 单工程
type projectVO struct {
	Id         int
	Name       string
	Url        string
	Status     int
	CreateTime string
	Version    string
}

// 获取工程详情响应
type projectDetailRes struct {
	Code int
	Msg  string
	Data struct {
		Id                  int
		Name                string
		Discription         string
		DisplayName         string
		ProjectName         string
		CompileMacro        string
		Mode                int
		Status              int
		UploadFilePath      string
		ProjectHeadLibPath  string
		PlatformTemplateId  int
		BoundaryTest        bool
		ExtDataAddressEnd   string
		ExtDataAddressStart string
		Version             string
	}
}

// 平台模板详情响应
type platformTemplateDetailRes struct {
	Code int
	Msg  string
	Data struct {
		Id              int
		Macro           string
		Name            string
		RelatedFileList []relatedFile
	}
}

// 获取编译器配置响应
type getCompilerRes struct {
	Code int
	Msg  string
	Data projectCompiler
}

type getProjectDirTreeRes struct {
	Code int
	Msg  string
	Data projectDirTree
}

type projectDirTree struct {
	ContextMenu bool             `json:"contextmenu"`
	Expand      bool             `json:"expand"`
	Label       string           `json:"label"`
	Length      int              `json:"length"`
	Path        string           `json:"path"`
	Type        int              `json:"type"`
	Children    []projectDirTree `json:"children"`
}

type addOrUpdateCompilerReq struct {
	Compiler projectCompiler `json:"compiler"`
	EnvVars  []struct{}      `json:"envVars"`
}

type projectCompiler struct {
	Id              *int   `json:"id"`
	ToolchainId     string `json:"toolchainId"`
	ProjectId       int    `json:"projectId"`
	PlatformIds     string `json:"platformIds"`
	Name            string `json:"name"`
	Type            string `json:"type"`
	Compiler        string `json:"compiler"`
	Linker          string `json:"linker"`
	Executor        string `json:"executor"`
	ConstructType   int    `json:"constructType"`
	Target          string `json:"target"`
	IgnoreSystemVar bool   `json:"ignoreSystemVar"`
	CaptureCmd      string `json:"captureCmd"`
	CompilerOption  string `json:"compilerOption"`
	LinkerOption    string `json:"linkerOption"`
	Macro           string `json:"macro"`
	CompileCmd      string `json:"compileCmd"`
	CCompileCmd     string `json:"cCompileCmd"`
	ExecuteCmd      string `json:"executeCmd"`
	LangH           string `json:"langH"`
	LangC           string `json:"langC"`
	LangCpp         string `json:"langCpp"`
	LangS           string `json:"langS"`
	Home            string `json:"home"`
	LibPath         string `json:"libPath"`
}

type relatedFile struct {
	Id    int    `json:"id"`
	Type  int    `json:"type"`
	Files []file `json:"files"`
}

type file struct {
	File    string `json:"file"`
	FileIdx int    `json:"fileIdx"`
}

// 创建平台模板请求
type platformTemplateSaveReq struct {
	Name            string        `json:"name"`
	Operation       string        `json:"operation"`
	ProjectId       int           `json:"projectId"`
	RelatedFileList []relatedFile `json:"relatedFileList"`
	Type            int           `json:"type"`
}

// 解析工程成功响应
type commonRes struct {
	Code int
	Msg  string
}

// 更新工程请求
type updateReq struct {
	Id                  int    `json:"id"`
	BoundaryTest        bool   `json:"boundaryTest"`
	ExtDataAddressEnd   string `json:"extDataAddressEnd"`
	ExtDataAddressStart string `json:"extDataAddressStart"`
	CompileMacro        string `json:"compileMacro"`
	UploadFilePath      string `json:"uploadFilePath"`
	ProjectHeadLibPath  string `json:"projectHeadLibPath"`
	PlatformTemplateId  int    `json:"platformTemplateId"`
}

// 获取工程目标响应
type getProjectTargetRes struct {
	Code int
	Msg  string
	Data struct {
		Id                 int    `json:"id"`
		Header             string `json:"header"`
		ProjectId          int    `json:"projectId"`
		Source             string `json:"source"`
		Branch             string `json:"branch"`
		LocalBranch        string `json:"localBranch"`
		LocalPath          string `json:"localPath"`
		RepositoryUsername string `json:"repositoryUsername"`
		Password           string `json:"password"`
		Url                string `json:"url"`
		Tag                string `json:"tag"`
		Type               int    `json:"type"`
		Version            string `json:"version"`
	}
}

// 创建/更新工程请求
type addOrUpdateReq struct {
	Id                  *int   `json:"id"`
	Description         string `json:"description"`
	Mode                int    `json:"mode"`
	BoundaryTest        bool   `json:"boundaryTest"`
	ExtDataAddressEnd   string `json:"extDataAddressEnd"`
	ExtDataAddressStart string `json:"extDataAddressStart"`
	Name                string `json:"name"`
	Version             string `json:"version"`
}

// 更新工程目标请求
type updateProjectTargetReq struct {
	Id        *int   `json:"id"`
	Header    string `json:"header"`
	ProjectId int    `json:"projectId"`
	Source    string `json:"source"`
}

// 忽略文件请求
type ignoreFilesReq struct {
	CompileIgnoreFileList []ignoreFile `json:"compileIgnoreFileList"`
	ProjectId             int          `json:"projectId"`
}

// 忽略文件请求
type ignoreFile struct {
	FilePath       string `json:"filePath"`
	IsAbsoluteFile string `json:"isAbsoluteFile"`
}

// 生成执行请求
type operateReq struct {
	ProjectId int `json:"projectId"`
	Type      int `json:"type"`
}

// 创建工程成功响应
type addProjectFromRepositoryRes struct {
	Code int
	Msg  string
	Data int
}

type projectStasticRes struct {
	Code int
	Msg  string
	Data struct {
		Id                   int
		Name                 string
		FunctionSize         int
		FunctionAnalysisSize int
		TestcaseSize         int
		Status               int
	}
}

// 增加报告请求
type addReportReq struct {
	DataFormat   int    `json:"dataFormat"`
	NeedDefect   int    `json:"needDefect"`
	ProjectId    int    `json:"projectId"`
	ProjectName  string `json:"projectName"`
	ReportName   string `json:"reportName"`
	ReportType   int    `json:"reportType"`
	SampleId     int    `json:"sampleId"`
	TestcaseType string `json:"testcaseType"`
}

// 报告列表
type listReportRes struct {
	Code int
	Msg  string
	Data struct {
		List []reportVO
	}
}

// 报告信息
type reportVO struct {
	Id        int    `json:"id"`
	IsGen     int    `json:"isGen"`
	Name      string `json:"name"`
	ProjectId int    `json:"projectId"`
}

// 登录获取token
func login(iunitHost string, username string, password string) string {
	// 获取用户ID
	res := postForm(iunitHost, LOGIN+"?username="+username+"&password="+MD5(password), "", nil)
	login := &loginRes{}
	err := json.Unmarshal([]byte(res), login)
	if err != nil {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + LOGIN + "?username=" + username + "&password=" + MD5(password)))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]登录失败：" + err.Error()))
		os.Exit(1)
	}
	return login.Data.Token
}

// 获取工程列表
func getProjectList(iunitHost string, token string) *projectListRes {
	// 获取工程列表
	res := post(iunitHost, PROJECT_LIST, token, "")
	projectList := &projectListRes{}
	err := json.Unmarshal([]byte(res), projectList)
	if err != nil {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + PROJECT_LIST))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]获取工程列表失败：" + err.Error()))
		os.Exit(1)
	}
	return projectList
}

// 根据工程名和版本号获取工程ID
func getProjectByProjectNameAndVerison(iunitHost string, token string, gitUrl string, gitUsername string, gitPassword string, gitBranch string, projectName string, projectVersion string, ignoreFile string) int {
	projectListRes := getProjectList(iunitHost, token)
	projectId := 0
	log.Printf("project.Status: ... %+v", projectListRes)

	for _, project := range projectListRes.Data {
		// 过滤掉非Git创建的工程
		if len(project.Url) == 0 {
			continue
		}
		// 过滤掉与当前Git项目不匹配的工程
		if strings.Replace(project.Url, ".git", "", -1) != strings.Replace(gitUrl, ".git", "", -1) {
			continue
		}
		// 过滤掉工程名不同的工程
		if project.Name != projectName {
			continue
		}
		// 如果版本相同，则直接返回工程ID
		if project.Version == projectVersion {
			log.Println("projectVersion: ... ", projectVersion)
			fmt.Println(LogInfo("[INFO]已存在相同版本，工程ID：" + strconv.Itoa(project.Id)))
			if project.Status == 2 {
				// 忽略文件处理
				ignoreFiles(iunitHost, token, ignoreFile, project.Id)
				return project.Id
			} else {
				log.Println("project.Status: ... ", project.Status)
				fmt.Println(LogInfo("[ERROR][iUnit]功能限制,工程状态非已完成,无法执行"))
				os.Exit(1)
			}
		}
		// 取最新创建的工程ID
		if project.Id > projectId {
			projectId = project.Id
		}
	}
	if projectId == 0 {
		fmt.Println(LogInfo("[ERROR][iUnit]未找到对应工程：") + projectName)
		os.Exit(1)
	}
	fmt.Println(LogInfo("[INFO]不存在相同版本,基于工程：" + strconv.Itoa(projectId) + " 创建新版本"))
	// 创建新版本
	return addProjectByGit(iunitHost, token, strconv.Itoa(projectId), gitUrl, gitUsername, gitPassword, gitBranch, projectVersion, ignoreFile)
}

// 获取工程详情
func getProjectDetail(iunitHost string, token string, projectId string) *projectDetailRes {
	// 获取工程详情
	res := get(iunitHost, PROJECT_DETAIL+"?id="+projectId, token)
	projectDetail := &projectDetailRes{}
	err := json.Unmarshal([]byte(res), projectDetail)
	if err != nil {
		fmt.Println(LogInfo("[ERROR][iUnit]获取工程详情失败：" + err.Error()))
		return nil
	}
	return projectDetail
}

// 获取工程目标
func getProjectTarget(iunitHost string, token string, projectId string) *getProjectTargetRes {
	res := get(iunitHost, GET_PROJECT_TARGET+"/"+projectId, token)
	projectTarget := &getProjectTargetRes{}
	err := json.Unmarshal([]byte(res), projectTarget)
	if err != nil {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + GET_PROJECT_TARGET + "/" + projectId))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]获取工程目标失败：" + err.Error()))
		os.Exit(1)
	}
	return projectTarget
}

// 获取工程编译器配置
func getProjectCompiler(iunitHost string, token string, projectId string) *getCompilerRes {
	// 获取工程编译器配置
	res := get(iunitHost, GET_CURRENT_COMPILER+"?projectId="+projectId, token)
	compiler := &getCompilerRes{}
	err := json.Unmarshal([]byte(res), compiler)
	if err != nil {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + GET_CURRENT_COMPILER + "?projectId=" + projectId))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]获取工程编译器配置失败：" + err.Error()))
		os.Exit(1)
	}
	return compiler
}

// 获取平台模板详情
func getPlatformtemplateDetail(iunitHost string, token string, platformTemplateId int) *platformTemplateDetailRes {
	// 获取平台模板详情
	res := get(iunitHost, PLATFROM_TEMPLATE_DETAIL+"?id="+strconv.Itoa(platformTemplateId), token)
	platformTemplateDetail := &platformTemplateDetailRes{}
	err := json.Unmarshal([]byte(res), platformTemplateDetail)
	if err != nil {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + PLATFROM_TEMPLATE_DETAIL + "?id=" + strconv.Itoa(platformTemplateId)))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]获取平台模板详情失败：" + err.Error()))
		os.Exit(1)
	}
	return platformTemplateDetail
}

// 新建平台模板详情
func savePlatformTemplate(iunitHost string, token string, projectId int, relatedFileList []relatedFile) int {
	req := &platformTemplateSaveReq{}
	req.Name = ""
	req.Operation = "create"
	req.ProjectId = projectId
	req.RelatedFileList = relatedFileList
	req.Type = 0
	platformTemplateSaveReqJson, _ := json.Marshal(req)
	// 创建平台模板
	res := post(iunitHost, PLATFROM_TEMPLATE_SAVE, token, string(platformTemplateSaveReqJson))
	platformTemplateDetail := &platformTemplateDetailRes{}
	err := json.Unmarshal([]byte(res), platformTemplateDetail)
	if err != nil {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + PLATFROM_TEMPLATE_SAVE))
		fmt.Println(LogInfo("请求内容：" + string(platformTemplateSaveReqJson)))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]创建平台模板失败：" + err.Error()))
		os.Exit(1)
	}
	fmt.Println(LogInfo("[SUCCESS][iUnit]创建平台模板成功: 模板ID=" + strconv.Itoa(platformTemplateDetail.Data.Id)))
	return platformTemplateDetail.Data.Id
}

// 添加工程-从git
func addProjectByGit(iunitHost string, token string, projectId string, gitUrl string, gitUsername string, gitPassword string, gitBranch string, projectVersion string, ignoreFile string) int {
	projectDetail := getProjectDetail(iunitHost, token, projectId)
	req := &addOrUpdateReq{}
	req.Name = projectDetail.Data.Name
	req.BoundaryTest = projectDetail.Data.BoundaryTest
	req.ExtDataAddressEnd = projectDetail.Data.ExtDataAddressEnd
	req.ExtDataAddressStart = projectDetail.Data.ExtDataAddressStart
	req.Description = projectDetail.Data.Discription
	req.Mode = projectDetail.Data.Mode
	// req.Version = projectVersion[len(projectVersion)-6:]
	req.Version = "1.0.0"
	addOrUpdateReqJson, _ := json.Marshal(req)
	res := post(iunitHost, ADD_OR_UPDATE_PROJECT, token, string(addOrUpdateReqJson))
	addProject := &projectDetailRes{}
	err := json.Unmarshal([]byte(res), addProject)
	if err != nil {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + ADD_OR_UPDATE_PROJECT))
		fmt.Println(LogInfo("请求内容: " + string(addOrUpdateReqJson)))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]创建工程失败：" + err.Error()))
		os.Exit(1)
	}

	fmt.Println(LogInfo("[SUCCESS][iUnit]创建工程成功: 工程ID=" + strconv.Itoa(addProject.Data.Id)))
	// 获取原工程编译器配置
	compiler := getProjectCompiler(iunitHost, token, projectId)
	// 更新新建工程编译器配置
	addOrUpdateCompiler(iunitHost, token, strconv.Itoa(addProject.Data.Id), compiler)
	// 从仓库添加工程
	addProjectFromRepository(iunitHost, token, strconv.Itoa(addProject.Data.Id), projectId, gitUrl, gitUsername, gitPassword, gitBranch, projectVersion, ignoreFile)
	// 忽略文件
	ignoreFiles(iunitHost, token, ignoreFile, addProject.Data.Id)
	return addProject.Data.Id
}

// 添加工程-从仓库
func addProjectFromRepository(iunitHost string, token string, projectId string, oldProjectId string, gitUrl string, gitUsername string, gitPassword string, gitBranch string, projectVersion string, ignoreFile string) int {
	formData := url.Values{
		"projectId":          {projectId},
		"url":                {gitUrl},
		"repositoryUsername": {gitUsername},
		"password":           {gitPassword},
		"branch":             {gitBranch},
		"targetType":         {"1"},
		"version":            {projectVersion},
		"oldProjectId":       {oldProjectId},
	}
	res := postForm(iunitHost, ADD_PROJECT_FROM_REPOSITORY, token, formData)
	addProjectFromRepositoryRes := &addProjectFromRepositoryRes{}
	err := json.Unmarshal([]byte(res), addProjectFromRepositoryRes)
	if err != nil || addProjectFromRepositoryRes.Code != 200 {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + ADD_PROJECT_FROM_REPOSITORY))
		fmt.Println(LogInfo("请求内容: " + formData.Encode()))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]创建工程失败：" + err.Error()))
		os.Exit(1)
	}
	return addProjectFromRepositoryRes.Data
}

// 添加或更新编译器
func addOrUpdateCompiler(iunitHost string, token string, projectId string, getComgetCompilerRes *getCompilerRes) {
	req := &addOrUpdateCompilerReq{}
	req.Compiler = getComgetCompilerRes.Data
	req.Compiler.Id = nil
	req.EnvVars = []struct{}{}
	projectIdInt, err := strconv.Atoi(projectId)
	if err != nil {
		fmt.Println(LogInfo("[ERROR]无效项目ID: " + err.Error()))
		os.Exit(1)
	}
	req.Compiler.ProjectId = projectIdInt
	addOrUpdateReqJson, _ := json.Marshal(req)
	// 更新编译器
	res := post(iunitHost, ADD_OR_UPDATE_COMPILER+"?projectId="+projectId, token, string(addOrUpdateReqJson))
	addOrUpdateCompiler := &commonRes{}
	err = json.Unmarshal([]byte(res), addOrUpdateCompiler)
	if err != nil || addOrUpdateCompiler.Code != 200 {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + ADD_OR_UPDATE_COMPILER + "?projectId=" + projectId))
		fmt.Println(LogInfo("请求内容: " + string(addOrUpdateReqJson)))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]创建编译器失败：" + err.Error()))
		os.Exit(1)
	}
}

// 更新工程
func updateProjectByGit(iunitHost string, token string, projectId int, compileMacro string, extDataAddressStart string, extDataAddressEnd string, uploadFilePath string, projectHeadLibPath string, platformTemplateId int) {
	req := &updateReq{}
	req.Id = projectId
	req.CompileMacro = compileMacro
	req.ExtDataAddressEnd = extDataAddressEnd
	req.ExtDataAddressStart = extDataAddressStart
	req.UploadFilePath = uploadFilePath
	req.ProjectHeadLibPath = projectHeadLibPath
	if platformTemplateId != 0 {
		platformTemplateDetail := getPlatformtemplateDetail(iunitHost, token, platformTemplateId)
		req.PlatformTemplateId = savePlatformTemplate(iunitHost, token, projectId, platformTemplateDetail.Data.RelatedFileList)
	}
	updateReqJson, _ := json.Marshal(req)
	// 更新工程
	res := post(iunitHost, UPDATE_PROJECT, token, string(updateReqJson))
	updateProject := &commonRes{}
	err := json.Unmarshal([]byte(res), updateProject)
	if err != nil || updateProject.Code != 200 {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + UPDATE_PROJECT))
		fmt.Println(LogInfo("请求内容: " + string(updateReqJson)))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]更新工程失败：" + err.Error()))
		os.Exit(1)
	}
}

// 忽略文件
func ignoreFiles(iunitHost string, token string, ignoreCompileFiles string, projectId int) {
	req := &ignoreFilesReq{}
	files := strings.Split(ignoreCompileFiles, ",")
	ignoreCompileFilesList := make([]ignoreFile, len(files))
	for i, file := range files {
		ignoreCompileFilesList[i] = ignoreFile{file, "false"}
	}
	req.CompileIgnoreFileList = ignoreCompileFilesList
	req.ProjectId = projectId
	ignoreFileReqJson, _ := json.Marshal(req)
	fmt.Println(LogInfo("[INFO]忽略文件：" + ignoreCompileFiles))
	// 忽略文件
	post(iunitHost, IGNORE_FILE, token, string(ignoreFileReqJson))
}

// 解析工程
func analyzeProject(iunitHost string, token string, projectId string, analyzedType string, timeout string) {
	formData := url.Values{
		"id":   {projectId},
		"type": {analyzedType},
	}
	// 获取用户ID
	res := postForm(iunitHost, ANALYZE_PROJECT, token, formData)
	analyzeProject := &commonRes{}
	err := json.Unmarshal([]byte(res), analyzeProject)
	if err != nil || analyzeProject.Code != 200 {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + ANALYZE_PROJECT))
		fmt.Println(LogInfo("请求内容: " + formData.Encode()))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]解析工程详情失败：" + err.Error()))
		os.Exit(1)
	}
	// 定时轮询状态
	ticker := time.NewTicker(3 * time.Second)
	defer ticker.Stop()

	// 设定一个跳出循环的条件，例如查询次数达到10次
	queryCount := 0

	timeoutNum, _ := strconv.Atoi(timeout)

	for range ticker.C {
		fmt.Println(LogInfo("解析执行中..."))
		// 测试任务执行结果查询
		projectDetail := getProjectDetail(iunitHost, token, projectId)
		if projectDetail == nil {
			fmt.Println(LogInfo("[ERROR][iUnit]获取工程详情为空"))
			continue
		}
		if projectDetail.Data.Status == 3 {
			fmt.Println(LogInfo("[ERROR][iUnit]解析工程失败"))
			os.Exit(1)
		}
		queryCount++
		// 判断是否达到跳出循环的条件
		if queryCount >= timeoutNum {
			fmt.Println(LogInfo("[ERROR][iUnit]解析执行超时，退出！"))
			os.Exit(1)
		}
		if projectDetail.Data.Status == 2 {
			fmt.Println(LogInfo("[SUCCESS]解析工程成功: 工程ID=" + projectId))
			break
		}
	}
}

func operateProject(iunitHost string, token string, projectId int, operateType int, timeout string) {
	req := &operateReq{}
	req.ProjectId = projectId
	req.Type = operateType
	operateReqJson, _ := json.Marshal(req)
	// 发起测试
	res := post(iunitHost, OPERATE_PROJECT, token, string(operateReqJson))
	operateProject := &commonRes{}
	err := json.Unmarshal([]byte(res), operateProject)
	if err != nil || operateProject.Code != 200 {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + OPERATE_PROJECT))
		fmt.Println(LogInfo("请求内容: " + string(operateReqJson)))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]执行工程失败：" + err.Error()))
		os.Exit(1)
	}
	fmt.Println(LogInfo("[SUCCESS]执行工程发起成功: 工程ID=" + strconv.Itoa(projectId)))

	// 定时轮询状态
	ticker := time.NewTicker(3 * time.Second)
	defer ticker.Stop()

	// 设定一个跳出循环的条件，例如查询次数达到10次
	queryCount := 0

	timeoutNum, _ := strconv.Atoi(timeout)

	for range ticker.C {
		// 获取进度
		getProjectStastict(iunitHost, token, strconv.Itoa(projectId))
		// 测试任务执行结果查询
		projectDetail := getProjectDetail(iunitHost, token, strconv.Itoa(projectId))
		queryCount++
		// 判断是否达到跳出循环的条件
		if queryCount >= timeoutNum {
			fmt.Println(LogInfo("[ERROR][iUnit]单元测试执行超时，退出!"))
			os.Exit(1)
		}
		if projectDetail.Data.Status == 2 {
			fmt.Println(LogInfo("[SUCCESS]单元测试执行完成，请在准入检测中查看结果."))
			break
		}
	}
}

// 获取工程数据
func getProjectStastict(iunitHost string, token string, projectId string) {
	formData := url.Values{
		"projectId": {projectId},
	}
	res := postForm(iunitHost, PROJECT_STASTIC, token, formData)
	projectStastic := &projectStasticRes{}
	err := json.Unmarshal([]byte(res), projectStastic)
	if err != nil || projectStastic.Code != 200 {
		fmt.Println(LogInfo("请求服务: " + iunitHost))
		fmt.Println(LogInfo("请求路径: " + PROJECT_STASTIC))
		fmt.Println(LogInfo("请求内容: " + formData.Encode()))
		fmt.Println(LogInfo("请求返回：" + res))
		fmt.Println(LogInfo("[ERROR][iUnit]获取工程列表失败：" + err.Error()))
		return
	}
	fmt.Println(LogInfo("当前函数分析进度：" + strconv.Itoa(projectStastic.Data.FunctionAnalysisSize) + "/" + strconv.Itoa(projectStastic.Data.FunctionSize) + "，已生成并执行用例：" + strconv.Itoa(projectStastic.Data.TestcaseSize) + "个"))
	fmt.Println(LogInfo("当前函数分析进度：" + strconv.Itoa(projectStastic.Data.FunctionAnalysisSize) + "/" + strconv.Itoa(projectStastic.Data.FunctionSize) + "，已生成并执行用例：" + strconv.Itoa(projectStastic.Data.TestcaseSize) + "个"))
	fmt.Println(LogInfo("当前函数分析进度：" + strconv.Itoa(projectStastic.Data.FunctionAnalysisSize) + "/" + strconv.Itoa(projectStastic.Data.FunctionSize) + "，已生成并执行用例：" + strconv.Itoa(projectStastic.Data.TestcaseSize) + "个"))
}

// 增加报告
func addReport(iunitHost string, token string, projectId int, reportName string, sampleId int, testcaseType string) {
	projectDetail := getProjectDetail(iunitHost, token, strconv.Itoa(projectId))
	req := &addReportReq{}
	req.DataFormat = 0
	req.NeedDefect = 1
	req.ProjectId = projectId
	req.ProjectName = projectDetail.Data.ProjectName + " " + projectDetail.Data.Version
	req.ReportName = reportName
	req.ReportType = 0
	req.SampleId = sampleId
	req.TestcaseType = testcaseType
	addReportReqJson, _ := json.Marshal(req)
	post(iunitHost, ADD_REPORT, token, string(addReportReqJson))
	getReport(iunitHost, token, reportName, projectId)
}

// 获取报告
func getReport(iunitHost string, token string, reportName string, projectId int) {
	formData := url.Values{
		"pageNum":    {"1"},
		"pageSize":   {"20"},
		"reportName": {reportName},
		"projectId":  {strconv.Itoa(projectId)},
	}
	res := postForm(iunitHost, LIST_REPORT, token, formData)
	listReport := &listReportRes{}
	err := json.Unmarshal([]byte(res), listReport)
	if err != nil || listReport.Data.List == nil || len(listReport.Data.List) == 0 {
		fmt.Println(LogInfo("iUnit测试报告生成中..."))
		os.Exit(0)
	} else {
		if listReport.Data.List[0].IsGen == 1 {
			fmt.Println(LogInfo("[SUCCSESS]iUnit测试报告生成成功"))
			os.Exit(0)
		}
	}
	// 定时轮询状态
	ticker := time.NewTicker(2 * time.Second)
	defer ticker.Stop()

	// 设定一个跳出循环的条件，例如查询次数达到10次
	queryCount := 0
	timeoutNum := 5
	for range ticker.C {
		// 获取报告
		getReport(iunitHost, token, reportName, projectId)
		queryCount++
		// 判断是否达到跳出循环的条件
		if queryCount >= timeoutNum {
			fmt.Println(LogInfo("[ERROR][iUnit]报告生成超时，退出！"))
			os.Exit(1)
		}
	}

}

// 通用POST方法
func post(host string, path string, token string, requestBody string) string {
	// fmt.Println("host:", LogInfo(host))
	// fmt.Println("path:", LogInfo(path))
	// fmt.Println("requestBody:", LogInfo(requestBody))
	requestMethod := "POST"
	req, _ := http.NewRequest(requestMethod, host+path, strings.NewReader(requestBody))
	// 添加认证请求头
	req.Header.Add("Authorization", token)
	req.Header.Add("Content-Type", "application/json;charset=UTF-8")
	client := &http.Client{}
	res, err := client.Do(req)
	if err != nil {
		fmt.Println(err)
		fmt.Println(LogInfo("[ERROR][iUnit]服务异常，请求iUnit接口失败"))
		os.Exit(1)
	}
	defer res.Body.Close()
	body, error := io.ReadAll(res.Body)
	if error != nil {
		fmt.Println(err)
		fmt.Println(LogInfo("[ERROR][iUnit]服务异常，解析iUnit接口返回失败"))
		os.Exit(1)
	}
	// fmt.Println("response:", LogInfo(string(body)))
	return string(body)
}

// 通用POST方法
func postForm(host string, path string, token string, formData url.Values) string {
	// fmt.Println("host:", LogInfo(host))
	// fmt.Println("path:", LogInfo(path))
	requestMethod := "POST"
	req, err := http.NewRequest(requestMethod, host+path, strings.NewReader(formData.Encode()))
	if err != nil {
		fmt.Println(err)
		fmt.Println(LogInfo("[ERROR]构建iUnit接口请求失败"))
		os.Exit(1)
	}
	// 添加认证请求头
	req.Header.Add("Authorization", token)
	req.Header.Add("Content-Type", "application/x-www-form-urlencoded") // 设置表单数据类型
	client := &http.Client{}
	res, err := client.Do(req)
	if err != nil {
		fmt.Println(err)
		fmt.Println(LogInfo("[ERROR][iUnit]服务异常，请求iUnit接口失败"))
		os.Exit(1)
	}
	defer res.Body.Close()
	body, err := io.ReadAll(res.Body)
	if err != nil {
		fmt.Println(err)
		fmt.Println(LogInfo("[ERROR][iUnit]服务异常，解析iUnit接口返回失败"))
		os.Exit(1)
	}
	// fmt.Println("response:", LogInfo(string(body)))
	return string(body)
}

// 通用GET方法
func get(host string, path string, token string) string {
	// fmt.Println("host:", LogInfo(host))
	// fmt.Println("path:", LogInfo(path))
	requestMethod := "GET"
	req, _ := http.NewRequest(requestMethod, host+path, nil)
	// 添加认证请求头
	req.Header.Add("Authorization", token)
	req.Header.Add("Content-Type", "application/json;charset=UTF-8")
	client := &http.Client{}
	res, err := client.Do(req)
	if err != nil {
		fmt.Println(err)
		fmt.Println(LogInfo("[ERROR][iUnit]服务异常，请求iUnit接口失败"))
		os.Exit(1)
	}
	defer res.Body.Close()
	body, error := io.ReadAll(res.Body)
	if error != nil {
		fmt.Println(err)
		fmt.Println(LogInfo("[ERROR][iUnit]服务异常，解析iUnit接口返回失败"))
		os.Exit(1)
	}
	// fmt.Println("response:", LogInfo(string(body)))
	return string(body)
}

// 打印日志，根据操作系统切换编码格式
func LogInfo(str string) string {
	sysType := runtime.GOOS
	if sysType == "windows" {
		// windows系统
		// return Utf8ToGbk(str)
		return (str)
	}
	// linux系统
	return str
}

// Utf8ToGbk UTF-8转GBK
func Utf8ToGbk(str string) string {
	s := []byte(str)
	reader := transform.NewReader(bytes.NewReader(s), simplifiedchinese.GBK.NewEncoder())
	d, e := io.ReadAll(reader)
	if e != nil {
		return ""
	}
	return string(d)
}

// 生成32位MD5
func MD5(text string) string {
	ctx := md5.New()
	ctx.Write([]byte(text))
	return strings.ToUpper(hex.EncodeToString(ctx.Sum(nil)))
}

var (
	Version   string
	Branch    string
	Commit    string
	Msg       string
	BuildTime string
)

func main() {

	var args struct {
		Version bool `arg:"-v,--version" help:"显示版本信息"`
		// iUnit服务地址，例如 http://172.26.195.52:8082
		IUnitHost string `arg:"required"`
		// iUnit用户名，例如 admin
		IUnitUsername string `arg:"required"`
		// iUnit密码，例如 E10ADC3949BA59ABBE56E057F20F883E
		IUnitPassword string `arg:"required"`
		// 工程名称
		ProjectName string `arg:"required"`
		// gitUrl
		GitUrl string `arg:"required"`
		// gitUsername
		GitUsername string `arg:"required"`
		// gitPassword
		GitPassword string `arg:"required"`
		// 分支
		GitBranch string `arg:"required"`
		// 提交版本
		CommitId string `arg:"required"`
		// 超时时间
		Timeout string `arg:"required"`
		// buildId
		BuildId string `arg:"required"`
		// jobId
		JobId string `arg:"required"`
		// sampleId
		SampleId string `arg:"required"`
		// testcaseType
		TestcaseType string `arg:"required"`
		// 忽略文件
		IgnoreFile string `arg:"required"`
	}

	arg.MustParse(&args)
	version := args.Version
	if version {
		log.Printf("Version: %s\n", Version)
		log.Printf("Branch: %s\n", Branch)
		log.Printf("Commit: %s\n", Commit)
		log.Printf("Msg: %s\n", Msg)
		log.Printf("BuildTime: %s\n", BuildTime)
		os.Exit(0)
	}
	iUnitHost := args.IUnitHost
	iUnitUsername := args.IUnitUsername
	iUnitPassword := args.IUnitPassword
	projectName := args.ProjectName
	gitUrl := args.GitUrl
	gitUsername := args.GitUsername
	gitPassword := args.GitPassword
	gitBranch := args.GitBranch
	commitId := args.CommitId
	timeout := args.Timeout
	buildId := args.BuildId
	jobId := args.JobId
	sampleId := args.SampleId
	testcaseType := args.TestcaseType
	ignoreFile := args.IgnoreFile

	inum := 0
	for ; inum < 100000; inum++ {
		// 登录获取token
		token := login(iUnitHost, iUnitUsername, iUnitPassword)
		log.Println("login: ... ", iUnitHost, iUnitUsername, iUnitPassword)
		// 根据工程名和提交号获取工程ID
		project := getProjectByProjectNameAndVerison(iUnitHost, token, gitUrl, gitUsername, gitPassword, gitBranch, projectName, commitId, ignoreFile)
		log.Println("getProjectByProjectNameAndVerison: ... ", project)

		// 解析工程
		analyzeProject(iUnitHost, token, strconv.Itoa(project), "1", timeout)
		log.Println("analyzeProject: ... ", project)

		// 执行工程
		operateProject(iUnitHost, token, project, 3, timeout)
		log.Println("operateProject: ... ", project)

		// // 报告模板ID，14对应601定制模板
		sample, _ := strconv.Atoi(sampleId)
		// 生成报告
		addReport(iUnitHost, token, project, buildId+"-"+jobId, sample, testcaseType)

		time.Sleep(500 * time.Millisecond)

	}

}
