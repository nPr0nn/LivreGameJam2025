/**********************************************************************************
*
* Fragment Shader: Glitch Effect (WebGL 1.0 Version)
*
* This shader creates a horizontal glitch/screen-tear effect that animates over time.
* It works by randomly displacing horizontal strips of the texture.
*
**********************************************************************************/

// WebGL 1.0 is based on an older version of GLSL
#version 100

// Precision must be declared for floats in WebGL fragment shaders
precision mediump float;

// Input attributes from the vertex shader are called 'varying' in this version
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Uniforms from the JavaScript/C code
uniform sampler2D texture0; // The character's texture
uniform float time;        // A time value that constantly increases

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
        // In WebGL 1.0, the texture sampling function is called texture2D()
        float r = texture2D(texture0, vec2(displacedTexCoord.x - 0.01, displacedTexCoord.y)).r;
        float g = texture2D(texture0, displacedTexCoord).g;
        float b = texture2D(texture0, vec2(displacedTexCoord.x + 0.01, displacedTexCoord.y)).b;
        float a = texture2D(texture0, displacedTexCoord).a; // Keep original alpha

        // Combine the separated channels and assign to the built-in gl_FragColor variable
        gl_FragColor = vec4(r, g, b, a) * fragColor;

    } else {
        // If the effect is not active, just draw the normal texture
        gl_FragColor = texture2D(texture0, fragTexCoord) * fragColor;
    }
}

