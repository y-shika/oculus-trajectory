/************************************************************************************
Filename    :   main.cpp
Content     :   First-person view test application for Oculus Rift
Created     :   11th May 2015
Authors     :   Tom Heath
Copyright   :   Copyright 2015 Oculus, Inc. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*************************************************************************************/
/// This is an entry-level sample, showing a minimal VR sample, 
/// in a simple environment.  Use WASD keys to move around, and cursor keys.
/// Dismiss the health and safety warning by tapping the headset, 
/// or pressing any key. 
/// It runs with DirectX11.
// Editor : Wizapply : Ovrvision Team


#include "Kernel/OVR_System.h"

// Include DirectX
//#include "Win32_DirectXAppUtil.h"
#include "Win32_GLAppUtil.h"

// Include the Oculus SDK
//#include "OVR_CAPI_D3D.h"
#include "OVR_CAPI_GL.h"

// OvrvisionPro
#include <ovrvision_pro.h>

//#include <opencv2/opencv.hpp>

#include <iostream>

//#pragma comment(lib, "opencv_core320.lib")
//#pragma comment(lib, "opencv_highgui320.lib")
//#pragma comment(lib, "opencv_imgproc320.lib")
#pragma comment(lib, "LibOVRKernel.lib")

/*
class CameraPlane {
	GLuint CreateShader(GLenum type, const GLchar* src)	{
		GLuint shader = glCreateShader(type);

		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);

		GLint r;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
		if (!r)
		{
			GLchar msg[1024];
			glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
			if (msg[0]) {
				OVR_DEBUG_LOG(("Compiling shader failed: %s\n", msg));
			}
			return 0;
		}

		return shader;
	}
	GLuint LinkProgram(GLuint vertexShader, GLuint pixelShader) {
		GLuint program = glCreateProgram();

		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);

		glLinkProgram(program);

		glDetachShader(program, vertexShader);
		glDetachShader(program, pixelShader);

		GLint r;
		glGetProgramiv(program, GL_LINK_STATUS, &r);
		if (!r)
		{
			GLchar msg[1024];
			glGetProgramInfoLog(program, sizeof(msg), 0, msg);
			OVR_DEBUG_LOG(("Linking shaders failed: %s\n", msg));
		}

		return program;
	}

public:
	struct Vertex {
		float x, y, z;
		float u, v;
	};
	
	GLuint prog;
	GLuint tex;
	GLuint vbo, ibo;

	CameraPlane() : prog(0), tex(0), vbo(0), ibo(0) {}
	~CameraPlane() {
		if (prog) glDeleteProgram(prog);
		if (tex) glDeleteTextures(1, &tex);
		if (vbo) glDeleteBuffers(1, &vbo);
		if (ibo) glDeleteBuffers(1, &ibo);
	}
	void init(int texW, int texH, int channels) {
		static const GLchar* VertexShaderSrc =
			"#version 150\n"
			"in      vec4 Position;\n"
			"in      vec2 TexCoord;\n"
			"out     vec2 oTexCoord;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = Position;\n"
			"   oTexCoord   = TexCoord;\n"
			"}\n";

		static const char* FragmentShaderSrc =
			"#version 150\n"
			"uniform sampler2D Texture0;\n"
			"in      vec2      oTexCoord;\n"
			"out     vec4      FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = pow(texture2D(Texture0, oTexCoord), vec4(2.2));\n"
			"}\n";

		GLuint vshader = CreateShader(GL_VERTEX_SHADER, VertexShaderSrc);
		GLuint fshader = CreateShader(GL_FRAGMENT_SHADER, FragmentShaderSrc);

		prog = LinkProgram(vshader, fshader);

		glDeleteShader(vshader);
		glDeleteShader(fshader);

		glBindFragDataLocation(prog, 0, "FragColor");

		glUniform1i(glGetUniformLocation(prog, "Texture0"), 0);


		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);

	}

	void draw(unsigned char *camImage, int width, int height) {
		float aspect = (float)height / (float)width * 0.82f;

		Vertex V[] = {
			{ -1.0f,  aspect, 1.0f, 0.0f, 0.0f },
			{ -1.0f, -aspect, 1.0f, 0.0f, 1.0f },
			{  1.0f, -aspect, 1.0f, 1.0f, 1.0f },
			{  1.0f,  aspect, 1.0f, 1.0f, 0.0f }
		};
		unsigned short F[] = {
			0, 1, 2,
			2, 3, 0
		};

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(V), V, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(F), F, GL_STATIC_DRAW);


		glUseProgram(prog);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, camImage);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		GLuint posLoc = glGetAttribLocation(prog, "Position");
		GLuint uvLoc  = glGetAttribLocation(prog, "TexCoord");

		glEnableVertexAttribArray(posLoc);
		glEnableVertexAttribArray(uvLoc);

		glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, x));
		glVertexAttribPointer(uvLoc,  2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, u));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

		glDisableVertexAttribArray(posLoc);
		glDisableVertexAttribArray(uvLoc);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}
};
*/

