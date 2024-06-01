#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec2 resolution;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
    // Fetch the texture color at the fragment position
    vec4 texelColor = texture(texture0, fragTexCoord);

    // Calculate the normalized texture coordinates
    vec2 uv = fragTexCoord;

    // Center the coordinates around (0.5, 0.5)
    uv = uv * 2.0 - 1.0;

    // Calculate the distance from the center
    float dist = length(uv);

    // Adjust the vignette effect strength and shape
    float vig = smoothstep(0.7, 1.0, dist);

    // Apply the vignette effect to the texture color
    finalColor = texelColor * mix(vec4(1.0), vec4(0.0), vig * 0.3);
}

