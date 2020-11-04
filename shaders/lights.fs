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

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

uniform vec3 uCamPos;

uniform sampler2D uDiffuse;
uniform sampler2D uSpecular;
uniform float uShininess;

uniform samplerCube uSkybox;

uniform Light uLight;

void main() 
{
    // ambient
    vec3 ambient = uLight.ambient * texture(uDiffuse, vTexCoord).rgb;
  	
    // diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(vPosition - uLight.position);
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = uLight.diffuse * diff * texture(uDiffuse, vTexCoord).rgb;
    
    // specular
    vec3 viewDir = normalize(uCamPos - vPosition);
    vec3 reflectDir = reflect(lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    vec3 specular = uLight.specular * spec * texture(uSpecular, vTexCoord).r;
    
    float dist = length(vPosition - uLight.position);
    float atten = uLight.atten_a * dist + uLight.atten_b * dist * dist;

    float cone = dot(lightDir, normalize(uLight.direction));
    float inten = clamp((cone - uLight.cone_b) / (uLight.cone_a - uLight.cone_b), 0.0, 1.0);

    vec3 lightColor = (ambient * inten + diffuse * inten + specular) / (atten + 1.0);

    vec3 envFlecDir = reflect(-viewDir, norm);
    vec3 envFracDir = refract(-viewDir, norm, 1.0 / 1.5);

    vec3 envFlecColor = texture(uSkybox, envFlecDir).rgb;
    vec3 envFracColor = texture(uSkybox, envFracDir).rgb;

    gl_FragColor = vec4(lightColor * 0.6 + envFlecColor * 0.2 + envFracColor * 0.2, 1.0);
}
