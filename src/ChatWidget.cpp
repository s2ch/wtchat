/*
 * ChatWidget.cpp
 *
 *  Created on: 11 Nov 2018
 *      Author: rkfg
 */

#include "ChatWidget.h"
#include "config.h"
#include <Wt/WText.h>

ChatWidget::ChatWidget() {
    setStyleClass("chatwidget");
}

void ChatWidget::addLine(const Wt::WString& line) {
    auto w_line = addWidget(std::make_unique<Wt::WText>(line));
    w_line->setInline(false);
    if (count() > MAX_LINES) {
        removeWidget(widget(0));
    }
    doJs(this, "{1}.scrollTop={1}.scrollHeight");
}
