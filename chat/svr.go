package chat

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"sync"
	"time"

	"github.com/gorilla/websocket"
)

// 消息类型常量
const (
	MsgTypeJoin    = "join"    // 用户加入
	MsgTypeLeave   = "leave"   // 用户离开
	MsgTypeMessage = "message" // 普通消息
	MsgTypeSystem  = "system"  // 系统消息
	MsgTypeList    = "list"    // 用户列表
)

// 消息结构
type Message struct {
	Type     string      `json:"type"`           // 消息类型
	Content  string      `json:"content"`        // 消息内容
	From     string      `json:"from"`           // 发送者ID
	FromName string      `json:"fromName"`       // 发送者名称
	Room     string      `json:"room"`           // 房间ID
	Time     time.Time   `json:"time"`           // 发送时间
	Data     interface{} `json:"data,omitempty"` // 额外数据
}

// 用户结构
type User struct {
	ID       string          // 用户唯一标识
	Name     string          // 用户名称
	Conn     *websocket.Conn // WebSocket连接
	Room     string          // 所在房间
	SendChan chan Message    // 消息发送通道
	IsActive bool            // 是否活跃
}

// 房间结构
type Room struct {
	ID    string           // 房间ID
	Name  string           // 房间名称
	Users map[string]*User // 用户ID -> 用户对象
	mutex sync.RWMutex     // 读写锁，保护用户列表
}

// 聊天室服务
type ChatService struct {
	Rooms map[string]*Room // 房间ID -> 房间对象
	Users map[string]*User // 用户ID -> 用户对象
	mutex sync.RWMutex     // 读写锁，保护房间和用户列表
}

// 创建新的聊天室服务
func NewChatService() *ChatService {
	return &ChatService{
		Rooms: make(map[string]*Room),
		Users: make(map[string]*User),
	}
}

// 创建新房间
func (cs *ChatService) CreateRoom(roomID, roomName string) *Room {
	cs.mutex.Lock()
	defer cs.mutex.Unlock()

	// 检查房间是否已存在
	if _, exists := cs.Rooms[roomID]; exists {
		return cs.Rooms[roomID]
	}

	// 创建新房间
	room := &Room{
		ID:    roomID,
		Name:  roomName,
		Users: make(map[string]*User),
	}

	cs.Rooms[roomID] = room
	log.Printf("创建房间: %s (%s)", roomName, roomID)

	return room
}

// 获取房间
func (cs *ChatService) GetRoom(roomID string) *Room {
	cs.mutex.RLock()
	defer cs.mutex.RUnlock()

	return cs.Rooms[roomID]
}

// 删除房间
func (cs *ChatService) DeleteRoom(roomID string) {
	cs.mutex.Lock()
	defer cs.mutex.Unlock()

	if room, exists := cs.Rooms[roomID]; exists {
		// 通知所有用户房间已关闭
		room.BroadcastMessage(Message{
			Type:    MsgTypeSystem,
			Content: "房间已关闭",
			From:    "system",
			Room:    roomID,
			Time:    time.Now(),
		})

		// 关闭所有用户连接
		for _, user := range room.Users {
			user.Close()
		}

		delete(cs.Rooms, roomID)
		log.Printf("删除房间: %s", roomID)
	}
}

// 添加用户到房间
func (cs *ChatService) AddUserToRoom(user *User, roomID string) bool {
	cs.mutex.RLock()
	room, exists := cs.Rooms[roomID]
	cs.mutex.RUnlock()

	if !exists {
		return false
	}

	room.mutex.Lock()
	// 检查用户是否已在房间中
	if _, exists := room.Users[user.ID]; exists {
		room.mutex.Unlock()
		return true
	}

	// 将用户添加到房间
	room.Users[user.ID] = user
	user.Room = roomID
	room.mutex.Unlock()

	// 广播用户加入消息
	room.BroadcastMessage(Message{
		Type:     MsgTypeJoin,
		Content:  fmt.Sprintf("%s 加入了房间", user.Name),
		From:     "system",
		FromName: "系统",
		Room:     roomID,
		Time:     time.Now(),
	})

	// 发送当前用户列表给新用户
	user.SendMessage(Message{
		Type:     MsgTypeList,
		Content:  "当前在线用户",
		From:     "system",
		FromName: "系统",
		Room:     roomID,
		Time:     time.Now(),
		Data:     room.GetUserList(),
	})

	log.Printf("用户 %s 加入房间 %s", user.Name, roomID)
	return true
}

// 从房间移除用户
func (cs *ChatService) RemoveUserFromRoom(userID, roomID string) {
	cs.mutex.RLock()
	room, exists := cs.Rooms[roomID]
	cs.mutex.RUnlock()

	if !exists {
		return
	}

	room.mutex.Lock()
	user, exists := room.Users[userID]
	if exists {
		delete(room.Users, userID)
		user.Room = ""
	}
	room.mutex.Unlock()

	if exists {
		// 广播用户离开消息
		room.BroadcastMessage(Message{
			Type:     MsgTypeLeave,
			Content:  fmt.Sprintf("%s 离开了房间", user.Name),
			From:     "system",
			FromName: "系统",
			Room:     roomID,
			Time:     time.Now(),
		})

		log.Printf("用户 %s 离开房间 %s", user.Name, roomID)
	}
}

