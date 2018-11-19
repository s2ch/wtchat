/*
 * ChatLine.cpp
 *
 *  Created on: 14 Nov 2018
 *      Author: rkfg
 */

#include "ChatLine.h"
#include <Wt/WLabel.h>

using namespace std;
using namespace Wt;

static const std::regex ref_regex("(&gt;&gt;[0-9]+)");

ChatLine::ChatLine(const Message& msg) :
        m_msg(msg) {
    m_label = addWidget(make_unique<WLabel>("<" + escapeText(msg.name, true) + ">"));
    m_label->setStyleClass(msg.anon ? "num clickable" : "name");
    auto text = regex_replace(escapeText(msg.message, true).toUTF8(), ref_regex, "<span class=\"num\">$1</span> ");
    addWidget(make_unique<WLabel>(" " + text));
}

Wt::EventSignal<Wt::WMouseEvent>& ChatLine::clicked() {
    return m_label->clicked();
}
