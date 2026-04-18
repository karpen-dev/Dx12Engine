struct PSInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 worldNormal : TEXCOORD0;
    float3 worldPosition : TEXCOORD1;
};

float4 main(PSInput input) : SV_TARGET {
    float3 normal = normalize(input.worldNormal);

    float3 lightDir = normalize(float3(0.5f, -1.0f, 0.3f));

    float diffuse = max(dot(normal, -lightDir), 0.2f);

    float ambient = 0.3f;

    float3 viewDir = normalize(-input.worldPosition);
    float3 reflectDir = reflect(lightDir, normal);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);

    float3 finalColor = input.color.rgb * (diffuse + ambient);
    finalColor += float3(0.5f, 0.5f, 0.5f) * specular;

    return float4(finalColor, 1.0f);
}