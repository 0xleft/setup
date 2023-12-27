#pragma once
#include <fstream>
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct PlayerConfig {
	std::string name;
	std::string cameraShake;
	std::string fov;
	std::string height;
	std::string angle;
	std::string distance;
	std::string stiffness;
	std::string swivel;
	std::string transition;
	std::string ballCam;
	std::string lastUpdated;
};

class ConfigManager
{
private:
	std::vector<PlayerConfig> playerConfigs;

public:
	std::vector<PlayerConfig> getPlayerConfigs() {
		return this->playerConfigs;
	}

	int load(std::filesystem::path path) {
		this->playerConfigs.clear();

		path = path / "setup_settings.json";

		if (!std::filesystem::exists(path)) {
			return 1;
		}

		try {
			std::ifstream f(path);
			json data = json::parse(f, nullptr, true);
			for (int i = 0; i < data.size(); i++) {
				auto obj = data.at(i);
				PlayerConfig config;
				config.name = obj["name"];
				config.cameraShake = obj["camera_shake"];
				config.fov = obj["fov"];
				config.height = obj["height"];
				config.angle = obj["angle"];
				config.distance = obj["distance"];
				config.stiffness = obj["stiffness"];
				config.swivel = obj["swivel"];
				config.transition = obj["transition"];
				config.ballCam = obj["ball_cam"];
				config.lastUpdated = obj["last_updated"];
				this->playerConfigs.push_back(config);
			}
		}

		catch (json::exception e) {
			LOG("Error loading config: {}", e.what());
			return 2;
		}
		catch (std::exception e) {
			LOG("Error loading config: {}", e.what());
			return 2;
		}
		return 0;
	}

	PlayerConfig getPlayerConfig(std::string name) {
		for (PlayerConfig playerConfig : this->playerConfigs) {
			if (playerConfig.name == name) {
				return playerConfig;
			}
		}

		PlayerConfig n;
		n.fov = "not found";
		return n;
	}
};

