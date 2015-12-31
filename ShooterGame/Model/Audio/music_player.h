
#pragma once

class MusicPlayer
{
public:
	MusicPlayer();
	void play(const std::string &filename, bool looping = false);
	void play(std::vector<std::string> playlist, bool looping = false);
	void stop();
	void update();

private:
	void shuffle();

	std::vector<std::string> playlist;
	size_t current_song_index;
	bool loop;
	float volume = 0.5f;

	SoundBuffer current_song;
	SoundBuffer_Session current_session;
};
