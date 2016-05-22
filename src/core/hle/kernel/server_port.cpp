// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <tuple>

#include "common/assert.h"

#include "core/hle/kernel/client_port.h"
#include "core/hle/kernel/kernel.h"
#include "core/hle/kernel/server_port.h"
#include "core/hle/kernel/thread.h"

namespace Kernel {

ServerPort::ServerPort() {}
ServerPort::~ServerPort() {}

ResultVal<SharedPtr<ServerPort>> ServerPort::Create(std::string name) {
    SharedPtr<ServerPort> server_port(new ServerPort);

    server_port->name = std::move(name);

    return MakeResult<SharedPtr<ServerPort>>(std::move(server_port));
}

bool ServerPort::ShouldWait() {
    // If there are no pending sessions, we wait until a new one is added.
    return pending_sessions.size() == 0;
}

void ServerPort::Acquire() {
    ASSERT_MSG(!ShouldWait(), "object unavailable!");
}

std::tuple<SharedPtr<ServerPort>, SharedPtr<ClientPort>> ServerPort::CreatePortPair(u32 max_sessions, std::string name) {
    auto server_port = ServerPort::Create(name + "Server").MoveFrom();
    auto client_port = ClientPort::Create(server_port, max_sessions, name + "Client").MoveFrom();

    return std::make_tuple(server_port, client_port);
}

} // namespace
