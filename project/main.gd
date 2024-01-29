extends Node2D

@onready var player: AudioStreamPlayer = get_node("AudioStreamPlayer")

var MIDI_FILES: Array[String] = [
	"deliberate_concealment fixed.mid",
]
var DEFAULT_BANK: String = "GENMIDI.wopl"
var loops: int = 0

func _ready():
	player.finished.connect(_on_song_end)
	
	player.stream = load_midi(MIDI_FILES[0])
	player.play()
	
func _unhandled_key_input(event):
	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_1:
			player.stream = load_midi(MIDI_FILES[0])
			player.play()
		elif event.keycode == KEY_2:
			player.pitch_scale = 0.75
		elif event.keycode == KEY_3:
			player.pitch_scale = 1.25
			
func _on_song_loop():
	loops += 1
	print("Number of loops: %s" % loops)
	
func _on_song_end():
	print("Finished playing")

func load_midi(
	filename: String,
) -> AudioStreamGDOPL3:
	var stream = AudioStreamGDOPL3.load_from_file(filename, DEFAULT_BANK)
	stream.loop = true
	stream.looped.connect(_on_song_loop)
	return stream
