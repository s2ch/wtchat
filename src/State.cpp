/*
 * State.cpp
 *
 *  Created on: 9 Nov 2018
 *      Author: rkfg
 */

#include "State.h"
#include <Wt/WServer.h>
#include <boost/format.hpp>

void State::addApp(const std::string& id, ChatApp* app) {
    m_apps.emplace(id, app);
}

void State::removeApp(const std::string& id) {
    m_apps.erase(id);
}

void State::broadcast(const std::string& skipId, std::function<void(ChatApp* app)> f) {
    for (auto& session : m_srv.sessions()) {
        auto id = session.sessionId;
        if (id != skipId) {
            auto app = m_apps.find(session.sessionId);
            if (app != m_apps.end() && app->second) {
                auto target = app->second;
                m_srv.post(id, [=] {f(target);});
            }
        }
    }
}

std::string State::getPostNumber() {
    std::lock_guard<std::mutex> lck(m_mutex);
    auto s = boost::format("%05d") % m_cnt++;
    return s.str();
}

State::State(Wt::WServer& srv) :
        m_srv(srv) {
}
