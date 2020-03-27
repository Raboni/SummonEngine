#include "stdafx.h"
#include "ParticleEmitterLoader.h"
#include "JsonManager.h"
#include <fstream>
#include <d3d11.h>

ParticleEmitterLoader* ParticleEmitterLoader::ourInstance = nullptr;
void ParticleEmitterLoader::CreateInstance(ID3D11Device* aDevice, const unsigned int aMaxEmitterAmount)
{
	if (ourInstance == nullptr)
	{
		ourInstance = new ParticleEmitterLoader(aDevice, aMaxEmitterAmount);
	}
}
ParticleEmitterLoader* ParticleEmitterLoader::GetInstance()
{
	return ourInstance;
}
void ParticleEmitterLoader::DestroyInstance()
{
	if (ourInstance != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
}

ParticleEmitterLoader::ParticleEmitterLoader(ID3D11Device* aDevice, const unsigned int aMaxEmitterAmount) : myEmitterMap(aMaxEmitterAmount)
{
	myDevice = aDevice;
}
ParticleEmitterLoader::~ParticleEmitterLoader()
{
}

ParticleEmitter* ParticleEmitterLoader::CreateParticleEmitter(const std::string& aAlias, const std::string& aTexture, const EmitterSettings& aSettings)
{
	HRESULT result;
	if (!myEmitterMap.Create(aAlias))
	{
		return nullptr;
	}
	ParticleEmitter* emitter = myEmitterMap.Get(aAlias);
	if (emitter == nullptr)
	{
		return emitter;
	}

	#pragma region Buffer
	unsigned int maxParticlesPerInstance = static_cast<unsigned int>(aSettings.myMaxLifeTime * (1.0f / aSettings.myMaxTimeBetweenParticleSpawns));
	D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
	vertexBufferDescription.ByteWidth = sizeof(ParticleData) * maxParticlesPerInstance;
	vertexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ID3D11Buffer* vertexBuffer;
	result = myDevice->CreateBuffer(&vertexBufferDescription, nullptr, &vertexBuffer);
	if (FAILED(result))
	{
		return nullptr;
	}
	#pragma endregion

	#pragma region Shader
	std::ifstream vsFile;
	vsFile.open("Shaders\\ParticleVertex.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	ID3D11VertexShader* vertexShader;
	result = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	if (FAILED(result))
	{
		return nullptr;
	}
	vsFile.close();

	std::ifstream gsFile;
	gsFile.open("Shaders\\ParticleGeometry.cso", std::ios::binary);
	std::string gsData = { std::istreambuf_iterator<char>(gsFile), std::istreambuf_iterator<char>() };
	ID3D11GeometryShader* geometryShader;
	result = myDevice->CreateGeometryShader(gsData.data(), gsData.size(), nullptr, &geometryShader);
	if (FAILED(result))
	{
		return nullptr;
	}
	gsFile.close();

	std::ifstream psFile;
	psFile.open("Shaders\\ParticlePixel.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	ID3D11PixelShader* pixelShader;
	result = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	if (FAILED(result))
	{
		return nullptr;
	}
	psFile.close();
	#pragma endregion

	#pragma region Layout
	CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC, unsigned int> layout(4);
	layout.Add({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "DIRECTION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "TIME", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "DISTANCETOCAMERA", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "SPEED", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	ID3D11InputLayout* inputLayout;
	result = myDevice->CreateInputLayout(layout.GetPointer(), layout.Size(), vsData.data(), vsData.size(), &inputLayout);
	if (FAILED(result))
	{
		return nullptr;
	}
	#pragma endregion

	ParticleEmitterData emitData;
	emitData.myMaxParticlesPerInstance = maxParticlesPerInstance;
	emitData.myStride = sizeof(ParticleData);
	emitData.myOffset = 0;
	emitData.myVertexBuffer = vertexBuffer;
	emitData.myVertexShader = vertexShader;
	emitData.myGeometryShader = geometryShader;
	emitData.myPixelShader = pixelShader;
	emitData.myTexture = TextureInstance(aTexture);
	emitData.myTopology = static_cast<unsigned short>(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	emitData.myInputLayout = inputLayout;
	emitter->myData = emitData;
	emitter->mySettings = aSettings;
	emitter->myShouldUpdateBuffer = false;
	emitter->myUserCount = 0;

	return emitter;
}
ParticleEmitter* ParticleEmitterLoader::CreateParticleEmitter(const std::string& aAlias, const std::string& aTexture, const std::string& aJsonAlias)
{
	HRESULT result;
	if (!myEmitterMap.Create(aAlias))
	{
		return nullptr;
	}
	ParticleEmitter* emitter = myEmitterMap.Get(aAlias);
	if (emitter == nullptr)
	{
		return emitter;
	}

	rapidjson::Document& doc = *JsonManager::GetInstance()->Get(aJsonAlias);
	EmitterSettings settings;
	settings.myStartColor = CU::Vector4f(doc["Color"]["Start"]["R"].GetFloat(), doc["Color"]["Start"]["G"].GetFloat(), doc["Color"]["Start"]["B"].GetFloat(), doc["Color"]["Start"]["A"].GetFloat());
	settings.myEndColor = CU::Vector4f(doc["Color"]["End"]["R"].GetFloat(), doc["Color"]["End"]["G"].GetFloat(), doc["Color"]["End"]["B"].GetFloat(), doc["Color"]["End"]["A"].GetFloat());
	settings.myMinEmitAngles = CU::Vector3f(doc["EmitAngles"]["Min"]["X"].GetFloat(), doc["EmitAngles"]["Min"]["Y"].GetFloat(), doc["EmitAngles"]["Min"]["Z"].GetFloat());
	settings.myMaxEmitAngles = CU::Vector3f(doc["EmitAngles"]["Max"]["X"].GetFloat(), doc["EmitAngles"]["Max"]["Y"].GetFloat(), doc["EmitAngles"]["Max"]["Z"].GetFloat());
	settings.myAcceleration = doc["Acceleration"].GetFloat();
	settings.myStartScale = doc["Scale"]["Start"].GetFloat();
	settings.myEndScale = doc["Scale"]["End"].GetFloat();
	settings.myMinTimeBetweenParticleSpawns = doc["TimeBetweenParticleSpawns"]["Min"].GetFloat();
	settings.myMaxTimeBetweenParticleSpawns = doc["TimeBetweenParticleSpawns"]["Max"].GetFloat();
	settings.myMinStartSpeed = doc["StartSpeed"]["Min"].GetFloat();
	settings.myMaxStartSpeed = doc["StartSpeed"]["Max"].GetFloat();
	settings.myMinLifeTime = doc["LifeTime"]["Min"].GetFloat();
	settings.myMaxLifeTime = doc["LifeTime"]["Max"].GetFloat();
	settings.myMaxSpawnOffset = doc["MaxSpawnOffset"].GetFloat();

	#pragma region Buffer
	unsigned int maxParticlesPerInstance = static_cast<unsigned int>(settings.myMaxLifeTime * (1.0f / settings.myMaxTimeBetweenParticleSpawns));
	D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
	vertexBufferDescription.ByteWidth = sizeof(ParticleData) * maxParticlesPerInstance;
	vertexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ID3D11Buffer* vertexBuffer;
	result = myDevice->CreateBuffer(&vertexBufferDescription, nullptr, &vertexBuffer);
	if (FAILED(result))
	{
		return nullptr;
	}
	#pragma endregion

	#pragma region Shader
	std::ifstream vsFile;
	vsFile.open("Shaders\\ParticleVertex.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	ID3D11VertexShader* vertexShader;
	result = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	if (FAILED(result))
	{
		return nullptr;
	}
	vsFile.close();

	std::ifstream gsFile;
	gsFile.open("Shaders\\ParticleGeometry.cso", std::ios::binary);
	std::string gsData = { std::istreambuf_iterator<char>(gsFile), std::istreambuf_iterator<char>() };
	ID3D11GeometryShader* geometryShader;
	result = myDevice->CreateGeometryShader(gsData.data(), gsData.size(), nullptr, &geometryShader);
	if (FAILED(result))
	{
		return nullptr;
	}
	gsFile.close();

	std::ifstream psFile;
	psFile.open("Shaders\\ParticlePixel.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	ID3D11PixelShader* pixelShader;
	result = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	if (FAILED(result))
	{
		return nullptr;
	}
	psFile.close();
	#pragma endregion

	#pragma region Layout
	CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC, unsigned int> layout(6);
	layout.Add({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "DIRECTION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "TIME", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "DISTANCETOCAMERA", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "SPEED", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	ID3D11InputLayout* inputLayout;
	result = myDevice->CreateInputLayout(layout.GetPointer(), layout.Size(), vsData.data(), vsData.size(), &inputLayout);
	if (FAILED(result))
	{
		return nullptr;
	}
	#pragma endregion

	ParticleEmitterData emitData;
	emitData.myMaxParticlesPerInstance = maxParticlesPerInstance;
	emitData.myStride = sizeof(ParticleData);
	emitData.myOffset = 0;
	emitData.myVertexBuffer = vertexBuffer;
	emitData.myVertexShader = vertexShader;
	emitData.myGeometryShader = geometryShader;
	emitData.myPixelShader = pixelShader;
	emitData.myTexture = TextureInstance(aTexture);
	emitData.myTopology = static_cast<unsigned short>(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	emitData.myInputLayout = inputLayout;
	emitter->myData = emitData;
	emitter->mySettings = settings;
	emitter->myShouldUpdateBuffer = false;
	emitter->myUserCount = 0;

	return emitter;
}
ParticleEmitter* ParticleEmitterLoader::GetParticleEmitter(const std::string& aAlias)
{
	return myEmitterMap.Get(aAlias);
}

void ParticleEmitterLoader::UpdateParticleEmitterBuffer(ParticleEmitter* aEmitter)
{
	if (aEmitter != nullptr && aEmitter->GetUserCount() > 0 && aEmitter->myShouldUpdateBuffer)
	{
		HRESULT result;
		ParticleEmitterData& data = aEmitter->GetData();
		D3D11_BUFFER_DESC vertexBufferDescription = { 0 };

		vertexBufferDescription.ByteWidth = sizeof(ParticleData) * data.myMaxParticlesPerInstance * aEmitter->GetUserCount();
		vertexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		aEmitter->myShouldUpdateBuffer = false;
		data.myVertexBuffer->Release();
		result = myDevice->CreateBuffer(&vertexBufferDescription, nullptr, &data.myVertexBuffer);
		if (FAILED(result))
		{
			data.myVertexBuffer = nullptr;
			return;
		}
	}
}
