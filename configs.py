import requests
from bs4 import BeautifulSoup



class Settings:
    def __init__(self, name, camera_shake, fov, height, angle, distance, stiffness, swivel, transition, ball_cam, last_updated):
        self.name = name
        self.camera_shake = camera_shake
        self.fov = fov
        self.height = height
        self.angle = angle
        self.distance = distance
        self.stiffness = stiffness
        self.swivel = swivel
        self.transition = transition
        self.ball_cam = ball_cam
        self.last_updated = last_updated

    def __str__(self):
        return f"{self.name} {self.fov} {self.height} {self.angle} {self.distance} {self.stiffness} {self.swivel} {self.transition} {self.ball_cam} {self.last_updated}"

def get_player_settings(url) -> list[Settings]:
    res = requests.get(url,headers={'User-Agent': 'Mozilla/5.0'})
    soup = BeautifulSoup(res.content, 'html.parser')

    table = soup.find_all('tbody')[1]
    rows = table.find_all('tr')

    settings = []

    for row in rows:
        cols = row.find_all('td')
        if len(cols) > 3:
            name = cols[0].text.strip()
            camera_shake = cols[2].text.strip()
            fov = cols[3].text.strip()
            height = cols[4].text.strip()
            angle = cols[5].text.strip()
            distance = cols[6].text.strip()
            stiffness = cols[7].text.strip()
            swivel = cols[8].text.strip()
            transition = cols[9].text.strip()
            ball_cam = cols[10].text.strip()
            last_updated = cols[11].text.strip()
            settings.append(Settings(name, camera_shake, fov, height, angle, distance, stiffness, swivel, transition, ball_cam, last_updated))

    return settings

s = get_player_settings("https://liquipedia.net/rocketleague/List_of_player_camera_settings")

import json


with open('settings.json', 'w+') as outfile:
    json.dump([ob.__dict__ for ob in s], outfile)