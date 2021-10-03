#version 460 core

struct Light {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float atten_a;
    float atten_b;

    float cone_a;
    float cone_b;
};

in vec3 gPosition;
in vec3 gNormal;
in vec2 gTexCoord;

uniform vec3 uCamPos;

uniform sampler2D uDiffuse;
uniform sampler2D uSpecular;
uniform float uShininess;

uniform samplerCube uSkybox;

uniform Light uLight;

void main() 
{
    // ambient
    vec3 ambient = uLight.ambient * texture(uDiffuse, gTexCoord).rgb;
  	
    // diffuse
    vec3 norm = normalize(gNormal);
    vec3 lightDir = normalize(gPosition - uLight.position);
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = uLight.diffuse * diff * texture(uDiffuse, gTexCoord).rgb;
    
    // specular
    vec3 viewDir = normalize(uCamPos - gPosition);
    vec3 halfDir = normalize(viewDir - lightDir);
    float spec = pow(max(dot(halfDir, norm), 0.0), uShininess);
    vec3 specular = uLight.specular * spec * texture(uSpecular, gTexCoord).r;
    
    float dist = length(gPosition - uLight.position);
    float atten = uLight.atten_a * dist + uLight.atten_b * dist * dist;

    float cone = dot(lightDir, normalize(uLight.direction));
    float inten = clamp((cone - uLight.cone_b) / (uLight.cone_a - uLight.cone_b), 0.0, 1.0);

    vec3 lightColor = (ambient * inten + diffuse * inten + specular) / (atten + 1.0);

    vec3 envFlecDir = reflect(-viewDir, norm);
    vec3 envFracDir = refract(-viewDir, norm, 1.0 / 1.5);

    vec3 envFlecColor = texture(uSkybox, envFlecDir).rgb;
    vec3 envFracColor = texture(uSkybox, envFracDir).rgb;

    gl_FragColor = vec4(lightColor * 0.8 + envFlecColor * 0.1 + envFracColor * 0.1, 1.0);
}
