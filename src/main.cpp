// Simple Hello World program
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include <utility>
#include "ChatApp.h"
#include "State.h"

int main(int argc, char **argv) {
    try {
        Wt::WServer server(argc, argv);
        State state(server);
        server.addEntryPoint(Wt::EntryPointType::Application, [&](const Wt::WEnvironment& env) {
            return std::make_unique<ChatApp>(env, server, state);
        });
        if (server.start()) {
            int sig = Wt::WServer::waitForShutdown();
            Wt::log("warn") << "Shutdown (signal = " << sig << ")";
            server.stop();
            if (sig == SIGHUP)
                Wt::WServer::restart(argc, argv, environ);
        }
    } catch (const Wt::WServer::Exception& e) {
        return 1;
    }
    return 0;
}
