//this code should compile
//
//UTF8 test (random UTF-8 chars):
//     ŠןɉъҹĦİ·̈ȥ۳ٴ̻ؽ̊ܚ
//     ʳϛƥȀۤҿ͔Ԝ҂ɝŨױʠɊ݉ܐ
//     ÌàۂɕӬҗѪ̹߮פڹړȾݒĸê
//     ǿ׋ق˿ɴ°ڥ׬щߖ΢Ùסȼ݅
//     ˗ΖɊΧ٭ϟڴޒ¢̇źʟǔ׍ը
//     ɕݫÞшǈ߳ȁىЇÌֶӔ͞ÓƿŇ
//     ߆ߺӰֱ̇ɷĜܴŦͼǃǒƀ̽ű
//     ɅʞܒЌ˴̯ۇ̡ƽׅʮù߅ԩ˸
[numthreads(8, 8, 8)] void main(uint3 global_i : SV_DispatchThreadID) {
    float4 vTest = float4((float)global_i.x, (float)global_i.y, (float)global_i.z, 0);
}
