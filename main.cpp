#include <iostream>

#define NOMINMAX
#include <windows.h>
#include <dxcapi.h>

#include <vector>

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
    HRESULT status = 0;
    HRESULT hr = result->GetStatus(&status);
    if (FAILED(hr) || FAILED(status))
    {
        throw std::runtime_error("IDxcResult::GetStatus failed with HRESULT = " + status);
    }

    ComPtr<IDxcBlob> shader_obj;
    ComPtr<IDxcBlobWide> outputName = {};
    hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader_obj), &outputName);
    if (FAILED(hr) || FAILED(status))
    {
        throw std::runtime_error("IDxcResult::GetStatus failed with HRESULT = " + status);
    }

    const auto shader_size = shader_obj->GetBufferSize();
    if (shader_size % sizeof(std::uint32_t) != 0)
    {
        throw std::runtime_error("Invalid SPIR-V buffer size");
    }

    const auto num_words = shader_size / sizeof(std::uint32_t);
    std::vector<std::uint32_t> spirv_buffer(num_words);

    memcpy(spirv_buffer.data(), shader_obj->GetBufferPointer(), shader_obj->GetBufferSize());

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

void testShaderCompilation(ComPtr<IDxcCompiler3>& dxc_compiler, const std::string& filePath, std::vector<LPCWSTR>& args)
{
    ComPtr<IDxcResult> result = compileHLSL(*dxc_compiler.GetAddressOf(), loadTextFile(filePath), args);
    std::string errors = getCompilationErrors(result);
    if (!errors.empty())
    {
        printErrors(errors);
    }
    else
    {
        std::vector<std::uint32_t> spirv_buffer = getSpirvData(result);
        printBufferData(spirv_buffer);
    }
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


    std::cout << "Test 1 : This one should compile\n\n";
    testShaderCompilation(dxc_compiler, "data/shaders/testCompilatonSuccess.hlsl", args);
    std::cout << "\n------------------------------\n\n";
    std::cout << "Test 2 : This one should print errors\n\n";
    testShaderCompilation(dxc_compiler, "data/shaders/testCompilatonError.hlsl", args);
}