#version 450 core

out vec4 fragColor;
#define PI 3.14159265359

const float AA = 2;

uniform vec2 u_resolution;
uniform float u_time;
uniform int smple;
uniform vec3 lightDir;

uniform int n;

uniform mat4 _CameraToWorld;
uniform mat4 _CameraInverseProjection;

uniform sampler2D skybox;

// struct Light {
// 	vec3 position;
// 	vec3 color;
// };

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Object {
	vec4 position;
	vec4 size;
	vec4 color;
	float smoothness;
	uint data;
	int padA;
	int padB;
};

struct Hit {
	vec3 position;
	float dist;
	vec4 color;
	int object;
};

layout(std140, binding = 0) uniform object_buffer {
	Object objects[10];
};

mat3 rotationX(float angle) {
	float s = sin(angle);
	float c = cos(angle);

	return mat3(1.0, 0.0, 0.0, 0.0, c, s, 0.0, -s, c);
}

mat3 rotationY(float angle) {
	float s = sin(angle);
	float c = cos(angle);

	return mat3(c, 0.0, -s, 0.0, 1.0, 0.0, s, 0.0, c);
}

mat3 rotationZ(float angle) {
	float s = sin(angle);
	float c = cos(angle);

	return mat3(c, s, 0.0, -s, c, 0.0, 0.0, 0.0, 1.0);
}

vec2 random2(vec2 p) {
	return fract(sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)))) * 43758.5453);
}

// 2D Random
float random(vec2 seed) {
	return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453123);
}

mat3 randomRot(float scatter, vec3 seed) {
	return rotationX((random(seed.xy) - 0.5f) * scatter) * rotationY((random(seed.yz) - 0.5f) * scatter) * rotationZ((random(seed.zx) - 0.5f) * scatter);
}

float sdSphere(vec3 pos, float r, vec3 sp) {
	vec3 d = pos - sp;
	//if(max(abs(d.x), abs(d.z)) < 17.5)
	//	d.xz = mod(d.xz + 2.5, 5) - 2.5;
	return length(d) - r;
}

