#ifndef __AUDIOENGINE__H__
#define __AUDIOENGINE__H__
#include "datastruct.h"
#include <string>
#include <map>
#include <vector>
#include <al.h>

class AudioEngine
{
public:
	AudioEngine();
	~AudioEngine();
	static void create();
	static AudioEngine* getTheInstance();
	void preloadMusic(std::string filename);
	ALuint getBuffer(std::string filename);
	int playMusic(std::string filename,float volume,bool loop);
	void playMusic(music_Lite lite);
	void stopMusic(int id);
	void setVolume(int id,float volume);
	void setLoop(int id, bool loop);
	void cleanAll();
private:
	int id;
	std::vector<std::pair<std::string, ALuint> > name2buf;
	std::vector<std::pair<int, ALuint> > id2source;
	static AudioEngine* theInstance;
};
#endif