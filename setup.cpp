#include "pch.h"
#include "Setup.h"
#include "ConfigManager.h"

#define NEWEST_CONFIG_URL "https://raw.githubusercontent.com/0xleft/rl_configs/master/settings.json";

BAKKESMOD_PLUGIN(Setup, "Setup", plugin_version, PERMISSION_ALL)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void Setup::onLoad()
{
	_globalCvarManager = cvarManager;
	this->configManager = new ConfigManager();

	cvarManager->registerCvar("selected_player", "", "name of the player selected", false, false, 0, false, 0, true);

	cvarManager->registerNotifier("setup_set_config", [this](std::vector<std::string> params) {
		if (params.size() > 1) {
			PlayerConfig config = this->configManager->getPlayerConfig(params[1]);
			if (config.fov == "not found") {
				LOG("Config not found for {}", params[1]);
				return;
			}

			cvarManager->getCvar("selected_player").setValue(config.name);

			handleSettingConfig();
			return;
		}

		handleSettingConfig();
		}, "set config of player name", PERMISSION_ALL);


	cvarManager->registerNotifier("setup_update_configs", [this](std::vector<std::string> params) {
		std::filesystem::path path = gameWrapper->GetDataFolder();
		path = path / "setup_settings.json";

		CurlRequest req;
		req.url = NEWEST_CONFIG_URL;

		HttpWrapper::SendCurlRequest(req, path.wstring(), [this](int code, std::wstring out_path) {
			LOG("Downloaded with code: {}", code);
			});

		}, "update settings.json", PERMISSION_ALL);

	cvarManager->registerNotifier("setup_reload_configs", [this](std::vector<std::string> params) {
		this->configManager->load(gameWrapper->GetDataFolder());
		LOG("Loaded {} configs", this->configManager->getPlayerConfigs().size());
		}, "reload configs", PERMISSION_ALL);

	this->configManager->load(gameWrapper->GetDataFolder());
	if (this->configManager->getPlayerConfigs().size() == 0) {
		cvarManager->executeCommand("setup_update_configs");
	}

	// Function TAGame.Camera_TA.ApplyCameraModifiers is funny
	gameWrapper->HookEvent("Function TAGame.Camera_TA.GetCameraSettings", [this](std::string eventname) {
		cvarManager->executeCommand("setup_set_config", false);
	});
}

void Setup::onUnload() {
	// remove setup_settings.json from data folder
}

void Setup::handleSettingConfig() {
	PlayerConfig playerConfig = this->getSelectedConfig(cvarManager->getCvar("selected_player").getStringValue());
	if (!playerConfig.isFound) {
		// LOG("Player not found");
		return;
	}

	ProfileCameraSettings settings;
	try {
		settings.FOV = std::stoi(playerConfig.fov);
		settings.Distance = std::stoi(playerConfig.distance);
		settings.Height = std::stoi(playerConfig.height);
		settings.Pitch = std::stof(playerConfig.angle);
		settings.Stiffness = std::stof(playerConfig.stiffness);
		settings.SwivelSpeed = std::stof(playerConfig.swivel);
		settings.TransitionSpeed = std::stof(playerConfig.transition);
	}
	catch (std::exception e) {
		LOG("Exception: {}", e.what());
	}

	if (!gameWrapper->GetCamera()) {
		LOG("camera is null");
		return;
	};

	gameWrapper->GetCamera().SetCameraSettings(settings);
}

void Setup::RenderSettings() {
	if (ImGui::Button("Update configs")) {
		gameWrapper->Execute([this](GameWrapper* gw) {
			cvarManager->executeCommand("setup_update_configs");
			});
	}

	if (ImGui::Button("Reset settings")) {
		gameWrapper->Execute([this](GameWrapper* gw) {
			cvarManager->getCvar("selected_player").setValue("");
			});
	}

	if (ImGui::Button("Reload configs")) {
		gameWrapper->Execute([this](GameWrapper* gw) {
			cvarManager->executeCommand("setup_reload_configs");
			});
	}

	ImGui::Text(std::format("Currently selected: {}", cvarManager->getCvar("selected_player").getStringValue()).c_str());

	std::vector<PlayerConfig> displayedConfigs;
	static char buf[128];
	ImGui::InputText("Player name", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue);
	if (buf == "") {
		displayedConfigs = this->configManager->getPlayerConfigs();
	}
	else {
		// hanlding where we are actualy searching
		for (PlayerConfig config : this->configManager->getPlayerConfigs()) {
			if (config.name.rfind(buf, 0) == 0) {
				displayedConfigs.push_back(config);
			}
		}
	}

	float availWidth = ImGui::GetContentRegionAvail().x;

	ImGui::BeginChild("Select player", ImVec2(availWidth / 2, 0));
	for (PlayerConfig item : displayedConfigs) {
		if (ImGui::Selectable(item.name.c_str())) {
			cvarManager->getCvar("selected_player").setValue(item.name);

			gameWrapper->Execute([this](GameWrapper* gw) {
				cvarManager->executeCommand("setup_set_config");
				});
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Config:");
	if (cvarManager->getCvar("selected_player").getStringValue() != "") {
		PlayerConfig selected = this->configManager->getPlayerConfig(cvarManager->getCvar("selected_player").getStringValue());
		if (selected.fov != "not found") {
			ImGui::Text(std::format("FOV: {}", selected.fov).c_str());
			ImGui::Text(std::format("Distance: {}", selected.distance).c_str());
			ImGui::Text(std::format("Height: {}", selected.height).c_str());
			ImGui::Text(std::format("Angle: {}", selected.angle).c_str());
			ImGui::Text(std::format("Stiffness: {}", selected.stiffness).c_str());
			ImGui::Text(std::format("Swivel speed: {}", selected.swivel).c_str());
			ImGui::Text(std::format("Transition speed: {}", selected.transition).c_str());
		}
	}
	ImGui::EndChild();
}