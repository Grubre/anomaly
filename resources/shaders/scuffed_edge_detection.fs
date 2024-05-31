#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(800, 450);

void main()
{
    float x = 1.0 / resolution.x;
    float y = 1.0 / resolution.y;

    vec3 horizEdge = vec3(0.0);
    horizEdge -= texture(texture0, vec2(fragTexCoord.x - x, fragTexCoord.y - y)).rgb * 1.0;
    horizEdge -= texture(texture0, vec2(fragTexCoord.x - x, fragTexCoord.y    )).rgb * 2.0;
    horizEdge -= texture(texture0, vec2(fragTexCoord.x - x, fragTexCoord.y + y)).rgb * 1.0;
    horizEdge += texture(texture0, vec2(fragTexCoord.x + x, fragTexCoord.y - y)).rgb * 1.0;
    horizEdge += texture(texture0, vec2(fragTexCoord.x + x, fragTexCoord.y    )).rgb * 2.0;
    horizEdge += texture(texture0, vec2(fragTexCoord.x + x, fragTexCoord.y + y)).rgb * 1.0;

    vec3 vertEdge = vec3(0.0);
    vertEdge -= texture(texture0, vec2(fragTexCoord.x - x, fragTexCoord.y - y)).rgb * 1.0;
    vertEdge -= texture(texture0, vec2(fragTexCoord.x    , fragTexCoord.y - y)).rgb * 2.0;
    vertEdge -= texture(texture0, vec2(fragTexCoord.x + x, fragTexCoord.y - y)).rgb * 1.0;
    vertEdge += texture(texture0, vec2(fragTexCoord.x - x, fragTexCoord.y + y)).rgb * 1.0;
    vertEdge += texture(texture0, vec2(fragTexCoord.x    , fragTexCoord.y + y)).rgb * 2.0;
    vertEdge += texture(texture0, vec2(fragTexCoord.x + x, fragTexCoord.y + y)).rgb * 1.0;

    vec3 edge = sqrt((horizEdge * horizEdge) + (vertEdge * vertEdge));
    float edgeIntensity = dot(edge, vec3(0.299, 0.587, 0.114)); // Convert to grayscale intensity

    finalColor = vec4(vec3(edgeIntensity), 1.0);
}