float sdRect(vec3 pos, vec3 size, vec3 sp) {
	vec3 q = abs(pos - sp) - size;
	return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

int Iterations = 15;
float Bailout = 2;
uniform float Power = 6;

float deBulb(vec3 pos) {
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for(int i = 0; i < Iterations; i++) {
		r = length(z);
		if(r > Bailout)
			break;

		// convert to polar coordinates
		float theta = acos(z.z / r);
		float phi = atan(z.y, z.x);
		dr = pow(r, Power - 1.0) * Power * dr + 1.0;

		// scale and rotate the point
		float zr = pow(r, Power);
		theta = theta * Power;
		phi = phi * Power;

		// convert back to cartesian coordinates
		z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
		z += pos;
	}
	return 0.5 * log(r) * r / dr;
}

float sd(Object object, vec3 sp) {
	uint type = object.data & ((1 << 30) - 1);
	if(type == 0)
		return sdSphere(object.position.xyz, object.size.y, sp);
	if(type == 1)
		return sdRect(object.position.xyz, object.size.xyz, sp);
	if(type == 2)
		return deBulb(((sp - object.position.xyz) / object.size.x)) * object.size.x;
}

vec4 sampleSkyBox(vec3 direction) {
	// Sample the skybox and write it
	float theta = acos(direction.y) / -PI;
	float phi = atan(-direction.z, direction.x) / -PI * 0.5f;
	return vec4(0, 0, 0.01, 0);//vec4(texture(skybox, vec2(phi, theta)).xyz, 0);
}

Hit query(vec3 sp) {
	Hit o;
	for(int i = 0; i < n; i++) {
		float dist = sd(objects[i], sp);
		if((objects[i].data & (1u << 30)) != 0)
			dist *= -1;

		Hit new = Hit(sp, dist, objects[i].color, i);

		if(i == 0)
			o = new;
		else {
			if((objects[i].data & (1u << 31)) != 0) {
				if(new.dist > o.dist)
					o = new;
			} else {
				if(new.dist < o.dist)
					o = new;
			}
		}
	}
	return o;
}

Ray CreateCameraRay(vec2 uv) {
    // Transform the camera origin to world space
	vec3 origin = (_CameraToWorld * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;

    // Invert the perspective projection of the view-space position
	vec3 direction = (_CameraInverseProjection * vec4(uv, 0.0f, 1.0f)).xyz;
    // Transform the direction from camera to world space and normalize
	direction = (_CameraToWorld * vec4(direction, 0.0f)).xyz;
	direction = normalize(direction);
	return Ray(origin, direction);
}

float eps = 1e-3;
int maxsteps = 300;

Hit march(Ray ray) {
	int cnt = 0;

	Hit hit;

	while(true) {
		Hit d = query(ray.origin);
		ray.origin += ray.direction * d.dist;

		if(d.dist < eps) {
			hit = d;
			hit.color = mix(vec4(vec3(0.64, 0.16, 0.16) * 2, 0), hit.color, smoothstep(0, 1, cnt / float(maxsteps) / 1.5f));
			break;
		}

		if(++cnt > maxsteps || d.dist > 1.f / eps)
			return Hit(ray.origin, d.dist, sampleSkyBox(ray.direction) + objects[0].color * pow(cnt / float(maxsteps) * 0.07, 1.5f), -1);
	}

	return hit;

}

vec3 render(vec2 fc) {

	vec2 uv = fc / u_resolution * 2.f - vec2(1.f);

	Ray ray = CreateCameraRay(uv);

	Hit hit = march(ray);

	vec3 outColor = vec3(0);
	vec3 colorMultiplier = vec3(1);

	for(int i = 0; i < 3; i++) {

		if(hit.object == -1) {
			outColor += hit.color.xyz * colorMultiplier;
			break;
		}

		float center = hit.dist;
		float x = query(hit.position + vec3(eps, 0, 0)).dist;
		float y = query(hit.position + vec3(0, eps, 0)).dist;
		float z = query(hit.position + vec3(0, 0, eps)).dist;

		vec3 normal = (vec3(x, y, z) - center) / eps;
		vec3 refl = reflect(ray.direction, normal);

		float scatter = PI * (1.f - objects[hit.object].smoothness);

		refl *= randomRot(scatter, normal + hit.position);

		vec3 direction = lightDir * randomRot(0.03, refl);

		Hit light = march(Ray(hit.position - direction * 5e-2, -direction));

		ray = Ray(hit.position + refl * eps * 10, refl);
		Hit newhit = march(ray);

		float diffuse = 0.f;
		float specular = 0.f;

		if(light.object == -1)
			diffuse = max(dot(normal, -lightDir), 0.f);

		//if(newhit.object == -1)
		//	specular = pow(max(dot(refl, -lightDir), 0.f), 8.f * objects[hit.object].smoothness);

		float lightMultiplier = max(min(mix(diffuse, specular, 0.1 + objects[hit.object].smoothness * 0.8), 1), hit.color.w);

		outColor += hit.color.xyz * colorMultiplier * lightMultiplier;
		colorMultiplier *= 0.5 * (mix(hit.color.xyz, vec3(1.f), objects[hit.object].smoothness * 0.5));

		hit = newhit;
	}

	return outColor;
}

void main() {
	vec2 fc = gl_FragCoord.xy;

	vec3 outColor = vec3(0);

	outColor += render(fc + random2(fc * u_time));

	// for(int i = 0; i < 3; i++) {
	// 	float r = random(fc * float(i + 453) * (u_time + 5.f)) * 2.f * PI;
	// 	vec2 f = fc + vec2(sin(r), cos(r)) * random(fc * r) * 4;

	// 	vec2 uv = f / u_resolution * 2.f - 1.f;

	// 	Hit bloom = march(CreateCameraRay(uv));

	// 	if(bloom.color.w > 0.5)
	// 		outColor += bloom.color.xyz + vec3(0.4);
	// }

	outColor = pow(outColor, vec3(1.0 / 1.5));

	fragColor = vec4(outColor, 1.f / float(smple));
}
