#include <iostream>

#define NOMINMAX
#include <windows.h>
#include <dxcapi.h>

#include <cstdint>
//#include <string_view>
#include <vector>
#include <locale>
#include <codecvt>

//For comPtr
#include <wrl/client.h>
using namespace Microsoft::WRL;


//Note : The reason for this is that a UTF-8 encoded string may contain multi-byte characters,
//       and the null character ('\0') may appear as a part of a multi-byte sequence.
//       Therefore, treating a UTF-8 encoded string as a null-terminated char* string may lead to incorrect
//       results or even undefined behavior.
//TODO : Test with data/UTF-8-Test1.txt
//TODO : Fix it to work with UTF.
//https://stackoverflow.com/questions/60640010/is-utf-8-which-represented-in-char-stdstring-enough-to-support-all-language
//https://en.cppreference.com/w/cpp/locale/codecvt_utf8
std::string utf8_to_string(const void* buffer, size_t size) {
    return std::string(reinterpret_cast<const char*>(buffer), size);
}

std::string getCompilationErrors(ComPtr<IDxcResult>& result)
{
    std::string errorString;

    IDxcBlobEncoding* errorBlob = nullptr;
    HRESULT hr = result->GetErrorBuffer(&errorBlob);

    //TODO : Check hr
    if (errorBlob == nullptr)
    {
        return errorString;
    }

    void* errorBuffer = errorBlob->GetBufferPointer();
    int bufSize = errorBlob->GetBufferSize();

    //TODO : Check hr
    if(errorBuffer == nullptr || bufSize == 0)
    {
        return errorString;
    }

    BOOL known;
    UINT32 codePage;
    hr = errorBlob->GetEncoding(&known, &codePage);

    //TODO : Check hr and validate encoding.
    return utf8_to_string(errorBuffer, bufSize);
}

int main(){
    std::string hlsl_str = R"(
        patate
        [numthreads(8, 8, 8)] void main(uint3 global_i : SV_DispatchThreadID) {
            // add code here
        }
    )";

    ComPtr<IDxcUtils> dxc_utils = {};
    ComPtr<IDxcCompiler3> dxc_compiler = {};
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxc_utils.ReleaseAndGetAddressOf()));
    //DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxc_utils));
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxc_compiler));

    DxcBuffer src_buffer;
    src_buffer.Ptr = &*hlsl_str.begin();
    src_buffer.Size = hlsl_str.size();
    src_buffer.Encoding = 0;

    std::vector<LPCWSTR> args;
    args.push_back(L"-Zpc");
    args.push_back(L"-HV");
    args.push_back(L"2021");
    args.push_back(L"-T");
    args.push_back(L"cs_6_0");
    args.push_back(L"-E");
    args.push_back(L"main");
    args.push_back(L"-spirv");
    args.push_back(L"-fspv-target-env=vulkan1.1");


    HRESULT hr = 0;

    ComPtr<IDxcResult> result;
    dxc_compiler->Compile(&src_buffer, args.data(), args.size(), nullptr, IID_PPV_ARGS(&result));

    std::string errors = getCompilationErrors(result);

    if (!errors.empty())
    {
        std::cout << "Errors : \n" << errors << std::endl;
    }
    

    HRESULT status;
    hr = result->GetStatus(&status);
    
    ComPtr<IDxcBlob> shader_obj;
    hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader_obj), nullptr);

    hr = result->GetStatus(&status);

    std::vector<std::uint32_t> spirv_buffer;
    spirv_buffer.resize(shader_obj->GetBufferSize() / sizeof(std::uint32_t));

    std::cout << "spv:\n";

    for (size_t i = 0; i < spirv_buffer.size(); ++i)
    {
        std::uint32_t spv_uint = static_cast<std::uint32_t*>(shader_obj->GetBufferPointer())[i];
        spirv_buffer[i] = spv_uint;
        std::cout << std::hex << (void*)spv_uint << " ";
        if (i % 8 == 7)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
    //std::cout<<"Hello, World!\n";
}