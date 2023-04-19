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
        
    return errors;
}

std::string loadTextFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    std::string content;

    if (file.is_open()) {
        // Read the entire file into a string
        content.assign((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
        file.close();
    }
    else {
        std::cout << "Failed to open file." << std::endl;
    }
    return content;
}

ComPtr<IDxcResult> compileHLSL(IDxcCompiler3* dxc_compiler, const std::string& hlslText, std::vector<LPCWSTR>& args)
{
    ComPtr<IDxcResult> result;

    DxcBuffer src_buffer;
    src_buffer.Ptr = &*hlslText.begin();
    src_buffer.Size = hlslText.size();
    src_buffer.Encoding = 0;

    dxc_compiler->Compile(&src_buffer, args.data(), args.size(), nullptr, IID_PPV_ARGS(&result));

    return result;
}

void printErrors(std::string& errors)
{
    if (!errors.empty())
    {
        std::cout << "Errors : \n" << errors << std::endl;
    }
}

std::vector<std::uint32_t> getSpirvData(ComPtr<IDxcResult>& result)
{
    HRESULT hr = 0;
    HRESULT status = 0;

    //TODO : Why separate hr and status values?
    hr = result->GetStatus(&status);

    //TODO : Check hr
    ComPtr<IDxcBlob> shader_obj;
    ComPtr<IDxcBlobWide> outputName = {};
    hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader_obj), &outputName);

    std::vector<std::uint32_t> spirv_buffer(shader_obj->GetBufferSize() / sizeof(std::uint32_t));

    //TODO : Use memcpy?
    for (size_t i = 0; i < spirv_buffer.size(); ++i){
        std::uint32_t spv_uint = static_cast<std::uint32_t*>(shader_obj->GetBufferPointer())[i];
        spirv_buffer[i] = spv_uint;
    }

    return spirv_buffer;
}


void printBufferData(std::vector<std::uint32_t>& spirv_buffer)
{
    std::cout << "spv:\n";
    for (size_t i = 0; i < spirv_buffer.size(); ++i){
        std::cout << std::hex << (void*)(spirv_buffer[i]) << " ";
        if (i % 8 == 7){
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

int main()
{
    ComPtr<IDxcUtils> dxc_utils = {};
    ComPtr<IDxcCompiler3> dxc_compiler = {};
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxc_utils));
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxc_compiler));\

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


    //Shader Test 1
    {
        ComPtr<IDxcResult> result = compileHLSL(*dxc_compiler.GetAddressOf(), loadTextFile("data/shaders/testCompilatonSuccess.hlsl"), args);
        std::string errors = getCompilationErrors(result);
        printErrors(errors);
        std::vector<std::uint32_t> spirv_buffer = getSpirvData(result);
        printBufferData(spirv_buffer);
    }


    //Shader Test 2
    {
        ComPtr<IDxcResult> result = compileHLSL(*dxc_compiler.GetAddressOf(), loadTextFile("data/shaders/testCompilatonError.hlsl"), args);
        std::string errors = getCompilationErrors(result);
        printErrors(errors);
        std::vector<std::uint32_t> spirv_buffer = getSpirvData(result);
        printBufferData(spirv_buffer);
    }
}