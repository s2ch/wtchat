/*
 * State.h
 *
 *  Created on: 9 Nov 2018
 *      Author: rkfg
 */

#ifndef SRC_STATE_H_
#define SRC_STATE_H_

#include <mutex>
#include <functional>
#include "ChatApp.h"

class ChatApp;

using AppMap = std::unordered_map<std::string, ChatApp*>;

class State {
private:
    AppMap m_apps;
    std::mutex m_mutex;
    Wt::WServer& m_srv;
    unsigned long m_cnt = 1;
public:
    void addApp(const std::string& id, ChatApp* app);
    void removeApp(const std::string& id);
    void broadcast(const std::string& skipId, std::function<void(ChatApp* app)> f);
    std::string getPostNumber();
    State(Wt::WServer& srv);
};

#endif /* SRC_STATE_H_ */