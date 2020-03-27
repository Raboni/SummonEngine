#pragma once
#include <Vector4.hpp>
#include <Vector3.hpp>
#include <string>

struct ID3D11Device;
struct ID3D11ShaderResourceView;

class EnviromentLight
{
public:
	EnviromentLight();
	EnviromentLight(const std::string& aCubemapPath, ID3D11Device* aDevice);
	~EnviromentLight() = default;

	void Init(const std::string& aCubemapPath, ID3D11Device* aDevice);
	void SetColor(const CU::Vector3f& aColor);
	void SetDirection(const CU::Vector3f& aDirection);
	const CU::Vector3f& GetColor() const;
	const CU::Vector3f& GetDirection() const;
	const std::string& GetPath() const;
	ID3D11ShaderResourceView* const* GetCubemap() const;
	const int GetMipLevels() const;

private:
	CU::Vector3f myColor;
	CU::Vector3f myDirection;
	std::string myPath;
	ID3D11ShaderResourceView* myCubemap;
	int myMipLevels;
};