struct VertexHomogeneous
{
            float4 Vertex:SV_POSITION;
}; 
cbuffer PerObject:register(b0)
{
            float4 Color;
};
float4 main(VertexHomogeneous IN) : SV_TARGET
{
	return Color;
}