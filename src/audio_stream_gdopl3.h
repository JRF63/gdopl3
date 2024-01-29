#ifndef AUDIO_STREAM_GDOPL3_H
#define AUDIO_STREAM_GDOPL3_H

#include <player.h>

#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback_resampled.hpp>

#include <memory>

namespace godot {

// Forward declaration
class AudioStreamGDOPL3Playback;

class AudioStreamGDOPL3 : public AudioStream {
	GDCLASS(AudioStreamGDOPL3, AudioStream)

	friend class AudioStreamGDOPL3Playback;

	std::unique_ptr<OPLPlayer> midi_player;
	String filename;
	bool loop = false;

	void emit_looping_signal();

protected:
	static void _bind_methods();

public:
	static Ref<AudioStreamGDOPL3> load_from_buffer(const PackedByteArray &buffer, const PackedByteArray &bank_buffer);

	static Ref<AudioStreamGDOPL3> load_from_file(const String &path, const String &bank_path);

	String get_filename() const;

	void set_loop(bool enable);

	bool get_loop() const;

	// Overrides

	virtual Ref<AudioStreamPlayback> _instantiate_playback() const override;

	virtual String _get_stream_name() const override;

	virtual double _get_length() const override;

	virtual bool _is_monophonic() const override;

	virtual double _get_bpm() const override;

	virtual int32_t _get_beat_count() const override;

	AudioStreamGDOPL3();
};

class AudioStreamGDOPL3Playback : public AudioStreamPlaybackResampled {
	GDCLASS(AudioStreamGDOPL3Playback, AudioStreamPlaybackResampled);

	friend class AudioStreamGDOPL3;

	Ref<AudioStreamGDOPL3> stream;
	bool active = false;
	int32_t loops = 0;

protected:
	static void _bind_methods();

public:
	// Overrides

	virtual void _start(double from_pos) override;

	virtual void _stop() override;

	virtual bool _is_playing() const override;

	virtual int32_t _get_loop_count() const override;

	virtual double _get_playback_position() const override;

	virtual void _seek(double position) override;

	virtual int32_t _mix_resampled(AudioFrame *dst_buffer, int32_t frame_count) override;

	virtual double _get_stream_sampling_rate() const override;

	AudioStreamGDOPL3Playback();
};

} // namespace godot

#endif
