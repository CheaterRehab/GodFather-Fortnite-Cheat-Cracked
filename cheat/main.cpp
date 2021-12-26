#include "stdafx.h"

static HWND get_process_wnd(uint32_t pid) {
	std::pair<HWND, uint32_t> params = { 0, pid };

	BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
		auto pParams = (std::pair<HWND, uint32_t>*)(lParam);
		uint32_t processId = 0;

		if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processId)) && processId == pParams->second) {
			SetLastError((uint32_t)-1);
			pParams->first = hwnd;
			return FALSE;
		}

		return TRUE;

		}, (LPARAM)&params);

	if (!bResult && GetLastError() == -1 && params.first)
		return params.first;

	return NULL;
}
HRESULT DirectXInit(HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Width;
	p_Params.BackBufferHeight = Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
	{
		p_Object->Release();
		exit(4);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImFontConfig font_config;
	font_config.OversampleH = 1;
	font_config.OversampleV = 1;
	font_config.PixelSnapH = true;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF,
		0x0400, 0x044F,
		0,
	};

	io.Fonts->AddFontFromFileTTF(E("C:\\Windows\\Fonts\\CONSOLA.ttf"), 13.f);

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(p_Device);
	ImGuiStyle& Style = ImGui::GetStyle();
	auto Color = Style.Colors;

	Style.WindowBorderSize = 0;

	Style.ChildRounding = 0;
	Style.FrameRounding = 0;
	Style.ScrollbarRounding = 0;
	Style.GrabRounding = 0;
	Style.PopupRounding = 0;
	Style.WindowRounding = 0;
	Style.WindowTitleAlign.x = 0.50f;
	Style.WindowPadding.x = 20.0f;

	Color[ImGuiCol_WindowBg] = ImColor(18, 18, 18, 255);

	Color[ImGuiCol_FrameBg] = ImColor(31, 31, 31, 255);
	Color[ImGuiCol_FrameBgActive] = ImColor(41, 41, 41, 255);
	Color[ImGuiCol_FrameBgHovered] = ImColor(41, 41, 41, 255);

	Color[ImGuiCol_Button] = ImColor(29, 29, 29, 255);
	Color[ImGuiCol_ButtonActive] = ImColor(32, 32, 32, 255);
	Color[ImGuiCol_ButtonHovered] = ImColor(36, 36, 36, 255);

	Color[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
	Color[ImGuiCol_Separator] = ImColor(36, 36, 36, 255);

	Color[ImGuiCol_ResizeGrip] = ImColor(30, 30, 30, 255);
	Color[ImGuiCol_ResizeGripActive] = ImColor(30, 30, 30, 255);
	Color[ImGuiCol_ResizeGripHovered] = ImColor(30, 30, 30, 255);

	Color[ImGuiCol_ChildBg] = ImColor(26, 26, 26, 255);

	Color[ImGuiCol_ScrollbarBg] = ImColor(24, 24, 24, 255);
	Color[ImGuiCol_ScrollbarGrab] = ImColor(24, 24, 24, 255);
	Color[ImGuiCol_ScrollbarGrabActive] = ImColor(24, 24, 24, 255);
	Color[ImGuiCol_ScrollbarGrabActive] = ImColor(24, 24, 24, 255);

	Color[ImGuiCol_Header] = ImColor(39, 39, 39, 255);
	Color[ImGuiCol_HeaderActive] = ImColor(39, 39, 39, 255);
	Color[ImGuiCol_HeaderHovered] = ImColor(39, 39, 39, 255);
	Color[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);

	p_Object->Release();
	return S_OK;
}
void SetupWindow()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		nullptr,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		("X"),
		LoadIcon(nullptr, IDI_APPLICATION)
	};

	RECT Rect;
	GetWindowRect(GetDesktopWindow(), &Rect);

	RegisterClassExA(&wcex);

	MyWnd = CreateWindowExA(NULL, E("X"), E("X"), WS_POPUP, Rect.left, Rect.top, Rect.right, Rect.bottom, NULL, NULL, wcex.hInstance, NULL);
	SetWindowLong(MyWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(MyWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	//SetWindowDisplayAffinity(MyWnd, 1);

	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(MyWnd, &margin);

	ShowWindow(MyWnd, SW_SHOW);
	UpdateWindow(MyWnd);
}

uint64_t Uworld,
LocalPlayers,
PlayerController,
LocalPawn,
LocalPlayerState,
LocalRootcomp,
Persistentlevel;

uint32_t localplayerID;
Vector3 LocalRelativeLocation;

bool isaimbotting;

typedef struct _FNlEntity {
	uint64_t Actor;
	int ID;
	uint64_t mesh;
	uint64_t PlayerState;
	uint64_t LocalPlayerState;
	bool Spotted;
}FNlEntity;
std::vector<FNlEntity> entityList;

float closestDistance = FLT_MAX;
DWORD_PTR closestPawn = NULL;
bool targetlocked = false;
float flRadarPos_x = 120;
float flRadarPos_y = 120;
float iSavedRadarX;
float iSavedRadarY;
int mouse_x, mouse_y;
uint64_t offset_uworld;
float zoom;
bool isRecentlyRendered;

int get_fps()
{
	using namespace std::chrono;
	static int count = 0;
	static auto last = high_resolution_clock::now();
	auto now = high_resolution_clock::now();
	static int fps = 0;

	count++;

	if (duration_cast<milliseconds>(now - last).count() > 1000) {
		fps = count;
		count = 0;
		last = now;
	}

	return fps;
}

static bool mouse_move(int x, int y)
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.time = 0;
	input.mi.dx = x;
	input.mi.dy = y;
	input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;
	SendInput(1, &input, sizeof(input));
	return true;
}
double GetCrossDistance(double x1, double y1, double z1, double x2, double y2, double z2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}
inline auto move_to(float x, float y) -> void {
	float center_x = (ImGui::GetIO().DisplaySize.x / 2);
	float center_y = (ImGui::GetIO().DisplaySize.y / 2);

	float smooth = aimbot::aimspeed;
	float target_x = 0.f;
	float target_y = 0.f;

	if (x != 0.f)
	{
		if (x > center_x)
		{
			target_x = -(center_x - x);
			target_x /= smooth;
			if (target_x + center_x > center_x * 2.f) target_x = 0.f;
		}

		if (x < center_x)
		{
			target_x = x - center_x;
			target_x /= smooth;
			if (target_x + center_x < 0.f) target_x = 0.f;
		}
	}

	if (y != 0.f)
	{
		if (y > center_y)
		{
			target_y = -(center_y - y);
			target_y /= smooth;
			if (target_y + center_y > center_y * 2.f) target_y = 0.f;
		}

		if (y < center_y)
		{
			target_y = y - center_y;
			target_y /= smooth;
			if (target_y + center_y < 0.f) target_y = 0.f;
		}
	}

	mouse_move((int)target_x, (int)(target_y));
}
FTransform GetBoneIndex(uint64_t mesh, int index)
{
	uint64_t bonearray = read<uint64_t>(mesh + 0x4a8);
	if (!bonearray) bonearray = read<uint64_t>(mesh + 0x4a8 + 0x10);
	return read<FTransform>(bonearray + (index * 0x30));
}
Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = read<FTransform>(mesh + 0x1c0);

	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}
