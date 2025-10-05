/**********************************************************************************
*
* Fragment Shader: Glitch Effect
*
* This shader creates a horizontal glitch/screen-tear effect that animates over time.
* It works by randomly displacing horizontal strips of the texture.
*
**********************************************************************************/

#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Uniforms from the C code
uniform sampler2D texture0; // The character's texture
uniform float time;        // A time value that constantly increases

// Output color for the fragment
out vec4 finalColor;

// A simple pseudo-random number generator
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main() {
    // Make the glitch effect happen in bursts
    // The sin wave creates a "pulse" for the effect's intensity
    float glitchIntensity = sin(time * 5.0) * 0.5 + 0.5;

    // Only apply the glitch when the intensity is high
    if (glitchIntensity > 0.8) {
        // Use the vertical texture coordinate and time to create moving horizontal bands
        float displacement = random(vec2(time, fragTexCoord.y * 20.0)) * 0.1;

        // Apply the displacement
        vec2 displacedTexCoord = fragTexCoord;
        displacedTexCoord.x += displacement * (glitchIntensity - 0.8);

        // Sample the texture at three slightly different horizontal positions
        // to create an RGB color separation effect.
        float r = texture(texture0, vec2(displacedTexCoord.x - 0.01, displacedTexCoord.y)).r;
        float g = texture(texture0, displacedTexCoord).g;
        float b = texture(texture0, vec2(displacedTexCoord.x + 0.01, displacedTexCoord.y)).b;
        float a = texture(texture0, displacedTexCoord).a; // Keep original alpha

        // Combine the separated channels for the final color
        finalColor = vec4(r, g, b, a) * fragColor;

    } else {
        // If the effect is not active, just draw the normal texture
        finalColor = texture(texture0, fragTexCoord) * fragColor;
    }
}