GLfloat vertex[][3] = {
	// 四角錘
	{ 0.0f, 0.0f, 0.0f },
	{ 0.2f, 0.0f, 0.0f },
	{ 0.1f, 0.2f, 0.1f },
	{ 0.0f, 0.0f, 0.2f },
	{ 0.2f, 0.0f, 0.2f },
};

int edge[][2] = {
	// 四角錘
	{ 0, 1 },
	{ 1, 2 },
	{ 2, 0 },
	{ 3, 4 },
	{ 4, 2 },
	{ 2, 3 },
	{ 0, 3 },
	{ 1, 4 }
};

// return true to retry later (e.g. after display lost)
static bool MainLoop(bool retryCreate)
{
	/*AllocConsole();
	FILE *fp = NULL;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "COIN$", "r", stdin);*/

	TextureBuffer * eyeRenderTexture[2] = { nullptr, nullptr };
	DepthBuffer   * eyeDepthBuffer[2] = { nullptr, nullptr };
	ovrGLTexture  * mirrorTexture = nullptr;
	GLuint          mirrorFBO = 0;
	Scene         * roomScene = nullptr;

	OVR::OvrvisionPro ovrvision;
	int width = 0, height = 0, pixelsize = 4;
	//CameraPlane camPlane;
	//cv::Mat imgL, imgR;


	ovrHmd HMD;
	ovrGraphicsLuid luid;
	ovrResult result = ovr_Create(&HMD, &luid);
	if (!OVR_SUCCESS(result))
		return retryCreate;

	ovrHmdDesc hmdDesc = ovr_GetHmdDesc(HMD);

	// Setup Window and Graphics
	// Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution
	ovrSizei windowSize = { hmdDesc.Resolution.w / 2, hmdDesc.Resolution.h / 2 };
	if (!Platform.InitDevice(windowSize.w, windowSize.h, reinterpret_cast<LUID*>(&luid)))
		goto Done;

	// Make eye render buffers
	for (int eye = 0; eye < 2; ++eye)
	{
		ovrSizei idealTextureSize = ovr_GetFovTextureSize(HMD, ovrEyeType(eye), hmdDesc.DefaultEyeFov[eye], 1);
		eyeRenderTexture[eye] = new TextureBuffer(HMD, true, true, idealTextureSize, 1, NULL, 1);
		eyeDepthBuffer[eye] = new DepthBuffer(eyeRenderTexture[eye]->GetSize(), 0);

		if (!eyeRenderTexture[eye]->TextureSet)
		{
			if (retryCreate) goto Done;
			VALIDATE(false, "Failed to create texture.");
		}
	}

	// Create mirror texture and an FBO used to copy mirror texture to back buffer
	result = ovr_CreateMirrorTextureGL(HMD, GL_SRGB8_ALPHA8, windowSize.w, windowSize.h, reinterpret_cast<ovrTexture**>(&mirrorTexture));
	if (!OVR_SUCCESS(result))
	{
		if (retryCreate) goto Done;
		VALIDATE(false, "Failed to create mirror texture.");
	}

	// Configure the mirror read buffer
	glGenFramebuffers(1, &mirrorFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture->OGL.TexId, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	ovrEyeRenderDesc EyeRenderDesc[2];
	EyeRenderDesc[0] = ovr_GetRenderDesc(HMD, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
	EyeRenderDesc[1] = ovr_GetRenderDesc(HMD, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

	// Turn off vsync to let the compositor do its magic
	wglSwapIntervalEXT(0);

	// Make scene - can simplify further if needed
	roomScene = new Scene(false);

	bool isVisible = true;

	int locationID = 0;
	OVR::Camprop cameraMode = OVR::OV_CAMVR_FULL;
	if (__argc > 2) {
		printf("Ovrvisin Pro mode changed.");
		//__argv[0]; ApplicationPath
		locationID = atoi(__argv[1]);
		cameraMode = (OVR::Camprop)atoi(__argv[2]);
	}

	if (ovrvision.Open(locationID, cameraMode)) {
		width = ovrvision.GetCamWidth();
		height = ovrvision.GetCamHeight();
		pixelsize = ovrvision.GetCamPixelsize();

		ovrvision.SetCameraSyncMode(false);
	}

	//camPlane.init(width, height, pixelsize);
	//imgL = cv::Mat(height, width, CV_8UC4, cv::Scalar(0, 0, 0, 0));
	//imgR = cv::Mat(height, width, CV_8UC4, cv::Scalar(0, 0, 0, 0));

	// Main loop
	while (Platform.HandleMessages())
	{
		// Keyboard inputs to adjust player position
		//static Vector3f Pos2(0.0f, 1.6f, -5.0f);
		//if (Platform.Key['W'] || Platform.Key[VK_UP])     Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(0, 0, -0.05f));
		//if (Platform.Key['S'] || Platform.Key[VK_DOWN])   Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(0, 0, +0.05f));
		//if (Platform.Key['D'])                          Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(+0.05f, 0, 0));
		//if (Platform.Key['A'])                          Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(-0.05f, 0, 0));
		//Pos2.y = ovr_GetFloat(HMD, OVR_KEY_EYE_HEIGHT, Pos2.y);

		// Get eye poses, feeding in correct IPD offset
		ovrVector3f ViewOffset[2] = { EyeRenderDesc[0].HmdToEyeViewOffset, EyeRenderDesc[1].HmdToEyeViewOffset };
		ovrPosef    EyeRenderPose[2];

		double           ftiming = ovr_GetPredictedDisplayTime(HMD, 0);
		// Keeping sensorSampleTime as close to ovr_GetTrackingState as possible - fed into the layer
		double           sensorSampleTime = ovr_GetTimeInSeconds();
		ovrTrackingState hmdState = ovr_GetTrackingState(HMD, ftiming, ovrTrue);
		ovr_CalcEyePoses(hmdState.HeadPose.ThePose, ViewOffset, EyeRenderPose);

		ovrvision.PreStoreCamData(OVR::Camqt::OV_CAMQT_DMSRMP);

		std::cout << EyeRenderPose[0].Position.x << ", " << EyeRenderPose[0].Position.y << ", " << EyeRenderPose[0].Position.z << std::endl;

		if (isVisible)
		{
			for (int eye = 0; eye < 2; ++eye)
			{
				// Increment to use next texture, just before writing
				eyeRenderTexture[eye]->TextureSet->CurrentIndex = (eyeRenderTexture[eye]->TextureSet->CurrentIndex + 1) % eyeRenderTexture[eye]->TextureSet->TextureCount;

				// Switch to eye render target
				eyeRenderTexture[eye]->SetAndClearRenderSurface(eyeDepthBuffer[eye]);



				// work space
				//

				//Get the pose information in XM format
				ovrPosef pose = EyeRenderPose[eye];
				Matrix3f R = Matrix3f(pose.Orientation).Transposed();
				Vector3f c = pose.Position;
				Vector3f t = (R * pose.Position) * -1.0f;
				Matrix4f view = Matrix4f(
					R.M[0][0], R.M[0][1], R.M[0][2], t[0],
					R.M[1][0], R.M[1][1], R.M[1][2], t[1],
					R.M[2][0], R.M[2][1], R.M[2][2], t[2],
					     0.0f,      0.0f,      0.0f, 1.0f
					);

				/*std::cout <<
					R.M[0][0] << ", " << R.M[0][1] << ", " << R.M[0][2] << ", " << c[0] << "\n" <<
					R.M[1][0] << ", " << R.M[1][1] << ", " << R.M[1][2] << ", " << c[1] << "\n" <<
					R.M[2][0] << ", " << R.M[2][1] << ", " << R.M[2][2] << ", " << c[2] << std::endl;

				std::cout << "\r" <<
					R.M[0][0] << ", " << R.M[1][0] << ", " << R.M[2][0] << "\t";*/


				//Matrix4f view = Matrix4f(pose.Orientation).Transposed();
				Matrix4f proj = ovrMatrix4f_Projection(hmdDesc.DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_RightHanded); // (angle, znear, zfar, 0?)
				
				glFrontFace(GL_CCW);

				glClearColor(0, 0, 0, 1);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glDisable(GL_DEPTH_TEST);

				//cv::cvtColor(imgL, imgL, cv::COLOR_BGRA2RGB);
				//if (eye == 0)
				//	camPlane.draw(ovrvision.GetCamImageBGRA(OVR::Cameye::OV_CAMEYE_LEFT), width, height);
				//else
				//	camPlane.draw(ovrvision.GetCamImageBGRA(OVR::Cameye::OV_CAMEYE_RIGHT), width, height);

				glUseProgram(0);

				glEnable(GL_DEPTH_TEST);

				//glUseProgram(0);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glMultTransposeMatrixf(proj.M[0]);

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				glMultTransposeMatrixf(view.M[0]);
				//glTranslated(0, 0.05, -0.85);
				glTranslated(0, 0.0, -1);

				float len = 0.2f;
				glBegin(GL_LINES);
				glColor3d(1, 0, 0);
				glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(len, 0.0f, 0.0f);
				glColor3d(0, 1, 0);
				glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, len, 0.0f);
				glColor3d(0, 0, 1);
				glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, len);
				glEnd();



				glBegin(GL_LINES);
				for (int i = 0; i < sizeof(edge) / (sizeof(int) * 2); ++i) {
					glColor3d(1.0, 0.0, 0.0);
					glVertex3fv(vertex[edge[i][0]]); glVertex3fv(vertex[edge[i][1]]);
				}
				glEnd();

				// Avoids an error when calling SetAndClearRenderSurface during next iteration.
				// Without this, during the next while loop iteration SetAndClearRenderSurface
				// would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
				// associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
				eyeRenderTexture[eye]->UnsetRenderSurface();
			}
		}

		// Do distortion rendering, Present and flush/sync

		// Set up positional data.
		ovrViewScaleDesc viewScaleDesc;
		viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
		viewScaleDesc.HmdToEyeViewOffset[0] = ViewOffset[0];
		viewScaleDesc.HmdToEyeViewOffset[1] = ViewOffset[1];

		ovrLayerEyeFov ld;
		ld.Header.Type = ovrLayerType_EyeFov;
		ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

		for (int eye = 0; eye < 2; ++eye)
		{
			ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureSet;
			ld.Viewport[eye] = Recti(eyeRenderTexture[eye]->GetSize());
			ld.Fov[eye] = hmdDesc.DefaultEyeFov[eye];
			ld.RenderPose[eye] = EyeRenderPose[eye];
			ld.SensorSampleTime = sensorSampleTime;
		}

		ovrLayerHeader* layers = &ld.Header;
		ovrResult result = ovr_SubmitFrame(HMD, 0, &viewScaleDesc, &layers, 1);
		// exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
		if (!OVR_SUCCESS(result))
			goto Done;

		isVisible = (result == ovrSuccess);

		// Blit mirror texture to back buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		GLint w = mirrorTexture->OGL.Header.TextureSize.w;
		GLint h = mirrorTexture->OGL.Header.TextureSize.h;
		glBlitFramebuffer(0, h, w, 0,
			0, 0, w, h,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		SwapBuffers(Platform.hDC);
	}

Done:
	delete roomScene;
	if (mirrorFBO) glDeleteFramebuffers(1, &mirrorFBO);
	if (mirrorTexture) ovr_DestroyMirrorTexture(HMD, reinterpret_cast<ovrTexture*>(mirrorTexture));
	for (int eye = 0; eye < 2; ++eye)
	{
		delete eyeRenderTexture[eye];
		delete eyeDepthBuffer[eye];
	}
	Platform.ReleaseDevice();
	ovr_Destroy(HMD);

	ovrvision.Close();
	FreeConsole();

	// Retry on ovrError_DisplayLost
	return retryCreate || OVR_SUCCESS(result) || (result == ovrError_DisplayLost);
}

// Display console
void DispConsole()
{
	AllocConsole();
	FILE *fp = NULL;

	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "COIN$", "r", stdin);
}


//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	OVR::System::Init();

	// Initializes LibOVR, and the Rift
	ovrResult result = ovr_Initialize(nullptr);
	VALIDATE(OVR_SUCCESS(result), "Failed to initialize libOVR.");

	VALIDATE(Platform.InitWindow(hinst, L"Ovrvision Pro for OculusSDK"), "Failed to open window.");

	DispConsole();

	Platform.Run(MainLoop);

	ovr_Shutdown();

	OVR::System::Destroy();

	return(0);
}

