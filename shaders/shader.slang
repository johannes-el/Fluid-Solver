struct VSInput {
       float3 inPosition : POSITION;
       float3 inColor    : COLOR0;
       float2 inTexCoord : TEXCOORD0;
};

struct UniformBuffer {
       float4x4 model;
       float4x4 view;
       float4x4 proj;

       float3 lightPos;
       float3 lightColor;
       float3 objectColor;
};

ConstantBuffer<UniformBuffer> ubo;

struct VSOutput {
       float4 pos : SV_Position;
       float3 fragColor;
       float2 fragTexCoord;
};

[shader("vertex")]
VSOutput vertMain(VSInput input) {
	 VSOutput output;
	 output.pos = mul(ubo.proj, mul(ubo.view, mul(ubo.model, float4(input.inPosition.x, input.inPosition.y, input.inPosition.z, 1.0))));
	 output.fragColor = input.inColor;
	 output.fragTexCoord = input.inTexCoord;
	 return output;
}

Sampler2D texture;

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_Target {
       // float4 color = texture.Sample(vertIn.fragTexCoord);
       // float3 result = color.rgb * ubo.lightColor;
       // return float4(result, color.a);

       return texture.Sample(vertIn.fragTexCoord);
}