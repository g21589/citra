// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "citra_qt/configure_system.h"
#include "citra_qt/ui_settings.h"
#include "ui_configure_system.h"

#include "core/hle/service/fs/archive.h"
#include "core/hle/service/cfg/cfg.h"

ConfigureSystem::ConfigureSystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigureSystem)
{
    ui->setupUi(this);

    // Limits the username length.
    // setMaxLength seems to count characters in char16_t,
    // which is what we need because we want to limit the name
    // to 20 bytes.
    ui->edit_username->setMaxLength(10);

    // Add days to the birthday combo box
    for (int i = 1; i <= 31; ++i) {
        ui->combo_birthday->addItem(QString::number(i));
    }
}

ConfigureSystem::~ConfigureSystem() {
}

void ConfigureSystem::setConfiguration(bool enabled) {
    this->enabled = enabled;

    if (!enabled) {
        ui->group_system_settings->setEnabled(false);
    }
    else {
        ui->group_system_settings->setEnabled(true);

        // Temporarily register archive types
        // and load the config savegame file to memory
        Service::FS::RegisterArchiveTypes();
        Service::CFG::LoadConfigNANDSaveFile();
        Service::FS::UnregisterArchiveTypes();

        // set username
        username = Service::CFG::GetUsername();
        ui->edit_username->setText(QString::fromStdU16String(username));

        // set birthday
        std::tie(birthmonth, birthday) = Service::CFG::GetBirthday();
        ui->combo_birthmonth->setCurrentIndex(birthmonth - 1);
        ui->combo_birthday->setCurrentIndex(birthday - 1);

        // set system language
        language_index = Service::CFG::GetSystemLanguage();
        ui->combo_language->setCurrentIndex(language_index);
    }
}

void ConfigureSystem::applyConfiguration() {
    if (!enabled)
        return;

    bool modified = false;

    // apply username
    std::u16string new_username = ui->edit_username->text().toStdU16String();
    if (new_username != username) {
        Service::CFG::SetUsername(new_username);
        modified = true;
    }

    // apply birthday
    int new_birthmonth = ui->combo_birthmonth->currentIndex() + 1;
    int new_birthday = ui->combo_birthday->currentIndex() + 1;
    if (birthmonth != new_birthmonth || birthday != new_birthday) {
        Service::CFG::SetBirthday(new_birthmonth, new_birthday);
        modified = true;
    }

    // apply language
    int new_language = ui->combo_language->currentIndex();
    if (language_index != new_language) {
        Service::CFG::SetSystemLanguage(static_cast<Service::CFG::SystemLanguage>(new_language));
        modified = true;
    }

    // update the config save game if any item is modified.
    if (modified)
        Service::CFG::UpdateConfigNANDSavegame();
}