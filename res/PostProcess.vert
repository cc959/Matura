layout(location = 0) in highp vec4 position;
layout(location = 1) in highp vec2 uv;

out highp vec2 _uv;

void main() {
	gl_Position = position;
	_uv = uv;
}
