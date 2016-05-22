// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <string>

#include "common/common_types.h"

#include "core/hle/kernel/kernel.h"

namespace Kernel {

class ServerPort final : public WaitObject {
public:
    /**
     * Creates a server port.
     * @param name Optional name of the server port
     * @return The created server port
     */
    static ResultVal<SharedPtr<ServerPort>> Create(std::string name = "Unknown");

    std::string GetTypeName() const override { return "ServerPort"; }
    std::string GetName() const override { return name; }

    static const HandleType HANDLE_TYPE = HandleType::ServerPort;
    HandleType GetHandleType() const override { return HANDLE_TYPE; }

    std::string name;                           ///< Name of port (optional)

    std::vector<SharedPtr<WaitObject>> pending_sessions; ///< ServerSessions waiting to be accepted by the port

    bool ShouldWait() override;
    void Acquire() override;

private:
    ServerPort();
    ~ServerPort() override;
};

} // namespace
