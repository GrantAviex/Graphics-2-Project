// D3D LAB 1a "Line Land".
// Author: L.Norri CD DRX, FullSail University

// Introduction:
// Welcome to the first lab of the Direct3D Graphics Programming class.
// This is the ONLY guided lab in this course! 
// Future labs will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

// TO BEGIN: Open the word document that acompanies this lab and cubet from the top.

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <iostream>
#include <ctime>
#include "XTime.h"
using namespace std;

// BEGIN PART 1
// TODO: PART 1 STEP 1a
#include <d3d11.h>
#include <d3d11shader.h>
// TODO: PART 1 STEP 1b
#pragma comment (lib, "d3d11.lib")

#include <DirectXMath.h>
#include <DirectXColors.h>
using namespace DirectX;
// TODO: PART 2 STEP 6
#include "Trivial_PS.csh"
#include "Trivial_VS.csh"
#include "SkyBox.csh"
#include "SkyBoxVS.csh"
#include "DDSTextureLoader.h"
#include <vector>
#include <fstream>
#include <istream>
#include <sstream>

#define BACKBUFFER_WIDTH	1280.0f
#define BACKBUFFER_HEIGHT	768.0f
//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

struct SIMPLE_VERTEX
{
	XMFLOAT3 pos;
	XMFLOAT3 uvw;
	XMFLOAT3 nrm;
};
struct VRAM_OBJECT
{
	XMFLOAT4X4 worldMatrix;
};
struct VRAM_SCENE
{
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
};
struct VRAM_AMBLIGHT
{
	XMFLOAT4 lightColor;
};
struct VRAM_POINTLIGHT
{
	XMFLOAT4 lightPos;
	XMFLOAT4 lightColor;
};
struct VRAM_DIRECTIONALLIGHT
{
	XMFLOAT4 dirLightColor;
	XMFLOAT4 dirLightDirection;
};
struct VRAM_SPOTLIGHT
{
	XMFLOAT4 spotLightPos;
	XMFLOAT4 spotLightColor;
	XMFLOAT4 spotLightDirection;
	XMFLOAT4 spotLightInnerConeRatio;
	XMFLOAT4 spotLightOuterConeRatio;
};
struct VRAM_SKYBOX
{
	XMFLOAT4 elapsedTime;
};
class DEMO_APP
{
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2

	IDXGISwapChain *swapchain;
	ID3D11Device *dev;
	ID3D11DeviceContext *devcon;
	ID3D11DeviceContext *deffCon;
	// TODO: PART 2 STEP 2
	ID3D11RenderTargetView *backbuffer;
	ID3D11Texture2D * zBuffer;
	ID3D11DepthStencilView *zStencil;
	ID3D11InputLayout *pLayout;
	ID3D11Buffer *pobjBuffer;
	ID3D11Buffer *pobjIndexBuffer;
	ID3D11Buffer *pSphereVertexBuffer;
	ID3D11Buffer *pSphereIndexBuffer;
	ID3D11Buffer *pPlaneVertexBuffer;
	ID3D11Buffer *pPlaneIndexBuffer;
	ID3D11ShaderResourceView * sphereResource;
	ID3D11VertexShader *pVS;
	ID3D11PixelShader *pPS;
	ID3D11SamplerState * pSampler;
	ID3D11CommandList* pCList;
	//ID3D11Buffer*      pVertexBuffer[2] = { NULL, NULL };
	// BEGIN PART 5
	// TODO: PART 5 STEP 1

	// TODO: PART 2 STEP 4
	XMFLOAT3 rot = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };

	// BEGIN PART 3
	// TODO: PART 3 STEP 1
	ID3D11Buffer *ConstBuffer;
	ID3D11Buffer *SceneBuffer;
	ID3D11Buffer *ConstSkyboxBuffer;
	ID3D11Buffer *ConstAmbLightBuffer;
	ID3D11Buffer *ConstDirectionalLightBuffer;
	//ID3D11Buffer *ConstSpotLightBuffer;
	XTime pXtime;

	// Obj Loader stuff




	ID3D11ShaderResourceView * planeResource;
	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;
	ID3D11PixelShader *skyBoxShader;
	ID3D11VertexShader *skyBoxVertShader;
	ID3D11ShaderResourceView * skyBoxResource;
	ID3D11RasterizerState * rState;
	ID3D11RasterizerState * rState2;

	VRAM_OBJECT sphere;
	VRAM_OBJECT plane;
	VRAM_OBJECT tunnel[4];
	VRAM_SCENE world;
	VRAM_SCENE world2;
	VRAM_AMBLIGHT ambLight;
	VRAM_DIRECTIONALLIGHT dirLight;
	VRAM_SKYBOX boxTimer;
public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1
	int vertCount;
	int IndCount;
	int sphereIndCount;
	std::vector<SIMPLE_VERTEX> skyBoxVerts;
	std::vector<int> skyBoxIndicies;
	std::vector<SIMPLE_VERTEX> objVerts[50];
	std::vector<int> objIndex[50];
	SIMPLE_VERTEX planeVertices[4];
	int planeIndicies[6];
	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool loadOBJ2(const char * path, std::vector<int>* objIndicies, std::vector<SIMPLE_VERTEX>* objVertices,
		ID3D11Buffer** meshVertBuff, ID3D11Buffer** meshIndexBuff);
	bool ShutDown();
	void CreateSphere(int LatLines, int LongLines, std::vector<int>* objIndicies, std::vector<SIMPLE_VERTEX>* objVertices, ID3D11Buffer** sphereVertBuff, ID3D11Buffer** sphereIndexBuff);
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

