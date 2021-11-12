#pragma once

BOOL WritePrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName) {
    char lpString[1024];
    sprintf(lpString, E("%d"), nInteger);
    return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}
BOOL WritePrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float nInteger, LPCSTR lpFileName) {
    char lpString[1024];
    sprintf(lpString, E("%f"), nInteger);
    return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}
float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName)
{
    char szData[32];

    GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);

    return (float)atof(szData);
}

void Save_Settings(LPCSTR path)
{
    WritePrivateProfileInt((E("Visuals")), (E("m_playeresp")), visuals::box, path);
    WritePrivateProfileInt((E("Visuals")), (E("m_boxstyle")), visuals::boxMode, path);
    WritePrivateProfileInt((E("Visuals")), (E("m_radarstyle")), menu::RadarStyle, path);
    WritePrivateProfileInt((E("Visuals")), (E("m_skeleton")), visuals::skel, path);
    WritePrivateProfileInt((E("Visuals")), (E("m_crosshair")), visuals::crosshair, path);
    WritePrivateProfileFloat((E("Visuals")), (E("m_crossize")), menu::crosshair_size, path);
    WritePrivateProfileInt((E("Visuals")), (E("m_playername")), visuals::name, path);
    WritePrivateProfileInt((E("Visuals")), (E("m_drawhead")), visuals::show_head, path);
    WritePrivateProfileInt((E("Visuals")), (E("m_outline")), visuals::outline, path);
    WritePrivateProfileInt((E("Visuals")), (E("m_snaplines")), visuals::lines, path);
    WritePrivateProfileInt((E("Visuals")), (E("m_linemode")), visuals::lineMode, path);
    WritePrivateProfileInt((E("Visuals")), (E("m_boxthick")), menu::box_thick, path);
    WritePrivateProfileInt((E("Visuals")), (E("m_radar")), menu::radar, path);

    WritePrivateProfileInt((E("Aimbot")), (E("m_aimbot")), aimbot::aimbot, path);
    WritePrivateProfileInt((E("Aimbot")), (E("m_smooth")), aimbot::smooth, path);
    WritePrivateProfileFloat((E("Aimbot")), (E("m_aimbotfov")), aimbot::aimfov, path);
    WritePrivateProfileFloat((E("Aimbot")), (E("m_aimbotsmooth")), aimbot::aimspeed, path);
    WritePrivateProfileInt((E("Aimbot")), (E("m_fovcircle")), aimbot::fovcircle, path);
    WritePrivateProfileInt((E("Aimbot")), (E("m_aimkey")), hotkeys::aimkey, path);
    WritePrivateProfileInt((E("Aimbot")), (E("m_hitbox")), aimbot::hitbox, path);
    WritePrivateProfileInt((E("Aimbot")), (E("m_prediction")), aimbot::prediction, path);

    WritePrivateProfileFloat((E("Colors")), (E("m_espr")), colors::espcol[0], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_espg")), colors::espcol[1], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_espb")), colors::espcol[2], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_espinvisibler")), colors::espcolvi[0], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_espinvisibleg")), colors::espcolvi[1], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_espinvisibleb")), colors::espcolvi[2], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_skelr")), colors::skelcol[0], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_skelg")), colors::skelcol[1], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_skelb")), colors::skelcol[2], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_invisibleskelr")), colors::skelcolvi[0], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_invisibleskelg")), colors::skelcolvi[1], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_invisibleskelb")), colors::skelcolvi[2], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_crossr")), colors::crosscol[0], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_crossg")), colors::crosscol[1], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_crossb")), colors::crosscol[2], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_fovcolr")), colors::fovcol[0], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_fovcolg")), colors::fovcol[1], path);
    WritePrivateProfileFloat((E("Colors")), (E("m_fovcolb")), colors::fovcol[2], path);
    WritePrivateProfileInt((E("Colors")), (E("m_rainbow")), menu::rainbow_colors, path);
    WritePrivateProfileFloat((E("Colors")), (E("m_rainbowspeed")), menu::rainbow_speed, path);
    WritePrivateProfileFloat((E("Colors")), (E("m_visible")), menu::rainbow_speed, path);

    WritePrivateProfileInt((E("Distance")), (E("m_playerdistance")), visuals::MaxDistance, path);
    WritePrivateProfileInt((E("Distance")), (E("m_esqueletoistance")), visuals::MaxSkeletonDrawDistance, path);
}
void Load_Settings(LPCSTR path)
{
    visuals::box = GetPrivateProfileIntA((E("Visuals")), (E("m_playeresp")), visuals::box, path);
    menu::RadarStyle = GetPrivateProfileIntA((E("Visuals")), (E("m_radarstyle")), menu::RadarStyle, path);
    visuals::boxMode = GetPrivateProfileIntA((E("Visuals")), (E("m_boxstyle")), visuals::boxMode, path);
    visuals::crosshair = GetPrivateProfileIntA((E("Visuals")), (E("m_crosshair")), visuals::crosshair, path);
    visuals::skel = GetPrivateProfileIntA((E("Visuals")), (E("m_skeleton")), visuals::skel, path);
    menu::crosshair_size = GetPrivateProfileFloat((E("Visuals")), (E("m_crossize")), menu::crosshair_size, path);
    visuals::name = GetPrivateProfileIntA((E("Visuals")), (E("m_playername")), visuals::name, path);
    visuals::show_head = GetPrivateProfileIntA((E("Visuals")), (E("m_drawhead")), visuals::show_head, path);
    visuals::outline = GetPrivateProfileIntA((E("Visuals")), (E("m_outline")), visuals::outline, path);
    visuals::lines = GetPrivateProfileIntA((E("Visuals")), (E("m_snaplines")), visuals::lines, path);
    visuals::lineMode = GetPrivateProfileIntA((E("Visuals")), (E("m_linemode")), visuals::lineMode, path);
    menu::box_thick = GetPrivateProfileIntA((E("Visuals")), (E("m_boxthick")), menu::box_thick, path);
    menu::radar = GetPrivateProfileIntA((E("Visuals")), (E("m_radar")), menu::radar, path);

    aimbot::aimbot = GetPrivateProfileIntA((E("Aimbot")), (E("m_aimbot")), aimbot::aimbot, path);
    aimbot::smooth = GetPrivateProfileIntA((E("Aimbot")), (E("m_smooth")), aimbot::smooth, path);
    aimbot::aimfov = GetPrivateProfileFloat((E("Aimbot")), (E("m_aimbotfov")), aimbot::aimfov, path);
    aimbot::aimspeed = GetPrivateProfileFloat((E("Aimbot")), (E("m_aimbotsmooth")), aimbot::aimspeed, path);
    aimbot::fovcircle = GetPrivateProfileIntA((E("Aimbot")), (E("m_fovcircle")), aimbot::fovcircle, path);
    hotkeys::aimkey = GetPrivateProfileIntA((E("Aimbot")), (E("m_aimkey")), hotkeys::aimkey, path);
    aimbot::hitbox = GetPrivateProfileIntA((E("Aimbot")), (E("m_hitbox")), aimbot::hitbox, path);
    aimbot::prediction = GetPrivateProfileIntA((E("Aimbot")), (E("m_prediction")), aimbot::prediction, path);

    colors::espcol[0] = GetPrivateProfileFloat((E("Colors")), (E("m_espr")), colors::espcol[0], path);
    colors::espcol[1] = GetPrivateProfileFloat((E("Colors")), (E("m_espg")), colors::espcol[1], path);
    colors::espcol[2] = GetPrivateProfileFloat((E("Colors")), (E("m_espb")), colors::espcol[2], path);
    colors::espcolvi[0] = GetPrivateProfileFloat((E("Colors")), (E("m_invisibleespr")), colors::espcolvi[0], path);
    colors::espcolvi[1] = GetPrivateProfileFloat((E("Colors")), (E("m_invisibleespg")), colors::espcolvi[1], path);
    colors::espcolvi[2] = GetPrivateProfileFloat((E("Colors")), (E("m_invisibleespb")), colors::espcolvi[2], path);
    colors::skelcol[0] = GetPrivateProfileFloat((E("Colors")), (E("m_skelr")), colors::skelcol[0], path);
    colors::skelcol[1] = GetPrivateProfileFloat((E("Colors")), (E("m_skelg")), colors::skelcol[1], path);
    colors::skelcol[2] = GetPrivateProfileFloat((E("Colors")), (E("m_skelb")), colors::skelcol[2], path);
    colors::skelcolvi[0] = GetPrivateProfileFloat((E("Colors")), (E("m_invisibleskelr")), colors::skelcolvi[0], path);
    colors::skelcolvi[1] = GetPrivateProfileFloat((E("Colors")), (E("m_invisibleskelg")), colors::skelcolvi[1], path);
    colors::skelcolvi[2] = GetPrivateProfileFloat((E("Colors")), (E("m_invisibleskelb")), colors::skelcolvi[2], path);
    colors::crosscol[0] = GetPrivateProfileFloat((E("Colors")), (E("m_crossr")), colors::crosscol[0], path);
    colors::crosscol[1] = GetPrivateProfileFloat((E("Colors")), (E("m_crossg")), colors::crosscol[1], path);
    colors::crosscol[2] = GetPrivateProfileFloat((E("Colors")), (E("m_crossb")), colors::crosscol[2], path);
    colors::fovcol[0] = GetPrivateProfileFloat((E("Colors")), (E("m_fovcolr")), colors::fovcol[0], path);
    colors::fovcol[1] = GetPrivateProfileFloat((E("Colors")), (E("m_fovcolg")), colors::fovcol[1], path);
    colors::fovcol[2] = GetPrivateProfileFloat((E("Colors")), (E("m_fovcolb")), colors::fovcol[2], path);

    visuals::MaxDistance = GetPrivateProfileIntA((E("Distance")), (E("m_playerdistance")), visuals::MaxDistance, path);
    visuals::MaxSkeletonDrawDistance = GetPrivateProfileIntA((E("Distance")), (E("m_esqueletoistance")), visuals::MaxSkeletonDrawDistance, path);

    menu::rainbow_colors = GetPrivateProfileIntA((E("Colors")), (E("m_rainbow")), menu::rainbow_colors, path);
    menu::rainbow_speed = GetPrivateProfileFloat((E("Colors")), (E("m_rainbowspeed")), menu::rainbow_speed, path);
};