/*
extern int InitializeCamPlane(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext, int w, int h, float zsize);
extern int RendererCamPlane(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext);
extern int SetCamImage(ID3D11DeviceContext* DeviceContext, unsigned char* camImage, unsigned int imagesize);
extern int CleanCamPlane();

//------------------------------------------------------------
// ovrSwapTextureSet wrapper class that also maintains the render target views
// needed for D3D11 rendering.
struct OculusTexture
{
    ovrHmd                   hmd;
	ovrSwapTextureSet      * TextureSet;
    static const int         TextureCount = 2;
	ID3D11RenderTargetView * TexRtv[TextureCount];

    OculusTexture() :
        hmd(nullptr),
        TextureSet(nullptr)
    {
        TexRtv[0] = TexRtv[1] = nullptr;
    }

    bool Init(ovrHmd _hmd, int sizeW, int sizeH)
	{
        hmd = _hmd;

		D3D11_TEXTURE2D_DESC dsDesc;
		dsDesc.Width = sizeW;
		dsDesc.Height = sizeH;
		dsDesc.MipLevels = 1;
		dsDesc.ArraySize = 1;
        dsDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		dsDesc.SampleDesc.Count = 1;   // No multi-sampling allowed
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.Usage = D3D11_USAGE_DEFAULT;
		dsDesc.CPUAccessFlags = 0;
		dsDesc.MiscFlags = 0;
		dsDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		ovrResult result = ovr_CreateSwapTextureSetD3D11(hmd, DIRECTX.Device, &dsDesc, ovrSwapTextureSetD3D11_Typeless, &TextureSet);
        if (!OVR_SUCCESS(result))
            return false;

        VALIDATE(TextureSet->TextureCount == TextureCount, "TextureCount mismatch.");

		for (int i = 0; i < TextureCount; ++i)
		{
			ovrD3D11Texture* tex = (ovrD3D11Texture*)&TextureSet->Textures[i];
			D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
			rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			DIRECTX.Device->CreateRenderTargetView(tex->D3D11.pTexture, &rtvd, &TexRtv[i]);
		}

        return true;
    }

	~OculusTexture()
	{
		for (int i = 0; i < TextureCount; ++i)
        {
            Release(TexRtv[i]);
        }
		if (TextureSet)
        {
            ovr_DestroySwapTextureSet(hmd, TextureSet);
        }
	}

	void AdvanceToNextTexture()
	{
		TextureSet->CurrentIndex = (TextureSet->CurrentIndex + 1) % TextureSet->TextureCount;
	}
};

// return true to retry later (e.g. after display lost)
static bool MainLoop(bool retryCreate)
{
    // Initialize these to nullptr here to handle device lost failures cleanly
	ovrTexture     * mirrorTexture = nullptr;
	OculusTexture  * pEyeRenderTexture[2] = { nullptr, nullptr };
	DepthBuffer    * pEyeDepthBuffer[2] = { nullptr, nullptr };
    Camera         * mainCam = nullptr;
	D3D11_TEXTURE2D_DESC td = {};

	OVR::OvrvisionPro ovrvision;
	int width = 0, height = 0, pixelsize=4;

	ovrHmd HMD;
	ovrGraphicsLuid luid;
	ovrResult result = ovr_Create(&HMD, &luid);
    if (!OVR_SUCCESS(result))
        return retryCreate;

    ovrHmdDesc hmdDesc = ovr_GetHmdDesc(HMD);

	// Setup Device and Graphics
	// Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution
    if (!DIRECTX.InitDevice(hmdDesc.Resolution.w / 2, hmdDesc.Resolution.h / 2, reinterpret_cast<LUID*>(&luid)))
        goto Done;

	// Make the eye render buffers (caution if actual size < requested due to HW limits). 
	ovrRecti         eyeRenderViewport[2];

	for (int eye = 0; eye < 2; ++eye)
	{
		ovrSizei idealSize = ovr_GetFovTextureSize(HMD, (ovrEyeType)eye, hmdDesc.DefaultEyeFov[eye], 1.0f);
		pEyeRenderTexture[eye] = new OculusTexture();
		if (!pEyeRenderTexture[eye]->Init(HMD, idealSize.w, idealSize.h))
		{
			if (retryCreate) goto Done;
			VALIDATE(OVR_SUCCESS(result), "Failed to create eye texture.");
		}
		pEyeDepthBuffer[eye] = new DepthBuffer(DIRECTX.Device, idealSize.w, idealSize.h);
		eyeRenderViewport[eye].Pos.x = 0;
		eyeRenderViewport[eye].Pos.y = 0;
		eyeRenderViewport[eye].Size = idealSize;
		if (!pEyeRenderTexture[eye]->TextureSet)
		{
			if (retryCreate) goto Done;
			VALIDATE(false, "Failed to create texture.");
		}
	}

	// Create a mirror to see on the monitor.
	td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	td.Width = DIRECTX.WinSizeW;
	td.Height = DIRECTX.WinSizeH;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.SampleDesc.Count = 1;
	td.MipLevels = 1;
    result = ovr_CreateMirrorTextureD3D11(HMD, DIRECTX.Device, &td, 0, &mirrorTexture);
    if (!OVR_SUCCESS(result))
    {
        if (retryCreate) goto Done;
        VALIDATE(false, "Failed to create mirror texture.");
    }

	// Create camera
    mainCam = new Camera(&XMVectorSet(0.0f, 1.6f, 5.0f, 0), &XMQuaternionIdentity());

	// Setup VR components, filling out description
	ovrEyeRenderDesc eyeRenderDesc[2];
	eyeRenderDesc[0] = ovr_GetRenderDesc(HMD, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
	eyeRenderDesc[1] = ovr_GetRenderDesc(HMD, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

    bool isVisible = true;

	int locationID = 0;
	OVR::Camprop cameraMode = OVR::OV_CAMVR_FULL;
	if (__argc > 2) {
		printf("Ovrvisin Pro mode changed.");
		//__argv[0]; ApplicationPath
		locationID = atoi(__argv[1]);
		cameraMode = (OVR::Camprop)atoi(__argv[2]);
	}

	if (ovrvision.Open(locationID, cameraMode)) {
		width = ovrvision.GetCamWidth();
		height = ovrvision.GetCamHeight();
		pixelsize = ovrvision.GetCamPixelsize();

		ovrvision.SetCameraSyncMode(false);

		InitializeCamPlane(DIRECTX.Device, DIRECTX.Context, width, height, 1.0f);
	}

	// Main loop
	while (DIRECTX.HandleMessages())
	{
		//XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, -0.05f, 0), mainCam->Rot);
		//XMVECTOR right   = XMVector3Rotate(XMVectorSet(0.05f, 0, 0, 0),  mainCam->Rot);
		//
		//if (DIRECTX.Key['W'] || DIRECTX.Key[VK_UP])	  mainCam->Pos = XMVectorAdd(mainCam->Pos, forward);
		//if (DIRECTX.Key['S'] || DIRECTX.Key[VK_DOWN]) mainCam->Pos = XMVectorSubtract(mainCam->Pos, forward);
		//if (DIRECTX.Key['D'])                         mainCam->Pos = XMVectorAdd(mainCam->Pos, right);
		//if (DIRECTX.Key['A'])                         mainCam->Pos = XMVectorSubtract(mainCam->Pos, right);
		//static float Yaw = 0;
		//if (DIRECTX.Key[VK_LEFT])  mainCam->Rot = XMQuaternionRotationRollPitchYaw(0, Yaw += 0.02f, 0);
		//if (DIRECTX.Key[VK_RIGHT]) mainCam->Rot = XMQuaternionRotationRollPitchYaw(0, Yaw -= 0.02f, 0);


		// Get both eye poses simultaneously, with IPD offset already included. 
		ovrPosef         EyeRenderPose[2];
		ovrVector3f      HmdToEyeViewOffset[2] = { eyeRenderDesc[0].HmdToEyeViewOffset,
			                                       eyeRenderDesc[1].HmdToEyeViewOffset };
        double frameTime = ovr_GetPredictedDisplayTime(HMD, 0);
        // Keeping sensorSampleTime as close to ovr_GetTrackingState as possible - fed into the layer
        double           sensorSampleTime = ovr_GetTimeInSeconds();
		ovrTrackingState hmdState = ovr_GetTrackingState(HMD, frameTime, ovrTrue);
		ovr_CalcEyePoses(hmdState.HeadPose.ThePose, HmdToEyeViewOffset, EyeRenderPose);
		
		ovrvision.PreStoreCamData(OVR::Camqt::OV_CAMQT_DMSRMP);

		// Render Scene to Eye Buffers
        if (isVisible)
        {
            for (int eye = 0; eye < 2; ++eye)
		    {
			    // Increment to use next texture, just before writing
			    pEyeRenderTexture[eye]->AdvanceToNextTexture();

			    // Clear and set up rendertarget
			    int texIndex = pEyeRenderTexture[eye]->TextureSet->CurrentIndex;
			    DIRECTX.SetAndClearRenderTarget(pEyeRenderTexture[eye]->TexRtv[texIndex], pEyeDepthBuffer[eye]);
			    DIRECTX.SetViewport((float)eyeRenderViewport[eye].Pos.x, (float)eyeRenderViewport[eye].Pos.y,
				    (float)eyeRenderViewport[eye].Size.w, (float)eyeRenderViewport[eye].Size.h);

			    //Get the pose information in XM format
			    XMVECTOR eyeQuat = XMVectorSet(EyeRenderPose[eye].Orientation.x, EyeRenderPose[eye].Orientation.y,
				                               EyeRenderPose[eye].Orientation.z, EyeRenderPose[eye].Orientation.w);
			    XMVECTOR eyePos = XMVectorSet(EyeRenderPose[eye].Position.x, EyeRenderPose[eye].Position.y, EyeRenderPose[eye].Position.z, 0);

			    // Get view and projection matrices for the Rift camera
			    XMVECTOR CombinedPos = XMVectorAdd(mainCam->Pos, XMVector3Rotate(eyePos, mainCam->Rot));
			    Camera finalCam(&CombinedPos, &(XMQuaternionMultiply(eyeQuat,mainCam->Rot)));
			    XMMATRIX view = finalCam.GetViewMatrix();
			    ovrMatrix4f p = ovrMatrix4f_Projection(eyeRenderDesc[eye].Fov, 0.2f, 1000.0f, ovrProjection_RightHanded);
			    XMMATRIX proj = XMMatrixSet(p.M[0][0], p.M[1][0], p.M[2][0], p.M[3][0],
				                            p.M[0][1], p.M[1][1], p.M[2][1], p.M[3][1],
				                            p.M[0][2], p.M[1][2], p.M[2][2], p.M[3][2],
				                            p.M[0][3], p.M[1][3], p.M[2][3], p.M[3][3]);
			    XMMATRIX prod = XMMatrixMultiply(view, proj);

				//Camera View
				if (eye == 0)
					SetCamImage(DIRECTX.Context, ovrvision.GetCamImageBGRA(OVR::Cameye::OV_CAMEYE_LEFT), width*pixelsize);
				else
					SetCamImage(DIRECTX.Context, ovrvision.GetCamImageBGRA(OVR::Cameye::OV_CAMEYE_RIGHT), width*pixelsize);
				
				RendererCamPlane(DIRECTX.Device, DIRECTX.Context);
		    }
        }

		// Initialize our single full screen Fov layer.
        ovrLayerEyeFov ld = {};
		ld.Header.Type = ovrLayerType_EyeFov;
		ld.Header.Flags = 0;

		for (int eye = 0; eye < 2; ++eye)
		{
			ld.ColorTexture[eye] = pEyeRenderTexture[eye]->TextureSet;
			ld.Viewport[eye] = eyeRenderViewport[eye];
			ld.Fov[eye] = hmdDesc.DefaultEyeFov[eye];
			ld.RenderPose[eye] = EyeRenderPose[eye];
            ld.SensorSampleTime = sensorSampleTime;
		}

        ovrLayerHeader* layers = &ld.Header;
        result = ovr_SubmitFrame(HMD, 0, nullptr, &layers, 1);
        // exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
        if (!OVR_SUCCESS(result))
            goto Done;

        isVisible = (result == ovrSuccess);

        // Render mirror
        ovrD3D11Texture* tex = (ovrD3D11Texture*)mirrorTexture;
        DIRECTX.Context->CopyResource(DIRECTX.BackBuffer, tex->D3D11.pTexture);
        DIRECTX.SwapChain->Present(0, 0);
	}

	// Release resources
Done:
    delete mainCam;
	if (mirrorTexture) ovr_DestroyMirrorTexture(HMD, mirrorTexture);
    for (int eye = 0; eye < 2; ++eye)
    {
	    delete pEyeRenderTexture[eye];
        delete pEyeDepthBuffer[eye];
    }
	DIRECTX.ReleaseDevice();
	ovr_Destroy(HMD);

	ovrvision.Close();
	CleanCamPlane();

    // Retry on ovrError_DisplayLost
    return retryCreate || OVR_SUCCESS(result) || (result == ovrError_DisplayLost);
}

//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	// Initializes LibOVR, and the Rift
	ovrResult result = ovr_Initialize(nullptr);
	VALIDATE(OVR_SUCCESS(result), "Failed to initialize libOVR.");

    VALIDATE(DIRECTX.InitWindow(hinst, L"Ovrvision Pro for OculusSDK"), "Failed to open window.");

    DIRECTX.Run(MainLoop);

	ovr_Shutdown();
	return(0);
}

*/