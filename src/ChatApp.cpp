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
#include <boost/algorithm/string.hpp>
#include <limits.h>

using namespace Wt;
using namespace std;

void doJs(WWidget* widget, const string& jsText) {
    widget->doJavaScript(boost::replace_all_copy(jsText, "@", widget->jsRef()));
}

void addText(ChatApp* target, const WEnvironment& env, const WString& text) {
    auto e = target->m_ta_chat->jsRef();
    doJs(target->m_ta_chat, "@.value += " + WWebWidget::jsStringLiteral(text) + " + '\\n'; @.scrollTop=@.scrollHeight");
}

ChatApp::ChatApp(const WEnvironment& env, WServer& srv, State& state) :
        WApplication(env), m_state(state) {
    enableUpdates(true);
    useStyleSheet(appRoot() + "css/style.css");
    auto theme = make_shared<WBootstrapTheme>();
    theme->setVersion(BootstrapVersion::v3);
    setTheme(theme);
    auto cont = root()->addNew<WContainerWidget>();
    root()->setStyleClass("container");
    cont->setHeight("100%");
    root()->setHeight("100%");
    auto layout = cont->setLayout(make_unique<Wt::WVBoxLayout>());
    layout->addWidget(make_unique<WLabel>("Чяти"));
    cont->setContentAlignment(AlignmentFlag::Center);
    m_ta_chat = layout->addWidget(make_unique<WTextArea>(), 1);
    m_ta_chat->setWidth("100%");
    m_ta_chat->setReadOnly(true);

    auto msg_cont = layout->addWidget(make_unique<WContainerWidget>())->setLayout(make_unique<Wt::WHBoxLayout>());
    auto tb_msg = msg_cont->addWidget(make_unique<WLineEdit>());
    auto tb_name = msg_cont->addWidget(make_unique<WLineEdit>());
    tb_name->setPlaceholderText("Аноним");
    tb_name->setWidth("10%");
    auto ni = env.cookies().find("name");
    if (ni != env.cookies().end()) {
        m_name = ni->second;
        tb_name->setText(m_name);
    }

    tb_name->blurred().connect([=] {
        m_name = tb_name->text();
        if (m_name.empty()) {
            removeCookie("name");
        } else {
            setCookie("name", m_name.toUTF8(), INT_MAX);
        }
    });

    auto b_send = layout->addWidget(std::make_unique<Wt::WPushButton>("Послать"));
    b_send->setWidth("100px");
    state.addApp(sessionId(), this);
    tb_msg->setFocus();
    tb_msg->enterPressed().connect([=] {
        b_send->clicked().emit(Wt::WMouseEvent());
    });
    b_send->clicked().connect([&, this, tb_msg]() {
        if (tb_msg->text().empty()) {
            return;
        }
        auto text = "<" + getName() + "> " + tb_msg->text();
        tb_msg->setText("");
        addText(this, env, text);
        state.broadcast(sessionId(), [&, text](ChatApp* target) {
                    addText(target, env, text);
                    target->triggerUpdate();
                });
    });
}

WString ChatApp::getName() {
    if (m_name.empty()) {
        return m_state.getPostNumber();
    }
    return m_name;
}

ChatApp::~ChatApp() {
    m_state.removeApp(sessionId());
}