bool DEMO_APP::loadOBJ2(const char * path, std::vector<int>* objIndicies, std::vector<SIMPLE_VERTEX>* objVertices,
	ID3D11Buffer** meshVertBuff, ID3D11Buffer** meshIndexBuff)
{
	HRESULT hr = 0;

	std::wifstream fileIn(path);	//Open file
	std::wstring meshMatLib;					//String to hold our obj material library filename

	//Arrays to store our model's information
	std::vector<XMFLOAT3> vertPos;
	std::vector<XMFLOAT3> vertNorm;
	std::vector<XMFLOAT2> vertTexCoord;
	std::vector<std::wstring> meshMaterials;

	//Vertex definition indices
	std::vector<int> vertPosIndex;
	std::vector<int> vertNormIndex;
	std::vector<int> vertTCIndex;

	//Make sure we have a default if no tex coords or normals are defined
	bool hasTexCoord = false;
	bool hasNorm = false;

	//Temp variables to store into vectors
	int vertPosIndexTemp;
	int vertNormIndexTemp;
	int vertTCIndexTemp;
	wchar_t checkChar;		//The variable we will use to store one char from file at a time
	std::wstring face;		//Holds the string containing our face vertices
	int vIndex = 0;			//Keep track of our vertex index count
	int triangleCount = 0;	//Total Triangles
	int totalVerts = 0;
	int meshTriangles = 0;

	//Check to see if the file was opened
	if (fileIn)
	{
		while (fileIn)
		{
			checkChar = fileIn.get();	//Get next char

			switch (checkChar)
			{
			case 'v':	//Get Vertex Descriptions
				checkChar = fileIn.get();
				if (checkChar == ' ')	//v - vert position
				{
					float vz, vy, vx;
					fileIn >> vx >> vy >> vz;	//Store the next three types

					vertPos.push_back(XMFLOAT3(vx, vy, vz));
				}
				if (checkChar == 't')	//vt - vert tex coords
				{
					float vtcu, vtcv;
					fileIn >> vtcu >> vtcv;		//Store next two types

					vertTexCoord.push_back(XMFLOAT2(vtcu, vtcv));

					hasTexCoord = true;	//We know the model uses texture coords
				}
				if (checkChar == 'n')	//vn - vert normal
				{
					float vnx, vny, vnz;
					fileIn >> vnx >> vny >> vnz;	//Store next three types

					vertNorm.push_back(XMFLOAT3(vnx, vny, vnz));

					hasNorm = true;	//We know the model defines normals
				}
				break;


				//Get Face Index
			case 'f':	//f - defines the faces
				checkChar = fileIn.get();
				if (checkChar == ' ')
				{
					face = L"";
					std::wstring VertDef;	//Holds one vertex definition at a time
					triangleCount = 0;

					checkChar = fileIn.get();
					while (checkChar != '\n')
					{
						face += checkChar;			//Add the char to our face string
						checkChar = fileIn.get();	//Get the next Character
						if (checkChar == ' ')		//If its a space...
							triangleCount++;		//Increase our triangle count
					}

					//Check for space at the end of our face string
					if (face[face.length() - 1] == ' ')
						triangleCount--;	//Each space adds to our triangle count

					triangleCount -= 1;		//Ever vertex in the face AFTER the first two are new faces

					std::wstringstream ss(face);

					if (face.length() > 0)
					{
						int firstVIndex, lastVIndex;	//Holds the first and last vertice's index

						for (int i = 0; i < 3; ++i)		//First three vertices (first triangle)
						{
							ss >> VertDef;	//Get vertex definition (vPos/vTexCoord/vNorm)

							std::wstring vertPart;
							int whichPart = 0;		//(vPos, vTexCoord, or vNorm)

							//Parse this string
							for (unsigned int j = 0; j < VertDef.length(); ++j)
							{
								if (VertDef[j] != '/')	//If there is no divider "/", add a char to our vertPart
									vertPart += VertDef[j];

								//If the current char is a divider "/", or its the last character in the string
								if (VertDef[j] == '/' || j == VertDef.length() - 1)
								{
									std::wistringstream wstringToInt(vertPart);	//Used to convert wstring to int

									if (whichPart == 0)	//If vPos
									{
										wstringToInt >> vertPosIndexTemp;
										vertPosIndexTemp -= 1;		//subtract one since c++ arrays start with 0, and obj start with 1

										//Check to see if the vert pos was the only thing specified
										if (j == VertDef.length() - 1)
										{
											vertNormIndexTemp = 0;
											vertTCIndexTemp = 0;
										}
									}

									else if (whichPart == 1)	//If vTexCoord
									{
										if (vertPart != L"")	//Check to see if there even is a tex coord
										{
											wstringToInt >> vertTCIndexTemp;
											vertTCIndexTemp -= 1;	//subtract one since c++ arrays start with 0, and obj start with 1
										}
										else	//If there is no tex coord, make a default
											vertTCIndexTemp = 0;

										//If the cur. char is the second to last in the string, then
										//there must be no normal, so set a default normal
										if (j == VertDef.length() - 1)
											vertNormIndexTemp = 0;

									}
									else if (whichPart == 2)	//If vNorm
									{
										std::wistringstream wstringToInt(vertPart);

										wstringToInt >> vertNormIndexTemp;
										vertNormIndexTemp -= 1;		//subtract one since c++ arrays start with 0, and obj start with 1
									}

									vertPart = L"";	//Get ready for next vertex part
									whichPart++;	//Move on to next vertex part					
								}
							}
							bool vertAlreadyExists = false;
							if (totalVerts >= 3)
							{
								for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
								{
									if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
									{
										if (vertTCIndexTemp == vertTCIndex[iCheck])
										{
											objIndicies->push_back(iCheck);
											vertAlreadyExists = true;
										}
									}
								}
							}
							if (!vertAlreadyExists)
							{
								vertPosIndex.push_back(vertPosIndexTemp);
								vertTCIndex.push_back(vertTCIndexTemp);
								vertNormIndex.push_back(vertNormIndexTemp);
								totalVerts++;
								objIndicies->push_back(totalVerts - 1);
							}

							if (i == 0)
							{
								firstVIndex = objIndicies->at(vIndex);

							}

							//If this was the last vertex in the first triangle, we will make sure
							//the next triangle uses this one (eg. tri1(1,2,3) tri2(1,3,4) tri3(1,4,5))
							if (i == 2)
							{
								lastVIndex = objIndicies->at(vIndex);
							}
							vIndex++;
						}

						meshTriangles++;
						for (int l = 0; l < triangleCount - 1; ++l)
						{
							objIndicies->push_back(firstVIndex);
							vIndex++;

							objIndicies->push_back(lastVIndex);
							vIndex++;

							ss >> VertDef;

							std::wstring vertPart;
							int whichPart = 0;

							for (unsigned int j = 0; j < VertDef.length(); ++j)
							{
								if (VertDef[j] != '/')
									vertPart += VertDef[j];
								if (VertDef[j] == '/' || j == VertDef.length() - 1)
								{
									std::wistringstream wstringToInt(vertPart);

									if (whichPart == 0)
									{
										wstringToInt >> vertPosIndexTemp;
										vertPosIndexTemp -= 1;

										//Check to see if the vert pos was the only thing specified
										if (j == VertDef.length() - 1)
										{
											vertTCIndexTemp = 0;
											vertNormIndexTemp = 0;
										}
									}
									else if (whichPart == 1)
									{
										if (vertPart != L"")
										{
											wstringToInt >> vertTCIndexTemp;
											vertTCIndexTemp -= 1;
										}
										else
											vertTCIndexTemp = 0;
										if (j == VertDef.length() - 1)
											vertNormIndexTemp = 0;

									}
									else if (whichPart == 2)
									{
										std::wistringstream wstringToInt(vertPart);

										wstringToInt >> vertNormIndexTemp;
										vertNormIndexTemp -= 1;
									}

									vertPart = L"";
									whichPart++;
								}
							}

							//Check for duplicate vertices
							bool vertAlreadyExists = false;
							if (totalVerts >= 3)	//Make sure we at least have one triangle to check
							{
								for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
								{
									if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
									{
										if (vertTCIndexTemp == vertTCIndex[iCheck])
										{
											objIndicies->push_back(iCheck);			//Set index for this vertex
											vertAlreadyExists = true;		//If we've made it here, the vertex already exists
										}
									}
								}
							}

							if (!vertAlreadyExists)
							{
								vertPosIndex.push_back(vertPosIndexTemp);
								vertTCIndex.push_back(vertTCIndexTemp);
								vertNormIndex.push_back(vertNormIndexTemp);
								totalVerts++;					//New vertex created, add to total verts
								objIndicies->push_back(totalVerts - 1);		//Set index for this vertex
							}

							lastVIndex = objIndicies->at(vIndex);

							meshTriangles++;
							vIndex++;
						}
					}
				}
				break;

			}
		}

	}

	IndCount = meshTriangles * 3;
	vertCount = totalVerts;


	fileIn.close();
	fileIn.open(meshMatLib.c_str());

	SIMPLE_VERTEX tempVert;

	for (int j = 0; j < totalVerts; ++j)
	{
		int x, y, z;
		x = vertPosIndex[j];
		y = vertNormIndex[j];
		z = vertTCIndex[j];
		tempVert.pos = vertPos[vertPosIndex[j]];
		if (hasNorm)
			tempVert.nrm = vertNorm[vertNormIndex[j]];
		else
			tempVert.nrm = { 0.0f, 0.0f, 0.0f };
		if (hasTexCoord)
		{
			XMFLOAT3 temp;
			temp.x = vertTexCoord[vertTCIndex[j]].x;
			temp.y = 1 - vertTexCoord[vertTCIndex[j]].y;
			temp.z = 0;
			tempVert.uvw = temp;
		}
		else
		{
			float randW = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1))) - 0;
			tempVert.uvw = { 1.0f, 0.0f, randW };
		}

		objVertices->push_back(tempVert);
	}

