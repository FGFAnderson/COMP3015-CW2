#version 460

out vec4 FragColor;

uniform sampler2D hdrBuffer;
uniform float logAverageLum;

uniform mat3 rgb2xyz = mat3(
        0.4124564, 0.2126729, 0.0193339,
        0.3575761, 0.7151522, 0.1191920,
        0.1804375, 0.0721750, 0.9503041);

uniform mat3 xyz2rgb = mat3(
        3.2404542, -0.9692660, 0.0336434,
        -1.5371385, 1.8760108, -0.2040259,
        -0.4985314, 0.0415560, 1.0572252);

uniform float exposure = 0.2;
uniform float white = 0.928;

void main()
{
    vec3 color = texture(hdrBuffer, TexCoords).rgb;

    // Convert to XYZ
    vec3 xyzCol = rgb2xyz * color;

    // Convert to xyY
    float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
    xyzSum = max(xyzSum, 0.001);
    vec3 xyYCol = vec3(xyzCol.x / xyzSum, xyzCol.y / xyzSum, xyzCol.y);

    // Apply Reinhard tone mapping
    float L = (exposure * xyYCol.z) / max(logAverageLum, 0.001);
    L = (L * (1.0 + L / (white * white))) / (1.0 + L);

    // Convert back to XYZ
    float y = max(xyYCol.y, 0.001);
    xyzCol.x = (L * xyYCol.x) / y;
    xyzCol.y = L;
    xyzCol.z = (L * (1.0 - xyYCol.x - xyYCol.y)) / y;

    // Convert back to RGB
    vec3 mapped = xyz2rgb * xyzCol;

    // Gamma correction
    float gamma = 2.2;
    vec3 corrected = pow(max(mapped, vec3(0.0)), vec3(1.0 / gamma));

    FragColor = vec4(corrected, 1.0);
}
