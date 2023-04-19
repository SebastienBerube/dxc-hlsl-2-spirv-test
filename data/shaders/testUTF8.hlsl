//patate éà
[numthreads(8, 8, 8)] void main(uint3 global_i : SV_DispatchThreadID) {
	// add code here
	float4 vTest = float4((float)global_i.x, (float)global_i.y, (float)global_i.z, 0);
    float4 v2 = v1;
    float4 vTest2 = float4((float)global_i.x, (float)global_i.y, (float)global_i.z, 0);
    float4 v4 = v3;
}