Vector3 ProjectWorldToScreen(Vector3 WorldLocation)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);
	Vector3 Camera;

	auto chain69 = read<uintptr_t>(LocalPlayers + 0xA8);
	uint64_t chain699 = read<uintptr_t>(chain69 + 8);

	Camera.x = read<float>(chain699 + 0x7F8);
	//printf("Camera.x :0x%llX\n", Camera.x);
	Camera.y = read<float>(LocalRootcomp + 0x12C);

	float test = asin(Camera.x);
	float degrees = test * (180.0 / M_PI);
	Camera.x = degrees;

	if (Camera.y < 0)
		Camera.y = 360 + Camera.y;

	D3DMATRIX tempMatrix = Matrix(Camera);
	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	uint64_t chain = read<uint64_t>(LocalPlayers + 0x70);
	uint64_t chain1 = read<uint64_t>(chain + 0x98);
	uint64_t chain2 = read<uint64_t>(chain1 + 0x140);
	//printf("chain2 :0x%llX\n", chain2);

	Vector3 vDelta = WorldLocation - read<Vector3>(chain2 + 0x10);
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	zoom = read<float>(chain699 + 0x590);
	//printf("zoom %f\n", zoom);

	float FovAngle = 80.0f / (zoom / 1.19f);
	float ScreenCenterX = Width / 2.0f;
	float ScreenCenterY = Height / 2.0f;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

	return Screenlocation;
}
bool IsInScreen(Vector3 pos, int over = 30) {
	if (pos.x > -over && pos.x < Width + over && pos.y > -over && pos.y < Height + over) {
		return true;
	}
	else {
		return false;
	}
}
bool IsVec3Valid(Vector3 vec3)
{
	return !(vec3.x == 0 && vec3.y == 0 && vec3.z == 0);
}

