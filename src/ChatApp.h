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
    State& m_state;
    Wt::WString m_name;
    friend void addText(ChatApp* target, const Wt::WEnvironment& env, const Wt::WString& text);
    Wt::WString getName();
public:
    ChatApp(const Wt::WEnvironment& env, Wt::WServer& srv, State& state);
    ~ChatApp();
};

#endif /* SRC_CHATAPP_H_ */