#pragma region vertexBuffer
	//// TODO: PART 2 STEP 3a
	D3D11_BUFFER_DESC bufferDesc2;
	ZeroMemory(&bufferDesc2, sizeof(bufferDesc2));
	bufferDesc2.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc2.ByteWidth = sizeof(SIMPLE_VERTEX) * objVertices->size();
	bufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc2.CPUAccessFlags = NULL;

	D3D11_SUBRESOURCE_DATA objInitData;
	objInitData.pSysMem = &objVertices->at(0);
	objInitData.SysMemPitch = 0;
	objInitData.SysMemSlicePitch = 0;

	dev->CreateBuffer(&bufferDesc2, &objInitData, meshVertBuff);

#pragma endregion
#pragma region indexBuffer
	//// TODO: PART 2 STEP 3a
	D3D11_BUFFER_DESC iBufferDesc2;
	ZeroMemory(&iBufferDesc2, sizeof(iBufferDesc2));

	iBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	iBufferDesc2.ByteWidth = sizeof(int) * objIndicies->size();
	iBufferDesc2.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA iObjInitData;
	iObjInitData.pSysMem = &objIndicies->at(0);
	iObjInitData.SysMemPitch = 0;
	iObjInitData.SysMemSlicePitch = 0;

	dev->CreateBuffer(&iBufferDesc2, &iObjInitData, meshIndexBuff);

#pragma endregion
	return true;
}
XMFLOAT4 Color(int choice)
{
	switch (choice)
	{
	case 0:
		return{ 1.0f, 0.0f, 0.0f, 1.0f };
	case 1:
		return{ 0.0f, 1.0f, 0.0f, 1.0f };
	case 2:
		return{ 0.0f, 0.0f, 1.0f, 1.0f };
	case 3:
		return{ 1.0f, 1.0f, 0.0f, 1.0f };
	case 4:
		return{ 1.0f, 0.0f, 1.0f, 1.0f };
	case 5:
		return{ 0.0f, 1.0f, 1.0f, 1.0f };
	default:
		return{ 1.0f, 1.0f, 1.0f, 1.0f };
	}
}
DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	srand((unsigned int)pXtime.TotalTime());
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, (UINT)BACKBUFFER_WIDTH, (UINT)BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"Lab 4", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//
#pragma region plane
	planeVertices[0].pos = { -225.0f, 0.0f, 75.0f };
	planeVertices[0].uvw = { 0.0f, 0.0f, 0.0f };
	planeVertices[1].pos = { 225.0f, 0.0f, 75.0f };
	planeVertices[1].uvw = { 6.0f, 0.0f, 0.0f };
	planeVertices[2].pos = { -225.0f, 0.0f, -75.0f };
	planeVertices[2].uvw = { 0.0f, 1.0f, 0.0f };
	planeVertices[3].pos = { 225.0f, 0.0f, -75.0f };
	planeVertices[3].uvw = { 6.0f, 1.0f, 0.0f };
	planeVertices[0].nrm = { 0.0f, 1.0f, 0.0f };
	planeVertices[1].nrm = { 0.0f, 1.0f, 0.0f };
	planeVertices[2].nrm = { 0.0f, 1.0f, 0.0f };
	planeVertices[3].nrm = { 0.0f, 1.0f, 0.0f };

	planeIndicies[0] = 0;
	planeIndicies[1] = 1;
	planeIndicies[2] = 2;
	planeIndicies[3] = 2;
	planeIndicies[4] = 1;
	planeIndicies[5] = 3;
#pragma endregion

#pragma region UVs
	ambLight.lightColor = { 0.5f, 0.5f, 0.5f, 1 };

	dirLight.dirLightColor = { 1, 1, 1, 1 };
	dirLight.dirLightDirection = { 0, -1, 0, 0 };

#pragma endregion

#pragma region CreateSwapChain
	DXGI_SWAP_CHAIN_DESC scd;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = window;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	// TODO: PART 1 STEP 3a
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION,
		&scd, &swapchain, &dev, NULL, &devcon);
	dev->CreateDeferredContext(0, &deffCon);
#pragma endregion
	// TODO: PART 1 STEP 3b
#pragma region bufferStuff
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// TODO: PART 1 STEP 4
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();
	// TODO: PART 1 STEP 5
#pragma endregion

	loadOBJ2("Assets/Models/Tunnel.obj", &objIndex[0], &objVerts[0], &pSphereVertexBuffer, &pSphereIndexBuffer);
