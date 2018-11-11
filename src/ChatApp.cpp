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

ChatApp::ChatApp(const WEnvironment& env, WServer& srv, State& state) :
        WApplication(env), m_state(state) {
    m_state.addApp(sessionId(), this);
    enableUpdates(true);
    useStyleSheet("/css/style.css");
    messageResourceBundle().use("templates/messages");
    auto theme = make_shared<WBootstrapTheme>();
    theme->setVersion(BootstrapVersion::v3);
    setTheme(theme);
    setTitle(WString::tr("title"));
    auto cont = root()->addNew<WContainerWidget>();
    root()->setStyleClass("container");
    cont->setHeight("100%");
    root()->setHeight("100%");
    auto layout = cont->setLayout(make_unique<Wt::WVBoxLayout>());
    layout->addWidget(make_unique<WLabel>(WString::tr("title")));
    cont->setContentAlignment(AlignmentFlag::Center);
    m_ta_chat = layout->addWidget(make_unique<ChatWidget>(), 1);
    m_ta_chat->setWidth("100%");
    loadLines();

    auto msg_cont = layout->addWidget(make_unique<WContainerWidget>())->setLayout(make_unique<Wt::WHBoxLayout>());
    m_tb_msg = msg_cont->addWidget(make_unique<WLineEdit>());
    m_tb_msg->setMaxLength(MAX_MSG_LENGTH);
    m_tb_name = msg_cont->addWidget(make_unique<WLineEdit>());
    m_tb_name->setPlaceholderText(WString::tr("anonymous"));
    m_tb_name->setWidth("10%");
    m_tb_name->setMaxLength(MAX_NAME_LENGTH);
    auto ni = env.cookies().find("name");
    if (ni != env.cookies().end()) {
        m_name = ni->second;
        m_tb_name->setText(m_name);
    }

    m_tb_name->blurred().connect(this, &ChatApp::updateName);

    auto b_send = layout->addWidget(std::make_unique<Wt::WPushButton>(WString::tr("send")));
    b_send->setWidth("100px");
    m_tb_msg->setFocus();
    m_tb_msg->enterPressed().connect([=] {
        b_send->clicked().emit(Wt::WMouseEvent());
    });
    b_send->clicked().connect(this, &ChatApp::sendMessage);
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
    auto msg = m_state.addLine(m_name, text);
    m_ta_chat->addLine(msg);
    m_state.broadcast(sessionId(), [&, msg](ChatApp* target) {
        target->m_ta_chat->addLine(msg);
        target->triggerUpdate();
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
