/*
 * ChatWidget.cpp
 *
 *  Created on: 11 Nov 2018
 *      Author: rkfg
 */

#include "ChatWidget.h"
#include "config.h"
#include <Wt/WText.h>
#include "ChatLine.h"

ChatWidget::ChatWidget() {
    setStyleClass("chatwidget");
}

void ChatWidget::addLine(const Message& msg) {
    auto line = addWidget(std::make_unique<ChatLine>(msg));
    if (msg.anon) {
        line->clicked().connect([=] {
            m_name_clicked(line->m_msg);
        });
    }
}
void ChatWidget::addLine(const Wt::WString& msg) {
    auto w_line = addWidget(std::make_unique<Wt::WText>(msg));
    w_line->setInline(false);
}

void ChatWidget::cleanup() {
    if (count() > MAX_LINES) {
        removeWidget(widget(0));
    }
    doJs(this, "{1}.scrollTop={1}.scrollHeight");
}

Wt::Signal<Message>& ChatWidget::nameClicked() {
    return m_name_clicked;
}
