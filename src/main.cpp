// Simple Hello World program
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include <Wt/WLabel.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WFitLayout.h>
#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WSignal.h>
#include <utility>
#include <unordered_map>

class ChatApp;

using AppMap = std::unordered_map<std::string, ChatApp*>;

class ChatApp: public Wt::WApplication {
private:
    Wt::WTextArea* m_ta_chat;
    AppMap& m_apps;
public:
    ChatApp(const Wt::WEnvironment& env, Wt::WServer& srv, AppMap& apps) :
            WApplication(env), m_apps(apps) {
        enableUpdates(true);
	useStyleSheet(appRoot() + "css/style.css");
	auto theme = std::make_shared<Wt::WBootstrapTheme>();
	theme->setVersion(Wt::BootstrapVersion::v3);
        setTheme(theme);
        auto cont = root()->addNew<Wt::WContainerWidget>();
        root()->setStyleClass("container");
        cont->setHeight("100%");
        root()->setHeight("100%");
        auto layout = cont->setLayout(std::make_unique<Wt::WVBoxLayout>());
        layout->addWidget(std::make_unique<Wt::WLabel>("Чяти"));
        cont->setContentAlignment(Wt::AlignmentFlag::Center);
        m_ta_chat = layout->addWidget(std::make_unique<Wt::WTextArea>(), 1);
        m_ta_chat->setWidth("100%");
        m_ta_chat->setReadOnly(true);
        auto tb_msg = layout->addWidget(std::make_unique<Wt::WLineEdit>());
        tb_msg->setWidth("100%");
        auto b_send = layout->addWidget(std::make_unique<Wt::WPushButton>("Послать"));
        b_send->setWidth("100px");
        apps.emplace(sessionId(), this);
        tb_msg->enterPressed().connect([=] {
            b_send->clicked().emit(Wt::WMouseEvent());
        });
        b_send->clicked().connect(
                [this, tb_msg, &srv, &apps]() {
                    auto text = tb_msg->text();
                    for (auto& session: srv.sessions()) {
                        srv.post(session.sessionId, [&, text, session] {
                                    auto target = apps.find(session.sessionId)->second;
                                    if (target) {
                                        //target->m_ta_chat->setText(target->m_ta_chat->text() + "<" + sessionId() + "> " + text + "\n");
                                        target->m_ta_chat->setText(target->m_ta_chat->text() + text + "\n");
                                        auto elem = target->m_ta_chat->jsRef();
                                        target->m_ta_chat->doJavaScript(elem + ".scrollTop=" + elem + ".scrollHeight");
                                        target->triggerUpdate();
                                    }
                                });
                    }
                    tb_msg->setText("");
                });
    }

    ~ChatApp() {
        m_apps.erase(sessionId());
    }
};

int main(int argc, char **argv) {
    AppMap apps;
    Wt::WServer server(argc, argv);
    server.addEntryPoint(Wt::EntryPointType::Application, [&](const Wt::WEnvironment& env) {
        return std::make_unique<ChatApp>(env, server, apps);
    });
    if (server.start()) {
        int sig = Wt::WServer::waitForShutdown();
        Wt::log("warn") << "Shutdown (signal = " << sig << ")";
        server.stop();
        if (sig == SIGHUP)
            Wt::WServer::restart(argc, argv, environ);
    }
    return 0;
}
