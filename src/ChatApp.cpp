/*
 * ChatApp2.cpp
 *
 *  Created on: 8 Nov 2018
 *      Author: rkfg
 */

#include "ChatApp.h"
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WTextArea.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WServer.h>
#include <Wt/WEnvironment.h>
#include <limits.h>
#include "config.h"
#include "utils.h"

using namespace Wt;
using namespace std;

ChatApp::ChatApp(const WEnvironment& env, State& state) :
        WApplication(env), m_state(state), m_env(env) {
    m_state.addApp(sessionId(), this);
    enableUpdates(true);
    useStyleSheet("/css/normalize.css");
    useStyleSheet("/css/style.css");
    useStyleSheet("/resources/font-awesome/css/font-awesome.min.css");
    messageResourceBundle().use("templates/messages");
    auto theme = make_shared<WBootstrapTheme>();
    theme->setVersion(BootstrapVersion::v3);
    setTheme(theme);
    auto cont = root()->addNew<WContainerWidget>();
    root()->setStyleClass("container");
    cont->setHeight("100%");
    root()->setHeight("100%");
    auto layout = cont->setLayout(make_unique<Wt::WVBoxLayout>());
    m_title = layout->addWidget(make_unique<WLabel>());
    cont->setContentAlignment(AlignmentFlag::Center);
    m_ta_chat = layout->addWidget(make_unique<ChatWidget>(), 1);
    m_ta_chat->setWidth("100%");
    loadLines();

    auto msg_cont = layout->addWidget(make_unique<WContainerWidget>())->setLayout(make_unique<Wt::WHBoxLayout>());
    m_tb_msg = msg_cont->addWidget(make_unique<WLineEdit>());
    m_tb_msg->setMaxLength(MAX_MSG_LENGTH);
    m_b_send = msg_cont->addWidget(std::make_unique<Wt::WPushButton>(WString::tr("send")));
    m_b_send->setWidth("100px");
    if (!env.javaScript()) {
        auto b_refresh = msg_cont->addWidget(std::make_unique<Wt::WPushButton>(WString::tr("reload")));
        b_refresh->setStyleClass("fa fa-refresh");
        b_refresh->clicked().connect(this, &WApplication::refresh);
    }
    m_tb_name = msg_cont->addWidget(make_unique<WLineEdit>());
    m_tb_name->setPlaceholderText(WString::tr("anonymous"));
    m_tb_name->setWidth("10%");
    m_tb_name->setMaxLength(MAX_NAME_LENGTH);
    auto ni = env.cookies().find("name");
    if (ni != env.cookies().end()) {
        m_name = ni->second;
        m_tb_name->setText(m_name);
    }
    m_tb_msg->setFocus();
    updateTitle();
    bindSignals();
}

void ChatApp::sendMessage() {
    auto text = m_tb_msg->text();
    if (!environment().javaScript()) {
        updateName();
    }
    if (text.empty()) {
        return;
    }
    auto text_len = text.value().length();
    if (text_len > MAX_MSG_LENGTH) {
        m_ta_chat->addLine(WString::trn("msg_too_long", text_len).arg(text_len).arg(MAX_MSG_LENGTH));
        return;
    }
    auto rl = ratelimit();
    if (rl) {
        m_ta_chat->addLine(WString::trn("ratelimited", rl).arg(rl));
        return;
    }
    if (!m_name.empty()) {
        auto name_len = m_name.value().length();
        if (name_len > MAX_NAME_LENGTH) {
            m_ta_chat->addLine(WString::tr("name_too_long").arg(name_len).arg(MAX_NAME_LENGTH));
            return;
        }
    }
    m_tb_msg->setText("");
    m_state.addLine().emit(m_name, text);
}

void ChatApp::updateTitle() {
    auto users_cnt = m_state.getUsersCount();
    auto title = WString::trn("title", users_cnt).arg(users_cnt);
    setTitle(title);
    m_title->setText(title);
    triggerUpdate();
    m_env.server()->schedule(60s, sessionId(), [=] {
        updateTitle();
    });
}

void ChatApp::bindSignals() {
    m_tb_name->blurred().connect(this, &ChatApp::updateName);
    m_tb_msg->enterPressed().connect([=] {
        m_b_send->clicked().emit(Wt::WMouseEvent());
    });
    m_b_send->clicked().connect(this, &ChatApp::sendMessage);
    m_state.lineAdded().connect([=](Message line) {
        /* we defer adding the line to the event loop because this signal is probably emitted
         * from another thread/context (by another user)
         */
        m_env.server()->post(sessionId(), [=] {
                    m_ta_chat->addLine(line);
                    triggerUpdate();
                });
    });
    m_ta_chat->nameClicked().connect([=](Message msg) {
        m_tb_msg->setText(">>" + msg.name + " " + m_tb_msg->text());
        m_tb_msg->setFocus();
    });
}

ChatApp::~ChatApp() {
    m_state.removeApp(sessionId());
}

unsigned int ChatApp::ratelimit() {
    auto now = time(NULL);
    if (m_timestamps.size() == MAX_MSG_COUNT) {
        auto oldest = m_timestamps.front();
        if (now - oldest < MAX_MSG_PERIOD) {
            return MAX_MSG_PERIOD - (now - oldest);
        }
        m_timestamps.pop_front();
    }
    m_timestamps.push_back(now);
    return 0;
}

void ChatApp::updateName() {
    m_name = m_tb_name->text();
    if (m_name.empty()) {
        removeCookie("name");
    } else {
        setCookie("name", m_name.toUTF8(), INT_MAX);
    }
}

void ChatApp::loadLines() {
    for (auto& line : m_state.getLines()) {
        m_ta_chat->addLine(line);
    }
}
