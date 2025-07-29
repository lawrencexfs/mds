package main

import (
	"log"
	"time"

	tgbotapi "github.com/go-telegram-bot-api/telegram-bot-api/v5"
)

// 用户订阅信息
type Subscription struct {
	UserID       int64
	Expiry       time.Time
	IsSubscribed bool
}

var subscriptions = make(map[int64]Subscription)

func main() {
	// 替换为你的 API Token
	bot, err := tgbotapi.NewBotAPI("YOUR_API_TOKEN")
	if err != nil {
		log.Panic(err)
	}

	bot.Debug = true

	log.Printf("Authorized on account %s", bot.Self.UserName)

	// 设置命令处理器
	u := tgbotapi.NewUpdate(0)
	u.Timeout = 60

	updates, err := bot.GetUpdatesChan(u)

	// 处理用户命令
	go func() {
		for update := range updates {
			if update.Message == nil {
				continue
			}

			log.Printf("[%s] %s", update.Message.From.UserName, update.Message.Text)

			if update.Message.Text == "/subscribe" {
				handleSubscription(update, bot)
			} else {
				msg := tgbotapi.NewMessage(update.Message.Chat.ID, update.Message.Text)
				bot.Send(msg)
			}
		}
	}()

	// 定时任务
	go func() {
		for {
			time.Sleep(60 * time.Second) // 每60秒执行一次
			checkSubscriptions(bot)
		}
	}()

	// 阻止主进程退出
	select {}
}

func handleSubscription(update tgbotapi.Update, bot *tgbotapi.BotAPI) {
	userID := update.Message.From.ID
	expiry := time.Now().AddDate(0, 1, 0) // 1个月后到期
	subscriptions[userID] = Subscription{
		UserID:       userID,
		Expiry:       expiry,
		IsSubscribed: true,
	}

	msg := tgbotapi.NewMessage(update.Message.Chat.ID, "You have successfully subscribed for 1 month!")
	bot.Send(msg)
}

func checkSubscriptions(bot *tgbotapi.BotAPI) {
	now := time.Now()
	for userID, sub := range subscriptions {
		if now.After(sub.Expiry) {
			delete(subscriptions, userID)
			msg := tgbotapi.NewMessage(userID, "Your subscription has expired.")
			bot.Send(msg)
		}
	}
}
