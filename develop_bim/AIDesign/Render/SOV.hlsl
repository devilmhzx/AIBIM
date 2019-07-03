struct Assmble
{
            float3 Vertex:POSITION;
};
struct VertexHomogeneous
{
            float4 Vertex:SV_POSITION;
};
cbuffer ViewProj:register(b0)
{
            float4x4 mat;
};
VertexHomogeneous main(Assmble IN )
{
           VertexHomogeneous OUT;
          OUT.Vertex = mul(float4(IN.Vertex, 1.f), mat);
          return OUT;
}