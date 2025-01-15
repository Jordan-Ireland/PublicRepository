#pragma once

#include <d3d11.h>
#include <wrl/client.h>

template<typename T>
class ConstantBuffer
{
public:
    ConstantBuffer();
    ~ConstantBuffer();

    bool Initialize(ID3D11Device* device);
    void Update(ID3D11DeviceContext* context, const T& data);
    void BindVS(ID3D11DeviceContext* context, UINT startSlot);
    void BindPS(ID3D11DeviceContext* context, UINT startSlot);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};

template<typename T>
ConstantBuffer<T>::ConstantBuffer()
{
}

template<typename T>
ConstantBuffer<T>::~ConstantBuffer()
{
}

template<typename T>
bool ConstantBuffer<T>::Initialize(ID3D11Device* device)
{
    CD3D11_BUFFER_DESC bDesc(sizeof(T), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    HRESULT hr = device->CreateBuffer(&bDesc, nullptr, buffer.GetAddressOf());
    return SUCCEEDED(hr);
}

template<typename T>
void ConstantBuffer<T>::Update(ID3D11DeviceContext* context, const T& data)
{
    D3D11_MAPPED_SUBRESOURCE gpuBuffer;
    context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &data, sizeof(T));
    context->Unmap(buffer.Get(), 0);
}

template<typename T>
void ConstantBuffer<T>::BindVS(ID3D11DeviceContext* context, UINT startSlot)
{
    context->VSSetConstantBuffers(startSlot, 1, buffer.GetAddressOf());
}

template<typename T>
void ConstantBuffer<T>::BindPS(ID3D11DeviceContext* context, UINT startSlot)
{
    context->PSSetConstantBuffers(startSlot, 1, buffer.GetAddressOf());
}