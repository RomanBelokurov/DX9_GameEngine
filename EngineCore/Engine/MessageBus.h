#ifndef __MESSAGE_BUS_H__
#define __MESSAGE_BUS_H__
#pragma once


#include <iostream>
#include <string>
#include <functional>
#include <queue>
#include <vector>

class Message
{
public:
    Message(const std::string event)
    {
        messageEvent = event;
    }

    std::string getEvent()
    {
        return messageEvent;
    }
private:
    std::string messageEvent;
};

class MessageBus
{
public:
    MessageBus() {};
    ~MessageBus() {};

    void addReceiver(std::function<void(Message)> messageReceiver)
    {
        receivers.push_back(messageReceiver);
    }

    void sendMessage(Message message)
    {
        messages.push(message);
    }

    void notify()
    {
        while (!messages.empty()) {
            for (auto iter = receivers.begin(); iter != receivers.end(); iter++) {
                (*iter)(messages.front());
            }

            messages.pop();
        }
    }

private:
    std::vector<std::function<void(Message)>> receivers;
    std::queue<Message> messages;
};

class BusNode
{
public:
    BusNode(MessageBus* messageBus)
    {
        this->messageBus = messageBus;
        this->messageBus->addReceiver(this->getNotifyFunc());
    }

    virtual void update() {}
protected:
    MessageBus* messageBus;

    std::function<void(Message)> getNotifyFunc()
    {
        auto messageListener = [=](Message message) -> void {
            this->onNotify(message);
        };
        return messageListener;
    }

    void send(Message message)
    {
        messageBus->sendMessage(message);
    }

    virtual void onNotify(Message message) { }
};



#endif

