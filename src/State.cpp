/*
 * State.cpp
 *
 *  Created on: 9 Nov 2018
 *      Author: rkfg
 */

#include "State.h"
#include "config.h"
#include <Wt/WServer.h>
#include <boost/format.hpp>

void State::addApp(const std::string& id, ChatApp* app) {
    std::lock_guard<std::recursive_mutex> lck(m_mutex);
    m_apps.emplace(id, app);
}

void State::removeApp(const std::string& id) {
    std::lock_guard<std::recursive_mutex> lck(m_mutex);
    m_apps.erase(id);
}

std::string State::getPostNumber() {
    std::lock_guard<std::recursive_mutex> lck(m_mutex);
    auto s = boost::format("%05d") % m_cnt++;
    return s.str();
}

State::State(Wt::WServer& srv) :
        m_srv(srv) {
    m_add_line_signal.connect(this, &State::doAddLine);
}

void State::doAddLine(Wt::WString name, const Wt::WString line) {
    std::lock_guard<std::recursive_mutex> lck(m_mutex);
    bool anon = false;
    if (name.empty()) {
        anon = true;
        name = getPostNumber();
    }
    auto result = Wt::WString("<span class='{1}'>&lt;{2}&gt;</span> {3}").arg(anon ? "num" : "name").arg(
            Wt::WWebWidget::escapeText(name, true)).arg(Wt::WWebWidget::escapeText(line));
    m_lines.push_back( { anon, name, line, result });
    if (m_lines.size() > MAX_LINES) {
        m_lines.pop_front();
    }
    m_line_signal.emit(m_lines.back());
}

std::list<Message> State::getLines() {
    std::lock_guard<std::recursive_mutex> lck(m_mutex);
    return m_lines;
}

unsigned int State::getUsersCount() {
    std::uniform_int_distribution<int> r(-(std::max((size_t) 1, m_apps.size() / 5)),
            std::max((size_t) 1, m_apps.size() / 5));
    auto dev = r(rd);
    return m_apps.size() + dev;
}

Wt::Signal<Message>& State::lineAdded() {
    return m_line_signal;
}

Wt::Signal<Wt::WString, const Wt::WString>& State::addLine() {
    return m_add_line_signal;
}
