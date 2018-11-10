/*
 * ChatApp2.h
 *
 *  Created on: 8 Nov 2018
 *      Author: rkfg
 */

#ifndef SRC_CHATAPP_H_
#define SRC_CHATAPP_H_

#include <Wt/WApplication.h>
#include <unordered_map>
#include <mutex>
#include "State.h"

class State;

class ChatApp: public Wt::WApplication {
private:
    Wt::WTextArea* m_ta_chat;
    Wt::WLineEdit* m_tb_msg;
    Wt::WLineEdit* m_tb_name;
    State& m_state;
    Wt::WString m_name;
    std::list<uint64_t> m_timestamps;
    void addText(ChatApp* target, const Wt::WString& text);
    Wt::WString getName();
    unsigned int ratelimit();
    void sendMessage();
    void updateName();
public:
    ChatApp(const Wt::WEnvironment& env, Wt::WServer& srv, State& state);
    ~ChatApp();
};

#endif /* SRC_CHATAPP_H_ */