#pragma region zBuffer
	D3D11_TEXTURE2D_DESC zBuffDesc;

	ZeroMemory(&zBuffDesc, sizeof(zBuffDesc));
	zBuffDesc.Width = (UINT)BACKBUFFER_WIDTH;
	zBuffDesc.Height = (UINT)BACKBUFFER_HEIGHT;
	zBuffDesc.MipLevels = 1;
	zBuffDesc.ArraySize = 1;
	zBuffDesc.Format = DXGI_FORMAT_D32_FLOAT;
	zBuffDesc.SampleDesc.Count = 1;
	zBuffDesc.SampleDesc.Quality = 0;
	zBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	zBuffDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	zBuffDesc.CPUAccessFlags = 0;
	zBuffDesc.MiscFlags = 0;
	dev->CreateTexture2D(&zBuffDesc, NULL, &zBuffer);
#pragma endregion
#pragma region zStencil
	D3D11_DEPTH_STENCIL_VIEW_DESC zDesc;

	ZeroMemory(&zDesc, sizeof(zDesc));

	zDesc.Format = DXGI_FORMAT_D32_FLOAT;
	zDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	zDesc.Texture2D.MipSlice = 0;
	dev->CreateDepthStencilView(zBuffer, &zDesc, &zStencil);
#pragma endregion

#pragma region ViewPort
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = BACKBUFFER_WIDTH;
	viewport.Height = BACKBUFFER_HEIGHT;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	deffCon->RSSetViewports(1, &viewport);

#pragma endregion


#pragma region vertexBuffer
	// TODO: PART 2 STEP 3a
	D3D11_BUFFER_DESC bufferDesc2;
	ZeroMemory(&bufferDesc2, sizeof(bufferDesc2));
	bufferDesc2.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc2.ByteWidth = sizeof(planeVertices);
	bufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc2.CPUAccessFlags = NULL;

	D3D11_SUBRESOURCE_DATA InitData2;
	InitData2.pSysMem = planeVertices;
	InitData2.SysMemPitch = 0;
	InitData2.SysMemSlicePitch = 0;

	dev->CreateBuffer(&bufferDesc2, &InitData2, &pPlaneVertexBuffer);

#pragma endregion
#pragma region indexBuffer
	// TODO: PART 2 STEP 3a
	D3D11_BUFFER_DESC iBufferDesc2;
	ZeroMemory(&iBufferDesc2, sizeof(iBufferDesc2));

	iBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	iBufferDesc2.ByteWidth = sizeof(planeIndicies);
	iBufferDesc2.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA iInitData2;
	iInitData2.pSysMem = planeIndicies;
	iInitData2.SysMemPitch = 0;
	iInitData2.SysMemSlicePitch = 0;

	dev->CreateBuffer(&iBufferDesc2, &iInitData2, &pPlaneIndexBuffer);

#pragma endregion

#pragma region ShaderStuff

	dev->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &pVS);
	dev->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &pPS);
	dev->CreatePixelShader(SkyBox, sizeof(SkyBox), NULL, &skyBoxShader);
	dev->CreateVertexShader(SkyBoxVS, sizeof(SkyBoxVS), NULL, &skyBoxVertShader);
	// set the shader objects
#pragma endregion
#pragma region inputLayout
	D3D11_INPUT_ELEMENT_DESC vLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVW", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	dev->CreateInputLayout(vLayout, _countof(vLayout), Trivial_VS, sizeof(Trivial_VS), &pLayout);

#pragma endregion

	D3D11_BUFFER_DESC constBufferDesc;
	ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.ByteWidth = sizeof(VRAM_OBJECT);
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dev->CreateBuffer(&constBufferDesc, NULL, &ConstBuffer);

	D3D11_BUFFER_DESC sceneBufferDesc;
	ZeroMemory(&sceneBufferDesc, sizeof(sceneBufferDesc));
	sceneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	sceneBufferDesc.ByteWidth = sizeof(VRAM_SCENE);
	sceneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	sceneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dev->CreateBuffer(&sceneBufferDesc, NULL, &SceneBuffer);

	D3D11_BUFFER_DESC skyboxBufferDesc;
	ZeroMemory(&skyboxBufferDesc, sizeof(skyboxBufferDesc));
	skyboxBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	skyboxBufferDesc.ByteWidth = sizeof(VRAM_SKYBOX);
	skyboxBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	skyboxBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dev->CreateBuffer(&skyboxBufferDesc, NULL, &ConstSkyboxBuffer);

	D3D11_BUFFER_DESC ambLightBufferDesc;
	ZeroMemory(&ambLightBufferDesc, sizeof(ambLightBufferDesc));
	ambLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ambLightBufferDesc.ByteWidth = sizeof(VRAM_AMBLIGHT);
	ambLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ambLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dev->CreateBuffer(&ambLightBufferDesc, NULL, &ConstAmbLightBuffer);
	D3D11_BUFFER_DESC directionalLightBufferDesc;
	ZeroMemory(&directionalLightBufferDesc, sizeof(directionalLightBufferDesc));
	directionalLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	directionalLightBufferDesc.ByteWidth = sizeof(VRAM_DIRECTIONALLIGHT);
	directionalLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	directionalLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dev->CreateBuffer(&directionalLightBufferDesc, NULL, &ConstDirectionalLightBuffer);
	//D3D11_BUFFER_DESC spotLightBufferDesc;
	//ZeroMemory(&spotLightBufferDesc, sizeof(spotLightBufferDesc));
	//spotLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//spotLightBufferDesc.ByteWidth = sizeof(VRAM_SPOTLIGHT);
	//spotLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//spotLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//dev->CreateBuffer(&spotLightBufferDesc, NULL, &ConstSpotLightBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
	ZeroMemory(&shaderDesc, sizeof(shaderDesc));
	shaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderDesc.Texture2D = { 0, 1 };

	CreateDDSTextureFromFile(dev, L"Assets/Textures/BackGroundCube1.dds", NULL, &skyBoxResource);
	CreateDDSTextureFromFile(dev, L"Assets/Textures/Tunnel.dds", NULL, &sphereResource);
	CreateDDSTextureFromFile(dev, L"Assets/Textures/highway.dds", NULL, &planeResource);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	dev->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	CD3D11_RASTERIZER_DESC rDesc(D3D11_DEFAULT);
	rDesc.FillMode = D3D11_FILL_SOLID;

	dev->CreateRasterizerState(&rDesc, &rState);

	rDesc.CullMode = D3D11_CULL_NONE;

	dev->CreateRasterizerState(&rDesc, &rState2);

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	dev->CreateSamplerState(&samplerDesc, &pSampler);
	boxTimer.elapsedTime.x = 0.0f;
	XMStoreFloat4x4(&world.viewMatrix,   XMMatrixTranslation(0.0f, -9.0f, 65.0f));
	XMStoreFloat4x4(&world2.viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&plane.worldMatrix, XMMatrixIdentity());
	XMMATRIX charMatrix = XMMatrixScaling(0.95f,0.95f,0.95f);
	charMatrix = charMatrix * XMMatrixRotationY(XMConvertToRadians(90));
	XMStoreFloat4x4(&tunnel[0].worldMatrix, charMatrix * XMMatrixTranslation(225.0f, -0.5f, 3.5f));
	XMStoreFloat4x4(&tunnel[1].worldMatrix, charMatrix * XMMatrixTranslation(-225.0f, -0.5f, 3.5f));
	//XMStoreFloat4x4(&sphere.worldMatrix, charMatrix);
	XMStoreFloat4x4(&world2.projectionMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(75), BACKBUFFER_WIDTH / BACKBUFFER_HEIGHT, 1.0f, 1000));
	XMStoreFloat4x4(&world.projectionMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(75), BACKBUFFER_WIDTH / BACKBUFFER_HEIGHT, 1.0f, 1000));
	CreateSphere(100, 100, &skyBoxIndicies, &skyBoxVerts, &pobjBuffer, &pobjIndexBuffer);
 	//CreateSphere(100, 100,&objIndex[0],&objVerts[0],&pSphereVertexBuffer, &pSphereIndexBuffer);
}


