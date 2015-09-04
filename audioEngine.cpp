#include "stdafx.h"
#include "audioEngine.h"
#include "Framework.h"
#include "socketHandler.h"
AudioEngine* AudioEngine::theInstance = NULL;

AudioEngine::AudioEngine()
{
	ALFWInit();
	if (!ALFWInitOpenAL())
	{
		throw 0;
		ALFWprintf("Failed to initialize OpenAL\n");
		ALFWShutdown();
		return;
	}
}

AudioEngine::~AudioEngine()
{

}

void AudioEngine::create()
{
	if (!theInstance)
	    theInstance = new AudioEngine();
}

AudioEngine* AudioEngine::getTheInstance()
{
	return theInstance;
}

void AudioEngine::preloadMusic(std::string filename)
{
	size_t size = name2buf.size();
	for (int i = 0; i < size; i++)
	{
		if (name2buf[i].first == filename)
		{
			return;
		}
	}
	ALuint buffer;
	alGenBuffers(1, &buffer);
	if (!ALFWLoadWaveToBuffer(filename.c_str(), buffer))
	{
		throw 0;
		ALFWprintf("Failed to load %s\n", ALFWaddMediaPath(filename.c_str()));
	}
	name2buf.push_back(std::pair<std::string, ALuint>(filename,buffer));
}

ALuint AudioEngine::getBuffer(std::string filename)
{
	size_t size = name2buf.size();
	for (int i = 0; i < size; i++)
	{
		if (name2buf[i].first == filename)
		{
			return name2buf[i].second;
		}
	}
	preloadMusic(filename);
	return getBuffer(filename);
}

void AudioEngine::playMusic(music_Lite lite)
{
	ALuint source;
	alGenSources(1, &source);
	alSourcei(source, AL_BUFFER,lite.buf );   //绑定buffer
	alSourcef(source, AL_GAIN, lite.volume);       //设定音量
	if (lite.loop)
		alSourcei(source, AL_LOOPING, AL_TRUE);
	else
		alSourcei(source, AL_LOOPING, AL_FALSE);
	alSourcePlay(source);
	id2source.push_back(std::pair<int, ALuint>(lite.id, source));
}

int AudioEngine::playMusic(std::string filename, float volume, bool loop)
{
	ALuint source;
	alGenSources(1, &source);
	ALuint buffer = getBuffer(filename);
	alSourcei(source, AL_BUFFER, buffer);   //绑定buffer
	alSourcef(source,AL_GAIN,volume);       //设定音量
	if (loop)
		alSourcei(source, AL_LOOPING, AL_TRUE);
	else
		alSourcei(source, AL_LOOPING, AL_FALSE);
	alSourcePlay(source);
	id++;
	id2source.push_back(std::pair<int, ALuint>(id,source));
	if (socketHandler::getTheInstance()->getNetworkMode() == SERVER)
	{
		music_Lite lite;
		lite.buf = socketHandler::getTheInstance()->name2Sound[filename];
		lite.id = id;
		lite.loop = loop;
		lite.volume = volume;
		socketHandler::getTheInstance()->sendMsg(MUSIC, &lite, sizeof(lite));
	}
	return id;
}

void AudioEngine::stopMusic(int id)
{
	std::vector<std::pair<int, ALuint> >::iterator end = id2source.end();
	for (std::vector<std::pair<int, ALuint> >::iterator it = id2source.begin(); it != end;++it)
	{
		if ((*it).first == id )
		{
			alDeleteSources(1, &((*it).second));
			id2source.erase(it);
			return;
		}
	}
}

void AudioEngine::setVolume(int id, float volume)
{
	std::vector<std::pair<int, ALuint> >::iterator end = id2source.end();
	for (std::vector<std::pair<int, ALuint> >::iterator it = id2source.begin(); it != end;++it)
	{
		if ((*it).first == id)
		{
			alSourcei((*it).second, AL_GAIN, volume);
			return;
		}
	}
}

void AudioEngine::setLoop(int id, bool loop)
{
	std::vector<std::pair<int, ALuint> >::iterator end = id2source.end();
	for (std::vector<std::pair<int, ALuint> >::iterator it = id2source.begin(); it != end;++it)
	{
		if ((*it).first == id)
		{
			if (loop)
				alSourcei((*it).second, AL_LOOPING, AL_TRUE);
			else
				alSourcei((*it).second, AL_LOOPING, AL_FALSE);
			return;
		}
	}
}

void AudioEngine::cleanAll()
{
	std::vector<std::pair<int, ALuint> >::iterator end = id2source.end();
	for (std::vector<std::pair<int, ALuint> >::iterator it = id2source.begin(); it != end;)
	{
		alDeleteSources(1, &((*it).second));
		it=id2source.erase(it);
	}
}