// 获取用户列表
func (room *Room) GetUserList() []map[string]string {
	room.mutex.RLock()
	defer room.mutex.RUnlock()

	userList := make([]map[string]string, 0, len(room.Users))
	for _, user := range room.Users {
		userList = append(userList, map[string]string{
			"id":   user.ID,
			"name": user.Name,
		})
	}

	return userList
}

// 广播消息到房间所有用户
func (room *Room) BroadcastMessage(msg Message) {
	room.mutex.RLock()
	defer room.mutex.RUnlock()

	for _, user := range room.Users {
		// 不向发送者重复发送消息
		if user.ID != msg.From {
			user.SendMessage(msg)
		}
	}
}

// 添加用户
func (cs *ChatService) AddUser(user *User) {
	cs.mutex.Lock()
	defer cs.mutex.Unlock()

	cs.Users[user.ID] = user
	user.IsActive = true

	log.Printf("用户 %s 加入系统", user.Name)
}

// 移除用户
func (cs *ChatService) RemoveUser(userID string) {
	cs.mutex.Lock()
	defer cs.mutex.Unlock()

	if user, exists := cs.Users[userID]; exists {
		// 如果用户在房间中，先从房间移除
		if user.Room != "" {
			cs.RemoveUserFromRoom(userID, user.Room)
		}

		user.IsActive = false
		delete(cs.Users, userID)

		log.Printf("用户 %s 离开系统", user.Name)
	}
}

// 获取用户
func (cs *ChatService) GetUser(userID string) *User {
	cs.mutex.RLock()
	defer cs.mutex.RUnlock()

	return cs.Users[userID]
}

// 发送消息给用户
func (user *User) SendMessage(msg Message) {
	select {
	case user.SendChan <- msg:
		// 消息已发送
	default:
		// 通道已满，关闭连接
		log.Printf("用户 %s 消息通道已满，关闭连接", user.Name)
		user.Close()
	}
}

// 关闭用户连接
func (user *User) Close() {
	if user.Conn != nil {
		user.Conn.Close()
		user.Conn = nil
	}
}

// WebSocket连接处理
func HandleWebSocket(chatService *ChatService, w http.ResponseWriter, r *http.Request) {
	// 解析URL参数
	roomID := r.URL.Query().Get("room")
	userID := r.URL.Query().Get("user")
	userName := r.URL.Query().Get("name")

	if roomID == "" || userID == "" || userName == "" {
		http.Error(w, "缺少必要参数", http.StatusBadRequest)
		return
	}

	// 升级HTTP连接为WebSocket连接
	upgrader := websocket.Upgrader{
		CheckOrigin: func(r *http.Request) bool {
			return true // 允许所有来源
		},
	}

	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Printf("WebSocket升级失败: %v", err)
		return
	}

	// 创建用户对象
	user := &User{
		ID:       userID,
		Name:     userName,
		Conn:     conn,
		Room:     "",
		SendChan: make(chan Message, 256), // 缓冲通道，防止阻塞
		IsActive: true,
	}

	// 添加用户到系统
	chatService.AddUser(user)

	// 确保用户离开时被正确清理
	defer func() {
		chatService.RemoveUser(userID)
		user.Close()
	}()

	// 加入房间
	if !chatService.AddUserToRoom(user, roomID) {
		// 房间不存在，创建默认房间
		chatService.CreateRoom(roomID, "默认房间")
		chatService.AddUserToRoom(user, roomID)
	}

	// 启动消息发送协程
	go func() {
		for msg := range user.SendChan {
			// 序列化消息
			data, err := json.Marshal(msg)
			if err != nil {
				log.Printf("消息序列化失败: %v", err)
				continue
			}

			// 发送消息
			err = conn.WriteMessage(websocket.TextMessage, data)
			if err != nil {
				log.Printf("发送消息失败: %v", err)
				user.Close()
				break
			}
		}
	}()

	// 处理接收到的消息
	for {
		// 设置读取超时
		conn.SetReadDeadline(time.Now().Add(90 * time.Second))

		// 读取消息
		_, message, err := conn.ReadMessage()
		if err != nil {
			if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
				log.Printf("WebSocket连接意外关闭: %v", err)
			}
			break
		}

		// 解析消息
		var msg Message
		if err := json.Unmarshal(message, &msg); err != nil {
			log.Printf("消息解析失败: %v", err)
			continue
		}

		// 设置消息发送者
		msg.From = userID
		msg.FromName = user.Name
		msg.Room = roomID
		msg.Time = time.Now()

		// 处理不同类型的消息
		switch msg.Type {
		case MsgTypeMessage:
			// 广播消息到房间
			room := chatService.GetRoom(roomID)
			if room != nil {
				room.BroadcastMessage(msg)
				log.Printf("用户 %s 发送消息: %s", user.Name, msg.Content)
			}
			// 可以添加更多消息类型处理
		}
	}
}

// 首页处理 - 从文件读取HTML内容
func HandleHome(w http.ResponseWriter, r *http.Request) {
	// 读取HTML文件内容
	htmlBytes, err := ioutil.ReadFile("m.html")
	if err != nil {
		http.Error(w, "无法读取HTML文件: "+err.Error(), http.StatusInternalServerError)
		return
	}

	// 设置Content-Type并返回HTML内容
	w.Header().Set("Content-Type", "text/html")
	w.Write(htmlBytes)
}

// func main() {

// }