void fn_cache() {

	while (true) {

		std::vector<FNlEntity> tmpList;

		if (!offset_uworld)
			offset_uworld = find_signature(E("\x48\x89\x05\x00\x00\x00\x00\x48\x8B\x4B\x78"), E("xxx????xxxx")) - sdk::module_base; // changes sometimes

		//printf("offset_uworld : 0x%llX\n", offset_uworld);

		Uworld = read<uint64_t>(sdk::module_base + offset_uworld);
		//printf("Uworld : 0x%llX\n", Uworld);

		uint64_t GameInstance = read<uint64_t>(Uworld + 0x190); // changes sometimes
		//printf("GameInstance : 0x%llX\n", GameInstance);

		LocalPlayers = read<uint64_t>(read<uint64_t>(GameInstance + 0x38));
		//printf("LocalPlayers : 0x%llX\n", LocalPlayers);

		PlayerController = read<uint64_t>(LocalPlayers + 0x30);
		//printf("PlayerController : 0x%llX\n", PlayerController);

		LocalPawn = read<uint64_t>(PlayerController + 0x2A8); // changes sometimes
		if (!LocalPawn)continue;
		//printf("LocalPawn : 0x%llX\n", LocalPawn);

		LocalRootcomp = read<uint64_t>(LocalPawn + 0x130);
		if (!LocalRootcomp)continue;
		//printf("LocalRootcomp : 0x%llX\n", LocalRootcomp);

		LocalRelativeLocation = read<Vector3>(LocalRootcomp + 0x11C);
		if (!IsVec3Valid(LocalRelativeLocation))continue;

		uintptr_t GameState = read<uintptr_t>(Uworld + 0x130); // changes sometimes
		if (!GameState)continue;
		//printf("GameState : 0x%llX\n", GameState);

		uintptr_t PlayerArray = read<uintptr_t>(GameState + 0x238);
		if (!PlayerArray)continue;
		//printf("PlayerArray : 0x%llX\n", PlayerArray);

		int Count = read<int>(GameState + (0x238 + sizeof(uint64_t)));

		//printf("%d\n", Count);

		for (int i = 0; i < Count; i++)
		{
			uint64_t CurrentPlayerState = read<uintptr_t>(PlayerArray + (i * sizeof(uint64_t)));
			if (!CurrentPlayerState) continue;
			//printf("CurrentPlayerState : 0x%llX\n", CurrentPlayerState);

			uintptr_t CurrentPawn = read<uintptr_t>(CurrentPlayerState + 0x280);
			if (!CurrentPawn) continue;

			uint64_t mesh = read<uint64_t>(CurrentPawn + 0x280);
			if (!mesh)continue;
			//printf("mesh : 0x%llX\n", mesh);

			uint32_t curactorid = read<uint32_t>(CurrentPawn + 0x18);
			if (!curactorid)continue;
			//printf("curactorid : %d\n", curactorid);

			uint64_t PlayerState = read<uint64_t>(CurrentPawn + 0x238); //sometimes changes (same as LocalPlayerState)
			if (!PlayerState)continue;

			uint64_t LocalPlayerState = read<uint64_t>(LocalPawn + 0x238); //sometimes changes
			if (!LocalPlayerState)continue;

			if (mesh != 0x00 && PlayerState != 0x00 && LocalPlayerState != 0x00) {
				FNlEntity fnlEntity{ };
				fnlEntity.Actor = CurrentPawn;
				fnlEntity.mesh = mesh;
				fnlEntity.PlayerState = PlayerState;
				fnlEntity.LocalPlayerState = LocalPlayerState;
				tmpList.push_back(fnlEntity);
			}
		}
		//entityList.clear();
		entityList = tmpList;
		std::this_thread::sleep_for(std::chrono::microseconds(250));
	}
}
void fn_esp() {

	float distance;

	RGBA ESPColor;
	RGBA ESPSkeleton;

	auto entityListCopy = entityList;

	for (auto entity : entityListCopy)
	{
		if (LocalPawn == entity.Actor)continue;

		uint64_t rootcomponent = read<uint64_t>(entity.Actor + 0x130);
		if (!rootcomponent)continue;

		Vector3 Relativelocation = read<Vector3>(rootcomponent + 0x11C);
		if (!IsVec3Valid(Relativelocation))continue;

		Vector3 Relative_Location = ProjectWorldToScreen(Relativelocation);
		if (!IsVec3Valid(Relative_Location))continue;

		uint64_t mesh = entity.mesh;
		if (!mesh)continue;

		Vector3 Foot = GetBoneWithRotation(mesh, 0);
		if (!IsVec3Valid(Foot))continue;

		uint64_t LocalPlayerState = entity.LocalPlayerState;

		uint32_t LocalTeamId = read<uint32_t>(LocalPlayerState + 0xF28); //sometimes changes
		if (!LocalTeamId)continue;

		uint64_t PlayerState = entity.PlayerState;
		if (!PlayerState)continue;

		uint32_t TeamId = read<uint32_t>(PlayerState + 0xF28); //sometimes changes
		if (!TeamId)continue;

		if (mesh != 0x00 && LocalTeamId != TeamId && (int)Foot.x != 0 && (int)Foot.z != 0)
		{

			RGBA Visible = { colors::skelcol[0] * 255, colors::skelcol[1] * 255, colors::skelcol[2] * 255, 255 };
			RGBA Invisible = { colors::skelcolvi[0] * 255, colors::skelcolvi[1] * 255, colors::skelcolvi[2] * 255, 255 };
			RGBA EspVisible = { colors::espcol[0] * 255, colors::espcol[1] * 255, colors::espcol[2] * 255, 255 };
			RGBA EspInvisible = { colors::espcolvi[0] * 255, colors::espcolvi[1] * 255, colors::espcolvi[2] * 255, 255 };

			ESPColor = EspInvisible;

			distance = LocalRelativeLocation.Distance(Relativelocation) / 100.f;

			Vector3 vHeadBone = GetBoneWithRotation(mesh, 68);
			if (!IsVec3Valid(vHeadBone))continue;
			Vector3 vHeadBoneOut = ProjectWorldToScreen(vHeadBone);
			if (!IsVec3Valid(vHeadBoneOut))continue;

			Vector3 vHip = GetBoneWithRotation(mesh, 2);
			if (!IsVec3Valid(vHip))continue;
			Vector3 vHipOut = ProjectWorldToScreen(vHip);
			if (!IsVec3Valid(vHipOut))continue;

			Vector3 vNeck = GetBoneWithRotation(mesh, 67);
			if (!IsVec3Valid(vNeck))continue;
			Vector3 vNeckOut = ProjectWorldToScreen(vNeck);
			if (!IsVec3Valid(vNeckOut))continue;

			Vector3 vUpperArmLeft = GetBoneWithRotation(mesh, 9);
			if (!IsVec3Valid(vUpperArmLeft))continue;
			Vector3 vUpperArmLeftOut = ProjectWorldToScreen(vUpperArmLeft);
			if (!IsVec3Valid(vUpperArmLeftOut))continue;

			Vector3 vUpperArmRight = GetBoneWithRotation(mesh, 38);
			if (!IsVec3Valid(vUpperArmRight))continue;
			Vector3 vUpperArmRightOut = ProjectWorldToScreen(vUpperArmRight);
			if (!IsVec3Valid(vUpperArmRightOut))continue;

			Vector3 vLeftHand = GetBoneWithRotation(mesh, 10);
			if (!IsVec3Valid(vLeftHand))continue;
			Vector3 vLeftHandOut = ProjectWorldToScreen(vLeftHand);
			if (!IsVec3Valid(vLeftHandOut))continue;

			Vector3 vRightHand = GetBoneWithRotation(mesh, 39);
			if (!IsVec3Valid(vRightHand))continue;
			Vector3 vRightHandOut = ProjectWorldToScreen(vRightHand);
			if (!IsVec3Valid(vRightHandOut))continue;

			Vector3 vLeftHand1 = GetBoneWithRotation(mesh, 11);
			if (!IsVec3Valid(vLeftHand1))continue;
			Vector3 vLeftHandOut1 = ProjectWorldToScreen(vLeftHand1);
			if (!IsVec3Valid(vLeftHandOut1))continue;

			Vector3 vRightHand1 = GetBoneWithRotation(mesh, 40);
			if (!IsVec3Valid(vRightHand1))continue;
			Vector3 vRightHandOut1 = ProjectWorldToScreen(vRightHand1);
			if (!IsVec3Valid(vRightHandOut1))continue;

			Vector3 vRightThigh = GetBoneWithRotation(mesh, 76);
			if (!IsVec3Valid(vRightThigh))continue;
			Vector3 vRightThighOut = ProjectWorldToScreen(vRightThigh);
			if (!IsVec3Valid(vRightThighOut))continue;

			Vector3 vLeftThigh = GetBoneWithRotation(mesh, 69);
			if (!IsVec3Valid(vLeftThigh))continue;
			Vector3 vLeftThighOut = ProjectWorldToScreen(vLeftThigh);
			if (!IsVec3Valid(vLeftThighOut))continue;

			Vector3 vRightCalf = GetBoneWithRotation(mesh, 77);
			if (!IsVec3Valid(vRightCalf))continue;
			Vector3 vRightCalfOut = ProjectWorldToScreen(vRightCalf);
			if (!IsVec3Valid(vRightCalfOut))continue;

			Vector3 vLeftCalf = GetBoneWithRotation(mesh, 70);
			if (!IsVec3Valid(vLeftCalf))continue;
			Vector3 vLeftCalfOut = ProjectWorldToScreen(vLeftCalf);
			if (!IsVec3Valid(vLeftCalfOut))continue;

			Vector3 vLeftFoot = GetBoneWithRotation(mesh, 73);
			if (!IsVec3Valid(vLeftFoot))continue;
			Vector3 vLeftFootOut = ProjectWorldToScreen(vLeftFoot);
			if (!IsVec3Valid(vLeftFootOut))continue;

			Vector3 vRightFoot = GetBoneWithRotation(mesh, 80);
			if (!IsVec3Valid(vRightFoot))continue;
			Vector3 vRightFootOut = ProjectWorldToScreen(vRightFoot);
			if (!IsVec3Valid(vRightFootOut))continue;

			Vector3 Pelvis = GetBoneWithRotation(mesh, 2);
			if (!IsVec3Valid(Pelvis))continue;
			Vector3 PelvisOut = ProjectWorldToScreen(Pelvis);
			if (!IsVec3Valid(PelvisOut))continue;

			if (distance <= visuals::MaxSkeletonDrawDistance && IsInScreen(vNeckOut))
			{
				if (visuals::skel) {

					DrawLine(vNeckOut.x, vNeckOut.y, vHeadBoneOut.x, vHeadBoneOut.y, &ESPSkeleton, menu::box_thick);
					DrawLine(PelvisOut.x, PelvisOut.y, vNeckOut.x, vNeckOut.y, &ESPSkeleton, menu::box_thick);

					DrawLine(vUpperArmLeftOut.x, vUpperArmLeftOut.y, vNeckOut.x, vNeckOut.y, &ESPSkeleton, menu::box_thick);
					DrawLine(vUpperArmRightOut.x, vUpperArmRightOut.y, vNeckOut.x, vNeckOut.y, &ESPSkeleton, menu::box_thick);

					DrawLine(vLeftHandOut.x, vLeftHandOut.y, vUpperArmLeftOut.x, vUpperArmLeftOut.y, &ESPSkeleton, menu::box_thick);
					DrawLine(vRightHandOut.x, vRightHandOut.y, vUpperArmRightOut.x, vUpperArmRightOut.y, &ESPSkeleton, menu::box_thick);

					DrawLine(vLeftHandOut.x, vLeftHandOut.y, vLeftHandOut1.x, vLeftHandOut1.y, &ESPSkeleton, menu::box_thick);
					DrawLine(vRightHandOut.x, vRightHandOut.y, vRightHandOut1.x, vRightHandOut1.y, &ESPSkeleton, menu::box_thick);

					DrawLine(vLeftThighOut.x, vLeftThighOut.y, vHipOut.x, vHipOut.y, &ESPSkeleton, menu::box_thick);
					DrawLine(vRightThighOut.x, vRightThighOut.y, vHipOut.x, vHipOut.y, &ESPSkeleton, menu::box_thick);

					DrawLine(vLeftCalfOut.x, vLeftCalfOut.y, vLeftThighOut.x, vLeftThighOut.y, &ESPSkeleton, menu::box_thick);
					DrawLine(vRightCalfOut.x, vRightCalfOut.y, vRightThighOut.x, vRightThighOut.y, &ESPSkeleton, menu::box_thick);

					DrawLine(vLeftFootOut.x, vLeftFootOut.y, vLeftCalfOut.x, vLeftCalfOut.y, &ESPSkeleton, menu::box_thick);
					DrawLine(vRightFootOut.x, vRightFootOut.y, vRightCalfOut.x, vRightCalfOut.y, &ESPSkeleton, menu::box_thick);
				}
			}

			if (distance <= visuals::MaxDistance && IsInScreen(vNeckOut))
			{
				Vector3 player_position = GetBoneWithRotation(mesh, 0);
				Vector3 player_screen = ProjectWorldToScreen(player_position);

				Vector3 BoxHead = GetBoneWithRotation(mesh, 96);
				Vector3 head_screen = ProjectWorldToScreen(Vector3(BoxHead.x, BoxHead.y + 0.3, BoxHead.z));

				Vector3 normal_head = ProjectWorldToScreen(BoxHead);

				float BoxHeight = player_screen.y - head_screen.y;
				float BoxWidth = BoxHeight / 1.8f;

				if (visuals::box && IsInScreen(vNeckOut))
				{
					if (visuals::boxMode == 0 || visuals::boxMode == 1)
					{
						if (visuals::outline)
						{
							DrawNormalBox(player_screen.x - BoxWidth / 2 + 1, head_screen.y, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
							DrawNormalBox(player_screen.x - BoxWidth / 2 - 1, head_screen.y, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
							DrawNormalBox(player_screen.x - BoxWidth / 2, head_screen.y + 1, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
							DrawNormalBox(player_screen.x - BoxWidth / 2, head_screen.y - 1, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
						}
						DrawNormalBox(player_screen.x - (BoxWidth / 2), head_screen.y, BoxWidth, BoxHeight, menu::box_thick, &ESPColor);
					}
					if (visuals::boxMode == 2 || visuals::boxMode == 3)
					{
						if (visuals::outline)
						{
							DrawCornerBox(player_screen.x - BoxWidth / 2 + 1, head_screen.y, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
							DrawCornerBox(player_screen.x - BoxWidth / 2 - 1, head_screen.y, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
							DrawCornerBox(player_screen.x - BoxWidth / 2, head_screen.y + 1, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
							DrawCornerBox(player_screen.x - BoxWidth / 2, head_screen.y - 1, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
						}
						DrawCornerBox(player_screen.x - (BoxWidth / 2), head_screen.y, BoxWidth, BoxHeight, menu::box_thick, &ESPColor);
					}

					if (visuals::boxMode == 1 || visuals::boxMode == 3) menu::box_thick = 1,
						DrawFilledRect(player_screen.x - (BoxWidth / 2), head_screen.y, BoxWidth, BoxHeight, &Col.filled);
				}

				if (visuals::lines && IsInScreen(vNeckOut))
				{
					if (visuals::lineMode == 0)
						DrawLine((Width / 2), Height, player_screen.x, player_screen.y, &ESPColor, menu::box_thick);

				}

				if (visuals::show_head && IsInScreen(vNeckOut))
					DrawCircle(normal_head.x, normal_head.y, BoxHeight / 25, &ESPColor, 15);

				if (visuals::name && IsInScreen(vNeckOut))
				{
					char buf[256];
					sprintf(buf, E("[ %dm ]"), (int)distance);
					DrawString(14, vHeadBoneOut.x, vHeadBoneOut.y - 10, &ESPColor, true, true, buf);
				}

				if (aimbot::aimbot && IsInScreen(vNeckOut)) {
					auto dx = vHeadBoneOut.x - (Width / 2);
					auto dy = vHeadBoneOut.y - (Height / 2);
					auto dz = vHeadBoneOut.z - (Depth / 2);
					auto dist = sqrtf(dx * dx + dy * dy + dz * dz) / 100.0f;
					if (dist < aimbot::aimfov && dist < closestDistance && targetlocked == false/* && isRecentlyRendered*/) {
						closestDistance = dist;
						closestPawn = entity.Actor;
					}
				}
			}
		}
	}

	if (closestPawn != 0) {
		if (aimbot::aimbot && closestPawn && Key.IsKeyPushing(hotkeys::aimkey))
		{
			targetlocked = true;
			isRecentlyRendered = true;

			uint64_t currentactormesh = read<uint64_t>(closestPawn + 0x280);
			auto rootHead = GetBoneWithRotation(currentactormesh, 66);
			Vector3 rootHeadOut = ProjectWorldToScreen(rootHead);

			if (aimbot::prediction) {

				Vector3 vellocity = read<Vector3>(currentactormesh + 0x140);
				float Distance = LocalRelativeLocation.Distance(rootHeadOut) / 100.f;
				float Time = Distance / (3000 / 100.f);
				rootHeadOut = rootHeadOut + vellocity * Time;
			}

			if (rootHeadOut.x != 0 || rootHeadOut.y != 0 || rootHeadOut.z != 0) {
				if ((GetCrossDistance(rootHeadOut.x, rootHeadOut.y, rootHeadOut.z, Width / 2, Height / 2, Depth / 2) <= aimbot::aimfov)) {
					DrawLine(Width / 2, Height / 2, rootHeadOut.x, rootHeadOut.y, &ESPColor, menu::box_thick);
					move_to(rootHeadOut.x, rootHeadOut.y);
				}
			}
		}
		else
		{
			isaimbotting = false;
			targetlocked = false;
			isRecentlyRendered = false;

			uint64_t currentactormesh = read<uint64_t>(closestPawn + 0x280);
			auto rootHead = GetBoneWithRotation(currentactormesh, 66);
			Vector3 rootHeadOut = ProjectWorldToScreen(rootHead);
			if (rootHeadOut.x != 0 || rootHeadOut.y != 0 || rootHeadOut.z != 0) {
				if ((GetCrossDistance(rootHeadOut.x, rootHeadOut.y, rootHeadOut.z, Width / 2, Height / 2, Depth / 2) <= aimbot::aimfov)) {
					DrawLine(Width / 2, Height / 2, rootHeadOut.x, rootHeadOut.y, &ESPColor, menu::box_thick);
				}
			}
			closestDistance = FLT_MAX;
			closestPawn = NULL;
		}
	}
}

Vector3 RotatePoint(Vector3 pointToRotate, Vector3 centerPoint, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck, bool angleInRadians = false);
Vector3 RotatePoint(Vector3 EntityPos, Vector3 LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck, bool angleInRadians)
{
	float r_1, r_2;
	float x_1, y_1;

	r_1 = -(EntityPos.y - LocalPlayerPos.y);
	r_2 = EntityPos.x - LocalPlayerPos.x;

	float Yaw = angle - 90.0f;

	float yawToRadian = Yaw * (float)(M_PI / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
	y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

	*viewCheck = y_1 < 0;

	x_1 *= zoom;
	y_1 *= zoom;

	int sizX = sizeX / 2;
	int sizY = sizeY / 2;

	x_1 += sizX;
	y_1 += sizY;

	if (x_1 < 5)
		x_1 = 5;

	if (x_1 > sizeX - 5)
		x_1 = sizeX - 5;

	if (y_1 < 5)
		y_1 = 5;

	if (y_1 > sizeY - 5)
		y_1 = sizeY - 5;


	x_1 += posX;
	y_1 += posY;


	return Vector3(x_1, y_1, 0);
}
void background()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Once);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.09f, 0.09f, 0.09f, 0.40f / 1.f * 2.f));
	static const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove;
	ImGui::Begin(E("##background"), nullptr, flags);
	ImGui::End();
	ImGui::PopStyleColor();
}
bool draw_abigsquare()
{
	static int MenuTab = 0;
	static int VisualTab = 0;

	ImGuiStyle* style = &ImGui::GetStyle();

	float
		TextSpaceLine = 90.f,
		SpaceLineOne = 120.f,
		SpaceLineTwo = 280.f,
		SpaceLineThr = 420.f;

	static auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;

	ImGui::PushFont(menu::SkeetFont);
	ImGui::Begin((E("The GodFather - Private Editon")), NULL, flags);
	{
		ImGui::PopFont();
		static float flRainbow;
		float flSpeed = 0.0003f;
		ImVec2 curPos = ImGui::GetCursorPos();
		ImVec2 curWindowPos = ImGui::GetWindowPos();
		curPos.x += curWindowPos.x;
		curPos.y += curWindowPos.y;
		RGBA FPS = Col.rainbow();
		DrawRectRainbow(curPos.x - 10 , curPos.y - 5, ImGui::GetWindowSize().x + Width, curPos.y + -10, flSpeed, &FPS, flRainbow);
		ImGui::SetWindowSize(ImVec2(590, 415), ImGuiCond_Once);
		{
			ImGui::BeginChild((E("##Tabs")), ImVec2(-1, 40.0f));
			{
				style->ItemSpacing = ImVec2(1, 1);
				ImGui::PushFont(menu::SkeetFont);
				{
					if (ImGui::Button(((E("Aimbot"))), ImVec2(185, 20)))
					{
						MenuTab = 0;
					}ImGui::SameLine();
					if (ImGui::Button(((E("Visuals"))), ImVec2(185, 20)))
					{
						MenuTab = 1;
					}ImGui::SameLine();
					if (ImGui::Button(((E("Config"))), ImVec2(185, 20)))
					{
						MenuTab = 2;
					}ImGui::SameLine();
				} ImGui::PopFont();
			}
			ImGui::EndChild();

			style->ItemSpacing = ImVec2(8, 8);

			if (MenuTab == 0)
			{
				ImGui::BeginChild((E("##Aimbot")), ImVec2(560.f, 330.f), true);
				{
					ImGui::Checkbox((E("Aimbot")), &aimbot::aimbot);
					ImGui::Checkbox((E("Prediction")), &aimbot::prediction);
					ImGui::Checkbox((E("Smooth")), &aimbot::smooth);
					ImGui::Checkbox((E("Show FOV")), &aimbot::fovcircle);
				}
				ImGui::EndChild();

				ImGui::SetCursorPos(ImVec2(30, 190));
				ImGui::BeginChild((E("##Aimbot2")), ImVec2(-15, 190.f), true);
				{
					ImGui::PushItemWidth(200.f);
					ImGui::Text((E("FOV: "))); ImGui::SameLine(SpaceLineOne);
					ImGui::SliderFloat(E("##fovkrai"), &aimbot::aimfov, 1.f, 700.f, E("%.2f"));

					ImGui::Text((E("Smooth: "))); ImGui::SameLine(SpaceLineOne);
					ImGui::SliderFloat(E("##smoothkrai"), &aimbot::aimspeed, 1.f, 180.f, E("%.2f"));

					ImGui::Text((E("Aim Key: "))); ImGui::SameLine(SpaceLineOne);
					HotkeyButton(hotkeys::aimkey, ChangeKey, keystatus);

					ImGui::PushItemWidth(100.f);
					ImGui::Text((E("Hitbox: "))); ImGui::SameLine(SpaceLineOne);
					ImGui::Combo((E("##BONES")), &aimbot::hitbox, Hitbox, IM_ARRAYSIZE(Hitbox));
					ImGui::PopItemWidth();

					ImGui::Text((E("FOV Color: "))); ImGui::SameLine(SpaceLineOne); ImGui::ColorEdit3((E("##fovcolor")), colors::fovcol);
				}
				ImGui::EndChild();
			}

			else if (MenuTab == 1)
			{
				if (VisualTab == 0)
				{
					ImGui::BeginChild((E("##Players1")), ImVec2(560.f, 330.f), true);
					{
						ImGui::Checkbox((E("Show Box")), &visuals::box);
						ImGui::SameLine(120.f);
						ImGui::Checkbox((E("Show Skeleton")), &visuals::skel);

						ImGui::Checkbox((E("Distance")), &visuals::name);
						ImGui::SameLine(120.f);
						ImGui::Checkbox((E("Show Head")), &visuals::show_head);

						ImGui::Checkbox((E("Show Lines")), &visuals::lines);
						ImGui::SameLine(120.f);
						ImGui::Checkbox((E("Crosshair")), &visuals::crosshair);

						ImGui::Checkbox((E("Radar")), &menu::radar);
						ImGui::SameLine(120.f);
						ImGui::Checkbox((E("Outline")), &visuals::outline);
					}
					ImGui::EndChild();

					ImGui::SetCursorPos(ImVec2(30, 190));
					ImGui::BeginChild((E("##Players22")), ImVec2(-15, 190.f), true);
					{	
						ImGui::PushItemWidth(200.f);
						ImGui::Text((E("Box Style"))); ImGui::SameLine(100.f);
						ImGui::Combo((E("##BOXSTYLES")), &visuals::boxMode, boxStyle, IM_ARRAYSIZE(boxStyle));

						ImGui::Text(E("Lines Style")); 	ImGui::SameLine(100.f);
						ImGui::Combo((E("##LINESSTYLE")), &visuals::lineMode, linesMode, IM_ARRAYSIZE(linesMode));

						ImGui::PushItemWidth(210.f);
						ImGui::Text((E("Box Thick: "))); ImGui::SameLine(TextSpaceLine);
						ImGui::SliderInt((E("##BoxThick")), &menu::box_thick, 1.f, 3.f);

						ImGui::PushItemWidth(200.f);
						ImGui::Text((E("Crosshair Size:"))); ImGui::SameLine(SpaceLineOne);
						ImGui::SliderFloat((E("##cross")), &menu::crosshair_size, 1.f, 100.f, E("%.2f"));

						ImGui::PushItemWidth(200.f);
						ImGui::Text((E("Radar Style:"))); ImGui::SameLine(SpaceLineOne);
						ImGui::Combo((E("##radar")), &menu::RadarStyle, RadarStyle, IM_ARRAYSIZE(RadarStyle));
					}
					ImGui::EndChild();
				}
			}

			else if (MenuTab == 2)
			{
				ImGui::BeginChild((E("##Setts")), ImVec2(560.f, 330.f), true);
				{
					ImGui::Text((E("Crosshair: "))); ImGui::SameLine(SpaceLineOne); ImGui::ColorEdit3((E("##cross")), colors::crosscol);
					ImGui::Text((E("Visible ESP: "))); ImGui::SameLine(SpaceLineOne); ImGui::ColorEdit3((E("##esp1")), colors::espcol);
					ImGui::Text((E("!Visible ESP: "))); ImGui::SameLine(SpaceLineOne); ImGui::ColorEdit3((E("##esp2")), colors::espcolvi);
					ImGui::Text((E("Visible Skel: "))); ImGui::SameLine(SpaceLineOne); ImGui::ColorEdit3((E("##skels1")), colors::skelcol);
					ImGui::Text((E("!Visible Skel: "))); ImGui::SameLine(SpaceLineOne); ImGui::ColorEdit3((E("##skels2")), colors::skelcolvi);
					ImGui::Text((E("Esp Distance:"))); ImGui::SameLine(SpaceLineOne); ImGui::SliderInt((E("##yawa")), &visuals::MaxDistance, 0, 500);
					ImGui::Text((E("Skel Distance:"))); ImGui::SameLine(SpaceLineOne); ImGui::SliderInt((E("##rai2131wor1s")), &visuals::MaxSkeletonDrawDistance, 0, 300);

					ImGui::PushItemWidth(100.f);
					ImGui::Text((E("Config: "))); ImGui::SameLine(60.f); ImGui::Combo((E("##settsas")), &menu::settsMode, settsName, IM_ARRAYSIZE(settsName));

					ImGui::SameLine(170.f);
					if (ImGui::Button(((E("Save"))), ImVec2(100, 20)))
					{
						if (menu::settsMode == 0)
							Save_Settings((E("C:\\FN-Default.ini")));
						if (menu::settsMode == 1)
							Save_Settings((E("C:\\FN-Setts1.ini")));
						if (menu::settsMode == 2)
							Save_Settings((E("C:\\FN-Setts2.ini")));
						if (menu::settsMode == 3)
							Save_Settings((E("C:\\FN-Setts3.ini")));
						if (menu::settsMode == 4)
							Save_Settings((E("C:\\FN-Setts4.ini")));
					}

					ImGui::SameLine(280.f);
					if (ImGui::Button(((E("Load"))), ImVec2(100, 20)))
					{
						if (menu::settsMode == 0)
							Load_Settings((E("C:\\FN-Default.ini")));
						if (menu::settsMode == 1)
							Load_Settings((E("C:\\FN-Setts1.ini")));
						if (menu::settsMode == 2)
							Load_Settings((E("C:\\FN-Setts2.ini")));
						if (menu::settsMode == 3)
							Load_Settings((E("C:\\FN-Setts3.ini")));
						if (menu::settsMode == 4)
							Load_Settings((E("C:\\FN-Setts4.ini")));
					}

					ImGui::PopItemWidth();
				}ImGui::EndChild();
			}
		}
	}
	ImGui::End();
	return true;
}
void DrawRadar()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 oldPadding = style.WindowPadding;
	float oldAlpha = style.Colors[ImGuiCol_WindowBg].w;
	style.WindowPadding = ImVec2(0, 0);
	style.Colors[ImGuiCol_WindowBg].w = (float)255 / 255.0f;
	ImGui::Begin(E(" RADAR "), &menu::radar, ImVec2(200, 200), 0.4F, ImGuiWindowFlags_NoCollapse);
	{
		ImVec2 siz = ImGui::GetWindowSize();
		ImVec2 pos = ImGui::GetWindowPos();


		ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
		windowDrawList->AddLine(ImVec2(pos.x + (siz.x / 2), pos.y + 0), ImVec2(pos.x + (siz.x / 2), pos.y + siz.y), 0xFF000000, 1.5f);
		windowDrawList->AddLine(ImVec2(pos.x + 0, pos.y + (siz.y / 2)), ImVec2(pos.x + siz.x, pos.y + (siz.y / 2)), 0xFF000000, 1.5f);

		auto entityListCopy = entityList;

		for (auto entity : entityListCopy)
		{
			uint64_t rootcomponent = read<uint64_t>(entity.Actor + 0x130);
			if (!rootcomponent)continue;

			Vector3 Relativelocation = read<Vector3>(rootcomponent + 0x11C);
			if (!IsVec3Valid(Relativelocation))continue;

			bool viewCheck = false;
			Vector3 EntityPos = RotatePoint(Relativelocation, LocalRelativeLocation, pos.x, pos.y, siz.x, siz.y, zoom, 2, &viewCheck);

			int s = 4;
			switch (menu::RadarStyle) // 0 - Box; 1 - Filled box; 2 - Circle; 3 - Filled circle;
			{
			case 0:
			{
				windowDrawList->AddRect(ImVec2(EntityPos.x - s, EntityPos.y - s),
					ImVec2(EntityPos.x + s, EntityPos.y + s),
					0xFFFFFFFF);
				break;
			}
			case 1:
			{
				windowDrawList->AddRectFilled(ImVec2(EntityPos.x - s, EntityPos.y - s),
					ImVec2(EntityPos.x + s, EntityPos.y + s),
					0xFFFFFFFF);
				break;
			}
			case 2:
			{
				windowDrawList->AddCircle(ImVec2(EntityPos.x, EntityPos.y), s, 0xFFFFFFFF);
				break;
			}
			case 3:
			{
				windowDrawList->AddCircleFilled(ImVec2(EntityPos.x, EntityPos.y), s, 0xFFFFFFFF);
				break;
			}
			default:
				break;
			}
		}
	}
	ImGui::End();
	style.WindowPadding = oldPadding;
	style.Colors[ImGuiCol_WindowBg].w = oldAlpha;
}
void shortcurts()
{
	if (Key.IsKeyPushing(VK_INSERT))
	{
		if (menu_key == false)
		{
			menu_key = true;
		}
		else if (menu_key == true)
		{
			menu_key = false;
		}
		Sleep(200);
	}
}
void render() {

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	fn_esp();

	RGBA FOVCIRCLE = { colors::fovcol[0] * 255, colors::fovcol[1] * 255, colors::fovcol[2] * 255, 255 };
	RGBA CROSS = { colors::crosscol[0] * 255, colors::crosscol[1] * 255, colors::crosscol[2] * 255, 255 };

	if (aimbot::fovcircle) {
		DrawCircle(Width / 2, Height / 2, float(aimbot::aimfov), &FOVCIRCLE, 100);
	}

	if (visuals::crosshair) {
		DrawLine((Width / 2) - menu::crosshair_size, (Height / 2), (Width / 2) + (menu::crosshair_size + 1), (Height / 2), &CROSS, menu::box_thick);
		DrawLine((Width / 2), (Height / 2) - menu::crosshair_size, (Width / 2), (Height / 2) + (menu::crosshair_size + 1), &CROSS, menu::box_thick);
	}

	if (menu::radar) {
		DrawRadar();
	}

	shortcurts();
	if (menu_key)
	{
		background();
		draw_abigsquare();
		ImGui::GetIO().MouseDrawCursor = 1;
	}
	else {
		ImGui::GetIO().MouseDrawCursor = 0;
	}

	char fpsinfo[64];
	sprintf(fpsinfo, E("FPS: %03d"), get_fps());
	DrawString(14, 100, 35, &Col.green, true, true, fpsinfo);

	ImGui::EndFrame();
	p_Device->SetRenderState(D3DRS_ZENABLE, false);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (p_Device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		p_Device->EndScene();
	}
	HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		p_Device->Reset(&p_Params);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}
WPARAM MainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();
		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		if (hwnd_active == GameWnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(MyWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(GameWnd, &rc);
		ClientToScreen(GameWnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameWnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{

			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			p_Params.BackBufferWidth = Width;
			p_Params.BackBufferHeight = Height;
			SetWindowPos(MyWnd, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			p_Device->Reset(&p_Params);
		}
		render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanuoD3D();
	DestroyWindow(MyWnd);

	return Message.wParam;
}
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_DESTROY:
		CleanuoD3D();
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (p_Device != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_Params.BackBufferWidth = LOWORD(lParam);
			p_Params.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = p_Device->Reset(&p_Params);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}
void CleanuoD3D()
{
	if (p_Device != NULL)
	{
		p_Device->EndScene();
		p_Device->Release();
	}
	if (p_Object != NULL)
	{
		p_Object->Release();
	}
}
int isTopwin()
{
	HWND hWnd = GetForegroundWindow();
	if (hWnd == GameWnd)
		return TopWindowGame;
	if (hWnd == MyWnd)
		return TopWindowMvoe;

	return 0;
}
void SetWindowToTarget()
{
	while (true)
	{
		GameWnd = get_process_wnd(sdk::process_id);
		if (GameWnd)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(GameWnd, &GameRect);
			Width = GameRect.right - GameRect.left;
			Height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(GameWnd, GWL_STYLE);
			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				Height -= 39;
			}
			ScreenCenterX = Width / 2;
			ScreenCenterY = Height / 2;
			MoveWindow(MyWnd, GameRect.left, GameRect.top, Width, Height, true);
		}
	}
}
int main() {
	printf(E("[!] Open driver connection: "));
	if (driver->Init(FALSE)) {
		printf(E("Success!\n"));
		driver->Attach(E(L"FortniteClient-Win64-Shipping.exe"));
		while (true)
		{
			SetupWindow();
			DirectXInit(MyWnd);

			sdk::process_id = driver->GetProcessId(E(L"FortniteClient-Win64-Shipping.exe"));

			sdk::module_base = driver->GetModuleBase(E(L"FortniteClient-Win64-Shipping.exe"));

			printf(E("BaseAddress :0x%llX\n"), sdk::module_base);

			HANDLE handle = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(fn_cache), nullptr, NULL, nullptr);
			CloseHandle(handle);

			MainLoop();
		}
		return 0;
	}
	printf(E("Failed!\n"));
	system(E("pause"));
	return 1;
}
