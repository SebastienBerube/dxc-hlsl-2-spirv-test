#include <iostream>

#define NOMINMAX
#include <windows.h>
#include <dxcapi.h>

#include <cstdint>
#include <vector>
#include <locale>
#include <codecvt>

#include <fstream>

#include <wrl/client.h>
using namespace Microsoft::WRL;

std::string getCompilationErrors(ComPtr<IDxcResult>& result)
{
    std::string errors;
    ComPtr<IDxcBlobWide> outputName = {};
    ComPtr<IDxcBlobUtf8> dxcErrorInfo = {};
    result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&dxcErrorInfo), &outputName);

    if (dxcErrorInfo != nullptr){
        errors = std::string(dxcErrorInfo->GetStringPointer());
    }
        
    return std::move(errors);
}

std::string loadTextFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    std::string content;

    if (file.is_open()) {
        // Read the entire file into a string
        content.assign((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
        file.close();

        // Print the contents of the string
        std::cout << content << std::endl;
    }
    else {
        std::cout << "Failed to open file." << std::endl;
    }

    return std::move(content);
}

int main()
{
    std::string hlsl_str = loadTextFile("data/shaders/testUTF8.hlsl");
    ComPtr<IDxcUtils> dxc_utils = {};
    ComPtr<IDxcCompiler3> dxc_compiler = {};
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxc_utils));
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
    ComPtr<IDxcBlobWide> outputName = {};
    hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader_obj), &outputName);

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
}