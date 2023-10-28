import os
import json
import shutil
import sys

# Constants for folders and files
ASSET_FOLDER = "./asset/Beatmaps/"  # Path to the asset folder where Beatmaps are stored
DATABASE_CONFIG_PATH = "./config/Database.cfg"  # Path to the Database.cfg file

# Function to add a beatmap
def add_beatmap(wav_path, cover_path=None):
    # Extract metadata
    beatmap_name = os.path.basename(wav_path).split('.')[0]
    beatmap_folder = os.path.join(ASSET_FOLDER, beatmap_name)

    # Create beatmap folder
    os.makedirs(beatmap_folder, exist_ok=True)

    # Copy .wav file
    shutil.copy(wav_path, os.path.join(beatmap_folder, "Song.wav"))

    # Copy cover image or use a default one
    if cover_path:
        shutil.copy(cover_path, os.path.join(beatmap_folder, "Cover.jpg"))
    else:
        shutil.copy("default_cover.jpg", os.path.join(beatmap_folder, "Cover.jpg"))

    # Create Config.json
    config_data = {
        "Name": beatmap_name,
        "Artist": "Unknown",
        "FolderPath": beatmap_name,
        "Difficulty": 42,
        "BPM": 42,
        "id": 0,  # Placeholder, manually edit this later
        "offset": 0
    }
    with open(os.path.join(beatmap_folder, "Config.json"), 'w') as f:
        json.dump(config_data, f, indent=4)

    # Update Database.cfg (append new line)
        next_id = 0
    with open(DATABASE_CONFIG_PATH, 'r') as f:
        content = f.read()
        next_id = content.count('id = ')  # Assuming each entry has a unique 'id = '

    # New entry to be added
    new_entry = f"""
        ,{{
        Name = "{beatmap_name}";
        Artist = "Unknown";
        FolderPath = "{beatmap_name}";
        Difficulty = 42;
        BPM = 42;
        id = {next_id};
        offset = 0;
        }}
    """

    # Insert new entry into existing content
    last_entry_index = content.rfind(");")
    updated_content = content[:last_entry_index] + new_entry + content[last_entry_index:]

    # Update Database.cfg
    with open(DATABASE_CONFIG_PATH, 'w') as f:
        f.write(updated_content)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python script_name.py /path/to/song.wav [optional:/path/to/cover.jpg]")
    else:
        wav_path = sys.argv[1]
        cover_path = sys.argv[2] if len(sys.argv) > 2 else None
        add_beatmap(wav_path, cover_path)
