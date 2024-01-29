#include "audio_stream_gdopl3.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>

using namespace godot;

constexpr long SAMPLE_RATE = 44100;
const char *LOOPING_SIGNAL = "looped";

Ref<AudioStreamGDOPL3> AudioStreamGDOPL3::load_from_buffer(const PackedByteArray &buffer, const PackedByteArray &bank_buffer) {
	Ref<AudioStreamGDOPL3> stream;
	stream.instantiate();

	stream->midi_player = std::make_unique<OPLPlayer>();

	if (!stream->midi_player->loadSequence(buffer.ptr(), buffer.size())) {
		ERR_FAIL_V_EDMSG(nullptr, "loadSequence failed");
	}

	if (!stream->midi_player->loadPatches(bank_buffer.ptr(), bank_buffer.size())) {
		ERR_FAIL_V_EDMSG(nullptr, "loadPatches failed");
	}

	stream->midi_player->setSampleRate(SAMPLE_RATE);

	return stream;
}

Ref<AudioStreamGDOPL3> AudioStreamGDOPL3::load_from_file(const String &path, const String &bank_path) {
	auto buffer = FileAccess::get_file_as_bytes(path);
	ERR_FAIL_COND_V_EDMSG(
			buffer.is_empty(), nullptr, "Cannot open file '" + path + "'.");

	auto bank_buffer = FileAccess::get_file_as_bytes(bank_path);
	ERR_FAIL_COND_V_EDMSG(
			bank_buffer.is_empty(), nullptr, "Cannot open file '" + bank_path + "'.");

	auto stream = AudioStreamGDOPL3::load_from_buffer(buffer, bank_buffer);
	if (stream == nullptr) {
		return stream;
	}

	stream->filename = path;

	return stream;
}

String AudioStreamGDOPL3::get_filename() const {
	return filename;
}

void AudioStreamGDOPL3::set_loop(bool enable) {
	loop = enable;
}

bool AudioStreamGDOPL3::get_loop() const {
	return loop;
}

Ref<AudioStreamPlayback> AudioStreamGDOPL3::_instantiate_playback() const {
	ERR_FAIL_NULL_V(midi_player, nullptr);

	Ref<AudioStreamGDOPL3Playback> playback;
	playback.instantiate();

	playback->stream = Ref<AudioStreamGDOPL3>(this);
	playback->active = false;
	playback->loops = 0;

	return playback;
}

String AudioStreamGDOPL3::_get_stream_name() const {
	return "";
}

double AudioStreamGDOPL3::_get_length() const {
	// Unsupported
	return 0.0;
}

bool AudioStreamGDOPL3::_is_monophonic() const {
	// `AudioStreamGDOPL3` is stereo
	return false;
}

double AudioStreamGDOPL3::_get_bpm() const {
	// Unsupported
	return 0.0;
}

int32_t AudioStreamGDOPL3::_get_beat_count() const {
	// Unsupported
	return 0;
}

void AudioStreamGDOPL3::emit_looping_signal() {
	emit_signal(LOOPING_SIGNAL);
}

void AudioStreamGDOPL3::_bind_methods() {
	ClassDB::bind_static_method("AudioStreamGDOPL3",
			D_METHOD("load_from_buffer", "buffer", "bank_buffer"),
			&AudioStreamGDOPL3::load_from_buffer);
	ClassDB::bind_static_method("AudioStreamGDOPL3",
			D_METHOD("load_from_file", "path", "bank_path"),
			&AudioStreamGDOPL3::load_from_file);

	ClassDB::bind_method(D_METHOD("get_filename"), &AudioStreamGDOPL3::get_filename);

	ClassDB::bind_method(D_METHOD("set_loop", "enable"),
			&AudioStreamGDOPL3::set_loop);
	ClassDB::bind_method(D_METHOD("get_loop"), &AudioStreamGDOPL3::get_loop);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "loop"), "set_loop", "get_loop");

	ADD_SIGNAL(MethodInfo(LOOPING_SIGNAL));
}

AudioStreamGDOPL3::AudioStreamGDOPL3() {
}

////////////////

void AudioStreamGDOPL3Playback::_start(double from_pos) {
	active = true;
	_seek(from_pos);
}

void AudioStreamGDOPL3Playback::_stop() {
	active = false;
}

bool AudioStreamGDOPL3Playback::_is_playing() const {
	return active;
}

int32_t AudioStreamGDOPL3Playback::_get_loop_count() const {
	return loops;
}

double AudioStreamGDOPL3Playback::_get_playback_position() const {
	// Unsupported
	return 0.0;
}

void AudioStreamGDOPL3Playback::_seek(double position) {
	// Unsupported
}

int32_t AudioStreamGDOPL3Playback::_mix_resampled(AudioFrame *dst_buffer, int32_t frame_count) {
	static_assert(std::alignment_of<AudioFrame>::value ==
			std::alignment_of<float>::value);

	ERR_FAIL_NULL_V(stream, 0);
	ERR_FAIL_NULL_V(stream->midi_player, 0);

	int frames_rendered = 0;
	while ((frames_rendered < frame_count) && !(stream->midi_player->atEnd())) {
		auto ptr = reinterpret_cast<float *>(dst_buffer + frames_rendered);
		stream->midi_player->generate(ptr, 1);
		frames_rendered++;
	}

	if (stream->midi_player->atEnd() && stream->loop) {
		// Hack used to emulate a rewind
		stream->midi_player->setLoop(true);
		stream->loop = false; // Pessimistically guard against infinite recursion
		frames_rendered += _mix_resampled(dst_buffer + frames_rendered, frame_count - frames_rendered);
		stream->midi_player->setLoop(false);
		stream->loop = true;
		stream->emit_looping_signal();
	}

	return frames_rendered;
}

double AudioStreamGDOPL3Playback::_get_stream_sampling_rate() const {
	return SAMPLE_RATE;
}

void AudioStreamGDOPL3Playback::_bind_methods() {
	// Purposely empty
}

AudioStreamGDOPL3Playback::AudioStreamGDOPL3Playback() {
}
