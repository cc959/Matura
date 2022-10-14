out vec4 fragmentColor;

in highp vec2 _uv;
uniform ivec2 _srcResolution;
uniform ivec2 _destResolution;

uniform sampler2D _texture;
uniform int mode;

const int downsample = 0;
const int upsample = 1;
const int merge = 2;

void main() {

	vec2 uv = _uv;

	if(mode == downsample) {
		uv *= 2;
		clamp(uv, vec2(0), vec2(_srcResolution) / vec2(textureSize(_texture, 0)));

		highp vec2 pixelSize = 1.0 / textureSize(_texture, 0);
		float x = pixelSize.x;
		float y = pixelSize.y;

		vec3 a = texture(_texture, vec2(uv.x - 2 * x, uv.y + 2 * y)).rgb;
		vec3 b = texture(_texture, vec2(uv.x, uv.y + 2 * y)).rgb;
		vec3 c = texture(_texture, vec2(uv.x + 2 * x, uv.y + 2 * y)).rgb;

		vec3 d = texture(_texture, vec2(uv.x - 2 * x, uv.y)).rgb;
		vec3 e = texture(_texture, vec2(uv.x, uv.y)).rgb;
		vec3 f = texture(_texture, vec2(uv.x + 2 * x, uv.y)).rgb;

		vec3 g = texture(_texture, vec2(uv.x - 2 * x, uv.y - 2 * y)).rgb;
		vec3 h = texture(_texture, vec2(uv.x, uv.y - 2 * y)).rgb;
		vec3 i = texture(_texture, vec2(uv.x + 2 * x, uv.y - 2 * y)).rgb;

		vec3 j = texture(_texture, vec2(uv.x - x, uv.y + y)).rgb;
		vec3 k = texture(_texture, vec2(uv.x + x, uv.y + y)).rgb;
		vec3 l = texture(_texture, vec2(uv.x - x, uv.y - y)).rgb;
		vec3 m = texture(_texture, vec2(uv.x + x, uv.y - y)).rgb;

		fragmentColor.xyz = e * 0.125;
		fragmentColor.xyz += (a + c + g + i) * 0.03125;
		fragmentColor.xyz += (b + d + f + h) * 0.0625;
		fragmentColor.xyz += (j + k + l + m) * 0.125;

		fragmentColor.w = 1;
	}

	if(mode == upsample) {

		uv /= 2;//vec2(_srcResolution) / vec2(textureSize(_texture, 0));

		clamp(uv, vec2(0), vec2(_srcResolution) / vec2(textureSize(_texture, 0)));

		highp vec2 pixelSize = 1.0 / textureSize(_texture, 0);

		vec2 filterRadius = pixelSize;

		float x = filterRadius.x;
		float y = filterRadius.y;

		vec3 a = texture(_texture, vec2(uv.x - x, uv.y + y)).rgb;
		vec3 b = texture(_texture, vec2(uv.x, uv.y + y)).rgb;
		vec3 c = texture(_texture, vec2(uv.x + x, uv.y + y)).rgb;

		vec3 d = texture(_texture, vec2(uv.x - x, uv.y)).rgb;
		vec3 e = texture(_texture, vec2(uv.x, uv.y)).rgb;
		vec3 f = texture(_texture, vec2(uv.x + x, uv.y)).rgb;

		vec3 g = texture(_texture, vec2(uv.x - x, uv.y - y)).rgb;
		vec3 h = texture(_texture, vec2(uv.x, uv.y - y)).rgb;
		vec3 i = texture(_texture, vec2(uv.x + x, uv.y - y)).rgb;

		fragmentColor.xyz = e * 4.0;
		fragmentColor.xyz += (b + d + f + h) * 2.0;
		fragmentColor.xyz += (a + c + g + i);
		fragmentColor.xyz *= 1.0 / 16.0;

		fragmentColor.w = 1;

	}

	if(mode == merge) {
		fragmentColor = vec4(texture2D(_texture, uv).xyz, 0.1);
	}

}