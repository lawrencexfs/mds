package main

import (
	"log"

	tgbotapi "github.com/go-telegram-bot-api/telegram-bot-api/v5"
)

// func main() {
// 	providerToken := "YOUR_API_TOKEN"
// 	bot, err := tgbotapi.NewBotAPI(providerToken)
// 	if err != nil {
// 		log.Panic(err)
// 	}

// 	bot.Debug = true
// 	log.Printf("Authorized on account %s", bot.Self.UserName)
// 	price := []tgbotapi.LabeledPrice{}

// 	// 发送发票
// 	invoice := tgbotapi.NewInvoice(111222333, "Test Invoice", "This is a test invoice", "123456789", providerToken, "x", "284685063:TEST:OTRiZjEyYjU4MzA2", price)
// 	invoice.Prices = []tgbotapi.LabeledPrice{
// 		{Label: "价格1", Amount: 1000},
// 		{Label: "价格2", Amount: 1002},
// 		{Label: "折扣", Amount: -10},
// 	}
// 	invoice.Currency = "CNY"
// 	invoice.NeedName = true
// 	invoice.NeedPhoneNumber = true
// 	invoice.NeedEmail = true
// 	invoice.SendPhoneNumberToProvider = true
// 	invoice.SendEmailToProvider = true

// 	_, err = bot.Send(invoice)
// 	if err != nil {
// 		log.Println(err)
// 	}
// }

func main() {
	bot, err := tgbotapi.NewBotAPI("YOUR_API_TOKEN")
	if err != nil {
		log.Panic(err)
	}

	bot.Debug = true
	log.Printf("Authorized on account %s", bot.Self.UserName)

	u := tgbotapi.NewUpdate(0)
	u.Timeout = 60

	updates := bot.GetUpdatesChan(u)
	// if err != nil {
	// 	log.Panic(err)
	// }

	go func() {
		for update := range updates {
			if update.PreCheckoutQuery != nil {
				handlePreCheckoutQuery(bot, update.PreCheckoutQuery)
			} else if update.Message != nil && update.Message.SuccessfulPayment != nil {
				handleSuccessfulPayment(bot, update.Message)
			}
		}
	}()

	select {}
}

func handlePreCheckoutQuery(bot *tgbotapi.BotAPI, query *tgbotapi.PreCheckoutQuery) {
	// 检查支付请求是否有效
	if query.InvoicePayload == "YOUR_PAYLOAD" {
		// 批准支付请求
		msg := tgbotapi.NewAnswerPreCheckoutQuery(query.ID, true)
		bot.Send(msg)
	} else {
		// 拒绝支付请求
		msg := tgbotapi.NewAnswerPreCheckoutQuery(query.ID, false)
		msg.ErrorMessage = "Invalid payload"
		bot.Send(msg)
	}
}

func handleSuccessfulPayment(bot *tgbotapi.BotAPI, msg *tgbotapi.Message) {
	// 确认支付成功
	log.Printf("Payment successful: %+v", msg.SuccessfulPayment)

	// 执行业务逻辑，如发货、更新订单状态等
	// 例如，向用户发送确认消息
	confirmMsg := tgbotapi.NewMessage(msg.Chat.ID, "Thank you for your payment!")
	bot.Send(confirmMsg)
}
