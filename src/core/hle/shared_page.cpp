// Copyright 2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <cstring>
#include <ctime>
#include <core/core_timing.h>

#include "core/hle/shared_page.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// 3DS Uses 1900 for Epoch instead of 1970
#define _3DS_EPOCH_OFFSET 3155673600L

namespace SharedPage {

SharedPageDef shared_page;

static int update_time_event;   ///< Time is updated/swapped every hour

void Init() {
    std::memset(&shared_page, 0, sizeof(shared_page));

    shared_page.date_time_update_counter = 1;

    // Some games wait until this value becomes 0x1, before asking running_hw
    shared_page.unknown_value = 0x1;

    update_time_event = CoreTiming::RegisterEvent("SharedPage::update_time_event", UpdateTimeCallback);
    // update now
    CoreTiming::ScheduleEvent(0, update_time_event);
}

static void UpdateTimeCallback(u64 /*userdata*/, int /*cycles_late*/) {
    shared_page.date_time_update_counter++;

    // 3DS uses 1/1/1900 for Epoch
    time_t plat_time = std::time(nullptr);
    tm console_epoch = {};
    console_epoch.tm_year = 100; // 2000
    u64_le console_time = (static_cast<u64_le>(std::difftime(plat_time, std::mktime(&console_epoch))) + _3DS_EPOCH_OFFSET) * 1000L;

    DateTime* current_time = (shared_page.date_time_update_counter & 1) ? &shared_page.date_time_1 : &shared_page.date_time_0;

    current_time->date_time = console_time;
    current_time->tick_rate = (u64_le)CoreTiming::GetClockFrequency();
    current_time->update_tick = CoreTiming::GetTicks();

    // run again in an hour
    CoreTiming::ScheduleEvent((u64_le)CoreTiming::GetClockFrequency() * 360ULL, update_time_event);
}

} // namespace
