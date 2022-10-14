layout(location = 0) in highp vec4 _position;
layout(location = 1) in highp vec2 _uv;

out highp vec2 uv;

void main() {
	gl_Position = _position;
	uv = _uv;
}
