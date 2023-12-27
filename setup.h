#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "ConfigManager.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH);

class Setup : public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase
{
	ConfigManager *configManager;

	void onLoad() override;
	void onUnload() override;
	void handleSettingConfig();

public:

	PlayerConfig getSelectedConfig(std::string selectedPlayer) {
		return this->configManager->getPlayerConfig(selectedPlayer);
	}

	void RenderSettings() override;
};
