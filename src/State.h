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
#include <random>
#include <Wt/WString.h>
#include "ChatApp.h"

class ChatApp;

using AppMap = std::unordered_map<std::string, ChatApp*>;

struct Message {
    bool anon;
    Wt::WString name;
    Wt::WString message;
    Wt::WString formatted;
};

class State: public Wt::WObject {
private:
    AppMap m_apps;
    std::recursive_mutex m_mutex;
    Wt::WServer& m_srv;
    unsigned long m_cnt = 1;
    std::list<Message> m_lines;
    std::random_device rd;
    Wt::Signal<Message> m_line_signal;
    Wt::Signal<Wt::WString, const Wt::WString> m_add_line_signal;
    void doAddLine(Wt::WString name, const Wt::WString line);
public:
    void addApp(const std::string& id, ChatApp* app);
    void removeApp(const std::string& id);
    std::string getPostNumber();
    State(Wt::WServer& srv);
    std::list<Message> getLines();
    unsigned int getUsersCount();
    Wt::Signal<Message>& lineAdded();
    Wt::Signal<Wt::WString, const Wt::WString>& addLine();
};

#endif /* SRC_STATE_H_ */