bool DEMO_APP::Run()
{
	UINT strides = sizeof(SIMPLE_VERTEX);
	UINT offset = 0;

	if (GetAsyncKeyState('Q') || GetAsyncKeyState('E') || GetAsyncKeyState('W') || GetAsyncKeyState('S') || GetAsyncKeyState('A') || GetAsyncKeyState('D'))
	{
		if (GetAsyncKeyState('Q'))
		{
			XMMATRIX worldWatever3 = XMLoadFloat4x4(&world.viewMatrix);
			XMMATRIX InvWorld = XMMatrixInverse(NULL, worldWatever3);
			XMMATRIX translationY = XMMatrixTranslation(0, 25.0f * (float)pXtime.Delta(), 0);
			InvWorld = InvWorld * translationY;
			worldWatever3 = XMMatrixInverse(NULL, InvWorld);
			XMStoreFloat4x4(&world.viewMatrix, worldWatever3);

		}
		else if (GetAsyncKeyState('E'))
		{
			XMMATRIX worldWatever3 = XMLoadFloat4x4(&world.viewMatrix);
			XMMATRIX InvWorld = XMMatrixInverse(NULL, worldWatever3);
			XMMATRIX translationY = XMMatrixTranslation(0, -25.0f * (float)pXtime.Delta(), 0);
			InvWorld = InvWorld * translationY;
			worldWatever3 = XMMatrixInverse(NULL, InvWorld);
			XMStoreFloat4x4(&world.viewMatrix, worldWatever3);
		}
		if (GetAsyncKeyState('W'))
		{
				XMMATRIX worldWatever3 = XMLoadFloat4x4(&world.viewMatrix);
				XMMATRIX InvWorld = XMMatrixInverse(NULL, worldWatever3);
				if (InvWorld.r[3].m128_f32[0] >= -225.0f && InvWorld.r[3].m128_f32[0] <= 225.0f && InvWorld.r[3].m128_f32[2] >= -75.0f && InvWorld.r[3].m128_f32[2] <= 75.0f)
				{
					XMMATRIX transXZ = XMMatrixTranslation(0, 0, 50.0f * (float)pXtime.Delta());
					XMFLOAT3 transRot;
					XMStoreFloat3(&transRot, InvWorld.r[2]);
					transRot.y = 0;
					XMMATRIX translationXZ = InvWorld;
					XMVECTOR posR;
					posR = XMLoadFloat3(&transRot);
					translationXZ.r[2] = posR;
					translationXZ = transXZ * translationXZ;
					InvWorld.r[3] = translationXZ.r[3];
					if (InvWorld.r[3].m128_f32[0] < -225.0f)
						InvWorld.r[3].m128_f32[0] = -224.8f;
					if (InvWorld.r[3].m128_f32[0] > 225.0f)
						InvWorld.r[3].m128_f32[0] = 224.8f;
					if (InvWorld.r[3].m128_f32[2] < -75.0f)
						InvWorld.r[3].m128_f32[2] = -74.8f;
					if (InvWorld.r[3].m128_f32[2] > 75.0f)
						InvWorld.r[3].m128_f32[2] = 74.8f;
					worldWatever3 = XMMatrixInverse(NULL, InvWorld);
					XMStoreFloat4x4(&world.viewMatrix, worldWatever3);
				}
		}
		else if (GetAsyncKeyState('S'))
		{
				XMMATRIX worldWatever3 = XMLoadFloat4x4(&world.viewMatrix);
				XMMATRIX InvWorld = XMMatrixInverse(NULL, worldWatever3);
				if (InvWorld.r[3].m128_f32[0] >= -225.0f && InvWorld.r[3].m128_f32[0] <= 225.0f && InvWorld.r[3].m128_f32[2] >= -75.0f && InvWorld.r[3].m128_f32[2] <= 75.0f)
				{
					XMMATRIX transXZ = XMMatrixTranslation(0, 0, -50.0f * (float)pXtime.Delta());
					XMFLOAT3 transRot;
					XMStoreFloat3(&transRot, InvWorld.r[2]);
					transRot.y = 0;
					XMMATRIX translationXZ = InvWorld;
					XMVECTOR posR;
					posR = XMLoadFloat3(&transRot);
					translationXZ.r[2] = posR;
					translationXZ = transXZ * translationXZ;
					InvWorld.r[3] = translationXZ.r[3];
					if (InvWorld.r[3].m128_f32[0] < -225.0f)
						InvWorld.r[3].m128_f32[0] = -224.8f;
					if (InvWorld.r[3].m128_f32[0] > 225.0f)
						InvWorld.r[3].m128_f32[0] = 224.8f;
					if (InvWorld.r[3].m128_f32[2] < -75.0f)
						InvWorld.r[3].m128_f32[2] = -74.8f;
					if (InvWorld.r[3].m128_f32[2] > 75.0f)
						InvWorld.r[3].m128_f32[2] = 74.8f;
					worldWatever3 = XMMatrixInverse(NULL, InvWorld);
					XMStoreFloat4x4(&world.viewMatrix, worldWatever3);
				}
		}
		if (GetAsyncKeyState('A'))
		{
				XMMATRIX worldWatever3 = XMLoadFloat4x4(&world.viewMatrix);
				XMMATRIX InvWorld = XMMatrixInverse(NULL, worldWatever3);
				if (InvWorld.r[3].m128_f32[0] >= -225.0f && InvWorld.r[3].m128_f32[0] <= 225.0f && InvWorld.r[3].m128_f32[2] >= -75.0f && InvWorld.r[3].m128_f32[2] <= 75.0f)
				{
					XMMATRIX transXZ = XMMatrixTranslation(-50.0f * (float)pXtime.Delta(), 0, 0);
					XMFLOAT3 transRot;
					XMStoreFloat3(&transRot, InvWorld.r[2]);
					transRot.y = 0;
					XMMATRIX translationXZ = InvWorld;
					XMVECTOR posR;
					posR = XMLoadFloat3(&transRot);
					translationXZ.r[2] = posR;
					translationXZ = transXZ * translationXZ;
					InvWorld.r[3] = translationXZ.r[3];
					if (InvWorld.r[3].m128_f32[0] < -225.0f)
						InvWorld.r[3].m128_f32[0] = -224.8f;
					if (InvWorld.r[3].m128_f32[0] > 225.0f)
						InvWorld.r[3].m128_f32[0] = 224.8f;
					if (InvWorld.r[3].m128_f32[2] < -75.0f)
						InvWorld.r[3].m128_f32[2] = -74.8f;
					if (InvWorld.r[3].m128_f32[2] > 75.0f)
						InvWorld.r[3].m128_f32[2] = 74.8f;
					worldWatever3 = XMMatrixInverse(NULL, InvWorld);
					XMStoreFloat4x4(&world.viewMatrix, worldWatever3);
				}
		}
		else if (GetAsyncKeyState('D'))
		{
				XMMATRIX worldWatever3 = XMLoadFloat4x4(&world.viewMatrix);
				XMMATRIX InvWorld = XMMatrixInverse(NULL, worldWatever3);
				if (InvWorld.r[3].m128_f32[0] >= -225.0f && InvWorld.r[3].m128_f32[0] <= 225.0f && InvWorld.r[3].m128_f32[2] >= -75.0f && InvWorld.r[3].m128_f32[2] <= 75.0f)
				{
					XMMATRIX transXZ = XMMatrixTranslation(50.0f * (float)pXtime.Delta(), 0, 0);
					XMFLOAT3 transRot;
					XMStoreFloat3(&transRot, InvWorld.r[2]);
					transRot.y = 0;
					XMMATRIX translationXZ = InvWorld;
					XMVECTOR posR;
					posR = XMLoadFloat3(&transRot);
					translationXZ.r[2] = posR;
					translationXZ = transXZ * translationXZ;
					InvWorld.r[3] = translationXZ.r[3];
					if (InvWorld.r[3].m128_f32[0] < -225.0f)
						InvWorld.r[3].m128_f32[0] = -224.8f;
					if (InvWorld.r[3].m128_f32[0] > 225.0f)
						InvWorld.r[3].m128_f32[0] = 224.8f;
					if (InvWorld.r[3].m128_f32[2] < -75.0f)
						InvWorld.r[3].m128_f32[2] = -74.8f;
					if (InvWorld.r[3].m128_f32[2] > 75.0f)
						InvWorld.r[3].m128_f32[2] = 74.8f;
					worldWatever3 = XMMatrixInverse(NULL, InvWorld);
					XMStoreFloat4x4(&world.viewMatrix, worldWatever3);
				}
		}
	}
	if (GetAsyncKeyState(VK_UP))
	{
		if (rot.y > -90.0f)
		{
			XMMATRIX worldWatever = XMLoadFloat4x4(&world.viewMatrix);
			XMMATRIX worldInv;
			worldInv = XMMatrixInverse(NULL, worldWatever);
			XMMATRIX rotationY = XMMatrixRotationX(XMConvertToRadians(-90.0f * (float)pXtime.Delta()));
			worldInv = rotationY * worldInv;
			worldWatever = XMMatrixInverse(NULL, worldInv);
			XMStoreFloat4x4(&world.viewMatrix, worldWatever);

			XMMATRIX worldWatever2 = XMLoadFloat4x4(&world2.viewMatrix);
			XMMATRIX worldInv2;
			worldInv2 = XMMatrixInverse(NULL, worldWatever2);
			XMMATRIX rotationY2 = XMMatrixRotationX(XMConvertToRadians(-90.0f * (float)pXtime.Delta()));
			worldInv2 = rotationY2 * worldInv2;
			worldWatever2 = XMMatrixInverse(NULL, worldInv2);
			XMStoreFloat4x4(&world2.viewMatrix, worldWatever2);

			rot.y += -90.0f * (float)pXtime.Delta();
		}
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		if (rot.y < 90.0f)
		{
			XMMATRIX worldWatever = XMLoadFloat4x4(&world.viewMatrix);
			XMMATRIX worldInv;
			worldInv = XMMatrixInverse(NULL, worldWatever);
			XMMATRIX rotationY = XMMatrixRotationX(XMConvertToRadians(90.0f * (float)pXtime.Delta()));
			worldInv = rotationY * worldInv;
			worldWatever = XMMatrixInverse(NULL, worldInv);
			XMStoreFloat4x4(&world.viewMatrix, worldWatever);

			XMMATRIX worldWatever2 = XMLoadFloat4x4(&world2.viewMatrix);
			XMMATRIX worldInv2;
			worldInv2 = XMMatrixInverse(NULL, worldWatever2);
			XMMATRIX rotationY2 = XMMatrixRotationX(XMConvertToRadians(90.0f * (float)pXtime.Delta()));
			worldInv2 = rotationY2 * worldInv2;
			worldWatever2 = XMMatrixInverse(NULL, worldInv2);
			XMStoreFloat4x4(&world2.viewMatrix, worldWatever2);

			rot.y += 90.0f * (float)pXtime.Delta();

		}
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		XMMATRIX worldWatever = XMLoadFloat4x4(&world.viewMatrix);
		XMMATRIX worldInv;
		worldInv = XMMatrixInverse(NULL, worldWatever);
		XMMATRIX rotationX = XMMatrixRotationY(XMConvertToRadians(-90.0f * (float)pXtime.Delta()));
		XMFLOAT3 transRot = { 0, 0, 0 };
		XMMATRIX rotY = worldInv;
		XMVECTOR posR = worldInv.r[3];
		rotY.r[3] = XMLoadFloat3(&transRot);
		rotY = rotY * rotationX;
		rotY.r[3] = posR;
		worldInv = rotY;
		worldWatever = XMMatrixInverse(NULL, worldInv);
		XMStoreFloat4x4(&world.viewMatrix, worldWatever);

		XMMATRIX worldWatever2 = XMLoadFloat4x4(&world2.viewMatrix);
		XMMATRIX worldInv2;
		worldInv2 = XMMatrixInverse(NULL, worldWatever2);
		XMMATRIX rotationX2 = XMMatrixRotationY(XMConvertToRadians(-90.0f * (float)pXtime.Delta()));
		XMMATRIX rotY2 = worldInv2;
		XMVECTOR posR2 = worldInv2.r[3];
		rotY2.r[3] = XMLoadFloat3(&transRot);
		rotY2 = rotY2 * rotationX;
		rotY2.r[3] = posR2;
		worldInv2 = rotY2;
		worldWatever2 = XMMatrixInverse(NULL, worldInv2);
		XMStoreFloat4x4(&world2.viewMatrix, worldWatever2);

		rot.x += -90.0f * (float)pXtime.Delta();
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		XMMATRIX worldWatever = XMLoadFloat4x4(&world.viewMatrix);
		XMMATRIX worldInv;
		worldInv = XMMatrixInverse(NULL, worldWatever);
		XMMATRIX rotationX = XMMatrixRotationY(XMConvertToRadians(90.0f * (float)pXtime.Delta()));
		XMFLOAT3 transRot = { 0, 0, 0 };
		XMMATRIX rotY = worldInv;
		XMVECTOR posR = worldInv.r[3];
		rotY.r[3] = XMLoadFloat3(&transRot);
		rotY = rotY * rotationX;
		rotY.r[3] = posR;
		worldInv = rotY;
		worldWatever = XMMatrixInverse(NULL, worldInv);
		XMStoreFloat4x4(&world.viewMatrix, worldWatever);

		XMMATRIX worldWatever2 = XMLoadFloat4x4(&world2.viewMatrix);
		XMMATRIX worldInv2;
		worldInv2 = XMMatrixInverse(NULL, worldWatever2);
		XMMATRIX rotationX2 = XMMatrixRotationY(XMConvertToRadians(90.0f * (float)pXtime.Delta()));
		XMMATRIX rotY2 = worldInv2;
		XMVECTOR posR2 = worldInv2.r[3];
		rotY2.r[3] = XMLoadFloat3(&transRot);
		rotY2 = rotY2 * rotationX;
		rotY2.r[3] = posR2;
		worldInv2 = rotY2;
		worldWatever2 = XMMatrixInverse(NULL, worldInv2);
		XMStoreFloat4x4(&world2.viewMatrix, worldWatever2);

		rot.x += 90.0f * (float)pXtime.Delta();
	}
	pXtime.Signal();
	boxTimer.elapsedTime.x += pXtime.Delta();
	//cubeVertices.rotationMatrix = XMMatrixRotationY(XMConvertToRadians(57.3) * pXtime.Delta()) * cubeMatrics.rotationMatrix;

	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	deffCon->ClearRenderTargetView(backbuffer, color);
	deffCon->ClearDepthStencilView(zStencil, D3D11_CLEAR_DEPTH, 1, 0);

	///////////////////////////////////////////////////////////////////////
	deffCon->IASetInputLayout(pLayout);
	deffCon->OMSetRenderTargets(1, &backbuffer, zStencil);
	deffCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deffCon->PSSetSamplers(0, 1, &pSampler);

#pragma region Skybox Rendering
	deffCon->VSSetShader(skyBoxVertShader, NULL, NULL);
	deffCon->PSSetShader(skyBoxShader, NULL, NULL);
	deffCon->PSSetShaderResources(0, 1, &skyBoxResource);
	deffCon->VSSetConstantBuffers(0, 1, &ConstBuffer);
	deffCon->VSSetConstantBuffers(1, 1, &SceneBuffer);
	deffCon->PSSetConstantBuffers(0, 1, &ConstSkyboxBuffer);
	D3D11_MAPPED_SUBRESOURCE resource2;
	memset(&resource2, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
	deffCon->Map(SceneBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &resource2);
	memcpy_s(resource2.pData, sizeof(world2), &world2, sizeof(world2));
	deffCon->Unmap(SceneBuffer, NULL);
	D3D11_MAPPED_SUBRESOURCE resourceLight;
	memset(&resourceLight, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
	deffCon->Map(ConstAmbLightBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &resourceLight);
	memcpy_s(resourceLight.pData, sizeof(ambLight), &ambLight, sizeof(ambLight));
	deffCon->Unmap(ConstAmbLightBuffer, NULL);
	D3D11_MAPPED_SUBRESOURCE resourceDirLight;
	memset(&resourceDirLight, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
	deffCon->Map(ConstDirectionalLightBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &resourceDirLight);
	memcpy_s(resourceDirLight.pData, sizeof(dirLight), &dirLight, sizeof(dirLight));
	deffCon->Unmap(ConstDirectionalLightBuffer, NULL);

	D3D11_MAPPED_SUBRESOURCE resource6;
	memset(&resource6, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
	deffCon->Map(ConstSkyboxBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &resource6);
	memcpy_s(resource6.pData, sizeof(boxTimer), &boxTimer, sizeof(boxTimer));
	deffCon->Unmap(ConstSkyboxBuffer, NULL);


	deffCon->OMSetDepthStencilState(DSLessEqual, 0);
	deffCon->RSSetState(rState2);
	deffCon->IASetVertexBuffers(0, 1, &pobjBuffer, &strides, &offset);
	deffCon->IASetIndexBuffer(pobjIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deffCon->DrawIndexed(skyBoxIndicies.size(), 0, 0);
#pragma endregion

	deffCon->RSSetState(rState);
	deffCon->OMSetDepthStencilState(NULL, 0);
	deffCon->VSSetShader(pVS, NULL, NULL);
	deffCon->PSSetShader(pPS, NULL, NULL);
	ID3D11ShaderResourceView* junkShader = NULL;
	deffCon->PSSetShaderResources(0, 1, &sphereResource);
	deffCon->VSSetConstantBuffers(0, 1, &ConstBuffer);
	deffCon->VSSetConstantBuffers(1, 1, &SceneBuffer);
	deffCon->PSSetConstantBuffers(0, 1, &ConstAmbLightBuffer);
	deffCon->PSSetConstantBuffers(1, 1, &ConstDirectionalLightBuffer);
	D3D11_MAPPED_SUBRESOURCE resourceWorld;
	memset(&resourceWorld, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
	deffCon->Map(SceneBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &resourceWorld);
	memcpy_s(resourceWorld.pData, sizeof(world), &world, sizeof(world));
	deffCon->Unmap(SceneBuffer, NULL);
	for (size_t i = 0; i < 2; i++)
	{
		D3D11_MAPPED_SUBRESOURCE resourceobj;
		memset(&resourceobj, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deffCon->Map(ConstBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &resourceobj);
		memcpy_s(resourceobj.pData, sizeof(tunnel[i]), &tunnel[i], sizeof(tunnel[i]));
		deffCon->Unmap(ConstBuffer, NULL);
	
	
		deffCon->IASetVertexBuffers(0, 1, &pSphereVertexBuffer, &strides, &offset);
		deffCon->IASetIndexBuffer(pSphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deffCon->DrawIndexed(objIndex[0].size(), 0, 0);
	}

	//D3D11_MAPPED_SUBRESOURCE resourceobj;
	//memset(&resourceobj, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//deffCon->Map(ConstBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &resourceobj);
	//memcpy_s(resourceobj.pData, sizeof(sphere), &sphere, sizeof(sphere));
	//deffCon->Unmap(ConstBuffer, NULL);
	//
	//
	//deffCon->IASetVertexBuffers(0, 1, &pSphereVertexBuffer, &strides, &offset);
	//deffCon->IASetIndexBuffer(pSphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//deffCon->DrawIndexed(objIndex[0].size(), 0, 0);

	deffCon->PSSetShaderResources(0, 1, &planeResource);
	D3D11_MAPPED_SUBRESOURCE resource4;
	memset(&resource4, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
	deffCon->Map(ConstBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &resource4);
	memcpy_s(resource4.pData, sizeof(plane), &plane, sizeof(plane));
	deffCon->Unmap(ConstBuffer, NULL);
	deffCon->IASetVertexBuffers(0, 1, &pPlaneVertexBuffer, &strides, &offset);
	deffCon->IASetIndexBuffer(pPlaneIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deffCon->DrawIndexed(6, 0, 0);


	
	deffCon->FinishCommandList(true, &pCList);
	devcon->ExecuteCommandList(pCList, true);

	pCList->Release();
	// TODO: PART 1 STEP 8
	swapchain->Present(0, 0);

	// END OF PART 1
	return true;
}

bool DEMO_APP::ShutDown()
{
	backbuffer->Release();
	zBuffer->Release();
	zStencil->Release();
	pLayout->Release();
	pVS->Release();
	pPS->Release();
	rState->Release();
	rState2->Release();
	pobjBuffer->Release();
	pobjIndexBuffer->Release();
	pSampler->Release();
	pCList->Release();
	DSLessEqual->Release();
	skyBoxShader->Release();
	skyBoxVertShader->Release();
	skyBoxResource->Release();
	sphereResource->Release();
	pPlaneIndexBuffer->Release();
	pPlaneVertexBuffer->Release();
	planeResource->Release();
	pSphereIndexBuffer->Release();
	pSphereVertexBuffer->Release();
	ConstBuffer->Release();
	SceneBuffer->Release();
	ConstSkyboxBuffer->Release();
	ConstAmbLightBuffer->Release();
	ConstDirectionalLightBuffer->Release();

	deffCon->Release();
	devcon->Release();
	dev->Release();
	swapchain->Release();
	UnregisterClass(L"DirectXApplication", application);
	return true;
}
void DEMO_APP::CreateSphere(int LatLines, int LongLines, std::vector<int>* sphereIndicies, std::vector<SIMPLE_VERTEX>* sphereVertices, ID3D11Buffer** sphereVertBuff, ID3D11Buffer** sphereIndexBuff)
{
	int NumSphereVertices = ((LatLines - 2) * LongLines) + 2;
	int NumSphereFaces = ((LatLines - 3)*(LongLines)* 2) + (LongLines * 2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	sphereVertices->resize(NumSphereVertices);

	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	sphereVertices->at(0).pos.x = 0.0f;
	sphereVertices->at(0).pos.y = 0.0f;
	sphereVertices->at(0).pos.z = 1.0f;

	for (int i = 0; i < LatLines - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14f / (LatLines - 1));
		XMMATRIX Rotationx = XMMatrixRotationX(spherePitch);
		for (int j = 0; j < LongLines; ++j)
		{
			sphereYaw = j * (6.28f / (LongLines));
			XMMATRIX Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			currVertPos = XMVector3Normalize(currVertPos);
			sphereVertices->at(i*LongLines + j + 1).pos.x = XMVectorGetX(currVertPos);
			sphereVertices->at(i*LongLines + j + 1).pos.y = XMVectorGetY(currVertPos);
			sphereVertices->at(i*LongLines + j + 1).pos.z = XMVectorGetZ(currVertPos);
		}
	}

	sphereVertices->at(NumSphereVertices - 1).pos.x = 0.0f;
	sphereVertices->at(NumSphereVertices - 1).pos.y = 0.0f;
	sphereVertices->at(NumSphereVertices - 1).pos.z = -1.0f;


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &sphereVertices->at(0); 
	dev->CreateBuffer(&vertexBufferDesc, &vertexBufferData, sphereVertBuff);


	sphereIndicies->resize(NumSphereFaces * 3);

	int k = 0;
	for (int l = 0; l < LongLines - 1; ++l)
	{
		sphereIndicies->at(k) = 0;
		sphereIndicies->at(k + 1) = l + 1;
		sphereIndicies->at(k + 2) = l + 2;
		k += 3;
	}

	sphereIndicies->at(k) = 0;
	sphereIndicies->at(k + 1) = LongLines;
	sphereIndicies->at(k + 2) = 1;
	k += 3;

	for (int i = 0; i < LatLines - 3; ++i)
	{
		for (int j = 0; j < LongLines - 1; ++j)
		{
			sphereIndicies->at(k) = i*LongLines + j + 1;
			sphereIndicies->at(k + 1) = i*LongLines + j + 2;
			sphereIndicies->at(k + 2) = (i + 1)*LongLines + j + 1;

			sphereIndicies->at(k + 3) = (i + 1)*LongLines + j + 1;
			sphereIndicies->at(k + 4) = i*LongLines + j + 2;
			sphereIndicies->at(k + 5) = (i + 1)*LongLines + j + 2;

			k += 6; // next quad
		}

		sphereIndicies->at(k) = (i*LongLines) + LongLines;
		sphereIndicies->at(k + 1) = (i*LongLines) + 1;
		sphereIndicies->at(k + 2) = ((i + 1)*LongLines) + LongLines;

		sphereIndicies->at(k + 3) = ((i + 1)*LongLines) + LongLines;
		sphereIndicies->at(k + 4) = (i*LongLines) + 1;
		sphereIndicies->at(k + 5) = ((i + 1)*LongLines) + 1;

		k += 6;
	}

	for (int l = 0; l < LongLines - 1; ++l)
	{
		sphereIndicies->at(k) = NumSphereVertices - 1;
		sphereIndicies->at(k + 1) = (NumSphereVertices - 1) - (l + 1);
		sphereIndicies->at(k + 2) = (NumSphereVertices - 1) - (l + 2);
		k += 3;
	}

	sphereIndicies->at(k) = NumSphereVertices - 1;
	sphereIndicies->at(k + 1) = (NumSphereVertices - 1) - LongLines;
	sphereIndicies->at(k + 2) = NumSphereVertices - 2;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(int) * NumSphereFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	sphereIndCount = NumSphereFaces * 3;
	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &sphereIndicies->at(0);
	dev->CreateBuffer(&indexBufferDesc, &iinitData, sphereIndexBuff);

}
//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && myApp.Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	myApp.ShutDown();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case (WM_DESTROY) : { PostQuitMessage(0); }
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//