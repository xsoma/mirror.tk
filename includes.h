#pragma once

#include "global_count.h"
#include "Hacks.h"
#include "Chams.h"
#include "Menu.h"
#include "circlestrafer.h"
#include "CBulletListener.h"
#include "Interfaces.h"
#include "Skinchanger.h"
#include "autodefuse.h"
#include "RenderManager.h"
#include "lodepng.h"
#include "knifebot.h"
#include "Visuals.h"
#include "EnginePrediction.h"
#include "MiscHacks.h"
#include "CRC32.h"
#include "Resolver.h"
#include "hitmarker.h"
#include "laggycompensation.h"
#include "DamageIndicator.h"
#include "RageBot.h"
#include "LagCompensation2.h"
#include "position_adjust.h"
#include "EnginePrediction.h"
#include "lin_extp.h"
#include "radar.h"
#include "fakelag.h"
#include "experimental.h"
#include "killsay.h"
#include "backdrop.h"
#include "animations.h"
#include "antiaim.h"
#include "singleton.h"

#include <intrin.h>
#include <d3d9.h>
#include <algorithm>

#define MakePtr(cast, ptr, addValue) (cast)( (DWORD)(ptr) + (DWORD)(addValue))
#ifdef NDEBUG
#define strenc( s ) std::string( cx_make_encrypted_string( s ) )
#define charenc( s ) strenc( s ).c_str()
#define wstrenc( s ) std::wstring( strenc( s ).begin(), strenc( s ).end() )
#define wcharenc( s ) wstrenc( s ).c_str()
#else
#define strenc( s ) ( s )
#define charenc( s ) ( s )
#define wstrenc( s ) ( s )
#define wcharenc( s ) ( s )
#endif
#ifdef NDEBUG
#define XorStr( s ) ( XorCompileTime::XorString< sizeof( s ) - 1, __COUNTER__ >( s, std::make_index_sequence< sizeof( s ) - 1>() ).decrypt() )
#else
#define XorStr( s ) ( s )
#endif

#define TEXTURE_GROUP_LIGHTMAP                      "Lightmaps"
#define TEXTURE_GROUP_WORLD                         "World textures"
#define TEXTURE_GROUP_MODEL                         "Model textures"
#define TEXTURE_GROUP_VGUI                          "VGUI textures"
#define TEXTURE_GROUP_PARTICLE                      "Particle textures"
#define TEXTURE_GROUP_DECAL                         "Decal textures"
#define TEXTURE_GROUP_SKYBOX                        "SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS                "ClientEffect textures"
#define TEXTURE_GROUP_OTHER                         "Other textures"
#define TEXTURE_GROUP_PRECACHED                     "Precached"
#define TEXTURE_GROUP_CUBE_MAP                      "CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET                 "RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED                   "Unaccounted textures"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER           "Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP     "Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR    "Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD    "World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS   "Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER    "Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER          "Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER         "Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER                  "DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL                    "ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS                 "Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS                "Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE         "RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS                 "Morph Targets"