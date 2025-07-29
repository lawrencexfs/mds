package main

import (
	"log"
	"time"

	tgbotapi "github.com/go-telegram-bot-api/telegram-bot-api/v5"
)

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

			msg := tgbotapi.NewMessage(update.Message.Chat.ID, update.Message.Text)
			bot.Send(msg)
		}
	}()

	// 定时任务
	go func() {
		for {
			time.Sleep(60 * time.Second) // 每60秒执行一次
			sendScheduledMessage(bot)
		}
	}()

	// 阻止主进程退出
	select {}
}

func sendScheduledMessage(bot *tgbotapi.BotAPI) {
	msg := tgbotapi.NewMessage(123456789, "This is a scheduled message!") // 替换为你的聊天ID
	_, err := bot.Send(msg)
	if err != nil {
		log.Println(err)
	}
}
