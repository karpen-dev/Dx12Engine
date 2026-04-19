cbuffer TransformBuffer : register(b0) {
    float4x4 modelViewProjection;
    float4x4 modelView;
    float3 lightDirection;
    float time;
};

struct VSInput {
    float3 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 worldNormal : TEXCOORD0;
    float3 worldPosition : TEXCOORD1;
};

VSOutput main(VSInput input) {
    VSOutput output;

    float4 pos = float4(input.position, 1.0f);
    output.position = mul(pos, modelViewProjection);
    output.worldPosition = mul(pos, modelView).xyz;
    output.worldNormal = mul(input.normal, (float3x3)modelView);
    output.worldNormal = normalize(output.worldNormal);
    output.color = input.color;

    return output;
}